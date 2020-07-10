#include "escorepch.h"
#pragma hdrstop

#include "EsEnumeration.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsEnumeration, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEnumeration, EsEnumerationIntf, itemAdd, void_Call_cr_EsString_cr_EsVariant_cr_EsString, esT("Append item to enumeration"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEnumeration, EsEnumerationIntf, countGet, ulong_CallConst, esT("Return enumeration items count"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEnumeration, EsEnumerationIntf, itemGet, EsVariant_CallConst_ulong, esT("Return enumeration items value by its index"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEnumeration, EsEnumerationIntf, valueGet, EsVariant_CallConst_cr_EsString, esT("Return enumeration value by its symbolic name"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEnumeration, EsEnumerationIntf, valueExists, bool_CallConst_cr_EsVariant, esT("Return true if enumeration contains value"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEnumeration, EsEnumerationIntf, valueIndexGet, ulong_CallConst_cr_EsVariant, esT("Return enumeration index of the value"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEnumeration, EsEnumerationIntf, valueSymbolGet, EsString_CallConst_cr_EsVariant, esT("Return enumeration symbolic name by its value"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEnumeration, EsEnumerationIntf, symbolLabelGet, EsString_CallConst_cr_EsString, esT("Return enumeration value label by its symbolic name"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEnumeration, EsEnumerationIntf, valueLabelGet, EsString_CallConst_cr_EsVariant, esT("Return enumeration value label by its value"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEnumeration, EsEnumerationIntf, hasSymbol, bool_CallConst_cr_EsString, esT("Return true is enumeration contains symbol member"))
  ES_DECL_PROP_INFO_RO(EsEnumeration, symbols, EsStringArray, NO_PROPERTY_LABEL, esT("Return all enum item names"))
  ES_DECL_PROP_INFO_RO(EsEnumeration, values, EsVariant, NO_PROPERTY_LABEL, esT("Return all enum item values"))
  ES_DECL_PROP_INFO_RO(EsEnumeration, labels, EsStringArray, NO_PROPERTY_LABEL, esT("Return all enum item labels"))
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsEnumeration::EsEnumeration(const EsString& enumTypeName) ES_NOTHROW :
m_contents(
  EsStringIndexedMap::ContainerUsesInterlock,
  enumTypeName
),
m_attrs(
  EsAttributes::create(
    enumTypeName,
    false
  )
)
{
  m_dynamic = true;
}
//---------------------------------------------------------------------------

// Non-dynamic constructor
EsEnumeration::EsEnumeration(
  const EsString& enumTypeName,
  ES_UNUSED(EsEnumerationNonDynamic dummy)
) ES_NOTHROW :
m_contents(
  EsStringIndexedMap::ContainerUsesInterlock,
  enumTypeName
),
m_attrs(
  EsAttributes::create(
    enumTypeName,
    false
  )
)
{
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsEnumeration::attributesAssign)(const EsAttributesIntf::Ptr& attrs) ES_NOTHROW
{
  m_attrs = attrs;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsEnumeration::attributeAdd)(const EsString& name, const EsVariant& val /*= EsVariant::null()*/)
{
  if(!m_attrs)
    m_attrs = EsAttributes::create(
      typeNameGet(),
      false
    );
  ES_ASSERT(m_attrs);

  m_attrs->attributeAdd(
    name,
    val
  );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsEnumeration::hasAttribute)(cr_EsString name) const ES_NOTHROW
{
  if(m_attrs)
    return m_attrs->attributeExists(name);

  return false;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsString::Array, EsEnumeration::attributeNamesGet)() const ES_NOTHROW
{
  if(m_attrs)
    return m_attrs->allNamesGet();

  return EsString::nullArray();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsEnumeration::attributeGet)(cr_EsString name) const ES_NOTHROW
{
  if(m_attrs)
    return m_attrs->attributeGet(name);

  return EsVariant::null();
}
//---------------------------------------------------------------------------

void EsEnumeration::itemAdd(const EsString& symbol, const EsVariant& val,  const EsString& label)
{
  if( symbol.empty() )
    EsException::Throw(
      esT("Could not add an enumeration item without a symolic name")
    );

  if( val.isEmpty() )
    EsException::Throw(
      esT("Could not add an enumeration item without a value")
    );

  if( val.isCollection() )
    EsException::Throw(
      esT("Could not add an enumeration item with value of collection type")
    );

  EsVariant item(EsVariant::VAR_VARIANT_COLLECTION);
  item.addToVariantCollection(val)
    .addToVariantCollection(
      label.empty() ?
        EsVariant::null() :
        EsVariant(label)
    );

  m_contents.itemAdd(
    symbol,
    item
  );
}
//---------------------------------------------------------------------------

// reflected services
//
ulong EsEnumeration::countGet() const ES_NOTHROW
{
  return m_contents.countGet();
}
//---------------------------------------------------------------------------

EsVariant EsEnumeration::itemGet(ulong idx) const
{
  return m_contents.valueGet(idx)[0];
}
//---------------------------------------------------------------------------

EsVariant EsEnumeration::valueGet(cr_EsString symbol) const
{
  ES_ASSERT(!symbol.empty());
  return m_contents.valueGet(symbol)[0];
}
//---------------------------------------------------------------------------

EsString EsEnumeration::labelGet(ulong idx) const
{
  const EsVariant& label = m_contents.valueGet(idx)[1];

  if( label.isEmpty() )
    return m_contents.nameGet(idx);

  return esTranslationGet( label.asString() );
}
//---------------------------------------------------------------------------

EsString EsEnumeration::symbolLabelGet(cr_EsString symbol) const
{
  ES_ASSERT(!symbol.empty());
  const EsVariant& label = m_contents.valueGet(symbol)[1];

  if( label.isEmpty() )
    return symbol;

  return esTranslationGet( label.asString() );
}
//---------------------------------------------------------------------------

EsString EsEnumeration::valueLabelGet(cr_EsVariant val) const
{
  ES_ASSERT(!val.isEmpty());
  ulong idx = valueIndexGet(val);

  return labelGet(idx);
}
//---------------------------------------------------------------------------

EsString EsEnumeration::valueSymbolGet(cr_EsVariant val) const
{
  ES_ASSERT(!val.isEmpty());
  ulong idx = valueIndexGet(val);

  return m_contents.nameGet(idx);
}
//---------------------------------------------------------------------------

bool EsEnumeration::valueExists(cr_EsVariant val) const ES_NOTHROW
{
  for(ulong idx = 0; idx < m_contents.countGet(); ++idx)
  {
    const EsVariant& itemVal = m_contents.valueGet(idx);
    ES_ASSERT(!itemVal.isEmpty());
    if( val == itemVal.itemGet(0) )
      return true;
  }

  return false;
}
//---------------------------------------------------------------------------

ulong EsEnumeration::valueIndexGet(cr_EsVariant val) const
{
  for(ulong idx = 0; idx < m_contents.countGet(); ++idx)
  {
    const EsVariant& itemVal = m_contents.valueGet(idx);
    if( val == itemVal.itemGet(0) )
      return static_cast<ulong>(idx);
  }

  EsException::Throw(
    esT("Could not find value index in enumeration '%s'"),
    m_contents.nameGet()
  );

  // pacify conpilers
  return 0;
}
//---------------------------------------------------------------------------

EsStringArray EsEnumeration::labelsGet() const ES_NOTHROW
{
  EsStringArray result;
  result.reserve(m_contents.countGet());
  for(ulong idx = 0; idx < m_contents.countGet(); ++idx )
    result.push_back( labelGet(idx) );

  return result;
}
//---------------------------------------------------------------------------

EsVariant EsEnumeration::valuesGet() const ES_NOTHROW
{
  EsVariant result(EsVariant::VAR_VARIANT_COLLECTION);
  for(ulong idx = 0; idx < m_contents.countGet(); ++idx )
  {
    const EsVariant& itemVal = m_contents.valueGet(idx);
    result.addToVariantCollection( itemVal.itemGet(0) );
  }

  return result;
}
//---------------------------------------------------------------------------

bool EsEnumeration::hasSymbol( cr_EsString name ) const ES_NOTHROW
{
  return m_contents.itemExists( name );
}
//---------------------------------------------------------------------------

EsStringArray EsEnumeration::get_symbols() const
{
  return symbolNamesGet();
}
//---------------------------------------------------------------------------

EsVariant EsEnumeration::get_values() const
{
  return valuesGet();
}
//---------------------------------------------------------------------------

EsStringArray EsEnumeration::get_labels() const
{
  return labelsGet();
}
//---------------------------------------------------------------------------

