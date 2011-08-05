#include "stdafx.h"
#pragma hdrstop

#include "EsScriptObject.h"

// -------------------------------------------------------------------------------------------------------
// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsScriptObject, NO_CLASS_DESCRIPTION)
	// reflected property infos declarations
	//
	ES_DECL_PROP_INFO(EsScriptObject, value, EsVariant, _i("Script object default property"), NO_DEFAULT_VAL)
ES_DECL_CLASS_INFO_END

// constructor for internal usage only
EsScriptObject::EsScriptObject(const EsString& typeName, EsScriptObjectIntf::Ptr ancestor, const EsMethodMapPtr& methods, EsScriptMachine& machine,
	DWORD flags, EsSharedBinBufferPtr buff) :
m_machine(machine),
m_methods(methods),
m_ancestor(ancestor),
m_parent(0),
m_typeName(typeName),
m_flags(flags),
m_data(buff),
m_size(0),
m_offs(0)
{
}

EsScriptObject::~EsScriptObject()
{
	if( m_methods )
	{
		// call _this_ script dtor, if defined, and not in metaclass mode
		EsCodeSection::Ptr dtor = findThisScriptedMethod( MethodInfoKeyT(0, c_reflectedDtrName) );
		if( !(ofMetaclass & m_flags) && dtor && !m_machine.isDestroying() )
		{
			m_machine.exec( dtor, EsVariant::s_null, this );
			wxLogDebug(wxT("Object '%s' scripted destructor called"), m_typeName.c_str());
		}
	}
	
	// unparent and cleanup own fields
	if( !m_fields.empty() )
	{
		size_t idx = m_fields.size()-1;
		do
		{
			// todo: free data buffer chunk??
			m_fields[idx]->setParent(0);
			
		} while(idx--);
		
		m_fields.clear();
		m_fieldsMap.clear();
	}
	
	// unparent ancestor
	if( m_ancestor )
		m_ancestor->setParent(0);
	
	// final cleanup
	m_parent = 0;
	m_flags = 0;
	m_offs = 0;
}

// create instance of script object from its metaclass instance. that's what gets performed on calling 'new' ScriptObject in script
ES_IMPL_INTF_METHOD(EsScriptObjectIntf::Ptr, EsScriptObject::createObject)(EsSharedBinBufferPtr buff) const
{
	EsScriptObjectIntf::Ptr result( new EsScriptObject(m_typeName, 
		m_ancestor ?  m_ancestor->internalClone(buff) : EsScriptObjectIntf::Ptr(), 
		m_methods, m_machine, (m_flags & ~ofMetaclass) | ofNeedUpdateLayout, buff) );
	// at this moment, all hierarhy of ancestors, including its fields, already created
	if( result->getAncestor() )
		result->getAncestor()->setParent( result.get() );
	
	wxLogDebug(wxT("New instance of '%s' object type created"), m_typeName.c_str());
	
	return result;
}

// add attribute to the object metaclass or instance
ES_IMPL_INTF_METHOD(void, EsScriptObject::addAttribute)(const EsString& name, const EsVariant& val)
{
	wxASSERT(!name.empty());
	
	if( isFinal() )
		EsException::ThrowFinalObjectMayNotBeModified(getTypeName());
		
	// check for attributes with such name
	EsIndexedNamesMap::right_const_iterator cit = m_attrsMap.right.find(name);
	if( cit != m_attrsMap.right.end() )
	{
		size_t idx = (*cit).second;
		wxLogDebug(wxT("Attribute's '%s' value ('%s') in script object '%s' in %s mode is replaced with '%s'"), 
			name.c_str(), EsScriptMachine::traceVariant(m_attrs[idx]).c_str(),
			getTypeName().c_str(), (m_flags & ofMetaclass) ? wxT("metaclass") : wxT("instance"),
			EsScriptMachine::traceVariant(val).c_str());
		if( val.isEmpty() )
			m_attrs[idx] = m_machine.emptyVal();
		else
			m_attrs[idx] = m_machine.declUnnamed(val);
	}
	else
	{
		if( val.isEmpty() )	
			m_attrs.push_back( m_machine.emptyVal() );
		else
			m_attrs.push_back( m_machine.declUnnamed(val) );
		m_attrsMap.insert( EsIndexedNamesMap::value_type(m_attrs.size()-1, name) );
		wxLogDebug(wxT("Attribute '%s'='%s' added to script object '%s' in %s mode"), 
			name.c_str(), EsScriptMachine::traceVariant(val).c_str(),
			getTypeName().c_str(), (m_flags & ofMetaclass) ? wxT("metaclass") : wxT("instance"));
	}
}

// get attribute value accessor
ES_IMPL_INTF_METHOD(EsValAccessor::Ptr, EsScriptObject::internalGetAttribute)(const EsString& name) const
{
	EsAccessorIntf::Ptr result;
	// check for attributes with such name
	EsIndexedNamesMap::right_const_iterator cit = m_attrsMap.right.find(name);
	if( cit != m_attrsMap.right.end() )
	{
		result = m_attrs[(*cit).second];
		wxASSERT(result);
	}
	else
		EsException::ThrowAttributeDoesNotExist(name, getTypeName());
		
	return result;
}

// check attribute existence
ES_IMPL_INTF_METHOD(bool, EsScriptObject::attributeExists)(const EsString& name) const
{
	wxASSERT(!name.empty());
	// check for attributes with such name
	EsIndexedNamesMap::right_const_iterator cit = m_attrsMap.right.find(name);
	return cit != m_attrsMap.right.end();
}

// get all assigned attribute names
ES_IMPL_INTF_METHOD(EsStringArray, EsScriptObject::getAttributeNames)() const
{
	EsStringArray names;
	names.reserve( m_attrs.size() );
	for( size_t idx = 0; idx < m_attrs.size(); ++idx )
	{
		EsIndexedNamesMap::left_const_iterator cit = m_attrsMap.left.find(idx);
		wxASSERT(cit != m_attrsMap.left.end());
		names.push_back( (*cit).second );
	}
	
	return names;
}

// get attribute value
ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::getAttribute)(const EsString& name) const
{
	return internalGetAttribute(name)->get();
}

// add field object to the script object
ES_IMPL_INTF_METHOD(void, EsScriptObject::addField)(const EsString& name, EsScriptObjectIntf::Ptr field)
{
	wxASSERT(!name.empty());
	wxASSERT(field);
	
	if( isFinal() )
		EsException::ThrowFinalObjectMayNotBeModified(getTypeName());
		
	if(	isPOD() )
		EsException::ThrowPodObjectMayNotContainFields(getTypeName());
	
	// check for fields with such name
	if( findField(name) )
		EsException::ThrowFieldAlreadyDeclared(name, getTypeName());

	m_fields.push_back(field);
	m_fieldsMap.insert( EsIndexedNamesMap::value_type(m_fields.size()-1, name) );
	
	// if we're not performing metaclass declaration compilation, set this object as field parent
	if( !(m_flags & ofMetaclass) )
		field->setParent( this );
		
	wxLogDebug(wxT("Field '%s' added to script object '%s' instance in '%s' mode"), 
		name.c_str(), m_typeName.c_str(), (m_flags & ofMetaclass) ? wxT("metaclass") : wxT("instance"));		
}

ES_IMPL_INTF_METHOD(EsScriptObjectIntf::Ptr, EsScriptObject::internalClone)(EsSharedBinBufferPtr buff) const
{
	wxLogDebug(wxT("internalClone called for '%s'"), getTypeName().c_str());
	// 1) create new instance of ourselves && all ancestors hierarchy
	EsScriptObjectIntf::Ptr result = createObject(buff);
	// 2) copy attributes from this instance to the newly created one
	for(size_t idx = 0; idx < m_attrs.size(); ++idx )
	{
		EsIndexedNamesMap::left_const_iterator cit = m_attrsMap.left.find(idx);
		wxASSERT( cit != m_attrsMap.left.end() );
		result->addAttribute( (*cit).second, m_attrs[idx]->get() );
	}
	// 3) create all nested fields, setting new copy of ourselves as its parent
	for(size_t idx = 0; idx < m_fields.size(); ++idx )
		result->addField( internalGetFieldName(idx), m_fields[idx]->internalClone(buff) );
	// seal object instance after internal cloning
	result->seal();
		
	return result;
}

// return name of the field by its index in internal collection
ES_IMPL_INTF_METHOD(const EsString&, EsScriptObject::internalGetFieldName)(size_t idx) const
{
	EsIndexedNamesMap::left_const_iterator cit = m_fieldsMap.left.find(idx);
	wxASSERT( cit != m_fieldsMap.left.end() );
	
	return (*cit).second;
}

// return root object for this object
ES_IMPL_INTF_METHOD(const EsScriptObjectIntf*, EsScriptObject::getRootObject)() const
{
	if( isRoot() )
		return this;
	else
		return getParent()->getRootObject();
}

ES_IMPL_INTF_METHOD(EsScriptObjectIntf*, EsScriptObject::getRootObject)()
{
	if( isRoot() )
		return this;
	else
		return getParent()->getRootObject();
}

// create new instance of EsScriptObject which is clone of the existing instance.
// if called from metaclass, new (default values used) object instance is created.
// may be called from the root objects (parent=NULL) only.
ES_IMPL_INTF_METHOD(EsScriptObjectIntf::Ptr, EsScriptObject::clone)() const
{
	wxLogDebug(wxT("clone called for '%s'"), m_typeName.c_str());
	if( !isRoot() )
		EsException::Throw(0, EsException::Generic, wxT("'clone' method may be called only for the root objects"));
	
	// create shared data buffer instance
	EsSharedBinBufferPtr buff( new EsBinBuffer );
	
	if( (m_flags & ofMetaclass) || !m_data )
		// reseve some space for the possible multiple data buffer resize and shuffle operations
		buff->reserve( sizeof(EsVariant) * getFieldCount() ); 
	else
		// copy data from the existing buffer
		buff->assign( m_data->begin(), m_data->end() );
	// perform internal clone of the object instance
	EsScriptObjectIntf::Ptr result = internalClone(buff);
	if( result ) // initiate memory layout update on newly cloned object
		result->internalUpdateLayout(0);
	
	return result;
}

// update object memory layout. fields and ancestor offsets and|or sizes get updated
// as a result of this process
//
ES_IMPL_INTF_METHOD(void, EsScriptObject::internalUpdateFieldsLayout)(size_t offs)
{
	wxLogDebug(wxT("internalUpdateFieldsLayout called for '%s' with offs=%d, ofNeedUpdateLayout is %s"), 
		m_typeName.c_str(), offs, (m_flags & ofNeedUpdateLayout) ? wxT("set") : wxT("not set"));
		
	if( m_flags & ofNeedUpdateLayout )
	{
		size_t localSize = m_ancestor ? m_ancestor->getSize() : 0;
		// set offset of the own fields
		for(size_t idx = 0; idx < m_fields.size(); ++idx)
		{
			EsScriptObjectIntf::Ptr field = m_fields[idx];
			field->internalUpdateLayout(offs+localSize);
			localSize += field->getSize();
		}

		m_size = localSize;
		m_flags &= ~ofNeedUpdateLayout;		
	}	
}

ES_IMPL_INTF_METHOD(void, EsScriptObject::internalUpdateLayout)(size_t offs)
{
	wxASSERT(ofMetaclass != (m_flags & ofMetaclass));

	wxLogDebug(wxT("internalUpdateLayout called for '%s' with offs=%d, ofNeedUpdateLayout is %s"), 
		m_typeName.c_str(), offs, (m_flags & ofNeedUpdateLayout) ? wxT("set") : wxT("not set"));
			
	if( m_flags & ofNeedUpdateLayout )
	{	
		size_t localSize = 0;

		EsScriptObjectIntf* obj = m_ancestor.get();
		while(obj->getAncestor().get())
			obj = obj->getAncestor().get();
			
		while(obj && obj != this)
		{
			obj->internalUpdateFieldsLayout(offs);
			obj->internalSetOffset(offs);
			localSize += obj->getSize();
			obj = obj->getParent();
		}
		
		internalUpdateFieldsLayout(offs);
		m_flags &= ~ofNeedUpdateLayout;		
	}
		
	internalSetOffset(offs);				
}

// handle data change event notification from subscription publisher
ES_IMPL_INTF_METHOD(void, EsScriptObject::internalOnDataChanged)(EsScriptObjectIntf* sender)
{
	wxASSERT(sender);
	EsScriptObject* obj = dynamic_cast<EsScriptObject*>(sender);
	wxASSERT(obj);
	wxLogDebug(wxT("internalOnDataChanged called for '%s' with sender='%s'"), m_typeName.c_str(), sender->getTypeName().c_str());
	if( !obj->m_updateSubscribers.empty() )
	{
		EsScriptObjectIntfWeakPtrList::iterator it = obj->m_updateSubscribers.begin();
		while(it != obj->m_updateSubscribers.end())
		{
			(*it)->internalInvalidateLayout();
			++it;
		}
	}
	else if( m_parent )
		m_parent->internalOnDataChanged(this);
}

// handle data change event notification from subscription publisher
ES_IMPL_INTF_METHOD(void, EsScriptObject::onDataChanged)(EsScriptObjectIntf* sender)
{
	wxASSERT(sender);
	wxLogDebug(wxT("onDataChanged called for '%s' with sender='%s'"), m_typeName.c_str(), sender->getTypeName().c_str());
	internalOnDataChanged(sender);
	// re-layout root object
	getRootObject()->internalUpdateLayout(0);
}

// internal write access to the object's offset value
ES_IMPL_INTF_METHOD(void, EsScriptObject::internalSetOffset)(size_t offs)
{
	// NB! size value must already be pre-calculated|known at the moment this method is called
	if( m_data->size() < offs + m_size )
	{
		size_t inc = offs + m_size - m_data->size();
		m_data->insert( m_data->begin()+offs, inc, 0 );
	}
	m_offs = offs;
	
	wxLogDebug(wxT("internalSetOffset called for '%s', offs=%d"), m_typeName.c_str(), offs);
}

// add object value update subscriber
ES_IMPL_INTF_METHOD(void, EsScriptObject::internalAddUpdateSubscriber)(EsScriptObjectIntf* subscriber)
{
	wxASSERT(0 != subscriber);
	if( m_updateSubscribers.end() == std::find(m_updateSubscribers.begin(), m_updateSubscribers.end(), subscriber) )
	{
		m_updateSubscribers.push_back(subscriber);
		wxLogDebug(wxT("Update subscriber '%s' added to '%s'"), subscriber->getTypeName().c_str(), m_typeName.c_str());
	}
}

// internal layout invalidation
ES_IMPL_INTF_METHOD(void, EsScriptObject::internalInvalidateLayout)()
{
	// set invalid layout flags for this object and all its parents
	m_flags |= ofNeedUpdateLayout;
	EsScriptObjectIntf* parent = m_parent;
	while( parent )
	{
		parent->internalInvalidateLayout();
		parent = parent->getParent();
	}
}

EsCodeSection::Ptr EsScriptObject::findThisScriptedMethod(const MethodInfoKeyT& key) const
{
	EsCodeSection::Ptr result;
	if( m_methods )
	{
		EsMethodMap::const_iterator cit = m_methods->find(key);
		if( cit != m_methods->end() )
			result = (*cit).second;
	}
	
	return result;
}

// finds the script object method declared in script.
// if method is not found, the empty pointer is returned
ES_IMPL_INTF_METHOD(EsCodeSection::Ptr, EsScriptObject::findScriptedMethod)(const MethodInfoKeyT& key) const
{
	EsCodeSection::Ptr result = findThisScriptedMethod(key);
	
	if( !result )
	{
		EsScriptObjectIntf::Ptr ancestor = m_ancestor;
		while( ancestor && !result )
		{
			result = ancestor->findScriptedMethod(key);
			ancestor = ancestor->getAncestor();
		}
	}
	
	return result;	
}

// find named field in this object or one of its ancestors. 
// if field is not found, the empty pointer is returned
ES_IMPL_INTF_METHOD(EsScriptObjectIntf::Ptr, EsScriptObject::findField)(const EsString& name) const
{
	EsScriptObjectIntf::Ptr result;
	EsIndexedNamesMap::right_const_iterator cit = m_fieldsMap.right.find(name);
	size_t idx;
	if( cit != m_fieldsMap.right.end() )
	{
		idx = (*cit).second;
		result = m_fields[idx];
	}
	else if( m_ancestor )
		result = m_ancestor->findField(name);
	
	return result;	
}

// return all field names, in order of their declaration in object|base objects
ES_IMPL_INTF_METHOD(EsStringArray, EsScriptObject::getFieldNames)() const
{
	EsStringArray result;
	result.reserve( getFieldCount() );
	
	// go to the top ancestor
	std::list<EsScriptObjectIntf::Ptr> hierarchy;
	EsScriptObjectIntf::Ptr obj = m_ancestor;
	while(obj)
	{
		hierarchy.push_front(obj);
		obj = obj->getAncestor();
	}
	
	// go down hierarchy, enumerating the root object's own fields last
	for( std::list<EsScriptObjectIntf::Ptr>::const_iterator cit = hierarchy.begin(); cit != hierarchy.end(); ++cit )
	{
		for(size_t idx = 0; idx < (*cit)->internalGetFieldCount(); ++idx)
			result.push_back( (*cit)->internalGetFieldName(idx) );
	}
	
	// do the same for this object
	for(size_t idx = 0; idx < internalGetFieldCount(); ++idx)
		result.push_back( internalGetFieldName(idx) );
	
	return result;
}

// return field count
ES_IMPL_INTF_METHOD(ulong, EsScriptObject::getFieldCount)() const
{
	size_t size = m_fields.size();
	EsScriptObjectIntf::Ptr ancestor = m_ancestor;
	while( ancestor )
	{
		size += ancestor->internalGetFieldCount();
		ancestor = ancestor->getAncestor();
	}
	
	return size;
}

// return field object by name
ES_IMPL_INTF_METHOD(EsScriptObjectIntf::Ptr, EsScriptObject::getField)(const EsString& name) const
{
	EsScriptObjectIntf::Ptr result = findField(name);
	if( !result )
		EsException::Throw(0, EsException::Generic, _("Field '%s' is not a member of object '%s' or any its base objects"),
			name.c_str(), m_typeName.c_str() );
	
	return result;
}

// call ancestor's method implementation (as if our 'this' was cast to ancestor's 'this')
ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::callAncestorMethod)(const EsString& name, const EsVariant& params, const EsString& ancestorTypeName)
{
	wxASSERT(!ancestorTypeName.empty());
	// find appropriate ancestor
	if( getTypeName() != ancestorTypeName )
	{
		EsScriptObjectIntf::Ptr ancestor = getAncestor();
		while( ancestor )
		{
			if( ancestor->getTypeName() == ancestorTypeName )
				return EsReflectedClassIntf::Ptr(ancestor)->callMethod(name, params);
			ancestor = ancestor->getAncestor();
		}
		// somehow, requested ancestor is not found in our base objects
		EsException::Throw(0, EsException::Generic, wxT("'%s' is not a base object of '%s'"),
			ancestorTypeName.c_str(),
			getTypeName().c_str());
	}
	else
		return callMethod(name, params);
		
	return EsVariant::s_null;	
}

// EsReflectedClassIntf interface implementation. EsScriptObject has its own specifics
// due to its dual nature - one part is C++ -coded reflection, other is script-coded
//
ES_IMPL_INTF_METHOD(bool, EsScriptObject::isKindOf)(const EsReflectedClassIntf* other) const
{
	bool result = false;
	if( other )
	{	
		// first, check reflected class part
		result = getClassInfo().isKindOf( other->getClassName() );
		
		if( !result )
		{
			// check type names
			if( m_typeName == other->getTypeName() )
				result = true;
			else
			{
				EsReflectedClassIntf::Ptr ancestor = m_ancestor;
				while( ancestor )
				{
					if( m_typeName == ancestor->getTypeName() )
					{
						result = true;
						break;
					}
					ancestor = dynamic_cast<EsScriptObject*>( ancestor.get() )->m_ancestor;
				}
			}
		}
	}

	return result;
}

ES_IMPL_INTF_METHOD(bool, EsScriptObject::isIndexed)() const
{
	return getClassInfo().isIndexed() ||
		( hasMethod(MethodInfoKeyT(0, wxT("getCount"))) && 
			( hasMethod(MethodInfoKeyT(1, wxT("getItem"))) || hasMethod(MethodInfoKeyT(2, wxT("setItem"))) ));
}

ES_IMPL_INTF_METHOD(EsString::Array, EsScriptObject::getPropertyNames)() const
{
	EsString::Array result = getClassInfo().getPropertyNames();
	// todo: get properties defined in script

	return result;
}

ES_IMPL_INTF_METHOD(EsString::Array, EsScriptObject::getPersistentPropertyNames)() const
{
	EsString::Array result = getClassInfo().getPropertyNames(true);
	// todo: get properties defined in script

	return result;
}

ES_IMPL_INTF_METHOD(bool, EsScriptObject::hasProperty)(const EsString& name) const
{
	return getClassInfo().hasProperty(name);
	// todo: check properties defined in script

}

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::getProperty)(const EsString& name) const
{
	// todo: get properties defined in script
	const PropertyInfo& info = getClassInfo().getPropertyInfo(name);
	return info.get(static_cast<EsBaseIntf*>(const_cast<ThisClass*>(this)));
}

ES_IMPL_INTF_METHOD(void, EsScriptObject::setProperty)(const EsString& name, const EsVariant& val)
{
	// todo: set properties defined in script
	const PropertyInfo& info = getClassInfo().getPropertyInfo(name);
	return info.set(static_cast<EsBaseIntf*>(this), val);
}

ES_IMPL_INTF_METHOD(EsString, EsScriptObject::getConfigString)() const
{ 
	EsString::Array props = getPersistentPropertyNames();
	EsDictionary dic( getTypeName() );
	for(size_t idx = 0; idx < props.size(); ++idx) 
	{
		const EsString& prop = props[idx];
		dic.setValue(prop, getProperty(prop), true); 
	}
	return dic.getAsString();
}

ES_IMPL_INTF_METHOD(void, EsScriptObject::setConfigString)(const EsString& str, bool discardUnknownProperties /*= true*/)
{ 
	EsDictionary dic(getTypeName()); 
	if( !str.empty() )
	{
		dic.setAsString(str);
		if( dic.getName() != getTypeName() ) 
			EsException::Throw(0, EsException::Generic,
			_("Wrong configuration name. Expected '%s', got '%s'"), getTypeName().c_str(), dic.getName().c_str() );
		EsString::Array names = dic.getAllNames();
		for(size_t idx = 0; idx < names.size(); ++idx) 
		{
			const EsString& prop = names[idx];
			if( hasProperty(prop) ) 
				setProperty(prop, dic.getValue(prop));
			else if( !discardUnknownProperties ) 
				EsException::Throw(0, EsException::Generic,	_("Unknown configuration property '%s'"), prop.c_str());
		}
	}
}	

ES_IMPL_INTF_METHOD(bool, EsScriptObject::hasMethod)(const MethodInfoKeyT& key) const
{
	return getClassInfo().hasMethod(key) || 
		findScriptedMethod(key);
}

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::callMethod)(const EsString& name, const EsVariant& params)
{
	EsVariant result;
	int paramsCount = params.isEmpty() ? 0 : (params.getType() == EsVariant::VAR_VARIANT_COLLECTION ? params.getCount() : 1);
	MethodInfoKeyT key(paramsCount, name);

	EsCodeSection::Ptr method = findScriptedMethod(key);
	if( method )
	{
		EsAccessorIntf::Ptr ret = m_machine.exec(method, params, this);
		if( ret )
			result = ret->get();
	}
	else
	{
		const MethodInfo* info = getClassInfo().findMethodInfo(key);
		if( info )
			result = info->call(static_cast<EsBaseIntf*>(const_cast<ThisClass*>(this)), params);
		else
			EsException::Throw(0, EsException::Generic, _("'%s', taking %d parameters, is not method of '%s', or any of its base objects"),
				name.c_str(), paramsCount, getTypeName().c_str());
	}
	
	return result;
}

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::call)(const EsString& name)
{
	return callMethod(name, EsVariant::s_null);
}

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::call)(const EsString& name, const EsVariant& param1)
{
	return callMethod(name, param1);
}

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::call)(const EsString& name, const EsVariant& param1, const EsVariant& param2)
{
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(param1)
		.addToVariantCollection(param2);		
	return callMethod(name, params);
}

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3)
{
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(param1)
		.addToVariantCollection(param2)
		.addToVariantCollection(param3);		
	return callMethod(name, params);
}

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4)
{
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(param1)
		.addToVariantCollection(param2)
		.addToVariantCollection(param3);		
	return callMethod(name, params);
}

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5)
{
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(param1)
		.addToVariantCollection(param2)
		.addToVariantCollection(param3)
		.addToVariantCollection(param4);
	return callMethod(name, params);
}

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6)
{
	EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
	params
		.addToVariantCollection(param1)
		.addToVariantCollection(param2)
		.addToVariantCollection(param3)
		.addToVariantCollection(param4)
		.addToVariantCollection(param5)
		.addToVariantCollection(param6);
	return callMethod(name, params);
}

ES_IMPL_INTF_METHOD(EsVariant, EsScriptObject::callClassMethod)(const EsString& name, const EsVariant& params) const
{ 
	int paramsCount = params.isEmpty() ? 0 : (params.getType() == EsVariant::VAR_VARIANT_COLLECTION ? params.getCount() : 1);
	const MethodInfo& info = getClassInfo().getClassMethodInfo(MethodInfoKeyT(paramsCount, name));
	return info.classCall(params); 
}

// reflected properties
//
// script object standard value property
EsVariant EsScriptObject::get_value() const
{
	EsBaseIntf::Ptr thisPtr( reinterpret_cast<EsBaseIntf*>(requestIntf( EsIID::fromIntf<EsBaseIntf>() )) );
	wxASSERT(thisPtr);
	return thisPtr;
}

void EsScriptObject::set_value(const EsVariant& val)
{
	EsException::Throw(0, EsException::Generic, wxT("The object of type '%s' does not support setting value directly"),
		getTypeName().c_str());
}

// set value parent notification
void EsScriptObject::onSetValueParentNotify()
{
	if( m_parent )
		m_parent->onDataChanged(this);
}