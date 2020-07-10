#ifndef _es_document_manager_h_
#define _es_document_manager_h_

/// @file EsDocumentManager.h
/// Reflected implementation of document manager interface.
///

/// Default implementation of document manager interface
///
class ESMDI_CLASS ES_INTF_IMPL2(EsDocumentManager, EsDocumentManagerIntf, EsReflectedClassIntf)

public:
  ES_DECL_REFLECTED_CLASS_BASE(EsDocumentManager)
  ES_DECL_ATTR_HANDLING_STD

  /// Reflected document manager creation
  ///
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

  /// EsDocumentManagerIntf implementation
  ///
  ES_DECL_REFLECTED_INTF_METHOD2(void, documentRegister, cr_EsString, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, documentIsRegistered, cr_EsString) ES_NOTHROW ES_OVERRIDE { return m_reg.itemExists(p1); }
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, documentIsSingleton, cr_EsString) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsString, documentDefaultExtGet, cr_EsString) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsStringArray, registeredDocumentTypesGet) ES_NOTHROW ES_OVERRIDE { return m_reg.namesGet(); }
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, documentRegistrationPayloadGet, cr_EsString) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(ulong, registeredDocumentCountGet) ES_OVERRIDE { return m_reg.countGet(); }
  ES_DECL_REFLECTED_INTF_METHOD1(EsVariant, documentCreate, cr_EsString) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(EsVariant, documentCreate, cr_EsString, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(EsVariant, documentOpen, cr_EsString, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, documentsGet,cr_EsString) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(ulong, documentsCountGet) ES_OVERRIDE { return m_docs.countGet(); }
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsVariant, allDocumentKeysGet) ES_OVERRIDE { return m_docs.allKeysGet(); }
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, documentFind, cr_EsVariant) ES_NOTHROW ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, documentFindFirstByType, cr_EsString) ES_NOTHROW ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsString, documentTypeFindByFileExt, cr_EsString) ES_OVERRIDE;

  ES_DECL_REFLECTED_INTF_METHOD1(void, documentClose, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, documentsCloseAll) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, documentsCloseAllRequest) ES_OVERRIDE;

  // Reflected alias to registerIn
  ES_DECL_REFLECTED_CLASS_METHOD6(EsVariant, registerInReflected, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD5(EsVariant, documentRegisterIn, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD5(EsVariant, singletonDocumentRegisterIn, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);

  // Complex type parsing and qualification
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, complexMainTypeGet, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, complexSubTypeGet, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, isComplexType, cr_EsVariant);

  /// Non-reflected services
  ///

  /// Complex type string helpers
  static EsString complexMainTypeGet(const EsString& cpxType);
  static EsString complexSubTypeGet(const EsString& cpxType);
  static bool isComplexType(const EsString& cpxType);

  /// Document registry helper.
  /// Creates default minimal document registration entry, and return 
  /// registry parameter object to allow additional information to be added 
  /// afterwards
  ///
  static EsAssocContainerIntf::Ptr registerIn(
    const EsDocumentManagerIntf::Ptr& mgr, 
    const EsString& docType,
    const EsString& defaultExt,
    const EsString& shortDescr,
    const EsString& filterStr,
    bool isSingleton = false
  );

  /// Semi-private services
  ///
  void documentKeyCheck(const EsVariant& key);
  void keyChange(cr_EsVariant oldKey, cr_EsVariant newKey);

protected:
  EsAssocContainerIntf::Ptr regInfoGetForType(const EsString& type) const;
  ulong newDocumentCounterGet(const EsString& type);
  EsVariant newDocumentKeyCreate(const EsString& type, const EsVariant& regParams);
  void documentAttach(const EsVariant& doc, cr_EsVariant key, cr_EsVariant params);
  void documentReadAndAttach(const EsVariant& doc, cr_EsVariant key, cr_EsVariant params);
  bool documentOfTypeExists(const EsString& type) const;

  static void documentTypeCheck(const EsString& type);

protected:
  EsStringIndexedMap m_reg;
  EsAssocContainer m_docs;
  std::map<EsString, ulong> m_newDocCounter;

private:
  EsDocumentManager();
  ES_NON_COPYABLE(EsDocumentManager);
};
//---------------------------------------------------------------------------------------

#endif //< _es_document_manager_h_

