//---------------------------------------------------------------------------
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
#include <System.IOUtils.hpp>
#endif

//---------------------------------------------------------------------------
EsString EsPath::stdAppPathGet()
{
  return stdLocalAppDataGet();
}
//---------------------------------------------------------------------------

EsString EsPath::stdAppDataGet()
{
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
	return TPath::GetPublicPath().c_str();
#else
# error EsPath::stdAppDataGet is not implemented for this Compiler RTL
#endif
}
//---------------------------------------------------------------------------

EsString EsPath::stdLocalAppDataGet()
{
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
	return TPath::GetLibraryPath().c_str();
#else
# error EsPath::stdLocalAppDataGet is not implemented for this Compiler RTL
#endif
}
//---------------------------------------------------------------------------

EsString EsPath::stdDocsGet()
{
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  return TPath::GetDocumentsPath().c_str();
#else
# error EsPath::stdDocsGet is not implemented for this Compiler RTL
#endif
}
//---------------------------------------------------------------------------

EsString EsPath::stdPublicDocsGet()
{
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  return TPath::GetSharedDocumentsPath().c_str();
#else
# error EsPath::stdPublicDocsGet is not implemented for this Compiler RTL
#endif
}
//---------------------------------------------------------------------------

EsString EsPath::stdDownloadsGet()
{
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  return TPath::GetDownloadsPath().c_str();
#else
# error EsPath::stdDocsGet is not implemented for this Compiler RTL
#endif
}
//---------------------------------------------------------------------------

EsString EsPath::stdPublicDownloadsGet()
{
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  return TPath::GetSharedDownloadsPath().c_str();
#else
# error EsPath::stdPublicDocsGet is not implemented for this Compiler RTL
#endif
}
//---------------------------------------------------------------------------
