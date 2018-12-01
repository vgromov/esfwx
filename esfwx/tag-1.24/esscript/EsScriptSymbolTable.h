#ifndef _es_sym_table_h_
#define _es_sym_table_h_

// symbol instances are held as payload VARIANT_COLLECTION in EsStringIndexedMap.
// item 0 is EsScriptValAccessorIntf::Ptr and item 1 is EsScriptSymbolFlag

// abstract symbol container interface
class ESSCRIPT_CLASS EsScriptSymbolTable
{
public:
  typedef std::shared_ptr<EsScriptSymbolTable> Ptr;

public:  
  EsScriptSymbolTable(bool checkTemplate = true) ES_NOTHROW;
  EsScriptSymbolTable(const EsScriptSymbolTable& src, EsScriptObjectIntf* parent);
  ~EsScriptSymbolTable() ES_NOTHROW;

  void clone(const EsScriptSymbolTable& src, EsScriptObjectIntf* parent = nullptr);

  bool isEmpty() const ES_NOTHROW { return m_contents.isEmpty(); }
  size_t sizeGet() const ES_NOTHROW { return m_contents.countGet(); }
  bool isTemplate() const ES_NOTHROW { return m_template; }

  void reset() { m_contents.clear(); }

  void symbolTemplateAdd(const EsString& name, ulong flags, const EsScriptDebugInfoIntf::Ptr& dbg = nullptr);
  EsScriptValAccessorIntf::Ptr symbolNonTemplateAdd(const EsString& name, const EsVariant& val, ulong flags, const EsScriptDebugInfoIntf::Ptr& dbg = nullptr);

  bool symbolExists(const EsString& name) const ES_NOTHROW { return m_contents.itemExists(name); }
  ulong symbolFlagsGet(const EsString& name) const;
  bool isSymbolBuiltIn(const EsString& name) const;

  const EsString& findFirstSymbolWithVal(const EsVariant& val) const ES_NOTHROW;
  EsScriptValAccessorIntf::Ptr symbolFind(const EsString& name, bool& found) const ES_NOTHROW;
  EsScriptValAccessorIntf::Ptr symbolGet(const EsString& name, bool doThrow) const;

  void symbolValSet(const EsString& name, const EsVariant& val);
  void symbolValSetIgnoreReadOnly(const EsString& name, const EsVariant& val);

  EsStringArray allSymbolNamesGet(bool includeBuiltIns = true) const ES_NOTHROW;
  const EsStringIndexedMap& symbolsGet() const ES_NOTHROW { return m_contents; }

  static EsScriptValAccessorIntf::Ptr valAccessorCreate(const EsString& name, const EsVariant& val, ulong flags);

private:
  // internal services
  //
  void checkTemplateOperation() const;
  void checkNonTemplateOperation() const;
  void accessorAdd(const EsScriptValAccessorIntf::Ptr& acc);

  static const EsScriptValAccessorIntf::Ptr& null() ES_NOTHROW;

  // prohibited functionality
  //
  EsScriptSymbolTable(const EsScriptSymbolTable& src) ES_REMOVEDECL;
  const EsScriptSymbolTable& operator =(const EsScriptSymbolTable&) ES_REMOVEDECL;

protected:
  EsStringIndexedMap m_contents;

  // check template|non template mode
  bool m_checkTemplate;
  // template flag
  bool m_template;

  friend class EsScriptMachine;
};

#endif // _es_sym_table_h_
