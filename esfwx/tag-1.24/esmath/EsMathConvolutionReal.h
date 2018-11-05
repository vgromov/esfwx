#ifndef _es_math_transformations_h_
#define _es_math_transformations_h_

// Convolution flags
//
enum class EsMathConvolutionFlag : ulong
{
  FilterIsSymmetrical = 0x0001,
  FilterIsNormalized	= 0x0002,
  UseFixedLsubst			= 0x0004,
  UseFixedRsubst      = 0x0008,
  SignalIsStable 			= 0x0010,

  // Special masks
  FilterMask					= static_cast<ulong>(EsMathConvolutionFlag::FilterIsSymmetrical)|
												static_cast<ulong>(EsMathConvolutionFlag::FilterIsNormalized)
};

// Convolution implementation
//
class ESMATH_CLASS ES_INTF_IMPL1(EsMathConvolutionReal, EsReflectedClassIntf)

public:
	ES_DECL_REFLECTED_CLASS_BASE(EsMathConvolutionReal)
	ES_DECL_ATTR_HANDLING_STD

  /// Non-reflected services
  ///

  EsMathConvolutionReal();
	EsMathConvolutionReal(const EsMathArrayReal& filter, ulong flags);
	EsMathConvolutionReal(size_t filterSize, EsMathArrayReal::const_pointer filter, ulong flags);

	/// validity check
	bool isOk() const { return !m_filter.get_empty(); }

	/// filter access
	///
	void filterSet(const EsMathArrayReal& filter, ulong flags = 0);
	void filterSet(size_t filterSize, EsMathArrayReal::const_pointer filter, ulong flags = 0);
	const EsMathArrayReal& filterGet() const { return m_filter; }

	/// Linear 1-dimentional convolution
	/// if signal is stable, we may use signal itself to define data at T < 0 domains
	/// if filter is symmetrical, we may use half of its length as offset from the
	/// signal sequence, to the first filtered point value
	///
	void process(const EsMathArrayReal& sig, EsMathArrayReal& out) const;

  /// Reflected services
  ///

  /// Default constructor. Return an empty object
  /// @return empty (no filter set) object instance
  ///
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

  /// Return object, initialized with specified filter
  /// @param p0 [in] either array object, or collection, containing filter data
  /// @return object instance, with sepcified filter data
  ///
  ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, cr_EsVariant);

  /// Return object, initialized with specified filter
  /// @param p0 [in] either array object, or collection, containing filter data
  /// @param p1 [in] filter, and|or convolution specific flags
  /// @return object instance, with sepcified filter data, and preset flags
  ///
  ES_DECL_REFLECTED_CLASS_CTOR2(EsBaseIntfPtr, cr_EsVariant, ulong);

	/// Linear 1-dimentional convolution.
  /// @param p0 [in]  EsVariant const reference, containing either real array object instance, or values collection
  /// @return         EsVariant with real array object instance, containing convolution result
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, process, cr_EsVariant);

  /// Properties
  ///
  ES_DECL_PROPERTY_RO(ok, bool)
  ES_DECL_PROPERTY_STD(flags, ulong)
  ES_DECL_PROPERTY_STD(substL, double)
  ES_DECL_PROPERTY_STD(substR, double)
  ES_DECL_PROPERTY(filter, EsVariant)

protected:
	EsMathArrayReal m_filter;
};

// Reflected convolution flags
//
namespace EsReflection
{
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESMATH_CLASS, EsMathConvolutionFlag)
  ES_DECL_ENUMERATION_ITEM(FilterIsSymmetrical, static_cast<ulong>(::EsMathConvolutionFlag::FilterIsSymmetrical))
  ES_DECL_ENUMERATION_ITEM(FilterIsNormalized,  static_cast<ulong>(::EsMathConvolutionFlag::FilterIsNormalized))
  ES_DECL_ENUMERATION_ITEM(UseFixedLsubst,      static_cast<ulong>(::EsMathConvolutionFlag::UseFixedLsubst))
  ES_DECL_ENUMERATION_ITEM(UseFixedRsubst,      static_cast<ulong>(::EsMathConvolutionFlag::UseFixedRsubst))
  ES_DECL_ENUMERATION_ITEM(SignalIsStable,      static_cast<ulong>(::EsMathConvolutionFlag::SignalIsStable))
  ES_DECL_ENUMERATION_ITEM(FilterMask,          static_cast<ulong>(::EsMathConvolutionFlag::FilterMask))
ES_DECL_ENUMERATION_END
}

#endif // _es_math_transformations_h_
