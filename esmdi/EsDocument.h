#ifndef _es_document_h_
#define _es_document_h_

/// @file EsDocument.h
/// Minimal reflected implementation for building MDI-aware frameworks
///

/// Common base implementation for EsDocumentIntf 
///
class ESMDI_CLASS ES_INTF_IMPL2(EsDocument, EsDocumentIntf, EsReflectedClassIntf)

protected:
  EsDocument(const EsAssocContainerIntf::Ptr& regInfo);

public:
  ES_DECL_REFLECTED_CLASS_BASE(EsDocument)
  ES_DECL_ATTR_HANDLING_STD

  /// EsDocumentIntf reflected implementation
  ///
  
  /// Return true if document is managed (attached to manager)
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isManaged) ES_NOTHROW ES_OVERRIDE { return m_mgr; }

  /// Document emptiness check
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isEmpty) ES_NOTHROW ES_OVERRIDE { return m_vals.isEmpty(); }

  /// New document check (never saved)
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isNew) ES_NOTHROW ES_OVERRIDE { return m_new; }
  
  /// Document have unsaved changes
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isModified) ES_NOTHROW ES_OVERRIDE { return m_modified; }

  /// Document identification
  ///

  /// Complex type stuff
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsString, complexTypeGet) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsString, complexSubTypeGet) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isComplexDocumentType) ES_OVERRIDE;

  /// Document key access - unique key for unambiguous document identification 
  /// in document manager collection
  ///
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(cr_EsVariant, keyGet) ES_NOTHROW ES_OVERRIDE { return m_key; }

  /// Get copy of document's registration info
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsVariant, regInfoGet) ES_OVERRIDE;

  /// Document properties access
  ///

  /// Return true if document property with 'name' exists, false otherwise
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, valueExists, cr_EsString) ES_NOTHROW ES_OVERRIDE { return m_vals.itemExists(p1); }

  /// Return all assigned property names
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsStringArray, valueNamesGet) ES_NOTHROW ES_OVERRIDE { return m_vals.namesGet(); }

  /// Return named property value. Throw, if value does not exist.
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(cr_EsVariant, valueGet, cr_EsString) ES_OVERRIDE { return m_vals.valueGet(p1); }

  /// Assign named property value. If value with such name does not exist in document - create it
  ES_DECL_REFLECTED_INTF_METHOD2(void, valueSet, cr_EsString, cr_EsVariant) ES_OVERRIDE;

  /// Remove value from the document
  ES_DECL_REFLECTED_INTF_METHOD1(void, valueDelete, cr_EsString) ES_OVERRIDE;

  /// Document manager object access
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsBaseIntfPtr, managerGet) ES_NOTHROW ES_OVERRIDE { return m_mgr; }

  /// Document serialization basic implementation
  ES_DECL_REFLECTED_INTF_METHOD1(void, read, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(void, write, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(void, writeCopy, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;

  /// Document change notification
  ES_DECL_INTF_METHOD(void, documentChangedNotify)(const EsString& subject, void* sender) const ES_OVERRIDE;

  /// Document sideload change notification
  ES_DECL_INTF_METHOD(void, documentSideloadChangedNotify)(const EsString& subject, void* sender) const ES_OVERRIDE;

  /// Reflected properties
  ///
  ES_DECL_PROPERTY_RO(manager, EsVariant);
  ES_DECL_PROPERTY_RO(key, EsVariant);
  ES_DECL_PROPERTY_RO(regInfo, EsVariant);
  ES_DECL_PROPERTY_RO(complexType, EsVariant);
  ES_DECL_PROPERTY_RO(complexSubType, EsVariant);
  ES_DECL_PROPERTY_RO(valueNames, EsVariant);
  ES_DECL_PROPERTY_RO(isEmpty, bool);
  ES_DECL_PROPERTY_RO(isNew, bool);
  ES_DECL_PROPERTY_RO(isModified, bool);
  ES_DECL_PROPERTY_RO(isComplexType, bool);
    
  // Non-reflected services
  //
  virtual bool doRead(cr_EsVariant params) = 0;
  virtual void doWrite(cr_EsVariant key, cr_EsVariant params, bool asCopy) = 0;

  void keySet(const EsVariant& key);
  void managerSet(const EsDocumentManagerIntf::Ptr& mgr);

protected:
  void sideloadFileDelete() ES_NOTHROW;
  virtual bool isSideloadName(const EsString& name) const = 0;

protected:
  // Manager weak reference
  EsDocumentManagerIntf::Ptr m_mgr;

  // Registration info reference for this document
  EsAssocContainerIntf::Ptr m_regInfo;

  // Document key
  EsVariant m_key;

  // Document named values container
  EsStringIndexedMap m_vals;

  // Document flags
  bool m_new;
  bool m_modified;

  ES_NO_DEFAULT_CTR(EsDocument);
  ES_NON_COPYABLE(EsDocument);
};
//---------------------------------------------------------------------------------------

#endif // _es_document_h_
