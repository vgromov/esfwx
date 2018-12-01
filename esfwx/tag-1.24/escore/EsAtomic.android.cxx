// Do not include this file directly in project.
// It's included in EsAtomic.cpp internally
//
#include <sys/atomics.h>

EsAtomic::int_t EsAtomic::inc(EsAtomic::int_t& subj) ES_NOTHROW
{
  return __atomic_inc( &subj );
}
//---------------------------------------------------------------------------

EsAtomic::int_t EsAtomic::dec(EsAtomic::int_t& subj) ES_NOTHROW
{
  return __atomic_dec( &subj );
}
//---------------------------------------------------------------------------

