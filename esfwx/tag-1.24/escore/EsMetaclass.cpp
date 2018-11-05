#include "escorepch.h"
#pragma hdrstop

#include "EsMetaclass.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsMetaclass, NO_CLASS_DESCR)
	// Reflected class services
	ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsMetaclass, getReflected, instance, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
	// Interface services
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, objectNamesGet, EsStringArray_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, baseObjectNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, hasClass, bool_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, isKindOf, bool_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, hasAttribute, bool_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, attributeNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, attributeGet, EsVariant_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, methodNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, classMethodNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, thisMethodNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, thisClassMethodNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, hasMethod, bool_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, hasClassMethod, bool_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, fqNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, thisFqNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, methodAttributeNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, methodHasAttribute, bool_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, methodAttributeGet, EsVariant_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, methodDescriptionGet, EsString_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, propertyNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, thisPropertyNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, persistentPropertyNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, thisPersistentPropertyNamesGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, hasProperty, bool_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, propertyCanRead, bool_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, propertyCanWrite, bool_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, propertyAttributeNamesGet, EsStringArray_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, propertyHasAttribute, bool_CallConst_cr_EsString_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, propertyAttributeGet, EsVariant_CallConst_cr_EsString_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, propertyLabelGet, EsString_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, propertyDescriptionGet, EsString_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, enumerationNamesGet, EsStringArray_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, enumerationSymbolsGet, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, objectCreate, EsVariant_CallConst_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, objectCreate, EsVariant_CallConst_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, objectCreate, EsVariant_CallConst_cr_EsString_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, objectCreate, EsVariant_CallConst_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, objectCreate, EsVariant_CallConst_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsMetaclass, EsMetaclassIntf, objectCreate, EsVariant_CallConst_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsMetaclass::EsMetaclass() ES_NOTHROW
{}
//---------------------------------------------------------------------------

EsMetaclassIntf::Ptr EsMetaclass::instance() ES_NOTHROW
{
	return EsMetaclass::getReflected();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMetaclass::getReflected() ES_NOTHROW
{
	static EsMetaclass s_meta;

	EsBaseIntf::Ptr tmp(s_meta.asBaseIntf(), false, false);
	return tmp;
}
//---------------------------------------------------------------------------

EsStringArray EsMetaclass::objectNamesGet() const ES_NOTHROW
{
	return EsClassInfo::classNamesGet();
}
//---------------------------------------------------------------------------

EsStringArray EsMetaclass::baseObjectNamesGet(const EsString& className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);

	EsStringArray result;
	if( info->hasAncestor() )
		result.push_back( info->ancestorGet()->nameGet() );

	return result;
}
//---------------------------------------------------------------------------

bool EsMetaclass::hasClass(const EsString& className) const ES_NOTHROW
{
	return nullptr != EsClassInfo::classInfoGet(className);
}
//---------------------------------------------------------------------------

bool EsMetaclass::isKindOf(const EsString& className, const EsString& otherName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	return info->isKindOf(otherName);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool EsMetaclass::hasAttribute(cr_EsString className, cr_EsString attrName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);

	EsAttributesIntf::Ptr attrs = info->attributesAccess();
	if( attrs )
		return attrs->attributeExists(attrName);

	return false;
}
//---------------------------------------------------------------------------

EsStringArray EsMetaclass::attributeNamesGet(cr_EsString className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);

	EsAttributesIntf::Ptr attrs = info->attributesAccess();
	EsStringArray result;
	if( attrs )
		result = attrs->allNamesGet();

	return result;
}
//---------------------------------------------------------------------------

EsVariant EsMetaclass::attributeGet(cr_EsString className, cr_EsString attrName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);

	EsAttributesIntf::Ptr attrs = info->attributesAccess();
	if( attrs )
		return attrs->attributeGet(attrName);
	else
		EsException::Throw(
      esT("No attributes declared for '%s'"),
      className
    );

	// pacify compilers
	return EsVariant::null();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsStringArray EsMetaclass::methodNamesGet(const EsString& className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);

	EsStringArray result;

	const EsMethodInfoKeysT& keys = info->methodKeysGet();
	result.reserve( keys.size() );

	for( size_t idx = 0; idx < keys.size(); ++idx )
		result.push_back( keys[idx].asString() );

	return result;
}
//---------------------------------------------------------------------------

EsStringArray EsMetaclass::classMethodNamesGet(const EsString& className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);

	EsStringArray result;

	const EsMethodInfoKeysT& keys = info->classMethodKeysGet();
	result.reserve( keys.size() );

	for( size_t idx = 0; idx < keys.size(); ++idx )
		result.push_back( keys[idx].asString() );

	return result;
}
//---------------------------------------------------------------------------

EsStringArray EsMetaclass::thisMethodNamesGet(const EsString& className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);

	EsStringArray result;

	const EsMethodInfoKeysT keys = info->methodKeysGet(false);
	result.reserve( keys.size() );

	for( size_t idx = 0; idx < keys.size(); ++idx )
		result.push_back( keys[idx].asString() );

	return result;
}
//---------------------------------------------------------------------------

EsStringArray EsMetaclass::thisClassMethodNamesGet(const EsString& className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);

	EsStringArray result;

	const EsMethodInfoKeysT& keys = info->classMethodKeysGet();
	result.reserve( keys.size() );

	for( size_t idx = 0; idx < keys.size(); ++idx )
		result.push_back( keys[idx].asString() );

	return result;
}
//---------------------------------------------------------------------------

bool EsMetaclass::hasMethod(const EsString& className, const EsString& methodName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);

	EsMethodInfoKeyT key(methodName);
	return info->hasMethod(key);
}
//---------------------------------------------------------------------------

bool EsMetaclass::hasClassMethod(const EsString& className, const EsString& methodName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);

	EsMethodInfoKeyT key(methodName);
	return info->hasClassMethod(key);
}
//---------------------------------------------------------------------------

EsStringArray EsMetaclass::fqNamesGet(cr_EsString className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);

  return info->fqMethodNamesGet();
}
//---------------------------------------------------------------------------

EsStringArray EsMetaclass::thisFqNamesGet(cr_EsString className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);

  return info->fqMethodNamesGet(false);
}
//---------------------------------------------------------------------------

EsStringArray EsMetaclass::methodAttributeNamesGet(cr_EsString fqName) const
{
  const EsMethodInfo* info = EsMethodInfo::infoFromFqNameGet(
    fqName,
    true
  );

  return info->attributesAccess()->allNamesGet();
}
//---------------------------------------------------------------------------

bool EsMetaclass::methodHasAttribute(cr_EsString fqName, cr_EsString name) const
{
  const EsMethodInfo* info = EsMethodInfo::infoFromFqNameGet(
    fqName,
    true
  );

  return info->attributesAccess()->attributeExists(name);
}
//---------------------------------------------------------------------------

EsVariant EsMetaclass::methodAttributeGet(cr_EsString fqName, cr_EsString name) const
{
  const EsMethodInfo* info = EsMethodInfo::infoFromFqNameGet(
    fqName,
    true
  );

  return info->attributesAccess()->attributeGet(name);
}
//---------------------------------------------------------------------------

EsString EsMetaclass::methodDescriptionGet(cr_EsString fqName) const
{
  const EsMethodInfo* info = EsMethodInfo::infoFromFqNameGet(
    fqName,
    true
  );

  return info->descriptionGet();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsStringArray EsMetaclass::propertyNamesGet(const EsString& className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	return info->propertyNamesGet();
}
//---------------------------------------------------------------------------

EsStringArray EsMetaclass::thisPropertyNamesGet(const EsString& className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	return info->propertyNamesGet(false, false);
}
//---------------------------------------------------------------------------

EsStringArray EsMetaclass::persistentPropertyNamesGet(const EsString& className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	return info->propertyNamesGet(true);
}
//---------------------------------------------------------------------------

EsStringArray EsMetaclass::thisPersistentPropertyNamesGet(const EsString& className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	return info->propertyNamesGet(true, false);
}
//---------------------------------------------------------------------------

bool EsMetaclass::hasProperty(const EsString& className, const EsString& propName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	return info->hasProperty(propName);
}
//---------------------------------------------------------------------------

bool EsMetaclass::propertyCanRead(const EsString& className, const EsString& propName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	return info->propertyInfoGet(propName).canRead();
}
//---------------------------------------------------------------------------

bool EsMetaclass::propertyCanWrite(const EsString& className, const EsString& propName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	return info->propertyInfoGet(propName).canWrite();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsStringArray EsMetaclass::propertyAttributeNamesGet(const EsString& className, const EsString& propName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	const EsPropertyInfo& pinfo = info->propertyInfoGet(propName);

	EsAttributesIntf::Ptr attrs = pinfo.attributesAccess();
	EsStringArray result;
	if( attrs )
		result = attrs->allNamesGet();

	return result;
}
//---------------------------------------------------------------------------

bool EsMetaclass::propertyHasAttribute(const EsString& className, const EsString& propName, const EsString& attrName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	const EsPropertyInfo& pinfo = info->propertyInfoGet(propName);

	EsAttributesIntf::Ptr attrs = pinfo.attributesAccess();
	if( attrs )
		return attrs->attributeExists(attrName);

	return false;
}
//---------------------------------------------------------------------------

EsVariant EsMetaclass::propertyAttributeGet(const EsString& className, const EsString& propName, const EsString& attrName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
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

	return EsVariant::null();
}
//---------------------------------------------------------------------------

EsString EsMetaclass::propertyLabelGet(const EsString& className, const EsString& propName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	const EsPropertyInfo& pinfo = info->propertyInfoGet(propName);
	return pinfo.labelGet();
}
//---------------------------------------------------------------------------

EsString EsMetaclass::propertyDescriptionGet(const EsString& className, const EsString& propName) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	const EsPropertyInfo& pinfo = info->propertyInfoGet(propName);
	return pinfo.descriptionGet();
}
//--------------------------------------------------------------------------------

EsStringArray EsMetaclass::enumerationNamesGet() const
{
  EsStringArray objs = objectNamesGet();

  std::remove_if(
    objs.begin(),
    objs.end(),
    [this](const EsString& obj) -> bool {
      return !isKindOf(
        obj, 
        EsEnumeration::classNameGetStatic()
      );
    }
  );

  return objs;
}
//--------------------------------------------------------------------------------

EsStringArray EsMetaclass::enumerationSymbolsGet(cr_EsString enu) const
{
  const EsClassInfo* info = EsClassInfo::classInfoGet(
    enu, 
    true
  );
    
  if( !info->isKindOf(EsEnumeration::classNameGetStatic()) )
    EsException::Throw( 
      esT("EsMetaclass: '%s' is not an enumeration class"),
      enu
    );

  const EsVariant& instance = info->classCall("instance");
  ES_ASSERT(!instance.isEmpty());

  return instance.propertyGet("symbols").asStringCollection();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsVariant EsMetaclass::objectCreate(const EsString& className) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	return info->classCall(EsStdNames::reflectedCtr());
}
//---------------------------------------------------------------------------

EsVariant EsMetaclass::objectCreate(const EsString& className, const EsVariant& p0) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	return info->classCall(EsStdNames::reflectedCtr(), p0);
}
//---------------------------------------------------------------------------

EsVariant EsMetaclass::objectCreate(const EsString& className, const EsVariant& p0, const EsVariant& p1) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	return info->classCall(EsStdNames::reflectedCtr(), p0, p1);
}
//---------------------------------------------------------------------------

EsVariant EsMetaclass::objectCreate(const EsString& className, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	return info->classCall(EsStdNames::reflectedCtr(), p0, p1, p2);
}
//---------------------------------------------------------------------------

EsVariant EsMetaclass::objectCreate(const EsString& className, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	return info->classCall(EsStdNames::reflectedCtr(), p0, p1, p2, p3);
}
//---------------------------------------------------------------------------

EsVariant EsMetaclass::objectCreate(const EsString& className, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3, const EsVariant& p4) const
{
	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
	return info->classCall(EsStdNames::reflectedCtr(), p0, p1, p2, p3, p4);
}
//---------------------------------------------------------------------------

