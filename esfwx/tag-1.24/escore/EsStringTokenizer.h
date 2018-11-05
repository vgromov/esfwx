#ifndef _es_tokenizer_h_
#define _es_tokenizer_h_

class ESCORE_CLASS ES_INTF_IMPL1(EsStringTokenizer, EsReflectedClassIntf)

public:
	EsStringTokenizer(const EsString& str, const EsString& separators = EsString::null());

	ES_DECL_REFLECTED_CLASS_BASE(EsStringTokenizer)
	ES_DECL_ATTR_HANDLING_STD

	// Reflected services
	//
  /// Default Ctor, create an empty tokenizer object, default set of ' \n\r\t' separators is used
 	ES_DECL_REFLECTED_CLASS_CTOR0(EsVariant);
	/// Ctor, given string to be tokenized, default set of ' \n\r\t' separators is used
	ES_DECL_REFLECTED_CLASS_CTOR1(EsVariant, cr_EsString);
	/// Ctor, given string to be tokenized, and string containing set of separators
	ES_DECL_REFLECTED_CLASS_CTOR2(EsVariant, cr_EsString, cr_EsString);
	/// Rewind tokenizer to the first token
	ES_DECL_REFLECTED_METHOD0(void, rewind);
	/// Indexed (slow) token access
	ES_DECL_REFLECTED_CONST_METHOD1(EsString, tokenGetAt, ulong);
	/// Reflected properties
	ES_DECL_PROPERTY_RO(moreTokens, bool)
	ES_DECL_PROPERTY_RO(tokensCount, ulong)
	ES_DECL_PROPERTY_RO(remainingTokens, ulong)
	ES_DECL_PROPERTY_RO(nextToken, EsString)
	ES_DECL_PROPERTY(text, EsString)
	ES_DECL_PROPERTY(separators, EsString)
  ES_DECL_PROPERTY(skipMultipleSeparators, bool)

private:
	void tokenize();

protected:
	EsString m_str;
	EsString m_seps;
	mutable size_t m_cnt;
  bool m_skipMultipleSeparators;
	// token lengths, mapped by its positions
	// it's automatically sorted in
	typedef std::map<ulong, ulong> MapT;
	mutable MapT::const_iterator m_it;
	MapT m_tokens;

private:
	// prohibited functionality
  EsStringTokenizer() ES_REMOVEDECL;
	EsStringTokenizer(const EsStringTokenizer&) ES_REMOVEDECL;
	EsStringTokenizer& operator=(const EsStringTokenizer&) ES_REMOVEDECL;
};

#endif // _es_tokenizer_h_