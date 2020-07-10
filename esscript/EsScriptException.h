#ifndef _es_script_exception_h_
#define _es_script_exception_h_

class ESSCRIPT_CLASS EsScriptException : public EsException
{
public:
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
__fastcall
#endif
  EsScriptException(ulong code, const EsString& reason, const EsScriptDebugInfoIntf::Ptr& debugInfo, int cnt = -1, bool doLogErrors = true, const EsVariant& payload = EsVariant::null());

public:
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  virtual __fastcall ~EsScriptException();
#endif

  static void Throw(ulong code, const EsString& reason, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr(), bool doLogErrors = true, const EsVariant& payload = EsVariant::null());
  static void ReThrow(const EsException& ex, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr(), bool doLogErrors = false);

  static void ThrowFinalObjectMayNotBeModified(const EsString& typeName, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowPodObjectMayNotContainFieldsVarsOrProps(const EsString& typeName, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowFieldOrVarAlreadyDeclared(const EsString& fieldName, const EsString& typeName, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowMetaclassAlreadyExists(const EsString& name, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowMetaclassDoesNotExist(const EsString& name, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowMemberVarIsNotDeclared(const EsString& name, const EsString& typeName, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowFieldOrMemberVarIsNotDeclared(const EsString& name, const EsString& typeName, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowFieldIsNotDeclared(const EsString& name, const EsString& typeName, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowValueIsNotMemberOfEnum(const EsVariant& val, const EsString& enumName, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowPerformingOperationOnTemporary(const EsString& op, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowBinBufferDoesNotMatchObject(const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowPerformingOperationOnReadOnly(const EsString& op, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowUnknownNamespaceOrService(const EsString& ns, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowPropertyAlreadyDeclared(const EsString& name, const EsString& obj, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowPropertyIsNotDeclared(const EsString& name, const EsString& obj, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowPropertyHasNoReadAccess(const EsString& name, const EsString& obj, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowPropertyHasNoWriteAccess(const EsString& name, const EsString& obj, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowCtorIsNotDeclared(const EsString& objName, ulong paramCount, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowCouldNotParseMicroExpression(const EsString& expr, const EsString& callCtx, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowConstexprExpected(const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
  static void ThrowUnhandledAstNodeId(long id, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());
};

/// Script exception code classification
///
enum class EsScriptExceptionCode
{
   EUnspecified,                        ///< Code is unspecified
   EExecution,                          ///< Script execution - related
   ESource,                             ///< Source file - related
   EBinary,                             ///< Binary and link - related
   ENamespace,                          ///< Namespace or service - related
   EObjectDecl,                         ///< Object declaration - related
   EObjectOp,                           ///< Object operation - related
   EMetaclass,                          ///< Metaclass - related
   EMemberDecl,                         ///< Member declaration - related
   EPropertyDecl,                       ///< Property declaration - related
   EPropertyOp,                         ///< Property operation - related
   EEnumDecl,                           ///< Enumeration declaration - related
   EEnumOp,                             ///< Enumeration operation - related
   EOperation,                          ///< Generic operation - related
   EParser,                             ///< Generic parser - related
   EConstexpr,                          ///< Constexpr - related
   EMethodDecl,                         ///< Method declaration - related
   EMethodParamCnt,                     ///< Method parameter count check
   EGenericDecl,                        ///< Generic declaration - related
   EGeneric,                            ///< Generic error
   EUserCodeBase                = 1000  ///< User script exception codes must have values greater or equal to this one
};

// Reflected enumeration isolation
namespace EsReflection
{
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESSCRIPT_CLASS, EsScriptExceptionCode)
  ES_DECL_ENUMERATION_ITEM_LABELLED(EUnspecified,  as_<ulong>(::EsScriptExceptionCode::EUnspecified), _i("Code is unspecified"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EExecution,    as_<ulong>(::EsScriptExceptionCode::EExecution), _i("Script execution - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(ESource,       as_<ulong>(::EsScriptExceptionCode::ESource), _i("Source file - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EBinary,       as_<ulong>(::EsScriptExceptionCode::EBinary), _i("Binary and link - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(ENamespace,    as_<ulong>(::EsScriptExceptionCode::ENamespace), _i("Namespace or service - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EObjectDecl,   as_<ulong>(::EsScriptExceptionCode::EObjectDecl), _i("Object declaration - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EObjectOp,     as_<ulong>(::EsScriptExceptionCode::EObjectOp), _i("Object operation - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EMetaclass,    as_<ulong>(::EsScriptExceptionCode::EMetaclass), _i("Metaclass - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EMemberDecl,   as_<ulong>(::EsScriptExceptionCode::EMemberDecl), _i("Member declaration - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EPropertyDecl, as_<ulong>(::EsScriptExceptionCode::EPropertyDecl), _i("Property declaration - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EPropertyOp,   as_<ulong>(::EsScriptExceptionCode::EPropertyOp), _i("Property operation - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EEnumDecl,     as_<ulong>(::EsScriptExceptionCode::EEnumDecl), _i("Enumeration declaration - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EEnumOp,       as_<ulong>(::EsScriptExceptionCode::EEnumOp), _i("Enumeration operation - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EOperation,    as_<ulong>(::EsScriptExceptionCode::EOperation), _i("Generic operation - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EParser,       as_<ulong>(::EsScriptExceptionCode::EParser), _i("Generic parser - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EConstexpr,    as_<ulong>(::EsScriptExceptionCode::EConstexpr), _i("Constexpr - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EMethodDecl,   as_<ulong>(::EsScriptExceptionCode::EMethodDecl), _i("Method declaration - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EMethodParamCnt,as_<ulong>(::EsScriptExceptionCode::EMethodParamCnt), _i("Method parameter count check"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EGenericDecl,  as_<ulong>(::EsScriptExceptionCode::EGenericDecl), _i("Generic declaration - related"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EGeneric,      as_<ulong>(::EsScriptExceptionCode::EGeneric), _i("Generic error"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(EUserCodeBase, as_<ulong>(::EsScriptExceptionCode::EUserCodeBase), _i("User script exception codes base"))
ES_DECL_ENUMERATION_END
}

#endif // _es_script_exception_h_