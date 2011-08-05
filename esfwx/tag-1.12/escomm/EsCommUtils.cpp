#include "stdafx.h"
#pragma hdrstop

#include "EsCommUtils.h"

// 0 = infinite retries
static bool receiveSpecificChar(EsIoChannelIntf::Ptr chnl, char c, size_t retries, EsIoBreakIntf::Ptr brk)
{
	BYTE r;
	bool ok = false;
	
	do
	{
		ok = 1 == chnl->getBytes(&r, 1, 1, brk) &&
			c == r;
		
		if( !ok &&
				0 != retries )
		{
			--retries;
			ok = ok && (0 != retries);
		}
		
	}	while( !ok &&
					(!brk || (brk && !brk->isBreaking())) );
	
	return ok;
}

bool EsCommUtils::receiveBinaryPattern(EsIoChannelIntf::Ptr chnl, const BYTE* pattern, size_t patternLen, EsIoBreakIntf::Ptr brk /*= EsIoBreakIntf::Ptr()*/)
{
	wxASSERT(pattern);
	const BYTE* pos = pattern;
	const BYTE* end = pattern+patternLen;
	bool ok = receiveSpecificChar(chnl, *pos++, 0, brk);
	while(pos < end && ok)
		ok = receiveSpecificChar(chnl, *pos++, 1, brk);
		
	return ok;
}

bool EsCommUtils::receiveBinaryPattern(EsIoChannelIntf::Ptr chnl, const EsBinBuffer& pattern, EsIoBreakIntf::Ptr brk /*= EsIoBreakIntf::Ptr()*/)
{
	return receiveBinaryPattern(chnl, &pattern[0], pattern.size(), brk);
}
