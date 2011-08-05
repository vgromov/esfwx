#include "stdafx.h"
#pragma hdrstop

#include "EsMathInternals.h"

EsMathFitInfo alglibInfoToFitInfo(alglib::ae_int_t info)
{
	if( 0 < info )
		return EsMathFitInfo_SUCCESS;
	else if( -3 == info )
		return EsMathFitInfo_INCONSISTENT_CONSTRAINTS;
	else if( -4 == info )
		return EsMathFitInfo_INCONVERGENCE_OCCURED;
	
	return EsMathFitInfo_FAIL;
}

void esMathFloatArrayToAlglib1dArray(const EsMathFloatArray& in, alglib::real_1d_array& out)
{
	out.setlength(in.size());
	for(size_t idx = 0; idx < in.size(); ++idx)
		out[idx] = in[idx];
}