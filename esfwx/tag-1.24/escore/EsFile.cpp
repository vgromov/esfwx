#include "escorepch.h"
#pragma hdrstop

#include "EsFile.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

namespace EsReflection
{
ES_DECL_ENUMERATION_INFO(EsFileFlag, NO_CLASS_DESCR)
}
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsFile, NO_CLASS_DESCR)
  // properties
  ES_DECL_PROP_INFO(    EsFile, name, EsString, _i("File Name"), esVT(""), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsFile, opened, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsFile, eof, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(    EsFile, length, ullong, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsFile, offset, llong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsFile, recentErrorString, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(    EsFile, flags, ulong, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_PERSISTENT(EsFile, delimiters, EsString, _i("Input text delimiters"), esVT(" \t\r\n"), NO_PROPERTY_DESCR)
  // reflected methods
  ES_DECL_REFLECTED_CTOR_INFO(EsFile, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(EsFile, EsBaseIntfPtr_ClassCall_cr_EsString_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, open, bool_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, close, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, seek, llong_Call_ullong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, seekRelative, llong_Call_llong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, seekEnd, llong_Call_ullong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, remove, void_Call, NO_METHOD_DESCR)
  // reading
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, int8Read, long_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, uint8Read, ulong_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, int16Read, long_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, uint16Read, ulong_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, int32Read, long_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, uint32Read, ulong_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, int64Read, llong_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, uint64Read, ullong_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, stringRead, EsString_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, binBufferRead, EsBinBuffer_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, readAllAsString, EsString_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, readAllAsBinBuffer, EsBinBuffer_Call, NO_METHOD_DESCR)
  // writing
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, int8Write, ullong_Call_long, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, uint8Write, ullong_Call_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, int16Write, ullong_Call_long, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, uint16Write, ullong_Call_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, int32Write, ullong_Call_long, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, uint32Write, ullong_Call_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, int64Write, ullong_Call_llong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, uint64Write, ullong_Call_ullong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, stringWrite, ullong_Call_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, binBufferWrite, ullong_Call_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, writeAllAsBinBuffer, ullong_Call_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsFile, writeAllAsString, ullong_Call_cr_EsString, NO_METHOD_DESCR)
  // static services
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsFile, nameExtGet, EsString_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsFile, nameGet, EsString_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsFile, extGet, EsString_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsFile, remove, void_ClassCall_cr_EsString, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsFile::EsFile() :
m_flags(0),
m_lastError(0),
m_file(0)
{
  // initialize properties to their default values
  ES_REFLECTED_PROPERTIES_RESET;
}
//---------------------------------------------------------------------------

EsFile::EsFile(const EsString& fileName, ulong flags) :
m_flags(flags),
m_lastError(0),
m_file(0)
{
  // initialize properties to their default values
  ES_REFLECTED_PROPERTIES_RESET;
  // Fix write flag, if missing
  if( m_flags & static_cast<ulong>(EsFileFlag::Append) )
    m_flags |= static_cast<ulong>(EsFileFlag::Write);
  // Keep file name
  m_name = fileName;
}
//---------------------------------------------------------------------------

EsFile::~EsFile()
{
  close();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsFile::NEW()
{
  std::unique_ptr<EsFile> f( new EsFile );
  f->m_dynamic = true;
  return f.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsFile::NEW(cr_EsString fileName, ulong flags)
{
  std::unique_ptr<EsFile> f( new EsFile(fileName, flags) );
  f->m_dynamic = true;
  return f.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

void EsFile::checkLengthOnRead(size_t len) const
{
  if(len > get_length()-get_offset())
    EsException::Throw(esT("Invalid data length %d would result in reading beyond EOF"));
}
//---------------------------------------------------------------------------

void EsFile::checkFileIsOpen() const
{
  if( 0 == m_file )
    EsException::Throw(
      esT("File '%s' is not open"),
      m_name
    );
}
//---------------------------------------------------------------------------

void EsFile::checkFileIsWriteable() const
{
  if( !(m_flags & static_cast<ulong>(EsFileFlag::Write)) )
    EsException::Throw(esT("Requested file operation requires write access"));
}
//---------------------------------------------------------------------------

EsString EsFile::get_name() const
{
  return m_name;
}
//---------------------------------------------------------------------------

void EsFile::set_name(const EsString& name)
{
  checkFileIsClosed();
  m_name = name;
}
//---------------------------------------------------------------------------

bool EsFile::get_opened() const
{
  return 0 != m_file;
}
//---------------------------------------------------------------------------

bool EsFile::get_eof() const
{
  checkFileIsOpen();
  return m_eof;
}
//---------------------------------------------------------------------------

EsString EsFile::get_recentErrorString() const
{
  return EsString::format(
    esT("'%s': %s"),
    m_name,
    EsUtilities::osErrorStringGet(m_lastError)
  );
}
//---------------------------------------------------------------------------

ulong EsFile::get_flags() const
{
  return m_flags;
}
//---------------------------------------------------------------------------

void EsFile::checkFileIsClosed() const
{
  if( 0 != m_file )
    EsException::Throw(
      esT("File '%s' must be closed"),
      m_name
    );
}
//---------------------------------------------------------------------------

void EsFile::set_flags(cr_ulong flags)
{
  checkFileIsClosed();
  m_flags = flags;
}
//---------------------------------------------------------------------------

EsString EsFile::get_delimiters() const
{
  return m_delimiters;
}
//---------------------------------------------------------------------------

void EsFile::set_delimiters(const EsString& val)
{
  checkFileIsClosed();
  m_delimiters = val;
}
//---------------------------------------------------------------------------

void EsFile::remove(const EsString& path)
{
  EsPath::fileRemove(path);
}
//---------------------------------------------------------------------------

void EsFile::remove()
{
  checkFileIsClosed();
  remove(m_name);
}
//---------------------------------------------------------------------------

ullong EsFile::rawRead(void* dest, ullong toRead)
{
  ullong result = read(dest, toRead);
  if(m_lastError)
    EsException::ThrowOsError(m_lastError);

  return result;
}
//---------------------------------------------------------------------------

ullong EsFile::rawWrite(const void* src, ullong toWrite)
{
  ullong result = write(src, toWrite);
  if(m_lastError)
    EsException::ThrowOsError(m_lastError);

  return result;
}
//---------------------------------------------------------------------------

EsString EsFile::readTextWord()
{
  ES_ASSERT(m_flags & static_cast<ulong>(EsFileFlag::Text));
  EsBinBuffer buff;
  EsBinBuffer::value_type b;
  buff.reserve(512);
  // skip delimiters
  while( 1 == read(&b, 1) && EsString::npos != m_delimiters.find(b) );
  if( !m_eof )
  {
    buff.push_back(b);
    // read until next delimiter encountered
    while( 1 == read(&b, 1) && EsString::npos == m_delimiters.find(b) )
      buff.push_back(b);

    EsByteString bs(buff.begin(), buff.end());
    return EsString::fromUtf8(bs);
  }

  return EsString::null();
}
//---------------------------------------------------------------------------

long EsFile::int8Read()
{
  long val = 0;
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    EsString str = readTextWord();
    if( !str.empty() )
      return EsString::toLong(str);
  }
  else if(sizeof(int8_t) == read(&val, sizeof(int8_t)) )
    return val;

  return 0;
}
//---------------------------------------------------------------------------

ulong EsFile::uint8Read()
{
  ulong val = 0;
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    EsString str = readTextWord();
    if( !str.empty() )
      return EsString::toULong(str);
  }
  else if(sizeof(uint8_t) == read(&val, sizeof(uint8_t)) )
    return val;

  return 0;
}

long EsFile::int16Read()
{
  long val = 0;
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    EsString str = readTextWord();
    if( !str.empty() )
      return EsString::toLong(str);
  }
  else if(sizeof(int16_t) == read(&val, sizeof(int16_t)) )
    return val;

  return 0;
}

ulong EsFile::uint16Read()
{
  ulong val = 0;
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    EsString str = readTextWord();
    if( !str.empty() )
      return EsString::toLong(str);
  }
  else if(sizeof(uint16_t) == read(&val, sizeof(uint16_t)) )
    return val;

  return 0;
}

long EsFile::int32Read()
{
  long val = 0;
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    EsString str = readTextWord();
    if( !str.empty() )
      return EsString::toLong(str);
  }
  else if(sizeof(int32_t) == read(&val, sizeof(int32_t)) )
    return val;

  return 0;
}

ulong EsFile::uint32Read()
{
  ulong val = 0;
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    EsString str = readTextWord();
    if( !str.empty() )
      return EsString::toULong(str);
  }
  else if(sizeof(uint32_t) == read(&val, sizeof(uint32_t)) )
    return val;

  return 0;
}

llong EsFile::int64Read()
{
  llong val = 0;
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    EsString str = readTextWord();
    if( !str.empty() )
      return EsString::toInt64(str);
  }
  else if( sizeof(int64_t) == read(&val, sizeof(int64_t)) )
    return val;

  return 0;
}

ullong EsFile::uint64Read()
{
  ullong val = 0;
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    EsString str = readTextWord();
    if( !str.empty() )
      return EsString::toUInt64(str);
  }
  else if( sizeof(uint64_t) == read(&val, sizeof(uint64_t)) )
    return val;

  return 0;
}

EsString EsFile::stringRead()
{
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
    return readTextWord();
  else
  {
    ulong len = 0;
    if( sizeof(uint32_t) == read(&len, sizeof(uint32_t)) )
    {
      if( len )
      {
        checkLengthOnRead(len);
        EsBinBuffer buff;
        buff.resize(len);

        if( len == read(buff.data(), len) )
        {
          EsByteString bs(buff.begin(), buff.end());
          return EsString::fromUtf8(bs);
        }
      }
    }
  }

  return EsString::null();
}

EsBinBuffer EsFile::binBufferRead()
{
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    const EsString& val = readTextWord();
    if( !val.empty() )
      return EsString::hexToBin(val);
  }
  else
  {
    ulong len = 0;
    if( sizeof(uint32_t) == read(&len, sizeof(uint32_t)) )
    {
      checkLengthOnRead(len);
      EsBinBuffer result;
      result.resize(len);

      if( len == read( result.data(), len ) )
        return result;
    }
  }

  return EsBinBuffer::null();
}

// writing
ullong EsFile::int8Write(long val)
{
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    const EsString& str = EsString::fromLong(val);
    if( writeString(str) )
      return str.size();
  }
  else
    return write(
      &val, 
      sizeof(int8_t)
    );

  return 0;
}

ullong EsFile::uint8Write(ulong val)
{
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    const EsString& str = EsString::fromULong(val);
    if( writeString(str) )
      return str.size();
  }
  else
    return write(
      &val, 
      sizeof(uint8_t)
    );

  return 0;
}

ullong EsFile::int16Write(long val)
{
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    const EsString& str = EsString::fromLong(val);
    if( writeString(str) )
      return str.size();
  }
  else
    return write(
      &val, 
      sizeof(int16_t)
    );

  return 0;
}

ullong EsFile::uint16Write(ulong val)
{
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    const EsString& str = EsString::fromULong(val);
    if( writeString(str) )
      return str.size();
  }
  else
    return write(
      &val, 
      sizeof(uint16_t)
    );

  return 0;
}

ullong EsFile::int32Write(long val)
{
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    const EsString& str = EsString::fromLong(val);
    if( writeString(str) )
      return str.size();
  }
  else
    return write(
      &val, 
      sizeof(int32_t)
    );

  return 0;
}

ullong EsFile::uint32Write(ulong val)
{
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    const EsString& str = EsString::fromULong(val);
    if( writeString(str) )
      return str.size();
  }
  else
    return write(
      &val, 
      sizeof(uint32_t)
    );

  return 0;
}

ullong EsFile::int64Write(llong val)
{
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    const EsString& str = EsString::fromInt64(val);
    if( writeString(str) )
      return str.size();
  }
  else
    return write(
      &val, 
      sizeof(int64_t)
    );

  return 0;
}

ullong EsFile::uint64Write(ullong val)
{
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    const EsString& str = EsString::fromUInt64(val);
    if( writeString(str) )
      return str.size();
  }
  else
    return write(
      &val, 
      sizeof(uint64_t)
    );

  return 0;
}

ullong EsFile::writeString(const EsString& str, bool prependLength /*= false*/)
{
  ullong written = 0;
  if( prependLength )
  {
    uint32_t len = static_cast<uint32_t>(str.size());
    written = write(
      &len, 
      sizeof(uint32_t)
    );
    ES_ASSERT(sizeof(uint32_t) == written);
  }

  if( !str.empty() )
  {
    const EsByteString& bs = EsString::toUtf8(str);
    written += write(static_cast<const void*>(bs.data()), bs.size());
  }

  return written;
}

ullong EsFile::writeAllAsString(cr_EsString val)
{
  return writeString(val);
}

ullong EsFile::stringWrite(cr_EsString val)
{
  return writeString(val, (m_flags & static_cast<ulong>(EsFileFlag::Text)) ? false : true);
}

ullong EsFile::binBufferWrite(cr_EsBinBuffer val)
{
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    const EsString& str = EsString::binToHex(val);
    if( writeString(str) )
      return str.size();
  }
  else
  {
    ulong len = static_cast<ulong>(val.size());
    if( 4 == write(&len, 4) )
    {
      if( !val.empty() )
        return 4 + write(static_cast<const void*>(val.data()), len);
      else
        return 4;
    }
  }

  return 0;
}

ullong EsFile::writeAllAsBinBuffer(cr_EsBinBuffer val)
{
  if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
  {
    const EsString& str = EsString::binToHex(val);
    if( writeString(str) )
      return str.size();
  }
  else
  {
    if( !val.empty() )
      return write(static_cast<const void*>(val.data()), val.size());
  }

  return 0;
}

EsString EsFile::readAllAsString()
{
  const EsBinBuffer& buff = readAllAsBinBuffer();
  if( !buff.empty() )
  {
    if( m_flags & static_cast<ulong>(EsFileFlag::Text) )
    {
      EsByteString bs(buff.begin(), buff.end());
      return EsString::fromUtf8(bs);
    }
    else
      return EsString::binToHex(buff);
  }

  return EsString::null();
}

EsBinBuffer EsFile::readAllAsBinBuffer()
{
  EsBinBuffer result;
  result.resize( static_cast<size_t>(get_length()) );
  if( result.size() == read(result.data(), result.size()) )
    return result;

  return EsBinBuffer::null();
}

// static services
EsString EsFile::nameExtGet(cr_EsString filePath)
{
  const EsPath& fn = EsPath::createFromFilePath( filePath );
  return fn.fileNameExtGet();
}

EsString EsFile::nameGet(cr_EsString filePath)
{
  const EsPath& fn = EsPath::createFromFilePath( filePath );
  return fn.fileNameGet();
}

EsString EsFile::extGet(cr_EsString filePath)
{
  const EsPath& fn = EsPath::createFromFilePath( filePath );
  return fn.fileExtGet();
}
//---------------------------------------------------------------------------

#if ES_OS == ES_OS_WINDOWS
# include "EsFile.win.cxx"
#elif defined(ES_POSIX_COMPAT)
# include "EsFile.posix.cxx"
#endif

