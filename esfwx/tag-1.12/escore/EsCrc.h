#ifndef _es_crc_h_
#define _es_crc_h_

// CRC 8, 16, 32 implementations.
//
// calculate CRC8 as of dallas-maxim 1-wire
//
class EKOSF_CORE_CLASS EsCRC8
{
public:
	// constructors
	EsCRC8(const EsBinBuffer& buff, BYTE initialSum = 0xFF);
	EsCRC8(const BYTE* buff = 0, size_t len = 0, BYTE initialSum = 0xFF);
	// append buffer to internal crc accumulator
	void update(const BYTE* buff, size_t len);
	inline void update(const EsBinBuffer& buff)
	{
		if( !buff.empty() )		
			update(&buff[0], buff.size());
	}
	// return CRC8 value
	inline BYTE getValue() const
	{
		return m_sum;
	}

private:
	BYTE m_sum;
};

// calculate checksum 16 as of RFC1071 (actually, it's IP _checksum_ algorithm)
// NB! checksum of checksum-ed buffer + checksum word is 0 ONLY for
// word-aligned buffers
//
class EKOSF_CORE_CLASS EsChecksum16
{
public:
	// ctors
	EsChecksum16(const EsBinBuffer& buff);
	EsChecksum16(const BYTE* buff = 0, size_t len = 0);
	// append buffer to the checksum calculations
	void append(const BYTE* buff, size_t len);
	inline void append(const EsBinBuffer& buff)
	{
		if( !buff.empty() )
			append(&buff[0], buff.size());
	}
	// return checksum value
	WORD getValue() const;

private:
	mutable bool m_dirty; // contents are dirty after append - need value recalc in get value
	bool m_leftover; // previously appended buffer had single leftover byte
	DWORD m_sum;
	mutable WORD m_crc; // cached crc value
};

// calculate crc16 CCIITT
//
class EKOSF_CORE_CLASS EsCrc16_CCIITT
{
public:
	// ctors
	EsCrc16_CCIITT(const EsBinBuffer& buff, WORD initialSum = 0xFFFF);
	EsCrc16_CCIITT(const BYTE* buff = 0, size_t len = 0, WORD initialSum = 0xFFFF);
	// append buffer to the checksum calculations
	void update(const BYTE* buff, size_t len);
	inline void update(const EsBinBuffer& buff)
	{
		if( !buff.empty() )
			update(&buff[0], buff.size());
	}
	// return checksum value
	WORD getValue() const
	{
		return m_sum;
	}

private:
	WORD m_sum;
};

// calculate CRC32 as of IEEE 802.3
//
class EKOSF_CORE_CLASS EsCRC32_IEEE802_3
{
public:
	// ctors
	EsCRC32_IEEE802_3( const EsBinBuffer& buff, DWORD initialSum = 0xFFFFFFFF );
	EsCRC32_IEEE802_3( const BYTE* buff = 0, size_t len = 0, DWORD initialSum = 0xFFFFFFFF );
	// append buffer to crc sum calculation
	void update( const BYTE* buff, size_t len );
	inline void update( const EsBinBuffer& buff )
	{
		if( !buff.empty() )
			update(&buff[0], buff.size());
	}
	// get CRC value
	inline DWORD getValue() const
	{
		return m_sum;
	}

private:
	static const DWORD c_table[256];
	DWORD m_sum;
};

#endif // _es_crc_h_