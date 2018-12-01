#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptCompiledBinary.h"
#include "EsStr.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_COMPILE_TIME_ASSERT(sizeof(long) >= EsScriptCompiledBinary::typeFieldSize, _EsScriptCompiledBinary_typeFieldSize);
ES_COMPILE_TIME_ASSERT(sizeof(ulong) >= sizeof(esU32), _EsScriptCompiledBinary_ulong_less_esU32);
ES_COMPILE_TIME_ASSERT(sizeof(ulong) >= sizeof(esI32), _EsScriptCompiledBinary_ulong_less_esI32);
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#define ES_MODERN_VER_START esT("1.23")
//--------------------------------------------------------------------------- 

// Binary block writer helper implementation
//
EsScriptCompiledBinaryWriter::
EsScriptCompiledBinaryBlockWriter::EsScriptCompiledBinaryBlockWriter(EsScriptCompiledBinaryWriter& owner, long type) :
m_owner(owner),
m_prevBlock(owner.m_activeBlock),
m_size(0)
{
  m_owner.m_activeBlock = this;
  m_owner.typeWrite(type);
  // append block size placeholder
  m_owner.m_buff.append(sizeof(esU32), reinterpret_cast<const esU8*>(&m_size));
  // remember block starting position
  m_start = static_cast<ulong>(m_owner.m_buff.size());
}
//---------------------------------------------------------------------------

EsScriptCompiledBinaryWriter::
EsScriptCompiledBinaryBlockWriter::~EsScriptCompiledBinaryBlockWriter()
{
  // finalize block, write real size value into placeholder
  m_size = static_cast<ulong>(m_owner.m_buff.size()) - m_start;
  memcpy(&m_owner.m_buff[m_start-sizeof(esU32)], &m_size, sizeof(esU32));
  m_owner.m_activeBlock = m_prevBlock;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Binary block writer implementation
//

EsScriptCompiledBinaryWriter::EsScriptCompiledBinaryWriter(const EsScriptMachine& machine, bool debugInfoRetain,
                                                           EsBinBuffer& buff) :
m_machine(machine),
m_activeBlock(nullptr),
m_debugInfoRetain(debugInfoRetain),
m_buff(buff)
{
  m_buff.clear();
  checkReserveEnough(reservationChunkSize);
  headerWrite();
  binaryLinksWrite();
  constantsWrite();
  enumsWrite();
  globalMethodsWrite();
  metaclassesWrite();
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::checkReserveEnough(ulong requiredSpace)
{
  size_t cap = m_buff.capacity();
  if( static_cast<ulong>(cap-m_buff.size()) < requiredSpace)
  {
    ulong requiredSpaceAligned = reservationChunkSize * (requiredSpace / reservationChunkSize) +
      ((requiredSpace % reservationChunkSize) ? reservationChunkSize : 0);

    m_buff.reserve(cap + requiredSpaceAligned);
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::typeWrite(long type)
{
  ES_ASSERT(  (0 <= type && EsVariant::VAR_VARIANT_COLLECTION >= type) ||
            (binaryTypeLinks <= type && binaryTypeHeader >= type) );

  // reserve EsScriptCompiledBinary::typeFieldSize bytes for the value type. variant and ordinary values got tagged by common variant types
  // special types - metaclasses, attributes, fields - have their own types
  //
  m_buff.append(
    EsScriptCompiledBinary::typeFieldSize,
    reinterpret_cast<const esU8*>(&type)
  );
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::stringWrite(const EsString& str)
{
  // write string block
  EsScriptCompiledBinaryBlockWriter block(*this, EsVariant::VAR_STRING);

  // write string length. NB! in chars, not bytes
  const EsBinBuffer& ustr = EsStr::toByteString(str, EsString::UTF8);

  checkReserveEnough(
    static_cast<ulong>(ustr.size()+sizeof(esU32))
  );
  esU32 tmp = static_cast<esU32>(ustr.size());

  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));
  m_buff.append(ustr);
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::stringArrayWrite(const EsStringArray& a, long customType /*= binaryTypeNone*/)
{
  EsScriptCompiledBinaryBlockWriter block(*this, (binaryTypeNone == customType) ? EsVariant::VAR_STRING_COLLECTION : customType);

  esU32 tmp = static_cast<esU32>(a.size());
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  for(size_t idx = 0; idx < tmp; ++idx)
    stringWrite( a[idx] );
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::variantCollectionWrite(const EsVariant& c)
{
  EsScriptCompiledBinaryBlockWriter block(*this, EsVariant::VAR_VARIANT_COLLECTION);

  esU32 tmp = c.countGet();
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  for(ulong idx = 0; idx < tmp; ++idx)
    variantWrite( c.itemGet(idx) );
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::attributesWrite(const EsAttributesIntf::Ptr& attrs)
{
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeAttributes);
  esU32 tmp = 0;
  EsStringArray a;
  if(attrs)
    a = attrs->allNamesGet();

  tmp = static_cast<esU32>(a.size());
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));
  if( !a.empty() )
  {
    esU8 interlocked = attrs->isInterlocked() ? 1 : 0;
    m_buff.append(sizeof(interlocked), &interlocked);

    stringWrite( attrs->ownerNameGet() );
    for(size_t idx = 0; idx < tmp; ++idx)
    {
      const EsString& att = a[idx];
      stringWrite(att);
      variantWrite(attrs->attributeGet(att));
    }
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::objectWrite(const EsVariant& o)
{
  // only smallest subset (i.e. range) of objects may be used as constants and, hence,
  // saved to compiled binary file. so comprehensive object writer
  // shouldn't be implemented here
  EsReflectedClassIntf::Ptr obj = o.asExistingObject();
  EsScriptCompiledBinaryBlockWriter block(*this, o.typeGet());
  stringWrite( obj->typeNameGet() );

  if( obj->is(EsRange::classNameGetStatic()) )
  {
    variantWrite( obj->propertyGet("min") );
    variantWrite( obj->propertyGet("isStrictMin") );
    variantWrite( obj->propertyGet("max") );
    variantWrite( obj->propertyGet("isStrictMax") );
  }
  else if(obj->is(EsAssocContainer::classNameGetStatic()))
  {
    EsAssocContainerIntf::Ptr ac = obj;
    ES_ASSERT(ac);

    ulong cnt = ac->countGet();
    variantWrite( cnt );
    for(ulong idx = 0; idx < cnt; ++idx)
    {
      const EsVariant& item = ac->itemGet(idx);
      variantWrite(item);
    }
  }
  else if( !obj->isKindOf(EsEnumeration::classNameGetStatic()) )
  {
    // we should never be here!
    ES_DEBUG_TRACE(
      esT("Wrinting constant objects of type '%s' is not supported!"),
      obj->typeNameGet()
    );
    ES_FAIL;
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::variantWrite(const EsVariant& var)
{
  int type = var.typeGet();
  if( EsVariant::VAR_STRING != type && !var.isCollection() && !var.isObject() )
    typeWrite(type);

  switch( type )
  {
  case EsVariant::VAR_EMPTY:
    break;
  case EsVariant::VAR_BOOL:
  case EsVariant::VAR_BYTE:
  case EsVariant::VAR_CHAR:
  case EsVariant::VAR_UINT:
    {
      esU32 tmp = var.asULong(m_machine.loc());
      m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));
    }
    break;
  case EsVariant::VAR_INT:
    {
      esI32 tmp = var.asLong(m_machine.loc());
      m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));
    }
    break;
  case EsVariant::VAR_UINT64:
    {
      esU64 tmp = var.asULLong(m_machine.loc());
      m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));
    }
    break;
  case EsVariant::VAR_INT64:
    {
      esI64 tmp = var.asLLong(m_machine.loc());
      m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));
    }
    break;
  case EsVariant::VAR_DOUBLE:
    {
      double tmp = var.asDouble(m_machine.loc());
      m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));
    }
    break;
  case EsVariant::VAR_BIN_BUFFER:
    {
      EsBinBuffer buff = var.asBinBuffer();
      esU32 tmp = static_cast<esU32>(buff.size());
      m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));
      m_buff.append(buff);
    }
    break;
  case EsVariant::VAR_STRING:
    stringWrite(var.asString(m_machine.loc()));
    break;
  case EsVariant::VAR_STRING_COLLECTION:
    stringArrayWrite(var.asStringCollection(m_machine.loc()));
    break;
  case EsVariant::VAR_VARIANT_COLLECTION:
    variantCollectionWrite(var);
    break;
  case EsVariant::VAR_OBJECT:
    objectWrite(var);
    break;
  default:
    ES_FAIL;
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::binaryLinksWrite()
{
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeLinks);

  esU32 tmp = static_cast<esU32>(m_machine.m_links.size());
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  for( EsScriptBinaryLinks::const_iterator cit = m_machine.m_links.begin(); cit != m_machine.m_links.end(); ++cit )
    stringWrite( (*cit).first );
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::externWrite(const EsString& name)
{
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeExtern);

  esU32 flags = m_machine.m_externs.symbolFlagsGet(name);
  // store extern flags
  m_buff.append(sizeof(flags), reinterpret_cast<const esU8*>(&flags));

  // store extern name
  stringWrite( name );
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::externsWrite()
{
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeExterns);

  const EsStringArray& syms = m_machine.m_externs.allSymbolNamesGet(false);

  esU32 tmp = static_cast<esU32>(syms.size());
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  for( size_t idx = 0; idx < syms.size(); ++idx )
  {
    const EsString& name = syms[idx];
    externWrite(name);
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::constantWrite(const EsString& name, const EsVariant& val)
{
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeConst);
  // store constant name
  stringWrite( name );
  // store constant value
  variantWrite( val );
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::constantsWrite()
{
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeConsts);
  // do not store built-in consts
  const EsStringArray& syms = m_machine.m_consts.allSymbolNamesGet(false);

  esU32 tmp = static_cast<esU32>(syms.size());
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  for( size_t idx = 0; idx < syms.size(); ++idx )
  {
    const EsString& name = syms[idx];
    constantWrite(
      name,
      m_machine.m_consts.symbolGet(
        name,
        true
      )->get()
    );
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::debugInfoWrite(const EsScriptDebugInfoIntf::Ptr& debugInfo)
{
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeDebugInfo);
  if(m_debugInfoRetain && debugInfo)
  {
    EsVariant v = debugInfo->lineGet();
    variantWrite(v);
    v = debugInfo->columnGet();
    variantWrite(v);
    stringWrite( debugInfo->fileGet() );
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::instructionWrite(const EsScriptInstruction& instruction)
{
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeInstruction);

  esU32 tmp = instruction.opcode();
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  ulong mask = instruction.payloadMask();
  tmp = mask;
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  if( !instruction.isEmptyPayload() )
  {
    if( mask & EsScriptInstruction::Payload0 )
    {
      tmp = instruction.raw0get();
      m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));
    }

    if( mask & EsScriptInstruction::Payload1 )
    {
      tmp = instruction.raw1get();
      m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));
    }

    if( mask & EsScriptInstruction::Payload2 )
    {
      tmp = instruction.raw2get();
      m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));
    }

    if( mask & EsScriptInstruction::Payload3 )
    {
      stringArrayWrite(
        instruction.qnameGet()
      );
    }
  }

  debugInfoWrite(instruction.debugInfoGet());
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::instructionsWrite(const EsScriptInstructions& instructions)
{
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeInstructions);

  esU32 tmp = static_cast<esU32>(instructions.size());
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  for( EsScriptInstructions::const_iterator cit = instructions.begin();  cit != instructions.end(); ++cit )
    instructionWrite( (*cit) );
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::tryCatchBlockWrite(const EsScriptTryCatchBlock& tryCatch)
{
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeTryCatchBlock);

  esU32 tmp = tryCatch.tryStart();
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  tmp = tryCatch.tryEnd();
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  tmp = tryCatch.catchStart();
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  tmp = tryCatch.catchEnd();
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::tryCatchBlocksWrite(const EsScriptTryCatchBlocks& tryCatchBlocks)
{
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeTryCatchBlocks);

  esU32 tmp = static_cast<esU32>(tryCatchBlocks.size());
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  for( EsScriptTryCatchBlocks::const_iterator cit = tryCatchBlocks.begin();  cit != tryCatchBlocks.end(); ++cit )
    tryCatchBlockWrite( (*cit) );
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::codeSectionWrite(const EsScriptCodeSection::Ptr& codeSection, long codeSectionTag/* = binaryTypeCodeSection*/)
{
  EsScriptCompiledBinaryBlockWriter block(*this, codeSectionTag);
  if( codeSection )
  {
    ES_ASSERT( codeSection->isTemplate() );

#if defined(ESSCRIPT_CODE_SECTION_TEXT_DUMP)
    const EsString& csContents = codeSection->asString();
    const EsString& csName = codeSection->nameGet();
    const EsString& dumpPath = EsPath::stdDocsGet() + esT("/csdump_") + csName + esT(".txt");
    EsFile csDump(
      dumpPath,
      static_cast<ulong>(EsFileFlag::Write)|
      static_cast<ulong>(EsFileFlag::Text)
    );

    if( csDump.open() )
      csDump.writeAllAsString(
        csContents
      );
#endif

    if( binaryTypeCodeSection == codeSectionTag )
    {
      // write name
      stringWrite(codeSection->nameGet());
      // write code section attributes
      attributesWrite(codeSection->attributesAccess());
      // write parameters and variables
      EsStringArray p = codeSection->paramNamesAccess();
      stringArrayWrite(p, binaryTypeParameters);

      // prepare parameters and variables collections for subtraction
      EsStringArray v = codeSection->variablesAccess().allSymbolNamesGet(false);
      std::sort(v.begin(), v.end());
      std::sort(p.begin(), p.end());
      EsStringArray trueVars(v.size());

      // exclude parameter names from variables
      EsStringArray::iterator end = std::set_difference(
        v.begin(),
        v.end(),
        p.begin(),
        p.end(),
        trueVars.begin()
      );
      trueVars.erase(
        end,
        trueVars.end()
      );

      stringArrayWrite(trueVars, binaryTypeVariables);
    }
    // write dependencies
    stringArrayWrite(codeSection->thisFieldDependenciesGet(), binaryTypeDependencies);
    stringArrayWrite(codeSection->thisMemberVarDependenciesGet(), binaryTypeDependencies);
    // write instructions
    instructionsWrite(codeSection->codeAccess());
    // write try-catch blocks
    tryCatchBlocksWrite(codeSection->tryCatchBlocksAccess());
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::codeSectionsWrite(const EsScriptMethodMapPtr& methods, long codeSectionTag /*= binaryTypeCodeSections*/)
{
  EsScriptCompiledBinaryBlockWriter block(*this, codeSectionTag);
  esU32 cnt = 0;
  if( methods )
    cnt = static_cast<esU32>(methods->size());
  m_buff.append(sizeof(cnt), reinterpret_cast<const esU8*>(&cnt));

  if( methods )
  {
    for( EsScriptMethodMap::const_iterator cit = methods->begin(); cit != methods->end(); ++cit )
      codeSectionWrite((*cit).second);
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::branchFieldWrite(const EsVariant& ifBranch)
{
  ES_ASSERT(!ifBranch.isEmpty());
  EsScriptObjectIntf::Ptr fld = EsScriptObject::fnodeFieldGet(ifBranch);
  ES_ASSERT(fld);
  ES_ASSERT(fld->flagsGet() & EsScriptObject::ofIfBranch);

  const EsStringArray& vars = fld->thisVariablesNamesGet();
  stringArrayWrite(vars, binaryTypeVariables);
  fieldsWrite(fld);
  codeSectionsWrite(fld->thisScriptedMethodsGet());
  propertiesWrite(fld);
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::fieldWrite(const EsString& name, const EsVariant& fnode )
{
  ES_ASSERT(!fnode.isEmpty());
  EsScriptObjectIntf::Ptr fld = EsScriptObject::fnodeFieldGet(fnode);
  EsAttributesIntf::Ptr attrs = EsScriptObject::fnodeAttrsGet(fnode);
  ES_ASSERT(fld);

  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeField);
  // write field object flags and name
  esU32 flags = fld->flagsGet();
  variantWrite(flags);
  // write field name
  stringWrite(name);
  // write field type (item type, in case of arrays)
  if( fld->isArray() )
    stringWrite(fld->itemTypeNameGet());
  else
    stringWrite(fld->typeNameGet());
  // write field attributes
  attributesWrite( attrs );

  // write field expression
  if( flags & (EsScriptObject::ofIf|EsScriptObject::ofArray) )
  {
    codeSectionWrite(fld->thisFieldExprGet(), binaryTypeFieldExpression);

    // write nested fields, if conditional branch is used
    if( flags & EsScriptObject::ofIf )
    {
      branchFieldWrite( fld->thisFieldGet(0) );
      branchFieldWrite( fld->thisFieldGet(1) );
    }
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::fieldsWrite(const EsScriptObjectIntf::Ptr& metaclass)
{
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeFields);
  const EsStringIndexedMap& m = metaclass->thisFieldsMapGet();

  esU32 tmp = static_cast<esU32>(m.countGet());
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  for(ulong idx = 0; idx < tmp; ++idx)
  {
    const EsVariant& fnode = m.valueGet(idx);
    const EsString& name = m.nameGet(idx);
    fieldWrite(name, fnode);
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::propertyWrite(const EsScriptObjectPropertyInfoIntf::Ptr& prop)
{
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeProperty);
  stringWrite(prop->nameGet());
  stringWrite(prop->readerNameGet(false));
  stringWrite(prop->writerNameGet(false));
  attributesWrite(prop->internalAttrsAccess());
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::propertiesWrite(const EsScriptObjectIntf::Ptr& metaclass)
{
  EsStringIndexedMap::Ptr props = metaclass->thisPropertiesMapGet();
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeProperties);

  esU32 tmp = props ? props->countGet() : 0;
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  if(props)
  {
    for(ulong idx = 0; idx < tmp; ++idx)
    {
      EsScriptObjectPropertyInfoIntf::Ptr prop = props->valueGet(idx).asExistingObject();
      ES_ASSERT(prop);
      propertyWrite(prop);
    }
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::metaclassWrite(const EsScriptObjectIntf::Ptr& metaclass)
{
  ES_ASSERT(metaclass);
  ES_ASSERT(!metaclass->isPOD());
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeMetaclass);
  // write metaclass flags
  variantWrite(metaclass->flagsGet());
  // write metaclass typeName
  EsString name = metaclass->typeNameGet();
  stringWrite(name);
  // write ancestor type name
  name.clear();
  EsScriptObjectIntf::Ptr ancestor = metaclass->ancestorGet();
  if( ancestor )
    name = ancestor->typeNameGet();
  stringWrite(name);
  // write class|instance attributes
  attributesWrite( metaclass->classAttrsAccess() );
  // write metaclass member vars
  const EsStringArray& vars = metaclass->thisVariablesNamesGet();
  stringArrayWrite(vars, binaryTypeVariables);
  // write metaclass fields
  fieldsWrite( metaclass );
  // write metaclass methods
  EsScriptMethodMapPtr methods = metaclass->thisScriptedMethodsGet();
  codeSectionsWrite( methods );
  // write metaclass properties
  propertiesWrite( metaclass );
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::metaclassesWrite()
{
  // enumerate all registered script metaclasses, skipping POD ones,
  EsScriptObject::ObjectsVector metaclasses;
  metaclasses.reserve( m_machine.m_metaclassesMap.countGet() );
  for( ulong idx = 0; idx < m_machine.m_metaclassesMap.countGet(); ++idx )
  {
    EsScriptObjectIntf::Ptr metaclass = m_machine.m_metaclassesMap.valueGet(idx).asExistingObject();
    if( metaclass->isPOD() )
      continue;

    metaclasses.push_back(metaclass);
  }

  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeMetaclasses);

  esU32 tmp = static_cast<esU32>(metaclasses.size());
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  for(size_t idx = 0; idx < metaclasses.size(); ++idx)
  {
    EsScriptObjectIntf::Ptr metaclass = metaclasses[idx];
    ESSCRIPT_COMPILED_TRACE3(esT("Metaclass '%s' written, rc=%d"), metaclass->typeNameGet(), metaclass->refGet())
    metaclassWrite(metaclass);
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::globalMethodsWrite()
{
  codeSectionsWrite(m_machine.m_globalMethods, binaryTypeGlobalMethods);
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::enumItemWrite(const EsString& symbol, const EsVariant& val, const EsString& label)
{
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeEnumItem);
  stringWrite(symbol);
  variantWrite(val);
  stringWrite(label);
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::enumAttributesWrite(const EsEnumerationIntf::Ptr& enu)
{
  // use custom procedure for enum attributes writing, because enum
  // is singleton and we do not need the Attribute Ptr access
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeAttributes);
  ES_ASSERT(enu);
  EsReflectedClassIntf::Ptr r = enu;
  ES_ASSERT(r);
  const EsStringArray& a = r->attributeNamesGet();

  esU32 tmp = static_cast<esU32>(a.size());
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  for(size_t idx = 0; idx < tmp; ++idx)
  {
    const EsString& att = a[idx];
    stringWrite(att);
    variantWrite(r->attributeGet(att));
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::enumWrite(const EsEnumerationIntf::Ptr& enu)
{
  ES_ASSERT(enu);
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeEnum);

  const EsStringArray& syms = enu->symbolNamesGet();
  const EsVariant& vals = enu->valuesGet();
  const EsStringArray& labels = enu->labelsGet();

  ES_ASSERT(syms.size() == static_cast<size_t>(vals.countGet()));
  ES_ASSERT(syms.size() == static_cast<size_t>(labels.size()));

  stringWrite(enu->typeNameGet());
  enumAttributesWrite(enu);

  esU32 tmp = static_cast<esU32>(syms.size());
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  for(ulong idx = 0; idx < tmp; ++ idx)
    enumItemWrite(
      syms[idx],
      vals.itemGet(idx),
      labels[idx]
    );
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::enumsWrite()
{
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeEnums);

  esU32 tmp = m_machine.m_enumsMap.countGet();
  m_buff.append(sizeof(tmp), reinterpret_cast<const esU8*>(&tmp));

  for( ulong idx = 0; idx < tmp; ++idx )
    enumWrite(
      m_machine.m_enumsMap.valueGet(idx).asExistingObject()
    );
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryWriter::headerWrite()
{
  EsScriptCompiledBinaryBlockWriter block(*this, binaryTypeHeader);
  // Write scripting host version string
  stringWrite(m_machine.versionGet());
  // Write source modules information
  variantWrite(m_machine.m_filesInfo);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// Compiled binary reader helpers implementation
//
// Object reader scope
EsScriptCompiledBinaryReader::
EsScriptObjectReadScope::EsScriptObjectReadScope(
  EsScriptCompiledBinaryReader& reader,
  const EsScriptObjectIntf::Ptr& obj
) :
m_reader(reader),
m_prevScope(reader.m_objectScope),
m_object(obj)
{
  m_reader.m_objectScope = this;
}
//---------------------------------------------------------------------------

EsScriptCompiledBinaryReader::
EsScriptObjectReadScope::~EsScriptObjectReadScope()
{
  m_reader.m_objectScope = m_prevScope;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// Code section reader scope
//
EsScriptCompiledBinaryReader::
EsScriptCodeSectionReadScope::EsScriptCodeSectionReadScope(
  EsScriptCompiledBinaryReader& reader,
  const EsScriptCodeSection::Ptr& cs
) :
m_reader(reader),
m_prevScope(reader.m_codeScope),
m_cs(cs)
{
  m_reader.m_codeScope = this;
}
//---------------------------------------------------------------------------

EsScriptCompiledBinaryReader::
EsScriptCodeSectionReadScope::~EsScriptCodeSectionReadScope()
{
  m_reader.m_codeScope = m_prevScope;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// Compiled binary reader implementation
//
EsScriptCompiledBinaryReader::EsScriptCompiledBinaryReader(
  EsScriptMachine& machine,
  bool debugInfoRetain,
  const EsBinBuffer& buff
) :
m_machine(machine),
m_objectScope(nullptr),
m_codeScope(nullptr),
m_buff(buff),
m_debugInfoRetain(debugInfoRetain),
m_pos(0),
m_size(static_cast<ulong>(buff.size()))
{
  m_machine.reset();
  headerRead();
  binaryLinksRead();
  constantsRead();
  enumsRead();
  globalMethodsRead();
  metaclassesRead();
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::checkBufferHasEnoughSpace(ulong chunkSize) const
{
  ES_ASSERT(m_size >= m_pos);

  if(chunkSize > m_size-m_pos)
    EsException::Throw(
      esT("Out of bounds while reading from binary buffer. Requested %d bytes, only %d bytes available."),
      chunkSize,
      m_size-m_pos
    );
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::chunkRead(void* dest, ulong chunkSize)
{
  checkBufferHasEnoughSpace(chunkSize);
  memcpy(dest, &m_buff[m_pos], chunkSize);
  m_pos += chunkSize;
}
//---------------------------------------------------------------------------

long EsScriptCompiledBinaryReader::typeRead(long expectedType /*= binaryTypeNone*/)
{
  long result = 0;
  chunkRead(&result, EsScriptCompiledBinary::typeFieldSize);

  // first, check that value read is proper type value
  if(
    (0 <= result && EsVariant::VAR_VARIANT_COLLECTION >= result) ||
    (binaryTypeLinks <= result && binaryTypeHeader >= result)
  )
  {
    // check expectedType as well, if other than -1 is specified
    if(binaryTypeNone != expectedType && expectedType != result)
      EsException::Throw(
        esT("Unexpected type value read. Expected %d, read %d."),
        expectedType,
        result
      );
  }
  else
    EsException::Throw(
      esT("Invalid type value read: %d."),
      result
    );

  return result;
}
//---------------------------------------------------------------------------

EsString EsScriptCompiledBinaryReader::internalStringRead()
{
  EsBinBuffer ustr;
  esU32 tmp;
  // read block size in bytes
  chunkRead(&tmp, sizeof(tmp));
  // read string length. NB! in chars, not bytes
  chunkRead(&tmp, sizeof(tmp));

  if( tmp )
  {
    ustr.resize(tmp);
    chunkRead(&ustr[0], tmp);
  }

  return EsStr::fromByteString(ustr, EsString::UTF8);
}
//---------------------------------------------------------------------------

EsString EsScriptCompiledBinaryReader::stringRead()
{
  typeRead(EsVariant::VAR_STRING);
  return internalStringRead();
}
//---------------------------------------------------------------------------

EsStringArray EsScriptCompiledBinaryReader::internalStringArrayRead()
{
  EsStringArray result;
  esU32 tmp;
  // read block size in bytes
  chunkRead(&tmp, sizeof(tmp));
  // read array items count
  chunkRead(&tmp, sizeof(tmp));
  result.reserve(tmp);
  for(esU32 idx = 0; idx < tmp; ++idx)
    result.push_back( stringRead() );

  return result;
}
//---------------------------------------------------------------------------

EsStringArray EsScriptCompiledBinaryReader::stringArrayRead(long expectedType /*= binaryTypeNone*/)
{
  typeRead(expectedType);
  return internalStringArrayRead();
}
//---------------------------------------------------------------------------

EsVariant EsScriptCompiledBinaryReader::internalVariantCollectionRead()
{
  EsVariant result(EsVariant::VAR_VARIANT_COLLECTION);
  esU32 tmp;
  // read block size in bytes
  chunkRead(&tmp, sizeof(tmp));
  // read collection items count
  chunkRead(&tmp, sizeof(tmp));
  for(esU32 idx = 0; idx < tmp; ++idx)
    result.addToVariantCollection( variantRead() );

  return result;
}
//---------------------------------------------------------------------------

EsVariant EsScriptCompiledBinaryReader::variantCollectionRead()
{
  typeRead(EsVariant::VAR_VARIANT_COLLECTION);
  return internalVariantCollectionRead();
}
//---------------------------------------------------------------------------

EsVariant EsScriptCompiledBinaryReader::objectRead()
{
  esU32 tmp;
  chunkRead(&tmp, sizeof(esU32));
  const EsString& typeName = stringRead();

  if( EsRange::classNameGetStatic() == typeName )
  {
    const EsVariant& min = variantRead();
    const EsVariant& strictMin = variantRead();
    const EsVariant& max = variantRead();
    const EsVariant& strictMax = variantRead();
    return EsRange::NEW(min, strictMin, max, strictMax);
  }
  else if( EsAssocContainer::classNameGetStatic() == typeName )
  {
    EsAssocContainerIntf::Ptr ac = EsAssocContainer::create();
    ES_ASSERT(ac);

    ulong cnt = variantRead().asULong();
    for(ulong idx = 0; idx < cnt; ++idx)
    {
      const EsVariant& item = variantRead();
      ac->itemAppend(item);
    }

    return ac;
  }
  else
    return m_machine.enumFind(
      typeName,
      true
    );
}
//---------------------------------------------------------------------------

EsVariant EsScriptCompiledBinaryReader::variantRead()
{
  long type = typeRead();
  // ensure read type is of variant type
  ES_ASSERT( 0 <= type && EsVariant::VAR_VARIANT_COLLECTION >= type );
  EsVariant result;

  switch( type )
  {
  case EsVariant::VAR_EMPTY:
    break;
  case EsVariant::VAR_BOOL:
  case EsVariant::VAR_BYTE:
  case EsVariant::VAR_CHAR:
  case EsVariant::VAR_UINT:
    {
      esU32 tmp;
      chunkRead(&tmp, sizeof(tmp));
      result = tmp;
      switch(type)
      {
      case EsVariant::VAR_BOOL:
        result = result.asBool(m_machine.loc());
        break;
      case EsVariant::VAR_BYTE:
        result = result.asByte(m_machine.loc());
        break;
      case EsVariant::VAR_CHAR:
        result = result.asChar(m_machine.loc());
        break;
      }
    }
    break;
  case EsVariant::VAR_INT:
    {
      esI32 tmp;
      chunkRead(&tmp, sizeof(tmp));
      result = tmp;
    }
    break;
  case EsVariant::VAR_UINT64:
    {
      esU64 tmp;
      chunkRead(&tmp, sizeof(tmp));
      result = tmp;
    }
    break;
  case EsVariant::VAR_INT64:
    {
      esI64 tmp;
      chunkRead(&tmp, sizeof(tmp));
      result = tmp;
    }
    break;
  case EsVariant::VAR_DOUBLE:
    {
      double tmp;
      chunkRead(&tmp, sizeof(tmp));
      result = tmp;
    }
    break;
  case EsVariant::VAR_BIN_BUFFER:
    {
      EsBinBuffer buff;
      esU32 tmp;
      chunkRead(&tmp, sizeof(tmp));
      if(tmp)
      {
        buff.resize(tmp);
        chunkRead(&buff[0], tmp);
      }

      result = buff;
    }
    break;
  case EsVariant::VAR_STRING:
    result = internalStringRead();
    break;
  case EsVariant::VAR_STRING_COLLECTION:
    result = internalStringArrayRead();
    break;
  case EsVariant::VAR_VARIANT_COLLECTION:
    result = internalVariantCollectionRead();
    break;
  case EsVariant::VAR_OBJECT:
    result = objectRead();
    break;
  default:
    ES_FAIL;
  }

  return result;
}
//---------------------------------------------------------------------------

EsAttributesIntf::Ptr EsScriptCompiledBinaryReader::attributesRead()
{
  EsAttributesIntf::Ptr result;
  typeRead(binaryTypeAttributes);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // items count
  chunkRead(&tmp, sizeof(tmp));
  if(tmp)
  {
    esU8 interlocked = 0;

    if( //< Modern attribute read
      EsUtilities::versionStrCompare(
        compilerVersionGet(),
        ES_MODERN_VER_START 
      ) != EsString::cmpLess
    )
      chunkRead( &interlocked, sizeof(interlocked) );

    result = EsAttributes::create(
      stringRead(),
      0 != interlocked
    );

    for(esU32 idx = 0; idx < tmp; ++idx)
    {
      const EsString& name = stringRead();
      const EsVariant& val = variantRead();

      result->attributeAdd(
        name,
        val
      );
    }
  }

  return result;
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::binaryLinksRead()
{
  typeRead(binaryTypeLinks);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // items count
  chunkRead(&tmp, sizeof(tmp));
  for( esU32 idx = 0; idx < tmp; ++idx )
    m_machine.linkAdd( stringRead() );
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::externRead()
{
  typeRead(binaryTypeExtern);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // read external symbol flags
  esU32 flags;
  chunkRead(&flags, sizeof(tmp));
  // read external symbol name
  const EsString& name = stringRead();

  if( flags & EsScriptSymbolFlag::ExternEnum )
    m_machine.externEnumDeclare(name);
  else if( flags & EsScriptSymbolFlag::ExternObject )
    m_machine.externMetaclassDeclare(name);
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::externsRead()
{
  typeRead(binaryTypeExterns);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // items count
  chunkRead(&tmp, sizeof(tmp));
  for( esU32 idx = 0; idx < tmp; ++idx )
    externRead();
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::constantRead()
{
  typeRead(binaryTypeConst);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // read constant name and value
  const EsString& name = stringRead();
  const EsVariant& val = variantRead();
  m_machine.constDeclare(name, val);
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::constantsRead()
{
  typeRead(binaryTypeConsts);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // items count
  chunkRead(&tmp, sizeof(tmp));
  for( esU32 idx = 0; idx < tmp; ++idx )
    constantRead();
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::variablesRead()
{
  ES_ASSERT(m_codeScope);

  EsScriptCodeSection::Ptr cs = m_codeScope->sectionGet();
  ES_ASSERT(cs);

  const EsStringArray& vars = stringArrayRead(binaryTypeVariables);
  for( size_t idx = 0; idx < vars.size(); ++idx )
    cs->variableDeclare( vars[idx] );
}
//---------------------------------------------------------------------------

EsScriptDebugInfoIntf::Ptr EsScriptCompiledBinaryReader::debugInfoRead()
{
  EsScriptDebugInfoIntf::Ptr result;
  typeRead(binaryTypeDebugInfo);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  if( tmp && m_debugInfoRetain )
  {
    ulong line = variantRead().asULong();
    ulong col = variantRead().asULong();
    const EsString& file = stringRead();
    result = EsScriptDebugInfo::create(line, col, file);
  }
  else // skip entire block
    m_pos += tmp;

  return result;
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::instructionRead(const EsScriptCodeSection::Ptr& cs)
{
  ES_ASSERT(cs);
  typeRead(binaryTypeInstruction);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // instruction opcode
  chunkRead(&tmp, sizeof(tmp));

  if( //< Legacy instruction read
    EsUtilities::versionStrCompare(
      compilerVersionGet(),
      ES_MODERN_VER_START
    ) == EsString::cmpLess
  )
  {
    const EsVariant& payload = variantRead();
    EsScriptDebugInfoIntf::Ptr dbg = debugInfoRead();

    EsScriptInstructionOpcode opcode = static_cast<EsScriptInstructionOpcode>(tmp);
    EsScriptInstruction& instr = cs->instructionAdd(
      opcode,
      dbg
    );

    if( !payload.isEmpty() )
    {
      if( payload.isCollection() )
      {
        EsString::Array qname;
        const EsVariant::Array& items = payload.asVariantCollection();
        int nonstrParam = 0;
        bool exclusive = false;
        for(size_t idx = 0; idx < items.size(); ++idx)
        {
          const EsVariant& item = items[idx];
          if(item.isString())
            qname.push_back(
              item.asString()
            );
          else
          {
            switch( nonstrParam )
            {
            case 0:
              if( EsVariant::VAR_BOOL == item.typeGet() ) //< operator expression
              {
                instr.raw1set( item.asBool() );
                exclusive = true;
              }
              else
                instr.raw0set( item.asULong() );
              break;
            case 1:
              if( exclusive )
                instr.raw0set( item.asULong() );
              else
                instr.raw1set( item.asULong() );
              break;
            case 2:
              instr.raw2set( item.asULong() );
              break;
            }
            ++nonstrParam;

            if( iLogicCheck == opcode )
              break;
          }
        }

        if( !qname.empty() )
          instr.qnameSet(qname);
      }
      else
      {
        if( payload.isString() )
          instr.nameSet(
            payload.asString()
          );
        else if( iLogicCheck == opcode )
        {
          instr.jumpOffsSet( joRel );
          instr.jumpOffsSet( payload.asLong() ); //< Logic check shortcut offset
        }
        else
          instr.raw0set(
            payload.asULong()
          );
      }
    }
  }
  else
  {
    EsScriptInstruction& instr = cs->instructionAdd(
      static_cast<EsScriptInstructionOpcode>(tmp)
    );

    // instruction payload mask
    chunkRead( &tmp, sizeof(tmp) );
    ulong mask = tmp;

    if( EsScriptInstruction::PayloadNone != mask )
    {
      if( mask & EsScriptInstruction::Payload0 )
      {
        chunkRead( &tmp, sizeof(tmp) );
        instr.raw0set( tmp );
      }

      if( mask & EsScriptInstruction::Payload1 )
      {
        chunkRead( &tmp, sizeof(tmp) );
        instr.raw1set( tmp );
      }

      if( mask & EsScriptInstruction::Payload2 )
      {
        chunkRead( &tmp, sizeof(tmp) );
        instr.raw2set( tmp );
      }

      if( mask & EsScriptInstruction::Payload3 )
        instr.qnameSet(
          stringArrayRead()
        );
    }

    instr.dbgInfoSet(
      debugInfoRead()
    );
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::instructionsRead()
{
  ES_ASSERT(m_codeScope);

  EsScriptCodeSection::Ptr cs = m_codeScope->sectionGet();
  ES_ASSERT(cs);

  typeRead(binaryTypeInstructions);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // items count
  chunkRead(&tmp, sizeof(tmp));
  for(esU32 idx = 0; idx < tmp; ++idx )
    instructionRead(cs);
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::tryCatchBlockRead(const EsScriptCodeSection::Ptr& cs)
{
  ES_ASSERT(cs);
  typeRead(binaryTypeTryCatchBlock);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // tryStart && end
  esU32 tryStart, tryEnd, catchStart, catchEnd;
  chunkRead(&tryStart, sizeof(tmp));
  chunkRead(&tryEnd, sizeof(tmp));
  // catchStart && end
  chunkRead(&catchStart, sizeof(tmp));
  chunkRead(&catchEnd, sizeof(tmp));
  cs->tryCatchBlockCreate(tryStart, tryEnd, catchStart, catchEnd);
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::tryCatchBlocksRead()
{
  ES_ASSERT(m_codeScope);

  EsScriptCodeSection::Ptr cs = m_codeScope->sectionGet();
  ES_ASSERT(cs);

  typeRead(binaryTypeTryCatchBlocks);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // items count
  chunkRead(&tmp, sizeof(tmp));
  for( esU32 idx = 0; idx < tmp; ++idx )
    tryCatchBlockRead(cs);
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::dependenciesRead(const EsScriptCodeSection::Ptr& cs)
{
  ES_ASSERT(cs);
  const EsStringArray& fldDependencies = stringArrayRead(binaryTypeDependencies);
  for(size_t idx = 0; idx < fldDependencies.size(); ++idx)
    cs->thisFieldDependencyAdd( fldDependencies[idx] );
  const EsStringArray& varDependencies = stringArrayRead(binaryTypeDependencies);
  for(size_t idx = 0; idx < varDependencies.size(); ++idx)
    cs->thisMemberVarDependencyAdd( varDependencies[idx] );
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::codeSectionAttributesAssign(const EsAttributesIntf::Ptr& attrs, const EsScriptCodeSection::Ptr& cs)
{
  ES_ASSERT(cs);
  if( attrs )
  {
    const EsStringArray& a = attrs->allNamesGet();
    for(size_t idx = 0; idx < a.size(); ++idx)
    {
      const EsString& name = a[idx];
      m_machine.functionAttributeDeclare(cs, name, attrs->attributeGet(name));
    }
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::codeSectionRead(long codeSectionTag /*= binaryTypeCodeSection*/)
{
  codeSectionTag = typeRead(codeSectionTag);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  if( tmp )
  {
    EsString name;
    EsAttributesIntf::Ptr attrs;
    EsStringArray params;
    EsScriptCodeSection::Ptr cs;

    if( binaryTypeCodeSection == codeSectionTag )
    {
      // read name
      name = stringRead();
      // read code section attributes
      attrs = attributesRead();
      // read parameters
      params = stringArrayRead(binaryTypeParameters);
      if( m_objectScope )
        cs = m_machine.methodDeclare(
          name,
          params,
          m_objectScope->objectGet()
        );
      else
      {
        if( EsScriptStdNames::startup() == name )
          cs = m_machine.startupMethodGet();
        else
          cs = m_machine.globalMethodDeclare(
            name,
            params
          );
      }
    }
    else if( binaryTypeFieldExpression == codeSectionTag )
    {
      ES_ASSERT(m_objectScope);
      cs = m_objectScope->objectGet()->thisFieldExprGet();
    }

    ES_ASSERT(cs);
    codeSectionAttributesAssign(attrs, cs);
    EsScriptCodeSectionReadScope csScope(
      *this,
      cs
    );

    if( binaryTypeCodeSection == codeSectionTag )
      variablesRead();
    // read dependencies
    dependenciesRead(cs);
    // read instructions
    instructionsRead();
    // read try-catch blocks
    tryCatchBlocksRead();
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::attributesAssignToObj(const EsAttributesIntf::Ptr& attrs, const EsScriptObjectIntf::Ptr& obj)
{
  if(attrs && !attrs->isEmpty())
  {
    const EsStringArray& aNames = attrs->allNamesGet();
    for( size_t idx = 0; idx < aNames.size(); ++idx )
      m_machine.objectAttributeDeclare(
        obj,
        aNames[idx],
        attrs->attributeGet(
          aNames[idx]
        )
      );
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::branchFieldRead(const EsScriptObjectIntf::Ptr& ifBranch)
{
  ES_ASSERT(ifBranch);
  ES_ASSERT(ifBranch->flagsGet() & EsScriptObject::ofIfBranch);

  EsScriptObjectReadScope fldScope(*this, ifBranch);
  // read scoped member vars
  memberVariablesRead();
  // read scoped fields
  fieldsRead();
  // read field internally scoped code sections
  codeSectionsRead();
  // read scoped properties
  propertiesRead();
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::fieldRead()
{
  ES_ASSERT( m_objectScope );
  typeRead(binaryTypeField);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  ES_ASSERT(tmp);

  // read field object flags
  tmp = variantRead().asULong();
  // read field name
  const EsString& name = stringRead();
  // read field type
  const EsString& type = stringRead();
  // read field attributes
  EsAttributesIntf::Ptr attrs = attributesRead();

  // declare field in currently active object scope, depending on its flags
  EsScriptObjectIntf::Ptr fld;
  EsScriptMachine::CompoundFieldCreationResult compoundFld;
  if(tmp & EsScriptObject::ofIf)
    compoundFld =  m_machine.metaclassIfFieldDeclare(m_objectScope->objectGet());
  else if(tmp & EsScriptObject::ofArray)
    compoundFld = m_machine.metaclassArrayFieldDeclare(
      m_objectScope->objectGet(),
      type,
      name,
      attrs
    );
  else
    fld = m_machine.metaclassFieldDeclare(
      m_objectScope->objectGet(),
      type,
      name,
      attrs
    );

  // read field expression
  if( tmp & (EsScriptObject::ofIf|EsScriptObject::ofArray) )
  {
    EsScriptObjectReadScope fldScope(
      *this,
      compoundFld.m_fieldMetaclass
    );

    EsScriptCodeSectionReadScope codeScope(
      *this,
      compoundFld.m_fieldExpr
    );

    codeSectionRead(binaryTypeFieldExpression);

    // read nested fields, if conditional branch is used
    if( tmp & EsScriptObject::ofIf )
    {
      const EsStringIndexedMap& m = compoundFld.m_fieldMetaclass->thisFieldsMapGet();
      branchFieldRead( m.valueGet(0).asExistingObject() );
      branchFieldRead( m.valueGet(1).asExistingObject() );
    }
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::fieldsRead()
{
  ES_ASSERT(m_objectScope);
  typeRead(binaryTypeFields);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // field count
  chunkRead(&tmp, sizeof(tmp));
  for(esU32 idx = 0; idx < tmp; ++idx)
    fieldRead();
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::codeSectionsRead(long tag /* = binaryTypeCodeSections*/)
{
  typeRead(tag);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // items count
  chunkRead(&tmp, sizeof(tmp));

  for(esU32 idx = 0; idx < tmp; ++idx)
    codeSectionRead();
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::memberVariablesRead()
{
  EsScriptObjectIntf::Ptr obj = m_objectScope->objectGet();
  ES_ASSERT(obj);
  const EsStringArray& vars = stringArrayRead(binaryTypeVariables);
  for( size_t idx = 0; idx < vars.size(); ++idx )
    obj->variableDeclare( vars[idx] );
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::propertyRead()
{
  typeRead(binaryTypeProperty);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  const EsString& name = stringRead();
  const EsString& readerName = stringRead();
  const EsString& writerName = stringRead();
  EsAttributesIntf::Ptr attrs = attributesRead();
  EsScriptObjectIntf::Ptr obj = m_objectScope->objectGet();
  ES_ASSERT(obj);
  obj->propertyDeclare(name, attrs, readerName, writerName);
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::propertiesRead()
{
  typeRead(binaryTypeProperties);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // properties count
  chunkRead(&tmp, sizeof(tmp));
  for(esU32 idx = 0; idx < tmp; ++idx)
    propertyRead();
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::metaclassRead()
{
  typeRead(binaryTypeMetaclass);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // read metaclass flags
  ulong flags = variantRead().asULong();
  // read metaclass typeName
  const EsString& name = stringRead();
  // read ancestor type name
  const EsString& baseName = stringRead();
  // read class attributes
  EsAttributesIntf::Ptr attrs = attributesRead();
  EsScriptObjectIntf::Ptr metaclass = m_machine.metaclassDeclare(name, baseName);
  ES_ASSERT(metaclass);
  attributesAssignToObj(attrs, metaclass);
  // enter metaclass scope
  EsScriptObjectReadScope metaclassScope(*this, metaclass);
  // read metaclass member vars
  memberVariablesRead();
  // read metaclass fields
  fieldsRead();
  // read metaclass methods
  codeSectionsRead();
  // read metaclass properties
  propertiesRead();
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::metaclassesRead()
{
  typeRead(binaryTypeMetaclasses);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // items count
  chunkRead(&tmp, sizeof(tmp));
  for(esU32 idx = 0; idx < tmp; ++idx)
    metaclassRead();
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::globalMethodsRead()
{
  codeSectionsRead(binaryTypeGlobalMethods);
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::enumItemRead(const EsEnumerationIntf::Ptr& enu)
{
  typeRead(binaryTypeEnumItem);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  const EsString& symbol = stringRead();
  const EsVariant& val = variantRead();
  const EsString& label = stringRead();
  m_machine.enumItemDeclare(enu, symbol, val, label);
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::enumAttributesRead(const EsEnumerationIntf::Ptr& enu)
{
  typeRead(binaryTypeAttributes);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // item count
  chunkRead(&tmp, sizeof(tmp));
  for(esU32 idx = 0; idx < tmp; ++idx)
  {
    const EsString& name = stringRead();
    const EsVariant& val = variantRead();
    m_machine.enumAttributeDeclare(
      enu,
      name,
      val
    );
  }
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::enumRead()
{
  typeRead(binaryTypeEnum);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  EsString name = stringRead();
  EsEnumerationIntf::Ptr enu = m_machine.enumDeclare(name);
  // attributes read
  enumAttributesRead(enu);
  // item count
  chunkRead(&tmp, sizeof(tmp));
  for(esU32 idx = 0; idx < tmp; ++ idx)
    enumItemRead(enu);
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::enumsRead()
{
  typeRead(binaryTypeEnums);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // item count
  chunkRead(&tmp, sizeof(tmp));
  for(esU32 idx = 0; idx < tmp; ++ idx)
    enumRead();
}
//---------------------------------------------------------------------------

void EsScriptCompiledBinaryReader::headerRead()
{
  typeRead(binaryTypeHeader);
  esU32 tmp;
  // block size
  chunkRead(&tmp, sizeof(tmp));
  // read compiler version
  m_compilerVersion = stringRead();

  if( //< Modern binary load
    EsUtilities::versionStrCompare(
      compilerVersionGet(),
      ES_MODERN_VER_START
    ) != EsString::cmpLess 
  )
  {
    typeRead(EsVariant::VAR_OBJECT);
    EsAssocContainerIntf::Ptr info = objectRead().asExistingObject();
    ES_ASSERT(info);

    if(!m_debugInfoRetain)
      info->clear();

    m_machine.m_filesInfo = info;
  }
  else
    m_machine.m_filesInfo->clear();
}
//---------------------------------------------------------------------------
