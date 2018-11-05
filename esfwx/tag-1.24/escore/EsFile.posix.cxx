//---------------------------------------------------------------------------
#include <sys/stat.h>

#ifndef O_BINARY
# define ES_O_BINARY  0
#else
# define ES_O_BINARY  O_BINARY
#endif

#ifndef S_IRUSR
# define ES_S_IRUSR   S_IREAD
#else
# define ES_S_IRUSR   S_IRUSR
#endif

#ifndef S_IWUSR
# define ES_S_IWUSR   S_IWRITE
#else
# define ES_S_IWUSR   S_IWUSR
#endif
//---------------------------------------------------------------------------

bool EsFile::open()
{
	if( 0 == m_file )
	{
		m_lastError = 0;
		int access = ES_O_BINARY;

		if( static_cast<ulong>(EsFileFlag::Read) == (m_flags & (static_cast<ulong>(EsFileFlag::Read)|static_cast<ulong>(EsFileFlag::Write))) )
			access |= O_RDONLY;
    else if( static_cast<ulong>(EsFileFlag::Write) == (m_flags & (static_cast<ulong>(EsFileFlag::Read)|static_cast<ulong>(EsFileFlag::Write))) )
			access |= O_WRONLY|O_CREAT;
		else if( (static_cast<ulong>(EsFileFlag::Read)|static_cast<ulong>(EsFileFlag::Write)) == (m_flags & (static_cast<ulong>(EsFileFlag::Read)|static_cast<ulong>(EsFileFlag::Write))) )
			access |= O_RDWR|O_CREAT;

		if( static_cast<ulong>(EsFileFlag::Append) != (m_flags & static_cast<ulong>(EsFileFlag::Append)) &&
				static_cast<ulong>(EsFileFlag::Write) == (m_flags & static_cast<ulong>(EsFileFlag::Write)) )
			access |= O_TRUNC;

		unsigned mode = 0;
		if( access & O_CREAT )
    {
  		mode = (ES_S_IRUSR|ES_S_IWUSR);

      if( static_cast<ulong>(EsFileFlag::Exclusive) & m_flags )
        access |= O_EXCL;

      if( EsPath::fileExists(m_name, EsString::null()) )
        access &= ~O_CREAT;
    }

		int fh = ::open( EsString::toUtf8(m_name).c_str(), access, mode);
		if( -1 == fh )
		{
			m_lastError = EsUtilities::osErrorCodeGet();
      m_file = 0;
		}
		else
			m_file = fh;
	}

	return 0 != m_file;
}

void EsFile::close()
{
	if( 0 != m_file )
	{
		if( -1 == ::close(m_file) )
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
  ullong result = 0;

	off_t offs = lseek(m_file, 0, SEEK_CUR);
  if( -1 == offs )
	{
		m_lastError = EsUtilities::osErrorCodeGet();
		EsException::ThrowOsError(m_lastError);
	}
  else
  {
    result = lseek(m_file, 0, SEEK_END);

    if( -1 == result )
    {
      m_lastError = EsUtilities::osErrorCodeGet();
      EsException::ThrowOsError(m_lastError);
    }
    else
    {
      if( -1 == lseek(m_file, offs, SEEK_SET) )
      {
        m_lastError = EsUtilities::osErrorCodeGet();
        EsException::ThrowOsError(m_lastError);
      }
    }
  }

  return result;
}

void EsFile::set_length(const ullong& len)
{
	checkFileIsOpen();
  checkFileIsWriteable();

  if( -1 == ftruncate(m_file, len) )
  {
    m_lastError = EsUtilities::osErrorCodeGet();
    EsException::ThrowOsError(m_lastError);
  }
}

llong EsFile::get_offset() const
{
	checkFileIsOpen();
	off_t offs = lseek(m_file, 0, SEEK_CUR);
	if( -1 == offs )
	{
		m_lastError = EsUtilities::osErrorCodeGet();
		EsException::ThrowOsError(m_lastError);
	}

	return offs;
}

llong EsFile::seek(ullong offs)
{
	checkFileIsOpen();
	long pos = lseek(m_file, offs, SEEK_SET);
	if( -1 == pos )
	{
		m_lastError = EsUtilities::osErrorCodeGet();
		EsException::ThrowOsError(m_lastError);
	}

	return pos;
}

llong EsFile::seekRelative(llong offs)
{
	checkFileIsOpen();
	long pos = lseek(m_file, offs, SEEK_CUR);
	if( -1 == pos )
	{
		m_lastError = EsUtilities::osErrorCodeGet();
		EsException::ThrowOsError(m_lastError);
	}

	return pos;
}

llong EsFile::seekEnd(ullong offs)
{
	checkFileIsOpen();
	long pos = lseek(m_file, offs, SEEK_END);
	if( -1 == pos )
	{
		m_lastError = EsUtilities::osErrorCodeGet();
		EsException::ThrowOsError(m_lastError);
	}

	return pos;
}

ullong EsFile::read(void* dest, ullong toRead)
{
	checkFileIsOpen();
	m_eof = false;

	ssize_t read = ::read(m_file, dest, toRead);
	if( -1 == read )
	{
		m_lastError = EsUtilities::osErrorCodeGet();
		read = 0;
	}
	else if( (ullong)read < toRead )
		m_eof = true;

	return static_cast<ullong>(read);
}

ullong EsFile::write(const void* src, ullong toWrite)
{
	checkFileIsOpen();
	m_eof = false;
	ssize_t written = ::write(m_file, src, toWrite);
	if( -1 == written )
	{
		m_lastError = EsUtilities::osErrorCodeGet();
		written = 0;
	}

	return static_cast<ullong>(written);
}