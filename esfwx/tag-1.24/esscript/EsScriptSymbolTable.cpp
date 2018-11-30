#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptSymbolTable.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// variable accessor
class ES_INTF_IMPL1(EsScriptValAccessor, EsScriptValAccessorIntf)

protected:
	// friend-only functionality
	EsScriptValAccessor(const EsString& name, const EsVariant& var, ulong flags, EsScriptObjectIntf* parent);
	static EsScriptValAccessorIntf::Ptr create(const EsString& name, const EsVariant& var, ulong flags, EsScriptObjectIntf* parent/* = 0*/)
	{
		return EsScriptValAccessorIntf::Ptr(new EsScriptValAccessor(name, var, flags, parent));
	}

public:
	virtual ~EsScriptValAccessor();
	// EsBaseIntf override
	//
	ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW
	{
		return classNameGet();
	}

	// interface impl
	ES_DECL_INTF_METHOD(EsVariant&, get)();
	ES_DECL_INTF_METHOD(bool, getReturnsTemporary)() const ES_NOTHROW { return false; }
	ES_DECL_INTF_METHOD(void, set)(const EsVariant& val);
	ES_DECL_INTF_METHOD(bool, isOk)() const ES_NOTHROW { return true; }
	ES_DECL_INTF_METHOD(EsString, trace)() const;
	ES_DECL_INTF_METHOD(bool, isReadOnly)() const ES_NOTHROW { return EsScriptSymbolFlag::ReadOnly == (m_flags & EsScriptSymbolFlag::ReadOnly); }
	ES_DECL_INTF_METHOD(bool, isBuiltIn)() const ES_NOTHROW { return EsScriptSymbolFlag::BuiltIn == (m_flags & EsScriptSymbolFlag::BuiltIn); }
	ES_DECL_INTF_METHOD(ulong, flagsGet)() const ES_NOTHROW { return m_flags; }
	ES_DECL_INTF_METHOD(const EsString&, nameGet)() const ES_NOTHROW { return m_name; }
	// add object value update subscriber
	ES_DECL_INTF_METHOD(void, updateSubscriberAdd)(EsScriptObjectIntf* subscriber)
	{
		ES_ASSERT(m_parent);
		ES_ASSERT(subscriber);
		if( m_updateSubscribers.end() == std::find(m_updateSubscribers.begin(), m_updateSubscribers.end(), subscriber) )
		{
			m_updateSubscribers.push_back(subscriber);
			ESSCRIPT_OBJECT_TRACE3(esT("Update subscriber '%s' added to value accesor '%s'"), subscriber->typeNameGet().c_str(), m_name.c_str())
		}
	}

protected:
	// data members
	EsScriptObjectIntf* m_parent;
	ulong m_flags;
	EsVariant m_val;
	EsString m_name;
	// list of weak references to update subscribers
	EsScriptObjectIntf::WeakPtrList m_updateSubscribers;

	friend class EsScriptSymbolTable;
	friend class EsScriptCodeSection;
	friend class EsScriptObject;
};

// values
//
EsScriptValAccessor::EsScriptValAccessor(const EsString& name, const EsVariant& val, ulong flags, EsScriptObjectIntf* parent) :
m_parent(parent),
m_flags(flags),
m_val(val),
m_name(name)
{
	m_dynamic = true;

	ESSCRIPT_VALACCESS_ASSERT_NOT_ACCESSOR(val)
	ESSCRIPT_VALACCESS_TRACE4(
    esT("EsScriptValAccessor (%s %p) created with '%s'"),
		m_name,
		EsVariant(
      this,
      EsVariant::ACCEPT_POINTER
    ),
		EsScriptMachine::traceVariant(m_val)
  )
}

EsScriptValAccessor::~EsScriptValAccessor()
{
	ESSCRIPT_VALACCESS_TRACE4(
    esT("EsScriptValAccessor (%s %p) deleted (had '%s')"),
		m_name,
    EsVariant(
      this,
      EsVariant::ACCEPT_POINTER
    ),
		EsScriptMachine::traceVariant(m_val)
  )
}
//---------------------------------------------------------------------------

// interface impl
ES_IMPL_INTF_METHOD(EsVariant&, EsScriptValAccessor::get)()
{
	ESSCRIPT_MACHINE_TRACE3(
    esT("EsScriptValAccessor %s::get()='%s'"),
		m_name,
		EsScriptMachine::traceVariant(m_val)
  )

	return m_val;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsScriptValAccessor::set)(const EsVariant& val)
{
	ESSCRIPT_VALACCESS_ASSERT_NOT_ACCESSOR(val)

	if( isReadOnly() )
		EsScriptException::ThrowPerformingOperationOnReadOnly(esT("set"));

	ESSCRIPT_MACHINE_TRACE4(esT("EsScriptValAccessor %s::set('%s') (was '%s')"),
		m_name,
		EsScriptMachine::traceVariant(val),
		EsScriptMachine::traceVariant(m_val)
  )

	m_val = val;

	// notify parent of data change
	if( m_parent )
		m_parent->publishDataChanged(m_updateSubscribers);
}

ES_IMPL_INTF_METHOD(EsString, EsScriptValAccessor::trace)() const
{
	return EsString::format(
    esT("EsScriptValAccessor '%s' has '%s'"),
		m_name,
		EsScriptMachine::traceVariant(m_val)
  );
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

EsScriptSymbolTable::EsScriptSymbolTable(bool checkTemplate /*= true*/) ES_NOTHROW :
m_contents( EsStringIndexedMap::ContainerWithoutInterlock ),
m_checkTemplate(checkTemplate),
m_template(true)
{
}

EsScriptSymbolTable::EsScriptSymbolTable(const EsScriptSymbolTable& src, EsScriptObjectIntf* parent/* = 0*/)
: m_contents( EsStringIndexedMap::ContainerWithoutInterlock)
{
	clone(src, parent);
}

EsScriptSymbolTable::~EsScriptSymbolTable() ES_NOTHROW
{
}

const EsScriptValAccessorIntf::Ptr& EsScriptSymbolTable::null() ES_NOTHROW
{
  static const EsScriptValAccessorIntf::Ptr sc_null;

  return sc_null;
}

void EsScriptSymbolTable::clone(const EsScriptSymbolTable& src, EsScriptObjectIntf* parent/* = nullptr*/)
{
	ES_ASSERT(src.isTemplate());

  m_contents.clear();
	m_checkTemplate = src.m_checkTemplate;
	m_template = false;

	for( ulong idx = 0; idx < src.m_contents.countGet(); ++idx )
	{
		const EsString& name = src.m_contents.nameGet(idx);
		ulong flags = src.m_contents.valueGet(idx).doInterpretAsVariantCollection()[1].asULong();

		EsScriptValAccessorIntf::Ptr acc = EsScriptValAccessor::create(
      name,
      EsVariant::null(),
      flags,
      parent
    );

		EsVariant val(EsVariant::VAR_VARIANT_COLLECTION);
		val.addToVariantCollection(acc)
			.addToVariantCollection(flags);

		m_contents.itemAdd(
      name,
      val
    );
	}
}

EsStringArray EsScriptSymbolTable::allSymbolNamesGet(bool includeBuiltIns /*= true*/) const ES_NOTHROW
{
	EsStringArray a;
	a.reserve( m_contents.countGet() );
	for( ulong idx = 0; idx < m_contents.countGet(); ++idx )
	{
		ulong flags = m_contents.valueGet(idx).doInterpretAsVariantCollection()[1].asULong();
		if(
      !includeBuiltIns &&
			(flags & EsScriptSymbolFlag::BuiltIn)
    )
			continue;

		const EsString& name = m_contents.nameGet(idx);
		a.push_back(name);
	}

	return a;
}

// symbol container implementation
bool EsScriptSymbolTable::isSymbolBuiltIn(const EsString& name) const
{
	ulong flags = m_contents.valueGet(name).doInterpretAsVariantCollection()[1].asULong();
	return flags & EsScriptSymbolFlag::BuiltIn;

	return false; // pacify compilers
}

const EsString& EsScriptSymbolTable::findFirstSymbolWithVal(const EsVariant& val) const ES_NOTHROW
{
	for( ulong idx = 0; idx < m_contents.countGet(); ++idx )
	{
		const EsScriptValAccessorIntf::Ptr& acc = m_contents.valueGet(idx).doInterpretAsVariantCollection()[0].asExistingObject();
		const EsVariant& other = acc->get();

		if(
      (val.isEmpty() && other.isEmpty()) ||
			(!val.isEmpty() && !other.isEmpty() && val.typeGet() == other.typeGet() && val == other)
    )
			return m_contents.nameGet(idx);
	}

	return EsString::null();
}

void EsScriptSymbolTable::symbolTemplateAdd(const EsString& name, ulong flags,
																		const EsScriptDebugInfoIntf::Ptr& dbg /*= EsScriptDebugInfoIntf::Ptr()*/)
{
	checkTemplateOperation();

	if( m_contents.itemExists(name) )
		EsScriptException::Throw(
      EsString::format(
        esT("Symbol '%s' is already declared"),
        name
      ),
      dbg
    );

	// Add slot with empty accessor
	EsVariant::Array v
#ifdef ES_MODERN_CPP
  = {
    EsScriptValAccessorIntf::Ptr(),
		flags
  };
#else
  ;
  v.reserve(2);
  v.push_back( EsScriptValAccessorIntf::Ptr() );
  v.push_back( flags );
#endif

	m_contents.itemAdd(
    name,
    v
  );
}

EsScriptValAccessorIntf::Ptr EsScriptSymbolTable::symbolNonTemplateAdd(const EsString& name, const EsVariant& val, ulong flags,
																		const EsScriptDebugInfoIntf::Ptr& dbg /*= EsScriptDebugInfoIntf::Ptr()*/)
{
	checkNonTemplateOperation();
	if( m_contents.itemExists(name) )
		EsScriptException::Throw(
      EsString::format(
        esT("Symbol '%s' is already declared"),
        name
      ),
      dbg
    );

	EsScriptValAccessorIntf::Ptr result = valAccessorCreate(
    name,
    val,
    flags
  );
	ES_ASSERT(result);

	accessorAdd( result );

	return result;
}

void EsScriptSymbolTable::accessorAdd(const EsScriptValAccessorIntf::Ptr& acc)
{
	EsVariant::Array v
#ifdef ES_MODERN_CPP
  = {
	  acc,
		acc->flagsGet()
  };
#else
  ;
  v.reserve(2);
  v.push_back(acc);
  v.push_back(acc->flagsGet());
#endif

	m_contents.itemAdd(
    acc->nameGet(),
    v
  );
}

ulong EsScriptSymbolTable::symbolFlagsGet(const EsString& name) const
{
	return m_contents.valueGet(name).doInterpretAsVariantCollection()[1].asULong();
}

EsScriptValAccessorIntf::Ptr EsScriptSymbolTable::symbolFind(const EsString& name, bool& found) const ES_NOTHROW
{
	ulong idx = m_contents.itemFind(name);
	found = (EsStringIndexedMap::npos != idx);

  if( found )
    return m_contents.valueGet( idx ).doInterpretAsVariantCollection()[0].asObject();

	return null();
}

EsScriptValAccessorIntf::Ptr EsScriptSymbolTable::symbolGet(const EsString& name, bool doThrow) const
{
	checkNonTemplateOperation();
	bool found;
	const EsScriptValAccessorIntf::Ptr& sym = symbolFind(name, found);

	if( found )
	{
#ifdef ES_DEBUG
    if( name != sym->nameGet() )
    {
      ES_DEBUG_TRACE(
        esT("EsScriptSymbolTable::symbolGet mismatch: looking for '%s', found '%s'"),
        name,
        sym->nameGet()
      );
    }
#endif
		ES_ASSERT(name == sym->nameGet());

		return sym;
	}
	else if( doThrow )
		EsScriptException::Throw(
      EsString::format(
        esT("Symbol '%s' was not found"),
        name
      )
    );

	return null();
}

void EsScriptSymbolTable::symbolValSet(const EsString& name, const EsVariant& val)
{
	const EsScriptValAccessorIntf::Ptr& sym = symbolGet(name, true);
	if( !sym )
		EsScriptException::Throw(
      EsString::format(
        esT("Symbol '%s' was not initialized"),
        name
      )
    );

	sym->set(val);
}

void EsScriptSymbolTable::symbolValSetIgnoreReadOnly(const EsString& name, const EsVariant& val)
{
	const EsScriptValAccessorIntf::Ptr& sym = symbolGet(name, true);
	if( !sym )
		EsScriptException::Throw(
      EsString::format(
        esT("Symbol '%s' was not initialized"),
        name
      )
    );

	ESSCRIPT_VALACCESS_ASSERT_NOT_ACCESSOR(val)

	EsScriptValAccessor* pval = ES_INTFPTR_TO_OBJECTPTR(sym, EsScriptValAccessor);
	ES_ASSERT(pval);

	pval->m_val = val;
}

EsScriptValAccessorIntf::Ptr EsScriptSymbolTable::valAccessorCreate(const EsString& name, const EsVariant& val, ulong flags)
{
	return EsScriptValAccessor::create(name, val, flags, 0);
}

// internal services
void EsScriptSymbolTable::checkTemplateOperation() const
{
	if( m_checkTemplate && !m_template )
		EsScriptException::Throw( esT("Operation only allowed on symbol table template") );
}

void EsScriptSymbolTable::checkNonTemplateOperation() const
{
	if( m_checkTemplate && m_template )
		EsScriptException::Throw( esT("Operation only allowed on non template symbol table") );
}
