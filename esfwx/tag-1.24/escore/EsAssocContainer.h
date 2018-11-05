#ifndef _es_assoc_container_h_
#define _es_assoc_container_h_

/// Associative container key => value interface
///
ES_DECL_INTF_BEGIN2( 84A3CB81, 27A940EF, 8B29486E, FA9BE1A7, EsAssocContainerIntf, EsBaseIntf )

	/// Assoc. collection api
  ///
	ES_DECL_INTF_METHOD(bool, isEmpty)() const = 0;
	ES_DECL_INTF_METHOD(void, clear)() = 0;
	ES_DECL_INTF_METHOD(EsVariant, allKeysGet)() const = 0;
	ES_DECL_INTF_METHOD(EsVariant, allValuesGet)() const = 0;
	ES_DECL_INTF_METHOD(void, keyChange)(cr_EsVariant, cr_EsVariant) = 0;
  ES_DECL_INTF_METHOD(const EsVariant&, valueGet)(cr_EsVariant) const = 0;
	ES_DECL_INTF_METHOD(const EsVariant&, valueGetDef)(cr_EsVariant, cr_EsVariant) const = 0;
	ES_DECL_INTF_METHOD(EsVariant, indexGet)(cr_EsVariant) const = 0;
	ES_DECL_INTF_METHOD(void, valueSet)(cr_EsVariant, cr_EsVariant) = 0;
	ES_DECL_INTF_METHOD(void, newValueSet)(cr_EsVariant, cr_EsVariant) = 0;
	ES_DECL_INTF_METHOD(bool, keyExists)(cr_EsVariant) const = 0;
	ES_DECL_INTF_METHOD(void, valueDelete)(cr_EsVariant) = 0;

	/// Indexed access api
  ///
	ES_DECL_INTF_METHOD(void, itemsClear)() = 0;
	ES_DECL_INTF_METHOD(ulong, countGet)() const = 0;

	/// itemGet returns key-value pair passed as VAR_VARIANT_COLLECTION
	ES_DECL_INTF_METHOD(EsVariant, itemGet)(cr_EsVariant) const = 0;
	ES_DECL_INTF_METHOD(void, itemSet)(cr_EsVariant, cr_EsVariant) = 0;
	ES_DECL_INTF_METHOD(void, itemDelete)(cr_EsVariant) = 0;
	/// itemAppend here expects key-value pair passed as VAR_VARIANT_COLLECTION in single argument
	ES_DECL_INTF_METHOD(void, itemAppend)(cr_EsVariant) = 0;

ES_DECL_INTF_END

// reflected associative container key => value
//
class EsAssocContainer;

class ESCORE_CLASS EsAssocContainerNode
{
public:
	typedef std::shared_ptr<EsAssocContainerNode> Ptr;

protected:
	EsAssocContainerNode(EsAssocContainer& owner, const EsVariant& key, const EsVariant& val);
	static EsAssocContainerNode::Ptr create(EsAssocContainer& owner, const EsVariant& key, const EsVariant& val);

public:
	const EsVariant& keyGet() const { return m_key; }
	const EsVariant& valGet() const { return m_val; }
	EsVariant& valGet() { return m_val; }

protected:
	void keySet(const EsVariant& key) { m_key = key; }

private:
	// prohibited functionality
	EsAssocContainerNode() ES_REMOVEDECL;
	EsAssocContainerNode(const EsAssocContainer&) ES_REMOVEDECL;
	const EsAssocContainerNode& operator= (const EsAssocContainerNode&) ES_REMOVEDECL;

protected:
	EsAssocContainer& m_owner;
	EsVariant m_key;
	EsVariant m_val;

	friend class EsAssocContainer;
};

class ESCORE_CLASS ES_INTF_IMPL2(EsAssocContainer, EsAssocContainerIntf, EsReflectedClassIntf)

public:
	EsAssocContainer(EsBaseIntf* owner);
  static EsAssocContainerIntf::Ptr create(EsBaseIntf* owner = nullptr);

  virtual ~EsAssocContainer();

	/// Reflection declaration
	///
	ES_DECL_REFLECTED_CLASS_BASE( EsAssocContainer )
	ES_DECL_ATTR_HANDLING_STD

	/// Reflected properties
	///

	/// Check for empty contents
	ES_DECL_PROPERTY_RO(empty, bool)
	/// Return elements count
	ES_DECL_PROPERTY_RO(count, ulong)
	/// Return collection of all container keys
	ES_DECL_PROPERTY_RO(keys, EsVariant)
	/// Return collection of all container values
	ES_DECL_PROPERTY_RO(values, EsVariant)

	/// Reflected methods
	///

	/// EsAssocContainerIntf reflection
	///
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isEmpty);
	ES_DECL_REFLECTED_INTF_METHOD0(void, clear);
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsVariant, allKeysGet);
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsVariant, allValuesGet);
	ES_DECL_REFLECTED_INTF_METHOD2(void, keyChange, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(cr_EsVariant, valueGet, cr_EsVariant);
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(cr_EsVariant, valueGetDef, cr_EsVariant, cr_EsVariant);
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, indexGet, cr_EsVariant);
	ES_DECL_REFLECTED_INTF_METHOD2(void, valueSet, cr_EsVariant, cr_EsVariant);
	ES_DECL_REFLECTED_INTF_METHOD2(void, newValueSet, cr_EsVariant, cr_EsVariant);
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, keyExists, cr_EsVariant);
	ES_DECL_REFLECTED_INTF_METHOD1(void, valueDelete, cr_EsVariant);

	/// Indexed access api
	///
	ES_DECL_REFLECTED_INTF_METHOD0(void, itemsClear);
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(ulong, countGet);
	ES_DECL_REFLECTED_INTF_METHOD1(void, itemDelete, cr_EsVariant);

	/// itemGet returns key-value pair passed as VAR_VARIANT_COLLECTION
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, itemGet, cr_EsVariant);
	ES_DECL_REFLECTED_INTF_METHOD2(void, itemSet, cr_EsVariant, cr_EsVariant);

	/// itemAppend here expects key-value pair passed as VAR_VARIANT_COLLECTION in single argument
	ES_DECL_REFLECTED_INTF_METHOD1(void, itemAppend, cr_EsVariant);

	/// Ctors
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);
	ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, cr_EsBaseIntfPtr);

  /// Cloning
  ES_DECL_REFLECTED_CONST_METHOD0(EsBaseIntfPtr, clone);

protected:
	typedef std::map<EsVariant, size_t> Mapped;
	// helper services
	Mapped::iterator checkKeyExistsAndNodeGet(const EsVariant& key, bool doThrow);
	Mapped::const_iterator checkKeyExistsAndNodeGet(const EsVariant& key, bool doThrow) const;
	static void throwKeyIsNotFound(const EsVariant& key);
	void checkKeyDoesNotExist(const EsVariant& key);
	// overridable services
	virtual void keyCheck(const EsVariant& key) const;
	virtual void valCheck(const EsVariant& key, const EsVariant& val) const;
	virtual void doClear();
	void indexCheck(ulong idx) const;
	void internalDelete(Mapped::iterator it, size_t idx);
	
protected:
	// NB! owner instance is not owned :) by this smartptr
	// to prevent instance deadlock
	EsBaseIntfPtr m_owner;

	typedef std::vector<EsAssocContainerNode::Ptr> Indexed;
#pragma warning(push)
#pragma warning(disable: 4251)
	Indexed m_indexed;
	Mapped m_mapped;
#pragma warning(pop)

private:
	// prohibited functionality
	EsAssocContainer() ES_REMOVEDECL;
	EsAssocContainer(const EsAssocContainer&) ES_REMOVEDECL;
	const EsAssocContainer& operator=(const EsAssocContainer&) ES_REMOVEDECL;
};

#endif // _es_assoc_container_h_