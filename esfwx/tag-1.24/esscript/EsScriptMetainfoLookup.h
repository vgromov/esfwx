#ifndef _es_script_metainfo_lookup_h_
#define _es_script_metainfo_lookup_h_

/// Script metaclass lookup services
///
class ESSCRIPT_CLASS ES_INTF_IMPL1(EsScriptMetainfoLookup, EsReflectedClassIntf)

public:
  EsScriptMetainfoLookup(const EsMetaclassIntf::Ptr& meta) ES_NOTHROW;

  ES_DECL_REFLECTED_CLASS_BASE(EsScriptMetainfoLookup)
  ES_DECL_ATTR_HANDLING_STD

  /// Reflected stuff
  ///

  /// Reflected constructor, taking metaclass instance
  ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, cr_EsBaseIntfPtr);

  /// Return array of object names which start with arg, if not empty. If null string is passed, return all registered reflected object names.
  /// The result is sorted alphabetically.
  /// @param [in, optional] start   - Starting object or namespace name lookup pattern
  /// @return                       - An array of found object or namespace names
  /// 
  ES_DECL_REFLECTED_CONST_METHOD1(EsStringArray, objectNamesFind, cr_EsString);

  /// Return array of global function call human-readable signatures, which start with arg, if not empty. 
  /// If null string is passed, return all global function signatures.
  /// The result is sorted alphabetically.
  /// @param [in, optional] start   - Starting global function name lookup pattern
  /// @return                       - An array of found global functions human-readable signature strings
  /// 
  ES_DECL_REFLECTED_CONST_METHOD1(EsStringArray, globalFunctionsFind, cr_EsString);

  /// Return array of variant services human-readable signatures, which start with arg, if not empty. 
  /// If null string is passed, return all variant services signatures.
  /// The result is sorted alphabetically.
  /// @param [in, optional] start   - Starting service name lookup pattern
  /// @return                       - An array of found variant service human-readable signature strings
  /// 
  ES_DECL_REFLECTED_CONST_METHOD1(EsStringArray, variantServicesFind, cr_EsString);

  /// Return array of static object service human-readable signatures, which start with arg, if not empty. 
  /// If null string is passed, return all object static services human-readable signatures.
  /// The result is sorted alphabetically.
  /// If an object or namespace is not registered, return an empty array.
  /// @param [in]           nsOrObj - Full namespace or object name string to lookup for static services
  /// @param [in, optional] start   - Starting service name lookup pattern
  /// @return                       - An array of found static services human-readable signature strings
  /// 
  ES_DECL_REFLECTED_CONST_METHOD2(EsStringArray, objectStaticMethodsFind, cr_EsString, cr_EsString);

  /// Return array of object service human-readable signatures, which start with arg, if not empty. 
  /// If null string is passed, return all object services human-readable signatures.
  /// The result is sorted alphabetically.
  /// If an object or namespace is not registered, return an empty array.
  /// @param [in]           nsOrObj - Full namespace or object name string to lookup for services
  /// @param [in, optional] start   - Starting service name lookup pattern
  /// @return                       - An array of found services human-readable signature strings
  /// 
  ES_DECL_REFLECTED_CONST_METHOD2(EsStringArray, objectMethodsFind, cr_EsString, cr_EsString);

  /// Return array of object variables names, which start with arg, if not empty. 
  /// If null string is passed, return all object variables.
  /// The result is sorted alphabetically.
  /// If an object or is not registered, return an empty array.
  /// @param [in]           nsOrObj - Full object name string to lookup for variables
  /// @param [in, optional] start   - Starting variable name lookup pattern
  /// @return                       - An array of found variable names
  /// 
  ES_DECL_REFLECTED_CONST_METHOD2(EsStringArray, objectVariablesFind, cr_EsString, cr_EsString);
  
  /// Return array of object fields names, which start with arg, if not empty. 
  /// If null string is passed, return all object fields names.
  /// The result is sorted alphabetically.
  /// If an object or is not registered, return an empty array.
  /// @param [in]           nsOrObj - Full object name string to lookup for fields
  /// @param [in, optional] start   - Starting field name lookup pattern
  /// @return                       - An array of found field names
  /// 
  ES_DECL_REFLECTED_CONST_METHOD2(EsStringArray, objectFieldsFind, cr_EsString, cr_EsString);

  /// Return array of object property names, which start with arg, if not empty. 
  /// If null string is passed, return all object property names.
  /// The result is sorted alphabetically.
  /// If an object or is not registered, return an empty array.
  /// @param [in]           nsOrObj - Full object name string to lookup for properties
  /// @param [in, optional] start   - Starting property name lookup pattern
  /// @return                       - An array of found property names
  /// 
  ES_DECL_REFLECTED_CONST_METHOD2(EsStringArray, objectPropertiesFind, cr_EsString, cr_EsString);

  /// Return array of enumeration names, which start with arg, if not empty. 
  /// If null string is passed, return all registered enumeration names.
  /// The result is sorted alphabetically.
  /// @param [in, optional] start   - Starting enumeration name lookup pattern
  /// @return                       - An array of found enumeration names
  /// 
  ES_DECL_REFLECTED_CONST_METHOD1(EsStringArray, enumNamesFind, cr_EsString);

  /// Return array of enumeration symbol names, which start with arg, if not empty. 
  /// If null string is passed, return all enumeration symbol names.
  /// The result is sorted alphabetically.
  /// @param [in]           enu     - Full enumeration name string to lookup for symbols
  /// @param [in, optional] start   - Starting enumeration name lookup pattern
  /// @return                       - An array of found enumeration symbol names
  /// 
  ES_DECL_REFLECTED_CONST_METHOD2(EsStringArray, enumSymbolsFind, cr_EsString, cr_EsString);

  /// Return global variables names
  ES_DECL_REFLECTED_CONST_METHOD1(EsStringArray, globalVarsFind, cr_EsString);

  /// Return system variables names
  ES_DECL_REFLECTED_CONST_METHOD1(EsStringArray, systemVarsFind, cr_EsString);

  /// Return constants names
  ES_DECL_REFLECTED_CONST_METHOD1(EsStringArray, constantsFind, cr_EsString);

  /// Return local variables, given namespace, code section name, and lookup start filter
  /// @param [in, optional]  ns     - Namespace (object) name to search method in. If null string is used - lookup global scripted methods
  /// @param [in]       mangledName - Magled method name, i.e. name|argument count
  /// @param [in, optional]  start  - Starting variable name lookup pattern
  /// @return                       - An array of found local variable names
  ///
  ES_DECL_REFLECTED_CONST_METHOD3(EsStringArray, localVarsFind, cr_EsString, cr_EsString, cr_EsString);

  /// Return script reserved words using lookup pattern
  ES_DECL_REFLECTED_CONST_METHOD2(EsStringArray, keywordsFind, cr_EsString, cr_EsString);

  /// Properties
  ///
  
  /// Read-only access to attached metaclass instance
  ES_DECL_PROPERTY_RO(metaclass, EsBaseIntfPtr)

protected:
  EsScriptMachine* vm() const;

protected:
  void enumerationsRemove(EsStringArray& objs) const;

protected:
  EsMetaclassIntf::Ptr m_meta;

private:
  EsScriptMetainfoLookup() ES_REMOVEDECL;
  EsScriptMetainfoLookup(const EsScriptMetainfoLookup&) ES_REMOVEDECL;
  EsScriptMetainfoLookup& operator=(const EsScriptMetainfoLookup&) ES_REMOVEDECL;
};

#endif // _es_script_metainfo_lookup_h_
