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
  EsCRC8(const esU8* buff = nullptr, ulong len = 0, esU8 initialSum = 0xFF);

  ES_DECL_REFLECTED_CLASS_BASE( EsCRC8 )
  ES_DECL_ATTR_HANDLING_STD

  // Reflected constructors
  //
  
  /// @brief Default reflected constructor.
  /// 
  /// @return An empty CRC8 object, with zero initial value.
  ///
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);
  
  /// @brief Reflected constructor with initial CRC value.
  /// 
  /// @return A CRC8 object, with initial CRC value set to constructor argument.
  ///
  ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, esU8);

  /// @brief Reflected constructor, taking data buffer, and an initial CRC value.
  /// 
  /// @return A CRC8 object, with pre-calculated CRC, using provided data buffer, and initial CRC value.
  ///
  ES_DECL_REFLECTED_CLASS_CTOR2(EsBaseIntfPtr, cr_EsBinBuffer, esU8);

  /// @brief Reflected constructor, taking data buffer, an initial CRC value, and 
  ///        from-to position range to pre-calculate a CRC value on.
  /// 
  /// @return A CRC8 object, with pre-calculated CRC, using provided data buffer, 
  ///         an initial CRC value, and specified from-to range to select data from
  ///         a provided data buffer.
  ///
  ES_DECL_REFLECTED_CLASS_CTOR4(EsBaseIntfPtr, cr_EsBinBuffer, esU8, ulong, ulong);

  /// Reset CRC to initial value
  ES_DECL_REFLECTED_METHOD0(void, reset);

  /// Reset CRC to specific value
  ES_DECL_REFLECTED_METHOD1(void, reset, esU8);

  /// Append buffer to internal crc accumulator
  void update(const esU8* buff, ulong len);
  ES_DECL_REFLECTED_METHOD1(void, update, cr_EsBinBuffer);
  ES_DECL_REFLECTED_METHOD3(void, update, cr_EsBinBuffer, ulong, ulong);

  /// Calculate CRC from buffer, existing sum accumulator got reset
  ES_DECL_PROPERTY_WO(buffer, EsBinBuffer)
  
  /// Return CRC8 value
  ES_DECL_PROPERTY_RO(value, esU8)

private:
  esU8 m_init;
  esU8 m_sum;
};

// calculate checksum 16 as of RFC1071 (actually, it's IP _checksum_ algorithm)
// NB! checksum of checksum-ed buffer + checksum word is 0 ONLY for
// word-aligned buffers
//
class ESCORE_CLASS ES_INTF_IMPL1(EsChecksum16, EsReflectedClassIntf)

public:
  // ctors
  EsChecksum16(const EsBinBuffer& buff, esU16 initialSum = 0);
  EsChecksum16(const esU8* buff = nullptr, ulong len = 0, esU16 initialSum = 0);

  ES_DECL_REFLECTED_CLASS_BASE( EsChecksum16 )
  ES_DECL_ATTR_HANDLING_STD

  // reflected constructors
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);
  ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, cr_EsBinBuffer);
  ES_DECL_REFLECTED_CLASS_CTOR2(EsBaseIntfPtr, cr_EsBinBuffer, esU16);
  ES_DECL_REFLECTED_CLASS_CTOR4(EsBaseIntfPtr, cr_EsBinBuffer, esU16, ulong, ulong);

  ES_DECL_REFLECTED_METHOD0(void, reset);
  ES_DECL_REFLECTED_METHOD1(void, reset, esU16);

  /// Append buffer to the checksum calculations
  void update(const esU8* buff, ulong len);
  ES_DECL_REFLECTED_METHOD1(void, update, cr_EsBinBuffer);
  ES_DECL_REFLECTED_METHOD3(void, update, cr_EsBinBuffer, ulong, ulong);

  /// Calculate sum from buffer, existing sum accumulator got reset
  ES_DECL_PROPERTY_WO(buffer, EsBinBuffer)

  /// Return checksum value
  ES_DECL_PROPERTY_RO(value, esU16)

private:
  esU32 m_sum;              //< Current checksum
  esU16 m_init;             //< Initial checksum
  mutable esU16 m_checksum; //< Cached checkdum value
  mutable bool m_dirty;     //< contents are dirty after update - need value recalc in get value
  bool m_leftover;          //< previously updated buffer had single leftover byte
};

// calculate crc16 CCIITT
//
class ESCORE_CLASS ES_INTF_IMPL1(EsCRC16_CCIITT, EsReflectedClassIntf)

public:
  // ctors
  EsCRC16_CCIITT(const EsBinBuffer& buff, esU16 initialSum = 0xFFFF);
  EsCRC16_CCIITT(const esU8* buff = nullptr, ulong len = 0, esU16 initialSum = 0xFFFF);

  ES_DECL_REFLECTED_CLASS_BASE( EsCRC16_CCIITT )
  ES_DECL_ATTR_HANDLING_STD

  // reflected constructors
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);
  ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, esU16);
  ES_DECL_REFLECTED_CLASS_CTOR2(EsBaseIntfPtr, cr_EsBinBuffer, esU16);
  ES_DECL_REFLECTED_CLASS_CTOR4(EsBaseIntfPtr, cr_EsBinBuffer, esU16, ulong, ulong);

  /// Reset CRC to initial value
  ES_DECL_REFLECTED_METHOD0(void, reset);

  /// Reset CRC to specific value
  ES_DECL_REFLECTED_METHOD1(void, reset, esU16);
  
  /// Append buffer to the checksum calculations
  void update(const esU8* buff, ulong len);
  ES_DECL_REFLECTED_METHOD1(void, update, cr_EsBinBuffer);
  ES_DECL_REFLECTED_METHOD3(void, update, cr_EsBinBuffer, ulong, ulong);

  /// Calculate CRC from buffer, existing sum accumulator got reset
  ES_DECL_PROPERTY_WO(buffer, EsBinBuffer)

  /// Return crc value
  ES_DECL_PROPERTY_RO(value, esU16)

private:
  esU16 m_init;
  esU16 m_sum;
};

// calculate CRC32 as of IEEE 802.3
//
class ESCORE_CLASS ES_INTF_IMPL1(EsCRC32_IEEE802_3, EsReflectedClassIntf)

public:
  // ctors
  EsCRC32_IEEE802_3( const EsBinBuffer& buff, esU32 initialSum = 0xFFFFFFFF );
  EsCRC32_IEEE802_3( const esU8* buff = nullptr, ulong len = 0, esU32 initialSum = 0xFFFFFFFF );

  ES_DECL_REFLECTED_CLASS_BASE( EsCRC32_IEEE802_3 )
  ES_DECL_ATTR_HANDLING_STD

  // reflected constructors
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);
  ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, esU32);
  ES_DECL_REFLECTED_CLASS_CTOR2(EsBaseIntfPtr, cr_EsBinBuffer, esU32);
  ES_DECL_REFLECTED_CLASS_CTOR4(EsBaseIntfPtr, cr_EsBinBuffer, esU32, ulong, ulong);

  /// Reset crc sum to default
  ES_DECL_REFLECTED_METHOD0(void, reset);

  /// Reset crc sum to specific value
  ES_DECL_REFLECTED_METHOD1(void, reset, esU32);

  /// Append buffer to crc sum calculation
  void update( const esU8* buff, ulong len );
  ES_DECL_REFLECTED_METHOD1(void, update, cr_EsBinBuffer);
  ES_DECL_REFLECTED_METHOD3(void, update, cr_EsBinBuffer, ulong, ulong);

  /// Calculate CRC from buffer, existing sum accumulator got reset
  ES_DECL_PROPERTY_WO(buffer, EsBinBuffer)
  
  /// Get CRC value
  ES_DECL_PROPERTY_RO(value, esU32)

private:
  esU32 m_init;
  esU32 m_sum;
};

#endif // _es_crc_h_