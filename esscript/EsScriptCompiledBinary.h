#ifndef _es_script_compiled_binary_h_
#define _es_script_compiled_binary_h_

// Custom binary block types
//
enum EsScriptBynaryType {
  binaryTypeNone                = -1,
  binaryTypeLinks                = 1 << 8,
  binaryTypeConst                = 2 << 8,
  binaryTypeConsts              = 3 << 8,
  binaryTypeEnumItem            = 4 << 8,
  binaryTypeEnum                = 5 << 8,
  binaryTypeEnums                = 6 << 8,
  binaryTypeAttributes          = 7 << 8,
  binaryTypeVariables            = 8 << 8,
  binaryTypeParameters          = 9 << 8,
  binaryTypeTryCatchBlock        = 10 << 8,
  binaryTypeTryCatchBlocks      = 11 << 8,
  binaryTypeDependencies        = 12 << 8,
  binaryTypeCodeSections        = 13 << 8,
  binaryTypeField                = 14 << 8,
  binaryTypeFields              = 15 << 8,
  binaryTypeMetaclass            = 16 << 8,
  binaryTypeMetaclasses          = 17 << 8,
  binaryTypeDebugInfo            = 18 << 8,
  binaryTypeInstruction          = 19 << 8,
  binaryTypeInstructions        = 20 << 8,
  binaryTypeCodeSection          = 21 << 8,
  binaryTypeFieldExpression      = 22 << 8,
  binaryTypeGlobalMethods        = 23 << 8,
  binaryTypeProperties          = 24 << 8,
  binaryTypeProperty            = 25 << 8,
  binaryTypeHeader              = 26 << 8,
  binaryTypeExterns              = 27 << 8,
  binaryTypeExtern              = 28 << 8,
};
//---------------------------------------------------------------------------

enum EsScriptCompiledBinary
{
  typeFieldSize                 = 2
};
//---------------------------------------------------------------------------

// ES compiled script binary writer
//
class EsScriptCompiledBinaryWriter
{
protected:
  // internal types and consts
  enum {
    reservationChunkSize = 131072,
  };

  // Block writer helper class
  class EsScriptCompiledBinaryBlockWriter
  {
  protected:
    EsScriptCompiledBinaryBlockWriter(EsScriptCompiledBinaryWriter& owner, long type);

  public:
    ~EsScriptCompiledBinaryBlockWriter();

  protected:
    EsScriptCompiledBinaryWriter& m_owner;
    EsScriptCompiledBinaryBlockWriter* m_prevBlock;
    // block starting position, block begins just after type id record
    ulong m_start;
    ulong m_size;

    ES_NO_DEFAULT_CTR(EsScriptCompiledBinaryBlockWriter);
    ES_NON_COPYABLE(EsScriptCompiledBinaryBlockWriter);

    friend class EsScriptCompiledBinaryWriter;
  };
  friend class EsScriptCompiledBinaryBlockWriter;

public:
  EsScriptCompiledBinaryWriter(const EsScriptMachine& machine, bool retainDebugInfo, EsBinBuffer& buff);

protected:
  // internal services
  void checkReserveEnough(ulong requiredSpace);
  void typeWrite(long type);
  void stringWrite(const EsString& str);
  void stringArrayWrite(const EsStringArray& a, long customType = binaryTypeNone);
  void variantCollectionWrite(const EsVariant& c);
  void objectWrite(const EsVariant& o);
  void variantWrite(const EsVariant& var);
  void attributesWrite(const EsAttributesIntf::Ptr& attrs);
  void binaryLinksWrite();
  void externWrite(const EsString& name);
  void externsWrite();
  void constantWrite(const EsString& name, const EsVariant& val);
  void constantsWrite();
  void debugInfoWrite(const EsScriptDebugInfoIntf::Ptr& debugInfo);
  void instructionWrite(const EsScriptInstruction& instruction);
  void instructionsWrite(const EsScriptInstructions& instructions);
  void tryCatchBlockWrite(const EsScriptTryCatchBlock& tryCatch);
  void tryCatchBlocksWrite(const EsScriptTryCatchBlocks& tryCatchBlocks);
  void codeSectionWrite(const EsScriptCodeSection::Ptr& codeSection, long codeSectionTag = binaryTypeCodeSection);
  void branchFieldWrite(const EsVariant& ifBranch);
  void fieldWrite(const EsString& name, const EsVariant& fnode);
  void fieldsWrite(const EsScriptObjectIntf::Ptr& metaclass);
  void codeSectionsWrite(const EsScriptMethodMapPtr& codeSections, long codeSectionsTag = binaryTypeCodeSections );
  void propertyWrite(const EsScriptObjectPropertyInfoIntf::Ptr& prop);
  void propertiesWrite(const EsScriptObjectIntf::Ptr& metaclass);
  void metaclassWrite(const EsScriptObjectIntf::Ptr& metaclass);
  void metaclassesWrite();
  void globalMethodsWrite();
  void enumItemWrite(const EsString& symbol, const EsVariant& val, const EsString& label);
  void enumAttributesWrite(const EsEnumerationIntf::Ptr& enu);
  void enumWrite(const EsEnumerationIntf::Ptr& enu);
  void enumsWrite();
  void headerWrite();

protected:
  // data members
  const EsScriptMachine& m_machine;
  EsScriptCompiledBinaryBlockWriter* m_activeBlock;
  bool m_debugInfoRetain;
  EsBinBuffer& m_buff;

  ES_NO_DEFAULT_CTR(EsScriptCompiledBinaryWriter);
  ES_NON_COPYABLE(EsScriptCompiledBinaryWriter);
};
//---------------------------------------------------------------------------

// ES compiled script binary reader
//
class EsScriptCompiledBinaryReader
{
protected:
  // Object scope reader helper class
  class EsScriptObjectReadScope
  {
  public:
    EsScriptObjectReadScope(EsScriptCompiledBinaryReader& reader, const EsScriptObjectIntf::Ptr& obj);
    ~EsScriptObjectReadScope();

    inline EsScriptObjectIntf::Ptr objectGet() const { return m_object; }

  protected:
    EsScriptCompiledBinaryReader& m_reader;
    EsScriptObjectReadScope* m_prevScope;
    EsScriptObjectIntf::Ptr m_object;

    ES_NO_DEFAULT_CTR(EsScriptObjectReadScope);
    ES_NON_COPYABLE(EsScriptObjectReadScope);

    friend class EsScriptCompiledBinaryReader;
  };
  friend class EsScriptObjectReadScope;

  // Code section scope reader helper class
  class EsScriptCodeSectionReadScope
  {
  public:
    EsScriptCodeSectionReadScope(EsScriptCompiledBinaryReader& reader, const EsScriptCodeSection::Ptr& cs);
    ~EsScriptCodeSectionReadScope();

    inline EsScriptCodeSection::Ptr sectionGet() const { return m_cs; }

  protected:
    EsScriptCompiledBinaryReader& m_reader;
    EsScriptCodeSectionReadScope* m_prevScope;
    EsScriptCodeSection::Ptr m_cs;

    ES_NO_DEFAULT_CTR(EsScriptCodeSectionReadScope);
    ES_NON_COPYABLE(EsScriptCodeSectionReadScope);

    friend class EsScriptCompiledBinaryReader;
  };
  friend class EsScriptCodeSectionReadScope;

public:
  EsScriptCompiledBinaryReader(EsScriptMachine& machine, bool retainDebugInfo, const EsBinBuffer& buff);
  const EsString& compilerVersionGet() const ES_NOTHROW { return m_compilerVersion; }

protected:
  // internal services
  void checkBufferHasEnoughSpace(ulong chunkSize) const;
  void chunkRead(void* dest, ulong chunkSize);
  long typeRead(long expectedType = binaryTypeNone);
  EsString internalStringRead();
  EsString stringRead();
  EsStringArray internalStringArrayRead();
  EsStringArray stringArrayRead(long customType = binaryTypeNone);
  EsVariant internalVariantCollectionRead();
  EsVariant variantCollectionRead();
  EsVariant objectRead();
  EsVariant variantRead();
  EsAttributesIntf::Ptr attributesRead();
  void binaryLinksRead();
  void externRead();
  void externsRead();
  void constantRead();
  void constantsRead();
  void variablesRead();
  EsScriptDebugInfoIntf::Ptr debugInfoRead();
  void instructionRead(const EsScriptCodeSection::Ptr& cs);
  void instructionsRead();
  void tryCatchBlockRead(const EsScriptCodeSection::Ptr& cs);
  void tryCatchBlocksRead();
  void dependenciesRead(const EsScriptCodeSection::Ptr& cs);
  void codeSectionRead(long codeSectionTag = binaryTypeCodeSection);
  void attributesAssignToObj(const EsAttributesIntf::Ptr& attrs, const EsScriptObjectIntf::Ptr& obj);
  void branchFieldRead(const EsScriptObjectIntf::Ptr& ifBranch);
  void fieldRead();
  void fieldsRead();
  void codeSectionAttributesAssign(const EsAttributesIntf::Ptr& attrs, const EsScriptCodeSection::Ptr& cs);
  void codeSectionsRead(long tag = binaryTypeCodeSections);
  void memberVariablesRead();
  void propertyRead();
  void propertiesRead();
  void metaclassRead();
  void metaclassesRead();
  void globalMethodsRead();
  void enumItemRead(const EsEnumerationIntf::Ptr& enu);
  void enumAttributesRead(const EsEnumerationIntf::Ptr& enu);
  void enumRead();
  void enumsRead();
  void headerRead();

protected:
  // data members
  EsScriptMachine& m_machine;
  EsScriptObjectReadScope* m_objectScope;
  EsScriptCodeSectionReadScope* m_codeScope;

  const EsBinBuffer& m_buff;
  bool m_debugInfoRetain;
  ulong m_pos;
  ulong m_size;
  EsString m_compilerVersion;

  ES_NO_DEFAULT_CTR(EsScriptCompiledBinaryReader);
  ES_NON_COPYABLE(EsScriptCompiledBinaryReader);
};

#ifdef ESSCRIPT_COMPILED_USE_TRACE
#  define ESSCRIPT_COMPILED_TRACE1(a)                ES_DEBUG_TRACE(a);
#  define ESSCRIPT_COMPILED_TRACE2(a, b)              ES_DEBUG_TRACE((a), (b));
#  define ESSCRIPT_COMPILED_TRACE3(a, b, c)          ES_DEBUG_TRACE((a), (b), (c));
#  define ESSCRIPT_COMPILED_TRACE4(a, b, c, d)        ES_DEBUG_TRACE((a), (b), (c), (d));
#  define ESSCRIPT_COMPILED_TRACE5(a, b, c, d, e)    ES_DEBUG_TRACE((a), (b), (c), (d), (e));
#  define ESSCRIPT_COMPILED_TRACE6(a, b, c, d, e, f)  ES_DEBUG_TRACE((a), (b), (c), (d), (e), (f));
#else
#  define ESSCRIPT_COMPILED_TRACE1(a)
#  define ESSCRIPT_COMPILED_TRACE2(a, b)
#  define ESSCRIPT_COMPILED_TRACE3(a, b, c)
#  define ESSCRIPT_COMPILED_TRACE4(a, b, c, d)
#  define ESSCRIPT_COMPILED_TRACE5(a, b, c, d, e)
#  define ESSCRIPT_COMPILED_TRACE6(a, b, c, d, e, f)
#endif

#endif // _es_script_compiled_binary_h_
