#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptlet.h"
#include "EsScriptCompiler.h"

// non-exported reflected services headers
#include "EsScriptDebug.h"
#include "EsStr.h"
#include "EsVar.h"
#include "EsCore.h"
#include "EsThreadWorker.h"
#include "EsEventHandler.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

#include "autolink.cxx"
//---------------------------------------------------------------------------

ES_REFLECTION_REGISTRAR_BEGIN(esscript)
	ES_REFLECTION_REGISTRAR_ENTRY(EsScriptDebug)
	ES_REFLECTION_REGISTRAR_ENTRY(EsVar)
	ES_REFLECTION_REGISTRAR_ENTRY(EsStrFlag)
	ES_REFLECTION_REGISTRAR_ENTRY(EsStrByteEncoding)
	ES_REFLECTION_REGISTRAR_ENTRY(EsStr)
	ES_REFLECTION_REGISTRAR_ENTRY(EsScriptMetaclass)
  ES_REFLECTION_REGISTRAR_ENTRY(EsScriptMetainfoLookup)
  ES_REFLECTION_REGISTRAR_ENTRY(EsScriptEnumeration)
	ES_REFLECTION_REGISTRAR_ENTRY(EsScriptObject)
	ES_REFLECTION_REGISTRAR_ENTRY(EsExceptionSeverity)
	ES_REFLECTION_REGISTRAR_ENTRY(EsExceptionFacility)
	ES_REFLECTION_REGISTRAR_ENTRY(EsExceptionInfo)
	ES_REFLECTION_REGISTRAR_ENTRY(EsScriptHost)
	ES_REFLECTION_REGISTRAR_ENTRY(EsCore)
	ES_REFLECTION_REGISTRAR_ENTRY(EsUtils)
	ES_REFLECTION_REGISTRAR_ENTRY(EsScript)
#if 1 == ES_USE_SCRIPTLETS
	ES_REFLECTION_REGISTRAR_ENTRY(EsScriptlet)
#endif
	ES_REFLECTION_REGISTRAR_ENTRY(EsThreadWorker)
	ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsThreadState)
	ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsThreadPriority)
	ES_REFLECTION_REGISTRAR_ENTRY(EsEventHandler)
ES_REFLECTION_REGISTRAR_END
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_CUSTOM_BEGIN(EsScript, NO_CLASS_DESCR)
  // Reflected interface
  //
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, includePathAdd, void_Call_cr_EsString_bool, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, includePathsClear, void_Call, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, linkPathAdd, void_Call_cr_EsString_bool, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, linkPathsClear, void_Call, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, compileText, bool_Call_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, compileFile, bool_Call_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, compiledBinaryGet, EsBinBuffer_CallConst_bool, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, compiledBinarySet, void_Call_cr_EsBinBuffer_bool, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, compiledBinaryLoad, EsBinBuffer_Call_cr_EsString_bool_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, marshal, void_CallConst_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, execAbort, void_Call, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, rewind, void_Call, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, exec, EsVariant_Call, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, exec, EsVariant_Call_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, exec, EsVariant_Call_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, exec, EsVariant_Call_cr_EsString_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, exec, EsVariant_Call_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, exec, EsVariant_Call_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, exec, EsVariant_Call_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, constantGet, EsVariant_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, variableGet, EsVariant_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, variableSet, void_Call_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, objectCreate, EsBaseIntfPtr_Call_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, objectCreate, EsBaseIntfPtr_Call_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, objectCreate, EsBaseIntfPtr_Call_cr_EsString_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, objectCreate, EsBaseIntfPtr_Call_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, objectCreate, EsBaseIntfPtr_Call_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, objectCreate, EsBaseIntfPtr_Call_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScript, EsScriptIntf, translatablesGet, EsStringArray_CallConst, NO_METHOD_DESCR)

  // Reflected services
  //
	ES_DECL_REFLECTED_CTOR_INFO(EsScript, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CTOR_INFO(EsScript, EsBaseIntfPtr_ClassCall_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
#if	defined(ES_USE_SCRIPTLETS) && 1 == ES_USE_SCRIPTLETS
	ES_DECL_REFLECTED_METHOD_INFO(EsScript, scriptletCreateReflected, scriptletCreate, EsBaseIntfPtr_Call_cr_EsString_cr_EsString_cr_EsString, NO_METHOD_DESCR)
#endif
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScript, enumEval, EsVariant_Call_cr_EsVariant, NO_METHOD_DESCR)

  // Properties
  //
	ES_DECL_PROP_INFO_RO(EsScript, compiled, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RO(EsScript, meta, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_PERSISTENT(EsScript, includePaths, EsStringArray, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_PERSISTENT(EsScript, linkPaths, EsStringArray, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO(EsScript, master, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO(EsScript, progressMonitor, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RO(EsScript, translatables, EsStringArray, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsScript::EsScript(const EsScriptIntf::Ptr& master) :
m_machine(*this),
#if	defined(ES_USE_SCRIPTLETS) && 1 == ES_USE_SCRIPTLETS
m_scriptlets(EsStringIndexedMap::ContainerWithoutInterlock),
#endif
m_compiled(false)
{
  m_dynamic = true;
	set_master(master);
}
//---------------------------------------------------------------------------

EsScript::~EsScript()
{
#if	defined(ES_USE_SCRIPTLETS) && 1 == ES_USE_SCRIPTLETS
	scriptletsReset();
#endif
}
//---------------------------------------------------------------------------

EsScriptIntf::Ptr EsScript::create(const EsScriptIntf::Ptr& master /*= EsScriptIntf::Ptr()*/)
{
	std::unique_ptr<EsScript> p(new EsScript(master));
	ES_ASSERT(p.get());
	return p.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsScript::NEW()
{
	return create();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsScript::NEW(cr_EsBaseIntfPtr master)
{
	return create(master);
}
//---------------------------------------------------------------------------

EsVariant EsScript::enumEval(const EsVariant& enumValExpr)
{
  EsRegEx re(
    esT("([_a-zA-Z]+[_a-zA-Z0-9]*)\\$\\$([_a-zA-Z]+[_a-zA-Z0-9]*)(\\$\\$label)?")
  );

  const EsString& expr = enumValExpr.asString();
  re.set_text(expr);
  if( !re.get_matches() )
    EsScriptException::ThrowCouldNotParseMicroExpression(
      expr,
      __func__,
      m_machine.currentDebugInfoGet()
    );

  const EsString& enumName = re.matchGet(1);
  ES_ASSERT(!enumName.empty());

  const EsString& enumSym = re.matchGet(2);
  ES_ASSERT(!enumSym.empty());

  ulong start, len;
  if( re.matchGet(start, len, 3) ) //< Return label expression
    return m_machine.enumValueLabelGet(
      enumName,
      enumSym
    );

  // Otherwise, return enum symbol value
  return m_machine.enumValueGet(
    enumName,
    enumSym
  );
}
//---------------------------------------------------------------------------

bool EsScript::get_compiled() const
{
	return isCompiled();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsScript::get_meta() const
{
	return vm().metaGet();
}
//---------------------------------------------------------------------------

EsStringArray EsScript::get_includePaths() const
{
	return includePathsGet();
}
//---------------------------------------------------------------------------

void EsScript::set_includePaths(const EsStringArray& paths)
{
	includePathsSet(
    paths,
    false
  );
}
//---------------------------------------------------------------------------

EsStringArray EsScript::get_linkPaths() const
{
	return linkPathsGet();
}
//---------------------------------------------------------------------------

void EsScript::set_linkPaths(const EsStringArray& paths)
{
	linkPathsSet(
    paths,
    false
  );
}
//---------------------------------------------------------------------------

void EsScript::set_master(const EsBaseIntfPtr& master)
{
	EsScriptIntf::Ptr s = master;
	EsScriptIntf::Ptr weakMaster(
    s.get(),
    false, false);

	m_master = weakMaster;
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsScript::get_master() const
{
	return m_master;
}
//---------------------------------------------------------------------------

void EsScript::set_progressMonitor(const EsBaseIntfPtr& monitor)
{
	progressMonitorSet(monitor);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsScript::get_progressMonitor() const
{
	return progressMonitorGet();
}
//---------------------------------------------------------------------------

EsStringArray EsScript::get_translatables() const
{
  return translatablesGet();
}
//---------------------------------------------------------------------------

void EsScript::pathAdd(const EsString& path, EsStringArray& collection, const EsString& subject, bool doThrow /*=true*/)
{
  if(path.empty())
    return;

  const EsPath& tmp = EsPath::createFromPath(path);
  if(tmp.dirExists())
  {
    const EsString& apath = tmp.pathGet();
    // add unique paths to includes list
    if(collection.end() == std::find(collection.begin(), collection.end(), apath))
      collection.push_back(apath);
  }
  else
  {
    const EsString& msg = EsString::format(
      esT("Path '%s' could not be added to '%s'"),
      path,
      subject
    );

    if(doThrow)
      EsScriptException::Throw(msg);
    else
      EsUtilities::logError(msg);
  }
}
//---------------------------------------------------------------------------

void EsScript::includePathAdd(const EsString& path, bool doThrow /*= true*/)
{
	pathAdd(
    path,
    m_includes,
    esT("include paths"),
    doThrow
  );
}
//---------------------------------------------------------------------------

void EsScript::includePathsClear()
{
	m_includes.clear();
}
//---------------------------------------------------------------------------

void EsScript::includePathsSet(const EsString::Array& paths, bool doThrow /*= true*/)
{
	m_includes.clear();
  for(size_t idx = 0; idx < paths.size(); ++idx)
      includePathAdd(
        paths[idx],
        doThrow
      );
}
//---------------------------------------------------------------------------

void EsScript::linkPathAdd(const EsString& path, bool doThrow /*= true*/)
{
	pathAdd(
    path,
    m_linkPaths,
    esT("link paths"),
    doThrow
  );
}
//---------------------------------------------------------------------------

void EsScript::linkPathsClear()
{
	m_linkPaths.clear();
}
//---------------------------------------------------------------------------

void EsScript::linkPathsSet(const EsStringArray& paths, bool doThrow /*= true*/)
{
	m_linkPaths.clear();
	for(size_t idx = 0; idx < paths.size(); ++idx)
		linkPathAdd(
      paths[idx],
      doThrow
    );
}
//---------------------------------------------------------------------------

EsScriptMachine& EsScript::vm()
{
	return m_machine;
}
//---------------------------------------------------------------------------

const EsScriptMachine& EsScript::vm() const
{
	return m_machine;
}
//---------------------------------------------------------------------------

void EsScript::marshal(const EsVariant& data) const
{
  m_machine.marshalData(data);
}
//---------------------------------------------------------------------------

bool EsScript::compileText( const EsString& src, const EsBreakIntf::Ptr& brk )
{
	m_compiled = false;
	return (m_compiled =
    EsScriptUtilities::compileFromString(
      src,
      m_machine,
      m_machine.m_filesInfo,
      brk
    )
  );
}
//---------------------------------------------------------------------------

bool EsScript::compileText( const EsString& src )
{
  return compileText(src, EsBreakIntf::Ptr());
}
//---------------------------------------------------------------------------

/// File finder helper
///
class EsScriptFileFinder : public EsPathEnumerator
{
public:
	EsScriptFileFinder(const EsString& root, const EsString& fileToFind) :
	EsPathEnumerator(root),
	m_fileToFind(fileToFind),
	m_found(false)
	{
	}

	const EsString& resultGet() const
	{
		return m_foundFile;
	}

	bool isOk() const
	{
		return m_found;
	}

protected:
	virtual bool onObject(const EsString& curPath, const EsString& name, bool isDir)
	{
		if(!isDir && name == m_fileToFind)
		{
			m_foundFile = curPath + name;
			m_found = true;
		}

		return !m_found;
	}

private:
	EsString m_foundFile;
	EsString m_fileToFind;
	bool m_found;
};
//---------------------------------------------------------------------------

EsString EsScript::absoluteFileNameGet(
  const EsString& fileName,
  const EsStringArray& searchPaths,
	bool searchSubfolders /*= false*/,
  const EsString& curFile /*= EsString::s_null*/
)
{
	if( !fileName.empty() )
	{
		// convert file name to the absolute path, using include files array as
		// additional paths for file search
		const EsPath& newf = EsPath::createFromFilePath(fileName);
		const EsPath& curf = EsPath::createFromFilePath(curFile);
		const EsString& curPath = curf.pathGet(
      static_cast<ulong>(EsPathFlag::Default)|
			static_cast<ulong>(EsPathFlag::ExcludeFile)|
			static_cast<ulong>(EsPathFlag::AppendSeparator)
    );

		if( newf.exists(curPath) )
			return newf.pathGet(static_cast<ulong>(EsPathFlag::Default), curPath);
		else
		{
			const EsString& prefixDir = newf.pathGet(
        static_cast<ulong>(EsPathFlag::AsIs)|
        static_cast<ulong>(EsPathFlag::ExcludeFile)|
        static_cast<ulong>(EsPathFlag::NativeSeparator)|
        static_cast<ulong>(EsPathFlag::AppendSeparator)
      );

			// search subfolders in current path
			if( searchSubfolders )
			{
				EsScriptFileFinder finder(curPath+prefixDir, newf.fileNameExtGet());
				finder.execute(
          static_cast<ulong>(EsPathEnumeratorFlag::Recursive)
        );
				if( finder.isOk() )
					return finder.resultGet();
			}

			// lookup search paths, if any
			for(size_t idx = 0; idx < searchPaths.size(); ++idx)
			{
				EsString include = searchPaths[idx];
				EsString::value_type sepCheck = include[include.size()-1];
				if( EsPath::c_nativeSeparator != sepCheck &&
						EsPath::c_nativeSeparator != sepCheck )
					include += EsPath::c_nativeSeparator;
				include += prefixDir;

				EsScriptFileFinder finder(include, newf.fileNameExtGet());
				finder.execute(searchSubfolders ?
          static_cast<ulong>(EsPathEnumeratorFlag::Recursive) :
          static_cast<ulong>(EsPathEnumeratorFlag::Default)
        );
				if( finder.isOk() )
					return finder.resultGet();
			}
		}
	}

	return EsString::null();
}
//---------------------------------------------------------------------------

bool EsScript::compileFile( const EsString& file, const EsBreakIntf::Ptr& brk )
{
	m_compiled = false;
	const EsString& srcFile = absoluteFileNameGet(
    file,
    m_includes
  );

	// check source file existence
	if( !EsPath::exists(srcFile, EsString::null()) )
		EsScriptException::Throw(
      EsString::format(
        esT("Source file '%s' could not be found"),
			  file
      )
    );

  m_compiled = EsScriptUtilities::compileFromFile(
    file,
    m_machine,
    m_machine.m_filesInfo,
    brk
  );

  return m_compiled;
}
//---------------------------------------------------------------------------

bool EsScript::compileFile( const EsString& file )
{
  return compileFile( file, EsBreakIntf::Ptr() );
}
//---------------------------------------------------------------------------

EsBinBuffer EsScript::compiledBinaryGet(bool retainDebug) const
{
	EsBinBuffer buff;
	EsScriptCompiledBinaryWriter writer(
    m_machine,
    retainDebug,
    buff
  );

  return buff;
}
//---------------------------------------------------------------------------

void EsScript::compiledBinarySet(const EsBinBuffer& buff, bool retainDebug)
{
  m_compiled = false;

  EsScriptCompiledBinaryReader reader(
    m_machine,
    retainDebug,
    buff
  );
  m_compiled = true;
}
//---------------------------------------------------------------------------

EsBinBuffer EsScript::compiledBinaryLoad(const EsString& fileName, bool retainDebug, const EsString& k)
{
  ES_DEBUG_TRACE(
    esT("EsScript::compiledBinaryLoad(fileName=%s, retainDebug=%d, key=%s)"),
    fileName,
    (int)retainDebug,
    k
  );

	const EsPath& file = EsPath::createFromFilePath(fileName);
	EsBinBuffer compiled;
	const EsString& fpath = file.pathGet();
	EsFile f(fpath, static_cast<ulong>(EsFileFlag::Read));
	if( f.open() )
	{
    compiled = f.readAllAsBinBuffer();
		f.close();

		if( !k.empty() )
			compiled = EsUtilities::eToB(compiled, k);
	}
	else
		EsScriptException::Throw( f.get_recentErrorString() );

	compiledBinarySet(
    compiled,
    retainDebug
  );

	return compiled;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsScript::reset)()
{
	vm().reset();
	m_compiled = false;
}
//---------------------------------------------------------------------------

#if	defined(ES_USE_SCRIPTLETS) && 1 == ES_USE_SCRIPTLETS

ES_IMPL_INTF_METHOD(EsScriptletIntf::Ptr, EsScript::scriptletCreate)(const EsString& name, const EsString& src, const EsString& args /*= EsString::null()*/)
{
	EsScriptletIntf::Ptr scl( new EsScriptlet(m_machine, name, src, args) );
	ES_ASSERT(scl);

	if( EsStringIndexedMap::npos == m_scriptlets.itemFind(scl->nameGet()) )
	{
		m_scriptlets.itemAdd(scl->nameGet(), scl);
		return scl;
	}

	return EsScriptletIntf::Ptr();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsScript::scriptletCreateReflected(cr_EsString name, cr_EsString src, cr_EsString args)
{
	return scriptletCreate(name, src, args);
}
//---------------------------------------------------------------------------

EsScriptletIntf::Ptr EsScript::scriptletGet(const EsString& name, size_t paramCount) const
{
	const EsString& trueName = EsScriptlet::nameFormat(name, paramCount);
	ulong idx = m_scriptlets.itemFind(trueName);
  if(EsStringIndexedMap::npos != idx)
	{
		EsScriptletIntf::Ptr scl = m_scriptlets.valueGet(idx).asExistingObject();
		ES_ASSERT(scl);
		return scl;
	}

	EsScriptException::Throw(
    EsString::format(
      esT("Could not find scriptlet '%s' taking %d parameters"),
      name,
      paramCount
    )
  );
	return EsScriptletIntf::Ptr();
}
//---------------------------------------------------------------------------

EsScriptletIntfPtrArray EsScript::scriptletsLoad(const EsString& file, const EsString& key)
{
	const EsPath& fn = EsPath::createFromFilePath(file);
	EsFile f(
    fn.pathGet(), key.empty() ?
      static_cast<ulong>(EsFileFlag::Read)|static_cast<ulong>(EsFileFlag::Text) :
      static_cast<ulong>(EsFileFlag::Read)
  );

	if( f.open() )
	{
		EsString contents;
		if( !key.empty() )
		{
			const EsBinBuffer& bb = f.binBufferRead();
			contents = EsString(bb.begin(), bb.end());
			contents = EsUtilities::eToS(contents, key);
		}
		else
			contents = f.readAllAsString();

		ES_ASSERT(!contents.empty());
		EsRegEx re(
      esT("scriptlet\\s+([_a-zA-Z][_0-9a-zA-Z]*)\\s*\\(([^)]*)\\)\\s*(.*?)endscriptlet\\s*")
    );
		// parse contents, load each group of matches as scriptlet
		re.set_text(contents);
		while(re.get_matches())
		{
			ES_ASSERT( 4 == re.get_matchCount() );
			const EsString& name = re.matchGet(1);
			const EsString& args = re.matchGet(2);
			const EsString& src = re.matchGet(3);

			scriptletCreate(name, src, args);

			size_t start, len;
			re.matchGet(start, len, 0);

			// move to the next portion
			contents = contents.substr(start, len);
			re.set_offset(start+len);
		}

		return scriptletsGet();
	}
	else
		EsScriptException::Throw(
      EsString::format(
        esT("Could not load scriptlet file '%s'"),
        file
      )
    );

	return EsScriptletIntfPtrArray();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsScriptletIntfPtrArray, EsScript::scriptletsGet)() const
{
	EsScriptletIntfPtrArray result;
	result.reserve(m_scriptlets.countGet());
	for(size_t idx = 0; idx < m_scriptlets.countGet(); ++idx)
	{
		EsScriptletIntf::Ptr scl = m_scriptlets.valueGet(idx).asExistingObject();
		ES_ASSERT(scl);
		result.push_back( scl );
	}

	return result;
}
//---------------------------------------------------------------------------

void EsScript::scriptletsReset()
{
	for(size_t idx = 0; idx < m_scriptlets.countGet(); ++idx)
	{
		EsScriptletIntf::Ptr scl = m_scriptlets.valueGet(idx).asExistingObject();
		ES_ASSERT(scl);
		scl->discard();
		m_scriptlets.valueSet(idx, EsVariant::null());
	}
	m_scriptlets.clear();
}
//---------------------------------------------------------------------------

#endif

const EsStringIndexedMap& EsScript::reservedWordsGet()
{
  static EsStringIndexedMap s_rw;

  if( s_rw.isEmpty() )
  {
    s_rw.itemAdd( esT("var") );
    s_rw.itemAdd( esT("const") );
    s_rw.itemAdd( esT("function") );
    s_rw.itemAdd( esT("return") );
    s_rw.itemAdd( esT("new") );
    s_rw.itemAdd( esT("enum") );
    s_rw.itemAdd( esT("object") );
    s_rw.itemAdd( esT("extends") );
    s_rw.itemAdd( esT("true") );
    s_rw.itemAdd( esT("false") );
    s_rw.itemAdd( esT("if") );
    s_rw.itemAdd( esT("else") );
    s_rw.itemAdd( esT("in") );
    s_rw.itemAdd( esT("for") );
    s_rw.itemAdd( esT("foreach") );
    s_rw.itemAdd( esT("while") );
    s_rw.itemAdd( esT("do") );
    s_rw.itemAdd( esT("break") );
    s_rw.itemAdd( esT("continue") );
    s_rw.itemAdd( esT("switch") );
    s_rw.itemAdd( esT("case") );
    s_rw.itemAdd( esT("default") );
    s_rw.itemAdd( esT("label") );
    s_rw.itemAdd( esT("goto") );
    s_rw.itemAdd( esT("throw") );
    s_rw.itemAdd( esT("rethrow") );
    s_rw.itemAdd( esT("try") );
    s_rw.itemAdd( esT("catch") );
    s_rw.itemAdd( esT("property") );
  }

  return s_rw;
}
//---------------------------------------------------------------------------

// make sure identifier is not a reserved word
void EsScript::checkNotReservedWord(const EsString& ident, const EsScriptDebugInfoIntf::Ptr& dbg)
{
	if( ident.empty() )
    return;

  if( reservedWordsGet().itemExists( ident ) )
      EsScriptException::Throw(
        EsString::format(
          esT("'%s' is a reserved word"),
          ident
        ),
        dbg
      );
}

// make sure identifier is not a registered reflected class name
void EsScript::checkNotRegisteredObject(const EsString& ident, const EsScriptDebugInfoIntf::Ptr& dbg)
{
	if( nullptr != EsClassInfo::classInfoGet(ident) )
		EsScriptException::Throw(
      EsString::format(
        esT("'%s' is already defined as object type"),
        ident
      ),
      dbg
    );
}
//---------------------------------------------------------------------------

EsProgressMonitorIntf::Ptr EsScript::progressMonitorGet() const
{
	const EsVariant& mon = m_machine.m_host.get_progressMonitor();
	if( !mon.isEmpty() )
		return mon.asExistingObject();

	return EsProgressMonitorIntf::Ptr();
}
//---------------------------------------------------------------------------

void EsScript::progressMonitorSet(const EsProgressMonitorIntf::Ptr& monitor)
{
	m_machine.m_host.set_progressMonitor(monitor);
}
//---------------------------------------------------------------------------

void EsScript::execAbort()
{
	vm().execAbort();
}
//---------------------------------------------------------------------------

void EsScript::rewind()
{
	vm().rewind(false); //< Do not re-start GC automatically
  vm().gcStop();
}
//---------------------------------------------------------------------------

EsStringArray EsScript::translatablesGet() const
{
  return vm().translatablesGet();
}
//---------------------------------------------------------------------------

bool EsScript::hasSourceModulesInfo() const ES_NOTHROW
{
  return vm().hasSourceModulesInfo();
}
//--------------------------------------------------------------------------------

EsVariant EsScript::sourceModuleExecutableLinesGet(const EsString& srcPath) const ES_NOTHROW
{
  return vm().sourceModuleExecutableLinesGet(srcPath);
}
//--------------------------------------------------------------------------------

EsVariant EsScript::sourceModuleHashGet(const EsString& srcPath) const ES_NOTHROW
{
  return vm().sourceModuleHashGet(srcPath);
}
//--------------------------------------------------------------------------------

EsString::Array EsScript::sourceModulesPathsGet() const ES_NOTHROW
{
  return vm().sourceModulesPathsGet();
}
//--------------------------------------------------------------------------------

bool EsScript::hasSourceModule(const EsString& srcPath) const ES_NOTHROW
{
  return vm().hasSourceModule(srcPath);
}
//--------------------------------------------------------------------------------

EsVariant EsScript::callClassMethod(cr_EsString name, cr_EsVariant params) const
{
  return classInfoGet().classCallMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScript::callMethod(cr_EsString name, cr_EsVariant params)
{
  const EsMethodInfoKeyT& key = EsMethodInfoKeyT(
    name,
    params
  );

  if( vm().hasGlobalMethod( key ) )
    return vm().callGlobalMethod(
      name,
      params
    );

  return classInfoGet().methodInfoGet(
    key,
    true
  ).call(
    ES_THISBASE_CAST,
    params
  );
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsVariant EsScript::call(const EsString& name)
{
  return callMethod(
    name,
    EsVariant::null()
  );
}
//---------------------------------------------------------------------------

EsVariant EsScript::call(const EsString& name, const EsVariant& param1)
{
	EsVariant::Array params;
  params.reserve(1);

	params.push_back(param1);

  return callMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScript::call(const EsString& name, const EsVariant& param1, const EsVariant& param2)
{
	EsVariant::Array params;
  params.reserve(2);

	params.push_back(param1);
	params.push_back(param2);

  return callMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScript::call(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3)
{
	EsVariant::Array params;
  params.reserve(3);

	params.push_back(param1);
	params.push_back(param2);
	params.push_back(param3);

  return callMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScript::call(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4)
{
	EsVariant::Array params;
  params.reserve(4);

	params.push_back(param1);
	params.push_back(param2);
	params.push_back(param3);
	params.push_back(param4);

  return callMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScript::call(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5)
{
	EsVariant::Array params;
  params.reserve(5);

	params.push_back(param1);
	params.push_back(param2);
	params.push_back(param3);
	params.push_back(param4);
	params.push_back(param5);

  return callMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScript::call(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6)
{
	EsVariant::Array params;
  params.reserve(6);

	params.push_back(param1);
	params.push_back(param2);
	params.push_back(param3);
	params.push_back(param4);
	params.push_back(param5);
	params.push_back(param6);

  return callMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScript::classCall(const EsString& name) const
{
  return callClassMethod(
    name,
    EsVariant::null()
  );
}
//---------------------------------------------------------------------------

EsVariant EsScript::classCall(const EsString& name, const EsVariant& param1) const
{
	EsVariant::Array params;
  params.reserve(1);

	params.push_back(param1);

  return callClassMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScript::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2) const
{
	EsVariant::Array params;
  params.reserve(2);

	params.push_back(param1);
	params.push_back(param2);

  return callClassMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScript::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) const
{
	EsVariant::Array params;
  params.reserve(3);

	params.push_back(param1);
	params.push_back(param2);
	params.push_back(param3);

  return callClassMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScript::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) const
{
	EsVariant::Array params;
  params.reserve(4);

	params.push_back(param1);
	params.push_back(param2);
	params.push_back(param3);
	params.push_back(param4);

  return callClassMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScript::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) const
{
	EsVariant::Array params;
  params.reserve(5);

	params.push_back(param1);
	params.push_back(param2);
	params.push_back(param3);
	params.push_back(param4);
	params.push_back(param5);

  return callClassMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScript::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6) const
{
	EsVariant::Array params;
  params.reserve(6);

	params.push_back(param1);
	params.push_back(param2);
	params.push_back(param3);
	params.push_back(param4);
	params.push_back(param5);
	params.push_back(param6);

  return callClassMethod(
    name,
    params
  );
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsVariant EsScript::exec()
{
	return vm().exec();
}
//---------------------------------------------------------------------------

EsVariant EsScript::exec(cr_EsString name)
{
	return vm().call(name);
}
//---------------------------------------------------------------------------

EsVariant EsScript::exec(cr_EsString name, cr_EsVariant p0)
{
	return vm().call(name, p0);
}
//---------------------------------------------------------------------------

EsVariant EsScript::exec(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1)
{
	return vm().call(name, p0, p1);
}
//---------------------------------------------------------------------------

EsVariant EsScript::exec(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2)
{
	return vm().call(name, p0, p1, p2);
}
//---------------------------------------------------------------------------

EsVariant EsScript::exec(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3)
{
	return vm().call(name, p0, p1, p2, p3);
}
//---------------------------------------------------------------------------

EsVariant EsScript::exec(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4)
{
	return vm().call(name, p0, p1, p2, p3, p4);
}
//---------------------------------------------------------------------------

EsVariant EsScript::constantGet(cr_EsString name) const
{
	return const_cast<EsScript*>(this)->vm().constantGet(name);
}
//---------------------------------------------------------------------------

EsVariant EsScript::variableGet(cr_EsString name) const
{
	return const_cast<EsScript*>(this)->vm().variableGet(name);
}
//---------------------------------------------------------------------------

void EsScript::variableSet(cr_EsString name, cr_EsVariant val)
{
	vm().variableSet(name, val);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsScript::objectCreate(cr_EsString name)
{
	return vm().objectCreate(name);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsScript::objectCreate(cr_EsString name, cr_EsVariant p0)
{
	return vm().objectCreate(name, p0);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsScript::objectCreate(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1)
{
	return vm().objectCreate(name, p0, p1);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsScript::objectCreate(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2)
{
	return vm().objectCreate(name, p0, p1, p2);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsScript::objectCreate(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3)
{
	return vm().objectCreate(name, p0, p1, p2, p3);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsScript::objectCreate(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4)
{
	return vm().objectCreate(name, p0, p1, p2, p3, p4);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsScript::objectCreate(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4, cr_EsVariant p5)
{
	return vm().objectCreate(name, p0, p1, p2, p3, p4, p5);
}
//---------------------------------------------------------------------------

