#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptIfObject.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// internal if_branch proxy class
class ES_INTF_IMPL_DERIVED(EsScriptIfBranchObject, EsScriptObject)

protected:
  // friends-only services
  EsScriptIfBranchObject(const EsScriptContext::Ptr& ctx, esU32 flags, const EsScriptMethodMapPtr& scopedMethods,
    const EsScriptObjectDataBufferPtr& buff, bool trueFalse) :
  EsScriptObject(trueFalse ? EsStdNames::_true() : EsStdNames::_false(), EsScriptObjectIntf::Ptr(), scopedMethods, ctx, flags, buff,
    EsAttributesIntf::Ptr()),
  m_true(trueFalse)
  {
  }

  static EsScriptObjectIntf::Ptr createMetaclass(const EsScriptContext::Ptr& ctx, bool trueFalse)
  {
    EsScriptMethodMapPtr scopedMethods( new EsScriptMethodMap );
    std::unique_ptr<EsScriptIfBranchObject> ptr(
      new EsScriptIfBranchObject(
        ctx,
        ofMetaclass|ofIfBranch,
        scopedMethods,
        nullptr,
        trueFalse
      )
    );
    ES_ASSERT(ptr);

    return ptr.release()->asBaseIntfPtr();
  }

  // base class overrides
  //
  // create instance of script object from its metaclass instance
  ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, objectCreate)(const EsScriptObjectDataBufferPtr& buff, bool splitCtx) const
  {
    std::unique_ptr<EsScriptIfBranchObject> ptr(
      new EsScriptIfBranchObject(
        m_ctx,
        (m_flags & ~ofMetaclass)|ofNeedUpdateLayout,
        m_methods,
        buff,
        m_true
      )
    );
    ES_ASSERT(ptr);

    // copy properties (actually, all instances, declared in metaclass, got shared)
    ptr->m_propsMap = m_propsMap;

    // clone member variables
    if( m_memberVars )
    {
      EsScriptSymbolTable::Ptr vars(
        new EsScriptSymbolTable(
          *m_memberVars.get(),
          nullptr
        )
      );
      ES_ASSERT(vars);

      ptr->m_memberVars = vars;
    }

    return ptr.release()->asBaseIntfPtr();
  }

  // format branch type name, take nesting into account
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    if(m_parent)
    {
      ES_ASSERT( m_parent->isIf() );
      return m_parent->typeNameGet() + esT("&") + m_typeName;
    }

    return m_typeName;
  }

  void invalidateFieldOffsets()
  {
    for(ulong idx = 0; idx < m_fieldsMap.countGet(); ++idx)
    {
      EsScriptObjectIntf::Ptr fld = fnodeFieldGet(m_fieldsMap.valueGet(idx));
      ES_ASSERT(fld);
      fld->internalOffsetSet(-1);
    }
  }

public:
  ES_DECL_REFLECTED_CLASS(EsScriptIfBranchObject, EsScriptObject)

private:
  bool m_true;
  friend class EsScriptIfObject;
};

ES_DECL_CLASS_INFO_DERIVED_BEGIN(EsScriptIfBranchObject, EsScriptObject, NO_CLASS_DESCR)
ES_DECL_CLASS_INFO_END

// ----------------------------------------------------------------------------------------------------------
// 'if' object itself
ES_DECL_CLASS_INFO_DERIVED_BEGIN(EsScriptIfObject, EsScriptObject, NO_CLASS_DESCR)
ES_DECL_CLASS_INFO_END

EsScriptIfObject::EsScriptIfObject(const EsScriptContext::Ptr& ctx, esU32 flags, const EsScriptObjectDataBufferPtr& buff, const EsScriptCodeSection::Ptr& expr) :
EsScriptObject(esT("if"), nullptr, nullptr, ctx, flags, buff, nullptr),
m_expr(expr),
m_exprValue(-1),
m_subscriptionInitialized(false)
{
}

EsScriptObjectIntf::Ptr EsScriptIfObject::createMetaclass(const EsScriptContext::Ptr& ctx)
{
  // create if metaclass wo expression code section
  std::unique_ptr<EsScriptIfObject> tmp(
    new EsScriptIfObject(
      ctx,
      ofMetaclass|ofIf,
      nullptr,
      nullptr
    )
  );
  ES_ASSERT(tmp);

  EsScriptObjectIntf::Ptr result = tmp.release()->asBaseIntfPtr();
  ES_ASSERT(result);

  // install expression code section
  EsScriptCodeSection::Ptr expr = EsScriptCodeSection::create(
    esT("condition"),
    EsString::nullArray(),
    result.get()
  );
  ES_ASSERT(expr);
  reinterpret_cast<EsScriptIfObject*>(result->implementorGet())->m_expr = expr;

  // add branches
  EsScriptObjectIntf::Ptr branch = EsScriptIfBranchObject::createMetaclass(
    ctx, 
    true
  );
  result->fieldConditionalAdd(branch);
  
  branch = EsScriptIfBranchObject::createMetaclass(
    ctx, 
    false
  );
  result->fieldConditionalAdd(branch);

  return result;
}

// create instance of script object from its metaclass instance. that's what gets performed on calling NEW ScriptObject in script
ES_IMPL_INTF_METHOD(EsScriptObjectIntf::Ptr, EsScriptIfObject::objectCreate)(const EsScriptObjectDataBufferPtr& buff, bool splitCtx) const
{
  std::unique_ptr<EsScriptIfObject> result(
    new EsScriptIfObject(
      m_ctx,
      (m_flags & ~ofMetaclass) | ofNeedUpdateLayout,
      buff,
      m_expr
    )
  );
  ES_ASSERT(result);

  ESSCRIPT_OBJECT_TRACE2(
    esT("New instance of '%s' object type created"),
    m_typeName
  )
  return result.release()->asBaseIntfPtr();
}

int EsScriptIfObject::internalExprEvaluate()
{
  if( isMetaclass() )
    return -1;

  EsScriptObjectIntf* exprThis = topNonProxyGet();
  // execute in expression mode
  EsScriptValAccessorIntf::Ptr result = m_ctx->vm()->exec(
    m_expr,
    EsVariant::null(),
    EsScriptEvalMode::evalExpr,
    exprThis
  );
  ES_ASSERT(result);

  return result->get().asBool() ? 1 : 0;
}

ES_IMPL_INTF_METHOD(void, EsScriptIfObject::setParent)(EsScriptObjectIntf* parent)
{
  EsScriptObject::setParent(parent);

  // in instance initialize update subscription, if any, but
  // only after conditional's internalClone call is complete
  if( 
    parent &&
    !isMetaclass() &&
    isFinal() 
  )
    initializeUpdateSubscription();
}

ES_IMPL_INTF_METHOD(void, EsScriptIfObject::initializeUpdateSubscription)()
{
  if( m_subscriptionInitialized )
    return;

  m_subscriptionInitialized = true;

  // set-up subscription to the update notifications
  ES_ASSERT( !isMetaclass() );
  ESSCRIPT_OBJECT_TRACE2(
    esT("initializeUpdateSubscription called for '%s'"),
    typeNameGet()
  )
  const EsString::Array& fldDependencies = m_expr->thisFieldDependenciesGet();
  for( size_t idx = 0; idx < fldDependencies.size(); ++idx )
  {
    const EsString& fldName = fldDependencies[idx];
    EsScriptObjectFieldFastFinder finder(this, fldName);
    EsScriptObjectIntf::Ptr fld = fnodeFieldGet( finder.resultGet() );
    ES_ASSERT(fld);
    fld->updateSubscriberAdd(this);
  }

  const EsString::Array& varDependencies = m_expr->thisMemberVarDependenciesGet();
  for( size_t idx = 0; idx < varDependencies.size(); ++idx )
  {
    EsScriptObjectVarFastFinder finder(this, varDependencies[idx]);
    EsScriptValAccessorIntf::Ptr var = finder.resultGet();
     ES_ASSERT(var);
    var->updateSubscriberAdd(this);
  }
}

// for IF fields only. in instance mode empty pointer is returned
// for inactive condition, in metaclass mode all conditions are accessible
ES_IMPL_INTF_METHOD(EsScriptObjectIntf::Ptr, EsScriptIfObject::conditionalBranchGet)(int condition) const
{
  EsScriptObjectIntf::Ptr fldTrue = m_fieldsMap.valueGet(0).asExistingObject();
  EsScriptObjectIntf::Ptr fldFalse = m_fieldsMap.valueGet(1).asExistingObject();

  if( !isMetaclass() )
  {
    if( -1 == condition )
      condition = m_exprValue;

    if( 1 == condition && (
        fldTrue->thisHasFields() ||
        fldTrue->thisScriptedMethodsGet()
      )
    )
      // select true branch (fldTrue), if not empty
      return fldTrue;
    else if( 0 == condition && (
        fldFalse->thisHasFields() ||
        fldFalse->thisScriptedMethodsGet()
      )
    )
      // select false branch (fldFalse), if not empty
      return fldFalse;
  }
  else
  {
    if( 1 == condition )
      // select true branch (fldTrue)
      return fldTrue;
    else if( 0 == condition )
      // select false branch (fldFalse)
      return fldFalse;
  }

  return nullptr;
}

// internal layout invalidation
ES_IMPL_INTF_METHOD(void, EsScriptIfObject::internalInvalidateLayout)(bool propagate)
{
  ES_ASSERT(2 == m_fieldsMap.countGet());

  // Invalidate both branches, if not invalidated already
  EsScriptObjectIntf::Ptr fldTrue = m_fieldsMap.valueGet(0).asExistingObject();
  EsScriptObjectIntf::Ptr fldFalse = m_fieldsMap.valueGet(1).asExistingObject();

  if( 
    fldTrue &&
    !fldTrue->isInvalid()
  )
    fldTrue->internalInvalidateLayout(false);

  if( 
    fldFalse &&
    !fldFalse->isInvalid()
  )
    fldFalse->internalInvalidateLayout(false);

  EsScriptObject::internalInvalidateLayout(propagate);
}

ES_IMPL_INTF_METHOD(void, EsScriptIfObject::internalUpdateLayout)(ulong offs)
{
  ES_ASSERT(!isMetaclass());

  ESSCRIPT_OBJECT_TRACE4(
    esT("internalUpdateLayout called for '%s' with offs=%d, ofNeedUpdateLayout is %s"),
    typeNameGet(),
    offs,
    (m_flags & ofNeedUpdateLayout) ?
      esVT("set") :
      esVT("not set")
  )

  int prevExprValue = m_exprValue;
  if( m_flags & ofNeedUpdateLayout )
  {
    m_exprValue = internalExprEvaluate();
    ES_ASSERT(-1 < m_exprValue);

    m_flags &= ~ofNeedUpdateLayout;
  }

  EsScriptObjectIntf::Ptr branch = conditionalBranchGet(m_exprValue);
  if(branch)
  {
    // invalidate field offsets for branch that's being made active,
    // or if our own offset is changing
    if( prevExprValue != m_exprValue ||
        static_cast<ulong>(m_offs) != offs )
    {
      EsScriptIfBranchObject* obj = reinterpret_cast<EsScriptIfBranchObject*>(
        branch->implementorGet()
      );
      ES_ASSERT(obj);
      obj->invalidateFieldOffsets();
    }

    branch->internalUpdateLayout(offs);
    m_size = branch->sizeGet();
  }
  else
    m_size = 0;

  internalOffsetSet(offs);
}

// format branch type name, take nesting into account
ES_IMPL_INTF_METHOD(EsString, EsScriptIfObject::typeNameGet)() const ES_NOTHROW
{
  EsString upper;
  if( m_parent )
    upper = m_parent->typeNameGet();

  if( !upper.empty() )
    return upper + esT("&") + m_typeName;

  return m_typeName;
}
