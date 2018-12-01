#ifdef ES_USE_SCRIPT_INSTRUCTION_CACHE
# include "EsScriptInstructionCache.h"
#endif

// indexed item accessor
class ES_INTF_IMPL1(EsItemAccessor, EsScriptValAccessorIntf)

protected:
  // friend-only functionality
  EsItemAccessor(const EsScriptValAccessorIntf::Ptr& valAcc, const EsScriptValAccessorIntf::Ptr& idxExprAcc);
  static EsScriptValAccessorIntf::Ptr create(const EsScriptValAccessorIntf::Ptr& valAcc, const EsScriptValAccessorIntf::Ptr& idxExprAcc)
  {
    return EsScriptValAccessorIntf::Ptr(new EsItemAccessor(valAcc, idxExprAcc));
  }

public:
  virtual ~EsItemAccessor();
  // EsBaseIntf override
  //
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW
  {
    return classNameGet();
  }

  // interface impl
  ES_DECL_INTF_METHOD(EsVariant&, get)();
  ES_DECL_INTF_METHOD(bool, getReturnsTemporary)() const ES_NOTHROW { return true; }
  ES_DECL_INTF_METHOD(void, set)(const EsVariant& val);  
  ES_DECL_INTF_METHOD(bool, isOk)() const ES_NOTHROW { return true; }
  ES_DECL_INTF_METHOD(EsString, trace)() const;
  ES_DECL_INTF_METHOD(bool, isReadOnly)() const ES_NOTHROW { return false; }
  ES_DECL_INTF_METHOD(bool, isBuiltIn)() const ES_NOTHROW { return false; }
  ES_DECL_INTF_METHOD(ulong, flagsGet)() const ES_NOTHROW { return EsScriptSymbolFlag::None; }
  ES_DECL_INTF_METHOD(const EsString&, nameGet)() const ES_NOTHROW { return EsString::null(); }
  // for member var accessors only
  ES_DECL_INTF_METHOD(void, updateSubscriberAdd)(EsScriptObjectIntf* subscriber) { ES_FAIL; }

protected:
  // data members
  EsScriptValAccessorIntf::Ptr m_valAcc;
  EsScriptValAccessorIntf::Ptr m_idxExprAcc;
  EsVariant m_valCache;

  friend class EsScriptThreadContext;
};

// temporary values accessor
class ES_INTF_IMPL1(EsScriptTmpValAccessor, EsScriptValAccessorIntf)

protected:
  EsScriptTmpValAccessor(const EsVariant& val);
  static EsScriptValAccessorIntf::Ptr create(const EsVariant& val)
  {
    return EsScriptValAccessorIntf::Ptr(new EsScriptTmpValAccessor(val));
  }
  
public:
  virtual ~EsScriptTmpValAccessor();

  // EsBaseIntf override
  //
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW
  { 
    return classNameGet(); 
  }

  // interface impl
  ES_DECL_INTF_METHOD(EsVariant&, get)();
  ES_DECL_INTF_METHOD(bool, getReturnsTemporary)() const ES_NOTHROW { return true; }
  ES_DECL_INTF_METHOD(void, set)(const EsVariant& val);
  ES_DECL_INTF_METHOD(EsString, trace)() const;
  ES_DECL_INTF_METHOD(bool, isOk)() const ES_NOTHROW { return true; }
  ES_DECL_INTF_METHOD(bool, isReadOnly)() const ES_NOTHROW { return false; }
  ES_DECL_INTF_METHOD(bool, isBuiltIn)() const ES_NOTHROW { return false; }
  ES_DECL_INTF_METHOD(ulong, flagsGet)() const ES_NOTHROW { return EsScriptSymbolFlag::None; }
  ES_DECL_INTF_METHOD(const EsString&, nameGet)() const ES_NOTHROW { return EsString::null(); }
  // for member var accessors only
  ES_DECL_INTF_METHOD(void, updateSubscriberAdd)(EsScriptObjectIntf* subscriber) { ES_FAIL; }

protected:
  // data members
  EsVariant m_val; // temporary value

  friend class EsScriptThreadContext;
};

// automatic indexed item accessor
class ES_INTF_IMPL1(EsAutoItemAccessor, EsScriptValAccessorIntf)

protected:
  // friend-only functionality
  EsAutoItemAccessor(const EsScriptValAccessorIntf::Ptr& valAcc);
  static EsScriptValAccessorIntf::Ptr create(const EsScriptValAccessorIntf::Ptr& valAcc)
  {
    return EsScriptValAccessorIntf::Ptr(new EsAutoItemAccessor(valAcc));
  }

public:
  virtual ~EsAutoItemAccessor();

  // EsBaseIntf override
  //
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW
  {
    return classNameGet();
  }

  // interface impl
  ES_DECL_INTF_METHOD(EsVariant&, get)();
  ES_DECL_INTF_METHOD(bool, getReturnsTemporary)() const ES_NOTHROW { return true; }
  ES_DECL_INTF_METHOD(void, set)(const EsVariant& val);
  ES_DECL_INTF_METHOD(bool, isOk)() const ES_NOTHROW;
  ES_DECL_INTF_METHOD(EsString, trace)() const;
  ES_DECL_INTF_METHOD(bool, isReadOnly)() const ES_NOTHROW { return false; }
  ES_DECL_INTF_METHOD(bool, isBuiltIn)() const ES_NOTHROW { return false; }
  ES_DECL_INTF_METHOD(ulong, flagsGet)() const ES_NOTHROW { return EsScriptSymbolFlag::None; }
  ES_DECL_INTF_METHOD(const EsString&, nameGet)() const ES_NOTHROW { return EsString::null(); }
  // for member var accessors only
  ES_DECL_INTF_METHOD(void, updateSubscriberAdd)(EsScriptObjectIntf* subscriber) { ES_FAIL; }

protected:
  // data members
  mutable size_t m_idx;
  EsScriptValAccessorIntf::Ptr m_valAcc;
  EsVariant m_valCache;

  friend class EsScriptThreadContext;
};

// object property accessor
class ES_INTF_IMPL1(EsPropertyAccessor, EsScriptValAccessorIntf)

protected:
  // friend-only functionality
  EsPropertyAccessor(const EsScriptValAccessorIntf::Ptr& objAcc, const EsScriptInstruction& instr);
  static EsScriptValAccessorIntf::Ptr create(const EsScriptValAccessorIntf::Ptr& objAcc,
    const EsScriptInstruction& instr)
  {
    return EsScriptValAccessorIntf::Ptr(new EsPropertyAccessor(objAcc, instr));
  }

public:
  virtual ~EsPropertyAccessor();

  // EsBaseIntf override
  //
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW
  {
    return classNameGet();
  }

  // interface impl
  ES_DECL_INTF_METHOD(EsVariant&, get)();
  ES_DECL_INTF_METHOD(bool, getReturnsTemporary)() const ES_NOTHROW { return true; }
  ES_DECL_INTF_METHOD(void, set)(const EsVariant& val);
  ES_DECL_INTF_METHOD(bool, isOk)() const ES_NOTHROW { return true; }
  ES_DECL_INTF_METHOD(EsString, trace)() const;
  ES_DECL_INTF_METHOD(bool, isReadOnly)() const ES_NOTHROW { return false; }
  ES_DECL_INTF_METHOD(bool, isBuiltIn)() const ES_NOTHROW { return false; }
  ES_DECL_INTF_METHOD(ulong, flagsGet)() const ES_NOTHROW { return EsScriptSymbolFlag::None; }
  ES_DECL_INTF_METHOD(const EsString&, nameGet)() const ES_NOTHROW { return m_propName; }
  // for member var accessors only
  ES_DECL_INTF_METHOD(void, updateSubscriberAdd)(EsScriptObjectIntf* subscriber) { ES_FAIL; }

protected:
  // Try to get property info from instruction cache or, if not there, find it in object,
  // and set-up instruction cache
  //
  const EsPropertyInfo* propInfoGetCaching(const EsReflectedClassIntf::Ptr& obj);

protected:
  // data members
  EsScriptValAccessorIntf::Ptr m_objAcc;
  EsString m_propName;
  EsVariant m_valCache;
  const EsScriptInstruction& m_instr;

  friend class EsScriptThreadContext;
};

// temporary accessor impl
//
EsScriptTmpValAccessor::EsScriptTmpValAccessor(const EsVariant& val) :
m_val(val)
{
  m_dynamic = true;
  ESSCRIPT_VALACCESS_ASSERT_NOT_ACCESSOR(val)
  ESSCRIPT_VALACCESS_TRACE3(
    esT("EsScriptTmpValAccessor (%p) created with '%s'"), 
    EsVariant(
      this,
      EsVariant::ACCEPT_POINTER
    ),
    EsScriptMachine::traceVariant(m_val)
  )
}

EsScriptTmpValAccessor::~EsScriptTmpValAccessor()
{
  ESSCRIPT_VALACCESS_TRACE3(
    esT("EsScriptTmpValAccessor (%p) deleted (had '%s')"), 
    EsVariant(
      this,
      EsVariant::ACCEPT_POINTER
    ),
    EsScriptMachine::traceVariant(m_val)
  )
}

ES_IMPL_INTF_METHOD(EsVariant&, EsScriptTmpValAccessor::get)()
{
  ESSCRIPT_VALACCESS_TRACE2(
    esT("EsScriptTmpValAccessor::get()='%s'"), 
    EsScriptMachine::traceVariant(m_val)
  )

  return m_val;
}

ES_IMPL_INTF_METHOD(void, EsScriptTmpValAccessor::set)(const EsVariant& val)
{
  ESSCRIPT_VALACCESS_ASSERT_NOT_ACCESSOR(val)
  ESSCRIPT_VALACCESS_TRACE3(
    esT("EsScriptTmpValAccessor::set='%s', (had '%s')"), 
    EsScriptMachine::traceVariant(val), 
    EsScriptMachine::traceVariant(m_val)
  )

  if( m_val.isObject() )
  {
    EsReflectedClassIntf::Ptr obj = m_val.asExistingObject();
    obj->propertySet(EsStdNames::value(), val);
  }
  else
    m_val = val;
}

ES_IMPL_INTF_METHOD(EsString, EsScriptTmpValAccessor::trace)() const
{
  return EsString::format(
    esT("EsScriptTmpValAccessor has'%s'"),
    EsScriptMachine::traceVariant(m_val)
  );
}

// items
//
EsItemAccessor::EsItemAccessor(const EsScriptValAccessorIntf::Ptr& valAcc, const EsScriptValAccessorIntf::Ptr& idxExprAcc)
{
  ES_ASSERT(valAcc);
  ES_ASSERT(idxExprAcc);
  m_dynamic = true;
  m_valAcc = valAcc;
  m_idxExprAcc = idxExprAcc;

  ESSCRIPT_VALACCESS_TRACE2(
    esT("EsItemAccessor (%p) created"), 
    EsVariant(
      this,
      EsVariant::ACCEPT_POINTER
    )
  )
}

EsItemAccessor::~EsItemAccessor()
{
  ESSCRIPT_VALACCESS_TRACE3(
    esT("EsItemAccessor (%p) deleted, (had '%s')"),
    EsVariant(
      this,
      EsVariant::ACCEPT_POINTER
    ),
    trace()
  )
}

// interface impl
ES_IMPL_INTF_METHOD(EsVariant&, EsItemAccessor::get)()
{
  const EsVariant& idx = m_idxExprAcc->get();
  const EsVariant& val = m_valAcc->get();

  m_valCache = val.itemGet(idx);

  ESSCRIPT_VALACCESS_TRACE3(
    esT("EsItemAccessor::get()['%s'] returns '%s'"),
    EsScriptMachine::traceVariant(idx),
    EsScriptMachine::traceVariant(m_valCache)
  )

  return m_valCache;
}

ES_IMPL_INTF_METHOD(void, EsItemAccessor::set)(const EsVariant& val)
{
  ESSCRIPT_VALACCESS_ASSERT_NOT_ACCESSOR(val)
  const EsVariant& idx = m_idxExprAcc->get();

  EsVariant& v = m_valAcc->get();
  v.itemSet( idx, val );

  if( !v.isObject() && m_valAcc->getReturnsTemporary() )
    m_valAcc->set(v);

  ESSCRIPT_VALACCESS_TRACE3(
    esT("EsItemAccessor::set('%s')['%s']"),
    EsScriptMachine::traceVariant(val),
    EsScriptMachine::traceVariant(idx)
  )
}

ES_IMPL_INTF_METHOD(EsString, EsItemAccessor::trace)() const
{
  const EsVariant& idx = m_idxExprAcc->get();
  const EsVariant& val = m_valAcc->get();
  const EsVariant& item = val.itemGet( idx );

  return EsString::format(
    esT("EsItemAccessor ['%s'] = '%s'"),
    EsScriptMachine::traceVariant(idx),
    EsScriptMachine::traceVariant(item)
  );
}

// auto items
//
EsAutoItemAccessor::EsAutoItemAccessor(const EsScriptValAccessorIntf::Ptr& valAcc) :
m_idx(0)
{
  ES_ASSERT(valAcc);
  m_dynamic = true;
  m_valAcc = valAcc;

  ESSCRIPT_VALACCESS_TRACE2(
    esT("EsAutoItemAccessor (%p) created"), 
    EsVariant(
      this,
      EsVariant::ACCEPT_POINTER
    )
  )
}

EsAutoItemAccessor::~EsAutoItemAccessor()
{
  ESSCRIPT_VALACCESS_TRACE3(
    esT("EsAutoItemAccessor (%p) deleted, (had '%s')"),
    EsVariant(
      this,
      EsVariant::ACCEPT_POINTER
    ),
    trace()
  )
}

// interface impl
ES_IMPL_INTF_METHOD(EsVariant&, EsAutoItemAccessor::get)()
{
  ES_ASSERT( isOk() );

  m_valCache = m_valAcc->get().itemGet( m_idx++ );

  ESSCRIPT_VALACCESS_TRACE3(
    esT("EsAutoItemAccessor::get()['%d'] returns '%s'"),
    m_idx,
    EsScriptMachine::traceVariant(m_valCache)
  )

  return m_valCache;
}

ES_IMPL_INTF_METHOD(void, EsAutoItemAccessor::set)(const EsVariant& val)
{
  ESSCRIPT_VALACCESS_ASSERT_NOT_ACCESSOR(val)
  EsScriptException::Throw(esT("Setting auto indexed item is not supported"));
}

ES_IMPL_INTF_METHOD(bool, EsAutoItemAccessor::isOk)() const ES_NOTHROW
{
  const EsVariant& val = m_valAcc->get();
  if( val.isIndexed() )
    return m_idx < static_cast<size_t>(val.countGet());

  return false;
}

ES_IMPL_INTF_METHOD(EsString, EsAutoItemAccessor::trace)() const
{
  if( isOk() )
  {
    const EsVariant& item = m_valAcc->get().itemGet( m_idx );
    return EsString::format(
      esT("EsAutoItemAccessor ['%d']='%s'"), 
      m_idx,
      EsScriptMachine::traceVariant(item)
    );
  }

  return esT("EsAutoItemAccessor is nok!");
}

// properties
//
EsPropertyAccessor::EsPropertyAccessor(const EsScriptValAccessorIntf::Ptr& objAcc,
  const EsScriptInstruction& instr) :
m_instr(instr)
{
  ES_ASSERT(objAcc);

  const EsString& propName = instr.nameGet();
  ES_ASSERT(!propName.empty());

  m_dynamic = true;
  m_objAcc = objAcc;
  m_propName = propName;

  ESSCRIPT_VALACCESS_TRACE3(
    esT("EsPropertyAccessor (%p) $'%s' created"),
    EsVariant(
      this,
      EsVariant::ACCEPT_POINTER
    ),
    m_propName
  )
}

EsPropertyAccessor::~EsPropertyAccessor()
{
  ESSCRIPT_VALACCESS_TRACE4(
    esT("EsPropertyAccessor (%p) $'%s' deleted, (had '%s')"),
    EsVariant(
      this,
      EsVariant::ACCEPT_POINTER
    ),
    m_propName,
    trace()
  )
}

const EsPropertyInfo* EsPropertyAccessor::propInfoGetCaching(const EsReflectedClassIntf::Ptr& obj)
{
#ifdef ES_USE_SCRIPT_INSTRUCTION_CACHE
  EsScriptInstructionCache& cache = m_instr.cacheGet();

  /// Miss the cache due to mismatched object type name
  if( !cache.matchesType( obj->typeNameGet() ) )
    cache.purge();

  if( !cache.isEmpty() )
    return cache.propInfoGet();
#endif

  const EsClassInfo& info = obj->classInfoGet();
  const EsPropertyInfo* pinfo = info.propertyInfoFind(m_propName);

#ifdef ES_USE_SCRIPT_INSTRUCTION_CACHE
  if( pinfo )
    cache.propInfoSet(pinfo, obj->typeNameGet().hashGet());
#endif

  return pinfo;
}

// interface impl
ES_IMPL_INTF_METHOD(EsVariant&, EsPropertyAccessor::get)()
{
  EsReflectedClassIntf::Ptr obj;
  obj = m_objAcc->get().asExistingObject();

  const EsPropertyInfo* info = propInfoGetCaching(obj);
  if( info )
  {
    m_valCache = info->get(
      obj->asWeakReference().get()
    );
  }
  else
    m_valCache = obj->propertyGet(m_propName);

  ESSCRIPT_VALACCESS_TRACE4(
    esT("EsPropertyAccessor::get()('%s$%s') returns '%s'"),
    obj->typeNameGet(),
    m_propName,
    EsScriptMachine::traceVariant(m_valCache)
  )

  return m_valCache;
}

ES_IMPL_INTF_METHOD(void, EsPropertyAccessor::set)(const EsVariant& val)
{
  ESSCRIPT_VALACCESS_ASSERT_NOT_ACCESSOR(val)
  EsReflectedClassIntf::Ptr obj = m_objAcc->get().asExistingObject();

  const EsPropertyInfo* info = propInfoGetCaching(obj);
  if( info )
  {
    info->set(
      obj->asWeakReference().get(),
      val
    );
  }
  else
     obj->propertySet(m_propName, val);

  ESSCRIPT_VALACCESS_TRACE4(
    esT("EsPropertyAccessor::set()('%s$%s', '%s'"),
    obj->typeNameGet(),
    m_propName,
    EsScriptMachine::traceVariant(val)
  )
}

ES_IMPL_INTF_METHOD(EsString, EsPropertyAccessor::trace)() const
{
  EsReflectedClassIntf::Ptr obj = m_objAcc->get().asExistingObject();
  const EsVariant& result = obj->propertyGet(m_propName);

  return EsString::format(
    esT("EsPropertyAccessor '%s$%s' = '%s'"),
    obj->typeNameGet(),
    m_propName,
    EsScriptMachine::traceVariant(result)
  );
}