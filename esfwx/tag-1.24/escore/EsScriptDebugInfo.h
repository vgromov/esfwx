#ifndef _es_script_debug_info_h_
#define _es_script_debug_info_h_

// script debug information interface and its implementations class
//
ES_DECL_INTF_BEGIN2(288D7781, 38564AFD, A2D82982, B74F43FD, EsScriptDebugInfoIntf, EsBaseIntf)
  ES_DECL_INTF_METHOD(ulong, lineGet)() const = 0;
  ES_DECL_INTF_METHOD(ulong, columnGet)() const = 0;  
  ES_DECL_INTF_METHOD(cr_EsString, fileGet)() const = 0;
  ES_DECL_INTF_METHOD(EsString, asString)() const = 0;
ES_DECL_INTF_END

/// EsScriptDebugInfoIntf implementation class.
/// It contains source file reference and positional information about debug event occurred.
/// For convenience, positional information is given in line, column coordinates,
/// both are 1-based
///
class ESCORE_CLASS ES_INTF_IMPL1(EsScriptDebugInfo,  EsScriptDebugInfoIntf)

protected:
  EsScriptDebugInfo(ulong line, ulong col, const EsString& file) ES_NOTHROW;

public:
  static EsScriptDebugInfoIntf::Ptr create(ulong line, ulong col, const EsString& file) ES_NOTHROW;
  static EsScriptDebugInfoIntf::Ptr create(const EsString& input, EsString::const_pointer pos, const EsString& file = EsString::null()) ES_NOTHROW;

  /// EsBaseIntf
  ///
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW { return esT("EsScriptDebugInfo"); }

  /// EsScriptDebugInfoIntf implementation
  ///
  ES_DECL_INTF_METHOD(ulong, lineGet)() const ES_NOTHROW { return m_line; }
  ES_DECL_INTF_METHOD(ulong, columnGet)() const ES_NOTHROW { return m_col; }
  ES_DECL_INTF_METHOD(cr_EsString, fileGet)() const ES_NOTHROW { return m_file; }
  ES_DECL_INTF_METHOD(EsString, asString)() const;

protected:
  ulong m_line;
  ulong m_col;
  EsString m_file;
};

#endif