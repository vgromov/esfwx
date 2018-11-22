// reflected methods
//
bool EsFile::open()
{
	if( 0 == m_file )
	{
		m_lastError = 0;
		m_eof = false;

		esU32 accessMode = 0;
		esU32 shareMode = (m_flags & static_cast<ulong>(EsFileFlag::Exclusive)) ? 0 : FILE_SHARE_READ|FILE_SHARE_WRITE;
		esU32 createMode = CREATE_ALWAYS;
		if( m_flags & static_cast<ulong>(EsFileFlag::Read) )
			accessMode |= GENERIC_READ;
		if( m_flags & static_cast<ulong>(EsFileFlag::Write) )
			accessMode |= GENERIC_WRITE;
		if( (m_flags & static_cast<ulong>(EsFileFlag::Append)) ||
        (m_flags & static_cast<ulong>(EsFileFlag::Read))
    )
			createMode = OPEN_EXISTING;

		HANDLE fh = ::CreateFile( m_name.c_str(),
			accessMode,
			shareMode,
			0,
			createMode,
			FILE_ATTRIBUTE_NORMAL,
			0 );
		if( INVALID_HANDLE_VALUE == fh )
			m_lastError = EsUtilities::osErrorCodeGet();
		else
			m_file = fh;
	}

	return 0 != m_file;
}

void EsFile::close()
{
	if( 0 != m_file )
	{
		if( !::CloseHandle(m_file) )
		{
			m_lastError = EsUtilities::osErrorCodeGet();
			ES_FAIL; // should not be here!!!
		}
		else
		{
			m_file = 0;
			m_lastError = 0;
		}
	}
}

ullong EsFile::get_length() const
{
	checkFileIsOpen();
	LARGE_INTEGER size;
	if( !::GetFileSizeEx(m_file, &size) )
	{
		m_lastError = EsUtilities::osErrorCodeGet();
		EsException::ThrowOsError(m_lastError);
	}

	return size.QuadPart;
}

void EsFile::set_length(const ullong& len)
{
	checkFileIsOpen();
  checkFileIsWriteable();

	LARGE_INTEGER dist;
  dist.QuadPart = len;
	if( !::SetFilePointerEx(m_file, dist, NULL, FILE_BEGIN) )
	{
		m_lastError = EsUtilities::osErrorCodeGet();
		EsException::ThrowOsError(m_lastError);
	}
  else if( !::SetEndOfFile(m_file) )
  {
		m_lastError = EsUtilities::osErrorCodeGet();
		EsException::ThrowOsError(m_lastError);
  }
}

llong EsFile::get_offset() const
{
	checkFileIsOpen();
	LARGE_INTEGER pos, offs;
	pos.QuadPart = 0;
	if( !::SetFilePointerEx(m_file, pos, &offs, FILE_CURRENT) )
	{
		m_lastError = EsUtilities::osErrorCodeGet();
		EsException::ThrowOsError(m_lastError);
	}

	return offs.QuadPart;
}

llong EsFile::seek(ullong offs)
{
	checkFileIsOpen();
	LARGE_INTEGER result, _offs;
	_offs.QuadPart = offs;
	if( !::SetFilePointerEx(m_file, _offs, &result, FILE_BEGIN) )
	{
		m_lastError = EsUtilities::osErrorCodeGet();
		EsException::ThrowOsError(m_lastError);
	}

	return result.QuadPart;
}

llong EsFile::seekRelative(llong offs)
{
	checkFileIsOpen();
	LARGE_INTEGER result, _offs;
	_offs.QuadPart = offs;
	if( !::SetFilePointerEx(m_file, _offs, &result, FILE_CURRENT) )
	{
		m_lastError = EsUtilities::osErrorCodeGet();
		EsException::ThrowOsError(m_lastError);
	}

	return result.QuadPart;
}

llong EsFile::seekEnd(ullong offs)
{
	checkFileIsOpen();
	LARGE_INTEGER result, _offs;
	_offs.QuadPart = offs;
	if( !::SetFilePointerEx(m_file, _offs, &result, FILE_END) )
	{
		m_lastError = EsUtilities::osErrorCodeGet();
		EsException::ThrowOsError(m_lastError);
	}

	return result.QuadPart;
}

ullong EsFile::read(void* dest, ullong toRead)
{
	checkFileIsOpen();
	m_eof = false;

	DWORD read = 0;
	if( 
    !::ReadFile(
       m_file, 
      dest, 
      static_cast<esU32>(toRead), 
      &read, 
      nullptr
    ) 
  )
		m_lastError = EsUtilities::osErrorCodeGet();
	else if( read < toRead )
		m_eof = true;

	return static_cast<ullong>(read);
}

ullong EsFile::write(const void* src, ullong toWrite)
{
	checkFileIsOpen();
	m_eof = false;

	DWORD written;
	if( 
    !::WriteFile(
      m_file, 
      src, 
      static_cast<esU32>(toWrite), 
      &written, 
      nullptr
    ) 
  )
		m_lastError = EsUtilities::osErrorCodeGet();

	return static_cast<ullong>(written);
}