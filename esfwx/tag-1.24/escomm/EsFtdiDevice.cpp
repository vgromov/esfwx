#include "escommpch.h"
#pragma hdrstop

#include "EsFtdiDevice.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_FTDI
//---------------------------------------------------------------------------

#include "EsFtdiMpsseDefs.h"
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsFtdiDevice, _i("Generic FTDI device"))
  /// Reflected interface stuff
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDevice, EsFtdiDeviceIntf, isValid, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDevice, EsFtdiDeviceIntf, flagsGet, esU32_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDevice, EsFtdiDeviceIntf, typeGet, esU32_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDevice, EsFtdiDeviceIntf, idGet, esU32_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDevice, EsFtdiDeviceIntf, locIdGet, esU32_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDevice, EsFtdiDeviceIntf, serialNumberStrGet, EsString_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDevice, EsFtdiDeviceIntf, descriptionStrGet, EsString_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDevice, EsFtdiDeviceIntf, isOpen, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDevice, EsFtdiDeviceIntf, isHiSpeed, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDevice, EsFtdiDeviceIntf, isMpsseSupported, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDevice, EsFtdiDeviceIntf, mpsseChannelsCntGet, esU32_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDevice, EsFtdiDeviceIntf, open, bool_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDevice, EsFtdiDeviceIntf, close, void_Call, NO_METHOD_DESCR)
  /// Reflected Generic FTDI services
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, read, EsVariant_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, write, EsVariant_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, baudRateSet, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, divisorSet, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, dataCharacteristicsSet, void_Call_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, flowControlSet, void_Call_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, reset, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, dtrSet, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, dtrClr, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, rtsSet, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, rtsClr, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, charsSet, void_Call_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, purge, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, timeoutsSet, void_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, breakOnSet, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, breakOffSet, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, inTaskStop, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, inTaskRestart, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, latencyTimerSet, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, latencyTimerGet, EsVariant_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, bitModeSet, void_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, bitModeGet, EsVariant_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, usbParametersSet, void_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, deadmanTimeoutSet, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, comPortIdleSet, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, comPortIdleCancel, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, eeConfigRead, EsVariant_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, eeConfigWrite, EsVariant_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDevice, eeEccRead, EsVariant_Call_cr_EsVariant, NO_METHOD_DESCR)
  // Properties
  ES_DECL_PROP_INFO_RO(               EsFtdiDevice, isOpen, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsFtdiDevice, isValid, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsFtdiDevice, status, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsFtdiDevice, ioAndEvtStatus, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsFtdiDevice, queueStatus, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsFtdiDevice, modemStatus, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsFtdiDevice, driverVersion, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsFtdiDevice, comPortNumber, long, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsFtdiDevice, deviceInfo, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(               EsFtdiDevice, queueStatusEx, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsFtdiDevice::EsFtdiDevice() :
m_owner( nullptr ),
m_stat( FT_OK ),
m_cfgClockRate(1000000),
m_cfgLatencyTimer( EsFtdiDeviceIntf::LATENCY_DEF )
{
  m_dynamic = true;
}
//---------------------------------------------------------------------------

EsFtdiDevice::EsFtdiDevice(EsFtdiDriver* owner /*= nullptr*/, const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& node /*= EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE()*/) :
m_owner( owner ),
m_node( node ),
m_stat( FT_OK ),
m_cfgClockRate(1000000),
m_cfgLatencyTimer( EsFtdiDeviceIntf::LATENCY_DEF )
{
  m_dynamic = true;
}
//---------------------------------------------------------------------------

EsBaseIntf::Ptr EsFtdiDevice::create(EsFtdiDriver& owner, const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& node)
{
  std::unique_ptr<EsFtdiDevice> ptr(
    new EsFtdiDevice(
      &owner,
      node
    )
  );

  return ptr.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

EsFtdiDevice::~EsFtdiDevice()
{
  try
  {
    close(); //< Do not allow any exception to escape destructor
  }
  catch(...)
  {
  }
}
//---------------------------------------------------------------------------

bool EsFtdiDevice::open()
{
  if( !isOpen() )
  {
    esU32& flags = EsFtdiDriver::nodeFlagsAccess( m_node );
    FT_HANDLE& handle = EsFtdiDriver::nodeHandleAccess(m_node);

    ES_ASSERT(m_owner);

    EsFtdiDriver::FT_STATUS stat = m_owner->ftOpenEx(
      reinterpret_cast<void*>(
        const_cast<char*>(
          m_node.serialNumberGet()
        )
      ),
      EsFtdiDriver::OPEN_BY_SERIAL_NUMBER,
      &handle
    );

    if( m_owner->statusCheck( stat ) )
      flags |= EsFtdiDriver::FLAGS_OPENED;
  }

  return isOpen();
}
//---------------------------------------------------------------------------

void EsFtdiDevice::close()
{
  esU32& flags = EsFtdiDriver::nodeFlagsAccess( m_node );
  FT_HANDLE& handle = EsFtdiDriver::nodeHandleAccess(m_node);

  if( !isValid() )
  {
    handle = nullptr;
    flags &= ~EsFtdiDriver::FLAGS_OPENED;
    m_tmpBuff.clear();
    return;
  }

  if( isOpen() )
  {
    ES_ASSERT(m_owner);

    EsFtdiDriver::FT_STATUS stat = m_owner->ftClose(
      handle
    );

    if( m_owner->statusCheck( stat ) )
    {
      handle = nullptr;
      flags &= ~EsFtdiDriver::FLAGS_OPENED;
      m_tmpBuff.clear();
    }
  }
}
//---------------------------------------------------------------------------

bool EsFtdiDevice::loopbackStateSet(bool set)
{
	esU8 in = 0;
	esU32 bytesWritten = 0;
	esU32 bufIdx = 0;

	if( set )
    in = MPSSE::CMD_TURN_ON_LOOPBACK;
	else
		in = MPSSE::CMD_TURN_OFF_LOOPBACK;

	return ftWrite(
    &in,
    1,
    &bytesWritten
  ) &&
  (1 == bytesWritten);
}
//---------------------------------------------------------------------------

void EsFtdiDevice::mpsseBufInit()
{
  if( m_tmpBuff.size() < MPSSE::MAX_IN_BUF_SIZE )
    m_tmpBuff.resize( MPSSE::MAX_IN_BUF_SIZE );
}
//---------------------------------------------------------------------------

bool EsFtdiDevice::inBuffClear()
{
	esU32 bytesInInputBuf = 0;

  if( !ftGetQueueStatus(&bytesInInputBuf) )
    return false;

	if( !bytesInInputBuf )
    return true;

  mpsseBufInit();

  do
  {
    esU32 toread = (bytesInInputBuf > MPSSE::MAX_IN_BUF_SIZE) ?
      MPSSE::MAX_IN_BUF_SIZE :
      bytesInInputBuf;

    esU32 numOfBytesRead = 0;

    if( !ftRead(
        m_tmpBuff.data(),
        toread,
        &numOfBytesRead
      )
    )
      return false;

    bytesInInputBuf = bytesInInputBuf - numOfBytesRead;

  } while( bytesInInputBuf );

	return true;
}
//---------------------------------------------------------------------------

bool EsFtdiDevice::mpsseCmdSend(esU8 cmd, int echo, esU8& cmdEchoed)
{
  cmdEchoed = 0; //< Nothing echoed
	int loopCounter = 0;
  bool sentOnce = false;

	do
	{
		ES_DEBUG_TRACE(
      esT("EsFtdiDevice::mpsseCmdSend loopCounter=%d"),
			loopCounter
    );

		// Check whether command has to be sent every time in the loop
  	esU32 bytesWritten = 0;
    if(
      (1 == echo && !sentOnce) ||
      (2 == echo)
    )
    {
	    if( !ftWrite(
          &cmd,
          1,
          &bytesWritten
        )
      )
        return false;

      sentOnce = true;
		}

		EsThread::sleep(1);

		// Read the no of bytes available in Receive buffer
  	esU32 bytesInInputBuf = 0;
		if( !ftGetQueueStatus(&bytesInInputBuf) )
      return false;

		ES_DEBUG_TRACE(
      esT("EsFtdiDevice::mpsseCmdSend bytesInInputBuf size=%d"),
      bytesInInputBuf
    );

		if( bytesInInputBuf )
		{
      mpsseBufInit();

      ES_ASSERT( bytesInInputBuf <= MPSSE::MAX_IN_BUF_SIZE );

    	esU32 numOfBytesRead = 0;
			if( !ftRead(
          m_tmpBuff.data(),
          bytesInInputBuf,
          &numOfBytesRead
        )
      )
        return false;

			if(numOfBytesRead)
			{
      	esU32 byteCounter = 0;

				do
				{
          cmdEchoed = m_tmpBuff[byteCounter];
					++byteCounter;

				} while(
          (byteCounter < numOfBytesRead) &&
          (cmdEchoed != cmd)
        );
			}
		}

    ++loopCounter;
		if( loopCounter > MPSSE::MAX_IN_BUF_SIZE )
		{
			ES_DEBUG_TRACE(
        esT("EsFtdiDevice::mpsseCmdSend Cmd IO loop exit after executing %d times"),
        loopCounter
      );

      return false;
		}

	} while(cmdEchoed != cmd);

	return true;
}
//---------------------------------------------------------------------------

bool EsFtdiDevice::mpsseSynch()
{
	if( !inBuffClear() )
    return false;

	esU8 cmdEchoed;
	// Send and receive command
  if( !mpsseCmdSend(
      MPSSE::CMD_ECHO_1,
      2, //< Countinuous re-send
      cmdEchoed
    )
  )
    return false;

	if( MPSSE::CMD_ECHO_1 != cmdEchoed )
    return false;

  if( !mpsseCmdSend(
      MPSSE::CMD_ECHO_2,
      1, //< Send once
      cmdEchoed
    )
  )
    return false;

	if(MPSSE::CMD_ECHO_2 != cmdEchoed)
    return false;

	return true;
}
//---------------------------------------------------------------------------

bool EsFtdiDevice::mpsseClockSet(esU32 clock)
{
	DWORD bytesWritten = 0;
	esU8 valH, valL;
	esU32 val;

	switch( m_node.typeGet() )
	{
  case EsFtdiDriver::DEVICE_2232C:
    // This is actually FT2232D but defined is FT_DEVICE_2232C
    // in D2XX. Also, it is the only FS device that supports MPSSE
    //
    val = (MPSSE::_30MHZ/clock) - 1;
    break;
  default:/* Assuming all new chips will he HS MPSSE devices */
    ES_DEBUG_TRACE(
      esT("EsFtdiDevice::mpsseClockSet: Unknown device type(0x%08X). Assuming high speed device"),
      static_cast<esU32>(
        m_node.typeGet()
      )
    );
    // Fall through
  case EsFtdiDriver::DEVICE_2232H:
  case EsFtdiDriver::DEVICE_4232H:
  case EsFtdiDriver::DEVICE_232H:
    if(clock <= MPSSE::_6MHZ)
    {
      val = MPSSE::ENABLE_CLOCK_DIVIDE;

      ES_DEBUG_TRACE(
        esT("EsFtdiDevice::mpsseClockSet: value=0x%0X ENABLE_CLOCK_DIVIDE"),
        (unsigned)val
      );

      if( !ftWrite(
          &val,
          1,
          &bytesWritten
        )
      )
        return false;

      val = (MPSSE::_6MHZ/clock) - 1;
    }
    else
    {
      val = MPSSE::DISABLE_CLOCK_DIVIDE;

      ES_DEBUG_TRACE(
        esT("EsFtdiDevice::mpsseClockSet: value=0x%0X DISABLE_CLOCK_DIVIDE"),
        (unsigned)val
      );

      if( !ftWrite(
          &val,
          1,
          &bytesWritten
        )
      )
        return false;

      val = (MPSSE::_30MHZ/clock) - 1;
    }
    break;
	}

	// Calculate valueH and ValueL
	valL = static_cast<esU8>(val);
	valH = static_cast<esU8>(val >> 8);

  mpsseBufInit();

	// Set the clock
	DWORD bufIdx = 0;
	m_tmpBuff[bufIdx++] = MPSSE::CMD_SET_CLOCK_FREQUENCY;

	ES_DEBUG_TRACE(
    esT("EsFtdiDevice::mpsseClockSet: valueL=0x%0X valueH=0x%0X"),
    (int)valL,
    (int)valH
  );

	m_tmpBuff[bufIdx++] = valL;
	m_tmpBuff[bufIdx++] = valH;

	if(
    ftWrite(
      m_tmpBuff.data(),
      bufIdx,
      &bytesWritten
    ) &&
    bufIdx == bytesWritten
  )
  {
    // Wait for rate to stablilze
	  EsThread::sleep(20);
    return true;
  }

  return false;
}
//---------------------------------------------------------------------------

bool EsFtdiDevice::mpsseReset()
{
  return ftSetBitMode( //< Reset MPSSE controller
    EsFtdiDriver::INTERFACE_MASK_IN,
    EsFtdiDriver::BITMODE_RESET
  );
}
//---------------------------------------------------------------------------

bool EsFtdiDevice::mpsseEnable()
{
  return ftSetBitMode( //< Enable MPSSE mode
    EsFtdiDriver::INTERFACE_MASK_IN,
    EsFtdiDriver::BITMODE_MPSSE
  );
}
//---------------------------------------------------------------------------

bool EsFtdiDevice::latencyAndRateSetup(esU8 latency, esU32 rate, bool setFlowOff)
{
	// SetLatencyTimer
	if( !ftSetLatencyTimer(latency) )
    return false;

  if( setFlowOff && !ftSetFlowControl( //< Turn on flow control to synchronize IN requests
      EsFtdiDriver::FLOW_RTS_CTS,
      0,
      0
    )
  )
    return false;

	// ResetMPSSE
  if( !mpsseReset() )
    return false;

	// EnableMPSSEInterface
  if( !mpsseEnable() )
    return false;

	// 20110608 - enabling loopback before sync
  if( !loopbackStateSet(true) )
    return false;

	// Sync MPSSE
  if( !mpsseSynch() )
    return false;

	// Wait for USB
	EsThread::sleep(50);

	// Set Clock frequency
  if( !mpsseClockSet(rate) )
    return false;

  return true;
}
//---------------------------------------------------------------------------

bool EsFtdiDevice::mpsseOpen()
{
  if( !isMpsseSupported() )
    EsException::Throw(
      esT("FTDI Device '%s' does not support MPSSE"),
      serialNumberStrGet()
    );

  if( EsFtdiDevice::open() ) //< Prevent from calling version from derived classes
  {
    // Prepare device for MPSSE mode
    if( !ftResetDevice() ) //< Reset USB device
    {
      close();
      return false;
    }

    esU32 cnt = 0;
    // Purge USB read buffer
    if( !ftGetQueueStatus(&cnt) )
    {
      close();
      return false;
    }

    if( cnt )
      read( cnt );

    if( !ftSetUSBParameters( //< Set USB request trasnfer size to 64K
        MPSSE::USB_INPUT_BUFFER_SIZE,
        MPSSE::USB_OUTPUT_BUFFER_SIZE //< According to docs, this parameter is ignored so far
      )
    )
    {
      close();
      return false;
    }

    if( !ftSetChars( //< Disable event and control characters
        0,
        MPSSE::DISABLE_EVENT,
        0,
        MPSSE::DISABLE_CHAR
      )
    )
    {
      close();
      return false;
    }

    if( !ftSetTimeouts( //< Sets the read and write timeouts in milliseconds
        MPSSE::DEVICE_READ_TIMEOUT,
        MPSSE::DEVICE_WRITE_TIMEOUT
      )
    )
    {
      close();
      return false;
    }

    if( !latencyAndRateSetup(
        EsFtdiDeviceIntf::LATENCY_DEF,
        m_cfgClockRate,
        true //< Turn off flow control
      )
    )
    {
      close();
      return false;
    }

    // Stop Loopback
    if( !loopbackStateSet(false) )
    {
      close();
      return false;
    }

    if( !inBuffClear() )
    {
      close();
      return false;
    }
  }

  return isOpen();
}
//---------------------------------------------------------------------------

bool EsFtdiDevice::mpsseChannelInit(esU32 rate)
{
  if( !ftResetDevice() )
    return false;

	// Purge
  if( !ftPurge(EsFtdiDriver::PURGE_RX | EsFtdiDriver::PURGE_TX) )
    return false;

	// Set USB buffer size
  if( !ftSetUSBParameters(
      MPSSE::USB_INPUT_BUFFER_SIZE,
      MPSSE::USB_OUTPUT_BUFFER_SIZE
    )
  )
    return false;

	// Sets the special characters for the device,
	// disable event and error characters
  //
  if( !ftSetChars(
      0,
      MPSSE::DISABLE_EVENT,
      0,
      MPSSE::DISABLE_CHAR
    )
  )
    return false;

	// SetTimeOut
#ifdef FT800_HACK
	if( !ftSetDeviceTimeOut(
      MPSSE::DEVICE_TIMEOUT_INFINITE,
      MPSSE::DEVICE_WRITE_TIMEOUT
    )
  )
    return false;
#else
  if( !ftSetTimeouts(
      MPSSE::DEVICE_READ_TIMEOUT,
      MPSSE::DEVICE_WRITE_TIMEOUT
    )
  )
    return false;
#endif

  latencyAndRateSetup(
    m_cfgLatencyTimer,
    rate,
    false //< Do not touch flow control
  );

	// Stop Loop back
  if( !loopbackStateSet(false) )
    return false;

  if( !inBuffClear() )
    return false;

	// Set i/o pin states
  if( !mpsseGpioLowSet(
      MPSSE::SET_LOW_BYTE_DATA_BITS_DATA,
      MPSSE::SET_LOW_BYTE_DATA_BITS_DATA
    )
  )
    return false;

  return true;
}
//---------------------------------------------------------------------------

bool EsFtdiDevice::mpsseGpioLowSet(esU8 val, esU8 dir)
{
  esU32 bufIdx = 0;

  mpsseBufInit();

	m_tmpBuff[bufIdx++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
	m_tmpBuff[bufIdx++] = val;
	m_tmpBuff[bufIdx++] = dir;

  esU32 bytesWritten = 0;

	return ftWrite(
    m_tmpBuff.data(),
    bufIdx,
    &bytesWritten
  );
}
//---------------------------------------------------------------------------

bool EsFtdiDevice::mpsseGpioWrite(esU8 val, esU8 dir)
{
	esU32 bufIdx = 0;

  mpsseBufInit();

#if 1 //def FT800_232HM
	m_tmpBuff[bufIdx++] = MPSSE::CMD_SET_DATA_BITS_HIGHBYTE;
	m_tmpBuff[bufIdx++] = val;
	m_tmpBuff[bufIdx++] = dir;
#else
	m_tmpBuff[bufIdx++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
	m_tmpBuff[bufIdx++] = val;
	m_tmpBuff[bufIdx++] = dir;
#endif

  esU32 bytesWritten = 0;
	if( !ftWrite(
      m_tmpBuff.data(),
      bufIdx,
      &bytesWritten
    )
  )
    return false;

	return bufIdx == bytesWritten;
}
//---------------------------------------------------------------------------

bool EsFtdiDevice::mpsseGpioRead(esU8& out)
{
	esU32 bufIdx = 0;

  mpsseBufInit();

#if 1 //def FT800_232HM
	m_tmpBuff[bufIdx++] = MPSSE::CMD_GET_DATA_BITS_HIGHBYTE;
	m_tmpBuff[bufIdx++] = MPSSE::CMD_SEND_IMMEDIATE;
#else
	m_tmpBuff[bufIdx++] = MPSSE::CMD_GET_DATA_BITS_LOWBYTE;
	m_tmpBuff[bufIdx++] = MPSSE::CMD_SEND_IMMEDIATE;
#endif

  out = 0;
  esU32 bytesTransfered = 0;

	if( !ftWrite(
      m_tmpBuff.data(),
      bufIdx,
		  &bytesTransfered
    )
  )
    return false;

	if( !ftRead(
      &out,
      1,
      &bytesTransfered
    )
  )
    return false;

	if(1 != bytesTransfered)
  {
		m_stat = FT_IO_ERROR;
    return false;
  }

	return true;
}
//---------------------------------------------------------------------------

ulong EsFtdiDevice::get_driverVersion() const
{
  esU32 ver = 0;

  const_cast<EsFtdiDevice&>(*this).ftGetDriverVersion(&ver);

  return static_cast<ulong>(ver);
}
//---------------------------------------------------------------------------

long EsFtdiDevice::get_comPortNumber() const
{
  esI32 cnt = 0;

  const_cast<EsFtdiDevice&>(*this).ftGetComPortNumber(&cnt);

  return static_cast<long>(cnt);
}
//---------------------------------------------------------------------------

EsVariant EsFtdiDevice::read(cr_EsVariant toRead)
{
  esU32 toread = toRead.asULong();
  if( !toread )
    return EsVariant::null();

  esU32 read = 0;

  EsBinBuffer bb( toread );
  if( ftRead(
      bb.data(),
      toread,
      &read
    )
  )
  {
    bb.resize( read );

    return bb;
  }

  return EsVariant::null();
}
//---------------------------------------------------------------------------

EsVariant EsFtdiDevice::write(cr_EsVariant out)
{
  if( out.isEmpty() )
    return 0;

  const EsBinBuffer& bb = out.asBinBuffer();
  esU32 written = 0;
  if( ftWrite(
      reinterpret_cast<void*>(
        const_cast<EsBinBuffer::pointer>(
          bb.data()
        )
      ),
      bb.size(),
      &written
    )
  )
    return static_cast<ulong>(written);

  return 0;
}
//---------------------------------------------------------------------------

void EsFtdiDevice::baudRateSet(cr_EsVariant br)
{
  ftSetBaudRate(
    br.asULong()
  );
}
//---------------------------------------------------------------------------

void EsFtdiDevice::divisorSet(cr_EsVariant d)
{
  ulong div = d.asULong();
  EsNumericCheck::checkRangeUInteger(
    div,
    0,
    65535,
    esT("FTDI device divisor")
  );

  ftSetDivisor(
    static_cast<esU16>(div)
  );
}
//---------------------------------------------------------------------------

void EsFtdiDevice::dataCharacteristicsSet( cr_EsVariant wordLen, cr_EsVariant stopBits, cr_EsVariant parity)
{
  ftSetDataCharacteristics(
    wordLen.asByte(),
    stopBits.asByte(),
    parity.asByte()
  );
}
//---------------------------------------------------------------------------

void EsFtdiDevice::flowControlSet(cr_EsVariant flowctl, cr_EsVariant xon, cr_EsVariant xoff)
{
  ftSetFlowControl(
    flowctl.asULong() & 0xFFFF,
    xon.asByte(),
    xoff.asByte()
  );
}
//---------------------------------------------------------------------------

void EsFtdiDevice::reset()
{
  ftResetDevice();
}
//---------------------------------------------------------------------------

void EsFtdiDevice::dtrSet()
{
  ftSetDtr();
}
//---------------------------------------------------------------------------

void EsFtdiDevice::dtrClr()
{
  ftClrDtr();
}
//---------------------------------------------------------------------------

void EsFtdiDevice::rtsSet()
{
  ftSetRts();
}
//---------------------------------------------------------------------------

void EsFtdiDevice::rtsClr()
{
  ftClrRts();
}
//---------------------------------------------------------------------------

ulong EsFtdiDevice::get_modemStatus() const
{
  esU32 stat = 0;

  const_cast<EsFtdiDevice&>(*this).ftGetModemStatus(&stat);

  return static_cast<ulong>(stat);
}
//---------------------------------------------------------------------------

void EsFtdiDevice::charsSet(cr_EsVariant evtCh, cr_EsVariant evtChEnable, cr_EsVariant errCh, cr_EsVariant errChEnable)
{
  ftSetChars(
    evtCh.asByte(),
    evtChEnable.asBool(),
    errCh.asByte(),
    errChEnable.asBool()
  );
}
//---------------------------------------------------------------------------

void EsFtdiDevice::purge(cr_EsVariant mask)
{
  ftPurge(
    mask.asULong() &
      (EsFtdiDriver::PURGE_RX | EsFtdiDriver::PURGE_TX)
  );
}
//---------------------------------------------------------------------------

void EsFtdiDevice::timeoutsSet( cr_EsVariant rxTmo, cr_EsVariant txTmo )
{
  ftSetTimeouts(
    rxTmo.asULong(),
    txTmo.asULong()
  );
}
//---------------------------------------------------------------------------

ulong EsFtdiDevice::get_queueStatus() const
{
  esU32 rxCnt = 0;

  const_cast<EsFtdiDevice&>(*this).ftGetQueueStatus(&rxCnt);

  return static_cast<ulong>(rxCnt);
}
//---------------------------------------------------------------------------

EsVariant EsFtdiDevice::get_ioAndEvtStatus() const
{
  esU32 rxCnt = 0;
  esU32 txCnt = 0;
  esU32 evtStat = 0;

  if(
    const_cast<EsFtdiDevice&>(*this).ftGetStatus(
      &rxCnt,
      &txCnt,
      &evtStat
    )
  )
  {
    const EsVariant::Array& va = {
      static_cast<ulong>(rxCnt),
      static_cast<ulong>(txCnt),
      static_cast<ulong>(evtStat)
    };

    return va;
  }

  return EsVariant::null();
}
//---------------------------------------------------------------------------

void EsFtdiDevice::breakOnSet()
{
  ftSetBreakOn();
}
//---------------------------------------------------------------------------

void EsFtdiDevice::breakOffSet()
{
  ftSetBreakOff();
}
//---------------------------------------------------------------------------

void EsFtdiDevice::inTaskStop()
{
  ftStopInTask();
}
//---------------------------------------------------------------------------

void EsFtdiDevice::inTaskRestart()
{
  ftRestartInTask();
}
//---------------------------------------------------------------------------

void EsFtdiDevice::latencyTimerSet(cr_EsVariant tmo)
{
  ftSetLatencyTimer(
    tmo.asByte()
  );
}
//---------------------------------------------------------------------------

EsVariant EsFtdiDevice::latencyTimerGet()
{
  esU8 tmo = 0;
  if(
    ftGetLatencyTimer(
      &tmo
    )
  )
    return tmo;

  return EsVariant::null();
}
//---------------------------------------------------------------------------

void EsFtdiDevice::bitModeSet(cr_EsVariant mask, cr_EsVariant mode)
{
  ftSetBitMode(
    mask.asByte(),
    mode.asByte()
  );
}
//---------------------------------------------------------------------------

EsVariant EsFtdiDevice::bitModeGet()
{
  esU8 bm = 0;
  if(
    ftGetBitMode(
      &bm
    )
  )
    return bm;

  return EsVariant::null();
}
//---------------------------------------------------------------------------

void EsFtdiDevice::usbParametersSet(cr_EsVariant inSize, cr_EsVariant outSize)
{
  ftSetUSBParameters(
    inSize.asULong(),
    outSize.asULong()
  );
}
//---------------------------------------------------------------------------

void EsFtdiDevice::deadmanTimeoutSet(cr_EsVariant tmo)
{
  ftSetDeadmanTimeout(
    tmo.asULong()
  );
}
//---------------------------------------------------------------------------

EsVariant EsFtdiDevice::get_deviceInfo() const
{
  return m_node.toVariant();
}
//---------------------------------------------------------------------------

ulong EsFtdiDevice::get_queueStatusEx() const
{
  esU32 stat = 0;

  const_cast<EsFtdiDevice&>(*this).ftGetQueueStatusEx(&stat);

  return static_cast<ulong>(stat);
}
//---------------------------------------------------------------------------

void EsFtdiDevice::comPortIdleSet()
{
  ftComPortIdle();
}
//---------------------------------------------------------------------------

void EsFtdiDevice::comPortIdleCancel()
{
  ftComPortCancelIdle();
}
//---------------------------------------------------------------------------

EsVariant EsFtdiDevice::eeConfigRead(cr_EsVariant addr)
{
  esU8 val = 0;

  if(
    ftEE_ReadConfig(
      addr.asByte(),
      &val
    )
  )
    return val;

  return EsVariant::null();
}
//---------------------------------------------------------------------------

EsVariant EsFtdiDevice::eeConfigWrite(cr_EsVariant addr, cr_EsVariant cfg)
{
  return ftEE_WriteConfig(
    addr.asByte(),
    cfg.asByte()
  );
}
//---------------------------------------------------------------------------

EsVariant EsFtdiDevice::eeEccRead(cr_EsVariant option)
{
  esU16 val = 0;

  if(
    ftEE_ReadECC(
      option.asByte(),
      &val
    )
  )
    return static_cast<ulong>(val);

  return EsVariant::null();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

bool EsFtdiDevice::get_isOpen() const
{
  return isOpen();
}
//---------------------------------------------------------------------------

bool EsFtdiDevice::get_isValid() const
{
  return isValid();
}
//---------------------------------------------------------------------------

ulong EsFtdiDevice::get_status() const
{
  return static_cast<ulong>(m_stat);
}
//---------------------------------------------------------------------------

#endif //< ES_COMM_USE_FTDI

