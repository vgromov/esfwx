// macro-x definitions file for ftdi channel implementation
//

// ftdi driver entries. each entry contains call name w\o FT_ prefix (which is added automatically)
// and the list of parameter types passed to the driver entry call
//
FTDI_DRIVER_DEV_ENTRY2( GetDriverVersion, FT_HANDLE, esU32* )
FTDI_DRIVER_ENTRY1( GetLibraryVersion, esU32* )
FTDI_DRIVER_ENTRY0( Rescan )
FTDI_DRIVER_ENTRY2( Reload, esU16, esU16 )
FTDI_DRIVER_DEV_ENTRY2( GetComPortNumber, FT_HANDLE, esI32* )
FTDI_DRIVER_ENTRY2( Open, int, FT_HANDLE* )
FTDI_DRIVER_ENTRY3( OpenEx, void*, esU32, FT_HANDLE* )
FTDI_DRIVER_ENTRY1( CreateDeviceInfoList, esU32* )
FTDI_DRIVER_ENTRY3( ListDevices, void*, void*, esU32 )
FTDI_DRIVER_ENTRY2( GetDeviceInfoList, FT_DEVICE_LIST_INFO_NODE*, esU32*)
FTDI_DRIVER_ENTRY1( Close, FT_HANDLE )
FTDI_DRIVER_DEV_ENTRY4( Read, FT_HANDLE, void*, esU32, esU32* )
FTDI_DRIVER_DEV_ENTRY4( Write, FT_HANDLE, const void*, esU32, esU32* )
FTDI_DRIVER_DEV_ENTRY2( SetBaudRate, FT_HANDLE, esU32 )
FTDI_DRIVER_DEV_ENTRY2( SetDivisor, FT_HANDLE, esU16 )
FTDI_DRIVER_DEV_ENTRY4( SetDataCharacteristics, FT_HANDLE, esU8, esU8, esU8 )
FTDI_DRIVER_DEV_ENTRY4( SetFlowControl, FT_HANDLE, esU16, esU8, esU8 )
FTDI_DRIVER_DEV_ENTRY1( ResetDevice, FT_HANDLE )
FTDI_DRIVER_DEV_ENTRY1( SetDtr, FT_HANDLE )
FTDI_DRIVER_DEV_ENTRY1( ClrDtr, FT_HANDLE )
FTDI_DRIVER_DEV_ENTRY1( SetRts, FT_HANDLE )
FTDI_DRIVER_DEV_ENTRY1( ClrRts, FT_HANDLE )
FTDI_DRIVER_DEV_ENTRY2( GetModemStatus, FT_HANDLE, esU32* )
FTDI_DRIVER_DEV_ENTRY5( SetChars, FT_HANDLE, esU8,  esU8, esU8, esU8 )
FTDI_DRIVER_DEV_ENTRY2( Purge, FT_HANDLE, esU32 )
FTDI_DRIVER_DEV_ENTRY3( SetTimeouts, FT_HANDLE, esU32, esU32 )
FTDI_DRIVER_DEV_ENTRY2( GetQueueStatus, FT_HANDLE, esU32* )
FTDI_DRIVER_DEV_ENTRY3( SetEventNotification, FT_HANDLE, esU32,  void* )
FTDI_DRIVER_DEV_ENTRY2( GetEventStatus, FT_HANDLE, esU32* )
FTDI_DRIVER_DEV_ENTRY4( GetStatus, FT_HANDLE, esU32*, esU32*, esU32* )
FTDI_DRIVER_DEV_ENTRY1( SetBreakOn, FT_HANDLE )
FTDI_DRIVER_DEV_ENTRY1( SetBreakOff, FT_HANDLE )
FTDI_DRIVER_DEV_ENTRY1( StopInTask, FT_HANDLE )
FTDI_DRIVER_DEV_ENTRY1( RestartInTask, FT_HANDLE )
FTDI_DRIVER_DEV_ENTRY2( SetLatencyTimer, FT_HANDLE, esU8 )
FTDI_DRIVER_DEV_ENTRY2( GetLatencyTimer, FT_HANDLE, esU8* )
FTDI_DRIVER_DEV_ENTRY3( SetBitMode, FT_HANDLE, esU8, esU8 )
FTDI_DRIVER_DEV_ENTRY2( GetBitMode, FT_HANDLE, esU8* )
FTDI_DRIVER_DEV_ENTRY3( SetUSBParameters, FT_HANDLE, esU32, esU32 )
FTDI_DRIVER_DEV_ENTRY2( SetDeadmanTimeout, FT_HANDLE, esU32 )
FTDI_DRIVER_DEV_ENTRY6( GetDeviceInfo, FT_HANDLE, FT_DEVICE*,  esU32*, char*, char*,  void*  )
FTDI_DRIVER_DEV_ENTRY2( GetQueueStatusEx, FT_HANDLE, esU32*  )
FTDI_DRIVER_DEV_ENTRY1( ComPortIdle, FT_HANDLE )
FTDI_DRIVER_DEV_ENTRY1( ComPortCancelIdle, FT_HANDLE )
// FT232H additional EEPROM functions
FTDI_DRIVER_DEV_ENTRY3( EE_ReadConfig, FT_HANDLE,  esU8,  esU8*  )
FTDI_DRIVER_DEV_ENTRY3( EE_WriteConfig, FT_HANDLE, esU8, esU8  )
FTDI_DRIVER_DEV_ENTRY3( EE_ReadECC, FT_HANDLE, esU8, esU16*  )
#if ES_OS == ES_OS_WINDOWS
FTDI_DRIVER_DEV_ENTRY8( IoCtl, FT_HANDLE, esU32, void*, esU32, void*, esU32, esU32*, LPOVERLAPPED )
FTDI_DRIVER_DEV_ENTRY2( SetWaitMask, FT_HANDLE, esU32 )
FTDI_DRIVER_DEV_ENTRY2( WaitOnMask, FT_HANDLE, esU32* )
#endif

// macro definitions housekeeping
#undef FTDI_DRIVER_ENTRY0
#undef FTDI_DRIVER_ENTRY1
#undef FTDI_DRIVER_ENTRY2
#undef FTDI_DRIVER_ENTRY3
#undef FTDI_DRIVER_ENTRY4
#undef FTDI_DRIVER_ENTRY5
#undef FTDI_DRIVER_ENTRY6
#undef FTDI_DRIVER_ENTRY7
#undef FTDI_DRIVER_ENTRY8
#undef FTDI_DRIVER_DEV_ENTRY1
#undef FTDI_DRIVER_DEV_ENTRY2
#undef FTDI_DRIVER_DEV_ENTRY3
#undef FTDI_DRIVER_DEV_ENTRY4
#undef FTDI_DRIVER_DEV_ENTRY5
#undef FTDI_DRIVER_DEV_ENTRY6
#undef FTDI_DRIVER_DEV_ENTRY7
#undef FTDI_DRIVER_DEV_ENTRY8

