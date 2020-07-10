#include "escommpch.h"
#pragma hdrstop

#include "EsFtdiDeviceMpsseSpi.h"

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

class EsFtdiDeviceMpsseSpiCtorAcc : public EsFtdiDeviceMpsseSpi
{
public:
  EsFtdiDeviceMpsseSpiCtorAcc(EsFtdiDriver* owner, const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& node) :
  EsFtdiDeviceMpsseSpi(
    owner,
    node
  )
  {}
};
//---------------------------------------------------------------------------

ES_DECL_CLASS_INFO_DERIVED_BEGIN( EsFtdiDeviceMpsseSpi, EsFtdiDevice, NO_CLASS_DESCR )
  /// SPI Intf
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDeviceMpsseSpi, EsFtdiMpsseSpiIntf, configApply, void_Call, NO_METHOD_DESCR )
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDeviceMpsseSpi, EsFtdiMpsseSpiIntf, misoPoll, bool_Call, NO_METHOD_DESCR )
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDeviceMpsseSpi, EsFtdiMpsseSpiIntf, write, esU32_Call_cr_EsBinBuffer_esU32, NO_METHOD_DESCR )
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDeviceMpsseSpi, EsFtdiMpsseSpiIntf, read, EsBinBuffer_Call_esU32_esU32, NO_METHOD_DESCR )
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDeviceMpsseSpi, EsFtdiMpsseSpiIntf, readWrite, EsBinBuffer_Call_cr_EsBinBuffer_esU32, NO_METHOD_DESCR )
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDeviceMpsseSpi, EsFtdiMpsseSpiIntf, csSet, void_Call_bool, NO_METHOD_DESCR )
  /// GPIO intf
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDeviceMpsseSpi, EsFtdiMpsseGpioIntf, GPIOwrite, EsVariant_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR )
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsFtdiDeviceMpsseSpi, EsFtdiMpsseGpioIntf, GPIOread, EsVariant_Call, NO_METHOD_DESCR )
  /// Properties
  ES_DECL_PROP_INFO_RANGE(            EsFtdiDeviceMpsseSpi, clockRate, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(EsFtdiMpsseSpiIntf::RATE_DEF), static_cast<ulong>(EsFtdiMpsseSpiIntf::RATE_MIN), static_cast<ulong>(EsFtdiMpsseSpiIntf::RATE_MAX), NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RANGE(            EsFtdiDeviceMpsseSpi, latencyTimer, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(EsFtdiDeviceIntf::LATENCY_DEF), static_cast<ulong>(EsFtdiDeviceIntf::LATENCY_MIN), static_cast<ulong>(EsFtdiDeviceIntf::LATENCY_MAX), NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RESTRICTED(       EsFtdiDeviceMpsseSpi, mode, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE0), ES_ENUM(EsReflection::EsFtdiMpsseSpiCpolCpha), NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RESTRICTED(       EsFtdiDeviceMpsseSpi, csLineCtl, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(EsFtdiMpsseSpiIntf::CS_LINE::ABCD_DBUS3), ES_ENUM(EsReflection::EsFtdiMpsseSpiCsLine), NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO(                  EsFtdiDeviceMpsseSpi, csLineActiveHigh, bool, NO_PROPERTY_LABEL, true, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO(                  EsFtdiDeviceMpsseSpi, pinDirectionInitial, byte, NO_PROPERTY_LABEL, static_cast<ulong>(0), NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO(                  EsFtdiDeviceMpsseSpi, pinDirectionFinal, byte, NO_PROPERTY_LABEL, static_cast<ulong>(0), NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO(                  EsFtdiDeviceMpsseSpi, pinStateInitial, byte, NO_PROPERTY_LABEL, static_cast<ulong>(0), NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO(                  EsFtdiDeviceMpsseSpi, pinStateFinal, byte, NO_PROPERTY_LABEL, static_cast<ulong>(0), NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(               EsFtdiDeviceMpsseSpi, pinDirectionCurrent, byte, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO(                  EsFtdiDeviceMpsseSpi, pinStateCurrent, byte, NO_PROPERTY_LABEL, static_cast<ulong>(0), NO_PROPERTY_DESCR )
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsFtdiDeviceMpsseSpi::EsFtdiDeviceMpsseSpi(EsFtdiDriver* owner, const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& node) :
EsFtdiDeviceMpsseSpi::BaseImplT(),
m_optSPI(0),
m_optPin(0),
m_currentPinState(0),
m_finalPinState(0),
m_flags(flagUnInitialized)
{
  ES_ASSERT( node.mpsseSupported() );
  m_owner = owner;
  m_node = node;
  m_cfgClockRate = EsFtdiMpsseSpiIntf::RATE_DEF;
}
//---------------------------------------------------------------------------

EsBaseIntf::Ptr EsFtdiDeviceMpsseSpi::create(EsFtdiDriver& owner, const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& node)
{
  std::unique_ptr<EsFtdiDeviceMpsseSpiCtorAcc> ptr = ES_MAKE_UNIQUE(
    EsFtdiDeviceMpsseSpiCtorAcc,
    &owner,
    node
  );
  ES_ASSERT(ptr);

  ptr->m_dynamic = true;
  return ptr.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::spiCsLineSelect()
{
  /* Ensure new CS lines is set as OUT */
  m_currentPinState |= ((1<<((m_currentPinState & MPSSE::SPI_CONFIG_OPTION_CS_MASK)>>2))<<3);

  /*Set initial state of clock line*/
  esU8 mode = static_cast<esU8>(m_optSPI & MPSSE::SPI_CONFIG_OPTION_MODE_MASK);
  switch(mode)
  {
    case 0:
    case 1:
      /* clock idle low */
      m_currentPinState &= 0xFEFF;
      break;

    case 2:
    case 3:
      /* clock idle high */
      m_currentPinState |= 0x0100;
      break;
    default:
      ES_FAIL; //< Should never be here
  }

  mpsseBufInit();

  esU32 noOfBytes = 0;
  m_tmpBuff[noOfBytes++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
  m_tmpBuff[noOfBytes++] = static_cast<esU8>((m_currentPinState & 0xFF00)>>8);/*Val*/
  m_tmpBuff[noOfBytes++] = static_cast<esU8>(m_currentPinState & 0x00FF); /*Dir*/

  esU32 noOfBytesTransferred = 0;
  if( !ftWrite(
      m_tmpBuff.data(),
      noOfBytes,
      &noOfBytesTransferred
    )
  )
    return false;

  if( noOfBytesTransferred != noOfBytes )
  {
    m_stat = FT_IO_ERROR;
    return false;
  }

  return true;
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::spiInit()
{
  if( !(m_flags & flagUnInitialized) )
    return true;

  // This is to ensure that the library puts the lines to correct directions
  // even if wrong values are passed by the user
  // Set initial direction of line SCLK  as OUT
  //
  esU32 optPin = m_optPin;
  optPin |= 0x00000001;/*Note: Direction is out if bit is 1!!! */
  /* Set initial direction of  MOSI line as OUT */
  optPin |= 0x00000002;
  /* Set initial direction of MISO line as IN */
  optPin &= 0xFFFFFFFB;
  /* Set initial direction of CS line as OUT */
  optPin |= ((1<<((m_optSPI & MPSSE::SPI_CONFIG_OPTION_CS_MASK)>>2))<<3);

  m_currentPinState = static_cast<esU16>(optPin) & 0xFFFF;
  m_finalPinState = static_cast<esU16>(optPin >> 16);

  if( !mpsseChannelInit(
        m_cfgClockRate
      )
  )
    return false;

  m_flags = 0;
  return spiCsLineSelect();
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::spiUnInit()
{
  mpsseBufInit();

  esU32 noOfBytes = 0;

  /* Set lines to final state */
  m_tmpBuff[noOfBytes++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
  m_tmpBuff[noOfBytes++] = static_cast<esU8>((m_finalPinState & 0xFF00) >> 8); /*Value*/
  m_tmpBuff[noOfBytes++] = static_cast<esU8>(m_finalPinState & 0x00FF); /*Direction*/

  esU32 noOfBytesTransferred = 0;
  if( !ftWrite(
      m_tmpBuff.data(),
      noOfBytes,
      &noOfBytesTransferred
    )
  )
    return false;

  m_flags = flagUnInitialized; //< Reset flags to uninitialized state

  return (noOfBytesTransferred == noOfBytes);
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::spiToggleCS(bool state)
{
  esU32 optSPI = m_optSPI;

  bool activeLow = (optSPI & MPSSE::SPI_CONFIG_OPTION_CS_ACTIVELOW) ?
    true :
    false;

  //direction = (esU8)config->currentPinState;/*get current state*/
  esU8 direction = static_cast<esU8>(m_currentPinState & 0x00FF);//20110718
  direction |= static_cast<esU8>((1<<((optSPI & MPSSE::SPI_CONFIG_OPTION_CS_MASK)>>2))<<3);

  //oldValue = (esU8)(8>>config->currentPinState);
  esU8 oldValue = static_cast<esU8>((m_currentPinState & 0xFF00)>>8);//20110718
  esU8 value = static_cast<esU8>((1<<((optSPI & MPSSE::SPI_CONFIG_OPTION_CS_MASK)>>2))<<3);

  ES_DEBUG_TRACE(
    esT("EsFtdiDeviceMpsseSpi::spiToggleCS oldValue=0x%0X value=0x%0X"),
    (int)oldValue,
    (int)value
  );

  if(state != activeLow)
    value = oldValue | value; /* set the CS line high */

  if(state == activeLow)
    value = oldValue & ~value;/* set the CS line low */

  m_currentPinState = static_cast<esU16>((static_cast<esU16>(value)<<8) | direction);/*save  dirn & value*/

  mpsseBufInit();

  esU32 noOfBytes = 0;
  /*MPSSE command to set low bytes*/
  m_tmpBuff[noOfBytes++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
  m_tmpBuff[noOfBytes++] = value;    /*value*/
  m_tmpBuff[noOfBytes++] = direction;  /*direction*/

  esU32 noOfBytesTransferred = 0;
  if( !ftWrite(
      m_tmpBuff.data(),
      noOfBytes,
      &noOfBytesTransferred
    )
  )
    return false;

  return (noOfBytesTransferred == noOfBytes);
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::spiIsBusy(bool& busy)
{
  /*Enable CS*/
  if( !spiToggleCS(true) )
    return false;

  ES_ASSERT( !m_tmpBuff.empty() );

  esU32 noOfBytes = 0;
  /*Send command to read*/
  m_tmpBuff[noOfBytes++] = MPSSE::CMD_GET_DATA_BITS_LOWBYTE;
  m_tmpBuff[noOfBytes++] = MPSSE::CMD_SEND_IMMEDIATE;

  esU32 noOfBytesTransferred = 0;
  if( !ftWrite(
      m_tmpBuff.data(),
      noOfBytes,
      &noOfBytesTransferred
    )
  )
    return false;

  if( noOfBytes != noOfBytesTransferred )
  {
    m_stat = FT_IO_ERROR;
    return false;
  }

  noOfBytesTransferred=0;

  /*Read*/
  if( !ftRead(
      m_tmpBuff.data(),
      1,
      &noOfBytesTransferred
    )
  )
    return false;

  if( 1 != noOfBytesTransferred )
  {
    m_stat = FT_IO_ERROR;
    return false;
  }

  ES_DEBUG_TRACE(
    esT("EsFtdiDeviceMpsseSpi::spiIsBusy Low byte read = 0x%0X"),
    (int)m_tmpBuff[0]
  );

  if(0 == (m_tmpBuff[0] & 0x04))
    busy = false;
  else
    busy = true;

  /*Disable CS*/
  return spiToggleCS(false);
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::spiWrite8bits(esU8 byte, esU8 len)
{
  mpsseBufInit();

  /*mode is given by bit1-bit0 of SPI_ChannelConfig.Options*/
  esU32 optSPI = m_optSPI;
  esU8 mode = static_cast<esU8>(optSPI & MPSSE::SPI_CONFIG_OPTION_MODE_MASK);

  esU32 noOfBytes = 0;
  /* Command to write 8bits */
  switch( mode )
  {
    case 0:
      m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_OUT_BITS_NEG_EDGE;
      break;
    case 1:
      m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_OUT_BITS_POS_EDGE;
      break;
    case 2:
      m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_OUT_BITS_POS_EDGE;
      break;
    case 3:
      m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_OUT_BITS_NEG_EDGE;
      break;
    default:
      ES_FAIL; //< Should never be here
  }

  m_tmpBuff[noOfBytes++] = len-1;/* 1bit->arg=0, for 8bits->arg=7 */
  m_tmpBuff[noOfBytes++] = byte;

  esU32 noOfBytesTransferred = 0;
  if( !ftWrite(
      m_tmpBuff.data(),
      noOfBytes,
      &noOfBytesTransferred
    )
  )
    return false;

  if( noOfBytesTransferred != noOfBytes )
    m_stat = FT_IO_ERROR;

  return (noOfBytesTransferred == noOfBytes);
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::spiWrite(EsBinBuffer::const_pointer buffer,
  esU32 sizeToTransfer, esU32& sizeTransferred, esU32 opts
)
{
  /* Mode is given by bit1-bit0 of SPI_ChannelConfig.Options */
  if(opts & EsFtdiMpsseSpiIntf::TRANSFER_OPTIONS_CHIPSELECT_ENABLE)
  {
    /* enable CHIPSELECT line for the channel */
    if( !spiToggleCS(true) )
      return false;
  }

  esU32 optSPI = m_optSPI;
  bool ok = true;
  if(opts & EsFtdiMpsseSpiIntf::TRANSFER_OPTIONS_SIZE_IN_BITS)
  {/* sizeToTransfer is in bits */
    sizeTransferred = 0;
    /* loop until all the bits are transferred */
    while(sizeTransferred < sizeToTransfer)
    {
      esU32 bitsToTransfer;
      if( (sizeToTransfer - sizeTransferred) >= 8)
        bitsToTransfer = 8;
      else
        bitsToTransfer = static_cast<esU8>(sizeToTransfer - sizeTransferred);

      esU8 byte = buffer[(sizeTransferred+1)/8];
      if(
        !spiWrite8bits(
          byte,
          static_cast<esU8>(bitsToTransfer)
        )
      )
      {
        ok = false;
        break;
      }

      sizeTransferred += bitsToTransfer;
    }
  }
  else
  {/* sizeToTransfer is in bytes */
    mpsseBufInit();

    /*mode is given by bit1-bit0 of SPI_ChannelConfig.Options*/
    esU8 mode = static_cast<esU8>(optSPI & MPSSE::SPI_CONFIG_OPTION_MODE_MASK);

    esU32 noOfBytes = 0;
    /* Command to write 8bits */
    switch(mode)
    {
      case EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE0:
        m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_OUT_BYTES_NEG_EDGE;
        break;
      case EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE1:
        m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_OUT_BYTES_POS_EDGE;
        break;
      case EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE2:
        m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_OUT_BYTES_POS_EDGE;
        break;
      case EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE3:
        m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_OUT_BYTES_NEG_EDGE;
        break;
      default:
        ES_FAIL; //< Should never be here
    }

    /* length low byte */
    m_tmpBuff[noOfBytes++] = static_cast<esU8>((sizeToTransfer-1) & 0x000000FF);
    /* length high byte */
    m_tmpBuff[noOfBytes++] = static_cast<esU8>(((sizeToTransfer-1) & 0x0000FF00)>>8);

    esU32 noOfBytesTransferred = 0;

    /* write command */
    if( !ftWrite(
        m_tmpBuff.data(),
        noOfBytes,
        &noOfBytesTransferred
      )
    )
    {
      ok = false;
      goto finalize;
    }

    if( noOfBytes != noOfBytesTransferred )
    {
      m_stat = FT_IO_ERROR;
      ok = false;
      goto finalize;
    }

    ok = ftWrite(
        buffer,
        sizeToTransfer,
        &sizeTransferred
    );
  }

finalize:
  if(opts & EsFtdiMpsseSpiIntf::TRANSFER_OPTIONS_CHIPSELECT_DISABLE)
  {
    /* disable CHIPSELECT line for the channel */
    spiToggleCS(false);
  }

  return ok;
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::spiRead8bits(esU8& byte, esU8 len)
{
  mpsseBufInit();

  esU32 optSPI = m_optSPI;

  /*mode is given by bit1-bit0 of SPI_ChannelConfig.Options*/
  esU8 mode = (optSPI & MPSSE::SPI_CONFIG_OPTION_MODE_MASK);
  esU32 noOfBytes = 0;

  /* Command to write 8bits */
  switch(mode)
  {
  case 0:
    m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_IN_BITS_POS_EDGE;
    break;
  case 1:
    m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_IN_BITS_NEG_EDGE;
    break;
  case 2:
    m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_IN_BITS_NEG_EDGE;
    break;
  case 3:
    m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_IN_BITS_POS_EDGE;
    break;
  default:
    ES_FAIL;
  }

  m_tmpBuff[noOfBytes++] = len-1;/* 1bit->arg=0, for 8bits->arg=7 */

  /*Command MPSSE to send data to PC immediately */
  m_tmpBuff[noOfBytes++] = MPSSE::CMD_SEND_IMMEDIATE;

  esU32 noOfBytesTransferred = 0;
  if( !ftWrite(
      m_tmpBuff.data(),
      noOfBytes,
      &noOfBytesTransferred
    )
  )
    return false;

  if( noOfBytesTransferred != noOfBytes )
  {
    m_stat = FT_IO_ERROR;
    return false;
  }

  noOfBytesTransferred = 0;
  if( !ftRead(
      m_tmpBuff.data(),
      1,
      &noOfBytesTransferred
    )
  )
    return false;

  if( noOfBytesTransferred != 1 )
  {
    m_stat = FT_IO_ERROR;
    return false;
  }

  byte = m_tmpBuff[0];

  return true;
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::spiRead(EsBinBuffer::pointer buffer, esU32 sizeToTransfer, esU32& sizeTransferred, esU32 opts)
{
  if(opts & EsFtdiMpsseSpiIntf::TRANSFER_OPTIONS_CHIPSELECT_ENABLE)
  {
    /* Enable CHIPSELECT line for the channel */
    if( !spiToggleCS(true) )
      return false;
  }

  bool ok = true;
  if(opts & EsFtdiMpsseSpiIntf::TRANSFER_OPTIONS_SIZE_IN_BITS)
  {/*sizeToTransfer is in bits*/
    sizeTransferred = 0;

    while( sizeTransferred < sizeToTransfer )
    {
      esU8 bitsToTransfer;

      if( (sizeToTransfer - sizeTransferred) >= 8 )
        bitsToTransfer = 8;
      else
        bitsToTransfer = static_cast<esU8>(sizeToTransfer - sizeTransferred);

      esU8 byte = 0;
      if( !spiRead8bits(
          byte,
          bitsToTransfer
        )
      )
      {
        ok = false;
        break;
      }

      buffer[(sizeTransferred+1)/8] = byte;

      sizeTransferred += bitsToTransfer;
    }
  }
  else
  {/*sizeToTransfer is in bytes*/
    mpsseBufInit();

    /*mode is given by bit1-bit0 of SPI_ChannelConfig.Options*/
    esU32 optSPI = m_optSPI;
    esU8 mode = static_cast<esU8>(optSPI & MPSSE::SPI_CONFIG_OPTION_MODE_MASK);

    esU32 noOfBytes = 0;
    /* Command to write 8bits */
    switch(mode)
    {
      case 0:
        m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_IN_BYTES_POS_EDGE;
        break;
      case 1:
        m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_IN_BYTES_NEG_EDGE;
        break;
      case 2:
        m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_IN_BYTES_NEG_EDGE;
        break;
      case 3:
        m_tmpBuff[noOfBytes++] = MPSSE::CMD_DATA_IN_BYTES_POS_EDGE;
        break;
      default:
        ES_FAIL; //< Should never be here
    }

    /* length LSB */
    m_tmpBuff[noOfBytes++] = static_cast<esU8>((sizeToTransfer-1) & 0x000000FF);

    /* length MSB */
    m_tmpBuff[noOfBytes++] = static_cast<esU8>(((sizeToTransfer-1) & 0x0000FF00)>>8);

    /*Command MPSSE to send data to PC immediately */
    m_tmpBuff[noOfBytes++] = MPSSE::CMD_SEND_IMMEDIATE;

    esU32 noOfBytesTransferred = 0;
    if( !ftWrite(
        m_tmpBuff.data(),
        noOfBytes,
        &noOfBytesTransferred
      )
    )
    {
      ok = false;
      goto finalize;
    }

    if( noOfBytesTransferred != noOfBytes )
    {
      m_stat = FT_IO_ERROR;
      ok = false;
      goto finalize;
    }

    ok = ftRead(
      buffer,
      sizeToTransfer,
      &sizeTransferred
    );
  }

finalize:
  if(opts & EsFtdiMpsseSpiIntf::TRANSFER_OPTIONS_CHIPSELECT_DISABLE)
  {
    /* Disable CHIPSELECT line for the channel */
    spiToggleCS(false);
  }

  return ok;
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::spiReadWrite(EsBinBuffer::const_pointer src, EsBinBuffer::pointer dest, esU32 sizeToTransfer, esU32& sizeTransferred, esU32 opts)
{
  mpsseBufInit();

  esU32 optSPI = m_optSPI;

  if(opts & EsFtdiMpsseSpiIntf::TRANSFER_OPTIONS_CHIPSELECT_ENABLE)
  {
    /* enable CHIPSELECT line for the channel */
    if( !spiToggleCS(true) )
      return false;
  }

  /*mode is given by bit1-bit0 of SPI_ChannelConfig.Options*/
  esU8 mode = static_cast<esU8>(optSPI & MPSSE::SPI_CONFIG_OPTION_MODE_MASK);

  /* start of transfer */
  bool ok = true;
  if(opts & EsFtdiMpsseSpiIntf::TRANSFER_OPTIONS_SIZE_IN_BITS)
  {/* sizeToTransfer is in bits */
    sizeTransferred=0;

    /* Command to write 8bits */
    switch(mode)
    {
      case EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE0:
        m_tmpBuff[0] = MPSSE::CMD_DATA_BITS_IN_POS_OUT_NEG_EDGE;
        break;
      case EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE1:
        m_tmpBuff[0] = MPSSE::CMD_DATA_BITS_IN_NEG_OUT_POS_EDGE;
        break;
      case EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE2:
        m_tmpBuff[0] = MPSSE::CMD_DATA_BITS_IN_NEG_OUT_POS_EDGE;
        break;
      case EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE3:
        m_tmpBuff[0] = MPSSE::CMD_DATA_BITS_IN_POS_OUT_NEG_EDGE;
        break;
      default:
        ES_FAIL; //< Should never be here
    }

    while( sizeTransferred < sizeToTransfer )
    {
      esU8 bitsToTransfer;

      if((sizeToTransfer - sizeTransferred)>=8)
        bitsToTransfer = 8;
      else
        bitsToTransfer = static_cast<esU8>(sizeToTransfer - sizeTransferred);

      m_tmpBuff[1] = bitsToTransfer - 1; /*takes value 0 for 1 bit; 7 for 8 bits*/
      m_tmpBuff[2] = src[(sizeTransferred+1)/8];

      esU32 noOfBytesTransferred = 0;

      /*Write command and data*/
      if( !ftWrite(
          m_tmpBuff.data(),
          3,
          &noOfBytesTransferred
        )
      )
      {
        ok = false;
        break;
      }

      if(3 > noOfBytesTransferred)
      {
        ok = false;
        break;
      }

      noOfBytesTransferred = 0;

      /*Read from buffer*/
      if( !ftRead(
          dest+((sizeTransferred+1)/8),
          1,
          &noOfBytesTransferred
        )
      )
      {
        ok = false;
        break;
      }

      if(1 > noOfBytesTransferred)
      {
        ok = false;
        break;
      }

      sizeTransferred += bitsToTransfer;
    }
  }
  else
  {/*sizeToTransfer is in bytes*/
    sizeTransferred = 0;

    /* Command to write 8bits */
    switch(mode)
    {
      case EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE0:
        m_tmpBuff[0] = MPSSE::CMD_DATA_BYTES_IN_POS_OUT_NEG_EDGE;
        break;
      case EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE1:
        m_tmpBuff[0] = MPSSE::CMD_DATA_BYTES_IN_NEG_OUT_POS_EDGE;
        break;
      case EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE2:
        m_tmpBuff[0] = MPSSE::CMD_DATA_BYTES_IN_NEG_OUT_POS_EDGE;
        break;
      case EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE3:
        m_tmpBuff[0] = MPSSE::CMD_DATA_BYTES_IN_POS_OUT_NEG_EDGE;
        break;
      default:
        ES_FAIL; //< Should neve be here
    }

    m_tmpBuff[1] = static_cast<esU8>((sizeToTransfer-1) & 0x000000FF);/* lengthL */
    m_tmpBuff[2] = static_cast<esU8>(((sizeToTransfer-1) & 0x0000FF00)>>8);/*lenghtH*/

    esU32 noOfBytesTransferred = 0;

    /*Write command*/
    if( !ftWrite(
        m_tmpBuff.data(),
        3,
        &noOfBytesTransferred
      )
    )
    {
      ok = false;
      goto finalize;
    }

    if( 3 > noOfBytesTransferred )
    {
      m_stat = FT_IO_ERROR;
      ok = false;
      goto finalize;
    }

    noOfBytesTransferred = 0;
    /*Write data*/
    if( !ftWrite(
        src,
        sizeToTransfer,
        &noOfBytesTransferred
      )
    )
    {
      ok = false;
      goto finalize;
    }

    sizeTransferred = noOfBytesTransferred;

    if( noOfBytesTransferred != sizeToTransfer )
    {
      m_stat = FT_IO_ERROR;
      ok = false;
      goto finalize;
    }

    noOfBytesTransferred = 0;
    /*Read to buffer*/
    if( !ftRead(
        dest,
        sizeToTransfer,
        &noOfBytesTransferred
      )
    )
    {
      ok = false;
      goto finalize;
    }

    sizeTransferred = noOfBytesTransferred;

    if( noOfBytesTransferred != sizeToTransfer )
    {
      m_stat = FT_IO_ERROR;
      ok = false;
      goto finalize;
    }
  }
  /* end of transfer */

finalize:
  if(opts & EsFtdiMpsseSpiIntf::TRANSFER_OPTIONS_CHIPSELECT_DISABLE)
  {
    /* disable CHIPSELECT line for the channel */
    spiToggleCS(false);
  }

  return ok;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::open()
{
  if( mpsseOpen() )
  {
    if( m_flags & flagUnInitialized )
      spiInit(); //< Initialize once
    else
      configApply();

    return (0 == m_flags);
  }

  return false;
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::close()
{
  if( isOpen() )
  {
    spiUnInit();
    EsFtdiDevice::close();
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::checkOpenChannel(const EsString& op) const
{
  if( !isOpen() )
    EsException::Throw(
      _("FTDI SPI MPSSE must be open to perform operation '%s'"),
      op
    );
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::GPIOwrite(esU8 val, esU8 dir)
{
  checkOpenChannel(esT("GPIO write"));

  return mpsseGpioWrite(
    val,
    dir
  );
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::GPIOread(esU8& out)
{
  checkOpenChannel(esT("GPIO read"));

  return mpsseGpioRead(
    out
  );
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::clockRateSet(esU32 rate)
{
  if( rate < RATE_MIN )
    rate = RATE_MIN;
  if( rate > RATE_MAX )
    rate = RATE_MAX;

  if( m_cfgClockRate != rate )
  {
    m_cfgClockRate = rate;
    m_flags |= flagRateChanged;
  }
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::latencyTimerSet(esU8 tmo)
{
  if( tmo < LATENCY_MIN )
    tmo = LATENCY_MIN;
  if( tmo > LATENCY_MAX )
    tmo = LATENCY_MAX;

  if( m_cfgLatencyTimer != tmo )
  {
    m_cfgLatencyTimer = tmo;
    m_flags |= flagLatencyChanged;
  }
}
//---------------------------------------------------------------------------

EsFtdiDeviceMpsseSpi::CPOL_CPHA EsFtdiDeviceMpsseSpi::modeGet() const
{
  switch( (m_optSPI & 0x03) )
  {
  case 0x00:
    return CPOL_CPHA::MODE0;
  case 0x01:
    return CPOL_CPHA::MODE1;
  case 0x02:
    return CPOL_CPHA::MODE2;
  default:
    return CPOL_CPHA::MODE3;
  }
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::modeSet( EsFtdiDeviceMpsseSpi::CPOL_CPHA opts )
{
  esU32 optSPI = (m_optSPI & 0x03);
  esU32 newOptSPI = 0; //< CPOL_CPHA::MODE0: - bits are already cleared
  switch(opts)
  {
  case CPOL_CPHA::MODE1:
    newOptSPI = 0x01;
    break;
  case CPOL_CPHA::MODE2:
    newOptSPI = 0x02;
    break;
  case CPOL_CPHA::MODE3:
    newOptSPI = 0x03;
    break;
  }

  if( newOptSPI != optSPI )
  {
    m_optSPI &= ~0x03; //< Clear 2 lower bits
    m_optSPI |= newOptSPI; //< Assign new opts

    // These changes are handled by CS select updater
    m_flags |= flagCsSelectChanged;
  }
}
//---------------------------------------------------------------------------

EsFtdiDeviceMpsseSpi::CS_LINE EsFtdiDeviceMpsseSpi::csLineCtlGet() const
{
  esU32 linesel = ((m_optSPI >> 2) & 0x07);

  switch( linesel )
  {
  case 0:
    return CS_LINE::ABCD_DBUS3;
  case 1:
    return CS_LINE::ABCD_DBUS4;
  case 2:
    return CS_LINE::ABCD_DBUS5;
  case 3:
    return CS_LINE::ABCD_DBUS6;
  default:
    return CS_LINE::ABCD_DBUS7;
  }
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::csLineCtlSet(EsFtdiDeviceMpsseSpi::CS_LINE ctl)
{
  esU32 optSPI = ((m_optSPI >> 2) & 0x07);
  esU32 newOptSPI = 0; //< CS_LINE::ABCD_DBUS3:
  switch( ctl )
  {
  case CS_LINE::ABCD_DBUS4:
    newOptSPI = 0x01;
    break;
  case CS_LINE::ABCD_DBUS5:
    newOptSPI = 0x02;
    break;
  case CS_LINE::ABCD_DBUS6:
    newOptSPI = 0x03;
    break;
  case CS_LINE::ABCD_DBUS7:
    newOptSPI = 0x04;
    break;
  }

  if( optSPI != newOptSPI )
  {
    // Clear CS line selection bits
    m_optSPI &= 0x1C;
    // Assign new opts
    m_optSPI |= (newOptSPI << 2);

    // These changes are handled by CS select updater
    m_flags |= flagCsSelectChanged;
  }
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::csLineActiveHighGet() const
{
  //  BIT5: ChipSelect is active high if this bit is 0
  return 0 == (m_optSPI & 0x20);
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::csLineActiveHighSet(bool high)
{
  bool activeHigh = csLineActiveHighGet();

  if( activeHigh != high )
  {
    m_optSPI &= ~0x20;
    if( !high )
      m_optSPI |= 0x20;

    // These changes are handled by CS select updater
    m_flags |= flagCsSelectChanged;
  }
}
//---------------------------------------------------------------------------

esU8 EsFtdiDeviceMpsseSpi::pinsDirInitGet() const
{
  return (m_optPin & 0xFF);
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::pinsDirInitSet(esU8 set)
{
  if( pinsDirInitGet() != set )
  {
    m_optPin &= ~0xFF;
    m_optPin |= set;

    // These changes are handled by CS select updater
    m_flags |= flagCsSelectChanged;
  }
}
//---------------------------------------------------------------------------

esU8 EsFtdiDeviceMpsseSpi::pinsDirFinalGet() const
{
  return ((m_optPin >> 16) & 0xFF);
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::pinsDirFinalSet(esU8 set)
{
  if( pinsDirFinalGet() != set )
  {
    m_optPin &= ~0xFF0000;
    m_optPin |= (static_cast<esU32>(set) << 16);
  }
}
//---------------------------------------------------------------------------

esU8 EsFtdiDeviceMpsseSpi::pinsStateInitGet() const
{
  return ((m_optPin >> 8) & 0xFF);
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::pinsStateInitSet(esU8 set)
{
  if( pinsStateInitGet() != set )
  {
    m_optPin &= ~0xFF00;
    m_optPin |= (static_cast<esU32>(set) << 8);
  }
}
//---------------------------------------------------------------------------

esU8 EsFtdiDeviceMpsseSpi::pinsStateFinalGet() const
{
  return ((m_optPin >> 24) & 0xFF);
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::pinsStateFinalSet(esU8 set)
{
  if( pinsStateFinalGet() != set )
  {
    m_optPin &= ~0xFF000000;
    m_optPin |= (static_cast<esU32>(set) << 24);
  }
}
//---------------------------------------------------------------------------

esU8 EsFtdiDeviceMpsseSpi::pinsDirCurrentGet() const
{
  return (m_currentPinState & 0xFF);
}
//---------------------------------------------------------------------------

esU8 EsFtdiDeviceMpsseSpi::pinsStateCurrentGet() const
{
  return ((m_currentPinState >> 8) & 0xFF);
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::pinsStateCurrentSet(esU8 set)
{
  // Mask-off input pins
  esU8 currentDir = pinsDirCurrentGet();
  esU8 currentState = pinsStateCurrentGet();
  // Preserve current vaue of SPI-occupied pins (CS, MOSI, MISO, CLK)
  esU8 spiMask = 0xF8;
  switch((m_optSPI >> 2) & 0x07)
  {
  case 0:
    spiMask &= ~0x08;
    break;
  case 1:
    spiMask &= ~0x10;
    break;
  case 2:
    spiMask &= ~0x20;
    break;
  case 3:
    spiMask &= ~0x40;
    break;
  case 4:
    spiMask &= ~0x80;
    break;
  }

  esU8 mask = spiMask & currentDir;
  currentState |= ((currentState & ~mask) | (set & mask));

  m_currentPinState &= 0x00FF; //< Clear current state bits
  m_currentPinState |= static_cast<esU16>(currentState << 8);

  mpsseBufInit();

  esU32 noOfBytes = 0;
  /*MPSSE command to set low bytes*/
  m_tmpBuff[noOfBytes++] = MPSSE::CMD_SET_DATA_BITS_LOWBYTE;
  m_tmpBuff[noOfBytes++] = static_cast<esU8>((m_currentPinState & 0xFF00)>>8);/*Val*/
  m_tmpBuff[noOfBytes++] = currentDir;

  esU32 noOfBytesTransferred = 0;
  if( !ftWrite(
      m_tmpBuff.data(),
      noOfBytes,
      &noOfBytesTransferred
    )
  )
    return false;

  return (noOfBytesTransferred == noOfBytes);
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::misoPoll()
{
  checkOpenChannel( esT("MISO Polling") );

  bool state = false;
  spiIsBusy( state );

  return state;
}
//---------------------------------------------------------------------------

esU32 EsFtdiDeviceMpsseSpi::write(EsBinBuffer::const_pointer src, esU32 len, esU32 opts)
{
  if( !len )
    return 0;

  checkOpenChannel( esT("write") );

  esU32 ret = 0;

  spiWrite(
    src,
    len,
    ret,
    opts
  );

  return ret;
}
//---------------------------------------------------------------------------

esU32 EsFtdiDeviceMpsseSpi::write(const EsBinBuffer& src, esU32 opts)
{
  if( src.empty() )
    return 0;

  return write(
    src.data(),
    static_cast<esU32>(src.size()),
    opts
  );
}
//---------------------------------------------------------------------------

esU32 EsFtdiDeviceMpsseSpi::read(EsBinBuffer::pointer dest, esU32 len, esU32 opts)
{
  if( !len )
    return 0;

  checkOpenChannel( esT("read") );

  esU32 ret = 0;

  spiRead(
    dest,
    len,
    ret,
    opts
  );

  return ret;
}
//---------------------------------------------------------------------------

EsBinBuffer EsFtdiDeviceMpsseSpi::read(esU32 len, esU32 opts)
{
  if( !len )
    return EsBinBuffer::null();

  EsBinBuffer ret(len);
  esU32 rcnt = read(
    ret.data(),
    len,
    opts
  );

  if( rcnt != len )
    ret.resize(rcnt);

  return ret;
}
//---------------------------------------------------------------------------

esU32 EsFtdiDeviceMpsseSpi::readWrite(EsBinBuffer::const_pointer src, EsBinBuffer::pointer dest, esU32 len, esU32 opts)
{
  ES_ASSERT( src != dest );

  if( !len )
    return 0;

  checkOpenChannel( esT("read-write") );

  esU32 ret = 0;

  spiReadWrite(
    src,
    dest,
    len,
    ret,
    opts
  );

  return ret;
}
//---------------------------------------------------------------------------

EsBinBuffer EsFtdiDeviceMpsseSpi::readWrite(const EsBinBuffer& src, esU32 opts)
{
  if( src.empty() )
    return EsBinBuffer::null();

  EsBinBuffer ret( src.size() );
  esU32 rcnt = readWrite(
    src.data(),
    ret.data(),
    static_cast<esU32>(src.size()),
    opts
  );

  if( ret.size() != rcnt )
    ret.resize( rcnt );

  return ret;
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::configApply()
{
  if( !havePendingConfigChanges() )
    return;

  if( !isOpen() )
    return;

  if( m_flags & flagLatencyChanged )
  {
    if( !ftSetLatencyTimer(m_cfgLatencyTimer) )
      return;

    m_flags &= ~flagLatencyChanged;
  }

  if( m_flags & flagRateChanged )
  {
    if( !mpsseClockSet(m_cfgClockRate) )
      return;

    m_flags &= ~flagRateChanged;
  }

  if( m_flags & flagCsSelectChanged )
  {
    if( !spiCsLineSelect() )
      return;

    m_flags &= ~flagCsSelectChanged;
  }
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::csSet(bool set)
{
  checkOpenChannel( esT("CS set") );

  spiToggleCS( set );
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsVariant EsFtdiDeviceMpsseSpi::GPIOread()
{
  esU8 out;
  if( !GPIOread(out) )
    return EsVariant::null();

  return out;
}
//---------------------------------------------------------------------------

ulong EsFtdiDeviceMpsseSpi::get_clockRate() const
{
  return clockRateGet();
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::set_clockRate(const ulong& rate)
{
  clockRateSet(rate);
}
//---------------------------------------------------------------------------

ulong EsFtdiDeviceMpsseSpi::get_latencyTimer() const
{
  return latencyTimerGet();
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::set_latencyTimer(const ulong& tmo)
{
  latencyTimerSet(static_cast<esU8>(tmo));
}
//---------------------------------------------------------------------------

ulong EsFtdiDeviceMpsseSpi::get_mode() const
{
  return static_cast<ulong>(
    modeGet()
  );
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::set_mode(const ulong& mode)
{
  if( !EsEnumHasValue<EsReflection::EsFtdiMpsseSpiCpolCpha>(mode) )
    EsException::Throw(
      esT("Unknown FTDI SPI CPOL_CPHA mode %d"),
      mode
    );

  modeSet(
    static_cast<EsFtdiMpsseSpiIntf::CPOL_CPHA>(mode)
  );
}
//---------------------------------------------------------------------------

ulong EsFtdiDeviceMpsseSpi::get_csLineCtl() const
{
  return static_cast<ulong>(
    csLineCtlGet()
  );
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::set_csLineCtl(const ulong& ctl)
{
  if( !EsEnumHasValue<EsReflection::EsFtdiMpsseSpiCsLine>(ctl) )
    EsException::Throw(
      esT("Unknown FTDI SPI CS Line selection %d"),
      ctl
    );

  csLineCtlSet(
    static_cast<EsFtdiMpsseSpiIntf::CS_LINE>(ctl)
  );
}
//---------------------------------------------------------------------------

bool EsFtdiDeviceMpsseSpi::get_csLineActiveHigh() const
{
  return csLineActiveHighGet();
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::set_csLineActiveHigh(const bool& high)
{
  csLineActiveHighSet(high);
}
//---------------------------------------------------------------------------

byte EsFtdiDeviceMpsseSpi::get_pinDirectionInitial() const
{
  return pinsDirInitGet();
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::set_pinDirectionInitial(const byte& dir)
{
  pinsDirInitSet(dir);
}
//---------------------------------------------------------------------------

byte EsFtdiDeviceMpsseSpi::get_pinDirectionFinal() const
{
  return pinsDirFinalGet();
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::set_pinDirectionFinal(const byte& dir)
{
  pinsDirFinalSet(dir);
}
//---------------------------------------------------------------------------

byte EsFtdiDeviceMpsseSpi::get_pinStateInitial() const
{
  return pinsStateInitGet();
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::set_pinStateInitial(const byte& state)
{
  pinsStateInitSet(state);
}
//---------------------------------------------------------------------------

byte EsFtdiDeviceMpsseSpi::get_pinStateFinal() const
{
  return pinsStateFinalGet();
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::set_pinStateFinal(const byte& state)
{
  pinsStateFinalSet(state);
}
//---------------------------------------------------------------------------

byte EsFtdiDeviceMpsseSpi::get_pinDirectionCurrent() const
{
  return pinsDirCurrentGet();
}
//---------------------------------------------------------------------------

byte EsFtdiDeviceMpsseSpi::get_pinStateCurrent() const
{
  return pinsStateCurrentGet();
}
//---------------------------------------------------------------------------

void EsFtdiDeviceMpsseSpi::set_pinStateCurrent(const byte& state)
{
  pinsStateCurrentSet(state);
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_FTDI
