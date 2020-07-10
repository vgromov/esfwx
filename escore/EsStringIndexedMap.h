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
    EsStringAssocContainerNodeT(const EsString& name, ulong idx, const EsVariant& payload = EsVariant::null()) ES_NOTHROW;
    inline const EsString& nameGet() const ES_NOTHROW { return m_name; }
    inline const EsVariant& payloadGet() const ES_NOTHROW { return m_payload; }

  protected:
    EsString m_name;
    ulong m_idx;
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
  EsStringIndexedMap(ContainerInterlocked interlocked = ContainerUsesInterlock, const EsString& containerName = EsString::null()) ES_NOTHROW;
  EsStringIndexedMap(const EsStringIndexedMap& src, ContainerInterlocked interlocked);
  EsStringIndexedMap(const EsStringIndexedMap& src);
  ~EsStringIndexedMap();

  inline bool isInterlocked() const ES_NOTHROW { return nullptr != m_cs; }

  /// Indexed map API
  ///

  /// Access container name
  inline const EsString& nameGet() const ES_NOTHROW { return m_containerName; }

  /// container emptiness check
  bool isEmpty() const ES_NOTHROW;

  /// Return container items count
  ulong countGet() const ES_NOTHROW;

  /// Find index of item with name, return (npos) if item is not found
  ulong itemFind(const EsString& name) const ES_NOTHROW;

  // The same as above, but for byte strings, with optional length specs
  ulong itemFind(const EsByteString& name, ulong len = 0) const ES_NOTHROW;

  /// Insert named item with payload, return new item index, or
  /// generate 'item already exists' exception, if 'strict' parameter is true.
  /// Otherwise, replace payload of already existing item.
  ///
  ulong itemAdd(const EsString& name, const EsVariant& val = EsVariant::null(), bool strict = true);

  // item deletion by name|index. NB! deletion performance is poor
  void itemDelete(ulong idx);
  void itemDelete(const EsString& name, bool doThrow = false);

  // remove all items from container
  void clear();

  // return array of all item names, in the order in which items were added
  EsString::Array namesGet() const ES_NOTHROW;

  // Return true if item with specified name exists
  bool itemExists(const EsString& name) const ES_NOTHROW;

  // Same as above, but for ByteStrings
  bool itemExists(const EsByteString& name, ulong len = 0) const ES_NOTHROW;

  // return item name at index
  const EsString& nameGet(ulong idx) const;

  // named payload access
  const EsVariant& valueGet(const EsString& name) const;
  void valueSet(const EsString& name, const EsVariant& val);

  // indexed payload access
  const EsVariant& valueGet(ulong idx) const;
  void valueSet(ulong idx, const EsVariant& val);

  // operators
  //
  // assignment
  EsStringIndexedMap& operator=(const EsStringIndexedMap& src);

  // equality|inequality comparison. NB! comparison is performed on item names only,
  // payload content is not counted for
  bool operator==(const EsStringIndexedMap& src) const ES_NOTHROW;
  bool operator!=(const EsStringIndexedMap& src) const ES_NOTHROW;

  static const ulong npos = static_cast<ulong>(-1);

protected:
  // internal helpers
  void copyFrom(const EsStringIndexedMap& src);

  // insert new item in collection and returns new item index, or throws an exception if item already exists
  ulong internalFind(const EsString& name) const ES_NOTHROW;
  ulong internalFind(const EsByteString& name, ulong len) const ES_NOTHROW;
  ulong internalInsert(const EsString& name, const EsVariant& payload, bool doThrow);
  void internalDelete(ulong idx);
  void throwItemDoesNotExist(const EsString& name) const;

protected:
  // data members
  mutable std::unique_ptr<EsCriticalSection> m_cs;
  EsString m_containerName;
  EsStringAssocMapT m_m;
  EsStringAssocVectorT m_v;
};

#endif

