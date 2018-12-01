#ifndef _es_math_array_complex_h_
#define _es_math_array_complex_h_

/// Math-specific array of complex values
///
class ESMATH_CLASS ES_INTF_IMPL1(EsMathArrayComplex, EsReflectedClassIntf)

public:
  typedef const EsMath::Complex& value_type;
  typedef EsMath::Complex*        pointer;
  typedef const EsMath::Complex* const_pointer;

public:
  ES_DECL_REFLECTED_CLASS_BASE(EsMathArrayComplex)
  ES_DECL_ATTR_HANDLING_STD

  /// Non-reflected services
  ///

  EsMathArrayComplex(ulong size = 0);
  EsMathArrayComplex(const EsMathArrayComplex& src);

  virtual ~EsMathArrayComplex();

  /// indexed item acess operators
  const EsMath::Complex& itemGet(ulong idx) const;
  void itemSet(ulong idx, const EsMath::Complex& val);

  /// direct data access
  const EsMath::Complex* dataGet() const;
  EsMath::Complex* dataAccess();
  void dataSet(ulong size, const EsMath::Complex* data);

  /// assignment operator
  EsMathArrayComplex& operator=(const EsMathArrayComplex& src);

  /// semi-private "black box" access.
  /// for internal usage only
  ///
  void* pimplGet() { return m_pimpl; }
  const void* pimplGet() const { return m_pimpl; }

  /// Return our copy as EsBaseIntfPtr
  EsBaseIntfPtr clone() const;

  /// Try to interpret variant as EsMathArrayComplex*.
  /// If variant could not be referenced as EsMathArrayComplex*,
  /// return nullptr
  ///
  static EsMathArrayComplex* fromVariant(const EsVariant& in);

  /// Reflected services
  ///

  /// Reflected Constructors
  ///

  /// Default constructor. Return empty array of size 0
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

  /// Copy constructor. Return object, that is copy of array|collection,
  /// passed as a parameter
  ///
  ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, cr_EsVariant);

  /// Clear array object
  ES_DECL_REFLECTED_METHOD0(void, clear);
  /// Fill array with values
  ES_DECL_REFLECTED_METHOD2(void, fill, double, double);
  /// Fill array with values from variant collection's [0] && [1] items
  ES_DECL_REFLECTED_METHOD1(void, fill, cr_EsVariant);
  /// Indexed object services
  ES_DECL_REFLECTED_CONST_METHOD0(ulong, countGet);
  ES_DECL_REFLECTED_METHOD1(void, countSet, ulong);
  /// Complex array item is presented as collection of [double, double]
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, _itemGet, ulong);
  ES_DECL_REFLECTED_METHOD2(void, itemSet, ulong, cr_EsVariant);

  /// Properties
  ///

  /// Emptiness check
  ES_DECL_PROPERTY_RO(empty, bool)
  /// Items count
  ES_DECL_PROPERTY(count, ulong)
  /// Persistent property - access|assign array as a collection of items
  ES_DECL_PROPERTY(items, EsVariant)

private:
  /// Internal helpers
  ///
  void copy(const EsMathArrayComplex& src);

protected:
  // "black box" storage implementation
  void* m_pimpl;

  friend class EsMathFft;
};

#endif // _es_math_array_complex_h_

