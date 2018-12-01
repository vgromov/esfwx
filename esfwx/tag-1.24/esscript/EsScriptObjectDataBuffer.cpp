#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptObjectDataBuffer.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

EsScriptObjectDataBuffer::EsScriptObjectDataBuffer(EsScriptObjectDataBuffer::private_access) :
EsBinBuffer()
{}
//--------------------------------------------------------------------------------

EsScriptObjectDataBuffer::Ptr EsScriptObjectDataBuffer::create()
{
  EsScriptObjectDataBuffer::Ptr ptr = std::make_shared<EsScriptObjectDataBuffer>(EsScriptObjectDataBuffer::private_access() );
  ES_ASSERT(ptr);

  return ptr;
}
//--------------------------------------------------------------------------------

EsScriptObjectDataBuffer::Ptr& EsScriptObjectDataBuffer::null()
{
  static EsScriptObjectDataBuffer::Ptr sc_null;
  return sc_null;
}
//--------------------------------------------------------------------------------

// directly assign buffer from other data
void EsScriptObjectDataBuffer::assignDirect(EsBinBuffer::const_pointer src, size_t offs, size_t count)
{
  ES_ASSERT(m_bb.size() >= offs+count);
  memcpy( &m_bb[offs], src, count);
}

// reset accumulated fixups
void EsScriptObjectDataBuffer::fixupsReset()
{
  ESSCRIPT_OBJECT_BUFFER_TRACE1(esT("fixupsReset()"))
  m_fixupsDown.clear();
  m_fixupsUp.clear();
  m_fixupsNew.clear();
}

// handle memory layout notification fixup
void EsScriptObjectDataBuffer::onDataLayoutFixup(long oldOffs, size_t newOffs, size_t objSize)
{
  ESSCRIPT_OBJECT_BUFFER_TRACE4(esT("onDataLayoutFixup(oldOffs=%d, newOffs=%d, objSize=%d)"),
    oldOffs, newOffs, objSize)
  
  // if more space is needed, grow data
  if( size() < newOffs+objSize )
  {
    resize(newOffs+objSize);
    ESSCRIPT_OBJECT_BUFFER_TRACE3(esT("data buffer size increased, new size=%d, capacity=%d"),
      newOffs+objSize, capacity())
  }
  
  // append new fixup entry
  if( -1 < oldOffs )
  {
    ES_ASSERT( static_cast<size_t>(oldOffs) != newOffs );
    if( static_cast<size_t>(oldOffs) < newOffs )
      m_fixupsUp[ newOffs ] = Fixup(oldOffs, newOffs, objSize);
    else
      m_fixupsDown[ newOffs ] = Fixup(oldOffs, newOffs, objSize);
  }
  else
    m_fixupsNew[ newOffs ] = Fixup(oldOffs, newOffs, objSize);
}

// apply accumulated fixups
void EsScriptObjectDataBuffer::fixupsApply(size_t newSize)
{
  ESSCRIPT_OBJECT_BUFFER_TRACE2(esT("fixupsApply(newSize=%d)"), newSize)
  
  // fist, apply 'Up' fixups, in reversed order
  for(Fixups::const_reverse_iterator cit = m_fixupsUp.rbegin(); cit != m_fixupsUp.rend(); ++cit)
  {
    const Fixup& fixup = (*cit).second;
    copyUp(fixup.m_oldOffs, fixup.m_newOffs, fixup.m_objSize);
  }
  // second, apply 'Down' fixups in direct order
  for(Fixups::const_iterator cit = m_fixupsDown.begin(); cit != m_fixupsDown.end(); ++cit)
  {
    const Fixup& fixup = (*cit).second;
    copyDown(fixup.m_oldOffs, fixup.m_newOffs, fixup.m_objSize);
  }
  // and finally, zero out new blocks in direct order
  for(Fixups::const_iterator cit = m_fixupsNew.begin(); cit != m_fixupsNew.end(); ++cit)
  {
    const Fixup& fixup = (*cit).second;
    // zero out data blocks of the newly laid-out objects
    zeroOut(fixup.m_newOffs, fixup.m_objSize);
  }

  // decrease size in the end, if needed
  size_t sze = size();
  if( sze > newSize )
  {
    resize(newSize);
    sze = newSize;
  }
#ifdef ES_DEBUG
  if( sze != newSize )
    ES_FAIL;
#endif
  // reset fixups accumulator
  fixupsReset();
}

void EsScriptObjectDataBuffer::copyUp(size_t oldStart, size_t newStart, size_t size)
{
  ES_ASSERT(oldStart < newStart);
  ES_ASSERT(size);
  
  size_t oldPos = oldStart+size-1; 
  size_t pos = newStart+size-1;
  while(size--)
    m_bb[pos--] = m_bb[oldPos--];
}

void EsScriptObjectDataBuffer::copyDown(size_t oldStart, size_t newStart, size_t size)
{
  ES_ASSERT(oldStart > newStart);
  ES_ASSERT(size);  
  
  while(size--)
    m_bb[newStart++] = m_bb[oldStart++];
}

void EsScriptObjectDataBuffer::zeroOut(size_t start, size_t size)
{
  ES_ASSERT(size);
  ES_ASSERT(start+size <= m_bb.size());

  memset(&m_bb[start], 0, size);
}
