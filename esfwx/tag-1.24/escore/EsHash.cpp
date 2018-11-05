#include "escorepch.h"
#pragma hdrstop

#include "EsHash.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// Compile time check for EsString-aware members
ES_COMPILE_TIME_ASSERT(sizeof(esU16) >= sizeof(EsString::value_type), EsStringValueTypeDoesNotFitInU16);
//---------------------------------------------------------------------------

// 32 bit FNV-1a
EsHashFNV_1a_32::EsHashFNV_1a_32()
{
	reset();
}
//---------------------------------------------------------------------------

EsHashFNV_1a_32::EsHashFNV_1a_32(const EsHashFNV_1a_32& src) :
m_hash( src.m_hash )
{}
//---------------------------------------------------------------------------

EsHashFNV_1a_32::EsHashFNV_1a_32(const EsString& str)
{
	reset();
	update(str);
}
//---------------------------------------------------------------------------

EsHashFNV_1a_32::EsHashFNV_1a_32(const EsByteString& bstr)
{
	reset();
	update(bstr);
}
//---------------------------------------------------------------------------

EsHashFNV_1a_32::EsHashFNV_1a_32(const EsBinBuffer& buff)
{
	reset();
	update(buff);
}
//---------------------------------------------------------------------------

void EsHashFNV_1a_32::update(const EsString& str) ES_NOTHROW
{
  size_t cnt = str.size();
  EsString::const_pointer cp = str.c_str();

	for(size_t idx = 0; idx < cnt; ++idx)
		m_hash = fnv1a<esU32>(static_cast<esU16>(cp[idx]), m_hash);
}
//---------------------------------------------------------------------------

void EsHashFNV_1a_32::update(const EsBinBuffer& buff) ES_NOTHROW
{
  size_t cnt = buff.size();
  EsBinBuffer::const_pointer cp = buff.data();

	for(size_t idx = 0; idx < cnt; ++idx)
		m_hash = fnv1a<esU32>(cp[idx], m_hash);
}
//---------------------------------------------------------------------------

void EsHashFNV_1a_32::update(const EsByteString& bstr) ES_NOTHROW
{
  size_t cnt = bstr.size();
  EsByteString::const_pointer cp = bstr.c_str();

	for(size_t idx = 0; idx < cnt; ++idx)
		m_hash = fnv1a<esU32>(static_cast<esU8>(cp[idx]), m_hash);
}
//---------------------------------------------------------------------------

void EsHashFNV_1a_32::reset() ES_NOTHROW
{
	m_hash = FNV_Traits<esU32>::c_init;
}
//---------------------------------------------------------------------------

const EsHashFNV_1a_32& EsHashFNV_1a_32::operator=(const EsHashFNV_1a_32& src) ES_NOTHROW
{
	m_hash = src.m_hash;
	return *this;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsHashFNV_1a_64::EsHashFNV_1a_64()
{
	reset();
}
//---------------------------------------------------------------------------

EsHashFNV_1a_64::EsHashFNV_1a_64(const EsHashFNV_1a_64& src) :
	m_hash( src.m_hash )
{}
//---------------------------------------------------------------------------

EsHashFNV_1a_64::EsHashFNV_1a_64(const EsString& str)
{
	reset();
	update(str);
}
//---------------------------------------------------------------------------

EsHashFNV_1a_64::EsHashFNV_1a_64(const EsByteString& bstr)
{
	reset();
	update(bstr);
}
//---------------------------------------------------------------------------

EsHashFNV_1a_64::EsHashFNV_1a_64(const EsBinBuffer& buff)
{
	reset();
	update(buff);
}
//---------------------------------------------------------------------------

void EsHashFNV_1a_64::update(const EsString& str) ES_NOTHROW
{
  size_t cnt = str.size();
  EsString::const_pointer cp = str.c_str();

	for(size_t idx = 0; idx < cnt; ++idx)
		m_hash = fnv1a<esU64>(static_cast<esU16>(cp[idx]), m_hash);
}
//---------------------------------------------------------------------------

void EsHashFNV_1a_64::update(const EsBinBuffer& buff) ES_NOTHROW
{
  size_t cnt = buff.size();
  EsBinBuffer::const_pointer cp = buff.data();

	for(size_t idx = 0; idx < cnt; ++idx)
		m_hash = fnv1a<esU64>(cp[idx], m_hash);
}
//---------------------------------------------------------------------------

void EsHashFNV_1a_64::update(const EsByteString& bstr) ES_NOTHROW
{
  size_t cnt = bstr.size();
  EsByteString::const_pointer cp = bstr.c_str();

	for(size_t idx = 0; idx < cnt; ++idx)
		m_hash = fnv1a<esU64>(static_cast<esU8>(cp[idx]), m_hash);
}
//---------------------------------------------------------------------------

void EsHashFNV_1a_64::reset() ES_NOTHROW
{
	m_hash = FNV_Traits<esU64>::c_init;
}
//---------------------------------------------------------------------------

const EsHashFNV_1a_64& EsHashFNV_1a_64::operator=(const EsHashFNV_1a_64& src) ES_NOTHROW
{
	m_hash = src.m_hash;
	return *this;
}
//---------------------------------------------------------------------------

