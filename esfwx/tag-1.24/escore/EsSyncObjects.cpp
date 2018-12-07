#include "escorepch.h"
#pragma hdrstop

#include "EsSyncObjects.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

EsMutex::Result EsMutex::tryLock()
{
  return lock(0);
}
//---------------------------------------------------------------------------

EsMutex::Result EsMutex::lock()
{
  return lock(ES_INFINITE);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsMutexLocker::EsMutexLocker(EsMutex& mx) :
m_mx(mx),
m_ok(false)
{
  ES_ASSERT( m_mx.isOk() );
  m_ok = EsMutex::resultOk == m_mx.lock();
}
//---------------------------------------------------------------------------

EsMutexLocker::~EsMutexLocker() ES_NOTHROW
{
  if( m_ok )
    m_mx.unlock();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsCriticalSectionLocker::EsCriticalSectionLocker(EsCriticalSection& obj) :
m_obj(obj)
{
  m_obj.enter();
}
//---------------------------------------------------------------------------

EsCriticalSectionLocker::~EsCriticalSectionLocker() ES_NOTHROW
{
  m_obj.leave();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsCriticalSectionPtrLocker::EsCriticalSectionPtrLocker( EsCriticalSection::Ptr& obj ) :
  m_obj( obj )
{
  if(m_obj)
    m_obj->enter();
}
//---------------------------------------------------------------------------

EsCriticalSectionPtrLocker::~EsCriticalSectionPtrLocker() ES_NOTHROW
{
  if( m_obj )
    m_obj->leave();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// Semaphore
//
EsSemaphore::Result EsSemaphore::wait()
{
  return wait(ES_INFINITE);
}
//---------------------------------------------------------------------------

EsSemaphore::Result EsSemaphore::tryWait()
{
  return wait(0);
}
//---------------------------------------------------------------------------

#if ES_OS == ES_OS_WINDOWS
# include "EsSyncObjects.win.cxx"
#elif defined(ES_POSIX_COMPAT)
# include "EsSyncObjects.posix.cxx"
#else
# error Syncronization objects implementation is not defined for this platform!
#endif
