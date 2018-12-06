#include "escorepch.h"
#pragma hdrstop

#include "EsAssocContainer.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

EsAssocContainerNode::EsAssocContainerNode(EsAssocContainer& owner, const EsVariant& key, const EsVariant& val) :
m_owner(owner),
m_key(key),
m_val(val)
{
}

EsAssocContainerNode::Ptr EsAssocContainerNode::create(EsAssocContainer& owner, const EsVariant& key, const EsVariant& val)
{
  return EsAssocContainerNode::Ptr(
    new EsAssocContainerNode(
      owner, 
      key, 
      val
    )
  );
}

// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsAssocContainer, NO_CLASS_DESCR)
  // reflected properties
  //
  ES_DECL_PROP_INFO_RO(EsAssocContainer, empty, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsAssocContainer, count, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)  
  ES_DECL_PROP_INFO_RO(EsAssocContainer, keys, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsAssocContainer, values, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  // reflected methods
  //
  ES_DECL_REFLECTED_METHOD_INFO_STD( EsAssocContainer, clone, EsBaseIntfPtr_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, isEmpty, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, clear, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, allKeysGet, EsVariant_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, allValuesGet, EsVariant_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, keyChange, void_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, valueGet, cr_EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, valueGetDef, cr_EsVariant_CallConst_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, indexGet, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, valueSet, void_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, newValueSet, void_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, keyExists, bool_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, valueDelete, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  // indexed access api
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, itemsClear, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, itemDelete, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, countGet, ulong_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, itemGet, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, itemSet, void_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsAssocContainer, EsAssocContainerIntf, itemAppend, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  // reflected class services
  //
  ES_DECL_REFLECTED_CTOR_INFO(EsAssocContainer, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(EsAssocContainer, EsBaseIntfPtr_ClassCall_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsAssocContainer::EsAssocContainer(EsBaseIntf* owner) :
m_owner(owner, false, false)
{
  // initialize properties to their default values
  ES_REFLECTED_PROPERTIES_RESET;
}
//---------------------------------------------------------------------------

EsAssocContainerIntf::Ptr EsAssocContainer::create(EsBaseIntf* owner /*= nullptr*/)
{
  std::unique_ptr<EsAssocContainer> tmp(new EsAssocContainer(owner));
  tmp->m_dynamic = true;

  return tmp.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

EsAssocContainer::~EsAssocContainer()
{
  m_indexed.clear();
  m_mapped.clear();
}
//---------------------------------------------------------------------------

bool EsAssocContainer::get_empty() const
{
  return isEmpty();
}
//---------------------------------------------------------------------------

ulong EsAssocContainer::get_count() const
{
  return static_cast<ulong>(m_indexed.size());
}
//---------------------------------------------------------------------------

EsVariant EsAssocContainer::get_keys() const
{
  return allKeysGet();
}
//---------------------------------------------------------------------------

EsVariant EsAssocContainer::get_values() const
{
  return allValuesGet();
}
//---------------------------------------------------------------------------

bool EsAssocContainer::isEmpty() const
{
  return m_indexed.empty();
}
//---------------------------------------------------------------------------

void EsAssocContainer::clear()
{
  doClear();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsAssocContainer::clone() const
{
  EsStreamIntf::Ptr stream = EsStreamBinary::create(
    static_cast<ulong>(EsStreamFlag::Read)|
    static_cast<ulong>(EsStreamFlag::Write),
    0,
    EsVariant::null(),
    m_owner
  );
  ES_ASSERT(stream);

  stream->objectWrite(
    const_cast<EsAssocContainer*>(this)->asBaseIntfPtrDirectWeak()
  );

  EsAssocContainerIntf::Ptr result = create(
    m_owner.get()
  );

  stream->rewind();
  stream->rootObjectTypeEntryLocate(
    result->typeNameGet(),
    true
  );
  stream->objectRead(
    result
  );

  return result;
}
//---------------------------------------------------------------------------

EsVariant EsAssocContainer::allKeysGet() const
{
  EsVariant::Array result;
  result.reserve(m_indexed.size());

  for(size_t idx = 0; idx < m_indexed.size(); ++idx)
    result.push_back( m_indexed[idx]->keyGet() );

  return result;
}
//---------------------------------------------------------------------------

EsVariant EsAssocContainer::allValuesGet() const
{
  EsVariant::Array result;
  result.reserve(m_indexed.size());

  for(size_t idx = 0; idx < m_indexed.size(); ++idx)
    result.push_back( m_indexed[idx]->valGet() );

  return result;
}
//---------------------------------------------------------------------------

void EsAssocContainer::throwKeyIsNotFound(const EsVariant& key)
{
  EsException::Throw(
    esT("Item with key '%s' could not be found"),
    key.asString()
  );
}
//---------------------------------------------------------------------------

EsAssocContainer::Mapped::iterator EsAssocContainer::checkKeyExistsAndNodeGet(const EsVariant& key, bool doThrow)
{
  Mapped::iterator it = m_mapped.find(key);

  if(m_mapped.end() == it && doThrow)
    throwKeyIsNotFound(key);

  return it;
}
//---------------------------------------------------------------------------

EsAssocContainer::Mapped::const_iterator EsAssocContainer::checkKeyExistsAndNodeGet(const EsVariant& key, bool doThrow) const
{
  Mapped::const_iterator cit = m_mapped.find(key);

  if(m_mapped.end() == cit && doThrow)
    throwKeyIsNotFound(key);

  return cit;
}
//---------------------------------------------------------------------------

void EsAssocContainer::keyChange(cr_EsVariant oldKey, cr_EsVariant newKey)
{
  checkKeyDoesNotExist(newKey);
  keyCheck(newKey);
  Mapped::iterator oldIt = checkKeyExistsAndNodeGet(oldKey, true);
  ulong idx = (*oldIt).second;
  EsAssocContainerNode::Ptr node = m_indexed[idx];
  m_mapped.erase(oldIt);
  node->keySet(newKey);
  m_mapped[newKey] = idx;
}
//---------------------------------------------------------------------------

const EsVariant& EsAssocContainer::valueGet(cr_EsVariant key) const
{
  keyCheck(key);
  Mapped::const_iterator cit = checkKeyExistsAndNodeGet(key, true);
  return m_indexed[(*cit).second]->valGet();
}
//---------------------------------------------------------------------------

const EsVariant& EsAssocContainer::valueGetDef(cr_EsVariant key, cr_EsVariant def) const
{
  keyCheck(key);
  Mapped::const_iterator cit = checkKeyExistsAndNodeGet(key, false);
  if(m_mapped.end() != cit)
    return m_indexed[(*cit).second]->valGet();

  return def;
}
//---------------------------------------------------------------------------

EsVariant EsAssocContainer::indexGet(cr_EsVariant key) const
{
  keyCheck(key);

  Mapped::const_iterator cit = checkKeyExistsAndNodeGet(key, false);
  if(m_mapped.end() != cit)
    return static_cast<ullong>((*cit).second);

  return EsVariant::null();
}
//---------------------------------------------------------------------------

void EsAssocContainer::valueSet(cr_EsVariant key, cr_EsVariant val)
{
  keyCheck(key);
  Mapped::iterator it = checkKeyExistsAndNodeGet(key, true);
  m_indexed[(*it).second]->valGet() = val;
}
//---------------------------------------------------------------------------

void EsAssocContainer::checkKeyDoesNotExist(const EsVariant& key)
{
  Mapped::const_iterator cit = checkKeyExistsAndNodeGet(key, false);
  if( m_mapped.end() != cit )
    EsException::Throw(
      esT("Item with key '%s' already exists"),
      key.asString()
    );
}
//---------------------------------------------------------------------------

void EsAssocContainer::newValueSet(cr_EsVariant key, cr_EsVariant val)
{
  keyCheck(key);
  checkKeyDoesNotExist(key);
  valCheck(key, val);

  EsAssocContainerNode::Ptr node = EsAssocContainerNode::create(*this, key, val);
  ES_ASSERT(node);
  m_indexed.push_back(node);
  m_mapped[key] = static_cast<ulong>(m_indexed.size()-1);
}
//---------------------------------------------------------------------------

bool EsAssocContainer::keyExists(cr_EsVariant key) const
{
  keyCheck(key);
  return m_mapped.end() != checkKeyExistsAndNodeGet(key, false);
}
//---------------------------------------------------------------------------

void EsAssocContainer::internalDelete(Mapped::iterator it, ulong idx)
{
  ES_ASSERT( m_mapped.end() != it );

  m_mapped.erase(it);
  m_indexed.erase(m_indexed.begin()+idx);
  // update indexes in mapped container for all nodes idx < size
  while(idx < m_indexed.size())
  {
    EsAssocContainerNode::Ptr node = m_indexed[idx];
    ES_ASSERT(node);

    const EsVariant& key = node->keyGet();
    it = m_mapped.find(key);
    ES_ASSERT( m_mapped.end() != it );
    (*it).second = idx++;
  }
}
//---------------------------------------------------------------------------

void EsAssocContainer::valueDelete(cr_EsVariant key)
{
  keyCheck(key);
  Mapped::iterator it = checkKeyExistsAndNodeGet(key, true);
  ES_ASSERT( m_mapped.end() != it );

  ulong idx = (*it).second;
  internalDelete(it, idx);
}
//---------------------------------------------------------------------------

// indexed access api
void EsAssocContainer::itemsClear()
{
  doClear();
}
//---------------------------------------------------------------------------

void EsAssocContainer::indexCheck(ulong idx) const
{
  if( idx >= m_indexed.size() )
    EsException::Throw(esT("Associative container item index %d out of bounds"), idx);
}
//---------------------------------------------------------------------------

void EsAssocContainer::itemDelete(const EsVariant& idx)
{
  if(idx.isNumeric())
  {
    ulong index = idx.asULong();
    indexCheck(index);
    EsAssocContainerNode::Ptr node = m_indexed[index];
    ES_ASSERT(node);
    Mapped::iterator it = checkKeyExistsAndNodeGet(node->keyGet(), false);

    internalDelete(it, index);
  }
  else
  {
    valueDelete(idx);
  }
}
//---------------------------------------------------------------------------

ulong EsAssocContainer::countGet() const
{
  return static_cast<ulong>(m_indexed.size());
}
//---------------------------------------------------------------------------

// itemGet returns key-value pair as VAR_VARIANT_COLLECTION
EsVariant EsAssocContainer::itemGet(cr_EsVariant idx) const
{
  EsVariant::Array result;
  if(idx.isNumeric())
  {
    ulong index = idx.asULong();
    indexCheck(index);
    EsAssocContainerNode::Ptr node = m_indexed[index];

    result.push_back(node->keyGet());
    result.push_back(node->valGet());
  }
  else
  {
    result.push_back(idx);
    result.push_back(
      valueGet(idx)
    );
  }

  return result;
}
//---------------------------------------------------------------------------

void EsAssocContainer::itemSet(cr_EsVariant idx, cr_EsVariant val)
{
  if(idx.isNumeric())
  {
    ulong index = idx.asULong();
    indexCheck(index);
    m_indexed[index]->valGet() = val;
  }
  else
    valueSet(
      idx,
      val
    );
}
//---------------------------------------------------------------------------

// itemAppend here expects key-value pair passed as VAR_VARIANT_COLLECTION in single argument
void EsAssocContainer::itemAppend(cr_EsVariant keyVal)
{
  if( !keyVal.isCollection() &&
      2 != keyVal.countGet() )
    EsException::Throw(esT("Key-value pair is expected"));

  newValueSet(
    keyVal[0], 
    keyVal[1]
  );
}
//---------------------------------------------------------------------------

// ctors
EsBaseIntfPtr EsAssocContainer::NEW()
{
  return EsAssocContainer::create();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsAssocContainer::NEW(const EsBaseIntfPtr& owner)
{
  return EsAssocContainer::create( owner.get() );
}
//---------------------------------------------------------------------------

// overridable services
//
void EsAssocContainer::keyCheck(const EsVariant& key) const
{
  if( key.isEmpty() )
    EsException::Throw(esT("Item key must not be empty"));

  if( key.isObject() )
  {
    // check for comparison method in embedded object
    EsReflectedClassIntf::Ptr obj = key.asExistingObject();
    ES_ASSERT(obj);
    EsMethodInfoKeyT methodKey(1, EsStdNames::compare());
    if( !obj->hasMethod( methodKey ) )
      EsException::Throw(
        esT("Key object must implement '%s' service"),
        methodKey.asString()
      );

    if( obj->isKindOf(esT("EsScriptObject")) &&
        !obj->call(esT("isRoot")).asBool() )
      EsException::Throw(esT("Key object must be root"));
  }
  else if( !(key.isNumeric() || EsVariant::VAR_STRING == key.typeGet()) )
    EsException::Throw(esT("Key must be either numeric, string, or existing object"));
}
//---------------------------------------------------------------------------

void EsAssocContainer::valCheck(const EsVariant& key, const EsVariant& val) const
{
  // do nothing by default
}
//---------------------------------------------------------------------------

void EsAssocContainer::doClear()
{
  m_indexed.clear();
  m_mapped.clear();
}
//---------------------------------------------------------------------------

