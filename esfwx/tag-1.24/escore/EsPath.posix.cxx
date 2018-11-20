//---------------------------------------------------------------------------
#include <sys/stat.h>
#if (ES_OS == ES_OS_LINUX) || (ES_OS == ES_OS_UNIX)
# include <limits.h> //< PATH_MAX
#endif

//---------------------------------------------------------------------------
long EsPath::isExisting(const EsString& obj)
{
	long result = 0;

	struct stat st;
	if( -1 == stat(EsString::toUtf8(obj).c_str(), &st) )
	{
		ulong error = EsUtilities::osErrorCodeGet();
		// handle non expected errors, like not enough ACL to access file|folder, etc
		if(	ENOENT != error )
			EsException::ThrowOsError(error);
	}
	else
	{
		if( S_ISDIR(st.st_mode) )
			result = 2;
		else
#if ES_OS == ES_OS_MAC || ES_OS == ES_OS_IOS
    {
      int ret = access(EsString::toUtf8(obj).c_str(), R_OK);
      if( 0 == ret || (0 > ret && errno == EACCES) )
        result = 1;
    }
#else
    if( S_ISREG(st.st_mode) )
			result = 1;
#endif
  }

	return result;
}
//---------------------------------------------------------------------------

EsString EsPath::cwdGet()
{
	EsString result;

  EsByteString bstr(PATH_MAX, 0);
	if( 0 == getcwd(const_cast<EsByteString::pointer>(bstr.c_str()), PATH_MAX) )
		EsException::ThrowOsError( EsUtilities::osErrorCodeGet() );
	else
    result = EsString::fromUtf8(bstr);

	return result;
}
//---------------------------------------------------------------------------

void EsPath::cwdSet(const EsString& cwd)
{
	if( -1 == chdir(EsString::toUtf8(cwd).c_str()) )
		EsException::ThrowOsError( EsUtilities::osErrorCodeGet() );
}
//---------------------------------------------------------------------------

void EsPath::directoryCreate(const EsString& path)
{
	mode_t mask = umask(0);
	umask(mask);
	if( -1 == mkdir(EsString::toUtf8(path).c_str(), 0777-mask) )
		EsException::ThrowOsError(EsUtilities::osErrorCodeGet());
}
//---------------------------------------------------------------------------

void EsPath::fileRemove(const EsString& path)
{
	if( -1 == remove(EsString::toUtf8(path).c_str()) )
		EsException::ThrowOsError(EsUtilities::osErrorCodeGet());
}
//---------------------------------------------------------------------------

void EsPath::dirRemove(const EsString& path)
{
	if( -1 == rmdir(EsString::toUtf8(path).c_str()) )
		EsException::ThrowOsError(EsUtilities::osErrorCodeGet());
}
//---------------------------------------------------------------------------

EsString EsPath::stdSysRootGet()
{
	return esT("/");
}
//---------------------------------------------------------------------------

EsString EsPath::stdHomeGet()
{
  return EsUtilities::environmentVarsListGet().valueGet(esT("HOME")).asString();
}
//---------------------------------------------------------------------------

EsStringArray EsPath::stdDataRootsGet()
{
  EsStringArray roots;
  // Read accessible storage mountpoints
  if( EsPath::fileExists(esT("/proc/mounts"), EsString::null()) )
  {
    EsFile fmounts(
      esT("/proc/mounts"),
      static_cast<ulong>(EsFileFlag::Read)|
      static_cast<ulong>(EsFileFlag::Text)
    );
    fmounts.set_delimiters(esT("\n\r"));

    if( fmounts.open() )
    {
      EsString line = fmounts.stringRead();
      while( !line.empty() )
      {
        EsStringTokenizer ltok(line);
        ulong cnt = ltok.get_tokensCount();
        if( cnt > 1 )
        {
          const EsString& src = ltok.tokenGetAt(0);
          if( 0 == src.find(esT("/dev/fuse")) )
          {
            const EsString& path = ltok.tokenGetAt(1);
            roots.push_back(path);
          }
        }

        line = fmounts.stringRead();
      }
      fmounts.close();
    }
  }

  return roots;
}
