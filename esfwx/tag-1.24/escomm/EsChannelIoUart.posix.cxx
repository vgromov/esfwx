// POSIX-specific UART channel methods implementation.
// NB! Do not include this file explicitly in project, it's got implicitly
// included in EsChannelIoUart class code.
//

#ifdef ES_COMM_USE_CHANNEL_UART

#include <termios.h>
#include <sys/ioctl.h>
#if ES_OS != ES_OS_ANDROID
# include <term.h>
#endif

// internal consts
enum {
  ES_UART_READ_SLEEP_GRANULARITY = 1,
};

EsChannelIoUart::EsChannelIoUart() :
m_sysError(0),
m_lineError(0),
m_com(-1)
{
  // initialize properties to their default values
  ES_REFLECTED_PROPERTIES_RESET;
}
//---------------------------------------------------------------------------

const EsVariant& EsChannelIoUart::baudsGet()
{
  static EsVariant sc_bauds =
    EsVariant(EsVariant::VAR_VARIANT_COLLECTION)
    .addToVariantCollection(2400)
    .addToVariantCollection(4800)
    .addToVariantCollection(9600)
#ifdef B14400
    .addToVariantCollection(14400)
#endif
    .addToVariantCollection(19200)
    .addToVariantCollection(38400)
    .addToVariantCollection(57600)
    .addToVariantCollection(115200)
#ifdef B128000
    .addToVariantCollection(128000)
#endif
#ifdef B230400
    .addToVariantCollection(230400)
#endif
#ifdef B256000
    .addToVariantCollection(256000)
#endif
#ifdef B460800
    .addToVariantCollection(460800)
#endif
#ifdef B500000
    .addToVariantCollection(500000)
#endif
#ifdef B576000
    .addToVariantCollection(576000)
#endif
#ifdef B921600
    .addToVariantCollection(921600)
#endif
#ifdef B1000000
    .addToVariantCollection(1000000)
#endif
#ifdef B1152000
    .addToVariantCollection(1152000)
#endif
#ifdef B1500000
    .addToVariantCollection(1500000)
#endif
#ifdef B2000000
    .addToVariantCollection(2000000)
#endif
#ifdef B2500000
    .addToVariantCollection(2500000)
#endif
#ifdef B3000000
    .addToVariantCollection(3000000)
#endif
#ifdef B3500000
    .addToVariantCollection(3500000)
#endif
#ifdef B4000000
    .addToVariantCollection(4000000)
#endif
    ;

  return sc_bauds;
}
//---------------------------------------------------------------------------

void EsChannelIoUart::set_port(const EsString& val)
{
  EsMutexLocker lock(m_mx);
  if( m_port != val )
  {
    if( -1 < m_com )
      EsException::Throw( _("Cannot change port while channel is active."));
    m_port = val;
  }
}
//---------------------------------------------------------------------------

// internal baud setter service
bool EsChannelIoUart::internalSetBaud(const long& val, bool doThrow)
{
  if( m_baud != val )
  {
    if( -1 < m_com )
    {
      if( doThrow )
        EsException::Throw( _("Cannot change baud rate while channel is active."));
      else
        return false;
    }

    m_baud = val;
  }

  return true;
}
//---------------------------------------------------------------------------

void EsChannelIoUart::set_bits(const long& val)
{
  EsMutexLocker lock(m_mx);
  if( m_bits != val )
  {
    if( -1 < m_com )
      EsException::Throw( _("Cannot change data bit length while channel is active."));
    m_bits = val;
  }
}
//---------------------------------------------------------------------------

void EsChannelIoUart::set_parity(const long &val)
{
  EsMutexLocker lock(m_mx);
  if( m_parity != val )
  {
    if( -1 < m_com )
      EsException::Throw( _("Cannot change parity while channel is active."));
    m_parity = val;
  }
}
//---------------------------------------------------------------------------

void EsChannelIoUart::set_stopBits(const long &val)
{
  EsMutexLocker lock(m_mx);
  if( m_stopBits != val )
  {
    if( -1 < m_com )
      EsException::Throw( _("Cannot change stop bits while channel is active."));
    m_stopBits = val;
  }
}
//---------------------------------------------------------------------------

void EsChannelIoUart::set_rxBuffLen(const long &val)
{
  EsMutexLocker lock(m_mx);
  if( m_rxBuffLen != val )
  {
    if( -1 < m_com )
      EsException::Throw( _("Cannot change RX buffer length while channel is active."));
    m_rxBuffLen = val;
  }
}
//---------------------------------------------------------------------------

void EsChannelIoUart::set_txBuffLen(const long &val)
{
  EsMutexLocker lock(m_mx);
  if( m_txBuffLen != val )
  {
    if( -1 < m_com )
      EsException::Throw( _("Cannot change TX buffer length while channel is active."));
    m_txBuffLen = val;
  }
}
//---------------------------------------------------------------------------

// decode comm line error into string
EsString EsChannelIoUart::decodeLineError(esU32 err)
{
  EsString result;

#if ES_OS != ES_OS_MAC
  if( err & CE_MODE )
    result += _("The requested mode is not supported, or the file handle is invalid.");
  if( err & CE_BREAK )
  {
    if( !result.empty() )
        result += esT('\n');
      result += _("The hardware detected a break condition.");
  }
  if( err & CE_FRAME )
  {
    if( !result.empty() )
      result += esT('\n');
    result += _("The hardware detected a framing error.");
  }
  if( err & CE_IOE )
  {
    if( !result.empty() )
      result += esT('\n');
    result += _("An I/O error occurred during communications with the device.");
  }
  if( err & CE_OVERRUN )
  {
    if( !result.empty() )
      result += esT('\n');
    result += _("A character-buffer overrun has occurred. The next character is lost.");
  }
  if( err & CE_RXOVER )
  {
    if( !result.empty() )
      result += esT('\n');
    result += _("An input buffer overflow has occurred. There is either no room in the input buffer, or a character was received after the end-of-file (EOF) character.");
  }
  if( err & CE_RXPARITY )
  {
    if( !result.empty() )
      result += esT('\n');
    result += _("The hardware detected a parity error.");
  }
  if( err & CE_TXFULL )
  {
    if( !result.empty() )
      result += esT('\n');
    result += _("The application tried to transmit a character, but the output buffer was full.");
  }
#endif

  return result;
}
//---------------------------------------------------------------------------

void EsChannelIoUart::internalLineBreak()
{
  if( -1 == tcsendbreak(m_com, 0) )
    m_sysError = EsUtilities::osErrorCodeGet();
}
//---------------------------------------------------------------------------

void EsChannelIoUart::lineBreak()
{
  EsMutexLocker lock(m_mx);
  if( -1 < m_com )
    internalLineBreak();
}
//---------------------------------------------------------------------------

// reset error & copmm state
void EsChannelIoUart::resetError()
{
  m_sysError = m_lineError = 0;
}
//---------------------------------------------------------------------------

static bool uartBaudSet(ulong baud, termios& options)
{
  speed_t speed = 0;
  switch( baud )
  {
  case 2400:
    speed = B2400;
    break;
  case 4800:
    speed = B4800;
    break;
  case 9600:
    speed = B9600;
    break;
#ifdef B14400
  case 14400:
    speed = B14400;
    break;
#endif
  case 19200:
    speed = B19200;
    break;
  case 38400:
    speed = B38400;
    break;
  case 57600:
    speed = B57600;
    break;
  case 115200:
    speed = B115200;
    break;
#ifdef B128000
  case 128000:
    speed = B128000;
    break;
#endif
#ifdef B230400
  case 230400:
    speed = B230400;
    break;
#endif
#ifdef B256000
  case 256000:
    speed = B256000;
    break;
#endif
#ifdef B460800
  case 460800:
    speed = B460800;
    break;
#endif
#ifdef B500000
  case 500000:
    speed = B500000;
    break;
#endif
#ifdef B576000
  case 576000:
    speed = B576000;
    break;
#endif
#ifdef B921600
  case 921600:
    speed = B921600;
    break;
#endif
#ifdef B1000000
  case 1000000:
    speed = B1000000;
    break;
#endif
#ifdef B1152000
  case 1152000:
    speed = B1152000;
    break;
#endif
#ifdef B1500000
  case 1500000:
    speed = B1500000;
    break;
#endif
#ifdef B2000000
  case 2000000:
    speed = B2000000;
    break;
#endif
#ifdef B2500000
  case 2500000:
    speed = B2500000;
    break;
#endif
#ifdef B3000000
  case 3000000:
    speed = B3000000;
    break;
#endif
#ifdef B3500000
  case 3500000:
    speed = B3500000;
    break;
#endif
#ifdef B4000000
  case 4000000:
    speed = B4000000;
    break;
#endif
  default:
    return false;
  }

  int ierr = cfsetispeed(&options, speed);
  int oerr = cfsetospeed(&options, speed);

  return -1 != ierr && -1 != oerr;
}
//---------------------------------------------------------------------------

static void uartParitySet(ulong parity, termios& options)
{
  switch( static_cast<EsUartIoParity>(parity) )
  {
  case EsUartIoParity::None:
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~PARODD;
    options.c_iflag &= ~(INPCK | ISTRIP | PARMRK);
    break;
  case EsUartIoParity::Odd:
    options.c_cflag |= PARENB;
    options.c_cflag |= PARODD;
    options.c_iflag &= ~(INPCK | ISTRIP | PARMRK);
    options.c_iflag |= (INPCK | ISTRIP);
    break;
  case EsUartIoParity::Even:
    options.c_cflag |= PARENB;
    options.c_cflag &= ~PARODD;
    options.c_iflag &= ~(INPCK | ISTRIP | PARMRK);
    options.c_iflag |= (INPCK | ISTRIP);
    break;
  case EsUartIoParity::Mark:
    options.c_cflag |= PARENB;
    options.c_cflag &= ~PARODD;
    options.c_iflag |= (INPCK | ISTRIP | PARMRK);
    break;
  case EsUartIoParity::Space:
    EsException::Throw(esT("UART Space parity is not supported on POSIX platform"));
    break;
  }
}
//---------------------------------------------------------------------------

static void uartByteSizeSet(ulong bits, termios& options)
{
  options.c_cflag &= ~CSIZE; //< Mask the character size bits

  switch( bits )
  {
  case 5:
    options.c_cflag |= CS5;
    break;
  case 6:
    options.c_cflag |= CS6;
    break;
  case 7:
    options.c_cflag |= CS7;
    break;
  case 8:
    if( options.c_iflag & ISTRIP )
      options.c_cflag |= CS7;
    else
      options.c_cflag |= CS8;
    break;
  default:
    EsException::Throw(esT("UART byte size %d is not supported"), bits);
  }
}
//---------------------------------------------------------------------------

static void uartStopBitsSet(ulong stopBits, termios& options)
{
  switch( static_cast<EsUartIoStopBits>(stopBits) )
  {
  case EsUartIoStopBits::_1:
    options.c_cflag &= ~CSTOPB;
    break;
  case EsUartIoStopBits::_2:
    options.c_cflag |= CSTOPB;
    break;
  default:
    EsException::Throw(esT("UART stop bit config %d is not supproted"), stopBits);
  }
}
//---------------------------------------------------------------------------

bool EsChannelIoUart::internalOpen()
{
  bool result = false;
  m_sysError = m_lineError = 0;
  const EsByteString& port = EsString::toUtf8(m_port);
  m_com = ::open(
    port.c_str(),
    O_RDWR    |
    O_NOCTTY  |
    O_NDELAY
  );

  if( -1 == m_com )
    m_sysError = EsUtilities::osErrorCodeGet();
  else
  {
    if( -1 == fcntl(m_com, F_SETFL, 0) ) //< We're open, so now we may clear NDELAY option
      m_sysError = EsUtilities::osErrorCodeGet();
    else
    {
      termios options;

      // Get current port options
      if( -1 == tcgetattr(m_com, &options) )
        m_sysError = EsUtilities::osErrorCodeGet();
      else
      {
        // Configure port options
        if( !uartBaudSet(m_baud, options) )
          m_sysError = EsUtilities::osErrorCodeGet();
        else
        {
          // Disable hardware flow control
          options.c_cflag &= ~CRTSCTS;

          // Local port owner - ignore modem status lines
          // Enable reciever
          options.c_cflag |= (CLOCAL | CREAD);

          // Prepare for non-canonical IO
          options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
          // No software flow control
          options.c_iflag &= ~(IXON | IXOFF | IXANY);
          // Use raw character input
          options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
          // Use raw output
          options.c_oflag &= ~OPOST;

          // Comm chars timeouts. Fetch bytes as they become available
          options.c_cc[VMIN] = 0;
          options.c_cc[VTIME] = 1;

          // Configure port parity
          uartParitySet(
            m_parity,
            options
          );

          // Configure byte size
          uartByteSizeSet(
            m_bits,
            options
          );

          // Configure stop bits
          uartStopBitsSet(
            m_stopBits,
            options
          );

          // Update port options
          if( -1 == tcsetattr(m_com, TCSANOW, &options) )
            m_sysError = EsUtilities::osErrorCodeGet();
          else
          {
            internalReset();
            ES_DUAL_CALL1_NR(
              m_monitor,
              EsChannelIoMonitorIntf,
              channelOpened,
              internalIdGet()
            );

            result = true;
          }
        }
      }
    }
  }

  if( m_sysError )
  {
    ::close(m_com);
    m_com = -1;

    checkLogLastError();
  }

  return result;
}
//---------------------------------------------------------------------------

void EsChannelIoUart::internalClose()
{
  if( -1 < m_com )
  {
    internalReset();
    ::close(m_com);

    m_com = -1;
    ES_DUAL_CALL1_NR(
      m_monitor,
      EsChannelIoMonitorIntf,
      channelClosed,
      internalIdGet()
    );
  }
}
//---------------------------------------------------------------------------

// port manipulation
ES_IMPL_INTF_METHOD(bool, EsChannelIoUart::open)()
{
  EsMutexLocker lock(m_mx);
  if( -1 < m_com )
    return true;
  else
    return internalOpen();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsChannelIoUart::isOpen)() const
{
  EsMutexLocker lock(m_mx);
  return -1 < m_com;
}
//---------------------------------------------------------------------------

esU32 EsChannelIoUart::internalPutBytes(const esU8* data, esU32 len, esU32 tmo)
{
  bool timeout = false;
  const esU8* pos = data;
  const esU8* end = data+len;

  if( m_com != 0 )
  {
    resetError();
    while( !timeout &&
          !checkLogLastError() &&
          !internalIsBreaking() &&
          pos < end )
    {
      int result = 0;
      // EsThread::Yield();
      result = write(
        m_com,
        data,
        len
      );

      if( -1 == result )
      {
        result = 0;
        m_sysError = EsUtilities::osErrorCodeGet();
        if( EAGAIN == m_sysError )
          m_sysError = 0;
      }

      // send "bytes sent" event
      if( result )
          ES_DUAL_CALL2_NR(
            m_monitor,
            EsChannelIoMonitorIntf,
            channelBytesSent,
            internalIdGet(),
            EsBinBuffer(
              data,
              data+result
            )
          );

      // move to the next buffer position
      pos += result;
    }

    if( internalIsBreaking() )
      internalLineBreak();
  }

  return pos-data;
}
//---------------------------------------------------------------------------

esU32 EsChannelIoUart::internalGetBytes(esU8* data, esU32 len, esU32 tmo)
{
  bool timeout = false;
  esU32 localTmo = tmo;
  esU8* pos = data;
  esU8* end = data+len;

  if( 0 != m_com && len )
  {
    resetError();
    while( !timeout &&
          !checkLogLastError() &&
          pos < end &&
          !internalIsBreaking() )
    {
      esU32 toRead = std::min(
        queryUnreadBytes(),
        static_cast<esU32>(end-pos)
      );

      if( toRead )
      {
        localTmo = tmo; // reset local timeout
        int result = 0;
        m_sysError = 0;

        result = read(m_com, pos, toRead);
        if( -1 == result )
        {
          result = 0;
          m_sysError = EsUtilities::osErrorCodeGet();
          if( EAGAIN == m_sysError )
            m_sysError = 0;
        }

        // send "bytes received" event
        if( result )
          ES_DUAL_CALL2_NR(
            m_monitor,
            EsChannelIoMonitorIntf,
            channelBytesReceived,
            internalIdGet(),
            EsBinBuffer(
              data,
              data+result
            )
          );

        // move to the next buffer position
        pos += result;
      }
      else // enter bytes polling
      {
        if( tmo )
          EsThread::sleep(ES_UART_READ_SLEEP_GRANULARITY);

        if( localTmo > ES_UART_READ_SLEEP_GRANULARITY )
          localTmo -= ES_UART_READ_SLEEP_GRANULARITY;
        else
        {
          timeout = true;
          if( m_resetOnRxTmo )
            internalReset();
        }
      }
    }
  }

  return pos-data;
}
//---------------------------------------------------------------------------

esU32 EsChannelIoUart::queryUnreadBytes()
{
  int bytes = 0;
  if( -1 == ioctl(m_com, FIONREAD, &bytes) )
    m_sysError = EsUtilities::osErrorCodeGet();

  return bytes;
}
//---------------------------------------------------------------------------

esU32 EsChannelIoUart::queryUnsentBytes()
{
  int bytes = 0;
  if( -1 == ioctl(m_com, TIOCOUTQ, &bytes) )
    m_sysError = EsUtilities::osErrorCodeGet();

  return bytes;
}
//---------------------------------------------------------------------------

void EsChannelIoUart::internalReset()
{
  if( -1 < m_com )
    resetError();
}
//---------------------------------------------------------------------------
#endif // ES_COMM_USE_CHANNEL_UART

//---------------------------------------------------------------------------
// UART devices enumerator impl. Look in platform-specific implementation files
//

EsUartEnumerator::EsUartEnumerator()
{}
//---------------------------------------------------------------------------

