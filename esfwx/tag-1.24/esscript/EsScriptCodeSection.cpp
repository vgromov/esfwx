#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptCodeSection.h"
#include "EsScriptThreadContext.h"

#ifdef ESSCRIPT_CODESECTION_USE_TRACE
#  define ESSCRIPT_CODESECTION_TRACE1(a)                  ES_DEBUG_TRACE(a);
#  define ESSCRIPT_CODESECTION_TRACE2(a, b)              ES_DEBUG_TRACE((a), (b));
#  define ESSCRIPT_CODESECTION_TRACE3(a, b, c)            ES_DEBUG_TRACE((a), (b), (c));
#  define ESSCRIPT_CODESECTION_TRACE4(a, b, c, d)        ES_DEBUG_TRACE((a), (b), (c), (d));
#  define ESSCRIPT_CODESECTION_TRACE5(a, b, c, d, e)      ES_DEBUG_TRACE((a), (b), (c), (d), (e));
#  define ESSCRIPT_CODESECTION_TRACE6(a, b, c, d, e, f)  ES_DEBUG_TRACE((a), (b), (c), (d), (e), (f));
#else
#  define ESSCRIPT_CODESECTION_TRACE1(a)
#  define ESSCRIPT_CODESECTION_TRACE2(a, b)
#  define ESSCRIPT_CODESECTION_TRACE3(a, b, c)
#  define ESSCRIPT_CODESECTION_TRACE4(a, b, c, d)
#  define ESSCRIPT_CODESECTION_TRACE5(a, b, c, d, e)
#  define ESSCRIPT_CODESECTION_TRACE6(a, b, c, d, e, f)
#endif

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// try-catch frame class implementation
//
EsScriptTryCatchBlock::EsScriptTryCatchBlock(
  EsScriptCodeSection* cs /*= nullptr*/,
  ulong tryStart /*= 0*/,
  ulong tryEnd /*= 0*/,
  ulong catchStart /*= 0*/,
  ulong catchEnd /*= 0*/
) ES_NOTHROW :
m_owner(cs),
m_tryInstructionsStart(tryStart),
m_tryInstructionsEnd(tryEnd),
m_catchInstructionsStart(catchStart),
m_catchInstructionsEnd(catchEnd),
m_tryActive(false)
{
  if(
    cs &&
    0 == tryStart &&
    0 == tryEnd &&
    0 == catchStart &&
    0 == catchEnd
  )
  {
    m_tryInstructionsEnd = m_tryInstructionsStart = cs->instructionEndPosGet();
    m_catchInstructionsStart = m_catchInstructionsEnd = m_tryInstructionsEnd;
  }
}
//---------------------------------------------------------------------------

EsScriptTryCatchBlock::EsScriptTryCatchBlock(const EsScriptTryCatchBlock& other) ES_NOTHROW
{
  this->operator=(other);
}
//---------------------------------------------------------------------------

const EsScriptTryCatchBlock& EsScriptTryCatchBlock::operator=(const EsScriptTryCatchBlock& other) ES_NOTHROW
{
  m_owner = other.m_owner;
  m_tryInstructionsStart = other.m_tryInstructionsStart;
  m_tryInstructionsEnd = other.m_tryInstructionsEnd;
  m_catchInstructionsStart = other.m_catchInstructionsStart;
  m_catchInstructionsEnd = other.m_catchInstructionsEnd;

  return *this;
}
//---------------------------------------------------------------------------

void EsScriptTryCatchBlock::updateInstructionRange() ES_NOTHROW
{
  ES_ASSERT(m_owner);
  if( m_tryActive )
  {
    m_tryInstructionsEnd = m_owner->instructionEndPosGet();

    ES_ASSERT(m_tryInstructionsStart <= m_tryInstructionsEnd);

    m_catchInstructionsStart = m_catchInstructionsEnd = m_tryInstructionsEnd;
  }
  else
  {
    m_catchInstructionsEnd = m_owner->instructionEndPosGet();

    ES_ASSERT(m_catchInstructionsStart <= m_catchInstructionsEnd);
  }

  ESSCRIPT_CODESECTION_TRACE5(
    esT("EsScriptTryCatchBlock::updateInstructionRange called\n\t")
    esT("try[%d, %d] catch[%d, %d]"),
    m_tryInstructionsStart,
    m_tryInstructionsEnd,
    m_catchInstructionsStart,
    m_catchInstructionsEnd
  );
}
//---------------------------------------------------------------------------

EsScriptInstructionOpcode EsScriptTryCatchBlock::execCatch(EsScriptThreadContext& ctx, EsScriptInstructions::const_iterator& instr)
{
  ES_ASSERT(m_owner);

  EsScriptInstructions::const_iterator tryCatchBlockEnd =
    m_owner->instructionStartGet()+m_catchInstructionsEnd;

  EsScriptInstructionOpcode result = ctx.exec(
    m_owner->instructionStartGet()+m_catchInstructionsStart,
    tryCatchBlockEnd,
    instr
  );

  if( instr < tryCatchBlockEnd )
    instr = tryCatchBlockEnd;

  return result;
}
//---------------------------------------------------------------------------

EsScriptInstructionOpcode EsScriptTryCatchBlock::exec(EsScriptThreadContext& ctx, EsScriptInstructions::const_iterator& instr)
{
  ES_ASSERT(m_owner);

  ESSCRIPT_CODESECTION_TRACE1(esT("EsScriptTryCatchBlock::exec called"))

  EsScriptInstructionOpcode result = iInvalid;
  try
  {
    EsScriptInstructions::const_iterator end =
      m_owner->instructionStartGet()+m_tryInstructionsEnd;

    ctx.m_csScope->stackSnapshotPush();

    result = ctx.exec(
      m_owner->instructionStartGet()+m_tryInstructionsStart,
      end,
      instr
    );

    EsScriptInstructions::const_iterator tryCatchBlockEnd =
      m_owner->instructionStartGet() + m_catchInstructionsEnd;

    if( instr < tryCatchBlockEnd )
      instr = tryCatchBlockEnd;
  }
  catch(const EsException& ex)
  {
    ctx.m_csScope->stackSnapshotPop();

    m_owner->m_vars.symbolValSetIgnoreReadOnly(
      EsScriptStdNames::exception(),
      EsExceptionInfo::create(
        ex,
        (*instr).debugInfoGet()
      )
    );
    result = execCatch(
      ctx,
      instr
    );
  }
  catch(const std::exception& ex)
  {
    ctx.m_csScope->stackSnapshotPop();

    m_owner->m_vars.symbolValSetIgnoreReadOnly(
      EsScriptStdNames::exception(),
      EsExceptionInfo::create(
        ex,
        (*instr).debugInfoGet()
      )
    );
    result = execCatch(
      ctx,
      instr
    );
  }
  catch(...)
  {
    ctx.m_csScope->stackSnapshotPop();

    EsScriptDebugInfoIntf::Ptr dbg = (*instr).debugInfoGet();
    EsString message;

    if( dbg )
      message = esT("Unknown exception occured at: ") + dbg->asString();
    else
      message = esT("Unknown exception occured");

    m_owner->m_vars.symbolValSetIgnoreReadOnly(
      EsScriptStdNames::exception(),
      EsExceptionInfo::create(
        ES_ENUM_ITEM(EsExceptionSeverity, Generic).asLong(),
        ES_ENUM_ITEM(EsExceptionFacility, Unknown).asLong(),
        0,
        message,
        EsVariant::null(),
        dbg
      )
    );

    result = execCatch(
      ctx,
      instr
    );
  }

  m_owner->activeTryCatchBlockDeselect();

  return result;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

// code section implementation
//
EsScriptCodeSection::EsScriptCodeSection(const EsString& name) :
m_name(name),
m_attrs(
  EsAttributes::create(
    name,
    false
  )
),
m_metaclass(nullptr),
m_this(nullptr),
m_template(true)
{
  ESSCRIPT_CODESECTION_TRACE2(esT("Code section '%s' created"), m_name.c_str())

  m_code.reset( new EsScriptInstructions );
  m_code->reserve(defCodeCnt);
  m_params.reset( new EsString::Array );
  m_params->reserve(defParamsCnt);
  m_thisFieldDependencies.reset( new EsString::Array );
  m_thisMemberVarDependencies.reset( new EsString::Array );
  m_thisFieldDependencies->reserve(defDependencyCount);
  m_thisMemberVarDependencies->reserve(defDependencyCount);

  // always add special built-in variable - for caught exception handling
  m_vars.symbolTemplateAdd(
    EsScriptStdNames::exception(),
    EsScriptSymbolFlag::BuiltIn|
    EsScriptSymbolFlag::ReadOnly
  );
}
//---------------------------------------------------------------------------

EsScriptCodeSection::EsScriptCodeSection(const EsString& name, EsScriptObjectIntf* metaclass) :
m_name(name),
m_attrs(
  EsAttributes::create(
    name,
    false
  )
),
m_metaclass(metaclass),
m_this(nullptr),
m_template(true)
{
  ES_ASSERT(metaclass);
  ESSCRIPT_CODESECTION_TRACE2(esT("Code section '%s' created"), m_name.c_str())
  m_code.reset( new EsScriptInstructions );
  m_code->reserve(defCodeCnt);
  m_params.reset( new EsString::Array );
  m_params->reserve(defParamsCnt);
  m_thisFieldDependencies.reset( new EsString::Array );
  m_thisFieldDependencies->reserve(defDependencyCount);
  m_thisMemberVarDependencies.reset( new EsString::Array );
  m_thisMemberVarDependencies->reserve(defDependencyCount);
  // always add special variable - for caught exception handling
  m_vars.symbolTemplateAdd(
    EsScriptStdNames::exception(),
    EsScriptSymbolFlag::BuiltIn|
    EsScriptSymbolFlag::ReadOnly
  );
  // since it's metaclass's method, add built-in "this" variable
  m_vars.symbolTemplateAdd(
    EsScriptStdNames::_this(),
    EsScriptSymbolFlag::BuiltIn|
    EsScriptSymbolFlag::ReadOnly
  );
}
//---------------------------------------------------------------------------

// special copy constructor. every templated src member gets cloned to non-template, but for the code container itself, which just gets referenced
EsScriptCodeSection::EsScriptCodeSection(const EsScriptCodeSection& src, EsScriptObjectIntf* This, const EsVariant& params) :
m_name(src.m_name),
m_metaclass(src.m_metaclass),
m_this(This),
m_thisFieldDependencies(src.m_thisFieldDependencies),
m_thisMemberVarDependencies(src.m_thisMemberVarDependencies),
m_attrs(src.m_attrs),
m_vars(src.m_vars, nullptr),
m_params(src.m_params),
m_code(src.m_code),
m_tryCatchBlocks(src.m_tryCatchBlocks),
m_template(false)
{
  parametersSet(params);
  // re-own try-catch blocks by this code section
  for(size_t idx = 0; idx < m_tryCatchBlocks.size(); ++idx)
    m_tryCatchBlocks[idx].m_owner = this;
  if( m_this )
  {
    // weak smartptr to the passed "This" object
    EsScriptObjectIntf::Ptr thisPtr(m_this, false, false);
    m_vars.symbolValSetIgnoreReadOnly(
      EsScriptStdNames::_this(),
      thisPtr
    );
  }
  ESSCRIPT_CODESECTION_TRACE2(esT("Code section '%s' created for execution"), m_name.c_str())
}
//---------------------------------------------------------------------------

EsScriptCodeSection::Ptr EsScriptCodeSection::create(const EsString& name, const EsString::Array& params)
{
  ES_ASSERT(!name.empty());
  EsScriptCodeSection::Ptr result( new EsScriptCodeSection(name) );
  ES_ASSERT(result);

  for( size_t idx = 0; idx < params.size(); ++idx )
    result->paramDeclare( params[idx] );

  return result;
}
//---------------------------------------------------------------------------

EsScriptCodeSection::Ptr EsScriptCodeSection::create(const EsString& name, const EsString::Array& params, EsScriptObjectIntf* metaclass)
{
  ES_ASSERT(!name.empty());
  ES_ASSERT(metaclass);
  EsScriptCodeSection::Ptr result(new EsScriptCodeSection(name, metaclass));
  ES_ASSERT(result);

  for( size_t idx = 0; idx < params.size(); ++idx )
    result->paramDeclare( params[idx] );

  return result;
}
//---------------------------------------------------------------------------

EsScriptCodeSection::~EsScriptCodeSection() ES_NOTHROW
{
  ESSCRIPT_CODESECTION_TRACE3(esT("Code section '%s' (taking %d parameters) dtor called"),
    m_name.c_str(), m_params->size())

  try
  {
    m_metaclass = nullptr;
    if( m_this )
    {
      // reset 'this' variable
      m_vars.symbolValSetIgnoreReadOnly(
        EsScriptStdNames::_this(),
        EsVariant::null()
      );
      m_this = nullptr;
    }
  }
  catch( ... )
  {
    // Do not allow anything to escape dtor
  }
}
//--------------------------------------------------------------------------------

const EsScriptCodeSection::Ptr& EsScriptCodeSection::null()
{
  static const EsScriptCodeSection::Ptr sc_null;

  return sc_null;
}

//---------------------------------------------------------------------------

EsScriptInstruction& EsScriptCodeSection::instructionAdd(
  ulong& pos,
  EsScriptInstructionOpcode opcode,
  const EsScriptDebugInfoIntf::Ptr& debugInfo /*=EsScriptDebugInfoIntf::Ptr()*/
)
{
  checkTemplateOperation();
  // assert we're inside try-catch block catch section for iRethrow
  if( iRethrow == opcode &&
      !m_tryCatchStack.empty() &&
      m_tryCatchBlocks[ m_tryCatchStack.back() ].m_tryActive
  )
    EsScriptException::Throw(
      esT("'rethrow' outside the 'catch' section"),
      debugInfo
    );

  ES_ASSERT(iInvalid != opcode);
  EsScriptInstruction instr(
    opcode,
    debugInfo
  );

  ES_ASSERT(m_code);
  m_code->push_back(instr);

  // update all try-catch blocks in stack
  for(size_t idx = 0; idx < m_tryCatchStack.size(); ++idx)
    m_tryCatchBlocks[ m_tryCatchStack[idx] ].updateInstructionRange();

  ESSCRIPT_CODESECTION_TRACE3(
    esT("instruction '%s' added to '%s'"),
    EsScriptInstruction::getOpcodeString(instr.opcode()),
    m_name.c_str()
  )

  pos = static_cast<ulong>(m_code->size()-1);

  return m_code->at(pos);
}
//---------------------------------------------------------------------------

EsScriptInstruction& EsScriptCodeSection::instructionModifyAt(ulong instrPos)
{
  checkTemplateOperation();
  ES_ASSERT(m_code);
  ES_ASSERT(instrPos < m_code->size());

  return m_code->at(instrPos);
}
//---------------------------------------------------------------------------

// add variable to code section, registering it in symtable
void EsScriptCodeSection::variableDeclare(const EsString& name, const EsScriptDebugInfoIntf::Ptr& dbg)
{
  m_vars.symbolTemplateAdd(
    name,
    EsScriptSymbolFlag::None,
    dbg
  );

  ESSCRIPT_CODESECTION_TRACE3(
    esT("'%s' declared in section '%s'"),
    name.c_str(),
    m_name.c_str()
  )
}
//---------------------------------------------------------------------------

void EsScriptCodeSection::variableDeclarePrivateReusable(const EsString& name, const EsScriptDebugInfoIntf::Ptr& dbg)
{
  if( !m_vars.symbolExists(name) )
  {
    m_vars.symbolTemplateAdd(
      name,
      EsScriptSymbolFlag::None,
      dbg
    );

    ESSCRIPT_CODESECTION_TRACE3(
      esT("Private reusable '%s' declared in section '%s'"),
      name.c_str(),
      m_name.c_str()
    )
  }
}
//---------------------------------------------------------------------------

// add input parameter to code section
void EsScriptCodeSection::paramDeclare(const EsString& name, const EsScriptDebugInfoIntf::Ptr& dbg)
{
  variableDeclare(
    name,
    dbg
  );
  m_params->push_back(name);

  ESSCRIPT_CODESECTION_TRACE3(
    esT("param '%s' declared in section '%s'"),
    name.c_str(),
    m_name.c_str()
  )
}
//---------------------------------------------------------------------------

// indexed parameter access
void EsScriptCodeSection::parameterSet(ulong idx, const EsVariant& val)
{
  ES_ASSERT(idx < static_cast<ulong>(m_params->size()));
  m_vars.symbolValSet(
    m_params->at(idx),
    val
  );
}
//---------------------------------------------------------------------------

// all-at-once parameters assignment
void EsScriptCodeSection::parametersSet(const EsVariant& params)
{
  if( params.typeGet() == EsVariant::VAR_VARIANT_COLLECTION )
  {
    ES_ASSERT(static_cast<size_t>(params.countGet()) == m_params->size());
    for( ulong idx = 0; idx < params.countGet(); ++idx )
      parameterSet(idx, params.itemGet(idx));
  }
  else if( !params.isEmpty() )
  {
    ES_ASSERT(1 == m_params->size());
    parameterSet(0, params);
  }
}
//---------------------------------------------------------------------------

EsScriptValAccessorIntf::Ptr EsScriptCodeSection::resultPop() ES_NOTHROW
{
  if( m_return )
  {
    EsScriptValAccessorIntf::Ptr result = m_return;
    m_return.reset();
    return result;
  }

  return EsScriptMachine::nullcGet();
}
//---------------------------------------------------------------------------

EsScriptObjectIntf* EsScriptCodeSection::metaclassGet() const ES_NOTHROW
{
  if( m_metaclass )
    return m_metaclass->topNonProxyGet();

  return nullptr;
}
//---------------------------------------------------------------------------

EsScriptObjectIntf* EsScriptCodeSection::thisGet() const ES_NOTHROW
{
  if( m_this )
    return m_this->topNonProxyGet();

  return nullptr;
}
//---------------------------------------------------------------------------

// try-catch blocks manipulation
//
// create new try-catch block, return its index in try-catch blocks collection,
// make new block the active one
ulong EsScriptCodeSection::tryCatchBlockCreate(ulong tryStart /*= 0*/, ulong tryEnd /*= 0*/, ulong catchStart /*= 0*/, ulong catchEnd /*= 0*/)
{
  EsScriptTryCatchBlock block(
    this,
    tryStart,
    tryEnd,
    catchStart,
    catchEnd
  );

  m_tryCatchBlocks.push_back(block);
  ulong idx = static_cast<ulong>(m_tryCatchBlocks.size()-1);
  m_tryCatchStack.push_back(idx);

  ESSCRIPT_CODESECTION_TRACE2(esT("new tryCatchBlock created with index %d, made active"), idx)

  return idx;
}
//---------------------------------------------------------------------------

// select currently active try-catch block
EsScriptInstructionOpcode EsScriptCodeSection::tryCatchBlockExecute(
  ulong tryCatchBlockIdx,
  EsScriptThreadContext& ctx,
  EsScriptInstructions::const_iterator& instr
)
{
  EsNumericCheck::checkRangeUInteger(
    0,
    m_tryCatchBlocks.empty() ?
      0 :
      static_cast<ulong>(m_tryCatchBlocks.size()-1),
    tryCatchBlockIdx,
    esT("tryCatch block index")
  );

  if(
    m_tryCatchStack.empty() ||
    m_tryCatchStack.back() != tryCatchBlockIdx
  )
    m_tryCatchStack.push_back(tryCatchBlockIdx);

  return m_tryCatchBlocks[tryCatchBlockIdx].exec(
    ctx,
    instr
  );
}
//---------------------------------------------------------------------------

// select try-block of the currently active try-catch block
void EsScriptCodeSection::tryBlockSelect()
{
  ES_ASSERT( !m_tryCatchStack.empty() );
  checkTemplateOperation();
  ESSCRIPT_CODESECTION_TRACE1(esT("Try block selected for active Try-Catch block"))
  m_tryCatchBlocks[m_tryCatchStack.back()].m_tryActive = true;
}
//---------------------------------------------------------------------------

// select catch-block of the currently active try-catch block
void EsScriptCodeSection::catchBlockSelect()
{
  ES_ASSERT( !m_tryCatchStack.empty() );
  checkTemplateOperation();
  ESSCRIPT_CODESECTION_TRACE1(esT("Catch block selected for active Try-Catch block"))
  m_tryCatchBlocks[m_tryCatchStack.back()].m_tryActive = false;
}
//---------------------------------------------------------------------------

// return true if the catch block is currently active
bool EsScriptCodeSection::isCatchBlockSelected() const
{
  checkTemplateOperation();
  return !m_tryCatchStack.empty() &&
    !m_tryCatchBlocks[m_tryCatchStack.back()].m_tryActive;
}
//---------------------------------------------------------------------------

// deselect the currently active try-catch block
void EsScriptCodeSection::activeTryCatchBlockDeselect() ES_NOTHROW
{
  ES_ASSERT( !m_tryCatchStack.empty() );
  ESSCRIPT_CODESECTION_TRACE2(esT("Exiting active Try-Catch block at %d"), m_tryCatchStack.size()-1)
  m_tryCatchStack.pop_back();
}
//---------------------------------------------------------------------------

#ifdef ES_DEBUG
EsString EsScriptCodeSection::asString() const
{
  EsString result = m_name;
  for(size_t idx = 0; idx < m_code->size(); ++idx )
    result += esT("\n") + EsScriptThreadContext::instructionAsString(m_code->at(idx));

  return result;
}
#endif
//---------------------------------------------------------------------------

