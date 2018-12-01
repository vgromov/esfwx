#ifndef _es_date_time_h_
#define _es_date_time_h_

// todo: add string parsing services to the date time span and date time to support its creation from string representation
// as well as comparison with strings (maybe even math ops with strings)
class EsDateTime;

/// EsTimeSpan reflected class wrapper around native esDT type
/// EsTimeSpan is naturally used in math expressions with EsDateTime objects
///
class ESCORE_CLASS ES_INTF_IMPL1(EsTimeSpan, EsReflectedClassIntf)

private:
  // internal initializer
  static EsBaseIntfPtr create(esDT dt);

public:
  EsTimeSpan();
  EsTimeSpan(const EsTimeSpan& other);
  EsTimeSpan(esDT dt);
  EsTimeSpan(const EsVariant& src);

  ES_DECL_REFLECTED_CLASS_BASE(EsTimeSpan)
  ES_DECL_ATTR_HANDLING_STD

  /// Time span string formatting services
  /// The following format specifiers are allowed after %:
  ///
  /// H - Number of Hours
  /// M - Number of Minutes
  /// S - Number of Seconds
  /// l - Number of Milliseconds
  /// D - Number of Days
  /// E - Number of Weeks
  /// % - The percent character
  /// Note that, for example, the number of hours in the description above
  ///  is not well defined: it can be either the total number of hours
  /// (for example, for a time span of 50 hours this would be 50) or just
  /// the hour part of the time span, which would be 2 in this case as 50 hours
  /// is equal to 2 days and 2 hours.
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(EsString, asString, cr_EsString);

  /// Ctor
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

  /// Abs service
  ES_DECL_REFLECTED_CONST_METHOD0(EsVariant, abs);

  /// Get clone of the current object
  ES_DECL_REFLECTED_CONST_METHOD0(EsVariant, clone);

  /// Reflected operators
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, compare, cr_EsVariant);
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, add, cr_EsVariant);
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, subtract, cr_EsVariant);
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, multiply, llong);
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, divide, llong);

  /// Reflected properties
  ///
  ES_DECL_PROPERTY(value, EsVariant)
  ES_DECL_PROPERTY(negative, bool)
  ES_DECL_PROPERTY(weeks, ulong)
  ES_DECL_PROPERTY(days, ulong)
  ES_DECL_PROPERTY(hours, ulong)
  ES_DECL_PROPERTY(minutes, ulong)
  ES_DECL_PROPERTY(seconds, ulong)
  ES_DECL_PROPERTY(milliseconds, ulong)
  ES_DECL_PROPERTY_RO(allWeeks, llong)
  ES_DECL_PROPERTY_RO(allDays, llong)
  ES_DECL_PROPERTY_RO(allHours, llong)
  ES_DECL_PROPERTY_RO(allMinutes, llong)
  ES_DECL_PROPERTY_RO(allSeconds, llong)
  ES_DECL_PROPERTY_RO(allMilliseconds, llong)

  /// Custom stream reader
  ES_DECL_REFLECTED_METHOD1(void, streamRead, cr_EsBaseIntfPtr);
  /// Custom stream writer
  ES_DECL_REFLECTED_METHOD1(void, streamWrite, cr_EsBaseIntfPtr);

  // operators
  operator esDT () const;
  EsTimeSpan& operator= (esDT src);
  bool operator== (const EsTimeSpan& other) const;
  bool operator< (const EsTimeSpan& other) const;
  bool operator<= (const EsTimeSpan& other) const { return operator<(other) || operator==(other); }
  bool operator> (const EsTimeSpan& other) const { return !operator<=(other); }
  bool operator>= (const EsTimeSpan& other) const { return operator==(other) || operator>(other); }
  friend ESCORE_FUNC( EsTimeSpan, operator+ (const EsTimeSpan& _1, const EsTimeSpan& _2) );
  friend ESCORE_FUNC( EsTimeSpan, operator+ (const EsTimeSpan& _1, llong _2) ); //< We may add llong, which is interpreted as milliseconds
  friend ESCORE_FUNC( EsTimeSpan, operator- (const EsTimeSpan& _1, const EsTimeSpan& _2) );
  friend ESCORE_FUNC( EsTimeSpan, operator- (const EsTimeSpan& _1, llong _2) );
  friend ESCORE_FUNC( EsTimeSpan, operator* (const EsTimeSpan& _1, llong _2) );
  friend ESCORE_FUNC( EsTimeSpan, operator/ (const EsTimeSpan& _1, llong _2) );

  // variant conversion helpers for Reflection
  static esDT fromVariant(const EsVariant& var);
  static EsVariant toVariant(esDT dt);

protected:
  void checkComposed() const;
  void checkDecomposed() const;
  long cmp(const EsTimeSpan* other) const;

private:
  // internal flags
  mutable ulong m_flags;
  // native storage
  mutable esDT m_dt;
  // decomposed parts of date time span
  mutable ulong m_days;
  mutable ulong m_hours;
  mutable ulong m_mins;
  mutable ulong m_secs;
  mutable ulong m_msecs;

  friend class EsDateTime;
};
ESCORE_FUNC( EsTimeSpan, operator+ (const EsTimeSpan& _1, const EsTimeSpan& _2) );
ESCORE_FUNC( EsTimeSpan, operator+ (const EsTimeSpan& _1, llong _2) ); //< We may add llong, which is interpreted as milliseconds
ESCORE_FUNC( EsTimeSpan, operator- (const EsTimeSpan& _1, const EsTimeSpan& _2) );
ESCORE_FUNC( EsTimeSpan, operator- (const EsTimeSpan& _1, llong _2) );
ESCORE_FUNC( EsTimeSpan, operator* (const EsTimeSpan& _1, llong _2) );
ESCORE_FUNC( EsTimeSpan, operator/ (const EsTimeSpan& _1, llong _2) );

/// EsDateTime reflected class wrapper around native esDT type
///
class ESCORE_CLASS ES_INTF_IMPL1(EsDateTime, EsReflectedClassIntf)

private:
  // internal initializer
  static EsBaseIntfPtr create(esDT dt);

public:
  EsDateTime();
  EsDateTime(const EsDateTime& other);
  EsDateTime(esDT dt);
  EsDateTime(const EsVariant& src);

  /// Validity check
  bool isOk() const;

  /// DateTime value composition
  /// Compose a EsDateTime object from separate components
  /// @param [in]    long  year
  /// @param [in]    ulong month
  /// @param [in]    ulong dayOfMonth
  /// @param [in]    ulong hour
  /// @param [in]    ulong minute
  /// @param [in]    ulong second
  /// @param [in]    ulong millisecond
  ///
  void compose(long year, ulong month, ulong dayOfMonth, ulong hour, ulong minute, ulong second, ulong millisecond);

  ES_DECL_REFLECTED_CLASS_BASE(EsDateTime)
  ES_DECL_ATTR_HANDLING_STD

  /// Date/time string formatting services
  ///
  /// Standard "asString" service, return date time string in ISO 8601 format
  /// using space as date and time parts separator
  ES_DECL_REFLECTED_CONST_METHOD0(EsString, asString);

  /// Get standard formatting date string
  ES_DECL_REFLECTED_CONST_METHOD0(EsString, asDateString);

  /// Get standard formatting time string
  ES_DECL_REFLECTED_CONST_METHOD0(EsString, asTimeString);

  /// Get clone of the current object
  ES_DECL_REFLECTED_CONST_METHOD0(EsVariant, clone);

  /// Date time formatting using format string.
  /// For format specifications see strftime documentation. Some, but not all C++
  /// formatting commands are supported. All standard formatting commands are
  /// supported.
  /// The following additional formatting flag is supported:
  /// l - Milliseconds
  /// The following non-standard command modifier is supported:
  /// '-' - do not prepend decimal output with zeroes
  ///
  /// @param [in]     - strftime-compatible formatting string
  /// @return         - formatted date time string
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(EsString, asString, cr_EsString);

  /// Date time formatting using format string and specific locale.
  /// For format specifications see strftime documentation. Some, but not all C++
  /// formatting commands are supported. All standard formatting commands are
  /// supported.
  /// The following additional formatting flag is supported:
  /// l - Milliseconds
  /// The following non-standard command modifier is supported:
  /// '-' - do not prepend decimal output with zeroes
  ///
  /// @param [in]     - requested locale name
  /// @param [in]     - strftime-compatible formatting string
  /// @return         - formatted date time string
  ///
  ES_DECL_REFLECTED_CONST_METHOD2(EsString, asString, cr_EsString, cr_EsString);

  /// Construction methods
  ///
  ES_DECL_REFLECTED_CLASS_METHOD0(EsBaseIntfPtr, reflectedNow);
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

  /// Reflected class services
  ///

  /// Return true if year is leap one, false otherwise
  ES_DECL_REFLECTED_CLASS_METHOD1(bool, isLeapYear, long);

  // Return number of days in month, given year and month. valid month range is [1..12]
  // 0 is returned otherwise.
  ES_DECL_REFLECTED_CLASS_METHOD2(ulong, daysInMonthGet, long, ulong);

  // Return days in year
  ES_DECL_REFLECTED_CLASS_METHOD1(ulong, daysInYearGet, long);

  // Return day of week, given year, month, dayOfMonth
  // Return 0 if input values are invalid. otherwise,
  // Return value from 1 (monday) to 7 (sunday)
  ES_DECL_REFLECTED_CLASS_METHOD3(ulong, dayOfWeekGet, long, ulong, ulong);

  /// Return true if daylight saving enabled
  ES_DECL_REFLECTED_CLASS_METHOD0(bool, isDstActive);

  /// Return UTC bias in the form of EsTimeSpan object
  ES_DECL_REFLECTED_CLASS_METHOD0(EsVariant, biasReflected)
  { return EsTimeSpan::toVariant( EsDateTime::bias() ); }

  /// Convert universal DateTime formatting sequence, (that is what embedded EsLocale
  /// implementation have in its storage),
  /// to the strftime - compatible format string
  /// @param [in] const EsString& - universal formatting string
  /// @return                     - strftime-compatible format string
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, universalFmtToStrftime, cr_EsString);

  // Reflected operators
  //
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, compare, cr_EsVariant);
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, add, cr_EsVariant);
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, subtract, cr_EsVariant);

  /// Custom stream reader
  ES_DECL_REFLECTED_METHOD1(void, streamRead, cr_EsBaseIntfPtr);
  /// Custom stream writer
  ES_DECL_REFLECTED_METHOD1(void, streamWrite, cr_EsBaseIntfPtr);

  // Reflected properties
  //
  ES_DECL_PROPERTY_RO(leapYear, bool)
  ES_DECL_PROPERTY_RO(daysInMonth, ulong)
  ES_DECL_PROPERTY_RO(daysInYear, ulong)
  ES_DECL_PROPERTY_RO(dayOfYear, ulong)
  ES_DECL_PROPERTY_RO(dayOfWeek, ulong)
  ES_DECL_PROPERTY(value, EsVariant)
  ES_DECL_PROPERTY(year, long)
  ES_DECL_PROPERTY(month, ulong)
  ES_DECL_PROPERTY(dayOfMonth, ulong)
  ES_DECL_PROPERTY(hours, ulong)
  ES_DECL_PROPERTY(minutes, ulong)
  ES_DECL_PROPERTY(seconds, ulong)
  ES_DECL_PROPERTY(milliseconds, ulong)

  /// Non-reflected stuff
  ///

  static EsDateTime now();

  /// Locale-specific date time formatting
  EsString asString(const std::locale& loc, const EsString& fmt) const;

  /// UTC bias
  static EsTimeSpan bias();

  /// Internal string formatter
  size_t strftime(const std::locale& loc, EsString::value_type* str, size_t maxsize, EsString::const_pointer fmt) const;

  /// operators
  operator esDT () const;
  EsDateTime& operator= (esDT src);
  bool operator== (const EsDateTime& other) const;
  bool operator< (const EsDateTime& other) const;
  bool operator<= (const EsDateTime& other) const { return operator<(other) || operator==(other); }
  bool operator> (const EsDateTime& other) const { return !operator<=(other); }
  bool operator>= (const EsDateTime& other) const { return operator==(other) || operator>(other); }
  const EsDateTime& operator+= (const EsDateTime& other);
  const EsDateTime& operator+= (llong other);
  const EsDateTime& operator-= (const EsDateTime& other);
  const EsDateTime& operator-= (llong other);

  friend ESCORE_FUNC( EsDateTime, operator+ (const EsDateTime& _1, const EsTimeSpan& _2) );
  friend ESCORE_FUNC( EsDateTime, operator+ (const EsDateTime& _1, llong _2) ); //< We may add llong, which is interpreted as milliseconds
  friend ESCORE_FUNC( EsDateTime, operator- (const EsDateTime& _1, const EsTimeSpan& _2) );
  friend ESCORE_FUNC( EsTimeSpan, operator- (const EsDateTime& _1, const EsDateTime& _2) );
  friend ESCORE_FUNC( EsDateTime, operator- (const EsDateTime& _1, llong _2) ); //< We may subtract llong, which is interpreted as milliseconds

  // Variant conversion helpers for Reflection
  static esDT fromVariant(const EsVariant& var);
  static EsVariant toVariant(esDT dt);

  // Empty date time value
  static const EsDateTime& null();

protected:
  // internal services
  //
  void checkDecomposed(bool doThrow = true) const;
  void checkComposed(bool doThrow = true) const;
  long cmp(const EsDateTime* other) const;

private:
  // native storage
  mutable esDT m_dt;
  // internal flags
  mutable ulong m_flags;
  // decomposed storage
  mutable long m_year;
  mutable ulong m_dayOfYear;
  mutable ulong m_month;
  mutable ulong m_dayOfMonth;
  mutable ulong m_hour;
  mutable ulong m_min;
  mutable ulong m_sec;
  mutable ulong m_msec;
};

ESCORE_FUNC( EsDateTime, operator+ (const EsDateTime& _1, const EsTimeSpan& _2) );
ESCORE_FUNC( EsDateTime, operator+ (const EsDateTime& _1, llong _2) );
ESCORE_FUNC( EsDateTime, operator- (const EsDateTime& _1, const EsTimeSpan& _2) );
ESCORE_FUNC( EsTimeSpan, operator- (const EsDateTime& _1, const EsDateTime& _2) );
ESCORE_FUNC( EsDateTime, operator- (const EsDateTime& _1, llong _2) );

#endif // _es_date_time_h_
