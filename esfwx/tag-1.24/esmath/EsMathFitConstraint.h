#ifndef _es_math_fit_constraint_h_
#define _es_math_fit_constraint_h_

enum class EsMathFitInfo : ulong
{
  None,
  Success,
  Inconvergence,
  ConstraintInconsistent,
  Fail
};

enum class EsMathFitConstraintKind : ulong
{
  Value,
  FirstDerivative
};

class ESMATH_CLASS EsMathFitConstraint
{
public:
  /// Non-reflectes services
  ///
	EsMathFitConstraint(double x = 0., double constraint = 0.,
		EsMathFitConstraintKind kind = EsMathFitConstraintKind::Value);

  /// X value, for which this constraint is defined
  double xGet() const { return m_x; }

  /// Constraint value
  double constraintGet() const { return m_constraint; }

  /// Constraint value kind
  EsMathFitConstraintKind kindGet() const { return m_kind; }

protected:
	double m_x;
	double m_constraint;
	EsMathFitConstraintKind m_kind;
};
typedef std::vector<EsMathFitConstraint> EsMathFitConstraints;

/// Try to construct constraints collection from EsVaraint
void constraintsFromVariant(EsMathFitConstraints& csx, const EsVariant& src);

namespace EsReflection
{
/// Fitting status reflected enumeration
///
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESMATH_CLASS, EsMathFitInfo)
	ES_DECL_ENUMERATION_ITEM_LABELLED(None,				            static_cast<ulong>(::EsMathFitInfo::None), _i("None"))
	ES_DECL_ENUMERATION_ITEM_LABELLED(Success,		            static_cast<ulong>(::EsMathFitInfo::Success), _i("Fit successful"))
	ES_DECL_ENUMERATION_ITEM_LABELLED(Inconvergence,	        static_cast<ulong>(::EsMathFitInfo::Inconvergence), _i("Fit inconvergence occured"))
	ES_DECL_ENUMERATION_ITEM_LABELLED(ConstraintInconsistent,	static_cast<ulong>(::EsMathFitInfo::ConstraintInconsistent), _i("Constraint is inconsistent"))
	ES_DECL_ENUMERATION_ITEM_LABELLED(Fail,				            static_cast<ulong>(::EsMathFitInfo::Fail), _i("Fitting failed"))
ES_DECL_ENUMERATION_END

/// Fitting constraint types
///
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESMATH_CLASS, EsMathFitConstraintKind)
	ES_DECL_ENUMERATION_ITEM_LABELLED(Value,				          static_cast<ulong>(::EsMathFitConstraintKind::Value), _i("Value"))
	ES_DECL_ENUMERATION_ITEM_LABELLED(FirstDerivative,	      static_cast<ulong>(::EsMathFitConstraintKind::FirstDerivative), _i("First derivative"))
ES_DECL_ENUMERATION_END
}

#endif // _es_math_fit_constraint_h_