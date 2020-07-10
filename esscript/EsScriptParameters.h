#ifndef _es_escript_parameters_h_
#define _es_escript_parameters_h_

/// Common ES Script parameters
///
struct ESSCRIPT_CLASS EsScriptParameters
{
  EsScriptParameters();

  /// Parameters serialization
  void load(const EsStreamIntf::Ptr& stream);
  void save(const EsStreamIntf::Ptr& stream) const;

  /// Parameters data
  EsString::Array m_includes;
  EsString::Array m_linkPaths;
  EsString::Array m_dbgLogSuppressIncl;
  EsString::Array m_dbgLogSuppressExcl;
  bool m_debugLogSuppress;
  bool m_dbgLogSuppressInclFirst;
  bool m_inclDebugInfo;
  bool m_translatablesSave;
};
//---------------------------------------------------------------------------

#endif //< _es_escript_parameters_h_
