#include "escorepch.h"
#pragma hdrstop

#include "EsStreamBinary.h"
#include "EsCryptoDesLegacy.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// Binary stream Magic ID
static const esU32 sc_bsMagic = 0xABBAEC0E;

// Minimum reservation chunk
static const esU32 sc_minChunk = 8192;

// Binary stream main header
struct EsStreamBinaryHeader
{
  esU32 m_magicId;  //< Magic ID = 0xABBAEC0E
  esU32 m_len;      //< Stream data length, excluding header
  esU32 m_flags;    //< Stream persistent flags, like Compressed and Encrypted
  esU32 m_crc;      //< Stream CRC32, excluding header
};

// Binary stream block header
struct EsStreamBinaryBlockHeader
{
  esU32 m_id;       //< Scope ID
  esU32 m_len;      //< Scope contents length
};

// Fixed sizes
enum {
  EsStreamBinaryHeader_SZE        = sizeof(EsStreamBinaryHeader),
  EsStreamBinaryBlockHeader_SZE   = sizeof(EsStreamBinaryBlockHeader),
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ES_DECL_CLASS_INFO_DERIVED_BEGIN(EsStreamBinary, EsStream, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(EsStreamBinary, EsBaseIntfPtr_ClassCall_ulong_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(EsStreamBinary, EsBaseIntfPtr_ClassCall_ulong_ulong_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsStreamBinary, save, void_CallConst_cr_EsString, NO_METHOD_DESCR)
  // Reflected properties
  //
  ES_DECL_PROP_INFO(EsStreamBinary, key, EsString, esT("Encryption key"), NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsStreamBinary::EsStreamBinary(ulong flags, ulong version, const EsVariant& src, const EsBaseIntfPtr& factory) :
EsStream(flags, version, factory)
{
  init(src, version);
  internalRewind();
}
//---------------------------------------------------------------------------

EsStreamIntf::Ptr EsStreamBinary::create(ulong flags, ulong version)
{
  return create(
    flags,
    0,
    EsVariant::null(),
    nullptr
  );
}
//---------------------------------------------------------------------------

EsStreamIntf::Ptr EsStreamBinary::create(ulong flags, const EsVariant& src, const EsBaseIntfPtr& factory /*= nullptr*/)
{
  return create(
    flags,
    0,
    src,
    factory
  );
}
//---------------------------------------------------------------------------

EsStreamIntf::Ptr EsStreamBinary::create(ulong flags, ulong version, const EsVariant& src, const EsBaseIntfPtr& factory /*= nullptr*/)
{
  std::unique_ptr<EsStreamBinary> p(
    new EsStreamBinary(
      flags, 
      version, 
      src, 
      factory
    )
  );
  ES_ASSERT(p);
  
  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsStreamBinary::~EsStreamBinary()
{
  if(
    (
      (as_<ulong>(EsStreamFlag::File)|as_<ulong>(EsStreamFlag::Write)) ==
        (m_flags & (as_<ulong>(EsStreamFlag::File)|as_<ulong>(EsStreamFlag::Write)))
    ) &&
    (
      !EsPath::fileExists(
        m_file,
        EsString::null()
      ) ||
      (m_flags & flagDirty)
    )
  )
  {
    // do not allow anything to throw from destructor
    try
    {
      internalFileSave( m_file );
    }
    catch(...)
    {}

    m_flags &= ~flagDirty;
  }
}
//---------------------------------------------------------------------------

EsString EsStreamBinary::get_key() const
{
  return m_key;
}
//---------------------------------------------------------------------------

void EsStreamBinary::set_key(const EsString& key)
{
  m_key = key;

  if( m_key.empty() )
    m_flags &= ~as_<ulong>(EsStreamFlag::Encrypted);
  else
    m_flags |= as_<ulong>(EsStreamFlag::Encrypted);
}
//---------------------------------------------------------------------------

void EsStreamBinary::init(const EsVariant& src, ulong version)
{
  if( src.isEmpty() )
    return;

  if( m_flags & as_<ulong>(EsStreamFlag::File) )
  {
    if( !EsPath::fileExists( src.asString(), EsString::null() ) )
    {
      // initialize just file name
      m_file = src.asString();
      return;
    }

    EsFile file(
      src.asString(),
      as_<ulong>(EsFileFlag::Read)
    );

    EsBinBuffer bb;

    if( file.open() )
    {
      bb = file.readAllAsBinBuffer();
      parse(bb);
      m_file = src.asString();
    }
    else
      EsException::Throw(
        esT("Could not load Binary stream '%s', '%s'"),
        src.asString(),
        EsUtilities::osErrorStringGet(
          file.recentErrorGet()
        )
      );
  }
  else // try to interpret src as binary buffer
    parse(
      src.asBinBuffer()
    );
}
//---------------------------------------------------------------------------

bool EsStreamBinary::internalFileSave(const EsString& fname) const
{
  EsBinBuffer bb;
  generate(bb);

  EsFile file(fname, as_<ulong>(EsFileFlag::Write));

  if( file.open() )
    return bb.size() == file.writeAllAsBinBuffer(bb);

  return false;
}
//---------------------------------------------------------------------------

EsString EsStreamBinary::asString() const
{
  EsBinBuffer bb;
  generate(bb);

  return EsString::binToHex(bb);
}
//---------------------------------------------------------------------------

void EsStreamBinary::save(const EsString& target /*= EsString::null()*/) const
{
  if( !target.empty() )
  {
    // try to expand file path to absolute full one
    const EsPath& fname = EsPath::createFromFilePath( target );
    if( fname.isOk() )
    {
      const EsString& fpath = fname.pathGet();
      if( internalFileSave(fpath) )
      {
        m_file = fpath;
        m_flags |= static_cast<ulong>(EsStreamFlag::File);
        m_flags &= ~flagDirty;
      }
    }
  }
  else if( !m_file.empty() )
  {
    ES_ASSERT(m_flags & static_cast<ulong>(EsStreamFlag::File));

    if( !internalFileSave(m_file) )
      EsException::Throw(
        esT("Could not save document to file '%s'"),
        m_file
      );

    m_flags &= ~flagDirty;
  }
  else
    EsException::Throw(
      esT("Could not save the document to default file, document must be saved to the file at list once first"));
}
//---------------------------------------------------------------------------

void EsStreamBinary::parse(const EsBinBuffer& bb)
{
  mainHeaderCheck(bb);

  const EsStreamBinaryHeader* hdr = reinterpret_cast<const EsStreamBinaryHeader*>(
    &bb[0]
  );

  // If needed, decompress and decode stream contents. Compression always comes last
  //
  EsBinBuffer data(
    bb.begin()+EsStreamBinaryHeader_SZE,
    bb.begin()+EsStreamBinaryHeader_SZE+hdr->m_len
  );

  if( hdr->m_flags & static_cast<ulong>(EsStreamFlag::Encrypted) )
  {
    if( m_key.empty() )
      EsException::Throw(
        esT("Encrypted flag was specified, but encryption key is not provided.")
      );

#ifdef ES_USE_CRYPTO_LEGACY
    EsCryptoDesLegacy des(m_key);
    des.process(data, data, false);
#else
    EsException::Throw(
      esT("Encrypted flag was specified, but core library was compiled without ES_USE_CRYPTO_LEGACY flag.")
    );
#endif
  }

  if( hdr->m_flags & static_cast<ulong>(EsStreamFlag::Compressed) )
  {
#ifdef ES_USE_ZLIB
    EsZipDecompressor zip;
    data = zip.decompress(data);
#else
    EsException::Throw( esT("Re-compilation with ES_USE_ZLIB is required to parse compressed streams") );
#endif
  }

  // Parse data contents, starting from the root scope
  BlockScope scope(
    *this,
    m_root
  );

  ulong pos = 0;
  blockParse(
    data,
    pos,
    static_cast<ulong>(data.size())
  );
}
//---------------------------------------------------------------------------

void EsStreamBinary::generate(EsBinBuffer& data) const
{
  data.reserve( sc_minChunk );

  BlockScope scope(
    const_cast<EsStreamBinary&>(*this),
    m_root
  );

  ulong pos = 0;
  blockToBuffer(data, pos);

  if( m_flags & static_cast<ulong>(EsStreamFlag::Compressed) )
  {
#ifdef ES_USE_ZLIB
    EsZipCompressor zip;
    data = zip.compress(data);
#else
    EsException::Throw( esT("Re-compilation with ES_USE_ZLIB is required to generate compressed streams") );
#endif
  }

  if( m_flags & static_cast<ulong>(EsStreamFlag::Encrypted) )
  {
    if( m_key.empty() )
      EsException::Throw(
        esT("Encrypted flag was specified, but encryption key is not provided.")
      );

#ifdef ES_USE_CRYPTO_LEGACY
    EsCryptoDesLegacy des(m_key);
    des.process(data, data);
#else
    EsException::Throw(
      esT("Encrypted flag was specified, but core library was compiled without ES_USE_CRYPTO_LEGACY flag.")
    );
#endif
  }

  mainHeaderPrepend(data);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Binary data parsing
//

void EsStreamBinary::mainHeaderCheck(const EsBinBuffer& bb)
{
  if( bb.size() < EsStreamBinaryHeader_SZE )
    EsException::Throw(
      esT("Binary stream size is too small.")
    );

  EsStreamBinaryHeader hdr;
  memcpy(
    static_cast<void*>(&hdr),
    static_cast<const void*>(bb.data()),
    EsStreamBinaryHeader_SZE
  );

  if( hdr.m_magicId != sc_bsMagic )
    EsException::Throw(
      esT("Binary stream ID mismatch. Stream format is not recognized.")
    );

  if( hdr.m_len > bb.size()-EsStreamBinaryHeader_SZE )
    EsException::Throw(
      esT("Binary stream is of wrong length. Stream contents is broken.")
    );

  EsCRC32_IEEE802_3 crc(
    bb.data()+EsStreamBinaryHeader_SZE,
    hdr.m_len,
    sc_bsMagic
  );

  if( hdr.m_crc != crc.get_value() )
    EsException::Throw(
      esT("Binary stream CRC check failed. Stream contents is broken.")
    );
}
//---------------------------------------------------------------------------

static void dataSpaceCheck(ulong pos, ulong end, ulong requestedLen)
{
  if( end < pos ||
      end - pos < requestedLen
  )
    EsException::Throw(
      esT("Error parsing binary stream. Trying to read data beyond the end block.")
    );
}
//---------------------------------------------------------------------------

template <typename IntT>
static IntT numericRead(const EsBinBuffer& data, ulong& pos, ulong end)
{
  ulong sze = sizeof(IntT);
  dataSpaceCheck(
    pos,
    end,
    sze
  );

  IntT result = 0;
  memcpy(
    static_cast<void*>(&result),
    static_cast<const void*>(data.data()+pos),
    sze
  );
  pos += sze;

  return result;
}
//---------------------------------------------------------------------------

static EsString stringRead(const EsBinBuffer& data, ulong& pos, ulong end)
{
  // Read string length first
  esU32 len = numericRead<esU32>(
    data,
    pos,
    end
  );

  if( !len )
    return EsString::null();

  // Read string contents
  dataSpaceCheck(
    pos,
    end,
    len
  );

  EsByteString bs(
    static_cast<EsByteString::const_pointer>(
      static_cast<const void*>(
        data.data()+pos
      )
    ),
    len
  );
  pos += len;

  return EsString::fromUtf8(bs);
}
//---------------------------------------------------------------------------

static EsBinBuffer bufferRead(const EsBinBuffer& data, ulong& pos, ulong end)
{
  // Read buffer length first
  esU32 len = numericRead<esU32>(
    data,
    pos,
    end
  );

  if( !len )
    return EsBinBuffer::null();

  // Read buffer contents
  EsBinBuffer bb( data.data()+pos, data.data()+pos+len );
  pos += len;

  return bb;
}
//---------------------------------------------------------------------------

static EsVariant varPODread(const EsBinBuffer& data, ulong& pos, ulong end, ulong type)
{
  ES_ASSERT( type <= EsVariant::VAR_STRING );

  if( EsVariant::VAR_EMPTY == type )
    return EsVariant::null();

  switch( type )
  {
  case EsVariant::VAR_BOOL:
    return EsVariant( static_cast<bool>(0 != numericRead<esU8>(data, pos, end))); //< Due to explicit ctor
  case EsVariant::VAR_BYTE:
    return EsVariant( numericRead<esU8>(data, pos, end) ); //< Due to explicit ctor
  case EsVariant::VAR_CHAR:
    return EsVariant(
      static_cast< EsString::value_type >(
        numericRead<esU32>(data, pos, end)
      ),
      EsVariant::ACCEPT_CHAR
    );
  case EsVariant::VAR_UINT:
    return numericRead<esU32>(data, pos, end);
  case EsVariant::VAR_INT:
    return numericRead<esI32>(data, pos, end);
  case EsVariant::VAR_UINT64:
    return numericRead<esU64>(data, pos, end);
  case EsVariant::VAR_INT64:
    return numericRead<esI64>(data, pos, end);
  case EsVariant::VAR_DOUBLE:
    return numericRead<double>(data, pos, end);
  case EsVariant::VAR_BIN_BUFFER:
    return bufferRead(data, pos, end);
  case EsVariant::VAR_STRING:
    return stringRead(data, pos, end);
  }

  return EsVariant::null();
}
//---------------------------------------------------------------------------

static EsVariant varPODread(const EsBinBuffer& data, ulong& pos, ulong end)
{
  // Read variant type id
  ulong type = numericRead<esU32>(data, pos, end);

  return varPODread(data, pos, end, type);
}
//---------------------------------------------------------------------------

void EsStreamBinary::blockParse(const EsBinBuffer& data, ulong& pos, ulong end)
{
  ES_ASSERT(m_block);

  dataSpaceCheck(pos, end, EsStreamBinaryBlockHeader_SZE);

  EsStreamBinaryBlockHeader hdr;
  memcpy(
    static_cast<void*>(&hdr),
    static_cast<const void*>(data.data()+pos),
    EsStreamBinaryBlockHeader_SZE
  );

  if( !EsStreamBlock::isKnownId(hdr.m_id) )  //< Just skip unknown block(s)
  {
    pos += EsStreamBinaryBlockHeader_SZE+hdr.m_len;
    return;
  }

  // Check if there is enough data space for block to be parsed
  dataSpaceCheck(pos+EsStreamBinaryBlockHeader_SZE, end, hdr.m_len);

  // Move to the start of the block's data
  pos += EsStreamBinaryBlockHeader_SZE;
  // Assign end boundary for this block
  end = pos + hdr.m_len;

  EsStreamBlock::Ptr block;

  if( EsStreamBlock::Item == hdr.m_id )
  {
    ulong idx = numericRead<esU32>(
      data,
      pos,
      end
    );

    block = m_block->itemAdd(idx);
  }
  else
  {
    const EsString& name = stringRead(
      data,
      pos,
      end
    );

    // Do not create root twice
    if( EsStreamBlock::root() == name )
    {
      ES_ASSERT( m_block->isRoot() );
      block = m_block;
    }
    else
      block = m_block->childAdd(
        name,
        hdr.m_id
      );
  }

  // Read attributes collection
  ulong attrCnt = numericRead<esU32>(data, pos, end);
  while( attrCnt )
  {
    const EsString& attrName = stringRead(data, pos, end);
    ES_ASSERT( !attrName.empty() );

    const EsVariant& attrVal = varPODread(data, pos, end);

    block->attributeAdd(
      attrName,
      attrVal
    );

    --attrCnt;
  }

  // Read and set optional POD payload
  if( block->haveType() &&
      EsVariant::VAR_STRING >= block->typeGet()
  )
  {
    block->payloadSet(
      varPODread(
        data,
        pos,
        end,
        block->typeGet()
      )
    );

    return; // No need to process further
  }

  if( pos < end )  //< If there are any nested blocks remain - parse them
  {
    // Initiate block scoping
    BlockScope scope( *this, block );

    // Parse block internal data, if any
    while( pos < end )
    {
      blockParse(
        data,
        pos,
        end
      );
    }
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Binary data generation
//

static inline void ensureDataHaveEnoughSpace(EsBinBuffer& data, ulong pos, ulong requestedLen)
{
  ulong newSize = pos + requestedLen;
  if( data.size() < newSize )
    data.resize( newSize );
}
//---------------------------------------------------------------------------

template <typename IntT>
static void numericWrite(EsBinBuffer& data, ulong& pos, IntT val)
{
  ulong sze = sizeof(IntT);
  ensureDataHaveEnoughSpace(
    data,
    pos,
    sze
  );

  memcpy(
    static_cast<void*>(data.data()+pos),
    static_cast<const void*>(&val),
    sze
  );

  pos += sze;
}
//---------------------------------------------------------------------------

static void stringWrite(EsBinBuffer& data, ulong& pos, const EsString& str)
{
  if( !str.empty() )
  {
    const EsByteString& bs = EsString::toUtf8( str );

    ulong sze = static_cast<ulong>(bs.size());

    // Write string length first
    numericWrite<esU32>(
      data,
      pos,
      sze
    );

    // Write string contents
    ensureDataHaveEnoughSpace(
      data,
      pos,
      sze
    );

    memcpy(
      static_cast<void*>(data.data()+pos),
      static_cast<const void*>(bs.data()),
      sze
    );

    pos += sze;
  }
  else
    numericWrite<esU32>(
      data,
      pos,
      0
    );
}
//---------------------------------------------------------------------------

static void bufferWrite(EsBinBuffer& data, ulong& pos, const EsBinBuffer& bb)
{
  if( !bb.empty() )
  {
    ulong sze = static_cast<ulong>(bb.size());

    // Write buffer length first
    numericWrite<esU32>(
      data,
      pos,
      sze
    );

    // Write string contents
    ensureDataHaveEnoughSpace(
      data,
      pos,
      sze
    );

    memcpy(
      static_cast<void*>(data.data()+pos),
      static_cast<const void*>(bb.data()),
      sze
    );

    pos += sze;
  }
  else
    numericWrite<esU32>(
      data,
      pos,
      0
    );
}
//---------------------------------------------------------------------------

static void varPODwrite(EsBinBuffer& data, ulong& pos, const EsVariant& var, bool typeWrite = true)
{
  ulong type = var.typeGet();

  ES_ASSERT( type <= EsVariant::VAR_STRING );

  if( typeWrite )
    numericWrite<esU32>(data, pos, type);

  switch( type )
  {
  case EsVariant::VAR_EMPTY:
    break;
  case EsVariant::VAR_BOOL:
    numericWrite<esU8>(data, pos, var.asBool() ? 1 : 0 );
    break;
  case EsVariant::VAR_BYTE:
    numericWrite<esU8>(data, pos, var.asByte());
    break;
  case EsVariant::VAR_CHAR:
    numericWrite<esU32>(data, pos, var.asChar());
    break;
  case EsVariant::VAR_UINT:
    numericWrite<esU32>(data, pos, var.asULong());
    break;
  case EsVariant::VAR_INT:
    numericWrite<esI32>(data, pos, var.asLong());
    break;
  case EsVariant::VAR_UINT64:
    numericWrite<esU64>(data, pos, var.asULLong());
    break;
  case EsVariant::VAR_INT64:
    numericWrite<esI64>(data, pos, var.asLLong());
    break;
  case EsVariant::VAR_DOUBLE:
    numericWrite<double>(data, pos, var.asDouble());
    break;
  case EsVariant::VAR_BIN_BUFFER:
    bufferWrite(data, pos, var.asBinBuffer());
    break;
  case EsVariant::VAR_STRING:
    stringWrite(data, pos, var.asString());
    break;
  }
}
//---------------------------------------------------------------------------

void EsStreamBinary::mainHeaderPrepend(EsBinBuffer& bb) const
{
  EsStreamBinaryHeader hdr;
  hdr.m_magicId = sc_bsMagic;
  hdr.m_len = static_cast<ulong>(bb.size());
  hdr.m_flags = m_flags & (as_<ulong>(EsStreamFlag::Compressed)|
                           as_<ulong>(EsStreamFlag::Encrypted)
                          );

  EsCRC32_IEEE802_3 crc(
    bb,
    sc_bsMagic
  );

  hdr.m_crc = crc.get_value();

  bb.insert(
    bb.begin(),
    reinterpret_cast<EsBinBuffer::const_pointer>(&hdr),
    reinterpret_cast<EsBinBuffer::const_pointer>(&hdr)+EsStreamBinaryHeader_SZE
  );
}
//---------------------------------------------------------------------------

void EsStreamBinary::blockToBuffer(EsBinBuffer& data, ulong& pos) const
{
  ES_ASSERT(m_block);

  ensureDataHaveEnoughSpace(data, pos, EsStreamBinaryBlockHeader_SZE);

  ulong hdrStart = pos;
  EsStreamBinaryBlockHeader hdr;

  hdr.m_id = m_block->idGet();

  // Move to the start of the block's data
  pos += EsStreamBinaryBlockHeader_SZE;
  ulong start = pos;

  if( EsStreamBlock::Item == hdr.m_id )
  {
    numericWrite<esU32>(
      data,
      pos,
      m_block->attributeGet(
        EsStreamBlock::index()
      ).asULong()
    );
  }
  else
  {
    stringWrite(
      data,
      pos,
      m_block->nameGet()
    );
  }

  // Write attributes collection
  if( m_block->haveAttributes() )
  {
    const EsStringIndexedMap& attrs = m_block->attributesGet();
    ulong attrCnt = static_cast<ulong>(attrs.countGet());

    ES_ASSERT(attrCnt);

    numericWrite<esU32>(data, pos, attrCnt);
    for(ulong idx = 0; idx < attrCnt; ++idx )
    {
      const EsString& attrName = attrs.nameGet(idx);
      stringWrite(data, pos, attrName);

      const EsVariant& attrVal = attrs.valueGet(idx);
      varPODwrite(data, pos, attrVal);
    }
  }
  else
    numericWrite<esU32>(data, pos, 0);

  // Write and set optional POD payload
  if( m_block->isPOD() )
  {
    // Write just POD data, without type spec
    varPODwrite(
      data,
      pos,
      m_block->payloadGet(),
      false
    );
  }

  // If there are any nested blocks remain - write them
  EsStreamBlock::Ptr child = m_block->firstChildGet();
  while( child )
  {
    // Initiate block scoping
    BlockScope scope( const_cast<EsStreamBinary&>(*this), child );

    // Write scoped child
    blockToBuffer(data, pos);

    // Proceed to the next child
    child = child->nextSiblingGet();
  }

  hdr.m_len = pos-start;
  memcpy(
    static_cast<void*>(data.data()+hdrStart),
    static_cast<const void*>(&hdr),
    EsStreamBinaryBlockHeader_SZE
  );
}
//---------------------------------------------------------------------------

