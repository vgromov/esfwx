// This file must not be included in project directly
// It's implicitly included in EsDateTime.cpp
//
#include <windows.h>
#include <timezoneapi.h>

static esDT internalNow()
{
	esDT _now;
	SYSTEMTIME st;
	::GetLocalTime(&st);
	dtComposeDateTime(&_now, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	return _now;
}

EsTimeSpan EsDateTime::bias()
{
  EsTimeSpan ts;

  EsDynamicLibrary::Ptr kernel = EsDynamicLibrary::load( esT("kernel32.dll") );
  ES_ASSERT(kernel);

  long bias = 0;
  DWORD result = 0;
  if( 0 > EsOsInfo::compare(6, 0) )
  {
    typedef DWORD (WINAPI* tzGetT)(LPTIME_ZONE_INFORMATION);
    tzGetT pfn = reinterpret_cast<tzGetT>(
      kernel->procAddrGet(esT("GetTimeZoneInformation"))
    );

    TIME_ZONE_INFORMATION tzi;
    result = pfn(&tzi);
    bias = tzi.Bias;
  }
  else
  {
    typedef DWORD (WINAPI* tzGetT)(PDYNAMIC_TIME_ZONE_INFORMATION);
    tzGetT pfn = reinterpret_cast<tzGetT>(
      kernel->procAddrGet(esT("GetDynamicTimeZoneInformation"))
    );

    DYNAMIC_TIME_ZONE_INFORMATION tzi;
    result = pfn(&tzi);
    bias = tzi.Bias;
  }

  if(TIME_ZONE_ID_INVALID == result)
    EsException::ThrowOsError(
      EsUtilities::osErrorCodeGet(),
      esT("Could not get timezone information: '%s'")
    );

  bool negative = bias < 0;
  if( negative )
    bias = -bias;

  ts.set_minutes(bias);
  ts.set_negative(negative);

  return ts;
}

bool EsDateTime::isDstActive()
{
  DWORD result = 0;

  EsDynamicLibrary::Ptr kernel = EsDynamicLibrary::load( esT("kernel32.dll") );
  ES_ASSERT(kernel);

  if( 0 > EsOsInfo::compare(6, 0) )
  {
    typedef DWORD (WINAPI* tzGetT)(LPTIME_ZONE_INFORMATION);
    tzGetT pfn = reinterpret_cast<tzGetT>(
      kernel->procAddrGet(esT("GetTimeZoneInformation"))
    );

    TIME_ZONE_INFORMATION tzi;
    result = pfn(&tzi);
  }
  else
  {
    typedef DWORD (WINAPI* tzGetT)(PDYNAMIC_TIME_ZONE_INFORMATION);
    tzGetT pfn = reinterpret_cast<tzGetT>(
      kernel->procAddrGet(esT("GetDynamicTimeZoneInformation"))
    );

    DYNAMIC_TIME_ZONE_INFORMATION tzi;
    result = pfn(&tzi);
  }

  if(TIME_ZONE_ID_INVALID == result)
    EsException::ThrowOsError(
      EsUtilities::osErrorCodeGet(),
      esT("Could not get timezone information: '%s'")
    );

  return TIME_ZONE_ID_DAYLIGHT == result;
}
