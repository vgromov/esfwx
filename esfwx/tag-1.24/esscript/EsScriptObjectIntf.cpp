#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptObjectIntf.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// object traversers. "normal" traversers are OK for run-time usage, while
// "fast" ones meant for compile time only
//
// base object traverser functionality helper class
EsScriptObjectTraverserBase::EsScriptObjectTraverserBase(
  const EsScriptObjectIntf* obj,
  bool enumerateFields, 
  bool allHierarchy
) ES_NOTHROW :
m_obj(obj),
m_continue(true),
m_enumerateFields(enumerateFields),
m_allHierarchy(allHierarchy)
{
  // always start from the top non-proxy
  ES_ASSERT(m_obj);
  m_obj = const_cast<EsScriptObjectIntf*>(m_obj)->topNonProxyGet();
  ES_ASSERT(m_obj);
}

void EsScriptObjectTraverserBase::onEnumerateFields(const EsScriptObjectIntf* obj) ES_NOTHROW
{
  if( !obj )
    return;

  const EsStringIndexedMap& sm = obj->thisFieldsMapGet();
  for(ulong idx = 0; idx < sm.countGet() && m_continue; ++idx)
  {
    EsScriptObjectIntf::Ptr fp = EsScriptObject::fnodeFieldGet(sm.valueGet(idx));
    if( fp->isIf() )
    {
      fp = fp->conditionalBranchGet(-1);
      onObject( fp.get() );
    }
    else
    {
      ES_ASSERT(!fp->isConditional());
      const EsString& name = sm.nameGet(idx);
      if( m_enumerateFields )
        m_continue = fieldProcess( fp.get(), name );
    }
  }
}

void EsScriptObjectTraverserBase::onObject(const EsScriptObjectIntf* obj) ES_NOTHROW
{
  if( !obj )
    return;

  m_continue = objectProcess(obj);
  if( m_continue )
  {
    if( m_enumerateFields )
      onEnumerateFields(obj);
    else
    {
      // just dive into _if_s
      const EsScriptObjectIntf::ObjectsVector& ifv = obj->conditionalFieldsGet();
      for(size_t idx = 0; idx < ifv.size() && m_continue; ++idx )
      {
        EsScriptObjectIntf::Ptr ifp = ifv[idx];
        // dive into active branch
        EsScriptObjectIntf::Ptr ifb = ifp->conditionalBranchGet(-1);
        onObject( ifb.get() );
      }
    }
  }
}

// object traverser helper class specialization for ancestor-to-most-derived sequence
EsScriptObjectTopDownHierarchyTraverser::EsScriptObjectTopDownHierarchyTraverser(
  const EsScriptObjectIntf* obj,
  bool enumerateFields, 
  bool allHierarchy
) ES_NOTHROW :
EsScriptObjectTraverserBase(obj, enumerateFields, allHierarchy)
{}

void EsScriptObjectTopDownHierarchyTraverser::doEnterAncestor(const EsScriptObjectIntf* ancestor) ES_NOTHROW
{
  if(m_continue && ancestor)
  {
    doEnterAncestor(ancestor->ancestorGet().get());
    onObject(ancestor);
  }    
}

void EsScriptObjectTopDownHierarchyTraverser::traverse() ES_NOTHROW
{
  if( m_allHierarchy )
    doEnterAncestor(m_obj->ancestorGet().get());
    
  if( m_continue )
    onObject(m_obj);
}

// object traverser helper class specialization for most-derived-to-ancestor sequence
EsScriptObjectDownTopHierarchyTraverser::EsScriptObjectDownTopHierarchyTraverser(
  const EsScriptObjectIntf* obj, 
  bool enumerateFields, 
  bool allHierarchy
) ES_NOTHROW :
EsScriptObjectTraverserBase(obj, enumerateFields, allHierarchy)
{}

void EsScriptObjectDownTopHierarchyTraverser::traverse() ES_NOTHROW
{
  onObject(m_obj);
  
  if( m_allHierarchy && m_continue && m_obj )
  {
    EsScriptObjectIntf::Ptr ancestor = m_obj->ancestorGet();
    while(m_continue && ancestor)
    {
      onObject(ancestor.get());
      ancestor = ancestor->ancestorGet();
    }
  }
}

// "Fast" traverser base functionality
EsScriptObjectFastFindBase::EsScriptObjectFastFindBase(const EsScriptObjectIntf* obj) ES_NOTHROW :
m_startObj(obj),
m_obj(obj),
m_continue(true)
{
  ES_ASSERT(m_startObj);
}

void EsScriptObjectFastFindBase::find() ES_NOTHROW
{
  onObject(m_obj);

  if( m_continue && m_obj )
  {
    EsScriptObjectIntf::Ptr ancestor = m_obj->ancestorGet();
    if(m_continue && ancestor)
    {
      m_obj = ancestor.get();
      find();
    }
  }
}

void EsScriptObjectFastFindBase::onObject(const EsScriptObjectIntf* obj) ES_NOTHROW
{
  if( !obj )
    return;

  if( obj->isIf() || obj->isArray() )
  {
    m_obj = obj->parentGet();
    find();
  }
  else
  {
    m_continue = objectProcess();
    if( m_continue && obj->isIfBranch() )
    {
      m_obj = obj->parentGet();
      find();
    }
  }
}
