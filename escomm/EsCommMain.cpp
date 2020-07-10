#include "escommpch.h"
#pragma hdrstop

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

# include "esfwxe/utils.c"
# include "esfwxe/commintf.c"

#ifdef ES_COMM_USE_RPC
extern "C" {

void ekosfWait(esU32 tmo)
{
  EsThread::sleep(tmo);
}

# include "esfwxe/crc.c"
# include "esfwxe/protocols/proprietary/protocolEkosf.c"

} //< extern "C"
#endif

ES_REFLECTION_REGISTRAR_BEGIN(escomm)

#ifdef ES_COMM_USE_CHANNEL_UART
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsUartIoParity)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsUartIoStopBits)
  ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoUart)
#endif

#ifdef ES_COMM_USE_FTDI
  ES_REFLECTION_REGISTRAR_ENTRY(EsFtdiDriver)
  ES_REFLECTION_REGISTRAR_ENTRY(EsFtdiDevice)
  ES_REFLECTION_REGISTRAR_ENTRY(EsFtdiDeviceMpsseI2c)
  ES_REFLECTION_REGISTRAR_ENTRY(EsFtdiDeviceMpsseSpi)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsFtdiDeviceKind)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsFtdiDeviceFlag)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsFtdiDeviceFlowCtl)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsFtdiDevicePurge)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsFtdiMpsseI2cClock)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsFtdiMpsseI2cIO)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsFtdiMpsseSpiCpolCpha)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsFtdiMpsseSpiCsLine)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsFtdiMpsseSpiIO)

# ifdef ES_COMM_USE_CHANNEL_EKONNECT
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsEkonnectIoParity)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsEkonnectIoStopBits)
  ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoEkonnect)
# endif

#endif

#ifdef ES_COMM_USE_SOCKETS
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsSocketError)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsSocketAddrType)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsSocketType)
  ES_REFLECTION_REGISTRAR_ENTRY(EsSocketAddr)
  ES_REFLECTION_REGISTRAR_ENTRY(EsSockets)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsSocketClient)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsSocketServer)

# ifdef ES_COMM_USE_CHANNEL_IO_SOCKET
  ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoSocketClient)
  ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoSocketServer)
# endif

# ifdef ES_COMM_USE_BLUETOOTH
  ES_REFLECTION_REGISTRAR_ENTRY(EsBluetoothDeviceClass)
  ES_REFLECTION_REGISTRAR_ENTRY(EsBluetoothRadio)
  ES_REFLECTION_REGISTRAR_ENTRY(EsBluetoothRadioEnumerator)
# endif

# ifdef ES_COMM_USE_BLUETOOTH_LE
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsBluetoothLEformatType)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsBluetoothLEscanResponseKey)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsBluetoothLEresult)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsBluetoothLEdescriptorKind)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsBluetoothLEcharacteristicProperty)
  ES_REFLECTION_REGISTRAR_ENTRY(EsBluetoothLEdescriptor)
  ES_REFLECTION_REGISTRAR_ENTRY(EsBluetoothLEcharacteristic)
  ES_REFLECTION_REGISTRAR_ENTRY(EsBluetoothLEservice)
  ES_REFLECTION_REGISTRAR_ENTRY(EsBluetoothLEdevice)
  ES_REFLECTION_REGISTRAR_ENTRY(EsBluetoothLEdeviceEnumerator)
  ES_REFLECTION_REGISTRAR_ENTRY(EsBluetoothLE)
# endif

# ifdef ES_COMM_USE_CHANNEL_BLUETOOTH
  ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoBluetoothClient)
# endif
#endif

#ifdef ES_COMM_USE_CHANNEL_BLUETOOTH_LE
  ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoBluetoothLE)
#endif

#ifdef ES_COMM_USE_CHANNEL_STUB
  ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoStub)
#endif

#ifdef ES_COMM_USE_RPC
  ES_REFLECTION_REGISTRAR_ENTRY(EsRpcMaster)
  ES_REFLECTION_REGISTRAR_ENTRY(EsRpcStatus)
  ES_REFLECTION_REGISTRAR_ENTRY(EsNetAddressPool)
#endif

#if defined(ES_COMM_USE_ANY_CHANNEL)

  ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoFactory)
  ES_REFLECTION_REGISTRAR_ENTRY(EsCommUtils)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsByteReadStatus)

#endif

ES_REFLECTION_REGISTRAR_END

#if !(ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND && defined(ECC_USE_PACKAGE))
# if defined(WIN32) && defined(ES_USE_DLLS)
BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason, LPVOID lpReserved)
{
  switch( fdwReason )
  {
  case DLL_PROCESS_ATTACH:
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    break;
  }

  return TRUE;
}
# endif
#endif

