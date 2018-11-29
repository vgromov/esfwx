#ifndef _es_enumeration_h_
#define _es_enumeration_h_

// enumeration API interface
ES_DECL_INTF_BEGIN2( CBDD7DC0, F0A0473C, 86947623, B263AB77, EsEnumerationIntf, EsBaseIntf)
	// items insertion
	ES_DECL_INTF_METHOD(void, itemAdd)(const EsString& symbol, const EsVariant& val, const EsString& label) = 0;
	// enumeration may represent itself as an idexed object, as values collection
	ES_DECL_INTF_METHOD(ulong, countGet)() const ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(EsVariant, itemGet)(ulong) const = 0;
	// emuneration-specific methods
	ES_DECL_INTF_METHOD(bool, valueExists)(cr_EsVariant) const ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(EsVariant, valueGet)(cr_EsString) const = 0;
	ES_DECL_INTF_METHOD(EsString, valueLabelGet)(cr_EsVariant) const = 0;
	ES_DECL_INTF_METHOD(ulong, valueIndexGet)(cr_EsVariant) const = 0;
	ES_DECL_INTF_METHOD(EsString, valueSymbolGet)(cr_EsVariant) const = 0;
	ES_DECL_INTF_METHOD(EsString, symbolLabelGet)(cr_EsString) const = 0;
	ES_DECL_INTF_METHOD(bool, hasSymbol)(cr_EsString) const ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(EsStringArray, symbolNamesGet)() const ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(EsStringArray, labelsGet)() const ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(EsVariant, valuesGet)() const ES_NOTHROW = 0;
ES_DECL_INTF_END

// enumeration class base. all enumerations are
// singletons, created at the moment of their declaration.
// singleton instances are accessed in C++ via static 'instanceGet' method
//
class ESCORE_CLASS ES_INTF_IMPL2( EsEnumeration, EsEnumerationIntf, EsReflectedClassIntf)

protected:
  enum EsEnumerationNonDynamic { enumerationNonDynamic };

	// friends-only services
	EsEnumeration(const EsString& enumTypeName) ES_NOTHROW;
  EsEnumeration(const EsString& enumTypeName, EsEnumerationNonDynamic dummy) ES_NOTHROW;
	
public:
	// reflection declaration
	//
	ES_DECL_REFLECTED_CLASS_BASE( EsEnumeration )

	// EsReflectedClassIntf attribute handling specifics
	ES_DECL_INTF_METHOD(void, attributesAssign)(const EsAttributesIntf::Ptr& attrs) ES_NOTHROW ES_OVERRIDE;
	ES_DECL_INTF_METHOD(void, attributeAdd)(const EsString& name, const EsVariant& val = EsVariant::null()) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, hasAttribute, cr_EsString) ES_NOTHROW ES_OVERRIDE;
	ES_DECL_INTF_METHOD(EsString::Array, attributeNamesGet)() const ES_NOTHROW ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, attributeGet, cr_EsString) ES_NOTHROW ES_OVERRIDE;
	
	// reflected EsEnumerationIntf services
	//
	// items insertion
	ES_DECL_REFLECTED_INTF_METHOD3(void, itemAdd, cr_EsString, cr_EsVariant, cr_EsString) ES_OVERRIDE;
	// enumeration may represent itself as an idexed object, as values collection
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(ulong, countGet) ES_NOTHROW ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, itemGet, ulong) ES_OVERRIDE;
	// emuneration-specific methods
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, valueExists, cr_EsVariant) ES_NOTHROW ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, valueGet, cr_EsString) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsString, valueLabelGet, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(ulong, valueIndexGet, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsString, valueSymbolGet, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsString, symbolLabelGet, cr_EsString) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, hasSymbol, cr_EsString) ES_NOTHROW ES_OVERRIDE;
	ES_DECL_INTF_METHOD(EsStringArray, symbolNamesGet)() const ES_NOTHROW ES_OVERRIDE { return m_contents.namesGet(); }
	ES_DECL_INTF_METHOD(EsStringArray, labelsGet)() const ES_NOTHROW ES_OVERRIDE;
	ES_DECL_INTF_METHOD(EsVariant, valuesGet)() const ES_NOTHROW ES_OVERRIDE;
	// reflected properties
	ES_DECL_PROPERTY_RO(symbols, EsStringArray)
	ES_DECL_PROPERTY_RO(values, EsVariant)
	ES_DECL_PROPERTY_RO(labels, EsStringArray)

protected:
	EsString labelGet(ulong idx) const;

protected:
	// data members
	//
	EsStringIndexedMap m_contents;
	EsAttributesIntf::Ptr m_attrs;
};

// actual enumeration declaration macros
//
// internal reflected enumeration declaration macro. used in several enumeration declarations macros
// must not be used directly for reflected enums declarations
#define ES_DECL_EXPORTED_ENUMERATION_BEGIN_INTERNAL(ExportSpec, EnumerationName) \
class ExportSpec ES_INTF_IMPL_DERIVED(EnumerationName, EsEnumeration) \
protected: \
	EnumerationName(const EsString& enumTypeName) : EsEnumeration(enumTypeName, EsEnumeration::enumerationNonDynamic) { itemsInit(); } \
public: \
	ES_DECL_REFLECTED_CLASS(EnumerationName, EsEnumeration) \
	virtual EsString typeNameGet() const ES_NOTHROW ES_OVERRIDE { return m_contents.nameGet(); } \
	static EsEnumerationIntf::Ptr instanceGet() ES_NOTHROW; \
	ES_DECL_REFLECTED_CLASS_METHOD0(EsVariant, instance) ES_NOTHROW; \
private: \
	void itemsInit()

// start reflected and dll-exported enumeration declaration. enumeration items declarations must be included as well
// inside declaration BEGIN ... END scope
#define ES_DECL_EXPORTED_ENUMERATION_BEGIN(ExportSpec, EnumerationName) \
	ES_DECL_EXPORTED_ENUMERATION_BEGIN_INTERNAL(ExportSpec, EnumerationName) {

// start reflected and dll-exported enumeration declaration. 
// separate enumeration items declarations must be included in cpp file inside corresponding INFO declaration BEGIN ... END scope
#define ES_DECLONLY_EXPORTED_ENUMERATION_BEGIN(ExportSpec, EnumerationName) \
	ES_DECL_EXPORTED_ENUMERATION_BEGIN_INTERNAL(ExportSpec, EnumerationName); };

// start reflected enumeration declaration. enumeration items declarations must be included as well
// inside declaration BEGIN ... END scope
#define ES_DECL_ENUMERATION_BEGIN(EnumerationName) \
	ES_DECL_EXPORTED_ENUMERATION_BEGIN(ES_NO_EXPORT_SPEC, EnumerationName)

// start reflected enumeration declaration.
// separate enumeration items declarations must be included in cpp file inside corresponding INFO declaration BEGIN ... END scope
#define ES_DECLONLY_ENUMERATION_BEGIN(EnumerationName) \
	ES_DECLONLY_EXPORTED_ENUMERATION_BEGIN(ES_NO_EXPORT_SPEC, EnumerationName)

// label-less enumeration item declaration macro
#define ES_DECL_ENUMERATION_ITEM(Symbol, Value) \
	itemAdd(ES_STRINGIZE(Symbol), (Value), EsString::null());

// labelled enumeration item declaration macro
#define ES_DECL_ENUMERATION_ITEM_LABELLED(Symbol, Value, Label) \
	itemAdd(ES_STRINGIZE(Symbol), (Value), (Label));

// enumeration declaration scope 
#define ES_DECL_ENUMERATION_END } };

// enumeration reflection macros (to bo placed in cpp)
//
// internal macro for reflected enumeration info definition. must not be used directly, only for 
// macro code re-use in several end-user macros defined below
#define ES_DECL_ENUMERATION_INFO_INTERNAL( EnumerationName, Descr ) \
	EsEnumerationIntf::Ptr EnumerationName ::instanceGet() ES_NOTHROW { \
	static EnumerationName s_instance(ES_STRINGIZE(EnumerationName)); \
	return s_instance.asBaseIntfPtrDirect(); \
} \
EsVariant EnumerationName::instance() ES_NOTHROW { \
	return instanceGet(); } \
ES_DECL_CLASS_INFO_DERIVED_BEGIN( EnumerationName, EsEnumeration, Descr ) \
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EnumerationName, instance, EsVariant_ClassCall, esT("Access ") ES_STRINGIZE(EnumerationName) esT(" enumeration instance")) \
ES_DECL_CLASS_INFO_END

// macro for reflected enumeration info declaration. to be placed in cpp
#define ES_DECL_ENUMERATION_INFO( EnumerationName, Descr ) \
	ES_DECL_ENUMERATION_INFO_INTERNAL( EnumerationName, Descr )

// enumeration reflection macros scope for separate enumeration items info declarations (to bo placed in cpp)
//
#define ES_DECL_ENUMERATION_INFO_BEGIN( EnumerationName, Descr ) \
	ES_DECL_ENUMERATION_INFO_INTERNAL( EnumerationName, Descr ) \
	void EnumerationName ::itemsInit() {

#define ES_DECL_ENUMERATION_INFO_END }

// enumeration symbolic access macros
//
#define ES_ENUM(EnumerationName) \
	EnumerationName ::instance()

template <typename EnumT>
bool EsEnumHasValue(const EsVariant& val) ES_NOTHROW
{
	return EnumT::instanceGet()->valueExists(val);
}

template <typename EnumT>
EsString::Array EsEnumSymbolsGet() ES_NOTHROW
{
	return EnumT::instanceGet()->symbolNamesGet();
}

template <typename EnumT>
EsVariant::Array EsEnumValuesGet() ES_NOTHROW
{
	return EnumT::instanceGet()->valuesGet().asVariantCollection();
}

template <typename EnumT>
EsString::Array EsEnumLabelsGet() ES_NOTHROW
{
	return EnumT::instanceGet()->labelsGet();
}

template <typename EnumT>
EsVariant EsEnumItemGet(const EsString& itemName)
{
	return EnumT::instanceGet()->valueGet(itemName);
}

template <typename EnumT>
EsString EsEnumSymbolGet(const EsVariant& value)
{
	return EnumT::instanceGet()->valueSymbolGet(value);
}

template <typename EnumT>
EsString EsEnumLabelGet(const EsVariant& value)
{
	return EnumT::instanceGet()->valueLabelGet(value);
}

#define ES_ENUM_ITEM(EnumerationName, ItemName) \
	EsEnumItemGet<EnumerationName>( ES_STRINGIZE(ItemName) )
//---------------------------------------------------------------------------

#define ES_SCRIPT_ENUM_ITEM(ScriptVar, EnumerationName, ItemName) \
	ScriptVar.call( "enumEval", EsVariant( ES_STRINGIZE(EnumerationName) esT("$$") ES_STRINGIZE(ItemName), EsVariant::ACCEPT_STRING) )
  //---------------------------------------------------------------------------

#endif // _es_enumeration_h_
