#include "esmathpch.h"
#pragma hdrstop

#include "EsMathSplineCalculator.h"

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsMathSplineCalculator, NO_CLASS_DESCR)
  // Constructors
  ES_DECL_REFLECTED_CTOR_INFO(      EsMathSplineCalculator, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
  // Services
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSplineCalculator, calculate, double_CallConst_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSplineCalculator, calculateV, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSplineCalculator, calculateVectorInplace, void_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  // Properties
  ES_DECL_PROP_INFO_RO(             EsMathSplineCalculator, isOk, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathSplineCalculator, minX, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathSplineCalculator, maxX, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathSplineCalculator, rangeX, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                EsMathSplineCalculator, nodes, EsVariant, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void EsMathSplineCalculator::checkIsValid() const
{
  if( !get_isOk() )
    EsMathException::Throw(
      esT("EsMathSplineCalculator is not in valid state")
    );
}
//--------------------------------------------------------------------------------

bool EsMathSplineCalculator::get_isOk() const 
{
  return !m_table.empty();
}
//---------------------------------------------------------------------------

double EsMathSplineCalculator::get_minX() const
{
  checkIsValid();

  return m_table[0].x();
}
//---------------------------------------------------------------------------

double EsMathSplineCalculator::get_maxX() const
{
  checkIsValid();

  return m_table[ m_table.size()-1 ].xp();
}
//---------------------------------------------------------------------------

EsVariant EsMathSplineCalculator::get_rangeX() const
{
  EsVariant::Array va = {
    get_minX(),
    get_maxX()
  };

  return va;
}
//---------------------------------------------------------------------------

EsVariant EsMathSplineCalculator::get_nodes() const
{
  if( !get_isOk() )
    return EsVariant::null();    

  EsVariant result(EsVariant::VAR_VARIANT_COLLECTION);
  for(size_t idx = 0; idx < m_table.size(); ++idx)
  {
    const EsMathSplineNode& node = m_table[idx];
    EsVariant::Array vn(6);
    vn[0] = node.x();
    vn[1] = node.xp();
    vn[2] = node.a();
    vn[3] = node.b();
    vn[4] = node.c();
    vn[5] = node.d();

    result.addToVariantCollection(vn);
  }

  return result;
}
//---------------------------------------------------------------------------

void EsMathSplineCalculator::set_nodes(const EsVariant& nodes)
{
  const EsVariant::Array& anodes = nodes.asVariantCollection();

  m_table.clear();
  m_table.reserve(
    anodes.size()
  );

  bool reducedNode = false;
  bool prevReducedNode = false;
  for(size_t idx = 0; idx < anodes.size(); ++idx)
  {
    const EsVariant::Array& node = anodes[idx].asVariantCollection();
    size_t memberCnt = node.size();
    if( 5 == memberCnt )
      reducedNode = true;
    else if( 6 == memberCnt )
      reducedNode = false;
    else
      EsMathException::Throw(
        EsString::format(
          esT("%s: node member count %d is not supported, expected 5 for reduced, or 6, for full node."),
          EsString::fromAscii(__func__),
          memberCnt
        ),
        EsScriptDebugInfo::create(
          __LINE__,
          0,
          EsString::fromAscii(__FILE__)
        )
      );

    // Previous node was a reduced one - initialize missed m_xp in previous table node
    if( 
      prevReducedNode && 
      !m_table.empty() 
    )
      m_table[m_table.size()-1].xp( 
        node[0].asDouble()
      );

    if( !reducedNode )
    {
      EsMathSplineNode mnode(
        node[0].asDouble(),
        node[1].asDouble(),
        node[2].asDouble(),
        node[3].asDouble(),
        node[4].asDouble(),
        node[5].asDouble()
      );

      m_table.push_back(
        mnode
      );
    }
    else
    {
      EsMathSplineNode mnode(
        node[0].asDouble(),
        node[0].asDouble(),
        node[1].asDouble(),
        node[2].asDouble(),
        node[3].asDouble(),
        node[4].asDouble()
      );

      m_table.push_back(
        mnode
      );
    }

    prevReducedNode = reducedNode;
  }

}
//---------------------------------------------------------------------------

int EsMathSplineCalculator::nodeFind(size_t& idx, double x) const ES_NOTHROW
{
  if( m_table.empty() )
    return -1;

  if( x < m_table[0].x() )
  {
    idx = 0;
    return -1;
  }

  if( x > m_table[m_table.size() - 1].xp() )
  {
    idx = m_table.size() - 1;
    return 1;
  }

  // Binary search in the [ x[0], ..., x[n-1] ]
  size_t leftIdx = 0;
  size_t rightIdx = m_table.size() - 1;

  while( leftIdx != rightIdx )
  {
    size_t midIdx = (leftIdx + rightIdx) / 2;

    if( m_table[midIdx].x() >= x )
      rightIdx = midIdx;
    else
      leftIdx = midIdx;
  }
      
  idx = leftIdx;
  return 0;  
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathSplineCalculator::NEW()
{
  std::unique_ptr<EsMathSplineCalculator> f = ES_MAKE_UNIQUE( EsMathSplineCalculator );
  f->m_dynamic = true;
  return f.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

double EsMathSplineCalculator::calculate(double x) const 
{
  checkIsValid();

  size_t idx = 0;
  int ok = nodeFind(
    idx,
    x
  );

  if( 0 == ok )
  {
    const EsMathSplineNode& node = m_table[idx];
    double delta = (x - node.x());

    return node.a() + (node.b() + (node.c() + node.d() * delta) * delta) * delta;
  }
  else if( ok < 0 )
    EsMathException::Throw(
      EsString::format(
        esT("%s: an argument %f is lower, than definition range minimum %f"),
        EsString::fromAscii(__func__),
        x,
        get_minX()
      ),
      EsScriptDebugInfo::create(
        __LINE__,
        0,
        EsString::fromAscii(__FILE__)
      )
    );
  else if( ok > 0 )
    EsMathException::Throw(
      EsString::format(
        esT("%s: an argument %f is greater, than definition range maximum %f"),
        EsString::fromAscii(__func__),
        x,
        get_maxX()
      ),
      EsScriptDebugInfo::create(
        __LINE__,
        0,
        EsString::fromAscii(__FILE__)
      )
    );

  // Pacify compilers
  return 0.;
}
//---------------------------------------------------------------------------

EsVariant EsMathSplineCalculator::calculateV(cr_EsVariant in) const
{
  if( in.isCollection() || in.isObject() )
  {
    EsMathArrayReal* ar = nullptr;
    EsBaseIntfPtr result = EsMathArrayReal::baseIntfPtrCreate(
      in,
      &ar
    );

    if( !result )
      EsMathException::Throw(
        EsString::format(
          esT("%s call error, input parameter could not be interpreted as an array or collection"),
          EsString::fromAscii(__func__)
        ),
        EsScriptDebugInfo::create(
          __LINE__,
          0,
          EsString::fromAscii(__FILE__)
        )
      );

    ulong cnt = ar->countGet();
    for(ulong idx = 0; idx < cnt; ++idx)
    {
      ar->itemSet(
        idx,
        calculate(
          ar->itemGet(
            idx
          )
        )
      );
    }

    return result;
  }
  else
    return calculate( in.asDouble() );
}
//---------------------------------------------------------------------------

void EsMathSplineCalculator::calculateVectorInplace(cr_EsVariant in) const
{
  EsMathArrayReal* ar = ES_VARIANT_TO_OBJECTPTR<EsMathArrayReal>(in);
  if( !ar )
    EsMathException::Throw(
      EsString::format(
        esT("%s call error, input parameter could not be interpreted as an array or collection"),
        EsString::fromAscii(__func__)
      ),
      EsScriptDebugInfo::create(
        __LINE__,
        0,
        EsString::fromAscii(__FILE__)
      )
    );

  ulong cnt = ar->countGet();
  for(ulong idx = 0; idx < cnt; ++idx)
  {
    ar->itemSet(
      idx,
      calculate(
        ar->itemGet(
          idx
        )
      )
    );
  }
}
//---------------------------------------------------------------------------
