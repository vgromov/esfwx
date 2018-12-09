#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptDataStack.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

EsScriptDataStack::EsScriptDataStack() ES_NOTHROW
{
#if 1 == ES_USE_VECTOR_FOR_STACK
  m_data.reserve(depthDefault);
#endif
}
//---------------------------------------------------------------------------

// data class trace
#if defined(DEBUG) && defined(ESSCRIPT_DATA_STACK_USE_TRACE)
void EsScriptDataStack::trace() const
{
  ES_DEBUG_TRACE(esT("data stack trace:"));
  for( size_t idx = 0; idx < m_data.size(); ++idx )
    ES_DEBUG_TRACE(esT("\t[%d]='%s'"), m_data.size()-1-idx, m_data[m_data.size()-1-idx]->trace());
}
# define EsScriptDataStack_TRACE trace();
#  define ESSCRIPT_DATASTACK_TRACE1(a)                  ES_DEBUG_TRACE(a);
#  define ESSCRIPT_DATASTACK_TRACE2(a, b)              ES_DEBUG_TRACE((a), (b));
#  define ESSCRIPT_DATASTACK_TRACE3(a, b, c)            ES_DEBUG_TRACE((a), (b), (c));
#  define ESSCRIPT_DATASTACK_TRACE4(a, b, c, d)        ES_DEBUG_TRACE((a), (b), (c), (d));
#  define ESSCRIPT_DATASTACK_TRACE5(a, b, c, d, e)      ES_DEBUG_TRACE((a), (b), (c), (d), (e));
#  define ESSCRIPT_DATASTACK_TRACE6(a, b, c, d, e, f)  ES_DEBUG_TRACE((a), (b), (c), (d), (e), (f));
#else
# define EsScriptDataStack_TRACE
#  define ESSCRIPT_DATASTACK_TRACE1(a)
#  define ESSCRIPT_DATASTACK_TRACE2(a, b)
#  define ESSCRIPT_DATASTACK_TRACE3(a, b, c)
#  define ESSCRIPT_DATASTACK_TRACE4(a, b, c, d)
#  define ESSCRIPT_DATASTACK_TRACE5(a, b, c, d, e)
#  define ESSCRIPT_DATASTACK_TRACE6(a, b, c, d, e, f)
#endif
//---------------------------------------------------------------------------

void EsScriptDataStack::clear() ES_NOTHROW
{
  m_data.clear();
  ESSCRIPT_DATASTACK_TRACE1(esT("data stack cleared"))
}
//---------------------------------------------------------------------------

void EsScriptDataStack::truncate(size_t newSize)
{
  ES_ASSERT(newSize <= m_data.size());
  if(newSize < m_data.size())
  {
    ESSCRIPT_DATASTACK_TRACE3(esT("data stack truncated from %d to %d"), m_data.size(), newSize)
    m_data.resize(newSize);
  }
}
//---------------------------------------------------------------------------

void EsScriptDataStack::push(const EsScriptValAccessorIntf::Ptr& val) ES_NOTHROW
{
  m_data.push_back(val);

  ESSCRIPT_DATASTACK_TRACE2(esT("data pushed. stack depth %d"), m_data.size())
  EsScriptDataStack_TRACE
}
//---------------------------------------------------------------------------

EsScriptValAccessorIntf::Ptr EsScriptDataStack::pop()
{
  ES_ASSERT( !m_data.empty() );
  EsScriptValAccessorIntf::Ptr result = m_data.back();
  m_data.pop_back();

  ESSCRIPT_DATASTACK_TRACE2(esT("data popped. stack depth %d"), m_data.size() )
  EsScriptDataStack_TRACE

  return result;
}
//---------------------------------------------------------------------------

