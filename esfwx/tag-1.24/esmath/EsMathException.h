#ifndef _es_math_exception_h_
#define _es_math_exception_h_

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
class ESMATH_PKG EsMathException : public EsException
#else
class ESMATH_CLASS EsMathException : public EsException
#endif
{
protected:
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  __fastcall
#endif
  EsMathException(const EsMathException& src);

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  __fastcall
#endif
	EsMathException(const EsString& msg, const EsScriptDebugInfoIntf::Ptr& dbg);

public:
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  __fastcall
#endif
	EsMathException(const EsMathException& src, const EsScriptDebugInfoIntf::Ptr& dbg);

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  virtual __fastcall ~EsMathException();
#endif

	static void Throw(const EsString& msg, const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr());
  static void ThrowArraySizesDoNotMatch( ulong cnt, ulong othercnt, const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr());

private:
  EsMathException& operator=(const EsMathException&) ES_REMOVEDECL;
};

#endif // _es_math_exception_h_

