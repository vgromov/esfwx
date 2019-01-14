#ifndef _es_exception_h_
#define _es_exception_h_

// Adapt our exceptions to the Delphi|C++ Builder Exception,
// or else C++ builder applications will not be able to intercept them in
// standard way (EExternal is caught instead)
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
#  include <System.SysUtils.hpp>
/// Generic core exception class
///
class PACKAGE EsException : public Exception
#else
class ESCORE_CLASS EsException
#endif
{
public:
  enum {
    // exception flavours
    severityWarning = 0,
    severityGeneric,
    severityCritical,
    // facility exception originated from
    facilityEsUnknown = 0xFFFF,
    facilityEsCommon,
    facilityEsOS,
    facilityEsNumeric,
    facilityEsFile,
    facilityEsSecurity,
    facilityEsRPC,
    facilityEsScript,
    facilityEsXml,
    facilityEsSockets,
    facilityEsMath,
    facilityEsRegEx
  };

public:
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  __fastcall
#endif
  EsException(const EsException& src);

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  __fastcall
#endif
  EsException(ulong code, ulong severity, ulong facility, const EsString& msg,
    const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr(),
    bool doLogErrors = true
  );

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  __fastcall
#endif
  EsException(const EsException& src, const EsScriptDebugInfoIntf::Ptr& dbg);

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  virtual __fastcall ~EsException();
#endif

  ulong codeGet() const ES_NOTHROW;
  ulong severityGet() const ES_NOTHROW;
  ulong facilityGet() const ES_NOTHROW;
  const EsString& messageGet() const ES_NOTHROW;
  const EsVariant& dataGet() const ES_NOTHROW;
  EsScriptDebugInfoIntf::Ptr debugInfoGet() const ES_NOTHROW;
  
  // Static services
  //
  // Pseudo-variadic Throw methods
  static void Throw(ulong code, ulong severity, ulong facility, EsString::const_pointer fmt);
  static void Throw(ulong code, ulong severity, ulong facility, EsString::const_pointer fmt, const EsVariant& arg0);
  static void Throw(ulong code, ulong severity, ulong facility, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1);
  static void Throw(ulong code, ulong severity, ulong facility, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2);
  static void Throw(ulong code, ulong severity, ulong facility, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3);
  static void Throw(ulong code, ulong severity, ulong facility, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4);
  static void Throw(ulong code, ulong severity, ulong facility, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5);
  static void Throw(ulong code, ulong severity, ulong facility, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6);
  static void Throw(ulong code, ulong severity, ulong facility, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7);
  static void Throw(ulong code, ulong severity, ulong facility, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8);

  static void Throw(EsString::const_pointer fmt);
  static void Throw(EsString::const_pointer fmt, const EsVariant& arg0);
  static void Throw(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1);
  static void Throw(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2);
  static void Throw(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3);
  static void Throw(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4);
  static void Throw(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5);
  static void Throw(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6);
  static void Throw(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7);
  static void Throw(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8);

  // Standard exceptions
  static void ThrowOsError(ulong code, const EsString& fmt = EsString::null());
  static void ThrowOsErrorStr(ulong code, const EsString& errstr, const EsString& fmt = EsString::null());
  static void ThrowNoValue(const EsString& ctx = EsString::null());
  static void ThrowCannotIndexItem(const EsString& ctx = EsString::null());
  static void ThrowNotSupportedForThisType(const EsString& op = EsString::null());
  static void ThrowNotSupportedForThisType(const EsString& type, const EsString& op);
  static void ThrowDivisionByZero(const EsString& ctx = EsString::null());
  static void ThrowOverflowInOperation(const EsString& op, const EsString& ctx = EsString::null());
  static void ThrowUnderflowInOperation(const EsString& op, const EsString& ctx = EsString::null());
  static void ThrowObjectTypeIsIncompatibleWith(const EsString& className, const EsString& ctx = EsString::null());
  static void ThrowUserDoesNotExist(const EsString& name, const EsString& ctx = EsString::null());
  static void ThrowUserAlreadyExists(const EsString& name, const EsString& ctx = EsString::null());
  static void ThrowAddingUsersWithAslIsNotAllowed(ulong asl, const EsString& ctx = EsString::null());
  static void ThrowPasswordNotValid(const EsString& ctx = EsString::null());
  static void ThrowUsersMayOnlyChangeThemselves(const EsString& ctx = EsString::null());
  static void ThrowAttributeDoesNotExist(const EsString& attribute, const EsString& typeName, const EsString& ctx = EsString::null());
  static void ThrowIncompatibleObjectTypes(const EsString& o1, const EsString& o2, const EsString& ctx = EsString::null());
  static void ThrowWrongObjectSize(const EsString& name, size_t expectedSize, size_t size, const EsString& ctx = EsString::null());
  static void ThrowIsNotAnObject(const EsString& ctx = EsString::null());
  static void ThrowObjectIsNotReflected(const EsString& ctx = EsString::null());

protected:
  void logEsException() const ES_NOTHROW;

protected:
  EsString m_msg;
  EsVariant m_data;
  EsScriptDebugInfoIntf::Ptr m_dbg;
  ulong m_code;
  ulong m_severity;
  ulong m_facility;

private:
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  __fastcall
#endif
  EsException() ES_REMOVEDECL;
  EsException& operator=(const EsException&) ES_REMOVEDECL;
};

// helper class for numeric checks
//
ES_DECL_EXPORTED_REFLECTED_SERVICES_BEGIN(EsNumericCheck, ESCORE_CLASS )

  /// void checkRangeFloat(double min, double max, double val, const EsString& name)
  ES_DECL_REFLECTED_CLASS_METHOD4(void, checkRangeFloat, double, double, double, cr_EsString);
  static inline void checkRangeFloat(double min, double max, double val) { checkRangeFloat(min, max, val, EsString::null()); }
  // void checkRangeInteger(long min, long max, long val, const EsString& name)
  ES_DECL_REFLECTED_CLASS_METHOD4(void, checkRangeInteger, long, long, long, cr_EsString);
  static inline void checkRangeInteger(long min, long max, long val) { checkRangeInteger(min, max, val, EsString::null()); }
  // void checkRangeUInteger(ulong min, ulong max, ulong val, const EsString& name)
  ES_DECL_REFLECTED_CLASS_METHOD4(void, checkRangeUInteger, ulong, ulong, ulong, cr_EsString);
  static inline void checkRangeUInteger(ulong min, ulong max, ulong val) { checkRangeUInteger(min, max, val, EsString::null()); }
  // void checkRangeLLInteger(llong min, llong max, llong val, const EsString& name)
  ES_DECL_REFLECTED_CLASS_METHOD4(void, checkRangeLLInteger, llong, llong, llong, cr_EsString);
  static inline void checkRangeLLInteger(llong min, llong max, llong val) { checkRangeLLInteger(min, max, val, EsString::null()); }
  // void checkRangeULLInteger(ullong min, ullong max, ullong val, const EsString& name)
  ES_DECL_REFLECTED_CLASS_METHOD4(void, checkRangeULLInteger, ullong, ullong, ullong, cr_EsString);
  static inline void checkRangeULLInteger(ullong min, ullong max, ullong val) { checkRangeULLInteger(min, max, val, EsString::null()); }

ES_DECL_REFLECTED_SERVICES_END
#endif // _es_exception_h_