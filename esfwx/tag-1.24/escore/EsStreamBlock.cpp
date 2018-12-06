#include "escorepch.h"
#pragma hdrstop

#include "EsStreamBlock.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

static const EsStringIndexedMap& idNamesLookupGet()
{
  static EsStringIndexedMap s_map;
  if( s_map.isEmpty() )
  {
    s_map.itemAdd( EsString::null() );
    s_map.itemAdd( esT("CONTEXT") );
    s_map.itemAdd( esT("FIELDS") );
    s_map.itemAdd( esT("PROPERTIES") );
    s_map.itemAdd( esT("ITEMS") );
    s_map.itemAdd( esT("OBJECT") );
    s_map.itemAdd( esT("FIELD") );
    s_map.itemAdd( esT("PROPERTY") );
    s_map.itemAdd( esT("ITEM") );
    s_map.itemAdd( esT("VALUE") );
  }

  return s_map;
};
//---------------------------------------------------------------------------

const EsString& EsStreamBlock::root()
{
  static const EsString s_root(esT("ROOT"), EsString::doHashInit);
  return s_root;
}
//---------------------------------------------------------------------------

const EsString& EsStreamBlock::name()
{
  static const EsString s_name(esT("NAME"), EsString::doHashInit);
  return s_name;
}
//---------------------------------------------------------------------------

const EsString& EsStreamBlock::version()
{
  static const EsString s_version(esT("VERSION"), EsString::doHashInit);
  return s_version;
}
//---------------------------------------------------------------------------

const EsString& EsStreamBlock::index()
{
  static const EsString s_index(esT("IDX"), EsString::doHashInit);
  return s_index;
}
//---------------------------------------------------------------------------

const EsString& EsStreamBlock::type()
{
  static const EsString s_type(esT("TYPE"), EsString::doHashInit);
  return s_type;
}
//---------------------------------------------------------------------------

const EsString& EsStreamBlock::typeName()
{
  static const EsString s_typename(esT("TYPENAME"), EsString::doHashInit);
  return s_typename;
}
//---------------------------------------------------------------------------

const EsString& EsStreamBlock::label()
{
  static const EsString s_label(esT("LABEL"), EsString::doHashInit);
  return s_label;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsStreamBlock::EsStreamBlock(EsStreamBlock* parent, ulong id, const EsString& name) :
m_id(id),
m_parent(parent),
m_first(0),
m_last(0),
m_prev(0),
m_next(0),
m_name(name)
{
  nameValidityCheck();
  idValidityCheck();

  // Handle specific block ID: Object
  // It have name == typeName, so add name to the typeName attribute
  // for the Object Blocks
  //
  if( Object == m_id )
  {
    typeNameSet( m_name );

    if( m_parent  )
    {
      if( m_parent->supportsTypes() )
        m_parent->typeSet( EsVariant::VAR_OBJECT );

      if( m_parent->supportsTypeName() )
      {
        if( !m_parent->haveTypeName() )
          m_parent->typeNameSet( m_name );
        else if( m_parent->typeNameGet() != m_name )
          EsException::Throw(
            esT("Nested Object Block type name does not match parent specs. ")
            esT("Got '%s', expected '%s'."),
            m_name,
            m_parent->typeNameGet()
          );
      }
    }
  }
}
//---------------------------------------------------------------------------

EsStreamBlock::~EsStreamBlock()
{
  // Cleanup all children at once
  reset();

  // Unlink from parent
  if( m_parent )
    m_parent->childRemove( this );
}
//---------------------------------------------------------------------------

const EsString& EsStreamBlock::idNameGet(ulong id)
{
  const EsStringIndexedMap& names = idNamesLookupGet();

  if( id >= names.countGet() )
    EsException::Throw(
      esT("Unknown block ID, %d"),
      id
    );

  return names.nameGet(id);
}
//---------------------------------------------------------------------------

ulong EsStreamBlock::idByNameGet(const EsString& name)
{
  const EsStringIndexedMap& names = idNamesLookupGet();

  ulong idx = names.itemFind(name);
  if( EsStringIndexedMap::npos == idx )
    return None;

  ES_ASSERT(idx < _IdsCnt_);
  return static_cast<ulong>(idx);
}
//---------------------------------------------------------------------------

void EsStreamBlock::nameValidityCheck() const
{
  if( !m_name.empty() )
  {
    if( !isRoot() && EsStreamBlock::root() == m_name )
      EsException::Throw(
        esT("Only Root Block may have a name '%s'"),
        EsStreamBlock::root()
      );

    if( isRoot() &&
        EsStreamBlock::root() != m_name
    )
      EsException::Throw(
        esT("Root Block must have a name '%s', got '%s'"),
        EsStreamBlock::root(),
        m_name
      );
    else if( Item == m_id ||
             Items == m_id ||
             Fields == m_id ||
             Properties == m_id
    )
      EsException::Throw(
        esT("Block '%s' may not have a name"),
        idNameGet()
      );
  }
  else
  {
    if( (!isRoot() && isContext()) ||
      Object == m_id ||
      Value == m_id ||
      Field == m_id
    )
      EsException::Throw(
        esT("Block '%s' should have a name"),
        idNameGet()
      );
  }
}
//---------------------------------------------------------------------------

static void throwBlockIdAddInBlockId(ulong id, ulong parentId)
{
  EsException::Throw(
    esT("Block '%s' may not be added to Block '%s'"),
    EsStreamBlock::idNameGet(id),
    EsStreamBlock::idNameGet(parentId)
  );
}
//---------------------------------------------------------------------------

void EsStreamBlock::idValidityCheck() const
{
  ulong parentId = None;
  if( m_parent )
    parentId = m_parent->idGet();

  switch( m_id )
  {
  case Context:
    if( parentId > None && Context != parentId )
      throwBlockIdAddInBlockId(m_id, parentId);
    break;
  case Item:
    if( Items != parentId )
      throwBlockIdAddInBlockId(m_id, parentId);
    break;
  case Value:
    if( parentId > None && !(Context == parentId || Object == parentId) )
      throwBlockIdAddInBlockId(m_id, parentId);
    break;
  case Object:
    break;
  case Field:
    if( Fields != parentId )
      throwBlockIdAddInBlockId(m_id, parentId);
    break;
  case Property:
    if( Properties != parentId )
      throwBlockIdAddInBlockId(m_id, parentId);
    break;
  case Items:
    if( parentId > None && Object > parentId )
      throwBlockIdAddInBlockId(m_id, parentId);
    break;
  case Fields:
  case Properties:
    if( parentId > None && Object != parentId )
      throwBlockIdAddInBlockId(m_id, parentId);
    break;
  default:
    EsException::Throw(
      esT("Block ID is not valid")
    );
    break;
  }
}
//---------------------------------------------------------------------------

void EsStreamBlock::checkSupportsVersioning() const
{
  if( !supportsVersion() )
    EsException::Throw(
      esT("Block '%s' does not support versioning"),
      idNameGet()
    );
}
//---------------------------------------------------------------------------

ulong EsStreamBlock::versionGet() const
{
  checkSupportsVersioning();

  if( m_attrs &&
      m_attrs->itemExists( EsStreamBlock::version() )
  )
    return m_attrs->valueGet(
      EsStreamBlock::version()
    ).asULong();

  return 0;
}
//---------------------------------------------------------------------------

void EsStreamBlock::internalAttrsInit()
{
  if( !m_attrs )
    m_attrs.reset(
      new EsStringIndexedMap(esT("EsStreamBlock"))
    );
}
//---------------------------------------------------------------------------

void EsStreamBlock::versionSet(ulong version)
{
  checkSupportsVersioning();

  if( version )
  {
    internalAttrsInit();

    ES_ASSERT(m_attrs);
    m_attrs->itemAdd(
      EsStreamBlock::version(),
      version,
      false
    );
  }
  else
  {
    if( m_attrs )
      m_attrs->itemDelete( EsStreamBlock::version() );
  }
}
//---------------------------------------------------------------------------

void EsStreamBlock::checkSupportsIndexing() const
{
  if( !supportsIndexing() )
    EsException::Throw(
      esT("Block '%s' does not support indexing"),
      idNameGet()
    );
}
//---------------------------------------------------------------------------

ulong EsStreamBlock::idxGet() const
{
  checkSupportsIndexing();

  return attributeGet(
    EsStreamBlock::index()
  ).asULong();
}
//---------------------------------------------------------------------------

void EsStreamBlock::checkSupportsTypes() const
{
  if( !supportsTypes() )
    EsException::Throw(
      esT("Block '%s' does not support types"),
      idNameGet()
    );
}
//---------------------------------------------------------------------------

void EsStreamBlock::checkAttrsInitialized() const
{
  if( !m_attrs )
    EsException::Throw(
      esT("Block '%s' attributes are not initialized"),
      idNameGet()
    );
}
//---------------------------------------------------------------------------

void EsStreamBlock::checkAttrsNotEmpty() const
{
  if( !m_attrs ||
      m_attrs->isEmpty()
  )
    EsException::Throw(
      esT("Block '%s' attributes are empty"),
      idNameGet()
    );
}
//---------------------------------------------------------------------------

bool EsStreamBlock::haveType() const
{
  if( !supportsTypes() )
    return false;

  return attributeExists( type() );
}
//---------------------------------------------------------------------------

ulong EsStreamBlock::typeGet() const
{
  checkSupportsTypes();
  checkAttrsNotEmpty();

  ulong typ = m_attrs->valueGet(
    EsStreamBlock::type()
  ).asULong();

  ES_ASSERT( typ <= EsVariant::VAR_VARIANT_COLLECTION );

  return typ;
}
//---------------------------------------------------------------------------

void EsStreamBlock::typeSet(ulong type)
{
  checkSupportsTypes();

  ES_ASSERT(type <= EsVariant::VAR_VARIANT_COLLECTION);

  ulong thisType = static_cast<ulong>(-1);
  if( attributeExists( EsStreamBlock::type() ) )
    thisType = m_attrs->valueGet(
      EsStreamBlock::type()
    ).asULong();

  if( thisType != type )
  {
    if( type <= EsVariant::VAR_STRING )
      m_payload.reset(
        new EsVariant(
          static_cast<EsVariant::Type>(type)
        )
      );
    else
      m_payload.reset();

    internalAttrsInit();

    ES_ASSERT(m_attrs);
    m_attrs->itemAdd(
      EsStreamBlock::type(),
      type,
      false
    );
  }
}
//---------------------------------------------------------------------------

void EsStreamBlock::checkSupportsTypeName() const
{
  if( !supportsTypeName() )
    EsException::Throw(
      esT("Block '%s' does not support type name"),
      idNameGet()
    );
}
//---------------------------------------------------------------------------

EsString EsStreamBlock::typeNameGet() const
{
  checkSupportsTypeName();

  if( Object == m_id )
    return m_name;

  checkAttrsNotEmpty();

  return m_attrs->valueGet(
    EsStreamBlock::typeName()
  ).asString();
}
//---------------------------------------------------------------------------

void EsStreamBlock::typeNameSet(const EsString& typeName)
{
  if( typeName.empty() )
    EsException::Throw(
      esT("Type name must not be an empty string")
    );

  checkSupportsTypeName();
  if( Object == m_id && typeName != m_name )
    EsException::Throw(
      esT("Type name of the object block may not be changed")
    );

  internalAttrsInit();
  m_attrs->itemAdd(
    EsStreamBlock::typeName(),
    typeName,
    false
  );
}
//---------------------------------------------------------------------------

void EsStreamBlock::childRemove(EsStreamBlock* child)
{
  ES_ASSERT(child);
  ES_ASSERT(child->m_parent == this);

  EsStreamBlock* prev = child->m_prev;
  EsStreamBlock* next = child->m_next;
  if( prev )
  {
    ES_ASSERT(prev->m_next == child);
    prev->m_next = next;
  }

  if( next )
  {
    ES_ASSERT(child->m_next->m_prev == child);
    child->m_next->m_prev = prev;
  }

  if( m_first == child )
    m_first = next;

  if( m_last == child )
    m_last = prev;

  child->m_parent = 0;
}
//---------------------------------------------------------------------------

EsStreamBlock::BlocksT::iterator EsStreamBlock::childItGet(EsString::HashT key, ulong id)
{
  if( m_children.empty() )
    return m_children.end();

  ES_ASSERT(id < _IdsCnt_);

  BlocksRangeT range =
    m_children.equal_range( key );

  if( None == id ) //< Return the first element found, if id is not specified
    return range.first;

  for( BlocksT::iterator it = range.first; it != range.second; ++it )
  {
    EsStreamBlock* child = (*it).second.get();
    ES_ASSERT(child);

    if( id == child->idGet() )
      return it;
  }

  return m_children.end();
}
//---------------------------------------------------------------------------

EsStreamBlock* EsStreamBlock::childGet(const EsString& name, ulong id /*= None*/)
{
  BlocksT::iterator it = childItGet(name.hashGet(), id);
  if( m_children.end() != it )
    return (*it).second.get();

  return 0;
}
//---------------------------------------------------------------------------

EsStreamBlock* EsStreamBlock::childAdd(const EsString& name, ulong id)
{
  return childAddBefore(0, name, id);
}
//---------------------------------------------------------------------------

void EsStreamBlock::internalChildAddBefore(EsStreamBlock* subj, const EsStreamBlock::Ptr& child, ullong key)
{
  ES_ASSERT(child);

  EsStreamBlock* p = child.get();
  if( subj )
  {
    ES_ASSERT( this == subj->m_parent );

    // Should add before the first element
    if( m_first == subj )
      m_first = p;

    EsStreamBlock* prev = subj->m_prev;
    if( prev )
    {
      prev->m_next = p;
      p->m_prev = prev;
    }

    subj->m_prev = p;
    p->m_next = subj;
  }
  else //< Otherwise, append after last child element
  {
    if( !m_first )
      m_first = p;

    EsStreamBlock* last = m_last;
    m_last = p;

    if( last )
    {
      last->m_next = p;
      p->m_prev = last;
    }
  }

  m_children.insert(
    BlocksT::value_type(
      key,
      child
    )
  );
}
//---------------------------------------------------------------------------

EsStreamBlock* EsStreamBlock::childAddBefore(EsStreamBlock* subj, const EsString& name, ulong id)
{
  ES_ASSERT(id > None);
  ES_ASSERT(id < _IdsCnt_);

  Ptr ptr(
    new EsStreamBlock(
      this,
      id,
      name
    )
  );

  internalChildAddBefore(
    subj,
    ptr,
    name.hashGet()
  );

  return ptr.get();
}
//---------------------------------------------------------------------------

EsStreamBlock* EsStreamBlock::itemAdd()
{
  return itemAdd(
    static_cast<ulong>(m_children.size())
  );
}
//---------------------------------------------------------------------------

EsStreamBlock* EsStreamBlock::itemAdd(ulong idx)
{
  Ptr ptr(
    new EsStreamBlock(
      this,
      Item,
      EsString::null()
    )
  );

  internalChildAddBefore(
    0,
    ptr,
    idx
  );

  ptr->attributeAdd(
    index(),
    idx
  );

  return ptr.get();
}
//---------------------------------------------------------------------------

EsStreamBlock* EsStreamBlock::itemGet(ulong idx)
{
  EsStreamBlock::BlocksT::iterator it = childItGet(idx, Item);
  if( m_children.end() != it )
    return (*it).second.get();

  return 0;
}
//---------------------------------------------------------------------------

const EsStringIndexedMap& EsStreamBlock::attributesGet() const
{
  checkAttrsInitialized();

  return *m_attrs;
}
//---------------------------------------------------------------------------

void EsStreamBlock::attributeAdd(const EsString& attrName, const EsVariant& attrVal)
{
  if( attrVal.typeGet() > EsVariant::VAR_STRING )
    EsException::Throw(
      esT("Attributes of type '%s' are not supported"),
      EsEnumSymbolGet<EsVariantType>(
        static_cast<ulong>(attrVal.typeGet())
      )
    );

  internalAttrsInit();

  ES_ASSERT( m_attrs );
  m_attrs->itemAdd(attrName, attrVal, false);
}
//---------------------------------------------------------------------------

bool EsStreamBlock::attributeExists(const EsString& attrName) const
{
  if( !m_attrs )
    return false;

  return m_attrs->itemExists(attrName);
}
//---------------------------------------------------------------------------

const EsVariant& EsStreamBlock::attributeGet(const EsString& attrName, bool doThrow /*= false*/) const
{
  if( doThrow )
  {
    checkAttrsInitialized();

    return m_attrs->valueGet(attrName);
  }

  if( m_attrs && m_attrs->itemExists(attrName) )
    return m_attrs->valueGet(attrName);

  return EsVariant::null();
}
//---------------------------------------------------------------------------

EsStreamBlock* EsStreamBlock::firstChildGet(ulong id) const
{
  EsStreamBlock* child = m_first;
  while( child )
  {
    if( id == child->m_id )
      return child;

    child = child->m_next;
  }

  return 0;
}
//---------------------------------------------------------------------------

EsStreamBlock* EsStreamBlock::nextSiblingGet(ulong id) const
{
  EsStreamBlock* child = m_next;

  while( child )
  {
    if( id == child->m_id )
      return child;

    child = child->m_next;
  }

  return 0;
}
//---------------------------------------------------------------------------

void EsStreamBlock::reset()
{
  ulong ver = 0;

  if( supportsVersion() )
    ver = versionGet();

  m_attrs.reset();
  m_payload.reset();
  m_children.clear();

  if( supportsVersion() )
    versionSet(ver);
}
//---------------------------------------------------------------------------

void EsStreamBlock::reset(ulong ver)
{
  m_attrs.reset();
  m_payload.reset();
  m_children.clear();

  if( ver )
    versionSet(ver);
}
//---------------------------------------------------------------------------

void EsStreamBlock::childRemove(const EsString& name, ulong id /*= None*/)
{
  BlocksT::iterator it = childItGet(
    name.hashGet(),
    id
  );

  if( m_children.end() != it )
    m_children.erase( it );
}
//---------------------------------------------------------------------------

void EsStreamBlock::checkSupportsPayload() const
{
  if( !supportsTypes() )
    EsException::Throw(
      esT("Payload is not supported for Block '%s'"),
      idNameGet()
    );
}
//---------------------------------------------------------------------------

void EsStreamBlock::attributeDelete(const EsString& attrName)
{
  if( m_attrs )
    m_attrs->itemDelete(attrName);
}
//---------------------------------------------------------------------------

void EsStreamBlock::payloadSet(const EsVariant& payload)
{
  checkSupportsPayload();

  // NB! only POD payload is supported
  if( payload.typeGet() > EsVariant::VAR_STRING )
    EsException::Throw(
      esT("Only POD types supported as stream block payload. Storage of '%s' type is attempted"),
      EsEnumSymbolGet<EsVariantType>(
        payload.typeGet()
      )
    );

  m_children.clear();
  attributeAdd(
    type(),
    static_cast<ulong>( payload.typeGet() )
  );

  if( !m_payload )
    m_payload.reset( new EsVariant(payload) );
  else
    *m_payload = payload;
}
//---------------------------------------------------------------------------

void EsStreamBlock::copyFrom(const EsStreamBlock& other)
{
  if( !isRoot() || !other.isRoot() )
    EsException::Throw(
      esT("Stream Block copyFrom call is only allowed for Root-level blocks")
    );

  reset(
    other.versionGet()
  );

  const EsStreamBlock* otherChild = other.firstChildGet();
  while(otherChild)
  {
    cloneAdd( otherChild );

    otherChild = otherChild->nextSiblingGet();
  }
}
//---------------------------------------------------------------------------

void EsStreamBlock::cloneAdd(const EsStreamBlock* src)
{
  ES_ASSERT(src);

  ulong id = src->idGet();
  const EsString& name = src->nameGet();

  Ptr ptr(
    new EsStreamBlock(
      this,
      id,
      name
    )
  );

  EsString::HashT key = (Item == id ?
    static_cast<EsString::HashT>(src->idxGet()) :
    name.hashGet()
  );

  internalChildAddBefore(
    0,
    ptr,
    key
  );

  // Copy attributes
  if( src->haveAttributes() )
  {
    ptr->internalAttrsInit();
    *(ptr->m_attrs) = src->attributesGet();
  }

  // Copy payload
  if( src->isPOD() )
  {
    ptr->m_payload.reset(
      new EsVariant(
        src->payloadGet()
      )
    );

    // No need to process further
    return;
  }

  // Append children
  const EsStreamBlock* otherChild = src->firstChildGet();
  while(otherChild)
  {
    ptr->cloneAdd( otherChild );

    otherChild = otherChild->nextSiblingGet();
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// Debug-only functionality. Dump Block contents and children to string, in aligned, readable manner
///

#ifdef ES_DEBUG

static void labelValDump(EsString& out, const EsString& lbl, const EsString& val, const EsString& pfx)
{
  out += pfx +
    EsString::format(
      esT("%10s: %s\n"),
      lbl,
      val
    );
}
//---------------------------------------------------------------------------

void EsStreamBlock::stringDump(EsString& out, ulong offs /*= 0*/) const
{
  EsString pfx(offs, esT(' '));

  labelValDump(out, esT("ID"), idNameGet(), pfx);
  if( !m_name.empty() )
    labelValDump(out, esT("Name"), m_name, pfx );

  stringDumpAttributes(out, offs);

  out += pfx + esT("(\n");

  stringDumpContents(out, offs+1);

  out += pfx + esT(")\n");
  out += pfx + esT("-----------------------------------------------\n");
}
//---------------------------------------------------------------------------

void EsStreamBlock::stringDumpAttributes(EsString& out, ulong offs) const
{
  if( m_attrs && !m_attrs->isEmpty() )
  {
    EsString pfx(offs, esT(' '));
    pfx += esT("@");

    const EsString::Array& anames = m_attrs->namesGet();
    for( size_t idx = 0; idx < anames.size(); ++idx )
    {
      const EsString& aname = anames[idx];
      const EsVariant& var = m_attrs->valueGet(aname);

      labelValDump(out, aname, var.asString(), pfx);
    }
  }
}
//---------------------------------------------------------------------------

void EsStreamBlock::stringDumpContents(EsString& out, ulong offs) const
{
  EsString pfx(offs, esT(' '));

  if( havePayload() )
  {
    ES_ASSERT( !haveChildren() );
    labelValDump(
      out,
      esT("Payload"),
      m_payload->isEmpty() ?
        EsString::null() :
        m_payload->asString(),
      pfx
    );
  }
  else
  {
    EsStreamBlock* child = firstChildGet();
    while( child )
    {
      child->stringDump(out, offs);

      child = child->nextSiblingGet();
    }
  }
}
//---------------------------------------------------------------------------

#endif
