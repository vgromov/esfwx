// Do not include this file directly in project.
// It's included in EsAtomic.cpp internally
//
#include <libkern/OSAtomic.h>

EsAtomic::int_t EsAtomic::inc(EsAtomic::int_t& subj) ES_NOTHROW
{
	return OSAtomicIncrement32( &subj );
}
//---------------------------------------------------------------------------

EsAtomic::int_t EsAtomic::dec(EsAtomic::int_t& subj) ES_NOTHROW
{
	return OSAtomicDecrement32( &subj );
}
//---------------------------------------------------------------------------

