#include "esscriptpch.h"
#pragma hdrstop

#include "EsVar.h"
#include "EsScriptMachine.h"
#include "EsScriptThreadContext.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Script context implementation
//

EsScriptContext::EsScriptContext(EsScriptMachine* machine) ES_NOTHROW :
m_machine(machine),
m_masterCtx(nullptr)
{
  ES_ASSERT(machine);
}
//---------------------------------------------------------------------------

EsScriptContext::~EsScriptContext() ES_NOTHROW
{
  unlink();
  while( !m_links.empty() )
    (*m_links.begin())->unlink();
}
//---------------------------------------------------------------------------

EsScriptContext::Ptr EsScriptContext::create(EsScriptMachine* machine) ES_NOTHROW
{
  return EsScriptContext::Ptr(new EsScriptContext(machine));
}
//---------------------------------------------------------------------------

void EsScriptContext::switchTo(EsScriptMachine* other) ES_NOTHROW
{
  ES_ASSERT(other);
  if(m_machine != other)
    m_machine = other;

  // Update all links as well
#ifdef ES_MODERN_CPP
  for( auto &v: m_links )
  {

#else
  for( std::set<EsScriptContext*>::const_iterator cit = m_links.begin(); cit != m_links.end(); ++cit )
  {
    EsScriptContext* v = (*cit);

#endif
    v->switchTo(other);
  }
}
//---------------------------------------------------------------------------

void EsScriptContext::linkAdd(EsScriptContext* link) ES_NOTHROW
{
  ES_ASSERT(link);
  m_links.insert( link );
  // Update link's machine to ours
  link->switchTo( m_machine );
}
//---------------------------------------------------------------------------

void EsScriptContext::linkDelete(EsScriptContext* link) ES_NOTHROW
{
  m_links.erase( link );
}
//---------------------------------------------------------------------------

void EsScriptContext::linkTo(const EsScriptContext::Ptr other) ES_NOTHROW
{
  ES_ASSERT(other);
  m_masterCtx = other.get();
  other->linkAdd(this);
}
//---------------------------------------------------------------------------

void EsScriptContext::unlink() ES_NOTHROW
{
  if( m_masterCtx )
  {
    m_masterCtx->linkDelete(this);
    m_masterCtx = nullptr;
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// EsScriptMachine maintenance thread implementation
//
EsScriptMachine::
GarbageCollector::GarbageCollector(EsScriptMachine& owner) :
EsThread(),
m_owner(owner)
{}
//---------------------------------------------------------------------------

long EsScriptMachine::GarbageCollector::worker()
{
  ESSCRIPT_MACHINE_TRACE1( esT("EsScriptMachine::GarbageCollector started") )

  while( !checkForStopping(4000) )
  {
    try
    {
      m_owner.threadCtxCleanup();
    }
    catch(...)
    {
    }
  }

  ESSCRIPT_MACHINE_TRACE1( esT("EsScriptMachine::GarbageCollector ended") )

  return 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// EsScriptMachine implementation
//
EsScriptMachine::EsScriptMachine(const EsScript& owner) :
m_owner(owner),
m_loc( EsLocale::locale() ),
m_ctxScriptThis(
  EsScriptContext::create(this)
),
m_gc(*this),
m_dbg(nullptr),
m_externs(false),
m_consts(false),
m_sysVars(false),
m_meta(*this),
m_host(*this),
m_ctxCur(nullptr),
m_abort(false),
m_destroying(false)
{
	ESSCRIPT_MACHINE_TRACE1( esT("Script machine ctor entered") )
  m_filesInfo = EsAssocContainer::create();

	reset();
}
// ---------------------------------------------------------------------------

EsScriptMachine::~EsScriptMachine()
{
  ESSCRIPT_MACHINE_TRACE1( esT("Script machine dtor entered") )

  m_destroying = true;

  reset();
}
//---------------------------------------------------------------------------

EsString EsScriptMachine::traceVariant(const EsVariant& v)
{
  if(v.isObject())
  {
    EsBaseIntfPtr obj = v.asObject();
    if(obj)
    {
      EsScriptValAccessorIntf::Ptr acc = obj;
      if(acc)
				return EsString::format(
          esT("object '%s'='%s'"),
          acc->typeNameGet(),
					acc->trace()
        );
      else
        return EsString::format(
          esT("object '%s'"),
          obj->typeNameGet()
        );
    }
    else
      return esT("null object");
  }
  else if(v.isCollection())
  {
    EsString str;
    for(int idx = 0; idx < v.countGet(); ++idx)
			str += EsString::format(
        esT("[%d]='%s';"),
        idx,
        traceVariant(v.itemGet(idx))
      );

    return str;
  }
  else if(!v.isEmpty())
    return v.asString();
  else
    return esT("null");
}
//---------------------------------------------------------------------------

EsScriptDebugInfoIntf::Ptr EsScriptMachine::currentDebugInfoGet() const ES_NOTHROW
{
  EsCriticalSectionLocker lock(m_cs);

  if( m_ctxCur )
    return m_ctxCur->currentDebugInfoGet();

  return nullptr; //< Pacify compiler
}
//---------------------------------------------------------------------------

void EsScriptMachine::debuggerSet(EsScriptDebug* dbg) ES_NOTHROW
{
  EsCriticalSectionLocker lock(m_csDbg);
  m_dbg = dbg;
}
//---------------------------------------------------------------------------

void EsScriptMachine::debuggerInstructionTrace(EsScriptInstructions::const_iterator instr)
{
  if( !m_dbg )
    return;

  EsCriticalSectionLocker lock(m_csDbg);
  if( m_dbg )
    m_dbg->onInstruction(instr);
}
//---------------------------------------------------------------------------

void EsScriptMachine::binLinksClear()
{
  while( !m_links.empty() )
  {
    EsScriptBinaryLinks::reverse_iterator last = m_links.rbegin();

    EsDynamicLibrary::Ptr lib = (*last).second;

    const EsPath& libpath = EsPath::createFromFilePath( (*last).first );
    const EsString& name = libpath.fileNameGet();

    EsDynamicLibrary::Pfn pfn = lib->procAddrGet(
      EsString::format(
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# if defined(_WIN64)
        esT("_%s_reflectionUninitDynamic"),
# else
        esT("__%s_reflectionUninitDynamic"),
# endif
#else
        esT("__%s_reflectionUninitDynamic"),
#endif
        name
      )
    );

    if( pfn ) //< Do unregister dynamic classinfos from our registry
      pfn();

    m_links.erase( (*last).first );  // Dll will implicitly unload here
  }
}
//---------------------------------------------------------------------------

void EsScriptMachine::reset()
{
  // destroy thread contexts first
  threadCtxsReset();

  // Destroy code sections
  m_globalMethods.reset(new EsScriptMethodMap);
  m_startup.reset();

  // destroy static and meta data
  m_externs.reset();
  m_consts.reset();
  m_sysVars.reset();
  m_metaclassesMap.clear();
  m_enumsMap.clear();
  m_translatables.clear();

  // cleanup binary links
  binLinksClear();

  // Reset debugger pointer each time
  debuggerSet(nullptr);

  // Cleanup an exec abortion flag
  m_abort = false;

  // Terminate garbage collector
  gcStop();

  // Cleanup compiled file infos
  m_filesInfo->clear();

  ESSCRIPT_MACHINE_TRACE1(esT("Script machine reset"))
  if(!m_destroying)
  {
    // add special built-in consts
    sysConstsInit();

    // initialize system variables
    sysVarsInit();

    // re-register meta classes and prepare startup entry point
    registerPodMetaclasses();

    m_startup = globalMethodDeclare(
      EsScriptStdNames::startup(),
      EsString::nullArray()
    );
  }
}
// ---------------------------------------------------------------------------

EsVariant EsScriptMachine::sourceModuleExecutableLinesGet(const EsString& srcPath) const ES_NOTHROW
{
  ES_ASSERT(m_filesInfo);
  if(!m_filesInfo->keyExists(srcPath))
    return EsVariant::null();

  const EsVariant& val = m_filesInfo->valueGet(srcPath);
  if(!val.isEmpty())
    return val[1];

  return EsVariant::null();
}
//--------------------------------------------------------------------------------

EsVariant EsScriptMachine::sourceModuleHashGet(const EsString& srcPath) const ES_NOTHROW
{
  ES_ASSERT(m_filesInfo);
  if(!m_filesInfo->keyExists(srcPath))
    return EsVariant::null();

  const EsVariant& val = m_filesInfo->valueGet(srcPath);
  if(!val.isEmpty())
    return val[0];

  return EsVariant::null();
}
//--------------------------------------------------------------------------------

EsString::Array EsScriptMachine::sourceModulesPathsGet() const ES_NOTHROW
{
  ES_ASSERT(m_filesInfo);

  return m_filesInfo->allKeysGet().asStringCollection();
}
//--------------------------------------------------------------------------------

bool EsScriptMachine::hasSourceModule(const EsString& srcPath) const ES_NOTHROW
{
  ES_ASSERT(m_filesInfo);

  return m_filesInfo->keyExists(srcPath);
}
//--------------------------------------------------------------------------------

bool EsScriptMachine::canMarshal() const ES_NOTHROW
{
  return m_owner.get_master();
}
// ---------------------------------------------------------------------------

// internal service for optional scripted objects marshalling to the master script context
void EsScriptMachine::marshalObject(const EsScriptObjectIntf::Ptr& obj) const
{
  EsScriptIntf::Ptr master = m_owner.get_master();
	if( obj &&
			master &&
			!obj->hasClassAttribute(esT("nomarshal")) )
  {
    EsScriptMachine* otherMachine = &master->vm();
    obj->machineSet(otherMachine);
  }
}
// ---------------------------------------------------------------------------

void EsScriptMachine::marshalData(const EsVariant& data) const
{
  if( !canMarshal() )
    return;

	if( data.isObject() ) //< marshal objects
	{
		EsScriptObjectIntf::Ptr so = data.asObject();
		if( so ) //< process script object, unless it's marked with 'nomarhsal' attribute
			marshalObject(so);
	}
	else if( EsVariant::VAR_VARIANT_COLLECTION == data.typeGet() ) //< marshal variant collection
	{
		const EsVariant::Array& da = data.asVariantCollection();
		for( size_t idx = 0; idx < da.size(); ++idx )
			marshalData( da[idx] );
	}
}
// ---------------------------------------------------------------------------

EsScriptValAccessorIntf::Ptr EsScriptMachine::nullcGet() ES_NOTHROW
{
	static EsScriptValAccessorIntf::Ptr s_nullc = EsScriptSymbolTable::valAccessorCreate(
		EsScriptStdNames::null(),
    EsVariant::null(),
    EsScriptSymbolFlag::ReadOnly |
    EsScriptSymbolFlag::BuiltIn
  );

  ES_ASSERT(s_nullc);
  return s_nullc;
}
// ---------------------------------------------------------------------------

void EsScriptMachine::sysConstsInit()
{
  // add special built-in consts
  m_consts.accessorAdd(
    nullcGet()
  );

  constDeclare(
    EsScriptStdNames::version(),
    versionGet(),
    true
  );
}
// ---------------------------------------------------------------------------

void EsScriptMachine::sysVarsInit()
{
  ES_ASSERT(m_sysVars.isEmpty());
  ulong flags = EsScriptSymbolFlag::BuiltIn | EsScriptSymbolFlag::ReadOnly;

  m_sysVars.symbolNonTemplateAdd(
    EsScriptStdNames::metaclass(),
    metaGet(),
    flags
  );
  m_sysVars.symbolNonTemplateAdd(
    EsScriptStdNames::scriptHost(),
    hostGet(),
    flags
  );
}
// ---------------------------------------------------------------------------

void EsScriptMachine::externEnumDeclare(const EsString& name)
{
  // An external symbol is always considered read-only as well
  ulong flags = EsScriptSymbolFlag::ReadOnly | EsScriptSymbolFlag::ExternEnum;

  if(!m_externs.symbolExists(name))
  {
    m_externs.symbolTemplateAdd(name, flags);

    ESSCRIPT_MACHINE_TRACE2(esT("An external enum '%s' declared"), name.c_str())
  }
  else
  {
    flags = m_externs.symbolFlagsGet(name);
    if( EsScriptSymbolFlag::ExternEnum != (flags & EsScriptSymbolFlag::ExternEnum) )
      EsScriptException::Throw(
        EsString::format(
          esT("An external symbol '%s' is already declared as the other type"),
          name
        ),
        currentDebugInfoGet()
      );
  }
}
// ---------------------------------------------------------------------------

void EsScriptMachine::externMetaclassDeclare(const EsString& name)
{
  // An external symbol is always considered read-only as well
  ulong flags = EsScriptSymbolFlag::ReadOnly | EsScriptSymbolFlag::ExternObject;

  if(!m_externs.symbolExists(name))
  {
    m_externs.symbolTemplateAdd(name, flags);

	  ESSCRIPT_MACHINE_TRACE2(esT("An external object '%s' declared"), name.c_str())
  }
  else
  {
    flags = m_externs.symbolFlagsGet(name);
    if( EsScriptSymbolFlag::ExternObject != (flags & EsScriptSymbolFlag::ExternObject) )
      EsScriptException::Throw(
        EsString::format(
          esT("An external symbol '%s' is already declared as the other type"),
          name
        ),
        currentDebugInfoGet()
      );
  }
}
// ---------------------------------------------------------------------------

bool EsScriptMachine::isExternalEnum(const EsString& name) const
{
  if(m_externs.symbolExists(name))
    return EsScriptSymbolFlag::ExternEnum == (EsScriptSymbolFlag::ExternEnum & m_externs.symbolFlagsGet(name));

  return false;
}
// ---------------------------------------------------------------------------

bool EsScriptMachine::isExternalMetaclass(const EsString& name) const
{
  if(m_externs.symbolExists(name))
    return EsScriptSymbolFlag::ExternObject == (EsScriptSymbolFlag::ExternObject & m_externs.symbolFlagsGet(name));

  return false;
}
// ---------------------------------------------------------------------------

EsScriptValAccessorIntf::Ptr EsScriptMachine::constDeclare(const EsString& name, const EsVariant& val, bool builtIn /*= false*/)
{
  ulong flags = EsScriptSymbolFlag::ReadOnly;
  if(builtIn)
    flags |= EsScriptSymbolFlag::BuiltIn;

	const EsScriptValAccessorIntf::Ptr& result = m_consts.symbolNonTemplateAdd(
    name,
    val,
    flags
  );

	ESSCRIPT_MACHINE_TRACE3(
    esT("Constant '%s'='%s' created"),
    name.c_str(),
    traceVariant(val).c_str()
  )

  return result;
}
// ---------------------------------------------------------------------------

// add new unnamed const and return its internal name. If const with such value is already declared,
// return existing name for its access
EsString EsScriptMachine::unnamedDeclare(const EsVariant& val)
{
  const EsString& sym = m_consts.findFirstSymbolWithVal(val);
  if( sym.empty() )
  {
    // create internal name for unnamed value
    const EsString& name = EsString::format(
      esT("?%d?%d?"),
      m_consts.sizeGet(),
      val.typeGet()
    );

    constDeclare(
      name,
      val
    );

    return name;
  }

  return sym;
}
// ---------------------------------------------------------------------------

void EsScriptMachine::checkMethodIsNotInMap(const EsString& mapName, const EsScriptMethodMapPtr& map,
	const EsMethodInfoKeyT& key, const EsScriptDebugInfoIntf::Ptr& dbg)
{
  // find if method signature already exists in script method table
  EsScriptMethodMap::const_iterator cit = map->find(key);
  if(cit != map->end())
  {
    if(mapName.empty())
			EsScriptException::Throw(
        EsString::format(
          esT("Global script method '%s', taking %d parameters, is already declared"),
          key.nameGet(),
          key.parametersCountGet(),
          mapName
        ),
        dbg
      );
    else
			EsScriptException::Throw(
        EsString::format(
          esT("Script method '%s', taking %d parameters, is already declared in '%s'"),
          key.nameGet(),
          key.parametersCountGet(),
          mapName
        ),
        dbg
      );
  }
}
// ---------------------------------------------------------------------------

void EsScriptMachine::addMethodToMap(const EsString& mapName, EsScriptMethodMapPtr& map,
	const EsMethodInfoKeyT& key, const EsScriptCodeSection::Ptr& method,
    const EsScriptDebugInfoIntf::Ptr& dbg)
{
  checkMethodIsNotInMap(
    mapName,
    map,
    key,
    dbg
  );

  map->insert(
    std::make_pair(
      key,
      method
    )
  );

	ESSCRIPT_MACHINE_TRACE4(
    esT("Method '%s', taking %d parameters, added to the script's namespace or object '%s'"),
		method->nameGet().c_str(),
    method->inputParametersCntGet(),
    mapName.c_str()
  )
}
// ---------------------------------------------------------------------------

void EsScriptMachine::deleteMethodFromMap(const EsString& mapName, EsScriptMethodMapPtr& map, const EsMethodInfoKeyT& key)
{
  if(map->find(key) != map->end())
  {
    map->erase(key);

		ESSCRIPT_MACHINE_TRACE4(
      esT("Method '%s', taking %d parameters, removed from the script's namespace or object '%s'"),
			key.nameGet().c_str(),
      key.parametersCountGet(),
      mapName.c_str()
    )
  }
}
// ---------------------------------------------------------------------------

// create new named code section
EsScriptCodeSection::Ptr EsScriptMachine::methodDeclare(const EsString& name, const EsString::Array& params, const EsScriptObjectIntf::Ptr& metaclass)
{
  ES_ASSERT(metaclass);

  if(metaclass->isFinal())
		EsScriptException::ThrowFinalObjectMayNotBeModified(
      metaclass->typeNameGet(),
			currentDebugInfoGet()
    );

  if(metaclass->isIf())
		EsScriptException::Throw(
      esT("Methods may not be declared in 'if' conditional object scope"),
      currentDebugInfoGet()
    );

	EsScriptCodeSection::Ptr result = EsScriptCodeSection::create(
    name,
    params,
    metaclass.get()
  );
  ES_ASSERT(result);

  EsMethodInfoKeyT key = methodKeyCreate(result);
  ES_ASSERT(key.isOk());

  // add method to the scoped method map
  EsScriptMethodMapPtr scopedMethods = metaclass->thisScriptedMethodsGet();
  // todo: issue hints if method may hide| be hidden by existing one
	addMethodToMap(
    metaclass->typeNameGet(),
    scopedMethods,
    key,
    result,
    currentDebugInfoGet()
  );

  return result;
}
// ---------------------------------------------------------------------------

EsScriptCodeSection::Ptr EsScriptMachine::globalMethodDeclare(const EsString& name, const EsString::Array& params)
{
  if( EsString::cmpEqual == name.compare(EsStdNames::reflectedCtr()) ||
      EsString::cmpEqual == name.compare(EsStdNames::reflectedDtr())
  )
    EsScriptException::Throw(
      EsString::format(
        esT("'%s' may not be declared outside of object declaration"),
        name
      ),
      currentDebugInfoGet()
    );

  EsScriptCodeSection::Ptr result = EsScriptCodeSection::create(
    name,
    params
  );
  EsMethodInfoKeyT key = methodKeyCreate(result);
  ES_ASSERT(key.isOk());

	addMethodToMap(
    EsString::null(),
    m_globalMethods,
    key,
    result,
    currentDebugInfoGet()
  );

  return result;
}
// ---------------------------------------------------------------------------

#include "build.h"

EsString EsScriptMachine::versionGet() const
{
	return COMPONENT_VERSION_STRING;
}
//---------------------------------------------------------------------------

void EsScriptMachine::linkAdd(const EsString& link)
{
	ES_ASSERT( !link.empty() );
	const EsString& linkName = EsScript::absoluteFileNameGet(
    link,
    m_owner.linkPathsGet()
  );

	if( !linkName.empty() )
	{
		const EsPath& fname = EsPath::createFromFilePath(linkName);
		const EsString& fullFname = fname.pathGet();
		const EsStringIndexedMap& loadedList = EsDynamicLibrary::loadedLibrariesGet();

		bool loaded = false;

		for(size_t idx = 0; idx < loadedList.countGet(); ++idx)
		{
			const EsString& loadedFname = loadedList.nameGet(idx);
			if( EsString::cmpEqual == EsString::scompare(fullFname, loadedFname, true, m_loc) )
			{
				loaded = true;
				break;
			}
		}

		if( m_links.end() == m_links.find( fname.fileNameExtGet() ) &&
				!loaded
    )
		{
      const EsString& name = fname.fileNameGet();
			EsDynamicLibrary::Ptr lib = EsDynamicLibrary::load( fullFname, false );
			if( lib )
			{
        // Try to locate reflection registrar entry
        EsDynamicLibrary::Pfn pfn = lib->procAddrGet(
          EsString::format(
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# if defined(_WIN64)
            esT("_%s_reflectionInitDynamic"),
# else
            esT("__%s_reflectionInitDynamic"),
# endif
#else
            esT("__%s_reflectionInitDynamic"),
#endif
            name
          )
        );

        if( pfn )
        {
          // Dynamically register foreign reflected classes in our own reflection registry
          reinterpret_cast<void (*)(void*)>(
            pfn
          )( EsClassInfo::classes );
        }

				m_links[ fname.fileNameExtGet() ] = lib;
				loaded = true;
			}
		}

		if( !loaded )
		{
			EsScriptException::Throw(
				EsString::format(
          esT("Could not link '%s', failed to load file ('%s')"),
				  linkName,
				  EsUtilities::osErrorStringGet(EsUtilities::osErrorCodeGet())
        ),
				currentDebugInfoGet()
      );
		}
	}
	else
		EsScriptException::Throw(
			EsString::format(
        esT("Could not link '%s', file was not found"),
        link
      ),
			currentDebugInfoGet()
    );
}
//---------------------------------------------------------------------------

const EsScriptSymbolTable& EsScriptMachine::vars() const ES_NOTHROW
{
	ES_ASSERT(m_startup);
	return m_startup->variablesAccess();
}
//---------------------------------------------------------------------------

EsScriptCodeSection::Ptr EsScriptMachine::globalMethodGet(const EsMethodInfoKeyT& key, bool doThrow /*= true*/) const
{
	if( !m_globalMethods )
#ifdef ES_MODERN_CPP
    return nullptr;
#else
    return EsScriptCodeSection::Ptr();
#endif

  // find if method signature exists in script method table
  EsScriptMethodMap::const_iterator cit = m_globalMethods->find(key);
  if( cit != m_globalMethods->end() )
    return (*cit).second;

  if( doThrow )
    EsScriptException::Throw(
      EsString::format(
        esT("Global script function '%s', taking %d parameters, is not declared"),
        key.nameGet(),
        key.parametersCountGet()
      )
    );

#ifdef ES_MODERN_CPP
    return nullptr;
#else
    return EsScriptCodeSection::Ptr();
#endif
}
//---------------------------------------------------------------------------

bool EsScriptMachine::hasGlobalMethod(const EsMethodInfoKeyT& key) const ES_NOTHROW
{
  return nullptr != globalMethodGet(key, false);
}
//---------------------------------------------------------------------------

EsMethodInfoKeyT EsScriptMachine::methodKeyCreate(const EsScriptCodeSection::Ptr& code)
{
  if(code)
    return EsMethodInfoKeyT(code->inputParametersCntGet(), code->nameGet());

  return EsMethodInfoKeyT();
}
//---------------------------------------------------------------------------

EsEnumerationIntf::Ptr EsScriptMachine::enumFind(const EsString& name, bool doThrow /*= false*/) const
{
  size_t idx = m_enumsMap.itemFind(name);
  if(static_cast<size_t>(-1) != idx)
    return m_enumsMap.valueGet(idx).asExistingObject();
  else
  {
    // search cpp reflected enums
    const EsClassInfo* info = EsClassInfo::classInfoGet(name, false);
    if(info && info->isKindOf(esT("EsEnumeration")))
      return info->classCall(esT("instance")).asExistingObject();
  }

  if(doThrow)
		EsScriptException::Throw(
      EsString::format(
        esT("Enumeration '%s' is not defined"),
			  name
      ),
      currentDebugInfoGet()
    );

  return EsEnumerationIntf::Ptr();
}
//---------------------------------------------------------------------------

EsEnumerationIntf::Ptr EsScriptMachine::enumDeclare(const EsString& name)
{
  if(enumFind(name))
		EsScriptException::Throw(
      EsString::format(
        esT("Enumeration '%s' is already defined"),
        name
      ),
      currentDebugInfoGet()
    );

	if( metaclassFind(name) ||
			EsClassInfo::classInfoGet(name)
  )
	  EsScriptException::Throw(
      EsString::format(
        esT("'%s' is already defined as class"),
        name
      ),
      currentDebugInfoGet()
    );

  EsEnumerationIntf::Ptr result = EsScriptEnumeration::create(
    *this,
    name
  );
  ES_ASSERT(result);

  m_enumsMap.itemAdd(
    name,
    result
  );

  return result;
}
//---------------------------------------------------------------------------

void EsScriptMachine::enumAttributeDeclare(const EsEnumerationIntf::Ptr& enu, const EsString& name, const EsVariant& val)
{
  ES_ASSERT(enu);
  EsReflectedClassIntf::Ptr r = enu;

  ES_ASSERT(r);
  r->attributeAdd(name, val);
}
//---------------------------------------------------------------------------

void EsScriptMachine::enumItemDeclare(const EsEnumerationIntf::Ptr& enu, const EsString& symbol,
																			const EsVariant& val, const EsString& label /*= EsString::null()*/)
{
  ES_ASSERT(enu);
  enu->itemAdd(
    symbol,
    val,
    label
  );
}
//---------------------------------------------------------------------------

EsVariant EsScriptMachine::enumValueGet(const EsString& name, const EsString& sym)
{
  EsReflectedClassIntf::Ptr enu = enumFind(name, true);
  if(enu->call(esT("hasSymbol"), sym).asBool())
    return enu->call(esT("valueGet"), sym);

  EsScriptException::Throw(
    EsString::format(
      loc(),
      esT("'%s' is not a member of enumeration '%s'"),
      sym,
      name
    ),
    currentDebugInfoGet()
  );

  // pacify compiler
  return EsVariant::null();
}
//---------------------------------------------------------------------------

EsVariant EsScriptMachine::enumValueLabelGet(const EsString& name, const EsString& sym)
{
  EsReflectedClassIntf::Ptr enu = enumFind(name, true);
  if(enu->call(esT("hasSymbol"), sym).asBool())
    return enu->call(esT("symbolLabelGet"), sym);

  EsScriptException::Throw(
    EsString::format(
      loc(),
      esT("'%s' is not a member of enumeration '%s'"),
      sym,
      name
    ),
    currentDebugInfoGet()
  );

  // pacify compiler
  return EsVariant::null();
}
// ---------------------------------------------------------------------------

EsScriptObjectIntf::Ptr EsScriptMachine::metaclassFind(const EsString &name, bool doThrow /*= false*/) const
{
  size_t idx = m_metaclassesMap.itemFind(name);
  if(static_cast<size_t>(-1) != idx)
    return m_metaclassesMap.valueGet(idx).asExistingObject();

  if(doThrow)
    EsScriptException::ThrowMetaclassDoesNotExist(name, currentDebugInfoGet());

  return nullptr; //< Pacify compiler
}
//---------------------------------------------------------------------------

void EsScriptMachine::registerMetaclass(const EsScriptObjectIntf::Ptr& metaclass)
{
  ES_ASSERT(metaclass);
  const EsString& name = metaclass->typeNameGet();

  ES_ASSERT(!metaclassFind(name));
  m_metaclassesMap.itemAdd(
    name,
    metaclass
  );

	ESSCRIPT_MACHINE_TRACE2(
    esT("Metaclass '%s' declaration added to script machine"),
    name.c_str()
  )
}
//---------------------------------------------------------------------------

void EsScriptMachine::registerPodMetaclasses()
{
  registerMetaclass(EsScript_esU8::createMetaclass(m_ctxScriptThis));
  registerMetaclass(EsScript_esU16::createMetaclass(m_ctxScriptThis));
  registerMetaclass(EsScript_esU32::createMetaclass(m_ctxScriptThis));
  registerMetaclass(EsScript_esU64::createMetaclass(m_ctxScriptThis));
  registerMetaclass(EsScript_esI8::createMetaclass(m_ctxScriptThis));
  registerMetaclass(EsScript_esI16::createMetaclass(m_ctxScriptThis));
  registerMetaclass(EsScript_esI32::createMetaclass(m_ctxScriptThis));
  registerMetaclass(EsScript_esI64::createMetaclass(m_ctxScriptThis));
  registerMetaclass(EsScript_esDT::createMetaclass(m_ctxScriptThis));
  registerMetaclass(EsScript_esF::createMetaclass(m_ctxScriptThis));
}
//---------------------------------------------------------------------------

EsScriptObjectIntf::Ptr EsScriptMachine::metaclassDeclare(const EsString &name, const EsString &baseName)
{
  EsScriptObjectIntf::Ptr result;
  // search class name among C++ reflected classes
  const EsClassInfo* info = EsClassInfo::classInfoGet(name);
  if(info)
    EsScriptException::ThrowMetaclassAlreadyExists(
      name,
      currentDebugInfoGet()
    );

  // search class name among enumerations
  if(enumFind(name))
		EsScriptException::Throw(
      EsString::format(
        esT("'%s' already defined as enumeration"),
        name
      ),
      currentDebugInfoGet()
    );

  EsScriptObjectIntf::Ptr baseMetaclass;
  if(!baseName.empty())
    baseMetaclass = metaclassFind(
      baseName,
      true
    );

  // check for existing metaclasses
  size_t idx = m_metaclassesMap.itemFind(name);
  if(static_cast<size_t>(-1) != idx)
    EsScriptException::ThrowMetaclassAlreadyExists(
      name,
      currentDebugInfoGet()
    );

  EsScriptMethodMapPtr methods;
  methods.reset(new EsScriptMethodMap);

	EsScriptObjectIntf::Ptr metaclass(
		new EsScriptObject(
      name,
      baseMetaclass,
      methods,
      m_ctxScriptThis,
      EsScriptObject::ofMetaclass,
		  EsScriptObjectDataBuffer::null(),
		  EsAttributes::create(
        name,
        false
      )
    )
  );

  // register metaclass
  registerMetaclass(metaclass);

  return metaclass;
}
//---------------------------------------------------------------------------

EsScriptObjectIntf::Ptr EsScriptMachine::metaclassFieldDeclare(
  const EsString& metaclassName,
	const EsString& fieldType,
  const EsString& fieldName,
  const EsAttributesIntf::Ptr& attrs
)
{
  EsScriptObjectIntf::Ptr metaclass = metaclassFind(metaclassName, true);
  return metaclassFieldDeclare(
    metaclass,
    fieldType,
    fieldName,
    attrs
  );
}
//---------------------------------------------------------------------------

EsScriptObjectIntf::Ptr EsScriptMachine::metaclassFieldDeclare(
  const EsScriptObjectIntf::Ptr& metaclass,
	const EsString& fieldType,
  const EsString& fieldName,
  const EsAttributesIntf::Ptr& attrs
)
{
  EsScriptObjectIntf::Ptr fieldMetaclass = metaclassFind(fieldType, true);
  // append field in metaclass declaration mode
  metaclass->fieldAdd(
    fieldName,
    fieldMetaclass,
    attrs
  );

  return fieldMetaclass;
}
//---------------------------------------------------------------------------

EsScriptMachine::CompoundFieldCreationResult EsScriptMachine::metaclassArrayFieldDeclare(
  const EsString& metaclassName,
	const EsString& fieldType,
  const EsString& fieldName,
  const EsAttributesIntf::Ptr& attrs
)
{
  EsScriptObjectIntf::Ptr metaclass = metaclassFind(metaclassName, true);
  return metaclassArrayFieldDeclare(
    metaclass,
    fieldType,
    fieldName,
    attrs
  );
}
//---------------------------------------------------------------------------

EsScriptMachine::CompoundFieldCreationResult EsScriptMachine::metaclassArrayFieldDeclare(const EsScriptObjectIntf::Ptr& metaclass,
	const EsString& fieldType, const EsString& fieldName, const EsAttributesIntf::Ptr& attrs)
{
  EsScriptObjectIntf::Ptr itemMetaclass = metaclassFind(fieldType, true);

  // append field in metaclass declaration mode
  CompoundFieldCreationResult result;
	result.m_fieldMetaclass = EsScriptArrayObject::createMetaclass(m_ctxScriptThis, itemMetaclass);
  ES_ASSERT(result.m_fieldMetaclass);

  result.m_fieldExpr = result.m_fieldMetaclass->thisFieldExprGet();
  metaclass->fieldAdd(
    fieldName,
    result.m_fieldMetaclass,
    attrs
  );

  return result;
}
//---------------------------------------------------------------------------

EsScriptMachine::CompoundFieldCreationResult EsScriptMachine::metaclassIfFieldDeclare(const EsString& metaclassName)
{
  EsScriptObjectIntf::Ptr metaclass = metaclassFind(metaclassName, true);
  return metaclassIfFieldDeclare(metaclass);
}
//---------------------------------------------------------------------------

EsScriptMachine::CompoundFieldCreationResult EsScriptMachine::metaclassIfFieldDeclare(const EsScriptObjectIntf::Ptr& metaclass)
{
  // append field in metaclass declaration mode
  CompoundFieldCreationResult result;
  result.m_fieldMetaclass = EsScriptIfObject::createMetaclass(m_ctxScriptThis);
  ES_ASSERT(result.m_fieldMetaclass);

  result.m_fieldExpr = result.m_fieldMetaclass->thisFieldExprGet();
  metaclass->fieldConditionalAdd(result.m_fieldMetaclass);

  return result;
}
//---------------------------------------------------------------------------

void EsScriptMachine::objectAttributeDeclare(
  const EsScriptObjectIntf::Ptr& obj,
  const EsString& attrName,
  const EsVariant& attrVal
)
{
  ES_ASSERT(obj);
  obj->addMetaclassAttribute(attrName, attrVal);
}
//---------------------------------------------------------------------------

void EsScriptMachine::functionAttributeDeclare(
  const EsScriptCodeSection::Ptr& function,
	const EsString& attrName,
  const EsVariant& attrVal
)
{
  ES_ASSERT(function);
  function->m_attrs->attributeAdd(attrName, attrVal);
}
//---------------------------------------------------------------------------

void EsScriptMachine::metaclassMemberVarDeclare(
  const EsScriptObjectIntf::Ptr& obj,
  const EsString& name,
  const EsScriptDebugInfoIntf::Ptr& dbg /* = EsScriptDebugInfoIntf::Ptr() */
)
{
  ES_ASSERT(obj);
  obj->variableDeclare(name, dbg);
}
//---------------------------------------------------------------------------

EsScriptThreadContext* EsScriptMachine::threadCtxFindInternal(EsThreadId threadId) const
{
  if( m_ctxCur && threadId == m_ctxCur->threadId() )
    return m_ctxCur;

#ifdef ES_MODERN_CPP
  auto cit
#else
  std::map<EsThreadId, EsScriptThreadContext*>::const_iterator cit
#endif
    = m_ctxs.find(threadId);

  if( cit != m_ctxs.end() )
    return (*cit).second;

  return nullptr;
}
//---------------------------------------------------------------------------

EsScriptThreadContext* EsScriptMachine::threadCtxFind(EsThreadId threadId) const
{
  EsCriticalSectionLocker lock(m_cs);
  return threadCtxFindInternal(threadId);
}
//---------------------------------------------------------------------------

void EsScriptMachine::threadCtxRemove(EsThreadId threadId)
{
  EsCriticalSectionLocker lock(m_cs);
#ifdef ES_MODERN_CPP
  auto it
#else
  std::map<EsThreadId, EsScriptThreadContext*>::iterator it
#endif
     = m_ctxs.find(threadId);

  if( it != m_ctxs.end() )
  {
    if( m_ctxCur && threadId == m_ctxCur->threadId() )
    {
      ESSCRIPT_MACHINE_TRACE2( esT("EsScriptMachine::threadCtxRemove: context 0x%08X cache nullified"), (int)threadId )
      m_ctxCur = nullptr;
    }

    m_ctxs.erase(it);

    ESSCRIPT_MACHINE_TRACE2( esT("EsScriptMachine::threadCtxRemove: context 0x%08X removed"), (int)threadId )
  }
}
//---------------------------------------------------------------------------

void EsScriptMachine::threadCtxCleanup()
{
  ESSCRIPT_MACHINE_TRACE1( esT("EsScriptMachine::threadCtxCleanup - Running maintenance...") )

  EsCriticalSectionLocker lock(m_cs);

  for(
#ifdef ES_MODERN_CPP
  auto it
#else
  std::map<EsThreadId, EsScriptThreadContext*>::iterator it
#endif
    = m_ctxs.begin(); it != m_ctxs.end(); )
  {
    EsScriptThreadContext* ctx = (*it).second;
    ES_ASSERT(ctx);

    if( !ctx->isExecuting() )
    {
      m_ctxs.erase(it++);
      if( m_ctxCur && m_ctxCur->threadId() == ctx->threadId() )
        m_ctxCur = nullptr;

      ESSCRIPT_MACHINE_TRACE2( esT("EsScriptMachine::threadCtxCleanup: context 0x%08X removed"), (int)ctx->threadId() )

      ES_DELETE(ctx);
    }
    else
      ++it;
  }
}
//---------------------------------------------------------------------------

EsScriptThreadContext* EsScriptMachine::threadCtxGet(EsThreadId threadId)
{
  EsCriticalSectionLocker lock(m_cs);
  EsScriptThreadContext* ctx = threadCtxFindInternal(threadId);
  if( ctx )
  {
    ESSCRIPT_MACHINE_TRACE2( esT("EsScriptMachine::threadCtxGet: Existing Thread context 0x%08X found"), (int)threadId )

    if( m_ctxCur != ctx )
    {
      ESSCRIPT_MACHINE_TRACE1( esT("... and is set current") )
      m_ctxCur = ctx;
    }

    return ctx;
  }

  std::unique_ptr<EsScriptThreadContext> tmp(
    new EsScriptThreadContext(
      *this,
      threadId
    )
  );
  ES_ASSERT( tmp );

  ESSCRIPT_MACHINE_TRACE2( esT("EsScriptMachine::threadCtxGet: New Thread context 0x%08X created and is set current"), (int)threadId )

  m_ctxs[threadId] = tmp.get();
  m_ctxCur = tmp.release();

  return m_ctxCur;
}
//---------------------------------------------------------------------------

void EsScriptMachine::threadCtxsReset()
{
  EsCriticalSectionLocker lock(m_cs);

  m_ctxCur = nullptr;

#ifdef ES_MODERN_CPP
  for( auto &v: m_ctxs )
  {

#else
  for( std::map<EsThreadId, EsScriptThreadContext*>::iterator it = m_ctxs.begin(); it != m_ctxs.end(); ++it )
  {
    std::map<EsThreadId, EsScriptThreadContext*>::value_type& v = (*it);

#endif

    ES_DELETE(
      v.second
    );
  }

  m_ctxs.clear();

  ESSCRIPT_MACHINE_TRACE1( esT("EsScriptMachine::threadCtxsReset()") )
}
//---------------------------------------------------------------------------

EsScriptValAccessorIntf::Ptr EsScriptMachine::exec(
  const EsScriptCodeSection::Ptr& cs,
  const EsVariant& params,
  EsScriptMachine::EvalMode evalMode,
  EsScriptObjectIntf* This
)
{
  EsScriptThreadContext* ctx = threadCtxGet(
    EsThread::currentIdGet()
  );
  ES_ASSERT(ctx);

  return ctx->exec(
    cs,
    params,
    evalMode,
    This
  );
}
//---------------------------------------------------------------------------

EsVariant EsScriptMachine::callGlobalMethod(
  const EsString& name,
  const EsVariant& params
)
{
  EsScriptThreadContext* ctx = threadCtxGet(
    EsThread::currentIdGet()
  );
  ES_ASSERT(ctx);

  rewindInternal(true);

  return ctx->callGlobalMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScriptMachine::callGlobalMethod(
  const EsMethodInfoKeyT& key,
  const EsVariant& params
)
{
  EsScriptThreadContext* ctx = threadCtxGet(
    EsThread::currentIdGet()
  );
  ES_ASSERT(ctx);

  rewindInternal(true);

  return ctx->callGlobalMethod(
    key,
    params
  );
}
//---------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsScriptMachine::objectCreateWithParameters(
  const EsString& name,
  const EsVariant& params
)
{
  EsScriptThreadContext* ctx = threadCtxGet(
    EsThread::currentIdGet()
  );
  ES_ASSERT(ctx);

  return ctx->objectCreateWithParameters(
    name,
    params
  );
}
//---------------------------------------------------------------------------

void EsScriptMachine::gcStop()
{
  m_gc.stopAndWait();
}
//---------------------------------------------------------------------------

void EsScriptMachine::rewindInternal(bool startGC)
{
  m_abort = false;

  // Start garbage collector thread, if needed
  if( startGC && EsThreadState::None == m_gc.stateGet() )
    m_gc.start(
      static_cast<ulong>(EsThreadPriority::Minimal)
    );
}
//--------------------------------------------------------------------------------

void EsScriptMachine::rewind(bool startGC /*= true*/)
{
  ESSCRIPT_MACHINE_TRACE1( esT("EsScriptMachine::rewind()") )

  threadCtxsReset();
  rewindInternal(startGC);
}
//---------------------------------------------------------------------------

void EsScriptMachine::translatableAdd(const EsString& strVal)
{
  if( !m_translatables.itemExists(strVal) )
    m_translatables.itemAdd(strVal);
}
//---------------------------------------------------------------------------

EsString::Array EsScriptMachine::translatablesGet() const
{
  return m_translatables.namesGet();
}
//---------------------------------------------------------------------------

EsVariant EsScriptMachine::exec()
{
  // rewind machine
  rewind();

  EsScriptThreadContext* ctx = threadCtxGet(
    EsThread::currentIdGet()
  );
  ES_ASSERT(ctx);

	return ctx->exec();
}
//---------------------------------------------------------------------------

EsVariant EsScriptMachine::call(const EsString& name)
{
  return callGlobalMethod(
    name,
    EsVariant::null()
  );
}
//---------------------------------------------------------------------------

EsVariant EsScriptMachine::call(const EsString& name, const EsVariant& p0)
{
#ifdef ES_MODERN_CPP
  EsVariant::Array params = {
    p0
  };

#else
  EsVariant::Array params;
  params.reserve(1);

  params.push_back(p0);
#endif

  return callGlobalMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScriptMachine::call(const EsString& name, const EsVariant& p0, const EsVariant& p1)
{
#ifdef ES_MODERN_CPP
  EsVariant::Array params = {
		p0,
		p1
  };

#else
  EsVariant::Array params;
  params.reserve(2);

  params.push_back(p0);
	params.push_back(p1);
#endif

  return callGlobalMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScriptMachine::call(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2)
{
#ifdef ES_MODERN_CPP
  EsVariant::Array params = {
		p0,
		p1,
    p2
  };

#else
  EsVariant::Array params;
  params.reserve(3);

  params.push_back(p0);
	params.push_back(p1);
	params.push_back(p2);
#endif

  return callGlobalMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScriptMachine::call(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3)
{
#ifdef ES_MODERN_CPP
  EsVariant::Array params = {
		p0,
		p1,
    p2,
    p3
  };

#else
  EsVariant::Array params;
  params.reserve(4);

  params.push_back(p0);
	params.push_back(p1);
	params.push_back(p2);
	params.push_back(p3);
#endif

  return callGlobalMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScriptMachine::call(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3, const EsVariant& p4)
{
#ifdef ES_MODERN_CPP
  EsVariant::Array params = {
		p0,
		p1,
    p2,
    p3,
    p4
  };

#else
  EsVariant::Array params;
  params.reserve(5);

  params.push_back(p0);
	params.push_back(p1);
	params.push_back(p2);
	params.push_back(p3);
	params.push_back(p4);
#endif

  return callGlobalMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScriptMachine::call(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3, const EsVariant& p4, const EsVariant& p5)
{
#ifdef ES_MODERN_CPP
  EsVariant::Array params = {
		p0,
		p1,
    p2,
    p3,
    p4,
    p5
  };

#else
  EsVariant::Array params;
  params.reserve(6);

  params.push_back(p0);
	params.push_back(p1);
	params.push_back(p2);
	params.push_back(p3);
	params.push_back(p4);
	params.push_back(p5);
#endif

  return callGlobalMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScriptMachine::call(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3, const EsVariant& p4, const EsVariant& p5, const EsVariant& p6)
{
#ifdef ES_MODERN_CPP
  EsVariant::Array params = {
		p0,
		p1,
    p2,
    p3,
    p4,
    p5,
    p6
  };

#else
  EsVariant::Array params;
  params.reserve(7);

  params.push_back(p0);
	params.push_back(p1);
	params.push_back(p2);
	params.push_back(p3);
	params.push_back(p4);
	params.push_back(p5);
	params.push_back(p6);
#endif

  return callGlobalMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsScriptMachine::objectCreate(const EsString& name)
{
  return objectCreateWithParameters(
    name,
    EsVariant::null()
  );
}
//---------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsScriptMachine::objectCreate(const EsString& name, const EsVariant& p0)
{
#ifdef ES_MODERN_CPP
  EsVariant::Array ps = {
		p0
  };

#else
  EsVariant::Array ps;
  ps.reserve(1);

  ps.push_back(p0);
#endif

  return objectCreateWithParameters(
    name,
    ps
  );
}
//---------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsScriptMachine::objectCreate(const EsString& name, const EsVariant& p0, const EsVariant& p1)
{
#ifdef ES_MODERN_CPP
  EsVariant::Array ps = {
		p0,
		p1
  };

#else
  EsVariant::Array ps;
  ps.reserve(2);

  ps.push_back(p0);
	ps.push_back(p1);
#endif

  return objectCreateWithParameters(
    name,
    ps
  );
}
//---------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsScriptMachine::objectCreate(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2)
{
#ifdef ES_MODERN_CPP
  EsVariant::Array ps = {
		p0,
		p1,
    p2
  };

#else
  EsVariant::Array ps;
  ps.reserve(3);

  ps.push_back(p0);
	ps.push_back(p1);
	ps.push_back(p2);
#endif

  return objectCreateWithParameters(
    name,
    ps
  );
}
//---------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsScriptMachine::objectCreate(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3)
{
#ifdef ES_MODERN_CPP
  EsVariant::Array ps = {
		p0,
		p1,
    p2,
    p3
  };

#else
  EsVariant::Array ps;
  ps.reserve(4);

  ps.push_back(p0);
	ps.push_back(p1);
	ps.push_back(p2);
	ps.push_back(p3);
#endif

  return objectCreateWithParameters(
    name,
    ps
  );
}
//---------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsScriptMachine::objectCreate(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3, const EsVariant& p4)
{
#ifdef ES_MODERN_CPP
  EsVariant::Array ps = {
		p0,
		p1,
    p2,
    p3,
    p4
  };

#else
  EsVariant::Array ps;
  ps.reserve(5);

  ps.push_back(p0);
	ps.push_back(p1);
	ps.push_back(p2);
	ps.push_back(p3);
	ps.push_back(p4);
#endif

  return objectCreateWithParameters(
    name,
    ps
  );
}
//---------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsScriptMachine::objectCreate(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3, const EsVariant& p4, const EsVariant& p5)
{
#ifdef ES_MODERN_CPP
  EsVariant::Array ps = {
		p0,
		p1,
    p2,
    p3,
    p4,
    p5
  };

#else
  EsVariant::Array ps;
  ps.reserve(6);

  ps.push_back(p0);
	ps.push_back(p1);
	ps.push_back(p2);
	ps.push_back(p3);
	ps.push_back(p4);
	ps.push_back(p5);
#endif

  return objectCreateWithParameters(
    name,
    ps
  );
}
//---------------------------------------------------------------------------

