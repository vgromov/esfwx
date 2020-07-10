//---------------------------------------------------------------------------

//EsVariant EsUtilities::macosSvcRegValSearch(io_service_t svc, const io_name_t plane,
//  CFStringRef key, IOOptionBits searchOptions /*= kIORegistryIterateRecursively*/ )
//{
//  return result;
//}
//---------------------------------------------------------------------------

//EsVariant EsUtilities::macosSvcRegValGet(io_service_t svc, CFStringRef key )
//{
//  return result;
//}
//---------------------------------------------------------------------------

//EsVariant EsUtilities::macosUsbInfoGetByParam(const io_name_t paramPlane, CFStringRef paramKey, const EsVariant& paramVal)
//{
//  ES_ASSERT( !paramVal.isEmpty() );

//  return result;
//}
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
