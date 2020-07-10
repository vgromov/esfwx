// Internal EsPath implementations for Linux-specific stuff
//
#include <limits.h> //< PATH_MAX

//---------------------------------------------------------------------------
EsString EsPath::stdAppPathGet()
{
  return stdLocalAppDataGet();
}
//---------------------------------------------------------------------------

EsString EsPath::stdAppDataGet()
{
  return stdLocalAppDataGet();
}
//---------------------------------------------------------------------------

EsString EsPath::stdLocalAppDataGet()
{
  EsByteString tmp(32, '\0');

  ssize_t cnt = snprintf(
    &tmp[0],
    tmp.size(),
    "/proc/%d/exe",
    getpid()
  );
  if( cnt <= 0 )
    return EsString::null();

  EsByteString out(PATH_MAX, '\0');
  ssize_t llen = readlink(
    tmp.c_str(),
    &out[0],
    cnt
  );
  if( llen <= 0 )
    return EsString::null();
  else
    out.resize(llen);

  return EsString::fromUtf8(out);
}
//---------------------------------------------------------------------------

EsString EsPath::stdDocsGet()
{
  EsPath path = EsPath::createFromPath(
    stdHomeGet()
  );

  path.dirAppend(esT("Documents"));

  return path.pathGet();
}
//---------------------------------------------------------------------------

EsString EsPath::stdPublicDocsGet()
{
  return stdDocsGet();
}
//---------------------------------------------------------------------------

EsString EsPath::stdDownloadsGet()
{
  EsPath path = EsPath::createFromPath(
    stdHomeGet()
  );

  path.dirAppend(esT("Downloads"));

  return path.pathGet();
}
//---------------------------------------------------------------------------

EsString EsPath::stdPublicDownloadsGet()
{
  return stdDownloadsGet();
}
//---------------------------------------------------------------------------
