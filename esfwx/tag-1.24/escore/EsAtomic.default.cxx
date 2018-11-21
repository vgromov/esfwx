// Do not include this file directly in project.
// It's included in EsAtomic.cpp internally
//
EsAtomic::int_t EsAtomic::inc(EsAtomic::int_t& subj) ES_NOTHROW
{
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_GNUC
  return __sync_add_and_fetch(&subj, 1);
#else
# error EsAtomic::inc not implemented!
#endif

	return ++subj;
}
//---------------------------------------------------------------------------

EsAtomic::int_t EsAtomic::dec(EsAtomic::int_t& subj) ES_NOTHROW
{
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_GNUC
  return __sync_sub_and_fetch(&subj, 1);
#else
# error EsAtomic::dec not implemented!
#endif

	return --subj;
}
//---------------------------------------------------------------------------
