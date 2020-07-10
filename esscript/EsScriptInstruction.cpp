#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptInstruction.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

EsScriptOpIds::EsScriptOpIds()
{
  // incdec ops
  //
  m_idsMap.itemAdd(esT("++"));  //opidInc,      // ++a | a++
  m_idsMap.itemAdd(esT("--"));  //opidDec,      // --a | a--
  // unary operators
  // NB! negation op is the same as opidSub, so use the latter
  //
  m_idsMap.itemAdd(esT("~"));    //opidBwNot,    // ~a
  m_idsMap.itemAdd(esT("!"));    //opidNot,      // !a
  // binary operators
  //
  // access
  m_idsMap.itemAdd(esT("."));    //opidFldAcc,    // a.b
  m_idsMap.itemAdd(esT("$"));    //opidPropAcc,  // a$b
  m_idsMap.itemAdd(esT("#"));    //opidVsvcAcc,// a#b
  m_idsMap.itemAdd(esT("@"));    //opidAttrAcc,  // a@b
  m_idsMap.itemAdd(esT("::"));  //opidScopeAcc, // a::b
  // bitwise
  m_idsMap.itemAdd(esT("&"));    //opidBwAnd,    // a&b
  m_idsMap.itemAdd(esT("|"));    //opidBwOr,      // a|b
  m_idsMap.itemAdd(esT("^"));    //opidBwXor,    // a^b
  // math
  m_idsMap.itemAdd(esT("<<"));  //opidLsh,      // a<<b
  m_idsMap.itemAdd(esT(">>"));  //opidRsh,      // a>>b
  m_idsMap.itemAdd(esT("+"));    //opidAdd,      // a+b
  m_idsMap.itemAdd(esT("-"));    //opidSub,      // a-b
  m_idsMap.itemAdd(esT("*"));    //opidMul,      // a*b
  m_idsMap.itemAdd(esT("/"));    //opidDiv,      // a/b
  m_idsMap.itemAdd(esT("%"));    //opidMod,      // a%b
  // logical
  m_idsMap.itemAdd(esT("||"));  //opidLogOr,    // a||b
  m_idsMap.itemAdd(esT("&&"));  //opidLogAnd,    // a&&b
  m_idsMap.itemAdd(esT("<"));    //opidLess,      // a<b
  m_idsMap.itemAdd(esT(">"));    //opidGreater,  // a>b
  m_idsMap.itemAdd(esT("=="));  //opidEqual,    // a==b
  m_idsMap.itemAdd(esT("!="));  //opidNotEqual,  // a!=b
  m_idsMap.itemAdd(esT("in"));  //opidIn,        // a in b
  m_idsMap.itemAdd(esT("<="));  //opidLeEqual,  // a<=b
  m_idsMap.itemAdd(esT(">="));  //opidGrEqual,  // a>=b
  // assignments
  m_idsMap.itemAdd(esT("="));    //opidAsn,      // a=b
  m_idsMap.itemAdd(esT("-="));  //opidAsnDec,    // a-=b
  m_idsMap.itemAdd(esT("+="));  //opidAsnInc,    // a+=b
  m_idsMap.itemAdd(esT("/="));  //opidAsnDiv,    // a/=b
  m_idsMap.itemAdd(esT("%="));  //opidAsnMod,    // a%=b
  m_idsMap.itemAdd(esT("*="));  //opidAsnMul,    // a*=b
  m_idsMap.itemAdd(esT("|="));  //opidAsnOr,    // a|=b
  m_idsMap.itemAdd(esT("&="));  //opidAsnAnd,    // a&=b
  m_idsMap.itemAdd(esT("^="));  //opidAsnXor,    // a^=b
  m_idsMap.itemAdd(esT("<<="));  //opidAsnLsh,    // a<<=b
  m_idsMap.itemAdd(esT(">>="));  //opidAsnRsh,    // a>>=b
}
//---------------------------------------------------------------------------

EsScriptOperatorIds EsScriptOpIds::getId(const EsString& op) const ES_NOTHROW
{
  ulong idx = m_idsMap.itemFind(op);
  if( EsStringIndexedMap::npos == idx )
    return opidInvalid;

  return static_cast<EsScriptOperatorIds>(idx);
}
//---------------------------------------------------------------------------

// opids static initializer
const EsScriptOpIds& EsScriptOpIds::instanceGet() ES_NOTHROW
{
  static EsScriptOpIds s_opids;
  return s_opids;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

const EsString::const_pointer c_opStrings[iOpcodesCount] = {
  esT("iInvalid"),
  esT("iVarInit"),
  esT("iLoadConstVal"),
  esT("iLoadImmediateVal"),
  esT("iLoadVarRef"),
  esT("iFuncCall"),
  esT("iExtFuncCall"),
  esT("iMethodCall"),
  esT("iVsvcCall"),
  esT("iConstructorCall"),
  esT("iRet"),
  esT("iRetCleanup"),
  esT("iAsn"),
  esT("iBinOpExpr"),
  esT("iUnaryOpExpr"),
  esT("iLogicCheck"),
  esT("iIncDec"),
  esT("iLoadItemRef"),
  esT("iAutoItemRefCreate"),
  esT("iAutoItemLoad"),
  esT("iIsOkLoad"),
  esT("iLoadMemberRef"),
  esT("iLoadPropRef"),
  esT("iLoadThisPropRef"),
  esT("iLoadAttributeVal"),
  esT("iLoadThisAttributeVal"),
  esT("iJump"),
  esT("iJumpFalse"),
  esT("iJumpTrue"),
  esT("iThrow"),
  esT("iEnterTryCatch"),
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// get opcode string representation
EsString::const_pointer EsScriptInstruction::getOpcodeString( EsScriptInstructionOpcode opcode ) ES_NOTHROW
{
  ES_ASSERT(opcode < iOpcodesCount);
  return c_opStrings[opcode];
}
//---------------------------------------------------------------------------
