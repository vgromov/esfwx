#ifndef _es_math_delay_line_h_
#define _es_math_delay_line_h_

/// Delay line implementation, with averageing
///
class ESMATH_CLASS ES_INTF_IMPL1(EsMathDelayLine, EsReflectedClassIntf)

public:
  EsMathDelayLine(ulong length);

  ES_DECL_REFLECTED_CLASS_BASE(EsMathDelayLine)
	ES_DECL_ATTR_HANDLING_STD

  /// Reflected Constructors
  ///

  /// Constructor. Return delay line object of specified length
  ///
  ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, cr_EsVariant);


  /// Reflected services
  ///

  /// Return true if delay line is empty
  ES_DECL_REFLECTED_CONST_METHOD0(bool, isEmpty);

  /// Return true if delay line is saturated
  ES_DECL_REFLECTED_CONST_METHOD0(bool, isSaturated);

  /// Return count of elements in delay line
  ES_DECL_REFLECTED_CONST_METHOD0(ulong, itemCountGet);

  /// Return accumulated data value
  ES_DECL_REFLECTED_CONST_METHOD0(double, accumulatorGet);

  /// Reset delay line state data
  ES_DECL_REFLECTED_METHOD0(void, clear);

  /// Push data to the delay line
  ES_DECL_REFLECTED_METHOD1(void, itemAppend, double);

  /// Get result value from the delay line
  ES_DECL_REFLECTED_CONST_METHOD0(double, valueGet);

  /// Reflected properties
  ///

  /// Emptiness check
  ES_DECL_PROPERTY_RO(empty, bool)
  /// Saturation check
  ES_DECL_PROPERTY_RO(saturated, bool)
  /// Items count
  ES_DECL_PROPERTY_RO(count, ulong)
  /// Current delay line accumulator value
  ES_DECL_PROPERTY_RO(accumulator, double)
  /// Current delay line value
  ES_DECL_PROPERTY_RO(value, double)
  /// Assign delay line length
  ES_DECL_PROPERTY_STD(length, ulong)

protected:
  // Internal services
  void internalClear();
  void internalItemAppend(double val);
  double internalValueGet() const;
  void internalLengthSet(ulong length);

protected:
  mutable EsCriticalSection m_cs;
  typedef std::list<double> BuffT;
  BuffT m_buff;
  double m_accum;

private:
  EsMathDelayLine();
  EsMathDelayLine(const EsMathDelayLine&);
  EsMathDelayLine& operator=(const EsMathDelayLine&);
};

/// Delay line extended implementation, with accumulated minimax calculation.
/// Accumulated minimax may be reset both manually, and automatically, by
/// accumulator counter overflow.
///
/// Delay line implementation, with averageing
///
class ESMATH_CLASS ES_INTF_IMPL1(EsMathDelayLineEx, EsReflectedClassIntf)

public:
  EsMathDelayLineEx(ulong length);

  ES_DECL_REFLECTED_CLASS_BASE(EsMathDelayLineEx)
	ES_DECL_ATTR_HANDLING_STD

  /// Reflected Constructors
  ///

  /// Constructor. Return delay line object of specified length
  ///
  ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, cr_EsVariant);


  /// Reflected services
  ///

  /// Return true if delay line is empty
  ES_DECL_REFLECTED_CONST_METHOD0(bool, isEmpty);

  /// Return true if delay line is saturated
  ES_DECL_REFLECTED_CONST_METHOD0(bool, isSaturated);

  /// Return count of elements in delay line
  ES_DECL_REFLECTED_CONST_METHOD0(ulong, itemCountGet);

  /// Return accumulated data value
  ES_DECL_REFLECTED_CONST_METHOD0(double, accumulatorGet);

  /// Reset delay line state data
  ES_DECL_REFLECTED_METHOD0(void, clear);

  /// Push data to the delay line
  ES_DECL_REFLECTED_METHOD1(void, itemAppend, double);

  /// Get result value from the delay line
  ES_DECL_REFLECTED_CONST_METHOD0(double, valueGet);

  /// Reset minimax calculations
  ES_DECL_REFLECTED_METHOD0(void, minimaxReset);

  /// Reflected properties
  ///

  /// Emptiness check
  ES_DECL_PROPERTY_RO(empty, bool)
  /// Saturation check
  ES_DECL_PROPERTY_RO(saturated, bool)
  /// Items count
  ES_DECL_PROPERTY_RO(count, ulong)
  /// Current delay line accumulator value
  ES_DECL_PROPERTY_RO(accumulator, double)
  /// Current delay line value
  ES_DECL_PROPERTY_RO(value, double)
  /// Current delay line minimum
  ES_DECL_PROPERTY_RO(min, double)
  /// Current delay line maximum
  ES_DECL_PROPERTY_RO(max, double)
  /// Count of nodes involved in minimax calculations
  ES_DECL_PROPERTY_RO(minimaxCount, ulong)
  /// Minimax reset threshold
  ES_DECL_PROPERTY_STD(minimaxResetThreshold, ulong)
  /// Delay line length
  ES_DECL_PROPERTY_STD(length, ulong)

protected:
  // Internal services
  void internalClear();
  void internalItemAppend(double val);
  double internalValueGet() const;
  void internalLengthSet(ulong length);
  void internalMinimaxReset();
  void calcMinimaxIfReset();

protected:
  mutable EsCriticalSection m_cs;
  typedef std::list<double> BuffT;
  BuffT m_buff;
  double m_accum;
  double m_min;
  double m_max;
  ulong m_minimaxCnt;
  bool m_minimaxReset;

private:
  EsMathDelayLineEx();
  EsMathDelayLineEx(const EsMathDelayLineEx&);
  EsMathDelayLineEx& operator=(const EsMathDelayLineEx&);
};

#endif // _es_math_delay_line_h_
