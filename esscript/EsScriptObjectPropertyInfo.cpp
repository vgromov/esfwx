#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptObjectPropertyInfo.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// script property info implementation
EsScriptObjectPropertyInfo::EsScriptObjectPropertyInfo(const EsString& name, const EsAttributesIntf::Ptr& attrs, EsScriptObjectIntf* parent,
  const EsString& readerName, const EsString& writerName) :
m_name(name),
m_attrs(attrs),
m_parent(parent),
m_readerName(readerName),
m_writerName(writerName)
{
  m_dynamic = true;
  ES_ASSERT(m_parent);
}

EsString::Array EsScriptObjectPropertyInfo::attributeNamesGet() const
{
  if( m_attrs )
    return m_attrs->allNamesGet();

  return EsString::nullArray();
}

EsVariant EsScriptObjectPropertyInfo::attributeGet(cr_EsString name) const
{
  if( m_attrs )
    return m_attrs->attributeGet(name);
  
  return EsVariant::null();
}

const EsString& EsScriptObjectPropertyInfo::readerNameGet(bool doThrow /*= true*/) const
{
  if(m_readerName.empty() && doThrow)
    EsScriptException::ThrowPropertyHasNoReadAccess(m_name, m_parent->typeNameGet());

  return m_readerName; 
}

const EsString& EsScriptObjectPropertyInfo::writerNameGet(bool doThrow /*= true*/) const
{ 
  if(m_writerName.empty() && doThrow)
    EsScriptException::ThrowPropertyHasNoWriteAccess(m_name, m_parent->typeNameGet());

  return m_writerName; 
}
