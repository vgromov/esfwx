#ifndef _es_script_object_property_info_h_
#define _es_script_object_property_info_h_

// script object property info interface definition
ES_DECL_INTF_BEGIN2( D4118B76, B38F4A15, AB4BE98F, D0E7CF37, EsScriptObjectPropertyInfoIntf, EsBaseIntf)
protected:  
  // assign instance attributes pointer
  ES_DECL_INTF_METHOD(EsAttributesIntf::Ptr, internalAttrsAccess)() const = 0;
  ES_DECL_INTF_METHOD(void, attrsAssign)(const EsAttributesIntf::Ptr& attrs) = 0;

public:
  ES_DECL_INTF_METHOD(bool, hasAttribute)(cr_EsString name) const  = 0;
  ES_DECL_INTF_METHOD(EsString::Array, attributeNamesGet)() const = 0;
  ES_DECL_INTF_METHOD(EsVariant, attributeGet)(cr_EsString name) const = 0;
  ES_DECL_INTF_METHOD(const EsString&, nameGet)() const = 0;
  ES_DECL_INTF_METHOD(bool, isPersistent)() const = 0;
  ES_DECL_INTF_METHOD(const EsString&, readerNameGet)(bool doThrow = true) const = 0;
  ES_DECL_INTF_METHOD(const EsString&, writerNameGet)(bool doThrow = true) const = 0;

  friend class EsScriptObject;
  friend class EsScriptCompiledBinaryWriter;
  friend class EsScriptCompiledBinaryReader;
ES_DECL_INTF_END

// property info intf implementation
class ES_INTF_IMPL1(EsScriptObjectPropertyInfo, EsScriptObjectPropertyInfoIntf)

protected:
  EsScriptObjectPropertyInfo(const EsString& name, const EsAttributesIntf::Ptr& attrs, EsScriptObjectIntf* parent,
    const EsString& readerName, const EsString& writerName);

public:
  // BaseIntf neccessary implementation
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return classNameGetStatic(); }

  // EsScriptObjectPropertyInfoIntf implementation
protected:  
  // assign instance attributes pointer
  ES_DECL_INTF_METHOD(EsAttributesIntf::Ptr, internalAttrsAccess)() const ES_OVERRIDE { return m_attrs; }
  ES_DECL_INTF_METHOD(void, attrsAssign)(const EsAttributesIntf::Ptr& attrs) ES_OVERRIDE { m_attrs = attrs; }

public:
  ES_DECL_INTF_METHOD(bool, hasAttribute)(cr_EsString name) const ES_OVERRIDE { return m_attrs && m_attrs->attributeExists(name); }
  ES_DECL_INTF_METHOD(EsString::Array, attributeNamesGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(EsVariant, attributeGet)(cr_EsString name) const ES_OVERRIDE;

  ES_DECL_INTF_METHOD(const EsString&, nameGet)() const ES_OVERRIDE { return m_name; }
  ES_DECL_INTF_METHOD(bool, isPersistent)() const ES_OVERRIDE
  { 
    return hasAttribute(esT("persistent")) && 
      !m_readerName.empty() && 
      !m_writerName.empty(); 
  }
  ES_DECL_INTF_METHOD(const EsString&, readerNameGet)(bool doThrow = true) const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(const EsString&, writerNameGet)(bool doThrow = true) const ES_OVERRIDE;

  // info intf object creator
  static EsScriptObjectPropertyInfoIntf::Ptr create(
    const EsString& name, 
    const EsAttributesIntf::Ptr& attrs, 
    EsScriptObjectIntf* parent,
    const EsString& readerName, 
    const EsString& writerName
  )
  {
    EsScriptObjectPropertyInfoIntf::Ptr result(  new EsScriptObjectPropertyInfo(name, attrs, parent,  readerName, writerName) );
    ES_ASSERT(result);
    return result;
  }
  
protected:
  // class attributes - shared among all instances of this class
  EsAttributesIntf::Ptr m_attrs;
  EsString m_name;
  // parent object pointer. parent is the object which holds this property
  EsScriptObjectIntf* m_parent;
  EsString m_readerName;
  EsString m_writerName;
  
private:
  EsScriptObjectPropertyInfo() ES_REMOVEDECL;
  EsScriptObjectPropertyInfo(const EsScriptObjectPropertyInfo&) ES_REMOVEDECL;
  EsScriptObjectPropertyInfo& operator=(const EsScriptObjectPropertyInfo&) ES_REMOVEDECL;
};

#endif