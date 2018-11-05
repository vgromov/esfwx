#ifndef _es_path_h_
#define _es_path_h_

/// @name Path processing flags
///
/// @{
enum class EsPathFlag : ulong {
  Absolute				= 0,									///< construct absolute path (pseudoflag, for readability)
  Relative				= 0x0001,							///< construct relative path
  ExpandEnvVars		= 0x0002,							///< try to expand environment variables, like $ENV_VAR, encountered in path
  NativeSeparator = 0x0004,							///< use platform native path separators when generating path strings
  AppendSeparator = 0x0008,							///< append separator at the end of the non-empty path, if it does not contain file part
  ExcludeFile			= 0x0010,							///< exclude file name && ext path component when generating path string
  AsIs						= 0x0020,							///< construct path string as-is, relative|absolute|expand vars flags and base path got ignored
  Default					= static_cast<ulong>(EsPathFlag::Absolute)|				///< default flags:
                    static_cast<ulong>(EsPathFlag::ExpandEnvVars)|	///<	-create absolute path, expanding environment variables
                    static_cast<ulong>(EsPathFlag::NativeSeparator) ///<	-use native path separator
};
/// @}

/// Generalized reflected implementation of file system path
///
class ESCORE_CLASS ES_INTF_IMPL1(EsPath, EsReflectedClassIntf)

private:
	/// Internal state flags
	///  
	enum {
		stateOk					= 0x0001,
		stateUnc				= 0x0002,
		stateUncRemote	= 0x0004,
		stateAbsolute		= 0x0008,
		stateHasVars		= 0x0010,
	};

public:
	ES_DECL_REFLECTED_CLASS_BASE(EsPath)
	ES_DECL_ATTR_HANDLING_STD

  EsPath();
  EsPath& operator=(const EsPath& src);
  EsPath(const EsPath& src);

	static EsPath create(const EsString& path, const EsString& fileName, const EsString& fileExt);
	static EsPath createFromPath(const EsString& path);
	static EsPath createFromFilePath(const EsString& filePath);

	bool isOk() const { return stateOk == (m_state & stateOk); }
	bool isUNC() const { return stateUnc == (m_state & stateUnc); }
	bool isRemote() const { return stateUncRemote == (m_state & stateUncRemote); }
	bool isRelative() const { return !isAbsolute(); }
	bool isAbsolute() const { return stateAbsolute == (m_state & stateAbsolute); }
	bool hasVars() const { return stateHasVars == (m_state & stateHasVars); }
	bool isFile() const { return !m_fileName.empty() || !m_fileExt.empty(); }
	bool hasDirs() const { return !m_dirs.empty(); }
	bool exists(const EsString& basePath = EsString::null()) const;
	bool fileExists(const EsString& basePath = EsString::null()) const;
	bool dirExists(const EsString& basePath = EsString::null()) const;
	/// Return separator which is currently used
	EsString::value_type separatorGet() const;
	/// Attempt to create path, creating all intermediate directories, if needed
	void pathCreate(const EsString& basePath = EsString::null()) const;

	/// Path parts access
	///
	/// Root|Volume
	EsString rootGet() const;
	const EsString& volumeGet() const { return m_volume; }
	/// Dirs manipulation
	ES_DECL_REFLECTED_METHOD1(void, dirAppend, cr_EsString);
	ES_DECL_REFLECTED_METHOD0(void, dirLastRemove);
	const EsString::Array& dirsGet() const { return m_dirs; }
	/// File name|ext
	const EsString& fileNameGet() const { return m_fileName; }
	void fileNameSet(const EsString& name);
	const EsString& fileExtGet() const { return m_fileExt; }
	void fileExtSet(const EsString& ext);
	EsString fileNameExtGet() const;

	/// Whole path access
	///
	/// Try build and return path, relative to the basePath argument. if basePath is empty, current working directory is used
	EsString pathGet(ulong flags = static_cast<ulong>(EsPathFlag::Default), const EsString& basePath = EsString::null() ) const;
	ES_DECL_REFLECTED_CONST_METHOD2(EsString, pathGetReflected, ulong, cr_EsString);

	/// @name Reflected properties
  ///
	/// @{
	ES_DECL_PROPERTY_RO(isOk, bool)
	ES_DECL_PROPERTY_RO(isUNC, bool)
	ES_DECL_PROPERTY_RO(isRemote, bool)
	ES_DECL_PROPERTY_RO(isRelative, bool)
	ES_DECL_PROPERTY_RO(isAbsolute, bool)
	ES_DECL_PROPERTY_RO(hasVars, bool)
	ES_DECL_PROPERTY_RO(isFile, bool)
	ES_DECL_PROPERTY_RO(hasDirs, bool)
	ES_DECL_PROPERTY_RO(root, EsString)
	ES_DECL_PROPERTY_RO(volume, EsString)
	ES_DECL_PROPERTY_RO(dirs, EsStringArray)
	ES_DECL_PROPERTY(fileName, EsString)
	ES_DECL_PROPERTY(fileExt, EsString)
	ES_DECL_PROPERTY_RO(fileNameExt, EsString)
  /// @}

	/// @name Reflected services
	///
	/// @{
  ES_DECL_REFLECTED_CONST_METHOD1(bool, existsReflected, cr_EsString);
	ES_DECL_REFLECTED_CONST_METHOD1(bool, fileExistsReflected, cr_EsString);
	ES_DECL_REFLECTED_CONST_METHOD1(bool, dirExistsReflected, cr_EsString);
	ES_DECL_REFLECTED_CONST_METHOD0(EsString, separatorGetReflected);
	ES_DECL_REFLECTED_CONST_METHOD1(void, pathCreateReflected, cr_EsString);
  /// @}

	/// @name Static services
	///
  /// @{
	/// Get current working directory
	ES_DECL_REFLECTED_CLASS_METHOD0(EsString, cwdGet);
	/// Set current working directory
	ES_DECL_REFLECTED_CLASS_METHOD1(void, cwdSet, cr_EsString);
	/// Check if path exists. p1 - path, p2 - base path
	ES_DECL_REFLECTED_CLASS_METHOD2(bool, exists, cr_EsString, cr_EsString);
	/// Check if file exists. p1 - path, p2 - base path
	ES_DECL_REFLECTED_CLASS_METHOD2(bool, fileExists, cr_EsString, cr_EsString);
	/// Check if directory exists. p1 - path, p2 - base path
	ES_DECL_REFLECTED_CLASS_METHOD2(bool, dirExists, cr_EsString, cr_EsString);
	/// Attempt to create path, creating all intermediate directories, if needed.
	/// p1 - path, p2 - base path
	ES_DECL_REFLECTED_CLASS_METHOD2(void, pathCreate, cr_EsString, cr_EsString);
	/// Remove file object. p1 - file path
	ES_DECL_REFLECTED_CLASS_METHOD1(void, fileRemove, cr_EsString);
	/// Remove dir object. p1 - directory path
	ES_DECL_REFLECTED_CLASS_METHOD1(void, dirRemove, cr_EsString);
	/// Convert string to the filename-safe form, i.e. replace all non safe symbols like /\:*?<>|
	/// with specified replacement symbol, '_' by-default. If additional symbols need to be replaced in file name,
	/// they should be specified in symbols parameter
	///
	static EsString makeSafe(const EsString& src, const EsString& symbols = EsString::null(), EsString::value_type replacement = esT('_'));
	ES_DECL_REFLECTED_CLASS_METHOD1(EsString, makeSafeReflected, cr_EsString);
	ES_DECL_REFLECTED_CLASS_METHOD3(EsString, makeSafeReflected, cr_EsString, cr_EsString, cr_EsVariant);
	/// Path part validity check
	static void checkPathStringValidity(const EsString& str, const EsString& context);
  /// @}


	/// @name Constants
	///
  /// @{
	static const EsString::value_type c_nativeSeparator;
	static const EsString::value_type c_genericSeparator;
  /// @}

	/// @name Standard paths access
	///
  /// @{
  ES_DECL_REFLECTED_CLASS_METHOD0(EsString, stdAppPathGet);
	ES_DECL_REFLECTED_CLASS_METHOD0(EsString, stdAppDataGet);
	ES_DECL_REFLECTED_CLASS_METHOD0(EsString, stdLocalAppDataGet);
	ES_DECL_REFLECTED_CLASS_METHOD0(EsString, stdHomeGet);
	ES_DECL_REFLECTED_CLASS_METHOD0(EsString, stdDocsGet);
	ES_DECL_REFLECTED_CLASS_METHOD0(EsString, stdPublicDocsGet);
	ES_DECL_REFLECTED_CLASS_METHOD0(EsString, stdDownloadsGet);
	ES_DECL_REFLECTED_CLASS_METHOD0(EsString, stdPublicDownloadsGet);
	ES_DECL_REFLECTED_CLASS_METHOD0(EsString, stdSysRootGet);
  ES_DECL_REFLECTED_CLASS_METHOD0(EsStringArray, stdDataRootsGet);
  /// @}

  /// @name Platform-specific stuff
  ///
  /// @{
#if ES_OS == ES_OS_WINDOWS
  ES_DECL_REFLECTED_CLASS_METHOD0(EsStringArray, stdDriveNamesGet);
#endif
  /// @}

	/// Reflected creators
	ES_DECL_REFLECTED_CLASS_METHOD3(EsBaseIntfPtr, createReflected, cr_EsString, cr_EsString, cr_EsString);
	ES_DECL_REFLECTED_CLASS_METHOD1(EsBaseIntfPtr, createFromPathReflected, cr_EsString);
	ES_DECL_REFLECTED_CLASS_METHOD1(EsBaseIntfPtr, createFromFilePathReflected, cr_EsString);

protected:
	// Check if path contains something at checkPos, that's not a legal separator char
	bool isRelativeRoot(size_t checkPos, const EsString& path);
	static void expandVars(EsString& str, const EsStringIndexedMap& vars);

	// Match and initialize root and volume fields, returned is offset from the
	// start of the path, where this service finished its job. 0 is returned if no root
	// and|or volume was matched
	size_t rootAndVolumeInit(const EsString& path);
	void fileNameExtParse(const EsString& fileNameExt);
	void pathParse(const EsString& path, bool hasFileName);

  // Path check
  static long isExisting(const EsString& obj);

  // Dir creator
  static void directoryCreate(const EsString& path);

	// Internal path component setters
	void rootSet(const EsString& root);
	void volumeSet(const EsString& volume);

	// Path construction stuff
	//
	void finalizePath(EsString& path, ulong flags, EsString::value_type sep) const;
	EsString constructAbsolutePath(ulong flags, const EsString& basePath) const;
	EsString constructRelativePath(ulong flags, const EsString& basePath) const;
	EsString constructPathAsIs(ulong flags) const;

protected:
	ulong m_state;
  EsString m_orig;
	EsString m_root;
	EsString m_volume;
	EsString::Array m_dirs;
	EsString m_fileName;
	EsString m_fileExt;
};

namespace EsReflection
{
/// Path class flags reflected enumeration
///
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCORE_CLASS, EsPathFlag)
	ES_DECL_ENUMERATION_ITEM_LABELLED(Absolute,					static_cast<ulong>(::EsPathFlag::Absolute),	esT("Construct absolute path"))
	ES_DECL_ENUMERATION_ITEM_LABELLED(Relative, 				static_cast<ulong>(::EsPathFlag::Relative),	esT("Construct relative path"))
	ES_DECL_ENUMERATION_ITEM_LABELLED(ExpandEnvVars, 		static_cast<ulong>(::EsPathFlag::ExpandEnvVars), esT("Try to expand environment variables, like $ENV_VAR, encountered in path"))
	ES_DECL_ENUMERATION_ITEM_LABELLED(NativeSeparator,	static_cast<ulong>(::EsPathFlag::NativeSeparator), esT("Use platform native path separators when generating path strings"))
	ES_DECL_ENUMERATION_ITEM_LABELLED(AppendSeparator,	static_cast<ulong>(::EsPathFlag::AppendSeparator), esT("Append separator at the end of the non-empty path, if it does not contain file part"))
	ES_DECL_ENUMERATION_ITEM_LABELLED(ExcludeFile,			static_cast<ulong>(::EsPathFlag::ExcludeFile), esT("Exclude file name and ext path component when generating path string"))
	ES_DECL_ENUMERATION_ITEM_LABELLED(AsIs,							static_cast<ulong>(::EsPathFlag::AsIs), esT("Construct path string as-is, relative|absolute|expand vars flags and base path got ignored"))
	ES_DECL_ENUMERATION_ITEM_LABELLED(Default,					static_cast<ulong>(::EsPathFlag::Default), esT("Default flags: create absolute path, expand environment variables, use native path separator"))
ES_DECL_ENUMERATION_END
}

#endif // _es_path_h_
