#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptArrayObject.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_CLASS_INFO_DERIVED_BEGIN(EsScriptArrayObject, EsScriptObject, NO_CLASS_DESCR)
	// reflected services
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptArrayObject, countGet, ulong_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptArrayObject, itemGet, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptArrayObject, itemSet, void_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

EsScriptArrayObject::EsScriptArrayObject(const EsScriptContext::Ptr& ctx, esU32 flags, const EsScriptObjectDataBufferPtr& buff,
	const EsScriptCodeSection::Ptr& sizeExpr, const EsScriptObjectIntf::Ptr& itemMetaclass) :
EsScriptObject(esT("array of ") + itemMetaclass->typeNameGet(), EsScriptObjectIntf::Ptr(), EsScriptMethodMapPtr(), ctx, flags,
	buff, EsAttributesIntf::Ptr()),
m_expr(sizeExpr),
m_itemMetaclass(itemMetaclass),
m_subscriptionInitialized(false),
m_directBinBufferAssignment(false)
{
}

EsScriptObjectIntf::Ptr EsScriptArrayObject::createMetaclass(const EsScriptContext::Ptr& ctx, const EsScriptObjectIntf::Ptr& itemMetaclass)
{
	ES_ASSERT(itemMetaclass);
	// create array metaclass wo expression code section
	std::unique_ptr<EsScriptArrayObject> tmp( 
		new EsScriptArrayObject(
			ctx, 
			ofMetaclass|ofFinal|ofArray,
			EsScriptObjectDataBufferPtr(), 
			EsScriptCodeSection::Ptr(), 
			itemMetaclass
		) 
	);
	ES_ASSERT(tmp.get());
	// install expression code section
	const EsScriptCodeSection::Ptr& expr = EsScriptCodeSection::create(
    esT("size"),
    EsString::nullArray(),
    tmp.get()
  );

	ES_ASSERT(expr);
	tmp->m_expr = expr;
	return tmp.release()->asBaseIntfPtrDirect(); 	
}

ES_IMPL_INTF_METHOD(void, EsScriptArrayObject::binBufferSet)(const EsBinBuffer& buff)
{
	ES_ASSERT(!buff.empty());
	EsCriticalSectionLocker lock(m_ctx->vm()->m_cs);

	// create temporary object, owning its own binary buffer
	const EsScriptObjectDataBufferPtr& pbuff = EsScriptObjectDataBuffer::create();
	EsScriptObjectIntf::Ptr tmp = internalClone(
    nullptr, 
    pbuff, 
    false
  );
	ES_ASSERT(tmp);
	EsScriptArrayObject* pa =
		reinterpret_cast<EsScriptArrayObject*>(
      tmp->requestIntf(
  			EsIID::fromIntf<EsBaseIntf>(),
        false
      )
    );
	ES_ASSERT(pa);
	
	try
	{
		pa->m_directBinBufferAssignment = true;
		pa->m_arr.reserve(m_arr.size());
		pa->internalUpdateLayout(0);
		pa->m_directBinBufferAssignment = false;
	}
	catch(...)
	{
		pa->m_directBinBufferAssignment = false;
		// rethrow
		throw;
	}
	
	EsBinBuffer::const_pointer pos = &buff[0];
	if( tmp->internalBinBufferSet(pos, pos+buff.size()) )
		copyFrom(tmp);
	else
		EsScriptException::ThrowBinBufferDoesNotMatchObject(
      m_ctx->vm()->currentDebugInfoGet()
    );
}

ulong EsScriptArrayObject::countGet() const
{
	return static_cast<ulong>(m_arr.size());
}

EsVariant EsScriptArrayObject::itemGet(const EsVariant& idx) const
{
	ulong size = countGet();
	long index = idx.asLong();
	EsVariant::indexAdjust(index, size);
	return m_arr[index];
}

void EsScriptArrayObject::itemSet(const EsVariant& idx, const EsVariant& item)
{
	ulong size = countGet();
	long index = idx.asLong();
	EsVariant::indexAdjust(index, size);
	EsReflectedClassIntf::Ptr obj = m_arr[index];
	ES_ASSERT(obj);

  if( obj->hasProperty( EsStdNames::value() ) )
  	obj->propertySet(EsStdNames::value(), item);
  else
    obj->copyFrom( item.asExistingObject() );
}

// create instance of script object from its metaclass instance. that's what gets performed on calling NEW ScriptObject in script
ES_IMPL_INTF_METHOD(EsScriptObjectIntf::Ptr, EsScriptArrayObject::objectCreate)(const EsScriptObjectDataBufferPtr& buff, bool splitCtx) const
{
	EsScriptObjectIntf::Ptr result( new EsScriptArrayObject(m_ctx, (m_flags & ~ofMetaclass) | ofNeedUpdateLayout,
		buff, m_expr, m_itemMetaclass) );
	ES_ASSERT(result);

	ESSCRIPT_OBJECT_TRACE3(esT("New instance of '%s of %s' object type created"), m_typeName.c_str(), m_itemMetaclass->typeNameGet().c_str() )
	return result;
}

ES_IMPL_INTF_METHOD(void, EsScriptArrayObject::setParent)(EsScriptObjectIntf* parent)
{
	EsScriptObject::setParent(parent);
	// in instance mode initialize update subscription, if any
	if( parent &&
			!isMetaclass() &&
			isFinal() )
		initializeUpdateSubscription();
}

ulong EsScriptArrayObject::internalCountGet()
{
	if( isMetaclass() )
		return 0;

	// execute in expressions mode, unless we're in direct assignment mode
	if( m_directBinBufferAssignment )
		return static_cast<ulong>(m_arr.capacity());
	else
	{
		EsScriptValAccessorIntf::Ptr result = m_ctx->vm()->exec(
      m_expr, 
      EsVariant::null(),
			EsScriptMachine::EvalMode::evalExpr, 
      topNonProxyGet()
    );
		ES_ASSERT(result);
		return result->get().asULong();
	}
}

ES_IMPL_INTF_METHOD(void, EsScriptArrayObject::initializeUpdateSubscription)()
{
	if( m_subscriptionInitialized )
		return;

	m_subscriptionInitialized = true;

	ES_ASSERT( !isMetaclass() );
	// set-up subscription to the update notifications
	ESSCRIPT_OBJECT_TRACE3(esT("initializeUpdateSubscription called for '%s' of '%s'"), typeNameGet().c_str(), m_itemMetaclass->typeNameGet().c_str())
	const EsString::Array& fldDependencies = m_expr->thisFieldDependenciesGet();
	for( size_t idx = 0; idx < fldDependencies.size(); ++idx )
	{
		EsScriptObjectFieldFastFinder finder(this, fldDependencies[idx]);
		EsScriptObjectIntf::Ptr fld = fnodeFieldGet( finder.resultGet() );
		ES_ASSERT(fld);
		fld->updateSubscriberAdd(this);
	}

	const EsString::Array& varDependencies = m_expr->thisMemberVarDependenciesGet();
	for( size_t idx = 0; idx < varDependencies.size(); ++idx )
	{
		EsScriptObjectVarFastFinder finder(this, varDependencies[idx]);
		EsScriptValAccessorIntf::Ptr var = finder.resultGet();
		ES_ASSERT(var);
		var->updateSubscriberAdd(this);
	}
}

EsScriptObjectIntf::Ptr EsScriptArrayObject::internalGetItem(ulong idx)
{
	ES_ASSERT(idx < m_arr.size());
	return m_arr[idx];
}

// (try to) set existing object from binary buffer
ES_IMPL_INTF_METHOD(bool, EsScriptArrayObject::internalBinBufferSet)(EsBinBuffer::const_pointer& pos, EsBinBuffer::const_pointer end)
{
	ES_ASSERT( isArray() );

	if( m_itemMetaclass->isPOD() )
	{
		// just copy buffer at once as-is
		if( static_cast<int>(m_size) <= end-pos )
		{
			m_data->assignDirect(pos, m_offs, m_size);
			pos += m_size;
			onSetValueParentNotify();
		}
		else
			return false;
	}
	else
	{
		for(int idx = 0; idx < call(EsStdNames::countGet()).asInt(); ++idx )
		{
			EsScriptObjectIntf::Ptr item = call(EsStdNames::itemGet(), idx).asExistingObject();
			ES_ASSERT(item);
			if( !item->internalBinBufferSet(pos, end) )
				return false;
		}
	}	
	return true;
}

ES_IMPL_INTF_METHOD(void, EsScriptArrayObject::internalUpdateLayout)(ulong offs)
{
	ES_ASSERT(!isMetaclass());

	ESSCRIPT_OBJECT_TRACE4(esT("internalUpdateLayout called for '%s' with offs=%d, ofNeedUpdateLayout is %s"), 
		typeNameGet().c_str(), offs, (m_flags & ofNeedUpdateLayout) ? esT("set") : esT("not set"))
	
	ulong localSize = 0;
	size_t cnt = m_arr.size();
	size_t newCnt = cnt;

	if( m_flags & ofNeedUpdateLayout  )
	{
		newCnt = internalCountGet();
		m_flags &= ~ofNeedUpdateLayout;
	}

	// update layout of existing items in range [0, min(cnt, newCnt)[
	for( size_t idx = 0; idx < std::min(cnt, newCnt); ++idx )
	{
		EsScriptObjectIntf::Ptr item = m_arr[idx];
		item->internalUpdateLayout(offs+localSize);
		localSize += item->sizeGet();
	}		

	if( cnt < newCnt )
	{
		m_arr.reserve( newCnt );

		for( size_t idx = cnt; idx < newCnt; ++idx )
		{
			EsScriptObjectIntf::Ptr item = m_itemMetaclass->internalClone(this, m_data);
			ES_ASSERT(item);
			
      m_arr.push_back(item);
			item->internalUpdateLayout(offs+localSize);
			localSize += item->sizeGet();
		}
	}
	else if( cnt > newCnt )
		m_arr.erase( m_arr.begin()+newCnt, m_arr.end() );

	// update our size cache
	m_size = localSize;
	// set our own offset
	internalOffsetSet(offs);
}
