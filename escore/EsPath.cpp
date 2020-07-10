#include "escorepch.h"
#pragma hdrstop

#include "EsPath.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

namespace EsReflection
{
ES_DECL_ENUMERATION_INFO(EsPathFlag, NO_CLASS_DESCR)
}

// path parsing patterns
//
#define ES_RE_SIMPLE_VOLUME             esT("[a-zA-Z]\\:")
#define ES_GUID_VOLUME                  esT("Volume\\{[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\\}")
#define ES_UNC_ROOT                     esT("\\\\{2}|/{2})(?:\\?[\\\\/])?(?:UNC[\\\\/])?")
// UNC root match
// group 1 = UNC root, group 2 = guid volume, group 3 = short volume, group 4 = server
static ES_CTSTR c_reUncRootAndVol     = esT("^((?:") ES_UNC_ROOT esT(")")
                                        esT("(?:((?:") ES_GUID_VOLUME esT(")|(?:") ES_RE_SIMPLE_VOLUME esT("))|([^\\\\/]+))");
// normal root|volume
// group 1 = entire root, group 2 = volume
static ES_CTSTR c_reGenericRootOrVol  = esT("^(/|") ES_RE_SIMPLE_VOLUME esT(")");
// path tokenization pattern
static ES_CTSTR c_rePathTokenizer     = esT("([^\\\\/]+)");
// env var match:
static ES_CTSTR c_reEvnVar            = esT("\\$([_a-zA-Z][_0-9a-zA-Z]*)");
// invalid path chars
#define ES_PATH_INVALID_CHARS           esT("\\/:*?\"|<>")

const EsString::value_type EsPath::c_nativeSeparator =
#if ES_OS == ES_OS_WINDOWS
  esT('\\');
#elif defined(ES_POSIX_COMPAT)
  esT('/');
#endif
const EsString::value_type EsPath::c_genericSeparator
                                      = esT('/');

// EsPath reflection declaration
//
ES_DECL_BASE_CLASS_INFO_BEGIN(EsPath, NO_CLASS_DESCR)
  // properties
  ES_DECL_PROP_INFO_RO(  EsPath, isOk, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsPath, isUNC, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsPath, isRemote, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsPath, isRelative, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsPath, isAbsolute, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsPath, hasVars, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsPath, isFile, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsPath, hasDirs, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsPath, root, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsPath, volume, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsPath, dirs, EsStringArray, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(     EsPath, fileName, EsString, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(     EsPath, fileExt, EsString, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsPath, fileNameExt, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  // reflected methods
  ES_DECL_REFLECTED_METHOD_INFO(EsPath, existsReflected, exists, bool_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(EsPath, fileExistsReflected, fileExists, bool_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(EsPath, dirExistsReflected, dirExists, bool_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(EsPath, separatorGetReflected, separatorGet, EsString_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(EsPath, pathCreateReflected, pathCreate, void_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(EsPath, pathGetReflected, pathGet, EsString_CallConst_ulong_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsPath, dirAppend, void_Call_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsPath, dirLastRemove, void_Call, NO_METHOD_DESCR)
  // static services
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsPath, createReflected, create, EsBaseIntfPtr_ClassCall_cr_EsString_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsPath, createFromPathReflected, createFromPath, EsBaseIntfPtr_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsPath, createFromFilePathReflected, createFromFilePath, EsBaseIntfPtr_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, fileRemove, void_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, dirRemove, void_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, cwdGet, EsString_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, cwdSet, void_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, exists, bool_ClassCall_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, fileExists, bool_ClassCall_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, dirExists, bool_ClassCall_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, pathCreate, void_ClassCall_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, stdAppPathGet, EsString_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, stdAppDataGet, EsString_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, stdLocalAppDataGet, EsString_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, stdHomeGet, EsString_ClassCall, NO_METHOD_DESCR)
   ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, stdDocsGet, EsString_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, stdPublicDocsGet, EsString_ClassCall, NO_METHOD_DESCR)
   ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, stdDownloadsGet, EsString_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, stdPublicDownloadsGet, EsString_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, stdSysRootGet, EsString_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsPath, makeSafeReflected, makeSafe, EsString_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsPath, makeSafeReflected, makeSafe, EsString_ClassCall_cr_EsString_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
#if ES_OS == ES_OS_WINDOWS
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, stdDriveNamesGet, EsStringArray_ClassCall, NO_METHOD_DESCR)
#endif
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsPath, stdDataRootsGet, EsStringArray_ClassCall, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
// NB! This line here is only to work around 10.3.+ toolchain bug
// Arguably, either compiler does not do its magic in generating proper implicit Dtor override,
// or linker does not link generated implicit code properly
//
EsPath::~EsPath() {}
#endif
//---------------------------------------------------------------------------

// EsPath implementation
EsPath::EsPath() :
m_state(stateAbsolute)
{
}
//---------------------------------------------------------------------------

EsPath::EsPath(const EsPath& src) :
m_state(src.m_state),
m_orig(src.m_orig),
m_root(src.m_root),
m_volume(src.m_volume),
m_dirs(src.m_dirs),
m_fileName(src.m_fileName),
m_fileExt(src.m_fileExt)
{}
//---------------------------------------------------------------------------

EsPath& EsPath::operator=(const EsPath& src)
{
  m_state = src.m_state;
  m_orig = src.m_orig;
  m_root = src.m_root;
  m_volume = src.m_volume;
  m_dirs = src.m_dirs;
  m_fileName = src.m_fileName;
  m_fileExt = src.m_fileExt;

  return *this;
}
//---------------------------------------------------------------------------

EsPath EsPath::create(const EsString& path, const EsString& fileName, const EsString& fileExt)
{
  EsPath fn;
  fn.pathParse(path, false);
  fn.fileNameSet(fileName);
  fn.fileExtSet(fileExt);

  return fn;
}
//---------------------------------------------------------------------------

EsPath EsPath::createFromPath(const EsString& path)
{
  EsPath fn;
  fn.pathParse(path, false);

  return fn;
}
//---------------------------------------------------------------------------

EsPath EsPath::createFromFilePath(const EsString& filePath)
{
  EsPath fn;
  fn.pathParse(filePath, true);

  return fn;
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsPath::createReflected(const EsString& path, const EsString& fileName, const EsString& fileExt)
{
  std::unique_ptr<EsPath> p = ES_MAKE_UNIQUE( 
    EsPath,
    EsPath::create(
      path, 
      fileName, 
      fileExt
    ) 
  );
  ES_ASSERT(p);
  
  p->m_dynamic = true;
  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsPath::createFromPathReflected(const EsString& path)
{
  std::unique_ptr<EsPath> p = ES_MAKE_UNIQUE( 
    EsPath,
    EsPath::createFromPath(path) 
  );
  ES_ASSERT(p);
  
  p->m_dynamic = true;
  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsPath::createFromFilePathReflected(const EsString& filePath)
{
  std::unique_ptr<EsPath> p = ES_MAKE_UNIQUE(
    EsPath,
    EsPath::createFromFilePath(filePath) 
  );
  ES_ASSERT(p);
  
  p->m_dynamic = true;
  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

bool EsPath::get_isOk() const
{
  return isOk();
}
//---------------------------------------------------------------------------

bool EsPath::get_isUNC() const
{
  return isUNC();
}
//---------------------------------------------------------------------------

bool EsPath::get_isRemote() const
{
  return isRemote();
}
//---------------------------------------------------------------------------

bool EsPath::get_isRelative() const
{
  return isRelative();
}
//---------------------------------------------------------------------------

bool EsPath::get_isAbsolute() const
{
  return isAbsolute();
}
//---------------------------------------------------------------------------

bool EsPath::get_hasVars() const
{
  return hasVars();
}
//---------------------------------------------------------------------------

bool EsPath::get_isFile() const
{
  return isFile();
}
//---------------------------------------------------------------------------

bool EsPath::get_hasDirs() const
{
  return hasDirs();
}
//---------------------------------------------------------------------------

EsString EsPath::get_root() const
{
  return rootGet();
}
//---------------------------------------------------------------------------

EsString EsPath::get_volume() const
{
  return volumeGet();
}
//---------------------------------------------------------------------------

EsStringArray EsPath::get_dirs() const
{
  return dirsGet();
}
//---------------------------------------------------------------------------

EsString EsPath::get_fileName() const
{
  return fileNameGet();
}
//---------------------------------------------------------------------------

void EsPath::set_fileName(const EsString& fname)
{
  fileNameSet(fname);
}
//---------------------------------------------------------------------------

EsString EsPath::get_fileExt() const
{
  return fileExtGet();
}
//---------------------------------------------------------------------------

void EsPath::set_fileExt(const EsString& fext)
{
  fileExtSet(fext);
}
//---------------------------------------------------------------------------

EsString EsPath::get_fileNameExt() const
{
  return fileNameExtGet();
}
//---------------------------------------------------------------------------

bool EsPath::existsReflected(const EsString& basePath) const
{
  return exists(basePath);
}
//---------------------------------------------------------------------------

bool EsPath::fileExistsReflected(const EsString& basePath) const
{
  return fileExists(basePath);
}
//---------------------------------------------------------------------------

bool EsPath::dirExistsReflected(const EsString& basePath) const
{
  return dirExists(basePath);
}
//---------------------------------------------------------------------------

EsString EsPath::separatorGetReflected() const
{
  return separatorGet();
}
//---------------------------------------------------------------------------

void EsPath::pathCreateReflected(const EsString& basePath) const
{
  pathCreate(basePath);
}
//---------------------------------------------------------------------------

EsString EsPath::pathGetReflected(ulong flags, cr_EsString basePath) const
{
  return pathGet(flags, basePath);
}
//---------------------------------------------------------------------------

EsString EsPath::makeSafeReflected(const EsString& in)
{
  return makeSafe(in);
}
//---------------------------------------------------------------------------

EsString EsPath::makeSafeReflected(const EsString& in, const EsString& symbols, const EsVariant& replacement)
{
  return makeSafe(in, symbols, replacement.asChar());
}
//---------------------------------------------------------------------------

void EsPath::fileNameSet(const EsString& name)
{
  checkPathStringValidity(name, esT("File name"));
  m_fileName = name;
}
//---------------------------------------------------------------------------

void EsPath::fileExtSet(const EsString& ext)
{
  checkPathStringValidity(ext, esT("File extension"));
  m_fileExt = ext;
}
//---------------------------------------------------------------------------

void EsPath::volumeSet(const EsString& vol)
{
  // todo: check validity
  m_volume = vol;
}
//---------------------------------------------------------------------------

EsString EsPath::fileNameExtGet() const
{
  EsString result = m_fileName;
  if( !m_fileExt.empty() )
  {
    result += esT('.');
    result += m_fileExt;
  }

  return result;
}
//---------------------------------------------------------------------------

void EsPath::dirAppend(const EsString& dir)
{
  if( dir.empty() )
    EsException::Throw(esT("Could not append empty directory name"));

  checkPathStringValidity(dir, esT("Directory name"));
  if( esT(".") == dir )
  {
    // if first dir is '.', just skip it, otherwise, report an error
    if( !m_dirs.empty() )
      EsException::Throw(esT("Current directory specifier '.' is valid only at the beginning of the path"));
  }
  else if( esT("..") == dir )
  {
    // if 'parent' specifier encountered, simplify it immediately, by removing upper directory,
    // unless it's not '~' == home specifier. otherwise, report an error
    if( !m_dirs.empty() )
    {
      const EsString& upperDir = m_dirs[m_dirs.size()-1];
      if( esT("..") != upperDir )
      {
        if( esT("~") == upperDir )
          EsException::Throw(esT("Could not change parent beyond Home directory"));

        // remove parent dir
        m_dirs.pop_back();
      }
      else
        m_dirs.push_back(dir);
    }
    else
      m_dirs.push_back(dir);
  }
  else
    m_dirs.push_back(dir);
}
//---------------------------------------------------------------------------

void EsPath::dirLastRemove()
{
  if( !m_dirs.empty() )
    m_dirs.pop_back();
}
//---------------------------------------------------------------------------

void EsPath::checkPathStringValidity(const EsString& str, const EsString& context)
{
  if( EsString::npos != str.find_first_of(ES_PATH_INVALID_CHARS) )
    EsException::Throw(
      esT("%s '%s' contains invalid symbols"),
      context,
      str
    );
}
//---------------------------------------------------------------------------

bool EsPath::isRelativeRoot(size_t checkPos, const EsString& path)
{
  // check if we're dealing with relative path, like c:some path
  return checkPos < path.size() &&
      c_nativeSeparator != path[checkPos] &&
      c_genericSeparator != path[checkPos];
}
//---------------------------------------------------------------------------

ulong EsPath::rootAndVolumeInit(const EsString& path)
{
  ulong result = 0;
  ulong start = 0, len = 0;

  EsRegEx re(
    c_reUncRootAndVol,
    static_cast<ulong>(EsRegExCompileFlag::DEFAULT)|
    static_cast<ulong>(EsRegExCompileFlag::ICASE)
  );
  ES_ASSERT(re.isOk());

  re.set_text(path);
  if( re.get_matches() )
  {
    // find out where our entire match ends
    re.matchGet(
      start, 
      result, 
      0
    );
    ES_ASSERT(0 == start);
    ES_ASSERT(result);
    // we're matched UNC path
    m_state |= stateUnc;

    // extract root match
    if( re.matchGet(start, len, 1) && len )
      rootSet( path.substr(start, len) );

    // extract guid volume match
    if( re.matchGet(start, len, 2) && len )
    {
      volumeSet( path.substr(start, len) );
      if( isRelativeRoot(result, path) )
        EsException::Throw(esT("Invalid GUID Volume specified"));
    }
    else if( re.matchGet(start, len, 3) && len )
    {
      // extract simple volume match
      volumeSet( path.substr(start, len) );
      // check if we're dealing with relative path, like c:some path
      if( !isRelativeRoot(result, path) )
        m_state |= stateAbsolute;
      else
        m_state &= ~stateAbsolute;
    }
    else if( re.matchGet(start, len, 4) && len )
    {
      volumeSet( path.substr(start, len) );
      m_state |= stateUncRemote;
    }
  }
  else
  {
    re.set_pattern(c_reGenericRootOrVol);
    ES_ASSERT(re.isOk());

    if( re.get_matches() )
    {
      // find out where our entire match ends
      re.matchGet(start, result, 0);
      ES_ASSERT(0 == start);
      ES_ASSERT(result);

      // extract root match
      if( re.matchGet(start, len, 1) && len )
      {
        rootSet( path.substr(start, len) );
        // optional check for "path relative to the volume" case
        if(
#if ES_OS == ES_OS_WINDOWS
        !m_volume.empty() &&
        !isRelativeRoot(result, path)
#else
        !isRelativeRoot(0, path)
#endif
        )
          m_state |= stateAbsolute;
        else
          m_state &= ~stateAbsolute;
      }
    }
    else
      m_state &= ~stateAbsolute;
  }

  return result;
}
//---------------------------------------------------------------------------

void EsPath::fileNameExtParse(const EsString& fileNameExt)
{
  size_t pos = fileNameExt.find_last_of(esT('.'));
  if( pos != EsString::npos )
  {
    if( pos > 0 )
      fileNameSet( fileNameExt.substr(0, pos) );

    fileExtSet( fileNameExt.substr(pos+1) );
  }
  else
    fileNameSet( fileNameExt );
}
//---------------------------------------------------------------------------

void EsPath::pathParse(const EsString& path, bool hasFileName)
{
  m_orig = path;
  EsString tmp = path;

  // Reset all variables
  m_state = stateAbsolute;
  m_root.clear();
  m_volume.clear();
  m_dirs.clear();
  m_fileName.clear();
  m_fileExt.clear();

  if( !tmp.empty() )
  {
    // find if path contains environment vars
    EsRegEx re(c_reEvnVar);
    ES_ASSERT(re.isOk());
    re.set_text(tmp);

    if (re.get_matches())
      m_state |= stateHasVars;

    // Expand environment vars, if any
    if(m_state & stateHasVars)
    {
      EsPath::expandVars(
        tmp,
        EsUtilities::environmentVarsListGet()
      );

      re.set_text(tmp);
    }

    ulong pathOffs = rootAndVolumeInit(tmp);
    re.set_offset(pathOffs);

    // todo: check the rest of path string for invalid separators (i.e. several consequtive in a row)

    // create path processing pattern & initialize regular expression
    re.set_pattern(c_rePathTokenizer);
    ES_ASSERT(re.isOk());

    // split path into parts
    EsString::Array pathParts;
    pathParts.reserve(16);

    while( re.get_matches() )
    {
      ulong start, len;
      if( re.matchGet(start, len, 1) && len )
      {
        EsString::const_iterator startIt = tmp.begin()+start;
        EsString::const_iterator endIt = startIt+len;
        pathParts.push_back( EsString(startIt, endIt) );
        re.set_offset( start+len );
      }
    }

    if( !pathParts.empty() )
    {
      ulong idx = 0;
      ulong end = static_cast<ulong>(pathParts.size());

      if( hasFileName )
        --end;

      m_dirs.reserve(end);
      while(idx < end)
        dirAppend(pathParts[idx++]);

      // finally, fill-in file and optionally extension parts
      if( hasFileName )
      {
        ES_ASSERT(idx < pathParts.size());
        fileNameExtParse(pathParts[idx]);
      }
    }
  }

  m_state |= stateOk;
}
//---------------------------------------------------------------------------

void EsPath::rootSet(const EsString& root)
{
  // if we're setting simple volume to the root,
  // clear the root and set volume to the volume field instead
  EsRegEx re( ES_RE_SIMPLE_VOLUME );
  ES_ASSERT(re.isOk());
  re.set_text( root );

  if( re.get_matches() )
  {
    m_root.clear();
    volumeSet( root );
  }
  else
  {
#if ES_OS != ES_OS_WINDOWS
    if( c_nativeSeparator != root )
#endif
      m_root = root;
#if ES_OS != ES_OS_WINDOWS
    else
    {
      m_root.clear();
      m_state |= stateAbsolute;
    }
#endif
  }
}
//---------------------------------------------------------------------------

EsString EsPath::rootGet() const
{
  EsString result = m_root;
  if( !m_volume.empty() )
  {
    if( m_state & stateUnc )
    {
      if( !result.empty() )
        result += esT('\\');
      result += m_volume;
    }
    else
    {
       if( !result.empty() )
        result += c_genericSeparator;
      result += m_volume;
    }
  }

  return result;
}
//---------------------------------------------------------------------------

void EsPath::expandVars(EsString& str, const EsStringIndexedMap& vars)
{
  EsRegEx re(c_reEvnVar);
  ES_ASSERT(re.isOk());
  re.set_text(str);

  while( re.get_matches() )
  {
    ulong start, len;
    if(  re.matchGet(start, len, 1) && len )
    {
      const EsString& varName = str.substr(start, len);
      ulong idx = vars.itemFind(varName);
      if( EsStringIndexedMap::npos != idx )
      {
        const EsString& val = vars.valueGet(idx).asEscapedString();
        str = re.replace(val);
      }
      re.set_offset(start+len);
    }
  }
}
//---------------------------------------------------------------------------

static void separatorsConvert(EsString& str, EsString::value_type sep)
{
  // just in case, convert all separators to sep
  for(size_t idx = 0; idx < str.size(); ++idx)
  {
    EsString::value_type& ch = str[idx];
    if( (EsPath::c_nativeSeparator == ch || EsPath::c_genericSeparator == ch) && sep != ch )
      ch = sep;
  }
}
//---------------------------------------------------------------------------

void EsPath::finalizePath(EsString& path, ulong flags, EsString::value_type sep) const
{
  if( !(flags & static_cast<ulong>(EsPathFlag::ExcludeFile)) && isFile() )
  {
    path += sep;
    path += fileNameExtGet();
  }

  if( (flags & static_cast<ulong>(EsPathFlag::AppendSeparator)) &&
      (!isFile() || (flags & static_cast<ulong>(EsPathFlag::ExcludeFile))) &&
      !path.empty() )
    path += sep;

  separatorsConvert(path, sep);
}
//---------------------------------------------------------------------------

EsString EsPath::constructRelativePath(ulong flags, const EsString& basePath) const
{
  ES_ASSERT( isAbsolute() );

  if( basePath.empty() )
    EsException::Throw(esT("Could not construct relative path using empty base path"));

  const EsPath& fnBase = EsPath::createFromPath(basePath);
  ES_ASSERT( fnBase.isOk() );
  if( fnBase.isRelative() )
    EsException::Throw(
      esT("Could not construct relative path using '%s' as base; base must not be relative path"),
      basePath
    );

  if( EsString::cmpEqual != EsString::scompare(rootGet(), fnBase.rootGet(), true) )
    EsException::Throw(
      esT("Could not construct relative path using '%s' as base; paths roots do not match"),
      basePath
    );

  const EsString::Array& baseDirs = fnBase.dirsGet();
  const EsString::Array& dirs = m_dirs;
  if( dirs.size() < baseDirs.size() )
    EsException::Throw(
      esT("Could not construct relative path using '%s' as base; existing path nesting level is less than that of base path"),
      basePath
    );

  size_t idx = 0;
  while(idx < baseDirs.size())
  {
    const EsString& dir = dirs[idx];
    const EsString& baseDir = baseDirs[idx];
    if( EsString::cmpEqual != EsString::scompare(dir, baseDir, true) )
      EsException::Throw(
        esT("Could not construct relative path using '%s' as base; directories '%s' and '%s' do not match"),
        basePath,
        dir,
        baseDir
      );

    ++idx;
  }

  // leave only the rest of dirs
  EsString::Array rest;
  rest.reserve( dirs.size()-idx );
  while( idx < dirs.size() )
  {
    rest.push_back( dirs[idx] );
    ++idx;
  }

  EsString::value_type sep = (flags & static_cast<ulong>(EsPathFlag::NativeSeparator)) ? c_nativeSeparator : c_genericSeparator;
  // construct relative path from rest dirs
  EsString result;
  idx = 0;
  while(idx < rest.size())
  {
    if( !result.empty() )
      result += sep;
    result += rest[idx];
    ++idx;
  }

  finalizePath(result, flags, sep);

  return result;
}
//---------------------------------------------------------------------------

EsString EsPath::constructAbsolutePath(ulong flags, const EsString& basePath) const
{
  ES_ASSERT( isRelative() );

  if( basePath.empty() )
    EsException::Throw(esT("Could not construct absolute path using empty base path"));

  const EsPath& fnBase = EsPath::createFromPath(basePath);
  ES_ASSERT( fnBase.isOk() );
  if( fnBase.isRelative() )
    EsException::Throw(
      esT("Could not construct absolute path using '%s' as base; base must not be relative path"),
      basePath
    );

  const EsString& baseVol = fnBase.volumeGet();
  if( !m_volume.empty() && !baseVol.empty() && 0 != m_volume.compare(baseVol, true) )
    EsException::Throw(
      esT("Could not construct absolute path using '%s' as base; volumes do not match"),
      basePath
    );

  EsString::Array dirs = fnBase.dirsGet();
  size_t idx = 0;
  while(idx < m_dirs.size())
  {
    const EsString& dir = m_dirs[idx];
    if(esT("..") == dir && !dirs.empty())
      dirs.pop_back();
    else
      dirs.push_back( dir );

    ++idx;
  }

  EsString::value_type sep;
  if( fnBase.isUNC() )
    sep = esT('\\');
  else
    sep = (flags & static_cast<ulong>(EsPathFlag::NativeSeparator)) ? c_nativeSeparator : c_genericSeparator;
  // construct absolute path using base
  EsString result = fnBase.rootGet();
  idx = 0;
  while(idx < dirs.size())
  {
    result += sep;
    result += dirs[idx];
    ++idx;
  }

  // finalize path construction
  finalizePath(result, flags, sep);

  return result;
}
//---------------------------------------------------------------------------

EsString EsPath::constructPathAsIs(ulong flags) const
{
  ES_ASSERT(isOk());

  EsString::value_type sep;
  if( isUNC() )
    sep = esT('\\');
  else
    sep = (flags & static_cast<ulong>(EsPathFlag::NativeSeparator)) ? c_nativeSeparator : c_genericSeparator;

  EsString result = rootGet();
  size_t idx = 0;
  if( !m_dirs.empty() )
  {
    if( isAbsolute() )
      result += sep;
    result += m_dirs[idx++];
  }
  while(idx < m_dirs.size())
  {
    result += sep;
    result += m_dirs[idx];
    ++idx;
  }

  finalizePath(result, flags, sep);

  return result;
}
//---------------------------------------------------------------------------

EsString EsPath::pathGet(ulong flags, const EsString& basePath /*= EsString::null()*/) const
{
  ES_ASSERT(isOk());

  if( !(flags & static_cast<ulong>(EsPathFlag::AsIs)) )
  {
    if( hasVars() && (flags & static_cast<ulong>(EsPathFlag::ExpandEnvVars)) )
    {
      EsString tmp = constructPathAsIs(flags);
      const EsStringIndexedMap& vars = EsUtilities::environmentVarsListGet();
      expandVars(tmp, vars);
      const EsPath& path = isFile() ? createFromFilePath(tmp) :
        createFromPath(tmp);

      flags &= ~static_cast<ulong>(EsPathFlag::ExpandEnvVars);
      return path.pathGet(flags, basePath);
    }

    if(  isAbsolute() && (flags & static_cast<ulong>(EsPathFlag::Relative)) )
      return constructRelativePath(flags, basePath.empty() ? cwdGet() : basePath);
    else if( isRelative() && !(flags & static_cast<ulong>(EsPathFlag::Relative)) )
      return constructAbsolutePath(flags, basePath.empty() ? cwdGet() : basePath);
  }

  return constructPathAsIs(flags);
}
//---------------------------------------------------------------------------

EsString EsPath::makeSafe(const EsString& src, const EsString& symbols, EsString::value_type replacement)
{
  EsString syms = EsString(esT("[,")) + ES_PATH_INVALID_CHARS;
  if( !symbols.empty() )
    syms += symbols;
  syms += esT("]+");

  EsRegEx re( syms );

  ES_ASSERT(re.isOk());
  re.set_text(src);

  return re.replace(replacement);
}
//---------------------------------------------------------------------------

bool EsPath::exists(const EsString& path, const EsString& basePath )
{
  const EsPath& p = EsPath::createFromPath(path);
  return p.isOk() && p.exists(basePath);
}
//---------------------------------------------------------------------------

bool EsPath::fileExists(const EsString& path, const EsString& basePath)
{
  const EsPath& p = EsPath::createFromFilePath(path);
  return p.isOk() && p.fileExists(basePath);
}
//---------------------------------------------------------------------------

bool EsPath::dirExists(const EsString& path, const EsString& basePath)
{
  const EsPath& p = EsPath::createFromPath(path);
  return p.isOk() && p.dirExists(basePath);
}
//---------------------------------------------------------------------------

bool EsPath::exists(const EsString& basePath /*= EsString::s_null*/) const
{
  const EsString& a = pathGet(static_cast<ulong>(EsPathFlag::Default), basePath);
  return 0 != isExisting(a);
}
//---------------------------------------------------------------------------

bool EsPath::fileExists(const EsString& basePath /*= EsString::s_null*/) const
{
  const EsString& a = pathGet(static_cast<ulong>(EsPathFlag::Default), basePath);
  return isFile() && 1 == isExisting(a);
}
//---------------------------------------------------------------------------

bool EsPath::dirExists(const EsString& basePath /*= EsString::s_null*/) const
{
  const EsString& a = pathGet(static_cast<ulong>(EsPathFlag::Default), basePath);
  return !isFile() && 2 == isExisting(a);
}
//---------------------------------------------------------------------------

EsString::value_type EsPath::separatorGet() const
{
  if( m_state & stateUnc )
    return esT('\\');
  else
    return c_nativeSeparator;
}
//---------------------------------------------------------------------------

void EsPath::pathCreate(const EsString& basePath /*= EsString::s_null*/) const
{
  const EsString& a = pathGet(static_cast<ulong>(EsPathFlag::Default), basePath);
  const EsPath& p = EsPath::createFromPath(a);
  ES_ASSERT(p.isOk());
  ES_ASSERT(p.isAbsolute());

  const EsString::Array& dirs = p.dirsGet();
  EsString curPath = p.rootGet();
  size_t idx = 0;
  if( !dirs.empty() )
  {
    EsString::value_type sep = p.separatorGet();
    curPath += sep;
    curPath += dirs[idx++];
    if( !isExisting(curPath) )
      directoryCreate(curPath);

    while(idx < dirs.size())
    {
      curPath += sep;
      curPath += dirs[idx++];
      if( !isExisting(curPath) )
        directoryCreate(curPath);
    }
  }
}
//---------------------------------------------------------------------------

void EsPath::pathCreate(const EsString& path, const EsString& basePath)
{
  const EsPath& p = EsPath::createFromPath(path);
  ES_ASSERT(p.isOk());
  p.pathCreate(basePath);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#if ES_OS == ES_OS_WINDOWS
# include "EsPath.win.cxx"
#elif ES_OS == ES_OS_MAC
# include "EsPath.mac.cxx"
#elif ES_OS == ES_OS_IOS
# include "EsPath.ios.cxx"
#elif ES_OS == ES_OS_ANDROID
# include "EsPath.android.cxx"
#elif ES_OS == ES_OS_LINUX
# include "EsPath.linux.cxx"
#endif

#if defined(ES_POSIX_COMPAT)
# include "EsPath.posix.cxx"
#endif


