//---------------------------------------------------------------------------

bool EsPathEnumerator::internalProcess(const EsString& nestedDir /*= EsString::null()*/)
{
	bool result = true;
	EsPathScope scope(m_curPath, nestedDir);
	// reserve space for nested dirs
	EsString::Array dirs;
	dirs.reserve(16);
	EsString curPath = m_curPath.pathGet(	static_cast<ulong>(EsPathFlag::Default)|
																				static_cast<ulong>(EsPathFlag::AppendSeparator)|
																				static_cast<ulong>(EsPathFlag::ExcludeFile) );
	const EsString& findStr = curPath + m_wildcard;

	// Process objects in current path
	WIN32_FIND_DATA data;
	HANDLE ff = ::FindFirstFile(findStr.c_str(), &data);
	if( INVALID_HANDLE_VALUE != ff )
	{
		try
		{
			do
			{
				EsString name(data.cFileName);
				bool isDir = FILE_ATTRIBUTE_DIRECTORY == (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
				if( isDir && (m_flags & static_cast<ulong>(EsPathEnumeratorFlag::Recursive)) &&
						esT(".") != name && esT("..") != name )
					dirs.push_back(name);

				result = onObject(curPath, name, isDir);

			} while( result && ::FindNextFile(ff, &data) );
		}
		catch(...)
		{
			::FindClose(ff);
			// rethrow
			throw;
		}
		::FindClose(ff);
	}

	// proceed with recursion, if needed
	if( result &&
      (m_flags & static_cast<ulong>(EsPathEnumeratorFlag::Recursive)) &&
      !dirs.empty()
  )
	{
		for(size_t idx = 0; idx < dirs.size() && result; ++idx)
			result = internalProcess(dirs[idx]);
	}

	return result;
}
//---------------------------------------------------------------------------
