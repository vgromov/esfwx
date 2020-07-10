#include "escorepch.h"
#pragma hdrstop

#include "EsPathEnumerator.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

class EsPathScope
{
public:
  EsPathScope(EsPath& path, const EsString& nestedDir = EsString::null()) :
  m_path(path),
  m_nested(false)
  {
    if( !nestedDir.empty() )
    {
      m_nested = true;
      m_path.dirAppend(nestedDir);
    }
  }

  ~EsPathScope()
  {
    if( m_nested )
      m_path.dirLastRemove();
  }

private:
  EsPath& m_path;
  bool m_nested;

private:
  EsPathScope() ES_REMOVEDECL;
  EsPathScope(const EsPathScope&) ES_REMOVEDECL;
  EsPathScope& operator=(const EsPathScope&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

EsPathEnumerator::EsPathEnumerator(const EsPath& path, const EsString& wildcard /*= esT("*")*/) :
m_flags(static_cast<ulong>(EsPathEnumeratorFlag::Default)),
m_curPath(EsPath::createFromPath(path.pathGet())),
m_wildcard(wildcard)
{
}
//---------------------------------------------------------------------------

EsPathEnumerator::EsPathEnumerator(const EsString& path, const EsString& wildcard /*= esT("*")*/) :
m_flags(static_cast<ulong>(EsPathEnumeratorFlag::Default)),
m_curPath(EsPath::createFromPath(path)),
m_wildcard(wildcard)
{
}
//---------------------------------------------------------------------------

void EsPathEnumerator::execute(ulong flags)
{
  ES_ASSERT(m_curPath.isOk());

  if( m_flags != flags )
    m_flags = flags;

  onStart();
  internalProcess();
  onEnd();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

namespace EsReflection
{
ES_DECL_ENUMERATION_INFO(EsPathEnumeratorFlag, NO_CLASS_DESCR)
}
//---------------------------------------------------------------------------

class EsPathEnumeratorImpl : public EsPathEnumerator
{
public:
  EsPathEnumeratorImpl(EsPathEnumeratorHelper& proxy, const EsString& path, const EsString& wildcard) :
  EsPathEnumerator(path, wildcard),
  m_proxy(proxy)
  {}

protected:
  virtual void onStart()
  {
    if( m_proxy.m_owner->hasMethod( EsMethodInfoKeyT(0, esT("onStart")) ) )
      m_proxy.m_owner->call(esT("onStart"));
  }
  virtual bool onObject(const EsString& curPath, const EsString& name, bool isDir)
  {
    if( m_proxy.m_owner->hasMethod( EsMethodInfoKeyT(3, esT("onObject")) ) )
      return m_proxy.m_owner->call(esT("onObject"), curPath, name, isDir).asBool();

    return true;
  }

protected:
  EsPathEnumeratorHelper& m_proxy;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class EsPathEnumeratorHelperCtorAccessor : public EsPathEnumeratorHelper
{
public:
  EsPathEnumeratorHelperCtorAccessor(const EsBaseIntfPtr& owner) :
  EsPathEnumeratorHelper(owner)
  {}
};
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsPathEnumeratorHelper, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(EsPathEnumeratorHelper, EsBaseIntfPtr_ClassCall_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsPathEnumeratorHelper, enumerate, void_Call_cr_EsString_cr_EsString_ulong, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsPathEnumeratorHelper::EsPathEnumeratorHelper(const EsBaseIntfPtr& owner)
{
  EsBaseIntfPtr tmp( owner.get(), false, false ); // ensure we're using weak reference
  m_owner = tmp;

  ES_ASSERT(m_owner);
  m_dynamic = true;
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsPathEnumeratorHelper::NEW(cr_EsBaseIntfPtr owner)
{
  std::unique_ptr<EsPathEnumeratorHelperCtorAccessor> enu = ES_MAKE_UNIQUE(
    EsPathEnumeratorHelperCtorAccessor,
    owner
  );
  ES_ASSERT(enu);

  return enu.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

void EsPathEnumeratorHelper::enumerate(cr_EsString path, cr_EsString wildcard, ulong flags)
{
  EsPathEnumeratorImpl impl(*this, path, wildcard);
  impl.execute(flags);
}
//---------------------------------------------------------------------------

#if ES_OS == ES_OS_WINDOWS
# include "EsPathEnumerator.win.cxx"
#elif defined(ES_POSIX_COMPAT)
# include "EsPathEnumerator.posix.cxx"
#endif
//---------------------------------------------------------------------------

