#include "escorepch.h"
#pragma hdrstop

#include "EsStringIndexedMap.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

EsStringIndexedMap::
EsStringAssocContainerNodeT::EsStringAssocContainerNodeT(const EsString& name, ulong idx, const EsVariant& payload/* = EsVariant::null()*/) ES_NOTHROW :
m_name(name),
m_idx(idx),
m_payload(payload)
{
  m_name.hashGet();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsStringIndexedMap::EsStringIndexedMap( EsStringIndexedMap::ContainerInterlocked interlocked /*=ContainerUsesInterlock*/) ES_NOTHROW
{
  if( ContainerUsesInterlock == interlocked )
    m_cs.reset( new EsCriticalSection );
}
//---------------------------------------------------------------------------

EsStringIndexedMap::EsStringIndexedMap(EsStringIndexedMap::ContainerInterlocked interlocked, const EsString& containerName /*= EsString::null()*/ ) ES_NOTHROW :
m_containerName(containerName)
{
  if( ContainerUsesInterlock == interlocked )
    m_cs.reset( new EsCriticalSection );
}
//---------------------------------------------------------------------------

EsStringIndexedMap::EsStringIndexedMap(const EsStringIndexedMap& src, EsStringIndexedMap::ContainerInterlocked interlocked)
{
  if( ContainerUsesInterlock == interlocked )
    m_cs.reset( new EsCriticalSection );

  copyFrom(src);
}
//---------------------------------------------------------------------------

EsStringIndexedMap::EsStringIndexedMap(const EsStringIndexedMap& src)
{
  m_cs.reset( new EsCriticalSection ); //< Create interlocked by default
  copyFrom(src);
}
//---------------------------------------------------------------------------

EsStringIndexedMap::~EsStringIndexedMap() ES_NOTHROW
{
  EsCriticalSectionPtrLocker lock(m_cs);

  // Do not allow anything to escape from dtor
  try
  {
    m_m.clear();
    m_v.clear();
  }
  catch(...)
  {}
}
//---------------------------------------------------------------------------

// internal helpers
//
ulong EsStringIndexedMap::internalInsert(const EsString& name, const EsVariant& payload, bool doThrow)
{
  ulong newIdx = static_cast<ulong>(m_v.size());
  EsStringAssocContainerNodePtrT pnode(
    new EsStringAssocContainerNodeT(
      name,
      newIdx,
      payload
    )
  );
  ES_ASSERT(pnode);

  if( !name.empty() )
  {
    const std::pair<EsStringAssocMapT::iterator, bool>& result =
      m_m.insert(
        EsStringAssocMapT::value_type(
          name.hashGet(),
          pnode
        )
      );

    if( result.second )
    {
      m_v.push_back(pnode);
      return newIdx;
    }
    else if( doThrow )
    {
      if(m_containerName.empty())
        EsException::Throw(
          esT("Item '%s' already exists"),
          name
        );
      else
        EsException::Throw(
          esT("Item '%s' already exists in '%s'"),
          name,
          m_containerName
        );
    }
  }
  else
  {
    // handle unnamed item insertion as well
    m_v.push_back(pnode);
    return newIdx;
  }

  return npos;
}
//---------------------------------------------------------------------------

void EsStringIndexedMap::copyFrom(const EsStringIndexedMap& src)
{
  EsCriticalSectionPtrLocker lock(m_cs);
  EsCriticalSectionPtrLocker otherLock(src.m_cs);
  m_v.clear();
  m_m.clear();
  m_v.reserve(src.m_v.size());
  for(size_t idx = 0; idx < src.m_v.size(); ++idx)
  {
    const EsStringAssocContainerNodePtrT& pnode = src.m_v[idx];
    ES_ASSERT(pnode);
    internalInsert(
      pnode->m_name,
      pnode->m_payload,
      false
    );
  }
}
//---------------------------------------------------------------------------

// indexed map API
//
bool EsStringIndexedMap::isEmpty() const ES_NOTHROW
{
  EsCriticalSectionPtrLocker lock(m_cs);
  return m_v.empty();
}
//---------------------------------------------------------------------------

ulong EsStringIndexedMap::countGet() const ES_NOTHROW
{
  EsCriticalSectionPtrLocker lock(m_cs);
  return static_cast<ulong>(m_v.size());
}
//---------------------------------------------------------------------------

ulong EsStringIndexedMap::internalFind(const EsString& name) const ES_NOTHROW
{
  const EsStringAssocMapT::const_iterator& cit = m_m.find( name.hashGet() );

  if( cit != m_m.end() )
    return (*cit).second->m_idx;

  return npos;
}
//---------------------------------------------------------------------------

ulong EsStringIndexedMap::internalFind(const EsByteString& name, ulong len) const ES_NOTHROW
{
  // NB! We do not support any UNICODE encodings in name here, so any call to find on non-single byte string
  // would not find anything
  //
  const EsString& namestr = EsString::fromAscii(
    name.c_str(),
    len
  );

  const EsStringAssocMapT::const_iterator& cit = m_m.find( namestr.hashGet() );

  if( cit != m_m.end() )
    return (*cit).second->m_idx;

  return npos;
}
//---------------------------------------------------------------------------

ulong EsStringIndexedMap::itemFind(const EsString& name) const ES_NOTHROW
{
  EsCriticalSectionPtrLocker lock(m_cs);
  return internalFind(name);
}
//---------------------------------------------------------------------------

// The same as above, but for byte strings
ulong EsStringIndexedMap::itemFind(const EsByteString& name, ulong len) const ES_NOTHROW
{
  EsCriticalSectionPtrLocker lock(m_cs);
  return internalFind(name, len);
}
//---------------------------------------------------------------------------

// insert named item with payload, return new item index, or
// generate 'item already exists' exception
ulong EsStringIndexedMap::itemAdd(const EsString& name, const EsVariant& payload /*= EsVariant::s_null*/, bool strict /*= true*/)
{
  EsCriticalSectionPtrLocker lock(m_cs);
  ulong result = internalInsert(name, payload, strict);
  if( EsStringIndexedMap::npos == result ) //< Item already exists in non-strict mode
  {
    result = internalFind(name);
    EsNumericCheck::checkRangeUInteger(
      0,
      static_cast<ulong>(m_v.size()),
      static_cast<ulong>(result)
    );

    m_v[result]->m_payload = payload;
  }

  return result;
}
//---------------------------------------------------------------------------

// item deletion by name|index. NB! deletion performance is poor
void EsStringIndexedMap::internalDelete(ulong idx)
{
  EsStringAssocContainerNodePtrT node = m_v[idx];
  m_v.erase(m_v.begin()+idx);
  m_m.erase(node->m_name.hashGet());
  // update all node indexes after the erased one
  for(ulong _idx = idx; _idx < m_v.size(); ++_idx )
    m_v[_idx]->m_idx = _idx;
}
//---------------------------------------------------------------------------

void EsStringIndexedMap::itemDelete(ulong idx)
{
  EsCriticalSectionPtrLocker lock(m_cs);
  EsNumericCheck::checkRangeUInteger(
    0,
    static_cast<ulong>(m_v.size()),
    static_cast<ulong>(idx)
  );
  internalDelete(idx);
}
//---------------------------------------------------------------------------

void EsStringIndexedMap::throwItemDoesNotExist(const EsString& name) const
{
  if( m_containerName.empty() )
    EsException::Throw(
      esT("Item '%s' does not exist"),
      name
    );
  else
    EsException::Throw(
      esT("Item '%s' does not exist in '%s'"),
      name,
      m_containerName
    );
}
//---------------------------------------------------------------------------

void EsStringIndexedMap::itemDelete(const EsString& name, bool doThrow /*= false*/)
{
  EsCriticalSectionPtrLocker lock(m_cs);
  ulong idx = internalFind(name);
  if(npos != idx )
    internalDelete(idx);
  else if( doThrow )
    throwItemDoesNotExist(name);
}
//---------------------------------------------------------------------------

void EsStringIndexedMap::clear()
{
  EsCriticalSectionPtrLocker lock(m_cs);
  m_m.clear();
  m_v.clear();
}
//---------------------------------------------------------------------------

EsString::Array EsStringIndexedMap::namesGet() const ES_NOTHROW
{
  EsCriticalSectionPtrLocker lock(m_cs);
  EsString::Array result;
  result.reserve(m_v.size());
  for(size_t idx = 0; idx < m_v.size(); ++idx)
  {
    const EsString& name = m_v[idx]->m_name;
    if(name.empty()) // do not return unnamed items
      continue;
    result.push_back( name );
  }
  return result;
}
//---------------------------------------------------------------------------

bool EsStringIndexedMap::itemExists(const EsString& name) const ES_NOTHROW
{
  return itemFind(name) != npos;
}
//---------------------------------------------------------------------------

bool EsStringIndexedMap::itemExists(const EsByteString& name, ulong len) const ES_NOTHROW
{
  return itemFind(name, len) != npos;
}
//---------------------------------------------------------------------------

// operators
//
const EsString& EsStringIndexedMap::nameGet(ulong idx) const
{
  EsCriticalSectionPtrLocker lock(m_cs);
  EsNumericCheck::checkRangeUInteger(
    0,
    static_cast<ulong>(m_v.size()),
    static_cast<ulong>(idx)
  );
  return m_v[idx]->m_name;
}
//---------------------------------------------------------------------------

// named payload access
const EsVariant& EsStringIndexedMap::valueGet(const EsString& name) const
{
  EsCriticalSectionPtrLocker lock(m_cs);
  const EsStringAssocMapT::const_iterator& cit = m_m.find( name.hashGet() );

  if( cit != m_m.end() )
    return (*cit).second->m_payload;
  else
    throwItemDoesNotExist(name);

  return EsVariant::null();
}
//---------------------------------------------------------------------------

void EsStringIndexedMap::valueSet(const EsString& name, const EsVariant& val)
{
  EsCriticalSectionPtrLocker lock(m_cs);
  const EsStringAssocMapT::const_iterator& cit = m_m.find( name.hashGet() );

  if( cit != m_m.end() )
    (*cit).second->m_payload = val;
  else
  {
    if( m_containerName.empty() )
      EsException::Throw(
        esT("Item '%s' does not exist"),
        name
      );
    else
      EsException::Throw(
        esT("Item '%s' does not exist in '%s'"),
        name,
        m_containerName
      );
  }
}
//---------------------------------------------------------------------------

// indexed payload access
const EsVariant& EsStringIndexedMap::valueGet(ulong idx) const
{
  EsCriticalSectionPtrLocker lock(m_cs);
  EsNumericCheck::checkRangeUInteger(
    0,
    static_cast<ulong>(m_v.size()),
    static_cast<ulong>(idx)
  );
  return m_v[idx]->m_payload;
}
//---------------------------------------------------------------------------

void EsStringIndexedMap::valueSet(ulong idx, const EsVariant& val)
{
  EsCriticalSectionPtrLocker lock(m_cs);
  EsNumericCheck::checkRangeUInteger(
    0,
    static_cast<ulong>(m_v.size()),
    static_cast<ulong>(idx)
  );
  m_v[idx]->m_payload = val;
}
//---------------------------------------------------------------------------

EsStringIndexedMap& EsStringIndexedMap::operator=(const EsStringIndexedMap& src)
{
  copyFrom(src);
  return *this;
}
//---------------------------------------------------------------------------

// equality|inequality comparison. NB! comparison is performed on item names only,
// payload content is not counted for
static inline bool EsStringAssocContainerNodePtrEqual(
  const EsStringIndexedMap::EsStringAssocContainerNodePtrT& _1,
  const EsStringIndexedMap::EsStringAssocContainerNodePtrT& _2
) ES_NOTHROW
{
  ES_ASSERT(_1);
  ES_ASSERT(_2);
  return _1->nameGet() == _2->nameGet();
}
//---------------------------------------------------------------------------

bool EsStringIndexedMap::operator==(const EsStringIndexedMap& src) const ES_NOTHROW
{
  EsCriticalSectionPtrLocker lock(m_cs);
  EsCriticalSectionPtrLocker otherLock(src.m_cs);

  if(m_v.empty() && src.m_v.empty())
    return true;
  else if(m_v.size() == src.m_v.size())
    return std::equal(
      m_v.begin(),
      m_v.end(),
      src.m_v.begin(),
      EsStringAssocContainerNodePtrEqual
    );

  return false;
}
//---------------------------------------------------------------------------

bool EsStringIndexedMap::operator!=(const EsStringIndexedMap& src) const ES_NOTHROW
{
  return !this->operator==(src);
}
//---------------------------------------------------------------------------
