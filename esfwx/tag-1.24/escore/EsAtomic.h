#ifndef _es_atomic_h_
#define _es_atomic_h_

/// Thread-safe atomic integer operations helper
///
class ESCORE_CLASS EsAtomic
{
public:
#if ES_OS == ES_OS_WINDOWS
  // Use windows OS functions
  typedef volatile LONG int_t;
#elif ES_OS == ES_OS_MAC
  typedef volatile int32_t int_t;
#else
  typedef int int_t;
#endif

public:
  static int_t inc(int_t& subj) ES_NOTHROW;
  static int_t dec(int_t& subj) ES_NOTHROW;

private:
  EsAtomic() ES_REMOVEDECL;
  EsAtomic(const EsAtomic&) ES_REMOVEDECL;
  EsAtomic& operator=(const EsAtomic&) ES_REMOVEDECL;
};

// Compatibility shortcurs
typedef EsAtomic::int_t EsAtomicInteger;
#define esAtomicInc(subj) EsAtomic::inc( (subj) )
#define esAtomicDec(subj) EsAtomic::dec( (subj) )

#endif // _es_atomic_h_
