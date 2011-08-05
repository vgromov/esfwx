#ifndef _es_val_accessors_h_
#define _es_val_accessors_h_

// special value accessor objects, used internally to provide transparent item|field|property access
// in lhs and rhs expressions
//
ES_DECL_INTF_BEGIN2(35E7B5D4, D6D8400f, BA799EED, CC38B82E, EsAccessorIntf, EsBaseIntf)
	ES_DECL_INTF_METHOD(EsVariant, get)() const = 0;
	ES_DECL_INTF_METHOD(EsVariant&, acc)() = 0;
	ES_DECL_INTF_METHOD(void, set)(const EsVariant& val) = 0;
ES_DECL_INTF_END 

// const accessor
class EsConstAccessor : public EsAccessorIntf
{
protected:
	EsConstAccessor(const EsVariant& val);
	static EsAccessorIntf::Ptr create(const EsVariant& var)
	{
		return EsAccessorIntf::Ptr(new EsConstAccessor(var));
	}
	
public:
	ES_INTF_MAP_BEGIN(EsConstAccessor)
		ES_INTF_SUPPORTS(EsConstAccessor, EsAccessorIntf)
	ES_INTF_MAP_END
	{
		delete this;	
	}
	
	// EsBaseIntf override
	//
	ES_DECL_INTF_METHOD(EsString, getTypeName)() const 
	{ 
		return getClassName(); 
	}
	
	// interface impl
	ES_DECL_INTF_METHOD(EsVariant, get)() const;
	ES_DECL_INTF_METHOD(EsVariant&, acc)();
	ES_DECL_INTF_METHOD(void, set)(const EsVariant& val);

protected:
	// data members
	EsVariant m_val; // const value
	
	friend class EsScriptMachine;	
	friend class EsCodeSection;
	friend class EsInlineCodeSection;
};

// variable accessor
class EsValAccessor : public EsAccessorIntf
{
protected:
	// friend-only functionality
	EsValAccessor(const EsVariant& var, const std::locale& loc);
	static EsAccessorIntf::Ptr create(const EsVariant& var, const std::locale& loc)
	{
		return EsAccessorIntf::Ptr(new EsValAccessor(var, loc));
	}
	
public:
	ES_INTF_MAP_BEGIN(EsValAccessor)
		ES_INTF_SUPPORTS(EsValAccessor, EsAccessorIntf)
	ES_INTF_MAP_END
	{
		delete this;	
	}
	
	// EsBaseIntf override
	//
	ES_DECL_INTF_METHOD(EsString, getTypeName)() const 
	{ 
		return getClassName(); 
	}
	
	// interface impl
	ES_DECL_INTF_METHOD(EsVariant, get)() const;
	ES_DECL_INTF_METHOD(EsVariant&, acc)();
	ES_DECL_INTF_METHOD(void, set)(const EsVariant& val);

protected:
	// data members
	const std::locale& m_loc;
	EsVariant m_val; // either value, or object 	
	
	friend class EsSymTable;
	friend class EsCodeSection;
	friend class EsInlineCodeSection;
	friend class EsScriptObject;
};

// indexed item accessor
class EsItemAccessor : public EsAccessorIntf
{
protected:
	// friend-only functionality
	EsItemAccessor(EsAccessorIntf::Ptr valAcc, EsAccessorIntf::Ptr idxExprAcc, const std::locale& loc);
	static EsAccessorIntf::Ptr create(EsAccessorIntf::Ptr valAcc, EsAccessorIntf::Ptr idxExprAcc, const std::locale& loc)
	{
		return EsAccessorIntf::Ptr(new EsItemAccessor(valAcc, idxExprAcc, loc));
	}
	
public:
	ES_INTF_MAP_BEGIN(EsItemAccessor)
		ES_INTF_SUPPORTS(EsItemAccessor, EsAccessorIntf)
	ES_INTF_MAP_END
	{
		delete this;	
	}

	// EsBaseIntf override
	//
	ES_DECL_INTF_METHOD(EsString, getTypeName)() const 
	{ 
		return getClassName(); 
	}
	
	// interface impl
	ES_DECL_INTF_METHOD(EsVariant, get)() const;
	ES_DECL_INTF_METHOD(EsVariant&, acc)();
	ES_DECL_INTF_METHOD(void, set)(const EsVariant& val);	
	
protected:
	// data members
	const std::locale& m_loc;
	EsAccessorIntf::Ptr m_valAcc;
	EsAccessorIntf::Ptr m_idxExprAcc;
	
	friend class EsScriptMachine;
	friend class EsCodeSection;
	friend class EsInlineCodeSection;
};

#endif // _es_val_accessors_h_
