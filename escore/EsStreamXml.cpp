#include "escorepch.h"
#pragma hdrstop

#include "EsStreamXml.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

class EsStreamXmlCtorAcc : public EsStreamXml
{
public:
  EsStreamXmlCtorAcc(ulong flags, ulong version, const EsString& src, const EsBaseIntfPtr& factory) :
  EsStreamXml(
    flags,
    version,
    src,
    factory
  )
  {}
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ES_DECL_CLASS_INFO_DERIVED_BEGIN(EsStreamXml, EsStream, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(EsStreamXml, EsBaseIntfPtr_ClassCall_ulong_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(EsStreamXml, EsBaseIntfPtr_ClassCall_ulong_ulong_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsStreamXml, save, void_CallConst_cr_EsString, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsStreamXml::EsStreamXml(ulong flags, ulong version, const EsString& src, const EsBaseIntfPtr& factory) :
EsStream(flags, version, factory)
{
  // Unsupported flags check
  if( flags & static_cast<ulong>(EsStreamFlag::Encrypted) )
    EsException::Throw(esT("Encryption is not supported in EsStreamXml"));

  if( flags & static_cast<ulong>(EsStreamFlag::Compressed) )
    EsException::Throw(esT("Compression is not supported in EsStreamXml"));

  init(src, version);
  internalRewind();
}
//---------------------------------------------------------------------------

EsStreamIntf::Ptr EsStreamXml::create(ulong flags, const EsString& src /*= EsString::null()*/,
    const EsBaseIntfPtr& factory /*= nullptr*/)
{
  return create(
    flags,
    0,
    src,
    factory
  );
}
//---------------------------------------------------------------------------

EsStreamIntf::Ptr EsStreamXml::create(ulong flags, ulong version, const EsString& src /*= EsString::null()*/,
    const EsBaseIntfPtr& factory /*= nullptr*/)
{
  std::unique_ptr<EsStreamXmlCtorAcc> p = ES_MAKE_UNIQUE(
    EsStreamXmlCtorAcc,
    flags,
    version,
    src,
    factory
  );
  ES_ASSERT(p);

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsStreamXml::~EsStreamXml()
{
  if(
    (
      as_<ulong>(EsStreamFlag::File)|as_<ulong>(EsStreamFlag::Write)) ==
        (m_flags & (as_<ulong>(EsStreamFlag::File)|as_<ulong>(EsStreamFlag::Write))
    ) &&
    (
      !EsPath::fileExists(m_file, EsString::null()) ||
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

void EsStreamXml::throwStreamIsMalformed(const EsString& expected, const EsString& got)
{
  EsException::Throw(
    esT("XML Stream is malformed, expected '%s', got '%s' node"),
    expected,
    got
  );
}
//---------------------------------------------------------------------------

void EsStreamXml::throwStreamIsMalformedNoAttr(const EsString& attr)
{
  EsException::Throw(
    esT("XML Stream is malformed, expected attribute '%s', is not found"),
    attr
  );
}
//---------------------------------------------------------------------------

void EsStreamXml::init(const EsString& src, ulong version)
{
  if( src.empty() )
    return;

  if( m_flags & static_cast<ulong>(EsStreamFlag::File) )
  {
    if( !EsPath::fileExists(src, EsString::null()) )
    {
      // initialize just file name
      m_file = src;
      return;
    }

    EsXmlDocument xml;
    if( xmlParseStatusOk == xml.loadFile(src, xmlParseDefault|xmlParseDeclaration).status )
    {
      parse(xml);
      m_file = src;
    }
    else
      EsException::Throw(
        esT("Could not load XML document '%s', or document is malformed"),
        src
      );
  }
  else // try to interpret src as xml stream in a string
  {
    EsXmlDocument xml;
    if( xmlParseStatusOk == xml.load(src) )
      parse(xml);
    else
      EsException::Throw(
        esT("Could not parse XML document, document is malformed")
      );
  }
}
//---------------------------------------------------------------------------

bool EsStreamXml::internalFileSave(const EsString& fname) const
{
  EsXmlDocument xml;
  generate(xml);

  return xml.saveFile(fname);
}
//---------------------------------------------------------------------------

EsString EsStreamXml::asString() const
{
  EsByteString bs;
  EsXmlWriterByteString bsw(bs);

  EsXmlDocument xml;
  generate(xml);

  xml.save(bsw);

  return EsString::fromUtf8(bs);
}
//---------------------------------------------------------------------------

void EsStreamXml::save(const EsString& target /*= EsString::null()*/) const
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

void EsStreamXml::xmlHeaderCheck(const EsXmlDocument& xml) const
{
  ES_ASSERT(xml);
  const EsXmlNode& decl = xml.firstChildGet();

  if( decl && xmlNodeDocDecl == decl.typeGet() )
  {
    if( 0 != EsString(esT("utf-8")).compare(decl.attributeGet(esT("encoding")).asString(), true) )
      EsException::Throw(esT("Document encoding is not supported, expected 'utf-8'"));

    if( 0 != EsString(esT("1.0")).compare(decl.attributeGet(esT("version")).asString(), true) )
      EsException::Throw(esT("Document version is not supported, expected 1.0"));
  }
  else
    EsException::Throw(esT("Document is malformed, XML document declaration node is missing"));

  const EsXmlNode& root = decl.nextSiblingGet(esT("CONTEXT"));
  const EsXmlAttribute& aname = root.attributeGet(
    EsStreamBlock::name().c_str()
  );
  if( !root ||
      !(EsString::cmpEqual == EsString::scompare( root.nameGet(), esT("CONTEXT")) &&
        aname &&
        EsString::cmpEqual == EsString::scompare( aname.valueGet(), esT("ROOT") ))
  )
    EsException::Throw(esT("Document is malformed, 'ROOT' element is expected at root"));
}
//---------------------------------------------------------------------------

void EsStreamXml::parse(const EsXmlDocument& xml)
{
  xmlHeaderCheck(xml);
  const EsXmlNode& root = xml.childFindByAttribute(
    esT("CONTEXT"),
    EsStreamBlock::name().c_str(),
    esT("ROOT")
  );

  ES_ASSERT( root );
  const EsXmlAttribute& ver = root.attributeGet(
    EsStreamBlock::version().c_str()
  );

  rootInit(ver ? ver.asUint(0) : 0);

  BlockScope scope(
    *this,
    m_root
  );

  EsXmlNode child = root.firstChildGet();
  while( child )
  {
    nodeParse(child);
    child = child.nextSiblingGet();
  }
}
//---------------------------------------------------------------------------

void EsStreamXml::nodeParse(const EsXmlNode& node)
{
  ES_ASSERT( m_block );
  ES_ASSERT( node );

  EsString name = node.nameGet();
  ulong id = EsStreamBlock::idByNameGet(
    name
  );

  if( EsStreamBlock::None == id )
    EsException::Throw(
      esT("Unknown XML stream node '%s'"),
      name
    );

  EsStreamBlock::Ptr block;
  if( EsStreamBlock::Item == id ) //< Create indexed block
  {
    const EsXmlAttribute& aidx = node.attributeGet(
      EsStreamBlock::index().c_str()
    );
    if( !aidx )
      throwStreamIsMalformedNoAttr( EsStreamBlock::index() );

    ulong idx = aidx.asUint();

    // Handle legacy malformed XML - items were allowed outside of an ITEM container
    if( EsStreamBlock::Items != m_block->idGet() )
    {
      EsStreamBlock::Ptr items = m_block->childGet(
        EsString::null(),
        EsStreamBlock::Items
      );

      if( !items )
        items = m_block->childAdd(
          EsString::null(),
          EsStreamBlock::Items
        );

      ES_ASSERT(items);
      m_block = items;
    }

    block = m_block->itemAdd(idx);
  }
  else //< Get new block name, create non-indexed block
  {
    name.clear();

    const EsXmlAttribute& aname = node.attributeGet(
      EsStreamBlock::name().c_str()
    );

    if( aname )
    {
      name = EsString::fromString(//toString(
        aname.valueGet()
      );
    }
    else if( EsStreamBlock::Object == id )
    {
      const EsXmlAttribute& atn = node.attributeGet(
        EsStreamBlock::typeName().c_str()
      );

      if( atn )
        name = atn.valueGet();
    }

    block = m_block->childAdd(
      name,
      id
    );
  }

  // Fill the rest of new block's data
  //

  // Optional type name
  if( EsStreamBlock::Object != id && block->supportsTypeName() )
  {
    const EsXmlAttribute& atn = node.attributeGet(
      EsStreamBlock::typeName().c_str()
    );

    if( atn )
    {
      name = atn.valueGet();
      block->typeNameSet( name );
    }
  }

  // Optional version
  if( block->supportsVersion() )
  {
    const EsXmlAttribute& aver = node.attributeGet(
      EsStreamBlock::version().c_str()
    );

    ulong version = 0;
    if( aver )
      version = aver.asUint(0);

    block->versionSet(version);
  }

  // Legacy XML field structure fixup
  EsStreamBlock::Ptr oblockFixup;

  // Optional Payload|Type
  if( block->supportsTypes() )
  {
    const EsXmlAttribute& atype = node.attributeGet(
      EsStreamBlock::type().c_str()
    );

    if( atype )
    {
      // Handle legacy type encoding (as literal value, instead of symbolic name)
      static const ulong sc_invalidType = 0xFFFF;
      ulong type = 0;

      try
      {
        type = EsEnumItemGet<EsVariantType>(
          atype.valueGet()
        ).asULong();
      }
      catch(...)
      {
        type = sc_invalidType;
      }

      if( sc_invalidType == type ) //< Conversion failed, try the Legacy one
        type = atype.asUint(sc_invalidType);

      if( sc_invalidType == type )
        EsException::Throw(
          esT("Could not read valid XML Block TYPE, stream is malformed")
        );

      if( EsVariant::VAR_EMPTY == type )
      {
        block->payloadSet(
          EsVariant::null()
        );

        // No further processing needed
        return;
      }

      // Assign POD value as Block payload
      if( type <= EsVariant::VAR_STRING )
      {
        const EsString& valstr = EsString::fromString(//toString(
          node.childValueGet()
        );

        EsVariant var = valstr;

        // Explicitly convert string variant to type, skip string type of course
        switch( type )
        {
        case EsVariant::VAR_BOOL:
          var = var.asBool();
          break;
        case EsVariant::VAR_BYTE:
          var = var.asByte();
          break;
        case EsVariant::VAR_CHAR:
          var = var.asChar();
          break;
        case EsVariant::VAR_UINT:
          var = var.asULong();
          break;
        case EsVariant::VAR_INT:
          var = var.asLong();
          break;
        case EsVariant::VAR_UINT64:
          var = var.asULLong();
          break;
        case EsVariant::VAR_INT64:
          var = var.asLLong();
          break;
        case EsVariant::VAR_DOUBLE:
          var = var.asDouble();
          break;
        case EsVariant::VAR_BIN_BUFFER:
          var = var.asBinBuffer();
          break;
        }

        block->payloadSet(var);

        // Exit immediately, no further processing is needed
        return;
      }

      block->typeSet(type);
    }
    else if( EsStreamBlock::Field == id && !atype )
    {
      // Fixup legacy XML - custom streaming allowed field object contents to omit
      // object container, and skip field type attribute
      //
      block->typeSet( EsVariant::VAR_OBJECT );

      ES_ASSERT( !name.empty() );
      ES_ASSERT( block->typeNameGet() == name );

      oblockFixup = block->childAdd(
        name,
        EsStreamBlock::Object
      );
      ES_ASSERT(oblockFixup);
    }
  }

  // Optional label
  if( block->isField() || block->isProperty() )
  {
    const EsXmlAttribute& albl = node.attributeGet(
      EsStreamBlock::label().c_str()
    );

    if( albl )
    {
      const EsString& valstr = EsString::fromString(//toString(
        albl.valueGet()
      );

      block->attributeAdd(
        EsStreamBlock::label(),
        valstr
      );
    }
  }

  // Parse compound node - dive into current block|oblockFixup scope
  BlockScope scope(
    *this,
    oblockFixup ? oblockFixup : block
  );

  EsXmlNode child = node.firstChildGet();
  while( child )
  {
    nodeParse(child);
    child = child.nextSiblingGet();
  }
}
//---------------------------------------------------------------------------

void EsStreamXml::generate(EsXmlDocument& xml) const
{
  EsXmlNode decl = xml.childAppend(xmlNodeDocDecl);
  ES_ASSERT(decl);
  decl.attributeAppend(esT("version")) = esT("1.0");
  decl.attributeAppend(esT("encoding")) = esT("UTF-8");
  decl.attributeAppend(esT("standalone")) = esT("yes");

  BlockScope scope(
    const_cast<EsStreamXml&>(*this),
    m_root
  );

  blockToXml(xml);
}
//---------------------------------------------------------------------------

void EsStreamXml::blockToXml(EsXmlNode& node) const
{
  ES_ASSERT(m_block);

  EsXmlNode nchild = node.childAppend();
  ES_ASSERT( nchild );

  // Fill-in element name
  nchild.nameSet(
    m_block->idNameGet().c_str()
  );

  // Fill-in element main attributes
  EsXmlAttribute attr;
  if( m_block->isItem() )
  {
    attr = nchild.attributeAppend(
      EsStreamBlock::index().c_str()
    );
    ES_ASSERT(attr);

    const EsString& valstr = m_block->attributeGet(
      EsStreamBlock::index()
    ).asString(
      EsString::StrNoFlags
    );

    attr.valueSet(
      valstr.c_str()
    );
  }
  else if( !m_block->isObject() && !m_block->nameIsEmpty() )
  {
    attr = nchild.attributeAppend(
      EsStreamBlock::name().c_str()
    );
    ES_ASSERT(attr);

    attr.valueSet(
      m_block->nameGet().c_str()
    );
  }

  // Fill the rest of attributes
  //

  // Optional version
  if( m_block->supportsVersion() )
  {
    unsigned int ver = m_block->versionGet();

    if( ver )
    {
      attr = nchild.attributeAppend(
        EsStreamBlock::version().c_str()
      );
      ES_ASSERT(attr);

      attr.valueSet(
        ver
      );
    }
  }

  // Optional label
  if( m_block->attributeExists( EsStreamBlock::label() ) )
  {
    attr = nchild.attributeAppend(
      EsStreamBlock::label().c_str()
    );
    ES_ASSERT(attr);

    const EsString& valstr = m_block->attributeGet(
      EsStreamBlock::label()
    ).asString(
      EsString::StrNoFlags
    );

    attr.valueSet(
      valstr.c_str()
    );
  }

  // Fill in the typename
  if( m_block->supportsTypeName() &&
      (m_block->isObject() ||
       m_block->attributeExists( EsStreamBlock::typeName())
      )
  )
  {
    attr = nchild.attributeAppend(
      EsStreamBlock::typeName().c_str()
    );
    ES_ASSERT(attr);

    attr.valueSet(
      m_block->typeNameGet().c_str()
    );
  }

  // Fill in the payload|type
  if( m_block->haveType() )
  {
    attr = nchild.attributeAppend(
      EsStreamBlock::type().c_str()
    );
    ES_ASSERT(attr);

    ulong type = m_block->typeGet();
    attr.valueSet(
      EsEnumSymbolGet<EsVariantType>( type ).c_str()
    );

    if( m_block->isPOD() )
    {
      const EsVariant& val = m_block->payloadGet();

      if( !val.isEmpty() )
      {
        EsXmlNode pcdata = nchild.childAppend( xmlNodePcdata );
        ES_ASSERT( pcdata );

        const EsString& valstr = m_block->payloadGet().asString(
          EsString::StrNoFlags
        );

        pcdata.valueSet(
          valstr.c_str()
        );
      }

      // No further processing needed
      return;
    }
  }

  // Add children content to the element
  EsStreamBlock::Ptr child = m_block->firstChildGet();
  while( child )
  {
    BlockScope scope(
      const_cast<EsStreamXml&>(*this),
      child
    );
    blockToXml(nchild);

    child = child->nextSiblingGet();
  }
}
//---------------------------------------------------------------------------
