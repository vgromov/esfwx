#include "esscriptpch.h"
#pragma hdrstop

#include "EsVar.h"
#include "EsScriptThreadContext.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#include "EsScriptValAccessors.cxx"
//---------------------------------------------------------------------------

EsScriptThreadContext::EsScriptThreadContext(EsScriptMachine& owner, EsThreadId threadId) ES_NOTHROW :
m_owner(owner),
m_threadId(threadId),
m_curInstr(nullptr),
m_csScope(nullptr),
m_executing(false)
{
  startupMethodSetCurrent();
}
//---------------------------------------------------------------------------

EsScriptThreadContext::~EsScriptThreadContext() ES_NOTHROW
{
}
//---------------------------------------------------------------------------

EsScriptCodeSection::Ptr EsScriptThreadContext::currentMethodGet() ES_NOTHROW
{
	ES_ASSERT(m_activeCode);
	return m_activeCode;
}
//---------------------------------------------------------------------------

const EsScriptCodeSection::Ptr& EsScriptThreadContext::currentMethodGet() const ES_NOTHROW
{
	ES_ASSERT(m_activeCode);
	return m_activeCode;
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::startupMethodSetCurrent(const EsScriptCodeSection::Ptr& section /*= EsScriptCodeSection::Ptr()*/)
{
  if( !section )
  {
    methodSetCurrent(
      m_owner.startupMethodGet()
    );
  }
  else
  {
    ES_ASSERT(section->isStartup());
    methodSetCurrent(
      section
    );
  }
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::methodSetCurrent(const EsMethodInfoKeyT& key)
{
  EsScriptCodeSection::Ptr result = m_owner.globalMethodGet(key);

  if(!result)
		EsScriptException::Throw(
      EsString::format(
        esT("Could not find global method '%s', taking %d parameters"),
        key.nameGet(),
        key.parametersCountGet()
      )
    );

  methodSetCurrent(result);
}
//---------------------------------------------------------------------------

EsString EsScriptThreadContext::instructionAsString(const EsScriptInstruction& instr)
{
  EsString result = EsString::format(
    esT("op: '%s'"),
    EsVariant(
		  EsScriptInstruction::getOpcodeString(instr.opcode()),
      EsVariant::ACCEPT_STRING
    )
  );

  if( !instr.isEmptyPayload() )
  {
    ulong mask = instr.payloadMask();
    if( mask & EsScriptInstruction::Payload0 )
      result += EsString::format(
        esT( "; payload[0]: %u" ),
        instr.raw0get()
      );
    if( mask & EsScriptInstruction::Payload1 )
      result += EsString::format(
        esT( "; payload[1]: %u" ),
        instr.raw1get()
      );
    if( mask & EsScriptInstruction::Payload2 )
      result += EsString::format(
        esT( "; payload[2]: %u" ),
        instr.raw2get()
      );
    if( mask & EsScriptInstruction::Payload3 )
      result += EsString::format(
        esT( "; payload[3]: '%s'" ),
        instr.qnameGet()
      );
  }

  EsScriptDebugInfoIntf::Ptr dbg = instr.debugInfoGet();
  if(dbg)
    result += EsString::format(
      esT("; debug: %s"),
      dbg->asString()
    );

  return result;
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::currentInstructionTrack(EsScriptInstructions::const_iterator instr)
{
  m_curInstr = &(*instr);

  m_owner.debuggerInstructionTrace(
    instr
  );
}
//---------------------------------------------------------------------------

EsScriptDebugInfoIntf::Ptr EsScriptThreadContext::currentDebugInfoGet() const ES_NOTHROW
{
  if(m_curInstr)
    return m_curInstr->debugInfoGet();

  return nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class EsScriptOptbl
{
private:
  typedef int (*_proxy)( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& si );

protected:
  // Specific proxies
  //

  // Perform assignment operation using instruction's var ref as lhs, and top of data stack as rhs
  static int iVarInit_iAsn_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& si )
  {
    This.doAsnOp(*si);

    return -1;
  }

  // Load constant from instruction to data stack
  static int iLoadConstVal_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& si )
  {
    ES_ASSERT(This.m_csScope);

    This.m_csScope->stackPush(
      This.m_owner.consts().symbolGet(
        (*si).nameGet(),
        true
      )
    );

    return -1;
  }

  static int iLoadImmediateVal_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& si )
  {
    This.doLoadImmediateVal(*si);

    return -1;
  }

  static int iLoadVarRef_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& si )
  {
    This.doLoadVarRef(*si);

    return -1;
  }

  static int iXXXcall_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& si )
  {
    This.doCall(*si);

    return -1;
  }

  // immediately break current code section execution, popping top of data stack to the result register
  static int iRet_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& )
  {
    ES_ASSERT(This.m_csScope);

    This.currentMethodGet()->resultSet(
      This.m_csScope->stackPop()
    );

    return 1; //< Do return iRet in machine execution loop
  }

  // we returned from the call statement. pop data stack
  static int iRetCleanup_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& )
  {
    ES_ASSERT(This.m_csScope);

    This.m_csScope->stackPop();

    return -1;
  }

  // access indexed item reference
  static int iLoadItemRef_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& )
  {
    This.doConstructItemAccessor();

    return -1;
  }

  // access for-each item reference
  static int iAutoItemRefCreate_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& )
  {
    This.doConstructAutoItemAccessor();

    return -1;
  }

  static int iAutoItemLoad_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& )
  {
    This.doAutoItemLoad();

    return -1;
  }

	// load top data accessor isOk result on the data stack
  static int iIsOkLoad_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& )
  {
    This.doIsOkLoad();

    return -1;
  }

  // create field accessor object
  static int iLoadMemberRef_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& si )
  {
    This.doConstructMemberAccessor(*si);

    return -1;
  }

  // create prop accessor object
  static int iLoadPropRef_iLoadThisPropRef_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& si )
  {
    This.doConstructPropAccessor(*si);

    return -1;
  }

  // access attribute value
  static int iLoadAttributeVal_iLoadThisAttributeVal_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& si )
  {
    This.doAccessAttributeValue(*si);

    return -1;
  }

  static int iJump_iJumpFalse_iJumpTrue_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& si )
	{
    if( This.doJump(si) )
      return 0;

    return -1;
  }

  static int iThrow_iRethrow_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& si )
  {
    This.doThrow(*si);

    return -1;
  }

  static int iEnterTryCatch_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& si )
  {
    EsScriptInstructionOpcode opcode = This.currentMethodGet()->tryCatchBlockExecute(
      (*si).tryCatchIdxGet(),
      This,
      si
    );

    if(iRet == opcode)
      return 1;

    return 0;
  }

  // do binary operation expression
  static int iBinOpExpr_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& si )
  {
    This.doBinOp(
      (*si).operatorIdGet()
    );

    return -1;
  }

  // do unary operation expression
  static int iUnaryOpExpr_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& si )
  {
    This.doUnaryOp(
      (*si).operatorIdGet()
    );

    return -1;
  }

  static int iLogicCheck_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& si )
  {
    This.doLogicCheck( si );

    return -1;
  }

  static int iIncDec_proxy( EsScriptThreadContext& This, EsScriptInstructions::const_iterator& si )
  {
    This.doIncDec( *si );

    return -1;
  }

public:
  // Main proxy dispatcher
  static int exec( EsScriptThreadContext& This, EsScriptInstructionOpcode opcode, EsScriptInstructions::const_iterator& si )
  {
    static const _proxy sc_jtbl[] = {
      nullptr,                                                          //< iInvalid
      EsScriptOptbl::iVarInit_iAsn_proxy,                               //< iVarInit
      EsScriptOptbl::iLoadConstVal_proxy,
      EsScriptOptbl::iLoadImmediateVal_proxy,
      EsScriptOptbl::iLoadVarRef_proxy,
      EsScriptOptbl::iXXXcall_proxy,                                    //< iFuncCall
      EsScriptOptbl::iXXXcall_proxy,                                    //< iExtFuncCall
      EsScriptOptbl::iXXXcall_proxy,                                    //< iMethodCall
      EsScriptOptbl::iXXXcall_proxy,                                    //< iVsvcCall
      EsScriptOptbl::iXXXcall_proxy,                                    //< iConstructorCall
      EsScriptOptbl::iRet_proxy,
      EsScriptOptbl::iRetCleanup_proxy,
      EsScriptOptbl::iVarInit_iAsn_proxy,                               //< iAsn
      EsScriptOptbl::iBinOpExpr_proxy,
      EsScriptOptbl::iUnaryOpExpr_proxy,
      EsScriptOptbl::iLogicCheck_proxy,
      EsScriptOptbl::iIncDec_proxy,
      EsScriptOptbl::iLoadItemRef_proxy,
      EsScriptOptbl::iAutoItemRefCreate_proxy,
      EsScriptOptbl::iAutoItemLoad_proxy,
      EsScriptOptbl::iIsOkLoad_proxy,
      EsScriptOptbl::iLoadMemberRef_proxy,
      EsScriptOptbl::iLoadPropRef_iLoadThisPropRef_proxy,               //< iLoadPropRef
      EsScriptOptbl::iLoadPropRef_iLoadThisPropRef_proxy,               //< iLoadThisPropRef
      EsScriptOptbl::iLoadAttributeVal_iLoadThisAttributeVal_proxy,     //< iLoadAttributeVal
      EsScriptOptbl::iLoadAttributeVal_iLoadThisAttributeVal_proxy,     //< iLoadThisAttributeVal
      EsScriptOptbl::iJump_iJumpFalse_iJumpTrue_proxy,                  //< iJump
      EsScriptOptbl::iJump_iJumpFalse_iJumpTrue_proxy,                  //< iJumpFalse
      EsScriptOptbl::iJump_iJumpFalse_iJumpTrue_proxy,                  //< iJumpTrue
      EsScriptOptbl::iThrow_iRethrow_proxy,                             //< iThrow
      EsScriptOptbl::iThrow_iRethrow_proxy,                             //< iRethrow
      EsScriptOptbl::iEnterTryCatch_proxy
    };

    return sc_jtbl[opcode](
      This,
      si
    );
  }

private:
  EsScriptOptbl() ES_REMOVEDECL;
  EsScriptOptbl(const EsScriptOptbl&) ES_REMOVEDECL;
  EsScriptOptbl& operator=(const EsScriptOptbl&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsScriptInstructionOpcode EsScriptThreadContext::exec(
  EsScriptInstructions::const_iterator beg,
  EsScriptInstructions::const_iterator end,
  EsScriptInstructions::const_iterator& instr
)
{
  instr = beg;

  int res = -1;  //< -1 execute next instruction normally, 0 skip next instruction increment, 1 return with iRet

  try
  {
    while(
      !m_owner.isAborting() &&
      instr < end
    )
    {
      currentInstructionTrack(instr);

#ifdef ESSCRIPT_INSTRUCTION_USE_TRACE
      ES_DEBUG_TRACE(
        esT("Pos: [%d]; %s"),
        instr-beg,
        instructionAsString(
          (*instr)
        )
      );
#endif

      EsScriptInstructionOpcode opcode = (*instr).opcode();

      res = EsScriptOptbl::exec(
        *this,
        opcode,
        instr
      );

      // Handle iRet/++instruction skipping here
      if( 0 < res )
        return iRet;
      else if( 0 == res )
        continue;

      ++instr;
    }
  }
  catch(const EsException& ex)
  {
    ES_DEBUG_TRACE(
      esT("EsScriptThreadContext::exec - catch(const EsException& ex)")
    );

    m_activeCode->resultPop();
    EsScriptException::ReThrow(
      ex,
      currentDebugInfoGet()
    );
  }
  catch(const std::exception& ex)
  {
    ES_DEBUG_TRACE(
      esT("EsScriptThreadContext::exec - catch(const std::exception& ex)")
    );

    m_activeCode->resultPop();
    const EsString& reason = EsString::fromUtf8(ex.what());
    EsScriptException::Throw(
      reason,
      currentDebugInfoGet()
    );
  }
  catch(...)
  {
    ES_DEBUG_TRACE(
      esT("EsScriptThreadContext::exec - catch(...)")
    );

    m_activeCode->resultPop();
    EsScriptDebugInfoIntf::Ptr dbg = currentDebugInfoGet();
    EsString message;

    if(dbg)
      message = esT("Unknown exception occured at: ") + dbg->asString();
    else
      message = esT("Unknown exception occured");

    EsScriptException::Throw(
      message,
      dbg
    );
  }

  return iInvalid;
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doLoadVarRef(const EsScriptInstruction& instr)
{
  ES_ASSERT(iLoadVarRef == instr.opcode());

  EsScriptValAccessorIntf::Ptr var;
  const EsString& name = instr.nameGet();
  switch( instr.variableTypeGet() )
  {
  case EsVarType::vtSystem:
    var = m_owner.sysVars().symbolGet(
      name,
      true
    );
    break;
  case EsVarType::vtGlobal:
    {
      ES_ASSERT(m_csScope);

      const EsScriptCodeSectionScope* startupScope = m_csScope->topScopeGet();
      ES_ASSERT(startupScope);

      EsScriptCodeSection::Ptr startup = startupScope->codeGet();
      ES_ASSERT( startup );
      ES_ASSERT( !startup->isTemplate() );

      var = startup->variableGet(name);
    }
    break;
  case EsVarType::vtLocal:
    var = currentMethodGet()->variableGet(name);
    break;
  }

  ES_ASSERT(var);
  ES_ASSERT(m_csScope);
  m_csScope->stackPush(var);
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doLoadImmediateVal(const EsScriptInstruction& instr)
{
  ES_ASSERT(iLoadImmediateVal == instr.opcode());

  // load constant from instruction to data stack
  EsIvalType ival = instr.immValTypeGet();

  if( EsIvalType::ivInplaceCollection == ival )
  {
    ulong cnt = instr.immValArgsCnt();

    EsVariant::Array tmp;
    tmp.resize( cnt );

    while( cnt-- )
    {
      EsScriptValAccessorIntf::Ptr acc = m_csScope->stackPop();
      tmp[ cnt ] = acc->get();
    }

    m_csScope->stackPush(
      EsScriptTmpValAccessor::create(tmp)
    );
  }
  else
  {
    const EsString& name = instr.nameGet();

    if( EsIvalType::ivEnum == ival )
    {
      EsReflectedClassIntf::Ptr enu = m_owner.enumFind(
        name,
        true
      );

      ES_ASSERT(m_csScope);

      m_csScope->stackPush(
        EsScriptTmpValAccessor::create(enu)
      );
    }
    else if( EsIvalType::ivMetaclass == ival )
    {
      EsScriptObjectIntf::Ptr metaclass = m_owner.metaclassFind(
        name,
        true
      );

      ES_ASSERT(m_csScope);

      m_csScope->stackPush(
        EsScriptTmpValAccessor::create(metaclass)
      );
    }
#ifdef ES_DEBUG
    else
      ES_FAIL; //< We should not be here!!!
#endif
  }
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doConstructItemAccessor()
{
  // create indexed item accessor object and push it onto data stack
  // data stack top-1 = value accessor
  // data stack top = index expression accessor
  //
  ES_ASSERT(m_csScope);

  EsScriptValAccessorIntf::Ptr idx = m_csScope->stackPop();
  EsScriptValAccessorIntf::Ptr val = m_csScope->stackPop();
  EsScriptValAccessorIntf::Ptr acc = EsItemAccessor::create(
    val,
    idx
  );
  m_csScope->stackPush(acc);
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doConstructAutoItemAccessor()
{
  // create automatic indexed item accessor object and push it onto data stack
  // data stack top-1 = value accessor
  // data stack top = index expression accessor
  //
  ES_ASSERT(m_csScope);

  EsScriptValAccessorIntf::Ptr val = m_csScope->stackPop();
  EsScriptValAccessorIntf::Ptr acc = EsAutoItemAccessor::create(val);
  m_csScope->stackPush(acc);
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doConstructMemberAccessor(const EsScriptInstruction& instr)
{
  ES_ASSERT(iLoadMemberRef == instr.opcode());

  const EsString& name = instr.nameGet();
  EsMemberRefType refType = instr.memberRefTypeGet();

  EsScriptObjectIntf::Ptr obj;
  if( EsMemberRefType::mrFieldOrVar == refType)
  {
    ES_ASSERT(m_csScope);

    EsScriptValAccessorIntf::Ptr val = m_csScope->stackPop();
    obj = val->get().asExistingObject();
  }
  else
    obj.reset(currentMethodGet()->thisGet(), false);
  ES_ASSERT(obj);

  EsScriptValAccessorIntf::Ptr acc;
  if( EsMemberRefType::mrThisVar != refType)
  {
    EsScriptObjectIntf::Ptr fld = obj->fieldFind(name);
    if(fld)
      acc = EsScriptTmpValAccessor::create(fld);
  }

  if(!acc && EsMemberRefType::mrThisField != refType)
    acc = obj->variableFind(name, false);
  if(!acc)
		EsScriptException::ThrowFieldOrMemberVarIsNotDeclared(
      name,
      obj->typeNameGet(),
      instr.debugInfoGet()
    );

  ES_ASSERT(m_csScope);
  m_csScope->stackPush(acc);
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doConstructPropAccessor(const EsScriptInstruction& instr)
{
	ES_ASSERT(iLoadPropRef == instr.opcode() || iLoadThisPropRef == instr.opcode());
  ES_ASSERT(m_csScope);

  EsScriptValAccessorIntf::Ptr val;
  if(iLoadPropRef == instr.opcode())
    val = m_csScope->stackPop();
  else
  {
		EsScriptObjectIntf::Ptr thisPtr(
      currentMethodGet()->thisGet(),
      false,
      false
    );

    val = EsScriptTmpValAccessor::create(thisPtr);
  }

	EsScriptValAccessorIntf::Ptr acc = EsPropertyAccessor::create(
		val,
		instr
  );
  m_csScope->stackPush(acc);
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doAccessAttributeValue(const EsScriptInstruction& instr)
{
	ES_ASSERT(iLoadAttributeVal == instr.opcode() || iLoadThisAttributeVal == instr.opcode());
  ES_ASSERT(m_csScope);

  EsScriptObjectIntf::Ptr obj;
  if(iLoadAttributeVal == instr.opcode())
  {
    EsScriptValAccessorIntf::Ptr val = m_csScope->stackPop();

    obj = val->get().asExistingObject();
  }
  else
    obj.reset(
      currentMethodGet()->thisGet(),
      false
    );

  EsScriptValAccessorIntf::Ptr acc;
  const EsString& name = instr.nameGet();
  if(obj)
  {
    // try active code section attributes first, if This is implied
		if(
      iLoadThisAttributeVal == instr.opcode() &&
			currentMethodGet()->attributesAccess()->attributeExists(name)
    )
			acc = EsScriptTmpValAccessor::create(
        currentMethodGet()->attributesAccess()->attributeGet(name)
      );
    else
      acc = EsScriptTmpValAccessor::create(
        obj->attributeGet(name)
      );
  }
  else
    // no "this" implied, try to get attribute from
    // the current code section
		acc = EsScriptTmpValAccessor::create(
      currentMethodGet()->attributesAccess()->attributeGet(name)
    );

  m_csScope->stackPush(acc);
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::variantServiceCall(
  EsVariant& result,
  const EsString& name,
  const EsVariant::Array& params,
  const EsScriptDebugInfoIntf::Ptr& dbg /*= EsScriptDebugInfoIntf::Ptr()*/
)
{
  const EsClassInfo& info = EsVar::classInfoGetStatic();
  const EsMethodInfoKeyT& key = EsMethodInfoKeyT(name, params);
  const EsMethodInfo& methodInfo = info.classMethodInfoGet( key );

  try
  {
    result = methodInfo.classCall(params);
  }
  catch(const EsException& ex)
  {
    ES_DEBUG_TRACE(
      esT("EsScriptThreadContext::variantServiceCall - catch(const EsException& ex)")
    );

    EsScriptException::ReThrow(
      ex,
      dbg
    );
  }
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doVariantServiceCall(
  const EsString& name,
  const EsVariant::Array& params,
  EsVariant& result
)
{
  variantServiceCall(
    result,
    name,
    params,
    currentDebugInfoGet()
  );
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doObjectMethodCall(
  const EsReflectedClassIntf::Ptr& obj,
  const EsString& name,
  const EsVariant::Array& params,
  EsVariant& result
)
{
  const EsMethodInfoKeyT& key = EsMethodInfoKeyT(
    name,
    params
  );

  esU64 typeHash = obj->typeNameGet().hashGet();
  const EsClassInfo& info = obj->classInfoGet();
  const EsMethodInfo* methodInfo = info.methodInfoFind( key );

  if( methodInfo ) //< Simple reflected class
  {
    result = methodInfo->call(
      obj->asWeakReference().get(),
      params
    );
    return;
  }
  else if( obj->isScripted() ) //< Try scripted class
  {
    EsScriptObjectIntf::Ptr sobj = obj;
    ES_ASSERT(sobj);

    EsScriptCodeSection::Ptr method = sobj->findScriptedMethod(key);
    if( method )      //< Scripted class
    {
      EsScriptValAccessorIntf::Ptr ret = exec(
        method,
        params,
        EsScriptEvalMode::evalFunc,
        sobj.get()
      );

      result = ret->get();
      return;
    }
  }

  EsScriptException::Throw(
    EsString::format(
      esT("'%s', taking %d parameters, is not method of '%s', or any of its base objects"),
      name,
      key.parametersCountGet(),
      obj->typeNameGet()
    ),
    currentDebugInfoGet()
  );
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doCallExtMethod(
  const EsString& name,
  const EsVariant::Array& params,
  const EsString& nameSpace,
  EsVariant& result
)
{
  callExtMethod(
    result,
    name,
    params,
    nameSpace,
    currentDebugInfoGet()
  );
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doCallGlobalMethod(
  const EsString& name,
  const EsVariant::Array& params,
  EsVariant& result
)
{
  EsScriptCodeSection::Ptr methodPtr = m_owner.globalMethodGet(
    EsMethodInfoKeyT(
      name,
      params
    )
  );

  const EsScriptCodeSection* method = methodPtr.get();
  ES_ASSERT(method);

  EsScriptValAccessorIntf::Ptr ret = exec(
    method,
    params,
    EsScriptEvalMode::evalFunc,
    nullptr
  );

  ES_ASSERT(ret);
  result = ret->get();
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doCall(const EsScriptInstruction& instr)
{
  EsScriptInstructionOpcode opcode = instr.opcode();

  ulong paramsCount = instr.argsCountGet();
  const EsString& name = instr.nameGet();

  EsString targetNamespace;
  if( 1 < instr.qnameGet().size() )
    targetNamespace = instr.qnameGet()[1];

  // check func parameters count
  ES_ASSERT(m_csScope);
  ES_ASSERT(paramsCount <= m_csScope->stackSizeGet());

  EsVariant::Array params;
  params.reserve(paramsCount);

  try
  {
    // For explicit handling of iVsvcCall preserve the deepest stack parameter for
    // This emulation (variant accessor itself) of the variant service calls
    if(iVsvcCall == opcode)
    {
      ES_ASSERT(paramsCount > 0);
      --paramsCount;
    }

    // Extract parameters from the stack and prepare them to the form acceptable for C++ calls
    for(ulong idx = 0; idx < paramsCount; ++idx)
      params.insert(
        params.begin(),
        m_csScope->stackPop()->get()
      );

    EsVariant callResult;
    switch(opcode)
    {
    case iMethodCall:
      if(!targetNamespace.empty()) // perform this object method call
      {
        EsScriptObjectIntf* scriptObj = currentMethodGet()->thisGet();

        ES_ASSERT(scriptObj);

				callResult = scriptObj->callAncestorMethod(
          name,
          params,
          targetNamespace
        );
      }
      else
      {
        ES_ASSERT(m_csScope->stackSizeGet() > 0);

        // Distinguish between object and variant methods
        EsScriptValAccessorIntf::Ptr acc = m_csScope->stackPop();
        if(acc->get().isObject())
        {
          EsReflectedClassIntf::Ptr obj = acc->get().asExistingObject();
          doObjectMethodCall(
            obj,
            name,
            params,
            callResult
          );
        }
        else
        {
          // Emulate This pointer for the variant service calls
          // by supplying variant accessor itself as the first parameter
          //
          params.insert(params.begin(), acc);
          doVariantServiceCall(
            name,
            params,
            callResult
          );
        }
      }
      break;
    case iConstructorCall:
      callResult = objectCreateWithParameters(
        name,
        params
      );
      break;
    case iExtFuncCall:
      doCallExtMethod(
        name,
        params,
        targetNamespace,
        callResult
      );
      break;
    case iFuncCall:
      doCallGlobalMethod(
        name,
        params,
        callResult
      );
      break;
    case iVsvcCall:
      // Emulate This pointer for the variant service calls
      // by supplying variant accessor itself as the first parameter
      //
      ES_ASSERT(m_csScope);

      params.insert(
        params.begin(),
        m_csScope->stackPop()
      );

      doVariantServiceCall(
        name,
        params,
        callResult
      );
      break;
    default:
      ES_FAIL;
      break;
    }

    m_csScope->stackPush(
      EsScriptTmpValAccessor::create(callResult)
    );
  }
  catch(const EsException& ex)
  {
    ES_DEBUG_TRACE(
      esT("void EsScriptThreadContext::doCall(const EsScriptInstruction& instr) - catch(const EsException& ex)")
    );

    m_activeCode->resultPop();
    EsScriptException::ReThrow(
      ex,
      currentDebugInfoGet()
    );
  }
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::binOpEval(long opid, const EsVariant& _1, const EsVariant& _2, EsVariant& result)
{
  ES_ASSERT(opidBwAnd <= opid && opidGrEqual >= opid);

  switch(opid)
  {
    // bitwise
  case opidBwAnd: // a&b
    result = _1 & _2;
    break;
  case opidBwOr: // a|b
    result = _1 | _2;
    break;
  case opidBwXor: // a^b
    result = _1 ^ _2;
    break;
    // math
  case opidLsh: // a<<b
    result = _1 << _2;
    break;
  case opidRsh: // a>>b
    result = _1 >> _2;
    break;
  case opidAdd: // a+b
    result = _1 + _2;
    break;
  case opidSub: // a-b
    result = _1 - _2;
    break;
  case opidMul: // a*b
    result = _1 * _2;
    break;
  case opidDiv: // a/b
    result = _1 / _2;
    break;
  case opidMod: // a%b
    result = _1 % _2;
    break;
    // logical. we get here, if no short-cirquit-evaluation happened earlier
  case opidLogOr: // a||b
    result = _1.asBool() || _2.asBool();
    break;
  case opidLogAnd: // a&&b
    result = _1.asBool() && _2.asBool();
    break;
    // comparison
  case opidLess: // a<b
    result = _1 < _2;
    break;
  case opidGreater: // a>b
    result = _1 > _2;
    break;
  case opidEqual: // a==b
    result = _1 == _2;
    break;
  case opidNotEqual: // a!=b
    result = _1 != _2;
    break;
  case opidIn: // a in b
    result = _2.has(_1);
    break;
  case opidLeEqual: // a<=b
    result = _1 <= _2;
    break;
  case opidGrEqual: // a>=b
    result = _1 >= _2;
    break;
  default:
    ES_FAIL; // we're never here
    break;
  }
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doBinOp(long opid)
{
  ES_ASSERT(opidBwAnd <= opid && opidGrEqual >= opid);
  ES_ASSERT(m_csScope);

  EsScriptValAccessorIntf::Ptr _2 = m_csScope->stackPop();
  EsScriptValAccessorIntf::Ptr _1 = m_csScope->stackPop();
  EsVariant tmp;

  binOpEval(
    opid,
    _1->get(),
    _2->get(),
    tmp
  );

  // Push temporary accessor on the top of the stack
  m_csScope->stackPush(
    EsScriptTmpValAccessor::create(tmp)
  );
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::unaryOpEval(long opid, EsVariant& val)
{
	ES_ASSERT(opid == opidBwNot || opid == opidNot || opid == opidSub );

  switch(opid)
  {
  case opidSub: // -a
    val = -val;
    break;
  case opidBwNot: // ~a
    val = ~val;
    break;
  case opidNot: // !a
    val = !val;
    break;
  }
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doUnaryOp(long opid)
{
	ES_ASSERT(opid == opidBwNot || opid == opidNot || opid == opidSub || opid == opidScopeAcc);

  if(opid != opidScopeAcc)
  {
    ES_ASSERT(m_csScope);

    EsScriptValAccessorIntf::Ptr acc = m_csScope->stackPop();
    EsVariant tmp = acc->get();

    unaryOpEval(
      opid,
      tmp
    );

    // replace accessor on the top of the stack
    m_csScope->stackPush(
      EsScriptTmpValAccessor::create(
        tmp
      )
    );
  }
  else
  {
    // todo: treat ext scope access differently
    // not supported so far
    ES_FAIL;
  }
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doAsnOp(const EsScriptInstruction& instr)
{
	EsScriptInstructionOpcode opcode = instr.opcode();
  ES_ASSERT(iAsn == opcode || iVarInit == opcode);

  EsScriptOperatorIds opid;
  bool asStmt = true;

  if(iVarInit == opcode)
    opid = opidAsn;
  else
  {
    opid = instr.operatorIdGet();
    asStmt = instr.isCallAsStmt();
  }

  ES_ASSERT(opidAsn <= opid && opidAsnRsh >= opid);
  ES_ASSERT(m_csScope);

  EsScriptValAccessorIntf::Ptr rhs = m_csScope->stackPop();
  EsScriptValAccessorIntf::Ptr lhs;

  if(iVarInit == opcode)
    lhs = currentMethodGet()->variableGet(
      instr.nameGet()
    );
  else
    lhs = m_csScope->stackPop();

  // assignments
  switch(opid)
  {
  case opidAsn: // a=b
    lhs->set(
      rhs->get()
    );
    break;
  case opidAsnDec: // a-=b
    if( lhs->getReturnsTemporary() )
      lhs->set(
        lhs->get() - rhs->get()
      );
    else
      lhs->get() -= rhs->get();
    break;
  case opidAsnInc: // a+=b
    if( lhs->getReturnsTemporary() )
      lhs->set(
        lhs->get() + rhs->get()
      );
    else
      lhs->get() += rhs->get();
    break;
  case opidAsnDiv: // a/=b
    if( lhs->getReturnsTemporary() )
      lhs->set(
        lhs->get() / rhs->get()
      );
    else
      lhs->get() /= rhs->get();
    break;
  case opidAsnMod: // a%=b
    if( lhs->getReturnsTemporary() )
      lhs->set(
        lhs->get() % rhs->get()
      );
    else
      lhs->get() %= rhs->get();
    break;
  case opidAsnMul: // a*=b
    if( lhs->getReturnsTemporary() )
      lhs->set(
        lhs->get() * rhs->get()
      );
    else
      lhs->get() *= rhs->get();
    break;
  case opidAsnOr: // a|=b
    if( lhs->getReturnsTemporary() )
      lhs->set(
        lhs->get() | rhs->get()
      );
    else
      lhs->get() |= rhs->get();
    break;
  case opidAsnAnd: // a&=b
    if( lhs->getReturnsTemporary() )
      lhs->set(
        lhs->get() & rhs->get()
      );
    else
      lhs->get() &= rhs->get();
    break;
  case opidAsnXor: // a^=b
    if( lhs->getReturnsTemporary() )
      lhs->set(
        lhs->get() ^ rhs->get()
      );
    else
      lhs->get() ^= rhs->get();
    break;
  case opidAsnLsh: // a<<=b
    if( lhs->getReturnsTemporary() )
      lhs->set(
        lhs->get() << rhs->get()
      );
    else
      lhs->get() <<= rhs->get();
    break;
  case opidAsnRsh: // a>>=b
    if( lhs->getReturnsTemporary() )
      lhs->set(
        lhs->get() >> rhs->get()
      );
    else
      lhs->get() >>= rhs->get();
    break;
  default:
    ES_FAIL; // we're never here
    break;
  }

  // push assignment result temporary to data stack, if used as an expression
  if( !asStmt )
    m_csScope->stackPush(
      EsScriptTmpValAccessor::create( lhs->get() )
    );
}
//---------------------------------------------------------------------------

static void inline incdecOp(EsScriptOperatorIds opid, const EsScriptValAccessorIntf::Ptr& val)
{
  if(opidInc == opid)
  {
    if( val->getReturnsTemporary() )
      val->set(
        ++val->get()
      );
    else
      ++val->get();
  }
  else
  {
    if( val->getReturnsTemporary() )
      val->set(
        --val->get()
      );
    else
      --val->get();
  }
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doIncDec(const EsScriptInstruction& instr)
{
  ES_ASSERT(instr.opcode() == iIncDec);

  bool asStmt = instr.isCallAsStmt();
  EsScriptOperatorIds opid = instr.operatorIdGet();
  bool postfix = instr.isPostfixOp();

  // Work with lhs inplace on data stack if it's expr.
  // othewise, remove data from stack
  //
  ES_ASSERT(m_csScope);

  EsScriptValAccessorIntf::Ptr val;
  if( asStmt ) //< We do not care if it's pre- or postfix, in case of statement
  {
    val = m_csScope->stackPop();

    incdecOp(
      opid,
      val
    );
  }
  else
  {
    // it's an expression, work with stack
    val = m_csScope->stackPop();

    EsScriptValAccessorIntf::Ptr tmpa;
    if(postfix)
      tmpa = EsScriptTmpValAccessor::create(
        val->get()
      );

    incdecOp(
      opid,
      val
    );

    if(postfix)
      m_csScope->stackPush(tmpa);
    else
      m_csScope->stackPush(val);
  }
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doLogicCheck(EsScriptInstructions::const_iterator& instr)
{
  ES_ASSERT( iLogicCheck == (*instr).opcode() );

  // Perform short-cirquit logic check
  // get enclosing binop opid
  //
	EsScriptInstructions::const_iterator binopInstr = instr+(*instr).jumpOffsGet();
	EsScriptOperatorIds opid = (*binopInstr).operatorIdGet();

  // Stack top contains left-hand operand of logic expression
  // if val evaluates to false - jump to enclosing iBinOpExpr.
  // execution will continue from istruction next to it
  //
  ES_ASSERT(m_csScope);

  EsScriptValAccessorIntf::Ptr check = m_csScope->stackPop();
  bool isTrue = check->get().asBool();

  if(
    (opidLogAnd == opid && !isTrue) ||
    (opidLogOr == opid && isTrue)
  )
  {
		ESSCRIPT_MACHINE_TRACE2(esT("Logic check short-evaluated, jumping +%d instructions"), binopInstr-instr)
		instr = binopInstr;
  }

  m_csScope->stackPush(check);
}
//---------------------------------------------------------------------------

bool EsScriptThreadContext::doJump(EsScriptInstructions::const_iterator& instr)
{
  EsScriptInstructionOpcode opcode = (*instr).opcode();
	ES_ASSERT(
    iJumpFalse == opcode ||
    iJumpTrue == opcode ||
    iJump == opcode
  );

  // optionally check jump conditions
  bool handleJump = true;

  if(opcode != iJump)
	{
    ES_ASSERT(m_csScope);

    bool isTrue = m_csScope->stackPop()->get().asBool();

  	handleJump = (iJumpFalse == opcode) ?
      !isTrue :
      isTrue;
  }

  if(handleJump)
  {
    EsJumpOpcode jo = (*instr).jumpOpcodeGet();
    long offs = (*instr).jumpOffsGet();

    EsScriptCodeSection::Ptr code = currentMethodGet();
    size_t instrStartPos = code->instructionStartPosGet();
    size_t instrPos = instr - code->instructionStartGet();
    size_t instrEndPos = code->instructionEndPosGet();

    switch(jo)
    {
    case joRel:
      // perform relative jump
      ES_ASSERT(instrPos + offs <= instrEndPos);
      instr += offs;
			ESSCRIPT_MACHINE_TRACE2(esT("Jumping %d instructions"), offs)
			break;
    case joAbs:
      ES_ASSERT(instrStartPos + offs <= instrEndPos);
      instr = code->instructionStartGet() + offs;
			ESSCRIPT_MACHINE_TRACE2(esT("Jumping to instruction at pos %d"), offs)
			break;
    }
  }

  return handleJump;
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doThrow(const EsScriptInstruction& instr)
{
  EsScriptDebugInfoIntf::Ptr debugInfo = instr.debugInfoGet();
  if(iThrow == instr.opcode())
  {
    ES_ASSERT(m_csScope);

    const EsString& reason = m_csScope->stackPop()->get().asString();
    EsScriptException::Throw(reason, debugInfo);
  }
  else // just rethrow
  {
    ES_ASSERT(iRethrow == instr.opcode());
    // before executing rethrow - deselect currently active try-catch block,
    // otherwise, it will not be able to clear itself
    currentMethodGet()->activeTryCatchBlockDeselect();
    throw;
  }
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doAutoItemLoad()
{
  ES_ASSERT(m_csScope);

  EsScriptValAccessorIntf::Ptr autoIdxAcc = m_csScope->stackPop();
  EsScriptValAccessorIntf::Ptr tmp = EsScriptTmpValAccessor::create(
    autoIdxAcc->get()
  );
  m_csScope->stackPush(autoIdxAcc);
  m_csScope->stackPush(tmp);
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::doIsOkLoad()
{
  ES_ASSERT(m_csScope);

  EsScriptValAccessorIntf::Ptr subj = m_csScope->stackPop();
  EsScriptValAccessorIntf::Ptr tmp = EsScriptTmpValAccessor::create(
    subj->isOk()
  );
  m_csScope->stackPush(subj);
  m_csScope->stackPush(tmp);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void EsScriptThreadContext::methodSetCurrent(const EsScriptCodeSection::Ptr& code)
{
  ES_ASSERT(code);

  m_activeCode = code;

	ESSCRIPT_MACHINE_TRACE3(
    esT("Method '%s', taking %d arguments, is set active"),
		m_activeCode->nameGet(),
		m_activeCode->inputParametersCntGet()
  )
}
//---------------------------------------------------------------------------

EsScriptValAccessorIntf::Ptr EsScriptThreadContext::exec(
  const EsScriptCodeSection* pcs,
  const EsVariant& params,
  EsScriptEvalMode evalMode,
  EsScriptObjectIntf* This
)
{
//  EsCriticalSectionLocker lock(m_cs);

  ES_ASSERT(pcs);

  // Set machine executing flag
  m_executing = true;

  // Clone code section before call
  EsScriptCodeSection::Ptr section = pcs->clone(
    This,
    params
  );

  // Create section execution scope
  EsScriptCodeSectionScope scope(
    *this,
    section
  );

  EsScriptInstructions::const_iterator instr;
  exec(
    section->instructionStartGet(),
    section->instructionEndGet(),
    instr
  );

  if(
    EsScriptEvalMode::evalExpr == evalMode &&
    !scope.stackIsEmpty()
  )
    section->resultSet(
      scope.stackPop()
    );

  EsScriptValAccessorIntf::Ptr result = section->resultPop();
  ES_ASSERT(result);

  return result;
}
//---------------------------------------------------------------------------

EsVariant EsScriptThreadContext::exec()
{
	EsScriptValAccessorIntf::Ptr result = exec(
    m_activeCode,
    EsVariant::null(),
    EsScriptEvalMode::evalExpr,
    nullptr
  );

  return result->get();
}
//---------------------------------------------------------------------------

EsVariant EsScriptThreadContext::callGlobalMethod(const EsMethodInfoKeyT& key, const EsVariant& params)
{
	ES_ASSERT((int)key.parametersCountGet() ==
    (params.isEmpty() ?
		  0 :
      ( params.typeGet() == EsVariant::VAR_VARIANT_COLLECTION ?
          params.countGet() :
          1
      )
    )
  );

  EsScriptCodeSection::Ptr method = m_owner.globalMethodGet(key);
	EsScriptValAccessorIntf::Ptr result = exec(
    method,
    params,
    EsScriptEvalMode::evalFunc,
    nullptr
  );
  ES_ASSERT(result);

  return result->get();
}
//---------------------------------------------------------------------------

EsVariant EsScriptThreadContext::callGlobalMethod(const EsString& name, const EsVariant& params)
{
	ulong paramsCount = params.isEmpty() ?
    0 :
    (
      params.typeGet() == EsVariant::VAR_VARIANT_COLLECTION ?
        params.countGet() :
        1
    );

  EsMethodInfoKeyT key(
    paramsCount,
    name
  );

  return callGlobalMethod(
    key,
    params
  );
}
//---------------------------------------------------------------------------

void EsScriptThreadContext::callExtMethod(
  EsVariant& result,
  const EsString& name,
  const EsVariant& params,
  const EsString& nameSpace,
  const EsScriptDebugInfoIntf::Ptr& dbg /*= EsScriptDebugInfoIntf::Ptr()*/
)
{
	ulong paramsCount = params.isEmpty() ?
    0 :
    (params.typeGet() == EsVariant::VAR_VARIANT_COLLECTION ?
      params.countGet() :
      1
    );

  // try	to call reflected C++ service
  const EsClassInfo* classInfo = EsClassInfo::classInfoGet(nameSpace, true);
  const EsMethodInfo& methodInfo = classInfo->classMethodInfoGet(
    EsMethodInfoKeyT(
      paramsCount,
      name
    )
  );

  try
  {
    result = methodInfo.classCall(params);
  }
  catch(const EsException& ex)
  {
    ES_DEBUG_TRACE(
      esT("EsScriptThreadContext::internalCallExtMethod - catch(const EsException& ex)")
    );

    EsScriptException::ReThrow(
      ex,
      dbg
    );
  }
}
//---------------------------------------------------------------------------

class EsScriptObjectCtrExecutor: public EsScriptObjectTopDownHierarchyTraverser
{
protected:
  typedef std::pair<EsScriptObjectIntf*, EsScriptCodeSection::Ptr> Ctr;
  typedef std::vector<Ctr> Ctrs;

public:
	EsScriptObjectCtrExecutor(EsScriptObjectIntf* obj, const EsMethodInfoKeyT& ctrKey) ES_NOTHROW :
	EsScriptObjectTopDownHierarchyTraverser(obj, false, true),
  m_ctrKey(ctrKey)
  {
    m_ctrs.reserve(8);
  }

  virtual bool objectProcess(const EsScriptObjectIntf* obj) ES_NOTHROW
  {
    ES_ASSERT(obj);

    EsScriptCodeSection::Ptr ctrCode = obj->findScriptedMethod(m_ctrKey, false);
    if(ctrCode)
			m_ctrs.push_back( std::make_pair(const_cast<EsScriptObjectIntf*>(obj), ctrCode) );

    return true;
  }

  void call(EsScriptThreadContext& vm, const EsVariant& params)
  {
    for(size_t idx = 0; idx < m_ctrs.size(); ++idx)
    {
      const Ctr& ctr = m_ctrs[idx];
			vm.exec(
        ctr.second,
        params,
        EsScriptEvalMode::evalFunc,
        ctr.first
      );
    }
  }

protected:
  EsMethodInfoKeyT m_ctrKey;
  Ctrs m_ctrs;
};
//---------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsScriptThreadContext::objectCreateWithParameters(
  const EsString& name,
  const EsVariant& params
)
{
  const EsClassInfo* info = EsClassInfo::classInfoGet(name);
  if(info)
  {
		EsReflectedClassIntf::Ptr result = info->classCallMethod(
      EsStdNames::reflectedCtr(),
      params
    ).asExistingObject();

    // Add script object creation support for streams, injecting current script metaclass manager as object factory instance
    EsStreamIntf::Ptr stream = result;
    if(stream)
      stream->factorySet(
        m_owner.metaGet()
      );

    return result;
  }

  // search registered metaclasses and create new object, if found
  EsScriptObjectIntf::Ptr obj = m_owner.metaclassFind(name);
  if(!obj)
    EsScriptException::ThrowMetaclassDoesNotExist(name, currentDebugInfoGet());

	obj = obj->clone(); // consider cloning as a default (compiler-defined) constructor
  if(obj)
  {
    // find if we're calling non-default constructor method
    ulong paramsCnt = 0;
    if(!params.isEmpty())
    {
      if(!params.isCollection())
        paramsCnt = 1;
      else
        paramsCnt = params.countGet();
    }

    // if there is default ctor explicitly defined for object - call it
    EsMethodInfoKeyT ctrKey(paramsCnt, EsStdNames::reflectedCtr());
    // automatically find and call all declared constructors, from the most basic
    // up the most derived
    EsScriptObjectCtrExecutor ctrExec(obj.get(), ctrKey);
    ctrExec.traverse();
    ctrExec.call(*this, params);
  }

  return obj;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// EsScriptCodeSectionScope implementation
//

EsScriptCodeSectionScope::EsScriptCodeSectionScope( EsScriptThreadContext& This, const EsScriptCodeSection::Ptr& code ) :
m_this(This),
m_code(code),
m_old(m_this.m_csScope)
{
#if 1 == ES_USE_VECTOR_FOR_STACK
  m_snapshots.reserve(depthDefault);
#endif

  ES_ASSERT(m_code);
  m_this.m_csScope = this;

	ESSCRIPT_MACHINE_CALL_TRACE2(
    esT("EsScriptCodeSectionScope::EsScriptCodeSectionScope: code '%s' pushed"),
    m_code->nameGet().c_str()
  )

  if(m_code->isStartup())
    m_this.startupMethodSetCurrent(m_code);
  else
    m_this.methodSetCurrent(m_code);
}
//---------------------------------------------------------------------------

EsScriptCodeSectionScope::~EsScriptCodeSectionScope()
{
	ESSCRIPT_MACHINE_CALL_TRACE2(
    esT("EsScriptCodeSectionScope::~EsScriptCodeSectionScope(): code '%s' popped"),
    m_code->nameGet().c_str()
  )

  m_this.m_csScope = m_old;
  if(m_old)
    m_this.methodSetCurrent(m_old->m_code);
  else
  {
    m_this.startupMethodSetCurrent();
    m_this.m_executing = false; //< We're back to the pre-started point

  	ESSCRIPT_MACHINE_CALL_TRACE1(
      esT("EsScriptCodeSectionScope::~EsScriptCodeSectionScope(): executing flag is reset")
    );
  }

  m_stack.clear();
}
//---------------------------------------------------------------------------

const EsScriptCodeSectionScope* EsScriptCodeSectionScope::topScopeGet() const ES_NOTHROW
{
  const EsScriptCodeSectionScope* scope = this;

  while( scope->m_old )
    scope = scope->m_old;

  return scope;
}
//---------------------------------------------------------------------------

void EsScriptCodeSectionScope::stackSnapshotPush()
{
	ESSCRIPT_MACHINE_CALL_TRACE2(esT("code '%s' stack snapshot taken"), m_code->nameGet().c_str())

  EsScriptDataStack::Ptr ptr(
    new EsScriptDataStack(m_stack)
  );

  m_snapshots.push_back(
    ptr
  );
}
//---------------------------------------------------------------------------

void EsScriptCodeSectionScope::stackSnapshotPop()
{
  ES_ASSERT( !m_snapshots.empty() );

	ESSCRIPT_MACHINE_CALL_TRACE2(esT("code '%s' stack snapshot restored"), m_code->nameGet().c_str())

  m_stack = *m_snapshots.back().get();
  m_snapshots.pop_back();
}
//---------------------------------------------------------------------------

