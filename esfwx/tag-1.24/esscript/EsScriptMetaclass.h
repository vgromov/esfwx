#ifndef _es_script_metaclass_h_
#define _es_script_metaclass_h_

/// Script metaclass services
///
class ESSCRIPT_CLASS ES_INTF_IMPL2_NON_RC(EsScriptMetaclass, EsMetaclassIntf, EsReflectedClassIntf)

protected:
	EsScriptMetaclass(const EsScriptMachine& owner) ES_NOTHROW;

public:
	ES_DECL_REFLECTED_CLASS_BASE(EsScriptMetaclass)
	ES_DECL_ATTR_HANDLING_STD

	/// Script-specific EsMetaclassIntf reflected implementation
	///
	/// Inheritance access
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsStringArray, objectNamesGet) ES_NOTHROW;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsStringArray, baseObjectNamesGet, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, hasClass, cr_EsString) ES_NOTHROW;
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(bool, isKindOf, cr_EsString, cr_EsString);
	/// Class attributes access
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(bool, hasAttribute, cr_EsString, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsStringArray, attributeNamesGet, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(EsVariant, attributeGet, cr_EsString, cr_EsString);
	/// Method info access
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsStringArray, methodNamesGet, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsStringArray, classMethodNamesGet, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsStringArray, thisMethodNamesGet, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsStringArray, thisClassMethodNamesGet, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(bool, hasMethod, cr_EsString, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(bool, hasClassMethod, cr_EsString, cr_EsString);
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsStringArray, fqNamesGet, cr_EsString);
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsStringArray, thisFqNamesGet, cr_EsString);

	/// Method attributes access by FQ method name
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsStringArray, methodAttributeNamesGet, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(bool, methodHasAttribute, cr_EsString, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(EsVariant, methodAttributeGet, cr_EsString, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsString, methodDescriptionGet, cr_EsString);

	/// Property info access
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsStringArray, propertyNamesGet, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsStringArray, thisPropertyNamesGet, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsStringArray, persistentPropertyNamesGet, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsStringArray, thisPersistentPropertyNamesGet, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(bool, hasProperty, cr_EsString, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(bool, propertyCanRead, cr_EsString, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(bool, propertyCanWrite, cr_EsString, cr_EsString);

	/// Property attributes access
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(EsStringArray, propertyAttributeNamesGet, cr_EsString, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD3(bool, propertyHasAttribute, cr_EsString, cr_EsString, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD3(EsVariant, propertyAttributeGet, cr_EsString, cr_EsString, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(EsString, propertyLabelGet, cr_EsString, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(EsString, propertyDescriptionGet, cr_EsString, cr_EsString);

  /// Enumeration info access
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsStringArray, enumerationNamesGet);
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsStringArray, enumerationSymbolsGet, cr_EsString);

	/// Shortcuts for creating objects with different count of parameters passed to ctor
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, objectCreate, cr_EsString);
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(EsVariant, objectCreate, cr_EsString, cr_EsVariant);
	ES_DECL_REFLECTED_INTF_CONST_METHOD3(EsVariant, objectCreate, cr_EsString, cr_EsVariant, cr_EsVariant);
	ES_DECL_REFLECTED_INTF_CONST_METHOD4(EsVariant, objectCreate, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant);
	ES_DECL_REFLECTED_INTF_CONST_METHOD5(EsVariant, objectCreate, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);
	ES_DECL_REFLECTED_INTF_CONST_METHOD6(EsVariant, objectCreate, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);

  /// Not part of EsMetaclassIntf, extended scripted information access using reflection
  ///

  /// Return script machine pointer
  ES_DECL_REFLECTED_CONST_METHOD0(EsVariant, scriptMachineGet);

protected:
  EsStringArray internalFqNamesGet(cr_EsString className, bool allHierarchy) const;
  EsAttributesIntf::Ptr methodAttrsAccess(cr_EsString fqName) const;

protected:
	const EsScriptMachine& m_owner;

private:
	EsScriptMetaclass(const EsScriptMetaclass&) ES_REMOVEDECL;
	const EsScriptMetaclass& operator=(const EsScriptMetaclass&) ES_REMOVEDECL;

	friend class EsScriptMachine;
};

#endif // _es_script_metaclass_h_