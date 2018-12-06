#include "escommpch.h"
#pragma hdrstop

#include "EsFtdiDeviceMpsseI2c.h"

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

ES_DECL_CLASS_INFO_DERIVED_BEGIN( EsFtdiDeviceMpsseI2c, EsFtdiDevice, NO_CLASS_DESCR )
  // I2C Intf
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDeviceMpsseI2c, EsFtdiMpsseI2cIntf, configApply, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDeviceMpsseI2c, EsFtdiMpsseI2cIntf, ping, bool_Call_esU16_bool, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDeviceMpsseI2c, EsFtdiMpsseI2cIntf, write, esU32_Call_esU16_cr_EsBinBuffer_esU32, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDeviceMpsseI2c, EsFtdiMpsseI2cIntf, read, EsBinBuffer_Call_esU16_esU32_esU32, NO_METHOD_DESCR)
  // GPIO Intf
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDeviceMpsseI2c, GPIOwrite, EsVariant_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(  EsFtdiDeviceMpsseI2c, GPIOread, EsVariant_Call, NO_METHOD_DESCR)
  // Properties
  ES_DECL_PROP_INFO_RESTRICTED(       EsFtdiDeviceMpsseI2c, clockRate, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(EsFtdiMpsseI2cIntf::CLOCK_STANDARD), ES_ENUM(EsReflection::EsFtdiMpsseI2cClock), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RANGE(            EsFtdiDeviceMpsseI2c, latencyTimer, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(EsFtdiDeviceIntf::LATENCY_DEF), static_cast<ulong>(EsFtdiDeviceIntf::LATENCY_MIN), static_cast<ulong>(EsFtdiDeviceIntf::LATENCY_MAX), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                  EsFtdiDeviceMpsseI2c, _3phaseDataClocking, bool, NO_PROPERTY_LABEL, true, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                  EsFtdiDeviceMpsseI2c, driveOnlyZero, bool, NO_PROPERTY_LABEL, false, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                  EsFtdiDeviceMpsseI2c, clockStretching, bool, NO_PROPERTY_LABEL, false, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsFtdiDeviceMpsseI2c::EsFtdiDeviceMpsseI2c(EsFtdiDriver* owner, const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& node) :
EsFtdiDeviceMpsseI2c::BaseImplT(),
m_opt3PhaseDataClocking(true),
m_optLoopback(false),
m_optClockStretching(false),
m_cfgChanged(true)
{
  ES_ASSERT( node.mpsseSupported() );
  m_owner = owner;
  m_node = node;
  m_cfgClockRate = EsFtdiMpsseI2cIntf::CLOCK_STANDARD;
}
//---------------------------------------------------------------------------

EsBaseIntf::Ptr EsFtdiDeviceMpsseI2c::create(EsFtdiDriver& owner, const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& node)
{
  std::unique_ptr<EsFtdiDeviceMpsseI2c> ptr(
    new EsFtdiDeviceMpsseI2c(
      &owner,
      node
    )
  );
  ES_ASSERT(ptr);

  ptr->m_dynamic = true;
  return ptr.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::open()
{
  if( mpsseOpen() )
  {
    configApply();
    return true;
  }

  return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseI2c::clockRateSet(esU32 rate)
{
  if( m_cfgClockRate != rate )
  {
    m_cfgClockRate = rate;
    m_cfgChanged = true;
  }
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseI2c::latencyTimerSet(esU8 tmo)
{
  if( tmo < LATENCY_MIN )
    tmo = LATENCY_MIN;
  if( tmo > LATENCY_MAX )
    tmo = LATENCY_MAX;

  if( m_cfgLatencyTimer != tmo )
  {
    m_cfgLatencyTimer = tmo;
    m_cfgChanged = true;
  }
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseI2c::_3phaseDataClockingSet(bool set)
{
  if( m_opt3PhaseDataClocking != set )
  {
    m_opt3PhaseDataClocking = set;
    m_cfgChanged = true;
  }
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseI2c::driveOnlyZeroSet(bool set)
{
  if( m_optLoopback != set )
  {
    m_optLoopback = set;
    m_cfgChanged = true;
  }
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseI2c::clockStretchingSet(bool set)
{
  if( m_optClockStretching != set )
  {
    m_optClockStretching = set;
    m_cfgChanged = true;
  }
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::i2cChannelInit( esU32 rate, esU32 opts )
{
  if( !mpsseChannelInit(
      rate
    )
  )
    return false;

  // The I2C master should actually drive the SDA line only when the output is LOW.
  // It should tristate the SDA line when the output should be high. This tristating
  // the SDA line during output HIGH is supported only in FT232H chip
  //
  if( (EsFtdiDriver::DEVICE_232H == m_node.typeGet()) && (opts & MPSSE::I2C_ENABLE_DRIVE_ONLY_ZERO))
  {
    mpsseBufInit();
    esU32 cnt = 0;

    ES_DEBUG_TRACE(esT("Enabling DRIVE_ONLY_ZERO"));

    m_tmpBuff[cnt++] = MPSSE::CMD_ENABLE_DRIVE_ONLY_ZERO;/* MPSSE command */
    m_tmpBuff[cnt++] = 0x03; /* LowByte */
    m_tmpBuff[cnt++] = 0x00; /* HighByte */

    esU32 noOfBytesTransferred = 0;
    if( !ftWrite(
        m_tmpBuff.data(),
        cnt,
        &noOfBytesTransferred
      )
    )
      return false;

    return (noOfBytesTransferred == cnt);
  }

  return true;
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::i2cInit( esU32 opts )
{
  esU32 rate = m_cfgClockRate;

  // Adjust clock rate if 3phase clocking should be enabled
  if( !(opts & MPSSE::I2C_DISABLE_3PHASE_CLOCKING) )
    rate = (rate * 3)/2;

  if( !i2cChannelInit(
      rate,
      opts
    )
  )
    return false;

  if( !(opts & MPSSE::I2C_DISABLE_3PHASE_CLOCKING) )
  {
    mpsseBufInit();

    esU32 noOfBytesTransferred = 0;
    m_tmpBuff[0] = MPSSE::CMD_ENABLE_3PHASE_CLOCKING;/* MPSSE command */

    if( !ftWrite(
        m_tmpBuff.data(),
        1,
        &noOfBytesTransferred
      )
    )
      return false;
  }

  return true;
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseI2c::configApply()
{
  if( !isOpen() )
    return;

  if( !m_cfgChanged )
    return;

  // Fill-in configuration structure
  esU32 opts = 0;
  if( !m_opt3PhaseDataClocking )
    opts |= MPSSE::I2C_DISABLE_3PHASE_CLOCKING;

  if( m_optLoopback )
    opts |= 0x02;
  if( m_optClockStretching )
    opts |= 0x04;

  if( i2cInit(opts) )
    m_cfgChanged = false;
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseI2c::checkAddr(esU8 addr)
{
  EsNumericCheck::checkRangeUInteger(
    0,
    127,
    addr,
    esT("I2C slave address")
  );
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::i2cWrite8bitsAndGetAck(esU8 data, bool& ack)
{
  ES_ASSERT( !m_tmpBuff.empty() );

  esU32 noOfBytes = 0;

  // Set direction
  m_tmpBuff[noOfBytes++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;  //< MPSSE command
  m_tmpBuff[noOfBytes++] = MPSSE::VALUE_SCLLOW_SDAHIGH;       //< Value
  m_tmpBuff[noOfBytes++] = MPSSE::DIRECTION_SCLOUT_SDAOUT;    //< Direction

  // Command to write 8bits
  m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_OUT_BITS_NEG_EDGE; //< MPSSE command
  m_tmpBuff[noOfBytes++] = MPSSE::DATA_SIZE_8BITS;
  m_tmpBuff[noOfBytes++] = data;

  // Set SDA to input mode before reading ACK bit
  m_tmpBuff[noOfBytes++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;  //< MPSSE command
  m_tmpBuff[noOfBytes++] = MPSSE::VALUE_SCLLOW_SDALOW;        //< Value
  m_tmpBuff[noOfBytes++] = MPSSE::DIRECTION_SCLOUT_SDAIN;     //< Direction

  // Command to get ACK bit
  m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_IN_BITS_POS_EDGE;  //< MPSSE command
  m_tmpBuff[noOfBytes++] = MPSSE::DATA_SIZE_1BIT;             //< Read only one bit

  // Command MPSSE to send data to PC immediately
  m_tmpBuff[noOfBytes++] = MPSSE::CMD_SEND_IMMEDIATE;

  esU32 noOfBytesTransferred = 0;
  if( !ftWrite(
      m_tmpBuff.data(),
      noOfBytes,
      &noOfBytesTransferred
    )
  )

  if(noOfBytes != noOfBytesTransferred)
    return false;

  // Get ack
  EsThread::sleep(1);
  if( !ftRead(
      m_tmpBuff.data(),
      1,
      &noOfBytesTransferred
    )
  )
    return false;

  if(1 != noOfBytesTransferred)
    return false;

  ack = (0x01 == (m_tmpBuff[0] & 0x01));

  return true;
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::i2cRead8bitsAndGiveAck(esU8& data, bool doAck)
{
  esU32 noOfBytes = 0;

  ES_ASSERT( !m_tmpBuff.empty() );

  // Set direction
  m_tmpBuff[noOfBytes++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;//< MPSSE command
  m_tmpBuff[noOfBytes++] = MPSSE::VALUE_SCLLOW_SDALOW;      //< Value
  m_tmpBuff[noOfBytes++] = MPSSE::DIRECTION_SCLOUT_SDAIN;   //< Direction

  // Command to read 8 bits
  m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_IN_BITS_POS_EDGE;
  m_tmpBuff[noOfBytes++] = MPSSE::DATA_SIZE_8BITS; /*0x00=1bit; 0x07=8bits*/

  // Command MPSSE to send data to PC immediately
  m_tmpBuff[noOfBytes++] = MPSSE::CMD_SEND_IMMEDIATE;

// ACK_GLITCH_WORKAROUND
  if( doAck )
  {
    /*set direction*/
    m_tmpBuff[noOfBytes++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
    m_tmpBuff[noOfBytes++] = MPSSE::VALUE_SCLLOW_SDALOW; /*Value*/
    m_tmpBuff[noOfBytes++] = MPSSE::DIRECTION_SCLOUT_SDAOUT; /*Direction*/

    /*Command to write Ack/nAck bit*/
    m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_OUT_BITS_NEG_EDGE;                      // GB CHANGED TO NEG
    m_tmpBuff[noOfBytes++] = MPSSE::DATA_SIZE_1BIT;
    m_tmpBuff[noOfBytes++] = 0x00;

    /*Back to Idle*/
    m_tmpBuff[noOfBytes++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
    m_tmpBuff[noOfBytes++] = MPSSE::VALUE_SCLLOW_SDAHIGH; /*Value*/
    m_tmpBuff[noOfBytes++] = MPSSE::DIRECTION_SCLOUT_SDAIN; /*Direction*/
  }
  else
  {
    /*set direction*/
    m_tmpBuff[noOfBytes++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
    m_tmpBuff[noOfBytes++] = MPSSE::VALUE_SCLLOW_SDAHIGH; /*Value*/
    m_tmpBuff[noOfBytes++] = MPSSE::DIRECTION_SCLOUT_SDAIN; /*Direction*/

    /*Command to write Ack/nAck bit*/
    m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_OUT_BITS_NEG_EDGE;                      // GB CHANGED TO NEG
    m_tmpBuff[noOfBytes++] = MPSSE::DATA_SIZE_1BIT;
    m_tmpBuff[noOfBytes++] = 0xFF;

    /*Back to Idle*/
    m_tmpBuff[noOfBytes++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
    m_tmpBuff[noOfBytes++] = MPSSE::VALUE_SCLLOW_SDAHIGH; /*Value*/
    m_tmpBuff[noOfBytes++] = MPSSE::DIRECTION_SCLOUT_SDAIN; /*Direction*/
  }
// ACK_GLITCH_WORKAROUND end

  esU32 noOfBytesTransferred = 0;
  if( !ftWrite(
      m_tmpBuff.data(),
      noOfBytes,
      &noOfBytesTransferred
    )
  )
    return false;

  if( noOfBytes != noOfBytesTransferred )
    return false;

  if( !ftRead(
      m_tmpBuff.data(),
      1,
      &noOfBytesTransferred
    )
  )
    return false;

  if( 1 != noOfBytesTransferred )
    return false;

  data = m_tmpBuff[0];

  return true;
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::i2cDeviceAddressWrite( esU16 addr, bool& ack, bool forWrite, bool _10bitAddr /*= false*/)
{
  if( _10bitAddr )
    EsException::Throw(
      esT("EsFtdiDeviceMpsseI2c::i2cDeviceAddressWrite: 10 bit addressing is not jet supported in MPSSE I2C")
    );

  addr = (addr << 1) & 0xFF;

  if( forWrite )
    addr &= MPSSE::I2C_ADDRESS_WRITE_MASK;
  else
    addr |= MPSSE::I2C_ADDRESS_READ_MASK;

  ES_DEBUG_TRACE(
    esT("EsFtdiDeviceMpsseI2c::i2cDeviceAddressWrite: 7bit I2C address plus direction bit = 0x%0X"),
    (unsigned)addr
  );

  mpsseBufInit();

  ack = true;
  if( !i2cWrite8bitsAndGetAck(
      static_cast<esU8>(addr),
      ack
    )
  )
    return false;

  return true;
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::i2cStart()
{
  mpsseBufInit();

  ES_ASSERT( m_tmpBuff.size() > (MPSSE::START_DURATION_1+MPSSE::START_DURATION_2+1)*3 );

  // SCL high, SDA high
  esU32 cnt = 0;
  for(int j = 0; j < MPSSE::START_DURATION_1; ++j)
  {
    m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
    m_tmpBuff[cnt++] = MPSSE::VALUE_SCLHIGH_SDAHIGH;
    m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT;
  }

  // SCL high, SDA low
  for(int j = 0; j < MPSSE::START_DURATION_2; ++j)
  {
    m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
    m_tmpBuff[cnt++] = MPSSE::VALUE_SCLHIGH_SDALOW;
    m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT;
  }

  // SCL low, SDA low
  m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
  m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDALOW;
  m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT;

  esU32 noOfBytesTransferred = 0;
  if( !ftWrite(
      m_tmpBuff.data(),
      cnt,
      &noOfBytesTransferred
    )
  )
    return false;

  return (noOfBytesTransferred == cnt);
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::i2cStop()
{
  mpsseBufInit();

  ES_ASSERT( m_tmpBuff.size() > (MPSSE::STOP_DURATION_1+MPSSE::STOP_DURATION_2+MPSSE::STOP_DURATION_3+1)*3 );

  // SCL low, SDA low
  esU32 cnt = 0;
  for(int j=0; j < MPSSE::STOP_DURATION_1; ++j)
  {
    m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
    m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDALOW;
    m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT;
  }

  // SCL high, SDA low
  for(int j=0; j < MPSSE::STOP_DURATION_2; ++j)
  {
    m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
    m_tmpBuff[cnt++] = MPSSE::VALUE_SCLHIGH_SDALOW;
    m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT;
  }

  // SCL high, SDA high
  for(int j=0; j < MPSSE::STOP_DURATION_3; ++j)
  {
    m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
    m_tmpBuff[cnt++] = MPSSE::VALUE_SCLHIGH_SDAHIGH;
    m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT;
  }

  m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
  m_tmpBuff[cnt++] = MPSSE::VALUE_SCLHIGH_SDAHIGH;
  m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLIN_SDAIN; // Tristate the SCL & SDA pins

  esU32 noOfBytesTransferred = 0;
  if( !ftWrite(
      m_tmpBuff.data(),
      cnt,
      &noOfBytesTransferred
    )
  )
    return false;

  return (cnt == noOfBytesTransferred);
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::ping(esU16 devAddr, bool forWrite/* = false*/)
{
  checkOpenChannel( esT("ping") );
  checkAddr(static_cast<esU8>(devAddr));

  if( !i2cStart() )
    return false;

  bool ack = true;
  if( !i2cDeviceAddressWrite(
      devAddr,
      ack,    //< Get ACK
      forWrite
    )
  )
    return false;

  if( !forWrite )
    i2cStop();

  return (0 == ack);
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::i2cDeviceRead(esU16 addr, EsBinBuffer::pointer buffer, esU32 sizeToTransfer, esU32& sizeTransferred, esU32 opts)
{
  bool ack = true;
  if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_FAST_TRANSFER)
    return i2cFastRead(
      addr,
      buffer,
      sizeToTransfer,
      sizeTransferred,
      ack,
      opts
    );

  // Write START bit
  if( (opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_START_BIT) &&
    !i2cStart()
  )
    return false;

  // Write device address (with LSB=1 => READ) & Get ACK
  if( !i2cDeviceAddressWrite(
      addr,
      ack,
      false //< Access Addr for read
    )
  )
    return false;

  if( ack ) //< Check acknowledgement of device address write
  {
    m_stat = EsFtdiDeviceIntf::FT_DEVICE_NOT_FOUND;
    return false;
  }

  for(sizeTransferred = 0; sizeTransferred < sizeToTransfer; ++sizeTransferred)
  {
    // Read byte to buffer & give ACK (or nACK if it is last byte and
    // I2C_TRANSFER_OPTIONS_NACK_LAST_BYTE is set)
    if( !i2cRead8bitsAndGiveAck(
        buffer[sizeTransferred],
        (sizeTransferred < (sizeToTransfer-1)) ?
          true :
          ((opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_NACK_LAST_BYTE) ?
            false :
            true
          )
      )
    )
      return false;
  }

  if( sizeTransferred != sizeToTransfer )
    m_stat = EsFtdiDeviceIntf::FT_IO_ERROR;
  else if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_STOP_BIT) //< Write STOP bit
    return i2cStop();

  return (EsFtdiDeviceIntf::FT_OK == m_stat);
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::i2cDeviceWrite(esU16 addr, EsBinBuffer::const_pointer buffer, esU32 sizeToTransfer, esU32& sizeTransferred, esU32 opts )
{
  if( !ftPurge(EsFtdiDriver::PURGE_RX | EsFtdiDriver::PURGE_TX) )
    return false;

  bool ack = true;
  if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_FAST_TRANSFER)
    return i2cFastWrite(
      addr,
      buffer,
      sizeToTransfer,
      sizeTransferred,
      ack,
      opts
    );

  /* Write START bit */
  if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_START_BIT)
  {
    if( !i2cStart() )
      return false;
  }

  // Write device address (with LSB=0 => Write) & Get ACK
  if( !(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_NO_ADDRESS) )
  {
    if( !i2cDeviceAddressWrite(
        addr,
        ack,
        true
      )
    )
      return false;
  }
  else //< Skip sending device address
    ack = false;

  if( ack ) //< Ack bit set actually means device nAcked
  {
    m_stat = EsFtdiDeviceIntf::FT_DEVICE_NOT_FOUND;
    return false;
  }

  // Loop until sizeToTransfer
  for(sizeTransferred = 0; sizeTransferred < sizeToTransfer; ++sizeTransferred)
  {
    //  Write byte to buffer & Get ACK
    if( !i2cWrite8bitsAndGetAck(
        buffer[sizeTransferred],
        ack
      )
    )
      return false;

    if( ack )
    {
      // Add bit in opts to return with error if device nAcked while being written to
      if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_BREAK_ON_NACK)
      {
        m_stat = EsFtdiDeviceIntf::FT_FAILED_TO_WRITE_DEVICE;
        return false;
      }
    }
  }

  if( sizeTransferred != sizeToTransfer )
  {
    m_stat = EsFtdiDeviceIntf::FT_IO_ERROR;
    return false;
  }

  /* Write STOP bit */
  if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_STOP_BIT)
    return i2cStop();

  return true;
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::i2cFastWrite(esU16 addr, EsBinBuffer::const_pointer buffer,
  esU32 sizeToTransfer, esU32& sizeTransferred, bool& ack, esU32 opts
)
{
  esU32 bitsToTransfer;
  if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_FAST_TRANSFER_BITS)
  {/* size is in bits */
    bitsToTransfer = sizeToTransfer;
  }
  else
  {/* size is in bytes */
    bitsToTransfer = sizeToTransfer * 8;
  }

  esU32 bytesToTransfer=
    (bitsToTransfer>0)?(((bitsToTransfer/8)==0)?1:((bitsToTransfer/8)+1)):(0);

  /* Calculate size of required buffer */
  esU32 sizeTotal = (bytesToTransfer*(6+5 // FASTWRITE_READ_ACK
  )) /*the size of data itself */
  + ((!(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_NO_ADDRESS)) ?
      11 :
      0
  )/*for address byte*/
  + ((opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_START_BIT) ?
      ((MPSSE::START_DURATION_1+MPSSE::START_DURATION_2+1)*3) :
      0
  ) /* size required for START */
  + ((opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_STOP_BIT) ?
      ((MPSSE::STOP_DURATION_1+MPSSE::STOP_DURATION_2+MPSSE::STOP_DURATION_3+1)*3) :
      0
  ); /* size for STOP */

  esU32 sizeOverhead = sizeTotal - bytesToTransfer;

  /* Allocate buffers */
  if( m_tmpBuff.size() < sizeTotal )
    m_tmpBuff.resize(sizeTotal);

  esU32 cnt = 0; /* index of cmdBuffer that is filled */

  /* Write START bit */
  if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_START_BIT)
  {
    /* SCL high, SDA high */
    for(esU32 j = 0; j < MPSSE::START_DURATION_1; ++j)
    {
      m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
      m_tmpBuff[cnt++] = MPSSE::VALUE_SCLHIGH_SDAHIGH;
      m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT;
    }

    /* SCL high, SDA low */
    for(esU32 j = 0; j < MPSSE::START_DURATION_2; ++j)
    {
      m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
      m_tmpBuff[cnt++] = MPSSE::VALUE_SCLHIGH_SDALOW;
      m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT;
    }

    /*SCL low, SDA low */
    m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
    m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDALOW;
    m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT;
  }

  esU32 bitsToRead = 0;
  if(!(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_NO_ADDRESS))
  {
    addr <<= 1;
    addr &= MPSSE::I2C_ADDRESS_WRITE_MASK;

    /*set direction*/
    m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
    m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDAHIGH; /*Value*/
    m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT; /*Direction*/

    /* write address + direction bit */
    m_tmpBuff[cnt++]= MPSSE::CMD_DATA_OUT_BITS_NEG_EDGE;/* MPSSE command */
    m_tmpBuff[cnt++]= MPSSE::DATA_SIZE_8BITS;
    m_tmpBuff[cnt++] = static_cast<esU8>(addr);

    /* Set SDA to input mode before reading ACK bit */
    m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
    m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDALOW; /*Value*/
    m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAIN; /*Direction*/

    /* Command to get ACK bit */
    m_tmpBuff[cnt++] = MPSSE::CMD_DATA_IN_BITS_POS_EDGE;/* MPSSE command */
    m_tmpBuff[cnt++] = MPSSE::DATA_SIZE_1BIT; /* Read only one bit */

    bitsToRead++;
  }

  /* add commands & data to buffer */
  esU32 j = 0;
  while( j < bitsToTransfer )
  {
    esU8 bitsInThisTransfer = ((bitsToTransfer-j)>8) ?
      8 :
      static_cast<esU8>(bitsToTransfer-j);

    /*set direction*/
    m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
    m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDAHIGH; /*Value*/
    m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT; /*Direction*/

    /* Command to write 8bits */
    bitsInThisTransfer = ((bitsToTransfer-j)>8) ?
      8 :
     static_cast<esU8>(bitsToTransfer-j);

    m_tmpBuff[cnt++]= MPSSE::CMD_DATA_OUT_BITS_NEG_EDGE;/* MPSSE command */
    m_tmpBuff[cnt++]= bitsInThisTransfer - 1;
    m_tmpBuff[cnt++] = buffer[j/8];

    /* Read 1bit ack after each 8bits written - only in byte mode */
    if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_FAST_TRANSFER_BYTES)
    {
      /* Set SDA to input mode before reading ACK bit */
      m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
      m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDALOW; /*Value*/
      m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAIN; /*Direction*/

      /* Command to get ACK bit */
      m_tmpBuff[cnt++] = MPSSE::CMD_DATA_IN_BITS_POS_EDGE;/* MPSSE command */
      m_tmpBuff[cnt++] = MPSSE::DATA_SIZE_1BIT; /* Read only one bit */

      bitsToRead++;
    }

    j += bitsInThisTransfer;
  }

  /* Write STOP bit */
  if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_STOP_BIT)
  {
    /* SCL low, SDA low */
    for( j = 0; j < MPSSE::STOP_DURATION_1; ++j)
    {
      m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
      m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDALOW;
      m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT;
    }

    /* SCL high, SDA low */
    for( j = 0; j < MPSSE::STOP_DURATION_2; ++j)
    {
      m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
      m_tmpBuff[cnt++] = MPSSE::VALUE_SCLHIGH_SDALOW;
      m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT;
    }

    /* SCL high, SDA high */
    for( j = 0; j < MPSSE::STOP_DURATION_3; ++j)
    {
      m_tmpBuff[cnt++]=MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
      m_tmpBuff[cnt++]=MPSSE::VALUE_SCLHIGH_SDAHIGH;
      m_tmpBuff[cnt++]=MPSSE::DIRECTION_SCLOUT_SDAOUT;
    }

    m_tmpBuff[cnt++]=MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
    m_tmpBuff[cnt++]=MPSSE::VALUE_SCLHIGH_SDAHIGH;
    m_tmpBuff[cnt++]=MPSSE::DIRECTION_SCLIN_SDAIN; /* Tristate the SCL & SDA pins */
  }

  esU32 transferred = 0;
  if( !ftWrite(
      m_tmpBuff.data(),
      cnt,
      &transferred
    )
  )
    return false;

  sizeTransferred = sizeToTransfer;

  /* Read ack of address */
  if(!(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_NO_ADDRESS))
  {
    esU8 addrAck = 0;
    if( !ftRead(
        &addrAck,
        1,
        &transferred
      )
    )
      return false;

    ack = (0x01 == (addrAck & 0x01));
  }

  /* if byte mode: read 1bit ack after each 8bits written */
  if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_FAST_TRANSFER_BYTES)
  {
    if( m_tmpBuff.size() < sizeToTransfer )
      m_tmpBuff.resize(sizeToTransfer);

    if( !ftRead(
        m_tmpBuff.data(),
        sizeToTransfer,
        &transferred
      )
    )
      return false;

    if( transferred == sizeToTransfer ) //< Check all bytes were acked
    {
      transferred = 0;
      for( auto &v : m_tmpBuff )
      {
        if( (v & 0x01) ) //< Nacked
        {
          sizeTransferred = transferred;
          m_stat = EsFtdiDeviceIntf::FT_IO_ERROR;
          return false;
        }

        ++transferred;
      }
    }
  }

  return true;
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::i2cFastRead(esU16 addr, EsBinBuffer::pointer buffer,
  esU32 sizeToTransfer, esU32& sizeTransferred,
  bool& ack, esU32 opts
)
{
  esU32 bitsToTransfer;
  if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_FAST_TRANSFER_BITS)
  {/* size is in bits */
    bitsToTransfer = sizeToTransfer;
  }
  else
  {/* size is in bytes */
    bitsToTransfer = sizeToTransfer * 8;
  }

  esU32 bytesToTransfer =
    (bitsToTransfer > 0) ?
      (
        ((bitsToTransfer / 8) == 0) ?
          1 :
          (bitsToTransfer/8)
      ) :
      0;

  /* Calculate size of required buffer */
  esU32 sizeTotal = (bytesToTransfer*12) /* the size of data itself */
    + ( (!(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_NO_ADDRESS)) ?
          11 :
          0
      )/* for address byte*/
    + ((opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_START_BIT) ?
        ((MPSSE::START_DURATION_1+MPSSE::START_DURATION_2+1)*3) :
        0
      ) /* size required for START */
    + ((opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_STOP_BIT) ?
        ((MPSSE::STOP_DURATION_1+MPSSE::STOP_DURATION_2+MPSSE::STOP_DURATION_3+1)*3) :
        0
      ) /* size for STOP */
    + 5; // READ ACK

  esU32 sizeOverhead = sizeTotal - bytesToTransfer;

  /* Allocate buffers */
  if( m_tmpBuff.size() < sizeTotal )
    m_tmpBuff.resize( sizeTotal );

  esU32 cnt = 0;
  /* Write START bit */
  if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_START_BIT)
  {
    /* SCL high, SDA high */
    for(esU32 esU32j = 0; esU32j < MPSSE::START_DURATION_1; ++esU32j)
    {
      m_tmpBuff[cnt++]=MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
      m_tmpBuff[cnt++]=MPSSE::VALUE_SCLHIGH_SDAHIGH;
      m_tmpBuff[cnt++]=MPSSE::DIRECTION_SCLOUT_SDAOUT;
    }

    /* SCL high, SDA low */
    for(esU32 j = 0; j < MPSSE::START_DURATION_2; ++j)
    {
      m_tmpBuff[cnt++]=MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
      m_tmpBuff[cnt++]=MPSSE::VALUE_SCLHIGH_SDALOW;
      m_tmpBuff[cnt++]=MPSSE::DIRECTION_SCLOUT_SDAOUT;
    }

    /*SCL low, SDA low */
    m_tmpBuff[cnt++]=MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
    m_tmpBuff[cnt++]=MPSSE::VALUE_SCLLOW_SDALOW;
    m_tmpBuff[cnt++]=MPSSE::DIRECTION_SCLOUT_SDAOUT;
  }

  if( !(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_NO_ADDRESS) )
  {
    addr <<= 1;
    addr |= MPSSE::I2C_ADDRESS_READ_MASK;
    addr &= 0xFF;

    /*set direction*/
    m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
    m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDAHIGH; /*Value*/
    m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT; /*Direction*/

    /* write address + direction bit */
    m_tmpBuff[cnt++] = MPSSE::CMD_DATA_OUT_BITS_NEG_EDGE;/* MPSSE command */
    m_tmpBuff[cnt++] = MPSSE::DATA_SIZE_8BITS;
    m_tmpBuff[cnt++] = static_cast<esU8>(addr);

    /* Set SDA to input mode before reading ACK bit */
    m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
    m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDALOW; /*Value*/
    m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAIN; /*Direction*/

    /* Command to get ACK bit */
    m_tmpBuff[cnt++] = MPSSE::CMD_DATA_IN_BITS_POS_EDGE;/* MPSSE command */
    m_tmpBuff[cnt++] = MPSSE::DATA_SIZE_1BIT; /* Read only one bit */
  }

  /* add commands & data to buffer */
  esU32 j = 0;
  while( j < bitsToTransfer)
  {
    esU32 bitsInThisTransfer = ((bitsToTransfer - j) > 8) ?
      8 :
      (bitsToTransfer-j);

    /*set direction*/
    m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
    m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDALOW; /*Value*/
    m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAIN; /*Direction*/

    /*Command to read 8 bits*/
    m_tmpBuff[cnt++] = MPSSE::CMD_DATA_IN_BITS_POS_EDGE;
    m_tmpBuff[cnt++] = static_cast<esU8>(bitsInThisTransfer - 1);

    /*Command MPSSE to send data to PC immediately */
    /*buffer[cnt++] = MPSSE_CMD_SEND_IMMEDIATE;*/

    /* Write 1bit ack after each 8bits read - only in byte mode */
    if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_FAST_TRANSFER_BYTES)
    {
/// NACK_GLITCH_WORKAROUND
// SCL Glitch fix for Graham
      if( j < (bitsToTransfer-1) )
      {
        //m_tmpBuff[cnt++] = SEND_ACK;
        /*set direction*/
        m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
        m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDALOW; /*Value*/
        m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT; /*Direction*/

        /*Command to write Ack/nAck bit*/
        m_tmpBuff[cnt++] = MPSSE::CMD_DATA_OUT_BITS_NEG_EDGE;                      // GB CHANGED TO NEG
        m_tmpBuff[cnt++] = MPSSE::DATA_SIZE_1BIT;
        m_tmpBuff[cnt++] = 0x00;

        /*Back to Idle*/
        m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
        m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDAHIGH; /*Value*/
        m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAIN; /*Direction*/
      }
      else
      {
        if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_NACK_LAST_BYTE)
        {
          //m_tmpBuff[cnt++] = SEND_NACK;
          /*set direction*/
          m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
          m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDAHIGH; /*Value*/
          m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAIN; /*Direction*/

          /*Command to write Ack/nAck bit*/
          m_tmpBuff[cnt++] = MPSSE::CMD_DATA_OUT_BITS_NEG_EDGE;                      // GB CHANGED TO NEG
          m_tmpBuff[cnt++] = MPSSE::DATA_SIZE_1BIT;
          m_tmpBuff[cnt++] = 0xFF;

          /*Back to Idle*/
          m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
          m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDAHIGH; /*Value*/
          m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAIN; /*Direction*/
        }
        else
        {
          //m_tmpBuff[cnt++] = SEND_ACK;
          /*set direction*/
          m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
          m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDALOW; /*Value*/
          m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT; /*Direction*/

          /*Command to write Ack/nAck bit*/
          m_tmpBuff[cnt++] = MPSSE::CMD_DATA_OUT_BITS_NEG_EDGE;                      // GB CHANGED TO NEG
          m_tmpBuff[cnt++] = MPSSE::DATA_SIZE_1BIT;
          m_tmpBuff[cnt++] = 0x00;

          /*Back to Idle*/
          m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
          m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDAHIGH; /*Value*/
          m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAIN; /*Direction*/
        }

      }
/// NACK_GLITCH_WORKAROUND end
    }

    j += bitsInThisTransfer;
  }
  sizeTransferred = j;

  /* Write STOP bit */
  if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_STOP_BIT)
  {
    /* SCL low, SDA low */
    for( j = 0; j < MPSSE::STOP_DURATION_1; ++j )
    {
      m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
      m_tmpBuff[cnt++] = MPSSE::VALUE_SCLLOW_SDALOW;
      m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT;
    }

    /* SCL high, SDA low */
    for( j = 0; j < MPSSE::STOP_DURATION_2; ++j )
    {
      m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
      m_tmpBuff[cnt++] = MPSSE::VALUE_SCLHIGH_SDALOW;
      m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT;
    }

    /* SCL high, SDA high */
    for( j = 0; j < MPSSE::STOP_DURATION_3; ++j )
    {
      m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
      m_tmpBuff[cnt++] = MPSSE::VALUE_SCLHIGH_SDAHIGH;
      m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLOUT_SDAOUT;
    }

    m_tmpBuff[cnt++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
    m_tmpBuff[cnt++] = MPSSE::VALUE_SCLHIGH_SDAHIGH;
    m_tmpBuff[cnt++] = MPSSE::DIRECTION_SCLIN_SDAIN; /* Tristate the SCL & SDA pins */
  }

  esU32 transferred = 0;
  if( !ftWrite(
      m_tmpBuff.data(),
      cnt,
      &transferred
    )
  )
    return false;

  if( transferred != cnt )
    return false;

  // Read the address ack bit
  if(!(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_NO_ADDRESS))
  {
    esU8 addrAck;
    if( !ftRead(
        &addrAck,
        1,
        &transferred
      )
    )
      return false;

    ack = 0x01 == (addrAck & 0x01);
  }

  /* read the actual data from the MPSSE-chip into the host system */
  if(opts & EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_FAST_TRANSFER_BYTES)
  {
    if( !ftRead(
        buffer,
        bytesToTransfer,
        &transferred
      )
    )
      return false;

    return (transferred == bytesToTransfer);
  }
  else
  {
    if( !ftRead(
        buffer,
        bytesToTransfer+1,
        &transferred
      )
    )
      return false;

    return (bytesToTransfer+1) == transferred;
  }
}
//---------------------------------------------------------------------------

esU32 EsFtdiDeviceMpsseI2c::write(esU16 devAddr, EsBinBuffer::const_pointer src, esU32 len, esU32 opts)
{
  if( !len )
    return 0;

  checkOpenChannel( esT("write") );
  checkAddr(static_cast<esU8>(devAddr));

  esU32 ret = 0;
  i2cDeviceWrite(
    devAddr,
    src,
    len,
    ret,
    opts
  );

  return ret;
}
//---------------------------------------------------------------------------

esU32 EsFtdiDeviceMpsseI2c::write(esU16 devAddr, const EsBinBuffer& src, esU32 opts)
{
  if( src.empty() )
    return 0;

  return write(
    devAddr,
    src.data(),
    static_cast<esU32>(src.size()),
    opts
  );
}
//---------------------------------------------------------------------------

esU32 EsFtdiDeviceMpsseI2c::read(esU16 devAddr, EsBinBuffer::pointer dest, esU32 len, esU32 opts)
{
  if( !len )
    return 0;

  checkOpenChannel( esT("read") );
  checkAddr(static_cast<esU8>(devAddr));

  esU32 ret = 0;

  i2cDeviceRead(
    devAddr,
    dest,
    len,
    ret,
    opts
  );

  return ret;
}
//---------------------------------------------------------------------------

EsBinBuffer EsFtdiDeviceMpsseI2c::read(esU16 devAddr, esU32 len, esU32 opts)
{
  if( !len )
    return EsBinBuffer::null();

  EsBinBuffer ret(len);
  esU32 rcnt = read(
    devAddr,
    ret.data(),
    len,
    opts
  );
  if( rcnt != len )
    ret.resize( rcnt );

  return ret;
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseI2c::checkOpenChannel(const EsString& op) const
{
  if( !isOpen() )
    EsException::Throw(
      _("FTDI I2C MPSSE must be open to perform operation '%s'"),
      op
    );
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::GPIOwrite(esU8 val, esU8 dir)
{
  checkOpenChannel(esT("GPIO write"));

  return mpsseGpioWrite(
    val,
    dir
  );
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::GPIOread(esU8& out)
{
  checkOpenChannel(esT("GPIO read"));

  return mpsseGpioRead(
    out
  );
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

ulong EsFtdiDeviceMpsseI2c::get_clockRate() const
{
  return clockRateGet();
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseI2c::set_clockRate(const ulong& rate)
{
  clockRateSet(rate);
}
//---------------------------------------------------------------------------

ulong EsFtdiDeviceMpsseI2c::get_latencyTimer() const
{
  return latencyTimerGet();
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseI2c::set_latencyTimer(const ulong& latency)
{
  latencyTimerSet(static_cast<esU8>(latency));
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::get__3phaseDataClocking() const
{
  return _3phaseDataClockingGet();
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseI2c::set__3phaseDataClocking(const bool& set)
{
  _3phaseDataClockingSet(set);
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::get_driveOnlyZero() const
{
  return driveOnlyZeroGet();
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseI2c::set_driveOnlyZero(const bool& set)
{
  driveOnlyZeroSet(set);
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseI2c::get_clockStretching() const
{
  return clockStretchingGet();
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseI2c::set_clockStretching(const bool& set)
{
  clockStretchingSet(set);
}
//---------------------------------------------------------------------------

EsVariant EsFtdiDeviceMpsseI2c::GPIOread()
{
  esU8 out;
  if( !GPIOread(out) )
    return EsVariant::null();

  return out;
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_FTDI
