#ifndef _es_range_h_
#define _es_range_h_

// Reflected implementation of value range
//
class ESCORE_CLASS ES_INTF_IMPL1(EsRange, EsReflectedClassIntf)

protected:
  EsRange(const EsVariant& min, bool strictMin, const EsVariant& max, bool strictMax);

  static void minMaxComplianceCheck(const EsVariant& min, const EsVariant& max, bool incl = false);

public:
  // reflection declaration
  //
  ES_DECL_REFLECTED_CLASS_BASE( EsRange )
  ES_DECL_ATTR_HANDLING_STD

  // Reflected services
  //
  // Value checking
  ES_DECL_REFLECTED_CONST_METHOD1(bool, has, cr_EsVariant);

  // Standard ctor
  ES_DECL_REFLECTED_CLASS_CTOR4(EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  // Non-standard constructors
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, createMinExcl, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, createMinIncl, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, createMaxExcl, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, createMaxIncl, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, createMinExclMaxExcl, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, createMinInclMaxIncl, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, createMinExclMaxIncl, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, createMinInclMaxExcl, cr_EsVariant, cr_EsVariant);

  // Properties
  ES_DECL_PROPERTY_RO(min, EsVariant)
  ES_DECL_PROPERTY_RO(isStrictMin, bool)
  ES_DECL_PROPERTY_RO(max, EsVariant)
  ES_DECL_PROPERTY_RO(isStrictMax, bool)

protected:
  EsVariant m_min;
  bool m_strictMin;
  EsVariant m_max;
  bool m_strictMax;
};

#endif // _es_range_h_
