#ifndef _es_variant_h_
#define _es_variant_h_

/// Variant data type, variable or constant which type is determined at runtime.
///
class ESCORE_CLASS EsVariant
{
public:
	// Possible types, sorted in the order of their promotional conversion
	//
	enum Type
	{
		VAR_EMPTY,              // No value in the variant
		VAR_BOOL,               // Variant has Boolean value
		VAR_BYTE,               // Variant has Byte value
		VAR_CHAR,               // Variant has Character value
		VAR_UINT,               // Variant has unsigned integer value. Its conversion rate is smaller than INT, not like in C
		VAR_INT,                // Variant has integer value
		VAR_UINT64,             // Variant has unsigned 64-bit integer value. Its conversion rate is smaller than INT, not like in C
		VAR_INT64,              // Variant has 64-bit integer value
		VAR_DOUBLE,             // Variant has double value
		VAR_BIN_BUFFER,					// Variant has the binary buffer
		VAR_STRING,             // Variant has string value
		VAR_STRING_COLLECTION,  // Variant has string collection value
		VAR_POINTER,						// Variant has generic pointer
		VAR_OBJECT,							// Variant has an base interface of object
		VAR_VARIANT_COLLECTION	// Array of any type of elements, array of variants
	};

	// Tag that allows telling string constructor from the binary buffer constructor.
	//
	enum AcceptStringType
	{
		ACCEPT_STRING
	};

	// Tag that allows telling EsString-char constructor from the something-something-int constructor.
	//
	enum AcceptCharType
	{
		ACCEPT_CHAR
	};

  // Tag that allows telling pointer constructor from the integer-based|bool constructors
	//
	enum AcceptPointerType
	{
		ACCEPT_POINTER
	};

	// Type for holding the array of variants
	//
	typedef std::vector<EsVariant> Array;

public: // Constructor and destructor:

	// Default object constructor.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_EMPTY.
	//
	EsVariant() ES_NOTHROW;

	// Object constructor that creates an empty object of a given type.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_EMPTY.
	//
	explicit EsVariant(Type type);

	// Construct the value of type bool and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_BOOL.
	// The value is set to one given.
	//
	explicit EsVariant(bool n) ES_NOTHROW;

	// Construct the value of type long and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_INT.
	// The value is set to one given.
	//
	EsVariant(long n)	ES_NOTHROW;

	// Construct the value of type long long and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_INT64.
	// The value is set to one given.
	//
	EsVariant(long long n) ES_NOTHROW;

	// Construct the value of type unsigned long and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_UINT.
	// The value is set to one given.
	//
	EsVariant(unsigned long n) ES_NOTHROW;

	// Construct the value of type unsigned long long and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_UINT64.
	// The value is set to one given.
	//
	EsVariant(unsigned long long n) ES_NOTHROW;

	// Construct the value of type int and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_INT.
	// The value is set to one given.
	//
	EsVariant(int n) ES_NOTHROW;

	// Construct the value of type unsigned int and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_UINT.
	// The value is set to one given.
	//
	EsVariant(unsigned n)	ES_NOTHROW;

	// Construct the value of type esU8 with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_UINT.
	// The value is set to one given.
	//
	explicit EsVariant(esU8 b) ES_NOTHROW;

	// Construct the value of type char and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_CHAR.
	// The value is set to one given.
	//
	EsVariant(EsString::value_type c, AcceptCharType) ES_NOTHROW;

	// Construct the value of type double and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_DOUBLE.
	// The value is set to one given.
	//
	EsVariant(double f)	ES_NOTHROW;

	// Construct the value of type string and with the value specified
	// as the constant character pointer.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_STRING.
	// The value is set to one given.
	//
	EsVariant(EsString::const_pointer p, AcceptStringType);
	EsVariant(EsString::const_pointer p, unsigned len, AcceptStringType);

	// Construct the value of type byte string,
	// and with the value specified as the constant pointer with length.
	// Note this is the way to create a variant of type VAR_BIN_BUFFER
	// with the non-copy constructor. This is because the implementation
	// with EsString might be the same as one of EsBinBuffer.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_BIN_BUFFER.
	// The value is set to one given.
	//
	EsVariant(const esU8* p, unsigned len);

	// Construct the value of type binary buffer,
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_BIN_BUFFER.
	// The value is set to one given.
	//
	EsVariant(const EsBinBuffer& s);

	// Construct the value of type string and with the value specified
	// as EsString. Note that EsBinBuffer might have the same implementation
	// as EsString, and it would be not obvious that the constructor from
	// byte string creates the variant with type VAR_STRING.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_STRING.
	// The value is set to one given.
	//
	EsVariant(const EsString& s);

	// Construct the value of type string collection and with the value specified
	// as parameter.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_STRING_COLLECTION.
	// The value is set to one given.
	//
	EsVariant(const EsString::Array& v);

	// Construct the value of type variant collection and with the value specified
	// as parameter.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_VARIANT_COLLECTION.
	// The value is set to one given.
	//
	EsVariant(const EsVariant::Array& v);

	// Construct the value of type interface pointer from some other interface pointer
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_OBJECT.
	// The value is set to reference the object given.
	//
	template < typename OtherIntfT >
	EsVariant(const EsIntfPtr< OtherIntfT >& intf)	:
	m_type(VAR_OBJECT)
	{
		EsBaseIntf::Ptr p = intf;
		m_value.m_intf.m_ptr = p.get();
		m_value.m_intf.m_own = p.own();
		if(m_value.m_intf.m_ptr && m_value.m_intf.m_own)
			m_value.m_intf.m_ptr->incRef();
	}

	// Construct the value of type void pointer
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_POINTER.
	// The value is set to reference the object given.
	//
	explicit EsVariant(void* ptr, AcceptPointerType) ES_NOTHROW;

	// Construct the value from the copy. Initialize the object
	// with the attributes of the other object.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built.
	// Has the same attributes as the other object given.
	//
	EsVariant(const EsVariant& v);

	// destroy the variant object, reclaim memory if the value was allocated dynamically
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is destroyed.
	//
	~EsVariant() ES_NOTHROW;

public: // Services that work with type of the variant:
	// Get the type of the variant object.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The type of the variant is returned, see Type constants.
	//
	inline Type typeGet() const ES_NOTHROW
	{
		return m_type;
	}

	/// Tells whether the variant is of type VAR_EMPTY, means not initialized
	/// with any value.
	///
	/// PRECONDITION: None
	///
	/// POSTCONDITION: True is returned if the variant is empty.
	///
	bool isEmpty() const ES_NOTHROW;

  /// Check if variant is of boolean type.
  /// @return           - true, if variant is of boolean type,
  ///                   - false otherwise.
  ///
  inline bool isBool() const ES_NOTHROW
  {
    return VAR_BOOL == m_type;
  }

	/// Whether the variant is of numeric type, so the arithmetic
	/// operations can be performed.
	/// The following types are arithmetic: VAR_BOOL,
	/// VAR_CHAR, VAR_INT, VAR_UINT, and VAR_DOUBLE.
	/// Empty and string types are not numeric.
	///
	/// PRECONDITION: None
	///
	/// POSTCONDITION: True if the variant is of string type.
	///
	inline bool isNumeric() const ES_NOTHROW
	{
		return m_type > VAR_EMPTY && m_type <= VAR_DOUBLE;
	}

	/// Whether the variant can be indexed.
	///
	/// PRECONDITION: None
	///
	/// POSTCONDITION: VAR_STRING, VAR_BIN_BUFFER, VAR_STRING_COLLECTION, and VAR_VARIANT_COLLECTION
	/// will return true, The other types cannot be indexed.
	///
	bool isIndexed() const;

	/// Whether the variant is a collection, array.
	///
	/// PRECONDITION: None
	///
	/// POSTCONDITION: VAR_STRING_COLLECTION, and VAR_VARIANT_COLLECTION
	/// will return true, The other types cannot be indexed.
	///
	inline bool isCollection() const ES_NOTHROW
	{
		return m_type == VAR_STRING_COLLECTION ||
			m_type == VAR_VARIANT_COLLECTION;
	}

  /// Whether the variant is an explicit variant collection type.
  ///
  /// PRECONDITION: None
  ///
  /// POSTCONDITION: VAR_VARIANT_COLLECTION
  /// will return true
  ///
  inline bool isVariantCollection() const ES_NOTHROW
  {
    return m_type == VAR_VARIANT_COLLECTION;
  }

  /// Whether the variant is an explicit string collection type.
  ///
  /// PRECONDITION: None
  ///
  /// POSTCONDITION: VAR_STRING_COLLECITON
  /// will return true
  ///
  inline bool isStringCollection() const ES_NOTHROW
  {
    return m_type == VAR_STRING_COLLECTION;
  }

	/// Check whether the variant is a string.
	///
	/// PRECONDITION: None
	///
	/// POSTCONDITION: VAR_STRING
	/// will return true
	///
	inline bool isString() const ES_NOTHROW
	{
		return m_type == VAR_STRING;
	}

	/// Check whether the variant is a binary buffer
	///
	/// PRECONDITION: None
	///
	/// POSTCONDITION: VAR_BIN_BUFFER
	/// will return true
	///
	inline bool isBinBuffer() const ES_NOTHROW
	{
		return m_type == VAR_BIN_BUFFER;
	}

	// Get the count of the elements in the variant, if they can be indexed.
	//
	// PRECONDITION: isIndexed should be true, or the attempt to use this
	// operation will lead to exception.
	//
	// POSTCONDITION: The count of the elements in the collection is returned.
	//
	int countGet() const;

	// Set the count of the elements in the variant, if they can be indexed.
	//
	// PRECONDITION: isIndexed should be true, or the attempt to use this
	// operation will lead to exception.
	//
	// POSTCONDITION: The count of the elements in the collection is updated.
	//
	void countSet(int newCount);

	// Whether the variant has an interface reference.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: True if the variant is of type interface.
	//
	bool isObject() const ES_NOTHROW;

	/// Return true if variant is of generic pointer type
	inline bool isPointer() const ES_NOTHROW
	{
  	return m_type == VAR_POINTER;
	}

public: // Assignment operators:

	// Assignment operator that takes variable of type bool.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value of type VAR_BOOL is assigned.
	//
	EsVariant& operator=(bool b)
	{
		return doSetInt(b, VAR_BOOL);
	}


	// Assignment operator that takes variable of type byte.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value of type VAR_BYTE is assigned.
	//
	EsVariant& operator=(esU8 b)
	{
		return doSetInt(b, VAR_BYTE);
	}

	// Assignment operator that takes variable of type char.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value of type VAR_CHAR is assigned.
	//
	EsVariant& operator=(EsString::value_type c)
	{
#if defined(ES_USE_WCHAR)
# if 2 == ES_WCHAR_SIZE
		return doSetInt(static_cast<long long>( static_cast<unsigned short>(c) ), VAR_CHAR);
# elif 4 == ES_WCHAR_SIZE
    return doSetInt(static_cast<long long>( static_cast<unsigned long>(c) ), VAR_CHAR);
# else
#   error Unsupported|unknown ES_WCHAR_SIZE!
# endif
#else
		return doSetInt(static_cast<long long>( static_cast<unsigned char>(c) ), VAR_CHAR);
#endif
	}

	// Assignment operator that takes variable of type int.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value of type VAR_INT is assigned.
	//
	EsVariant& operator=(int n)
	{
		return doSetInt(n, VAR_INT);
	}

	// Assignment operator that takes variable of type unsigned int.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value of type VAR_UINT is assigned.
	//
	EsVariant& operator=(unsigned n)
	{
		return doSetInt(n, VAR_UINT);
	}

	// Assignment operator that takes variable of type long.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value of type VAR_INT is assigned.
	//
	EsVariant& operator=(long n)
	{
		return doSetInt(n, VAR_INT);
	}

	// Assignment operator that takes variable of type unsigned long.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value of type VAR_UINT is assigned.
	//
	EsVariant& operator=(unsigned long n)
	{
		return doSetInt(n, VAR_UINT);
	}

	// Assignment operator that takes variable of type long long.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value of type VAR_INT64 is assigned.
	//
	EsVariant& operator=(long long n)
	{
		return doSetInt(n, VAR_INT64);
	}

	// Assignment operator that takes variable of type unsigned long long.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value of type VAR_UINT is assigned.
	//
	EsVariant& operator=(unsigned long long n)
	{
		return doSetInt(n, VAR_UINT64);
	}

	// Assignment operator that takes variable of type double.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value of type VAR_DOUBLE is assigned.
	//
	EsVariant& operator=(double f);

	// Assignment operator that takes variable of type
	// pointer to the constant zero terminated string.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value of type VAR_STRING is assigned.
	//
	EsVariant& operator=(const EsString::value_type* p);

	// Assignment operator that takes variable of type string.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value of type VAR_STRING is assigned.
	//
	EsVariant& operator=(const EsString& s);

	// Assignment operator that takes variable of type string collection.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value of type VAR_STRING_COLLECTION is assigned.
	//
	EsVariant& operator=(const EsString::Array& s);

	// Assignment operator that takes variable of type variant vector.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value of type VAR_VARIANT_COLLECTION is assigned.
	//
	EsVariant& operator=(const EsVariant::Array& s);

	// Assignment operator that takes variable of type EsVariant.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value has the same type and value as the one given.
	//
	EsVariant& operator= (const EsVariant& v);

	// Assignment operator that takes variable of type EsBinBuffer
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value has type VAR_BIN_BUFFER, and value specified.
	//
	inline EsVariant& operator= (const EsBinBuffer& v)
	{
		assignBinBuffer(v);
		return *this;
	}

	// Assign the Binary buffer to the variant type
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value has type VAR_BIN_BUFFER, and value specified.
	//
	void assignBinBuffer(const EsBinBuffer& v);

	// Assign the bin buffer to the variant type.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value has type VAR_BIN_BUFFER, and value specified
	// with pointer and length.
	//
	void assign(const esU8* v, size_t len);

	// Assign the string to the variant type.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value has type VAR_STRING, and value specified
	// with pointer and length.
	//
	void assignString(const EsString::value_type* v, size_t len);

	// Assignment operator that takes variable of type EsBaseIntf.
	// Variant will take ref-counted ownership of the object
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value has the same type and value as the one given.
	//
	EsVariant& operator=(const EsBaseIntf::Ptr& v);

	// Assignment operator that takes variable of type OtherIntfT::Ptr.
	// Variant will take ref-counted ownership of the object
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value has the same type and value as the one given.
	//
	template < typename OtherIntfT >
	EsVariant& operator=(const EsIntfPtr< OtherIntfT >& v)
	{
		return operator= ( v.template request< EsBaseIntf >() );
	}

	// Assignment operator that takes variable of type void*.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value has the same type and value as the one given.
	//
	EsVariant& operator=(const void* ptr);

public: // Conversion services:

	// Interpret the variant value as type bool, if possible.
	// The type of the value should allow conversion
	// of it into boolean. It should either be boolean itself,
	// or numeric. If the value is numeric, nonzero would mean TRUE.
	// Also, string, byte string, and string collection, if empty,
	// will yield to False. If they are not empty, their conversion to
	// Long will be attempted as the result compared with zero.
	//
	// PRECONDITION: The conversion should be possible.
	// If the current value is of incompatible type,
	// bad conversion is thrown.
	//
	// POSTCONDITION: Boolean representation of the value is returned.
	//
	bool asBool(const std::locale& loc = EsLocale::locale()) const;

	// Interpret the variant value as byte, if possible.
	// The type of the value should fit within one byte.
	//
	// PRECONDITION: The conversion should be possible.
	// Bad conversion can be thrown in cases the type is incompatible
	// or the range is bad.
	//
	// POSTCONDITION: Byte representation of the value is returned.
	//
	esU8 asByte(const std::locale& loc = EsLocale::locale()) const;

	// Interpret the variant value as type EsString::value_type, if possible.
	// The type of the value should allow conversion
	// of it into EsString::value_type. It should either be VAR_CHAR itself,
	// or it should be numeric with the allowed range,
	// or it should be a string with the size 1.
	//
	// PRECONDITION: The conversion should be possible.
	// Bad conversion can be thrown in cases the type is incompatible
	// or the range is bad.
	//
	// POSTCONDITION: Char representation of the value is returned.
	//
	EsString::value_type asChar(const std::locale& loc = EsLocale::locale()) const;

	// Interpret the variant value as double word.
	// This service is like asInt or asUInt, but it will ignore
	// the sign and never throw an exception or overflow.
	//
	// PRECONDITION: The conversion should be possible.
	// Bad conversion can be thrown in cases the type is incompatible
	// or the range is bad.
	//
	// POSTCONDITION: Double word representation of the value is returned.
	//
	esU32 asInternalDWord(const std::locale& loc = EsLocale::locale()) const;

	// Interpret the variant value as quad word.
	// This service is like asLLong or asULLong, but it will ignore
	// the sign and never throw an exception or overflow.
	//
	// PRECONDITION: The conversion should be possible.
	// Bad conversion can be thrown in cases the type is incompatible
	// or the range is bad.
	//
	// POSTCONDITION: Quad word representation of the value is returned.
	//
	esU64 asInternalQWord(const std::locale& loc = EsLocale::locale()) const;

	// Interpret the variant value as integer type, if possible.
	// The type of the value should allow conversion
	// of it into integer. The numeric type has to fit within
	// the range of integer, and the string has to be the valid
	// string representation of integer.
	//
	// PRECONDITION: The conversion should be possible.
	// Bad conversion can be thrown in cases the type is incompatible
	// or the range is bad.
	//
	// POSTCONDITION: Integer representation of the value is returned.
	//
	inline int asInt(const std::locale& loc = EsLocale::locale()) const
	{
		//ES_COMPILE_TIME_ASSERT(sizeof(int) == sizeof(long), IntSizeEqLongSize); // Careful with architectures where sizes of int and long do not match
		return static_cast<int>( asLong(loc) );
	}

	// Interpret the variant value as unsigned integer type, if possible.
	// The type of the value should allow conversion
	// of it into unsigned integer. The numeric type has to fit within
	// the range of integer, and the string has to be the valid
	// string representation of unsigned integer.
	//
	// PRECONDITION: The conversion should be possible.
	// Bad conversion can be thrown in cases the type is incompatible
	// or the range is bad.
	//
	// POSTCONDITION: Unsigned integer representation of the value is returned.
	//
	inline unsigned asUInt(const std::locale& loc = EsLocale::locale()) const
	{
		//ES_COMPILE_TIME_ASSERT(sizeof(unsigned) == sizeof(unsigned long), UnsignedSizeEqUnsignedLongSize); // Careful with architectures where sizes of int and long do not match
		return static_cast<unsigned>( asULong(loc) );
	}

	// Interpret the variant value as long integer type, if possible.
	// The type of the value should allow conversion
	// of it into long integer. The numeric type has to fit within
	// the range of integer, and the string has to be the valid
	// string representation of long integer.
	//
	// PRECONDITION: The conversion should be possible.
	// Bad conversion can be thrown in cases the type is incompatible
	// or the range is bad.
	//
	// POSTCONDITION: Long integer representation of the value is returned.
	//
	long asLong(const std::locale& loc = EsLocale::locale()) const;

	// Interpret the variant value as unsigned long integer type, if possible.
	// The type of the value should allow conversion
	// of it into unsigned long integer. The numeric type has to fit within
	// the range of integer, and the string has to be the valid
	// string representation of unsigned long integer.
	//
	// PRECONDITION: The conversion should be possible.
	// Bad conversion can be thrown in cases the type is incompatible
	// or the range is bad.
	//
	// POSTCONDITION: Unsigned long integer representation of the value is returned.
	//
	unsigned long asULong(const std::locale& loc = EsLocale::locale()) const;

	// Interpret the variant value as long long integer type, if possible.
	// The type of the value should allow conversion
	// of it into long long integer. The numeric type has to fit within
	// the range of integer, and the string has to be the valid
	// string representation of long long integer.
	//
	// PRECONDITION: The conversion should be possible.
	// Bad conversion can be thrown in cases the type is incompatible
	// or the range is bad.
	//
	// POSTCONDITION: Long long integer representation of the value is returned.
	//
	long long asLLong(const std::locale& loc = EsLocale::locale()) const;

	// Interpret the variant value as unsigned long long integer type, if possible.
	// The type of the value should allow conversion
	// of it into unsigned long long integer. The numeric type has to fit within
	// the range of integer, and the string has to be the valid
	// string representation of unsigned long long integer.
	//
	// PRECONDITION: The conversion should be possible.
	// Bad conversion can be thrown in cases the type is incompatible
	// or the range is bad.
	//
	// POSTCONDITION: Unsigned long long integer representation of the value is returned.
	//
	unsigned long long asULLong(const std::locale& loc = EsLocale::locale()) const;

	// Interpret the variant value as double precision floating point, if possible.
	// The type of the value should allow conversion
	// of it into double precision floating point.If this is the string,
	// the string has to be the valid string representation of double precision number.
	//
	// PRECONDITION: The conversion should be possible.
	// Bad conversion can be thrown in cases the type is incompatible
	// or the range is bad.
	//
	// POSTCONDITION: The double precision number representation of the value is returned.
	//
	double asDouble(const std::locale& loc = EsLocale::locale()) const;

	// Interpret the variant value as byte string, if possible.
	// The only value that cannot be interpreted as byte string is an empty value.
	//
	// PRECONDITION: If the value of the variant is not initialized,
	// the no value exception is thrown.
	//
	// POSTCONDITION: Byte string representation of the value is returned.
	//
	EsBinBuffer asBinBuffer() const;

	// Interpret the variant value as string, if possible.
	// The only value that cannot be interpreted as string is an empty value.
	// If the variant is an object, its AsString method is called.
	// For Boolean value, its string representations are numeric: 1 or 0.
	//
	// PRECONDITION: If the value of the variant is not initialized,
	// the no value exception is thrown.
	//
	// POSTCONDITION: String representation of the value is returned.
	//
	EsString asString(const std::locale& loc = EsLocale::locale()) const;

	// Interpret the variant value as string, if possible, using mask that specifies the conversions to make.
	// The mask is combination of EsUtilities::StrMasks.
	// The only value that cannot be interpreted as string is an empty value.
	// If the variant is an object, its AsString method is called.
	// For Boolean value, its string representations are numeric: 1 or 0.
	//
	// PRECONDITION: If the value of the variant is not initialized,
	// the no value exception is thrown.
	//
	// POSTCONDITION: String representation of the value is returned.
	//
	EsString asString(unsigned mask, const std::locale& loc = EsLocale::locale()) const;

	// Interpret the variant value as a string with C escapes, if possible.
	// The only value that cannot be interpreted as string is an empty value.
	// If the variant is an object, its AsString method is called, then
	// converted to escaped string.
	// For Boolean value, its string representations are numeric: 1 or 0.
	//
	// PRECONDITION: If the value of the variant is not initialized,
	// the no value exception is thrown.
	//
	// POSTCONDITION: Escaped string representation of the value is returned.
	//
	EsString asEscapedString(const std::locale& loc = EsLocale::locale()) const;

	// Interpret the variant value as string collection, if possible.
	// The string collection returns directly.
	// The only value that cannot be interpreted as string is an empty value.
	// For the rest, AsString is attempted and the resulting collection
	// will have only one string in it.
	//
	// PRECONDITION: If the value of the variant is not initialized,
	// the no value exception is thrown.
	//
	// POSTCONDITION: String collection representation of the value is returned.
	//
	EsString::Array asStringCollection(const std::locale& loc = EsLocale::locale()) const;

	// Interpret the variant value as variant collection, if possible.
	// The variant collection returns directly.
	// The only value that cannot be interpreted as variant is an empty value.
	// For the rest, the resulting collection will have only one element.
	//
	// PRECONDITION: If the value of the variant is not initialized,
	// the no value exception is thrown.
	//
	// POSTCONDITION: Variant collection representation of the value is returned.
	//
	EsVariant::Array asVariantCollection() const;

	// Interpret the variant value as object reference, if possible.
	// The only value that can be interpreted as object is an object itself.
	//
	// PRECONDITION: An exception is thrown in case the value is not of type object.
	//
	// POSTCONDITION: Object reference is returned.
	//
	EsBaseIntf::Ptr asObject();

	// Interpret the variant value as object reference, if possible.
	// The only value that can be interpreted as object is an object itself.
	//
	// PRECONDITION: An exception is thrown in case the value is not of type object.
	//
	// POSTCONDITION: Object reference is returned.
	//
	inline EsBaseIntf::Ptr asObject() const
	{
		return const_cast<EsVariant*>(this)->asObject();
	}

	// Interpret the variant value as an existing non-NULL object reference, if possible.
	// The only value that can be interpreted as object is an object itself.
	//
	// PRECONDITION: An exception is thrown in case the value is not of type object.
	// If the object is NULL, a no value exception is thrown.
	//
	// POSTCONDITION: Object reference is returned.
	//
	EsBaseIntf::Ptr asExistingObject();

	// Interpret the variant value as an existing non-NULL constant object reference, if possible.
	// The only value that can be interpreted as object is an object itself.
	//
	// PRECONDITION: An exception is thrown in case the value is not of type object.
	// If the object is NULL, a no value exception is thrown.
	//
	// POSTCONDITION: Constant object reference is returned.
	//
	EsBaseIntf::Ptr asExistingObject() const;

	// Interpret the variant value as generic pointer, if possible.
	//
	// PRECONDITION: An exception is thrown in case the value is not of type object.
	//
	// POSTCONDITION: Object reference is returned.
	//
	void* asPointer();

	// Interpret the variant value as generic pointer, if possible.
	//
	// PRECONDITION: An exception is thrown in case the value is not of type object.
	//
	// POSTCONDITION: Object reference is returned.
	//
	inline void* asPointer() const
	{
		return const_cast<EsVariant*>(this)->asPointer();
	}

	// Discard the value of the variant type.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: Type of the variant becomes VAR_EMPTY.
	//
	void setEmpty() ES_NOTHROW;

	// Discard the value of the variant type, and if it is an object, delete it.
	// This corresponds to a concept of owned object.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The variant is set to empty, and if it was an interface|object, it is deleted.
	//
	void setEmptyWithObjectDelete();

	// Discard the value of the variant type, and set the null,
	// empty or zero value of this given type.
	// The null value will depend on the given type.
	// For string it is an empty string, and for an integer it is zero.
	// If no parameter is given, the type is preserved while its value becomes NULL.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: Type of the variant becomes one given, with null value.
	//
	void setToNull(Type type = (Type)-1);

	// Efficiently swap the value with the given value.
	//
	// PRECONDITION: None.
	//
	// POSTCONDITION: The values are swapped.
	//
	void swap(EsVariant& other) ES_NOTHROW;

	// Fast method that moves the value to another variant, and sets the other variant type to Empty.
	// This is possible only because variant values can always be moved.
	//
	// PRECONDITION: None.
	//
	// POSTCONDITION: The value is moved to this variant, other variant is set to empty.
	//
	void move(EsVariant& other) ES_NOTHROW;

	// Return this object with the power of the object given.
	// The power type returned is always esD.
	//
	// PRECONDITION: The type should be compatible with the power operation.
	// The compatible types are those that can be cast to esD.
	// If not, the bad conversion is thrown. In case any of the values
	// are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: Powered esD value returned.
	//
	EsVariant pow(const EsVariant& val, const std::locale& loc = EsLocale::locale()) const;

	// Get element by index.
	// One can check isIndexed property to know if the variant can be
	// indexed. Also there is a countGet, which is callable only for
	// IsIndexed, and will return the number of items in the variant.
	//
	// PRECONDITION: The type should allow subscripting,
	// such as VAR_STRING, VAR_BIN_BUFFER or VAR_STRING_COLLECTION.
	// If an item is an object, it shall have a reflected service with the name Item.
	// Otherwise the conversion exception is thrown.
	//
	// POSTCONDITION: The indexed item is returned.
	// Its type depends on the type of the variant.
	//
	EsVariant itemGet(const EsVariant& index) const;

	// Set element by index.
	// One can check isIndexed property to know if the variant can be
	// indexed. Also there is a countGet, which is callable only for
	// isIndexed, and will return the number of items in the variant.
	//
	// PRECONDITION: The type should allow subscripting,
	// such as VAR_STRING, VAR_BIN_BUFFER or VAR_STRING_COLLECTION.
	// If an item is an object, it shall have a reflected service with the name itemSet.
	// Otherwise the conversion exception is thrown.
	//
	// POSTCONDITION: The indexed item is set to one given.
	//
	void itemSet(const EsVariant& index, const EsVariant& value, const std::locale& loc = EsLocale::locale());

  // Remove an element by its index
	// One can check isIndexed property to know if the variant can be
	// indexed. Also there is a countGet, which is callable only for
	// isIndexed, and will return the number of items in the variant.
	//
	// PRECONDITION: The type should allow subscripting,
	// such as VAR_STRING, VAR_BIN_BUFFER or VAR_STRING_COLLECTION.
	// If an item is an object, it shall have a reflected service with the name itemDelete.
	// Otherwise the conversion exception is thrown.
	//
	// POSTCONDITION: The indexed item is set to one given.
	//
	void itemDelete(const EsVariant& index);

	// Returns true if a given item is in the variant.
	// If the variant is a collection of any kind,
	// true will mean the parameter is contained it in.
	// If the variant is a simple variant, 'has' means equality.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: True will mean the value signified by a given variant
	// is within this variant value.
	//
	bool has(const EsVariant&) const;

	// Add a given variant as a whole to the collection.
	// This call is different from operator+=, as it does not
	// unroll the collection items in case the given parameter is a collection.
	//
	// PRECONDITION: The type of the variant shall be VARIANT_COLLECTION,
	// there is a debug check.
	//
	// POSTCONDITION: A variant is added to collection. Reference to variant itself is returned
	//
	EsVariant& addToVariantCollection(const EsVariant& v, bool toFront = false);

	// Adjust a given index to standards of indexing in script
	// So the negative index will mean counting from the end of the array.
	//
	// PRECONDITION: The index shall be in range -count to count - 1,
	// where count is a signed integer. Otherwise an exception is thrown.
	//
	// POSTCONDITION: An index is adjusted to denote the real array element.
	//
	static void indexAdjust(int& index, unsigned count);

	// Adjust a given slice to standards of slicing in script.
	// So the negative index will mean counting from the end of the array.
	// A negative slice will mean no elements.
	//
	// PRECONDITION: Slice is always adjusted correctly.
	//
	// POSTCONDITION: A slice is adjusted, so it denotes the real array element range.
	//
	static int sliceAdjust(int &from, int &to, unsigned count);

	// Return the slice of values for types that support subscrips.
	// One can check IsIndexed property to know if the variant can be
	// indexed. Also there is a countGet, which is callable only for
	// isIndexed, and will return the number of items in the variant.
	//
	// PRECONDITION: The type should allow subscripting,
	// such as VAR_STRING, VAR_BIN_BUFFER or VAR_STRING_COLLECTION.
	// If an item is an object, it shall have a reflected service with the name itemGet.
	// Otherwise the conversion exception is thrown.
	//
	// POSTCONDITION: Result variant has the same type as this,
	// but it has only a given subrange of elements.
	//
	EsVariant sliceGet(int from, int to) const;

	/// Set the slice of values for types that support subscrips.
	/// Shrink or grow the number of items if necessary.
	/// One can check IsIndexed property to know if the variant can be
	/// indexed. Also there is a GetCount, which is callable only for
	/// IsIndexed, and will return the number of items in the variant.
	///
	/// PRECONDITION: The type should allow subscripting,
	/// such as VAR_STRING, VAR_BIN_BUFFER or VAR_STRING_COLLECTION.
	/// If an item is an object, it shall have a reflected service with the name SetItem.
	/// Otherwise the conversion exception is thrown.
	///
	/// POSTCONDITION: Result variant has a given slice of items set to new values.
	/// In case the number of values is lesser or bigger than the target slice,
	/// the number of items will change.
	///
	//void sliceSet(int from, int to, const EsVariant& values);

  /// Generic algorithms
  ///

  /// Value|member look-up
  /// Return index of member if found, or null, if not found
  ///
  EsVariant find(const EsVariant& var) const;
	EsVariant findFirstOf(const EsVariant& var) const;
	EsVariant findLastOf(const EsVariant& var) const;

	/// Replace part of sequence, with another one, starting from specified offset.
  /// If count is not 0, replace cnt elements from the sequence with var. Otherwise,
  /// var length is used to find how many source elements to be replaced.
  ///
	void replace(const EsVariant& var, ulong offs, ulong cnt = 0);

	/// Sort elements collection
	void sortAscending();
	void sortDescending();

  /// Reverse order of elements in collection
	void reverse();

  /// Operators
  ///
  inline EsVariant operator[](const EsVariant& idx) const { return itemGet(idx); }

	// Equality operator. Standard conversions apply.
	//
	// PRECONDITION: In case any of the values are not initialized,
	// no value exception is thrown.
	//
	// POSTCONDITION: True if the values of operands are logically equal.
	//
	bool operator==(const EsVariant&) const;

	// Inequality operator. Standard conversions apply.
	//
	// PRECONDITION: In case any of the values are not initialized,
	// no value exception is thrown.
	//
	// POSTCONDITION: True if the values of operands are not logically equal.
	//
	bool operator!=(const EsVariant& other) const
	{
		return !operator==(other);
	}

	// Less-than operator. Standard conversions apply.
	//
	// PRECONDITION: In case any of the values are not initialized,
	// no value exception is thrown.
	//
	// POSTCONDITION: True if the left value is smaller than the right value.
	//
	bool operator<(const EsVariant&) const;

	// Greater-than operator. Standard conversions apply.
	//
	// PRECONDITION: In case any of the values are not initialized,
	// no value exception is thrown.
	//
	// POSTCONDITION: True if the left value is greater than the right value.
	//
	bool operator>(const EsVariant&) const;

	// Less-or-equal-than operator. Standard conversions apply.
	//
	// PRECONDITION: In case any of the values are not initialized,
	// no value exception is thrown.
	//
	// POSTCONDITION: True if the left value is smaller or equal than the right value.
	//
	bool operator<=(const EsVariant& v) const
	{
		return !operator>(v);
	}

	// Greater-or-equal-than operator. Standard conversions apply.
	//
	// PRECONDITION: In case any of the values are not initialized,
	// no value exception is thrown.
	//
	// POSTCONDITION: True if the left value is bigger or equal than the right value.
	//
	bool operator>=(const EsVariant& v) const
	{
		return !operator<(v);
	}

	// Operator OR. Standard conversions apply.
	//
	// Note that there is no difference between logical OR and bitwise OR.
	// Logical or is applied for bool operands, and the resulting value is bool.
	// For all the other numeric values the bitwise OR is performed.
	//
	// PRECONDITION: The operand types should be convertible to esBL or numeric.
	// Otherwise bad conversion is raised.
	// In case any of the values are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: In case of both Boolean operands, return true if either one is true.
	// In case of the numeric values, do bitwise OR.
	//
	EsVariant operator|(const EsVariant&) const;

	// Operator AND. Standard conversions apply.
	// Note that there is no difference between logical AND and bitwise AND.
	// Logical and is applied for bool operands, the resulting value is bool.
	// For all the other numeric values bitwise and is performed.
	//
	// PRECONDITION: The operand types should be convertible to esBL or numeric.
	// Otherwise bad conversion is raised.
	// In case any of the values are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: In case of both boolean operands, return true if both are true.
	// In case of the numeric values, do bitwise AND.
	//
	EsVariant operator&(const EsVariant&) const;

	// Operator XOR. Standard conversions apply.
	// Note that there is no difference between logical XOR and bitwise XOR.
	// Logical XOR is applied for bool operands, the resulting value is bool.
	// For all the other numeric values bitwise XOR is performed.
	//
	// PRECONDITION: The operand types should be convertible to esBL or numeric.
	// Otherwise bad conversion is raised.
	// In case any of the values are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: In case of both boolean operands, return true if both
	// have the same value. In case of the numeric values, do bitwise XOR.
	//
	EsVariant operator^(const EsVariant&) const;

	// Unary bitwise negation operator.
	//
	// PRECONDITION: The type should be convertible to either bool
	// or unsigned integer. Otherwise bad conversion is raised.
	// In case any of the values are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: The result returned, NOT operator on this.
	//
	EsVariant operator~() const;

	// Unary logical negation operator.
	//
	// PRECONDITION: The type should be convertible to either bool
	// or unsigned integer. Otherwise bad conversion is raised.
	// In case any of the values are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: The result returned, NOT operator on this.
	//
	EsVariant operator!() const;

	// Unary operator minus.
	// Please note that the unary operator plus is not defined for EsVariant.
	//
	// PRECONDITION: The type should be compatible with the operation.
	// It should be arithmetic. Otherwise the conversion exception is reported.
	// In case any of the values are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: The result returned, minus operator on this.
	//
	EsVariant operator-() const;

	// Binary operator plus. The interpretation depends on the context.
	//
	// First the conversion is applied according to the MDL rules.
	// Preconditions apply for the conversion. If the converted values
	// are of type TYPE_STRING, then the strings are concatenated.
	// If the converted values are of the numeric type, the values are added.
	// The result value is returned, and this object is not changed.
	//
	// PRECONDITION: The type should be compatible with the operation.
	// Otherwise the conversion exception is thrown. In case any of
	// the values are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: The result returned, a result of the operator plus.
	//
	EsVariant operator+(const EsVariant&) const;

	// Binary operator minus. Applicable for numerics or sets only.
	//
	// First the conversion is applied according to the MDL rules.
	// Preconditions apply for the conversion.
	// The converted values should be of numeric type.
	//
	// PRECONDITION: The type should be compatible with the operation.
	// Otherwise the conversion exception is thrown. In case any of
	// the values are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: The result returned, a result of the operator minus.
	//
	EsVariant operator-(const EsVariant&) const;

	// Binary multiplication operator. Applicable for numerics or sets only.
	//
	// First the conversion is applied according to the MDL rules.
	// Preconditions apply for the conversion.
	// The converted values should be of numeric type.
	//
	// PRECONDITION: The type should be compatible with the operation.
	// Otherwise the conversion exception is thrown. In case any of
	// the values are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: The result returned, a result of the multiplication operator.
	//
	EsVariant operator*(const EsVariant&) const;

	// Binary division operator. Applicable for numerics only.
	//
	// First the conversion is applied according to the MDL rules.
	// Preconditions apply for the conversion.
	// The converted values should be of numeric type.
	//
	// PRECONDITION: The type should be compatible with the operation,
	// which means to be convertible to numeric type.
	// Otherwise the conversion exception is thrown. In case any of
	// the values are not initialized, no value exception is thrown.
	// If the second argument is zero, division by zero is thrown.
	//
	// POSTCONDITION: The result returned, the result of the division operator.
	//
	EsVariant operator/(const EsVariant&) const;

	// Binary modulus operator. Applicable for numerics only.
	// Modulus produces a reminder value from the division operator if both
	// arguments are positive.
	//
	// First the conversion is applied according to the MDL rules.
	// Preconditions apply for the conversion.
	// The converted values should be of numeric type.
	//
	// PRECONDITION: The type should be compatible with the operation
	// (be numeric). Otherwise the conversion exception is thrown. In case any of
	// the values are not initialized, no value exception is thrown.
	// If the second argument is zero, division by zero is thrown.
	//
	// POSTCONDITION: The result returned, a result of the modulus operator.
	//
	EsVariant operator%(const EsVariant&) const;

	// Bitwise left shift operator. Standard conversions apply.
	//
	// PRECONDITION: The type should be compatible with the operation,
	// which means to be numeric.
	// Otherwise the conversion exception is thrown. In case any of
	// the values are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: Bitwise left shift is performed.
	//
	EsVariant operator<<(const EsVariant&) const;

	// Bitwise right shift operator. Standard conversions apply.
	//
	// PRECONDITION: The type should be compatible with the operation,
	// which means to be numeric.
	// Otherwise the conversion exception is thrown. In case any of
	// the values are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: Bitwise right shift is performed.
	//
	EsVariant operator>>(const EsVariant&) const;

	// Prefix increment operator.
	// It never attempts to change the type of this variant.
	//
	// PRECONDITION: The type should allow incrementation,
	// which means it is numeric. Otherwise the exception is thrown.
	//
	// POSTCONDITION: The value is incremented, returned afterwards.
	//
	EsVariant& operator++();

	// Prefix decrement operator.
	// It never attempts to change the type of this variant.
	//
	// PRECONDITION: The type should allow incrementation,
	// which means it is numeric. Otherwise the exception is thrown.
	//
	// POSTCONDITION: The value is incremented, returned afterwards.
	//
	EsVariant& operator--();

	// Binary operator increment by value.
	//
	// PRECONDITION: The type should be compatible with the operation.
	// Otherwise the conversion exception is thrown. In case any of
	// the values are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: The value is incremented, this returned.
	//
	EsVariant& operator+=(const EsVariant&);

	// Binary operator decrement by value.
	//
	// PRECONDITION: The type should be compatible with the operation.
	// Otherwise the conversion exception is thrown. In case any of
	// the values are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: The value is decremented, this returned.
	//
	EsVariant& operator-=(const EsVariant&);

	// Binary operator multiply by value.
	//
	// PRECONDITION: The type should be compatible with the operation.
	// Otherwise the conversion exception is thrown. In case any of
	// the values are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: The value is multiplied, this returned.
	//
	EsVariant& operator*=(const EsVariant&);

	// Binary operator divide by value.
	//
	// PRECONDITION: The type should be compatible with the operation.
	// Otherwise the conversion exception is thrown. In case any of
	// the values are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: The value is divided, this returned.
	//
	EsVariant& operator/=(const EsVariant&);

	EsVariant& operator%=(const EsVariant&);
	EsVariant& operator>>=(const EsVariant&);
	EsVariant& operator<<=(const EsVariant&);
	EsVariant& operator|=(const EsVariant&);
	EsVariant& operator&=(const EsVariant&);
	EsVariant& operator^=(const EsVariant&);

#ifdef ES_MODERN_CPP
  /// Constructor with move semantics
  EsVariant(EsVariant&& other) ES_NOTHROW;

  /// Move assignment
  EsVariant& operator=(EsVariant&& other) ES_NOTHROW;
#endif

  /// Return contained type kind string. In case of non-null object, return object type
  /// Otherwise, return an empty string
  ///
  EsString kindGet() const;

  /// Object-specific proxy services
  ///

  /// Return true if variant is object of specified type
  bool isKindOf(const EsString& type) const;

  /// Return a type name of contained object
  EsString typeNameGet() const;

  /// Metaclass info proxies
  bool hasMethod(const EsString& mangledName) const;
  bool hasMethod(const EsString& name, ulong paramsCnt) const;
  bool hasProperty(const EsString& name) const;

  /// Try to cast contents implicitly to reflected object, and call its method
  EsVariant call(const EsString& method);
  EsVariant call(const EsString& method, const EsVariant& arg0);
  EsVariant call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1);
  EsVariant call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2);
  EsVariant call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3);
  EsVariant call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4);
  EsVariant call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5);
  EsVariant classCall(const EsString& method);
  EsVariant classCall(const EsString& method, const EsVariant& arg0);
  EsVariant classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1);
  EsVariant classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2);
  EsVariant classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3);
  EsVariant classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4);
  EsVariant classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5);

  /// Deal with variant container constness
  EsVariant call(const EsString& method) const;
  EsVariant call(const EsString& method, const EsVariant& arg0) const;
  EsVariant call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1) const;
  EsVariant call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2) const;
  EsVariant call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3) const;
  EsVariant call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4) const;
  EsVariant call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5) const;
  EsVariant classCall(const EsString& method) const;
  EsVariant classCall(const EsString& method, const EsVariant& arg0) const;
  EsVariant classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1) const;
  EsVariant classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2) const;
  EsVariant classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3) const;
  EsVariant classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4) const;
  EsVariant classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5) const;

  /// Common service shot-cuts
  EsVariant clone() const;

  /// Property access proxies
  EsVariant propertyGet(const EsString& property) const;
  void propertySet(const EsString& property, const EsVariant& val);

  /// Deal with constness
  void propertySet(const EsString& property, const EsVariant& val) const;

  /// Field access proxy
  EsVariant fieldGet(const EsString& field) const;

private: //< Services:
	// Internal assignment service that constructs integer-based variant value
	// with the particular type given.
	//
	// PRECONDITION: type has to be of generic integer type. Otherwise the
	// behavior is undefined.
	//
	// POSTCONDITION: The variant is initialized with the value and type given.
	//
	EsVariant& doSetInt(long long value, Type type) ES_NOTHROW;

public: //< Semi-public services that has to be used carefully:
	// Interpret the internals of the variant as bool.
	//
	// PRECONDITION: the type is VAR_BOOL, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The value of bool is returned.
	//
	inline bool doInterpretAsBool() const ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_BOOL);
		return m_value.m_llong != 0;
	}

	// Interpret the internals of the variant as byte.
	//
	// PRECONDITION: the type is VAR_BOOL, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The value of byte is returned.
	//
	inline esU8 doInterpretAsByte() const ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_BYTE);
		return static_cast<esU8>(m_value.m_ullong);
	}

	// Interpret the internals of the variant as UINT.
	//
	// PRECONDITION: the type is VAR_UINT, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The value of UINT is returned.
	//
	inline EsString::value_type doInterpretAsChar() const ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_CHAR);
		return static_cast<EsString::value_type>(m_value.m_ullong);
	}

	// Interpret the internals of the variant as INT.
	//
	// PRECONDITION: the type is VAR_INT, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The value of INT is returned.
	//
	inline int doInterpretAsInt() const ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_INT);
		return static_cast<int>(m_value.m_llong);
	}

	// Interpret the internals of the variant as UINT.
	//
	// PRECONDITION: the type is VAR_UINT, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The value of UINT is returned.
	//
	inline unsigned doInterpretAsUInt() const ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_UINT);
		return static_cast<unsigned>(m_value.m_ullong);
	}

	// Interpret the internals of the variant as standard string.
	//
	// PRECONDITION: the type is VAR_STRING, otherwise the behavior
	// is undefined. The non-UNICODE version has a flexibility that
	// VAR_BIN_BUFFER is also allowed. The debugger version has the assert operator.
	//
	// POSTCONDITION: The reference to the variant buffer as it was a string
	// is returned.
	//
	inline EsString& doInterpretAsString() ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_STRING);
		return *reinterpret_cast<EsString*>(&m_value);
	}

	// Interpret the internals of the variant as constant standard string.
	//
	// PRECONDITION: the type is VAR_STRING, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The reference to the variant buffer as it was a
	// constant string is returned.
	//
	inline const EsString& doInterpretAsString() const ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_STRING);
		return *reinterpret_cast<const EsString*>(&m_value);
	}

	// Interpret the internals of the variant as byte string.
	//
	// PRECONDITION: the type is VAR_BIN_BUFFER, otherwise the behavior
	// is undefined. The non-UNICODE version has a flexibility that
	// VAR_STRING is also allowed. The debugger version has the assert operator.
	//
	// POSTCONDITION: The reference to the variant buffer as it was a string
	// is returned.
	//
	inline EsBinBuffer& doInterpretAsBinBuffer() ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_BIN_BUFFER);
		return *reinterpret_cast<EsBinBuffer*>(&m_value);
	}

	// Interpret the internals of the variant as constant byte string.
	//
	// PRECONDITION: the type is VAR_BIN_BUFFER, otherwise the behavior
	// is undefined. The non-UNICODE version has a flexibility that
	// VAR_STRING is also allowed. The debugger version has the assert operator.
	//
	// POSTCONDITION: The reference to the variant buffer as it was a constant string
	// is returned.
	//
	inline const EsBinBuffer& doInterpretAsBinBuffer() const ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_BIN_BUFFER);
		return *reinterpret_cast<const EsBinBuffer*>(&m_value);
	}

	// Interpret the internals of the variant as string collection.
	//
	// PRECONDITION: the type is VAR_STRING_COLLECTION, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The reference to the variant buffer as it was a string collection
	// is returned.
	//
	inline EsString::Array& doInterpretAsStringCollection() ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_STRING_COLLECTION);
		return *reinterpret_cast<EsString::Array*>(&m_value);
	}

	// Interpret the internals of the variant as constant string collection.
	//
	// PRECONDITION: the type is VAR_STRING_COLLECTION, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The reference to the variant buffer as it was a
	// constant string collection is returned.
	//
	inline const EsString::Array& doInterpretAsStringCollection() const ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_STRING_COLLECTION);
		return *reinterpret_cast<const EsString::Array*>(&m_value);
	}

	// Interpret the internals of the variant as variant collection.
	//
	// PRECONDITION: the type is VAR_VARIANT_COLLECTION, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The reference to the variant buffer as it was a
	// variant collection is returned.
	//
	inline EsVariant::Array& doInterpretAsVariantCollection() ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_VARIANT_COLLECTION);
		return *reinterpret_cast<EsVariant::Array*>(&m_value);
	}

	// Interpret the internals of the variant as constant variant collection.
	//
	// PRECONDITION: the type is VAR_VARIANT_COLLECTION, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The reference to the variant buffer as it was a
	// constant variant collection is returned.
	//
	inline const EsVariant::Array& doInterpretAsVariantCollection() const ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_VARIANT_COLLECTION);
		return *reinterpret_cast<const EsVariant::Array*>(&m_value);
	}

	// Interpret the internals of the variant as an interface.
	//
	// PRECONDITION: the type is VAR_OBJECT, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The incRef-ed interface smartptr is returned.
	// only if variant owns the interface
	//
	inline EsBaseIntf::Ptr doInterpretAsObject() ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_OBJECT);
		return EsBaseIntf::Ptr(m_value.m_intf.m_ptr, m_value.m_intf.m_own, m_value.m_intf.m_own);
	}

	inline EsBaseIntf::Ptr doInterpretAsObject() const ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_OBJECT);
		return EsBaseIntf::Ptr(m_value.m_intf.m_ptr, m_value.m_intf.m_own, m_value.m_intf.m_own);
	}

	inline void doAssignToEmpty(bool b) ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		m_type = VAR_BOOL;
		m_value.m_llong = b;
	}

	inline void doAssignToEmpty(esU8 b) ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		m_type = VAR_BYTE;
		m_value.m_ullong = b;
	}

	inline void doAssignToEmpty(EsString::value_type c) ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		m_type = VAR_CHAR;
#if defined(ES_USE_WCHAR)
# if 2 == ES_WCHAR_SIZE
		m_value.m_ullong = static_cast<unsigned long long>( static_cast<unsigned short>(c) );
# elif 4 == ES_WCHAR_SIZE
    m_value.m_ullong = static_cast<unsigned long long>( static_cast<unsigned long>(c) );
# else
#   error Unsupported|unknown ES_WCHAR_SIZE!
# endif
#else
		m_value.m_ullong = static_cast<unsigned long long>( static_cast<unsigned char>(c) );
#endif
	}

	inline void doAssignToEmpty(int n) ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		m_type = VAR_INT;
		m_value.m_llong = n;
	}

	inline void doAssignToInt(int n) ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_INT);
		m_value.m_llong = n;
	}

	inline void doAssignToEmpty(unsigned n) ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		m_type = VAR_UINT;
		m_value.m_ullong = n;
	}

	inline void doAssignToUInt(unsigned n) ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_UINT);
		m_value.m_ullong = n;
	}

	inline void doAssignToEmpty(long n) ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		m_type = VAR_INT;
		m_value.m_llong = n;
	}

	inline void doAssignToEmpty(unsigned long n) ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		m_type = VAR_UINT;
		m_value.m_ullong = n;
	}

	inline void doAssignToEmpty(long long n) ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		m_type = VAR_INT64;
		m_value.m_llong = n;
	}

	inline void doAssignToEmpty(unsigned long long n) ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		m_type = VAR_UINT64;
		m_value.m_ullong = n;
	}

	inline void doAssignToEmpty(double f) ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		m_type = VAR_DOUBLE;
		m_value.m_double = f;
	}

	inline void doAssignToEmpty(const EsString::value_type* s)
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		m_type = VAR_STRING;
		new((void*)&m_value) EsString(s);
	}

	inline void doAssignToEmpty(const EsString& s)
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		m_type = VAR_STRING;
		new((void*)&m_value) EsString(s);
	}

	inline void doAssignToEmpty(const EsString::Array& s)
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		m_type = VAR_STRING_COLLECTION;
		new((void*)&m_value) EsString::Array(s);
	}

	inline void doAssignToEmpty(const EsVariant::Array& s)
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		m_type = VAR_VARIANT_COLLECTION;
		new((void*)&m_value) EsVariant::Array(s);
	}

	void doAssignToEmpty(const EsVariant& other)
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		switch ( other.m_type )
		{
		case VAR_EMPTY:
			m_type = other.m_type;
			break;
		case VAR_DOUBLE:
			m_value.m_double = other.m_value.m_double; // This will work for all the other types, including VAR_EMPTY
			m_type = other.m_type;
			break;
		case VAR_STRING:
			doAssignToEmpty(other.doInterpretAsString());
			break;
		case VAR_BIN_BUFFER:
			doAssignEsBinBufferToEmpty(other.doInterpretAsBinBuffer());
			break;
		case VAR_STRING_COLLECTION:
			doAssignToEmpty(other.doInterpretAsStringCollection());
			break;
		case VAR_VARIANT_COLLECTION:
			doAssignToEmpty(other.doInterpretAsVariantCollection());
			break;
		case VAR_OBJECT:
			doAssignToEmpty(other.m_value.m_intf.m_ptr, other.m_value.m_intf.m_own);
			break;
		default:
			m_value.m_ullong = other.m_value.m_ullong; // This will work for all the other types
			m_type = other.m_type;
			break;
		}
	}

	inline void doAssignEsBinBufferToEmpty(const EsBinBuffer& v)
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		m_type = VAR_BIN_BUFFER;
		new((void*)&m_value) EsBinBuffer(v);
	}

	inline void doAssignToEmpty(EsBaseIntf* i, bool own) ES_NOTHROW
	{
		ES_ASSERT(m_type == VAR_EMPTY);
		m_type = VAR_OBJECT;
		m_value.m_intf.m_ptr = i;
		m_value.m_intf.m_own = own;
		if( m_value.m_intf.m_ptr && m_value.m_intf.m_own )
			m_value.m_intf.m_ptr->incRef();
	}

  /// Assign the value of type interface pointer from some other interface pointer,
  /// without the use of smartptr interlock. Use with caution, for micro optimizations
  ///
  /// PRECONDITION: None
  ///
  /// POSTCONDITION: The object is built. The type is set to VAR_OBJECT.
  /// The value is set to reference the object given.
  ///
  template < typename OtherIntfT >
  inline void doAssignOtherIntfPtrToEmptyWithoutInterlock( const EsIntfPtr< OtherIntfT >& intf )
  {
    ES_ASSERT( m_type == VAR_EMPTY );
    m_type = VAR_OBJECT;

    EsBaseIntf::Ptr p;
    p.assignNonInterlocked( intf );

    m_value.m_intf.m_ptr = p.get();
    m_value.m_intf.m_own = p.own();
    if( m_value.m_intf.m_ptr && m_value.m_intf.m_own )
      m_value.m_intf.m_ptr->incRef();
  }

public:
	/// Empty variant, the same as NULL for pointers
	static const EsVariant& null() ES_NOTHROW;

  /// Empty collection variant
  static const EsVariant::Array& nullCollection() ES_NOTHROW;

  /// Empty object variant
	static const EsVariant& nullObject() ES_NOTHROW;

  /// Dump variant contents to a string
  static EsString dump(const EsVariant& v);

private: // Methods:
	// Set the type of the variant, private.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: Type of the variant becomes the one given, the previous value
	// is discarded. If the new type is string, the string is not created.
	//
	inline void doSetType(Type type) ES_NOTHROW
	{
		doCleanup();
		m_type = type;
	}

	// release object interface and nullify object interface pointer
	void releaseObject() ES_NOTHROW;
	// Cleanup variant contents
	void doCleanup() ES_NOTHROW;
  // Internal contents move services
  void internalMove(EsVariant& other) ES_NOTHROW;

private: // Data:
	// Variable to hold the variant value.
	// It is the first data in the class for making sure it is aligned to the eight byte boundary.
	//
	union Value
	{
		// Used for storing of reflected object interface pointer
		//
		struct {
			EsBaseIntf* m_ptr;
			bool m_own;

		} m_intf;

		// used for storing void pointer
		void* m_pointer;

		// Used for storing of all numerics
		//
		long long m_llong;

		// Used for storing of all unsigned numerics and characters
		//
		unsigned long long m_ullong;

		// Used for storing of all floating point values
		//
		double m_double;

		// Used for storing of string data, mapped to EsString
		//
		esU8 m_string[ sizeof(EsString) ];

		// Used for storing of string data, mapped to EsBinBuffer
		//
		esU8 m_binBuffer[ sizeof(EsBinBuffer) ];

		// Used for storing of string data, mapped to EsString and EsBinBuffer
		//
		esU8 m_stringCollection[ sizeof(EsString::Array) ];

		// Used for storing the vector of variants
		//
		esU8 m_variantCollection[ sizeof(EsVariant::Array) ];

	} m_value;

	// Type of the current value within variant.
	//
	Type m_type;
};
typedef EsVariant::Array EsVariantArray;

#endif // _es_variant_h_
