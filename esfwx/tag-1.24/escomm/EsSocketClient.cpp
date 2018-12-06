#include "escommpch.h"
#pragma hdrstop

#include "EsSocketClient.h"

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

#ifdef ES_COMM_USE_SOCKETS

//---------------------------------------------------------------------------
// Socket client implementation
//
EsSocketClient::EsSocketClient() :
EsSocket()
{}
//---------------------------------------------------------------------------

// Services implementation
bool EsSocketClient::checkActive() const
{
  return m_impl && (m_impl->isConnected() || m_impl->isMessageOriented());
}
//---------------------------------------------------------------------------

bool EsSocketClient::doOpen(bool doThrow)
{
  ES_ASSERT(m_impl);
  if( !m_impl->isMessageOriented() )
    return m_impl->connect(doThrow);

  return true;
}
//---------------------------------------------------------------------------

void EsSocketClient::doClose(bool graceful, bool doThrow)
{
  ES_ASSERT(m_impl);

  // Try to close connection gracefully
  //
  if( graceful )
    onSendFarewell( *m_impl );

  // Signal client - initiated shutdown
  m_impl->shutdown(
    EsSocketShutdownMode::Transmit,
    doThrow
  );

  if( m_impl->isConnected() &&
      graceful
  )
  {
    // Read residual server response, if any
    EsBinBuffer bb;
    bb.reserve(
      maxInChunkSizeGet()
    );

    EsBinBuffer::value_type chunk[128];
    while( 1 )
    {
      ulong read = m_impl->receive(
        chunk,
        sizeof(chunk),
        1000,
        false
      );

      if( read &&
          static_cast<long>(EsSocketError::None) == m_impl->errorGet()
      )
      {
        bb.append(
          read,
          chunk
        );
      }
      else
        break;
    }

    // Handle received farewell data
    if( bb.size() )
      onServerFarewellReceived(
        bb.data(),
        bb.size()
      );
  }

  m_impl->close(doThrow);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

namespace EsReflection {

ES_DECL_BASE_CLASS_INFO_BEGIN(EsSocketClient, _i("Client network socket"))
  // Reflected services
   ES_DECL_REFLECTED_CTOR_INFO(              EsSocketClient, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(        EsSocketClient, open, bool_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(        EsSocketClient, close, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(        EsSocketClient, send, ulong_Call_cr_EsBinBuffer_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(        EsSocketClient, receive, EsBinBuffer_Call_ulong_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(        EsSocketClient, reset, void_Call, NO_METHOD_DESCR)
  // Reflected property infos declarations
  ES_DECL_PROP_INFO_PERSISTENT(             EsSocketClient, address, EsVariant, _i("Server address"), EsSocketAddr::null(), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_PERSISTENT(             EsSocketClient, doThrow, bool, _i("Throw exceptions on Socket errors"), false, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RESTRICTED_PERSISTENT(  EsSocketClient, socketType, ulong, _i("Client Socket type"), static_cast<ulong>(::EsSocketType::MessageOriented), ES_ENUM(EsSocketType), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsSocketClient, isOk, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsSocketClient, isActive, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                     EsSocketClient, error, long, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_WO(                     EsSocketClient, ioHandler, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END

} // namespace EsReflection

typedef EsReflection::EsSocketClient EsSocketClientReflected;
//---------------------------------------------------------------------------
//--------------------------------------------------------------------------- 

EsSocketClientReflected::Client::Client(EsSocketClientReflected& owner) :
m_owner(owner)
{}
//---------------------------------------------------------------------------

void EsSocketClientReflected::Client::onSendFarewell(EsSocket::Impl& io)
{
  if(
    !m_owner.m_ioHandler.isEmpty() &&
    m_owner.m_ioHandler.hasMethod(
      "onSendFarewell",
      1
    )
  )
  {
    const EsVariant& result = m_owner.m_ioHandler.call(
      "onSendFarewell",
      m_owner.asWeakReference()
    );

    if( !result.isEmpty() )
    {
      const EsBinBuffer& bb = result.asBinBuffer();
      if( io.isMessageOriented() )
      {
        io.sendTo(
          m_owner.m_addr,
          bb.data(),
          bb.size(),
          0,
          m_owner.m_doThrow
        );
      }
      else
      {
        io.send(
          bb.data(),
          bb.size(),
          0,
          m_owner.m_doThrow
        );
      }
    }
  }
}
//---------------------------------------------------------------------------

void EsSocketClientReflected::Client::onServerFarewellReceived(EsBinBuffer::const_pointer data, ulong len)
{
  if(
    !m_owner.m_ioHandler.isEmpty() &&
    m_owner.m_ioHandler.hasMethod(
      "onServerFarewellReceived",
      2
    )
  )
  {
    EsBinBuffer bb(
      data,
      data+len
    );

    m_owner.m_ioHandler.call(
      "onServerFarewellReceived",
      m_owner.asWeakReference(),
      bb
    );
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsSocketClientReflected::EsSocketClient() :
m_cli(*this)
{
  // initialize properties to their default values
  ES_REFLECTED_PROPERTIES_RESET;
}
//---------------------------------------------------------------------------

EsVariant EsSocketClientReflected::NEW(cr_EsVariant socketType)
{
  std::unique_ptr<EsSocketClientReflected> cli( new EsSocketClientReflected );
  ES_ASSERT( cli);

  cli->set_socketType(
    socketType.asULong()
  );
  cli->m_dynamic = true;

  return cli.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

bool EsSocketClientReflected::open()
{
  if( m_cli.isActive() )
    return true;

  ::EsSocketType type = static_cast< ::EsSocketType >(m_socketType);

  return m_cli.open(
    m_addr,
    type
  );
}
//---------------------------------------------------------------------------

void EsSocketClientReflected::close()
{
  if( !m_cli.isActive() )
    return;

  m_cli.close(
    true,     //< Always try to close gracefully
    m_doThrow
  );
}
//---------------------------------------------------------------------------

ulong EsSocketClientReflected::send(cr_EsBinBuffer bb, ulong tmo)
{
  ::EsSocketType type = static_cast< ::EsSocketType >(m_socketType);
  if(::EsSocketType::MessageOriented == type)
  {
    return m_cli.sendTo(
      m_addr,
      bb,
      tmo,
      m_doThrow
    );
  }
  else
    return m_cli.send(
      bb,
      tmo,
      m_doThrow
    );
}
//---------------------------------------------------------------------------

EsBinBuffer EsSocketClientReflected::receive(ulong len, ulong tmo)
{
  ::EsSocketType type = static_cast< ::EsSocketType >(m_socketType);
  if(::EsSocketType::MessageOriented == type)
  {
    const EsDateTime& start = EsDateTime::now();
    EsBinBuffer bb( len );
    EsBinBuffer::pointer pos = bb.data();
    EsBinBuffer::pointer end = pos + len;

    ulong toRead = len;

    do
    {
      ulong chunk = m_cli.receiveFrom(
        m_addr,
        pos,
        toRead,
        tmo,
        m_doThrow
      );

      pos += chunk;
      toRead -= chunk;

      const EsTimeSpan& span = EsDateTime::now() - start;

      ES_DEBUG_TRACE(
        esT("EsSocketClientReflected::receive (MessageOriented): read chunk of %d bytes, %d ms spent"),
        chunk,
        span.get_allMilliseconds()
      );

      if( tmo < span.get_allMilliseconds() )
        break;

    } while(
      (pos < end) &&
      tmo
    );

    bb.resize( end-pos );

    return bb;
  }
  else
    return m_cli.receive(
      len,
      tmo,
      m_doThrow
    );
}
//---------------------------------------------------------------------------

void EsSocketClientReflected::reset()
{
  m_cli.reset();
}
//---------------------------------------------------------------------------

bool EsSocketClientReflected::get_isOk() const
{
  return m_cli.isOk();
}
//---------------------------------------------------------------------------

bool EsSocketClientReflected::get_isActive() const
{
  return m_cli.isActive();
}
//---------------------------------------------------------------------------

long EsSocketClientReflected::get_error() const
{
  return m_cli.errorGet();
}
//---------------------------------------------------------------------------

ulong EsSocketClientReflected::get_socketType() const
{
  return m_socketType;
}
//---------------------------------------------------------------------------

void EsSocketClientReflected::set_socketType(const ulong& type)
{
  ES_ASSERT( static_cast< ::EsSocketType >(type) > ::EsSocketType::None );

#ifdef ES_COMM_USE_BLUETOOTH
  ES_ASSERT( ::EsSocketType::Bluetooth >= static_cast< ::EsSocketType >(type) );
#else
  ES_ASSERT( static_cast< ::EsSocketType >(type) > ::EsSocketType::MessageOriented );
#endif

  m_socketType = type;
}
//---------------------------------------------------------------------------

EsVariant EsSocketClientReflected::get_address() const
{
  return m_addr.clone();
}
//---------------------------------------------------------------------------

void EsSocketClientReflected::set_address(const EsVariant& vaddr)
{
  EsSocketAddr* addr = EsSocketAddr::fromVariant(vaddr);
  ES_ASSERT(addr);

  m_addr = *addr;
}
//---------------------------------------------------------------------------

bool EsSocketClientReflected::get_doThrow() const
{
  return m_doThrow;
}
//---------------------------------------------------------------------------

void EsSocketClientReflected::set_doThrow(const bool& doThrow)
{
  m_doThrow = doThrow;
}
//---------------------------------------------------------------------------

void EsSocketClientReflected::set_ioHandler(const EsVariant& handler)
{
  m_ioHandler = handler;
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_SOCKETS
