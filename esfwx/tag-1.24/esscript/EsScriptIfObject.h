#ifndef _es_script_if_object_h_
#define _es_script_if_object_h_

// conditional field object internals
class ES_INTF_IMPL_DERIVED(EsScriptIfObject, EsScriptObject)

protected:
  // friends-only services
  EsScriptIfObject(const EsScriptContext::Ptr& ctx, esU32 flags, const EsScriptObjectDataBufferPtr& buff, const EsScriptCodeSection::Ptr& expr);
  static EsScriptObjectIntf::Ptr createMetaclass(const EsScriptContext::Ptr& ctx);
  // base class overrides
  //
  ES_DECL_INTF_METHOD(void, setParent)(EsScriptObjectIntf* parent) ES_OVERRIDE;
  // create instance of script object from its metaclass instance
  ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, objectCreate)(const EsScriptObjectDataBufferPtr& buff, bool splitCtx) const ES_OVERRIDE;
  // internal layout invalidation
  ES_DECL_INTF_METHOD(void, internalInvalidateLayout)(bool propagate) ES_OVERRIDE;
  // update object memory layout. fields and ancestor offsets and|or sizes get updated
  // as a result of this process
  ES_DECL_INTF_METHOD(void, internalUpdateLayout)(ulong offs) ES_OVERRIDE;
  // for IF fields only. in instance mode empty pointer is returned
  // for inactive condition, in metaclass mode all conditions are accessible
  ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, conditionalBranchGet)(int condition) const ES_OVERRIDE;
  // access field expression, valid for fields with expression only, namely, for arrays and conditional fields
  ES_DECL_INTF_METHOD(EsScriptCodeSection::Ptr, thisFieldExprGet)() const ES_NOTHROW ES_OVERRIDE { return m_expr; }

public:
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE;

public:
  ES_DECL_REFLECTED_CLASS(EsScriptIfObject, EsScriptObject)

protected:
  // internal service
  void initializeUpdateSubscription();
  int internalExprEvaluate();

protected:
  // data members
  //
  // logical expression reference
  EsScriptCodeSection::Ptr m_expr;
  // current branch value, 0 is false, 1 is true, -1 is undefined
  int m_exprValue;
  // subscription initialization flag
  bool m_subscriptionInitialized;

  friend class EsScriptMachine;
  friend class EsScriptObject;
};

#endif // _es_script_if_object_h_
