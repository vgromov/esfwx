/*#include <Macapi.Helpers.hpp>
#if ES_OS == ES_OS_IOS
# include <FMX.Helpers.iOS.hpp>
#else
# include <Macapi.Foundation.hpp>
#endif
*/
#include <IOKit/usb/USBSpec.h>
#include <IOKit/hid/IOHIDKeys.h>
#define kIOUSBDeviceClassName  "IOUSBDevice"
//---------------------------------------------------------------------------

EsVariant EsUtilities::macosSvcRegValSearch(io_service_t svc, const io_name_t plane,
  CFStringRef key, IOOptionBits searchOptions /*= kIORegistryIterateRecursively*/ )
{
  CFTypeRef val = IORegistryEntrySearchCFProperty(
    svc,
    plane,
    key,
    kCFAllocatorDefault,
    searchOptions
  );

  if( !val )
    return EsVariant::null();

  EsVariant result;

  if( CFGetTypeID(val) == CFNumberGetTypeID() )
  {
    long out = 0;
    if( CFNumberGetValue( static_cast<CFNumberRef>(val), kCFNumberSInt32Type, &out) )
      result = out;
  }
  else if( CFGetTypeID(val) == CFStringGetTypeID() )
    result = EsString::cfStringRefToString( static_cast<CFStringRef>(val) );

  CFRelease( val );

  return result;
}
//---------------------------------------------------------------------------


EsVariant EsUtilities::macosSvcRegValGet(io_service_t svc, CFStringRef key )
{
  CFTypeRef val = IORegistryEntryCreateCFProperty(
    svc,
    key,
    kCFAllocatorDefault,
    kNilOptions
  );

  if( !val )
    return EsVariant::null();

  EsVariant result;

  if( CFGetTypeID(val) == CFNumberGetTypeID() )
  {
    long out = 0;
    if( CFNumberGetValue( static_cast<CFNumberRef>(val), kCFNumberSInt32Type, &out) )
      result = out;
  }
  else if( CFGetTypeID(val) == CFStringGetTypeID() )
    result = EsString::cfStringRefToString( static_cast<CFStringRef>(val) );

  CFRelease( val );

  return result;
}
//---------------------------------------------------------------------------

EsVariant EsUtilities::macosUsbInfoGetByParam(const io_name_t paramPlane, CFStringRef paramKey, const EsVariant& paramVal)
{
  ES_ASSERT( !paramVal.isEmpty() );

  EsVariant result(EsVariant::VAR_VARIANT_COLLECTION);
  CFMutableDictionaryRef matchingDict = IOServiceMatching(kIOUSBDeviceClassName);

  if( matchingDict )
  {
    io_iterator_t iter;
    kern_return_t kr = IOServiceGetMatchingServices(
      kIOMasterPortDefault,
      matchingDict,
      &iter
    );

    if(KERN_SUCCESS == kr)
    {
      io_service_t device;

      while( (device = IOIteratorNext(iter)) )
      {
        const EsVariant& devParam = macosSvcRegValSearch(device, paramPlane, paramKey);

        if( !devParam.isEmpty() &&
          devParam == paramVal
        )
        {
          EsVariant val = macosSvcRegValGet(device, CFSTR(kUSBVendorID) );
          if( !val.isEmpty() )
          {
            result.addToVariantCollection( val );
            val = macosSvcRegValGet(device, CFSTR(kUSBProductID) );

            if( !val.isEmpty() )
            {
              result.addToVariantCollection( val );

              val = macosSvcRegValGet(device, CFSTR(kUSBProductString) );
              if( !val.isEmpty() )
              {
                result.addToVariantCollection( val );

                val = macosSvcRegValGet(device, CFSTR(kUSBVendorString) );
                if( !val.isEmpty() )
                {
                  result.addToVariantCollection( val );

                  val = macosSvcRegValGet(device, CFSTR(kUSBSerialNumberString) );
                  if( !val.isEmpty() )
                    result.addToVariantCollection( val );
                  else
                  {
                    val = macosSvcRegValGet(device, CFSTR(kIOHIDSerialNumberKey) );
                    if( !val.isEmpty() )
                      result.addToVariantCollection( val );
                  }
                }
              }
            }
          }

          IOObjectRelease(device);
          break;
        }

        IOObjectRelease(device);
      }
    }

    IOObjectRelease(iter);
  }

  return result;
}
//---------------------------------------------------------------------------

void EsUtilities::urlOpen(const EsString& url)
{
#if ES_OS == ES_OS_IOS
  _di_NSURL nsurl = StrToNSUrl(
    EsString::toString(
      url,
      EsString::StrNoCEscape
    ).c_str()
  );
  ES_ASSERT(nsurl);

  if( SharedApplication()->canOpenURL(nsurl) )
    SharedApplication()->openUrl(nsurl);
  else
    EsException::Throw(
      esT("Could not open URL: '%s'"),
      url.c_str()
    );

#else

  EsByteString cmd = "open ";
  cmd += EsString::toUtf8(
    EsString::toString(
      url,
      EsString::StrNoCEscape
    )
  );

  system(
    cmd.c_str()
  );

#endif
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ulong EsUtilities::SystickPeriodChanger::systickSet(ulong ES_UNUSED(period))
{
  return 0;
}
//---------------------------------------------------------------------------

void EsUtilities::SystickPeriodChanger::systickRestore(ulong ES_UNUSED(period))
{
}
//---------------------------------------------------------------------------


