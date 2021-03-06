#include "escorepch.h"
#pragma hdrstop

#include "EsRange.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

class EsRangeCtorAcc : public EsRange
{
public:
  EsRangeCtorAcc(const EsVariant& min, bool strictMin, const EsVariant& max, bool strictMax) :
  EsRange(
    min,
    strictMin,
    max,
    strictMax
  )
  {}
};
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsRange, esT("Reflected inline range class"))
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsRange, has, bool_CallConst_cr_EsVariant, esT("Return true if value is in range, false otherwise"))
  ES_DECL_REFLECTED_CTOR_INFO(EsRange, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, esT("Create range object, given min, isStrictMin, max, isStrictMax"))
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsRange, createMinExcl, EsVariant_ClassCall_cr_EsVariant, esT("Create range excluding lower boundary, and the open higher one"))
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsRange, createMinIncl, EsVariant_ClassCall_cr_EsVariant, esT("Create range including lower boundary, and the open higher one"))
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsRange, createMaxExcl, EsVariant_ClassCall_cr_EsVariant, esT("Create range excluding higher boundary, and the open lower one"))
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsRange, createMaxIncl, EsVariant_ClassCall_cr_EsVariant, esT("Create range including higher boundary, and the open lower one"))
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsRange, createMinExclMaxExcl, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant, esT("Create range excluding both lower and higher boundaries"))
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsRange, createMinInclMaxIncl, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant, esT("Create range including both lower and higher boundaries"))
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsRange, createMinExclMaxIncl, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant, esT("Create range excluding lower, and including higher boundary"))
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsRange, createMinInclMaxExcl, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant, esT("Create range including lower, and excluding higher boundary"))
  ES_DECL_PROP_INFO_RO(EsRange, min, EsVariant, NO_PROPERTY_LABEL, esT("Range minimum boundary"))
  ES_DECL_PROP_INFO_RO(EsRange, isStrictMin, bool, NO_PROPERTY_LABEL, esT("Is minimum inclusive or strict (exclusive)"))
  ES_DECL_PROP_INFO_RO(EsRange, max, EsVariant, NO_PROPERTY_LABEL, esT("Range maximum boundary"))
  ES_DECL_PROP_INFO_RO(EsRange, isStrictMax, bool, NO_PROPERTY_LABEL, esT("Is maximum inclusive or strict (exclusive)"))
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsRange::EsRange(const EsVariant& min, bool strictMin, const EsVariant& max, bool strictMax) :
m_min(min),
m_strictMin(strictMin),
m_max(max),
m_strictMax(strictMax)
{
  m_dynamic = true;
}
//---------------------------------------------------------------------------

bool EsRange::has(const EsVariant& val) const
{
  bool minOk = true;
  bool maxOk = true;

  if( !m_min.isEmpty() )
  {
    if( m_strictMin )
      minOk = m_min < val;
    else
      minOk = m_min <= val;
  }

  if( !m_max.isEmpty() )
  {
    if( m_strictMax )
      maxOk = m_max > val;
    else
      maxOk = m_max >= val;
  }

  return minOk && maxOk;
}
//---------------------------------------------------------------------------

EsVariant EsRange::NEW(const EsVariant& min, const EsVariant& isStrictMin, const EsVariant& max, const EsVariant& isStrictMax)
{
  std::unique_ptr<EsRangeCtorAcc> range = ES_MAKE_UNIQUE(
    EsRangeCtorAcc,
    min, 
    isStrictMin.asBool(), 
    max, 
    isStrictMax.asBool()
  );
  ES_ASSERT(range);
  
  return range.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsVariant EsRange::createMinExcl(const EsVariant& min)
{
  ES_ASSERT(!min.isEmpty());

  std::unique_ptr<EsRangeCtorAcc> range = ES_MAKE_UNIQUE(
    EsRangeCtorAcc,
    min, 
    true, 
    EsVariant::null(), 
    false
  );
  ES_ASSERT(range);
  
  return range.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsVariant EsRange::createMinIncl(const EsVariant& min)
{
  ES_ASSERT(!min.isEmpty());

  std::unique_ptr<EsRangeCtorAcc> range = ES_MAKE_UNIQUE(
    EsRangeCtorAcc,
    min,
    false,
    EsVariant::null(),
    false
  );
  ES_ASSERT(range);

  return range.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsVariant EsRange::createMaxExcl(const EsVariant& max)
{
  ES_ASSERT(!max.isEmpty());

  std::unique_ptr<EsRangeCtorAcc> range = ES_MAKE_UNIQUE(
    EsRangeCtorAcc,
    EsVariant::null(),
    false,
    max,
    true
  );
  ES_ASSERT(range);
  
  return range.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsVariant EsRange::createMaxIncl(const EsVariant& max)
{
  ES_ASSERT(!max.isEmpty());

  std::unique_ptr<EsRangeCtorAcc> range = ES_MAKE_UNIQUE(
    EsRangeCtorAcc,
    EsVariant::null(),
    false,
    max,
    false
  );
  ES_ASSERT(range);

  return range.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

void EsRange::minMaxComplianceCheck(const EsVariant& min, const EsVariant& max, bool incl /*= false*/)
{
  if(incl && min > max)
    EsException::Throw(
      esT("Range object minimum value must be less than or equal to maximum")
    );
  else if(!incl && min >= max)
    EsException::Throw(
      esT("Range object minimum value must be less than maximum")
    );
}
//---------------------------------------------------------------------------

EsVariant EsRange::createMinExclMaxExcl(const EsVariant& min, const EsVariant& max)
{
  ES_ASSERT(!min.isEmpty());
  ES_ASSERT(!max.isEmpty());

  minMaxComplianceCheck(min, max);

  std::unique_ptr<EsRangeCtorAcc> range = ES_MAKE_UNIQUE(
    EsRangeCtorAcc,
    min,
    true,
    max,
    true
  );
  ES_ASSERT(range);

  return range.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsVariant EsRange::createMinInclMaxIncl(const EsVariant& min, const EsVariant& max)
{
  ES_ASSERT(!min.isEmpty());
  ES_ASSERT(!max.isEmpty());

  minMaxComplianceCheck(min, max, true);

  std::unique_ptr<EsRangeCtorAcc> range = ES_MAKE_UNIQUE(
    EsRangeCtorAcc,
    min,
    false,
    max,
    false
  );
  ES_ASSERT(range);

  return range.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsVariant EsRange::createMinExclMaxIncl(const EsVariant& min, const EsVariant& max)
{
  ES_ASSERT(!min.isEmpty());
  ES_ASSERT(!max.isEmpty());

  minMaxComplianceCheck(min, max, true);

  std::unique_ptr<EsRangeCtorAcc> range = ES_MAKE_UNIQUE(
    EsRangeCtorAcc,
    min,
    true,
    max,
    false
  );
  ES_ASSERT(range);

  return range.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsVariant EsRange::createMinInclMaxExcl(const EsVariant& min, const EsVariant& max)
{
  ES_ASSERT(!min.isEmpty());
  ES_ASSERT(!max.isEmpty());

  minMaxComplianceCheck(min, max, true);

  std::unique_ptr<EsRangeCtorAcc> range = ES_MAKE_UNIQUE(
    EsRangeCtorAcc,
    min,
    false,
    max,
    true
  );
  ES_ASSERT(range);

  return range.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsVariant EsRange::get_min() const
{
  return m_min;
}
//---------------------------------------------------------------------------

EsVariant EsRange::get_max() const
{
  return m_max;
}
//---------------------------------------------------------------------------

bool EsRange::get_isStrictMin() const
{
  return m_strictMin;
}
//---------------------------------------------------------------------------

bool EsRange::get_isStrictMax() const
{
  return m_strictMax;
}
//---------------------------------------------------------------------------

void EsRange::customCopyFrom(const EsReflectedClassIntf::Ptr& src)
{
  ES_ASSERT(src);

  const EsRange* ptr = ES_INTFPTR_TO_OBJECTPTR<EsRange>(src);
  ES_ASSERT(ptr);

  m_min = ptr->m_min;
  m_strictMin = ptr->m_strictMin;
  m_max = ptr->m_max;
  m_strictMax = ptr->m_strictMax;
}
//---------------------------------------------------------------------------
