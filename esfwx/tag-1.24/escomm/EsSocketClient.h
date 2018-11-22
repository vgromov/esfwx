#ifndef _es_socket_client_h_
#define _es_socket_client_h_

#ifdef ES_COMM_USE_SOCKETS

/// Client socket class
///
class ESCOMM_CLASS EsSocketClient : public EsSocket
{
public:
  EsSocketClient();

protected:
  /// EsSocket services implementation
  ///
  virtual bool checkActive() const ES_OVERRIDE;
  virtual bool doOpen(bool doThrow) ES_OVERRIDE;
  virtual void doClose(bool graceful, bool doThrow) ES_OVERRIDE;

  /// Overridable callbacks
  virtual void onSendFarewell(EsSocket::Impl& io) = 0;
  virtual void onServerFarewellReceived(EsBinBuffer::const_pointer data, ulong len) = 0;

private:
  // Prohibited functionality
  //
  EsSocketClient(const EsSocketClient&) ES_REMOVEDECL;
  EsSocketClient& operator=(const EsSocketClient&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

/// Reflected client socket interface
///
namespace EsReflection {

class ES_INTF_IMPL1(EsSocketClient, EsReflectedClassIntf)

protected:
  class Client : public ::EsSocketClient
  {
  public:
    Client(EsReflection::EsSocketClient& owner);

  protected:
    /// Overridable callbacks implementation
    virtual void onSendFarewell(EsSocket::Impl& io) ES_OVERRIDE;
    virtual void onServerFarewellReceived(EsBinBuffer::const_pointer data, ulong len) ES_OVERRIDE;

  protected:
    EsReflection::EsSocketClient& m_owner;
  };
  friend class Client;

public:
  EsSocketClient();

	ES_DECL_REFLECTED_CLASS_BASE( EsSocketClient )
	ES_DECL_ATTR_HANDLING_STD

  /// Reflected services
  ///

  /// Create client socket of specific socketType
 	ES_DECL_REFLECTED_CLASS_CTOR1(EsVariant, cr_EsVariant);

  /// Open-close
  ES_DECL_REFLECTED_METHOD0(bool, open);
  ES_DECL_REFLECTED_METHOD0(void, close);

  /// Send-receive
  ///

  /// Send data buffer
  ES_DECL_REFLECTED_METHOD2(ulong, send, cr_EsBinBuffer, ulong);

  /// Receive data buffer
  ES_DECL_REFLECTED_METHOD2(EsBinBuffer, receive, ulong, ulong);

  /// Reset socket
  ES_DECL_REFLECTED_METHOD0(void, reset);

  /// Reflected properties
  ///
	ES_DECL_PROPERTY_RO(  isOk, bool)
	ES_DECL_PROPERTY_RO(  isActive, bool)
	ES_DECL_PROPERTY_RO(  error, long)
  ES_DECL_PROPERTY_STD( socketType, ulong)
	ES_DECL_PROPERTY(     address, EsVariant)
	ES_DECL_PROPERTY_STD( doThrow, bool)
	ES_DECL_PROPERTY_WO(  ioHandler, EsVariant)       ///< Attacheable IO handler for the client. Its reflected methods will be called
                                                    ///  in current execution context. It should implement some, or all methods
                                                    ///  with the same names as Clent overridables. Handler implementation must have an additional
                                                    ///  first parameter, in which a non-refcounted reference to this object instance is passed.
                                                    ///  If no Handler is assigned, the server object just does nothing.

protected:
  Client m_cli; ///< Actual client implementation
  EsSocketAddr m_addr;
  EsVariant m_ioHandler;
};

}

#endif // ES_COMM_USE_SOCKETS

#endif // _es_socket_client_h_