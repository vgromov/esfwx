#ifndef _es_math_h_
#define _es_math_h_

namespace EsMath {

// floating point numbers equality comparison
EKOSF_MATH_FUNC bool areEqualFloats(double _1, double _2, INT64 maxUlps = 1);
EKOSF_MATH_FUNC bool areEqualFloats(float _1, float _2, INT32 maxUlps = 1);

// Round the floating number, using the number of the floating point digits
// after comma. Note that the function can yield imprecise result.
// If numDecimalPlaces is zero, the behavior is equivalent to Round0.
//
// Here is the usage:
//       double x = EsMath::round(0.555, 1);    // 0.6
//       double y = EsMath::round(1.234567, 2); // 1.23
//
// PRECONDITION: None.
//
// POSTCONDITION: The floating point value rounded to the given type is returned.
//
EKOSF_MATH_FUNC double round(double val, int numDecimalPlaces = 0);

}

#endif // _es_math_h_