#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptPodObjects.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#define ES_IMPL_POD_OBJECT_REFLECTION_START(DataType) \
  ES_DECL_CLASS_INFO_DERIVED_BEGIN(EsScript_## DataType, EsScriptObject, NO_CLASS_DESCR) \
    ES_DECL_PROP_INFO_PERSISTENT(EsScript_## DataType, value, EsVariant, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR) \
    ES_DECL_REFLECTED_METHOD_INFO_STD(EsScript_## DataType, asString, EsString_CallConst, NO_METHOD_DESCR) \
    ES_DECL_REFLECTED_METHOD_INFO_STD(EsScript_## DataType, add, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR) \
    ES_DECL_REFLECTED_METHOD_INFO_STD(EsScript_## DataType, subtract, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR) \
    ES_DECL_REFLECTED_METHOD_INFO_STD(EsScript_## DataType, multiply, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR) \
    ES_DECL_REFLECTED_METHOD_INFO_STD(EsScript_## DataType, divide, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR) \
    ES_DECL_REFLECTED_METHOD_INFO_STD(EsScript_## DataType, compare, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)

#define ES_IMPL_POD_OBJECT_REFLECTION_END(DataType) \
  ES_DECL_CLASS_INFO_END \
  void EsScript_## DataType ::set_value(const EsVariant& val) \
  { validate(val); m_moniker.set_value( To_ ## DataType(val) ); } \
  EsVariant EsScript_## DataType ::get_value() const \
  { return From_ ## DataType( m_moniker.get_value() ); } \
  EsString EsScript_## DataType ::asString() const \
  { return get_value().asString(); } \
  EsVariant EsScript_## DataType ::add(const EsVariant& other) const \
  { return get_value() + other; } \
  EsVariant EsScript_## DataType ::subtract(const EsVariant& other) const \
  { return get_value() - other; } \
  EsVariant EsScript_## DataType ::multiply(const EsVariant& other) const \
  { return get_value() * other; } \
  EsVariant EsScript_## DataType ::divide(const EsVariant& other) const \
  { return get_value() / other; } \
  EsVariant EsScript_## DataType ::compare(const EsVariant& other) const \
  { const EsVariant& tmp = get_value(); return (tmp < other) ? static_cast<long>(-1) : ((tmp > other) ? static_cast<long>(1) : static_cast<long>(0)); } \
  ES_IMPL_INTF_METHOD(bool, EsScript_## DataType ::internalBinBufferSet)(EsBinBuffer::const_pointer& pos, EsBinBuffer::const_pointer end) { \
    return m_moniker.internalBinBufferSet(pos, end); } \
  void EsScript_## DataType ::validate(const EsVariant& val) const \
  { EsScriptObject::validate(val); Check_ ## DataType ## _Range(val); }

#define ES_IMPL_POD_OBJECT(DataType) \
  ES_IMPL_POD_OBJECT_REFLECTION_START(DataType) \
  ES_IMPL_POD_OBJECT_REFLECTION_END(DataType)

// define POD objects implementation
ES_IMPL_POD_OBJECT(esU8)
ES_IMPL_POD_OBJECT(esU16)
ES_IMPL_POD_OBJECT(esU32)
ES_IMPL_POD_OBJECT(esU64)
ES_IMPL_POD_OBJECT(esI8)
ES_IMPL_POD_OBJECT(esI16)
ES_IMPL_POD_OBJECT(esI32)
ES_IMPL_POD_OBJECT(esI64)
ES_IMPL_POD_OBJECT(esF)
ES_IMPL_POD_OBJECT(esD)

// use specialized POD implementation for esDT, as its value is EsDateTime object
ES_IMPL_POD_OBJECT_REFLECTION_START(esDT)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScript_esDT, streamWrite, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScript_esDT, streamRead, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
ES_IMPL_POD_OBJECT_REFLECTION_END(esDT)

void EsScript_esDT::streamWrite(cr_EsBaseIntfPtr p)
{
  EsStreamIntf::Ptr stream = p;
  ES_ASSERT(stream);
  const EsVariant& val = get_value();
  esDT dt = EsDateTime::fromVariant(val);
  stream->valueWrite(dt);
}

void EsScript_esDT::streamRead(cr_EsBaseIntfPtr p)
{
  EsStreamIntf::Ptr stream = p;
  ES_ASSERT(stream);
  const EsVariant& val = stream->valueRead();
  set_value( EsDateTime::toVariant(val.asULLong()) );
}

