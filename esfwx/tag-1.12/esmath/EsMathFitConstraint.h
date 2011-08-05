#ifndef _es_math_fit_constraint_h_
#define _es_math_fit_constraint_h_

enum EsMathFitConstraintKind {
	EsMathFitConstraint_VALUE,
	EsMathFitConstraint_FIRST_DERIVATIVE
};

class EKOSF_MATH_CLASS EsMathFitConstraint
{
public:
	EsMathFitConstraint(double x = 0., double val = 0., 
		EsMathFitConstraintKind kind = EsMathFitConstraint_VALUE);

	double xGet() const
	{
		return m_x;
	}
	double valGet() const
	{
		return m_val;
	}
	EsMathFitConstraintKind kindGet() const
	{
		return m_kind;
	}

protected:
	double m_x;
	double m_val;
	EsMathFitConstraintKind m_kind;
};
typedef std::vector<EsMathFitConstraint> EsMathFitConstraints;

#endif // _es_math_fit_constraint_h_