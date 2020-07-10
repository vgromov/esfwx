#ifndef _es_composite_file_h_
#define _es_composite_file_h_

//// composite file support class. allows to create
//// single file from multiple sources. supplies composite file 
//// blocks parsing|access as well
////
//class ESCORE_CLASS EsCompositeFile
//{
//public:
//  // nested types
//  //
//  // known composite block types
//  enum {
//    // firmware block
//    blockTypeFirmware,
//    // localized messages block, language id goes to m_info field
//    blockTypeMessages,
//    // special value - end of known block ids
//    KnownBlockType,
//    // special value - empty block
//    blockNoType = 0xFFFFFFFF,
//  };
//  // composite block location table header
//  //
//  struct LoctableHeader
//  {
//    LoctableHeader() : 
//    m_size(0),
//    m_guid0(c_guid0),
//    m_guid1(c_guid1),
//    m_guid2(c_guid2),
//    m_guid3(c_guid3),
//    m_crc32(0)
//    {
//    }
//    
//    // check if locheader guid is ok
//    bool isGuidOk() const
//    {
//      return m_guid0 == c_guid0 &&
//        m_guid1 == c_guid1 &&
//        m_guid2 == c_guid2 &&
//        m_guid3 == c_guid3;
//    }
//
//    // loctable entries size in bytes
//    esU32 m_size;
//    // loctable guid components
//    esU32 m_guid0;  
//    esU32 m_guid1;
//    esU32 m_guid2;
//    esU32 m_guid3;
//    // loctable CRC
//    esU32 m_crc32;
//    
//    static const esU32 c_guid0 = 0xE405E9E6;
//    static const esU32 c_guid1 = 0xA59E4C64;
//    static const esU32 c_guid2 = 0xB664DB32;
//    static const esU32 c_guid3 = 0x690AB77C;
//  };
//  // composite block location entry
//  //
//  struct LoctableEntry
//  {
//    LoctableEntry() :
//    m_type(blockNoType),
//    m_info(0),
//    m_offs(0),
//    m_len(0),
//    m_crc32(0)
//    {}
//    
//    // block type & info
//    esU32 m_type;
//    esU32 m_info;
//    // block offset in bytes from the end of the composite file
//    esU32 m_offs;
//    // block length in bytes
//    esU32 m_len;
//    // block contents CRC
//    esU32 m_crc32;
//  };
//  typedef std::vector<LoctableEntry> LoctableEntries;
//    
//public:
//  // just create empty composite file object
//  EsCompositeFile();
//  // return current file path
//  EsString getPath() const;
//  // create new composite file object, resettings all existing contents
//  void create(const EsString& file, const EsString& baseFile);
//  void create(const EsString& file, const EsBinBuffer& baseBlock);
//  // append composite block to new file
//  void appendBlock(const EsString& file, esU32 type, esU32 info = 0);
//  void appendBlock(const EsBinBuffer& block, esU32 type, esU32 info = 0);
//  // read and parse existing composite file
//  void load(const EsString& file);
//  // access composite blocks
//  size_t getBlocksCount() const;
//  esU32 getBlockType(size_t idx) const;
//  esU32 getBlockInfo(size_t idx) const;
//  EsBinBuffer getBlock(size_t idx) const;
//  
//protected:
//  // internal services
//  //
//  // get all file contents except loctable as binary buffer
//  void getAllExceptLoctable(EsBinBuffer& buff);
//  // reset loctable
//  void resetLoctable();
//  // recalculate loctable and update file
////  void updateLoctable(wxFileOutputStream& fs);  
//  // header field update
//  void updateLoctableHdr();
//  // write loctable to the target file
////  void writeLoctable(wxFileOutputStream& fs);
//  // get offset from the end of the current file to the beginning of loctable
//  esU32 getLoctableLen() const;
//  
//protected:
//  // data members
//  //
//  // composite file name
//  EsPath m_file;
//  bool m_newFile;
//  // loctable
//  LoctableHeader m_locHdr;
//  LoctableEntries m_locEntries;
//  bool m_locTableDirty;
//};

#endif // _es_composite_file_h_
