/// Do not include this file in project, or compile it directly
/// It's implicitly included from EsRegEx.cpp
///
#include <regex>

class EsRegExImplStd : public EsRegEx::Impl
{
public:
  typedef std::basic_regex<EsString::value_type> RegExT;
  typedef std::match_results<EsString::const_iterator> RegexMatchT;

public:
	EsRegExImplStd() :
  m_compiled(false),
  m_matched(false)
	{}

	virtual ~EsRegExImplStd()
	{}

	virtual inline bool compile(const EsString& pattern, ulong flags) ES_OVERRIDE
	{
		try
		{
      m_compiled = false;

      m_re.imbue(
        EsLocale::locale()
      );

      m_re.assign(
        pattern.begin(),
        pattern.end(),
        flagsToCompileOptions(flags)
      );

      m_matched = false;
      m_compiled = true;
		}
		catch(std::exception& ex)
		{
      EsString msg( ex.what() );
      EsException::Throw(
        0,
        EsException::severityGeneric,
        EsException::facilityEsCommon,
        msg.c_str()
      );
		}

		return m_compiled;
	}

  void internalMatch(ulong flags) const
  {
    if( !m_matched )
    {
      std::regex_constants::match_flag_type reflags = flagsToMatchOptions(flags);

      m_matched = std::regex_search(
        m_beg,
        m_end,
        m_matches,
        m_re,
        reflags
      );
    }
  }

	virtual inline bool matchGet(ulong& start, ulong& len, ulong subExprIdx) const ES_OVERRIDE
	{
    if( m_matched && subExprIdx < static_cast<ulong>(m_matches.size()) )
    {
      len = m_matches.length(subExprIdx);
      start = m_matches.position(subExprIdx);

      return 0 != len;
    }

    return false;
	}

	virtual inline ulong matchCountGet() const ES_OVERRIDE
	{
    if( m_matched )
      return m_matches.size();

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
		if( offs < text.size() )
		{
      m_beg = text.begin()+offs;
      m_end = text.end();
      internalMatch(flags);
		}

		return m_matched;
	}

	virtual inline EsString replace(const EsString& text, ulong offs, const EsString& replacement, ulong flags) const ES_OVERRIDE
	{
    ES_ASSERT(m_compiled);

		if( offs < text.size() )
		{
      m_beg = text.begin()+offs;
      m_end = text.end();

      EsString result;
      std::regex_replace(
        std::back_inserter(result),
        m_beg,
        m_end,
        m_re,
        static_cast<const EsBasicStringT&>(replacement),
        flagsToReplaceOptions(flags)
      );

      return result;
		}

		return EsString::null();
	}

  // option translation helper
  static inline RegExT::flag_type flagsToCompileOptions(ulong flags)
  {
    RegExT::flag_type opts = RegExT::ECMAScript;

    if( flags & static_cast<ulong>(EsRegExCompileFlag::ICASE) )
      opts |= RegExT::icase;

//    if( flags & static_cast<ulong>(EsRegExCompileFlag::SINGLE_LINE) )
//    {
//      opts >> System_Regularexpressionscore__1::preMultiLine;
//      opts << System_Regularexpressionscore__1::preSingleLine;
//    }

    if( flags & static_cast<ulong>(EsRegExCompileFlag::NOSUB) )
      opts |= RegExT::nosubs;
//    if( flags & static_cast<ulong>(EsRegExCompileFlag::MATCH_AT_START) )
//      opts |= System_Regularexpressionscore__1::preAnchored;
//    if( flags & static_cast<ulong>(EsRegExCompileFlag::UNGREEDY) )
//      opts |= RegExT:: System_Regularexpressionscore__1::preUnGreedy;
//    if( flags & static_cast<ulong>(EsRegExCompileFlag::IGNORE_WHITESPACE) )
//      opts << System_Regularexpressionscore__1::preExtended;

    if( flags & static_cast<ulong>(EsRegExCompileFlag::OPTIMIZE) )
      opts |= RegExT::optimize;

    return opts;
  }

  static inline std::regex_constants::match_flag_type flagsToMatchOptions(ulong flags)
  {
    std::regex_constants::match_flag_type opts = std::regex_constants::match_default;

    if( flags & static_cast<ulong>(EsRegExMatchFlag::NOT_BOL) )
      opts |= std::regex_constants::match_not_bol;
    if( flags & static_cast<ulong>(EsRegExMatchFlag::NOT_EOL) )
      opts |= std::regex_constants::match_not_eol;
    if( flags & static_cast<ulong>(EsRegExMatchFlag::NOT_NULL) )
      opts |= std::regex_constants::match_not_null;

    return opts;
  }

  static inline std::regex_constants::match_flag_type flagsToReplaceOptions(ulong flags)
  {
    std::regex_constants::match_flag_type opts = std::regex_constants::match_default;

    if( flags & static_cast<ulong>(EsRegExReplaceFlag::NOT_BOL) )
      opts |= std::regex_constants::match_not_bol;
    if( flags & static_cast<ulong>(EsRegExReplaceFlag::NOT_EOL) )
      opts |= std::regex_constants::match_not_eol;
    if( flags & static_cast<ulong>(EsRegExReplaceFlag::NOT_NULL) )
      opts |= std::regex_constants::match_not_null;

    return opts;
  }

protected:
	RegExT m_re;
  mutable RegexMatchT m_matches;
  mutable bool m_compiled;
  mutable bool m_matched;
  mutable EsString::const_iterator m_beg;
  mutable EsString::const_iterator m_end;
};
typedef EsRegExImplStd EsRegExImpl;
