#ifndef _es_reg_ex_h_
#define _es_reg_ex_h_

#if defined(ES_USE_REGEX)

/// NB! ECC RegEx support uses utf-8 sting buffers internally, so any length and offset
/// values may|will differ from ones needed for built-in EsString type, if it
/// uses wide or semi-wide implementation and strings contain international symbols.
/// Beware!!!!
///

/// Non-reflected RegEx flags, to be used in native code.
/// Not all flags are supported in all Regex flavours
///
enum class EsRegExCompileFlag : ulong
{
  ICASE             = 0x00000001,
	SINGLE_LINE       = 0x00000002,
	NOSUB             = 0x00000004,
	MATCH_AT_START    = 0x00000008,
	UNGREEDY          = 0x00000010,
	IGNORE_WHITESPACE = 0x00000020,
  OPTIMIZE          = 0x00000040,
  DOTALL            = 0x00000080,
	DEFAULT           = 0,
};

enum class EsRegExMatchFlag : ulong
{
  NOT_BOL           = 0x00000001,
	NOT_EOL           = 0x00000002,
	NOT_NULL          = 0x00000004,
	DEFAULT           = EsRegExMatchFlag::NOT_NULL
};

enum class EsRegExReplaceFlag : ulong
{
  NOT_BOL           = 0x00000001,
	NOT_EOL           = 0x00000002,
	NOT_NULL          = 0x00000004,
	DEFAULT           = EsRegExReplaceFlag::NOT_NULL
};

/// Reflected regex
///
class ESCORE_CLASS ES_INTF_IMPL1(EsRegEx, EsReflectedClassIntf)

public:
	class ES_ABSTRACT Impl
	{
	public:
		virtual ~Impl() {}

		/// Compile regex and return true if OK, false otherwise
		virtual bool compile(const EsString &pattern, ulong flags) = 0;

		/// Get the start position and the length of the match of the expression (if subExprIdx is 0)
		/// or a bracketed subexpression (index different from 0).
    ///
		virtual bool matchGet(ulong& start, ulong& len, ulong subExprIdx) const = 0;

		/// Returns the size of the array of matches, i.e.
		/// the number of bracketed subexpressions plus one for the expression itself, or 0 on error.
		/// May only be called after successful call to compile(), and only if ES_RE_NOSUB was _not_ used.
    ///
		virtual ulong matchCountGet() const = 0;

		/// Return true if this is a valid compiled regular expression, false otherwise.
		virtual bool isOk() const = 0;

		/// Matches the precompiled regular expression against the string text, returns true if matches and false otherwise.
		/// offs argument should be set to non-zero, to skip portion of the string before match
    ///
		virtual bool matches(const EsString &text, ulong offs, ulong flags) const = 0;

		/// Replaces the current regular expression in the string 'text', with the text in replacement
		virtual EsString replace(const EsString& text, ulong offs, const EsString& replacement, ulong flags) const = 0;
	};

public:
	EsRegEx();
	EsRegEx(const EsString& patt, ulong flags = static_cast<ulong>(EsRegExCompileFlag::DEFAULT));
	virtual ~EsRegEx();

public:
	ES_DECL_REFLECTED_CLASS_BASE(EsRegEx)
	ES_DECL_ATTR_HANDLING_STD

	// direct (non-reflected) usage stuff
	bool isOk() const { return m_compiled; }
	bool matchGet(ulong& start, ulong& len, ulong subExprIdx) const;

	// reflected services
	//
	// ctors
	ES_DECL_REFLECTED_CLASS_CTOR0(EsVariant);
	ES_DECL_REFLECTED_CLASS_CTOR2(EsVariant, cr_EsString, ulong);
  
	/// Regex services
	///
  /// Compile regex
	ES_DECL_REFLECTED_METHOD2(bool, compile, cr_EsString, ulong);
	/// Two item array [start, end] of match is returned, or null otherwise
	ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, matchRangeGet, ulong);
	// Returns the part of string corresponding to the match where index is interpreted as above. 
	ES_DECL_REFLECTED_CONST_METHOD1(EsString, matchGet, ulong); 
	// Replaces the current regular expression in the string 'text', with the text in replacement
	ES_DECL_REFLECTED_METHOD1(EsString, replace, cr_EsString);
	ES_DECL_REFLECTED_METHOD2(EsString, replace, cr_EsString, ulong);

	// reflected properties
	ES_DECL_PROPERTY_RO(ok, bool)
	ES_DECL_PROPERTY_RO(matchCount, ulong)
	ES_DECL_PROPERTY_RO(matches, bool)
	ES_DECL_PROPERTY(pattern, EsString)
	ES_DECL_PROPERTY(text, EsString)
	ES_DECL_PROPERTY(offset, ulong)
	ES_DECL_PROPERTY(compileFlags, ulong)
	ES_DECL_PROPERTY(matchFlags, ulong)
	ES_DECL_PROPERTY(replaceFlags, ulong)

protected:
	Impl* m_impl;
	ulong m_compileFlags;
	ulong m_matchFlags;
	ulong m_replaceFlags;
	bool m_compiled;
	EsString m_patt;
	mutable EsString m_text;
	ulong m_offs;

private:
	// prohibited functionality
	EsRegEx(const EsRegEx&);
	EsRegEx& operator=(const EsRegEx&);
};

/// Reflected regex flags. Actual declaration is regex impl - dependent.
/// Not all flags are supported in all RegEx flavours.
///
namespace EsReflection
{
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCORE_CLASS, EsRegExCompileFlag)
	ES_DECL_ENUMERATION_ITEM(DEFAULT, static_cast<ulong>(::EsRegExCompileFlag::DEFAULT))
	ES_DECL_ENUMERATION_ITEM(ICASE, static_cast<ulong>(::EsRegExCompileFlag::ICASE))
	ES_DECL_ENUMERATION_ITEM(SINGLE_LINE, static_cast<ulong>(::EsRegExCompileFlag::SINGLE_LINE))
	ES_DECL_ENUMERATION_ITEM(NOSUB, static_cast<ulong>(::EsRegExCompileFlag::NOSUB))
	ES_DECL_ENUMERATION_ITEM(MATCH_AT_START, static_cast<ulong>(::EsRegExCompileFlag::MATCH_AT_START))
	ES_DECL_ENUMERATION_ITEM(UNGREEDY, static_cast<ulong>(::EsRegExCompileFlag::UNGREEDY))
	ES_DECL_ENUMERATION_ITEM(IGNORE_WHITESPACE, static_cast<ulong>(::EsRegExCompileFlag::IGNORE_WHITESPACE))
	ES_DECL_ENUMERATION_ITEM(OPTIMIZE, static_cast<ulong>(::EsRegExCompileFlag::OPTIMIZE))
ES_DECL_ENUMERATION_END

ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCORE_CLASS, EsRegExMatchFlag)
	ES_DECL_ENUMERATION_ITEM(DEFAULT, static_cast<ulong>(::EsRegExMatchFlag::DEFAULT))
	ES_DECL_ENUMERATION_ITEM(NOT_NULL, static_cast<ulong>(::EsRegExMatchFlag::NOT_NULL))
	ES_DECL_ENUMERATION_ITEM(NOT_BOL, static_cast<ulong>(::EsRegExMatchFlag::NOT_BOL))
	ES_DECL_ENUMERATION_ITEM(NOT_EOL, static_cast<ulong>(::EsRegExMatchFlag::NOT_EOL))
ES_DECL_ENUMERATION_END

ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCORE_CLASS, EsRegExReplaceFlag)
	ES_DECL_ENUMERATION_ITEM(DEFAULT, static_cast<ulong>(::EsRegExReplaceFlag::DEFAULT))
	ES_DECL_ENUMERATION_ITEM(NOT_NULL, static_cast<ulong>(::EsRegExReplaceFlag::NOT_NULL))
	ES_DECL_ENUMERATION_ITEM(NOT_BOL, static_cast<ulong>(::EsRegExReplaceFlag::NOT_BOL))
	ES_DECL_ENUMERATION_ITEM(NOT_EOL, static_cast<ulong>(::EsRegExReplaceFlag::NOT_EOL))
ES_DECL_ENUMERATION_END
}

#endif

#endif // _es_reg_ex_h_