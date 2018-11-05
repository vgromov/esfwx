#include "esmathpch.h"
#pragma hdrstop

#include "EsMathInternals.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#include "alglib/alglibinternal.cpp"
#include "alglib/alglibmisc.cpp"
#include "alglib/dataanalysis.cpp"
#include "alglib/diffequations.cpp"
#include "alglib/fasttransforms.cpp"
#include "alglib/integration.cpp"
#include "alglib/interpolation.cpp"
#include "alglib/linalg.cpp"
#include "alglib/optimization.cpp"
#include "alglib/solvers.cpp"
#include "alglib/specialfunctions.cpp"
#include "alglib/statistics.cpp"
#include "alglib/ap.cpp"

//---------------------------------------------------------------------------

EsMathFitInfo alglibInfoToFitInfo(alglib::ae_int_t info)
{
	if( 0 < info )
		return EsMathFitInfo::Success;
	else if( -3 == info )
		return EsMathFitInfo::ConstraintInconsistent;
	else if( -4 == info )
		return EsMathFitInfo::Inconvergence;

	return EsMathFitInfo::Fail;
}
//---------------------------------------------------------------------------

