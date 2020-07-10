#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptException.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

namespace EsReflection
{
ES_DECL_ENUMERATION_INFO(EsScriptExceptionCode, esT("Script exception classificatoin codes"))
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// script exception class
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
__fastcall
#endif
EsScriptException::EsScriptException(ulong code, const EsString& reason, const EsScriptDebugInfoIntf::Ptr& debugInfo, int cnt /*= -1*/, bool doLogErrors /*= true*/, const EsVariant& payload /*= EsVariant::null()*/) :
EsException(
  code,
  EsException::severityGeneric,
  EsException::facilityEsScript,
  debugInfo ?
    EsString::format(
      esT("%s: '%s'"),
      debugInfo->asString(),
      reason
     ) :
    reason,
  debugInfo,
  doLogErrors,
  payload
)
{
  const EsVariant::Array& evtPayload = {
    m_msg,
    debugInfo,
    cnt
  };

  EsEventDispatcher::eventPost(
    ES_EVTC_SCRIPT,
    static_cast<ulong>(EsScript::evtError),
    evtPayload
  );
}
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
__fastcall EsScriptException::~EsScriptException()
{}
#endif
//---------------------------------------------------------------------------

void EsScriptException::Throw(ulong code, const EsString& reason, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/, bool doLogErrors /*= true*/, const EsVariant& payload /*= EsVariant::null()*/)
{
  ES_DEBUG_TRACE(esT("EsScriptException::Throw"));

  throw EsScriptException(
    code,
    reason, 
    debugInfo, 
    -1,
    doLogErrors,
    payload
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ReThrow(const EsException& ex, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/, bool doLogErrors /*= false*/)
{
  ES_DEBUG_TRACE(esT("EsScriptException::ReThrow"));

  if( EsException::facilityEsScript != ex.facilityGet() )
    EsScriptException::Throw(
      ex.codeGet(),
      ex.messageGet(),
      debugInfo,
      doLogErrors,
      ex.dataGet()
    );
  else
    throw;
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowPodObjectMayNotContainFieldsVarsOrProps(const EsString& typeName, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EObjectDecl),
    EsString::format(
      esT("Object '%s' is of POD type and may not contain fields, member variables, or properties"),
      typeName
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowFinalObjectMayNotBeModified(const EsString& typeName, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EObjectDecl),
    EsString::format(
      esT("Object '%s' is of final type and may not be modified"),
      typeName
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowFieldOrVarAlreadyDeclared(const EsString& fieldName, const EsString& typeName, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EMemberDecl),
    EsString::format(
      esT("Field or member variable '%s' is already declared in '%s' or one of its base objects"),
      fieldName,
      typeName
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowMetaclassAlreadyExists(const EsString& name, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EMetaclass),
    EsString::format(
      esT("Object or namespace '%s' is already declared"),
      name
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowMetaclassDoesNotExist(const EsString& name, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EMetaclass),
    EsString::format(
      esT("Object '%s' is not declared"),
      name
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowMemberVarIsNotDeclared(const EsString& name, const EsString& typeName, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EMemberDecl),
    EsString::format(
      esT("Member variable '%s' is not declared in '%s'"),
      name,
      typeName
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowFieldOrMemberVarIsNotDeclared(const EsString& name, const EsString& typeName, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EMemberDecl),
    EsString::format(
      esT("Neither field nor member variable '%s' is declared in '%s' or any of its base objects"),
      name,
      typeName
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowFieldIsNotDeclared(const EsString& name, const EsString& typeName, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EMemberDecl),
    EsString::format(
      esT("Field '%s' is not a member of object '%s' or any its base objects"),
      name,
      typeName
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowValueIsNotMemberOfEnum(const EsVariant& val, const EsString& enumName, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EEnumOp),
    EsString::format(
      esT("Value '%s' is not a member of '%s' enumeration"),
      val.asString(),
      enumName
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowPerformingOperationOnTemporary(const EsString& op, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EOperation),
    EsString::format(
      esT("Operation '%s' is being performed on temporary value"),
      op
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowBinBufferDoesNotMatchObject(const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EObjectOp),
    esT("Could not set object buffer, source buffer size is less than expected by the target object"),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowPerformingOperationOnReadOnly(const EsString& op, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EOperation),
    EsString::format(
      esT("Operation '%s' is being performed on read-only value"),
      op
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowUnknownNamespaceOrService(const EsString& ns, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::ENamespace),
    EsString::format(
      esT("'%s' is not known namespace or service name"),
      ns
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowPropertyAlreadyDeclared(const EsString& name, const EsString& obj, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EPropertyDecl),
    EsString::format(
      esT("Property '%s' is already declared in '%s' or one of its ancestors"),
      name,
      obj
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowPropertyIsNotDeclared(const EsString& name, const EsString& obj, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EPropertyDecl),
    EsString::format(
      esT("Property '%s' is not declared in '%s' or any of its ancestors"),
      name,
      obj
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowPropertyHasNoReadAccess(const EsString& name, const EsString& obj, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EPropertyOp),
    EsString::format(
      esT("Property '%s$%s' has no read access"),
      obj,
      name
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowPropertyHasNoWriteAccess(const EsString& name, const EsString& obj, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EPropertyOp),
    EsString::format(
      esT("Property '%s$%s' has no write access"),
      obj,
      name
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowCtorIsNotDeclared(const EsString& objName, ulong paramCount, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EObjectDecl),
    EsString::format(
      esT("Constructor, taking %d parameters, is not defined in '%s' or any of its ancestors"),
      paramCount,
      objName
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowCouldNotParseMicroExpression(const EsString& expr, const EsString& callCtx, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EParser),
    EsString::format(
      esT("Could not parse micro expression: '%s' in '%s'"),
      expr,
      callCtx
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowConstexprExpected(const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EConstexpr),
    esT("Constant expression expected"),
    debugInfo
  );
}
//---------------------------------------------------------------------------

void EsScriptException::ThrowUnhandledAstNodeId(long id, const EsScriptDebugInfoIntf::Ptr& debugInfo /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  EsScriptException::Throw(
    as_<ulong>(EsScriptExceptionCode::EParser),
    EsString::format(
      esT("Unhandled AST node ID: %d"),
      id
    ),
    debugInfo
  );
}
//---------------------------------------------------------------------------


