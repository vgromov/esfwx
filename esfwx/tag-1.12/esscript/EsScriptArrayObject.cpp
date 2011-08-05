#include "stdafx.h"
#pragma hdrstop

#include "EsScriptArrayObject.h"

ES_DECL_CLASS_INFO_BEGIN(EsScriptArrayObject, ES_DECL_REFLECTED_ANCESTORS1(EsScriptObject), NO_CLASS_DESCRIPTION)
	// reflected services
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptArrayObject, getCount, int_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptArrayObject, getItem, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptArrayObject, setItem, void_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

EsScriptArrayObject::EsScriptArrayObject(EsScriptMachine& machine, DWORD flags, EsSharedBinBufferPtr buff, 
	EsCodeSection::Ptr sizeExpr, EsScriptObjectIntf::Ptr itemMetaclass) :
EsScriptObject(wxT("array"), EsScriptObjectIntf::Ptr(), EsMethodMapPtr(), machine, flags, buff),
m_sizeExpr(sizeExpr),
m_itemMetaclass(itemMetaclass)
{
}

EsScriptObjectIntf::Ptr EsScriptArrayObject::createMetaclass(EsScriptMachine& machine, EsCodeSection::Ptr sizeExpr, EsScriptObjectIntf::Ptr itemMetaclass)
{ 
	wxASSERT(itemMetaclass);
	return EsScriptObjectIntf::Ptr( new EsScriptArrayObject(machine, ofMetaclass|ofFinal|ofArray, EsSharedBinBufferPtr(), sizeExpr, itemMetaclass) ); 
}

int EsScriptArrayObject::getCount() const
{
	return m_arr.size();
}

EsVariant EsScriptArrayObject::getItem(const EsVariant& idx) const
{
	int size = getCount();
	int index = idx.asInt();
	EsVariant::adjustIndex(index, size);
	return m_arr[index]->getProperty(wxT("value"));
}

void EsScriptArrayObject::setItem(const EsVariant& idx, const EsVariant& item)
{
	int size = getCount();
	int index = idx.asInt();
	EsVariant::adjustIndex(index, size);
	m_arr[index]->setProperty(wxT("value"), item);
}

// create instance of script object from its metaclass instance. that's what gets performed on calling NEW ScriptObject in script
ES_IMPL_INTF_METHOD(EsScriptObjectIntf::Ptr, EsScriptArrayObject::createObject)(EsSharedBinBufferPtr buff) const
{
	EsScriptArrayObject* inst = new EsScriptArrayObject(m_machine, (m_flags & ~ofMetaclass) | ofNeedUpdateLayout, 
		buff, m_sizeExpr, m_itemMetaclass);
	EsScriptObjectIntf::Ptr result( inst );
	wxASSERT(result);

	wxLogDebug(wxT("New instance of '%s of %s' object type created"), m_typeName.c_str(), m_itemMetaclass->getTypeName().c_str() );
	return result;
}

ES_IMPL_INTF_METHOD(void, EsScriptArrayObject::setParent)(EsScriptObjectIntf* parent)
{
	EsScriptObject::setParent(parent);
	// initialize update subscription, if any
	initializeUpdateSubscription();
}

size_t EsScriptArrayObject::internalCountGet()
{
	m_machine.exec(m_sizeExpr, EsVariant::s_null, m_parent);
	return m_machine.popDataValue()->get().asULong();
}

void EsScriptArrayObject::initializeUpdateSubscription()
{
	// set-up subscription to the update notifications
	if( ofMetaclass != (m_flags & ofMetaclass) &&
			m_parent)
	{
		wxLogDebug(wxT("initializeUpdateSubscription called for '%s' of '%s'"), getTypeName().c_str(), m_itemMetaclass->getTypeName().c_str());
		const EsString::Array& dependencies = m_sizeExpr->thisFieldDependenciesGet();
		for( size_t idx = 0; idx < dependencies.size(); ++idx )
		{
			EsScriptObjectIntf::Ptr fld = m_parent->findField(dependencies[idx]);
			wxASSERT(fld);
			fld->internalAddUpdateSubscriber(this);
		}
	}
}

EsScriptObjectIntf::Ptr EsScriptArrayObject::internalGetItem(size_t idx)
{
	wxASSERT(idx < m_arr.size());
	return m_arr[idx];
}

ES_IMPL_INTF_METHOD(void, EsScriptArrayObject::internalUpdateLayout)(size_t offs)
{
	wxASSERT(ofMetaclass != (m_flags & ofMetaclass));

	wxLogDebug(wxT("internalUpdateLayout called for '%s' with offs=%d, ofNeedUpdateLayout is %s"), 
		getTypeName().c_str(), offs, (m_flags & ofNeedUpdateLayout) ? wxT("set") : wxT("not set"));
		
	if( m_flags & ofNeedUpdateLayout )
	{
		size_t localSize = 0;
		size_t cnt = m_arr.size();
		size_t newCnt = internalCountGet();
		
		// update layout of existing items in range [0, min(cnt, newCnt)[
		for( size_t idx = 0; idx < std::min(cnt, newCnt); ++idx )
		{
			EsScriptObjectIntf::Ptr item = m_arr[idx];
			item->internalUpdateLayout(offs+localSize);
			localSize += item->getSize();
		}		
		
		if( cnt < newCnt )
		{
			m_arr.reserve( newCnt );
			
			for( size_t idx = cnt; idx < newCnt; ++idx )
			{
				EsScriptObjectIntf::Ptr item = m_itemMetaclass->createObject(m_data);
				wxASSERT(item);
				// set us as inst parent
				item->setParent(this);
				m_arr.push_back(item);
				item->internalUpdateLayout(offs+localSize);
				localSize += item->getSize();
			}
		}
		else 
			m_arr.erase( m_arr.begin()+newCnt, m_arr.end() );
		
		// update our size cache
		m_size = localSize;
		m_flags &= ~ofNeedUpdateLayout;
	}
	
	internalSetOffset(offs);
}