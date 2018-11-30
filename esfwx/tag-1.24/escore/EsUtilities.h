#ifndef _es_utilities_h_
#define _es_utilities_h_

#include <math.h>

/// @file				EsUtilities.h
///							Ekosf Core Utilities
///							Misc. utility classes and procedures for Ekosf Core
///	Author:			Vsevolod V Gromov
///	Copyright:	EkoSfera 2009-2013
///

/// Application log levels
/// Log messages got sent through common EsEventDispatcher,
/// using "applog" category. Standard log event id is always 0,
/// and log level is the first parameter in payload, logged value follows.
enum class EsAppLogLevel : ulong {
	Info,				///< Generic log message
	Warning,		///< Warning log message
	Error,			///< Error log message
	Debug,      ///< Debug log message
	Custom,			///< Custom log levels, if any, should have values greater than
											///	or equal to this id.
};

/// Application security levels
///
enum class EsAppSecurityLevel : ulong {
	aslNone,						///< No user is logged on
	aslGuest,
	aslUser,
	aslSuperuser,
	aslAdministrator,
	aslDeveloper,
};

#ifndef ES_CONST_ARRAY_COUNT
/// Return count of items in constant array, declared elsewhere
///
#	define ES_CONST_ARRAY_COUNT(a)	static_cast<size_t>(sizeof(a)/sizeof(a[0]))
#endif

//---------------------------------------------------------------------------
namespace EsReflection {

/// Reflected Core utility services
///
ES_DECL_REFLECTED_SERVICES_BEGIN( EsUtilities )

  /// Take desired systick period, and return actually set value, with which
  /// systickRestore should be called
  /// @param[in]  new systick period
  /// @return     actually assigned systick period
  ///
	ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, systickChange, cr_EsVariant);

  /// Restore systick period, previously changed by systickChange call
  /// @param[in]  systick period to be cancelled
  ///
	ES_DECL_REFLECTED_CLASS_METHOD1(void, systickRestore, cr_EsVariant);

ES_DECL_REFLECTED_SERVICES_END

}
//---------------------------------------------------------------------------

/// Additional Core types definitions
///
#if (ES_OS_LINUX == ES_OS) || (ES_OS_LINUX == ES_OS)
/// GUID data compatibility declaration for non-Windows OSes
struct EsGUID
{
  uint32_t Data1;
  uint16_t Data2;
  uint16_t Data3;
  uint8_t Data4[ 8 ];

  static const EsGUID& null() ES_NOTHROW;
};
typedef EsGUID GUID;

# ifndef GUID_NULL
#   define GUID_NULL EsGUID::null()
# endif

ES_COMPILE_TIME_ASSERT(sizeof(EsGUID) == 16, _EsGUID_size_is_equal_to_16_bytes_);

#endif
//---------------------------------------------------------------------------

/// Core utility functions and templates
///
namespace EsUtilities
{
/// Get error code from OS.
///
/// @return	.
///
ESCORE_FUNC( ulong, osErrorCodeGet() );

/// Get error string from OS error code.
///
/// @param	code	The code.
///
/// @return	.
///
ESCORE_FUNC( EsString, osErrorStringGet(ulong code) );

/// Return assoc container of environment variables.
///
/// @return	.
///
ESCORE_FUNC( EsStringIndexedMap, environmentVarsListGet() );

/// Global logging wrappers.
///

/// Application Logger event catergory
#define ES_EVTC_APPLOG esT("applog")

/// @param	msg	The message.
/// @param	val	(optional) the value.
///
ESCORE_FUNC( void, logInfo(const EsString& msg, const EsVariant& val = EsVariant::null()) );

/// Logs a debug.
///
/// @param	msg	The message.
/// @param	val	(optional) the value.
///
ESCORE_FUNC( void, logDebug(const EsString& msg, const EsVariant& val = EsVariant::null()) );

/// Logs a warning.
///
/// @param	msg	The message.
/// @param	val	(optional) the value.
///
ESCORE_FUNC( void, logWarning(const EsString& msg, const EsVariant& val = EsVariant::null()) );

/// Logs an error.
///
/// @param	msg	The message.
/// @param	val	(optional) the value.
///
ESCORE_FUNC( void, logError(const EsString& msg, const EsVariant& val = EsVariant::null()) );

/// Logs custom message
///
///	@param	id Custom log level id (NB! internally, Custom got added to it)
/// @param	msg	The message.
/// @param	val	(optional) the value.
///
ESCORE_FUNC( void, logCustom(ulong id, const EsString& msg, const EsVariant& val = EsVariant::null()) );

/// Return readable asl string.
///
/// @param	asl	The asl.
///
/// @return	.
///
ESCORE_FUNC( EsString, aslStringGet(ulong asl) );

/// Get languages installed under the specified directory. only use non-empty language folders
/// containing mo files with specified base name.
///
/// @param	languagesRoot	The languages root.
/// @param	baseMoName	 	Name of the base mo.
///
/// @return	.
///
ESCORE_FUNC( EsString::Array, installedLanguagesGet(const EsString& languagesRoot, const EsString& baseMoName) );

/// Convert GUID to string.
///
/// @param	guid	    Unique identifier.
/// @param	canonical	If true, use canonical GUID string formatting (with dashes as separators,
///                   but not enclosed in curly brackets)
///                   If false - just convert GUID to continuous HEX string
///
/// @return	String representation of GUID.
///
ESCORE_FUNC( EsString, GUIDtoStr(const GUID& guid, bool canonical = false) );

/// Get GUID from its string representation.
/// Throw error if str is not valid representation of GUID.
/// An empty string is interpreted as GUID_NULL.
///
/// @param	str	String representation of GUID
///
/// @return	GUID.
///
ESCORE_FUNC( GUID, GUIDfromStr(const EsString& str) );

#ifdef ES_USE_CRYPTO_LEGACY

/// Compress and encrypt source block b using supplied key
///
/// @param	b	 	The const EsBinBuffer&amp; to process.
/// @param	key	The key.
///
/// @return	Compressed and encoded data block.
///
ESCORE_FUNC( EsBinBuffer, bToE(const EsBinBuffer& b, const EsString& key) );

/// Decrypt and decompress source block e using supplied key
///
/// @param	e	 	The const EsBinBuffer&amp; to process.
/// @param	key	The key.
///
/// @return	Decoded and decompressed data block.
///
ESCORE_FUNC( EsBinBuffer, eToB(const EsBinBuffer& e, const EsString& key) );

/// String codec based on bToE|eToB.
///
/// @param	s	 	The const EsString&amp; to process.
/// @param	key	The key.
///
/// @return	The given data converted to an EsString.
///
ESCORE_FUNC( EsString, sToE(const EsString& s, const EsString& key) );

/// Converts this object to the s.
///
/// @param	e	 	The const EsString&amp; to process.
/// @param	key	The key.
///
/// @return	The given data converted to an EsString.
///
ESCORE_FUNC( EsString, eToS(const EsString& e, const EsString& key) );

#endif // ES_USE_CRYPTO_LEGACY

/// Byte inversion
///

/// inverse byte order in 2-byte integer.
///
/// @param	in	The input 2-byte value.
///
/// @return	Inversed 2-byte value.
///
inline esU16 inverse2(esU16 in) ES_NOTHROW
{
	ulong b0 = in & 0xFF;
	ulong b1 = (in >> 8);

	return static_cast<esU16>(b0 << 8) + static_cast<esU16>(b1);
}

/// Inverse byte order in 4-byte integer.
///
/// @param	in	The input 4-byte value.
///
/// @return	Inversed 4-byte value.
///
inline esU32 inverse4(esU32 in) ES_NOTHROW
{
	ulong w0 = inverse2(in & 0xFFFF);
	ulong w1 = inverse2(in >> 16);

	return (w0 << 16) + w1;
}

/// Inverse byte order in 8-byte integer.
///
/// @param	in	The input 8-byte value.
///
/// @return	Inversed 8-byte value.
///
inline esU64 inverse8(esU64 in) ES_NOTHROW
{
	ulong l0 = inverse4(in & IMMEDIATE_UINT64(0xFFFFFFFF));
	ulong l1 = inverse4(in >> 32);

	return (static_cast<esU64>(l0) << 32) + static_cast<esU64>(l1);
}
//---------------------------------------------------------------------------

/// Break nn bit integer into components
template <typename inT>
class iinterpret
{
public:
  enum  {
    inByteSize = sizeof(inT),
    inWordSize = inByteSize/2,
    inDwordSize = inWordSize/2
  };

public:
  explicit iinterpret(const inT& in)
  {
    m_in = in;

    static_assert(
      std::is_integral<inT>::value,
      "EsUtilities::iinterpret specification type must be integral type"
    );
  }

  inline esU8& asU8(size_t b)
  {
    ES_ASSERT(b < inByteSize);
    return m_b[b];
  }
  inline const esU8& asU8(size_t b) const
  {
    ES_ASSERT(b < inByteSize);
    return m_b[b];
  }

  inline esU16& asU16(size_t w)
  {
    ES_ASSERT(w < inWordSize);
    return m_w[w];
  }
  inline const esU16& asU16(size_t w) const
  {
    ES_ASSERT(w < inWordSize);
    return m_w[w];
  }

  inline esU32& asU32(size_t u)
  {
    ES_ASSERT(u < inWordSize);
    return m_u[u];
  }
  inline const esU32& asU32(size_t u) const
  {
    ES_ASSERT(u < inWordSize);
    return m_u[u];
  }

protected:
  union {
    inT m_in;
    esU8 m_b[inByteSize];
    esU16 m_w[inWordSize];
    esU32 m_u[inDwordSize];
  };

private:
  iinterpret() ES_REMOVEDECL;
  iinterpret(const iinterpret&) ES_REMOVEDECL;
  iinterpret& operator=(const iinterpret&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

/// Rounding double to int type.
///
/// @tparam	T	Type of the typename t.
/// @param	d	The double to process.
///
/// @return	.
///
template <typename T>
inline T roundTo(double d) ES_NOTHROW
{
	if( d >= 0.0 )
		return static_cast<T>(d + 0.5);
	else
		return static_cast<T>(d - 0.5);
}
//---------------------------------------------------------------------------

/// Rounding double to nearest integral double, greater in terms of modulus.
///
/// @param	val	The value to process.
///
/// @return	nearest greater rounded double, in terms of modulus.
///
inline double round0(double val) ES_NOTHROW
{
	if( val >= 0.0 )
		return floor(val + 0.5);
	else
		return ceil(val - 0.5);
}
//---------------------------------------------------------------------------

/// Floating point numbers equality comparison
///
/// Double internals-aware equality comparison
ESCORE_FUNC( bool, areEqualFloats(double _1, double _2, size_t maxUlps = 1) ES_NOTHROW );

/// Float internals-aware equality comparison
ESCORE_FUNC( bool, areEqualFloats(float _1, float _2, size_t maxUlps = 1) ES_NOTHROW );
//---------------------------------------------------------------------------

/// Try to parse string argument as a vestion string 
/// (AKA major[sep]minor<[sep]build><[sep]revision>),
/// and return processed parts as variant collection
/// If string is not recognizable as version string, return an empty variant
/// 
ESCORE_FUNC( EsVariant, versionStrParse(const EsString& str) );
//---------------------------------------------------------------------------

/// Version string (AKA major[sep]minor<[sep]build><[sep]revision>) comparison
/// As of convention, return EsString::cmpLess, if _1 < _2 
/// EsString::cmpEqual, if _1 == _2
/// EsString::greater otherwise
/// If either of string may not be recognized as a version string, throw an exception
/// NB! By convention, non-existing version string members are considered 0, so, 
/// if, say str1 is 1.2 and str2 is 1.2.0.0, they are considered equal.
/// 
ESCORE_FUNC( long, versionStrCompare(const EsString& _1, const EsString& _2) );
//---------------------------------------------------------------------------

/// Platform-specific stuff
///
ESCORE_FUNC( void, urlOpen(const EsString& url) );

#if defined( ES_POSIX_COMPAT )

/// POSIX stuff
///

#if ES_OS != ES_OS_ANDROID

/// Sysctl call wrappers
ESCORE_FUNC( int, sysctlIntGet(int subjId, int dataId) );
ESCORE_FUNC( EsString, sysctlStrGet(int subjId, int dataId) );

#endif

/// MAC OS | iOS stuff
///
# if ES_OS == ES_OS_MAC || \
     ES_OS == ES_OS_IOS

/// Try to find registry key in svc object. Return empty variant if nothing was found
/// otherwise, return either integer or string value, depending on key type
///
ESCORE_FUNC( EsVariant, macosSvcRegValSearch(io_service_t svc, const io_name_t plane, CFStringRef key, IOOptionBits searchOptions = kIORegistryIterateRecursively ) );


/// Try to read registry key from svc object. Return empty variant if nothing read
/// otherwise, return either integer or string value, depending on key type
///
ESCORE_FUNC( EsVariant, macosSvcRegValGet(io_service_t svc, CFStringRef key) );

/// Try to locate and retrieve VID, PID, and serial information for the USB
/// device, which is identified by /dev/xxx path string
/// If found, returned is collection of [VID,PID,Serial], null variant otherwise
///
ESCORE_FUNC( EsVariant, macosUsbInfoGetByParam(const io_name_t paramPlane, CFStringRef paramKey, const EsVariant& paramVal) );

# elif ES_OS == ES_OS_ANDROID

ESCORE_FUNC(JNIEnv*, jniEnvGet() );
ESCORE_FUNC(JavaVM*, jvmGet() );

# endif

#endif

//---------------------------------------------------------------------------
/// OS systick timer period local changer, for those OSes, where it's supported, RAII
///
class ESCORE_CLASS SystickPeriodChanger
{
public:
  SystickPeriodChanger(ulong ms);
  ~SystickPeriodChanger();

protected:
  static ulong systickSet(ulong period);
  static void systickRestore(ulong period);

protected:
  ulong m_ms;

private:
  SystickPeriodChanger() ES_REMOVEDECL;
  SystickPeriodChanger(const SystickPeriodChanger&) ES_REMOVEDECL;
  SystickPeriodChanger& operator=(const SystickPeriodChanger&) ES_REMOVEDECL;

  /// Allow access from reflected service
  ///
  friend EsVariant EsReflection::EsUtilities::systickChange(cr_EsVariant);
  friend void EsReflection::EsUtilities::systickRestore(cr_EsVariant);
};

#ifdef ES_MODERN_CPP
/// Scoped action helper. Constructor expects two functors,
/// the first one is executed upon scoped object construction,
/// the second one is executed in object destructor.
/// If constructor functor throws, soped action is considered invalid, and no destructor functor is executed.
///
class EsScopedAction
{
public:
  typedef std::function<void()> ActionT;

public:
  EsScopedAction(ActionT onEnter, ActionT onExit) :
  m_invalid(false)
  {
    m_onEnter = onEnter;
    m_onExit = onExit;

    try
    {
      if(m_onEnter)
        m_onEnter();
    }
    catch(...)
    {
      m_invalid = true;
    }
  }

  ~EsScopedAction()
  {
    if(m_invalid)
      return;

    if(!m_onExit)
      return;

    try
    {
      m_onExit();
    }
    catch(...)
    {
      // Do not allow to anything to throw from destructor
    }
  }

private:
  ActionT m_onEnter;
  ActionT m_onExit;
  bool m_invalid;

private:
  EsScopedAction() ES_REMOVEDECL;
  EsScopedAction(const EsScopedAction&) ES_REMOVEDECL;
  EsScopedAction& operator=(const EsScopedAction&) ES_REMOVEDECL;
};
#endif //< ES_MODERN_CPP

} // namespace EsUtilities

/// Inplace typecast
template <typename ToT, typename FromT>
inline ToT as_(FromT t) { return static_cast<ToT>(t); }

#endif // _es_utilities_h_
