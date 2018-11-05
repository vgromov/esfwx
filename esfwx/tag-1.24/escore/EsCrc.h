#ifndef _es_crc_h_
#define _es_crc_h_

// CRC 8, 16, 32 implementations.
//
// calculate CRC8 as of dallas-maxim 1-wire
//
class ESCORE_CLASS ES_INTF_IMPL1(EsCRC8, EsReflectedClassIntf)

public:
	// constructors
	EsCRC8(const EsBinBuffer& buff, esU8 initialSum = 0xFF);
	EsCRC8(const esU8* buff = 0, size_t len = 0, esU8 initialSum = 0xFF);

	ES_DECL_REFLECTED_CLASS_BASE( EsCRC8 )
	ES_DECL_ATTR_HANDLING_STD

	// reflected constructors
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);
	ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, esU8);
	ES_DECL_REFLECTED_CLASS_CTOR2(EsBaseIntfPtr, cr_EsBinBuffer, esU8);

	// append buffer to internal crc accumulator
	void update(const esU8* buff, size_t len);
	ES_DECL_REFLECTED_METHOD1(void, update, cr_EsBinBuffer)
	{
		if( !p1.empty() )		
			update(&p1[0], p1.size());
	}
	// return CRC8 value
	ES_DECL_PROPERTY_RO(value, esU8)

private:
	esU8 m_sum;
};

// calculate checksum 16 as of RFC1071 (actually, it's IP _checksum_ algorithm)
// NB! checksum of checksum-ed buffer + checksum word is 0 ONLY for
// word-aligned buffers
//
class ESCORE_CLASS ES_INTF_IMPL1(EsChecksum16, EsReflectedClassIntf)

public:
	// ctors
	EsChecksum16(const EsBinBuffer& buff);
	EsChecksum16(const esU8* buff = 0, size_t len = 0);

	ES_DECL_REFLECTED_CLASS_BASE( EsChecksum16 )
	ES_DECL_ATTR_HANDLING_STD

	// reflected constructors
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);
	ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, cr_EsBinBuffer);

	// append buffer to the checksum calculations
	void update(const esU8* buff, size_t len);
  ES_DECL_REFLECTED_METHOD0(void, reset);
	ES_DECL_REFLECTED_METHOD1(void, update, cr_EsBinBuffer);
	ES_DECL_REFLECTED_METHOD3(void, update, cr_EsBinBuffer, ulong, ulong);

	// calculate sum from buffer, existing sum accumulator got reset
  ES_DECL_PROPERTY_WO(buffer, EsBinBuffer)
	// return checksum value
	ES_DECL_PROPERTY_RO(value, esU16)

private:
	bool m_dynamic;
	mutable bool m_dirty; // contents are dirty after update - need value recalc in get value
	bool m_leftover; // previously updated buffer had single leftover byte
	esU32 m_sum;
	mutable esU16 m_checksum; // cached crc value
};

// calculate crc16 CCIITT
//
class ESCORE_CLASS ES_INTF_IMPL1(EsCRC16_CCIITT, EsReflectedClassIntf)

public:
	// ctors
	EsCRC16_CCIITT(const EsBinBuffer& buff, esU16 initialSum = 0xFFFF);
	EsCRC16_CCIITT(const esU8* buff = 0, size_t len = 0, esU16 initialSum = 0xFFFF);

	ES_DECL_REFLECTED_CLASS_BASE( EsCRC16_CCIITT )
	ES_DECL_ATTR_HANDLING_STD

	// reflected constructors
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);
	ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, esU16);
	ES_DECL_REFLECTED_CLASS_CTOR2(EsBaseIntfPtr, cr_EsBinBuffer, esU16);

	// append buffer to the checksum calculations
	void update(const esU8* buff, size_t len);
	ES_DECL_REFLECTED_METHOD1(void, update, cr_EsBinBuffer)
	{
		if( !p1.empty() )
			update(&p1[0], p1.size());
	}
	// return crc value
	ES_DECL_PROPERTY_RO(value, esU16)

private:
	bool m_dynamic;
	esU16 m_sum;
};

// calculate CRC32 as of IEEE 802.3
//
class ESCORE_CLASS ES_INTF_IMPL1(EsCRC32_IEEE802_3, EsReflectedClassIntf)

public:
	// ctors
	EsCRC32_IEEE802_3( const EsBinBuffer& buff, esU32 initialSum = 0xFFFFFFFF );
	EsCRC32_IEEE802_3( const esU8* buff = 0, size_t len = 0, esU32 initialSum = 0xFFFFFFFF );

	ES_DECL_REFLECTED_CLASS_BASE( EsCRC32_IEEE802_3 )
	ES_DECL_ATTR_HANDLING_STD

	// reflected constructors
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);
	ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, esU32);
	ES_DECL_REFLECTED_CLASS_CTOR2(EsBaseIntfPtr, cr_EsBinBuffer, esU32);

  /// Reset crc sum to default
  void reset();

  /// Reset crc sum to specific value
  void reset(esU32 initialSum);

	// append buffer to crc sum calculation
	void update( const esU8* buff, size_t len );
	ES_DECL_REFLECTED_METHOD1(void, update, cr_EsBinBuffer)
	{
		if( !p1.empty() )
			update(&p1[0], p1.size());
	}
	// get CRC value
	ES_DECL_PROPERTY_RO(value, esU32)

private:
	bool m_dynamic;
  esU32 m_initialSum;
	esU32 m_sum;
};

#endif // _es_crc_h_