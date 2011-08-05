#ifndef _Reflection_h_
#define _Reflection_h_

// special marker macro - augment localizable property label extraction into pot file
#define _i		wxT

// forward declaration(s)
//
class PropertyInfo;
class MethodInfoKeyT;
class ClassInfo;
class EsBaseIntf;

// reflected type aliasing. reflected type names must be single word without spaces
typedef const EsString&				cr_EsString;
#define To_EsString(var)			(var).asString()
#define To_cr_EsString(var)		(var).asString()
#define From_EsString(s)			(s)

typedef const EsString::Array& cr_EsStringArray;
typedef EsString::Array				EsStringArray;
#define To_EsStringArray(var)	(var).asStringCollection()
#define To_cr_EsStringArray(var) (var).asStringCollection()
#define From_EsStringArray(a)	(a)

typedef void*									p_void;
typedef const void*						cp_void;
#define To_p_void(var)				(var).asPointer()
#define To_cp_void(var)				(var).asPointer()
#define From_p_void(p)				(p)

typedef const long&						cr_long;
#define To_long(var)					(var).asLong()
#define To_cr_long(var)				(var).asLong()
#define From_long(l)					(l)

typedef unsigned long					ulong;
typedef const ulong&					cr_ulong;
#define To_ulong(var)					(var).asULong()
#define To_cr_ulong(var)			(var).asULong()
#define From_ulong(u)					(u)

typedef const bool&						cr_bool;
#define To_bool(var)					(var).asBool()
#define To_cr_bool(var)				(var).asBool()
#define From_bool(b)					(b)

typedef wxObject*							p_wxObject;
#define To_p_wxObject(var)		(var).asWxObject()
#define From_p_wxObject(p)		(p)

typedef wxEvtHandler*					p_wxEvtHandler;
#define To_p_wxEvtHandler(var) dynamic_cast<wxEvtHandler*>(var.asWxObject())

typedef EsBaseIntf::Ptr				EsBaseIntfPtr;
#define To_EsBaseIntfPtr(var)		(var).asObject()
#define From_EsBaseIntfPtr(ip)	(ip)

typedef const EsBinBuffer&		cr_EsBinBuffer;
#define To_cr_EsBinBuffer(var) (var).asBinBuffer()
#define From_EsBinBuffer(bb)	(bb)

// dummy define for variant-to variant casts
typedef const EsVariant&			cr_EsVariant;
#define To_EsVariant(var)			(var)	
#define To_cr_EsVariant(var)	(var)
#define From_EsVariant(v)			(v)

// misc stubs
#define From_double(d)				(d)
#define From_int(i)						(i)

// RPC reflection stubs
#define To_UINT8(var)					(var).asByte()
#define From_UINT8(u)					(u)
#define To_INT8(var)					static_cast<INT8>((var).asInt())
#define From_INT8(i)					static_cast<int>(i)
#define To_UINT16(var)				static_cast<UINT16>((var).asULong())
#define From_UINT16(u)				static_cast<ulong>(u)
#define To_WORD								To_UINT16
#define To_INT16(var)					static_cast<INT16>((var).asLong())
#define From_INT16(i)					static_cast<int>(i)
#define To_UINT32(var)				(var).asULong()
#define From_UINT32(u)				(u)
#define To_INT32(var)					(var).asLong()
#define From_INT32(i)					(i)
#define To_UINT64(var)				(var).asULLong()
#define From_UINT64(u)				(u)
#define To_INT64(var)					(var).asLLong()
#define From_INT64(i)					(i)
#define To_FLOAT(var)					static_cast<FLOAT>((var).asDouble())
#define From_FLOAT(f)					static_cast<double>(f)
#define To_BOOL(var)					static_cast<BOOL>((var).asBool() == true)
#define From_BOOL(b)					static_cast<bool>((b) == TRUE)
#define To_DATETIME(var)			static_cast<DATETIME>(EsDateTime(var))
#define From_DATETIME(dt)			static_cast<EsVariant>(EsDateTime(dt))
#define To_BYTEARRAY(var)			static_cast<BYTEARRAY>((var).asBinBuffer())
#define From_BYTEARRAY(ba)		EsBinBuffer(ba)
// RPC return types stubs (for EkosfRPCServices thread-safe wrappers, namely for BYTEARRAY safe copying on return)
#define UINT8_T								UINT8
#define INT8_T								INT8
#define UINT16_T							UINT16
#define INT16_T								INT16
#define UINT32_T							UINT32
#define INT32_T								INT32
#define UINT64_T							UINT64
#define INT64_T								INT64
#define FLOAT_T								FLOAT
#define BOOL_T								BOOL
#define DATETIME_T						DATETIME
#define BYTEARRAY_T						EsBinBuffer
#define To_TS_TYPE_HELPER(x)	x ##_T
#define To_TS_TYPE(x)					To_TS_TYPE_HELPER(x)

// reflected constructor and destructor names
//
extern EKOSF_CORE_DATA EsString::const_pointer c_reflectedCtrName;
extern EKOSF_CORE_DATA EsString::const_pointer c_reflectedDtrName;

// reflection helpers
//
#include <escore/ReflectionGenDefs.h>

// small subset of reflection|serialization|validation helpers
//
ES_DECL_INTF_BEGIN2( 97C0CB8C, 421B4ef5, A85E85C3, 107B1948, EsReflectedClassIntf, EsBaseIntf )
	// generic pointer types of reflected method && class method
	typedef void (EsBaseIntf:: *CallT)(void);
	typedef void (*ClassCallT)(void);
	// reflection interface methods
	//
	ES_DECL_INTF_METHOD(const ClassInfo&, getClassInfo)() const = 0;
	ES_DECL_INTF_METHOD(bool, isKindOf)(const EsReflectedClassIntf* other) const = 0;
	ES_DECL_INTF_METHOD(bool, isIndexed)() const = 0;
	ES_DECL_INTF_METHOD(EsString::Array, getPropertyNames)() const = 0;
	ES_DECL_INTF_METHOD(EsString::Array, getPersistentPropertyNames)() const = 0;	
	ES_DECL_INTF_METHOD(bool, hasProperty)(const EsString& name) const = 0;
	ES_DECL_INTF_METHOD(EsVariant, getProperty)(const EsString& name) const = 0;
	ES_DECL_INTF_METHOD(void, setProperty)(const EsString& name, const EsVariant& val) = 0;
	ES_DECL_INTF_METHOD(EsString, getConfigString)() const = 0;
	ES_DECL_INTF_METHOD(void, setConfigString)(const EsString& str, bool ignoreUnknownProps) = 0;	
	ES_DECL_INTF_METHOD(bool, hasMethod)(const MethodInfoKeyT& key) const = 0;
	ES_DECL_INTF_METHOD(EsVariant, callMethod)(const EsString& name, const EsVariant& params) = 0;
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name) = 0;
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1) = 0;
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2) = 0;
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) = 0;
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) = 0;
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) = 0;
	ES_DECL_INTF_METHOD(EsVariant, callClassMethod)(const EsString& name, const EsVariant& params) const = 0;
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name) const = 0;
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1) const = 0;
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2) const = 0;
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) const = 0;
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) const = 0;
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) const = 0;
ES_DECL_INTF_END

// generate reflection signatures
#include <escore/ReflectionDefSignatures.h>
// generate reflected methods parameter count
#include <escore/ReflectionDefParamCount.h>
// generate reflected method type casts
#include <escore/ReflectionDefTypeCasts.h>

// method info map key. both name, and parameters count used
//
class EKOSF_CORE_CLASS MethodInfoKeyT
{
public:
	MethodInfoKeyT(int paramsCount = 0, const EsString& name = EsString::s_null) :
			m_paramsCount(paramsCount), m_name(name) {}
	inline bool isOk() const { return !m_name.empty(); }
	inline const EsString& getName() const { return m_name; }
	inline int getParametersCount() const { return m_paramsCount; }
	// comparison operators, augmenting map key requirements
	bool operator< (const MethodInfoKeyT& other) const;
	inline bool operator ==(const MethodInfoKeyT& other) const
	{	return m_name == other.m_name && m_paramsCount == other.m_paramsCount; }

protected:
	int m_paramsCount;
	EsString m_name;
};
typedef std::vector<MethodInfoKeyT> MethodInfoKeysT;

// method information metaclass
//
class EKOSF_CORE_CLASS MethodInfo
{
protected:	
	// signature strings container
	class EKOSF_CORE_CLASS SigStringsT
	{
	public:
		inline SigStringsT& add(MethodSignature sig, const EsString& str) { m_sigs[sig] = str; return *this; }
		EsString getString(int sig) const;
		
	protected:
		typedef std::map<int, EsString> SigsT;
#pragma warning(push)
#pragma warning(disable: 4251)
		SigsT m_sigs;
#pragma warning(pop)
	};
	
public:
	MethodInfo(ClassInfo& owner, int sig,	BYTE paramsCount, const EsString& name, 
		const EsString& descr, EsReflectedClassIntf::CallT method);
	MethodInfo(ClassInfo& owner, int sig, BYTE paramsCount, const EsString& name, 
		const EsString& descr, EsReflectedClassIntf::ClassCallT classMethod, int tagClassCallInfo);
	MethodInfo(ClassInfo& owner, int sig,	const EsIID& iid, BYTE paramsCount, 
		const EsString& name, const EsString& descr, EsReflectedClassIntf::CallT method ); 
	// member access
	inline const ClassInfo& getClassInfo() const { return m_owner; }
	inline int getSignature() const { return m_signature; }
	inline int getParametersCount() const { return m_paramsCount; }
	inline const EsString& getName() const { return m_name; }
	inline EsString getDescr() const 
	{ 
		if(m_descr.empty()) 
			return m_name; 
		else 
			return wxGetTranslation( m_descr.c_str() );
	}
	// signature checks 
	inline bool isClassMethod() const { return m_signature > maxMethodSignature && m_signature < classMethodSignaturesEnd; }
	static void checkSignature(int sig);
	// return signature string by signature id
	inline EsString getSignatureString() const { return getSignatureString(m_signature); }
	static inline EsString getSignatureString(int sig) { return s_sigStrings.getString(sig); }
	// caller methods
	//
	// object member function caller
	EsVariant call(EsBaseIntf* obj, const EsVariant& params) const;
	// class function caller
	EsVariant classCall(const EsVariant& params) const;

protected:
	// parameter checks
	void checkParamsCount(const EsVariant& params) const;

private:
	// prohibited functionality
	MethodInfo();
	MethodInfo(const MethodInfo&);
	MethodInfo& operator=(const MethodInfo&);

protected:
	const ClassInfo& m_owner;
	// method signature, interface name & param count
	int m_signature;
	EsIID m_iid;
	int m_paramsCount;
	// method name & description.
	EsString m_name;
	EsString m_descr;
	// method pointer
	union {
	EsReflectedClassIntf::CallT m_method;
	EsReflectedClassIntf::ClassCallT m_classMethod;
	};
	// signature strings
	static const SigStringsT s_sigStrings;
};

// property info metaclass
//
class EKOSF_CORE_CLASS PropertyInfo
{
public:
	// restriction types 
	enum RestrictionType {
		rxtNone,
		rxtMin,
		rxtMinExcl,
		rxtMax,
		rxtMaxExcl,
		rxtRange,
		rxtRangeMinExcl,
		rxtRangeMaxExcl,
		rxtRangeExcl,
		rxtEnum
	};

	// property restriction meta class
	class EKOSF_CORE_CLASS Restriction
	{
	public:
		Restriction(const EsVariant& val = EsVariant::s_null, PropertyInfo::RestrictionType type = PropertyInfo::rxtNone);
		Restriction(const Restriction& src) : m_type(src.m_type), m_val(src.m_val) {}

		inline PropertyInfo::RestrictionType getType() const { return m_type; }
		// return restriction value(s)
		inline EsVariant getVal() const { return m_val; }

	protected:
		PropertyInfo::RestrictionType m_type;
		EsVariant m_val;

		friend class PropertyInfo;
	};

public:
	PropertyInfo(ClassInfo& owner, const EsString& name, const EsString& descr,
		const EsVariant& def, EsReflectedClassIntf::CallT reader, int readerSig, EsReflectedClassIntf::CallT writer, int writerSig,
		const PropertyInfo::Restriction& restriction);
	// property info services
	//
	inline const ClassInfo& getClassInfo() const { return m_owner; }
	bool validate(const EsVariant& val, bool doThrow = false) const;
	inline const EsVariant& getDefault() const { return m_def; }
	inline bool hasDefault() const { return !m_def.isEmpty(); }
	inline bool isEnum() const { return rxtEnum == m_restriction.getType(); }
	inline const PropertyInfo::Restriction& getRestriction() const { return m_restriction; }
	inline const EsString& getName() const { return m_name; }
	inline EsString getDescr() const 
	{ 
		if( m_descr.empty() ) 
			return m_name; 
		else 
			return wxGetTranslation( m_descr.c_str() ); 
	}
	inline bool canRead() const { return 0 != m_reader; }
	inline bool canWrite() const { return 0 != m_writer; }
	// generic value access
	EsVariant get(const EsBaseIntf* obj) const;
	void set(EsBaseIntf* obj, const EsVariant& val) const;
	// reset property value to default, if there is one. if property has no default, exception is thrown
	void reset(EsBaseIntf* obj) const;
	// enumeration-only services
	//
	// return label for enum value
	EsString getValueLabel(const EsVariant& val) const;
	// return all value labels
	EsString::Array getAllLabels() const;
	// return all enum values
	EsVariant getAllValues() const;
	// return enum item at index. item may be of collection type, if value label is provided
	EsVariant getEnumItem(size_t idx) const;
	// return enum item value at index
	EsVariant getEnumValue(size_t idx) const;

protected:
	// validation helpers
	bool validateMin(const EsVariant& val, bool doThrow) const;
	bool validateMax(const EsVariant& val, bool doThrow) const;
	bool validateRange(const EsVariant& val, bool doThrow) const;
	bool validateEnum(const EsVariant& val, bool doThrow) const;

private:
	// prohibited services
	PropertyInfo();
	PropertyInfo( const PropertyInfo& );
	PropertyInfo& operator=( const PropertyInfo& );

protected:
	const ClassInfo& m_owner;
	EsString m_name;
	EsString m_descr;
	EsVariant m_def;
	EsReflectedClassIntf::CallT m_reader;
	int m_readerSignature;
	EsReflectedClassIntf::CallT m_writer;
	int m_writerSignature;
	Restriction m_restriction;
};

// metaclass information holder
//
class EKOSF_CORE_CLASS ClassInfo
{
public:
	typedef std::list<const PropertyInfo*> PropertyInfosT;
	typedef std::map<MethodInfoKeyT, const MethodInfo*> MethodInfosT;
	// class info reference container wrapper class.
	// used for whole class info registrar, as well as for immediate 
	// reflected base classes registrar
	//
	class EKOSF_CORE_CLASS ClassInfosT
	{
	private:
		typedef containers::structured_map<EsString, const ClassInfo*> MapT;
	
	public:
		typedef MapT::const_iterator IteratorT;

	public:
		// default ctor
		ClassInfosT() {}
		// chaining-capable item addition
		ClassInfosT& add(const ClassInfo* info);
		// remove item by name
		inline void erase(const EsString& name) { m_infos.erase(name); }
		// serch item by name
		const ClassInfo* find(const EsString& name) const;
		// checking
		inline bool empty() const { return m_infos.empty(); }
		// iteration
		inline IteratorT begin() const { return m_infos.begin(); }
		inline IteratorT end() const { return m_infos.end(); }
		// size access
		inline size_t size() const { return m_infos.size(); }

	private:
#pragma warning(push)
#pragma warning(disable: 4251)
		MapT m_infos;
#pragma warning(pop)
	};

public:
	ClassInfo(const EsString& name, const EsString& descr, const ClassInfosT& ancestors);
	~ClassInfo();

	// inheritance access
	bool isKindOf(const EsString& name) const;
	inline bool isKindOf(const ClassInfo& info) const { return isKindOf( info.getName() ); }
	EsString::Array getImmediateAncestors() const;
	// class info access
	inline const EsString& getName() const { return m_name; }
	inline EsString getDescr() const 
	{ 
		if(m_descr.empty()) 
			return m_name; 
		else 
			return wxGetTranslation(m_descr.c_str()); 
	}
	// reflected property information access
	EsString::Array getPropertyNames(bool onlyPersistent = false, bool allHierarchy = true) const;
	EsString::Array getPersistentPropertyNames(bool allHierarchy = true) const
	{ return getPropertyNames(true, allHierarchy); }
	bool hasProperty(const EsString& name, bool allHierarchy = true) const;
	const PropertyInfo* findPropertyInfo(const EsString& name, bool allHierarchy = true ) const;
	const PropertyInfo& getPropertyInfo(const EsString& name, bool allHierarchy = true ) const;
	PropertyInfosT getPropertyInfos(bool onlyPersistent = false, bool allHierarchy = true) const;
	PropertyInfosT getPersistentPropertyInfos(bool allHierarchy = true) const
	{ return getPropertyInfos(true, allHierarchy); }
	// reflected method infos access
	MethodInfoKeysT getMethodKeys(bool allHierarchy = true) const;
	bool hasMethod( const MethodInfoKeyT& key, bool allHierarchy = true ) const;
	const MethodInfo* findMethodInfo(const MethodInfoKeyT& key, bool allHierarchy = true ) const;
	const MethodInfo& getMethodInfo(const MethodInfoKeyT& key, bool allHierarchy = true ) const;
	MethodInfoKeysT getClassMethodKeys(bool allHierarchy = false) const;
	const MethodInfo* findClassMethodInfo(const MethodInfoKeyT& key, bool allHierarchy = true ) const;	
	const MethodInfo& getClassMethodInfo(const MethodInfoKeyT& key, bool allHierarchy = false ) const;
	// property services simplified
	EsVariant propertyGet(EsBaseIntf* obj, const EsString& name) const;
	void propertySet(EsBaseIntf* obj, const EsString& name, const EsVariant& val) const;
	void propertyReset(EsBaseIntf* obj, const EsString& name) const;
	void resetAllProperties(EsBaseIntf* obj) const;
	// generic checks
	//
	// isIndexed returns true if standard indexed access services are registered for the class:
	// getCount() and getItem(index) and | or setItem(index, value)
	// 
	bool isIndexed() const;
	// method services simplified
	EsVariant call(EsBaseIntf* obj, const EsString& name) const;
	EsVariant call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1) const;
	EsVariant call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2) const;
	EsVariant call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) const;	
	EsVariant call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) const;	
	EsVariant call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) const;		
	EsVariant call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6) const;	
	EsVariant callMethod(EsBaseIntf* obj, const EsString& name, const EsVariant& params) const;
	EsVariant classCall(const EsString& name) const;
	EsVariant classCall(const EsString& name, const EsVariant& param1) const;
	EsVariant classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2) const;
	EsVariant classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) const;	
	EsVariant classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) const;	
	EsVariant classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) const;		
	EsVariant classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6) const;	
	EsVariant classCallMethod(const EsString& name, const EsVariant& params) const;
	// static services. registered reflected classes access
	static EsString::Array getClassNames();
	static const ClassInfo* getClassInfo(const EsString& name, bool Throw = false );

protected:
	// use "local static init on first use" idiom for the class info registry
	static ClassInfosT& classes();
	// misc registration helpers
	void appendPropertyNames(EsString::Array& out, bool onlyPersistent, bool allHierarchy) const;
	void appendPropertyInfos(PropertyInfosT& out, bool onlyPersistent, bool allHierarchy) const;
	void appendMethodKeys(MethodInfoKeysT& out, bool allHierarchy) const;
	void appendClassMethodNames(MethodInfoKeysT& out, bool allHierarchy) const;
	
protected:
	// ancestor classes
	ClassInfosT m_ancestors;
	// class name
	EsString m_name;
	// class description
	EsString m_descr;
#pragma warning(push)
#pragma warning(disable: 4251)
	// reflected properties
	PropertyInfosT m_propInfos;
	// normal reflected services
	MethodInfosT m_methods;
	// reflected class services
	MethodInfosT m_classMethods;
#pragma warning(pop)

private:
	ClassInfo();
	ClassInfo(const ClassInfo&);
	ClassInfo& operator=(const ClassInfo&);

	friend class PropertyInfo;
	friend class MethodInfo;
};

#include <escore/ReflectionMethodDeclDefs.h>
#include <escore/ReflectionMethodInfoDeclDefs.h>
#include <escore/ReflectionPropertyDeclDefs.h>
#include <escore/ReflectionReflectedClassDefs.h>

#define ES_THIS_REFLECTED_PROP_INFO_GET(Name) \
	(&getClassInfoStatic().getPropertyInfo( wxT(#Name) ))

#define ES_REFLECTED_PROP_INFO_GET(Class, Name) \
	(& Class## ::getClassInfoStatic().getPropertyInfo( wxT(#Name) ))

// reflection registrar declaration and implementation macros
//
#ifdef EKOSF_USEDLLS

#	define ES_DECL_REFLECTION_REGISTRAR(ModuleName)

#	define ES_REFLECTION_REGISTRAR_BEGIN(ModuleName) \
	void reflectionInit() {

#else

#	define ES_DECL_REFLECTION_REGISTRAR(ModuleName) \
	namespace ModuleName { void reflectionInit(); }

#	define ES_REFLECTION_REGISTRAR_BEGIN(ModuleName) \
	void ModuleName## ::reflectionInit() {

#endif

#define ES_REFLECTION_REGISTRAR_ENTRY(ClassName) \
	ClassName## ::getClassInfoStatic();

#define ES_REFLECTION_REGISTRAR_END }

#endif // _Reflection_h_