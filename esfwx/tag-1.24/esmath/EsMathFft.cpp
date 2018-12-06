#include "esmathpch.h"
#pragma hdrstop

#include "EsMathFft.h"
#include "EsMathInternals.h"
#include "alglib/fasttransforms.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsMathFft, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMathFft, transformReal, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_REFLECTED_SERVICES_INFO_END
//---------------------------------------------------------------------------

void EsMathFft::transform(const EsMathArrayReal& in, EsMathArrayComplex& out)
{
  ES_ALGLIB_TRY
  alglib::fftr1d(alglibCastFromArray(in), alglibCastFromArray(out));
  ES_ALGLIB_CATCH
}
//---------------------------------------------------------------------------

EsVariant EsMathFft::transformReal( cr_EsVariant in )
{
  std::unique_ptr<EsMathArrayComplex> out( new EsMathArrayComplex );
  ES_ASSERT(out);
  out->m_dynamic = true;

  EsMathArrayReal sig;
  sig.set_items(in);

  transform(sig, *out.get());

  return out.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

