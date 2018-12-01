#ifndef _es_comm_utils_h_
#define _es_comm_utils_h_

/// Byte read status
///
enum class EsByteReadStatus : ulong
{
  Expired,  ///< Timeout|retries expired, byte was not read
  Cancelled,///< Channel IO was cancelled
  Success   ///< Specified byte was read
};

ES_DECL_EXPORTED_REFLECTED_SERVICES_BEGIN(EsCommUtils, ESCOMM_CLASS)

  /// Try to read specific byte from channel. If nothing is read - retry to read char specified count of times.
  /// @param retries retries=0 is considered infinite.
  /// @param tmo is timeout in ms, used for single char read attempt.
  /// @return charReadStatus enumeration member
  ///
  static EsByteReadStatus specificByteReceive(const EsChannelIoIntf::Ptr& chnl, esU8 b, ulong retries, ulong tmo);

  /// Try to read specific byte pattern from channel. charRetries have the same meaning as above.
  /// @param charRetries charRetries=0 is considered infinite.
  /// @param tmo is timeout in ms, used for single char read attempt.
  /// @return true if specified pattern was read, false otherwise
  ///
  static bool binaryPatternReceive(const EsChannelIoIntf::Ptr& chnl, const esU8* pattern, ulong patternLen, ulong charRetries = 1, ulong tmo = 1);

  /// Try to read specific byte pattern from channel. charRetries have the same meaning as above.
  /// @param charRetries charRetries=0 is considered infinite.
  /// @param tmo is timeout in ms, used for single char read attempt.
  /// Return true if specified char was read, false otherwise.
  ///
  static bool binaryPatternReceive(const EsChannelIoIntf::Ptr& chnl, const EsBinBuffer& pattern, ulong charRetries = 1, ulong tmo = 1);

  /// Reflected functionality
  ///
  ES_DECL_REFLECTED_CLASS_METHOD4(EsVariant, specificByteReceiveReflected, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD4(EsVariant, binaryPatternReceiveReflected, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);

ES_DECL_REFLECTED_SERVICES_END
//---------------------------------------------------------------------------

namespace EsReflection
{

/// Byte read status reflected enumeration
///
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCOMM_CLASS, EsByteReadStatus)
  ES_DECL_ENUMERATION_ITEM(Expired,    static_cast<ulong>(::EsByteReadStatus::Expired))
  ES_DECL_ENUMERATION_ITEM(Cancelled, static_cast<ulong>(::EsByteReadStatus::Cancelled))
  ES_DECL_ENUMERATION_ITEM(Success,    static_cast<ulong>(::EsByteReadStatus::Success))
ES_DECL_ENUMERATION_END

}

#endif // _es_comm_utils_h_
