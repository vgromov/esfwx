#include "stdafx.h" 
#pragma hdrstop

#include "EsMathInternals.h"
#include "EsMathFitConstraint.h"

EsMathFitConstraint::EsMathFitConstraint(double x /*= 0.*/, double val /*= 0.*/, 
	EsMathFitConstraintKind kind /*= EsMathFitConstraint_VALUE*/) :
m_x(x),
m_val(val),
m_kind(kind)
{}