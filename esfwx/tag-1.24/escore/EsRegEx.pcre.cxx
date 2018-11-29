/// Do not include this file in project, or compile it directly
/// It's implicitly included from EsRegEx.cpp
///
#define HAVE_CONFIG_H
#ifdef META_ESCAPE //< Defined in wingdi.h
# undef META_ESCAPE
#endif

#define PCRE2_EXP_DEFN
#include "pcre/src/config.h"
#include "pcre/src/pcre2.h"
#include "pcre/src/pcre2_chartables.c"
#include "pcre/src/pcre2_auto_possess.c"
#undef APTROWS
#undef APTCOLS
#include "pcre/src/pcre2_compile.c"
#undef NLBLOCK
#undef PSSTART
#undef PSEND
#include "pcre/src/pcre2_config.c"
#include "pcre/src/pcre2_context.c"
#include "pcre/src/pcre2_dfa_match.c"
#undef NLBLOCK
#undef PSSTART
#undef PSEND
#include "pcre/src/pcre2_error.c"
#include "pcre/src/pcre2_find_bracket.c"
#include "pcre/src/pcre2_jit_compile.c"
#include "pcre/src/pcre2_maketables.c"
#include "pcre/src/pcre2_match.c"
#include "pcre/src/pcre2_match_data.c"
#include "pcre/src/pcre2_newline.c"
#include "pcre/src/pcre2_ord2utf.c"
#include "pcre/src/pcre2_pattern_info.c"
#include "pcre/src/pcre2_serialize.c"
#include "pcre/src/pcre2_string_utils.c"
#include "pcre/src/pcre2_study.c"
#include "pcre/src/pcre2_substitute.c"
#include "pcre/src/pcre2_substring.c"
#include "pcre/src/pcre2_tables.c"
#include "pcre/src/pcre2_ucd.c"
#include "pcre/src/pcre2_valid_utf.c"
#include "pcre/src/pcre2_xclass.c"
#undef HAVE_CONFIG_H

class EsRegExImplPcre : public EsRegEx::Impl
{
public:
	EsRegExImplPcre() :
  m_reCode(nullptr),
  m_reMatch(nullptr),
  m_matchGroupCount(0),
  m_ovector(nullptr),
  m_ovectorCnt(0),
  m_compiled(false),
  m_matched(false),
  m_beg(nullptr),
  m_end(nullptr)
	{}

	virtual ~EsRegExImplPcre()
	{
    matchReset(nullptr);
    codeReset(nullptr);
  }

  void matchReset(pcre2_match_data* newMatch) const ES_NOTHROW
  {
    m_matched = false;
    m_ovector = nullptr;
    m_ovectorCnt = 0;

    if( m_reMatch )
    {
      pcre2_match_data_free( m_reMatch );
      m_reMatch = nullptr;
    }

    if( newMatch )
      m_reMatch = newMatch;
  }

  void codeReset(pcre2_code* newCode) const ES_NOTHROW
  {
    m_matched = false;
    m_compiled = false;

    if( m_reCode )
    {
      pcre2_code_free( m_reCode );
      m_reCode = nullptr;
    }

    if( newCode )
      m_reCode = newCode;
  }

  void pcreErrorGet(int errorCode, PCRE2_UCHAR* msg, size_t msgLen) const
  {
    pcre2_get_error_message(
      errorCode,
      msg,
      static_cast<PCRE2_SIZE>(msgLen)
    );
  }

  void pcreErrorThrow(const EsString& fmt, int errorCode, PCRE2_SIZE errorOffs) const
  {
    PCRE2_UCHAR error[256];
    pcreErrorGet(
      errorCode,
      error,
      sizeof(error)
    );

    // Do not use EsString formatter here, because it itself is based upon EsRegEx
    const EsString& msg = fmt + EsString( reinterpret_cast<EsString::const_pointer>(error) ) + esT(" at: ") + EsString::fromULong(errorOffs);

    EsException::Throw(
      errorCode,
      EsException::severityGeneric,
      EsException::facilityEsRegEx,
      msg.c_str()
    );
  }

	virtual inline bool compile(const EsString& pattern, ulong flags) ES_OVERRIDE
	{
    int errorCode = 0;
    PCRE2_SIZE errorOffs = 0;
    codeReset(
      pcre2_compile(
        reinterpret_cast<PCRE2_SPTR>(
          pattern.data()
        ),
        static_cast<PCRE2_SIZE>(pattern.size()),
        flagsToCompileOptions(flags),
        &errorCode,
        &errorOffs,
        nullptr
      )
    );

    if( !m_reCode )
      pcreErrorThrow(
        esT("EsRegEx compilation error: "),
        errorCode,
        errorOffs
      );

    m_compiled = true;

		return m_compiled;
	}

  void internalMatch(ulong flags) const
  {
    if( !m_matched )
    {
      ES_ASSERT(m_reCode);

      if( !m_reMatch )
      {
        m_matchGroupCount = 0;
        pcre2_pattern_info(
          m_reCode,
          PCRE2_INFO_CAPTURECOUNT,
          &m_matchGroupCount
        );

        matchReset(
          pcre2_match_data_create_from_pattern(
            m_reCode,
            nullptr
          )
        );
      }

      ES_ASSERT( m_reMatch );

      int errorCode = pcre2_match(
        m_reCode,
        reinterpret_cast<PCRE2_SPTR>(m_beg),
        static_cast<PCRE2_SIZE>(m_end-m_beg),
        0,
        flagsToMatchOptions(flags),
        m_reMatch,
        nullptr
      );

      if( errorCode < 0 )
      {
        if( 
          PCRE2_ERROR_NOMATCH == errorCode || 
          PCRE2_ERROR_PARTIAL == errorCode
        ) //< Still OK, though no match
          return;

        pcreErrorThrow(
          esT("EsRegEx match error: "),
          errorCode,
          0
        );
      }

      m_ovector = pcre2_get_ovector_pointer(
        m_reMatch
      );
      m_ovectorCnt = pcre2_get_ovector_count(
        m_reMatch
      ) * 2; //< In PCRE2 the number of pairs in vector got returned, so use plain count for our internal usage

      m_matched = true;
    }
  }

	virtual inline bool matchGet(ulong& start, ulong& len, ulong subExprIdx) const ES_OVERRIDE
	{
    if( m_matched && subExprIdx < m_matchGroupCount+1 )
    {
      ES_ASSERT(m_ovector);

      size_t idx = subExprIdx*2;

      ES_ASSERT(idx < m_ovectorCnt);
      ES_ASSERT(idx+1 < m_ovectorCnt);

      len = m_ovector[idx+1]-m_ovector[idx];
      start = m_ovector[idx];

      return 0 != len;
    }

    return false;
	}

	virtual inline ulong matchCountGet() const ES_OVERRIDE
	{
    if( m_matched )
      return m_matchGroupCount+1;

		return 0;
	}

	virtual inline bool isOk() const ES_OVERRIDE
	{
		return m_compiled;
	}

	virtual inline bool matches(const EsString& text, ulong offs, ulong flags) const ES_OVERRIDE
	{
    ES_ASSERT(m_compiled);

    m_matched = false;
    m_ovector = nullptr;
    m_ovectorCnt = 0;

    size_t len = text.size();
		if( offs < len )
		{
      m_beg = text.data();
      m_end = m_beg + len;
      m_beg += offs;

      internalMatch(flags);
		}

		return m_matched;
	}

	virtual inline EsString replace(const EsString& text, ulong offs, const EsString& replacement, ulong flags) const ES_OVERRIDE
	{
//    ES_DEBUG_TRACE(
//      esT("EsRegEx.pcre.replace(text='%s', offs=%d, replacement='%s', flags=%d)"),
//      text,
//      (int)offs,
//      replacement,
//      (int)flags
//    );
    ES_ASSERT(m_compiled);

		if( offs < text.size() )
		{
      ES_ASSERT(m_reCode);

      if( !m_reMatch )
      {
        m_matchGroupCount = 0;
        pcre2_pattern_info(
          m_reCode,
          PCRE2_INFO_CAPTURECOUNT,
          &m_matchGroupCount
        );

        matchReset(
          pcre2_match_data_create_from_pattern(
            m_reCode,
            nullptr
          )
        );
      }

      ES_ASSERT( m_reMatch );

      m_beg = text.data();
      m_end = m_beg + text.size();
      m_beg += offs;

      PCRE2_SIZE outlen = 0;

//      ES_DEBUG_TRACE(esT("Simulating replacement, calculating outlen"));
      int errorCode = pcre2_substitute(
        m_reCode,
        reinterpret_cast<PCRE2_SPTR>(m_beg),
        static_cast<PCRE2_SIZE>(m_end-m_beg),
        0,
        flagsToReplaceOptions(flags),
        m_reMatch,
        nullptr,
        reinterpret_cast<PCRE2_SPTR>(
          replacement.data()
        ),
        static_cast<PCRE2_SIZE>(replacement.size()),
        nullptr,
        &outlen
      );

      if( errorCode < 0 && PCRE2_ERROR_NOMEMORY != errorCode )
        pcreErrorThrow(
          esT("EsRegEx replace error: "),
          errorCode,
          0
        );

      if( 0 == errorCode ) //< No matches == no replacements
      {
//        ES_DEBUG_TRACE(esT("No replacements were made, returning a copy of input string"));

        return EsString(
          m_beg,
          m_end
        );
      }

      if( 0 == outlen )
      {
//        ES_DEBUG_TRACE(esT("An empty output string results from RegEx replacements"));

        return EsString::null();
      }

//      ES_DEBUG_TRACE(esT("Preparing output string of %d length"), (int)outlen);
      EsString result(
        outlen,
        static_cast<EsString::value_type>(0)
      );

//      ES_DEBUG_TRACE(esT("Running actual replacement"));
      errorCode = pcre2_substitute(
        m_reCode,
        reinterpret_cast<PCRE2_SPTR>(m_beg),
        static_cast<PCRE2_SIZE>(m_end-m_beg),
        0,
        flagsToReplaceOptions(flags),
        m_reMatch,
        nullptr,
        reinterpret_cast<PCRE2_SPTR>(
          replacement.c_str()
        ),
        static_cast<PCRE2_SIZE>(
          replacement.size()
        ),
        reinterpret_cast<PCRE2_UCHAR*>(
          result.data()
        ),
        &outlen
      );
      ES_ASSERT( errorCode >= 0 );
      ES_ASSERT( outlen > 0 );

      result.resize(outlen); //< Make sure we have proper character length after real replacement

      return result;
		}

		return EsString::null();
	}

  // option translation helper
  static inline uint32_t flagsToCompileOptions(ulong flags)
  {
    uint32_t opts = PCRE2_UTF | PCRE2_MULTILINE | PCRE2_NO_UTF_CHECK;

    if( flags & static_cast<ulong>(EsRegExCompileFlag::ICASE) )
      opts |= PCRE2_CASELESS;

    if( flags & static_cast<ulong>(EsRegExCompileFlag::SINGLE_LINE) )
      opts &= ~PCRE2_MULTILINE;

    if( flags & static_cast<ulong>(EsRegExCompileFlag::NOSUB) )
      opts |= PCRE2_NO_AUTO_CAPTURE;

    if( flags & static_cast<ulong>(EsRegExCompileFlag::MATCH_AT_START) )
      opts |= PCRE2_ANCHORED;

    if( flags & static_cast<ulong>(EsRegExCompileFlag::UNGREEDY) )
      opts |= PCRE2_UNGREEDY;

    if( flags & static_cast<ulong>(EsRegExCompileFlag::IGNORE_WHITESPACE) )
      opts |= PCRE2_EXTENDED;

    if( flags & static_cast<ulong>(EsRegExCompileFlag::DOTALL) )
      opts |= PCRE2_DOTALL;

    return opts;
  }

  static inline uint32_t flagsToMatchOptions(ulong flags)
  {
    uint32_t opts = PCRE2_PARTIAL_SOFT | PCRE2_NO_UTF_CHECK;

    if( flags & static_cast<ulong>(EsRegExMatchFlag::NOT_BOL) )
      opts |= PCRE2_NOTBOL;
    if( flags & static_cast<ulong>(EsRegExMatchFlag::NOT_EOL) )
      opts |= PCRE2_NOTEOL;
    if( flags & static_cast<ulong>(EsRegExMatchFlag::NOT_NULL) )
      opts |= PCRE2_NOTEMPTY;

    return opts;
  }

  static inline uint32_t flagsToReplaceOptions(ulong flags)
  {
    uint32_t opts = PCRE2_SUBSTITUTE_OVERFLOW_LENGTH | PCRE2_SUBSTITUTE_GLOBAL | PCRE2_NO_UTF_CHECK | PCRE2_SUBSTITUTE_EXTENDED;

    if( flags & static_cast<ulong>(EsRegExReplaceFlag::NOT_BOL) )
      opts |= PCRE2_NOTBOL;
    if( flags & static_cast<ulong>(EsRegExReplaceFlag::NOT_EOL) )
      opts |= PCRE2_NOTEOL;
    if( flags & static_cast<ulong>(EsRegExReplaceFlag::NOT_NULL) )
      opts |= PCRE2_NOTEMPTY;

    return opts;
  }

protected:
  mutable pcre2_code* m_reCode;
  mutable pcre2_match_data* m_reMatch;
  mutable uint32_t m_matchGroupCount;
  mutable PCRE2_SIZE* m_ovector;
  mutable uint32_t m_ovectorCnt;
  mutable bool m_compiled;
  mutable bool m_matched;
  mutable EsString::const_pointer m_beg;
  mutable EsString::const_pointer m_end;
};
typedef EsRegExImplPcre EsRegExImpl;