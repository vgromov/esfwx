#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptObject.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

static const EsString& fixedSizeAttr() ES_NOTHROW
{
  static const EsString sc_fixedSizeAttr( esT("fixedSize") );
  return sc_fixedSizeAttr;
}
//---------------------------------------------------------------------------

// field finder
class EsScriptObjectFieldFinder : public EsScriptObjectDownTopHierarchyTraverser
{
public:
  EsScriptObjectFieldFinder(const EsScriptObjectIntf* obj, const EsString& name) ES_NOTHROW :
  EsScriptObjectDownTopHierarchyTraverser(obj, false, true),
  m_name(name)
  {}

  virtual bool objectProcess(const EsScriptObjectIntf* obj) ES_NOTHROW
  {
    ES_ASSERT(obj);
    const EsStringIndexedMap& sm = obj->thisFieldsMapGet();
    ulong idx = sm.itemFind(m_name);
    if(EsStringIndexedMap::npos != idx)
    {
      m_result = sm.valueGet(idx);
      return false; // break traversal process
    }

    return true;
  }

  const EsVariant& resultGet() const ES_NOTHROW { return m_result; }

protected:
  const EsString& m_name;
  EsVariant m_result;

private:
  EsScriptObjectFieldFinder() ES_REMOVEDECL;
  EsScriptObjectFieldFinder(const EsScriptObjectFieldFinder&) ES_REMOVEDECL;
  EsScriptObjectFieldFinder& operator=(const EsScriptObjectFieldFinder&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

// object variable finder
class EsScriptObjectVariableFinder : public EsScriptObjectDownTopHierarchyTraverser
{
public:
  EsScriptObjectVariableFinder(const EsScriptObjectIntf* obj, const EsString& name) ES_NOTHROW :
  EsScriptObjectDownTopHierarchyTraverser(obj, false, true),
  m_name(name),
  m_found(false)
  {}

  virtual bool objectProcess(const EsScriptObjectIntf* obj) ES_NOTHROW
  {
    ES_ASSERT(obj);
    EsScriptSymbolTable::Ptr vars = obj->thisVariablesGet();
    if(vars)
    {
      EsScriptValAccessorIntf::Ptr var = vars->symbolFind(m_name, m_found);
      if( m_found )
      {
        m_result = var;
        return false; // break traversal process
      }
    }

    return true;
  }

  bool found() const ES_NOTHROW { return m_found; }
  EsScriptValAccessorIntf::Ptr resultGet() const ES_NOTHROW { return m_result; }

protected:
  const EsString& m_name;
  bool m_found;
  EsScriptValAccessorIntf::Ptr m_result;

private:
  EsScriptObjectVariableFinder() ES_REMOVEDECL;
  EsScriptObjectVariableFinder(const EsScriptObjectVariableFinder&) ES_REMOVEDECL;
  EsScriptObjectVariableFinder& operator=(const EsScriptObjectVariableFinder&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

// field names collector
class EsScriptObjectFieldNamesCollector : public EsScriptObjectTopDownHierarchyTraverser
{
public:
  EsScriptObjectFieldNamesCollector(const EsScriptObjectIntf* obj) ES_NOTHROW :
  EsScriptObjectTopDownHierarchyTraverser(obj, true, true)
  {
    m_names.reserve(32);
  }

  virtual bool fieldProcess(const EsScriptObjectIntf* fld, const EsString& name) ES_NOTHROW
  {
    m_names.push_back(name);
    return true;
  }

  const EsString::Array& resultGet() const ES_NOTHROW { return m_names; }

protected:
  EsString::Array m_names;

private:
  EsScriptObjectFieldNamesCollector() ES_REMOVEDECL;
  EsScriptObjectFieldNamesCollector(const EsScriptObjectFieldNamesCollector&) ES_REMOVEDECL;
  EsScriptObjectFieldNamesCollector& operator=(const EsScriptObjectFieldNamesCollector&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

// var names collector
class EsScriptObjectVarNamesCollector : public EsScriptObjectTopDownHierarchyTraverser
{
public:
  EsScriptObjectVarNamesCollector(const EsScriptObjectIntf* obj) ES_NOTHROW :
  EsScriptObjectTopDownHierarchyTraverser(obj, false, true)
  {
    m_names.reserve(32);
  }

  virtual bool objectProcess(const EsScriptObjectIntf* obj) ES_NOTHROW
  {
    ES_ASSERT(obj);
    const EsStringArray& names = obj->thisVariablesNamesGet();
    m_names.insert(m_names.end(), names.begin(), names.end());

    return true;
  }

  const EsString::Array& resultGet() const ES_NOTHROW { return m_names; }

protected:
  EsString::Array m_names;

private:
  EsScriptObjectVarNamesCollector() ES_REMOVEDECL;
  EsScriptObjectVarNamesCollector(const EsScriptObjectVarNamesCollector&) ES_REMOVEDECL;
  EsScriptObjectVarNamesCollector& operator=(const EsScriptObjectVarNamesCollector&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

// property names collector
class EsScriptObjectPropNamesCollector : public EsScriptObjectTopDownHierarchyTraverser
{
public:
  EsScriptObjectPropNamesCollector(const EsScriptObjectIntf* obj, bool persistentOnly) ES_NOTHROW :
  EsScriptObjectTopDownHierarchyTraverser(obj, false, true),
  m_persistentOnly(persistentOnly)
  {
    m_names.reserve(32);
  }

  virtual bool objectProcess(const EsScriptObjectIntf* obj) ES_NOTHROW
  {
    ES_ASSERT(obj);
    const EsStringArray& names = obj->thisScriptedPropertiesNamesGet(m_persistentOnly);
    m_names.insert(m_names.end(), names.begin(), names.end());

    return true;
  }

  const EsString::Array& resultGet() const ES_NOTHROW { return m_names; }

protected:
  EsString::Array m_names;
  bool m_persistentOnly;

private:
  EsScriptObjectPropNamesCollector() ES_REMOVEDECL;
  EsScriptObjectPropNamesCollector(const EsScriptObjectPropNamesCollector&) ES_REMOVEDECL;
  EsScriptObjectPropNamesCollector& operator=(const EsScriptObjectPropNamesCollector&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

// scripted object keys collector
class EsScriptObjectMethodKeysCollector : public EsScriptObjectTopDownHierarchyTraverser
{
public:
  EsScriptObjectMethodKeysCollector(const EsScriptObjectIntf* obj, bool allHierarchy) ES_NOTHROW :
  EsScriptObjectTopDownHierarchyTraverser(obj, false, allHierarchy)
  {
    m_keys.reserve(32);
  }

  virtual bool objectProcess(const EsScriptObjectIntf* obj) ES_NOTHROW ES_OVERRIDE
  {
    ES_ASSERT(obj);

    EsScriptMethodMapPtr methods = obj->thisScriptedMethodsGet();
    if( methods )
      for(EsScriptMethodMap::const_iterator cit = methods->begin(); cit != methods->end(); ++cit)
        m_keys.push_back((*cit).first);

    return true;
  }

  const EsMethodInfoKeysT& resultGet() ES_NOTHROW
  {
    // sort and filter only unique records
    std::sort(m_keys.begin(), m_keys.end());
    EsMethodInfoKeysT::iterator newEnd = std::unique(m_keys.begin(), m_keys.end());
    m_keys.erase(newEnd, m_keys.end());

    return m_keys;
  }

protected:
  EsMethodInfoKeysT m_keys;

private:
  EsScriptObjectMethodKeysCollector() ES_REMOVEDECL;
  EsScriptObjectMethodKeysCollector(const EsScriptObjectMethodKeysCollector&) ES_REMOVEDECL;
  EsScriptObjectMethodKeysCollector& operator=(const EsScriptObjectMethodKeysCollector&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

// object method finder
class EsScriptObjectMethodFinder : public EsScriptObjectDownTopHierarchyTraverser
{
public:
  EsScriptObjectMethodFinder(const EsScriptObjectIntf* obj, const EsMethodInfoKeyT& key, bool allHierarchy) ES_NOTHROW :
  EsScriptObjectDownTopHierarchyTraverser(obj, false, allHierarchy),
  m_key(key)
  {}

  virtual bool objectProcess(const EsScriptObjectIntf* obj) ES_NOTHROW ES_OVERRIDE
  {
    ES_ASSERT(obj);
    EsScriptMethodMapPtr methods = obj->thisScriptedMethodsGet();
    if(methods)
    {
      EsScriptMethodMap::const_iterator cit = methods->find(m_key);
      if( cit != methods->end() )
      {
        m_result = (*cit).second;
        return false; // break traversal process
      }
    }

    return true;
  }

  EsScriptCodeSection::Ptr resultGet() const ES_NOTHROW { return m_result; }

protected:
  EsMethodInfoKeyT m_key;
  EsScriptCodeSection::Ptr m_result;

private:
  EsScriptObjectMethodFinder() ES_REMOVEDECL;
  EsScriptObjectMethodFinder(const EsScriptObjectMethodFinder&) ES_REMOVEDECL;
  EsScriptObjectMethodFinder& operator=(const EsScriptObjectMethodFinder&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

// script object property finder
class EsScriptObjectPropertyFinder : public EsScriptObjectDownTopHierarchyTraverser
{
public:
  EsScriptObjectPropertyFinder(const EsScriptObjectIntf* obj, const EsString& name) ES_NOTHROW :
  EsScriptObjectDownTopHierarchyTraverser(obj, false, true),
  m_name(name)
  {}

  virtual bool objectProcess(const EsScriptObjectIntf* obj) ES_NOTHROW ES_OVERRIDE
  {
    ES_ASSERT(obj);
    EsStringIndexedMap::Ptr props = obj->thisPropertiesMapGet();
    if(props)
    {
      ulong idx = props->itemFind(m_name);
      if( EsStringIndexedMap::npos != idx )
      {
        m_result = props->valueGet(idx).asExistingObject();
        return false; // break traversal process
      }
    }

    return true;
  }

  EsScriptObjectPropertyInfoIntf::Ptr resultGet() const ES_NOTHROW { return m_result; }

protected:
  const EsString& m_name;
  EsScriptObjectPropertyInfoIntf::Ptr m_result;

private:
  EsScriptObjectPropertyFinder() ES_REMOVEDECL;
  EsScriptObjectPropertyFinder(const EsScriptObjectPropertyFinder&) ES_REMOVEDECL;
  EsScriptObjectPropertyFinder& operator=(const EsScriptObjectPropertyFinder&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#define ES_DECL_BASE_SCRIPT_CLASS_INFO_BEGIN( ClassName, Descr ) \
  ES_IMPL_REFLECTED_PROPERTIES(ClassName) \
  ES_DECL_CLASS_INFO_BEGIN( ClassName, Descr ) \
  ES_DECL_DEFAULT_REFLECTED_METHOD_INFOS(ClassName)

// -------------------------------------------------------------------------------------------------------
// reflection class info declaration
ES_DECL_BASE_SCRIPT_CLASS_INFO_BEGIN(EsScriptObject, NO_CLASS_DESCR)
  // reflected property infos declarations
  //
  ES_DECL_PROP_INFO_RO(EsScriptObject, size, ulong, _i("Script object size in bytes"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsScriptObject, offset, ulong, _i("Script object global offset in bytes"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsScriptObject, classAttributeNames, EsStringArray, _i("Script object class attribute names"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsScriptObject, fieldNames, EsStringArray, _i("Script object field names"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(   EsScriptObject, buffer, EsBinBuffer, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsScriptObject, __scriptHost, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  // reflected interface methods
  //
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, isMetaclass, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, isRoot, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, isFinal, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, isPOD, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, isArray, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, isIf, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, isIfBranch, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, isConditional, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, hasClassAttribute, bool_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, classAttributeGet, EsVariant_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, isInvalid, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, isAllInvalid, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, hasField, bool_CallConst_cr_EsString, esT("Check field existence"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, firstFieldNameGet, EsString_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, nextFieldNameGet, EsString_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, fieldGet, EsBaseIntfPtr_CallConst_cr_EsString, esT("Return script object field by name"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, fieldAttrsGet, EsBaseIntfPtr_CallConst_cr_EsString, esT("Return script object field's attributes by field name"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, hasVariable, bool_CallConst_cr_EsString, esT("Check member variable existence"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, varNamesGet, EsStringArray_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, variableGet, cr_EsVariant_CallConst_cr_EsString, esT("Get member variable value by its name"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptObject, EsScriptObjectIntf, variableSet, void_Call_cr_EsString_cr_EsVariant, esT("Set member variable value by its name"))

  ES_DECL_REFLECTED_METHOD_INFO(     EsScriptObject, reflectedClone, clone, EsBaseIntfPtr_CallConst, esT("Create clone of root object"))
  ES_DECL_REFLECTED_METHOD_INFO_STD( EsScriptObject, useContextOf, void_Call_cr_EsBaseIntfPtr, esT("Use execution context of specified object"))

  // reflected methods
  //
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

// constructor for internal usage only
EsScriptObject::EsScriptObject(
  const EsString& typeName,
  const EsScriptObjectIntf::Ptr& ancestor,
  const EsScriptMethodMapPtr& methods,
  const EsScriptContext::Ptr& ctx,
  esU32 flags,
  const EsScriptObjectDataBufferPtr& buff,
  const EsAttributesIntf::Ptr& attrsClass
) :
m_ctx(ctx),
m_methods(methods),
m_ancestor(ancestor),
m_parent(nullptr),
m_typeName(typeName),
m_flags(flags),
m_data(buff),
m_attrsClass(attrsClass),
m_fieldsMap(EsStringIndexedMap::ContainerWithoutInterlock),
m_size(0),
m_offs(-1)
{
  m_dynamic = true;
#if defined(ES_DEBUG) && defined(ESSCRIPT_OBJECT_USE_TRACE)
  if( m_ancestor )
  {
    ESSCRIPT_OBJECT_TRACE3(
      esT("Object '%s extends %s' constructor called"),
      m_typeName,
      m_ancestor->typeNameGet()
    )
  }
  else
  {
    ESSCRIPT_OBJECT_TRACE2(
      esT("Object '%s' constructor called"),
      m_typeName
    )
  }
#endif
}
//---------------------------------------------------------------------------

void EsScriptObject::destroy() ES_NOTHROW
{
  ES_ASSERT( m_dynamic );
  ES_ASSERT( !m_destroying );

  m_destroying = true;

  ESSCRIPT_OBJECT_TRACE3(
    esT("'%s' destroy called in '%s' mode"),
    m_typeName,
    isMetaclass() ?
      esVT("metaclass") :
      esVT("instance")
  )

  if( m_methods && !isPOD() && !isConditional() && !isArray() )
  {
    // call _this_ script dtor, if defined, and not in metaclass mode
    EsScriptObjectMethodFinder finder(
      this,
      EsMethodInfoKeyT(
        0,
        EsStdNames::reflectedDtr()
      ),
      false
    );
    finder.traverse();

    EsScriptCodeSection::Ptr dtor = finder.resultGet();
    if( !isMetaclass() && dtor && !m_ctx->vm()->isDestroying() )
    {
      m_ctx->vm()->exec(
        dtor,
        EsVariant::null(),
        EsScriptEvalMode::evalFunc,
        this
      );

      ESSCRIPT_OBJECT_TRACE2(
        esT("Object '%s' scripted destructor called"),
        m_typeName
      )
    }

    m_methods.reset();
  }

  delete this;
}
//---------------------------------------------------------------------------

EsScriptObject::~EsScriptObject()
{
  ESSCRIPT_OBJECT_TRACE3(
    esT("'%s' destructor called in '%s' mode"),
    m_typeName,
    isMetaclass() ?
      esVT("metaclass") :
      esVT("instance")
  )

  // unparent and cleanup own fields
  if( !m_fieldsMap.isEmpty() )
  {
    ulong idx = m_fieldsMap.countGet()-1;
    do
    {
      // todo: free data buffer chunk??
      const EsScriptObjectIntf::Ptr& fld =
        fnodeFieldGet(m_fieldsMap.valueGet(idx));
      ES_ASSERT(fld);

      fld->setParent(nullptr);

    } while(idx--);

    m_fieldsMap.clear();
  }

  // unparent ancestor
  if( m_ancestor )
    m_ancestor->setParent(nullptr);

  // final cleanup
  m_parent = nullptr;
  m_flags = 0;
  m_offs = -1;

  m_ctx.reset();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsMetaclassIntf::Ptr, EsScriptObject::metaGet)() const ES_NOTHROW
{
  return m_ctx->vm()->metaGet();
}
//---------------------------------------------------------------------------

// create instance of script object from its metaclass instance.
ES_IMPL_INTF_METHOD(EsScriptObjectIntf::Ptr, EsScriptObject::objectCreate)(
  const EsScriptObjectDataBufferPtr& buff,
  bool splitCtx
) const
{
  ESSCRIPT_OBJECT_TRACE2(
    esT("%s::objectCreate {"),
    typeNameGet()
  )

  EsScriptContext::Ptr ctx = m_ctx;
  if(splitCtx)
    ctx = EsScriptContext::create(m_ctx->vm());

  EsScriptObject* obj = nullptr;

  std::unique_ptr<EsScriptObject> result(
    obj = new EsScriptObject(
      m_typeName,
      m_ancestor ?
      m_ancestor->internalClone(
          nullptr,
          buff
        ) :
        EsScriptObjectIntf::Ptr(),
      m_methods,
      ctx,
      (m_flags & ~ofMetaclass) | ofNeedUpdateLayout,
      buff,
      m_attrsClass
    )
  );
  ES_ASSERT(result);

  // copy properties (actually, all instances, declared in metaclass, got shared)
  obj->m_propsMap = m_propsMap;

  // clone member variables
  if(m_memberVars)
  {
    EsScriptSymbolTable::Ptr varsCloned(
      new EsScriptSymbolTable(
        *m_memberVars.get(),
        obj->asIntfT<EsScriptObjectIntf>(false) //< Cast obj to EsScriptObjectIntf, do not incref
      )
    );
    ES_ASSERT(varsCloned);

    obj->m_memberVars = varsCloned;
  }

  // at this moment, all hierarhy of ancestors, including its fields, already created
  if(obj->ancestorGet() )
    obj->ancestorGet()->setParent(
      obj->asIntfT<EsScriptObjectIntf>(false) //< Cast obj to EsScriptObjectIntf, do not incref
    );

  ESSCRIPT_OBJECT_TRACE2(
    esT("New instance of '%s' object type created"),
    m_typeName
  )
  ESSCRIPT_OBJECT_TRACE2(
    esT("%s::objectCreate }"),
    typeNameGet()
  )

  return result.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::classCall)(const EsString& name) const
{
  return classInfoGet().classCall(name);
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::classCall)(const EsString& name, const EsVariant& param1) const
{
  return classInfoGet().classCall(name, param1);
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2) const
{
  return classInfoGet().classCall(name, param1, param2);
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) const
{
  return classInfoGet().classCall(name, param1, param2, param3);
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) const
{
  return classInfoGet().classCall(name, param1, param2, param3, param4);
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) const
{
  return classInfoGet().classCall(name, param1, param2, param3, param4, param5);
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6) const
{
  return classInfoGet().classCall(name, param1, param2, param3, param4, param5, param6);
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsAttributesIntf::Ptr, EsScriptObject::classAttrsAccess)() const ES_NOTHROW
{
  return m_attrsClass;
}
//---------------------------------------------------------------------------

// assign instance attributes to the instance-type object
ES_IMPL_INTF_METHOD(void, EsScriptObject::instAttrsAssign)(const EsAttributesIntf::Ptr& attrs) ES_NOTHROW
{
  ES_ASSERT(!isMetaclass());
  ES_ASSERT(!isConditional());
  m_attrsInst = attrs;
}
//---------------------------------------------------------------------------

// internal service meant for script compile time only
// assign attributes to this metaclass object. use attributes owner name as field name.
ES_IMPL_INTF_METHOD(void, EsScriptObject::thisFieldAttributesAssign)(const EsAttributesIntf::Ptr& attrs)
{
  ES_ASSERT(attrs);
  const EsString& fname = attrs->ownerNameGet();
  ulong idx = m_fieldsMap.itemFind(fname);
  if( EsStringIndexedMap::npos != idx )
  {
    EsVariant fnode = m_fieldsMap.valueGet(idx);
    if( EsVariant::VAR_VARIANT_COLLECTION == fnode.typeGet() )
      fnode.itemSet(1, attrs);
    else
    {
      EsVariant tmp(EsVariant::VAR_VARIANT_COLLECTION);
      tmp.addToVariantCollection(fnode)
        .addToVariantCollection(attrs);
      fnode = tmp;
    }

    m_fieldsMap.valueSet(idx, fnode);
  }
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsScriptObject::hasAttribute)(cr_EsString p1) const ES_NOTHROW
{
  if( m_attrsInst )
    return m_attrsInst->attributeExists(p1);
  else if( isMetaclass() && m_attrsClass )
    return m_attrsClass->attributeExists(p1);

  return false;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsScriptObject::hasClassAttribute)(cr_EsString p1) const ES_NOTHROW
{
  if( m_attrsClass )
    return m_attrsClass->attributeExists(p1);

  return false;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsString::Array, EsScriptObject::attributeNamesGet)() const ES_NOTHROW
{
  if( m_attrsInst )
    return m_attrsInst->allNamesGet();
  else if( isMetaclass() && m_attrsClass )
    return m_attrsClass->allNamesGet();

  return EsString::nullArray();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsString::Array, EsScriptObject::classAttributeNamesGet)() const ES_NOTHROW
{
  if( m_attrsClass )
    return m_attrsClass->allNamesGet();

  return EsString::nullArray();
}
//---------------------------------------------------------------------------

void EsScriptObject::throwNoAttributeTable() const
{
  EsScriptException::Throw(
    EsString::format(
      esT("Object '%s' does not have attribute table"),
      typeNameGet()
    ),
    m_ctx->vm()->currentDebugInfoGet()
  );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::attributeGet)(cr_EsString p1) const
{
  if( m_attrsInst )
    return m_attrsInst->attributeGet(p1);
  else if( isMetaclass() && m_attrsClass )
    return m_attrsClass->attributeGet(p1);

  throwNoAttributeTable();
  return EsVariant::null();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::classAttributeGet)(cr_EsString p1) const
{
  if( m_attrsClass )
    return m_attrsClass->attributeGet(p1);

  throwNoAttributeTable();
  return EsVariant::null();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsScriptObject::attributeAdd)(const EsString& name, const EsVariant& val)
{
  if( m_attrsInst )
    m_attrsInst->attributeAdd(name, val);
  else if( isMetaclass() && m_attrsClass )
    m_attrsClass->attributeAdd(name, val);

  throwNoAttributeTable();
}
//---------------------------------------------------------------------------

// add attribute to the object metaclass or instance
ES_IMPL_INTF_METHOD(void, EsScriptObject::attributeAddInstOrMetaclass)(const EsString& name, const EsVariant& val, bool metaclass)
{
  ES_ASSERT( !name.empty() );

  // check for attributes with such name
  if( metaclass && m_attrsClass )
  {
    if( m_attrsClass->attributeExists(name) )
      m_attrsClass->attributeSet(name, val);
    else
      m_attrsClass->attributeSet(name, val, true);
  }
  else if( !metaclass && m_attrsInst )
  {
    if( m_attrsInst->attributeExists(name) )
      m_attrsInst->attributeSet(name, val);
    else
      m_attrsInst->attributeSet(name, val, true);
  }
  else
    throwNoAttributeTable();
}
//---------------------------------------------------------------------------

// field node crackers
EsScriptObjectIntf::Ptr EsScriptObject::fnodeFieldGet(const EsVariant& fnode)
{
  if( fnode.isObject() )
    return fnode.asExistingObject();
  else if( EsVariant::VAR_VARIANT_COLLECTION == fnode.typeGet() )
  {
    ES_ASSERT( 2 == fnode.countGet() );
    return fnode[0].asExistingObject();
  }

  return EsScriptObjectIntf::Ptr();
}
//---------------------------------------------------------------------------

EsAttributesIntf::Ptr EsScriptObject::fnodeAttrsGet(const EsVariant& fnode)
{
  if( EsVariant::VAR_VARIANT_COLLECTION == fnode.typeGet() )
  {
    ES_ASSERT( 2 == fnode.countGet() );
    return fnode.itemGet(1).asExistingObject();
  }

  return EsAttributesIntf::Ptr();
}
//---------------------------------------------------------------------------

// add field object to the script object
ES_IMPL_INTF_METHOD(void, EsScriptObject::fieldAdd)(
  const EsString& name,
  const EsScriptObjectIntf::Ptr& field,
  const EsAttributesIntf::Ptr& attrs,
  const EsScriptDebugInfoIntf::Ptr& dbg
)
{
  ESSCRIPT_OBJECT_TRACE2(
    esT("%s::fieldAdd {"),
    typeNameGet()
  )

  ES_ASSERT(!name.empty());
  ES_ASSERT(field);
  const EsString& typeName = typeNameGet();

  if( isFinal() )
    EsScriptException::ThrowFinalObjectMayNotBeModified(typeName, dbg);

  if(  isPOD() || isArray() )
    EsScriptException::ThrowPodObjectMayNotContainFieldsVarsOrProps(typeName, dbg);

  if( field->isConditional() )
    EsScriptException::Throw(esT("Conditional fields must be added using specific 'fieldConditionalAdd' interface"), dbg);

  // check if fields with such name are already declared
  EsScriptObjectFieldFastFinder finder(this, name);
  if( finder.found() )
    EsScriptException::ThrowFieldOrVarAlreadyDeclared(name, typeNameGet(), dbg);

  EsVariant::Array newFnode;
  newFnode.resize( 2 );

  // Do not use interlocking within same thread for performance tune-up
  newFnode[0].doAssignOtherIntfPtrToEmptyWithoutInterlock( field );
  if( attrs )
  {
    // Do not use interlocking within same thread for performance tune-up
    newFnode[1].doAssignOtherIntfPtrToEmptyWithoutInterlock( attrs );

    m_fieldsMap.itemAdd(
      name,
      newFnode
    );

    // for real-life fields assign instance attributes as well
    if( !isMetaclass() && !field->isMetaclass() )
      field->instAttrsAssign(attrs);
  }
  else
    m_fieldsMap.itemAdd(
      name,
      newFnode[0]
    );

  field->setParent(
    asIntfT<EsScriptObjectIntf>(false) //< Cast this to EsScriptObjectIntf, do not incref
  );

  ESSCRIPT_OBJECT_TRACE4(
    esT("Field '%s' added to script object '%s' instance in '%s' mode"),
    name,
    typeNameGet(),
    isMetaclass() ?
      esVT("metaclass") :
      esVT("instance")
  )

  ESSCRIPT_OBJECT_TRACE2(
    esT("%s::fieldAdd }"),
    typeNameGet()
  )
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsScriptObject::seal)() ES_NOTHROW
{
  ESSCRIPT_OBJECT_TRACE2(
    esT("Object %s is sealed"),
    typeNameGet()
  )
  m_flags |= ofFinal;
}
//---------------------------------------------------------------------------

// add conditional field object to the script object. conditionals are special unnamed fields to augment dynamic object layout
ES_IMPL_INTF_METHOD(void, EsScriptObject::fieldConditionalAdd)(const EsScriptObjectIntf::Ptr& field,
                                                    const EsScriptDebugInfoIntf::Ptr& dbg)
{
  ESSCRIPT_OBJECT_TRACE2(
    esT("%s::fieldConditionalAdd {"),
    typeNameGet()
  )

  ES_ASSERT(field);
  ES_ASSERT(field->isConditional());

  if( isFinal() )
    EsScriptException::ThrowFinalObjectMayNotBeModified(typeNameGet(), dbg);

  if(  isPOD() )
    EsScriptException::ThrowPodObjectMayNotContainFieldsVarsOrProps(typeNameGet(), dbg);

  // add unnamed item to the map (it will not be accessible via named API)
  m_fieldsMap.itemAdd(EsString::null(), field);
  // add conditional field to the special separate array for fast access
  m_conditionals.push_back(field);

  field->setParent(
    asIntfT<EsScriptObjectIntf>(false) //< Cast to EsScriptObjectIntf, do not incref 
  );

  ESSCRIPT_OBJECT_TRACE4(
    esT("Conditional field '%s' added to script object '%s' instance in '%s' mode"),
    field->typeNameGet(),
    typeNameGet(),
    (m_flags & ofMetaclass) ?
      esVT("metaclass") :
      esVT("instance")
  )
  ESSCRIPT_OBJECT_TRACE2(
    esT("%s::fieldConditionalAdd }"),
    typeNameGet()
  )
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsScriptObjectIntf::Ptr, EsScriptObject::internalClone)(
  EsScriptObjectIntf* parent,
  const EsScriptObjectDataBufferPtr& buff,
  bool splitCtx
) const
{
  ESSCRIPT_OBJECT_TRACE2(
    esT("%s::internalClone {"),
    typeNameGet()
  )
  ESSCRIPT_OBJECT_TRACE3(
    esT("internalClone called for '%s', split exec. context '%s'"),
    typeNameGet(),
    splitCtx ?
      esVT("yes") :
      esVT("no")
  )

  // 1) create new instance of ourselves && all ancestors hierarchy
  EsScriptObjectIntf::Ptr result = objectCreate(
    buff,
    splitCtx
  );
  ES_ASSERT(result);

  result->setParent(parent);
  // 2) create all nested fields, setting new copy of ourselves as its parent
  for(ulong idx = 0; idx < m_fieldsMap.countGet(); ++idx )
  {
    const EsVariant& fnode = m_fieldsMap.valueGet(idx);
    const EsScriptObjectIntf::Ptr& fld = fnodeFieldGet(fnode);
    const EsAttributesIntf::Ptr& attrs = fnodeAttrsGet(fnode);

    if( fld->isConditional() )
      result->fieldConditionalAdd(
        fld->internalClone(
          result.get(),
          buff
        )
      );
    else
      result->fieldAdd(
        m_fieldsMap.nameGet(idx),
        fld->internalClone(
          result.get(),
          buff
        ),
        attrs
      );
  }
  // 3) seal object instance after internal cloning
  result->seal();

  ESSCRIPT_OBJECT_TRACE2(
    esT("%s::internalClone }"),
    typeNameGet()
  )

  return result;
}
//---------------------------------------------------------------------------

// access field expression, valid for fields with expression only, namely, for arrays and conditional fields
ES_IMPL_INTF_METHOD(EsScriptCodeSection::Ptr, EsScriptObject::thisFieldExprGet)() const ES_NOTHROW
{
  // should not be here!
  ES_FAIL;
  return EsScriptCodeSection::Ptr();
}
//---------------------------------------------------------------------------

// get pointer to the top non-conditional object. conditional fields are just proxies for
// dynamically laid-out fields and methods
ES_IMPL_INTF_METHOD(EsScriptObjectIntf*, EsScriptObject::topNonProxyGet)() ES_NOTHROW
{
  EsScriptObjectIntf* This = this;
  EsScriptObjectIntf* Parent = This->parentGet();
  while( Parent && (This->isConditional() || This->isArray()) )
  {
    This = Parent;
    Parent = This->parentGet();
  }

  ESSCRIPT_OBJECT_TRACE3(
    esT("topNonProxyGet for '%s' returned '%s'"),
    typeNameGet(),
    (nullptr == This) ?
      EsStdNames::null() :
      This->typeNameGet()
    )

  return This;
}
//---------------------------------------------------------------------------

// access This scoped method table
ES_IMPL_INTF_METHOD(EsScriptMethodMapPtr, EsScriptObject::thisScriptedMethodsGet)() const ES_NOTHROW
{
  return m_methods;
}
//---------------------------------------------------------------------------

// return root object for this object
ES_IMPL_INTF_METHOD(const EsScriptObjectIntf*, EsScriptObject::getRootObject)() const ES_NOTHROW
{
  if( isRoot() )
    return this;
  else
    return parentGet()->getRootObject();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsScriptObjectIntf*, EsScriptObject::getRootObject)() ES_NOTHROW
{
  if( isRoot() )
    return this;
  else
    return parentGet()->getRootObject();
}
//---------------------------------------------------------------------------

// set script object parent.
ES_IMPL_INTF_METHOD(void, EsScriptObject::setParent)(EsScriptObjectIntf* parent)
{
  // set parent even in metaclass mode.
  // while in metaclass mode parent pointer value is temporary,
  // its value is preserved during field compilation,
  // and is used in compile time handling of 'get top non-proxy' calls, etc.
  m_parent = parent;
  // use parent's execution context
  if( parent )
  {
    EsScriptObject* pa = reinterpret_cast<EsScriptObject*>(
      parent->implementorGet()
    );
    ES_ASSERT(pa);

    m_ctx = pa->m_ctx;
  }

  ESSCRIPT_OBJECT_TRACE5(
    esT("Parent %p is set for '%s' (%p) in '%s' mode"),
    EsVariant((void*)parent, EsVariant::ACCEPT_POINTER),
    typeNameGet(),
    EsVariant((void*)this, EsVariant::ACCEPT_POINTER),
    isMetaclass() ?
      esVT("metaclass") :
      esVT("instance")
  )
}
//---------------------------------------------------------------------------

// create new instance of EsScriptObject which is clone of the existing instance.
// if called from metaclass, new (default values used) object instance is created.
// may be called from the root objects (parent=NULL) only.
ES_IMPL_INTF_METHOD(EsScriptObjectIntf::Ptr, EsScriptObject::clone)() const
{
  ESSCRIPT_OBJECT_TRACE2(
    esT("clone called for '%s'"),
    typeNameGet()
  )
  if( !isMetaclass() && !isRoot() )
    EsScriptException::Throw(
      esT("'clone' method may be called only for the root objects"),
      m_ctx->vm()->currentDebugInfoGet()
    );

  // create shared data buffer instance
  bool needApplyFixups = false;
  EsScriptObjectDataBufferPtr buff = EsScriptObjectDataBuffer::create();
  if( !isMetaclass() && m_data )
    // copy data from the existing buffer
    buff->assign( m_data->begin(), m_data->end() );
  else
    needApplyFixups = true;

  // perform internal clone of the object instance
  EsScriptObjectIntf::Ptr result = internalClone(
    nullptr,
    buff,
    true
  );
  if( result )
  {
    // initiate memory layout update on newly cloned object
    result->internalUpdateLayout(0);
    // apply accumulated data buffer fixups, if data was created from scratch
    if( needApplyFixups )
      buff->fixupsApply( result->sizeGet() );
  }

  return result;
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsScriptObject::reflectedClone() const
{
  EsReflectedClassIntf::Ptr clone = m_ctx->vm()->objectCreate(typeNameGet());
  EsScriptObjectIntf::Ptr thisWeakRef = const_cast<EsScriptObject*>(this)->asBaseIntfPtrDirectWeak();
  ES_ASSERT(thisWeakRef);

  clone->copyFrom(thisWeakRef);

  return clone;
}
//---------------------------------------------------------------------------

void EsScriptObject::useContextOf(const EsBaseIntf::Ptr& other)
{
  EsScriptObjectIntf::Ptr sobj;
  EsScriptValAccessorIntf::Ptr vacc = other;
  if( vacc )
    sobj = vacc->get().asObject();
  else
    sobj = other;

  if( sobj )
  {
    EsScriptObject* pobj = ES_INTFPTR_TO_OBJECTPTR(sobj, EsScriptObject);
    ES_ASSERT(pobj);

    m_ctx->linkTo( pobj->m_ctx );
  }
  else
    EsScriptException::Throw(
      esT("Could not assign execution context from null, or non-scripted object instance"),
      m_ctx->vm()->currentDebugInfoGet()
    );
}
//---------------------------------------------------------------------------

// update object memory layout. fields and ancestor offsets and|or sizes get updated
// as a result of this process
//
ES_IMPL_INTF_METHOD(void, EsScriptObject::internalUpdateFieldsLayout)(ulong offs)
{
  ESSCRIPT_OBJECT_TRACE4(
    esT("internalUpdateFieldsLayout called for '%s' with offs=%d, ofNeedUpdateLayout is %s"),
    m_typeName,
    offs,
    (m_flags & ofNeedUpdateLayout) ?
      esVT("set") :
      esVT("not set")
  )

  if(
    (m_flags & ofNeedUpdateLayout) ||
    m_offs != static_cast<long>(offs)
  )
  {
    ulong localSize = m_ancestor ? m_ancestor->sizeGet() : 0;
    // set offset of the own fields
    for(ulong idx = 0; idx < m_fieldsMap.countGet(); ++idx)
    {
      const EsScriptObjectIntf::Ptr& field = fnodeFieldGet(m_fieldsMap.valueGet(idx));
      ES_ASSERT(field);

      ESSCRIPT_OBJECT_TRACE3(
        esT("calling internalUpdateLayout(%d) on filed '%s'"),
        offs+localSize,
        m_fieldsMap.nameGet(idx)
      )

      field->internalUpdateLayout(offs+localSize);
      localSize += field->sizeGet();
    }

    // check if we're not exceeding fixed size value
    if( m_attrsClass && m_attrsClass->attributeExists( fixedSizeAttr() ) )
    {
      ulong fixedSize = m_attrsClass->attributeGet( fixedSizeAttr() ).asULong();
      EsNumericCheck::checkRangeUInteger(
        1,
        fixedSize,
        localSize,
        esT("Calculated object size")
      );
    }

    m_size = localSize;
    m_flags &= ~ofNeedUpdateLayout;
  }
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsScriptObject::internalUpdateLayout)(ulong offs)
{
  ES_ASSERT( !isMetaclass() );

  ESSCRIPT_OBJECT_TRACE4(
    esT("internalUpdateLayout called for '%s' with offs=%d, ofNeedUpdateLayout is %s"),
    m_typeName,
    offs,
    (m_flags & ofNeedUpdateLayout) ?
      esVT("set") :
      esVT("not set")
  )

  if(
    (m_flags & ofNeedUpdateLayout) ||
    (static_cast<ulong>(m_offs) != offs)
  )
  {
    // get the most basic ancestor
    EsScriptObjectIntf* obj = m_ancestor.get();
    while( obj && obj->ancestorGet().get() )
      obj = obj->ancestorGet().get();

    while(obj && obj != this)
    {
      obj->internalUpdateFieldsLayout(offs);
      obj->internalOffsetSet(offs);
      obj = obj->parentGet();
    }

    internalUpdateFieldsLayout(offs);
    m_flags &= ~ofNeedUpdateLayout;
  }

  internalOffsetSet(offs);
}
//---------------------------------------------------------------------------

// for IF fields only, in instance mode. in metaclass mode empty pointer is returned
ES_IMPL_INTF_METHOD(EsScriptObjectIntf::Ptr, EsScriptObject::conditionalBranchGet)(int condition) const
{
  // not implemented by default, should not be called for ordinary script classes
  ES_FAIL_MSG(esT("Branch access is only make sense for IF conditional fields"));
  // pacify compilers
  return 0;
}
//---------------------------------------------------------------------------

// handle data change event notification from subscription publisher
ES_IMPL_INTF_METHOD(void, EsScriptObject::internalPublishDataChanged)(const EsScriptObjectIntf::WeakPtrList& subscribers)
{
  if( !subscribers.empty() )
  {
    for( EsScriptObjectIntf::WeakPtrList::const_iterator it = subscribers.begin();
         it != subscribers.end(); ++it)
    {
      if( !(*it)->isInvalid() )
        (*it)->internalInvalidateLayout(true);
    }
  }
  else if( m_parent )
    m_parent->internalPublishDataChanged(m_updateSubscribers);
}
//---------------------------------------------------------------------------

// handle data change event notification from subscription publisher
ES_IMPL_INTF_METHOD(void, EsScriptObject::publishDataChanged)(const EsScriptObjectIntf::WeakPtrList& subscribers)
{
  m_data->fixupsReset();
  internalPublishDataChanged(subscribers);
  // re-layout root object
  getRootObject()->internalUpdateLayout(0);
  // re-apply data buffer fixups
  m_data->fixupsApply( getRootObject()->sizeGet() );
}
//---------------------------------------------------------------------------

// internal write access to the object's offset value
ES_IMPL_INTF_METHOD(void, EsScriptObject::internalOffsetSet)(long offs)
{
  // NB! size value must already be pre-calculated|known at the moment this method is called
  //
  // if changing offset of POD object or fixed sized compound object - send notification to data buffer object.
  // on layout update completion, data size && blocks positioning fixups will be applied as appropriate
  if(
    -1 < offs &&
    offs != m_offs &&
    (isPOD() || (m_attrsClass && m_attrsClass->attributeExists( fixedSizeAttr() )))
  )
    m_data->onDataLayoutFixup(
      m_offs,
      offs,
      sizeGet()
    );

  ESSCRIPT_OBJECT_TRACE4(
    esT("%s::internalOffsetSet( from %d to %d )"),
    m_typeName,
    m_offs,
    offs
  )

  m_offs = offs;
}
//---------------------------------------------------------------------------

// add object value update subscriber
ES_IMPL_INTF_METHOD(void, EsScriptObject::updateSubscriberAdd)(EsScriptObjectIntf* subscriber)
{
  ES_ASSERT(nullptr != subscriber);
  if( m_updateSubscribers.end() == std::find(m_updateSubscribers.begin(), m_updateSubscribers.end(), subscriber) )
  {
    m_updateSubscribers.push_back(subscriber);
    ESSCRIPT_OBJECT_TRACE3(
      esT("Update subscriber '%s' added to '%s'"),
      subscriber->typeNameGet(),
      m_typeName
    )
  }
}
//---------------------------------------------------------------------------

// Return true if this object and all its parents are invalid
ES_IMPL_INTF_METHOD(bool, EsScriptObject::isAllInvalid)() const ES_NOTHROW
{
  bool isInvalid = ofNeedUpdateLayout == (m_flags & ofNeedUpdateLayout);
  EsScriptObjectIntf* parent = m_parent;
  while( parent && isInvalid )
  {
    isInvalid = parent->isInvalid();
    parent = parent->parentGet();
  }

  return isInvalid;
}
//---------------------------------------------------------------------------

// internal layout invalidation
ES_IMPL_INTF_METHOD(void, EsScriptObject::internalInvalidateLayout)(bool propagate)
{
  ESSCRIPT_OBJECT_TRACE3(
    esT("%s::internalInvalidateLayout(%s) called"),
    typeNameGet(),
    EsString::booleanStringGet(propagate)
  )
  // set invalid layout flags for this object and all its parents,
  // if propagation is set to true
  m_flags |= ofNeedUpdateLayout;

  if( propagate && !isAllInvalid() )
  {
    EsScriptObjectIntf* parent = m_parent;
    while( parent )
    {
      parent->internalInvalidateLayout(propagate);
      parent = parent->parentGet();
    }
  }
}
//---------------------------------------------------------------------------

// return object scripted method keys, with optional search restrictions
ES_IMPL_INTF_METHOD(EsMethodInfoKeysT, EsScriptObject::scriptedMethodKeysGet)(bool allHierarchy /*= true*/) const ES_NOTHROW
{
  EsMethodInfoKeysT result;
  if( !isPOD() && !isArray() && !isIf() )
  {
    EsScriptObjectMethodKeysCollector collector(
      asIntfT<EsScriptObjectIntf>(false), //< Cast this to EsScriptObjectIntf, do not incref
      allHierarchy
    );
    collector.traverse();
    result = collector.resultGet();
  }

  return result;
}
//---------------------------------------------------------------------------

// get object as binary buffer
ES_IMPL_INTF_METHOD(EsBinBuffer, EsScriptObject::binBufferGet)() const ES_NOTHROW
{
  EsBinBuffer result;
  EsBinBuffer::const_iterator start = m_data->begin() + offsGet();
  EsBinBuffer::const_iterator end = start + sizeGet();
  result.assign(start, end);

  return result;
}
//---------------------------------------------------------------------------

// (try to) set existing object from binary buffer
ES_IMPL_INTF_METHOD(bool, EsScriptObject::internalBinBufferSet)(EsBinBuffer::const_pointer& pos, EsBinBuffer::const_pointer end)
{
  ES_ASSERT( !isPOD() );

  EsString fieldName = firstFieldNameGet();
  while( !fieldName.empty() )
  {
    EsScriptObjectIntf::Ptr field = fieldGet(fieldName);
    if( !field->internalBinBufferSet(pos, end) )
      return false;

    fieldName = nextFieldNameGet(fieldName);
  }

  return true;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsScriptObject::binBufferSet)(const EsBinBuffer& buff)
{
  ES_ASSERT(!buff.empty());

  // create temporary object
  const EsScriptObjectIntf::Ptr& tmp = m_ctx->vm()->objectCreate(typeNameGet());
  ES_ASSERT(tmp);

  // assign our existing variable values to temporary object
  // NB! only unconditional values may exist in tmp at the moment of creation
  const EsStringArray& vars = tmp->varNamesGet();
  for(size_t idx = 0; idx < vars.size(); ++idx)
  {
    const EsString& varName = vars[idx];
    EsScriptValAccessorIntf::Ptr var = variableFind(
      varName, 
      false
    );
    if( var )
      tmp->variableSet(varName, var->get());
  }

  EsBinBuffer::const_pointer pos = &buff[0];
  if( tmp->internalBinBufferSet(pos, pos+buff.size()) )
    copyFrom(tmp);
  else
    EsScriptException::ThrowBinBufferDoesNotMatchObject(
      m_ctx->vm()->currentDebugInfoGet()
    );
}
//---------------------------------------------------------------------------

// finds the script object method declared in script.
// if method is not found, the empty pointer is returned
ES_IMPL_INTF_METHOD(EsScriptCodeSection::Ptr, EsScriptObject::findScriptedMethod)(const EsMethodInfoKeyT& key, bool allHierarchy) const ES_NOTHROW
{
  if( isPOD() || isArray() || isIf() )
    return EsScriptCodeSection::Ptr();

  EsScriptObjectMethodFinder finder(
    asIntfT<EsScriptObjectIntf>(false), //< Cast this to EsScriptObjectIntf, do not incref 
    key, 
    allHierarchy
  );
  finder.traverse();

  return finder.resultGet();
}
//---------------------------------------------------------------------------

// internal fnode finder service
EsVariant EsScriptObject::fnodeFind(const EsString& name) const ES_NOTHROW
{
  if( !isPOD() && !isArray() )
  {
    EsScriptObjectFieldFinder finder(
      asIntfT<EsScriptObjectIntf>(false), //< Cast this to EsScriptObjectIntf, do not incref
      name
    );
    finder.traverse();
    return finder.resultGet();
  }

  return EsVariant::null();
}
//---------------------------------------------------------------------------

// find named field in this object, conditional containers, or one of its ancestors.
// if field is not found, the empty pointer is returned
ES_IMPL_INTF_METHOD(EsScriptObjectIntf::Ptr, EsScriptObject::fieldFind)(const EsString& name) const ES_NOTHROW
{
  const EsVariant& fnode = fnodeFind(name);
  const EsScriptObjectIntf::Ptr& result = fnodeFieldGet( fnode );

  ESSCRIPT_OBJECT_TRACE4(
    esT("'%s' fieldFind('%s') returned %s"),
    typeNameGet(),
    name,
    (result ?
      esVT("field object") :
      esVT("null")
    )
  )

  return result;
}
//---------------------------------------------------------------------------

// return all field names, in order of their declaration in object|base objects
ES_IMPL_INTF_METHOD(EsStringArray, EsScriptObject::fieldNamesGet)() const ES_NOTHROW
{
  if( !isPOD() && !isArray() )
  {
    EsScriptObjectFieldNamesCollector collector(
      asIntfT<EsScriptObjectIntf>(false) //< Cast this to EsScriptObjectIntf, do not incref
    );
    collector.traverse();
    return collector.resultGet();
  }

  return EsString::nullArray();
}
//---------------------------------------------------------------------------

// return field object by name
ES_IMPL_INTF_METHOD(EsBaseIntfPtr, EsScriptObject::fieldGet)(const EsString& name) const
{
  const EsScriptObjectIntf::Ptr& result = fieldFind(name);

  if( !result )
    EsScriptException::ThrowFieldIsNotDeclared(
      name,
      m_typeName,
      m_ctx->vm()->currentDebugInfoGet()
    );

  return result;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsBaseIntfPtr, EsScriptObject::fieldAttrsGet)(const EsString& name) const
{
  const EsVariant& fnode = fnodeFind(name);
  return fnodeAttrsGet(fnode);
}
//---------------------------------------------------------------------------

// call ancestor's method implementation (as if our 'this' was cast to ancestor's 'this')
ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::callAncestorMethod)(const EsString& name, const EsVariant& params, const EsString& ancestorTypeName)
{
  ES_ASSERT(!ancestorTypeName.empty());

  // find appropriate ancestor
  if( typeNameGet() != ancestorTypeName )
  {
    EsScriptObjectIntf::Ptr ancestor = ancestorGet();
    while( ancestor )
    {
      if( ancestor->typeNameGet() == ancestorTypeName )
      {
        EsReflectedClassIntf::Ptr robj = ancestor;
        ES_ASSERT(robj);
        return robj->callMethod(name, params);
      }
      ancestor = ancestor->ancestorGet();
    }

    // somehow, requested ancestor is not found in our base objects
    EsScriptException::Throw(
      EsString::format(
        esT("'%s' is not a base object of '%s'"),
        ancestorTypeName,
        typeNameGet()
      ),
      m_ctx->vm()->currentDebugInfoGet()
    );
  }
  else
    return callMethod(name, params);

  return EsVariant::null();
}
//---------------------------------------------------------------------------

// EsReflectedClassIntf interface implementation. EsScriptObject has its own specifics
// due to its dual nature - one part is C++ -coded reflection, other is script-coded
//
ES_IMPL_INTF_METHOD(bool, EsScriptObject::isKindOf)(const EsReflectedClassIntf::Ptr& other) const ES_NOTHROW
{
  if( other )
    return isKindOf( other->typeNameGet() );

  return false;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsScriptObject::isKindOf)(const EsString& other) const ES_NOTHROW
{
  bool result = false;
  if( !other.empty() )
  {
    // first, check reflected class part
    result = classInfoGet().isKindOf( other );

    if( !result )
    {
      // check type names
      if( other == typeNameGet() )
        result = true;
      else
      {
        EsScriptObjectIntf::Ptr ancestor = m_ancestor;
        while( ancestor )
        {
          if( other == ancestor->typeNameGet() )
          {
            result = true;
            break;
          }
          ancestor = ancestor->ancestorGet();
        }
      }
    }
  }

  return result;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsScriptObject::isKindOf)(const EsClassInfo& other) const ES_NOTHROW
{
  return isKindOf( other.nameGet() );
}
//---------------------------------------------------------------------------

EsVariant EsScriptObject::varCopy(const EsVariant& src) const
{
  if( src.isObject() )
  {
    EsReflectedClassIntf::Ptr robj = src.asObject();
    if( robj )
    {
      // try to copy object by value
      const EsReflectedClassIntf::Ptr& cpy = m_ctx->vm()->objectCreate( robj->typeNameGet() );
      ES_ASSERT(cpy);

      cpy->copyFrom( robj );

      return cpy;
    }
  }
  else if( EsVariant::VAR_VARIANT_COLLECTION == src.typeGet() )
  {
    // try to copy variant collection by value
    const EsVariant::Array& asrc = src.doInterpretAsVariantCollection();
    EsVariant::Array result;
    result.reserve( asrc.size() );
    for(size_t idx = 0; idx < asrc.size(); ++idx)
      result.push_back( varCopy( asrc[idx] ) );

    return result;
  }

  return src;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsScriptObject::copyFrom)(cr_EsBaseIntfPtr p1)
{
  EsReflectedClassIntf::Ptr other = p1;
  if( other )
  {
    if( other->typeNameGet() == typeNameGet() )
    {
      EsScriptObjectIntf::Ptr intf = other;
      ES_ASSERT(intf);

      if( intf.get() == this )
        EsScriptException::Throw(
          esT("Could not copy from itself"),
          m_ctx->vm()->currentDebugInfoGet()
        );
      else
      {
        if( isPOD() )
          propertySet( 
            EsStdNames::value(), 
            other->propertyGet(EsStdNames::value()) 
          );
        else if( isArray() )
        {
          for(int idx = 0; idx < call(EsStdNames::countGet()).asInt(); ++idx )
          {
            EsScriptObjectIntf::Ptr thisItem = call(EsStdNames::itemGet(), idx).asExistingObject();
            ES_ASSERT(thisItem);
            EsScriptObjectIntf::Ptr otherItem = other->call(EsStdNames::itemGet(), idx).asExistingObject();
            ES_ASSERT(otherItem);
            EsReflectedClassIntf::Ptr robj = thisItem;
            ES_ASSERT(robj);
            robj->copyFrom( otherItem );
          }
        }
        else
        {
          // try member-by-member copy. only variables,
          // which exist at the moment of copying will be assigned
          const EsStringArray& varNames = intf->varNamesGet();
          for(size_t idx = 0; idx < varNames.size(); ++idx)
          {
            const EsString& varName = varNames[idx];
            EsScriptValAccessorIntf::Ptr ourVar = variableFind(varName, false);
            if( ourVar )
            {
              const EsVariant& src = intf->variableGet(varName);
              ourVar->set( varCopy(src) );
            }
          }

          // field-by-field copy
          EsString fieldName = intf->firstFieldNameGet();
          while( !fieldName.empty() )
          {
            EsReflectedClassIntf::Ptr thisField = fieldGet(fieldName);
            EsReflectedClassIntf::Ptr otherField = intf->fieldGet(fieldName);
            thisField->copyFrom( otherField );
            fieldName = intf->nextFieldNameGet(fieldName);
          }
        }
      }
    }
    else
      EsScriptException::Throw(
        esT("Could not copy from object of different type"),
        m_ctx->vm()->currentDebugInfoGet()
      );
  }
  else
    EsScriptException::Throw(
      esT("Could not copy from null object"),
      m_ctx->vm()->currentDebugInfoGet()
    );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsBaseIntfPtr, EsScriptObject::asWeakReference)() const ES_NOTHROW
{
  return const_cast<EsScriptObject*>(this)->asBaseIntfPtrDirectWeak();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsScriptObject::isIndexed)() const ES_NOTHROW
{
  return classInfoGet().isIndexed() ||
    ( hasMethod(EsMethodInfoKeyT(0, EsStdNames::countGet())) &&
      ( hasMethod(EsMethodInfoKeyT(1, EsStdNames::itemGet())) ||
        hasMethod(EsMethodInfoKeyT(2, EsStdNames::itemSet())) ));
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsString::Array, EsScriptObject::propertyNamesGet)() const ES_NOTHROW
{
  EsString::Array result = classInfoGet().propertyNamesGet();

  if( !isPOD() && !isArray() && !isIf() )
  {
    EsScriptObjectPropNamesCollector collector(
      asIntfT<EsScriptObjectIntf>(false), //< Cast this to EsScriptObjectIntf, do not incref
      false
    );
    collector.traverse();

    const EsString::Array& scripted = collector.resultGet();
    result.insert(result.end(), scripted.begin(), scripted.end());
  }

  return result;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsString::Array, EsScriptObject::persistentPropertyNamesGet)() const ES_NOTHROW
{
  EsString::Array result = classInfoGet().propertyNamesGet(true);

  if( !isPOD() && !isArray() && !isIf() )
  {
    EsScriptObjectPropNamesCollector collector(
      asIntfT<EsScriptObjectIntf>(false), //< Cast this to EsScriptObjectIntf, do not incref
      true
    );
    collector.traverse();

    const EsString::Array& scripted = collector.resultGet();
    result.insert(result.end(), scripted.begin(), scripted.end());
  }

  return result;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsScriptObject::hasProperty)(const EsString& name) const ES_NOTHROW
{
  if( !classInfoGet().hasProperty(name) &&
      !isPOD() && !isArray() && !isIf() )
  {
    EsScriptObjectPropertyFinder finder(
      asIntfT<EsScriptObjectIntf>(false), //< Cast this to EsScriptObjectIntf, do not incref
      name
    );
    finder.traverse();

    return finder.resultGet();
  }

  return true;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsScriptObject::propertyCanRead)(const EsString& name) const
{
  EsScriptObjectPropertyInfoIntf::Ptr prop;
  if( !classInfoGet().hasProperty(name) &&
    !isPOD() && !isArray() && !isIf() )
  {
    EsScriptObjectPropertyFinder finder(
      asIntfT<EsScriptObjectIntf>(false), //< Cast this to EsScriptObjectIntf, do not incref
      name
    );
    finder.traverse();
    prop = finder.resultGet();
  }

  if( prop )
    return !prop->readerNameGet(false).empty();
  else
  {
    const EsPropertyInfo* info = classInfoGet().propertyInfoFind(name);
    if( !info )
      EsScriptException::ThrowPropertyIsNotDeclared(
        name,
        typeNameGet(),
        m_ctx->vm()->currentDebugInfoGet()
      );

    return info->canRead();
  }
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsScriptObject::propertyCanWrite)(const EsString& name) const
{
  EsScriptObjectPropertyInfoIntf::Ptr prop;
  if( !classInfoGet().hasProperty(name) &&
    !isPOD() && !isArray() && !isIf() )
  {
    EsScriptObjectPropertyFinder finder(
      asIntfT<EsScriptObjectIntf>(false), //< Cast this to EsScriptObjectIntf, do not incref
      name
    );
    finder.traverse();
    prop = finder.resultGet();
  }

  if( prop )
    return !prop->writerNameGet(false).empty();
  else
  {
    const EsPropertyInfo* info = classInfoGet().propertyInfoFind(name);
    if( !info )
      EsScriptException::ThrowPropertyIsNotDeclared(
        name,
        typeNameGet(),
        m_ctx->vm()->currentDebugInfoGet()
      );

    return info->canWrite();
  }
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::propertyGet)(const EsString& name) const
{
  EsVariant result;
  EsScriptObjectPropertyInfoIntf::Ptr prop;
  if( !isPOD() && !isArray() && !isIf() )
  {
    EsScriptObjectPropertyFinder finder(
      asIntfT<EsScriptObjectIntf>(false), //< Cast this to EsScriptObjectIntf, do not incref
      name
    );
    finder.traverse();
    prop = finder.resultGet();
  }

  if( prop )
  {
    const EsString& rname = prop->readerNameGet();
    result = const_cast<EsScriptObject*>(this)->call(rname);
  }
  else
  {
    const EsPropertyInfo* info = classInfoGet().propertyInfoFind(name);
    if( !info )
      EsScriptException::ThrowPropertyIsNotDeclared(
        name,
        typeNameGet(),
        m_ctx->vm()->currentDebugInfoGet()
      );

    result = info->get(  
      this
    );
  }

  return result;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsScriptObject::propertySet)(const EsString& name, const EsVariant& val)
{
  EsScriptObjectPropertyInfoIntf::Ptr prop;
  if( !isPOD() && !isArray() && !isIf() )
  {
    EsScriptObjectPropertyFinder finder(
      asIntfT<EsScriptObjectIntf>(false), //< Cast this to EsScriptObjectIntf, do not incref
      name
    );
    finder.traverse();
    prop = finder.resultGet();
  }

  if( prop )
  {
    const EsString& wname = prop->writerNameGet();
    call(wname, val);
  }
  else
  {
    const EsPropertyInfo* info = classInfoGet().propertyInfoFind(name);
    if( !info )
      EsScriptException::ThrowPropertyIsNotDeclared(
        name, 
        typeNameGet(), 
        m_ctx->vm()->currentDebugInfoGet()
      );

    info->set( 
      this, 
      val
    );
  }
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsScriptObject::hasMethod)(const EsMethodInfoKeyT& key) const ES_NOTHROW
{
  return classInfoGet().hasMethod(key) ||
    findScriptedMethod(key);
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsScriptObject::hasClassMethod)(const EsMethodInfoKeyT& key) const ES_NOTHROW
{
  return classInfoGet().hasClassMethod(key);
  // todo: support class methods definition in script ???
}
//---------------------------------------------------------------------------

bool EsScriptObject::hasMethod(cr_EsString name, ulong paramCnt) const ES_NOTHROW
{
  return hasMethod(
    EsMethodInfoKeyT(
      paramCnt,
      name
    )
  );
}
//---------------------------------------------------------------------------

bool EsScriptObject::hasClassMethod(cr_EsString name, ulong paramCnt) const ES_NOTHROW
{
  return hasClassMethod(
    EsMethodInfoKeyT(
      paramCnt,
      name
    )
  );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::callMethod)(const EsString& name, const EsVariant& params)
{
  int paramsCount = 0;
  if( !params.isEmpty() )
  {
    ES_ASSERT(EsVariant::VAR_VARIANT_COLLECTION == params.typeGet());
    paramsCount = params.countGet();
  }

  EsMethodInfoKeyT key(paramsCount, name);
  EsScriptCodeSection::Ptr method = findScriptedMethod(key);
  if( method )
  {
    EsScriptValAccessorIntf::Ptr ret = m_ctx->vm()->exec(
      method,
      params,
      EsScriptEvalMode::evalFunc,
      this
    );

    return ret->get();
  }

  const EsMethodInfo* info = classInfoGet().methodInfoFind(key);
  if( info )
    return info->call(
      this, 
      params
    );

  EsScriptException::Throw(
    EsString::format(
      esT("'%s', taking %d parameters, is not method of '%s', or any of its base objects"),
      name,
      paramsCount,
      typeNameGet()
    ),
    m_ctx ?
      m_ctx->vm()->currentDebugInfoGet() :
      EsScriptDebugInfoIntf::Ptr()
  );

  return EsVariant::null(); //< Pacify compiler
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::call)(const EsString& name)
{
  return callMethod(
    name,
    EsVariant::null()
  );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::call)(const EsString& name, const EsVariant& param1)
{
  EsVariant::Array params = {
    param1
  };

  return callMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::call)(const EsString& name, const EsVariant& param1, const EsVariant& param2)
{
  EsVariant::Array params = {
    param1,
    param2
  };

  return callMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3)
{
  EsVariant::Array params = {
    param1,
    param2,
    param3
  };

  return callMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4)
{
  EsVariant::Array params = {
    param1,
    param2,
    param3,
    param4
  };

  return callMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5)
{
  EsVariant::Array params = {
    param1,
    param2,
    param3,
    param4,
    param5
  };

  return callMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6)
{
  EsVariant::Array params = {
    param1,
    param2,
    param3,
    param4,
    param5,
    param6
  };

  return callMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::callClassMethod)(const EsString& name, const EsVariant& params) const
{
  int paramsCount = 0;
  if( !params.isEmpty() )
  {
    ES_ASSERT(EsVariant::VAR_VARIANT_COLLECTION == params.typeGet());
    paramsCount = params.countGet();
  }

  const EsMethodInfo& info = classInfoGet().classMethodInfoGet(EsMethodInfoKeyT(paramsCount, name));
  return info.classCall(params);
}
//---------------------------------------------------------------------------

// set value parent notification
void EsScriptObject::onSetValueParentNotify()
{
  if( m_parent )
    m_parent->publishDataChanged(m_updateSubscribers);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// reflected properties
//
ulong EsScriptObject::get_size() const
{
  return sizeGet();
}
//---------------------------------------------------------------------------

ulong EsScriptObject::get_offset() const
{
  return offsGet();
}
//---------------------------------------------------------------------------

EsStringArray EsScriptObject::get_classAttributeNames() const
{
  return classAttributeNamesGet();
}
//---------------------------------------------------------------------------

EsStringArray EsScriptObject::get_fieldNames() const
{
  return fieldNamesGet();
}
//---------------------------------------------------------------------------

EsBinBuffer EsScriptObject::get_buffer() const
{
  return binBufferGet();
}
//---------------------------------------------------------------------------

void EsScriptObject::set_buffer(const EsBinBuffer& buff)
{
  binBufferSet(buff);
}
//---------------------------------------------------------------------------

EsVariant EsScriptObject::get___scriptHost() const
{
  ES_ASSERT(m_ctx);
  return m_ctx->vm()->hostGet();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// reflected methods
//
bool EsScriptObject::hasField(const EsString& name) const ES_NOTHROW
{
  return fieldFind(name);
}
//---------------------------------------------------------------------------

// return the first field name. if object contain no fields, empty string is returned
EsString EsScriptObject::firstFieldNameGet() const ES_NOTHROW
{
  const EsStringArray& names = fieldNamesGet();
  if( !names.empty() )
    return names[0];

  return EsString::null();
}

// return field object name which is next to one specified. if specified name is the last one,
// the empty string is returned
EsString EsScriptObject::nextFieldNameGet(const EsString& name) const ES_NOTHROW
{
  const EsStringArray& names = fieldNamesGet();
  if( !names.empty() )
  {
    for(size_t idx = 0; idx < names.size(); ++idx)
      if( 0 == name.compare(names[idx]) &&
          (idx+1) < names.size() )
        return names[idx+1];
  }

  return EsString::null();
}

// value validator (meaningful for POD objects only)
ES_IMPL_INTF_METHOD(void, EsScriptObject::validate)(const EsVariant& val) const
{
  ES_ASSERT(isPOD());
  if( m_attrsInst )
  {
    const EsVariant& rx = m_attrsInst->attributeGetDef(esT("restriction"), EsVariant::null());
    if( !rx.isEmpty() && !rx.has(val) )
      EsScriptException::Throw(esT("Validation failed"), m_ctx->vm()->currentDebugInfoGet());
  }
}

ES_IMPL_INTF_METHOD(ulong, EsScriptObject::sizeGet)() const ES_NOTHROW
{
  if( m_attrsClass && m_attrsClass->attributeExists( fixedSizeAttr() ) )
    return m_attrsClass->attributeGet( fixedSizeAttr() ).asULong();

  return m_size;
}

// member variables manipulation
ES_IMPL_INTF_METHOD(void, EsScriptObject::variableDeclare)(const EsString& name, const EsScriptDebugInfoIntf::Ptr& dbg)
{
  ES_ASSERT(!name.empty());

  if( isFinal() )
    EsScriptException::ThrowFinalObjectMayNotBeModified(typeNameGet(), dbg);

  if(  isPOD() || isArray() || isIf() )
    EsScriptException::ThrowPodObjectMayNotContainFieldsVarsOrProps(typeNameGet(), dbg);

  // check for fields or vars with such name
  EsScriptObjectFieldFastFinder ffinder(
    asIntfT<EsScriptObjectIntf>(false), //< Cast to EsScriptObjectIntf, non incref-ed
    name
  );
  EsScriptObjectVarFastFinder vfinder(
    asIntfT<EsScriptObjectIntf>(false), //< Cast to EsScriptObjectIntf, non incref-ed 
    name
  );
  if( ffinder.found() || vfinder.found() )
    EsScriptException::ThrowFieldOrVarAlreadyDeclared(name, typeNameGet(), dbg);

  if( !m_memberVars )
    m_memberVars = EsScriptSymbolTable::Ptr(
      new EsScriptSymbolTable(false)
    );

  m_memberVars->symbolTemplateAdd(name, EsScriptSymbolFlag::None, dbg);
}

// return only this object member variable names
ES_IMPL_INTF_METHOD(EsStringArray, EsScriptObject::thisVariablesNamesGet)() const ES_NOTHROW
{
  if( m_memberVars )
    return m_memberVars->allSymbolNamesGet();

  return EsString::nullArray();
}

ES_IMPL_INTF_METHOD(EsStringArray, EsScriptObject::thisScriptedPropertiesNamesGet)(bool persistentOnly) const ES_NOTHROW
{
  if( !m_propsMap )
    return EsString::nullArray();

  EsStringArray result;
  result.reserve( m_propsMap->countGet() );

  for(ulong idx = 0; idx < m_propsMap->countGet(); ++idx)
  {
    EsScriptObjectPropertyInfoIntf::Ptr propInfo = m_propsMap->valueGet(idx).asExistingObject();
    if( persistentOnly && !propInfo->isPersistent() )
      continue;

    result.push_back(propInfo->nameGet());
  }

  return result;
}

ES_IMPL_INTF_METHOD(EsScriptMachine*, EsScriptObject::machineGet)() ES_NOTHROW
{
  return m_ctx->vm();
}

ES_IMPL_INTF_METHOD(void, EsScriptObject::machineSet)(EsScriptMachine* other)
{
  ES_ASSERT( isRoot() );
  ES_ASSERT( !isMetaclass() );

  m_ctx->switchTo(other);
}

// find member var accessor by name
ES_IMPL_INTF_METHOD(EsScriptValAccessorIntf::Ptr, EsScriptObject::variableFind)(const EsString& name, bool doThrow /*= false*/) const
{
  EsScriptValAccessorIntf::Ptr result;
  if( !isPOD() && !isArray() && !isIf() )
  {
    EsScriptObjectVariableFinder varFinder(
      asIntfT<EsScriptObjectIntf>(false), //< Cast this to EsScriptObjectIntf, do not incref
      name
    );
    varFinder.traverse();
    result = varFinder.resultGet();
    if( !varFinder.found() && doThrow )
      EsScriptException::ThrowMemberVarIsNotDeclared(
        name,
        typeNameGet(),
        m_ctx->vm()->currentDebugInfoGet()
      );
  }

  return result;
}

// return true if variable with specified name exists in object or its ancestors
ES_IMPL_INTF_METHOD(bool, EsScriptObject::hasVariable)(const EsString& name) const ES_NOTHROW
{
   if( !isPOD() && !isArray() && !isIf() )
  {
    EsScriptObjectVariableFinder varFinder(
      asIntfT<EsScriptObjectIntf>(false), //< Cast this to EsScriptObjectIntf, do not incref
      name
    );
    varFinder.traverse();

    return varFinder.found();
  }

  return false;
}

// return all variable names
ES_IMPL_INTF_METHOD(EsStringArray, EsScriptObject::varNamesGet)() const ES_NOTHROW
{
  if( !isPOD() && !isArray() && !isIf() )
  {
    EsScriptObjectVarNamesCollector collector(
      asIntfT<EsScriptObjectIntf>(false) //< Cast this to EsScriptObjectIntf, do not incref
    );
    collector.traverse();
    return collector.resultGet();
  }

  return EsString::nullArray();
}

// return member variable value
ES_IMPL_INTF_METHOD(const EsVariant&, EsScriptObject::variableGet)(const EsString& name) const
{
  EsScriptValAccessorIntf::Ptr var = variableFind(name, true);
  return var->get();
}

// set member variable value
ES_IMPL_INTF_METHOD(void, EsScriptObject::variableSet)(const EsString& name, const EsVariant& val)
{
  EsScriptValAccessorIntf::Ptr var = variableFind(name, true);
  var->set(val);
  // If we're settings script object to our variable, link its execution context to us
}

EsStringArray EsScriptObject::get_memberVarNames() const
{
  return varNamesGet();
}

// properties declaration and manipulation
bool EsScriptObject::thisHasScriptedProperty(const EsString& name) const ES_NOTHROW
{
  if( m_propsMap )
    return m_propsMap->itemExists(name);

  return false;
}

void EsScriptObject::propertyDeclare(const EsString& name, const EsAttributesIntf::Ptr& attrs,
  const EsString& readerName, const EsString& writerName)
{
  ES_ASSERT( isMetaclass() );
  const EsString& typeName = typeNameGet();

  if( isFinal() )
    EsScriptException::ThrowFinalObjectMayNotBeModified(typeName, m_ctx->vm()->currentDebugInfoGet());

  if( isPOD() || isArray() || isIf() )
    EsScriptException::ThrowPodObjectMayNotContainFieldsVarsOrProps(typeName, m_ctx->vm()->currentDebugInfoGet());

  EsScriptObjectPropFastFinder ffinder(
    asIntfT<EsScriptObjectIntf>(false), //< Cast to EsScriptObjectIntf, non incref-ed
    name
  );
  if( ffinder.found() )
    EsScriptException::ThrowPropertyAlreadyDeclared(name, typeName, m_ctx->vm()->currentDebugInfoGet());

  if( !m_propsMap )
    m_propsMap.reset(
      new EsStringIndexedMap(
        typeNameGet(),
        EsStringIndexedMap::ContainerWithoutInterlock
      )
    );
  ES_ASSERT(m_propsMap);

  EsScriptObjectPropertyInfoIntf::Ptr propInfo = EsScriptObjectPropertyInfo::create(
    name,
    attrs,
    this,
    readerName,
    writerName
  );
  ES_ASSERT(propInfo);

  m_propsMap->itemAdd(
    name,
    propInfo
  );
}

EsStringArray EsScriptObject::propertyAttributeNamesGet(cr_EsString propName) const
{
  EsStringArray result;
  EsScriptObjectPropertyInfoIntf::Ptr prop;
  if( !isPOD() && !isArray() && !isIf() )
  {
    EsScriptObjectPropertyFinder finder(
      asIntfT<EsScriptObjectIntf>(false), //< Cast to EsScriptObjectIntf, non incref-ed
      propName
    );
    finder.traverse();
    prop = finder.resultGet();
  }

  if( prop )
    result = prop->attributeNamesGet();
  else
  {
    const EsPropertyInfo* info = classInfoGet().propertyInfoFind(propName);
    if( !info )
      EsScriptException::ThrowPropertyIsNotDeclared(
        propName, 
        typeNameGet(), 
        m_ctx->vm()->currentDebugInfoGet()
      );

    EsAttributesIntf::Ptr attrs = info->attributesAccess();
    if( attrs )
      result = attrs->allNamesGet();
  }

  return result;
}

bool EsScriptObject::propertyHasAttribute(cr_EsString propName, cr_EsString attrName) const
{
  EsScriptObjectPropertyInfoIntf::Ptr prop;
  if( !isPOD() && !isArray() && !isIf() )
  {
    EsScriptObjectPropertyFinder finder(
      asIntfT<EsScriptObjectIntf>(false), //< Cast to EsScriptObjectIntf, non incref-ed
      propName
    );
    finder.traverse();
    prop = finder.resultGet();
  }

  if( prop )
    return prop->hasAttribute(attrName);
  else
  {
    const EsPropertyInfo* info = classInfoGet().propertyInfoFind(propName);
    if( !info )
      EsScriptException::ThrowPropertyIsNotDeclared(
        propName, 
        typeNameGet(), 
        m_ctx->vm()->currentDebugInfoGet()
      );
    EsAttributesIntf::Ptr attrs = info->attributesAccess();
    if( attrs )
      return attrs->attributeExists(attrName);
  }

  return false;
}

EsVariant EsScriptObject::propertyAttributeGet(cr_EsString propName, cr_EsString attrName) const
{
  EsScriptObjectPropertyInfoIntf::Ptr prop;
  if( !isPOD() && !isArray() && !isIf() )
  {
    EsScriptObjectPropertyFinder finder(
      asIntfT<EsScriptObjectIntf>(false), //< Cast to EsScriptObjectIntf, non incref-ed
      propName
    );
    finder.traverse();
    prop = finder.resultGet();
  }

  if( prop )
    return prop->attributeGet(attrName);
  else
  {
    const EsPropertyInfo* info = classInfoGet().propertyInfoFind(propName);
    if( !info )
      EsScriptException::ThrowPropertyIsNotDeclared(
        propName, 
        typeNameGet(),
        m_ctx->vm()->currentDebugInfoGet()
      );
    EsAttributesIntf::Ptr attrs = info->attributesAccess();
    if( attrs )
      return attrs->attributeGet(attrName);
  }
  // pacify compilers
  return EsVariant::null();
}

EsString EsScriptObject::propertyLabelGet(cr_EsString propName) const
{
  EsScriptObjectPropertyInfoIntf::Ptr prop;
  if( !isPOD() && !isArray() && !isIf() )
  {
    EsScriptObjectPropertyFinder finder(
      asIntfT<EsScriptObjectIntf>(false), //< Cast to EsScriptObjectIntf, non incref-ed
      propName
    );
    finder.traverse();
    prop = finder.resultGet();
  }

  if( prop )
  {
    if( prop->hasAttribute(esT("label")) )
      return prop->attributeGet(esT("label")).asString();
  }
  else
  {
    const EsPropertyInfo* info = classInfoGet().propertyInfoFind(propName);
    if( !info )
      EsScriptException::ThrowPropertyIsNotDeclared(
        propName, 
        typeNameGet(), 
        m_ctx->vm()->currentDebugInfoGet()
      );

    return info->labelGet();
  }

  // pacify compilers
  return EsString::null();
}

EsString EsScriptObject::propertyDescriptionGet(cr_EsString propName) const
{
  EsScriptObjectPropertyInfoIntf::Ptr prop;
  if( !isPOD() && !isArray() && !isIf() )
  {
    EsScriptObjectPropertyFinder finder(
      asIntfT<EsScriptObjectIntf>(false), //< Cast to EsScriptObjectIntf, non incref-ed
      propName
    );
    finder.traverse();
    prop = finder.resultGet();
  }

  if( prop )
  {
    if( prop->hasAttribute(esT("help")) )
      return prop->attributeGet(esT("help")).asString();
  }
  else
  {
    const EsPropertyInfo* info = classInfoGet().propertyInfoFind(propName);
    if( !info )
      EsScriptException::ThrowPropertyIsNotDeclared(
        propName,
        typeNameGet(),
        m_ctx->vm()->currentDebugInfoGet()
      );

    return info->descriptionGet();
  }

  // pacify compilers
  return EsString::null();
}

//---------------------------------------------------------------------------
// "Fast" traversers specialization
//
// Fast field finder
EsScriptObjectFieldFastFinder::EsScriptObjectFieldFastFinder(
  const EsScriptObjectIntf* obj,
  const EsString& name
) ES_NOTHROW :
EsScriptObjectFastFindBase(obj),
m_found(false),
m_name(name)
{
  ES_ASSERT(!m_name.empty());
}

bool EsScriptObjectFieldFastFinder::found() ES_NOTHROW
{
  if( !m_found )
    find();

  return m_found;
}

const EsVariant& EsScriptObjectFieldFastFinder::resultGet() ES_NOTHROW
{
  if( !m_found )
    find();

  ESSCRIPT_OBJECT_TRACE4(
    esT("Field '%s' fast search, called for '%s', returned %s"),
    m_name,
    m_startObj->typeNameGet(),
    (m_result.isEmpty() ?
      esVT("null") :
      esVT("field node")
    )
  )

  return m_result;
}

// overridable services
bool EsScriptObjectFieldFastFinder::objectProcess() ES_NOTHROW
{
  const EsStringIndexedMap& sm = m_obj->thisFieldsMapGet();
  ulong idx = sm.itemFind(m_name);
  if( EsStringIndexedMap::npos != idx )
  {
    m_result = sm.valueGet(idx);
    m_found = true;
    return false; // break search
  }

  return true;
}

// Fast variable finder
EsScriptObjectVarFastFinder::EsScriptObjectVarFastFinder(
  const EsScriptObjectIntf* obj,
  const EsString& name
) ES_NOTHROW :
EsScriptObjectFastFindBase(obj),
m_name(name),
m_found(false)
{
  ES_ASSERT(!m_name.empty());
}

EsScriptValAccessorIntf::Ptr EsScriptObjectVarFastFinder::resultGet() ES_NOTHROW
{
  if( !m_found )
    find();

  ESSCRIPT_OBJECT_TRACE4(
    esT("Var '%s' fast search, called for '%s', returned %s"),
    m_name,
    m_startObj->typeNameGet(),
    (m_found ?
      esVT("var accessor") :
      esVT("null")
    )
  )

  return m_result;
}

bool EsScriptObjectVarFastFinder::found() ES_NOTHROW
{
  if( !m_found )
    find();

  return m_found;
}

// overridable services
bool EsScriptObjectVarFastFinder::objectProcess() ES_NOTHROW
{
  EsScriptSymbolTable::Ptr vars = m_obj->thisVariablesGet();

  if( vars )
  {
    EsScriptValAccessorIntf::Ptr sym = vars->symbolFind(m_name, m_found);
    if( m_found )
    {
      m_result = sym;
      return false; // break search
    }
  }

  return true;
}

// Fast method finder
EsScriptObjectMethodFastFinder::EsScriptObjectMethodFastFinder(
  const EsScriptObjectIntf* obj,
  const EsMethodInfoKeyT& key
) ES_NOTHROW :
EsScriptObjectFastFindBase(obj),
m_key(key)
{
  ES_ASSERT(m_key.isOk());
}

EsScriptCodeSection::Ptr EsScriptObjectMethodFastFinder::resultGet() ES_NOTHROW
{
  find();

  ESSCRIPT_OBJECT_TRACE5(
    esT("Method '%s[%d]' fast search, called for '%s', returned %s"),
    m_key.nameGet(),
    m_key.parametersCountGet(),
    m_startObj->typeNameGet(),
    (m_result ?
      esVT("code section") :
      esVT("null")
    )
  )

  return m_result;
}

// overridable services
bool EsScriptObjectMethodFastFinder::objectProcess() ES_NOTHROW
{
  EsScriptMethodMapPtr smm = m_obj->thisScriptedMethodsGet();
  if( smm )
  {
    EsScriptMethodMap::const_iterator cit = smm->find(m_key);
    if( cit != smm->end() )
    {
      m_result = (*cit).second;
      return false; // break search
    }
  }

  return true;
}

// Fast property finder
EsScriptObjectPropFastFinder::EsScriptObjectPropFastFinder(
  const EsScriptObjectIntf* obj,
  const EsString& name
) ES_NOTHROW :
EsScriptObjectFastFindBase(obj),
m_name(name),
m_result(false)
{
  ES_ASSERT(!m_name.empty());
}

bool EsScriptObjectPropFastFinder::found() ES_NOTHROW
{
  if( !m_result )
    find();

  ESSCRIPT_OBJECT_TRACE4(
    esT("Property '%s' fast search, called for '%s', returned %s"),
    m_name,
    m_startObj->typeNameGet(),
    (m_result ?
      esVT("found") :
      esVT("not found")
    )
  )

  return m_result;
}

// overridable services
bool EsScriptObjectPropFastFinder::objectProcess() ES_NOTHROW
{
  EsStringIndexedMap::Ptr pm = m_obj->thisPropertiesMapGet();
  if( pm )
  {
    size_t idx = pm->itemFind(m_name);
    if( EsStringIndexedMap::npos != idx )
    {
      m_result = true;
      return false; // break search
    }
  }

  return true;
}
