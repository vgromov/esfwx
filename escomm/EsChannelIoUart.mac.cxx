// MAC superset of POSIX-specific UART channel methods implementation.
// NB! Do not include this file explicitly in project, it's got implicitly
// included in EsChannelIoUart class code.
//
// Implementation is taken from:
// File: SerialPortSample.c
// Abstract: Command line tool that demonstrates how to use IOKitLib to find all serial ports on OS X. Also shows how to open, write to, read from, and close a serial port.
// Version: 1.5
//

#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
//---------------------------------------------------------------------------

// Returns an iterator across all known modems. Caller is responsible for
// releasing the iterator when iteration is complete.
static bool macosSerialSvcsFind(io_iterator_t *matchingServices, CFStringRef svcName)
{
  // Serial devices are instances of class IOSerialBSDClient.
  // Create a matching dictionary to find those instances.
  //
  CFMutableDictionaryRef classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);

  if(classesToMatch)
  {
    // Look for devices that claim to be modems.
    CFDictionarySetValue(
      classesToMatch,
      CFSTR(kIOSerialBSDTypeKey),
      svcName
    );

    // Each serial device object has a property with key
    // kIOSerialBSDTypeKey and a value that is one of kIOSerialBSDAllTypes,
    // kIOSerialBSDModemType, or kIOSerialBSDRS232Type. You can experiment with the
    // matching by changing the last parameter in the above call to CFDictionarySetValue.

    // As shipped, this sample is only interested in modems,
    // so add this property to the CFDictionary we're matching on.
    // This will find devices that advertise themselves as modems,
    // such as built-in and USB modems. However, this match won't find serial modems.

    // Get an iterator across all matching devices.
    kern_return_t result = IOServiceGetMatchingServices(
      kIOMasterPortDefault,
      classesToMatch,
      matchingServices
    );

    return KERN_SUCCESS == result;
  }

  return false;
}
//---------------------------------------------------------------------------

// Return path of a device pointed to by serialPortIterator
static EsString macosSerialDevPathGet(io_object_t svc)
{
  EsString path;

  if( svc )
  {
    // Get the callout device's path (/dev/cu.xxxxx). The callout device should almost always be
    // used: the dialin device (/dev/tty.xxxxx) would be used when monitoring a serial port for
    // incoming calls, e.g. a fax listener.
    CFTypeRef devKey = IORegistryEntryCreateCFProperty(
      svc,
      CFSTR(kIOCalloutDeviceKey),
      kCFAllocatorDefault,
      0
    );

    CFStringRef pathAsCFString = 0;
    if( CFGetTypeID(devKey) == CFStringGetTypeID() )
      pathAsCFString = static_cast<CFStringRef>(devKey);

    if( pathAsCFString )
      path = EsString::cfStringRefToString( pathAsCFString );

    CFRelease( devKey );
  }

  return path;
}
//---------------------------------------------------------------------------

void EsUartEnumerator::enumerate(bool busyPortsInclude /*= true*/)
{
  // Clear the output array
  m_infos.clear();
  m_infos.reserve(8);

  io_iterator_t matchingServices;
  if( macosSerialSvcsFind(&matchingServices, CFSTR(kIOSerialBSDModemType)) )
  {
    IOIteratorReset(matchingServices);

    io_object_t serialSvc;
    while( (serialSvc = IOIteratorNext(matchingServices)) )
    {
      const EsPath& path = EsPath::createFromFilePath(
        macosSerialDevPathGet(serialSvc)
      );

      // Info item
      EsUartInfo::Ptr info( new EsUartInfo );

      info->m_strName = path.fileNameExtGet();
      info->m_strFriendlyName = info->m_strName;
      info->m_strDevPath = path.pathGet();
      info->m_isModem = true;
      info->m_isUSBdevice = EsString::npos != info->m_strName.find(esT("usb"));
      if( info->m_isUSBdevice )
      {
        // Try to read USB PID && VID && serial information, and emulate windows instance string
        // to comply with generic serial string extractors used in scripted code
        // USB\VID_XXXX&PID_XXXX\SERIAL
        //
        const EsVariant& ids = EsUtilities::macosUsbInfoGetByParam(
          kIOServicePlane,
          CFSTR(kIOCalloutDeviceKey),
          info->m_strDevPath
        );

        int cnt = ids.countGet();
        if( 2 <= cnt )
        {
          info->m_strInstPath = EsString::format(
            esT("USB\\VID_%0.4hX&PID_%0.4hX"),
            ids.itemGet(0),
            ids.itemGet(1)
          );
        }

        if( 5 <= cnt )
          info->m_strInstPath += esT("\\") + ids.itemGet(4).asString();

        if( 4 <= cnt )
        {
          const EsString& prodstr = ids.itemGet(2).asString();
          const EsString& vendorstr = ids.itemGet(3).asString();
          if( !prodstr.empty() && !vendorstr.empty() )
            info->m_strFriendlyName = EsString::format(
              esT("%s %s (%s)"),
              prodstr,
              vendorstr,
              info->m_strName
            );
        }
      }

      m_infos.push_back(info);

      IOObjectRelease(serialSvc);
    }
  }
}
//---------------------------------------------------------------------------

EsUartEnumerator::EsUartAvail EsUartEnumerator::availabilityCheck(const EsUartInfo::Ptr& info)
{
  EsUartAvail result = uartAvailable;

  ES_ASSERT(info);

  // TODO: Is there check for port availability in xxxX systems?

  // Port is available
  return result;
}
//---------------------------------------------------------------------------
