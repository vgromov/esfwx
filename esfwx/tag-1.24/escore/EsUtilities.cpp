#include "escorepch.h"
#pragma hdrstop

#include "EsUtilities.h"
#include <esfwxe/datetime.h>
#include <algorithm>
#include <escore/EsCryptoDesLegacy.h>

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#if ES_OS == ES_OS_WINDOWS
# include "EsUtilities.win.cxx"
#elif defined(ES_POSIX_COMPAT)
# if ES_OS == ES_OS_MAC || \
     ES_OS == ES_OS_IOS
#   include "EsUtilities.mac.cxx"
# elif ES_OS == ES_OS_ANDROID
#   include "EsUtilities.android.cxx"
# endif
# include "EsUtilities.posix.cxx"
#endif
//---------------------------------------------------------------------------

static inline void logEventPost(const EsString& msg, ulong level, const EsVariant& val) ES_NOTHROW
{
	EsVariant::Array payload;
  payload.reserve(2);

	payload.push_back( msg );
	payload.push_back( val );

	EsEventDispatcher::eventPost(ES_EVTC_APPLOG, level, payload);
}
//---------------------------------------------------------------------------

void EsUtilities::logInfo(const EsString& msg, const EsVariant& val /*= EsVariant::s_null*/)
{
	logEventPost(msg, static_cast<ulong>(EsAppLogLevel::Info), val);
}
//---------------------------------------------------------------------------

void EsUtilities::logDebug(const EsString& msg, const EsVariant& val /*= EsVariant::s_null*/)
{
  ES_DEBUG_TRACE(esT("LogDebug: %s"), msg);
	logEventPost(msg, static_cast<ulong>(EsAppLogLevel::Debug), val);
}
//---------------------------------------------------------------------------

void EsUtilities::logWarning(const EsString& msg, const EsVariant& val /*= EsVariant::s_null*/)
{
  ES_DEBUG_TRACE(esT("LogWarning: %s"), msg);
	logEventPost(msg, static_cast<ulong>(EsAppLogLevel::Warning), val);
}
//---------------------------------------------------------------------------

void EsUtilities::logError(const EsString& msg, const EsVariant& val /*= EsVariant::s_null*/)
{
  ES_DEBUG_TRACE(esT("LogError: %s"), msg);
	logEventPost(msg, static_cast<ulong>(EsAppLogLevel::Error), val);
}
//---------------------------------------------------------------------------

void EsUtilities::logCustom(ulong id, const EsString& msg, const EsVariant& val /*= EsVariant::s_null*/)
{
  ES_DEBUG_TRACE(esT("LogCustom: %s"), msg);
	logEventPost(msg, static_cast<ulong>(EsAppLogLevel::Custom)+id, val);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// return readable asl string
EsString EsUtilities::aslStringGet(ulong asl)
{
	EsString result;

	switch( static_cast<EsAppSecurityLevel>(asl) )
	{
	case EsAppSecurityLevel::aslNone: // no user is logged on
		result = _("None");
		break;
	case EsAppSecurityLevel::aslGuest:
		result = _("Guest");
		break;
	case EsAppSecurityLevel::aslUser:
		result = _("User");
		break;
	case EsAppSecurityLevel::aslSuperuser:
		result = _("Superuser");
		break;
	case EsAppSecurityLevel::aslAdministrator:
		result = _("Administrator");
		break;
	case EsAppSecurityLevel::aslDeveloper:
		result = _("Developer");
		break;
	default:
		result = _("Invalid role value");
		break;
	}

	return result;
}
//---------------------------------------------------------------------------

#if (ES_OS_LINUX == ES_OS) || (ES_OS_LINUX == ES_OS)
const EsGUID& EsGUID::null() ES_NOTHROW
{
  static const EsGUID sc_null = {0};

  return sc_null;
}
#endif

// convert GUID to string
EsString EsUtilities::GUIDtoStr(const GUID& guid, bool canonical /*= false*/)
{
  if( canonical )
  {
    return EsString::format(esT("%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX"),
      guid.Data1, guid.Data2, guid.Data3,
      guid.Data4[0], guid.Data4[1],
      guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
  }
  else
	{
   	EsString result;
    EsString::binToHex(reinterpret_cast<const esU8*>(&guid), sizeof(GUID), result);

	  return result;
  }
}
//---------------------------------------------------------------------------

// convert string to GUID
GUID EsUtilities::GUIDfromStr(const EsString& str)
{
  GUID result = GUID_NULL;

  if( !str.empty() )
  {
    EsRegEx rex(
      esT("(\\{?([a-fA-F0-9]{8})\\-([a-fA-F0-9]{4})\\-([a-fA-F0-9]{4})\\-([a-fA-F0-9]{4})\\-([a-fA-F0-9]{12})\\}?)|")
      esT("([a-fA-F0-9]{32})")
    );

    rex.set_text(str);
    if( rex.get_matches() )
    {
      const EsString& canonical = rex.matchGet(1);
      if( canonical.empty() )
      {
        EsString::hexToBin(
          rex.matchGet(7).c_str(),
          32,
          reinterpret_cast<esU8*>(&result),
          sizeof(result)
        );
      }
      else
      {
        EsString::hexToBin(
          rex.matchGet(2).c_str(),
          8,
          reinterpret_cast<esU8*>(&result.Data1),
          4
        );
        result.Data1 = inverse4(result.Data1);

        EsString::hexToBin(
          rex.matchGet(3).c_str(),
          4,
          reinterpret_cast<esU8*>(&result.Data2),
          2
        );
        result.Data2 = inverse2(result.Data2);

        EsString::hexToBin(
          rex.matchGet(4).c_str(),
          4,
          reinterpret_cast<esU8*>(&result.Data3),
          2
        );
        result.Data3 = inverse2(result.Data3);

        EsString::hexToBin(
          rex.matchGet(5).c_str(),
          4,
          result.Data4,
          2
        );

        EsString::hexToBin(
          rex.matchGet(6).c_str(),
          12,
          result.Data4+2,
          6
        );
      }
    }
    else
      EsException::Throw(
        _("'%s' is not a valid GUID string"),
        str
      );
  }

  return result;
}

#ifdef ES_USE_CRYPTO_LEGACY

// firmware processing (compression+encryption)
EsBinBuffer EsUtilities::bToE(const EsBinBuffer& b, const EsString& key)
{
	EsCryptoDesLegacy des( key );

	EsBinBuffer dest;

#ifdef ES_USE_ZLIB
  EsBinBuffer src;
	EsZipCompressor zip;
	src = zip.compress(b);
	des.process(dest, src);
#else
	des.process(dest, b);
#endif

	return dest;
}

EsBinBuffer EsUtilities::eToB(const EsBinBuffer& e, const EsString& key)
{
	EsBinBuffer src;
	EsCryptoDesLegacy des( key );

	des.process(src, e, false);

#ifdef ES_USE_ZLIB
	EsZipDecompressor zip;
	return zip.decompress(src);
#else
  return src;
#endif
}

EsString EsUtilities::sToE(const EsString& s, const EsString& key)
{
	if( !s.empty() )
	{
		EsBinBuffer::const_pointer sBegin = (EsBinBuffer::const_pointer)s.data();
		EsBinBuffer::const_pointer sEnd = sBegin+s.size()*EsString::value_size;
		EsBinBuffer in(sBegin, sEnd);
		const EsBinBuffer& out = EsUtilities::bToE(in, key);
		return EsString::binToHex(out);
	}

	return EsString::null();
}

EsString EsUtilities::eToS(const EsString& e, const EsString& key)
{
	if( !e.empty() )
	{
		const EsBinBuffer& in = EsString::hexToBin(e);
		const EsBinBuffer& out = EsUtilities::eToB(in, key);
		ES_ASSERT( 0 == out.size() % EsString::value_size );
		EsString::const_pointer beg = reinterpret_cast<EsString::const_pointer>(&out[0]);
		EsString::const_pointer end = beg+(out.size() / EsString::value_size);
		return EsString(beg, end);
	}

	return EsString::null();
}

#endif // ES_USE_CRYPTO_LEGACY

//---------------------------------------------------------------------------
// float-specific checks and comparison stuff
template <size_t size>
class TypeWithSize {
 public:
  // This prevents the user from using TypeWithSize<N> with incorrect
  // values of N.
  typedef void UInt;
};

// The specialization for size 4.
template <>
class TypeWithSize<4> {
 public:
  // unsigned int has size 4 in both gcc and MSVC.
  //
  // As base/basictypes.h doesn't compile on Windows, we cannot use
  // uint32, uint64, and etc here.
  typedef int Int;
  typedef unsigned int UInt;
};

// The specialization for size 8.
template <>
class TypeWithSize<8>
{
 public:
  typedef long long Int;  // NOLINT
  typedef unsigned long long UInt;  // NOLINT
};

// RawType: the raw floating-point type (either float or double)
// kMaxUlps: How many ULP's (Units in the Last Place) we want to tolerate when
// comparing two numbers.  The larger the value, the more error we
// allow.  A 0 value means that two numbers must be exactly the same
// to be considered equal.
//
// The maximum error of a single floating-point operation is 0.5
// units in the last place.  On Intel CPU's, all floating-point
// calculations are done with 80-bit precision, while double has 64
// bits.  Therefore, 4 should be enough for ordinary use.
//
// See the following article for more details on ULP:
// http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm.
template <typename RawType, size_t kMaxUlps = 4>
class FloatingPoint
{
 public:
  // Defines the unsigned integer type that has the same size as the
  // floating point number.
  typedef typename TypeWithSize<sizeof(RawType)>::UInt Bits;

  // Constants.

  // # of bits in a number.
  static const size_t kBitCount = 8*sizeof(RawType);

  // # of fraction bits in a number.
  static const size_t kFractionBitCount =
    std::numeric_limits<RawType>::digits - 1;

  // # of exponent bits in a number.
  static const size_t kExponentBitCount = kBitCount - 1 - kFractionBitCount;

  // The mask for the sign bit.
  static const Bits kSignBitMask = static_cast<Bits>(1) << (kBitCount - 1);

  // The mask for the fraction bits.
  static const Bits kFractionBitMask =
    ~static_cast<Bits>(0) >> (kExponentBitCount + 1);

  // The mask for the exponent bits.
  static const Bits kExponentBitMask = ~(kSignBitMask | kFractionBitMask);

	// Constructs a FloatingPoint from a raw floating-point number.
  //
  // On an Intel CPU, passing a non-normalized NAN (Not a Number)
  // around may change its bits, although the new value is guaranteed
  // to be also a NAN.  Therefore, don't expect this constructor to
  // preserve the bits in x when x is a NAN.
  explicit FloatingPoint(const RawType& x) { u_.value_ = x; }

  // Static methods

  // Reinterprets a bit pattern as a floating-point number.
  //
  // This function is needed to test the AlmostEquals() method.
  static inline RawType ReinterpretBits(const Bits bits) ES_NOTHROW
  {
    FloatingPoint fp(0);
    fp.u_.bits_ = bits;
    return fp.u_.value_;
  }

  // Returns the floating-point number that represent positive infinity.
  static inline RawType Infinity() ES_NOTHROW
  {
    return ReinterpretBits(kExponentBitMask);
  }

  // Non-static methods

  // Returns the bits that represents this number.
  inline const Bits &bits() const ES_NOTHROW { return u_.bits_; }

  // Returns the exponent bits of this number.
  inline Bits exponent_bits() const ES_NOTHROW { return kExponentBitMask & u_.bits_; }

  // Returns the fraction bits of this number.
  inline Bits fraction_bits() const ES_NOTHROW { return kFractionBitMask & u_.bits_; }

  // Returns the sign bit of this number.
  inline Bits sign_bit() const ES_NOTHROW { return kSignBitMask & u_.bits_; }

  // Returns true iff this is NAN (not a number).
  inline bool is_nan() const ES_NOTHROW
  {
    // It's a NAN if the exponent bits are all ones and the fraction
    // bits are not entirely zeros.
    return (exponent_bits() == kExponentBitMask) && (fraction_bits() != 0);
  }

  // Returns true iff this number is at most kMaxUlps ULP's away from
  // rhs.  In particular, this function:
  //
  //   - returns false if either number is (or both are) NAN.
  //   - treats really large numbers as almost equal to infinity.
  //   - thinks +0.0 and -0.0 are 0 DLP's apart.
  inline bool AlmostEquals(const FloatingPoint& rhs, size_t maxUlps = 1) const ES_NOTHROW
	{
    // The IEEE standard says that any comparison operation involving
    // a NAN must return false.
    if(is_nan() || rhs.is_nan())
			return false;

    return DistanceBetweenSignAndMagnitudeNumbers(u_.bits_, rhs.u_.bits_)
			<= std::max(static_cast<size_t>(1), std::min(static_cast<size_t>(kMaxUlps), maxUlps));
  }

 private:
  // The data type used to store the actual floating-point number.
  union FloatingPointUnion
	{
    RawType value_;  // The raw floating-point number.
    Bits bits_;      // The bits that represent the number.
  };

  // Converts an integer from the sign-and-magnitude representation to
  // the biased representation.  More precisely, let N be 2 to the
  // power of (kBitCount - 1), an integer x is represented by the
  // unsigned number x + N.
  //
  // For instance,
  //
  //   -N + 1 (the most negative number representable using
  //          sign-and-magnitude) is represented by 1;
  //   0      is represented by N; and
  //   N - 1  (the biggest number representable using
  //          sign-and-magnitude) is represented by 2N - 1.
  //
  // Read http://en.wikipedia.org/wiki/Signed_number_representations
  // for more details on signed number representations.
  static inline Bits SignAndMagnitudeToBiased(const Bits &sam) ES_NOTHROW
	{
    if(kSignBitMask & sam)
		{
      // sam represents a negative number.
      return ~sam + 1;
    }
		else
		{
      // sam represents a positive number.
      return kSignBitMask | sam;
    }
  }

  // Given two numbers in the sign-and-magnitude representation,
  // returns the distance between them as an unsigned number.
  static inline Bits DistanceBetweenSignAndMagnitudeNumbers(const Bits &sam1, const Bits &sam2) ES_NOTHROW
	{
    const Bits biased1 = SignAndMagnitudeToBiased(sam1);
    const Bits biased2 = SignAndMagnitudeToBiased(sam2);
    return (biased1 >= biased2) ? (biased1 - biased2) : (biased2 - biased1);
  }

  FloatingPointUnion u_;
};

typedef FloatingPoint<float> EsFlt;
typedef FloatingPoint<double> EsDbl;

bool EsUtilities::areEqualFloats(double _1, double _2, size_t maxUlps) ES_NOTHROW
{
	return EsDbl(_1).AlmostEquals(EsDbl(_2), maxUlps);
}

bool EsUtilities::areEqualFloats(float _1, float _2, size_t maxUlps) ES_NOTHROW
{
	return EsFlt(_1).AlmostEquals(EsFlt(_2), maxUlps);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsUtilities::SystickPeriodChanger::SystickPeriodChanger(ulong ms) :
m_ms(0)
{
  m_ms = systickSet(
    ms
  );
}
//---------------------------------------------------------------------------

EsUtilities::SystickPeriodChanger::~SystickPeriodChanger()
{
  systickRestore( m_ms ); //< Restore previous period
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

namespace EsReflection
{
ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsUtilities, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsUtilities, systickChange, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsUtilities, systickRestore, void_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_REFLECTED_SERVICES_INFO_END
}
//---------------------------------------------------------------------------

EsVariant EsReflection::EsUtilities::systickChange(cr_EsVariant period)
{
  return ::EsUtilities::SystickPeriodChanger::systickSet(
    period.asULong()
  );
}
//---------------------------------------------------------------------------

void EsReflection::EsUtilities::systickRestore(cr_EsVariant period)
{
  ::EsUtilities::SystickPeriodChanger::systickRestore(
    period.asULong()
  );
}
//---------------------------------------------------------------------------

