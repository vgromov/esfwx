#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptMetaclass.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsScriptMetaclass, NO_CLASS_DESCR)
	// Interface services
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, objectNamesGet, EsStringArray_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, baseObjectNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, hasClass, bool_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, isKindOf, bool_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, hasAttribute, bool_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, attributeNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, attributeGet, EsVariant_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, methodNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, classMethodNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, thisMethodNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, thisClassMethodNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, hasMethod, bool_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, hasClassMethod, bool_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, fqNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, thisFqNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, methodAttributeNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, methodHasAttribute, bool_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, methodAttributeGet, EsVariant_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, methodDescriptionGet, EsString_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, propertyNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, thisPropertyNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, persistentPropertyNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, thisPersistentPropertyNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, hasProperty, bool_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, propertyCanRead, bool_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, propertyCanWrite, bool_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, propertyAttributeNamesGet, EsStringArray_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, propertyHasAttribute, bool_CallConst_cr_EsString_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, propertyAttributeGet, EsVariant_CallConst_cr_EsString_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, propertyLabelGet, EsString_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, propertyDescriptionGet, EsString_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, enumerationNamesGet, EsStringArray_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, enumerationSymbolsGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, objectCreate, EsVariant_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, objectCreate, EsVariant_CallConst_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, objectCreate, EsVariant_CallConst_cr_EsString_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, objectCreate, EsVariant_CallConst_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, objectCreate, EsVariant_CallConst_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptMetaclass, EsMetaclassIntf, objectCreate, EsVariant_CallConst_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)

  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetaclass, scriptMachineGet, EsVariant_CallConst, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsScriptMetaclass::EsScriptMetaclass(const EsScriptMachine& owner) ES_NOTHROW :
m_owner(owner)
{}
//---------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::objectNamesGet() const ES_NOTHROW
{
	EsStringArray names = EsClassInfo::classNamesGet();
	const EsStringArray& metaNames = m_owner.metaclassNamesGet();
	names.reserve(names.size() + metaNames.size());
	names.insert(names.end(), metaNames.begin(), metaNames.end());

	return names;
}
//---------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::baseObjectNamesGet(const EsString& className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);
	EsStringArray result;

	if(info)
	{
		if( info->hasAncestor() )
			result.push_back( info->ancestorGet()->nameGet() );
	}
	else
	{
		EsScriptObjectIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);
		metaclass = metaclass->ancestorGet();
		if( metaclass )
			result.push_back( metaclass->typeNameGet() );
	}

	return result;
}
//---------------------------------------------------------------------------

bool EsScriptMetaclass::hasClass(const EsString& className) const ES_NOTHROW
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);
	if(info)
		return true;
	else
		return m_owner.metaclassFind(
      className,
      false
    );
}
//---------------------------------------------------------------------------

bool EsScriptMetaclass::isKindOf(const EsString& className, const EsString& otherName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);

	if( info )
		return info->isKindOf(otherName);
	else
	{
		EsReflectedClassIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		return metaclass->isKindOf(otherName);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/// Query if class has attribute
bool EsScriptMetaclass::hasAttribute(cr_EsString className, cr_EsString attrName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);

	if( info )
	{
		EsAttributesIntf::Ptr attrs = info->attributesAccess();
		if( attrs )
			return attrs->attributeExists(attrName);
	}
	else
	{
		EsReflectedClassIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		return metaclass->hasAttribute(attrName);
	}

	return false;
}
//---------------------------------------------------------------------------

/// Query names of all attributes for the specified class
EsStringArray EsScriptMetaclass::attributeNamesGet(cr_EsString className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);

	EsStringArray result;
	if( info )
	{
		EsAttributesIntf::Ptr attrs = info->attributesAccess();
		if( attrs )
			result = attrs->allNamesGet();
	}
	else
	{
		EsReflectedClassIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		result = metaclass->attributeNamesGet();
	}

	return result;
}
//---------------------------------------------------------------------------

/// Return value associated with specified attribute
EsVariant EsScriptMetaclass::attributeGet(cr_EsString className, cr_EsString attrName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);

	if( info )
	{
		EsAttributesIntf::Ptr attrs = info->attributesAccess();
		if( attrs )
			return attrs->attributeGet(attrName);
		else
			EsException::Throw(
        esT("No attributes declared for '%s'"),
        className
      );
	}
	else
	{
		EsReflectedClassIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		return metaclass->attributeGet(attrName);
	}

	// pacify compilers
	return EsVariant::null();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::methodNamesGet(const EsString& className) const
{
	EsStringArray result;
  EsMethodInfoKeysT keys;

  if( className.empty() )
  {
    const EsScriptMethodMap& globalMethods = m_owner.globalMethodsMapGet();
    keys.reserve(globalMethods.size());
    for( auto const& globalMethod : globalMethods )
    {
      keys.push_back(
        globalMethod.first
      );
    }
  }
  else
  {
    const EsClassInfo* info = EsClassInfo::classInfoGet(className);
    if( info )
      keys = info->methodKeysGet();
    else
    {
      EsScriptObjectIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
      ES_ASSERT(metaclass);
      result = methodNamesGet(metaclass->classNameGet());
      keys = metaclass->scriptedMethodKeysGet();
    }
  }

  result.reserve(result.size() + keys.size());
  for( size_t idx = 0; idx < keys.size(); ++idx )
    result.push_back(keys[idx].asString());

	return result;
}
//---------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::classMethodNamesGet(const EsString& className) const
{
	return EsMetaclass::instance()->classMethodNamesGet(className);
}
//---------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::thisMethodNamesGet(const EsString& className) const
{
	EsStringArray result;
	EsMethodInfoKeysT keys;

	const EsClassInfo* info = EsClassInfo::classInfoGet(className);
	if( info )
		keys = info->methodKeysGet(false);
	else
	{
		EsScriptObjectIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);
		result = thisMethodNamesGet( metaclass->classNameGet() );
		keys = metaclass->scriptedMethodKeysGet(false);
	}

	result.reserve( result.size() + keys.size() );
	for( size_t idx = 0; idx < keys.size(); ++idx )
		result.push_back( keys[idx].asString() );

	return result;
}
//---------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::thisClassMethodNamesGet(const EsString& className) const
{
	return EsMetaclass::instance()->thisClassMethodNamesGet(className);
}
//---------------------------------------------------------------------------

bool EsScriptMetaclass::hasMethod(const EsString& className, const EsString& methodName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);

	EsMethodInfoKeyT key(methodName);

	if( info )
		return info->hasMethod(key);
	else
	{
		EsReflectedClassIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		return metaclass->hasMethod(key);
	}
}
//---------------------------------------------------------------------------

bool EsScriptMetaclass::hasClassMethod(const EsString& className, const EsString& methodName) const
{
	return EsMetaclass::instance()->hasClassMethod(className, methodName);
}
//---------------------------------------------------------------------------

static EsString signatureStrFromScriptedMethodKey(const EsMethodInfoKeyT& key)
{
  ES_ASSERT( key.isOk() );
  size_t cnt = key.parametersCountGet();

  EsString result = esT("EsVariant_Call");
  for(size_t idx = 0; idx < cnt; ++idx)
    result += esT("_cr_EsVariant");

  return result;
}
//---------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::internalFqNamesGet(cr_EsString className, bool allHierarchy) const
{
  EsString::Array result;

  if( className.empty() )
  { 
    const EsScriptMethodMap& globalMethods = m_owner.globalMethodsMapGet();
    result.reserve(
      globalMethods.size()
    );

    for( auto const& methodItem : globalMethods )
    {
      const EsMethodInfoKeyT& key = methodItem.first;
      const EsString& sig = signatureStrFromScriptedMethodKey(key);
      const EsString& fqName =
        esT("::") +
        key.asString() +
        esT("|") +
        sig;

      result.push_back(
        fqName
      );
    }
  }
  else
  {
    const EsClassInfo* info = EsClassInfo::classInfoGet(className);

    if( info )
      return info->fqMethodNamesGet(allHierarchy);

    EsScriptObjectIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
    ES_ASSERT(metaclass);

    result = fqNamesGet(metaclass->classNameGet()); //< Get C++ reflected method FQ names
    EsMethodInfoKeysT keys = metaclass->scriptedMethodKeysGet(allHierarchy); //< Get scripted methods keys.

    result.reserve(
      result.size() +
      keys.size()
    );

    const EsString& metaclassName = metaclass->typeNameGet();

    for( auto const& key: keys )
    {
      const EsString& sig = signatureStrFromScriptedMethodKey(key);
      const EsString& fqName = metaclassName + esT(".") + key.asString() + esT("|") +
        sig;

      result.push_back(
        fqName
      );
    }
  }

  return result;
}
//---------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::fqNamesGet(cr_EsString className) const
{
  return internalFqNamesGet(className, true);
}
//---------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::thisFqNamesGet(cr_EsString className) const
{
  return internalFqNamesGet(className, false);
}
//---------------------------------------------------------------------------

EsAttributesIntf::Ptr EsScriptMetaclass::methodAttrsAccess(cr_EsString fqName) const
{
  EsString className;
  bool isClassMethod;
  EsString methodName;
  int paramCount;
  EsString signature;

  EsMethodInfo::fqNameParse(
    fqName,
    className,
    isClassMethod,
    methodName,
    paramCount,
    signature
  );

  EsMethodInfoKeyT key(
    paramCount,
    methodName
  );

	const EsClassInfo* info = EsClassInfo::classInfoGet(className);
  ES_ASSERT(info);

  if( info )
  {
    const EsMethodInfo* minfo = isClassMethod ?
      info->classMethodInfoFind( key, true ) :
      info->methodInfoFind( key, true );

    if( minfo )
      return minfo->attributesAccess();
  }

  EsScriptObjectIntf::Ptr metaclass = m_owner.metaclassFind(className);

  if( !metaclass )
    EsException::Throw(
      esT("Could not find reflected class or service, or scripted class or namespace '%s'"),
      className
    );

  EsScriptCodeSection::Ptr method = metaclass->findScriptedMethod( key );
  if( method )
    return method->attributesAccess();

  EsException::Throw(
    esT("Could not find method with FW name: '%s', in any reflected or scripted class or service"),
    fqName
  );

  return nullptr; //< Pacify compilers
}
//---------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::methodAttributeNamesGet(cr_EsString fqName) const
{
  EsAttributesIntf::Ptr attrs = methodAttrsAccess(fqName);
  ES_ASSERT(attrs);

  return attrs->allNamesGet();
}
//---------------------------------------------------------------------------

bool EsScriptMetaclass::methodHasAttribute(cr_EsString fqName, cr_EsString name) const
{
  EsAttributesIntf::Ptr attrs = methodAttrsAccess(fqName);
  ES_ASSERT(attrs);

  return attrs->attributeExists(name);
}
//---------------------------------------------------------------------------

EsVariant EsScriptMetaclass::methodAttributeGet(cr_EsString fqName, cr_EsString name) const
{
  EsAttributesIntf::Ptr attrs = methodAttrsAccess(fqName);
  ES_ASSERT(attrs);

  return attrs->attributeGet(name);
}
//---------------------------------------------------------------------------

EsString EsScriptMetaclass::methodDescriptionGet(cr_EsString fqName) const
{
  EsAttributesIntf::Ptr attrs = methodAttrsAccess(fqName);
  ES_ASSERT(attrs);

  EsString className;
  bool isClassMethod;
  EsString methodName;
  int paramCount;
  EsString signature;

  EsMethodInfo::fqNameParse(
    fqName,
    className,
    isClassMethod,
    methodName,
    paramCount,
    signature
  );

	EsString result = EsString::format(
    esT("%s(%s)"),
    methodName,
    signature
  );

	EsVariant help = attrs->attributeGetDef(
    esT("help"),
    EsVariant::null()
  );

	if( !help.isEmpty() )
		result += esT("\n") + esTranslationGet( help.asString() );

  return result;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::propertyNamesGet(const EsString& className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);
	if( info )
		return info->propertyNamesGet();
	else
	{
		EsReflectedClassIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		return metaclass->propertyNamesGet();
	}
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::thisPropertyNamesGet(const EsString& className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);

	EsStringArray result;
	if(info)
		result = info->propertyNamesGet(false, false);
	else
	{
		EsScriptObjectIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		EsStringIndexedMap::Ptr props = metaclass->thisPropertiesMapGet();
		if( props )
			result = props->namesGet();
	}

	return result;
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::persistentPropertyNamesGet(const EsString& className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);
	if(info)
		return info->propertyNamesGet(true);
	else
	{
		EsReflectedClassIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		return metaclass->persistentPropertyNamesGet();
	}
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::thisPersistentPropertyNamesGet(const EsString& className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);
	if(info)
		return info->propertyNamesGet(true, false);
	else
	{
		EsReflectedClassIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		// todo: return _this_ properties
		return metaclass->persistentPropertyNamesGet();
	}
}
//--------------------------------------------------------------------------------

bool EsScriptMetaclass::hasProperty(const EsString& className, const EsString& propName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);

	if( info )
		return info->hasProperty(propName);
	else
	{
		EsReflectedClassIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		return metaclass->hasProperty(propName);
	}
}
//--------------------------------------------------------------------------------

bool EsScriptMetaclass::propertyCanRead(const EsString& className, const EsString& propName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);

	if( info )
		return info->propertyInfoGet(propName).canRead();
	else
	{
		EsReflectedClassIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		return metaclass->propertyCanRead(propName);
	}
}
//--------------------------------------------------------------------------------

bool EsScriptMetaclass::propertyCanWrite(const EsString& className, const EsString& propName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);

	if( info )
		return info->propertyInfoGet(propName).canWrite();
	else
	{
		EsReflectedClassIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		return metaclass->propertyCanWrite(propName);
	}
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::propertyAttributeNamesGet(const EsString& className, const EsString& propName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);

	if( info )
	{
		const EsPropertyInfo& pinfo = info->propertyInfoGet(propName);

		EsAttributesIntf::Ptr attrs = pinfo.attributesAccess();
		EsStringArray result;
		if( attrs )
			result = attrs->allNamesGet();

		return result;
	}
	else
	{
		EsReflectedClassIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		return metaclass->propertyAttributeNamesGet(propName);
	}
}
//--------------------------------------------------------------------------------

bool EsScriptMetaclass::propertyHasAttribute(const EsString& className, const EsString& propName, const EsString& attrName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);

	if( info )
	{
		const EsPropertyInfo& pinfo = info->propertyInfoGet(propName);

		EsAttributesIntf::Ptr attrs = pinfo.attributesAccess();
		if( attrs )
			return attrs->attributeExists(attrName);
	}
	else
	{
		EsReflectedClassIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		return metaclass->propertyHasAttribute(propName, attrName);
	}

	return false;
}
//--------------------------------------------------------------------------------

EsVariant EsScriptMetaclass::propertyAttributeGet(const EsString& className, const EsString& propName, const EsString& attrName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);

	if( info )
	{
		const EsPropertyInfo& pinfo = info->propertyInfoGet(propName);

		EsAttributesIntf::Ptr attrs = pinfo.attributesAccess();
		if( attrs )
			return attrs->attributeGet(attrName);
		else
			EsException::Throw(
        esT("Property '%s' attribute '%s' does not exist"),
				propName,
        attrName
      );
	}
	else
	{
		EsReflectedClassIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		return metaclass->propertyAttributeGet(propName, attrName);
	}

	return EsVariant::null();
}
//--------------------------------------------------------------------------------

EsString EsScriptMetaclass::propertyLabelGet(const EsString& className, const EsString& propName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);
	if( info )
	{
		const EsPropertyInfo& pinfo = info->propertyInfoGet(propName);
		return pinfo.labelGet();
	}
	else
	{
		EsReflectedClassIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		return metaclass->propertyLabelGet(propName);
	}
}
//--------------------------------------------------------------------------------

EsString EsScriptMetaclass::propertyDescriptionGet(const EsString& className, const EsString& propName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className);
	if( info )
	{
		const EsPropertyInfo& pinfo = info->propertyInfoGet(propName);
		return pinfo.descriptionGet();
	}
	else
	{
		EsReflectedClassIntf::Ptr metaclass = m_owner.metaclassFind(className, true);
		ES_ASSERT(metaclass);

		return metaclass->propertyDescriptionGet(propName);
	}
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::enumerationNamesGet() const
{
  EsStringArray enums = EsMetaclass::instance()->enumerationNamesGet();
  const EsStringArray& scriptEnums = m_owner.enumNamesGet();
  
  enums.reserve(
    enums.size() + scriptEnums.size()
  );
  for( auto const& scriptEnu : scriptEnums )
    enums.push_back(
      scriptEnu
    );

  return enums;
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetaclass::enumerationSymbolsGet(cr_EsString enu) const
{
  EsEnumerationIntf::Ptr penu = m_owner.enumFind(enu);
  if(penu)
    return penu->symbolNamesGet();

  return EsMetaclass::instance()->enumerationSymbolsGet(enu);
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsVariant EsScriptMetaclass::objectCreate(const EsString& className) const
{
	return const_cast<EsScriptMachine&>(m_owner).objectCreate(className);
}
//--------------------------------------------------------------------------------

EsVariant EsScriptMetaclass::objectCreate(const EsString& className, const EsVariant& p0) const
{
	return const_cast<EsScriptMachine&>(m_owner).objectCreate(className, p0);
}
//--------------------------------------------------------------------------------

EsVariant EsScriptMetaclass::objectCreate(const EsString& className, const EsVariant& p0, const EsVariant& p1) const
{
	return const_cast<EsScriptMachine&>(m_owner).objectCreate(className, p0, p1);
}
//--------------------------------------------------------------------------------

EsVariant EsScriptMetaclass::objectCreate(const EsString& className, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2) const
{
	return const_cast<EsScriptMachine&>(m_owner).objectCreate(className, p0, p1, p2);
}
//--------------------------------------------------------------------------------

EsVariant EsScriptMetaclass::objectCreate(const EsString& className, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3) const
{
	return const_cast<EsScriptMachine&>(m_owner).objectCreate(className, p0, p1, p2, p3);
}
//--------------------------------------------------------------------------------

EsVariant EsScriptMetaclass::objectCreate(const EsString& className, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3, const EsVariant& p4) const
{
	return const_cast<EsScriptMachine&>(m_owner).objectCreate(className, p0, p1, p2, p3, p4);
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsVariant EsScriptMetaclass::scriptMachineGet() const
{
  return EsVariant(
    const_cast<EsScriptMachine*>(&m_owner),
    EsVariant::ACCEPT_POINTER
  );
}
//--------------------------------------------------------------------------------
