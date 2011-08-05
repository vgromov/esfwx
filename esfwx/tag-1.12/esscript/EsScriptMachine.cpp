#include "stdafx.h"
#pragma hdrstop

#include "EsScriptMachine.h"

const EsString::const_pointer EsScriptMachine::s_global = wxT("__global__");
const EsString::const_pointer c_startup = wxT("__startup__");

EsScriptMachine::EsScriptMachine() :
m_loc( std::locale::classic() ),
m_empty(EsVariant::s_null),
m_consts(false), // consts symbol table is special, because it's singleton and uses both template and non-template modes simultaneously
m_destroying(false)
{
	wxLogDebug(wxT("Script machine ctor entered"));
	m_calls.reserve(defCallStackDepth);
	m_data.reserve(defDataStackDepth);
	m_tmpPool.reserve(defTmpPoolSize);
	reset();
}

EsScriptMachine::~EsScriptMachine()
{
	wxLogDebug(wxT("Script machine dtor entered"));
	m_destroying = true;	
	reset();
}

void EsScriptMachine::reset()
{
	m_calls.clear();
	m_data.clear();
	m_consts.reset();
	m_unnamed.clear();
	m_metaclassMap.clear();
	m_startup.reset();
	m_activeCode.reset();
	m_code.clear();
	wxLogDebug(wxT("script machine reset"));
	if( !m_destroying )
	{
		registerPodMetaclasses();
		addMethod( createMethod(c_startup) );
		setActiveStartupTemplate();
	}
}

void EsScriptMachine::declConst(const EsString& name, const EsVariant& val)
{
	m_consts.addSymbol(name, EsConstAccessor::create(val));
	wxLogDebug(wxT("Constant '%s'='%s' created"), name.c_str(), val.asString().c_str());
}

// add new unnamed const and return its accessor. if such const already declared, return its accessor
EsAccessorIntf::Ptr EsScriptMachine::declUnnamed(const EsVariant& val)
{
	EsUnnamedMap::iterator it = m_unnamed.find(val);
	if( it != m_unnamed.end() )
		return (*it).second;
		
	EsAccessorIntf::Ptr sym = EsConstAccessor::create(val);
	m_unnamed[val] = sym;
		
	return sym;
}

// create new named code section
EsCodeSection::Ptr EsScriptMachine::createMethod(const EsString& name, const EsString& namespaceOrObject /*= s_global*/)	
{
	EsScriptObjectIntf::Ptr metaclass = findMetaclass(namespaceOrObject);
	if( !metaclass )
		if( EsString::cmpEqual == name.compare(c_reflectedCtrName) || 
				EsString::cmpEqual == name.compare(c_reflectedDtrName) )
			EsException::Throw(0, EsException::Generic, _("'%s' may not be declared outside of object declaration") );

	EsCodeSection::Ptr ptr = EsCodeSection::create(name, namespaceOrObject, m_loc); 
	wxLogDebug(wxT("Method with name '%s' created for, but not added to '%s' namespace"), name.c_str(), namespaceOrObject.c_str());
	
	if( metaclass )
		ptr->setThis(metaclass.get());
	
	return ptr;
}

const std::locale& EsScriptMachine::loc() const
{
	return m_loc;
}

// specific standalone accessor, keeping empty constant value
EsAccessorIntf::Ptr EsScriptMachine::emptyVal() const
{
	return EsAccessorIntf::Ptr( const_cast<EsConstAccessor*>(&m_empty), false, false);
}
	
// r-o access to constants
const EsSymTable& EsScriptMachine::consts() const
{
	return m_consts;
}	

// return currently active method
EsCodeSection::Ptr EsScriptMachine::getActiveMethod()
{
	wxASSERT(m_activeCode);
	return m_activeCode;
}
	
// state machine stuff
//

// stack depth monitors
size_t EsScriptMachine::getCallStackDepth() const
{
	return m_calls.size();
}

size_t EsScriptMachine::getDataStackDepth() const
{
	return m_data.size();
}

// add method code section
void EsScriptMachine::addMethod(EsCodeSection::Ptr section)
{
	wxASSERT(section);
	// find if namespaceOrObject is already registered in map. if so, retrieve pointer to its method table,
	// otherwise, create new namespace and method map and register it in code map
	EsMethodMapPtr methods = getMethodTemplates(section->getNamespace(), false);
	if( !methods )
	{
		methods.reset( new EsMethodMap );
		m_code[section->getNamespace()] = methods;
	}
	
	MethodInfoKeyT key = constructMethodKey(section);
	wxASSERT(key.isOk());
	// find if method signature already exists in script method table
	EsMethodMap::const_iterator cit = methods->find(key);
	if( cit != methods->end() )
		EsException::Throw(0, EsException::Generic, _("Script function '%s', taking %d parameters, already declared in '%s'"),
		section->getName().c_str(), section->getInputParamCnt(), section->getNamespace().c_str());

	methods->insert( std::make_pair(key, section) );
	wxLogDebug(wxT("Method '%s', taking %d parameters, added to the script's namespace or object '%s'"), 
		section->getName().c_str(), section->getInputParamCnt(), section->getNamespace().c_str());	
}

// get section by key
EsCodeSection::Ptr EsScriptMachine::getMethodTemplate(const MethodInfoKeyT& key, const EsString& namespaceOrObject, bool doThrow /*= true*/) const
{
	EsCodeSection::Ptr result;
	EsMethodMapPtr methods = getMethodTemplates(namespaceOrObject, doThrow);
	if( methods )
	{
		// find if method signature exists in script method table
		EsMethodMap::const_iterator cit = methods->find(key);
		if( cit != methods->end() )
			result = (*cit).second;
		else if( doThrow )
			EsException::Throw(0, EsException::Generic, _("Script function '%s', taking %d parameters, is not declared in '%s'"),
				key.getName().c_str(), key.getParametersCount(), namespaceOrObject.c_str());
	}
	
	return result;
}

// return script method key given method code section ptr
MethodInfoKeyT EsScriptMachine::constructMethodKey(EsCodeSection::Ptr code)
{
	if( code )
		return MethodInfoKeyT(code->getInputParamCnt(), code->getName());
	
	return MethodInfoKeyT();
}

// return object or namespace methods
EsMethodMapPtr EsScriptMachine::getMethodTemplates(const EsString& namespaceOrObject /*= s_global*/, bool doThrow /*= true*/) const
{
	EsMethodMapPtr result;
	EsCodeMap::const_iterator itCode = m_code.find(namespaceOrObject);
	if( itCode == m_code.end())
	{
		if( doThrow )
			EsException::Throw(0, EsException::Generic, _("'%s' is not a known object or namespace name"), 
				namespaceOrObject.c_str());
	}
	else
		result = (*itCode).second;

	return result;
}

// get all namespace methods keys
MethodInfoKeysT EsScriptMachine::getAllMethodsKeys(const EsString& namespaceOrObject /*= s_global*/) const
{
	EsMethodMapPtr methods = getMethodTemplates(namespaceOrObject);
	wxASSERT(methods);	
	MethodInfoKeysT result;
	result.reserve( methods->size() );
	
	for(EsMethodMap::const_iterator cit = methods->begin(); cit != methods->end(); ++cit)
		result.push_back( (*cit).first );
	
	return result;
}

// set startup code current active code
void EsScriptMachine::setActiveStartupTemplate()
{
	setActiveMethodTemplate(MethodInfoKeyT(0, c_startup));
	m_startup = m_activeCode;
}

// return startup code
EsCodeSection::Ptr EsScriptMachine::getStartup()
{
	return m_startup;
}

// try to set current code section by name. if not found, an exception is thrown
void EsScriptMachine::setActiveMethodTemplate(const MethodInfoKeyT& key, const EsString& namespaceOrObject /*= s_global*/)
{
	EsCodeSection::Ptr result = getMethodTemplate(key, namespaceOrObject);
	if( !result )
		EsException::Throw(0, EsException::Generic, _("'%s', taking %d parameters, is not a method of '%s'"),
			key.getName().c_str(), key.getParametersCount(), namespaceOrObject.c_str());

	setActiveMethod(result);
}

EsString EsScriptMachine::traceVariant(const EsVariant& v)
{
	if( v.isObject() )
	{
		EsBaseIntfPtr obj = v.asObject();
		if( obj )
		{
			EsAccessorIntf::Ptr acc = obj;
			if( acc )
				return EsString::format(wxT("object '%s'='%s'"), acc->getTypeName().c_str(), 
					traceVariant(acc->get()).c_str());
			else
				return EsString::format(wxT("object '%s'"), obj->getTypeName().c_str());
		}
		else
			return wxT("null object");
	}
	else if( v.isCollection() )
	{
		EsString str;
		for( int idx = 0; idx < v.getCount(); ++idx )
			str += EsString::format(wxT("[%d]='%s';"), idx, traceVariant(v.getItem(idx)).c_str());
		return str;
	}
	else if( !v.isEmpty() )
		return v.asString();
	else
		return wxT("null");	
}

void EsScriptMachine::traceInstruction(EsInstructions::const_iterator instr)
{
	wxLogDebug(wxT("op: '%s' at %d in '%s'::'%s'"), EsInstruction::getOpcodeString((*instr).opcode()), instr-getActiveMethod()->instrStart(),
		getActiveMethod()->getNamespace().c_str(), getActiveMethod()->getName().c_str());
	const EsVariant& params = (*instr).param();
	if( params.isCollection() )
		wxLogDebug(wxT("\tparams: %s"), traceVariant(params).c_str());
	else
	{
		if( iBinOpExpr == (*instr).opcode() )
			wxLogDebug(wxT("\tparam: %s"), EsScript::getBinOpStr(params.asLong()).c_str());
		else
			wxLogDebug(wxT("\tparam: %s"), traceVariant(params).c_str());
	}
}

// internal code execution method
EsInstructionOpcode EsScriptMachine::exec(EsInstructions::const_iterator beg, EsInstructions::const_iterator end)
{
	EsInstructions::const_iterator instr = beg;
	while( instr != end )
	{
		traceInstruction( instr );
		EsInstructionOpcode opcode = (*instr).opcode();

		switch( opcode )	
		{
		case iVarInit:
			// perform assignment operation using instruction's var ref as lhs, and top of data stack as rhs
			doAsnOp((*instr));
			break;
		case iLoadConstVal:
			// load constant from instruction to data stack
			pushDataValue( (*instr).param().asExistingObject() );
			break;	
		case iFuncCall:
		case iExtFuncCall:
		case iMethodCall:
		case iConstructorCall:
			doCall((*instr));
			break;	
		case iRet:
			// immediately break current code section execution, popping top of data stack to the result register
			getActiveMethod()->setResult(	popDataValue() );
			return iRet;	
		case iRetCleanup:
			// we returned from the function call statement. pop data stack to retval register
			popDataValue();
			break;	
		case iLoadVarRef:
			doLoadVarRef((*instr));
			break;
		case iLoadItemRef:
			doConstructItemAccessor();
			break;	
		case iLoadFieldRef:
		case iLoadThisFieldRef:
			// create field accessor object
			doConstructFieldAccessor((*instr));
			break;	
		case iLoadPropRef:
		case iLoadThisPropRef:
			// create prop accessor object
			break;	
		case iLoadAttributeVal:
		case iLoadThisAttributeVal:
			// access attribute value
			doAccessAttributeValue((*instr));
			break;	
		case iBinOpExpr:
			// do binary operation expression
			doBinOp( (*instr).param().asLong() );	
			break;
		case iUnaryOpExpr:
			// do unary operation expression
			doUnaryOp( (*instr).param().asLong() );	
			break;
		case iLogicCheck:
			doLogicCheck(instr);
			break;
		case iAsn:
			// do assignment operation, given current instruction
			doAsnOp((*instr));
			break;
		case iIncDec:
			doIncDec((*instr));
			break;
		case iJump:
		case iJumpFalse:		
			if( doJump(instr) )
				continue;
			break;
		}
		
		++instr;
	}

	return iInvalid;
}

// execution helpers
//
// load variable reference to the data stack
void EsScriptMachine::doLoadVarRef(const EsInstruction& instr)
{
	wxASSERT(iLoadVarRef == instr.opcode());
	wxASSERT(EsVariant::VAR_VARIANT_COLLECTION == instr.param().getType());
	wxASSERT(2 == instr.param().getCount());
	
	EsAccessorIntf::Ptr var;
	EsString name = instr.param().getItem(0).asString();
	// if global variable
	switch( instr.param().getItem(1).asInt() )
	{
	case vtGlobal:
		var = getStartup()->getVariable( name );
		break;
	case vtLocal:
		var = getActiveMethod()->getVariable( name );
		break;
	}
	wxASSERT(var);
	pushDataValue(var);
}

// construct indexed item accessor and push it onto data stack
void EsScriptMachine::doConstructItemAccessor()
{
	// create indexed item accessor object and push it onto data stack
	// data stack top-1 = value accessor
	// data stack top = index expression accessor
	EsAccessorIntf::Ptr idx = popDataValue();
	EsAccessorIntf::Ptr val = getDataAtOffs(0);
	EsAccessorIntf::Ptr acc = EsItemAccessor::create(val, idx, m_loc);
	setDataAtOffs(0, acc);
}

// construct object field accessor and push it onto data stack
void EsScriptMachine::doConstructFieldAccessor(const EsInstruction& instr)
{
	wxASSERT(iLoadFieldRef == instr.opcode() || iLoadThisFieldRef == instr.opcode());
	wxASSERT(EsVariant::VAR_STRING == instr.param().getType());
	
	EsScriptObjectIntf::Ptr obj;
	if( iLoadFieldRef == instr.opcode() )
	{
		EsAccessorIntf::Ptr val = getDataAtOffs(0);
		obj = val->acc().asExistingObject();
	}
	else
		obj.reset( getActiveMethod()->getThis(), false );
		
	obj = obj->getField( instr.param().asString() );
	wxASSERT(obj);
	EsAccessorIntf::Ptr acc = createTemporaryVal( obj );
	if( iLoadFieldRef == instr.opcode() )
		setDataAtOffs(0, acc);
	else
		pushDataValue(acc);
}

void EsScriptMachine::doAccessAttributeValue(const EsInstruction& instr)
{
	wxASSERT(iLoadAttributeVal == instr.opcode() || iLoadThisAttributeVal == instr.opcode());
	wxASSERT(EsVariant::VAR_STRING == instr.param().getType());
	
	EsScriptObjectIntf::Ptr obj;
	if( iLoadAttributeVal == instr.opcode() )
	{
		EsAccessorIntf::Ptr val = getDataAtOffs(0);
		obj = val->acc().asExistingObject();
	}
	else
		obj.reset( getActiveMethod()->getThis(), false );
	
	EsAccessorIntf::Ptr acc = obj->internalGetAttribute( instr.param().asString() );
	if( iLoadAttributeVal == instr.opcode() )
		setDataAtOffs(0, acc);
	else
		pushDataValue(acc);	
}

void EsScriptMachine::doCall(const EsInstruction& instr)
{
	EsInstructionOpcode opcode = instr.opcode();
	wxASSERT(instr.param().getType() == EsVariant::VAR_VARIANT_COLLECTION && instr.param().getCount() > 1);
	int paramsCount = instr.param().getItem(0).asInt();
	EsString name = instr.param().getItem(1).asString();
	EsString targetNamespace;
	if( 3 == instr.param().getCount() )
		targetNamespace = instr.param().getItem(2).asString();
	// check func parameters count
	wxASSERT(paramsCount <= static_cast<int>(getDataStackDepth()));
	// extract parameters from the stack and prepare them to the form acceptable for C++ calls
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	for(int idx = 0; idx < paramsCount; ++idx)
		params.addToVariantCollection(popDataValue()->get(), true);
	
	switch(opcode)
	{	
	case iMethodCall:
		if( !targetNamespace.empty() ) // perform this object method call
		{
			EsScriptObjectIntf* scriptObj = getActiveMethod()->getThis();
			wxASSERT( scriptObj );
			pushDataValue( createTemporaryVal(
				scriptObj->callAncestorMethod(name, params, targetNamespace)) );
		}
		else
		{
			wxASSERT(getDataStackDepth() > 0);
			EsReflectedClassIntf::Ptr obj = popDataValue()->get().asExistingObject();
			pushDataValue( createTemporaryVal(obj->callMethod(name, params)) );
		}
		break;
	case iConstructorCall:
		wxASSERT(instr.param().getCount() == 2);
		pushDataValue( 
			createTemporaryVal(createObject(name, params)) );
		break;
	case iExtFuncCall:
		wxASSERT(instr.param().getCount() == 3);
		pushDataValue(
			createTemporaryVal(callExtFunction(name, params, targetNamespace)) );
		break;
	case iFuncCall:
		wxASSERT(instr.param().getCount() == 3);
		pushDataValue(
			createTemporaryVal(callFunction(name, params, targetNamespace)) );
		break;
	default:
		wxFAIL;
		break;	
	}
}

void EsScriptMachine::doBinOp(long opid)
{
	wxASSERT( opidBwAnd <= opid && opidGrEqual >= opid );
	// pop _2 from data stack, operate on _1 inplace, in data stack
	EsAccessorIntf::Ptr _2 = popDataValue();
	EsAccessorIntf::Ptr _1 = getDataAtOffs(0);
	EsVariant tmpVal = _1->get();
	switch( opid )
	{
	// bitwise
	case opidBwAnd:		// a&b
		tmpVal &= _2->get();
		break;	
	case opidBwOr:		// a|b
		tmpVal |= _2->get();
		break;
	case opidBwXor:		// a^b
		tmpVal ^= _2->get();
		break;
	// math
	case opidLsh:			// a<<b
		tmpVal <<= _2->get();
		break;
	case opidRsh:			// a>>b
		tmpVal >>= _2->get();
		break;
	case opidAdd:			// a+b
		tmpVal += _2->get();
		break;
	case opidSub:			// a-b
		tmpVal -= _2->get();
		break;
	case opidMul:			// a*b
		tmpVal *= _2->get();
		break;
	case opidDiv:			// a/b
		tmpVal /= _2->get();
		break;	
	case opidMod:			// a%b
		tmpVal %= _2->get();
		break;
	// logical. we get here, if no short-cirquit-evaluation happened earlier
	case opidLogOr:		// a||b
		tmpVal = tmpVal.asBool() || _2->get().asBool();
		break;
	case opidLogAnd:	// a&&b
		tmpVal = tmpVal.asBool() && _2->get().asBool();
		break;
	// comparison	
	case opidLess:		// a<b
		tmpVal = tmpVal < _2->get();
		break;
	case opidGreater:	// a>b
		tmpVal = tmpVal > _2->get();
		break;
	case opidEqual:		// a==b
		tmpVal = tmpVal == _2->get();
		break;
	case opidNotEqual:// a!=b
		tmpVal = tmpVal != _2->get();
		break;
	case opidLeEqual:	// a<=b
		tmpVal = tmpVal <= _2->get();
		break;
	case opidGrEqual:	// a>=b
		tmpVal = tmpVal >= _2->get();
		break;
	default:
		wxFAIL; // we're never here
		break;
	}
	// replace with temporary accessor on the top of the stack
	EsAccessorIntf::Ptr tmp = createTemporaryVal(tmpVal);
	setDataAtOffs(0, tmp);	
}

void EsScriptMachine::doUnaryOp(long opid)
{
	wxASSERT(opid == opidBwNot || opid == opidNot || opid == opidSub || opid == opidExtScope);
	if( opid != opidExtScope )
	{
		EsVariant val = getDataAtOffs(0)->get();
		switch(opid)
		{
		case opidSub:			// -a
			val = -val;
			break;
		// there is no difference between logical and bitwise not
		// refer to the EsVariant comments on unary ! operator
		case opidBwNot:		// ~a
		case opidNot:			// !a
			val = !val;
			break;
		}
		// replace accessor on the top of the stack
		EsAccessorIntf::Ptr tmp = createTemporaryVal(val);
		setDataAtOffs(0, tmp);
	}
	else
	{
		// todo: treat ext scope access differently
	}	
}

void EsScriptMachine::doAsnOp(const EsInstruction& instr)
{
	EsInstructionOpcode opcode = static_cast<EsInstructionOpcode>(instr.opcode());
	wxASSERT( iAsn == opcode || iVarInit == opcode );
	EsScriptOperatorIds opid;
	bool asStmt = true;
	EsVariant param = instr.param();
	if(iVarInit == opcode)
		opid = opidAsn;
	else
	{
		wxASSERT( EsVariant::VAR_VARIANT_COLLECTION == param.getType() && param.getCount() == 2 );
		opid = static_cast<EsScriptOperatorIds>(param.getItem(0).asLong());
		asStmt = param.getItem(1).asBool();
	}
	wxASSERT( opidAsn <= opid && opidAsnRsh >= opid );
	EsAccessorIntf::Ptr rhs = popDataValue();
	EsAccessorIntf::Ptr lhs;
	if(iVarInit == opcode) 
		lhs = getActiveMethod()->getVariable(param.asString());
	else
		lhs = popDataValue();
	
	// assignments
	switch( opid )
	{
	case opidAsn:			// a=b
		lhs->set( rhs->get() );
		break;
	case opidAsnDec:	// a-=b
		lhs->set( lhs->get() - rhs->get() );
		break;
	case opidAsnInc:	// a+=b
		lhs->set( lhs->get() + rhs->get() );
		break;
	case opidAsnDiv:	// a/=b
		lhs->set( lhs->get() / rhs->get() );
		break;
	case opidAsnMod:	// a%=b
		lhs->set( lhs->get() % rhs->get() );
		break;
	case opidAsnMul:	// a*=b
		lhs->set( lhs->get() * rhs->get() );
		break;
	case opidAsnOr:		// a|=b
		lhs->set( lhs->get() | rhs->get() );
		break;
	case opidAsnAnd:	// a&=b
		lhs->set( lhs->get() & rhs->get() );
		break;
	case opidAsnXor:	// a^=b
		lhs->set( lhs->get() ^ rhs->get() );
		break;
	case opidAsnLsh:	// a<<=b
		lhs->set( lhs->get() << rhs->get() );
		break;
	case opidAsnRsh:	// a>>=b
		lhs->set( lhs->get() >> rhs->get() );
		break;
	default:
		wxFAIL; // we're never here
		break;	
	}
	
	// push assignment result temporary to data stack
	if( !asStmt )
	{
		EsAccessorIntf::Ptr tmp = createTemporaryVal( lhs->get() );
		pushDataValue(tmp);
	}
}			

void EsScriptMachine::doIncDec(const EsInstruction& instr)
{
	wxASSERT(instr.opcode() == iIncDec);
	const EsVariant& param = instr.param();
	wxASSERT(param.getType() == EsVariant::VAR_VARIANT_COLLECTION);
	wxASSERT(param.getCount() == 3);
	bool asStmt = param.getItem(0).asBool();
	EsScriptOperatorIds opid = static_cast<EsScriptOperatorIds>(param.getItem(1).asLong());
	bool postfix = param.getItem(2).asBool();
	// work with lhs inplace on data stack if it's expr.
	// othewise, remove data from stack
	EsAccessorIntf::Ptr val;
	if( asStmt )
	{
		val = popDataValue();
		// we do not care if it's pre- or postfix, in case of statement
		if( opidInc == opid )
			val->set( ++(val->get()) );
		else
			val->set( --(val->get()) );	
	}
	else
	{
		// it's an expression, work with stack
		val = getDataAtOffs(0);
		
		if( postfix )
		{
			// we replace stack variable reference with its value inplace,
			// then change variable itself
			EsVariant v = val->get();
			EsAccessorIntf::Ptr tmp = createTemporaryVal(v);
			if( opidInc == opid )
				val->set( ++v );
			else
				val->set( --v );
			setDataAtOffs(0, tmp);
		}
		else
		{
			if( opidInc == opid )
				val->set( ++(val->get()) );
			else
				val->set( --(val->get()) );	
		}
	}
}

void EsScriptMachine::doLogicCheck(EsInstructions::const_iterator& instr)
{
	// perform short-cirquit logic check
	// get enclosing binop opid
	EsInstructions::const_iterator binopInstr = instr+(*instr).param().asULong();
	EsScriptOperatorIds opid = static_cast<EsScriptOperatorIds>((*binopInstr).param().asLong());
	// stack top contains left-hand operand of logic expression
	// if val evaluates to false - jump to enclosing iBinOpExpr.
	// execution will continue from istruction next to it
	if( (opidLogAnd == opid && !getDataAtOffs(0)->get().asBool()) ||
		(opidLogOr == opid && getDataAtOffs(0)->get().asBool()) ) 
	{
		wxLogDebug(wxT("Logic check short-evaluated, jumping +%d instructions"), binopInstr-instr);
		instr = binopInstr;
	}
}
			
bool EsScriptMachine::doJump(EsInstructions::const_iterator& instr)
{
	EsInstructionOpcode opcode = (*instr).opcode();
	EsVariant param = (*instr).param();
	wxASSERT((iJumpFalse == opcode || 
						iJumpTrue == opcode || 
						iJump == opcode) && 
						2 == param.getCount());
						 
	// optionally check jump conditions
	bool handleJump = true;
	EsJumpOpcode jo = static_cast<EsJumpOpcode>(param.getItem(0).asLong());
	long offs = param.getItem(1).asLong();
	if( opcode != iJump )
		handleJump = iJumpFalse ? !popDataValue()->get().asBool() : popDataValue()->get().asBool();
	
	if( handleJump )
	{
		EsCodeSection::Ptr code = getActiveMethod();
		switch(jo)
		{
		case joRel:
			// perform relative jump
			instr += offs;
			wxLogDebug(wxT("Jumping %d instructions"), offs);			
			break;
		case joAbs:
			instr = code->instrStart()+offs;
			wxLogDebug(wxT("Jumping to instruction at pos %d"), offs);			
			break;
		}
	}
	
	return handleJump;
}

// execution entries
//
// just set current script code as-is. no checks performed
void EsScriptMachine::setActiveMethod(EsCodeSection::Ptr code)
{
	wxASSERT(code);
	
#ifdef DEBUG
	EsCodeMap::const_iterator cit = m_code.find( code->getNamespace() );
	wxASSERT( cit != m_code.end() );
	wxASSERT( (*cit).second );
	EsMethodMap::const_iterator methodIt = (*cit).second->find( constructMethodKey(code) );
	wxASSERT( methodIt != (*cit).second->end() );
	wxASSERT((*methodIt).second->getName() == code->getName());
	wxASSERT((*methodIt).second->getNamespace() == code->getNamespace());
#endif
		
	m_activeCode = code;
	wxLogDebug(wxT("Method '%s'::'%s', taking %d arguments, is set active"), 
		m_activeCode->getNamespace().c_str(),
		m_activeCode->getName().c_str(),
		m_activeCode->getInputParamCnt());
}

EsAccessorIntf::Ptr EsScriptMachine::exec(EsCodeSection::Ptr cs, const EsVariant& params, EsScriptObjectIntf* This /*= 0*/)
{
	wxASSERT(cs);
	EsAccessorIntf::Ptr result;
	// push call stack
	pushCall(getActiveMethod());
	// clone code section before call
	EsCodeSection::Ptr section = cs->clone();
	section->setParams(params);
	section->setThis(This);
	// set this section as the current one
	setActiveMethod(section);
	
	try
	{
		exec(m_activeCode->instrStart(), m_activeCode->instrEnd());
		result = m_activeCode->popResult();
		// pop call
		section = popCall();
		// restore previous code section
		setActiveMethod(section);
	}
	catch(...)
	{
		// pop call
		section = popCall();
		// restore previous code section
		setActiveMethod(section);
		throw; // rethrow
	}
	
	return result;
}

void EsScriptMachine::rewind()
{
	// cleanup stacks
	m_calls.clear();
	m_data.clear();
	// set global section template as active
	setActiveStartupTemplate();
	
	wxLogDebug(wxT("Script machine was rewinded"));
}

// register base script POD (Plain Old Data) metaclasses
void EsScriptMachine::registerPodMetaclasses()
{
	registerMetaclass( EsScriptUINT8Object::createMetaclass(*this) );
	registerMetaclass( EsScriptUINT16Object::createMetaclass(*this) );
	registerMetaclass( EsScriptUINT32Object::createMetaclass(*this) );
	registerMetaclass( EsScriptUINT64Object::createMetaclass(*this) );
	registerMetaclass( EsScriptINT8Object::createMetaclass(*this) );
	registerMetaclass( EsScriptINT16Object::createMetaclass(*this) );
	registerMetaclass( EsScriptINT32Object::createMetaclass(*this) );
	registerMetaclass( EsScriptINT64Object::createMetaclass(*this) );
	registerMetaclass( EsScriptDATETIMEObject::createMetaclass(*this) );
	registerMetaclass( EsScriptFLOATObject::createMetaclass(*this) );
}

// execute entire script
EsVariant EsScriptMachine::exec()
{
	// rewind machine
	rewind();
	// clone active startup section 
	m_activeCode = m_activeCode->clone();
	m_startup = m_activeCode;
	// execute it
	exec(m_activeCode->instrStart(), m_activeCode->instrEnd());
	// return result
	EsAccessorIntf::Ptr result = m_activeCode->popResult();
	if( result )
		return result->get();
	else
		return EsVariant::s_null;
}

// call hash stack manipulation
void EsScriptMachine::pushCall(EsCodeSection::Ptr code)
{	
	m_calls.push_back( code );
	wxLogDebug(wxT("caller '%s::%s' pushed. call stack depth %d"), 
		code->getNamespace().c_str(), code->getName().c_str(), m_calls.size());	
}

EsCodeSection::Ptr EsScriptMachine::popCall()
{
	wxASSERT( !m_calls.empty() );
	EsCodeSection::Ptr code = m_calls.back();
	m_calls.pop_back();
	
	wxLogDebug(wxT("'%s::%s' popped. call stack depth %d"), 
		code->getNamespace().c_str(), code->getName().c_str(), m_calls.size());
	
	return code;
}

#ifdef DEBUG
void EsScriptMachine::traceDataStack() const
{
	for( size_t idx = 0; idx < m_data.size(); ++idx )
		wxLogDebug(wxT("\t[%d]='%s'"), m_data.size()-1-idx, EsScriptMachine::traceVariant(m_data[m_data.size()-1-idx]->get()).c_str());
}
#else
#	define traceDataStack()
#endif

// data stack manipulation
void EsScriptMachine::pushDataValue(EsAccessorIntf::Ptr val)
{
	m_data.push_back(val);
	
	wxLogDebug(wxT("data pushed. stack depth %d"), m_data.size());
	traceDataStack();
}

EsAccessorIntf::Ptr EsScriptMachine::popDataValue()
{
	wxASSERT( !m_data.empty() );
	EsAccessorIntf::Ptr result = m_data.back();
	m_data.pop_back();
	
	wxLogDebug(wxT("data popped. stack depth %d"), m_data.size() );
	traceDataStack();	
	
	return result;
}

// indexed (offset from the top) access to the data stack
EsAccessorIntf::Ptr EsScriptMachine::getDataAtOffs(size_t offs) const
{
	wxASSERT( !m_data.empty() );
	wxASSERT(offs < m_data.size());
	
	EsAccessorIntf::Ptr result = m_data[m_data.size() - offs - 1];
	wxLogDebug(wxT("data '%s' get at stack offs %d"), EsScriptMachine::traceVariant(result->get()).c_str(), offs);

	return result;
}

void EsScriptMachine::setDataAtOffs(size_t offs, EsAccessorIntf::Ptr val)
{
	wxASSERT(!m_data.empty());
	wxASSERT(offs < m_data.size());
	m_data[m_data.size() - offs - 1] = val;
	
	wxLogDebug(wxT("data '%s' set at stack offs %d"), EsScriptMachine::traceVariant(val->get()).c_str(), offs);
}

// temporary values pool support
//
EsTmpAccessor::EsTmpAccessor(const EsVariant& val, EsScriptMachine& owner) :
m_owner(owner),
m_val(val)
{
	wxLogDebug(wxT("temporary value ctor entered"));
}

EsTmpAccessor::~EsTmpAccessor()
{
	wxLogDebug(wxT("temporary value dtor entered"));
}
	
ES_IMPL_INTF_METHOD(void, EsTmpAccessor::destroy)()
{
	// if owner's pool destruction flag is set - actually destroy tmp object
	// otherwise, return it to the owner's tmp pool
	if( m_owner.m_destroying )
		delete this;
	else
	{
		m_owner.m_tmpPool.push_back( EsAccessorIntf::Ptr(this, true) );
		wxLogDebug(wxT("temporary value returned to pool, pool size: %d"), m_owner.m_tmpPool.size());
	}
}

// interface impl
ES_IMPL_INTF_METHOD(EsVariant, EsTmpAccessor::get)() const
{
	wxLogDebug(wxT("EsTmpAccessor::get()='%s'"), EsScriptMachine::traceVariant(m_val).c_str());
	if( m_val.isObject() )
	{
		EsReflectedClassIntf::Ptr obj = m_val.asExistingObject();
		wxASSERT(obj);
		return obj->getProperty(EsVariant::s_value);
	}
	else
		return m_val;
}

ES_IMPL_INTF_METHOD(EsVariant&, EsTmpAccessor::acc)()
{
	wxLogDebug(wxT("EsTmpAccessor::acc() for '%s'"), EsScriptMachine::traceVariant(m_val).c_str());
	return m_val;
}

ES_IMPL_INTF_METHOD(void, EsTmpAccessor::set)(const EsVariant& val)
{
	wxLogDebug(wxT("EsTmpAccessor::set('%s') for '%s'"), EsScriptMachine::traceVariant(val).c_str(), EsScriptMachine::traceVariant(m_val).c_str());
	if( m_val.isObject() )
	{
		EsReflectedClassIntf::Ptr obj = m_val.asExistingObject();
		wxASSERT(obj);
		obj->setProperty(EsVariant::s_value, val);
	}
	else
		m_val = val;
}

EsAccessorIntf::Ptr EsScriptMachine::createTemporaryVal(const EsVariant& tmp)
{
	if( m_tmpPool.empty() )
	{
		EsAccessorIntf::Ptr result(new EsTmpAccessor(tmp, *this));
		wxLogDebug(wxT("\ttmp='%s'"), EsScriptMachine::traceVariant(tmp).c_str());
		return result;
	}
	else
	{
		wxLogDebug(wxT("temporary value created from pool"));
		wxLogDebug(wxT("\ttmp='%s'"), EsScriptMachine::traceVariant(tmp).c_str());
		EsAccessorIntf::Ptr tmpAcc = m_tmpPool.back();
		m_tmpPool.pop_back();
		tmpAcc->acc() = tmp; // set internal value
		return tmpAcc;
	}
}

size_t EsScriptMachine::getTmpPoolSize() const
{
	return m_tmpPool.size();
}

// call script method given name and parameters
EsVariant EsScriptMachine::call(const EsString& name, const EsString& objectOrNamespace /*= s_global*/)
{
	return callFunction(name, EsVariant::s_null, objectOrNamespace);
}

EsVariant EsScriptMachine::call(const EsString& name, const EsVariant& p0, const EsString& objectOrNamespace /*= s_global*/)
{
	return callFunction(name, p0, objectOrNamespace);
}

EsVariant EsScriptMachine::call(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsString& objectOrNamespace /*= s_global*/)
{
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(p0)
		.addToVariantCollection(p1);
	return callFunction(name, params, objectOrNamespace);
}

EsVariant EsScriptMachine::call(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsString& objectOrNamespace /*= s_global*/)
{
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(p0)
		.addToVariantCollection(p1)
		.addToVariantCollection(p2);
	return callFunction(name, params, objectOrNamespace);
}

EsVariant EsScriptMachine::call(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3, const EsString& objectOrNamespace /*= s_global*/)
{
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(p0)
		.addToVariantCollection(p1)
		.addToVariantCollection(p2)
		.addToVariantCollection(p3);		
	return callFunction(name, params, objectOrNamespace);
}

EsVariant EsScriptMachine::call(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3, const EsVariant& p4, const EsString& objectOrNamespace /*= s_global*/)
{
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(p0)
		.addToVariantCollection(p1)
		.addToVariantCollection(p2)
		.addToVariantCollection(p3)
		.addToVariantCollection(p4);	
	return callFunction(name, params, objectOrNamespace);
}

EsVariant EsScriptMachine::call(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3, const EsVariant& p4, const EsVariant& p5, const EsString& objectOrNamespace /*= s_global*/)
{
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(p0)
		.addToVariantCollection(p1)
		.addToVariantCollection(p2)
		.addToVariantCollection(p3)
		.addToVariantCollection(p4)
		.addToVariantCollection(p5);
	return callFunction(name, params, objectOrNamespace);
}

EsVariant EsScriptMachine::callFunction(const EsString& name, const EsVariant& params, const EsString& objectOrNamespace /*= s_global*/)
{
	int paramsCount = params.isEmpty() ? 0 : (params.getType() == EsVariant::VAR_VARIANT_COLLECTION ? params.getCount() : 1);
	MethodInfoKeyT key(paramsCount, name);

	EsCodeSection::Ptr method = getMethodTemplate(key, objectOrNamespace);
	EsAccessorIntf::Ptr result = exec(method, params);
	if( result )
		return result->get();
	else
		return EsVariant::s_null;
}

EsVariant EsScriptMachine::callExtFunction(const EsString& name, const EsVariant& params, const EsString& nameSpace)
{
	int paramsCount = params.isEmpty() ? 0 : (params.getType() == EsVariant::VAR_VARIANT_COLLECTION ? params.getCount() : 1);
	MethodInfoKeyT key(paramsCount, name);
	
	// first, try	to call C++ service
	const ClassInfo* classInfo = ClassInfo::getClassInfo(nameSpace, true);
	const MethodInfo& methodInfo = classInfo->getClassMethodInfo(key);
	
	return methodInfo.classCall(params);
}

void EsScriptMachine::throwMetaclassAlreadyExists(const EsString& name)
{
	EsException::Throw(0, EsException::Generic, wxT("Object or namespace '%s' is already declared"), name.c_str());
}

void EsScriptMachine::throwMetaclassDoesNotExist(const EsString& name)
{
	EsException::Throw(0, EsException::Generic, wxT("Object '%s' is not declared"), name.c_str());
}

EsScriptObjectIntf::Ptr EsScriptMachine::findMetaclass(const EsString &name) const
{
	EsScriptObjectIntf::Ptr result;
	EsScriptMetaclassMap::const_iterator cit = m_metaclassMap.find(name);
	if( cit != m_metaclassMap.end() )
		result = (*cit).second;

	return result;
}

void EsScriptMachine::registerMetaclass(EsScriptObjectIntf::Ptr metaclass)
{
	EsString name = metaclass->getTypeName();
	if( findMetaclass(name) )
		throwMetaclassAlreadyExists(name);

	m_metaclassMap[name] = metaclass;
	
	wxLogDebug(wxT("Metaclass '%s' declaration added to script machine"), name.c_str());
}

EsScriptObjectIntf::Ptr EsScriptMachine::declareMetaclass(const EsString &name, const EsString &baseName)
{
	EsScriptObjectIntf::Ptr result;
	// search class name among C++ reflected classes
	const ClassInfo* info = ClassInfo::getClassInfo(name);
	if( info )
		throwMetaclassAlreadyExists(name);

	EsScriptObjectIntf::Ptr baseMetaclass;
	if( !baseName.empty() )
	{
		baseMetaclass = findMetaclass(baseName);
		if( !baseMetaclass )
			throwMetaclassDoesNotExist(baseName);
		else if( baseMetaclass->isFinal() )
			EsException::Throw(0, EsException::Generic,
				_("Object '%s' is of final type and may not be extended"), 
				baseName.c_str());
	}

	// check for existing namespaces
	EsCodeMap::const_iterator code = m_code.find(name);
	if( code != m_code.end() )
		throwMetaclassAlreadyExists(name);

	EsMethodMapPtr methods;
	methods.reset( new EsMethodMap );
	m_code[name] = methods;

	EsScriptObjectIntf::Ptr metaclass( new EsScriptObject(name, baseMetaclass, methods, *this, EsScriptObject::ofMetaclass,
		EsScriptObjectIntf::EsSharedBinBufferPtr()) );
	// always create default version attributes
	metaclass->addAttribute(wxT("version"), 0);
	metaclass->addAttribute(wxT("revision"), 0);
	// register metaclass
	registerMetaclass(metaclass);
	
	return metaclass;
}

// declare metaclass simple|array field
EsScriptObjectIntf::Ptr EsScriptMachine::declareMetaclassField(const EsString& metaclassName, const EsString& fieldType, const EsString& fieldName)
{
	EsScriptObjectIntf::Ptr metaclass = findMetaclass(metaclassName);
	if( !metaclass )
		throwMetaclassDoesNotExist(metaclassName);
	
	EsScriptObjectIntf::Ptr fieldMetaclass = findMetaclass(fieldType);
	if( !fieldMetaclass )
		throwMetaclassDoesNotExist(fieldType);
	// append field in metaclass declaration mode
	metaclass->addField(fieldName, fieldMetaclass);
	
	return fieldMetaclass;
}

EsScriptObjectIntf::Ptr EsScriptMachine::declareMetaclassArrayField(const EsString& metaclassName, 
	const EsString& fieldType, const EsString& fieldName, EsCodeSection::Ptr sizeExpr)
{
	EsScriptObjectIntf::Ptr metaclass = findMetaclass(metaclassName);
	if( !metaclass )
		throwMetaclassDoesNotExist(metaclassName);
	
	EsScriptObjectIntf::Ptr fieldMetaclass = findMetaclass(fieldType);
	if( !fieldMetaclass )
		throwMetaclassDoesNotExist(fieldType);
	// append field in metaclass declaration mode
	EsScriptObjectIntf::Ptr arrayField = EsScriptArrayObject::createMetaclass(*this, sizeExpr, fieldMetaclass);
	metaclass->addField(fieldName, arrayField);
	
	return arrayField;
}

// declare attribute for the given object
void EsScriptMachine::declareObjectAttribute(EsScriptObjectIntf::Ptr obj, const EsString& attrName, const EsVariant& attrVal)
{
	wxASSERT(obj);
	obj->addAttribute( attrName, attrVal );
}

// create and return object instance by object name.
EsReflectedClassIntf::Ptr EsScriptMachine::createObject(const EsString& name, const EsVariant& params)
{
	const ClassInfo* info = ClassInfo::getClassInfo(name);
	if( info )
		return info->classCallMethod(c_reflectedCtrName, params).asExistingObject();
	
	// search registered metaclasses and create new object, if found
	EsScriptObjectIntf::Ptr obj = findMetaclass(name);
	if( !obj )
		throwMetaclassDoesNotExist(name);

	obj = obj->clone(); // consider cloning as a default (compiler-defined) constructor
	if( obj )
	{
		// find if we're calling non-default constructor method
		if( !params.isEmpty() && 
				(!params.isCollection() ||
						(params.isCollection() && 
						 0 != params.getCount())) )
			obj->callMethod(c_reflectedCtrName, params);
		else
		{
			// if there is default ctor explicitly defined for object - call it
			MethodInfoKeyT defaultCtrKey(0, c_reflectedCtrName);
			EsCodeSection::Ptr explicitDefaultCtr = obj->findScriptedMethod(defaultCtrKey);
			if( explicitDefaultCtr )
				obj->callMethod(c_reflectedCtrName, EsVariant::s_null);
		}
	}
	
	return obj;
}
