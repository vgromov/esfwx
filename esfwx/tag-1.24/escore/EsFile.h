#ifndef _es_file_h_
#define _es_file_h_

enum class EsFileFlag : ulong
{
  Read        = 0x0001,
  Write       = 0x0002,
  Append      = 0x0004,
  Exclusive   = 0x0008,
  Text        = 0x0010,
};

/// Reflected class for file operations
///
class ESCORE_CLASS ES_INTF_IMPL1(EsFile, EsReflectedClassIntf)

public:
  EsFile();
  EsFile(const EsString& fileName, ulong flags);
  virtual ~EsFile();
  
  ES_DECL_REFLECTED_CLASS_BASE(EsFile)
  ES_DECL_ATTR_HANDLING_STD
  
  // reflected properties
  //
  ES_DECL_PROPERTY_STD(name, EsString)
  ES_DECL_PROPERTY_STD(delimiters, EsString)        ///< only for text files
  ES_DECL_PROPERTY_RO(recentErrorString, EsString)
  ES_DECL_PROPERTY(length, ullong)
  ES_DECL_PROPERTY_RO(offset, llong)
  ES_DECL_PROPERTY_STD(flags, ulong)
  ES_DECL_PROPERTY_RO(opened, bool)
  ES_DECL_PROPERTY_RO(eof, bool)

  // reflected methods
  //
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);
  ES_DECL_REFLECTED_CLASS_CTOR2(EsBaseIntfPtr, cr_EsString, ulong);
  ES_DECL_REFLECTED_METHOD0(bool, open);
  ES_DECL_REFLECTED_METHOD0(void, close);
  ES_DECL_REFLECTED_METHOD1(llong, seek, ullong);
  ES_DECL_REFLECTED_METHOD1(llong, seekRelative, llong);
  ES_DECL_REFLECTED_METHOD1(llong, seekEnd, ullong);
  ES_DECL_REFLECTED_METHOD0(void, remove);
  // reading
  ullong rawRead(void* dest, ullong toRead);
  ES_DECL_REFLECTED_METHOD0(long, int8Read);
  ES_DECL_REFLECTED_METHOD0(ulong, uint8Read);
  ES_DECL_REFLECTED_METHOD0(long, int16Read);
  ES_DECL_REFLECTED_METHOD0(ulong, uint16Read);
  ES_DECL_REFLECTED_METHOD0(long, int32Read);
  ES_DECL_REFLECTED_METHOD0(ulong, uint32Read);
  ES_DECL_REFLECTED_METHOD0(llong, int64Read);
  ES_DECL_REFLECTED_METHOD0(ullong, uint64Read);
  ES_DECL_REFLECTED_METHOD0(EsString, stringRead);
  ES_DECL_REFLECTED_METHOD0(EsBinBuffer, binBufferRead);
  ES_DECL_REFLECTED_METHOD0(EsString, readAllAsString);
  ES_DECL_REFLECTED_METHOD0(EsBinBuffer, readAllAsBinBuffer);
  // writing
  ullong rawWrite(const void* src, ullong toWrite);
  ES_DECL_REFLECTED_METHOD1(ullong, int8Write, long);
  ES_DECL_REFLECTED_METHOD1(ullong, uint8Write, ulong);
  ES_DECL_REFLECTED_METHOD1(ullong, int16Write, long);
  ES_DECL_REFLECTED_METHOD1(ullong, uint16Write, ulong);
  ES_DECL_REFLECTED_METHOD1(ullong, int32Write, long);
  ES_DECL_REFLECTED_METHOD1(ullong, uint32Write, ulong);
  ES_DECL_REFLECTED_METHOD1(ullong, int64Write, llong);
  ES_DECL_REFLECTED_METHOD1(ullong, uint64Write, ullong);
  ES_DECL_REFLECTED_METHOD1(ullong, stringWrite, cr_EsString);
  ES_DECL_REFLECTED_METHOD1(ullong, binBufferWrite, cr_EsBinBuffer);
  ES_DECL_REFLECTED_METHOD1(ullong, writeAllAsString, cr_EsString);
  ES_DECL_REFLECTED_METHOD1(ullong, writeAllAsBinBuffer, cr_EsBinBuffer);

  // static services
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, nameExtGet, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, nameGet, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, extGet, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(void, remove, cr_EsString);

  ulong recentErrorGet() const { return m_lastError; }

  ullong read(void* dest, ullong toRead);
  ullong write(const void* src, ullong toWrite);

protected:
  ullong writeString(const EsString& str, bool prependLength = false);
  EsString readTextWord();
  void checkLengthOnRead(size_t len) const;
  void checkFileIsOpen() const;
  void checkFileIsClosed() const;
  void checkFileIsWriteable() const;

protected:
  mutable ulong m_lastError;
  bool m_eof;

#if ES_OS == ES_OS_WINDOWS
  HANDLE m_file;
#elif defined(ES_POSIX_COMPAT)
  int m_file;
#endif

private:
  // prohibited functionality
  EsFile(const EsFile&) ES_REMOVEDECL;
  EsFile& operator=(const EsFile&) ES_REMOVEDECL;
};

namespace EsReflection
{
/// File flags reflected enumeration
///
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCORE_CLASS, EsFileFlag)
  ES_DECL_ENUMERATION_ITEM_LABELLED(Read,        static_cast<ulong>(::EsFileFlag::Read),      esT("File read access"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Write,      static_cast<ulong>(::EsFileFlag::Write),     esT("File write access"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Append,      static_cast<ulong>(::EsFileFlag::Append),    esT("File append access"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Exclusive,  static_cast<ulong>(::EsFileFlag::Exclusive), esT("File exclusive access"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Text,        static_cast<ulong>(::EsFileFlag::Text),      esT("File is of text type"))
ES_DECL_ENUMERATION_END
}

#endif // _es_file_h_
