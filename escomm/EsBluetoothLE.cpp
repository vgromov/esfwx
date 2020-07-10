#include "escommpch.h"
#pragma hdrstop

#include "EsBluetoothLE.h"
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_BLUETOOTH_LE
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

// Reflection INFO declarations
namespace EsReflection
{
ES_DECL_ENUMERATION_INFO(EsBluetoothLEformatType, esT("BLE Format Type enumeration"))
ES_DECL_ENUMERATION_INFO(EsBluetoothLEscanResponseKey, esT("BLE Scan Response Key enumeration"))
ES_DECL_ENUMERATION_INFO(EsBluetoothLEresult, esT("BLE Operation Result enumeration"))
ES_DECL_ENUMERATION_INFO(EsBluetoothLEdescriptorKind, esT("BLE Descriptor Kind enumeration"))
ES_DECL_ENUMERATION_INFO(EsBluetoothLEcharacteristicProperty, esT("BLE Characteristic Property enumeration"))
}

// Platform - independent BLE classes implementation part
//
//---------------------------------------------------------------------------

// BLE descriptor
//

#define ES_BLE_DESCRIPTOR_PROPERTY_S_IS_NOT_VALID_FOR_KIND_S \
  _("Descriptor property '%s' is not valid for descriptor kind '%s'")

// Known descriptors database, indexed by descriptor kinds
#define ES_BLE_DESCRIPTOR_BEGIN struct EsBluetoothLEknownDescriptor { \
  EsString::const_pointer m_descr; \
  ulong m_uuid; }; \
  static const EsBluetoothLEknownDescriptor sc_knownDescrs[] = {
#define ES_BLE_DESCRIPTOR_ENTRY(Name, Descr, Uuid) { Descr, Uuid },
#define ES_BLE_DESCRIPTOR_END { 0, 0 } };
#include "EsBluetoothLEdescriptor.defs.h"
//---------------------------------------------------------------------------

static int knownDescriptorRecordIdxFind(ulong uuid)
{
  int idx = 0;
  while(1)
  {
    const EsBluetoothLEknownDescriptor& dcr = sc_knownDescrs[idx];

    if( !dcr.m_descr )
      break;

    if( dcr.m_uuid == uuid )
      return idx;

    ++idx;
  }

  return -1;
}
//---------------------------------------------------------------------------

static int knownDescriptorRecordIdxFind(const GUID& uuid)
{
  if( EsBluetoothUUID::isOk(uuid) )
    return knownDescriptorRecordIdxFind(uuid.Data1);

  return -1;
}
//---------------------------------------------------------------------------
//--------------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsBluetoothLEdescriptor, esT("BLE descriptor"))
  // Properties
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEdescriptor, isKnown,               bool,       NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEdescriptor, isOrphan,              bool,       NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEdescriptor, isBroadcast,           bool,       NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO(                        EsBluetoothLEdescriptor, isIndicate,            bool,       NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO(                        EsBluetoothLEdescriptor, isNotifying,           bool,       NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEdescriptor, isReliableWrite,       bool,       NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEdescriptor, isWritableAuxiliaries, bool,       NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEdescriptor, uuidShort,             ulong,      NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEdescriptor, uuid,                  EsVariant,  NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEdescriptor, kind,                  ulong,      NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEdescriptor, kindName,              EsString,   NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO(                        EsBluetoothLEdescriptor, value,                 EsBinBuffer,NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEdescriptor, exponent,              long,       NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEdescriptor, formatType,            ulong,      NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEdescriptor, formatUnitGet,         EsVariant,  NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEdescriptor, formatDescriptionGet,  EsVariant,  NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEdescriptor, userDescription,       EsString,   NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEdescriptor, characteristic,        EsVariant,  NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )

  /// Reflected services
  ES_DECL_REFLECTED_METHOD_INFO(            EsBluetoothLEdescriptor, readReflected, read,   ulong_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(        EsBluetoothLEdescriptor, readCached,            ulong_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(            EsBluetoothLEdescriptor, writeReflected, write, ulong_Call, NO_METHOD_DESCR)
  
  /// Static reflected services
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothLEdescriptor, isKnownUUID,           bool_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothLEdescriptor, kindNameGetByKind,     EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothLEdescriptor, kindNameGetByUUID,     EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothLEdescriptor, kindGetByUUID,         EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothLEdescriptor, uuidGetByKind,         EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//--------------------------------------------------------------------------------

EsBluetoothLEdescriptor::EsBluetoothLEdescriptor(const EsBaseIntfPtr& chx /*= nullptr*/) :
m_chx(chx),
m_kind(EsBluetoothLEdescriptorKind::Unknown)
{
  m_dynamic = true; //< Should be always dynamically allocated and de-allocated
}
//---------------------------------------------------------------------------

bool EsBluetoothLEdescriptor::get_isKnown() const
{ 
  return isKnownUUID(m_uuid); 
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEdescriptor::get_isOrphan() const
{
  return nullptr == m_chx.get();
}
//--------------------------------------------------------------------------------

ulong EsBluetoothLEdescriptor::get_uuidShort() const 
{ 
  return m_uuid.uuidGet().Data1; 
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEdescriptor::get_uuid() const 
{
  return EsUUID::toVariant(uuidGet());
}
//--------------------------------------------------------------------------------

ulong EsBluetoothLEdescriptor::get_kind() const
{
  return static_cast<ulong>( kindGet() );
}
//--------------------------------------------------------------------------------

EsString EsBluetoothLEdescriptor::get_kindName() const
{
  return kindNameGet();
}
//--------------------------------------------------------------------------------

EsBinBuffer EsBluetoothLEdescriptor::get_value() const
{
  return valueGet();
}
//--------------------------------------------------------------------------------

void EsBluetoothLEdescriptor::set_value(const EsBinBuffer& val)
{
  return valueSet(val);
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEdescriptor::get_isBroadcast() const
{
  return isBroadcast();
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEdescriptor::get_isIndicate() const
{
  return isIndicate();
}
//--------------------------------------------------------------------------------

void EsBluetoothLEdescriptor::set_isIndicate(const bool& val)
{
  indicateSet(val);
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEdescriptor::get_isNotifying() const
{
  return isNotify();
}
//--------------------------------------------------------------------------------

void EsBluetoothLEdescriptor::set_isNotifying(const bool& val)
{
  notifySet(val);
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEdescriptor::get_isReliableWrite() const
{
  return isReliableWrite();
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEdescriptor::get_isWritableAuxiliaries() const
{
  return isWritableAuxiliaries();
}
//--------------------------------------------------------------------------------

long EsBluetoothLEdescriptor::get_exponent() const
{
  return exponentGet();
}
//--------------------------------------------------------------------------------

ulong EsBluetoothLEdescriptor::get_formatType() const
{
  return static_cast<ulong>(formatTypeGet());
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEdescriptor::get_formatUnitGet() const
{
  return EsUUID::toVariant( formatUnitGet() );
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEdescriptor::get_formatDescriptionGet() const
{
  return EsUUID::toVariant(formatDescriptionGet());
}
//--------------------------------------------------------------------------------

EsString EsBluetoothLEdescriptor::get_userDescription() const
{
  return userDescriptionGet();
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEdescriptor::get_characteristic() const
{
  return m_chx;
}
//--------------------------------------------------------------------------------

ulong EsBluetoothLEdescriptor::readReflected()
{
  return static_cast<ulong>(read());
}
//--------------------------------------------------------------------------------

ulong EsBluetoothLEdescriptor::readCached()
{
  return static_cast<ulong>(read(true));
}
//--------------------------------------------------------------------------------

ulong EsBluetoothLEdescriptor::writeReflected()
{
  return static_cast<ulong>(write());
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEdescriptor::isKnownUUID(cr_EsVariant uuid)
{
  if(uuid.isEmpty())
    return false;

  if( uuid.isNumeric() )
    return isKnownUUID(  
      uuid.asULong()
    );

  return isKnownUUID(
    EsUUID::fromVariant(uuid)
  );
}
//--------------------------------------------------------------------------------

EsString EsBluetoothLEdescriptor::kindNameGetByKind(cr_EsVariant kind)
{
  return kindNameGetByKind(
    as_<EsBluetoothLEdescriptorKind>(kind.asULong())
  );
}
//--------------------------------------------------------------------------------

EsString EsBluetoothLEdescriptor::kindNameGetByUUID(cr_EsVariant uuid)
{
  if(uuid.isNumeric())
    return kindNameGetByUUID(
      uuid.asULong()
    );

  return kindNameGetByUUID(
    EsUUID::fromVariant(uuid)
  );
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEdescriptor::kindGetByUUID(cr_EsVariant uuid)
{
  if(uuid.isNumeric())
    return as_<ulong>(
      kindGetByUUID(
        uuid.asULong()
      )
    );

  return as_<ulong>(
    kindGetByUUID(
      EsUUID::fromVariant(uuid)
    )
  );
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEdescriptor::uuidGetByKind(cr_EsVariant kind)
{
  if(kind.isEmpty())
    return EsVariant::null();

  return EsUUID::toVariant( 
    uuidGetByKind(
      as_<EsBluetoothLEdescriptorKind>(kind.asULong())
    )
  );
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsString EsBluetoothLEdescriptor::kindNameGet() const 
{ 
  return get_isKnown() ? 
    kindNameGetByKind(m_kind) : 
    EsString::null(); 
}
//--------------------------------------------------------------------------------

const EsBluetoothLEcharacteristic* EsBluetoothLEdescriptor::characteristicGet() const
{ 
  return ES_INTFPTR_TO_OBJECTPTR<EsBluetoothLEcharacteristic>( m_chx );
}
//--------------------------------------------------------------------------------

EsBluetoothLEcharacteristic* EsBluetoothLEdescriptor::characteristicGet()
{ 
  return ES_INTFPTR_TO_OBJECTPTR<EsBluetoothLEcharacteristic>(m_chx);
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEdescriptor::isKnownUUID(ulong uuid)
{
  return -1 != knownDescriptorRecordIdxFind(uuid);
}
//---------------------------------------------------------------------------

bool EsBluetoothLEdescriptor::isKnownUUID(const GUID& uuid)
{
  return -1 != knownDescriptorRecordIdxFind(uuid);
}
//---------------------------------------------------------------------------

EsString EsBluetoothLEdescriptor::kindNameGetByKind(EsBluetoothLEdescriptorKind kind)
{
  ulong idx = static_cast<ulong>(kind);
  if(idx < static_cast<ulong>(EsBluetoothLEdescriptorKind::Unknown))
    return EsStringI18n::translate(
      sc_knownDescrs[idx].m_descr
    );

  return _("Unknown");
}
//---------------------------------------------------------------------------

EsString EsBluetoothLEdescriptor::kindNameGetByUUID(ulong uuid)
{
  int idx = knownDescriptorRecordIdxFind(uuid);
  if( -1 < idx )
    return EsStringI18n::translate(sc_knownDescrs[idx].m_descr);

  return _("Unknown");
}
//---------------------------------------------------------------------------

EsString EsBluetoothLEdescriptor::kindNameGetByUUID(const GUID& uuid)
{
  int idx = knownDescriptorRecordIdxFind(uuid);
  if( -1 < idx )
    return EsStringI18n::translate(sc_knownDescrs[idx].m_descr);

  return _("Unknown");
}
//---------------------------------------------------------------------------

EsBluetoothLEdescriptorKind EsBluetoothLEdescriptor::kindGetByUUID(ulong uuid)
{
  int idx = knownDescriptorRecordIdxFind(uuid);
  if( -1 < idx )
    return static_cast<EsBluetoothLEdescriptorKind>(idx);

  return EsBluetoothLEdescriptorKind::Unknown;
}
//---------------------------------------------------------------------------

EsBluetoothLEdescriptorKind EsBluetoothLEdescriptor::kindGetByUUID(const GUID& uuid)
{
  int idx = knownDescriptorRecordIdxFind(uuid);
  if( -1 < idx )
    return static_cast<EsBluetoothLEdescriptorKind>(idx);

  return EsBluetoothLEdescriptorKind::Unknown;
}
//---------------------------------------------------------------------------

GUID EsBluetoothLEdescriptor::uuidGetByKind(EsBluetoothLEdescriptorKind kind)
{
  size_t idx = static_cast<size_t>(kind);
  if(idx < static_cast<size_t>(EsBluetoothLEdescriptorKind::Unknown))
    return EsBluetoothUUID::fromShortened(
      sc_knownDescrs[idx].m_uuid
    );

  return GUID_NULL;
}
//---------------------------------------------------------------------------

void EsBluetoothLEdescriptor::throwInvalidPropForKind(const EsString& prop, EsBluetoothLEdescriptorKind kind)
{
  EsException::Throw( 
    ES_BLE_DESCRIPTOR_PROPERTY_S_IS_NOT_VALID_FOR_KIND_S,
    prop,
    kindNameGetByKind(kind)
  );
}
//---------------------------------------------------------------------------
//--------------------------------------------------------------------------------

#define ES_BLE_CHARACTERISTIC_BEGIN
#define ES_BLE_CHARACTERISTIC_ENTRY(Uuid, Name) m_data[Uuid] = Name;
#define ES_BLE_CHARACTERISTIC_END
class EsKnownChxMap
{
public:
  typedef std::map<ulong, EsString> DataT;

public:
  EsKnownChxMap()
  {
#include "EsBluetoothLEcharacteristic.defs.h"
  }

  bool has(ulong uuid) const
  {
    return m_data.end() != m_data.find(uuid);
  }

  const EsString& descrGet(ulong uuid) const
  {
    DataT::const_iterator cit = m_data.find(uuid);
    if( cit != m_data.end() )
      return (*cit).second;

    return EsString::null();
  }

  static const EsKnownChxMap& instGet()
  {
    static EsKnownChxMap s_cm;
    return s_cm;
  }

protected:
  DataT m_data;
};
//---------------------------------------------------------------------------
//--------------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsBluetoothLEcharacteristic, esT("BLE characteristic class"))
  // Properties
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEcharacteristic, isKnown, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEcharacteristic, isOrphan, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEcharacteristic, knownName, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEcharacteristic, name, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEcharacteristic, properties, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEcharacteristic, propertiesStr, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEcharacteristic, canRead, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEcharacteristic, canWrite, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEcharacteristic, canNotify, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEcharacteristic, canIndicate, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEcharacteristic, subscribed, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEcharacteristic, uuidShort, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEcharacteristic, uuid, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEcharacteristic, service, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEcharacteristic, descriptors, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEcharacteristic, descriptorsUUIDs, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                        EsBluetoothLEcharacteristic, value, EsBinBuffer, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)

  /// Reflected services
  ES_DECL_REFLECTED_METHOD_INFO_STD(        EsBluetoothLEcharacteristic, descriptorFind, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(        EsBluetoothLEcharacteristic, descriptorGet, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(        EsBluetoothLEcharacteristic, subscribe, EsVariant_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(            EsBluetoothLEcharacteristic, readReflected, read, ulong_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(        EsBluetoothLEcharacteristic, readCached, ulong_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(            EsBluetoothLEcharacteristic, writeReflected, write, ulong_Call, NO_METHOD_DESCR)

  /// Static reflected services
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothLEcharacteristic, isKnownUUID, bool_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothLEcharacteristic, knownNameGetByUUID, EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothLEcharacteristic, propertiesStringGet, EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//--------------------------------------------------------------------------------

EsBluetoothLEcharacteristic::EsBluetoothLEcharacteristic(const EsBaseIntfPtr& svc) :
m_svc(svc),
m_props(0)
{
  m_dynamic = true; //< Use only in dynamic scenarios

  m_dcrs = EsAssocContainer::create(
    asBaseIntf()
  );
}
//---------------------------------------------------------------------------

bool EsBluetoothLEcharacteristic::get_isOrphan() const
{ 
  return nullptr == m_svc.get(); 
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEcharacteristic::get_isKnown() const
{ 
  return isKnownUUID( m_uuid.uuidGet() ); 
}
//--------------------------------------------------------------------------------

EsString EsBluetoothLEcharacteristic::get_knownName() const 
{ 
  return get_isKnown() ? 
    knownNameGetByUUID(m_uuid.uuidGet()) : 
    EsString::null(); 
}
//--------------------------------------------------------------------------------

EsString EsBluetoothLEcharacteristic::get_name() const
{
  if(get_isKnown())
    return knownNameGetByUUID(m_uuid.uuidGet());

  if( m_dcrs )
  {
    const GUID& uuid = EsBluetoothLEdescriptor::uuidGetByKind(
      EsBluetoothLEdescriptorKind::UserDescription
    );

    EsBluetoothLEdescriptor* dcr = descriptorFind(uuid);

    if( dcr )
      return dcr->userDescriptionGet();
  }

  return EsString::null();
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEcharacteristic::get_subscribed() const
{
  return isSubscribed();
}
//--------------------------------------------------------------------------------

ulong EsBluetoothLEcharacteristic::get_properties() const 
{ 
  return m_props;
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEcharacteristic::get_canRead() const 
{ 
  return (m_props & EsBluetoothLEcharacteristicProperty::Read);
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEcharacteristic::get_canWrite() const 
{ 
  return (m_props & EsBluetoothLEcharacteristicProperty::Write);
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEcharacteristic::get_canNotify() const 
{ 
  return (m_props & EsBluetoothLEcharacteristicProperty::Notify);
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEcharacteristic::get_canIndicate() const 
{ 
  return (m_props & EsBluetoothLEcharacteristicProperty::Indicate);
}
//--------------------------------------------------------------------------------

EsString EsBluetoothLEcharacteristic::get_propertiesStr() const 
{ 
  return propertiesStringGet(m_props); 
}
//--------------------------------------------------------------------------------

ulong EsBluetoothLEcharacteristic::get_uuidShort() const 
{ 
  return m_uuid.uuidGet().Data1; 
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEcharacteristic::get_uuid() const 
{ 
  return EsUUID::toVariant( m_uuid ); 
}
//---------------------------------------------------------------------------

EsVariant EsBluetoothLEcharacteristic::get_service() const
{
  return m_svc;
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEcharacteristic::get_descriptors() const
{
  return m_dcrs->allValuesGet();
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEcharacteristic::get_descriptorsUUIDs() const
{
  return m_dcrs->allKeysGet();
}
//--------------------------------------------------------------------------------

EsBinBuffer EsBluetoothLEcharacteristic::get_value() const
{
  return valueGet();
}
//--------------------------------------------------------------------------------

void EsBluetoothLEcharacteristic::set_value(const EsBinBuffer& val)
{
  valueSet(val);
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEcharacteristic::descriptorFind(const EsVariant& uuid) const
{
  if(m_dcrs->keyExists(uuid))
    return m_dcrs->valueGet(uuid);

  return EsVariant::null();
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEcharacteristic::descriptorGet(const EsVariant& uuid) const
{
  const EsVariant& result = descriptorFind(uuid);
  
  if(result.isEmpty())
    EsException::Throw(
      _("Could not find descriptor with UUID %s in characteristic %s"),
      uuid.trace(),
      m_uuid.asString()
    );

  return result;
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEcharacteristic::subscribe(const EsVariant& on)
{
  return as_<ulong>( subscribe(on.asBool()) );
}
//--------------------------------------------------------------------------------

ulong EsBluetoothLEcharacteristic::readReflected()
{
  return static_cast<ulong>(read());
}
//--------------------------------------------------------------------------------

ulong EsBluetoothLEcharacteristic::readCached()
{
  return static_cast<ulong>(read(true));
}
//--------------------------------------------------------------------------------

ulong EsBluetoothLEcharacteristic::writeReflected()
{
  return static_cast<ulong>(write());
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEcharacteristic::isKnownUUID(cr_EsVariant uuid)
{
  if(uuid.isEmpty())
    return false;

  if(uuid.isNumeric())
    return isKnownUUID( uuid.asULong() );

  return isKnownUUID(
    EsUUID::fromVariant(uuid)
  );
}
//--------------------------------------------------------------------------------

EsString EsBluetoothLEcharacteristic::knownNameGetByUUID(cr_EsVariant uuid)
{
  if(uuid.isEmpty())
    return EsString::null();

  if(uuid.isNumeric())
    return knownNameGetByUUID(uuid.asULong());

  return knownNameGetByUUID(
    EsUUID::fromVariant(uuid)
  );
}
//--------------------------------------------------------------------------------

EsString EsBluetoothLEcharacteristic::propertiesStringGet(cr_EsVariant uuid)
{
  return propertiesStringGet(
    uuid.asULong()
  );
}
//--------------------------------------------------------------------------------

EsBluetoothLEdescriptor* EsBluetoothLEcharacteristic::descriptorFind(const GUID& uuid) const
{
  const EsVariant& dcr = descriptorFind(
    EsUUID::toVariant(
      uuid
    )
  );

  return ES_VARIANT_TO_OBJECTPTR<EsBluetoothLEdescriptor>(dcr);
}
//---------------------------------------------------------------------------

const EsBluetoothLEservice* EsBluetoothLEcharacteristic::serviceGet() const
{
  return ES_INTFPTR_TO_OBJECTPTR<EsBluetoothLEservice>(m_svc);
}
//--------------------------------------------------------------------------------

EsBluetoothLEservice* EsBluetoothLEcharacteristic::serviceGet() 
{ 
  return ES_INTFPTR_TO_OBJECTPTR<EsBluetoothLEservice>(m_svc);
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEcharacteristic::isKnownUUID(ulong uuid)
{
  return EsKnownChxMap::instGet().has(uuid);
}
//---------------------------------------------------------------------------

bool EsBluetoothLEcharacteristic::isKnownUUID(const GUID& uuid)
{
  if( EsBluetoothUUID::isOk(uuid) )
    return isKnownUUID(uuid.Data1);

  return false;
}
//---------------------------------------------------------------------------

EsString EsBluetoothLEcharacteristic::knownNameGetByUUID(ulong uuid)
{
  const EsString& descr = EsKnownChxMap::instGet().descrGet(uuid);
  if( !descr.empty() )
    return EsStringI18n::translate(descr);

  return _("Unknown");
}
//---------------------------------------------------------------------------

EsString EsBluetoothLEcharacteristic::knownNameGetByUUID(const GUID& uuid)
{
  if( EsBluetoothUUID::isOk(uuid) )
    return knownNameGetByUUID(uuid.Data1);

  return _("Unknown");
}
//---------------------------------------------------------------------------

EsString EsBluetoothLEcharacteristic::propertiesStringGet(ulong props)
{
  EsString result;

  if( props & EsBluetoothLEcharacteristicProperty::Broadcast )
    result += _("Broadcastable");

  if( props & EsBluetoothLEcharacteristicProperty::ExtendedProps )
  {
    if( !result.empty() )
      result += esT(";");
    result += _("Have extended properties");
  }

  if( props & EsBluetoothLEcharacteristicProperty::Notify )
  {
    if( !result.empty() )
      result += esT(";");
    result += _("Notification");
  }

  if( props & EsBluetoothLEcharacteristicProperty::Indicate )
  {
    if( !result.empty() )
      result += esT(";");
    result += _("Indication");
  }

  if( props & EsBluetoothLEcharacteristicProperty::Read )
  {
    if( !result.empty() )
      result += esT(";");
    result += _("Read");
  }

  if( props & EsBluetoothLEcharacteristicProperty::Write )
  {
    if( !result.empty() )
      result += esT(";");
    result += _("Write");
  }

  if( props & EsBluetoothLEcharacteristicProperty::WriteNoResponse )
  {
    if( !result.empty() )
      result += esT(";");
    result += _("Write without response");
  }

  if( props & EsBluetoothLEcharacteristicProperty::SignedWrite )
  {
    if( !result.empty() )
      result += esT(";");
    result += _("Signed write");
  }

  return result;
}
//--------------------------------------------------------------------------------

void EsBluetoothLEcharacteristic::descriptorsCleanup()
{
  while( !m_dcrs->isEmpty() )
  {
    EsBluetoothLEdescriptor* dcr = ES_VARIANT_TO_OBJECTPTR<EsBluetoothLEdescriptor>( m_dcrs->itemGet( 0 )[1] );
    ES_ASSERT( dcr );

    dcr->m_chx.reset();

    m_dcrs->itemDelete( 0 );
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// LE Service base implementation
//
#define ES_BLE_SERVICE_BEGIN
#define ES_BLE_SERVICE_ENTRY(Uuid, Name) m_data[Uuid] = Name;
#define ES_BLE_SERVICE_END
class EsKnownSvcMap
{
public:
  typedef std::map<ulong, EsString> DataT;

public:
  EsKnownSvcMap()
  {
#include "EsBluetoothLEservice.defs.h"
  }

  bool has(ulong uuid) const
  {
    return m_data.end() != m_data.find(uuid);
  }

  const EsString& descrGet(ulong uuid) const
  {
    DataT::const_iterator cit = m_data.find(uuid);
    if( cit != m_data.end() )
      return (*cit).second;

    return EsString::null();
  }

  static const EsKnownSvcMap& instGet()
  {
    static EsKnownSvcMap s_sm;
    return s_sm;
  }

protected:
  DataT m_data;
};

//---------------------------------------------------------------------------
//--------------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsBluetoothLEservice, esT("BLE service class"))
  // Properties
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEservice, isKnown, bool,                    NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEservice, isOrphan, bool,                   NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEservice, uuidShort, ulong,                 NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEservice, uuid, EsVariant,                  NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEservice, knownName, EsString,              NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEservice, characteristicsUUIDs, EsVariant,  NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEservice, characteristics, EsVariant,       NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsBluetoothLEservice, device, EsVariant,                NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)

  // Reflected services
  ES_DECL_REFLECTED_METHOD_INFO_STD(        EsBluetoothLEservice, characteristicFind, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(        EsBluetoothLEservice, characteristicGet, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)

  // Static services
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothLEservice, isKnownUUID, bool_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothLEservice, knownNameGetByUUID, EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//--------------------------------------------------------------------------------

EsBluetoothLEservice::EsBluetoothLEservice(const EsBaseIntfPtr& dev /*= nullptr*/) :
m_dev(dev)
{
  m_dynamic = true; //< Only use dynamic allocation

  m_chxs = EsAssocContainer::create(
    asBaseIntf()
  );
}
//---------------------------------------------------------------------------

bool EsBluetoothLEservice::get_isKnown() const
{
  return isKnownUUID(m_uuid.uuidGet());
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEservice::get_isOrphan() const
{
  return nullptr == m_dev.get();
}
//--------------------------------------------------------------------------------

ulong EsBluetoothLEservice::get_uuidShort() const
{
  return m_uuid.uuidGet().Data1;
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEservice::get_uuid() const
{
  return EsUUID::toVariant( m_uuid.uuidGet() );
}
//--------------------------------------------------------------------------------

EsString EsBluetoothLEservice::get_knownName() const
{
  return get_isKnown() ?
    knownNameGetByUUID(m_uuid) :
    EsString::null();
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEservice::get_characteristicsUUIDs() const
{
  return m_chxs->allKeysGet();
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEservice::get_characteristics() const
{
  return m_chxs->allValuesGet();
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEservice::get_device() const
{
  return m_dev;
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEservice::characteristicFind(cr_EsVariant uuid) const
{
  if(m_chxs->keyExists(uuid))
    return m_chxs->valueGet(uuid);

  return EsVariant::null();
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEservice::characteristicGet(cr_EsVariant uuid) const
{
  const EsVariant& chx = characteristicFind(uuid);

  if(chx.isEmpty())
    EsException::Throw(
      _("Could not find characteristic with UUID %s in service %s"),
      uuid.trace(),
      m_uuid.asString()
    );

  return chx;
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEservice::isKnownUUID(cr_EsVariant uuid)
{
  if(uuid.isEmpty())
    return false;

  if(uuid.isNumeric())
    return isKnownUUID(
      uuid.asULong()
    );

  return isKnownUUID(
    EsUUID::fromVariant(uuid)
  );
}
//--------------------------------------------------------------------------------

EsString EsBluetoothLEservice::knownNameGetByUUID(cr_EsVariant uuid)
{
  if(uuid.isEmpty())
    return EsString::null();

  if(uuid.isNumeric())
    return knownNameGetByUUID(
      uuid.asULong()
    );

  return knownNameGetByUUID(
    EsUUID::fromVariant(uuid)
  );
}
//--------------------------------------------------------------------------------

EsBluetoothLEcharacteristic* EsBluetoothLEservice::characteristicFind(const GUID& uuid) const
{
  const EsVariant& vchx = characteristicFind(
    EsUUID::toVariant(
      uuid
    )
  );

  return ES_VARIANT_TO_OBJECTPTR<EsBluetoothLEcharacteristic>(vchx);
}
//--------------------------------------------------------------------------------

EsBluetoothLEcharacteristic* EsBluetoothLEservice::characteristicGet(const GUID& uuid) const
{
  const EsVariant& vchx = characteristicGet(
    EsUUID::toVariant(
      uuid
    )
  );

  return ES_VARIANT_TO_OBJECTPTR<EsBluetoothLEcharacteristic>(vchx);
}
//--------------------------------------------------------------------------------

const EsBluetoothLEdevice* EsBluetoothLEservice::deviceGet() const 
{
  return ES_INTFPTR_TO_OBJECTPTR<EsBluetoothLEdevice>(m_dev);
}
//--------------------------------------------------------------------------------

EsBluetoothLEdevice* EsBluetoothLEservice::deviceGet() 
{ 
  return ES_INTFPTR_TO_OBJECTPTR<EsBluetoothLEdevice>(m_dev);
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEservice::isKnownUUID(ulong uuid)
{
  return EsKnownSvcMap::instGet().has(uuid);
}
//---------------------------------------------------------------------------

bool EsBluetoothLEservice::isKnownUUID(const GUID& uuid)
{
  return EsBluetoothUUID::isOk(uuid) &&
    EsKnownSvcMap::instGet().has(uuid.Data1);
}
//---------------------------------------------------------------------------

EsString EsBluetoothLEservice::knownNameGetByUUID(ulong uuid)
{
  const EsString& descr = EsKnownSvcMap::instGet().descrGet(uuid);
  if( !descr.empty() )
    return EsStringI18n::translate(descr);

  return _("Unknown");
}
//---------------------------------------------------------------------------

EsString EsBluetoothLEservice::knownNameGetByUUID(const GUID& uuid)
{
  if( EsBluetoothUUID::isOk(uuid) )
    return knownNameGetByUUID(uuid.Data1);

  return _("Unknown");
}
//--------------------------------------------------------------------------------

void EsBluetoothLEservice::characteristicsCleanup()
{
  while( !m_chxs->isEmpty() )
  {
    EsBluetoothLEcharacteristic* chx = ES_VARIANT_TO_OBJECTPTR<EsBluetoothLEcharacteristic>( m_chxs->itemGet( 0 )[1] );
    ES_ASSERT(chx);

    chx->m_svc.reset();

    m_chxs->itemDelete(0);
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsBluetoothLEdevice, esT("BLE device class"))
  // Properties
  ES_DECL_PROP_INFO_RO(                 EsBluetoothLEdevice, radio,          EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                 EsBluetoothLEdevice, address,        EsString,  NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                 EsBluetoothLEdevice, name,           EsString,  NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                 EsBluetoothLEdevice, id,             EsString,  NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                 EsBluetoothLEdevice, recentRssi,     long,      NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                 EsBluetoothLEdevice, connected,      bool,      NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                 EsBluetoothLEdevice, servicesUUIDs,  EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                 EsBluetoothLEdevice, services,       EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)

  // Reflected services
  ES_DECL_REFLECTED_METHOD_INFO_STD(    EsBluetoothLEdevice, serviceFind, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(    EsBluetoothLEdevice, serviceGet, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(        EsBluetoothLEdevice, rssiQueryReflected, rssiQuery, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(        EsBluetoothLEdevice, disconnectReflected, disconnect, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(        EsBluetoothLEdevice, discoverReflected, discover, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(        EsBluetoothLEdevice, reliableWriteBeginReflected, reliableWriteBegin, bool_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(        EsBluetoothLEdevice, reliableWriteAbortReflected, reliableWriteAbort, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(        EsBluetoothLEdevice, reliableWriteExecuteReflected, reliableWriteExecute, bool_Call, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//--------------------------------------------------------------------------------

// BT LE device base implemetation
//
EsBluetoothLEdevice::EsBluetoothLEdevice( const EsBaseIntfPtr& radio, const EsString& addr, const EsString& name ) :
m_radio( radio ),
m_addr(addr),
m_name(name),
m_rssi(0)
{
  m_dynamic = true; //< Always use dynamic allocation and deallocation

  m_svcs = EsAssocContainer::create(
    asBaseIntf()
  );
}
//---------------------------------------------------------------------------

EsVariant EsBluetoothLEdevice::get_radio() const
{
  return m_radio;
}
//--------------------------------------------------------------------------------

EsString EsBluetoothLEdevice::get_address() const
{
  return m_addr;
}
//--------------------------------------------------------------------------------

EsString EsBluetoothLEdevice::get_name() const
{
  return m_name;
}
//--------------------------------------------------------------------------------

EsString EsBluetoothLEdevice::get_id() const
{
  return idGet();
}
//--------------------------------------------------------------------------------

long EsBluetoothLEdevice::get_recentRssi() const
{
  return m_rssi;
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEdevice::get_connected() const
{
  return isConnected();
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEdevice::get_servicesUUIDs() const
{
  return m_svcs->allKeysGet();
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEdevice::get_services() const
{
  return m_svcs->allValuesGet();
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEdevice::serviceFind(cr_EsVariant uuid) const
{
  ES_ASSERT(m_svcs);
  if(m_svcs->keyExists(uuid))
    return m_svcs->valueGet(uuid);

  return EsVariant::null();
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEdevice::serviceGet(cr_EsVariant uuid) const
{
  const EsVariant& result = serviceFind(uuid);

  if(result.isEmpty())
    EsException::Throw(
      _("Could not find service with UUID: %s in device %s(%s)"),
      uuid.trace(),
      get_id(),
      get_name()
    );

  return result;
}
//--------------------------------------------------------------------------------

void EsBluetoothLEdevice::rssiQueryReflected()
{
  rssiQuery();
}
//--------------------------------------------------------------------------------

void EsBluetoothLEdevice::disconnectReflected()
{
  disconnect();
}
//--------------------------------------------------------------------------------

void EsBluetoothLEdevice::discoverReflected()
{
  discover();
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEdevice::reliableWriteBeginReflected()
{
  return reliableWriteBegin();
}
//--------------------------------------------------------------------------------

void EsBluetoothLEdevice::reliableWriteAbortReflected()
{
  reliableWriteAbort();
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEdevice::reliableWriteExecuteReflected()
{
  return reliableWriteExecute();
}
//--------------------------------------------------------------------------------

EsBluetoothLEservice* EsBluetoothLEdevice::serviceFind(const GUID& uuid) const
{
  const EsVariant& vsvc = serviceFind(
    EsUUID::toVariant(uuid)
  );

  return ES_VARIANT_TO_OBJECTPTR<EsBluetoothLEservice>( vsvc );
}
//--------------------------------------------------------------------------------

EsBluetoothLEservice* EsBluetoothLEdevice::serviceGet(const GUID& uuid) const
{
  const EsVariant& vsvc = serviceGet(
    EsUUID::toVariant(uuid)
  );

  return ES_VARIANT_TO_OBJECTPTR<EsBluetoothLEservice>(vsvc);
}
//--------------------------------------------------------------------------------

const EsBluetoothRadio* EsBluetoothLEdevice::radioGet() const
{ 
  return ES_INTFPTR_TO_OBJECTPTR<EsBluetoothRadio>( m_radio );
}
//--------------------------------------------------------------------------------

EsBluetoothRadio* EsBluetoothLEdevice::radioGet()
{
  return ES_INTFPTR_TO_OBJECTPTR<EsBluetoothRadio>(m_radio);
}
//--------------------------------------------------------------------------------

void EsBluetoothLEdevice::servicesCleanup()
{
  while( !m_svcs->isEmpty() )
  {
    EsBluetoothLEservice* svc = ES_VARIANT_TO_OBJECTPTR<EsBluetoothLEservice>(m_svcs->itemGet(0)[1]);
    ES_ASSERT(svc);

    svc->m_dev.reset();

    m_svcs->itemDelete( 0 );
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsBluetoothLEdeviceEnumerator, esT("BLE device enumerator class"))
  // Properties
  ES_DECL_PROP_INFO_RO(               EsBluetoothLEdeviceEnumerator, radio, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsBluetoothLEdeviceEnumerator, devicesIDs, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsBluetoothLEdeviceEnumerator, devices, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)

  // Reflected services
  ES_DECL_REFLECTED_METHOD_INFO(      EsBluetoothLEdeviceEnumerator, enumerateReflected, enumerate, void_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsBluetoothLEdeviceEnumerator, stop, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsBluetoothLEdeviceEnumerator, reset, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(      EsBluetoothLEdeviceEnumerator, deviceFindByNameReflected, deviceFindByName, EsVariant_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(      EsBluetoothLEdeviceEnumerator, deviceFindByIDreflected, deviceFindByID, EsVariant_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(      EsBluetoothLEdeviceEnumerator, deviceFindByAddressReflected, deviceFindByAddress, EsVariant_CallConst_cr_EsString, NO_METHOD_DESCR)

  // Constructor
  ES_DECL_REFLECTED_CTOR_INFO(        EsBluetoothLEdeviceEnumerator, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//--------------------------------------------------------------------------------

// BT LE devices enumerator
//
EsBluetoothLEdeviceEnumerator::EsBluetoothLEdeviceEnumerator( const EsBaseIntfPtr& radio ) :
m_radio( radio ),
m_doInquiry(false),
m_inquiryTmo(0)
{
  m_devs = EsAssocContainer::create(
    asBaseIntf()
  );
}
//---------------------------------------------------------------------------

EsVariant EsBluetoothLEdeviceEnumerator::NEW(cr_EsVariant radio)
{
  if(radio.isEmpty())
    EsException::ThrowNoValue();

  if(!radio.isKindOf(EsBluetoothRadio::classNameGetStatic()))
    EsException::ThrowObjectTypeIsIncompatibleWith(
      radio.typeNameGet(),
      classNameGetStatic()
    );

  std::unique_ptr<EsBluetoothLEdeviceEnumerator> pobj = ES_MAKE_UNIQUE(
    EsBluetoothLEdeviceEnumerator,
    radio.asExistingObject()
  );
  ES_ASSERT(pobj);

  pobj->m_dynamic = true;

  return pobj.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEdeviceEnumerator::get_radio() const
{
  return m_radio;
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEdeviceEnumerator::get_devicesIDs() const 
{
  return m_devs->allKeysGet();
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEdeviceEnumerator::get_devices() const
{
  return m_devs->allValuesGet();
}
//--------------------------------------------------------------------------------

void EsBluetoothLEdeviceEnumerator::enumerateReflected(cr_EsVariant doIniquiry, cr_EsVariant inquiryTmo)
{
  enumerate(
    doIniquiry.asBool(),
    inquiryTmo.asULong()
  );
}
//--------------------------------------------------------------------------------

void EsBluetoothLEdeviceEnumerator::enumerate(bool doInquiry, ulong inquiryTmo)
{
  reset();

  m_doInquiry = doInquiry;
  m_inquiryTmo = inquiryTmo;
  enumerate();
}
//--------------------------------------------------------------------------------

void EsBluetoothLEdeviceEnumerator::stop()
{
  // TODO:
}
//--------------------------------------------------------------------------------

void EsBluetoothLEdeviceEnumerator::reset()
{
  stop();
  m_devs->clear();
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEdeviceEnumerator::deviceFindByNameReflected(const EsString& name) const
{
  for(ulong idx = 0; idx < m_devs->countGet(); ++idx)
  {
    const EsVariant& item = m_devs->itemGet(idx);
    ES_ASSERT(!item.isEmpty());
    ES_ASSERT(!item.isVariantCollection());

    const EsVariant& dev = item[1];
    if(name == dev.propertyGet(esT("name")).asString())
      return dev;
  }

  return EsVariant::null();
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEdeviceEnumerator::deviceFindByIDreflected(const EsString& id) const
{
  if(m_devs->keyExists(id))
    return m_devs->valueGet(id);

  return EsVariant::null();
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLEdeviceEnumerator::deviceFindByAddressReflected(const EsString& addr) const
{
  for(ulong idx = 0; idx < m_devs->countGet(); ++idx)
  {
    const EsVariant& item = m_devs->itemGet(idx);
    ES_ASSERT(!item.isEmpty());
    ES_ASSERT(!item.isVariantCollection());

    const EsVariant& dev = item[1];
    if(addr == dev.propertyGet(esT("address")).asString())
      return dev;
  }

  return EsVariant::null();
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEdeviceEnumerator::onDeviceEnumerated(const EsReflectedClassIntf::Ptr& pdev)
{
  ES_ASSERT(pdev);

  const EsVariant& id = pdev->propertyGet(esT("id"));
  if(m_devs->keyExists(id))
    m_devs->valueSet(
      id,
      pdev
    );
  else
    m_devs->newValueSet(
      id,
      pdev
    );

  return true;
}
//---------------------------------------------------------------------------

EsBluetoothLEdevice* EsBluetoothLEdeviceEnumerator::deviceFindByName(const EsString& name) const
{
  return ES_VARIANT_TO_OBJECTPTR<EsBluetoothLEdevice>(
    deviceFindByNameReflected(name)
  );
}
//---------------------------------------------------------------------------

EsBluetoothLEdevice* EsBluetoothLEdeviceEnumerator::deviceFindByID(const EsString& id) const
{
  return ES_VARIANT_TO_OBJECTPTR<EsBluetoothLEdevice>(
    deviceFindByIDreflected(id)
  );
}
//--------------------------------------------------------------------------------

EsBluetoothLEdevice* EsBluetoothLEdeviceEnumerator::deviceFindByAddress(const EsString& address) const
{
  return ES_VARIANT_TO_OBJECTPTR<EsBluetoothLEdevice>(
    deviceFindByNameReflected(address)
  );
}
//---------------------------------------------------------------------------
//--------------------------------------------------------------------------------

// Platform-specific BLE classes implementations
//
#if ES_OS_WINDOWS == ES_OS
# include "EsBluetoothLE.win.cxx"
#elif ES_OS_ANDROID == ES_OS
# include "EsBluetoothLE.android.cxx"
#endif

//---------------------------------------------------------------------------
//--------------------------------------------------------------------------------

ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsBluetoothLE, NO_CLASS_DESCR)
  // Reflected static services
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothLE, initialize, void_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothLE, initialized, bool_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothLE, uninitialize, void_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothLE, radiosGet, EsVariant_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(      EsBluetoothLE, resultCodeStringGetReflected, resultCodeStringGet, EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_REFLECTED_SERVICES_INFO_END
//--------------------------------------------------------------------------------

EsVariant EsBluetoothLE::radiosGet()
{
  if( !initialized() )
    initialize();

  return EsBluetoothRadioEnumerator::enumerate();
}
//--------------------------------------------------------------------------------

EsString EsBluetoothLE::resultCodeStringGetReflected(cr_EsVariant code)
{
  return resultCodeStringGet(
    as_<EsBluetoothLEresult>(code.asULong())
  );
}
//--------------------------------------------------------------------------------

EsString EsBluetoothLE::resultCodeStringGet(EsBluetoothLEresult code)
{
  switch( code )
  {
  case EsBluetoothLEresult::gattSuccess:
    return _("Success");
  case EsBluetoothLEresult::gattReadNotPermitted:
    return _("Read not permitted");
  case EsBluetoothLEresult::gattWriteNotPermitted:
    return _("Write not permitted");
  case EsBluetoothLEresult::gattInsufficientAutentication:
    return _("Insufficient authentication");
  case EsBluetoothLEresult::gattRequestNotSupported:
    return _("Request not supported");
  case EsBluetoothLEresult::gattInvalidOffset:
    return _("Invalid offset");
  case EsBluetoothLEresult::gattInvalidAttributeLength:
    return _("Invalid attribute length");
  case EsBluetoothLEresult::gattInsufficientEncryption:
    return _("Insufficient encryption");
  case EsBluetoothLEresult::gattFailure:
    return _("Generic failure");
  }

  return _("Unknown failure");
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_BLUETOOTH_LE
