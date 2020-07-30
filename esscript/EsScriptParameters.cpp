#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptParameters.h"
//--------------------------------------------------------------------------------

EsScriptParameters::EsScriptParameters() :
m_dbgLogSuppressIncl{esT(".*")},
m_debugLogSuppress(false),
m_dbgLogSuppressInclFirst(true),
m_inclDebugInfo(false),
m_translatablesSave(false)
{
}
//--------------------------------------------------------------------------------

void EsScriptParameters::load(const EsStreamIntf::Ptr& stream)
{
  EsStreamContextExistingScope scope(stream, esT("Params"));
  if(scope.isOk())
  {
    m_inclDebugInfo = stream->valueRead(
      esT("IncludeDebug"),
      false
    ).asBool();

    m_translatablesSave = stream->valueRead(
      esT("SaveTranslatables"),
      false
    ).asBool();

    m_debugLogSuppress = stream->valueRead(
      esT("DebugLogSuppress"),
      false
    ).asBool();

    m_dbgLogSuppressInclFirst = stream->valueRead(
      esT("DebugLogSuppressInclFirst"),
      true
    ).asBool();

    m_includes = stream->valueRead(
      esT("IncludesSearch"),
      EsString::nullArray()
    ).asStringCollection();

    m_linkPaths = stream->valueRead(
      esT("LibrariesSearch"),
      EsString::nullArray()
    ).asStringCollection();

    m_dbgLogSuppressIncl = stream->valueRead(
      esT("DbgLogSuppressIncl"),
      EsString::nullArray()
    ).asStringCollection();

    m_dbgLogSuppressExcl = stream->valueRead(
      esT("DbgLogSuppressExcl"),
      EsString::nullArray()
    ).asStringCollection();
  }
}
//---------------------------------------------------------------------------

void EsScriptParameters::save(const EsStreamIntf::Ptr& stream) const
{
  EsStreamContextExistingOrNewScope scope(stream, esT("Params"));
  if(scope.isOk())
  {
    stream->valueWrite(
      esT("IncludeDebug"),
      m_inclDebugInfo
    );

    stream->valueWrite(
      esT("SaveTranslatables"),
      m_translatablesSave
    );

    stream->valueWrite(
      esT("DebugLogSuppress"),
      m_debugLogSuppress
    );

    stream->valueWrite(
      esT("DebugLogSuppressInclFirst"),
      m_dbgLogSuppressInclFirst
    );

    stream->valueWrite(
      esT("IncludesSearch"),
      m_includes
    );

    stream->valueWrite(
      esT("LibrariesSearch"),
      m_linkPaths
    );

    stream->valueWrite(
      esT("DbgLogSuppressIncl"),
      m_dbgLogSuppressIncl
    );

    stream->valueWrite(
      esT("DbgLogSuppressExcl"),
      m_dbgLogSuppressExcl
    );
  }
}
//--------------------------------------------------------------------------------
