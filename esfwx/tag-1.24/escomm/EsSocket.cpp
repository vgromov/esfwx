#include "escommpch.h"
#pragma hdrstop

#include "EsSocket.h"
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_SOCKETS

# ifdef ES_COMM_USE_BLUETOOTH
#   include "EsBluetooth.h"
# endif
//---------------------------------------------------------------------------

// Internal implementation flags
enum {
  flagRawResolved   = 0x0001,
  flagNameResolved  = 0x0002,
  flagSvcResolved   = 0x0004,
  flagPortResolved  = 0x0008,
  flagConnected     = 0x0010,
  flagBound         = 0x0020,
  flagListening     = 0x0040,
};
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
__fastcall
#endif
EsSocketException::EsSocketException(EsSocketError code, const EsString& msg) :
EsException(static_cast<long>(code),
  EsException::severityGeneric,
  EsException::facilityEsSockets,
  msg.empty() ?
    EsSockets::errorStringGet(static_cast<long>(code)) :
    msg
)
{
}
//---------------------------------------------------------------------------

void EsSocketException::Throw(EsSocketError code, const EsString& msg /*= EsString::null()*/)
{
  throw EsSocketException(code, msg);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

// Common address functionality implementation
//
const EsString& EsSocketAddr::Impl::rawGet(bool passive) const
{
  const_cast<EsSocketAddr::Impl*>(this)->rawResolve(passive);

  return m_raw;
}
//---------------------------------------------------------------------------

const EsString& EsSocketAddr::Impl::nameGet() const
{
  const_cast<EsSocketAddr::Impl*>(this)->nameResolve();

  return m_name;
}
//---------------------------------------------------------------------------

void EsSocketAddr::Impl::nameSet(const EsString& name)
{
  m_aliases.clear();
  if( m_name != name )
  {
    m_name = name;
    m_flags |= flagNameResolved;

    if( m_name.empty() )
      rawSet( anyAddrGet() );
    else
      m_flags &= ~flagRawResolved;
  }
}
//---------------------------------------------------------------------------

ulong EsSocketAddr::Impl::portGet() const
{
  if( !(m_flags & flagPortResolved) )
    const_cast<EsSocketAddr::Impl*>(this)->portResolve();

  return m_port;
}
//---------------------------------------------------------------------------

const EsString& EsSocketAddr::Impl::serviceGet() const
{
  if( !(m_flags & flagSvcResolved) )
    const_cast<EsSocketAddr::Impl*>(this)->serviceResolve();

  return m_svc;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#if ES_OS == ES_OS_WINDOWS
# define _WINSOCK_DEPRECATED_NO_WARNINGS
# include <winsock2.h>
# include <ws2tcpip.h>

# ifdef ES_COMM_USE_BLUETOOTH
#   include <ws2bth.h>
# endif

# include "EsSocket.win.cxx"

# ifdef ES_COMM_USE_BLUETOOTH
#   include "EsSocketBluetooth.win.cxx"
# endif

#elif ES_OS == ES_OS_ANDROID

# include "EsSocket.posix.cxx"

# ifdef ES_COMM_USE_BLUETOOTH
#   include "EsSocketBluetooth.android.cxx"
# endif

#elif ES_OS == ES_OS_MAC

# include "EsSocket.posix.cxx"

# ifdef ES_COMM_USE_BLUETOOTH
#   error Bluetooth classic socket is not implemented on this platform!
# endif

#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsSocketAddr, esT("Resolvable Socket address class"))
  /// Services
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsSocketAddr, nameResolve, bool_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsSocketAddr, portResolve, bool_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsSocketAddr, serviceResolve, bool_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsSocketAddr, rawResolve, bool_Call_bool, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsSocketAddr, clone, EsBaseIntfPtr_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsSocketAddr, asRawString, EsString_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsSocketAddr, asReadableString, EsString_CallConst, NO_METHOD_DESCR)
  /// Class services
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsSocketAddr, null, EsVariant_ClassCall, NO_METHOD_DESCR)
  // Ctors
  ES_DECL_REFLECTED_CTOR_INFO(            EsSocketAddr, EsVariant_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(            EsSocketAddr, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  /// Properties
  ES_DECL_PROP_INFO_RO(                   EsSocketAddr, isOk, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                   EsSocketAddr, error, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RESTRICTED_PERSISTENT(EsSocketAddr, addressType, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(EsSocketAddrType::None), ES_ENUM(EsReflection::EsSocketAddrType), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_PERSISTENT(           EsSocketAddr, rawAddress, EsString, NO_PROPERTY_LABEL, EsString::null(), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                      EsSocketAddr, resolvedAddress, EsString, NO_PROPERTY_LABEL, EsString::null(), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                      EsSocketAddr, service, EsString, NO_PROPERTY_LABEL, EsString::null(), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RANGE_PERSISTENT(     EsSocketAddr, port, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(0), static_cast<ulong>(0), static_cast<ulong>(65535), NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

/// Default constructor
EsSocketAddr::EsSocketAddr(
  EsSocketAddrType type /*= EsSocketAddrType::None*/,
  const EsString& addr /*= EsString::null()*/,
  ulong port /*= 0*/) :
m_impl(nullptr)
{
  typeSet(type);

  if( type != EsSocketAddrType::None )
  {
    if( !addr.empty() )
    {
      if( m_impl->rawParse(addr) )
        rawSet(addr);
      else
        nameSet(addr);
    }

    portSet(port);
  }
  else
  {
    if( !addr.empty() || 0 != port )
      EsSockets::errorCheck( static_cast<long>(EsSocketError::AddressTypeNotSpecified) );
  }
}
//---------------------------------------------------------------------------

EsSocketAddr::EsSocketAddr(EsSocketAddrType type,
  const EsString& addr,
  const EsString& svc) :
m_impl(nullptr)
{
  typeSet(type);

  if( type != EsSocketAddrType::None )
  {
    if( !addr.empty() )
    {
      if( m_impl->rawParse(addr) )
        rawSet(addr);
      else
        nameSet(addr);
    }

    serviceSet(svc);
  }
  else
  {
    if( !addr.empty() || !svc.empty() )
      EsSockets::errorCheck( static_cast<long>(EsSocketError::AddressTypeNotSpecified) );
  }
}
//---------------------------------------------------------------------------

EsSocketAddr::EsSocketAddr(const EsSocketAddr& src) :
m_impl(nullptr)
{
  if(src.m_impl)
    m_impl = src.m_impl->clone();
}
//---------------------------------------------------------------------------

EsSocketAddr::~EsSocketAddr()
{
  ES_DELETE(m_impl);
}
//---------------------------------------------------------------------------

#ifdef ES_MODERN_CPP

EsSocketAddr::EsSocketAddr(EsSocketAddr&& src) :
m_impl(src.m_impl)
{
  src.m_impl = nullptr;
}
//---------------------------------------------------------------------------

EsSocketAddr& EsSocketAddr::operator=(EsSocketAddr&& src)
{
  ES_DELETE(m_impl);

  m_impl = src.m_impl;
  src.m_impl = nullptr;

  return *this;
}

#endif
//---------------------------------------------------------------------------

EsSocketAddr& EsSocketAddr::operator=(const EsSocketAddr& src)
{
  ES_DELETE(m_impl);
  if(src.m_impl)
    m_impl = src.m_impl->clone();

  return *this;
}
//---------------------------------------------------------------------------

EsSocketAddr* EsSocketAddr::fromVariant(const EsVariant& in)
{
  if( in.isObject() )
  {
    EsReflectedClassIntf::Ptr obj  = in.asObject();
    if( obj && obj->isKindOf(classNameGetStatic()) )
      return ES_INTFPTR_TO_OBJECTPTR(obj, EsSocketAddr);
  }

  return nullptr;
}
//---------------------------------------------------------------------------

EsString EsSocketAddr::asRawString() const
{
  return const_cast<EsSocketAddr*>(this)->rawGet(true) +
    esT(":") +
    EsString::fromULong(
      portGet()
    );
}
//---------------------------------------------------------------------------

EsString EsSocketAddr::asReadableString() const
{
  return (const_cast<EsSocketAddr*>(this)->nameResolve() ?
      nameGet() :
      rawGet(true)
    ) +
    esT(":") +
    EsString::fromULong(
      portGet()
    );
}
//---------------------------------------------------------------------------

/// Return true if address is set-up and valid
bool EsSocketAddr::isOk() const
{
  return m_impl && static_cast<long>(EsSocketError::None) == m_impl->errorGet();
}
//---------------------------------------------------------------------------

void EsSocketAddr::validCheck() const
{
  if( !m_impl )
    EsSockets::errorCheck(static_cast<long>(EsSocketError::AddressInvalid));
}
//---------------------------------------------------------------------------

/// Get socket address type
EsSocketAddrType EsSocketAddr::typeGet() const
{
  if( m_impl )
    return m_impl->typeGet();

  return EsSocketAddrType::None;
}
//---------------------------------------------------------------------------

long EsSocketAddr::errorGet() const
{
  if( m_impl )
    return m_impl->errorGet();

  return static_cast<long>(EsSocketError::NotInitialized);
}
//---------------------------------------------------------------------------

void EsSocketAddr::errorCheck() const
{
  if( !m_impl->noError() )
    EsSockets::errorCheck( m_impl->errorGet() );
}
//---------------------------------------------------------------------------

/// Retrieve reference to the native address structure
void* EsSocketAddr::nativeGet() const
{
  validCheck();
  return m_impl->nativeGet();
}
//---------------------------------------------------------------------------

/// Retrieve size of the native address structure in bytes
size_t EsSocketAddr::nativeSizeGet() const
{
  validCheck();
  return m_impl->nativeSizeGet();
}
//---------------------------------------------------------------------------

void EsSocketAddr::reconstructFromNative()
{
  validCheck();
  m_impl->reconstructFromNative();
  errorCheck();
}
//---------------------------------------------------------------------------

/// Set socket address type
void EsSocketAddr::typeSet(EsSocketAddrType type)
{
  if( typeGet() != type )
  {
    ES_DELETE(m_impl);

    switch(type)
    {
    case EsSocketAddrType::IPV4:
      m_impl = new EsSocketAddrIPV4;
      break;
#ifdef ES_COMM_USE_BLUETOOTH
    case EsSocketAddrType::Bluetooth:
      m_impl = new EsSocketAddrBT;
      break;
#endif
    default:
      EsSockets::errorCheck(
        static_cast<long>(EsSocketError::AddressTypeNotSupported) );
    }
  }
}
//---------------------------------------------------------------------------

/// Get 'raw' address representation as string (like x.x.x.x for ip v4 or x:x:x:x for ip v6)
const EsString& EsSocketAddr::rawGet(bool passive /*= false*/) const
{
  validCheck();
  const EsString& result = m_impl->rawGet(passive);
  errorCheck();

  return result;
}
//---------------------------------------------------------------------------

/// Set 'raw' address string representation
void EsSocketAddr::rawSet(const EsString& addr)
{
  validCheck();
  m_impl->rawSet(addr);
  errorCheck();
}
//---------------------------------------------------------------------------

/// Get 'resolved' address representation as string (like host name, or device name)
const EsString& EsSocketAddr::nameGet() const
{
  validCheck();
  return m_impl->nameGet();
}
//---------------------------------------------------------------------------

/// Set 'resolved' address string representation
void EsSocketAddr::nameSet(const EsString& name)
{
  validCheck();
  m_impl->nameSet(name);
  errorCheck();
}
//---------------------------------------------------------------------------

/// Get address port value
ulong EsSocketAddr::portGet() const
{
  validCheck();
  ulong result = m_impl->portGet();
  errorCheck();

  return result;
}
//---------------------------------------------------------------------------

/// Set address port value
void EsSocketAddr::portSet(ulong port)
{
  validCheck();
  m_impl->portSet(port);
  errorCheck();
}
//---------------------------------------------------------------------------

/// Get service name
const EsString& EsSocketAddr::serviceGet() const
{
  validCheck();
  return m_impl->serviceGet();
}
//---------------------------------------------------------------------------

/// Set service name
void EsSocketAddr::serviceSet(const EsString& svc)
{
  validCheck();
  m_impl->serviceSet(svc);
  errorCheck();
}
//---------------------------------------------------------------------------

bool EsSocketAddr::operator==(const EsSocketAddr& other) const
{
  if( m_impl )
    return m_impl->isEqualTo(other.m_impl);

  return m_impl == other.m_impl;
}
//---------------------------------------------------------------------------

bool EsSocketAddr::operator<(const EsSocketAddr& other) const
{
  if( m_impl )
    return m_impl->isLessThan(other.m_impl);

  return m_impl < other.m_impl;
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsSocketAddr::clone() const
{
  std::unique_ptr<EsSocketAddr> p( new EsSocketAddr(*this) );
  ES_ASSERT(p);

  p->m_dynamic = true;

  return p.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

/// Resolve raw address to the name
bool EsSocketAddr::rawResolve(bool passive)
{
  validCheck();
  return m_impl->rawResolve(passive);
}
//---------------------------------------------------------------------------

/// Resolve name to the raw address
bool EsSocketAddr::nameResolve()
{
  validCheck();
  return m_impl->nameResolve();
}
//---------------------------------------------------------------------------

bool EsSocketAddr::portResolve()
{
  validCheck();
  return m_impl->portResolve();
}
//---------------------------------------------------------------------------

bool EsSocketAddr::serviceResolve()
{
  validCheck();
  return m_impl->serviceResolve();
}
//---------------------------------------------------------------------------

/// Reset recent error code
void EsSocketAddr::errorReset()
{
  if(m_impl)
    m_impl->errorReset();
}
//---------------------------------------------------------------------------

/// Return default empty address object
const EsSocketAddr& EsSocketAddr::none()
{
  static const EsSocketAddr s_none;
  return s_none;
}
//---------------------------------------------------------------------------

EsVariant EsSocketAddr::null()
{
  return none().clone();
}
//---------------------------------------------------------------------------

EsVariant EsSocketAddr::NEW()
{
#ifdef ES_MODERN_CPP
  std::unique_ptr<EsSocketAddr> p = std::make_unique<EsSocketAddr>();
#else
  std::unique_ptr<EsSocketAddr> p( new EsSocketAddr );
#endif

  ES_ASSERT(p.get());
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

EsVariant EsSocketAddr::NEW(cr_EsVariant vtype, cr_EsVariant vaddr, cr_EsVariant vportOrSvc)
{
  if( !ES_ENUM(EsReflection::EsSocketAddrType).has( vtype ) )
    EsSocketException::Throw(
      EsSocketError::AddressTypeNotSupported,
      _("Trying to create Socket address object with unknown or unsupported address type")
    );

  std::unique_ptr<EsSocketAddr> p;

  EsSocketAddrType type = static_cast<EsSocketAddrType>(
    vtype.asULong()
  );

  if( vportOrSvc.isNumeric() )
#ifdef ES_MODERN_CPP
    p = std::make_unique<EsSocketAddr>(
      type,
      vaddr.asString(),
      vportOrSvc.asULong()
    );
#else
    p.reset(
      new EsSocketAddr(
        type,
        vaddr.asString(),
        vportOrSvc.asULong()
      )
    );
#endif
  else
#ifdef ES_MODERN_CPP
    p = std::make_unique<EsSocketAddr>(
      type,
      vaddr.asString(),
      vportOrSvc.asString()
    );
#else
    p.reset(
      new EsSocketAddr(
        type,
        vaddr.asString(),
        vportOrSvc.asString()
      )
    );
#endif

  ES_ASSERT(p.get());
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

bool EsSocketAddr::get_isOk() const
{
  return isOk();
}
//---------------------------------------------------------------------------

ulong EsSocketAddr::get_error() const
{
  return errorGet();
}
//---------------------------------------------------------------------------

ulong EsSocketAddr::get_addressType() const
{
  return static_cast<ulong>(
    typeGet()
  );
}
//---------------------------------------------------------------------------

void EsSocketAddr::set_addressType(const ulong& type)
{
  typeSet(
    static_cast<EsSocketAddrType>(type)
  );
}
//---------------------------------------------------------------------------

EsString EsSocketAddr::get_rawAddress() const
{
  return rawGet();
}
//---------------------------------------------------------------------------

void EsSocketAddr::set_rawAddress(const EsString& raw)
{
  rawSet(
    raw
  );
}
//---------------------------------------------------------------------------

EsString EsSocketAddr::get_resolvedAddress() const
{
  return nameGet();
}
//---------------------------------------------------------------------------

void EsSocketAddr::set_resolvedAddress(const EsString& addr)
{
  nameSet(
    addr
  );
}
//---------------------------------------------------------------------------

EsString EsSocketAddr::get_service() const
{
  return serviceGet();
}
//---------------------------------------------------------------------------

void EsSocketAddr::set_service(const EsString& svc)
{
  serviceSet(
    svc
  );
}
//---------------------------------------------------------------------------

ulong EsSocketAddr::get_port() const
{
  return portGet();
}
//---------------------------------------------------------------------------

void EsSocketAddr::set_port(const ulong& port)
{
  portSet(
    port
  );
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsSocket::Impl::Impl(const EsSocketAddr& addr, EsSocketType type) :
m_addr(addr),
m_error(0),
m_type(type),
m_flags(0),
m_maxInChunk(0),
m_maxOutChunk(0)
{
  checkSocketDefaultType();
  checkSocketTypeAddrMatch();
}
//---------------------------------------------------------------------------

void EsSocket::Impl::checkSocketDefaultType()
{
  EsSocketAddrType atype = m_addr.typeGet();
  if( EsSocketAddrType::None == atype )
    EsSockets::errorCheck(
      static_cast<long>(EsSocketError::AddressTypeNotSpecified) );

  if( EsSocketType::None == m_type ) //< Defaulting to the ConnectionOriented|Bluetooth socket
  {
    switch( atype )
    {
    case EsSocketAddrType::IPV4:
    case EsSocketAddrType::IPV6:
      m_type = EsSocketType::ConnectionOriented;
      break;
#ifdef ES_COMM_USE_BLUETOOTH
    case EsSocketAddrType::Bluetooth:
      m_type = EsSocketType::Bluetooth;
      break;
#endif
    }
  }
}
//---------------------------------------------------------------------------

void EsSocket::Impl::checkSocketTypeAddrMatch()
{
  switch( m_type )
  {
  case EsSocketType::ConnectionOriented:
  case EsSocketType::MessageOriented:
    if( EsSocketAddrType::IPV4 != m_addr.typeGet() &&
        EsSocketAddrType::IPV6 != m_addr.typeGet() )
      EsException::Throw(_("Incompatible socket type and address; IPV4 or IPV6 address expected"));
    break;
#ifdef ES_COMM_USE_BLUETOOTH
  case EsSocketType::Bluetooth:
    if( EsSocketAddrType::Bluetooth != m_addr.typeGet() )
      EsException::Throw(_("Incompatible socket type and address; Bluetooth address expected"));
    break;
#endif
  }
}
//---------------------------------------------------------------------------

bool EsSocket::Impl::isListening() const
{
  return m_flags & flagListening;
}
//---------------------------------------------------------------------------

bool EsSocket::Impl::isBound() const
{
  return m_flags & flagBound;
}
//---------------------------------------------------------------------------

bool EsSocket::Impl::isConnected() const
{
  return m_flags & flagConnected;
}
//---------------------------------------------------------------------------

bool EsSocket::Impl::isMessageOriented() const
{
  return EsSocketType::MessageOriented == m_type;
}
//---------------------------------------------------------------------------

void EsSocket::Impl::reset()
{
  m_error = static_cast<long>(EsSocketError::None);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Socket base implementation
//
EsSocket::EsSocket(EsSocket::Impl* impl) :
m_impl(impl)
{
  ES_ASSERT(m_impl);
}
//---------------------------------------------------------------------------

EsSocket::EsSocket() :
m_impl(nullptr)
{}
//---------------------------------------------------------------------------

EsSocket::~EsSocket()
{
  // Just in case
  ES_DELETE(m_impl);
}
//---------------------------------------------------------------------------

/// Check if socket was created
bool EsSocket::isOk() const
{
  return nullptr != m_impl;
}
//---------------------------------------------------------------------------

/// Return true if socket is OK and connected
bool EsSocket::isActive() const
{
  if( m_impl )
    return checkActive();

  return false;
}
//---------------------------------------------------------------------------

EsSocketType EsSocket::typeGet() const
{
  if( isOk() )
    return m_impl->typeGet();

  return EsSocketType::None;
}
//---------------------------------------------------------------------------

EsSocket::Impl* EsSocket::implCreate(const EsSocketAddr& addr, EsSocketType type)
{
  return new EsSocketImpl(
    addr,
    type
  );
}
//---------------------------------------------------------------------------

bool EsSocket::open(const EsSocketAddr& addr, EsSocketType type /*= EsSocketType::None*/)
{
  if( nullptr == m_impl )
  {
    m_impl = new EsSocketImpl(addr, type);

    ES_ASSERT(m_impl);
  }
  else
  {
    if( addr != m_impl->addrGet() )
      EsSocketException::Throw(EsSocketError::Generic, _("Socket already opened with other address"));

    if( EsSocketType::None != type && m_impl->typeGet() != type )
      EsSocketException::Throw(EsSocketError::Generic, _("Socket already opened with another socket type specification"));
  }

  return nullptr != m_impl && doOpen(false);
}
//---------------------------------------------------------------------------

/// Close socket, gracefully terminating the connection, if requested
void EsSocket::close(bool graceful /*= true*/, bool doThrow /*= true*/)
{
  if( m_impl )
  {
    doClose(graceful, doThrow);

    ES_DELETE(m_impl);
  }
}
//---------------------------------------------------------------------------

/// Get recent socket error code(s)
long EsSocket::errorGet() const
{
  if( m_impl )
    return m_impl->errorGet();

  return static_cast<long>(EsSocketError::NotInitialized);
}
//---------------------------------------------------------------------------

/// Get address associated with the socket
const EsSocketAddr& EsSocket::addressGet(bool doThrow/* = false*/) const
{
  if( m_impl )
    return m_impl->addrGet();
  else if( doThrow )
    throwNotInitialized();

  // Pacify compilers
  return EsSocketAddr::none();
}
//---------------------------------------------------------------------------

/// Send data
ulong EsSocket::send(const esU8* data, ulong len, ulong tmo, bool doThrow)
{
  if( m_impl )
    return m_impl->send(
      data,
      len,
      ((tmo < tmoMin) ?
        tmoMin :
        tmo
      ),
      doThrow
    );
  else if( doThrow )
    throwNotInitialized();

  // Pacify compiler
  return 0;
}
//---------------------------------------------------------------------------

ulong EsSocket::send(const EsBinBuffer& bb, ulong tmo, bool doThrow)
{
  if( !m_impl )
  {
    if( doThrow )
      throwNotInitialized();

    return 0;
  }

  if( !bb.empty() )
    return send(
      bb.data(),
      bb.size(),
      tmo,
      doThrow
    );

  return 0;
}
//---------------------------------------------------------------------------

/// Send data to address
ulong EsSocket::sendTo(const EsSocketAddr& addr, const esU8* data, ulong len, ulong tmo, bool doThrow)
{
  if( m_impl )
    return m_impl->sendTo(
      addr,
      data,
      len,
      ((tmo < tmoMin) ?
        tmoMin :
        tmo
      ),
      doThrow
    );
  else if( doThrow )
    throwNotInitialized();

  // Pacify compiler
  return 0;
}
//---------------------------------------------------------------------------

ulong EsSocket::sendTo(const EsSocketAddr& addr, const EsBinBuffer& bb, ulong tmo, bool doThrow)
{
  if( !m_impl )
  {
    if( doThrow )
      throwNotInitialized();

    return 0;
  }

  if( !bb.empty() )
    return sendTo(
      addr,
      bb.data(),
      bb.size(),
      tmo,
      doThrow
    );

  return 0;
}
//---------------------------------------------------------------------------

/// Receive data
ulong EsSocket::receive(esU8* data, ulong len, ulong tmo, bool doThrow)
{
  if( m_impl )
    return m_impl->receive(
      data,
      len,
      ((tmo < tmoMin) ?
        tmoMin :
        tmo
      ),
      doThrow
    );
  else if( doThrow )
    throwNotInitialized();

  // Pacify compiler
  return 0;
}
//---------------------------------------------------------------------------

EsBinBuffer EsSocket::receive(ulong len, ulong tmo, bool doThrow)
{
  if( !m_impl )
  {
    if( doThrow )
      throwNotInitialized();

    return EsBinBuffer::null();
  }

  if( len )
  {
    EsBinBuffer result(len);
    ulong read = receive(
      result.data(),
      len,
      tmo,
      doThrow
    );

    result.resize(read);

    return result;
  }

  return EsBinBuffer::null();
}
//---------------------------------------------------------------------------

/// Receive data, capturing sender's address
ulong EsSocket::receiveFrom(EsSocketAddr& addr, esU8* data, ulong len, ulong tmo, bool doThrow)
{
  if( m_impl )
    return m_impl->receiveFrom(
      addr,
      data,
      esMin(
        len,
        static_cast<ulong>(m_impl->maxInChunkGet())
      ),
      ((tmo < tmoMin) ?
        tmoMin :
        tmo
      ),
      doThrow
    );
  else if( doThrow )
    throwNotInitialized();

  // Pacify compiler
  return 0;
}
//---------------------------------------------------------------------------

EsBinBuffer EsSocket::receiveFrom(EsSocketAddr& addr, ulong len, ulong tmo, bool doThrow)
{
  if( !m_impl )
  {
    if( doThrow )
      throwNotInitialized();

    return EsBinBuffer::null();
  }

  if( len )
  {
    EsBinBuffer result(len);
    ulong read = receiveFrom(
      addr,
      result.data(),
      len,
      tmo,
      doThrow
    );

    result.resize(read);

    return result;
  }

  return EsBinBuffer::null();
}
//---------------------------------------------------------------------------

/// Get size of incoming pending data
ulong EsSocket::incomingSizeGet(ulong tmo /*= 0*/, bool doThrow /*= false*/)
{
  if( m_impl )
    return m_impl->incomingSizeGet(
      ((tmo < tmoMin) ?
        tmoMin :
        tmo
      ),
      doThrow
    );
  else if( doThrow )
    throwNotInitialized();

  // Pacify compiler
  return 0;
}
//---------------------------------------------------------------------------

ulong EsSocket::maxInChunkSizeGet() const
{
  if( m_impl )
    return m_impl->maxInChunkGet();

  throwNotInitialized();
  return 0;
}
//---------------------------------------------------------------------------

ulong EsSocket::maxOutChunkSizeGet() const
{
  if( m_impl )
    return m_impl->maxOutChunkGet();

  throwNotInitialized();
  return 0;
}
//---------------------------------------------------------------------------

void EsSocket::throwNotInitialized()
{
  EsSockets::errorCheck(
    static_cast<long>(EsSocketError::NotInitialized) );
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

namespace EsReflection {

ES_DECL_ENUMERATION_INFO(EsSocketError, esT("Socket error status values"))
ES_DECL_ENUMERATION_INFO(EsSocketAddrType, esT("Socket address types"))
ES_DECL_ENUMERATION_INFO(EsSocketType, esT("Socket types"))

}
//---------------------------------------------------------------------------

ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsSockets, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsSockets, initialize, void_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsSockets, uninitialize, void_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsSockets, errorStringGet, EsString_ClassCall_long, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsSockets, errorCheck, void_ClassCall_long, NO_METHOD_DESCR)
ES_DECL_REFLECTED_SERVICES_INFO_END
//---------------------------------------------------------------------------

// Retrieve socket error string
EsString EsSockets::errorStringGet( long err )
{
  switch( err )
  {
  case static_cast<long>(EsSocketError::None):
    return EsString::null();
  case static_cast<long>(EsSocketError::Generic):
    return _("Error");
  case static_cast<long>(EsSocketError::NotInitialized):
    return _("Object not initialized");
  case static_cast<long>(EsSocketError::EngineNotInitialized):
    return _("Socket engine is not initialized");
  case static_cast<long>(EsSocketError::NetDown):
    return _("Network subsystem is down");
  case static_cast<long>(EsSocketError::UnsupportedSocketType):
    return _("Socket type is not supported");
  case static_cast<long>(EsSocketError::InvalidParameter):
    return _("Invalid parameter in socket operation");
  case static_cast<long>(EsSocketError::Busy):
    return _("Socket is busy");
  case static_cast<long>(EsSocketError::NotConnected):
    return _("Socket is not connected");
  case static_cast<long>(EsSocketError::InvalidSocket):
    return _("Invalid (uninitialized) socket");
  case static_cast<long>(EsSocketError::TemporaryError):
    return _("Temporary socket service error; try again later");
  case static_cast<long>(EsSocketError::NonRecoverableError):
    return _("Non recoverable socket service error");
  case static_cast<long>(EsSocketError::AddressTypeNotSupported):
    return _("Socket address type or family is not supported");
  case static_cast<long>(EsSocketError::AddressInvalid):
    return _("Socket address is invalid");
  case static_cast<long>(EsSocketError::AddressTypeNotSpecified):
    return _("Socket address type is not specified");
  case static_cast<long>(EsSocketError::NotEnoughMemory):
    return _("Not enough memory to complete socket service call");
  case static_cast<long>(EsSocketError::HostNotFound):
    return _("Could not find host");
  case static_cast<long>(EsSocketError::NoMoreDescriptors):
    return _("No more socket descriptors available");
  case static_cast<long>(EsSocketError::NoBuffersAvailable):
    return _("No more buffer space available, socket cannot not be created");
  case static_cast<long>(EsSocketError::ProtocolNotSupported):
    return _("Specified protocol is not supported");
  case static_cast<long>(EsSocketError::ProtocolOptionNotSupported):
    return _("Specified option is not supported for this protocol");
  case static_cast<long>(EsSocketError::WrongProtocolForType):
    return _("Protocol type is wrong for this socket type");
  case static_cast<long>(EsSocketError::WrongSocketTypeForAddress):
    return _("Socket type is not supported in specified address family");
  case static_cast<long>(EsSocketError::AddressInUse):
    return _("Address is in use");
  case static_cast<long>(EsSocketError::AlreadyConnected):
    return _("Socket is already connected");
  case static_cast<long>(EsSocketError::OperationNotSupported):
    return _("Socket does not support operation");
  case static_cast<long>(EsSocketError::ConnectionAborted):
    return _("Connection aborted");
  case static_cast<long>(EsSocketError::ConnectionResetByPeer):
    return _("Connection reset by peer");
  case static_cast<long>(EsSocketError::ConnectionDropped):
    return _("Network dropped connection on reset");
  case static_cast<long>(EsSocketError::ConnectionRefused):
    return _("Remote host actively refuses connection");
  case static_cast<long>(EsSocketError::TimedOut):
    return _("Socket operation timed out");
  case static_cast<long>(EsSocketError::OperationCancelled):
    return _("Socket operation was cancelled");
  case static_cast<long>(EsSocketError::NoDataOfRequestedType):
    return _("No data of requested type is available");
  case static_cast<long>(EsSocketError::PointerParameterIsInvalid):
    return _("Socket operation pointer parameter is invalid");
  case static_cast<long>(EsSocketError::CouldNotResolveServiceFromPort):
    return _("Could not resolve service from port");
  case static_cast<long>(EsSocketError::CouldNotResolvePortFromService):
    return _("Could not resolve port from service");
  case static_cast<long>(EsSocketError::AddressNotAvailable):
    return _("Cannot assign requested address. The requested address is not valid in its context");
  case static_cast<long>(EsSocketError::MessageIsTooLong):
    return _("Message is too long. A message sent on a datagram socket is larger than the allowed limit");
  }

  return _("Unknown socket error");
}
//---------------------------------------------------------------------------

/// Check for error code, and, if set, throws an error
void EsSockets::errorCheck( long err )
{
  if( static_cast<long>(EsSocketError::None) != err )
  {
    if( static_cast<long>(EsSocketError::Unknown) > err ||
        static_cast<long>(EsSocketError::_top) <= err )
      err = static_cast<long>(EsSocketError::Unknown);

    EsSocketException::Throw( static_cast<EsSocketError>(err) );
  }
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_SOCKETS

