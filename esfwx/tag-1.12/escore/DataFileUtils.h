#ifndef _data_file_utils_h_
#define _data_file_utils_h_

// exported wrapper for data file C functions
//
namespace DataFile
{
	// standard file page tags: HDR_MAIN, HDR, DATA, 8 0s == EMPTY_PAGE
	EsString EKOSF_CORE_FUNC getPageTagHdrMain();
	EsString EKOSF_CORE_FUNC getPageTagHdr();
	EsString EKOSF_CORE_FUNC getPageTagData();
	// compound tag is used in main header when file contains geterogenous data types
	EsString EKOSF_CORE_FUNC getCompoundTypeTag(); 
	// custom data header tags	
	EsString EKOSF_CORE_FUNC getPageLuxHdrTag();
	EsString EKOSF_CORE_FUNC getPageCdmHdrTag();
	EsString EKOSF_CORE_FUNC getPageKeoHdrTag();
	
}; // namespace DataFile

#endif // _data_file_utils_h_