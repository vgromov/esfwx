#ifndef _es_math_internals_h_
#define _es_math_internals_h_

EsMathFitInfo alglibInfoToFitInfo(alglib::ae_int_t info);
void esMathFloatArrayToAlglib1dArray(const EsMathFloatArray& in, alglib::real_1d_array& out);

#endif // _es_math_internals_h_