#include "escorepch.h"
#pragma hdrstop

#include "EsUUID.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// UUID reflected class
//
ES_DECL_BASE_CLASS_INFO_BEGIN(EsUUID, NO_CLASS_DESCR)
  // Constructors
  ES_DECL_REFLECTED_CTOR_INFO(EsUUID, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
   ES_DECL_REFLECTED_CTOR_INFO(EsUUID, EsBaseIntfPtr_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  // Services
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsUUID, compare, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsUUID, asString, EsString_CallConst_bool, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsUUID, streamRead, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsUUID, streamWrite, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  // Class services
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsUUID, generateReflected, generate, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
  // Properties
  ES_DECL_PROP_INFO(EsUUID, string, EsString, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsUUID::EsUUID(const GUID& uuid /*= EsUUID::null()*/) :
m_uuid(uuid)
{
}
//---------------------------------------------------------------------------

EsUUID::EsUUID(const EsString& uuidStr) :
m_uuid(GUID_NULL)
{
  stringSet(uuidStr);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsUUID::create(const GUID& uuid/* = EsUUID::null()*/)
{
  std::unique_ptr<EsUUID> p( new EsUUID(uuid) );
  ES_ASSERT(p.get());
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsString EsUUID::stringGet() const
{
  return EsUtilities::GUIDtoStr(m_uuid);
}
//---------------------------------------------------------------------------

void EsUUID::stringSet(const EsString& str)
{
  m_uuid = EsUtilities::GUIDfromStr(str);
}
//---------------------------------------------------------------------------

bool EsUUID::operator== (const EsUUID& other) const
{
  return 0 == memcmp(&m_uuid, &other.m_uuid, sizeof(GUID));
}
//---------------------------------------------------------------------------

GUID EsUUID::fromVariant(const EsVariant& var)
{
  if( var.isEmpty() )
    return GUID_NULL;
  else if( var.isString() )
    return EsUtilities::GUIDfromStr(var.asString());
  else if( EsVariant::VAR_BIN_BUFFER == var.typeGet() )
  {
    const EsBinBuffer& bb = var.asBinBuffer();
    if( sizeof(GUID)*2 == bb.size() ) //< ASCII
    {
      EsByteString bs( bb.begin(), bb.end() );
      return EsUtilities::GUIDfromStr( EsString::fromAscii(bs) );
    }
    else if( sizeof(GUID) == bb.size() )  //< Binary buffer
    {
      GUID uuid = GUID_NULL;
      memcpy(&uuid, &bb[0], sizeof(GUID));

      return uuid;
    }
  }
  else if( var.isKindOf(classNameGetStatic()) )
  {
    EsReflectedClassIntf::Ptr o = var.asExistingObject();
    EsUUID* uuid = ES_INTFPTR_TO_OBJECTPTR(o, EsUUID);
    ES_ASSERT(uuid);

    return uuid->uuidGet();
  }

  EsException::Throw(
    esT("Could not convert variant to UUID")
  );

  // Pacify compilers
  return GUID_NULL;
}
//---------------------------------------------------------------------------

EsVariant EsUUID::toVariant(const GUID& uuid)
{
  return EsUUID::create(uuid);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsUUID::NEW()
{
  return EsUUID::create();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsUUID::NEW(cr_EsVariant val)
{
  return EsUUID::create(
    EsUUID::fromVariant(val)
  );
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsUUID::generateReflected()
{
  return EsUUID::create(
    EsUUID::generate()
  );
}
//---------------------------------------------------------------------------

EsVariant EsUUID::compare(cr_EsVariant val) const
{
  const GUID& uuid = EsUUID::fromVariant(val);
  long result = memcmp(&uuid, &m_uuid, sizeof(GUID));

  return result;
}
//---------------------------------------------------------------------------

EsString EsUUID::asString(bool canonical) const
{
  return EsUtilities::GUIDtoStr(
    m_uuid,
    canonical
  );
}
//---------------------------------------------------------------------------

EsString EsUUID::get_string() const
{
  return asString(true);
}
//---------------------------------------------------------------------------

void EsUUID::set_string(const EsString& str)
{
  stringSet(str);
}
//---------------------------------------------------------------------------

void EsUUID::streamRead(cr_EsBaseIntfPtr in)
{
  EsStreamIntf::Ptr stream = in;
  ES_ASSERT(stream);

  const EsBinBuffer& bb = stream->valueRead().asBinBuffer();
  ES_ASSERT( sizeof(GUID) == bb.size() );

  memcpy(&m_uuid, &bb[0], sizeof(GUID));
}
//---------------------------------------------------------------------------

void EsUUID::streamWrite(cr_EsBaseIntfPtr out)
{
  EsStreamIntf::Ptr stream = out;
  ES_ASSERT(stream);

  EsBinBuffer bb(
    reinterpret_cast<EsBinBuffer::const_pointer>(&m_uuid),
    reinterpret_cast<EsBinBuffer::const_pointer>(&m_uuid)+sizeof(GUID)
  );
  stream->valueWrite(bb);
}
//---------------------------------------------------------------------------

const GUID& EsUUID::null()
{
  static const GUID sc_null = {0};

  return sc_null;
}
//---------------------------------------------------------------------------

#if ES_OS == ES_OS_WINDOWS
# include "EsUUID.win.cxx"
#elif defined(ES_POSIX_COMPAT)
# include "EsUUID.posix.cxx"
#else
# error EsUUID::generate is not implemented for this platform!
#endif


