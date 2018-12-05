#ifndef _es_ftdi_device_mpsse_i2c_h_
#define _es_ftdi_device_mpsse_i2c_h_

#ifdef ES_COMM_USE_FTDI

#include "EsFtdiDriver.h"
#include "EsFtdiDevice.h"

/// FTDI I2C over MPSSE device implementation
///
class ESCOMM_CLASS ES_INTF_IMPL_DERIVED2( EsFtdiDeviceMpsseI2c, EsFtdiDevice,
                                                                EsFtdiMpsseI2cIntf,
                                                                EsFtdiMpsseGpioIntf
                                        )

protected:
  EsFtdiDeviceMpsseI2c(EsFtdiDriver* owner, const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& node);
  static EsBaseIntf::Ptr create(EsFtdiDriver& owner, const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& node);

public:
  /// Minimal derived Reflection services declaration
  ///
  ES_DECL_REFLECTED_CLASS( EsFtdiDeviceMpsseI2c, EsFtdiDevice )

  /// EsBaseIntf implementation
  ///
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return classNameGetStatic(); }

  /// EsFtdiDeviceIntf overrides
  ///
  ES_DECL_INTF_METHOD(bool, open)() ES_OVERRIDE;

  /// EsFtdiMpsseI2cIntf impl
  ///
  /// Configuration elements access. If channel is currently open, configuration changes
  /// will take place upon next successfull channel opening.
  ///
  ES_DECL_INTF_METHOD(esU32, clockRateGet)() const ES_OVERRIDE { return m_cfgClockRate; }
  ES_DECL_INTF_METHOD(void, clockRateSet)(esU32 rate) ES_OVERRIDE;

  ES_DECL_INTF_METHOD(esU8, latencyTimerGet)() const ES_OVERRIDE { return m_cfgLatencyTimer; }
  ES_DECL_INTF_METHOD(void, latencyTimerSet)(esU8 tmo) ES_OVERRIDE;

  ES_DECL_INTF_METHOD(bool, _3phaseDataClockingGet)() const ES_OVERRIDE { return m_opt3PhaseDataClocking; }
  ES_DECL_INTF_METHOD(void, _3phaseDataClockingSet)(bool set) ES_OVERRIDE;

  ES_DECL_INTF_METHOD(bool, driveOnlyZeroGet)() const ES_OVERRIDE { return m_optLoopback; }
  ES_DECL_INTF_METHOD(void, driveOnlyZeroSet)(bool set) ES_OVERRIDE;

  ES_DECL_INTF_METHOD(bool, clockStretchingGet)() const ES_OVERRIDE { return m_optClockStretching; }
  ES_DECL_INTF_METHOD(void, clockStretchingSet)(bool set) ES_OVERRIDE;

  /// (Re) apply pending configuration changes
  ES_DECL_REFLECTED_INTF_METHOD0(void, configApply) ES_OVERRIDE;

  /// Try to ping slave at specified address
  ES_DECL_REFLECTED_INTF_METHOD2(bool, ping, esU16, bool) ES_OVERRIDE;

  /// Write data to I2C channel, to slave devAddr, using I2C transfer options - see corresponding enum decl
  /// Returned is count of bytes written
  ///
  ES_DECL_INTF_METHOD(esU32, write)(esU16 devAddr, EsBinBuffer::const_pointer src, esU32 len, esU32 opts) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD3(esU32, write, esU16, cr_EsBinBuffer, esU32) ES_OVERRIDE;

  /// Read data from I2C channel, from slave devAddr, using I2C transfer options - see corresponding enum decl
  /// Returned is count of bytes read|acquired data buffer
  ///
  ES_DECL_INTF_METHOD(esU32, read)(esU16 devAddr, EsBinBuffer::pointer dest, esU32 len, esU32 opts) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD3(EsBinBuffer, read, esU16, esU32, esU32) ES_OVERRIDE;

  /// EsFtdiMpsseGpioIntf impl
  ///

  /// Configure direction and initial state of GPIO pins. Must be called at least once
  /// before reading from GPIO
  ///
  ES_DECL_INTF_METHOD(bool, GPIOwrite)(esU8 val, esU8 dir) ES_OVERRIDE;

  /// Read NB! input lines of GPIO, previously configured by GPIOwrite command
  ES_DECL_INTF_METHOD(bool, GPIOread)(esU8& out) ES_OVERRIDE;

  /// Reflected I2C services
  ///
  ES_DECL_REFLECTED_METHOD2(EsVariant, GPIOwrite, cr_EsVariant, cr_EsVariant) { return GPIOwrite(p1.asByte(), p2.asByte()); }
  ES_DECL_REFLECTED_METHOD0(EsVariant, GPIOread);

  /// Reflected I2C properties
  ///
  ES_DECL_PROPERTY(clockRate, ulong)
  ES_DECL_PROPERTY(latencyTimer, ulong)
  ES_DECL_PROPERTY(_3phaseDataClocking, bool)
  ES_DECL_PROPERTY(driveOnlyZero, bool)
  ES_DECL_PROPERTY(clockStretching, bool)

protected:
  // Check channel is open for op
  void checkOpenChannel(const EsString& op) const;

  // Check address is within proper range 0..127
  static void checkAddr(esU8 addr);

  /// Internal I2C IO helpers
  ///
  bool i2cChannelInit( esU32 rate, esU32 opts );
  bool i2cInit( esU32 opts );
  bool i2cStart();
  bool i2cStop();
  bool i2cWrite8bitsAndGetAck( esU8 data, bool& ack );
  bool i2cRead8bitsAndGiveAck( esU8& data, bool doAck);
  bool i2cDeviceAddressWrite( esU16 addr, bool& ack, bool forWrite, bool _10bitAddr = false);
  bool i2cDeviceRead(esU16 addr, EsBinBuffer::pointer buffer, esU32 toWrite, esU32& written, esU32 opts);
  bool i2cDeviceWrite(esU16 addr, EsBinBuffer::const_pointer buffer, esU32 sizeToTransfer, esU32& sizeTransferred, esU32 opts);
  bool i2cFastWrite(esU16 addr, EsBinBuffer::const_pointer buffer, esU32 sizeToTransfer, esU32& sizeTransferred, bool& ack, esU32 opts);
  bool i2cFastRead(esU16 addr, EsBinBuffer::pointer buffer, esU32 sizeToTransfer, esU32& sizeTransferred, bool& ack, esU32 opts);

protected:
  /// I2C Configuration members
  ///

  // NOTE: m_cfgClockRate usage
  // There were 2 functions I2C_TurnOn/OffDivideByFive
  // ClockinghiSpeedDevice (FTC_HANDLE fthandle) in the old DLL. This function turns on the
  // divide by five for the MPSSE clock to allow the hi-speed devices FT2232H and FT4232H to
  // clock at the same rate as the FT2232D device. This allows for backward compatibility
  // NOTE: This feature is probably a per chip feature and not per device

  // These members provide a way to enable/disable features
  // specific to the protocol that are implemented in the chip
  //
  // BIT0    : 3PhaseDataClocking - Setting this bit will turn on 3 phase data clocking for a
  //     FT2232H dual hi-speed device or FT4232H quad hi-speed device. Three phase
  //     data clocking, ensures the data is valid on both edges of a clock
  //
  bool m_opt3PhaseDataClocking;

  // BIT1    : Loopback, AKA I2C_ENABLE_DRIVE_ONLY_ZERO
  // The I2C master should actually drive the SDA line only when the output is LOW. It should
  // tristate the SDA line when the output should be high. This tristating the SDA line during high
  // output is supported only in FT232H chip. This feature is called DriveOnlyZero feature and is
  // enabled when the following bit is set in the options parameter in function I2C_Init */
  //
  bool m_optLoopback;

  // BIT2    : Clock stretching
  bool m_optClockStretching;

  bool m_cfgChanged; //< Pending changes to config options

private:
  EsFtdiDeviceMpsseI2c() ES_REMOVEDECL;
  EsFtdiDeviceMpsseI2c(const EsFtdiDeviceMpsseI2c&) ES_REMOVEDECL;
  EsFtdiDeviceMpsseI2c& operator=(const EsFtdiDeviceMpsseI2c&) ES_REMOVEDECL;

  friend class EsFtdiDriver;
};

#endif // ES_COMM_USE_FTDI

#endif // _es_ftdi_device_mpsse_i2c_h_
