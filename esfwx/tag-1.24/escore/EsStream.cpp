#include "escorepch.h"
#pragma hdrstop

#include "EsStream.h"

#ifdef ES_USE_STREAM_TRACE
# define ES_STREAM_TRACE ES_DEBUG_TRACE
#else
# define ES_STREAM_TRACE(...)
#endif

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// Reflected enumeration
namespace EsReflection
{
ES_DECL_ENUMERATION_INFO(EsStreamFlag, NO_CLASS_DESCR)
}
//---------------------------------------------------------------------------

// Stream scopes implementation
//
// read
EsStream::
ObjectReadScope::ObjectReadScope(EsStream& stream, const EsReflectedClassIntf::Ptr& obj):
m_stream(stream),
m_oldScope(m_stream.m_objReadScope),
m_obj(obj),
m_customReading(m_stream.doesCustomReading())
{
  m_stream.m_objReadScope = this;
  m_stream.objectReadBegin(m_obj);
}
//---------------------------------------------------------------------------

EsStream::
ObjectReadScope::~ObjectReadScope()
{
  m_stream.objectReadEnd(m_obj);
  // restore custom reading flag
  if(m_customReading)
    m_stream.m_flags |= EsStream::flagCustomRead;
  else
    m_stream.m_flags &= ~EsStream::flagCustomRead;

  // restore obj reading scope
  m_stream.m_objReadScope = m_oldScope;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsStream::
FieldsReadScope::FieldsReadScope(EsStream& stream, const EsReflectedClassIntf::Ptr& obj):
m_stream(stream),
m_obj(obj),
m_readWasStarted(false)
{
  m_readWasStarted = m_stream.fieldsReadBegin(m_obj);
}
//---------------------------------------------------------------------------

EsStream::
FieldsReadScope::~FieldsReadScope()
{
  if( m_readWasStarted )
    m_stream.fieldsReadEnd(m_obj);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsStream::
PropertiesReadScope::PropertiesReadScope(EsStream& stream, const EsReflectedClassIntf::Ptr& obj):
m_stream(stream),
m_obj(obj),
m_readWasStarted(false)
{
  m_readWasStarted = m_stream.propertiesReadBegin(m_obj);
}
//---------------------------------------------------------------------------

EsStream::
PropertiesReadScope::~PropertiesReadScope()
{
  if( m_readWasStarted )
    m_stream.propertiesReadEnd(m_obj);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsStream::
ItemsReadScope::ItemsReadScope(EsStream& stream, EsVariant& var):
m_stream(stream),
m_var(var),
m_readWasStarted(false),
m_doAppend(false),
m_cnt(0)
{
  m_readWasStarted = m_stream.itemsReadBegin(m_var, m_cnt, m_doAppend);
}
//---------------------------------------------------------------------------

EsStream::
ItemsReadScope::~ItemsReadScope()
{
  if( m_readWasStarted )
    m_stream.itemsReadEnd(m_var);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// write
EsStream::
ObjectWriteScope::ObjectWriteScope(EsStream& stream, const EsReflectedClassIntf::Ptr& obj):
m_stream(stream),
m_oldScope(m_stream.m_objWriteScope),
m_obj(obj),
m_customWriting(m_stream.doesCustomWriting())
{
  m_stream.m_objWriteScope = this;
  m_stream.objectWriteBegin(m_obj);
  m_stream.m_flags |= flagDirty;
}
//---------------------------------------------------------------------------

EsStream::
ObjectWriteScope::~ObjectWriteScope()
{
  m_stream.objectWriteEnd(m_obj);
  // restore custom writing status
  if( m_customWriting )
    m_stream.m_flags |= EsStream::flagCustomWrite;
  else
    m_stream.m_flags &= ~EsStream::flagCustomWrite;

  // restore obj write scope
  m_stream.m_objWriteScope = m_oldScope;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsStream::
FieldsWriteScope::FieldsWriteScope(EsStream& stream, const EsReflectedClassIntf::Ptr& obj):
m_stream(stream),
m_obj(obj)
{
  m_stream.fieldsWriteBegin(m_obj);
}
//---------------------------------------------------------------------------

EsStream::
FieldsWriteScope::~FieldsWriteScope()
{
  m_stream.fieldsWriteEnd(m_obj);
}
//---------------------------------------------------------------------------

EsStream::
PropertiesWriteScope::PropertiesWriteScope(EsStream& stream, const EsReflectedClassIntf::Ptr& obj):
m_stream(stream),
m_obj(obj)
{
  m_stream.propertiesWriteBegin(m_obj);
}
//---------------------------------------------------------------------------

EsStream::
PropertiesWriteScope::~PropertiesWriteScope()
{
  m_stream.propertiesWriteEnd(m_obj);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsStream::
ItemsWriteScope::ItemsWriteScope(EsStream& stream, const EsVariant& var):
m_stream(stream),
m_var(var),
m_writeWasStarted(false)
{
  m_writeWasStarted = m_stream.itemsWriteBegin(m_var);
}
//---------------------------------------------------------------------------

EsStream::
ItemsWriteScope::~ItemsWriteScope()
{
  if( m_writeWasStarted )
    m_stream.itemsWriteEnd(m_var);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsStream, NO_CLASS_DESCR)
  // reflected interface methods
  //
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, objectCreate, EsBaseIntfPtr_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, objectRead, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, objectWrite, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, isEmpty, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, rewind, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, reset, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, reset, void_Call_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, contextOpenCreate, bool_Call_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, contextOpenCreate, bool_Call_cr_EsString_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, contextOpenExisting, bool_Call_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, contextCurrentClose, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, contextRemove, void_Call_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, contextCurrentReset, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, contextCurrentReset, void_Call_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, contextCurrentNameGet, EsString_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, contextCurrentIsEmpty, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, contextCurrentVersionGet, ulong_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, contextCurrentVersionSet, void_Call_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, rootVersionGet, ulong_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, valueRead, EsVariant_Call_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, valueReadObject, void_Call_cr_EsString_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, valueWrite, void_Call_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, valueWrite, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, valueRead, EsVariant_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, valueExists, bool_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, valueRemove, void_Call_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, firstRootObjectLocate, bool_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, nextRootObjectLocate, bool_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, rootObjectTypeNameGet, EsString_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsStream, EsStreamIntf, rootObjectTypeEntryLocate, bool_Call_cr_EsString_bool, NO_METHOD_DESCR)
  // Reflected specific object component streaming, to augment custom stream writers & readers
  ES_DECL_REFLECTED_METHOD_INFO(EsStream, fieldsReadReflected, fieldsRead, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(EsStream, propertiesReadReflected, propertiesRead, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(EsStream, itemsReadReflected, itemsRead, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(EsStream, fieldsWriteReflected, fieldsWrite, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(EsStream, propertiesWriteReflected, propertiesWrite, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(EsStream, itemsWriteReflected, itemsWrite, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(EsStream, fromStreamReflected, fromStream, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(EsStream, asStringReflected, asString, EsString_CallConst, NO_METHOD_DESCR)
  // reflected properties
  //
  ES_DECL_PROP_INFO_RO(EsStream, empty, bool, esT("Stream is empty"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(   EsStream, factory, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR )
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsStream::EsStream(ulong flags, ulong version, const EsBaseIntfPtr& factory) :
m_objReadScope(nullptr),
m_objWriteScope(nullptr),
m_flags(flags),
m_ctx(nullptr),
m_block(nullptr),
m_rootObjIt(nullptr)
{
  m_dynamic = true;
  m_factory = factory;

  rootInit(version);
  internalRewind();
}
//---------------------------------------------------------------------------

/// Reflected specific object component streaming, to augment custom stream writers & readers
void EsStream::fieldsReadReflected(cr_EsBaseIntfPtr obj)
{
  fieldsRead(obj);
}
//---------------------------------------------------------------------------

void EsStream::propertiesReadReflected(cr_EsBaseIntfPtr obj)
{
  propertiesRead(obj);
}
//---------------------------------------------------------------------------

void EsStream::itemsReadReflected(cr_EsBaseIntfPtr obj)
{
  EsVariant var = obj;
  itemsRead(var);
}
//---------------------------------------------------------------------------

void EsStream::fieldsWriteReflected(cr_EsBaseIntfPtr obj)
{
  fieldsWrite(obj);
}
//---------------------------------------------------------------------------

void EsStream::propertiesWriteReflected(cr_EsBaseIntfPtr obj)
{
  propertiesWrite(obj);
}
//---------------------------------------------------------------------------

void EsStream::itemsWriteReflected(cr_EsBaseIntfPtr obj)
{
  itemsWrite(obj);
}
//---------------------------------------------------------------------------

bool EsStream::get_empty() const
{
  return isEmpty();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsStream::get_factory() const
{
  return m_factory;
}
//---------------------------------------------------------------------------

void EsStream::set_factory(const EsBaseIntfPtr& factory)
{
  factorySet(factory);
}
//---------------------------------------------------------------------------

bool EsStream::isEmpty() const
{
  return !m_root || m_root->isEmpty();
}
//---------------------------------------------------------------------------

void EsStream::internalStateReset()
{
  // Make current context the current node
  m_block = m_ctx;

  // Reset root object iterator, just in case
  m_rootObjIt = 0;
}
//---------------------------------------------------------------------------

void EsStream::rootInit(ulong version)
{
  m_root.reset(
    new EsStreamBlock(
      0,
      EsStreamBlock::Context,
      EsStreamBlock::root()
    )
  );
  m_root->versionSet(version);
  m_ctx = m_root.get();

  m_flags |= flagDirty;
}
//---------------------------------------------------------------------------

void EsStream::internalRewind()
{
  m_ctx = m_block = m_root.get();

  internalStateReset();
}
//---------------------------------------------------------------------------

void EsStream::rewind()
{
  internalRewind();
}
//---------------------------------------------------------------------------

void EsStream::reset()
{
  ES_ASSERT(m_root);
  m_root->reset();
  internalRewind();
}
//---------------------------------------------------------------------------

void EsStream::reset(ulong version)
{
  ES_ASSERT(m_root);
  m_root->reset(version);
  internalRewind();
}
//---------------------------------------------------------------------------

void EsStream::fromStream(const EsStreamIntf::Ptr& other)
{
  if( !other )
    EsException::Throw( esT("Could not copy stream from an empty object") );

  const EsStream* sobj = static_cast<const EsStream*>( other.get() );
  if( sobj == this )
    EsException::Throw( esT("Could not copy stream from itself") );

  checkStreamWriteable();

  m_root->copyFrom(
    *(sobj->m_root)
  );
}
//---------------------------------------------------------------------------

void EsStream::fromStreamReflected(const EsBaseIntf::Ptr& other)
{
  EsStreamIntf::Ptr stream = other;

  fromStream(stream);
}
//---------------------------------------------------------------------------

EsString EsStream::asStringReflected() const
{
  return asString();
}
//---------------------------------------------------------------------------

bool EsStream::contextOpenCreate(const EsString& name)
{
  return contextOpenCreate(name, 0);
}
//---------------------------------------------------------------------------

#ifdef ES_DEBUG
EsString EsStream::debugDump() const
{
  EsString out;
  m_root->stringDump(out);

  return out;
}
#endif
//---------------------------------------------------------------------------

bool EsStream::contextOpenCreate(const EsString& name, ulong newVersion)
{
  if( contextOpenExisting(name) )
    return true;
  else if( m_flags & static_cast<ulong>(EsStreamFlag::Write) )
  {
    checkContextNameNotReserved(name);

    ES_ASSERT(m_ctx);
    EsStreamBlock* newCtx = m_ctx->childAdd(
      name,
      EsStreamBlock::Context
    );

    ES_ASSERT(newCtx);

    newCtx->versionSet(newVersion);
    m_ctx = newCtx;

    internalStateReset();

    m_flags |= flagDirty;

    return true;
  }

  return false;
}
//---------------------------------------------------------------------------

bool EsStream::contextOpenExisting(const EsString& name)
{
  checkContextNameNotEmpty(name);

  ES_ASSERT(m_ctx);
  EsStreamBlock* newCtx = m_ctx->childGet(
    name,
    EsStreamBlock::Context
  );
  if( newCtx )
  {
    m_ctx = newCtx;

    internalStateReset();

    return true;
  }

  return false;
}
//---------------------------------------------------------------------------

void EsStream::contextCurrentClose()
{
  ES_ASSERT(m_ctx);
  EsStreamBlock* parent = m_ctx->parentGet();
  if( parent )
  {
    m_ctx = parent;

    internalStateReset();
  }
}
//---------------------------------------------------------------------------

void EsStream::contextRemove(const EsString& name)
{
  checkContextNameNotEmpty(name);
  checkContextNameNotReserved(name);
  checkStreamWriteable();

  ES_ASSERT(m_ctx);
  EsStreamBlock* ctx = m_ctx->childGet(
    name,
    EsStreamBlock::Context
  );

  if( ctx )
  {
    m_ctx->childRemove(ctx);

    internalStateReset();

    m_flags |= flagDirty;
  }
}
//---------------------------------------------------------------------------

void EsStream::contextCurrentReset()
{
  checkStreamWriteable();
  ES_ASSERT(m_ctx);

  m_ctx->reset();

  internalStateReset();

  m_flags |= flagDirty;
}
//---------------------------------------------------------------------------

void EsStream::contextCurrentReset(ulong newVersion)
{
  checkStreamWriteable();
  ES_ASSERT(m_ctx);

  m_ctx->reset(newVersion);

  internalStateReset();

  m_flags |= flagDirty;
}
//---------------------------------------------------------------------------

EsString EsStream::contextCurrentNameGet() const
{
  ES_ASSERT(m_ctx);
  return m_ctx->nameGet();
}
//---------------------------------------------------------------------------

bool EsStream::contextCurrentIsEmpty() const
{
  ES_ASSERT(m_ctx);
  return m_ctx->isEmpty();
}
//---------------------------------------------------------------------------

ulong EsStream::contextCurrentVersionGet() const
{
  ES_ASSERT(m_ctx);
  return m_ctx->versionGet();
}
//---------------------------------------------------------------------------

void EsStream::contextCurrentVersionSet(ulong newVersion)
{
  checkStreamWriteable();

  ES_ASSERT(m_ctx);

  m_ctx->versionSet( newVersion );
}
//---------------------------------------------------------------------------

ulong EsStream::rootVersionGet() const
{
  ES_ASSERT(m_root);
  return m_root->versionGet();
}
//---------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsStream::doObjectCreate(const EsString& typeName) const
{
  if( m_factory && m_factory->hasMethod( EsMethodInfoKeyT(1, esT("objectCreate")) ) )
    return m_factory->call(esT("objectCreate"), typeName).asExistingObject();
  else
  {
    const EsClassInfo* info = EsClassInfo::classInfoGet(typeName, true);
    ES_ASSERT(info);

    return info->classCall(EsStdNames::reflectedCtr()).asExistingObject();
  }
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsStream::objectCreate()
{
  // try to create object from the current stream position
  EsReflectedClassIntf::Ptr obj = objectCreateAtCurrentPos();
  if( obj ) // read data into created object
    objectRead(obj);

  return obj;
}
//---------------------------------------------------------------------------

void EsStream::fieldsRead(const EsReflectedClassIntf::Ptr& obj)
{
  FieldsReadScope fieldsScope(*this, obj);
  if( fieldsScope.isOk() )
  {
    // if reading script object, read fields one-by-one, because object may be
    // laid-out dynamically during reading process
    EsString name = obj->call(esT("firstFieldNameGet")).asString();
    while( !name.empty() )
    {
      EsReflectedClassIntf::Ptr fld = obj->call(esT("fieldGet"), name).asExistingObject();
      ES_ASSERT(fld);
      fieldRead(name, fld);
      name = obj->call(esT("nextFieldNameGet"), name).asString();
    }
  }
}
//---------------------------------------------------------------------------

void EsStream::propertiesRead(const EsReflectedClassIntf::Ptr& obj)
{
  PropertiesReadScope propertiesScope(*this, obj);
  if( propertiesScope.isOk() )
  {
    // otherwise walk persistent properties
    const EsStringArray& props = obj->persistentPropertyNamesGet();
    for(size_t idx = 0; idx < props.size(); ++idx)
    {
      const EsString& name = props[idx];
      propertyRead(name, obj);
    }
  }
}
//---------------------------------------------------------------------------

void EsStream::itemsRead(EsVariant& var)
{
  ItemsReadScope itemsScope(*this, var);

  // handle special case - indexed _object_ read
  ES_STREAM_TRACE(esT("itemsRead itemsScope=%s"), itemsScope.isOk() ? esT("true") : esT("false"));

  if( itemsScope.isOk() )
  {
    ulong cnt = itemsScope.countGet();

    ES_STREAM_TRACE(esT("itemsRead with items cnt=%d"), cnt);

    for(ulong idx = 0; idx < cnt; ++idx)
      itemRead(
        idx,
        var,
        itemsScope.doAppend()
      );
  }
}
//---------------------------------------------------------------------------

void EsStream::objectRead(const EsBaseIntfPtr& ptr)
{
  checkStreamReadable();

  EsReflectedClassIntf::Ptr obj = ptr;
  ES_ASSERT(obj);

  ObjectReadScope readScope(*this, obj);
  // handle custom object serialization
  if( obj->hasMethod( EsMethodInfoKeyT(1, esT("streamRead")) ) )
  {
    ES_STREAM_TRACE( esT("objectRead(%s) reading reflected object via streamRead"), obj->typeNameGet().c_str() );

    m_flags |= flagCustomRead;
    EsBaseIntf::Ptr streamPtr(asBaseIntf(), false, false);
    obj->call(esT("streamRead"), streamPtr);
  }
  else if( obj->isScripted() )
  {
    if( obj->call(esT("isPOD")).asBool() ) //< Simple types, assign inline value property
    {
      ES_STREAM_TRACE( esT("objectRead(%s) reading scripted object as POD value"), obj->typeNameGet().c_str() );

      obj->propertySet(
        EsStdNames::value(),
        valueRead()
      );
      return;
    }

    ES_STREAM_TRACE( esT("objectRead(%s) reading scripted object as fields and properties"), obj->typeNameGet().c_str() );
    fieldsRead(obj);
    propertiesRead(obj);
  }
  else
  {
    ES_STREAM_TRACE( esT("objectRead(%s) reading reflected object via properties and items"), obj->typeNameGet().c_str() );
    propertiesRead(obj);

    EsVariant var = obj;
    itemsRead(var);
  }
}
//---------------------------------------------------------------------------

void EsStream::fieldsWrite(const EsReflectedClassIntf::Ptr& obj)
{
  // if writing script object - iterate fields collection
  const EsStringArray& names = obj->propertyGet(esT("fieldNames")).asStringCollection();
  if(!names.empty())
  {
    FieldsWriteScope fieldsScope(*this, obj);

    for( size_t idx = 0; idx < names.size(); ++idx)
    {
      const EsString& name = names[idx];
      EsReflectedClassIntf::Ptr fld = obj->call(esT("fieldGet"), name).asExistingObject();
      ES_ASSERT(fld);

      fieldWrite(name, fld);
    }
  }
}
//---------------------------------------------------------------------------

void EsStream::propertiesWrite(const EsReflectedClassIntf::Ptr& obj)
{
  // otherwise walk persistent properties first
  const EsStringArray& names = obj->persistentPropertyNamesGet();
  if(!names.empty())
  {
    PropertiesWriteScope propertiesScope(*this, obj);

    for(size_t idx = 0; idx < names.size(); ++idx)
    {
      const EsString& name = names[idx];
      const EsVariant& val = obj->propertyGet(name);
      const EsString& label = obj->propertyLabelGet(name);
      propertyWrite(name, val, label);
    }
  }
}
//---------------------------------------------------------------------------

void EsStream::itemsWrite(const EsVariant& var)
{
  ItemsWriteScope itemsScope(*this, var);

  if( itemsScope.isOk() )
  {
    ulong cnt = var.countGet();
    for(ulong idx = 0; idx < cnt; ++idx)
      itemWrite(idx, var);
  }
}
//---------------------------------------------------------------------------

void EsStream::objectWrite(const EsBaseIntfPtr& ptr)
{
  checkStreamWriteable();

  EsReflectedClassIntf::Ptr obj = ptr;
  ES_ASSERT(obj);

  ObjectWriteScope writeScope(*this, obj);

  // handle custom object serialization
  if( obj->hasMethod( EsMethodInfoKeyT(1, esT("streamWrite")) ) )
  {
    m_flags |= flagCustomWrite;
    obj->call(
      esT("streamWrite"),
      asWeakReference()
    );
  }
  else if( obj->isScripted() )
  {
    if( obj->call(esT("isPOD")).asBool() ) //< Simple types, write inline value property
    {
      valueWrite(
        obj->propertyGet(
          EsStdNames::value()
        )
      );
    }
    else
    {
      fieldsWrite(obj);
      propertiesWrite(obj);
    }
  }
  else
  {
    propertiesWrite(obj);
    itemsWrite(obj);
  }
}
//---------------------------------------------------------------------------

bool EsStream::rootObjectTypeEntryLocate(const EsString& typeName, bool doThrow)
{
  if( firstRootObjectLocate() )
  {
    do
    {
      const EsString& curTypeName = rootObjectTypeNameGet();
      if( curTypeName == typeName )
        return true;

    } while( nextRootObjectLocate() );
  }

  if( doThrow )
    EsException::Throw(
      esT("Could not find Block for root object '%s' in stream"),
      typeName
    );

  return false;  //< Pacify compilers
}
//---------------------------------------------------------------------------

void EsStream::valueWrite(const EsString& name, const EsVariant& val)
{
  checkStreamWriteable();

  EsStreamBlock* block = m_objWriteScope ? m_block : m_ctx;
  ES_ASSERT(block);

  block->childRemove(
    name,
    EsStreamBlock::Value
  );

  EsStreamBlock* vnode = block->childAdd(
    name,
    EsStreamBlock::Value
  );
  ES_ASSERT(vnode);

  BlockScope scope(*this, vnode);
  valueWrite(val);

  m_flags |= flagDirty;
}
//---------------------------------------------------------------------------

EsVariant EsStream::valueRead(const EsString& name, const EsVariant& def /*= EsVariant::null()*/)
{
  checkStreamReadable();

  EsStreamBlock* block = m_objReadScope ? m_block : m_ctx;
  ES_ASSERT(block);

  ES_STREAM_TRACE( esT("valueRead(%s, def)"), name.c_str() );

  EsStreamBlock* vnode = block->childGet(
    name,
    EsStreamBlock::Value
  );

  if( vnode )
  {
    BlockScope scope(*this, vnode);
    return valueRead();
  }
  else
    return def;
}
//---------------------------------------------------------------------------

void EsStream::valueReadObject(const EsString& name, const EsBaseIntf::Ptr& obj)
{
  checkStreamReadable();

  EsStreamBlock* block = m_objReadScope ? m_block : m_ctx;
  ES_ASSERT(block);

  ES_STREAM_TRACE( esT("valueReadObject(%s, obj)"), name.c_str() );

  EsStreamBlock* vnode = block->childGet(
    name,
    EsStreamBlock::Value
  );

  if( vnode )
  {
    BlockScope scope(*this, vnode);

    if( EsVariant::VAR_OBJECT != m_block->typeGet() )
      EsException::Throw(
        esT("Value '%s' does not contain an object data"),
        name
      );

    BlockScope objScope(*this, m_block->firstChildGet());
    objectRead(obj);
  }
}
//---------------------------------------------------------------------------

bool EsStream::valueExists(const EsString& name) const
{
  ES_ASSERT(m_block);

  return 0 != m_block->childGet(
    name,
    EsStreamBlock::Value
  );
}
//---------------------------------------------------------------------------

void EsStream::valueRemove(const EsString& name)
{
  checkStreamWriteable();

  ES_ASSERT(m_block);
  m_block->childRemove(
    name,
    EsStreamBlock::Value
  );

  m_flags |= flagDirty;
}
//---------------------------------------------------------------------------

bool EsStream::firstRootObjectLocate()
{
  ES_ASSERT(m_ctx);
  m_rootObjIt = m_ctx->firstChildGet(
    EsStreamBlock::Object
  );
  m_block = m_rootObjIt;

  return 0 != m_rootObjIt;
}
//---------------------------------------------------------------------------

bool EsStream::nextRootObjectLocate()
{
  if( m_rootObjIt )
  {
    m_rootObjIt = m_rootObjIt->nextSiblingGet(
      EsStreamBlock::Object
    );
    m_block = m_rootObjIt;
  }

  return 0 != m_rootObjIt;
}
//---------------------------------------------------------------------------

EsString EsStream::objectBlockTypeNameGet(EsStreamBlock* bobj) const
{
  if( !bobj )
    return EsString::null();

  if( !bobj->isObject() )
    EsException::Throw(
      esT("Object block is expected")
    );

  return bobj->typeNameGet();
}
//---------------------------------------------------------------------------

EsString EsStream::rootObjectTypeNameGet() const
{
  return objectBlockTypeNameGet(m_rootObjIt);
}
//---------------------------------------------------------------------------

// Stream implementation-specific internal services
//
// object component writers
//
// root object clause
void EsStream::objectWriteBegin(const EsReflectedClassIntf::Ptr& obj)
{
  // Check if we already have an object node of the same type at the current context
  // and eliminate existing node
  ES_ASSERT(m_block);

  const EsString& typeName = obj->typeNameGet();

  m_block->childRemove(
    typeName,
    EsStreamBlock::Object
  );

  // Set type|typename of the node, owning the object node
  if( !m_block->isContext() )
  {
    m_block->typeSet( EsVariant::VAR_OBJECT );
    m_block->typeNameSet( typeName );
  }

  // Append object node
  EsStreamBlock* bobj = m_block->childAdd(
    typeName,
    EsStreamBlock::Object
  );
  ES_ASSERT(bobj);

  // Handle object auto-versioning
  if( obj->hasAttribute( EsStdNames::version() ) )
    bobj->versionSet(
      obj->attributeGet(
        EsStdNames::version()
      ).asULong()
    );

  m_block = bobj;
}
//---------------------------------------------------------------------------

void EsStream::objectWriteEnd(const EsReflectedClassIntf::Ptr& obj)
{
  ES_ASSERT(m_block);
  EsStreamBlock* parent = m_block->parentGet();

  ES_ASSERT(parent);
  m_block = parent;
}
//---------------------------------------------------------------------------

void EsStream::propertiesWriteBegin(const EsReflectedClassIntf::Ptr& obj)
{
  ES_ASSERT(m_block);
  EsStreamBlock* props = m_block->childAdd(
    EsString::null(),
    EsStreamBlock::Properties
  );

  ES_ASSERT(props);
  m_block = props;
}
//---------------------------------------------------------------------------

void EsStream::propertiesWriteEnd(const EsReflectedClassIntf::Ptr& obj)
{
  ES_ASSERT(m_block);
  EsStreamBlock* parent = m_block->parentGet();

  ES_ASSERT(parent);
  ES_ASSERT(EsStreamBlock::Object == parent->idGet());
  m_block = parent;
}
//---------------------------------------------------------------------------

void EsStream::valueWrite(const EsVariant& val)
{
  ES_ASSERT(m_block);
  ulong type = val.typeGet();

  if( EsVariant::VAR_POINTER == type )
    EsException::Throw(
      esT("Streaming of values of type 'Pointer', is not supported")
    );

  if( type <= EsVariant::VAR_STRING )
    m_block->payloadSet(val);
  else if( val.isCollection() )
  {
    if( !m_block->isContext() )
      m_block->typeSet(type);

    itemsWrite(val);
  }
  else
  {
    ES_ASSERT(EsVariant::VAR_OBJECT == type);

    if( val.isEmpty() )
      m_block->typeSet(EsVariant::VAR_EMPTY);
    else
      objectWrite( val.asExistingObject() );
  }
}
//---------------------------------------------------------------------------

void EsStream::propertyWrite(const EsString& name, const EsVariant& val, const EsString& label)
{
  ES_ASSERT(m_block);

  EsStreamBlock* prop = m_block->childAdd(
    name,
    EsStreamBlock::Property
  );
  ES_ASSERT(prop);

  if( !label.empty() )
    prop->attributeAdd(
      EsStreamBlock::label(),
      label
    );

  BlockScope scope(*this, prop);
  valueWrite(val);
}
//---------------------------------------------------------------------------

bool EsStream::itemsWriteBegin(const EsVariant& obj)
{
  if( !obj.isCollection() )
  {
    if( obj.isObject() )
    {
      EsReflectedClassIntf::Ptr pobj = obj.asExistingObject();
      if( !pobj->isIndexed() )
        return false;
    }
    else
      return false;
  }

  ES_ASSERT(m_block);

  EsStreamBlock* items = m_block->childAdd(
    EsString::null(),
    EsStreamBlock::Items
  );

  ES_ASSERT(items);
  m_block = items;

  return true;
}
//---------------------------------------------------------------------------

void EsStream::itemsWriteEnd(const EsVariant& obj)
{
  ES_ASSERT(m_block);
  EsStreamBlock* parent = m_block->parentGet();

  ES_ASSERT(parent);
  m_block = parent;
}
//---------------------------------------------------------------------------

void EsStream::itemWrite(ulong idx, const EsVariant& obj)
{
  const EsVariant& val = obj.itemGet(idx);

  EsStreamBlock* item = m_block->itemAdd(idx);
  ES_ASSERT(item);

  BlockScope itemScope(*this, item);
  valueWrite(val);
}
//---------------------------------------------------------------------------

void EsStream::fieldsWriteBegin(const EsReflectedClassIntf::Ptr& obj)
{
  ES_ASSERT(obj);
  ES_ASSERT(m_block);

  EsStreamBlock* fields = m_block->childAdd(
    EsString::null(),
    EsStreamBlock::Fields
  );
  ES_ASSERT(fields);

  m_block = fields;
}
//---------------------------------------------------------------------------

void EsStream::fieldsWriteEnd(const EsReflectedClassIntf::Ptr& obj)
{
  ES_ASSERT(m_block);
  EsStreamBlock* parent = m_block->parentGet();

  ES_ASSERT(parent);
  ES_ASSERT(EsStreamBlock::Object == parent->idGet());

  m_block = parent;
}
//---------------------------------------------------------------------------

void EsStream::fieldWrite(const EsString& name, const EsReflectedClassIntf::Ptr& fld)
{
  ES_ASSERT(fld);
  ES_ASSERT(m_block);

  EsStreamBlock* block = m_block->childAdd(
    name,
    EsStreamBlock::Field
  );
  ES_ASSERT(block);

  BlockScope scope(*this, block);

  m_block->typeNameSet(
    fld->typeNameGet()
  );

  if( fld->hasAttribute( EsStdNames::label() ) )
  {
    const EsString&  label = fld->attributeGet(
      EsStdNames::label()
    ).asString();

    m_block->attributeAdd(
      EsStreamBlock::label(),
      label
    );
  }

  if( fld->call(esT("isPOD")).asBool() )
  {
    if( fld->hasMethod( EsMethodInfoKeyT(1, esT("streamWrite")) ) )
      fld->call(
        esT("streamWrite"),
        asWeakReference()
      );
    else
      valueWrite(
        fld->propertyGet(EsStdNames::value())
      );
  }
  else if( fld->call(esT("isArray")).asBool() )
  {
    m_block->typeSet( EsVariant::VAR_OBJECT );
    itemsWrite(fld);
  }
  else
    objectWrite(fld);
}
//---------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsStream::objectCreateAtCurrentPos()
{
  ES_ASSERT(m_block);
  const EsString& typeName = objectBlockTypeNameGet(m_block);

  return doObjectCreate(typeName);
}
//---------------------------------------------------------------------------

void EsStream::objectReadBegin(const EsReflectedClassIntf::Ptr& obj)
{
  ES_ASSERT( m_block );
  ES_ASSERT( EsStreamBlock::Object == m_block->idGet() );

  const EsString& typeName = objectBlockTypeNameGet(m_block);

  if( typeName != obj->typeNameGet() )
    EsException::Throw(
      esT("Object type name does not match stream entry, expected '%s' got '%s'"),
      obj->typeNameGet(),
      typeName
    );
}
//---------------------------------------------------------------------------

void EsStream::objectReadEnd(const EsReflectedClassIntf::Ptr& obj)
{
}
//---------------------------------------------------------------------------

bool EsStream::propertiesReadBegin(const EsReflectedClassIntf::Ptr& obj)
{
  ES_ASSERT( m_block );
  EsStreamBlock* props = m_block->childGet(
    EsString::null(),
    EsStreamBlock::Properties
  );

  if( props )
  {
    m_block = props;
    return true;
  }

  return false;
}
//---------------------------------------------------------------------------

void EsStream::propertiesReadEnd(const EsReflectedClassIntf::Ptr& obj)
{
  ES_ASSERT( m_block );

  EsStreamBlock* parent = m_block->parentGet();
  ES_ASSERT(parent);
  ES_ASSERT( EsStreamBlock::Object == parent->idGet() );

  m_block = parent;
}
//---------------------------------------------------------------------------

EsVariant EsStream::valueRead()
{
  ES_ASSERT( m_block );

  ulong type = m_block->typeGet();

  ES_STREAM_TRACE( esT("valueRead(type=%s)"), EsEnumSymbolGet<EsVariantType>(type).c_str() );

  if( EsVariant::VAR_EMPTY == type )
    return EsVariant::null();

  if( EsVariant::VAR_POINTER == type )
    EsException::Throw(esT("Pointer type serialization is not supported"));

  if( type <= EsVariant::VAR_STRING )
    return m_block->payloadGet();

  if( EsVariant::VAR_OBJECT == type )
  {
    BlockScope scope(
      *this,
      m_block->firstChildGet()
    );

    if( scope.isOk() )
    {
      EsReflectedClassIntf::Ptr obj = objectCreateAtCurrentPos();
      ES_ASSERT(obj);

      objectRead(obj);
      return obj;
    }

    return EsBaseIntfPtr();
  }

  if( EsVariant::VAR_VARIANT_COLLECTION == type ||
      EsVariant::VAR_STRING_COLLECTION == type
  )
  {
    EsVariant result(static_cast<EsVariant::Type>(type));
    itemsRead(result);

    return result;
  }

  return EsVariant::null();
}
//---------------------------------------------------------------------------

void EsStream::propertyRead(const EsString& name, const EsReflectedClassIntf::Ptr& obj)
{
  ES_ASSERT(m_block);

  ES_STREAM_TRACE(esT("propertyRead(%s)"), name.c_str());

  BlockScope scope(
    *this,
    m_block->childGet(
      name,
      EsStreamBlock::Property
    )
  );
  if( scope.isOk() )
    obj->propertySet(
      name,
      valueRead()
    );
  else if( !(m_flags & static_cast<ulong>(EsStreamFlag::NonStrictProperties)) )
    EsException::Throw(
      esT("Could not read property entry for '%s'"),
      name
    );
}
//---------------------------------------------------------------------------

bool EsStream::itemsReadBegin(EsVariant& var, ulong& cnt, bool& doAppend)
{
  ES_ASSERT( m_block );

  EsStreamBlock* items = m_block->childGet(
    EsString::null(),
    EsStreamBlock::Items
  );

  if( items )
  {
    m_block = items;

    // return count of item nodes
    cnt = static_cast<ulong>(items->childrenCountGet());

    if( var.isCollection() )
    {
      var.countSet(cnt);
       return true;
    }

    if( var.isObject() && var.isIndexed() )
    {
      if( var.hasMethod( EsStdNames::countSet(), 1 ) )
      {
        var.countSet(cnt);
        return true;
      }

      if( var.hasMethod( EsStdNames::countGet(), 0 ) )
      {
        ulong objCnt = var.countGet();
        if( 0 == objCnt &&
            var.hasMethod( EsStdNames::itemAppend(), 1 )
        )
        {
          doAppend = true;
          return true;
        }

        if( objCnt != cnt )
          EsException::Throw(
            esT("Unexpected count of items when reading indexed object. Expected %d got %d"),
            objCnt,
            cnt
          );
        else
          return true;
      }
    }
  }

  return false;
}
//---------------------------------------------------------------------------

void EsStream::itemsReadEnd(EsVariant& var)
{
  ES_ASSERT( m_block );

  m_block = m_block->parentGet();
}
//---------------------------------------------------------------------------

void EsStream::itemRead(ulong idx, EsVariant& var, bool doAppend)
{
  ES_ASSERT(m_block);

  ES_STREAM_TRACE(esT("itemRead idx=%d, doAppend=%s"), idx, doAppend ? esT("true") : esT("false"));

  EsStreamBlock* bitem = m_block->itemGet(idx);
  BlockScope item(
    *this,
    bitem
  );

  if( !item.isOk() )
    EsException::Throw(
      esT("Indexed item '%d' could not be found in current block"),
      idx
    );

  if( doAppend )
    var.call(
      EsStdNames::itemAppend(),
      valueRead()
    );
  else
    var.itemSet(
      idx,
      valueRead()
    );
}
//---------------------------------------------------------------------------

bool EsStream::fieldsReadBegin(const EsReflectedClassIntf::Ptr& obj)
{
  ES_ASSERT(m_block);
  EsStreamBlock* fields = m_block->childGet(EsString::null(), EsStreamBlock::Fields);

  if( fields )
  {
    m_block = fields;
    return true;
  }

  return false;
}
//---------------------------------------------------------------------------

void EsStream::fieldsReadEnd(const EsReflectedClassIntf::Ptr& obj)
{
  ES_ASSERT( m_block );

  EsStreamBlock* parent = m_block->parentGet();
  ES_ASSERT( parent );
  ES_ASSERT( EsStreamBlock::Object == parent->idGet() );

  m_block = parent;
}
//---------------------------------------------------------------------------

void EsStream::typeNameCheck(const EsString& typeName, const EsReflectedClassIntf::Ptr& obj)
{
  if( typeName != obj->typeNameGet() )
    EsException::Throw(
      esT("Object type does not match block specs. Expected '%s', got '%s'"),
      obj->typeNameGet(),
      typeName
    );
}
//---------------------------------------------------------------------------

void EsStream::fieldRead(const EsString& name, const EsReflectedClassIntf::Ptr& fld)
{
  ES_ASSERT(m_block);

  ES_STREAM_TRACE(esT("fieldRead(%s,fld=%s)"), name.c_str(), fld->typeNameGet().c_str());

  BlockScope fldScope(
    *this,
    m_block->childGet(
      name,
      EsStreamBlock::Field
    )
  );

  ES_STREAM_TRACE(esT("fldScope.isOk()=%s"), fldScope.isOk() ? esT("true") : esT("false"));

  if( fldScope.isOk() )
  {
    const EsString& typeName = m_block->typeNameGet();
    typeNameCheck(typeName, fld);

    if( fld->call(esT("isPOD")).asBool() ) //< Simple types, assign value property
    {
      if( fld->hasMethod( EsMethodInfoKeyT(1, esT("streamRead")) ) )
      {
        ES_STREAM_TRACE(esT("... as POD with streamRead"));

        fld->call(
          esT("streamRead"),
          asWeakReference()
        );
      }
      else
      {
        ES_STREAM_TRACE(esT("... as POD with value"));

        fld->propertySet(
          EsStdNames::value(),
          valueRead()
        );
      }
    }
    else if( fld->call(esT("isArray")).asBool() ) //< Array type - read items
    {
      EsVariant var = fld;

      ES_STREAM_TRACE(esT("... as Array of items"));
      itemsRead(var);
    }
    else //< Read field as object
    {
      ES_STREAM_TRACE(esT("... as Object"));

      BlockScope objScope(
        *this,
        m_block->childGet(
          typeName,
          EsStreamBlock::Object
        )
      );

      objectRead(fld);
    }
  }
  else if( !(m_flags & static_cast<ulong>(EsStreamFlag::NonStrictFields)) )
    EsException::Throw(
      esT("Field '%s' could not be found in stream"),
      name
    );
}
//---------------------------------------------------------------------------

void EsStream::checkContextNameNotEmpty(const EsString& name)
{
  if( name.empty() )
    EsException::Throw(esT("Stream context name mey not be empty"));
}
//---------------------------------------------------------------------------

void EsStream::checkContextNameNotReserved(const EsString& name)
{
  if( EsStreamBlock::root() == name )
    EsException::Throw(esT("Stream context name 'ROOT' is reserved and may not be used to create or remove context"));
}
//---------------------------------------------------------------------------

void EsStream::checkStreamReadable() const
{
  if( !(m_flags & static_cast<ulong>(EsStreamFlag::Read)) )
    EsException::Throw(esT("Stream is not configured for reading"));
}
//---------------------------------------------------------------------------

void EsStream::checkStreamWriteable() const
{
  if( !(m_flags & static_cast<ulong>(EsStreamFlag::Write)) )
    EsException::Throw(esT("Stream is not configured for writing"));
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsStreamContextExistingScope::EsStreamContextExistingScope(const EsStreamIntf::Ptr& stream, const EsString& context) :
m_opened(false),
m_stream(stream),
m_context(context)
{
  if( m_stream )
    m_opened = m_stream->contextOpenExisting(context);
}
//---------------------------------------------------------------------------

EsStreamContextExistingScope::~EsStreamContextExistingScope()
{
  if( m_stream && m_opened )
    m_stream->contextCurrentClose();
}
//---------------------------------------------------------------------------

ulong EsStreamContextExistingScope::versionGet() const
{
  if( isOk() )
    return m_stream->contextCurrentVersionGet();

  EsException::Throw(
    esT("Could not read '%s' context version, context is not valid"),
    m_context
  );
  return 0; //< Pacify compilers
}
//---------------------------------------------------------------------------

void EsStreamContextExistingScope::versionSet(ulong newVersion)
{
  if( isOk() )
    m_stream->contextCurrentVersionSet(newVersion);
  else
    EsException::Throw(
      esT("Could not write '%s' context version, context is not valid"),
      m_context
    );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsStreamContextExistingOrNewScope::EsStreamContextExistingOrNewScope(const EsStreamIntf::Ptr& stream, const EsString& context) :
m_opened(false),
m_stream(stream),
m_context(context)
{
  if( m_stream )
    m_opened = m_stream->contextOpenCreate(context);
}
//---------------------------------------------------------------------------

EsStreamContextExistingOrNewScope::EsStreamContextExistingOrNewScope(const EsStreamIntf::Ptr& stream,
  const EsString& context, ulong newVersion) :
m_opened(false),
m_stream(stream),
m_context(context)
{
  if( m_stream )
  {
    m_opened = m_stream->contextOpenCreate(context);
    versionSet(newVersion);
  }
}
//---------------------------------------------------------------------------

EsStreamContextExistingOrNewScope::~EsStreamContextExistingOrNewScope()
{
  if( m_stream && m_opened )
    m_stream->contextCurrentClose();
}
//---------------------------------------------------------------------------

ulong EsStreamContextExistingOrNewScope::versionGet() const
{
  if( isOk() )
    return m_stream->contextCurrentVersionGet();

  EsException::Throw(
    esT("Could not read '%s' context version, context is not valid"),
    m_context
  );
  return 0; //< Pacify compilers
}
//---------------------------------------------------------------------------

void EsStreamContextExistingOrNewScope::versionSet(ulong newVersion)
{
  if( isOk() )
    m_stream->contextCurrentVersionSet(newVersion);
  else
    EsException::Throw(
      esT("Could not write '%s' context version, context is not valid"),
      m_context
    );
}
//---------------------------------------------------------------------------

