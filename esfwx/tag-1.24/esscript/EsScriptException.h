#ifndef _es_script_exception_h_
#define _es_script_exception_h_

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
class PACKAGE EsScriptException : public EsException
#else
class ESSCRIPT_CLASS EsScriptException : public EsException
#endif
{
public:
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
__fastcall
#endif
	EsScriptException(const EsString& reason, const EsScriptDebugInfoIntf::Ptr& debugInfo, int cnt = -1, bool doLogErrors = true);

public:
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  virtual __fastcall ~EsScriptException();
#endif

	static void Throw(const EsString& reason, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr(), bool doLogErrors = true);
	static void ReThrow(const EsException& ex, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr());

	bool hasDebugInfo() const;
	EsScriptDebugInfoIntf::Ptr debugInfoGet() const;

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

#endif // _es_script_exception_h_