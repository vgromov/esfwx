#ifndef _es_ftdi_device_h_
#define _es_ftdi_device_h_

#ifdef ES_COMM_USE_FTDI

#include "EsFtdiDriver.h"

class ESCOMM_CLASS ES_INTF_IMPL2( EsFtdiDevice, EsFtdiDeviceIntf, EsReflectedClassIntf )

protected:
  EsFtdiDevice();
  EsFtdiDevice(EsFtdiDriver* owner, const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& node );
  static EsBaseIntf::Ptr create(EsFtdiDriver& owner, const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& node);

public:
  virtual ~EsFtdiDevice();

  // Reflection declaration
  //
  ES_DECL_REFLECTED_CLASS_BASE( EsFtdiDevice )
  ES_DECL_ATTR_HANDLING_STD

  /// EsFtdiDeviceIntf implementation, partially reflected
  ///

  /// Object validity check
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isValid) ES_OVERRIDE { return m_owner && m_owner->isOk(); }

  /// Return recent driver call status
  ES_DECL_INTF_METHOD(FT_STATUS, statusGet)() const ES_OVERRIDE { return m_stat; }

  /// Bound device info access
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(esU32, flagsGet) ES_OVERRIDE { return m_node.flagsGet(); }
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(esU32, typeGet) ES_OVERRIDE { return m_node.typeGet(); }
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(esU32, idGet) ES_OVERRIDE { return m_node.idGet(); }
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(esU32, locIdGet) ES_OVERRIDE { return m_node.locIdGet(); }
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsString, serialNumberStrGet) ES_OVERRIDE { return m_node.serialNumberStrGet(); }
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsString, descriptionStrGet) ES_OVERRIDE { return m_node.descriptionStrGet(); }
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isOpen) ES_OVERRIDE { return m_node.isOpen(); }
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isHiSpeed) ES_OVERRIDE { return m_node.isHiSpeed(); }
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isMpsseSupported) ES_OVERRIDE { return m_node.mpsseSupported(); }
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(esU32, mpsseChannelsCntGet) ES_OVERRIDE { return m_node.mpsseChannelsCntGet(); }

  /// Open or close Device
  ES_DECL_REFLECTED_INTF_METHOD0(bool, open) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, close) ES_OVERRIDE;

  /// Selected FTDI device services for reflected usage
  ///
  ES_DECL_REFLECTED_METHOD1(EsVariant, read, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD1(EsVariant, write, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD1(void, baudRateSet, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD1(void, divisorSet, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD3(void, dataCharacteristicsSet, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD3(void, flowControlSet, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD0(void, reset);
  ES_DECL_REFLECTED_METHOD0(void, dtrSet);
  ES_DECL_REFLECTED_METHOD0(void, dtrClr);
  ES_DECL_REFLECTED_METHOD0(void, rtsSet);
  ES_DECL_REFLECTED_METHOD0(void, rtsClr);
  ES_DECL_REFLECTED_METHOD4(void, charsSet, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD1(void, purge, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD2(void, timeoutsSet, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD0(void, breakOnSet);
  ES_DECL_REFLECTED_METHOD0(void, breakOffSet);
  ES_DECL_REFLECTED_METHOD0(void, inTaskStop);
  ES_DECL_REFLECTED_METHOD0(void, inTaskRestart);
  ES_DECL_REFLECTED_METHOD1(void, latencyTimerSet, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD0(EsVariant, latencyTimerGet);
  ES_DECL_REFLECTED_METHOD2(void, bitModeSet, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD0(EsVariant, bitModeGet);
  ES_DECL_REFLECTED_METHOD2(void, usbParametersSet, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD1(void, deadmanTimeoutSet, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD0(void, comPortIdleSet);
  ES_DECL_REFLECTED_METHOD0(void, comPortIdleCancel);

  /// FT232H additional EEPROM functions
  ///
  ES_DECL_REFLECTED_METHOD1(EsVariant, eeConfigRead, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD2(EsVariant, eeConfigWrite, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD1(EsVariant, eeEccRead, cr_EsVariant);

  /// Generate FTDI generic device services
	#define FTDI_DRIVER_ENTRY0(Name)
	#define FTDI_DRIVER_ENTRY1(Name, Param1)
	#define FTDI_DRIVER_ENTRY2(Name, Param1, Param2)
	#define FTDI_DRIVER_ENTRY3(Name, Param1, Param2, Param3)
	#define FTDI_DRIVER_ENTRY4(Name, Param1, Param2, Param3, Param4)
	#define FTDI_DRIVER_ENTRY5(Name, Param1, Param2, Param3, Param4, Param5)
	#define FTDI_DRIVER_ENTRY6(Name, Param1, Param2, Param3, Param4, Param5, Param6)
	#define FTDI_DRIVER_ENTRY7(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7)
	#define FTDI_DRIVER_ENTRY8(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)
	#define FTDI_DRIVER_DEV_ENTRY1(Name, Param1) ES_DECL_INTF_METHOD(bool, ft## Name)() ES_OVERRIDE { ES_ASSERT(m_owner); m_stat = m_owner->ft## Name( EsFtdiDriver::nodeHandleAccess(m_node) ); return m_owner->statusCheck( m_stat ); }
	#define FTDI_DRIVER_DEV_ENTRY2(Name, Param1, Param2) ES_DECL_INTF_METHOD(bool, ft## Name)(Param2 p2) ES_OVERRIDE { ES_ASSERT(m_owner); m_stat = m_owner->ft## Name( EsFtdiDriver::nodeHandleAccess(m_node), p2 ); return m_owner->statusCheck( m_stat ); }
	#define FTDI_DRIVER_DEV_ENTRY3(Name, Param1, Param2, Param3) ES_DECL_INTF_METHOD(bool, ft## Name)(Param2 p2, Param3 p3) ES_OVERRIDE { ES_ASSERT(m_owner); m_stat = m_owner->ft## Name( EsFtdiDriver::nodeHandleAccess(m_node), p2, p3 ); return m_owner->statusCheck( m_stat ); }
	#define FTDI_DRIVER_DEV_ENTRY4(Name, Param1, Param2, Param3, Param4) ES_DECL_INTF_METHOD(bool, ft## Name)(Param2 p2, Param3 p3, Param4 p4) ES_OVERRIDE { ES_ASSERT(m_owner); m_stat = m_owner->ft## Name( EsFtdiDriver::nodeHandleAccess(m_node), p2, p3, p4 ); return m_owner->statusCheck( m_stat ); }
	#define FTDI_DRIVER_DEV_ENTRY5(Name, Param1, Param2, Param3, Param4, Param5) ES_DECL_INTF_METHOD(bool, ft## Name)(Param2 p2, Param3 p3, Param4 p4, Param5 p5) ES_OVERRIDE { ES_ASSERT(m_owner); m_stat = m_owner->ft## Name( EsFtdiDriver::nodeHandleAccess(m_node), p2, p3, p4, p5 ); return m_owner->statusCheck( m_stat ); }
	#define FTDI_DRIVER_DEV_ENTRY6(Name, Param1, Param2, Param3, Param4, Param5, Param6) ES_DECL_INTF_METHOD(bool, ft## Name)(Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) ES_OVERRIDE { ES_ASSERT(m_owner); m_stat = m_owner->ft## Name( EsFtdiDriver::nodeHandleAccess(m_node), p2, p3, p4, p5, p6 ); return m_owner->statusCheck( m_stat ); }
	#define FTDI_DRIVER_DEV_ENTRY7(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7) ES_DECL_INTF_METHOD(bool, ft## Name)(Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) ES_OVERRIDE { ES_ASSERT(m_owner); m_stat = m_owner->ft## Name( EsFtdiDriver::nodeHandleAccess(m_node), p2, p3, p4, p5, p6, p7 ); return m_owner->statusCheck( m_stat ); }
	#define FTDI_DRIVER_DEV_ENTRY8(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8) ES_DECL_INTF_METHOD(bool, ft## Name)(Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) ES_OVERRIDE { ES_ASSERT(m_owner); m_stat = m_owner->ft## Name( EsFtdiDriver::nodeHandleAccess(m_node), p2, p3, p4, p5, p6, p7, p8 ); return m_owner->statusCheck( m_stat ); }
	#include "EsFtdiDriver.win.hxx"

  // Reflected device properties
  ES_DECL_PROPERTY_RO(isOpen, bool)
  ES_DECL_PROPERTY_RO(isValid, bool)
  ES_DECL_PROPERTY_RO(status, ulong)
  ES_DECL_PROPERTY_RO(ioAndEvtStatus, EsVariant)
  ES_DECL_PROPERTY_RO(queueStatus, ulong)
  ES_DECL_PROPERTY_RO(queueStatusEx, ulong)
  ES_DECL_PROPERTY_RO(modemStatus, ulong)
  ES_DECL_PROPERTY_RO(driverVersion, ulong)
  ES_DECL_PROPERTY_RO(comPortNumber, long)
  ES_DECL_PROPERTY_RO(deviceInfo, EsVariant)

protected:
  /// Internal services
  ///

  bool loopbackStateSet(bool set);
  bool inBuffClear();
  bool latencyAndRateSetup(esU8 latency, esU32 rate, bool setFlowOff);

  /// MPSSE helpers
  void mpsseBufInit();
  bool mpsseReset();
  bool mpsseEnable();
  bool mpsseSynch();
  bool mpsseCmdSend(esU8 cmd, int echo, esU8& cmdEchoed); //< echo = 0 - no echo, 1 - echo once, 2 - continuous echo
  bool mpsseClockSet(esU32 clock);
  bool mpsseOpen();
  bool mpsseChannelInit(esU32 rate);
  bool mpsseGpioLowSet(esU8 val, esU8 dir);
  bool mpsseGpioWrite(esU8 val, esU8 dir);
  bool mpsseGpioRead(esU8& out);

protected:
  EsFtdiDriver* m_owner;
  EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE m_node;
  mutable EsBinBuffer m_tmpBuff;
  FT_STATUS m_stat;

	esU32	m_cfgClockRate; ///< Device lock rate, used mainly in deriverd classes

  //  Latency timer timeout in milliseconds.
  //	Valid range is 1 – 255
  //	In the FT8U232AM and FT8U245AM devices, the receive buffer timeout that is used to flush
  //	remaining data from the receive buffer was fixed at 16 ms. In all other FTDI devices, this
  //	timeout is programmable and can be set at 1 ms intervals between 1ms and 255 ms.  This
  //	allows the device to be better optimized for protocols requiring faster response times from
  //	short data packets
  //	NOTE: This is a per-chip feature and not per device
  //
  esU8 m_cfgLatencyTimer;

private:
  EsFtdiDevice(const EsFtdiDevice&) ES_REMOVEDECL;
  EsFtdiDevice& operator=(const EsFtdiDevice&) ES_REMOVEDECL;

  friend class EsFtdiDriver;
};

#endif // ES_COMM_USE_FTDI

#endif // _es_ftdi_device_h_
