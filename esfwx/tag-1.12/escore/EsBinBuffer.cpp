#include "stdafx.h"
#pragma hdrstop

#include "EsBinBuffer.h"

EsBinBuffer::EsBinBuffer( const EsVariant& v ) :
m_bb( v.asBinBuffer().m_bb )
{}

EsBinBuffer::EsBinBuffer( const BYTEARRAY& ba) :
m_bb()
{
	m_bb.reserve(reserveCount);
	if( ba.size )
		std::copy(ba.data, ba.data+ba.size, std::inserter(m_bb, m_bb.end()));
}

EsBinBuffer::operator BYTEARRAY () const
{
	BYTEARRAY ba;
	ba.size = m_bb.size();
	ba.data = const_cast<BYTE*>(m_bb.empty() ? 0 : &m_bb[0]);

	return ba;
}

// BYTEARRAY assignment
EsBinBuffer& EsBinBuffer::operator= (const BYTEARRAY& src)
{
	m_bb.clear();
	m_bb.reserve(reserveCount);
	if( src.size )
		std::copy(src.data, src.data+src.size, std::inserter(m_bb, m_bb.end()));
	
	return *this;
}

// EsBinBuffer assignment
EsBinBuffer& EsBinBuffer::operator= (const EsBinBuffer& src)
{
	m_bb = src.m_bb;

	return *this;
}

// summation
EsBinBuffer& EsBinBuffer::operator+= (const EsBinBuffer& src)
{
	m_bb.reserve(m_bb.size()+src.size());
	m_bb.insert(m_bb.end(), src.m_bb.begin(), src.m_bb.end());
	
	return *this;
}

EsBinBuffer& EsBinBuffer::operator+= (const BYTEARRAY& src)
{
	return operator+= ( EsBinBuffer(src) );
}

// char strings to EsBinBuffer conversion
EsBinBuffer EsBinBuffer::fromAscii(const char* str, int strLen /*= -1*/)
{
	EsBinBuffer result;
	
	if( str )
	{
		if( strLen < 0 )
			strLen = strlen(str);
		std::copy(str, str+strLen, std::inserter(result, result.end()));
	}
	
	return result;
}

EsBinBuffer EsBinBuffer::fromAscii(const std::string& str)
{
	return EsBinBuffer::fromAscii(str.c_str(), -1);
}