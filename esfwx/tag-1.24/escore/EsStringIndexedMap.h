#ifndef _es_string_indexed_map_h_
#define _es_string_indexed_map_h_

// indexed map is special container,
// which provides dual access, name by index and index by name
// optional payload may be associated with each container item
// item deletion is supported as well, but this operation is 
// not performance-efficient
//
class ESCORE_CLASS EsStringIndexedMap
{
public:
  enum ContainerInterlocked {
    ContainerUsesInterlock,
    ContainerWithoutInterlock
  };
  
  typedef std::shared_ptr< EsStringIndexedMap > Ptr;

	// internal types
	//
	class EsStringAssocContainerNodeT
	{
  public:
		EsStringAssocContainerNodeT(const EsString& name, size_t idx, const EsVariant& payload = EsVariant::null()) ES_NOTHROW;
    const EsString& nameGet() const ES_NOTHROW { return m_name; }
    const EsVariant& payloadGet() const ES_NOTHROW { return m_payload; }

  protected:
		EsString m_name;
		size_t m_idx;
		EsVariant m_payload;

  private:
    EsStringAssocContainerNodeT() ES_REMOVEDECL;
    EsStringAssocContainerNodeT(const EsStringAssocContainerNodeT&) ES_REMOVEDECL;
    EsStringAssocContainerNodeT& operator= (const EsStringAssocContainerNodeT&) ES_REMOVEDECL;

    friend class EsStringIndexedMap;
	};
	typedef std::shared_ptr< EsStringAssocContainerNodeT > EsStringAssocContainerNodePtrT;
	typedef std::map< EsString::HashT, EsStringAssocContainerNodePtrT > EsStringAssocMapT;
	typedef std::vector< EsStringAssocContainerNodePtrT > EsStringAssocVectorT;

public:
  EsStringIndexedMap( ContainerInterlocked interlocked ) ES_NOTHROW;
	EsStringIndexedMap(const EsString& containerName = EsString::null(), ContainerInterlocked interlocked = ContainerUsesInterlock) ES_NOTHROW;
	EsStringIndexedMap(const EsStringIndexedMap& src, ContainerInterlocked interlocked = ContainerUsesInterlock);
  ~EsStringIndexedMap() ES_NOTHROW;

  inline bool isInterlocked() const ES_NOTHROW { return nullptr != m_cs; }

	/// Indexed map API
	///

	/// Access container name
	const EsString& nameGet() const ES_NOTHROW { return m_containerName; }

	/// container emptiness check
	bool isEmpty() const ES_NOTHROW;

	/// Return container items count
	size_t countGet() const ES_NOTHROW;

	/// Find index of item with name, return (npos) if item is not found
	size_t itemFind(const EsString& name) const ES_NOTHROW;

  // The same as above, but for byte strings, with optional length specs
	size_t itemFind(const EsByteString& name, size_t len = 0) const ES_NOTHROW;

	/// Insert named item with payload, return new item index, or
	/// generate 'item already exists' exception, if 'strict' parameter is true.
  /// Otherwise, replace payload of already existing item.
  ///
	size_t itemAdd(const EsString& name, const EsVariant& val = EsVariant::null(), bool strict = true);

	// item deletion by name|index. NB! deletion performance is poor
	void itemDelete(size_t idx);
	void itemDelete(const EsString& name, bool doThrow = false);

  // remove all items from container
	void clear();

  // return array of all item names, in the order in which items were added
	EsString::Array namesGet() const ES_NOTHROW;

	// Return true if item with specified name exists
	bool itemExists(const EsString& name) const ES_NOTHROW;

	// Same as above, but for ByteStrings
	bool itemExists(const EsByteString& name, size_t len = 0) const ES_NOTHROW;

	// return item name at index
	const EsString& nameGet(size_t idx) const;

	// named payload access
	const EsVariant& valueGet(const EsString& name) const;
	void valueSet(const EsString& name, const EsVariant& val);
	
  // indexed payload access
	const EsVariant& valueGet(size_t idx) const;
	void valueSet(size_t idx, const EsVariant& val);

	// operators
	//
	// assignment
	EsStringIndexedMap& operator=(const EsStringIndexedMap& src);

	// equality|inequality comparison. NB! comparison is performed on item names only,
	// payload content is not counted for
	bool operator==(const EsStringIndexedMap& src) const ES_NOTHROW;
	bool operator!=(const EsStringIndexedMap& src) const ES_NOTHROW;

	static const size_t npos = static_cast<size_t>(-1);

protected:
	// internal helpers
	void copyFrom(const EsStringIndexedMap& src);

	// insert new item in collection and returns new item index, or throws an exception if item already exists
	size_t internalFind(const EsString& name) const ES_NOTHROW;
  size_t internalFind(const EsByteString& name, size_t len) const ES_NOTHROW;
	size_t internalInsert(const EsString& name, const EsVariant& payload, bool doThrow);
	void internalDelete(size_t idx);
	void throwItemDoesNotExist(const EsString& name) const;

protected:
	// data members
	mutable std::unique_ptr<EsCriticalSection> m_cs;
	EsString m_containerName;
	EsStringAssocMapT m_m;
	EsStringAssocVectorT m_v;
};

#endif

