#ifndef _es_comm_utils_h_
#define _es_comm_utils_h_

namespace EsCommUtils
{
EKOSF_COMM_FUNC bool receiveBinaryPattern(EsIoChannelIntf::Ptr chnl, const BYTE* pattern, size_t patternLen, EsIoBreakIntf::Ptr brk = EsIoBreakIntf::Ptr());
EKOSF_COMM_FUNC bool receiveBinaryPattern(EsIoChannelIntf::Ptr chnl, const EsBinBuffer& pattern, EsIoBreakIntf::Ptr brk = EsIoBreakIntf::Ptr());
}

#endif // _es_comm_utils_h_