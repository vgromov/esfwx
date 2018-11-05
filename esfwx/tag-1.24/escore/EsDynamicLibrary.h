#ifndef _es_dynamic_library_h_
#define _es_dynamic_library_h_

#if ES_OS == ES_OS_WINDOWS
	typedef HMODULE EsDynamicLibraryHandle;
#elif defined(ES_POSIX_COMPAT)
	typedef void* EsDynamicLibraryHandle;
#endif

#if ES_OS == ES_OS_WINDOWS || \
    defined(ES_POSIX_COMPAT)

#define EsDynamicLibrary_SUPPORTED

/// Dynamically linked modules support classes
///
class ESCORE_CLASS EsDynamicLibrary
{
public:
	typedef void (*Pfn)();
	typedef std::shared_ptr<EsDynamicLibrary> Ptr;

private:
	EsDynamicLibrary(EsDynamicLibraryHandle handle, const EsString& path);

public:
	~EsDynamicLibrary();

	// check if library is loaded
	bool isOk() const { return 0 != m_handle; }
	// load library, specified by path argument. if library could not be loaded
	// and doThrow is true (default), an exception is thrown
	static Ptr load(const EsString& path, bool doThrow = true);
	void unload();
	// return information of all libraries, loaded by the current process
	static const EsStringIndexedMap& loadedLibrariesGet();
  // return library full path
  const EsString& pathGet() const { return m_path; }
  // return library version string
  const EsString& versionGet() const { return m_version; }
	// try to acquire exported symbol address
	void* symbolAddrGet(const EsString& name, bool doThrow = true);
	// try to acquire procedure address by its name
	// if address is not found, and doThrow is true (default)
	// an exception is thrown
	Pfn procAddrGet(const EsString& name, bool doThrow = true);
	// somewhat msw - specific
	Pfn procAddrGetAorW(const EsString& name, bool doThrow = true);

protected:
	void checkLoaded() const;
  static EsString versionExtract(const EsString& path);
  // library info (version) indexed by its full path
	static EsStringIndexedMap& libsGet();

private:
	EsDynamicLibraryHandle m_handle;
  EsString m_path;
	EsString m_version;

private:
	// prohibited functionality
	EsDynamicLibrary() ES_REMOVEDECL;
	EsDynamicLibrary(const EsDynamicLibrary&) ES_REMOVEDECL;
	EsDynamicLibrary& operator= (const EsDynamicLibrary&) ES_REMOVEDECL;
};

#endif // EsDynamicLibrary defined

#endif // _es_dynamic_library_h_
