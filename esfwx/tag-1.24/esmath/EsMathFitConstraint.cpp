#include "esmathpch.h"
#pragma hdrstop

#include "EsMathFitConstraint.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

namespace EsReflection
{
ES_DECL_ENUMERATION_INFO(EsMathFitInfo, NO_CLASS_DESCR)
ES_DECL_ENUMERATION_INFO(EsMathFitConstraintKind, NO_CLASS_DESCR)
}
//---------------------------------------------------------------------------

EsMathFitConstraint::EsMathFitConstraint(double x /*= 0.*/, double constraint /*= 0.*/,
	EsMathFitConstraintKind kind /*= EsMathFitConstraintKind::Value*/) :
m_x(x),
m_constraint(constraint),
m_kind(kind)
{}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void constraintsFromVariant(EsMathFitConstraints& csx, const EsVariant& src)
{
  csx.clear();

  if( src.isCollection() )
  {
    const EsVariant& item = src.itemGet(0);

    if( item.isCollection() )
    {
      for(int idx = 0; idx < src.countGet(); ++idx)
      {
        const EsVariant& constraintItem = src.itemGet(idx);

        csx.push_back(
          EsMathFitConstraint(
            constraintItem.itemGet(0).asDouble(),
            constraintItem.itemGet(1).asDouble(),
            static_cast<EsMathFitConstraintKind>(constraintItem.itemGet(2).asULong())
          )
        );
      }
    }
    else
    {
      csx.push_back(
        EsMathFitConstraint(
          item.asDouble(),
          src.itemGet(1).asDouble(),
          static_cast<EsMathFitConstraintKind>(src.itemGet(2).asULong())
        )
      );
    }
  }
}
//---------------------------------------------------------------------------
