#include "escorepch.h"
#pragma hdrstop

#include "EsCoreRtl.h"

//---------------------------------------------------------------------------
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
#	include <System.hpp>
#	include <System.Math.hpp>
# pragma package(smart_init)

#ifdef __cplusplus
  extern "C" {
#endif

int es_finite(double d)
{
	return IsInfinite(d) ? 0 : 1;
}
//---------------------------------------------------------------------------

int es_finitef(float f)
{
	return IsInfinite(f) ? 0 : 1;
}
//---------------------------------------------------------------------------

int es_isnan(double d)
{
	return IsNan(d) ? 1 : 0;
}
//---------------------------------------------------------------------------

int es_isnanf(float f)
{
	return IsNan(f) ? 1 : 0;
}
//---------------------------------------------------------------------------

float es_ceilf(float f)
{
  return Ceil(f);
}
//---------------------------------------------------------------------------

double es_ceil(double d)
{
  return Ceil(d);
}
//---------------------------------------------------------------------------

float es_logf(float f)
{
  return Ln(f);
}
//---------------------------------------------------------------------------

double es_log(double d)
{
  return Ln(d);
}
//---------------------------------------------------------------------------

float es_log10f(float f)
{
  return Log10(f);
}
//---------------------------------------------------------------------------

double es_log10(double d)
{
  return Log10(d);
}
//---------------------------------------------------------------------------

double es_log2(double d)
{
  return Log2(d);
}
//---------------------------------------------------------------------------

double es_logN(double n, double d)
{
  return LogN(n, d);
}
//---------------------------------------------------------------------------

float es_powf(float x, float n)
{
  return Power(x, n);
}
//---------------------------------------------------------------------------

double es_pow(double x, double n)
{
  return Power(x, n);
}
//---------------------------------------------------------------------------

double es_exp(double n)
{
  return Exp(n);
}
//---------------------------------------------------------------------------

double es_sqrt(double x)
{
  return Sqrt(x);
}
//---------------------------------------------------------------------------

double es_modf(double x, double* ipart)
{
  if( ipart )
    *ipart = Int(x);

  return Frac(x);
}
//---------------------------------------------------------------------------

float es_modff(float x, float* ipart)
{
  if( ipart )
    *ipart = Int(x);

  return Frac(x);
}
//---------------------------------------------------------------------------

double es_abs(double f)
{
  return std::fabs(f);
}
//---------------------------------------------------------------------------

double es_sin(double rad)
{
  return Sin(rad);
}
//---------------------------------------------------------------------------

double es_sinh(double val)
{
  return Sinh(val);
}
//---------------------------------------------------------------------------

double es_asin(double val)
{
  return ArcSin(val);
}
//---------------------------------------------------------------------------

double es_cos(double rad)
{
  return Cos(rad);
}
//---------------------------------------------------------------------------

double es_cosh(double val)
{
  return Cosh(val);
}
//---------------------------------------------------------------------------

double es_acos(double val)
{
  return ArcCos(val);
}
//---------------------------------------------------------------------------

double es_tan(double rad)
{
  return Tan(rad);
}
//---------------------------------------------------------------------------

double es_tanh(double val)
{
  return Tanh(val);
}
//---------------------------------------------------------------------------

double es_atan(double val)
{
  return ArcTan(val);
}
//---------------------------------------------------------------------------

double es_atan_yx(double y, double x)
{
  return ArcTan2(y, x);
}
//---------------------------------------------------------------------------

//# if defined(_WIN64)
//// win64 lib bug
//double exp2(double d)
//{
//  return pow(2., d);
//}
//# endif
////---------------------------------------------------------------------------

#ifdef __cplusplus
  }
#endif

#else

#ifdef __cplusplus
  extern "C" {
#endif

double es_logN(double n, double d)
{
  return log(d)/log(n);
}

#ifdef __cplusplus
  }
#endif

#endif
//---------------------------------------------------------------------------

#if (ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND) && \
    defined(ES_WCHAR_IS_NOT_WCHAR_T)

#ifdef __cplusplus
  extern "C" {
#endif

ES_COMPILE_TIME_ASSERT(sizeof(unsigned) >= sizeof(ES_UCHAR), ucharSizeCheck);

size_t es_strlen(ES_CTSTR s)
{
	register ES_CTSTR p = s;
	while(*p) ++p;

	return p-s;
}
//---------------------------------------------------------------------------

int es_strcmp(ES_CTSTR s1, ES_CTSTR s2)
{
  register const ES_UCHAR* p1 = (const ES_UCHAR*) s1;
  register const ES_UCHAR* p2 = (const ES_UCHAR*) s2;
  unsigned c1, c2;

  do
  {
    c1 = (unsigned char) *p1++;
    c2 = (unsigned char) *p2++;
    if (c1 == esT('\0'))
    	return c1 - c2;
  }
  while (c1 == c2);

  return c1 - c2;
}
//---------------------------------------------------------------------------

int es_stricmp(ES_CTSTR s1, ES_CTSTR s2)
{
#ifdef ES_UNICODE
  ES_ASSERT( !esT("esStricmp is not implemented for UNICODE") );
#endif

  return es_strcmp(s1, s2);
}
//---------------------------------------------------------------------------

int es_strncmp(ES_CTSTR s1, ES_CTSTR s2, size_t n)
{
  unsigned c1 = esT('\0');
  unsigned c2 = esT('\0');

  if(n >= 4)
  {
    size_t n4 = n >> 2;

    do
    {
      c1 = (ES_UCHAR) *s1++;
      c2 = (ES_UCHAR) *s2++;
      if (c1 == esT('\0') || c1 != c2)
        return c1 - c2;

      c1 = (ES_UCHAR) *s1++;
      c2 = (ES_UCHAR) *s2++;
      if (c1 == esT('\0') || c1 != c2)
        return c1 - c2;

      c1 = (ES_UCHAR) *s1++;
      c2 = (ES_UCHAR) *s2++;
      if (c1 == esT('\0') || c1 != c2)
        return c1 - c2;

      c1 = (ES_UCHAR) *s1++;
      c2 = (ES_UCHAR) *s2++;
      if (c1 == esT('\0') || c1 != c2)
        return c1 - c2;

    } while (--n4 > 0);

    n &= 3;
  }

  while (n > 0)
  {
    c1 = (ES_UCHAR) *s1++;
    c2 = (ES_UCHAR) *s2++;
    if (c1 == esT('\0') || c1 != c2)
    	return c1 - c2;

    n--;
  }

  return c1-c2;
}
//---------------------------------------------------------------------------

ES_CTSTR es_strchr(ES_CTSTR s, ES_CHAR c)
{
	do
  {
		if(*s == c)
 			return s;

	} while(*s++);

	return 0;
}
//---------------------------------------------------------------------------

ES_CTSTR es_strstr(ES_CTSTR s1, ES_CTSTR s2)
{
	register ES_CTSTR s = s1;
	register ES_CTSTR p = s2;

	do
  {
		if(!*p)
			return s1;

		if(*p == *s)
    {
			++p;
			++s;
		}
    else
    {
			p = s2;
			if(!*s)
				return 0;

			s = ++s1;
		}

	} while (1);
}
//---------------------------------------------------------------------------

extern "C" void tm2esDT(const struct tm *val, esDT* dt);
size_t es_strftime(ES_CHAR* str, size_t maxsize, ES_CTSTR fmt, const struct tm *ts)
{
  esDT dt;
	tm2esDT(ts, &dt);
  EsDateTime edt(dt);

  return edt.strftime(
    EsLocale::locale(),
    str,
    maxsize,
    fmt
  );
}
//---------------------------------------------------------------------------

#ifdef __cplusplus
  }
#endif

#endif

// Special short-cut for exception throwing, before actual exception class is declared
void EsExceptionThrow(ES_CTSTR error)
{
  EsException::Throw(error);
}

