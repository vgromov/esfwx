#include "escorepch.h"
#pragma hdrstop

#include "EsBinBuffer.h"
#include <iterator>

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

EsBinBuffer::EsBinBuffer( const EsVariant& v ) :
m_bb( v.doInterpretAsBinBuffer().m_bb )
{}
//---------------------------------------------------------------------------

EsBinBuffer::EsBinBuffer( const esBA& ba) :
m_bb(ba.data, ba.size)
{}
//---------------------------------------------------------------------------

EsBinBuffer::~EsBinBuffer() ES_NOTHROW
{}
//---------------------------------------------------------------------------

const EsBinBuffer& EsBinBuffer::null() ES_NOTHROW
{
  static const EsBinBuffer sc_null;
  return sc_null;
}
//---------------------------------------------------------------------------

EsBinBuffer::size_type EsBinBuffer::search(const EsBinBuffer& src) const
{
  const_iterator cit = std::search(
    m_bb.begin(),
    m_bb.end(),
    src.m_bb.begin(),
    src.m_bb.end()
  );

  if( cit != m_bb.end() )
    return static_cast<EsBinBuffer::size_type>(cit-m_bb.begin());

  return EsBinBuffer::npos;
}
//---------------------------------------------------------------------------

EsBinBuffer::size_type EsBinBuffer::size() const ES_NOTHROW
{
  return m_bb.size();
}
//---------------------------------------------------------------------------

EsBinBuffer::operator esBA () const
{
  esBA ba;
  ba.size = static_cast<esU32>(m_bb.size());
  ba.data = const_cast<esU8*>(m_bb.empty() ? 0 : &m_bb[0]);

  return ba;
}
//---------------------------------------------------------------------------

EsBinBuffer::pointer EsBinBuffer::data() ES_NOTHROW
{
  return m_bb.data();
}
//---------------------------------------------------------------------------

EsBinBuffer::const_pointer EsBinBuffer::data() const ES_NOTHROW
{
  return m_bb.data();
}
//---------------------------------------------------------------------------

EsBinBuffer::reference EsBinBuffer::operator[](size_type _Pos)
{
  return m_bb[_Pos];
}
//---------------------------------------------------------------------------

EsBinBuffer::const_reference EsBinBuffer::operator[](size_type _Pos) const
{
  return m_bb[_Pos];
}
//---------------------------------------------------------------------------

EsBinBuffer& EsBinBuffer::operator= (const esBA& src)
{
  m_bb.assign(src.data, src.size);
  return *this;
}
//---------------------------------------------------------------------------

EsBinBuffer& EsBinBuffer::operator= (const EsBinBuffer& src)
{
  m_bb = src.m_bb;
  return *this;
}
//---------------------------------------------------------------------------

void EsBinBuffer::append(const EsBinBuffer& src)
{
  m_bb.append(src.m_bb);
}
//---------------------------------------------------------------------------

void EsBinBuffer::append(size_t size, const esU8* buff)
{
  ES_ASSERT(buff);
  if( size )
    m_bb.append(buff, size);
}
//---------------------------------------------------------------------------

EsBinBuffer EsBinBuffer::operator+ (const EsBinBuffer& src) const
{
  EsBinBuffer tmp( *this );
  tmp += src;

  return tmp;
}
//---------------------------------------------------------------------------

EsBinBuffer& EsBinBuffer::operator+= (const EsBinBuffer& src)
{
  append(src);

  return *this;
}
//---------------------------------------------------------------------------

EsBinBuffer& EsBinBuffer::operator+= (const esBA& src)
{
  append(src.size, src.data);

  return *this;
}
//---------------------------------------------------------------------------

