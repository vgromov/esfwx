#include "escorepch.h"
#pragma hdrstop

#include "EsScriptDebugInfo.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

class EsScriptDebugInfoCtorAcc : public EsScriptDebugInfo
{
public:
  EsScriptDebugInfoCtorAcc(ulong line, ulong col, const EsString& file) :
  EsScriptDebugInfo(
    line,
    col,
    file
  )
  {}
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsScriptDebugInfo::EsScriptDebugInfo(ulong line, ulong col, const EsString& file) ES_NOTHROW :
m_line(line),
m_col(col),
m_file(file)
{
}
//---------------------------------------------------------------------------

EsScriptDebugInfoIntf::Ptr EsScriptDebugInfo::create(ulong line, ulong col, const EsString& file) ES_NOTHROW
{
  std::unique_ptr<EsScriptDebugInfoCtorAcc> ptr = ES_MAKE_UNIQUE(
    EsScriptDebugInfoCtorAcc,
    line,
    col,
    file
  );
  ES_ASSERT(ptr);

  ptr->m_dynamic = true;
  return ptr.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsScriptDebugInfoIntf::Ptr EsScriptDebugInfo::create(const EsString& input, EsString::const_pointer pos, const EsString& file /*= EsString::s_null*/) ES_NOTHROW
{
  ulong line = 1;
  EsString::const_pointer start = input.c_str();
  EsString::const_pointer cur = start;
  while( cur <= pos )
  {
    if( esT('\n') == *cur++ )
    {
      ++line;
      if( esT('\r') == *cur )
        ++cur;

      start = cur;
    }
  }
  ulong col = 1 + ( (pos > start) ? static_cast<ulong>(pos-start) : 0);

  return create(
    line,
    col,
    file
  );
}
//---------------------------------------------------------------------------

EsString EsScriptDebugInfo::asString() const
{
  if( m_file.empty() )
    return EsString::format(
      esT("at (%d, %d)"),
      m_line,
      m_col
    );
  else
    return EsString::format(
      esT("in '%s' at (%d, %d)"),
      m_file,
      m_line,
      m_col
    );
}
//---------------------------------------------------------------------------
