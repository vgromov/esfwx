//---------------------------------------------------------------------------

#if ES_OS == ES_OS_ANDROID
# include <linux/sysctl.h>
#else
# include <sys/sysctl.h>
#endif
//---------------------------------------------------------------------------

static EsCriticalSection& envCsGet()
{
  static EsCriticalSection s_cs;

  return s_cs;
}
//---------------------------------------------------------------------------

extern char **environ;
ulong EsUtilities::osErrorCodeGet()
{
	return errno;
}
//---------------------------------------------------------------------------

EsString EsUtilities::osErrorStringGet(ulong code)
{
	EsString result;

	EsCriticalSectionLocker lock(envCsGet());
	const char* errStr = strerror(code);
	if( errStr )
		result = EsString::fromUtf8(errStr);
	else
		// if this happens, something is seriously wrong, so don't use i18n _() here
		result = EsString::format(esT("Unknown OS error %lx"), code);

	return result;
}
//---------------------------------------------------------------------------

EsStringIndexedMap EsUtilities::environmentVarsListGet()
{
	EsStringIndexedMap result;

	EsCriticalSectionLocker lock(envCsGet());
	int idx = 0;
	while(environ[idx])
	{
		EsString var, val;

		// parse name, value pair
		char* beg = environ[idx++];
		char* pos = beg;

		while( *pos )
		{
			if( esT('=') == *pos++ )
			{
				var = EsString::fromUtf8( EsByteString(beg, pos-1) );
				val = EsString::fromUtf8(pos);
				val = EsString::trim(val);
				break;
			}
		}

		if( var.empty() )
			var = EsString::fromUtf8(beg);
		var = EsString::trim(var);
		result.itemAdd(var, val);
	}

	return result;
}
//---------------------------------------------------------------------------

#if ES_OS != ES_OS_ANDROID

int EsUtilities::sysctlIntGet(int subjId, int dataId)
{
  int mib[2];

  mib[0] = subjId;
  mib[1] = dataId;

  int result = 0;
  size_t len;

  len = sizeof(result);
  sysctl(mib, 2, &result, &len, NULL, 0);

  return result;
}
//---------------------------------------------------------------------------

EsString EsUtilities::sysctlStrGet(int subjId, int dataId)
{
  int mib[2];

  mib[0] = subjId;
  mib[1] = dataId;

  size_t len = 0;
  sysctl(mib, 2, NULL, &len, NULL, 0);

  EsByteString bs(len, '\0');
  sysctl(mib, 2, &bs[0], &len, NULL, 0);

  return EsString::fromUtf8(bs);
}
//---------------------------------------------------------------------------

#if ES_OS != ES_OS_MAC && ES_OS != ES_OS_IOS
void EsUtilities::urlOpen(const EsString& url)
{
  EsByteString cmd = "x-www-browser ");
  cmd += EsString::toUtf8(
    EsString::toString(
      url,
      EsString::StrNoCEscape
    )
  );

  system(
    cmd.c_str()
  );
}
#endif

//---------------------------------------------------------------------------
#endif

