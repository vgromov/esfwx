// Do not include this file directly in project.
// It's included in EsAtomic.cpp internally
//

EsAtomic::int_t EsAtomic::inc(EsAtomic::int_t& subj) ES_NOTHROW
{
	return ::InterlockedIncrement( &subj );
}
//---------------------------------------------------------------------------

EsAtomic::int_t EsAtomic::dec(EsAtomic::int_t& subj) ES_NOTHROW
{
	return ::InterlockedDecrement( &subj );
}
//---------------------------------------------------------------------------

