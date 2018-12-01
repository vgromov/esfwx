#include "escommpch.h"
#pragma hdrstop

#include "EsCommUtils.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

namespace EsReflection
{
ES_DECL_ENUMERATION_INFO(EsByteReadStatus, NO_CLASS_DESCR)
}
//---------------------------------------------------------------------------

ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsCommUtils, esT("Communication channel factory"))
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsCommUtils, specificByteReceiveReflected, specificByteReceive, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR )
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsCommUtils, binaryPatternReceiveReflected, binaryPatternReceive, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR )
ES_DECL_REFLECTED_SERVICES_INFO_END
//---------------------------------------------------------------------------

EsByteReadStatus EsCommUtils::specificByteReceive(const EsChannelIoIntf::Ptr& chnl, esU8 b, ulong retries, ulong tmo)
{
  EsBaseIntfPtr breaker = chnl->breakerGet();
  EsVariant breaking = false;
  while( !breaking.asBool() )
  {
    esU8 r;
    if( 1 == chnl->bytesGet(&r, 1, tmo) &&
        b == r )
    {
      return EsByteReadStatus::Success;
    }
    else if( 0 != retries )
    {
      --retries;
      if( 0 != chnl->errorGet() )
        chnl->reset();
    }
    else
      break;

    ES_DUAL_CALL0(breaking, breaker, EsBreakIntf, isBreaking);
  }

  if( breaking.asBool() )
    return EsByteReadStatus::Cancelled;

  return EsByteReadStatus::Expired;
}
//---------------------------------------------------------------------------

bool EsCommUtils::binaryPatternReceive(const EsChannelIoIntf::Ptr& chnl, const esU8* pattern, ulong patternLen,
  ulong charRetries /*= 1*/, ulong tmo /*= 1*/)
{
  ES_ASSERT(pattern);
  const esU8* pos = pattern;
  const esU8* end = pattern+patternLen;
  bool ok = EsByteReadStatus::Success == specificByteReceive(
    chnl, 
    *pos++, 
    patternLen, 
    tmo
  );
  
  while(pos < end && ok)
    ok = EsByteReadStatus::Success == specificByteReceive(
      chnl, 
      *pos++, 
      1, 
      tmo
    );

  return ok;
}
//---------------------------------------------------------------------------

bool EsCommUtils::binaryPatternReceive(const EsChannelIoIntf::Ptr& chnl, const EsBinBuffer& pattern,
  ulong charRetries /*= 1*/, ulong tmo /*= 1*/)
{
  return binaryPatternReceive(
    chnl, 
    &pattern[0], 
    static_cast<ulong>(pattern.size()), 
    charRetries, 
    tmo
  );
}
//---------------------------------------------------------------------------

EsVariant EsCommUtils::specificByteReceiveReflected(cr_EsVariant chnl, cr_EsVariant b, cr_EsVariant retries, cr_EsVariant tmo)
{
  return static_cast<ulong>(
    specificByteReceive(
      chnl.asExistingObject(),
      b.asByte(),
      retries.asULong(),
      tmo.asULong()
    )
  );
}
//---------------------------------------------------------------------------

EsVariant EsCommUtils::binaryPatternReceiveReflected(cr_EsVariant chnl, cr_EsVariant pattern, cr_EsVariant retries, cr_EsVariant tmo)
{
  return binaryPatternReceive(
    chnl.asExistingObject(),
    pattern.asBinBuffer(),
    retries.asULong(),
    tmo.asULong()
  );
}
//---------------------------------------------------------------------------
