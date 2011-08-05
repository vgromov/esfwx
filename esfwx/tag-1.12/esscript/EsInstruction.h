#ifndef _es_instruction_h_
#define _es_instruction_h_

// script operator ids
enum EsScriptOperatorIds {
	// special value
	//
	opidInvalid = -1,
	// incdec ops
	//
	opidInc,			// ++a | a++
	opidDec,			// --a | a--
	// unary operators
	// NB! negation op is the same as opidSub, so use the latter
	//
	opidBwNot,		// ~a
	opidNot,			// !a
	// binary operators
	//
	// access	
	opidExtScope, // ::a
	opidFldAcc,		// a.b
	opidPropAcc,	// a$b
	opidAttrAcc,	// a@b
	opidScopeAcc, // a::b
	// bitwise
	opidBwAnd,		// a&b
	opidBwOr,			// a|b
	opidBwXor,		// a^b
	// math
	opidLsh,			// a<<b
	opidRsh,			// a>>b
	opidAdd,			// a+b
	opidSub,			// a-b
	opidMul,			// a*b
	opidDiv,			// a/b	
	opidMod,			// a%b
	// logical
	opidLogOr,		// a||b
	opidLogAnd,		// a&&b
	opidLess,			// a<b
	opidGreater,	// a>b
	opidEqual,		// a==b
	opidNotEqual,	// a!=b
	opidLeEqual,	// a<=b
	opidGrEqual,	// a>=b
	// assignments
	opidAsn,			// a=b
	opidAsnDec,		// a-=b
	opidAsnInc,		// a+=b
	opidAsnDiv,		// a/=b
	opidAsnMod,		// a%=b
	opidAsnMul,		// a*=b
	opidAsnOr,		// a|=b
	opidAsnAnd,		// a&=b
	opidAsnXor,		// a^=b
	opidAsnLsh,		// a<<=b
	opidAsnRsh,		// a>>=b
};

// script instruction opcodes
enum EsInstructionOpcode {
	iInvalid,
	iVarInit,												// variable initialization instruction
	iLoadConstVal,									// load constant value to data stack
	iLoadVarRef,										// load variable reference to data stack
	iFuncCall,											// script|script namespace service function call
	iExtFuncCall,										// external static service call
	iMethodCall,										// script object method call
	iConstructorCall,								// object constructor call
	iRet,														// return instruction, load top of data stack into retval
	iRetCleanup,										// additional instruction, moving function retval from data stack to retval register
	iAsn,														// assignment instruction
	iBinOpExpr,											// binary operation expression instruction. param contains opid code
	iUnaryOpExpr,										// unary operation expression instruction. param contains opid code
	iLogicCheck,										// logic check instruction, for short-cirquit logical expression evaluation
	iIncDec,												// increment-decrement instruction
	iLoadItemRef,										// perform access to variable's item at index
	iLoadFieldRef,									// perform access to variable's field
	iLoadThisFieldRef,							// perform access to the field using implicit 'this' pointer
	iLoadPropRef,										// perform access to variable's property
	iLoadThisPropRef,								// perform access to property using implicit 'this' pointer
	iLoadAttributeVal,							// perform access to variable's attribute value
	iLoadThisAttributeVal,					// perform access to attribute value using implicit 'this' pointer
	iJump,													// jump, param contains jump opcode + extra info, which is jump type (relative or absolute), followed by instruction offset
	iJumpFalse,											// jump if false, param contains jump opcode + extra info, meaning is the same as iJump		
	iJumpTrue,											// jump if true, param contains jump opcode + extra info, meaning is the same as iJump	
	// special value, must go last
	iOpcodesCount
};

// jump extra opcodes
enum EsJumpOpcode
{
	joRel,		// relative jump - next parameter contains relative jump instruction offset
	joAbs,		// absolute jump - next parameter contains absolute instruction position to jump to
};

class EsInstruction
{
protected:
	// friends-only functionality
	EsInstruction(EsInstructionOpcode opcode, const EsVariant& param);
	
public:	
	EsInstruction();

	bool isValid() const
	{
		return m_opcode != iInvalid;
	}
	
	EsInstructionOpcode opcode() const
	{
		return m_opcode;
	}
	
	const EsVariant& param() const
	{
		return m_param;
	}
	
	// static services
	//
	// get opcode string representation
	static EsString::const_pointer getOpcodeString( EsInstructionOpcode opcode );
	
protected:	
	EsInstructionOpcode m_opcode;
	EsVariant m_param;
	
	friend class EsCodeSection;
	friend class EsScriptMachine;
	friend class EsScriptObject;
};

typedef std::vector<EsInstruction> EsInstructions;
typedef boost::shared_ptr<EsInstructions> EsInstructionsPtr;

#endif // _es_instruction_h_
