#ifndef _es_stream_block_h_
#define _es_stream_block_h_

// Forward delcs
class EsStream;

/// Generic stream block
///
class ESCORE_CLASS EsStreamBlock : public std::enable_shared_from_this<EsStreamBlock>
{
public:
  typedef std::shared_ptr<EsStreamBlock> Ptr;
  typedef std::weak_ptr<EsStreamBlock> WeakPtr;

  /// Stream Block IDs
  enum
  {
    None         = 0,   ///< Block is undefined, block is considered invalid
    Context,            ///< Block is context block, either root, or nested one

    Fields,             ///< Block is object fields group
    Properties,         ///< Block is object properties group
    Items,              ///< Block is items collection group

    Object,             ///< Block is object contents group
    Field,              ///< Block is field data
    Property,           ///< Block is property data
    Item,               ///< Block is item data
    Value,              ///< Block is named value data

    _IdsCnt_            ///< Special value, must go last
  };

protected:
  /// Internal types
  typedef std::shared_ptr<EsVariant> ValPtr;
  typedef std::shared_ptr<EsStringIndexedMap> AttrsPtr;
  typedef std::multimap<EsString::HashT, EsStreamBlock::Ptr> BlocksT;
  typedef std::pair<BlocksT::iterator, BlocksT::iterator> BlocksRangeT;

protected:
  EsStreamBlock(const EsStreamBlock::Ptr& parent, ulong id, const EsString& name);

public:
  ~EsStreamBlock();

  /// Misc checks and short-cuts
  ///

  /// Return true if block is of known type, i.e., of valid ID
  static bool isKnownId(ulong id) ES_NOTHROW { return None < id && id < _IdsCnt_; }

  /// Return true if block is of known type, i.e., of valid ID
  bool isKnownId() const ES_NOTHROW { return isKnownId(m_id); }

  /// Return true, if block is of context type, root or nested
  bool isContext() const ES_NOTHROW { return Context == m_id; }

  /// Return true, if block is root context
  bool isRoot() const ES_NOTHROW;

  /// Return true if block is of Object type
  bool isObject() const ES_NOTHROW { return Object == m_id; }

  /// Return true if block is of Field type
  bool isField() const ES_NOTHROW { return Field == m_id; }

  /// Return true if block is of Property type
  bool isProperty() const ES_NOTHROW { return Property == m_id; }

  /// Return true if block is of item type
  bool isItem() const ES_NOTHROW { return Item == m_id; }

  /// Return true, if block does not have any content
  bool isEmpty() const ES_NOTHROW { return m_children.empty() && !m_payload; }

  /// Return true if block contains children blocks
  bool haveChildren() const ES_NOTHROW { return !m_children.empty(); }

  /// Return true, if block have only single child
  bool haveSingleChild() const ES_NOTHROW { return 1 == m_children.size(); }

  /// Return count of immediate children blocks
  size_t childrenCountGet() const ES_NOTHROW { return m_children.size(); }

  /// Return true if block is the first child in its parent
  bool isFirst() const ES_NOTHROW;

  /// Return true if block is the last child in its parent
  bool isLast() const ES_NOTHROW;

  /// Return true, if block have POD payload value
  bool isPOD() const ES_NOTHROW { return nullptr != m_payload; }

  /// Return true, if block's name is empty
  bool nameIsEmpty() const ES_NOTHROW { return m_name.empty(); }

  /// Return true if block may contain type attribute
  bool supportsTypes() const ES_NOTHROW { return m_id >= Object && m_id < _IdsCnt_; }

  /// Return true if Block supports type name attribute
  bool supportsTypeName() const ES_NOTHROW { return Object <= m_id && Value >= m_id; }

  /// Generic Block API
  ///

  /// Return block ID, assigned upon construction
  ulong idGet() const ES_NOTHROW { return m_id; }

  /// Return string name of known block ID. If id is unknown, an exception is thrown
  static const EsString& idNameGet(ulong id);

  /// Return string name of this block ID. If id is unknown, an exception is thrown
  const EsString& idNameGet() const { return idNameGet(m_id); }

  /// Return ID value by ID name string. If unknown ID name is specified, None is returned
  static ulong idByNameGet(const EsString& name);

  /// Return true, if block supports versioning. Version attribute is only supported for
  /// the Contexts and Objects
  ///
  bool supportsVersion() const ES_NOTHROW { return Object == m_id || Context == m_id; }

  /// Return block version, or throw, if versioning is not supported for this block type
  ulong versionGet() const;

  /// Assing version attribute to the block, if it's supported
  void versionSet(ulong version);

  /// Return true if block supports indexing (is of Item type)
  bool supportsIndexing() const ES_NOTHROW { return Item == m_id; }

  /// Return block index, if this block supports indexing (is of Item kind).
  /// Otherwise, an exception is thrown
  ///
  ulong idxGet() const;

  /// Return true if block have type ID attribute assigned
  bool haveType() const;

  /// Return block type ID. It's the same as EsVariantType for block, which have
  /// their payload assigned. For compound blocks, like collections or objects,
  /// the type is stored in attribute collection, instead.
  ///
  ulong typeGet() const;

  /// Set block type ID, if it's supported. Note, that assigning POD payload,
  /// will effectively reset any other type attribute value, if it was previously set
  ///
  void typeSet(ulong type);

  /// Return block name string, if block supportes naming. For object blocks,
  /// name is the object type name
  const EsString& nameGet() const ES_NOTHROW { return m_name; }

  /// Return true, if Block have type name attribute assigned
  bool haveTypeName() const ES_NOTHROW { return attributeExists( typeName() ); }

  /// Return block type name attribute, if it's supported. For object blocks,
  /// the block name is returned.
  ///
  EsString typeNameGet() const;

  /// Assign type name attribute, if supported
  void typeNameSet(const EsString& typeName);

  /// Return true if POD payload is assigned to block
  bool havePayload() const ES_NOTHROW { return nullptr != m_payload; }

  /// Return block's POD payload reference, or throws, if payoad is not assigned
  const EsVariant& payloadGet() const { return *m_payload.get(); }

  /// Set block's payload, if supported. Otherwise, throws an exception.
  void payloadSet(const EsVariant& payload);

  /// Remove payload from the block
  void payloadReset() ES_NOTHROW { m_payload.reset(); }

  /// Try to find child block based on its name and ID. Return 0, if not found
  EsStreamBlock::Ptr childGet(const EsString& name, ulong id = None);

  /// Append child block with specific name, and ID
  EsStreamBlock::Ptr childAdd(const EsString& name, ulong id);

  /// Append child block before the subj one. If subj is 0, just append child to the end
  EsStreamBlock::Ptr childAddBefore(const EsStreamBlock::Ptr& subj, const EsString& name, ulong id);

  /// Reset block contents. Version, if it's supported, is retained
  void reset();

  /// Reset block contents. Version, if it's supported, is set to ver
  void reset(ulong ver);

  /// Return block's parent
  EsStreamBlock::Ptr parentGet() const ES_NOTHROW { return m_parent.lock(); }

  /// Remove child block with specified name and ID. If ID == None, look-up blocks
  /// by name only, and remove the first one found.
  ///
  void childRemove(const EsString& name, ulong id = None);

  /// Remove child by its pointer. Child must be an immediate child, otherwise,
  /// no operation is performed.
  ///
  void childRemove(const EsStreamBlock::Ptr& child);

  /// Get the first child
  EsStreamBlock::Ptr firstChildGet() const ES_NOTHROW { return m_first.lock(); }

  /// Get the last child
  EsStreamBlock::Ptr lastChildGet() const ES_NOTHROW { return m_last.lock(); }

  /// Get the previous sibling
  EsStreamBlock::Ptr prevSiblingGet() const ES_NOTHROW { return m_prev.lock(); }

  /// Get the next sibling
  EsStreamBlock::Ptr nextSiblingGet() const ES_NOTHROW { return m_next.lock(); }

  /// Get the first immediate child with the specified ID
  EsStreamBlock::Ptr firstChildGet(ulong id) const;

  /// Get the next sibling with the specified ID
  EsStreamBlock::Ptr nextSiblingGet(ulong id) const;

  /// Optional Attributes API
  ///

  /// Return true if block have attribute records
  bool haveAttributes() const ES_NOTHROW { return nullptr != m_attrs && !m_attrs->isEmpty(); }

  /// Return RO attributes map, if exists, otherwise, throws
  const EsStringIndexedMap& attributesGet() const;

  /// Return true, if attribute with specified name exists in block
  bool attributeExists(const EsString& attrName) const;

  /// Delete attributes container
  void attributesReset();

  /// Add named attribute with specific value. If attrubute with the same name already exists,
  /// replace its value with the specified one.
  ///
  void attributeAdd(const EsString& attrName, const EsVariant& attrVal);

  /// Return an attribute value, or throw, if value does not exist
  const EsVariant& attributeGet(const EsString& attrName, bool doThrow = false) const;

  /// Remove specific attribute from block, if any
  void attributeDelete(const EsString& attrName);

  /// Specific Block access API
  ///

  /// Append an indexed Item child block, with auto-indexing
  EsStreamBlock::Ptr itemAdd();

  /// Append an indexed Item child block, with specified index
  EsStreamBlock::Ptr itemAdd(ulong idx);

  /// Try to get the Item child by its index. Return 0, if not found
  EsStreamBlock::Ptr itemGet(ulong idx);

  /// Stream copying
  ///

  /// Copy entire contents of other to us. Only root-level copying calls are allowed.
  /// If copyFrom is called woth non-root level Blocks, an exception is thrown
  ///
  void copyFrom(const EsStreamBlock::Ptr& other);

  /// Misc known names
  ///
  static const EsString& root();
  static const EsString& name();
  static const EsString& version();
  static const EsString& index();
  static const EsString& type();
  static const EsString& typeName();
  static const EsString& label();

  /// Debug-only functionality. Dump Block contents and children to string, in aligned, readable manner
#ifdef ES_DEBUG
  void stringDump(EsString& out, ulong offs = 0) const;

protected:
  void stringDumpHeader(EsString& out, ulong offs) const;
  void stringDumpAttributes(EsString& out, ulong offs) const;
  void stringDumpContents(EsString& out, ulong offs) const;
#endif

protected:
  /// Internal block addition helper
  void internalChildAddBefore(const EsStreamBlock::Ptr& subj, const EsStreamBlock::Ptr& child, ullong key);

  /// Return the first child iterator by specified key, and id
  BlocksT::iterator childItGet(EsString::HashT key, ulong id);

  /// Check ID validity, throw, if ID is not valid
  void idValidityCheck() const;

  /// Check if block name is valid
  void nameValidityCheck() const;

  /// Check if block supports versioning
  void checkSupportsVersioning() const;

  /// Check if block supports type attribute
  void checkSupportsTypes() const;

  /// Check if attribute container is initialized
  void checkAttrsInitialized() const;

  /// Check if attribute container is not empty
  void checkAttrsNotEmpty() const;

  /// Check if block supports type name
  void checkSupportsTypeName() const;

  /// Check if Block supports payload
  void checkSupportsPayload() const;

  /// Check if Block supports indexed access|indexing
  void checkSupportsIndexing() const;

  /// Initialize attribute contained once
  void internalAttrsInit();

  /// Insert clone of src into this Block children chain
  void cloneAdd(const EsStreamBlock::Ptr& src);

protected:
  ulong m_id;                             ///< Block ID
  EsStreamBlock::WeakPtr m_parent;        ///< Parent block
  EsStreamBlock::WeakPtr m_first;         ///< First child block
  EsStreamBlock::WeakPtr m_last;          ///< Last child block
  EsStreamBlock::WeakPtr m_prev;          ///< Previous sibling
  EsStreamBlock::WeakPtr m_next;          ///< Next sibling
  BlocksT m_children;                     ///< Mapped children collection
  EsString m_name;                        ///< Optional block name
  AttrsPtr m_attrs;                       ///< Optional block attributes
  ValPtr m_payload;                       ///< Optional block payload value

private:
  EsStreamBlock() ES_REMOVEDECL;
  EsStreamBlock(const EsStreamBlock&) ES_REMOVEDECL;
  EsStreamBlock& operator=(const EsStreamBlock&) ES_REMOVEDECL;

  friend class EsStream;
};

#endif // _es_stream_block_h_
