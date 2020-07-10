#include <CoreServices/CoreServices.h>
//---------------------------------------------------------------------------

EsString EsPath::stdAppPathGet()
{
  CFBundleRef mainBundle = CFBundleGetMainBundle();
  if(!mainBundle)
      return EsString::null();

  CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
  if(!mainBundleURL)
      return EsString::null();

  CFStringRef cfStringRef = CFURLCopyFileSystemPath(mainBundleURL, kCFURLPOSIXPathStyle);
  if(!cfStringRef)
      return EsString::null();

  EsString result = EsString::cfStringRefToString(cfStringRef);

  CFRelease(mainBundleURL);
  CFRelease(cfStringRef);

  return result;
}
//---------------------------------------------------------------------------

EsString EsPath::stdDocsGet()
{
  EsPath path = createFromPath( stdHomeGet() );
  path.dirAppend( esT("Documents") );

  return path.pathGet();
}
//---------------------------------------------------------------------------

EsString EsPath::stdAppDataGet()
{
  EsPath path = createFromPath( stdHomeGet() );
  path.dirAppend( esT("Library") );
  path.dirAppend( esT("Application Support") );

  return path.pathGet();
}
//---------------------------------------------------------------------------

EsString EsPath::stdLocalAppDataGet()
{
  return esT("/Library/Application Support");
}
//---------------------------------------------------------------------------

EsString EsPath::stdPublicDocsGet()
{
  EsPath path = createFromPath( stdHomeGet() );
  path.dirAppend( esT("Public") );

  return path.pathGet();
}
//---------------------------------------------------------------------------

EsString EsPath::stdDownloadsGet()
{
  EsPath path = createFromPath( stdHomeGet() );
  path.dirAppend( esT("Downloads") );

  return path.pathGet();
}
//---------------------------------------------------------------------------

EsString EsPath::stdPublicDownloadsGet()
{
  return stdPublicDocsGet();
}
//---------------------------------------------------------------------------
