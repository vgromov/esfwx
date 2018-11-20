#include "esmathpch.h"
#pragma hdrstop

#include "EsMathUtils.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// Math constants
//
const double EsMath::c_pi = 3.141592653589793;
const double EsMath::c_2pi = 6.283185307179586;
const double EsMath::c_pi_2 = 1.570796326794897;
const double EsMath::c_pi_180 = 0.0174532925199433;
const double EsMath::c_e = 2.71828182845904523536029;
//---------------------------------------------------------------------------

ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsMath, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, pi, double_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, _2pi, double_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, pi_2, double_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, pi_180, double_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, e, double_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, isInf, bool_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, isNaN, bool_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, areEqualFloats, bool_ClassCall_double_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, areEqualFloats, bool_ClassCall_double_double_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, pow10, double_ClassCall_long, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, pow2, double_ClassCall_long, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, round, double_ClassCall_double_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, round, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, degToRad, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, radToDeg, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, powNM, double_ClassCall_double_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, exp, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, sqrt, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, modf, EsVariant_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, abs, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, log, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, log2, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, log10, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, logN, double_ClassCall_double_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, sin, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, sinh, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, asin, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, cos, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, cosh, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, acos, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, tan, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, tanh, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, atan, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, atanYX, double_ClassCall_double_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, geoDistanceEstimate, double_ClassCall_double_double_double_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, geoDistanceEstimateFast, double_ClassCall_double_double_double_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, geoBearing, double_ClassCall_double_double_double_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMath, diff, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_REFLECTED_SERVICES_INFO_END
//---------------------------------------------------------------------------

double EsMath::pi()
{
  return c_pi;
}
//---------------------------------------------------------------------------

double EsMath::_2pi()
{
  return c_2pi;
}
//---------------------------------------------------------------------------

double EsMath::pi_2()
{
  return c_pi_2;
}
//---------------------------------------------------------------------------

double EsMath::pi_180()
{
  return c_pi_180;
}
//---------------------------------------------------------------------------

double EsMath::e()
{
  return c_e;
}
//---------------------------------------------------------------------------

bool EsMath::isInf(double val)
{
  return esIsInf(val);
}
//---------------------------------------------------------------------------

bool EsMath::isNaN(double val)
{
  return esIsNan(val);
}
//---------------------------------------------------------------------------

bool EsMath::areEqualFloats(double _1, double _2, ulong maxUlps)
{
  if( !maxUlps )
    maxUlps = 1;
  else if( maxUlps > 16 )
    maxUlps = 16;

  return EsUtilities::areEqualFloats(_1, _2, maxUlps);
}
//---------------------------------------------------------------------------

bool EsMath::areEqualFloats(double _1, double _2)
{
  return EsUtilities::areEqualFloats(_1, _2, 1);
}
//---------------------------------------------------------------------------

// round double value to the specified number of decimal places
//
double EsMath::pow10(long power)
{
	static const double sc_powers10 [] =
	{
		0.0000001,   //  [0] = pow(10.0, -7)
		0.000001,    //  [1] = pow(10.0, -6)
		0.00001,     //  [2] = pow(10.0, -5)
		0.0001,      //  [3] = pow(10.0, -4)
		0.001,       //  [4] = pow(10.0, -3)
		0.01,        //  [5] = pow(10.0, -2)
		0.1,         //  [6] = pow(10.0, -1)
		1.0,         //  [7] = pow(10.0,  0)
		10.0,        //  [8] = pow(10.0,  1)
		100.0,       //  [9] = pow(10.0,  2)
		1000.0,      // [10] = pow(10.0,  3)
		10000.0,     // [11] = pow(10.0,  4)
		100000.0,    // [12] = pow(10.0,  5)
		1000000.0,   // [13] = pow(10.0,  6)
		10000000.0,  // [14] = pow(10.0,  7)
		100000000.0  // [15] = pow(10.0,  8)
	};

	long index = power + 7;
	if ( index >= 0 && index < (int)ES_CONST_ARRAY_COUNT(sc_powers10) )
		return sc_powers10[index];

	return esPow(10.0, power); // Default, inefficient implementation
}
//---------------------------------------------------------------------------

double EsMath::pow2(long power)
{
	static const double sc_negativePowers2 [] =
	{
		0.00390625,  //  [0] = pow(2.0, -8)
		0.0078125,   //  [0] = pow(2.0, -7)
		0.015625,    //  [1] = pow(2.0, -6)
		0.03125,     //  [2] = pow(2.0, -5)
		0.0625,      //  [3] = pow(2.0, -4)
		0.125,       //  [4] = pow(2.0, -3)
		0.25,        //  [5] = pow(2.0, -2)
		0.5          //  [6] = pow(2.0, -1)
	};

	if( power >= 0 )
	{
		if( power < 32 )
			return static_cast<double>( static_cast<unsigned>(1u << power));
	}
	else
	{
		long powerIndex = power + ES_CONST_ARRAY_COUNT(sc_negativePowers2);
		if( powerIndex >= 0 )
			return sc_negativePowers2[powerIndex];
	}

	return esPow(2.0, power);
}
//---------------------------------------------------------------------------

double EsMath::round(double val, ulong numDecimalPlaces)
{
	if ( numDecimalPlaces == 0 ) // special much faster and more precise case
		return EsUtilities::round0(val);

	double multiplier = EsMath::pow10(numDecimalPlaces);
	return EsUtilities::round0(val * multiplier) / multiplier;
}
//---------------------------------------------------------------------------

double EsMath::round(double val)
{
  return round(val, 0);
}
//---------------------------------------------------------------------------

// convert deg to rad and vice versa
double EsMath::degToRad(double deg)
{
	double n;
	double frac = std::modf(deg, &n);
	deg = static_cast<double>(static_cast<long>(n) % 360) + frac;
	return (deg * c_2pi) / 360.;
}
//---------------------------------------------------------------------------

double EsMath::radToDeg(double rad)
{
	return 360. * rad / c_2pi;
}
//---------------------------------------------------------------------------

double EsMath::powNM(double n, double m)
{
  return esPow(n ,m);
}
//---------------------------------------------------------------------------

double EsMath::exp(double n)
{
  return esExp(n);
}
//---------------------------------------------------------------------------

double EsMath::sqrt(double val)
{
  return esSqrt(val);
}
//---------------------------------------------------------------------------

EsVariant EsMath::modf(double val)
{
  EsVariant::Array result;
  result.reserve(2);

  double i, f;
  f = esModf(val, &i);

  result.push_back(i);
  result.push_back(f);

  return result;
}
//---------------------------------------------------------------------------

double EsMath::abs(double val)
{
  return esAbs(val);
}
//---------------------------------------------------------------------------

double EsMath::log(double val)
{
  return esLog(val);
}
//---------------------------------------------------------------------------

double EsMath::log2(double val)
{
  return esLog2(val);
}
//---------------------------------------------------------------------------

double EsMath::log10(double val)
{
  return esLog10(val);
}
//---------------------------------------------------------------------------

double EsMath::logN(double base, double val)
{
  return esLogN(base, val);
}
//---------------------------------------------------------------------------

double EsMath::sin(double rad)
{
  return esSin(rad);
}
//---------------------------------------------------------------------------

double EsMath::sinh(double val)
{
  return esSinh(val);
}
//---------------------------------------------------------------------------

double EsMath::asin(double val)
{
  return esAsin(val);
}
//---------------------------------------------------------------------------

double EsMath::cos(double rad)
{
  return esCos(rad);
}
//---------------------------------------------------------------------------

double EsMath::cosh(double val)
{
  return esCosh(val);
}
//---------------------------------------------------------------------------

double EsMath::acos(double val)
{
  return esAcos(val);
}
//---------------------------------------------------------------------------

double EsMath::tan(double rad)
{
  return esTan(rad);
}
//---------------------------------------------------------------------------

double EsMath::tanh(double val)
{
  return esTanh(val);
}
//---------------------------------------------------------------------------

double EsMath::atan(double val)
{
  return esAtan(val);
}
//---------------------------------------------------------------------------

double EsMath::atanYX(double y, double x)
{
  return esAtanYX(y, x);
}
//---------------------------------------------------------------------------

double EsMath::geoDistanceEstimate(double lat1, double lon1, double lat2, double lon2)
{
  static const double _2er = 6371000*2; //< Earth radius

  double alat1 = degToRad( lat1 );
  double alat2 = degToRad( lat2 );

  double adlonSin2 = sin(
    degToRad(lon2-lon1) / 2.
  );
  double adlatSin2 = sin(
    degToRad(lat2-lat1) /2.
  );

  double a = adlatSin2 * adlatSin2 +
    cos( alat1 ) * cos( alat2 ) *
    adlonSin2 * adlonSin2;

  return _2er * atan2( sqrt(a), sqrt(1-a) );
}
//---------------------------------------------------------------------------

double EsMath::geoDistanceEstimateFast(double lat1, double lon1, double lat2, double lon2)
{
  static const double er = 6371000;

  double x = degToRad(lon2-lon1) * cos( degToRad(lat1+lat2)/2. );
  double y = degToRad(lat2-lat1);

  return sqrt(x*x + y*y) * er;
}
//---------------------------------------------------------------------------

double EsMath::geoBearing(double lat1, double lon1, double lat2, double lon2)
{
  lat1 = lat1 * c_pi_180; // degToRad(lat1);
  lat2 = lat2 * c_pi_180; // degToRad(lat2);

  double dlon = (lon2-lon1)*c_pi_180; //degToRad(lon2 - lon1);
  double cos_lat2 = cos(lat2);
  double y = sin(dlon) * cos_lat2;
  double x = cos(lat1) * sin(lat2) - sin(lat1) * cos_lat2 * cos(dlon);

  return radToDeg( atan2(y, x) );
}
//---------------------------------------------------------------------------

EsVariant EsMath::diff(const EsVariant& in)
{
  EsMathArrayReal* direct = nullptr;
  EsVariant out = EsMathArrayReal::baseIntfPtrCreate(in, &direct);
  ES_ASSERT(direct);
  ES_ASSERT(direct->countGet() > 1);

  double prev = direct->itemGet(0);
  double val = direct->itemGet(1);

  direct->itemSet(
    0,
    val-prev
  );
  direct->itemSet(
    1,
    val-prev
  );

  prev = val;
  ulong cnt = direct->countGet();
  for(ulong idx = 2; idx < cnt; ++idx)
  {
    val = direct->itemGet(idx);

    direct->itemSet(
      idx,
      val-prev
    );

    prev = val;
  }

  return out;
}
//---------------------------------------------------------------------------

