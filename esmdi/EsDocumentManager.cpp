#include "esmdipch.h"
#pragma hdrstop

#include "EsDocumentManager.h"
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

// Reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsDocumentManager, NO_CLASS_DESCR)
  // EsDocumentManagerIntf
  //
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, documentRegister, void_Call_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, documentIsRegistered, EsVariant_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, documentIsSingleton, EsVariant_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, documentDefaultExtGet, EsString_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, registeredDocumentTypesGet, EsStringArray_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, documentRegistrationPayloadGet, EsVariant_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, registeredDocumentCountGet, ulong_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, documentCreate, EsVariant_Call_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, documentCreate, EsVariant_Call_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, documentOpen, EsVariant_Call_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, documentsGet, EsVariant_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, documentsCountGet, ulong_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, allDocumentKeysGet, EsVariant_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, documentFind, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, documentFindFirstByType, EsVariant_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, documentTypeFindByFileExt, EsString_CallConst_cr_EsString, NO_METHOD_DESCR) 
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, documentClose, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, documentsCloseAll, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocumentManager, EsDocumentManagerIntf, documentsCloseAllRequest, void_Call, NO_METHOD_DESCR)
  // Reflected services
  //
  ES_DECL_REFLECTED_CTOR_INFO(        EsDocumentManager, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsDocumentManager, registerInReflected, registerIn, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsDocumentManager, documentRegisterIn, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsDocumentManager, singletonDocumentRegisterIn, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsDocumentManager, complexMainTypeGet, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsDocumentManager, complexSubTypeGet, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsDocumentManager, isComplexType, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//--------------------------------------------------------------------------------

EsDocumentManager::EsDocumentManager() :
m_reg(
  EsStringIndexedMap::ContainerWithoutInterlock,
  esT("EsDocumentManager")
),
m_docs(
  nullptr
)
{
  m_dynamic = true;
}
//--------------------------------------------------------------------------------

EsBaseIntfPtr EsDocumentManager::NEW()
{
  std::unique_ptr<EsDocumentManager> ptr = ES_MAKE_UNIQUE( EsDocumentManager );
  ES_ASSERT(ptr);
  
  return ptr.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------

void EsDocumentManager::documentRegister(cr_EsString type, cr_EsVariant params)
{
  if(m_reg.itemExists(type))
    EsException::Throw(
      esT("Document of type '%s' is already registered"),
      type
    );

  documentTypeCheck(type);

  m_reg.itemAdd(
    type,
    params
  );
}
//--------------------------------------------------------------------------------

EsAssocContainerIntf::Ptr EsDocumentManager::regInfoGetForType(const EsString& type) const
{
  if(!m_reg.itemExists(type))
    EsException::Throw(
      esT("Document of type '%s' is not registered"),
      type
    );

  EsAssocContainerIntf::Ptr regVals = m_reg.valueGet(type).asObject();
  return regVals;
}
//--------------------------------------------------------------------------------

EsVariant EsDocumentManager::documentIsSingleton(cr_EsString type) const
{
  EsAssocContainerIntf::Ptr regVals = regInfoGetForType(type);
  ES_ASSERT(regVals);

  return regVals->keyExists(esVT("singleton")) && 
    regVals->valueGet(esVT("singleton")).asBool();
}
//--------------------------------------------------------------------------------

EsString EsDocumentManager::documentDefaultExtGet(cr_EsString type) const
{
  const EsVariant& regVals = regInfoGetForType(type);
  ES_ASSERT(!regVals.isEmpty());

  return regVals[esVT("defaultExt")][1].asString();
}
//--------------------------------------------------------------------------------

bool EsDocumentManager::documentOfTypeExists(const EsString& type) const
{
  return !documentFindFirstByType(type).isEmpty();
}
//--------------------------------------------------------------------------------

EsVariant EsDocumentManager::documentFindFirstByType(cr_EsString type) const ES_NOTHROW
{
  const EsVariant::Array& vals = m_docs.allValuesGet().asVariantCollection();
  for(auto const& val : vals )
  {
    const EsVariant& doc = val[0];
    if(doc.isKindOf(type))
      return doc;
  }

  return EsVariant::nullObject();
}
//--------------------------------------------------------------------------------

EsString EsDocumentManager::documentTypeFindByFileExt(cr_EsString ext) const
{
  EsString lookup = esT("*.") + ext;
  for( ulong idx = 0; idx < m_reg.countGet(); ++idx )
  {
    const EsVariant& item = m_reg.valueGet(idx);
    ES_ASSERT(!item.isEmpty());

    const EsString& filterStr = item[esVT("filterStr")].asString();
    
    if( EsString::npos != filterStr.find(lookup) )
      return m_reg.nameGet(idx);
  }

  return EsString::null();
}
//--------------------------------------------------------------------------------

EsVariant EsDocumentManager::documentRegistrationPayloadGet(cr_EsString type) const
{
  return m_reg.valueGet(type);
}
//--------------------------------------------------------------------------------

EsVariant EsDocumentManager::documentCreate(cr_EsString type, cr_EsVariant params)
{
  documentTypeCheck(type);

  // Get registry entry for type
  EsAssocContainerIntf::Ptr reg = regInfoGetForType(type);
  ES_ASSERT(reg);

  if(
    reg &&
    reg->keyExists(esVT("singleton")) &&
    documentOfTypeExists(type)
  )
    EsException::Throw(
      _("Singleton document instance of type '%s' already exists.\nOnly one instance of singleton document type is allowed."),
      type
    );


  // Create document using reflection services
  const EsVariant& newDoc = EsMetaclass::instance()->objectCreate(
    complexMainTypeGet(type),
    reg
  );

  // Key must be the first (or the only one) parameter
  EsVariant key;
  if(params.isCollection())
  {
    ES_ASSERT(params.countGet() > 0);
    key = params[0];
  }
  else
    key = params;

  // Create new document type key using type and optional registry parameters
  if( !key.isEmpty() )
    documentKeyCheck(key);

  const EsVariant& newKey = !key.isEmpty() ? 
    key : 
    newDocumentKeyCreate(
      type,
      reg
    );

  // Attach document object with new, or existing key
  EsDocument* docptr = ES_VARIANT_TO_OBJECTPTR<EsDocument>(newDoc);
  ES_ASSERT(docptr);

  docptr->keySet(newKey);

  documentAttach(
    newDoc,
    newKey,
    params.isCollection() ?
      params :
      EsVariant::null()
  );

  return newDoc;
}
//--------------------------------------------------------------------------------

EsVariant EsDocumentManager::documentCreate(cr_EsString type)
{
  return documentCreate(type, EsVariant::null());
}
//--------------------------------------------------------------------------------

EsVariant EsDocumentManager::documentOpen(cr_EsString type, cr_EsVariant params)
{
  if(params.isEmpty())
    return documentCreate(type);

  if(!type.empty())
    documentTypeCheck(type);

  // Key must be the first (or the only one) parameter
  EsVariant key;
  if(params.isCollection())
  {
    ES_ASSERT(params.countGet() > 0);
    key = params[0];
  }
  else
    key = params;

  documentKeyCheck(key);

  // Open only one singleton document, if another one is open-close it first
  if( documentIsSingleton(type).asBool() )
  {
    EsDocumentIntf::Ptr singleDoc = documentFindFirstByType(type).asObject();
    if( singleDoc )
      documentClose(
        singleDoc->keyGet()
      );
  }

  // Find if document with such type (if specified) and key already open
  // If type is specified, key found, but document is of other type - throw an error
  EsVariant doc = documentFind(key);
  if(!doc.isEmpty() && !type.empty() && !doc.isKindOf(type))
    EsException::Throw(
      esT("documentOpen - incompatible document type '%s' requested, open document is of type '%s'"),
      type,
      doc.typeNameGet()
    );

  // Document not found - open the new one
  if(doc.isEmpty())
  {
    // Get registry entry for type
    const EsVariant& regParams = m_reg.valueGet(type);

    // Create document using reflection services
    doc = EsMetaclass::instance()->objectCreate(
      complexMainTypeGet(type),
      regParams
    );
    EsDocument* docptr = ES_VARIANT_TO_OBJECTPTR<EsDocument>(doc);
    ES_ASSERT(docptr);

    docptr->keySet(key);

    documentReadAndAttach(
      doc,
      key,
      params
    );
  }
  else
  {
    const EsVariant::Array& payload = {
      key,
      params
    };

    EsEventDispatcher::eventPost(
      ES_EVTC_MDI,
      as_<ulong>(EsMdiEvt::DocumentOpened),
      payload
    );
  }

  return doc;
}
//--------------------------------------------------------------------------------

EsVariant EsDocumentManager::documentFind(cr_EsVariant key) const ES_NOTHROW
{
  const EsVariant& docWithParams = m_docs.valueGetDef(
    key, 
    EsVariant::null() 
  );

  if( !docWithParams.isEmpty() )
    return docWithParams[0];

  return EsVariant::null();
}
//--------------------------------------------------------------------------------

EsVariant EsDocumentManager::documentsGet(cr_EsString type) const
{
  if( !type.empty() )
    documentTypeCheck(type);

  EsVariant::Array result;
  const EsVariant::Array& docsWithParameters = m_docs.allValuesGet().asVariantCollection();
  result.reserve( docsWithParameters.size() );

  for(auto const& docWithParam : docsWithParameters )
  {
    if( type.empty() || docWithParam[0].isKindOf(type))
      result.push_back( 
        docWithParam[0] 
      );
  }

  return result;
}
//--------------------------------------------------------------------------------

void EsDocumentManager::documentClose(cr_EsVariant key)
{
  documentKeyCheck(key);

  const EsVariant& doc = documentFind(key);
  ES_ASSERT(!doc.isEmpty());

  if(
    doc.hasProperty(esT("canClose")) &&
    !doc.propertyGet(esT("canClose")).asBool()
  )
  {
    if( doc.hasProperty(esT("cannotCloseReason")) )
      EsException::Throw(
        doc.propertyGet(esT("cannotCloseReason")).asString()
      );
    else
      EsException::Throw(
        _("Document %s may not be closed."),
        key.asString()
      );
  }

  const EsVariant::Array& payload = {
    key,
    doc.typeNameGet()
  };

  EsDocument* docptr = ES_VARIANT_TO_OBJECTPTR<EsDocument>(doc);
  ES_ASSERT(docptr);

  docptr->managerSet( nullptr );
  m_docs.itemDelete( key );

  // Post closed event
  EsEventDispatcher::eventPost(
    ES_EVTC_MDI,
    as_<ulong>(EsMdiEvt::DocumentClosed),
    payload
  );
}
//--------------------------------------------------------------------------------

void EsDocumentManager::documentsCloseAll()
{
  const EsVariant::Array& keys = m_docs.allKeysGet().asVariantCollection();
  for( auto const& key : keys )
    documentClose(key);

  // Post closed all event
  EsEventDispatcher::eventPost(
    ES_EVTC_MDI,
    as_<ulong>(EsMdiEvt::AllDocumentsClosed)
  );
}
//--------------------------------------------------------------------------------

void EsDocumentManager::documentsCloseAllRequest()
{
  const EsVariant::Array& keys = m_docs.allKeysGet().asVariantCollection();
  if( keys.empty() )
    return;

  // Post close all request
  EsEventDispatcher::eventPost(
    ES_EVTC_MDI,
    as_<ulong>(EsMdiEvt::DocumentsCloseRequest),
    keys
  );
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

void EsDocumentManager::keyChange(cr_EsVariant oldKey, cr_EsVariant newKey)
{
  m_docs.keyChange(
    oldKey,
    newKey
  );

  // Post key changed event
  const EsVariant::Array& payload = {
    oldKey,
    newKey
  };

  EsEventDispatcher::eventPost(
    ES_EVTC_MDI,
    as_<ulong>(EsMdiEvt::DocumentKeyChanged),
    payload
  );
}
//--------------------------------------------------------------------------------

ulong EsDocumentManager::newDocumentCounterGet(const EsString& type)
{
  auto it = m_newDocCounter.find(type);
  if(it != m_newDocCounter.end())
    return (*it).second++;
  else
    m_newDocCounter[type] = 1;

  return 0;
}
//--------------------------------------------------------------------------------

EsVariant EsDocumentManager::newDocumentKeyCreate(const EsString& type, const EsVariant& regParams)
{
  ES_ASSERT(!type.empty());
  ES_ASSERT(!regParams.isEmpty());

  ulong cnt = newDocumentCounterGet(type);

  const EsString& shortName = regParams[esVT("shortDescr")][1].asString();
  const EsString& ext = regParams[esVT("defaultExt")][1].asString();

  return EsString::format(
    esT("New %s %d.%s"),
    shortName,
    cnt,
    ext
  );
}
//--------------------------------------------------------------------------------

void EsDocumentManager::documentAttach(const EsVariant& doc, cr_EsVariant key, cr_EsVariant& params)
{
  ES_ASSERT(!doc.isEmpty());
  ES_ASSERT(!key.isEmpty());

  EsDocument* docptr = ES_VARIANT_TO_OBJECTPTR<EsDocument>(doc);
  ES_ASSERT(docptr);

  ES_DEBUG_TRACE(
    esT("EsDocumentManager::documentAttach key='%s', params:\n%s"),
    key,
    EsVariant::dump(params)
  );

  if( docptr->isNew() )
    docptr->emptyContentsInit(params);
  docptr->managerSet( asWeakReference() );

  // Add doc to the docs container
  const EsVariant::Array& docWithParams = {
    doc,
    params
  };

  m_docs.newValueSet(
    key,
    docWithParams
  );

  const EsVariant::Array& payload = {
    key,
    params
  };

  // Post new opened event
  EsEventDispatcher::eventPost(
    ES_EVTC_MDI,
    as_<ulong>(EsMdiEvt::DocumentCreated),
    payload
  );
}
//--------------------------------------------------------------------------------

void EsDocumentManager::documentReadAndAttach(const EsVariant& doc, cr_EsVariant key, cr_EsVariant& params)
{
  ES_ASSERT(!doc.isEmpty());
  ES_ASSERT(!key.isEmpty());
  
  // Read document, using key as a source, with optional params
  EsDocument* docptr = ES_VARIANT_TO_OBJECTPTR<EsDocument>(doc);
  ES_ASSERT(docptr);

  // Some internal document data structure may require one-time initialization 
  // if we're opening into an empty or newly created document
  //
  if( 
    docptr->isEmpty() ||  
    docptr->isNew()
  )
    docptr->emptyContentsInit(params);

  docptr->read(params);

  documentAttach(
    doc,
    key,
    params
  );
}
//--------------------------------------------------------------------------------

EsVariant EsDocumentManager::registerInReflected(
  const EsVariant& mgr,
  const EsVariant& docType,
  const EsVariant& defaultExt,
  const EsVariant& shortDescr,
  const EsVariant& filterStr,
  const EsVariant& isSingleton
)
{
  return registerIn(
    mgr.asExistingObject(),
    docType.asString(),
    defaultExt.asString(),
    shortDescr.asString(),
    filterStr.asString(),
    isSingleton.asBool()
  );
}
//--------------------------------------------------------------------------------

EsVariant EsDocumentManager::documentRegisterIn(
  const EsVariant& mgr,
  const EsVariant& docType,
  const EsVariant& defaultExt,
  const EsVariant& shortDescr,
  const EsVariant& filterStr
)
{
  return registerIn(
    mgr.asExistingObject(),
    docType.asString(),
    defaultExt.asString(),
    shortDescr.asString(),
    filterStr.asString()
  );
}
//--------------------------------------------------------------------------------

EsVariant EsDocumentManager::singletonDocumentRegisterIn(
  const EsVariant& mgr,
  const EsVariant& docType,
  const EsVariant& defaultExt,
  const EsVariant& shortDescr,
  const EsVariant& filterStr
)
{
  return registerIn(
    mgr.asExistingObject(),
    docType.asString(),
    defaultExt.asString(),
    shortDescr.asString(),
    filterStr.asString(),
    true
  );
}
//--------------------------------------------------------------------------------

EsAssocContainerIntf::Ptr EsDocumentManager::registerIn(
  const EsDocumentManagerIntf::Ptr& mgr,
  const EsString& docType,
  const EsString& defaultExt,
  const EsString& shortDescr,
  const EsString& filterStr,
  bool isSingleton /*= false*/
)
{
  ES_ASSERT(mgr);
  
  EsAssocContainerIntf::Ptr regInfo = EsAssocContainer::create();
  ES_ASSERT(regInfo);

  regInfo->newValueSet(
    esVT("type"),
    docType
  );

  regInfo->newValueSet(
    esVT("defaultExt"),
    defaultExt
  );
  regInfo->newValueSet(
    esVT("shortDescr"),
    shortDescr
  );
  regInfo->newValueSet(
    esVT("filterStr"),
    filterStr
  );

  if( isSingleton )
    regInfo->newValueSet(
      esVT("singleton"),
      true
    );

  mgr->documentRegister(
    docType,
    regInfo
  );

  return regInfo;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

void EsDocumentManager::documentTypeCheck(const EsString& type)
{
  if(type.empty())
    EsException::Throw(
      esT("Empty document type is not allowed")
    );

  const EsClassInfo* info = EsClassInfo::classInfoGet( complexMainTypeGet(type) );
  if(nullptr == info)
    EsException::Throw(
      esT("Document type '%s' is unknown"),
      type
    );

  if(!info->isKindOf(esT("EsDocument")))
    EsException::Throw(
      esT("Document type '%s' must derive from EsDocument base"),
      type
    );
}
//--------------------------------------------------------------------------------

void EsDocumentManager::documentKeyCheck(const EsVariant& key)
{
  if(key.isEmpty())
    EsException::Throw(
      esT("Document key may not be an empty value")
    );
}
//--------------------------------------------------------------------------------

#define ES_COMPLEX_TYPE_SEP esT(':')

EsString EsDocumentManager::complexMainTypeGet(const EsString& cpxType)
{
  size_t pos = cpxType.find(ES_COMPLEX_TYPE_SEP);
  if(EsString::npos != pos)
    return cpxType.substr(0, pos);
  
  return cpxType;
}
//--------------------------------------------------------------------------------

EsString EsDocumentManager::complexSubTypeGet(const EsString& cpxType)
{
  size_t pos = cpxType.find(ES_COMPLEX_TYPE_SEP);
  if(EsString::npos != pos)
    return cpxType.substr(pos+1);

  return EsString::null();
}
//--------------------------------------------------------------------------------

bool EsDocumentManager::isComplexType(const EsString& cpxType)
{
  size_t pos = cpxType.find(ES_COMPLEX_TYPE_SEP);
  return EsString::npos != pos && pos > 0;
}
//--------------------------------------------------------------------------------

EsVariant EsDocumentManager::complexMainTypeGet(const EsVariant& cpxType)
{
  return complexMainTypeGet(
    cpxType.asString()
  );
}
//--------------------------------------------------------------------------------

EsVariant EsDocumentManager::complexSubTypeGet(const EsVariant& cpxType)
{
  return complexSubTypeGet(cpxType.asString());
}
//--------------------------------------------------------------------------------

EsVariant EsDocumentManager::isComplexType(const EsVariant& cpxType)
{
  return EsVariant(
    isComplexType(cpxType.asString())
  );
}
//--------------------------------------------------------------------------------
