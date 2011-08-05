#include "stdafx.h"
#pragma hdrstop

#include "EsMath.h"

bool EsMath::areEqualFloats(double _1, double _2, INT64 maxUlps)
{
	// Make sure maxUlps is non-negative and small enough that the
	// default NAN won't compare as equal to anything.
	wxASSERT(maxUlps > 0 && maxUlps < IMMEDIATE_INT64(4*1024*1024*1024));
	INT64 aInt = *reinterpret_cast<INT64*>(&_1);
	// Make aInt lexicographically ordered as a twos-complement int
	if(aInt < 0)
		aInt = IMMEDIATE_INT64(0x8000000000000000) - aInt;
	// Make bInt lexicographically ordered as a twos-complement int
	INT64 bInt = *reinterpret_cast<INT64*>(&_2);
	if(bInt < 0)
		bInt = IMMEDIATE_INT64(0x8000000000000000) - bInt;
	INT64 intDiff = aInt - bInt;
	if( 0 > intDiff )
		intDiff = -intDiff;
	return intDiff <= maxUlps;
}

bool EsMath::areEqualFloats(float _1, float _2, INT32 maxUlps)
{
	// Make sure maxUlps is non-negative and small enough that the
	// default NAN won't compare as equal to anything.
	wxASSERT(maxUlps > 0 && maxUlps < 4*1024*1024);
	INT32 aInt = *reinterpret_cast<INT32*>(&_1);
	// Make aInt lexicographically ordered as a twos-complement int
	if(aInt < 0)
		aInt = 0x80000000 - aInt;
	// Make bInt lexicographically ordered as a twos-complement int
	INT32 bInt = *reinterpret_cast<INT32*>(&_2);
	if(bInt < 0)
		bInt = 0x80000000 - bInt;
	INT32 intDiff = abs(aInt - bInt);
	return intDiff <= maxUlps;
}

// round double value to the specified number of decimal places
//

static double pow10(int power)
{
	static const double s_powers10 [] = 
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

	int index = power + 7;
	if ( index >= 0 && index < (int)ES_CONST_ARRAY_COUNT(s_powers10) )
		return s_powers10[index];
	return pow(10.0, int(power)); // Default, inefficient implementation
}

static double pow2(int power)
{
	static const double s_negativePowers2 [] = 
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

	if ( power >= 0 )
	{
		if ( power < 32 )
			return double(unsigned(1u << power));
	}
	else 
	{
		int powerIndex = power + ES_CONST_ARRAY_COUNT(s_negativePowers2);
		if ( powerIndex >= 0 )
			return s_negativePowers2[powerIndex];
	}
	return pow(2.0, power);
}

double EsMath::round(double val, int numDecimalPlaces /*= 0*/)
{
	if ( numDecimalPlaces == 0 ) // special much faster and more precise case
		return EsUtilities::round0(val);

	double multiplier = pow10(numDecimalPlaces);
	return EsUtilities::round0(val * multiplier) / multiplier;
}
