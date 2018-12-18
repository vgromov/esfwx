#ifndef _es_math_array_real_h_
#define _es_math_array_real_h_

/// Math-specific array of real values
///
class ESMATH_CLASS ES_INTF_IMPL1(EsMathArrayReal, EsReflectedClassIntf)

public:
  typedef double         value_type;
  typedef double*       pointer;
  typedef const double* const_pointer;

public:
  ES_DECL_REFLECTED_CLASS_BASE(EsMathArrayReal)
  ES_DECL_ATTR_HANDLING_STD

  /// Non-reflected services
  ///

  EsMathArrayReal(ulong size = 0, const double* data = 0);
  EsMathArrayReal(const EsMathArrayReal& src);

  virtual ~EsMathArrayReal();

  /// Direct data access
  const double* dataGet() const;
  double* dataAccess();
  void dataSet(ulong size, const double* data);

  /// Assignment operator
  EsMathArrayReal& operator=(const EsMathArrayReal& src);

  /// Semi-private "black box" access.
  /// For internal usage only
  ///
  void* pimplGet() { return m_pimpl; }
  const void* pimplGet() const { return m_pimpl; }

  /// Return our dynamic copy as EsBaseIntfPtr
  ///
  ES_DECL_REFLECTED_CONST_METHOD0(EsBaseIntfPtr, clone);

  /// Create EsMathArrayReal object, optionally filling-in its items from src,
  /// return created object as EsVariant-compatible EsBaseIntfPtr.
  /// Optionally, if outp is not nullptr, set it to the created object pointer
  /// for later direct use.
  ///
  static EsBaseIntfPtr baseIntfPtrCreate(cr_EsVariant src, EsMathArrayReal** outp);

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

  /// Pseudo-constructors. Assign real array from binary buffer contents
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBaseIntfPtr, newFromU8buffer, cr_EsBinBuffer);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBaseIntfPtr, newFromI8buffer, cr_EsBinBuffer);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBaseIntfPtr, newFromU16buffer, cr_EsBinBuffer);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBaseIntfPtr, newFromI16buffer, cr_EsBinBuffer);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBaseIntfPtr, newFromU32buffer, cr_EsBinBuffer);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBaseIntfPtr, newFromI32buffer, cr_EsBinBuffer);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBaseIntfPtr, newFromFloatBuffer, cr_EsBinBuffer);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBaseIntfPtr, newFromDoubleBuffer, cr_EsBinBuffer);

  /// Recalculate array minimax
  ES_DECL_REFLECTED_METHOD0(void, minimaxRecalc);

  /// Clear array object
  ES_DECL_REFLECTED_METHOD0(void, clear);

  /// Fill array with value
  ES_DECL_REFLECTED_METHOD1(void, fill, double);

  /// Standard math reflected operators
  ///

  /// Vector addition. vector + vector, or vector + double is supported.
  ES_DECL_REFLECTED_METHOD1(EsVariant, add, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD1(void, add_inplace, cr_EsVariant);

  /// Vector subtraction. vector - vector, or vector - double is supported.
  ES_DECL_REFLECTED_METHOD1(EsVariant, subtract, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD1(void, subtract_inplace, cr_EsVariant);

  /// Vector multiplication. vector * vector, or vector * double is supported.
  ES_DECL_REFLECTED_METHOD1(EsVariant, multiply, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD1(void, multiply_inplace, cr_EsVariant);

  /// Vector division. vector / vector, or vector / double is supported.
  ES_DECL_REFLECTED_METHOD1(EsVariant, divide, cr_EsVariant);
  ES_DECL_REFLECTED_METHOD1(void, divide_inplace, cr_EsVariant);

  /// Direct assignment from binary buffer. These shortcut versions do not throw
  /// exceptions, just return false on any error occurred.
  ///
  ES_DECL_REFLECTED_METHOD1(bool, fromU8buffer, cr_EsBinBuffer);
  ES_DECL_REFLECTED_METHOD1(bool, fromI8buffer, cr_EsBinBuffer);
  ES_DECL_REFLECTED_METHOD1(bool, fromU16buffer, cr_EsBinBuffer);
  ES_DECL_REFLECTED_METHOD1(bool, fromI16buffer, cr_EsBinBuffer);
  ES_DECL_REFLECTED_METHOD1(bool, fromU32buffer, cr_EsBinBuffer);
  ES_DECL_REFLECTED_METHOD1(bool, fromI32buffer, cr_EsBinBuffer);
  ES_DECL_REFLECTED_METHOD1(bool, fromFloatBuffer, cr_EsBinBuffer);
  ES_DECL_REFLECTED_METHOD1(bool, fromDoubleBuffer, cr_EsBinBuffer);

  /// Direct assignment from binary buffer with range check and spike elimination
  /// These 'full' versions of services above, do throw an exceptions,
  /// if misc. error conditions are encountered durng buffer assignment, when doThrow
  /// parameter is set to true
  ///
  /// @param      const EsBinBuffer& src
  /// @param      double min
  /// @param      double max
  /// @param      bool rangeReplace       - if true, chech if values, being assigned, are in min-max range,
  ///                                       and if not doThrow, replace all values greater than max with max,
  ///                                       and all values less than min, with min
  /// @param      bool doThrow            - throw an exception if any assignment error occurred,
  ///                                       otherwise, return false
  /// @return                             - true, if assignment is successful,
  ///                                       false, if doThrow is false, and assignment is not
  ///                                       successful. Otherwise, an exception is thrown.
  ///
  ES_DECL_REFLECTED_METHOD5(bool, fromU8buffer, cr_EsBinBuffer, double, double, bool, bool);
  ES_DECL_REFLECTED_METHOD5(bool, fromI8buffer, cr_EsBinBuffer, double, double, bool, bool);
  ES_DECL_REFLECTED_METHOD5(bool, fromU16buffer, cr_EsBinBuffer, double, double, bool, bool);
  ES_DECL_REFLECTED_METHOD5(bool, fromI16buffer, cr_EsBinBuffer, double, double, bool, bool);
  ES_DECL_REFLECTED_METHOD5(bool, fromU32buffer, cr_EsBinBuffer, double, double, bool, bool);
  ES_DECL_REFLECTED_METHOD5(bool, fromI32buffer, cr_EsBinBuffer, double, double, bool, bool);
  ES_DECL_REFLECTED_METHOD5(bool, fromFloatBuffer, cr_EsBinBuffer, double, double, bool, bool);
  ES_DECL_REFLECTED_METHOD5(bool, fromDoubleBuffer, cr_EsBinBuffer, double, double, bool, bool);

  /// Indexed object services
  ///
  ES_DECL_REFLECTED_CONST_METHOD0(ulong, countGet);
  ES_DECL_REFLECTED_METHOD1(void, countSet, ulong);
  ES_DECL_REFLECTED_CONST_METHOD1(double, itemGet, ulong);
  ES_DECL_REFLECTED_METHOD2(void, itemSet, ulong, double);

  /// Return array contents as formatted string (each element is formatted
  /// according fmt parameter, or, if not set, using default: '%f')
  /// Each element is separated with sep, or, if not set, by ';'
  ///
  ES_DECL_REFLECTED_CONST_METHOD2(EsString, asString, cr_EsString, cr_EsString);

  /// Custom stream reader
  ES_DECL_REFLECTED_METHOD1(void, streamRead, cr_EsBaseIntfPtr);

  /// Custom stream writer
  ES_DECL_REFLECTED_METHOD1(void, streamWrite, cr_EsBaseIntfPtr);

  /// Properties
  ///

  /// "Array is dirty" flag.
  /// IF set to true, it will cause minimax recalculation on next call to any
  /// minimax getter
  ///
  ES_DECL_PROPERTY(dirty, bool)
  /// Emptiness check
  ES_DECL_PROPERTY_RO(empty, bool)
  /// Minimum value in array
  ES_DECL_PROPERTY_RO(min, double)
  /// Maximum value in array
  ES_DECL_PROPERTY_RO(max, double)
  /// Index of minimum value
  ES_DECL_PROPERTY_RO(minIdx, ulong)
  /// Index of maximum value
  ES_DECL_PROPERTY_RO(maxIdx, ulong)
  /// Total sum value
  ES_DECL_PROPERTY_RO(total, double)
  /// Items count
  ES_DECL_PROPERTY(count, ulong)
  /// Access|assign array as a collection of items
  ES_DECL_PROPERTY(items, EsVariant)

private:
  // Internal helpers
  //
  void copy(const EsMathArrayReal& src);

protected:
  // "black box" storage implementation
  void* m_pimpl;
  mutable bool m_needRecalc;
  mutable ulong m_minIdx;
  mutable ulong m_maxIdx;
  mutable double m_total;

  friend class EsMathConvolutionReal;
};
//---------------------------------------------------------------------------

#endif // _es_math_array_real_h_
