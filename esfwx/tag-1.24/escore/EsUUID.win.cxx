/// Internal Windows implementation of GUID generator. Do not include this file in your project directly
/// It's being implicitly included from EsUUID.cpp
///
#include <rpcdce.h>
#pragma comment(lib, "rpcrt4")

GUID EsUUID::generate()
{
  GUID uuid;
  
  RPC_STATUS result = UuidCreateSequential(
    &uuid
  );

  if( result != RPC_S_OK && result != RPC_S_UUID_LOCAL_ONLY )
    EsException::ThrowOsError(
      result,
      esT("Could not generate UUID, reason: '%s'")
    );
  
  return uuid;
}
