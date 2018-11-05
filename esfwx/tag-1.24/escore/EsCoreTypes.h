#ifndef _es_core_types_h_
#define _es_core_types_h_

/// basic type defines for ekosf core library
/// especially, for reflection types short-cuts
///
typedef void*									p_void;
typedef const void*						cp_void;
typedef const long&						cr_long;
typedef unsigned long					ulong;
typedef const ulong&					cr_ulong;
typedef long long							llong;
typedef const llong&					cr_llong;
typedef unsigned long long		ullong;
typedef const ullong&					cr_ullong;
typedef const double&					cr_double;
typedef const bool&						cr_bool;
typedef unsigned char         byte;
typedef const byte&           cr_byte;

#define ES_MAKE_ULLONG(low, high)	((ullong)(ulong)(low) + ((ullong)(ulong)(high) << 32))

#define ES_MAKE_UUID( Data32_0, Data16_0, Data16_1, Data8_0, Data8_1, Data8_2, Data8_3, Data8_4, Data8_5, Data8_6, Data8_7) \
  {(Data32_0), (Data16_0), (Data16_1), {(Data8_0), (Data8_1), (Data8_2), (Data8_3), (Data8_4), (Data8_5), (Data8_6), (Data8_7)}}

#endif // _es_core_types_h_
