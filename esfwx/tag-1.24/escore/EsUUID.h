#ifndef _es_uuid_h_
#define _es_uuid_h_

/// UUID reflected utility class
///
class ESCORE_CLASS ES_INTF_IMPL1(EsUUID, EsReflectedClassIntf)

private:
	// internal initializer
	static EsBaseIntfPtr create(const GUID& uuid = EsUUID::null());

public:
	EsUUID(const GUID& uuid = EsUUID::null());

  /// Get GUID from its string representation.
  /// Throw error if str is not valid representation of GUID.
  /// An empty string is interpreted as GUID_NULL.
  ///
  /// @param	uuidStr	String representation of GUID
  ///
	EsUUID(const EsString& uuidStr);

	ES_DECL_REFLECTED_CLASS_BASE(EsUUID)
	ES_DECL_ATTR_HANDLING_STD

  /// Constructors
  ///

  /// Create null UUID
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

  /// Create UUID from variant. Flexible construction method, creates either copy of another UUID object,
  /// if variant contains it, or tries to create UUID from properly sized binary buffer (if any),
  /// or string, if exists
  ///
 	ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, cr_EsVariant);

  /// Generate new UUID
  ES_DECL_REFLECTED_CLASS_METHOD0(EsBaseIntfPtr, generateReflected);

  /// Standard comparison implementation for reflected usage
  ///
	ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, compare, cr_EsVariant);

  /// Convert GUID to string.
  ///
  /// @param	canonical	If true, use canonical GUID string formatting (with dashes as separators,
  ///                   but not enclosed in curly brackets)
  ///                   If false - just convert GUID to continuous HEX string
  ///
  /// @return	String representation of GUID.
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(EsString, asString, bool);

	/// Custom stream reader
	ES_DECL_REFLECTED_METHOD1(void, streamRead, cr_EsBaseIntfPtr);

	/// Custom stream writer
	ES_DECL_REFLECTED_METHOD1(void, streamWrite, cr_EsBaseIntfPtr);

  /// Properties
  ///

  /// String GUID representation. Read accessor always
  /// return canonical string GUID representation
  ///
  ES_DECL_PROPERTY(string, EsString)

  /// Non-reflected services
  ///

  /// String access
  EsString stringGet() const;
  void stringSet(const EsString& str);

  /// GUID access
  const GUID& uuidGet() const { return m_uuid; }
  void uuidSet(const GUID& uuid) { m_uuid = uuid; }

  /// Static service to generate unique ID
  static GUID generate();

  /// Operators
  ///

  /// Type conversion
  operator const GUID& () const { return m_uuid; }

  /// Assignment
	EsUUID& operator= (const GUID& uuid) { m_uuid = uuid; return *this; }

  /// Equality comparison
	bool operator== (const EsUUID& other) const;

	// Variant conversion helpers for Reflection
	static GUID fromVariant(const EsVariant& var);
	static EsVariant toVariant(const GUID& uuid);

  // Return null GUID constant object
  static const GUID& null();

protected:
  GUID m_uuid;
};

#endif // _es_uuid_h_