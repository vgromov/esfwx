#include "esmdipch.h"
#pragma hdrstop

#include "EsDocument.h"
//--------------------------------------------------------------------------------

// Reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsDocument, NO_CLASS_DESCR)
  // Services
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, isManaged, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, isEmpty, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, isNew, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, isModified, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, keyGet, cr_EsVariant_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, regInfoGet, EsVariant_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, complexTypeGet, EsString_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, complexSubTypeGet, EsString_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, isComplexDocumentType, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, valueExists, bool_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, valueNamesGet, EsStringArray_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, valueGet, cr_EsVariant_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, valueSet, void_Call_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, valueDelete, void_Call_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, managerGet, EsBaseIntfPtr_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, read, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, write, void_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsDocument, EsDocumentIntf, writeCopy, void_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  // Properties
  ES_DECL_PROP_INFO_RO(               EsDocument, manager,        EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsDocument, key,            EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsDocument, regInfo,        EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsDocument, complexType,    EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsDocument, complexSubType, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsDocument, valueNames,     EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsDocument, isEmpty,        bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsDocument, isNew,          bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsDocument, isModified,     bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsDocument, isComplexType,  bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsDocument::EsDocument(const EsAssocContainerIntf::Ptr& regInfo) :
m_regInfo(regInfo),
m_vals(
  EsStringIndexedMap::ContainerWithoutInterlock,
  esT("EsDocument")
),
m_new(true),
m_modified(false)
{
  m_dynamic = true; //< Allow only dunamic creation, through derived classes
}
//--------------------------------------------------------------------------------

EsString EsDocument::complexTypeGet() const
{
  ES_ASSERT(m_regInfo);
  return m_regInfo->valueGet(esVT("type")).asString();
}
//--------------------------------------------------------------------------------

EsString EsDocument::complexSubTypeGet() const
{
  const EsString& cpxType = complexTypeGet();

  return EsDocumentManager::complexSubTypeGet(cpxType);
}
//--------------------------------------------------------------------------------

bool EsDocument::isComplexDocumentType() const
{
  const EsString& cpxType = complexTypeGet();

  return EsDocumentManager::isComplexType(cpxType);
}
//--------------------------------------------------------------------------------

EsVariant EsDocument::regInfoGet() const
{ 
  ES_ASSERT(m_regInfo); 

  EsVariant regInfo = m_regInfo;
  return regInfo.clone();
}
//--------------------------------------------------------------------------------

void EsDocument::keySet(const EsVariant& key)
{
  if(m_key != key)
  {
    if(isManaged())
    {
      EsDocumentManager* mgr = ES_INTFPTR_TO_OBJECTPTR<EsDocumentManager>(m_mgr);
      ES_ASSERT(mgr);

      mgr->documentKeyCheck(key);

      EsVariant oldKey = m_key;
      m_key = key;

      mgr->keyChange(
        oldKey,
        m_key
      );
    }
    else
      m_key = key;
  }
}
//--------------------------------------------------------------------------------

void EsDocument::managerSet(const EsDocumentManagerIntf::Ptr& mgr)
{
  if(mgr)
    m_mgr.reset(
      mgr.get(),
      false
    );
  else
    m_mgr.reset();
}
//--------------------------------------------------------------------------------

void EsDocument::valueSet(cr_EsString name, cr_EsVariant val)
{
  if( !isSideloadName(name) )
  {
    if( m_vals.itemExists(name) )
    {
      const EsVariant& oldval = m_vals.valueGet(name);
      m_modified = oldval != val;
    }
    else
      m_modified = true;
  }

  m_vals.itemAdd(
    name,
    val,
    false
  );
}
//--------------------------------------------------------------------------------

void EsDocument::valueDelete(cr_EsString name)
{
  m_vals.itemDelete(name);
  m_modified = true;
}
//--------------------------------------------------------------------------------

void EsDocument::read(cr_EsVariant& params)
{
  if( doRead(params) )
  {
    m_modified = false;
    m_new = false;

    sideloadRead();

    if( isManaged() )
    {
      documentChangedNotify(
        EsString::null(),
        nullptr
      );

      documentSideloadChangedNotify(
        EsString::null(),
        nullptr
      );
    }
  }
}
//--------------------------------------------------------------------------------

void EsDocument::write(cr_EsVariant& key, cr_EsVariant& params)
{
  ES_ASSERT(isManaged());

  EsDocumentManager* mgr = ES_INTFPTR_TO_OBJECTPTR<EsDocumentManager>(m_mgr);
  ES_ASSERT(mgr);

  bool keyChange = keyGet() != key;
  if(keyChange)
    mgr->documentKeyCheck(key);
  
  bool stateChange = false;
  if(isModified() || isNew())
  {
    doWrite(
      key, 
      params,
      false
    );

    m_modified = false;
    m_new = false;

    stateChange = true;
  }

  if(keyChange)
  {
    sideloadFileDelete(); //< Remove sideload for an old key

    keySet(key);
  }

  if(stateChange)
    EsEventDispatcher::eventPost(
      ES_EVTC_MDI,
      as_<ulong>(EsMdiEvt::DocumentStateChanged),
      m_key
    );
      
  ES_ASSERT(!isNew());
  sideloadWrite();
}
//--------------------------------------------------------------------------------

void EsDocument::writeCopy(cr_EsVariant& target, cr_EsVariant& params)
{
  ES_ASSERT(isManaged());

  EsDocumentManager* mgr = ES_INTFPTR_TO_OBJECTPTR<EsDocumentManager>(m_mgr);
  ES_ASSERT(mgr);

  if(keyGet() == target)
    EsException::Throw(
      EsException::codeNone,
      EsException::facilityEsMDI,
      EsException::severityGeneric,
      _("Could not write document copy to the same target %s, as an original"),
      target
    );

  // Do not touch existing contents or sideload, when writing document's copy
  doWrite(
    target, 
    params,
    true
  );
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsVariant EsDocument::get_manager() const
{
  return m_mgr;
}

EsVariant EsDocument::get_key() const
{
  return m_key;
}
//--------------------------------------------------------------------------------

EsVariant EsDocument::get_regInfo() const
{
  return m_regInfo;
}
//--------------------------------------------------------------------------------

EsVariant EsDocument::get_complexType() const
{
  return complexTypeGet();
}
//--------------------------------------------------------------------------------

EsVariant EsDocument::get_complexSubType() const
{
  return complexSubTypeGet();
}
//--------------------------------------------------------------------------------

EsVariant EsDocument::get_valueNames() const
{
  return valueNamesGet();
}
//--------------------------------------------------------------------------------

bool EsDocument::get_isEmpty() const
{
  return isEmpty();
}
//--------------------------------------------------------------------------------

bool EsDocument::get_isNew() const
{
  return isNew();
}
//--------------------------------------------------------------------------------

bool EsDocument::get_isModified() const
{
  return isModified();
}
//--------------------------------------------------------------------------------

bool EsDocument::get_isComplexType() const
{
  return isComplexDocumentType();
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

void EsDocument::sideloadFileDelete() ES_NOTHROW
{
  const EsString& sideloadPathStr = m_key.asString() + esT(".sl");
  if(EsPath::fileExists(sideloadPathStr, EsString::null()))
  {
    try
    {
      EsPath::fileRemove(sideloadPathStr);
    }
    catch(...)
    {
    }
  }
}
//--------------------------------------------------------------------------------

void EsDocument::documentChangedNotify(const EsString& subject, void* sender) const
{
  if(!isManaged())
    return;

  // Post document changed event with existing key
  const EsVariant::Array& payload = {
    m_key,
    subject,
    EsVariant(sender, EsVariant::ACCEPT_POINTER)
  };

  EsEventDispatcher::eventPost(
    ES_EVTC_MDI,
    as_<ulong>(EsMdiEvt::DocumentChanged),
    payload
  );
}
//--------------------------------------------------------------------------------

void EsDocument::documentSideloadChangedNotify(const EsString& subject, void* sender) const
{
  if(!isManaged())
    return;

  // Post document changed event with existing key
  const EsVariant::Array& payload = {
    m_key,
    subject,
    EsVariant(sender, EsVariant::ACCEPT_POINTER)
  };

  EsEventDispatcher::eventPost(
    ES_EVTC_MDI,
    as_<ulong>(EsMdiEvt::DocumentSideloadChanged),
    payload
  );
}
//--------------------------------------------------------------------------------
