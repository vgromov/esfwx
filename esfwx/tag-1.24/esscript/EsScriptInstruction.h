#ifndef _es_instruction_h_
#define _es_instruction_h_

// Forward decls
class EsScriptCodeSection;

// Script operator ids
enum EsScriptOperatorIds {
  // special value
  //
  opidInvalid = -1,
  // incdec ops
  //
  opidInc,      // ++a | a++
  opidDec,      // --a | a--
  // unary operators
  // NB! negation op is the same as opidSub, so use the latter
  //
  opidBwNot,    // ~a
  opidNot,      // !a
  // binary operators
  //
  // access
  opidFldAcc,    // a.b
  opidPropAcc,  // a$b
  opidVsvcAcc,  // a#b
  opidAttrAcc,  // a@b
  opidScopeAcc, // a::b
  // bitwise
  opidBwAnd,    // a&b
  opidBwOr,      // a|b
  opidBwXor,    // a^b
  // math
  opidLsh,      // a<<b
  opidRsh,      // a>>b
  opidAdd,      // a+b
  opidSub,      // a-b
  opidMul,      // a*b
  opidDiv,      // a/b
  opidMod,      // a%b
  // logical
  opidLogOr,    // a||b
  opidLogAnd,    // a&&b
  opidLess,      // a<b
  opidGreater,  // a>b
  opidEqual,    // a==b
  opidNotEqual,  // a!=b
  opidIn,        // a in b
  opidLeEqual,  // a<=b
  opidGrEqual,  // a>=b
  // assignments
  opidAsn,      // a=b
  opidAsnDec,    // a-=b
  opidAsnInc,    // a+=b
  opidAsnDiv,    // a/=b
  opidAsnMod,    // a%=b
  opidAsnMul,    // a*=b
  opidAsnOr,    // a|=b
  opidAsnAnd,    // a&=b
  opidAsnXor,    // a^=b
  opidAsnLsh,    // a<<=b
  opidAsnRsh,    // a>>=b
};

// script opid<-->op string dictionary, AKA operator symbol table
// opids static symbol table
class EsScriptOpIds
{
public:
  EsScriptOperatorIds getId(const EsString& op) const ES_NOTHROW;
  const EsString& getIdStr(long id) const {  return m_idsMap.nameGet(id); }
  static const EsScriptOpIds& instanceGet() ES_NOTHROW;

private:
  EsScriptOpIds();

protected:
  // NB!! opid is item index.
  // ids Map initialization order _must_ be the same as
  // opids declaration order
  EsStringIndexedMap m_idsMap;
};

// script instruction opcodes
enum EsScriptInstructionOpcode {
  iInvalid,
  iVarInit,                        // variable initialization instruction
  iLoadConstVal,                  // load constant value to data stack
  iLoadImmediateVal,              // load immediate value to data stack (for instance, enum val or item label)
  iLoadVarRef,                    // load variable reference to data stack
  iFuncCall,                      // script global method call
  iExtFuncCall,                    // external method call
  iMethodCall,                    // script object method call
  iVsvcCall,                      // explicit variant service call
  iConstructorCall,                // object constructor call
  iRet,                            // return instruction, load top of data stack into retval
  iRetCleanup,                    // additional instruction, moving function retval from data stack to retval register
  iAsn,                            // assignment instruction
  iBinOpExpr,                      // binary operation expression instruction. param contains opid code
  iUnaryOpExpr,                    // unary operation expression instruction. param contains opid code
  iLogicCheck,                    // logic check instruction, for short-cirquit logical expression evaluation
  iIncDec,                        // increment-decrement instruction
  iLoadItemRef,                    // perform access to variable's item at index
  iAutoItemRefCreate,              // create auto item reference object on the data stack
  iAutoItemLoad,                  // load current auto item on data stack with internal index post increment
  iIsOkLoad,                      // load top data accessor isOk result on the data stack
  iLoadMemberRef,                  // perform access to variable's or explicit 'this' field | member variable
  iLoadPropRef,                    // perform access to variable's property
  iLoadThisPropRef,                // perform access to property using implicit 'this' pointer
  iLoadAttributeVal,              // perform access to variable's attribute value
  iLoadThisAttributeVal,          // perform access to attribute value using implicit 'this' pointer
  iJump,                          // jump, param contains jump opcode + extra info, which is jump type (relative or absolute), followed by instruction offset
  iJumpFalse,                      // jump if false, param contains jump opcode + extra info, meaning is the same as iJump
  iJumpTrue,                      // jump if true, param contains jump opcode + extra info, meaning is the same as iJump
  iThrow,                          // exception throw instruction
  iRethrow,                        // caught exception re-throw instruction
  iEnterTryCatch,                  // enter try-catch block scope
  // special value, must go last
  iOpcodesCount
};

/// Jump extra opcodes
enum EsJumpOpcode
{
  joRel,    // relative jump - next parameter contains relative jump instruction offset
  joAbs,    // absolute jump - next parameter contains absolute instruction position to jump to
};

/// Misc consts
///

/// Variable types
enum EsVarType {
  vtLocal = 0,
  vtGlobal,
  vtSystem
};

/// Member reference types
enum EsMemberRefType {
  mrThisField = 0,
  mrThisVar,
  mrFieldOrVar
};

/// Immediate value types
enum EsIvalType {
  ivMetaclass = 0,
  ivEnum,
  ivInplaceCollection
};

/// Script instruction implementation
class EsScriptInstruction
{
public:
  // Instruction payload slot masks
  enum Payload {
    PayloadNone = 0,
    Payload0 = 0x00000001,
    Payload1 = 0x00000002,
    Payload2 = 0x00000004,
    Payload3 = 0x00000008
  };

public:
  /// Default constructor - creates invalid empty instruction
  EsScriptInstruction() ES_NOTHROW :
  m_opcode(iInvalid),
  m_payloadMask(PayloadNone)
  {}

  /// Check if instruction is valid
  inline bool isValid() const ES_NOTHROW
  {
    return m_opcode != iInvalid;
  }

  /// Get instruction opcode
  inline EsScriptInstructionOpcode opcode() const ES_NOTHROW
  {
    return m_opcode;
  }

  /// Get instruction payload mask
  inline ulong payloadMask() const ES_NOTHROW
  {
    return m_payloadMask;
  }

  /// Return true if empty payload
  inline bool isEmptyPayload() const ES_NOTHROW
  {
    return PayloadNone == m_payloadMask;
  }

  /// Read-only access to payload. Debug versions would assert if payload is not set
  ///

  /// Optional parameter 0 read-only access
  ///
  inline EsVarType variableTypeGet() const ES_NOTHROW
  {
    ES_ASSERT(m_payloadMask & Payload0);
    return m_payload0.m_varType;
  }

  inline EsMemberRefType memberRefTypeGet() const ES_NOTHROW
  {
    ES_ASSERT(m_payloadMask & Payload0);
    return m_payload0.m_refType;
  }

  inline EsIvalType immValTypeGet() const ES_NOTHROW
  {
    ES_ASSERT(m_payloadMask & Payload0);
    return m_payload0.m_ivalType;
  }

  inline EsScriptOperatorIds operatorIdGet() const ES_NOTHROW
  {
    ES_ASSERT(m_payloadMask & Payload0);
    return m_payload0.m_opId;
  }

  inline EsJumpOpcode jumpOpcodeGet() const ES_NOTHROW
  {
    ES_ASSERT(m_payloadMask & Payload0);
    return m_payload0.m_jmpId;
  }

  inline long argsCountGet() const ES_NOTHROW
  {
    ES_ASSERT(m_payloadMask & Payload0);
    return m_payload0.m_argscnt;
  }

  inline ulong tryCatchIdxGet() const ES_NOTHROW
  {
    ES_ASSERT(m_payloadMask & Payload0);
    return m_payload0.m_tryCatchIdx;
  }

  /// Optional parameter 1 read-only access
  ///
  inline long jumpOffsGet() const ES_NOTHROW
  {
    ES_ASSERT(m_payloadMask & Payload1);
    return m_payload1.m_jmpOffs;
  }

  inline bool isCallAsStmt() const ES_NOTHROW
  {
    ES_ASSERT(m_payloadMask & Payload1);
    return m_payload1.m_asStmt;
  }

  inline ulong immValArgsCnt() const ES_NOTHROW
  {
    ES_ASSERT(m_payloadMask & Payload1);
    return m_payload1.m_ivalArgsCnt;
  }

  /// Optional parameter 2 read-only access
  ///
  inline bool isPostfixOp() const ES_NOTHROW
  {
    ES_ASSERT(m_payloadMask & Payload2);
    return m_payload2.m_isPostfix;
  }

  /// Optional parameter 3 rea-only access
  ///
  inline const EsString::Array& qnameGet() const ES_NOTHROW
  {
    ES_ASSERT(m_payloadMask & Payload3);
    return m_qname;
  }

  /// Short-cut name-only access
  inline const EsString& nameGet() const ES_NOTHROW
  {
    ES_ASSERT(m_payloadMask & Payload3);
    ES_ASSERT(0 < m_qname.size());
    return m_qname[0];
  }

  /// Access debug information associated with an instruction
  inline EsScriptDebugInfoIntf::Ptr debugInfoGet() const ES_NOTHROW
  {
    return m_debugInfo;
  }

  /// Static services
  ///

  /// Get opcode string representation
  static EsString::const_pointer getOpcodeString(EsScriptInstructionOpcode opcode) ES_NOTHROW;

protected:
  // Friends-only functionality
  //
  EsScriptInstruction(
    EsScriptInstructionOpcode opcode,
    const EsScriptDebugInfoIntf::Ptr& debugInfo
  ) ES_NOTHROW :
    m_opcode(opcode),
    m_payloadMask(PayloadNone),
    m_debugInfo(debugInfo)
  {}

public:
  // Semi-private services, do not use explicitly outside the compiler-bin script reader code
  // TODO: we may want to use better friendship with access key object idiom
  //

  inline esU32 raw0get() const ES_NOTHROW
  {
    ES_ASSERT( m_payloadMask & Payload0 );
    return m_payload0.m_raw0;
  }

  inline esU32 raw1get() const ES_NOTHROW
  {
    ES_ASSERT( m_payloadMask & Payload1 );
    return m_payload1.m_raw1;
  }

  inline esU32 raw2get() const ES_NOTHROW
  {
    ES_ASSERT( m_payloadMask & Payload2 );
    return m_payload2.m_raw2;
  }

  // Opcode setter
  //
  inline EsScriptInstruction& opcodeSet(EsScriptInstructionOpcode opcode) ES_NOTHROW
  {
    m_opcode = opcode;
    return *this;
  }

  // Debug info setter
  //
  inline EsScriptInstruction& dbgInfoSet(const EsScriptDebugInfoIntf::Ptr& dbg) ES_NOTHROW
  {
    m_debugInfo = dbg;
    return *this;
  }

  // Optional param 0 setters
  //
  inline EsScriptInstruction& variableTypeSet(EsVarType type) ES_NOTHROW
  {
    m_payload0.m_varType = type;
    m_payloadMask |= Payload0;
    return *this;
  }

  inline EsScriptInstruction& memberRefTypeSet(EsMemberRefType type) ES_NOTHROW
  {
    m_payload0.m_refType = type;
    m_payloadMask |= Payload0;
    return *this;
  }

  inline EsScriptInstruction& immValTypeSet(EsIvalType type) ES_NOTHROW
  {
    m_payload0.m_ivalType = type;
    m_payloadMask |= Payload0;
    return *this;
  }

  inline EsScriptInstruction& operatorIdSet(EsScriptOperatorIds opId) ES_NOTHROW
  {
    m_payload0.m_opId = opId;
    m_payloadMask |= Payload0;
    return *this;
  }

  inline EsScriptInstruction& jumpOpcodeSet(EsJumpOpcode jmpId) ES_NOTHROW
  {
    m_payload0.m_jmpId = jmpId;
    m_payloadMask |= Payload0;
    return *this;
  }

  inline EsScriptInstruction& argsCountSet( long argscnt) ES_NOTHROW
  {
    m_payload0.m_argscnt = argscnt;
    m_payloadMask |= Payload0;
    return *this;
  }

  inline EsScriptInstruction& tryCatchIdxSet(ulong idx) ES_NOTHROW
  {
    m_payload0.m_tryCatchIdx = idx;
    m_payloadMask |= Payload0;
    return *this;
  }

  inline void raw0set( esU32 val ) ES_NOTHROW
  {
    m_payload0.m_raw0 = val;
    m_payloadMask |= Payload0;
  }

  // Optional param 1 setters
  //
  inline EsScriptInstruction& jumpOffsSet( long jmpOffs) ES_NOTHROW
  {
    m_payload1.m_jmpOffs = jmpOffs;
    m_payloadMask |= Payload1;
    return *this;
  }

  inline EsScriptInstruction& callAsStmtSet(bool asStmt) ES_NOTHROW
  {
    m_payload1.m_asStmt = asStmt;
    m_payloadMask |= Payload1;
    return *this;
  }

  inline EsScriptInstruction& immValArgsCntSet(ulong argscnt) ES_NOTHROW
  {
    m_payload1.m_ivalArgsCnt = argscnt;
    m_payloadMask |= Payload1;
    return *this;
  }

  inline void raw1set( esU32 val ) ES_NOTHROW
  {
    m_payload1.m_raw1 = val;
    m_payloadMask |= Payload1;
  }

  // Optional param 2 setters
  //
  inline EsScriptInstruction& isPostfixSet(bool isPostfix) ES_NOTHROW
  {
    m_payload2.m_isPostfix = isPostfix;
    m_payloadMask |= Payload2;
    return *this;
  }

  inline void raw2set( esU32 val ) ES_NOTHROW
  {
    m_payload2.m_raw2 = val;
    m_payloadMask |= Payload2;
  }

  // Optional param 3 setters
  //
  inline EsScriptInstruction& qnameSet(const EsString::Array& qname) ES_NOTHROW
  {
    m_qname = qname;
    m_payloadMask |= Payload3;
    return *this;
  }

  inline EsScriptInstruction& nameSet(const EsString& name) ES_NOTHROW
  {
    if(m_qname.empty())
      m_qname.push_back(name);
    else
      m_qname[0] = name;
    m_payloadMask |= Payload3;
    return *this;
  }

protected:
  EsScriptInstructionOpcode m_opcode;
  ulong m_payloadMask;

  // Optional param 0
  union {
    EsVarType m_varType;
    EsMemberRefType m_refType;
    EsIvalType m_ivalType;
    EsScriptOperatorIds m_opId;
    EsJumpOpcode m_jmpId;
    long m_argscnt;
    ulong m_tryCatchIdx;
    esU32 m_raw0;

  } m_payload0;

  // Optional param 1
  union {
    long m_jmpOffs;
    bool m_asStmt;
    ulong m_ivalArgsCnt;
    esU32 m_raw1;

  } m_payload1;

  // Optional param 2
  union
  {
    bool m_isPostfix;
    esU32 m_raw2;

  } m_payload2;

  // Optional payload 3 - name (at 0), or qualified name (at 0), with namespaces, from most immediate (next to name, at 1), to most top (at last idx)
  EsString::Array m_qname;

  // Optional debug information
  EsScriptDebugInfoIntf::Ptr m_debugInfo;

  friend class EsScriptCodeSection;
  friend class EsScriptMachine;
  friend class EsScriptObject;
};

typedef std::vector<EsScriptInstruction> EsScriptInstructions;
typedef std::shared_ptr<EsScriptInstructions> EsScriptInstructionsPtr;

#endif // _es_instruction_h_
