#ifndef _es_stream_h_
#define _es_stream_h_

/// Stream interface
///
ES_DECL_INTF_BEGIN2(B07715A1, 5FB749AC, ADC74A9B, 3B36E662, EsStreamIntf, EsBaseIntf)

  /// Stream emptiness check
  ES_DECL_INTF_METHOD(bool, isEmpty)() const = 0;

  /// Rewind stream to the beginning
  ES_DECL_INTF_METHOD(void, rewind)() = 0;

  /// Reset stream, erasing its contents
  ES_DECL_INTF_METHOD(void, reset)() = 0;

  /// Reset stream, erasing its contents, assigning specified root context version
  ES_DECL_INTF_METHOD(void, reset)(ulong newVersion) = 0;

  /// Open existing|create new context in currently active one
  ES_DECL_INTF_METHOD(bool, contextOpenCreate)(const EsString& name) = 0;

  /// Open existing|create new context in currently active one, assigning version to newly created one
  ES_DECL_INTF_METHOD(bool, contextOpenCreate)(const EsString& name, ulong newVersion) = 0;

  /// Open existing context
  ES_DECL_INTF_METHOD(bool, contextOpenExisting)(const EsString& name) = 0;

  /// Close currently active context, except for the root one
  ES_DECL_INTF_METHOD(void, contextCurrentClose)() = 0;

  /// Remove context, if exists, in currently active one
  ES_DECL_INTF_METHOD(void, contextRemove)(const EsString& name) = 0;

  /// Remove all entries from the currently active context
  ES_DECL_INTF_METHOD(void, contextCurrentReset)() = 0;

  /// Remove all entries from the currently active context, assigning version for the empty context
  ES_DECL_INTF_METHOD(void, contextCurrentReset)(ulong newVersion) = 0;

  /// Get name of currently active context
  ES_DECL_INTF_METHOD(EsString, contextCurrentNameGet)() const = 0;

  /// Check if current context is empty
  ES_DECL_INTF_METHOD(bool, contextCurrentIsEmpty)() const = 0;

  /// Get current context version
  ES_DECL_INTF_METHOD(ulong, contextCurrentVersionGet)() const = 0;

  /// Set current context version. Stream must be writeable.
  ES_DECL_INTF_METHOD(void, contextCurrentVersionSet)(ulong newVersion) = 0;

  /// Get root context version
  ES_DECL_INTF_METHOD(ulong, rootVersionGet)() const = 0;

  /// Read-write named value from|to the current context
  ///

  /// Read named value. if value does not exist - return supplied default value
  ES_DECL_INTF_METHOD(EsVariant, valueRead)(const EsString& name, const EsVariant& def = EsVariant::null()) = 0;

  /// Read named object value. try read object value into existing object
  ES_DECL_INTF_METHOD(void, valueReadObject)(const EsString& name, const EsBaseIntfPtr& obj) = 0;

  /// If value with such name already exists in current context - just rewrite it
  ES_DECL_INTF_METHOD(void, valueWrite)(const EsString& name, const EsVariant& val) = 0;

  /// Inline value read and write
  ES_DECL_INTF_METHOD(EsVariant, valueRead)() = 0;
  ES_DECL_INTF_METHOD(void, valueWrite)(const EsVariant& val) = 0;

  /// Named value existence check
  ES_DECL_INTF_METHOD(bool, valueExists)(const EsString& name) const = 0;

  /// Named value delete
  ES_DECL_INTF_METHOD(void, valueRemove)(const EsString& name) = 0;

  /// Create|read object from stream
  ES_DECL_INTF_METHOD(EsBaseIntfPtr, objectCreate)() = 0;
  ES_DECL_INTF_METHOD(void, objectRead)(const EsBaseIntfPtr& obj) = 0;

  /// Write object to stream
  ES_DECL_INTF_METHOD(void, objectWrite)(const EsBaseIntfPtr& obj) = 0;

  /// Iterate root object entries in stream
  ES_DECL_INTF_METHOD(bool, firstRootObjectLocate)() = 0;
  ES_DECL_INTF_METHOD(bool, nextRootObjectLocate)() = 0;

  /// Return current object type name
  ES_DECL_INTF_METHOD(EsString, rootObjectTypeNameGet)() const = 0;

  /// Locate root object entry in stream by its type name,
  /// return true if entry is found, false otherwise.
  /// optionally, throw exception if no entry is found and
  /// doThrow is set
  ///
  ES_DECL_INTF_METHOD(bool, rootObjectTypeEntryLocate)(cr_EsString typeName, bool doThrow) = 0;

  /// Return stream contents as string
  ES_DECL_INTF_METHOD(EsString, asString)() const = 0;

  /// Set custom object creation factory
  ES_DECL_INTF_METHOD(void, factorySet)(const EsBaseIntfPtr& factory) = 0;

  /// Copy contents from other stream object
  ES_DECL_INTF_METHOD(void, fromStream)(const EsStreamIntf::Ptr& other) = 0;

  /// Debug-only functionality. Dump Block contents and children to string, in aligned, readable manner
#ifdef ES_DEBUG
  ES_DECL_INTF_METHOD(EsString, debugDump)() const = 0;
#endif

ES_DECL_INTF_END

/// Stream flags
///
enum class EsStreamFlag : ulong
{
  Read                = 0x01, ///< Stream is open for Read operations,
                              ///  if stream is file-based, and file exists, file contents got read and parsed first
  Write               = 0x02, ///< Stream is open for Write operations
  File                = 0x04, ///< Stream is file-based, and supports file save operations,
                              ///  and automatic file save upon object destruction
  Encrypted           = 0x08, ///< Stream contents are encrypted. Not all stream types support encryption
  Compressed          = 0x10, ///< Stream contents are copressed. Not all stream types support compression
  NonStrictProperties = 0x20, ///< Objects, when reading from stream, do not require all persistent properties to
                              ///  be present in stream contents
  NonStrictFields     = 0x40  ///< Objects, when reading from stream, do not require all fields to
                              ///  be present in stream contents
};

/// Reflected object serialization stream interface
///
class ESCORE_CLASS ES_INTF_IMPL2(EsStream, EsStreamIntf, EsReflectedClassIntf)

public:
  enum {
    // internal flags
    flagCustomRead  = 0x01000000,
    flagCustomWrite = 0x02000000,
    flagDirty       = 0x80000000
  };

protected:
  class BlockScope
  {
  public:
    BlockScope(EsStream& owner, EsStreamBlock* block) ES_NOTHROW :
    m_old(nullptr),
    m_block(block),
    m_owner(owner)
    {
      m_old = m_owner.m_block;
      m_owner.m_block = m_block;
    }

    ~BlockScope()
    {
      m_owner.m_block = m_old;
    }

    inline bool isOk() const ES_NOTHROW { return 0 != m_block; }

  protected:
    EsStreamBlock* m_old;
    EsStreamBlock* m_block;
    EsStream& m_owner;

  private:
    BlockScope();
    BlockScope(const BlockScope&);
    BlockScope& operator=(const BlockScope&);
  };

  class ObjectReadScope
  {
  public:
    ObjectReadScope(EsStream& stream, const EsReflectedClassIntf::Ptr& obj);
    ~ObjectReadScope();

  protected:
    EsStream& m_stream;
    ObjectReadScope* m_oldScope;
    const EsReflectedClassIntf::Ptr& m_obj;
    bool m_customReading;

  private:
    ObjectReadScope();
    ObjectReadScope(const ObjectReadScope&);
    ObjectReadScope& operator=(const ObjectReadScope&);
  };
  friend class ObjectReadScope;

  class FieldsReadScope
  {
  public:
    FieldsReadScope(EsStream& stream, const EsReflectedClassIntf::Ptr& obj);
    ~FieldsReadScope();
    inline bool isOk() const ES_NOTHROW { return m_readWasStarted; }

  protected:
    EsStream& m_stream;
    const EsReflectedClassIntf::Ptr& m_obj;
    bool m_readWasStarted;

  private:
    FieldsReadScope();
    FieldsReadScope(const FieldsReadScope&);
    FieldsReadScope& operator=(const FieldsReadScope&);
  };
  friend class FieldsReadScope;

  class PropertiesReadScope
  {
  public:
    PropertiesReadScope(EsStream& stream, const EsReflectedClassIntf::Ptr& obj);
    ~PropertiesReadScope();
    inline bool isOk() const ES_NOTHROW { return m_readWasStarted; }

  protected:
    EsStream& m_stream;
    const EsReflectedClassIntf::Ptr& m_obj;
    bool m_readWasStarted;

  private:
    PropertiesReadScope();
    PropertiesReadScope(const PropertiesReadScope&);
    PropertiesReadScope& operator=(const PropertiesReadScope&);
  };
  friend class PropertiesReadScope;

  class ItemsReadScope
  {
  public:
    ItemsReadScope(EsStream& stream, EsVariant& var);
    ~ItemsReadScope();
    inline bool isOk() const ES_NOTHROW { return m_readWasStarted; }
    inline bool doAppend() const ES_NOTHROW { return m_doAppend; }
    inline ulong countGet() const ES_NOTHROW { return m_cnt; }

  protected:
    EsStream& m_stream;
    EsVariant& m_var;
    bool m_readWasStarted;
    bool m_doAppend;
    ulong m_cnt;

  private:
    ItemsReadScope();
    ItemsReadScope(const ItemsReadScope&);
    ItemsReadScope& operator=(const ItemsReadScope&);
  };
  friend class ItemsReadScope;

  class ObjectWriteScope
  {
  public:
    ObjectWriteScope(EsStream& stream, const EsReflectedClassIntf::Ptr& obj);
    ~ObjectWriteScope();

  protected:
    EsStream& m_stream;
    ObjectWriteScope* m_oldScope;
    const EsReflectedClassIntf::Ptr& m_obj;
    bool m_customWriting;

  private:
    ObjectWriteScope();
    ObjectWriteScope(const ObjectWriteScope&);
    ObjectWriteScope& operator=(const ObjectWriteScope&);
  };
  friend class ObjectWriteScope;

  class FieldsWriteScope
  {
  public:
    FieldsWriteScope(EsStream& stream, const EsReflectedClassIntf::Ptr& obj);
    ~FieldsWriteScope();

  protected:
    EsStream& m_stream;
    const EsReflectedClassIntf::Ptr& m_obj;

  private:
    FieldsWriteScope();
    FieldsWriteScope(const FieldsWriteScope&);
    FieldsWriteScope& operator=(const FieldsWriteScope&);
  };
  friend class FieldsWriteScope;

  class PropertiesWriteScope
  {
  public:
    PropertiesWriteScope(EsStream& stream, const EsReflectedClassIntf::Ptr& obj);
    ~PropertiesWriteScope();

  protected:
    EsStream& m_stream;
    const EsReflectedClassIntf::Ptr& m_obj;

  private:
    PropertiesWriteScope();
    PropertiesWriteScope(const PropertiesWriteScope&);
    PropertiesWriteScope& operator=(const PropertiesWriteScope&);
  };
  friend class PropertiesWriteScope;

  class ItemsWriteScope
  {
  public:
    ItemsWriteScope(EsStream& stream, const EsVariant& var);
    ~ItemsWriteScope();
    inline bool isOk() const ES_NOTHROW { return m_writeWasStarted; }

  protected:
    EsStream& m_stream;
    const EsVariant& m_var;
    bool m_writeWasStarted;

  private:
    ItemsWriteScope();
    ItemsWriteScope(const ItemsWriteScope&);
    ItemsWriteScope& operator=(const ItemsWriteScope&);
  };
  friend class ItemsWriteScope;

protected:
  EsStream(ulong flags, ulong version, const EsBaseIntfPtr& factory);

public:
  ES_DECL_REFLECTED_CLASS_BASE(EsStream)
  ES_DECL_ATTR_HANDLING_STD

  /// EsStreamIntf implementation && reflection declaration
  ///
  ES_DECL_REFLECTED_INTF_METHOD0(EsBaseIntfPtr, objectCreate) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(void, objectRead, cr_EsBaseIntfPtr) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(void, objectWrite, cr_EsBaseIntfPtr) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isEmpty) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, rewind) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, reset) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(void, reset, ulong) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(bool, contextOpenCreate, cr_EsString) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(bool, contextOpenCreate, cr_EsString, ulong) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(bool, contextOpenExisting, cr_EsString) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, contextCurrentClose) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(void, contextRemove, cr_EsString) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, contextCurrentReset) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(void, contextCurrentReset, ulong) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsString, contextCurrentNameGet) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, contextCurrentIsEmpty) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(ulong, contextCurrentVersionGet) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(void, contextCurrentVersionSet, ulong) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(ulong, rootVersionGet) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(EsVariant, valueRead, cr_EsString, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(void, valueReadObject, cr_EsString, cr_EsBaseIntfPtr) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(void, valueWrite, cr_EsString, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, valueExists, cr_EsString) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(void, valueRemove, cr_EsString) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(bool, firstRootObjectLocate) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(bool, nextRootObjectLocate) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsString, rootObjectTypeNameGet) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(bool, rootObjectTypeEntryLocate, cr_EsString, bool) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(void, valueWrite, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(EsVariant, valueRead) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, factorySet)(const EsBaseIntfPtr& factory) ES_OVERRIDE { m_factory = factory; }
  ES_DECL_INTF_METHOD(void, fromStream)(const EsStreamIntf::Ptr& other) ES_OVERRIDE;

  /// Reflected object component streaming, to augment custom stream writers & readers
  ES_DECL_REFLECTED_METHOD1(void, fieldsReadReflected, cr_EsBaseIntfPtr);
  ES_DECL_REFLECTED_METHOD1(void, propertiesReadReflected, cr_EsBaseIntfPtr);
  ES_DECL_REFLECTED_METHOD1(void, itemsReadReflected, cr_EsBaseIntfPtr);
  ES_DECL_REFLECTED_METHOD1(void, fieldsWriteReflected, cr_EsBaseIntfPtr);
  ES_DECL_REFLECTED_METHOD1(void, propertiesWriteReflected, cr_EsBaseIntfPtr);
  ES_DECL_REFLECTED_METHOD1(void, itemsWriteReflected, cr_EsBaseIntfPtr);
  ES_DECL_REFLECTED_METHOD1(void, fromStreamReflected, cr_EsBaseIntfPtr);
  ES_DECL_REFLECTED_CONST_METHOD0(EsString, asStringReflected);

  /// Reflected properties
  ///
  ES_DECL_PROPERTY_RO(empty, bool)
  ES_DECL_PROPERTY(factory, EsBaseIntfPtr)

  /// Debug-only functionality. Dump Block contents and children to string, in aligned, readable manner
#ifdef ES_DEBUG
  ES_DECL_INTF_METHOD(EsString, debugDump)() const;
#endif

protected:
  /// Stream Model helpers
  void rootInit(ulong version);
  void internalRewind();
  void internalStateReset();
  EsString objectBlockTypeNameGet(EsStreamBlock* bobj) const;

  /// Object creation helper
  EsReflectedClassIntf::Ptr doObjectCreate(const EsString& typeName) const;

  /// Stream implementation-specific internal services
  ///

  /// Custom operations check
  bool doesCustomReading() const { return 0 != (m_flags & flagCustomRead); }
  bool doesCustomWriting() const { return 0 != (m_flags & flagCustomWrite); }

  /// Object component writers
  ///

  /// Object clause
  void objectWriteBegin(const EsReflectedClassIntf::Ptr& obj);
  void objectWriteEnd(const EsReflectedClassIntf::Ptr& obj);

  /// Persistent properties
  void propertiesWriteBegin(const EsReflectedClassIntf::Ptr& obj);
  void propertiesWriteEnd(const EsReflectedClassIntf::Ptr& obj);
  void propertyWrite(const EsString& name, const EsVariant& val, const EsString& label);

  /// Indexed items
  bool itemsWriteBegin(const EsVariant& obj);
  void itemsWriteEnd(const EsVariant& obj);
  void itemWrite(ulong idx, const EsVariant& obj);

  /// Fields
  void fieldsWriteBegin(const EsReflectedClassIntf::Ptr& obj);
  void fieldsWriteEnd(const EsReflectedClassIntf::Ptr& obj);
  void fieldWrite(const EsString& name, const EsReflectedClassIntf::Ptr& fld);

  /// Internal group writing services.
  void fieldsWrite(const EsReflectedClassIntf::Ptr& obj);
  void propertiesWrite(const EsReflectedClassIntf::Ptr& obj);
  void itemsWrite(const EsVariant& var);

  /// Object component readers
  ///

  /// Object creation from current data stream position
  EsReflectedClassIntf::Ptr objectCreateAtCurrentPos();

  /// Object clause
  void objectReadBegin(const EsReflectedClassIntf::Ptr& obj);
  void objectReadEnd(const EsReflectedClassIntf::Ptr& obj);

  /// Properties
  bool propertiesReadBegin(const EsReflectedClassIntf::Ptr& obj);
  void propertiesReadEnd(const EsReflectedClassIntf::Ptr& obj);
  void propertyRead(const EsString& name, const EsReflectedClassIntf::Ptr& obj);

  /// Indexed items
  bool itemsReadBegin(EsVariant& var, ulong& cnt, bool& doAppend);
  void itemsReadEnd(EsVariant& var);
  void itemRead(ulong idx, EsVariant& var, bool doAppend);

  /// Fields
  bool fieldsReadBegin(const EsReflectedClassIntf::Ptr& obj);
  void fieldsReadEnd(const EsReflectedClassIntf::Ptr& obj);
  void fieldRead(const EsString& name, const EsReflectedClassIntf::Ptr& fld);

  /// Internal group reading services.
  void fieldsRead(const EsReflectedClassIntf::Ptr& obj);
  void propertiesRead(const EsReflectedClassIntf::Ptr& obj);
  void itemsRead(EsVariant& var);

  /// Internal checks
  ///
  static void checkContextNameNotEmpty(const EsString& name);
  static void checkContextNameNotReserved(const EsString& name);
  static void typeNameCheck(const EsString& typeName, const EsReflectedClassIntf::Ptr& obj);
  void checkStreamReadable() const;
  void checkStreamWriteable() const;

protected:
  // Object scopes
  ObjectReadScope* m_objReadScope;
  ObjectWriteScope* m_objWriteScope;

  // Stream flags
  mutable ulong m_flags;

  // Local object factory
  EsReflectedClassIntf::Ptr m_factory;

  // Stream Obect Model internals. All stream IO is performed on this model.
  // Derived classes just parse specific storage into model|create storage from model
  //
  mutable EsStreamBlock* m_ctx;
  mutable EsStreamBlock* m_block;
  mutable EsStreamBlock* m_rootObjIt;
  EsStreamBlock::Ptr m_root;

  friend class BlockScope;
};

/// RAII stream context scoping helper classes
///
class ESCORE_CLASS EsStreamContextExistingScope
{
public:
  EsStreamContextExistingScope(const EsStreamIntf::Ptr& stream, const EsString& context);
  ~EsStreamContextExistingScope();

  bool isOk() const { return m_opened; }

  /// Version access
  ulong versionGet() const;
  void versionSet(ulong newVersion);

protected:
  bool m_opened;
  EsStreamIntf::Ptr m_stream;
  EsString m_context;

private:
  EsStreamContextExistingScope();
  EsStreamContextExistingScope(const EsStreamContextExistingScope&);
};

class ESCORE_CLASS EsStreamContextExistingOrNewScope
{
public:
  EsStreamContextExistingOrNewScope(const EsStreamIntf::Ptr& stream, const EsString& context);
  EsStreamContextExistingOrNewScope(const EsStreamIntf::Ptr& stream, const EsString& context, ulong newVersion);
  ~EsStreamContextExistingOrNewScope();

  bool isOk() const { return m_opened; }

  /// Version access
  ulong versionGet() const;
  void versionSet(ulong newVersion);

protected:
  bool m_opened;
  EsStreamIntf::Ptr m_stream;
  EsString m_context;

private:
  EsStreamContextExistingOrNewScope();
  EsStreamContextExistingOrNewScope(const EsStreamContextExistingOrNewScope&);
};

namespace EsReflection
{
/// Stream flags reflected enumeration
///
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCORE_CLASS, EsStreamFlag)
  ES_DECL_ENUMERATION_ITEM_LABELLED(Read,                 static_cast<ulong>(::EsStreamFlag::Read), esT("Stream with read access"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Write,                static_cast<ulong>(::EsStreamFlag::Write), esT("Stream with write access"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(File,                 static_cast<ulong>(::EsStreamFlag::File), esT("File-based stream"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Encrypted,            static_cast<ulong>(::EsStreamFlag::Encrypted), esT("Encrypt stream contents"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Compressed,           static_cast<ulong>(::EsStreamFlag::Compressed), esT("Compress stream contents"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(NonStrictProperties,  static_cast<ulong>(::EsStreamFlag::NonStrictProperties), esT("Do not require all properties to be present in stream"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(NonStrictFields,      static_cast<ulong>(::EsStreamFlag::NonStrictFields), esT("Do not require all fields to be present in stream"))
ES_DECL_ENUMERATION_END
}

#endif // _es_stream_h_
