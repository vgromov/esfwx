//---------------------------------------------------------------------------
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>
//---------------------------------------------------------------------------

bool EsPathEnumerator::internalProcess(const EsString& nestedDir /*= EsString::null()*/)
{
  bool result = true;
  EsPathScope scope(
    m_curPath,
    nestedDir
  );

  // reserve space for nested dirs
  EsString::Array dirs;
  dirs.reserve(16);
  EsString curPath = m_curPath.pathGet(  static_cast<ulong>(EsPathFlag::Default)|
                                        static_cast<ulong>(EsPathFlag::AppendSeparator)|
                                        static_cast<ulong>(EsPathFlag::ExcludeFile) );
  const EsByteString& patt = EsString::toUtf8( m_wildcard );

  const EsByteString& bpath = EsString::toUtf8(curPath);
  DIR* dirp = opendir( bpath.c_str() );

  if( !dirp )
    EsException::ThrowOsError(
      EsUtilities::osErrorCodeGet(),
      esT("EsPathEnumerator error: '%s'")
    );

  try
  {
    do
    {
      errno = 0;
      dirent* entry = readdir( dirp );

      if( entry )
      {
        if( 0 == fnmatch( patt.c_str(), entry->d_name, 0 ) )
        {
          EsByteString bname( entry->d_name );
          const EsString& name = EsString::fromUtf8( bname );

          bool isDir = DT_DIR == entry->d_type;
          bool isLink = DT_LNK == entry->d_type;
          if( isLink )
          {
            bname = bpath+bname+"/";  //< Add trailing slash, to 'look' at link destination, not link itself
            struct stat st;
            int ret = lstat( bname.c_str(), &st );
            if( 0 == ret )
              isDir = S_ISDIR( st.st_mode );
            else if( ENOENT != ret )
              EsException::ThrowOsError(
                EsUtilities::osErrorCodeGet()
              );
          }

          if( isDir &&
              (m_flags & static_cast<ulong>(EsPathEnumeratorFlag::Recursive)) &&
              esT(".") != name &&
              esT("..") != name
          )
            dirs.push_back(name);

          result = onObject(curPath, name, isDir);
        }
      }
      else
        break;

    } while( result );
  }
  catch(...)
  {
    if(dirp)
    {
      closedir(dirp);
      dirp = 0;
    }

    // Rethrow
    throw;
  }

  if(dirp)
  {
    closedir(dirp);
    dirp = 0;
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
