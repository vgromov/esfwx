#ifndef _es_ftdi_device_mpsse_spi_h_
#define _es_ftdi_device_mpsse_spi_h_

#include "EsFtdiDriver.h"
#include "EsFtdiDevice.h"

#ifdef ES_COMM_USE_FTDI

/// FTDI SPI over MPSSE device implementation
///
class ESCOMM_CLASS ES_INTF_IMPL_DERIVED2( EsFtdiDeviceMpsseSpi, EsFtdiDevice,
                                                                EsFtdiMpsseSpiIntf,
                                                                EsFtdiMpsseGpioIntf
                                        )
protected:
  /// Internal flags
  ///
  enum {
    flagUnInitialized         = 0x0001,
    flagLatencyChanged        = 0x0002,
    flagRateChanged           = 0x0004,
    flagCsSelectChanged       = 0x0008
  };

protected:
  EsFtdiDeviceMpsseSpi(EsFtdiDriver* owner, const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& node);
  static EsBaseIntf::Ptr create(EsFtdiDriver& owner, const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& node);

public:
  /// Minimal derived Reflection services declaration
  ///
  ES_DECL_REFLECTED_CLASS( EsFtdiDeviceMpsseSpi, EsFtdiDevice )

  /// EsBaseIntf implementation
  ///
   ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return classNameGetStatic(); }

  /// EsFtdiDeviceIntf overrides
  ///
  ES_DECL_INTF_METHOD(bool, open)() ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, close)() ES_OVERRIDE;

  /// EsFtdiMpsseSpiIntf implementation
  ///
  
  /// Configuration fields access
  ///
  ES_DECL_INTF_METHOD(esU32, clockRateGet)() const ES_OVERRIDE { return m_cfgClockRate; }
  ES_DECL_INTF_METHOD(void, clockRateSet)(esU32 rate) ES_OVERRIDE;

  ES_DECL_INTF_METHOD(esU8, latencyTimerGet)() const ES_OVERRIDE { return m_cfgLatencyTimer; }
  ES_DECL_INTF_METHOD(void, latencyTimerSet)(esU8 tmo) ES_OVERRIDE;

  /// SPI data capture and polarity control
  ES_DECL_INTF_METHOD(EsFtdiMpsseSpiIntf::CPOL_CPHA, modeGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, modeSet)( EsFtdiMpsseSpiIntf::CPOL_CPHA opts ) ES_OVERRIDE;

  /// SPI CS line selector control
  ES_DECL_INTF_METHOD(EsFtdiMpsseSpiIntf::CS_LINE, csLineCtlGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, csLineCtlSet)(EsFtdiMpsseSpiIntf::CS_LINE ctl) ES_OVERRIDE;

  /// (Re) apply pending configuration changes
  ES_DECL_REFLECTED_INTF_METHOD0(void, configApply) ES_OVERRIDE;

  /// SPI active state (low or high) of CS line
  ES_DECL_INTF_METHOD(bool, csLineActiveHighGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, csLineActiveHighSet)(bool set) ES_OVERRIDE;

  /// Pins initial direction, initial state, final direction, final state
  /// State of the pins other than ones used for SPI, may be controlled here.
  /// SPI-occupied lines states and configs will be internally preserved for SPI use
  ///
  ES_DECL_INTF_METHOD(esU8, pinsDirInitGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, pinsDirInitSet)(esU8 set) ES_OVERRIDE;

  ES_DECL_INTF_METHOD(esU8, pinsDirFinalGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, pinsDirFinalSet)(esU8 set) ES_OVERRIDE;

  ES_DECL_INTF_METHOD(esU8, pinsStateInitGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, pinsStateInitSet)(esU8 set) ES_OVERRIDE;

  ES_DECL_INTF_METHOD(esU8, pinsStateFinalGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, pinsStateFinalSet)(esU8 set) ES_OVERRIDE;

  // Pins current direction and state
  ES_DECL_INTF_METHOD(esU8, pinsDirCurrentGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(esU8, pinsStateCurrentGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(bool, pinsStateCurrentSet)(esU8 set) ES_OVERRIDE;

  /// Return current MISO line state
  ES_DECL_REFLECTED_INTF_METHOD0(bool, misoPoll) ES_OVERRIDE;

  /// Write data to SPI channel, using SPI transfer options - see corresponding enum decl
  /// Returned is count of bytes written
  ///
  ES_DECL_INTF_METHOD(esU32, write)(EsBinBuffer::const_pointer src, esU32 len, esU32 opts) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(esU32, write, cr_EsBinBuffer, esU32) ES_OVERRIDE;

  /// Read data from SPI channel, using SPI transfer options - see corresponding enum decl
  /// Returned is count of bytes read|acquired data buffer
  ///
  ES_DECL_INTF_METHOD(esU32, read)(EsBinBuffer::pointer dest, esU32 len, esU32 opts) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(EsBinBuffer, read, esU32, esU32) ES_OVERRIDE;

  /// Perform combined SPI read-write call
  ES_DECL_INTF_METHOD(esU32, readWrite)(EsBinBuffer::const_pointer src, EsBinBuffer::pointer dest, esU32 len, esU32 opts) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(EsBinBuffer, readWrite, cr_EsBinBuffer, esU32) ES_OVERRIDE;

  /// Set state of CS line
  ES_DECL_REFLECTED_INTF_METHOD1(void, csSet, bool) ES_OVERRIDE;
  
  /// EsFtdiMpsseGpioIntf implementation
  ///

  /// Configure direction and initial state of GPIO pins. Must be called at least once
  /// before reading from GPIO
  ///
  ES_DECL_INTF_METHOD(bool, GPIOwrite)(esU8 val, esU8 dir) ES_OVERRIDE;

  /// Read NB! input lines of GPIO, previously configured by GPIOwrite command
  ES_DECL_INTF_METHOD(bool, GPIOread)(esU8& out) ES_OVERRIDE;

  /// Reflected GPIO services
  ///
  ES_DECL_REFLECTED_METHOD2(EsVariant, GPIOwrite, cr_EsVariant, cr_EsVariant) { return GPIOwrite(p1.asByte(), p2.asByte()); }
  ES_DECL_REFLECTED_METHOD0(EsVariant, GPIOread);

  /// Reflected SPI properties
  ///
  ES_DECL_PROPERTY(clockRate, ulong)
  ES_DECL_PROPERTY(latencyTimer, ulong)
  ES_DECL_PROPERTY(mode, ulong)
  ES_DECL_PROPERTY(csLineCtl, ulong)
  ES_DECL_PROPERTY(csLineActiveHigh, bool)
  ES_DECL_PROPERTY(pinDirectionInitial, byte)
  ES_DECL_PROPERTY(pinDirectionFinal, byte)
  ES_DECL_PROPERTY(pinStateInitial, byte)
  ES_DECL_PROPERTY(pinStateFinal, byte)
  ES_DECL_PROPERTY_RO(pinDirectionCurrent, byte)
  ES_DECL_PROPERTY(pinStateCurrent, byte)

protected:
  // Check channel is open for op
  void checkOpenChannel(const EsString& op) const;
  inline bool havePendingConfigChanges() const { return 0 != m_flags; }

  // Internal SPI helpers
  //
  bool spiCsLineSelect();
  bool spiInit();
  bool spiUnInit();
  bool spiToggleCS(bool state);
  bool spiIsBusy(bool& busy);
  bool spiWrite8bits(esU8 byte, esU8 len);
  bool spiWrite(EsBinBuffer::const_pointer buffer, esU32 sizeToTransfer, esU32& sizeTransferred, esU32 opts);
  bool spiRead8bits(esU8& byte, esU8 len);
  bool spiRead(EsBinBuffer::pointer buffer, esU32 sizeToTransfer, esU32& sizeTransferred, esU32 opts);
  bool spiReadWrite(EsBinBuffer::const_pointer src, EsBinBuffer::pointer dest, esU32 sizeToTransfer, esU32& sizeTransferred, esU32 opts);

protected:
  // Configuration fields
  //

  //  This member provides a way to enable/disable features
  //  specific to the protocol that are implemented in the chip
  //  BIT1-0=CPOL-CPHA:  00 - MODE0 - data captured on rising edge, propagated on falling
  //            01 - MODE1 - data captured on falling edge, propagated on rising
  //            10 - MODE2 - data captured on falling edge, propagated on rising
  //            11 - MODE3 - data captured on rising edge, propagated on falling
  //  BIT4-BIT2: 000 - A/B/C/D_DBUS3=ChipSelect
  //       : 001 - A/B/C/D_DBUS4=ChipSelect
  //       : 010 - A/B/C/D_DBUS5=ChipSelect
  //       : 011 - A/B/C/D_DBUS6=ChipSelect
  //       : 100 - A/B/C/D_DBUS7=ChipSelect
  //  BIT5: ChipSelect is active high if this bit is 0
  //  BIT6 -BIT31    : Reserved
  //
  esU32  m_optSPI;

  // BIT7  -BIT0:  Initial direction of the pins
  // BIT15 -BIT8:  Initial values of the pins
  // BIT23 -BIT16: Final direction of the pins
  // BIT31 -BIT24: Final values of the pins
  //
  esU32  m_optPin;

  // BIT7  -BIT0: Current direction of the pins
  // BIT15 -BIT8:   Current values of the pins
  //
  esU16  m_currentPinState;

  // Upon spi channel close, return pins to this state
  esU16 m_finalPinState;

  /// Config && state change flags
  esU32 m_flags;

private:
  EsFtdiDeviceMpsseSpi() ES_REMOVEDECL;
  EsFtdiDeviceMpsseSpi(const EsFtdiDeviceMpsseSpi&) ES_REMOVEDECL;
  EsFtdiDeviceMpsseSpi& operator=(const EsFtdiDeviceMpsseSpi&) ES_REMOVEDECL;

  friend class EsFtdiDriver;
};

#endif // ES_COMM_USE_FTDI

#endif // _es_ftdi_device_mpsse_spi_h_
