#include <stdafx.h>
#pragma hdrstop

#include "DataFileUtils.h"

// standard file page tags: HDR_MAIN, HDR, DATA, 8 0s == EMPTY_PAGE
EsString DataFile::getPageTagHdrMain() 
{ 
	return EsString(reinterpret_cast<const char*>(c_pageTagHdrMain), 
		reinterpret_cast<const char*>(c_pageTagHdrMain)+FileTag_SZE); 
}

EsString DataFile::getPageTagHdr() 
{ 
	return EsString(reinterpret_cast<const char*>(c_pageTagHdr), 
		reinterpret_cast<const char*>(c_pageTagHdr)+FileTag_SZE); 
}

EsString DataFile::getPageTagData() 
{ 
	return EsString(reinterpret_cast<const char*>(c_pageTagData), 
		reinterpret_cast<const char*>(c_pageTagData)+FileTag_SZE); 
}

// compound data type tag
EsString DataFile::getCompoundTypeTag() 
{ 
	return EsString(reinterpret_cast<const char*>(c_compoundTag), 
		reinterpret_cast<const char*>(c_compoundTag)+FileTag_SZE); 
}

// custom data header tags
EsString DataFile::getPageLuxHdrTag() 
{ 
	return EsString(reinterpret_cast<const char*>(c_luxHdrTag), 
		reinterpret_cast<const char*>(c_luxHdrTag)+FileTag_SZE); 
}

EsString DataFile::getPageCdmHdrTag() 
{ 
	return EsString(reinterpret_cast<const char*>(c_cdmHdrTag), 
		reinterpret_cast<const char*>(c_cdmHdrTag)+FileTag_SZE); 
}

EsString DataFile::getPageKeoHdrTag() 
{ 
	return EsString(reinterpret_cast<const char*>(c_keoHdrTag), 
		reinterpret_cast<const char*>(c_keoHdrTag)+FileTag_SZE); 
}
