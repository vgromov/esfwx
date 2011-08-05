#ifndef _es_variant_h_
#define _es_variant_h_

// Variant data type, variable or constant which type is determined at runtime.
//
class EKOSF_CORE_CLASS EsVariant
{
public: // Types:

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
		VAR_WXOBJECT,						// Variant has wxObject pointer
		VAR_OBJECT_EMBEDDED,		// Variant has reflected object passed by value (used for small objects, like DateTime)
		VAR_VARIANT_COLLECTION, // Array of any type of elements, array of variants
		VAR_VARIANT             // Variant type by itself. This tag is used externally
														// by applications to denote variant as the whole type.
														// EsVariant::getType will never return it.
	};

	// Tag that allows telling string constructor from the binary buffer constructor.
	//
	enum AcceptStringType
	{
		ACCEPT_STRING
	};

	// Tag that allows telling an embedded object from an ordinary object
	//
	enum AcceptObjectEmbedded
	{
		ACCEPT_OBJECT_EMBEDDED
	};

	enum
	{
		OBJECT_EMBEDDED_SIZE_IN_DWORDS = 9, // embedded object size in double machine words
		defVarCollectionSize = 16,					// default pool size for variant collection 
	};

	// Type definition used to store embedded objects
	//
	typedef UINT32 ObjectEmbeddedPlaceholder[OBJECT_EMBEDDED_SIZE_IN_DWORDS];

	// Local placeholder class, used for passing small objects by value.
	// Note that no operation is defined, everything is private, and not exportable.
	//
	class ObjectByValue
	{
		// Placeholder field of the exact size (pointer size in DWORDS is subtracted for class vtable pointer)
		//
		UINT32 m_placeholder[OBJECT_EMBEDDED_SIZE_IN_DWORDS-(sizeof(void*)/4)];

	public:
		virtual ~ObjectByValue()
		{
		}
	};

	// Type for holding the array of variants
	//
	typedef std::vector<EsVariant> EsVariantVector;

public: // Constructor and destructor:

	// Default object constructor.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_EMPTY.
	//
	EsVariant() :
	m_type(VAR_EMPTY)
	{
	}

	// Object constructor that creates an empty object of a given type.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_EMPTY.
	//
	EsVariant(Type type) :
	m_type(VAR_EMPTY)
	{
		setToNull(type);
	}

	// Construct the value of type bool and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_BOOL.
	// The value is set to one given.
	//
	explicit EsVariant(bool n)	:
	m_type(VAR_BOOL)
	{
		m_value.m_llong = n ? 1 : 0;
	}

	// Construct the value of type long and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_INT.
	// The value is set to one given.
	//
	EsVariant(long n)	:
	m_type(VAR_INT)
	{
		m_value.m_llong = n;
	}

	// Construct the value of type long long and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_INT64.
	// The value is set to one given.
	//
	EsVariant(long long n)	:
	m_type(VAR_INT64)
	{
		m_value.m_llong = n;
	}

	// Construct the value of type unsigned long and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_UINT.
	// The value is set to one given.
	//
	EsVariant(unsigned long n) :
	m_type(VAR_UINT)
	{
		m_value.m_ullong = n;
	}

	// Construct the value of type unsigned long long and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_UINT64.
	// The value is set to one given.
	//
	EsVariant(unsigned long long n) :
	m_type(VAR_UINT64)
	{
		m_value.m_ullong = n;
	}

	// Construct the value of type int and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_INT.
	// The value is set to one given.
	//
	EsVariant(int n) :
	m_type(VAR_INT)
	{
		m_value.m_llong = n;
	}

	// Construct the value of type unsigned int and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_UINT.
	// The value is set to one given.
	//
	EsVariant(unsigned n)	:
	m_type(VAR_UINT)
	{
		m_value.m_ullong = n;
	}

	// Construct the value of type UINT8 with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_UINT.
	// The value is set to one given.
	//
	EsVariant(UINT8 b) :
	m_type(VAR_BYTE)
	{
		m_value.m_ullong = static_cast<unsigned long long>(b);
	}

	// Construct the value of type char and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_CHAR.
	// The value is set to one given.
	//
	EsVariant(wxChar c) :
	m_type(VAR_CHAR)
	{
#if UNICODE
		wxASSERT_MIN_BITSIZE(wxChar, 16); // make sure we are not wrong with the below assumption on c size
		m_value.m_ullong = static_cast<unsigned long long>( static_cast<unsigned short>(c) );
#else
		m_value.m_ullong = static_cast<unsigned long long>( static_cast<unsigned char>(c) );
#endif
	}

	// Construct the value of type double and with the value specified.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_DOUBLE.
	// The value is set to one given.
	//
	EsVariant(double f)	:
	m_type(VAR_DOUBLE)
	{
		m_value.m_double = f;
	}

	// Construct the value of type string and with the value specified
	// as the constant character pointer.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_STRING.
	// The value is set to one given.
	//
	EsVariant(const wxChar* p) :
	m_type(VAR_STRING)
	{
		new((void*)&m_value) EsString(p);
	}
	
	EsVariant(const wxChar* p, unsigned len, AcceptStringType) :
	m_type(VAR_STRING)
	{
		new((void*)&m_value) EsString(p, len);
	}

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
	EsVariant(const UINT8* p, unsigned len)	:
	m_type(VAR_BIN_BUFFER)
	{
		new((void*)&m_value) EsBinBuffer(p, p+len);
	}

	// Construct the value of type binary buffer,
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_BIN_BUFFER.
	// The value is set to one given.
	//
	EsVariant(const EsBinBuffer& s) :
	m_type(VAR_BIN_BUFFER)
	{
		new((void*)&m_value) EsBinBuffer(s);
	}

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
	EsVariant(const EsString& s) :
	m_type(VAR_STRING)
	{
		new((void*)&m_value) EsString(s);
	}

	// Construct the value of type string collection and with the value specified
	// as parameter.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_STRING_COLLECTION.
	// The value is set to one given.
	//
	EsVariant(const EsString::Array& v)	:
	m_type(VAR_STRING_COLLECTION)
	{
		new((void*)&m_value) EsString::Array(v);
	}

	// Construct the value of type variant collection and with the value specified
	// as parameter.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_VARIANT_COLLECTION.
	// The value is set to one given.
	//
	EsVariant(const EsVariantVector& v)	:
	m_type(VAR_VARIANT_COLLECTION)
	{
		new((void*)&m_value) EsVariantVector(v);
	}

	// Construct the value of type base interface pointer
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_OBJECT.
	// The value is set to reference the object given.
	//
	EsVariant(const EsBaseIntf::Ptr& intf)	:
	m_type(VAR_OBJECT)
	{
		m_value.m_intf = intf.get();
		if(m_value.m_intf)
			m_value.m_intf->incRef();
	}
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
		EsBaseIntf::Ptr p = intf.request<EsBaseIntf>();
		m_value.m_intf = p.get();
		if(m_value.m_intf)
			m_value.m_intf->incRef();
	}
	
	// Copy the embedded object to the variant.
	//
	// PRECONDITION: The given embedded object shall be nonzero, or there is a debug check.
	//
	// POSTCONDITION: The object is built. The type is set to VAR_OBJECT_EMBEDDED.
	// The value is copied to the embedded buffer.
	//	
	EsVariant(const EsBaseIntf* intf, AcceptObjectEmbedded) :
	m_type(VAR_OBJECT_EMBEDDED)
	{
		wxASSERT(intf);
		doCopyObjectEmbedded( intf );
	}	
	
	// Copy the embedded object to the variant.
	//
	// PRECONDITION: The given embedded object shall be nonzero, or there is a debug check.
	//
	// POSTCONDITION: The object is built. The type is set to VAR_OBJECT_EMBEDDED.
	// The value is copied to the embedded buffer.
	//
	EsVariant(const ObjectByValue& o) :
	m_type(VAR_OBJECT_EMBEDDED)
	{
		wxASSERT(&o);
		doCopyObjectEmbedded(&o);
	}
	
	// Construct the value of type wxObject pointer
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_WXOBJECT.
	// The value is set to reference the object given.
	//
	explicit EsVariant(wxObject* obj)	:
	m_type(VAR_WXOBJECT)
	{
		m_value.m_wxObj = obj;
	}
	
	// Construct the value of type void pointer
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built. The type is set to VAR_POINTER.
	// The value is set to reference the object given.
	//
	explicit EsVariant(void* ptr)	:
	m_type(VAR_POINTER)
	{
		m_value.m_pointer = ptr;
	}
	
	// Construct the value from the copy. Initialize the object
	// with the attributes of the other object.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is built.
	// Has the same attributes as the other object given.
	//
	EsVariant(const EsVariant& v) : 
	m_type(VAR_EMPTY)
	{
		doAssignToEmpty(v);
	}

	// destroy the variant object, reclaim memory if the value was allocated dynamically
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The object is destroyed.
	//
	~EsVariant() ES_NOTHROW
	{
		doCleanup();
	}

public: // Services that work with type of the variant:
	// Get the type of the variant object.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The type of the variant is returned, see Type constants.
	//
	Type getType() const
	{
		return m_type;
	}
	
	// Tells whether the variant is of type VAR_EMPTY, means not initialized
	// with any value.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: True is returned if the variant is empty.
	//
	bool isEmpty() const
	{
		return m_type == VAR_EMPTY || 
			((m_type == VAR_OBJECT || m_type == VAR_WXOBJECT) && m_value.m_intf == 0);
	}

	// Whether the variant is of numeric type, so the arithmetic
	// operations can be performed.
	// The following types are arithmetic: VAR_BOOL,
	// VAR_CHAR, VAR_INT, VAR_UINT, and VAR_DOUBLE.
	// Empty and string types are not numeric.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: True if the variant is of string type.
	//
	bool isNumeric() const
	{
		return m_type > VAR_EMPTY && m_type <= VAR_DOUBLE;
	}

	// Whether the variant can be indexed.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: VAR_STRING, VAR_BIN_BUFFER, VAR_STRING_COLLECTION, and VAR_VARIANT_COLLECTION
	// will return true, The other types cannot be indexed.
	//
	bool isIndexed() const;

	// Whether the variant is a collection, array.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: VAR_STRING_COLLECTION, and VAR_VARIANT_COLLECTION
	// will return true, The other types cannot be indexed.
	//
	bool isCollection() const
	{
		return m_type == VAR_STRING_COLLECTION ||
			m_type == VAR_VARIANT_COLLECTION;
	}

	// Get the count of the elements in the variant, if they can be indexed.
	//
	// PRECONDITION: IsIndexed should be true, or the attempt to use this
	// operation will lead to exception.
	//
	// POSTCONDITION: The count of the elements in the collection is returned.
	//
	int getCount() const;

	// Whether the variant has an interface reference.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: True if the variant is of type interface.
	//
	bool isObject() const
	{
		return m_type == VAR_OBJECT || m_type == VAR_WXOBJECT || m_type == VAR_OBJECT_EMBEDDED;
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
	EsVariant& operator=(UINT8 b)
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
	EsVariant& operator=(wxChar c)
	{
#if UNICODE
		wxCOMPILE_TIME_ASSERT(sizeof(wxChar) == 2, wxCharSizeEq2); // make sure we are not wrong with the below assumption on c size
		return doSetInt(static_cast<long long>( static_cast<unsigned short>(c) ), VAR_CHAR);
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
	EsVariant& operator=(const wxChar* p);

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
	EsVariant& operator=(const EsVariantVector& s);

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
	void assign(const UINT8* v, unsigned len);

	// Assign the string to the variant type.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value has type VAR_STRING, and value specified
	// with pointer and length.
	//
	void assignString(const wxChar* v, unsigned len);

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
		return operator= ( v.request< EsBaseIntf >() );
	}
		
	// Assignment operator that takes ObjectByValue stub, as handled by reflection.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value has the same type and value as the one given.
	//
	EsVariant& operator=(const ObjectByValue& o);	
	
	// Assignment operator that takes variable of type wxObject.
	// Note that the variant does not own the object,
	// and it is a responsibility of the application to ensure
	// that the object is not discarded before its reference.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: The previous value is discarded.
	// The new value has the same type and value as the one given.
	//
	EsVariant& operator=(const wxObject* obj);
	
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
	bool asBool(const std::locale& loc = std::locale()) const;

	// Interpret the variant value as byte, if possible.
	// The type of the value should fit within one byte.
	//
	// PRECONDITION: The conversion should be possible.
	// Bad conversion can be thrown in cases the type is incompatible
	// or the range is bad.
	//
	// POSTCONDITION: Byte representation of the value is returned.
	//
	UINT8 asByte(const std::locale& loc = std::locale()) const;

	// Interpret the variant value as type wxChar, if possible.
	// The type of the value should allow conversion
	// of it into wxChar. It should either be VAR_CHAR itself,
	// or it should be numeric with the allowed range,
	// or it should be a string with the size 1.
	//
	// PRECONDITION: The conversion should be possible.
	// Bad conversion can be thrown in cases the type is incompatible
	// or the range is bad.
	//
	// POSTCONDITION: Char representation of the value is returned.
	//
	wxChar asChar(const std::locale& loc = std::locale()) const;

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
	UINT32 asInternalDWord(const std::locale& loc = std::locale()) const;

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
	UINT64 asInternalQWord(const std::locale& loc = std::locale()) const;

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
	int asInt(const std::locale& loc = std::locale()) const
	{
		wxCOMPILE_TIME_ASSERT(sizeof(int) == sizeof(long), IntSizeEqLongSize); // Careful with architectures where sizes of int and long do not match
		return (int)asLong(loc);
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
	unsigned asUInt(const std::locale& loc = std::locale()) const
	{
		wxCOMPILE_TIME_ASSERT(sizeof(unsigned) == sizeof(unsigned long), UnsignedSizeEqUnsignedLongSize); // Careful with architectures where sizes of int and long do not match
		return (unsigned)asULong(loc);
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
	long asLong(const std::locale& loc = std::locale()) const;

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
	unsigned long asULong(const std::locale& loc = std::locale()) const;

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
	long long asLLong(const std::locale& loc = std::locale()) const;

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
	unsigned long long asULLong(const std::locale& loc = std::locale()) const;

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
	double asDouble(const std::locale& loc = std::locale()) const;

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
	EsString asString(const std::locale& loc = std::locale()) const;

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
	EsString asString(unsigned mask, const std::locale& loc = std::locale()) const;

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
	EsString asEscapedString(const std::locale& loc = std::locale()) const;

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
	EsString::Array asStringCollection(const std::locale& loc = std::locale()) const;

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
	EsVariantVector asVariantCollection() const;

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

	// Interpret the variant value as wxObject reference, if possible.
	// The only value that can be interpreted as object is an object itself.
	//
	// PRECONDITION: An exception is thrown in case the value is not of type object.
	//
	// POSTCONDITION: Object reference is returned.
	//
	wxObject* asWxObject();

	// Interpret the variant value as wxObject reference, if possible.
	// The only value that can be interpreted as object is an object itself.
	//
	// PRECONDITION: An exception is thrown in case the value is not of type object.
	//
	// POSTCONDITION: Object reference is returned.
	//
	wxObject* asWxObject() const
	{
		return const_cast<EsVariant*>(this)->asWxObject();
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
	inline EsBaseIntf::Ptr asExistingObject() const
	{
		return const_cast<EsVariant*>(this)->asExistingObject();
	}

	// Interpret the variant value as an existing non-NULL wxObject reference, if possible.
	// The only value that can be interpreted as object is an object itself.
	//
	// PRECONDITION: An exception is thrown in case the value is not of type object.
	// If the object is NULL, a no value exception is thrown.
	//
	// POSTCONDITION: Object reference is returned.
	//
	wxObject* asExistingWxObject();

	// Interpret the variant value as an existing non-NULL constant wxObject reference, if possible.
	// The only value that can be interpreted as object is an object itself.
	//
	// PRECONDITION: An exception is thrown in case the value is not of type object.
	// If the object is NULL, a no value exception is thrown.
	//
	// POSTCONDITION: Constant object reference is returned.
	//
	wxObject* asExistingWxObject() const
	{
		return const_cast<EsVariant*>(this)->asExistingWxObject();
	}

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
	void* asPointer() const
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
	// This corresponds to a concept of owned variant.
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
	void swap(EsVariant&) ES_NOTHROW;

	// Fast method that moves the value to another variant, and sets the other variant type to Empty.
	// This is possible only because variant values can always be moved.
	//
	// PRECONDITION: None.
	//
	// POSTCONDITION: The value is moved to this variant, other variant is set to empty.
	//
	void moveFrom(EsVariant& other) ES_NOTHROW;

	// Return this object with the power of the object given.
	// The power type returned is always DOUBLE.
	//
	// PRECONDITION: The type should be compatible with the power operation.
	// The compatible types are those that can be cast to DOUBLE.
	// If not, the bad conversion is thrown. In case any of the values
	// are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: Powered DOUBLE value returned.
	//
	EsVariant pow(const EsVariant& val, const std::locale& loc = std::locale()) const;

	// Get element by index.
	// One can check isIndexed property to know if the variant can be
	// indexed. Also there is a getCount, which is callable only for
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
	EsVariant getItem(const EsVariant& index) const;

	// Set element by index.
	// One can check IsIndexed property to know if the variant can be
	// indexed. Also there is a GetCount, which is callable only for
	// IsIndexed, and will return the number of items in the variant.
	//
	// PRECONDITION: The type should allow subscripting,
	// such as VAR_STRING, VAR_BIN_BUFFER or VAR_STRING_COLLECTION.
	// If an item is an object, it shall have a reflected service with the name SetItem.
	// Otherwise the conversion exception is thrown.
	//
	// POSTCONDITION: The indexed item is set to one given.
	//
	void setItem(const EsVariant& index, const EsVariant& value, const std::locale& loc = std::locale());

	// Returns true if a given item is in the variant.
	// If the variant is a collection of any kind,
	// true will mean the parameter is contained it in.
	// If the variant is a simple variant, IN means equality.
	//
	// This method is using the value as SET.
	// There is no matter how many duplicate items are present in either of the values.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: True will mean the value signified by a given variant
	// is within this variant value.
	//
	bool isIn(const EsVariant&) const;

	// Add a given variant as a whole to the collection.
	// This call is different from operator+=, as it does not
	// unroll the collection items in case the given parameter is a collection.
	//
	// PRECONDITION: The type of the variant shall be VARIANT_COLLECTION,
	// there is a debug check.
	//
	// POSTCONDITION: A variant is added to collection. Reference to variant itself is returned
	//
	EsVariant& addToVariantCollection(const EsVariant& v, bool toFront = false)
	{
		if( toFront )
			doInterpretAsVariantCollection().insert(doInterpretAsVariantCollection().begin(), v);
		else
			doInterpretAsVariantCollection().push_back(v);
		return *this;
	}

	// Adjust a given index to standards of indexing in script
	// So the negative index will mean counting from the end of the array.
	//
	// PRECONDITION: The index shall be in range -count to count - 1,
	// where count is a signed integer. Otherwise an exception is thrown.
	//
	// POSTCONDITION: An index is adjusted to denote the real array element.
	//
	static void adjustIndex(int& index, unsigned count);

	//// Adjust a given slice to standards of slicing in script.
	//// So the negative index will mean counting from the end of the array.
	//// A negative slice will mean no elements.
	////
	//// PRECONDITION: Slice is always adjusted correctly.
	////
	//// POSTCONDITION: A slice is adjusted, so it denotes the real array element range.
	////
	//static int AdjustSlice(int &from, int &to, unsigned count);

	//// Return the slice of values for types that support subscrips.
	//// One can check IsIndexed property to know if the variant can be
	//// indexed. Also there is a GetCount, which is callable only for
	//// IsIndexed, and will return the number of items in the variant.
	////
	//// PRECONDITION: The type should allow subscripting,
	//// such as VAR_STRING, VAR_BIN_BUFFER or VAR_STRING_COLLECTION.
	//// If an item is an object, it shall have a reflected service with the name SetItem.
	//// Otherwise the conversion exception is thrown.
	////
	//// POSTCONDITION: Result variant has the same type as this,
	//// but it has only a given subrange of elements.
	////
	//EsVariant GetSlice(int from, int to) const;

	//// Set the slice of values for types that support subscrips.
	//// Shrink or grow the number of items if necessary.
	//// One can check IsIndexed property to know if the variant can be
	//// indexed. Also there is a GetCount, which is callable only for
	//// IsIndexed, and will return the number of items in the variant.
	////
	//// PRECONDITION: The type should allow subscripting,
	//// such as VAR_STRING, VAR_BIN_BUFFER or VAR_STRING_COLLECTION.
	//// If an item is an object, it shall have a reflected service with the name SetItem.
	//// Otherwise the conversion exception is thrown.
	////
	//// POSTCONDITION: Result variant has a given slice of items set to new values.
	//// In case the number of values is lesser or bigger than the target slice,
	//// the number of items will change.
	////
	//void SetSlice(int from, int to, const EsVariant& values);

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
	// PRECONDITION: The operand types should be convertible to BOOL or numeric.
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
	// PRECONDITION: The operand types should be convertible to BOOL or numeric.
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
	// PRECONDITION: The operand types should be convertible to BOOL or numeric.
	// Otherwise bad conversion is raised.
	// In case any of the values are not initialized, no value exception is thrown.
	//
	// POSTCONDITION: In case of both boolean operands, return true if both
	// have the same value. In case of the numeric values, do bitwise XOR.
	//
	EsVariant operator^(const EsVariant&) const;

	// Unary operator not.
	// Note that there is no difference between logical NOT and bitwise NOT.
	// Logical NOT is applied for bool operands, the resulting value is bool.
	// For all the other numeric values bitwise NOT is performed.
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

	// variant-based string formatter. fmt parameter must contain string variant
	// with printf-compatible formatting control codes. 
	// args should be collection of variants with values to format.
	// returned is variant containing resulting string. exceptions may be thrown if
	// arguments count|types do not match the formatting field count|field types
	static EsVariant format(const EsVariant& fmt, const EsVariant& args, const std::locale& loc = std::locale());

private: // Services:

	// Internal assignment service that constructs integer-based variant value
	// with the particular type given.
	//
	// PRECONDITION: type has to be of generic integer type. Otherwise the
	// behavior is undefined.
	//
	// POSTCONDITION: The variant is initialized with the value and type given.
	//
	EsVariant& doSetInt(long long value, Type type) ES_NOTHROW;

public: // Semi-public services that has to be used carefully:

	// Interpret the internals of the variant as bool.
	//
	// PRECONDITION: the type is VAR_BOOL, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The value of bool is returned.
	//
	bool doInterpretAsBool() const
	{
		wxASSERT(m_type == VAR_BOOL);
		return m_value.m_llong != 0;
	}

	// Interpret the internals of the variant as byte.
	//
	// PRECONDITION: the type is VAR_BOOL, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The value of byte is returned.
	//
	UINT8 doInterpretAsByte() const
	{
		wxASSERT(m_type == VAR_BYTE);
		return static_cast<UINT8>(m_value.m_ullong);
	}

	// Interpret the internals of the variant as UINT.
	//
	// PRECONDITION: the type is VAR_UINT, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The value of UINT is returned.
	//
	wxChar doInterpretAsChar() const
	{
		wxASSERT(m_type == VAR_CHAR);
		return static_cast<wxChar>(m_value.m_ullong);
	}

	// Interpret the internals of the variant as INT.
	//
	// PRECONDITION: the type is VAR_INT, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The value of INT is returned.
	//
	int doInterpretAsInt() const
	{
		wxASSERT(m_type == VAR_INT);
		return static_cast<int>(m_value.m_llong);
	}

	// Interpret the internals of the variant as UINT.
	//
	// PRECONDITION: the type is VAR_UINT, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The value of UINT is returned.
	//
	unsigned doInterpretAsUInt() const
	{
		wxASSERT(m_type == VAR_UINT);
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
	EsString& doInterpretAsString()
	{
		wxASSERT(m_type == VAR_STRING);
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
	const EsString& doInterpretAsString() const
	{
		wxASSERT(m_type == VAR_STRING);
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
	EsBinBuffer& doInterpretAsBinBuffer()
	{
		wxASSERT(m_type == VAR_BIN_BUFFER);
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
	const EsBinBuffer& doInterpretAsBinBuffer() const
	{
		wxASSERT(m_type == VAR_BIN_BUFFER);
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
	EsString::Array& doInterpretAsStringCollection()
	{
		wxASSERT(m_type == VAR_STRING_COLLECTION);
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
	const EsString::Array& doInterpretAsStringCollection() const
	{
		wxASSERT(m_type == VAR_STRING_COLLECTION);
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
	EsVariantVector& doInterpretAsVariantCollection()
	{
		wxASSERT(m_type == VAR_VARIANT_COLLECTION);
		return *reinterpret_cast<EsVariantVector*>(&m_value);
	}

	// Interpret the internals of the variant as constant variant collection.
	//
	// PRECONDITION: the type is VAR_VARIANT_COLLECTION, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The reference to the variant buffer as it was a
	// constant variant collection is returned.
	//
	const EsVariantVector& doInterpretAsVariantCollection() const
	{
		wxASSERT(m_type == VAR_VARIANT_COLLECTION);
		return *reinterpret_cast<const EsVariantVector*>(&m_value);
	}

	// Interpret the internals of the variant as an interface.
	//
	// PRECONDITION: the type is VAR_OBJECT, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The incRef-ed interface smartptr is returned.
	//
	EsBaseIntf::Ptr doInterpretAsObject()
	{
		wxASSERT(m_type == VAR_OBJECT);
		return EsBaseIntf::Ptr(m_value.m_intf, true);
	}

	// Interpret the internals of the variant as a constant interface.
	//
	// PRECONDITION: the type is VAR_OBJECT, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The incRef-ed interface smartptr is returned.
	//
	inline EsBaseIntf::Ptr doInterpretAsObject() const
	{
		wxASSERT(m_type == VAR_OBJECT);
		return EsBaseIntf::Ptr(m_value.m_intf, true);
	}

	// Interpret the internals of the variant as wxObject pointer.
	//
	// PRECONDITION: the type is VAR_WXOBJECT, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The pointer to the object is returned.
	//
	wxObject* doInterpretAsWxObject()
	{
		wxASSERT(m_type == VAR_WXOBJECT);
		return m_value.m_wxObj;
	}

	// Interpret the internals of the variant as a constant wxObject pointer.
	//
	// PRECONDITION: the type is VAR_WXOBJECT, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The pointer to the constant object is returned.
	//
	const wxObject* doInterpretAsWxObject() const
	{
		wxASSERT(m_type == VAR_WXOBJECT);
		return m_value.m_wxObj;
	}

	// Interpret the internals of the variant as an embedded object.
	//
	// PRECONDITION: the type is VAR_OBJECT_EMBEDDED, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The pointer to the object embedded into the variant is returned.
	//
	EsBaseIntf* doInterpretAsObjectEmbedded()
	{
		wxASSERT(m_type == VAR_OBJECT_EMBEDDED);
		// alignment: wxASSERT((long)&m_value.m_embedded % 8 == 0);
		return reinterpret_cast<EsBaseIntf*>(m_value.m_embedded);
	}

	// Interpret the internals of the variant as a constant embedded object.
	//
	// PRECONDITION: the type is VAR_OBJECT_EMBEDDED, otherwise the behavior
	// is undefined. The debugger version has the assert operator.
	//
	// POSTCONDITION: The pointer to the constant object embedded into the variant is returned.
	//
	const EsBaseIntf* doInterpretAsObjectEmbedded() const
	{
		wxASSERT(m_type == VAR_OBJECT_EMBEDDED);
		// alignment: wxASSERT((long)&m_value.m_embedded % 8 == 0);
		return reinterpret_cast<const EsBaseIntf*>(m_value.m_embedded);
	}

	// Copy embedded object from the given pointer
	//
	// PRECONDITION: Called privately at proprietary place and condition, no check is done
	//
	// POSTCONDITION: The object denoting the pointer given is copied
	//
	void doCopyObjectEmbedded(const void* ptr)
	{
		for ( int i = 0; i < OBJECT_EMBEDDED_SIZE_IN_DWORDS; ++i )
			m_value.m_embedded[i] = static_cast<const UINT32*>(ptr)[i];
	}

	void doAssignToEmpty(bool b)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_BOOL;
		m_value.m_llong = b;
	}

	void doAssignToEmpty(UINT8 b)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_BYTE;
		m_value.m_ullong = b;
	}

	void doAssignToEmpty(wxChar c)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_CHAR;
#if UNICODE
		wxASSERT_MIN_BITSIZE(wxChar, 16); // make sure we are not wrong with the below assumption on c size
		m_value.m_ullong = static_cast<unsigned long long>( static_cast<unsigned short>(c) );
#else
		m_value.m_ullong = static_cast<unsigned long long>( static_cast<unsigned char>(c) );
#endif
	}

	void doAssignToEmpty(int n)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_INT;
		m_value.m_llong = n;
	}

	void doAssignToInt(int n)
	{
		wxASSERT(m_type == VAR_INT);
		m_value.m_llong = n;
	}

	void doAssignToEmpty(unsigned n)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_UINT;
		m_value.m_ullong = n;
	}

	void doAssignToUInt(unsigned n)
	{
		wxASSERT(m_type == VAR_UINT);
		m_value.m_ullong = n;
	}

	void doAssignToEmpty(long n)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_INT;
		m_value.m_llong = n;
	}

	void doAssignToEmpty(unsigned long n)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_UINT;
		m_value.m_ullong = n;
	}

	void doAssignToEmpty(long long n)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_INT64;
		m_value.m_llong = n;
	}

	void doAssignToEmpty(unsigned long long n)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_UINT64;
		m_value.m_ullong = n;
	}

	void doAssignToEmpty(double f)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_DOUBLE;
		m_value.m_double = f;
	}

	void doAssignToEmpty(const wxChar* s)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_STRING;
		new((void*)&m_value) EsString(s);
	}

	void doAssignToEmpty(const EsString& s)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_STRING;
		new((void*)&m_value) EsString(s);
	}

	void doAssignToEmpty(const EsString::Array& s)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_STRING_COLLECTION;
		new((void*)&m_value) EsString::Array(s);
	}

	void doAssignToEmpty(const EsVariantVector& s)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_VARIANT_COLLECTION;
		new((void*)&m_value) EsVariantVector(s);
	}

	void doAssignToEmpty(const EsVariant& other)
	{
		wxASSERT(m_type == VAR_EMPTY);
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
		case VAR_OBJECT_EMBEDDED:
			doAssignObjectEmbeddedToEmpty(other.doInterpretAsObjectEmbedded());
			break;	
		case VAR_OBJECT:
			doAssignToEmpty(other.m_value.m_intf);
			break;			
		default:
			m_value.m_ullong = other.m_value.m_ullong; // This will work for all the other types
			m_type = other.m_type;
			break;
		}
	}

	void doAssignToEmpty(const ObjectByValue& o)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_OBJECT_EMBEDDED;
		doCopyObjectEmbedded(&o);
	}

	void doAssignEsBinBufferToEmpty(const EsBinBuffer& v)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_BIN_BUFFER;
		new((void*)&m_value) EsBinBuffer(v);
	}

	void doAssignObjectEmbeddedToEmpty(const EsBaseIntf* o)
	{
		wxASSERT(m_type == VAR_EMPTY);
		wxASSERT(o != NULL);
		m_type = VAR_OBJECT_EMBEDDED;
		doCopyObjectEmbedded(o);
	}

	void doAssignToEmpty(EsBaseIntf* i)
	{
		wxASSERT(m_type == VAR_EMPTY);
		m_type = VAR_OBJECT;
		m_value.m_intf = i;
		if( m_value.m_intf )
			m_value.m_intf->incRef();
	}

public:
	// Empty variant, the same as NULL for pointers
	//
	static const EsVariant s_null;
	
	// Reflection-specific standard service names
	//
	static const EsString::pointer s_asString;
	static const EsString::pointer s_compare;
	static const EsString::pointer s_add;
	static const EsString::pointer s_subtract;
	static const EsString::pointer s_multiply;
	static const EsString::pointer s_divide;
	static const EsString::pointer s_getCount;
	static const EsString::pointer s_getItem;
	static const EsString::pointer s_setItem;
	static const EsString::pointer s_setToNull;
	static const EsString::pointer s_value;

private: // Methods:
	// Set the type of the variant, private.
	//
	// PRECONDITION: None
	//
	// POSTCONDITION: Type of the variant becomes the one given, the previous value
	// is discarded. If the new type is string, the string is not created.
	//
	void doSetType(Type type) ES_NOTHROW
	{
		doCleanup();
		m_type = type;
	}

	// release object interface and nullify object interface pointer 
	void releaseObject();
	// cleanup variant contents
	void doCleanup() ES_NOTHROW;

private: // Data:
	// Type of the current value within variant.
	//
	Type m_type;

	// Variable to hold the variant value.
	// It is the first data in the class for making sure it is aligned to the eight byte boundary.
	//
	union Value
	{
		// Used for storing of reflected object interface pointer
		//
		EsBaseIntf* m_intf;

		// Used for storing of embedded object
		//
		ObjectEmbeddedPlaceholder m_embedded;		
		
		// used for storing wxObject pointer
		//
		wxObject* m_wxObj;
		
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
		BYTE m_string[ sizeof(EsString) ];

		// Used for storing of string data, mapped to EsBinBuffer
		//
		BYTE m_byteString[ sizeof(EsBinBuffer) ];

		// Used for storing of string data, mapped to EsString and EsBinBuffer
		//
		BYTE m_stringCollection[ sizeof(EsString::Array) ];

		// Used for storing the vector of variants
		//
		BYTE m_variantCollection[ sizeof(EsVariantVector) ];

	} m_value;
};

#endif // _es_variant_h_
