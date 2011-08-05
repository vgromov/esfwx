#include "stdafx.h"
#pragma hdrstop

#include <algorithm>
#include "Reflection.h"

// reflected constructor and destructor names
//
EsString::const_pointer c_reflectedCtrName = wxT("new");
EsString::const_pointer c_reflectedDtrName = wxT("delete");

// method info class implementation
//
bool MethodInfoKeyT::operator <(const MethodInfoKeyT& other) const 
{ 
	if( m_name < other.m_name )
		return true;
	else if( EsString::cmpEqual == m_name.compare(other.m_name) )
		return m_paramsCount < other.m_paramsCount;

	return false;
}

// signature strings map
//
EsString MethodInfo::SigStringsT::getString(int sig) const
{
	SigsT::const_iterator cit = m_sigs.find(sig);
	if( cit != m_sigs.end() )
		return cit->second.c_str();
	else
		return EsString::format(_("Unknown signature %d"), sig);
}

// generate signature-string map generation
const MethodInfo::SigStringsT MethodInfo::s_sigStrings = MethodInfo::SigStringsT()
#include <escore/ReflectionDefSigStrings.hxx>

MethodInfo::MethodInfo(ClassInfo& owner, int sig, BYTE paramsCount, 
											 const EsString& name, const EsString& descr, EsReflectedClassIntf::CallT method) :
m_owner(owner), 
m_signature(sig), 
m_paramsCount(paramsCount), 
m_name(name), 
m_descr(descr), 
m_method(method)
{
	wxASSERT( method );
	wxASSERT( sig > invalidSignature );
	wxASSERT( sig < methodSignaturesEnd );

	owner.m_methods[ MethodInfoKeyT(m_paramsCount, m_name) ] = this;
}

MethodInfo::MethodInfo(ClassInfo& owner, int sig, BYTE paramsCount, 
											 const EsString& name, const EsString& descr, EsReflectedClassIntf::ClassCallT classMethod, int WXUNUSED(tagClassCallInfo)) :
m_owner(owner), 
m_signature(sig), 
m_paramsCount(paramsCount), 
m_name(name), 
m_descr(descr), 
m_classMethod(classMethod)
{
	wxASSERT( classMethod );
	wxASSERT( sig > invalidSignature );
	wxASSERT( maxMethodSignature < sig && sig < classMethodSignaturesEnd );

	// register class pointer in owner, depending on whether it's ordinary method or class method
	owner.m_classMethods[ MethodInfoKeyT(m_paramsCount, m_name) ] = this;
}

// initialize interface method info
MethodInfo::MethodInfo(ClassInfo& owner, int sig,	const EsIID& iid, BYTE paramsCount, 
		const EsString& name, const EsString& descr, EsReflectedClassIntf::CallT method ) :
m_owner(owner), 
m_signature(sig), 
m_iid(iid),
m_paramsCount(paramsCount), 
m_name(name), 
m_descr(descr), 
m_method(method)
{
	wxASSERT( method );
	wxASSERT( sig > invalidSignature );
	wxASSERT( sig < methodSignaturesEnd );

	owner.m_methods[ MethodInfoKeyT(m_paramsCount, m_name) ] = this;
} 

//MethodInfo::~MethodInfo()
//{
//	// unregister class pointer in owner, depending on whether it's ordinary method or class method
//	if( isClassMethod() )
//		const_cast<ClassInfo::MethodInfosT&>(m_owner.m_classMethods).erase( MethodInfoKeyT(m_paramsCount, m_name) );
//	else
//		const_cast<ClassInfo::MethodInfosT&>(m_owner.m_methods).erase( MethodInfoKeyT(m_paramsCount, m_name) );
//}

void MethodInfo::checkSignature(int sig)
{
	if( sig <= invalidSignature )
		EsException::Throw(0, EsException::Generic, _("Invalid method signature value: %d."), sig);
	else if( sig >= methodSignaturesEnd && sig <= maxMethodSignature )
		EsException::Throw(0, EsException::Generic, _("Unknown method signature: %d."), sig);
	else if( sig >= classMethodSignaturesEnd )
		EsException::Throw(0, EsException::Generic, _("Unknown class method signature: %d."), sig);
}

// caller methods
//
// parameter checks
void MethodInfo::checkParamsCount(const EsVariant& params) const
{
	if( m_paramsCount )
	{
		if( (1 < m_paramsCount && EsVariant::VAR_VARIANT_COLLECTION != params.getType()) ||
				(1 == m_paramsCount && params.isEmpty()) )
			EsException::Throw(0, EsException::Generic, _("Parameter list expected in call to %s.%s of type %s."),
				m_owner.getName().c_str(), m_name.c_str(), getSignatureString().c_str());
		else if( EsVariant::VAR_VARIANT_COLLECTION == params.getType() && 
							m_paramsCount != params.getCount() )
			EsException::Throw(0, EsException::Generic, _("Wrong parameters count in call to %s.%s of type %s. Expected %d, got %d."), 
				m_owner.getName().c_str(), m_name.c_str(), getSignatureString().c_str(),
				m_paramsCount, params.getCount());
	}
	else
	{
		if( !params.isEmpty() && !(params.getType() == EsVariant::VAR_VARIANT_COLLECTION && params.getCount() > 0) )
			EsException::Throw(0, EsException::Generic, _("Method %s.%s of type %s does not expect parameters."),
				m_owner.getName().c_str(), m_name.c_str(), getSignatureString().c_str());
	}
}

// object member function caller
EsVariant MethodInfo::call(EsBaseIntf* obj, const EsVariant& params) const
{
	// assert it's normal method
	if( isClassMethod() )
		EsException::Throw(0, EsException::Generic, _("The method %s.%s of type %s is class method."),
		m_owner.getName().c_str(), m_name.c_str(), getSignatureString().c_str() );
	else if( !obj )
		EsException::Throw(0, EsException::Generic, _("Cannot call method %s.%s of type %s without an object."),
		m_owner.getName().c_str(), m_name.c_str(), getSignatureString().c_str() );
		
	checkParamsCount(params);
	
	EsVariant result;
	
	// cast to proper "this" if it's reflected interface method. 
	// maintain proper refcount with EsBaseIntf::Ptr
	EsBaseIntf::Ptr ptr;
	if( !m_iid.empty() )
	{
		ptr.reset( reinterpret_cast<EsBaseIntf*>(obj->requestIntf(m_iid)) );
		obj = ptr.get();
	}
	
	// generate method calls
	#include <escore/ReflectionDefCalls.hxx>

	return result;
}

// class function caller
EsVariant MethodInfo::classCall(const EsVariant& params) const
{
	// assert it's class method
	if( !isClassMethod() )
		EsException::Throw(0, EsException::Generic, _("The method %s.%s of type %s is not a class method and cannot be called without an object."),
		m_owner.getName().c_str(), m_name.c_str(), getSignatureString().c_str() );
		
	checkParamsCount(params);

	EsVariant result;

	// generate method class calls
	#include <escore/ReflectionDefClassCalls.hxx>
	
	return result;
}

// restriction implementation
//

PropertyInfo::Restriction::Restriction(const EsVariant& val, PropertyInfo::RestrictionType type) :
m_type(type),
m_val(val)
{
	// perform restriction value asserts
	if( PropertyInfo::rxtNone == type )
	{
		if( !val.isEmpty() )
			EsException::Throw(0, EsException::Generic, _("Restriction value may not be non-NULL for empty restriction type"));
	}
	else
	{
		if( val.isEmpty() )
			EsException::Throw(0, EsException::Generic, _("Restriction value may not be NULL for non-empty restriction type"));
		else
		{
			if( type >= PropertyInfo::rxtRange )
			{
				// assert that restriction value is of array type
				if( m_val.getType() != EsVariant::VAR_VARIANT_COLLECTION )
					EsException::Throw(0, EsException::Generic, _("Wrong restriction type. Array type is expected."));
				else if( PropertyInfo::rxtRange == type )
				{
					if( 2 != m_val.getCount() )
						EsException::Throw(0, EsException::Generic, _("Wrong element count in range restriction definition. Expected 2, encountered %d."), 
							m_val.getCount() );
					else if( m_val.getItem(0) > m_val.getItem(1) )
						EsException::Throw(0, EsException::Generic, _("Wrong range restriction. Min value may not be greater than Max one.") );
				}
				else if( PropertyInfo::rxtEnum == type )
				{
					if( 0 == m_val.getCount() )
						EsException::Throw(0, EsException::Generic, _("Empty enum restriction definition is not allowed.") );
				}
			}
		}
	}
}

// prop info name comparison predicate
struct PropertyInfoNameIs
{
	PropertyInfoNameIs(const EsString& name) : m_name(name) {}
	inline bool operator() (const PropertyInfo* info) const 
	{ wxASSERT(info); return m_name == info->getName(); }

	const EsString& m_name;
};

// PropertyInfo implementation
//
PropertyInfo::PropertyInfo(ClassInfo& owner, const EsString& name, 
													 const EsString& descr, const EsVariant& def,
													 EsReflectedClassIntf::CallT reader, int readerSig,
													 EsReflectedClassIntf::CallT writer, int writerSig,
													 const PropertyInfo::Restriction& restriction) :
m_owner(owner),
m_name(name),
m_descr(descr),
m_def(def), 
m_reader(reader),
m_readerSignature(readerSig),
m_writer(writer),
m_writerSignature(writerSig),
m_restriction(restriction)
{	
	// assert reader
	wxASSERT( m_reader );
	// assert reader & writer signatures
	wxASSERT( invalidSignature < readerSig && readerSig < methodSignaturesEnd );
	wxASSERT( m_writer ? (invalidSignature < writerSig && writerSig < methodSignaturesEnd) : true );

	// validate default value, if set
	if( !m_def.isEmpty() )
		validate(m_def, true);
	// append reflected property info to the metaclass
	owner.m_propInfos.push_back(this);
}

//PropertyInfo::~PropertyInfo()
//{
//	// append reflected propertsy info to the metaclass
//	const_cast<ClassInfo::PropertyInfosT&>(m_owner.m_propInfos).remove(this);
//}

// generic value access
EsVariant PropertyInfo::get(const EsBaseIntf* obj) const
{
	if( !obj )
		EsException::Throw(0, EsException::Generic, _("Cannot get property without an object."));

	EsVariant result;
	
	// generate property getters
	#include <escore/ReflectionDefGetters.hxx>
	
	return result;
}

void PropertyInfo::set(EsBaseIntf* obj, const EsVariant& val) const
{
	if( !obj )
		EsException::Throw(0, EsException::Generic, _("Cannot set property without an object."));
	
	// generate setters
	#include <escore/ReflectionDefSetters.hxx>
}

// reset property value to default, if there is one. if property has no default, exception is thrown
void PropertyInfo::reset(EsBaseIntf* obj) const
{
	if( hasDefault() )
		set(obj, m_def);
	else
		EsException::Throw(0, EsException::Generic, _("Property %s.%s does not have default value."),
		m_owner.getName().c_str(), getName().c_str());
}

bool PropertyInfo::validate(const EsVariant &val, bool doThrow/* = false*/) const
{
	switch( m_restriction.getType() )
	{
	case rxtMin:
	case rxtMinExcl:
		return validateMin(val, doThrow);
	case rxtMax:
	case rxtMaxExcl:
		return validateMax(val, doThrow);
	case rxtRange:
	case rxtRangeMinExcl:
	case rxtRangeMaxExcl:
	case rxtRangeExcl:
		return validateRange(val, doThrow);
	case rxtEnum:
		return validateEnum(val, doThrow);
	default:
		return true;
	}
}

bool PropertyInfo::validateMin(const EsVariant& val, bool doThrow) const
{
	wxASSERT( rxtMin == m_restriction.getType() || rxtMinExcl == m_restriction.getType() );
	bool ok = (rxtMin == m_restriction.getType()) ? 
		(val >= m_restriction.getVal()) :
		(val > m_restriction.getVal());
	if( !ok && doThrow )
	{
		if(rxtMin == m_restriction.getType()) 
			EsException::Throw(0, EsException::Generic, _("Value of %s must be greater than or equal to %s"), 
				m_name.c_str(), m_restriction.getVal().asString().c_str() );	
		else
			EsException::Throw(0, EsException::Generic, _("Value of %s must be greater than %s"), 
				m_name.c_str(), m_restriction.getVal().asString().c_str() );	
	}

	return ok;
}

bool PropertyInfo::validateMax(const EsVariant& val, bool doThrow) const
{
	wxASSERT( rxtMax == m_restriction.getType() || rxtMaxExcl == m_restriction.getType() );
	bool ok = (rxtMax == m_restriction.getType()) ? 
		(val <= m_restriction.getVal()) :
		(val < m_restriction.getVal());
	if( !ok && doThrow )
	{
		if( rxtMax == m_restriction.getType() )
			EsException::Throw(0, EsException::Generic, _("Value of %s must be less than or equal to %s"), 
				m_name.c_str(),	m_restriction.getVal().asString().c_str() );
		else
			EsException::Throw(0, EsException::Generic, _("Value of %s must be less than %s"), 
				m_name.c_str(), m_restriction.getVal().asString().c_str() );
	}

	return ok;
}

bool PropertyInfo::validateRange(const EsVariant& val, bool doThrow) const
{
	wxASSERT( rxtRange <= m_restriction.getType() && rxtRangeExcl >= m_restriction.getType() );
	bool ok = false;
	switch( m_restriction.getType() )
	{
	case rxtRange:
		ok = (val >= m_restriction.getVal().getItem(0)) && 
			(val <= m_restriction.getVal().getItem(1));
		break;
	case rxtRangeMinExcl:
		ok = (val > m_restriction.getVal().getItem(0)) && 
			(val <= m_restriction.getVal().getItem(1));
		break;
	case rxtRangeMaxExcl:
		ok = (val >= m_restriction.getVal().getItem(0)) && 
			(val < m_restriction.getVal().getItem(1));
		break;
	default:
		ok = (val > m_restriction.getVal().getItem(0)) && 
			(val < m_restriction.getVal().getItem(1));
		break;
	}
	
	if( !ok && doThrow )
	{	
		EsString errFmt;
		switch( m_restriction.getType() )
		{
		case rxtRange:
			errFmt = _("Value of %s must fall in [%s,%s] range");
			break;
		case rxtRangeMinExcl:
			errFmt = _("Value of %s must fall in ]%s,%s] range");
			break;
		case rxtRangeMaxExcl:
			errFmt = _("Value of %s must fall in [%s,%s[ range");
			break;
		default:
			errFmt = _("Value of %s must fall in ]%s,%s[ range");
			break;
		}

		EsException::Throw(0, EsException::Generic, 
			errFmt.c_str(),
			m_name.c_str(), 
			m_restriction.getVal().getItem(0).asString().c_str(),
			m_restriction.getVal().getItem(1).asString().c_str());	
	}

	return ok;
}

bool PropertyInfo::validateEnum(const EsVariant& val, bool doThrow) const
{
	wxASSERT( rxtEnum == m_restriction.getType() );
	bool ok = false;
	for( int idx = 0; idx < m_restriction.getVal().getCount() && !ok; ++idx )
	{
		EsVariant item = m_restriction.getVal().getItem(idx);
		
		if( item.getType() == EsVariant::VAR_VARIANT_COLLECTION )
		{
			wxASSERT(item.getCount() == 2);
			ok = val == item.getItem(0);
		}
		else
			ok = val == item;
	}
	
	if( !ok && doThrow )
	{
		EsString enumVals = EsString::fromStringArray( 
			m_restriction.getVal().asStringCollection(), wxT("\n\t") );
		EsException::Throw(0, EsException::Generic, 
			_("Value of %s must be one of:\n\t%s"), m_name.c_str(), enumVals.c_str());	
	}

	return ok;
}

// enumeration-only services
//
// return label for enum value
EsString PropertyInfo::getValueLabel(const EsVariant& val) const
{
	if( rxtEnum != m_restriction.getType() )
		EsException::Throw(0, EsException::Generic, _("getValueLabel service is only valid for enumeration properties"));

	const EsVariant& items = m_restriction.getVal();
	for(int idx = 0; idx < items.getCount(); ++idx )
	{
		EsVariant var = items.getItem(idx);
		EsVariant ourVal;
		EsString name;
		if( var.isCollection() )
		{
			ourVal = var.getItem(0);
			name = var.getItem(1).asString();
		}
		else
		{
			ourVal = var;
			name = var.asString();
		}
		
		if( ourVal == val )
			return EsString( wxGetTranslation(name.c_str()) );
	}
	
	EsException::Throw(0, EsException::Generic, _("Value %s is not a member of enumeration %s."), 
		val.asString().c_str(),
		m_name.c_str() );
		
	return EsString::s_null;
}

// return all labels
EsString::Array PropertyInfo::getAllLabels() const
{
	if( rxtEnum != m_restriction.getType() )
		EsException::Throw(0, EsException::Generic, _("getAllLabels service is only valid for enumeration properties"));

	EsString::Array result;
	const EsVariant& items = m_restriction.getVal();
	result.reserve(items.getCount());
	for(int idx = 0; idx < items.getCount(); ++idx )
	{
		EsVariant var = items.getItem(idx);
		EsString lbl;
		if( var.isCollection() )
			lbl = var.getItem(1).asString();
		else
			lbl = var.asString();
			
		lbl = wxGetTranslation(lbl.c_str());
		result.push_back(lbl);
	}

	return result;
}

// return all enum values
EsVariant PropertyInfo::getAllValues() const
{
	if( rxtEnum != m_restriction.getType() )
		EsException::Throw(0, EsException::Generic, _("getAllValues service is only valid for enumeration properties"));

	EsVariant result(EsVariant::VAR_VARIANT_COLLECTION);
	const EsVariant& items = m_restriction.getVal();
	for(int idx = 0; idx < items.getCount(); ++idx )
	{
		EsVariant var = items.getItem(idx);
		if( var.isCollection() )
			result.addToVariantCollection( var.getItem(0) );
		else
			result.addToVariantCollection( var );
	}
	
	return result;
}

// return enum item value at index
EsVariant PropertyInfo::getEnumItem(size_t idx) const
{
	if( rxtEnum != m_restriction.getType() )
		EsException::Throw(0, EsException::Generic, _("getEnumItem service is only valid for enumeration properties"));
	
	EsVariant result;

	if( idx < static_cast<size_t>(m_restriction.m_val.getCount()) )
		result = m_restriction.m_val.getItem(idx);
	else
		EsException::Throw(0, EsException::Generic, _("Invalid enumeration %s item index %d."), 
			m_name.c_str(), idx);

	return result;
}

// return enum item value at index
EsVariant PropertyInfo::getEnumValue(size_t idx) const
{
	EsVariant result = getEnumItem(idx);
	if( result.isCollection() )
		result = result.getItem(0);
	return result;
}

// class infos map implementation
//
// chaining-capable item addition
ClassInfo::ClassInfosT& ClassInfo::ClassInfosT::add(const ClassInfo* info) 
{  
	wxASSERT(info);
	m_infos[info->getName()] = info;
	return *this;
}

// serch item by name
const ClassInfo* ClassInfo::ClassInfosT::find(const EsString& name) const 
{
	ClassInfosT::MapT::const_iterator cit = m_infos.find(name);
	if( cit != m_infos.end() )
		return cit->second;

	return 0;
}

// class info metaclass implementation
//
ClassInfo::ClassInfo(const EsString& name, const EsString& descr, const ClassInfo::ClassInfosT& ancestors) :
m_ancestors(ancestors),
m_name(name),
m_descr(descr)
{
	classes().add(this);
}

ClassInfo::~ClassInfo()
{
	classes().erase(m_name);
}

// inheritance access
bool ClassInfo::isKindOf(const EsString& name) const
{
	// check self
	if( name == m_name )
		return true;
	else
	{
		// look-up ancestor by name
		for(ClassInfosT::IteratorT it = m_ancestors.begin(); it != m_ancestors.end(); ++it)
			if( it->second->isKindOf(name) )
				return true;
	}

	return false;
}

EsString::Array ClassInfo::getImmediateAncestors() const
{
	EsString::Array result;
	result.reserve(m_ancestors.size());

	for( ClassInfosT::IteratorT it = m_ancestors.begin(); it != m_ancestors.end(); ++it )
		result.push_back( it->first );

	return result;
}

// reflected information access
// 
// property names enumeration helper
void ClassInfo::appendPropertyNames(EsString::Array& out, bool onlyPersistent, bool allHierarchy) const 
{
	if( allHierarchy && !m_ancestors.empty() )
	{
		for(ClassInfosT::IteratorT cit = m_ancestors.begin(); cit != m_ancestors.end(); ++cit)
			cit->second->appendPropertyNames(out, onlyPersistent, allHierarchy);
	}

	for(PropertyInfosT::const_iterator cit = m_propInfos.begin(); cit != m_propInfos.end(); ++cit)
	{
		if( onlyPersistent )
		{
			if( (*cit)->canRead() && (*cit)->canWrite() )
				out.push_back( (*cit)->getName() );
		}
		else
			out.push_back( (*cit)->getName() );
	}
}

void ClassInfo::appendPropertyInfos(PropertyInfosT& out, bool onlyPersistent, bool allHierarchy) const
{
	if( allHierarchy && !m_ancestors.empty() )
	{
		for(ClassInfosT::IteratorT cit = m_ancestors.begin(); cit != m_ancestors.end(); ++cit)
			cit->second->appendPropertyInfos(out, onlyPersistent, allHierarchy);
	}

	for(PropertyInfosT::const_iterator cit = m_propInfos.begin(); cit != m_propInfos.end(); ++cit)
	{
		if( onlyPersistent )
		{
			if( (*cit)->canRead() && (*cit)->canWrite() )
				out.push_back( *cit );				
		}
		else
			out.push_back( *cit );
	}
}

EsString::Array ClassInfo::getPropertyNames(bool onlyPersistent /*= false*/, bool allHierarchy /*= true*/) const
{
	EsString::Array result;
	appendPropertyNames(result, onlyPersistent, allHierarchy);

	return result;
}

ClassInfo::PropertyInfosT ClassInfo::getPropertyInfos(bool onlyPersistent /*= false*/, bool allHierarchy /*= true*/) const
{
	PropertyInfosT result;
	appendPropertyInfos(result, onlyPersistent, allHierarchy);
	
	return result;
}

const PropertyInfo* ClassInfo::findPropertyInfo(const EsString& name, bool allHierarchy) const
{
	PropertyInfosT::const_iterator cit = std::find_if(m_propInfos.begin(), m_propInfos.end(),
		PropertyInfoNameIs(name) );
	if( cit != m_propInfos.end() )
		return *cit;
	else if( allHierarchy )
	{
		const PropertyInfo* result = 0;
		for( ClassInfosT::IteratorT cit = m_ancestors.begin(); cit != m_ancestors.end() && !result; ++cit )
			result = cit->second->findPropertyInfo(name, allHierarchy);

		return result;
	}

	return 0;
}

bool ClassInfo::hasProperty(const EsString& name, bool allHierarchy /*= true*/) const
{
	return findPropertyInfo(name, allHierarchy) != 0;
}

const PropertyInfo& ClassInfo::getPropertyInfo(const EsString& name, bool allHierarchy /*= true*/ ) const
{
	const PropertyInfo* result = findPropertyInfo(name, allHierarchy);
	if( !result )
		EsException::Throw(0, EsException::Generic, _("There is no \"%s\" property in class \"%s\"."), 
			name.c_str(), m_name.c_str());

	return *result;
}

// reflected method infos access
void ClassInfo::appendMethodKeys(MethodInfoKeysT& out, bool allHierarchy) const
{
	if( allHierarchy && !m_ancestors.empty() )
	{
		for(ClassInfosT::IteratorT cit = m_ancestors.begin(); cit != m_ancestors.end(); ++cit)
			cit->second->appendMethodKeys(out, allHierarchy);
	}
	else
	{
		for(MethodInfosT::const_iterator cit = m_methods.begin(); cit != m_methods.end(); ++cit)
			out.push_back( cit->first );
	}
}

void ClassInfo::appendClassMethodNames(MethodInfoKeysT& out, bool allHierarchy) const
{
	if( allHierarchy && !m_ancestors.empty() )
	{
		for(ClassInfosT::IteratorT cit = m_ancestors.begin(); cit != m_ancestors.end(); ++cit)
			cit->second->appendClassMethodNames(out, allHierarchy);
	}
	else
	{
		for(MethodInfosT::const_iterator cit = m_classMethods.begin(); cit != m_classMethods.end(); ++cit)
			out.push_back( cit->first );
	}
}

const MethodInfo* ClassInfo::findMethodInfo(const MethodInfoKeyT& key, bool allHierarchy) const
{
	MethodInfosT::const_iterator cit = m_methods.find(key);
	if( cit != m_methods.end() )
		return cit->second;
	else if( allHierarchy )
	{
		const MethodInfo* result = 0;
		for( ClassInfosT::IteratorT cit = m_ancestors.begin(); cit != m_ancestors.end() && !result; ++cit )
			result = cit->second->findMethodInfo(key, allHierarchy);

		return result;
	}

	return 0;
}

const MethodInfo* ClassInfo::findClassMethodInfo(const MethodInfoKeyT& key, bool allHierarchy) const
{
	MethodInfosT::const_iterator cit = m_classMethods.find(key);
	if( cit != m_classMethods.end() )
		return cit->second;
	else if( allHierarchy )
	{
		const MethodInfo* result = 0;
		for( ClassInfosT::IteratorT cit = m_ancestors.begin(); cit != m_ancestors.end() && !result; ++cit )
			result = cit->second->findMethodInfo(key, allHierarchy);

		return result;
	}

	return 0;
}

MethodInfoKeysT ClassInfo::getMethodKeys(bool allHierarchy /*= true*/) const
{
	MethodInfoKeysT result;
	result.reserve(32);
	
	appendMethodKeys(result, allHierarchy);
	
	return result;
}

bool ClassInfo::hasMethod( const MethodInfoKeyT& key, bool allHierarchy /*= true*/ ) const
{
	const MethodInfo* info = findMethodInfo(key, allHierarchy);
	return info != 0;
}

const MethodInfo& ClassInfo::getMethodInfo(const MethodInfoKeyT& key, bool allHierarchy /*= true*/ ) const
{
	const MethodInfo* info = findMethodInfo(key, allHierarchy);
	if( !info )
		EsException::Throw(0, EsException::Generic, _("Method %s, taking %d parameters, is not found in class %s"),
			key.getName().c_str(), key.getParametersCount(), getName().c_str() );
	
	return *info;
}

MethodInfoKeysT ClassInfo::getClassMethodKeys(bool allHierarchy /*= false*/) const
{
	MethodInfoKeysT result;
	result.reserve(16);
	
	appendClassMethodNames(result, allHierarchy);
	
	return result;
}

const MethodInfo& ClassInfo::getClassMethodInfo(const MethodInfoKeyT& key, bool allHierarchy /*= false*/ ) const
{
	const MethodInfo* info = findClassMethodInfo(key, allHierarchy);
	if( !info )
		EsException::Throw(0, EsException::Generic, _("Class method %s, taking %d parameters, is not found in class %s"),
			key.getName().c_str(), key.getParametersCount(), getName().c_str() );
	
	return *info;
}

// property services simplified
EsVariant ClassInfo::propertyGet(EsBaseIntf* obj, const EsString& name) const
{
	const PropertyInfo& info = getPropertyInfo(name);
	return info.get(obj);
}

void ClassInfo::propertySet(EsBaseIntf* obj, const EsString& name, const EsVariant& val) const
{
	const PropertyInfo& info = getPropertyInfo(name);
	info.set(obj, val);
}

void ClassInfo::propertyReset(EsBaseIntf* obj, const EsString& name) const
{
	const PropertyInfo& info = getPropertyInfo(name);
	return info.reset(obj);
}

void ClassInfo::resetAllProperties(EsBaseIntf* obj) const
{
	PropertyInfosT allProps = getPropertyInfos();
	for(PropertyInfosT::const_iterator cit = allProps.begin(); cit != allProps.end(); ++cit)
	{
		if( (*cit)->hasDefault() )
			(*cit)->reset(obj);
	}
}

// return true if standard indexed services are found
// getCount() and getItem(index) and | or setItem(index, value)
// 
bool ClassInfo::isIndexed() const
{
	return hasMethod(MethodInfoKeyT(0, wxT("getCount"))) && 
		( hasMethod(MethodInfoKeyT(1, wxT("getItem"))) || hasMethod(MethodInfoKeyT(2, wxT("setItem"))) );
}

// method services simplified
EsVariant ClassInfo::call(EsBaseIntf* obj, const EsString& name) const
{
	const MethodInfo& info = getMethodInfo(MethodInfoKeyT(0, name));
	return info.call(obj, EsVariant::s_null);
}

EsVariant ClassInfo::call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1) const
{
	const MethodInfo& info = getMethodInfo(MethodInfoKeyT(1, name));
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params.addToVariantCollection(param1);
	return info.call(obj, params);
}

EsVariant ClassInfo::call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2) const
{
	const MethodInfo& info = getMethodInfo(MethodInfoKeyT(2, name));
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(param1)
		.addToVariantCollection(param2);
	return info.call(obj, params);
}

EsVariant ClassInfo::call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) const
{
	const MethodInfo& info = getMethodInfo(MethodInfoKeyT(3, name));
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(param1)
		.addToVariantCollection(param2)	
		.addToVariantCollection(param3);
	return info.call(obj, params);
}

EsVariant ClassInfo::call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) const
{
	const MethodInfo& info = getMethodInfo(MethodInfoKeyT(4, name));
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(param1)
		.addToVariantCollection(param2)	
		.addToVariantCollection(param3)		
		.addToVariantCollection(param4);
	return info.call(obj, params);
}

EsVariant ClassInfo::call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) const
{
	const MethodInfo& info = getMethodInfo(MethodInfoKeyT(5, name));
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(param1)
		.addToVariantCollection(param2)	
		.addToVariantCollection(param3)		
		.addToVariantCollection(param4)			
		.addToVariantCollection(param5);
	return info.call(obj, params);
}

EsVariant ClassInfo::call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6) const
{
	const MethodInfo& info = getMethodInfo(MethodInfoKeyT(6, name));
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(param1)
		.addToVariantCollection(param2)	
		.addToVariantCollection(param3)		
		.addToVariantCollection(param4)			
		.addToVariantCollection(param5)	
		.addToVariantCollection(param6);	
	return info.call(obj, params);
}

EsVariant ClassInfo::callMethod(EsBaseIntf* obj, const EsString& name, const EsVariant& params) const
{
	int paramsCount = params.isEmpty() ? 0 : (params.getType() == EsVariant::VAR_VARIANT_COLLECTION ? params.getCount() : 1);
	const MethodInfo& info = getMethodInfo(MethodInfoKeyT(paramsCount, name));
	return info.call(obj, params);
}

EsVariant ClassInfo::classCall(const EsString& name) const
{
	const MethodInfo& info = getClassMethodInfo(MethodInfoKeyT(0, name));
	return info.classCall(EsVariant::s_null);
}

EsVariant ClassInfo::classCall(const EsString& name, const EsVariant& param1) const
{
	const MethodInfo& info = getClassMethodInfo(MethodInfoKeyT(1, name));
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params.addToVariantCollection(param1);
	return info.classCall(params);
}

EsVariant ClassInfo::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2) const
{
	const MethodInfo& info = getClassMethodInfo(MethodInfoKeyT(2, name));
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(param1)
		.addToVariantCollection(param2);
	return info.classCall(params);
}

EsVariant ClassInfo::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) const
{
	const MethodInfo& info = getClassMethodInfo(MethodInfoKeyT(3, name));
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(param1)
		.addToVariantCollection(param2)	
		.addToVariantCollection(param3);	
	return info.classCall(params);
}

EsVariant ClassInfo::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) const
{
	const MethodInfo& info = getClassMethodInfo(MethodInfoKeyT(4, name));
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(param1)
		.addToVariantCollection(param2)	
		.addToVariantCollection(param3)		
		.addToVariantCollection(param4);		
	return info.classCall(params);
}

EsVariant ClassInfo::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) const
{
	const MethodInfo& info = getClassMethodInfo(MethodInfoKeyT(5, name));
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(param1)
		.addToVariantCollection(param2)	
		.addToVariantCollection(param3)		
		.addToVariantCollection(param4)			
		.addToVariantCollection(param5);
	return info.classCall(params);
}

EsVariant ClassInfo::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6) const
{
	const MethodInfo& info = getClassMethodInfo(MethodInfoKeyT(6, name));
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(param1)
		.addToVariantCollection(param2)	
		.addToVariantCollection(param3)		
		.addToVariantCollection(param4)			
		.addToVariantCollection(param5)	
		.addToVariantCollection(param6);
	return info.classCall(params);
}

EsVariant ClassInfo::classCallMethod(const EsString& name, const EsVariant& params) const
{
	int paramsCount = params.isEmpty() ? 0 : (params.getType() == EsVariant::VAR_VARIANT_COLLECTION ? params.getCount() : 1);
	const MethodInfo& info = getClassMethodInfo(MethodInfoKeyT(paramsCount, name));
	return info.classCall(params);
}

// static services. registered reflected classes access
//
// use local static init on first use idiom for the class info registry
ClassInfo::ClassInfosT& ClassInfo::classes()
{
	static ClassInfosT s_classes;
	return s_classes;
}

EsString::Array ClassInfo::getClassNames()
{
	EsString::Array names;

	names.reserve(classes().size());
	for( ClassInfosT::IteratorT it = classes().begin(); it != classes().end(); ++it )
		names.push_back( it->first );

	return names;
}

const ClassInfo* ClassInfo::getClassInfo(const EsString& name, bool Throw/* = false*/ )
{
	const ClassInfo* result = 0;

	result = classes().find(name);
	if( !result && Throw )
		EsException::Throw(0, EsException::Generic, _("Class \"%s\" is not registered."), name.c_str() );

	return result;
}
