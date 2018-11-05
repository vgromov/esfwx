#include "escorepch.h"
#pragma hdrstop

#include "EsCrc.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

namespace _c_crc_impl_
{
#	define CRC_LOCAL_LINKAGE_DECL static
#	include "crcConfig.h"
#	include <esfwxe/crc.c>
# undef CRC_LOCAL_LINKAGE_DECL
}

// EsCRC8 implementation
//
ES_DECL_BASE_CLASS_INFO_BEGIN(EsCRC8, NO_CLASS_DESCR)
	// properties
	ES_DECL_PROP_INFO_RO(EsCRC8, value, esU8, NO_PROPERTY_LABEL, esT("Current CRC accumulator value"))
	// reflected constructors
	ES_DECL_REFLECTED_CTOR_INFO(EsCRC8, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CTOR_INFO(EsCRC8, EsBaseIntfPtr_ClassCall_esU8, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CTOR_INFO(EsCRC8, EsBaseIntfPtr_ClassCall_cr_EsBinBuffer_esU8, NO_METHOD_DESCR)
	// reflected methods
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsCRC8, update, void_Call_cr_EsBinBuffer, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

// constructors
EsCRC8::EsCRC8(const EsBinBuffer& buff, esU8 initialSum /*= 0xFF*/) : 
m_sum(initialSum)
{
	update(buff);
}

EsCRC8::EsCRC8(const esU8* buff/* = 0*/, size_t len/* = 0*/, esU8 initialSum /*= 0xFF*/) :
m_sum(initialSum)
{
	update(buff, len);
}

EsBaseIntfPtr EsCRC8::NEW()
{
	std::unique_ptr<EsCRC8> p( new EsCRC8 );
	ES_ASSERT(p.get());
	p->m_dynamic = true;
	return p.release()->asBaseIntfPtrDirect();
}

EsBaseIntfPtr EsCRC8::NEW(esU8 initialSum)
{
	std::unique_ptr<EsCRC8> p( new EsCRC8(0, 0, initialSum) );
	ES_ASSERT(p.get());	
	p->m_dynamic = true;
	return p.release()->asBaseIntfPtrDirect();
}

EsBaseIntfPtr EsCRC8::NEW(cr_EsBinBuffer buff, esU8 initialSum)
{
	EsCRC8* p = new EsCRC8(buff, initialSum);
	p->m_dynamic = true;
	return p->asBaseIntfPtrDirect();
}

// append buffer to internal crc accumulator
void EsCRC8::update(const esU8* buff, size_t len)
{
	m_sum = _c_crc_impl_::crc8(m_sum, buff, len);
}

esU8 EsCRC8::get_value() const
{
	return m_sum;
}

// calculate checksum16 as of RFC1071 (actually, it's IP _checksum_ algorithm)	
//
ES_DECL_BASE_CLASS_INFO_BEGIN(EsChecksum16, NO_CLASS_DESCR)
	// properties
	ES_DECL_PROP_INFO_RO(EsChecksum16, value, esU16, NO_PROPERTY_LABEL, esT("Current checksum value"))
	ES_DECL_PROP_INFO_WO(EsChecksum16, buffer, EsBinBuffer, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	// reflected constructors
	ES_DECL_REFLECTED_CTOR_INFO(EsChecksum16, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CTOR_INFO(EsChecksum16, EsBaseIntfPtr_ClassCall_cr_EsBinBuffer, NO_METHOD_DESCR)
	// reflected methods
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsChecksum16, reset, void_Call, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsChecksum16, update, void_Call_cr_EsBinBuffer, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsChecksum16, update, void_Call_cr_EsBinBuffer_ulong_ulong, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

EsChecksum16::EsChecksum16(const EsBinBuffer& buff) :
m_dynamic(false),
m_sum(0),
m_checksum(0),
m_dirty(false),
m_leftover(false)
{
	update( buff );
}

EsChecksum16::EsChecksum16(const esU8* buff /*= 0*/, size_t len /*= 0*/) :
m_dynamic(false),
m_sum(0),
m_checksum(0),
m_dirty(false),
m_leftover(false)
{
	update(buff, len);
}

EsBaseIntfPtr EsChecksum16::NEW()
{
	EsChecksum16* p = new EsChecksum16;
	p->m_dynamic = true;

	return p->asBaseIntfPtrDirect();
}

EsBaseIntfPtr EsChecksum16::NEW(cr_EsBinBuffer buff)
{
	EsChecksum16* p = new EsChecksum16(buff);
	p->m_dynamic = true;

	return p->asBaseIntfPtrDirect();
}

void EsChecksum16::reset()
{
	m_sum = 0;
	m_dirty = true;
}

// append buffer to the calculations
void EsChecksum16::update(const esU8* buff, size_t len)
{
	if( !buff || !len )
		return;
	
	if( m_leftover )
	{
		// consider this byte as MSB in the esU16, from which previous left-over byte was already added
		m_sum += static_cast<esU16>(*buff) << 8;
		++buff;
		--len;
		m_leftover = false;	
	}

	while( len > 1 )  
	{
		m_sum += *reinterpret_cast<const esU16*>(buff);
		buff += 2;
		len -= 2;
	}

	// optional 1 byte left-over
  if( len > 0 )
  {
  	m_sum += *buff;
		m_leftover = true;
	}
	
	m_dirty = true;
}

void EsChecksum16::update(cr_EsBinBuffer bb)
{
	if( !bb.empty() )
		update(&bb[0], bb.size());
}

void EsChecksum16::update(cr_EsBinBuffer bb, ulong from, ulong to)
{
	if( !bb.empty() )
	{
		if( from >= to )
			EsException::Throw(esT("EsChecksum16 update range is invalid"));

		EsNumericCheck::checkRangeUInteger(0, bb.size()-1, from, esT("from"));
		EsNumericCheck::checkRangeUInteger(from, bb.size(), to, esT("to"));
		update(&bb[from], to-from);
	}
}

esU16 EsChecksum16::get_value() const
{
	if( m_dirty )
	{
		m_checksum = _c_crc_impl_::checksum16FromSum(m_sum);
		m_dirty = false;
	}

	return m_checksum;
}

void EsChecksum16::set_buffer( const EsBinBuffer& bb )
{
	reset();
  update(&bb[0], bb.size());
}

// EsCRC16_CCIITT implementation
//
ES_DECL_BASE_CLASS_INFO_BEGIN(EsCRC16_CCIITT, NO_CLASS_DESCR)
	// properties
	ES_DECL_PROP_INFO_RO(EsCRC16_CCIITT, value, esU16, NO_PROPERTY_LABEL, esT("Current CRC accumulator value"))
	// reflected constructors
	ES_DECL_REFLECTED_CTOR_INFO(EsCRC16_CCIITT, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CTOR_INFO(EsCRC16_CCIITT, EsBaseIntfPtr_ClassCall_esU16, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CTOR_INFO(EsCRC16_CCIITT, EsBaseIntfPtr_ClassCall_cr_EsBinBuffer_esU16, NO_METHOD_DESCR)
	// reflected methods
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsCRC16_CCIITT, update, void_Call_cr_EsBinBuffer, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

// ctors
EsCRC16_CCIITT::EsCRC16_CCIITT(const EsBinBuffer& buff, esU16 initialSum /*= 0xFFFF*/) :
m_dynamic(false),
m_sum(initialSum)
{
	update(buff);
}

EsCRC16_CCIITT::EsCRC16_CCIITT(const esU8* buff /*= 0*/, size_t len /*= 0*/, esU16 initialSum /*= 0xFFFF*/) :
m_dynamic(false),
m_sum(initialSum)
{
	update(buff, len);
}

EsBaseIntfPtr EsCRC16_CCIITT::NEW() 
{ 
	EsCRC16_CCIITT* p = new EsCRC16_CCIITT();
	p->m_dynamic = true;
	return p->asBaseIntfPtrDirect();
}

EsBaseIntfPtr EsCRC16_CCIITT::NEW(esU16 initialSum) 
{ 
	EsCRC16_CCIITT* p = new EsCRC16_CCIITT(0, 0, initialSum);
	p->m_dynamic = true;
	return p->asBaseIntfPtrDirect();
}

EsBaseIntfPtr EsCRC16_CCIITT::NEW(cr_EsBinBuffer buff, esU16 initialSum) 
{ 
	EsCRC16_CCIITT* p = new EsCRC16_CCIITT(buff, initialSum);
	p->m_dynamic = true;
	return p->asBaseIntfPtrDirect();
}

// append buffer to the checksum calculations
void EsCRC16_CCIITT::update(const esU8* buff, size_t len)
{
	ES_ASSERT(buff);
	m_sum = _c_crc_impl_::crc16ccitt(m_sum, buff, len);
}

esU16 EsCRC16_CCIITT::get_value() const
{
	return m_sum;
}

// CRC32 implementation as of IEEE 802.3
//
ES_DECL_BASE_CLASS_INFO_BEGIN(EsCRC32_IEEE802_3, NO_CLASS_DESCR)
	// properties
	ES_DECL_PROP_INFO_RO(EsCRC32_IEEE802_3, value, esU32, NO_PROPERTY_LABEL, esT("Current CRC accumulator value"))
	// reflected constructors
	ES_DECL_REFLECTED_CTOR_INFO(EsCRC32_IEEE802_3, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CTOR_INFO(EsCRC32_IEEE802_3, EsBaseIntfPtr_ClassCall_esU32, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CTOR_INFO(EsCRC32_IEEE802_3, EsBaseIntfPtr_ClassCall_cr_EsBinBuffer_esU32, NO_METHOD_DESCR)
	// reflected methods
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsCRC32_IEEE802_3, update, void_Call_cr_EsBinBuffer, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

// ctors
EsCRC32_IEEE802_3::EsCRC32_IEEE802_3( const EsBinBuffer& buff, esU32 initialSum /*= 0xFFFFFFFF*/ ) :
m_initialSum(initialSum),
m_sum(initialSum)
{
	update( buff );
}

EsCRC32_IEEE802_3::EsCRC32_IEEE802_3( const esU8* buff /*= 0*/, size_t len /*= 0*/, esU32 initialSum /*= 0xFFFFFFFF*/ ) :
m_initialSum(initialSum),
m_sum(initialSum)
{
	update( buff, len );
}

/// Reset crc sum to default
void EsCRC32_IEEE802_3::reset()
{
  m_sum = m_initialSum;
}

/// Reset crc sum to specific value
void EsCRC32_IEEE802_3::reset(esU32 initialSum)
{
  m_initialSum = initialSum;
  m_sum = initialSum;
}

EsBaseIntfPtr EsCRC32_IEEE802_3::NEW()
{
	EsCRC32_IEEE802_3* p = new EsCRC32_IEEE802_3;
	p->m_dynamic = true;
	return p->asBaseIntfPtrDirect();
}

EsBaseIntfPtr EsCRC32_IEEE802_3::NEW(esU32 initialSum) 
{ 
	EsCRC32_IEEE802_3* p = new EsCRC32_IEEE802_3(0, 0, initialSum);
	p->m_dynamic = true;
	return p->asBaseIntfPtrDirect();
}

EsBaseIntfPtr EsCRC32_IEEE802_3::NEW(cr_EsBinBuffer buff, esU32 initalSum)
{ 
	EsCRC32_IEEE802_3* p = new EsCRC32_IEEE802_3(buff, initalSum);
	p->m_dynamic = true;
	return p->asBaseIntfPtrDirect();
}

// append buffer to crc sum calculation
void EsCRC32_IEEE802_3::update( const esU8* buff, size_t len )
{
	if( buff && len )
	  m_sum = _c_crc_impl_::crc32ieee802_3(m_sum, buff, len);
}

// current value access
esU32 EsCRC32_IEEE802_3::get_value() const
{
	return m_sum;
}