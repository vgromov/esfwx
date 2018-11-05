//---------------------------------------------------------------------------

ulong EsUtilities::osErrorCodeGet()
{
	return ::GetLastError();
}
//---------------------------------------------------------------------------

EsString EsUtilities::osErrorStringGet(ulong code)
{
	EsString result;
  // get error message from system
  LPVOID lpMsgBuf;
  if( ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
          NULL,
          code,
          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
          (LPTSTR)&lpMsgBuf,
          0,
          NULL) == 0 )
		// if this happens, something is seriously wrong, so don't use i18n _() here
		result = EsString::format(esT("Unknown OS error %lx"), code);
	else
	{
		result.assign((EsString::const_pointer)lpMsgBuf);
		::LocalFree(lpMsgBuf);
	}

	return result;
}
//---------------------------------------------------------------------------

struct EsEnvBlockGuard
{
	EsEnvBlockGuard() : m_block( ::GetEnvironmentStrings() ) {}
	~EsEnvBlockGuard() { if(m_block) ::FreeEnvironmentStrings(m_block); }
	operator bool () const { return 0 != m_block; }
	operator LPTCH () const { return m_block; }

	LPTCH m_block;

private:
	EsEnvBlockGuard(const EsEnvBlockGuard&);
	EsEnvBlockGuard& operator=(const EsEnvBlockGuard&);
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsStringIndexedMap EsUtilities::environmentVarsListGet()
{
	EsStringIndexedMap result;

	EsEnvBlockGuard block;
	if( block )
	{
		LPTCH pos = block;
		LPTCH beg = block;

		while( 1 )
		{
			if( 0 == *pos++ )
			{
				EsString var;
				// parse varStr into name, value pair
				LPTCH ppos = beg;
				while( ppos < pos )
				{
					if( esT('=') == *ppos++ )
					{
						var.assign(beg, ppos-1);
						break;
					}
				}

				EsString val(ppos++, pos);

				// trim leading|trailing spaces, just in case
				var = EsString::trim(var);
				if( !var.empty() )
				{
					val = EsString::trim(val);
					result.itemAdd(var, val);
				}
							
				if( 0 == *pos )
					break;

				beg = pos;
			}
		}
	}
	else
		EsException::ThrowOsError( osErrorCodeGet() );

	return result;
}
//---------------------------------------------------------------------------

void EsUtilities::urlOpen(const EsString& url)
{
  ShellExecute(
    nullptr,
    esT("open"),
    EsString::toString(
      url,
      EsString::StrNoCEscape
    ).c_str(),
    nullptr,
    nullptr,
    SW_SHOWNORMAL
  );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# include <timeapi.h>
#else
# include <mmsystem.h>
#endif

ulong EsUtilities::SystickPeriodChanger::systickSet(ulong period)
{
  TIMECAPS tc;
  if (TIMERR_NOERROR != timeGetDevCaps(
    &tc,
    sizeof(TIMECAPS)
  )
    )
    return 0;

  period = std::min(
    std::max(
      tc.wPeriodMin,
      static_cast<UINT>(period)
    ),
    tc.wPeriodMax
  );

  if (TIMERR_NOERROR != timeBeginPeriod(period))
    period = 0; //< Signal we've not set new period successfully

  return period;
}
//---------------------------------------------------------------------------

void EsUtilities::SystickPeriodChanger::systickRestore(ulong period)
{
  if (period)
    timeEndPeriod(period);
}
//---------------------------------------------------------------------------
