#include "escorepch.h"
#pragma hdrstop

#include "EsStringTokenizer.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#define ES_DEFAULT_SEPARATORS " \n\r\t"

ES_DECL_BASE_CLASS_INFO_BEGIN(EsStringTokenizer, NO_CLASS_DESCR)
	ES_DECL_REFLECTED_CTOR_INFO(EsStringTokenizer, EsVariant_ClassCall, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CTOR_INFO(EsStringTokenizer, EsVariant_ClassCall_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CTOR_INFO(EsStringTokenizer, EsVariant_ClassCall_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsStringTokenizer, rewind, void_Call, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsStringTokenizer, tokenGetAt, EsString_CallConst_ulong, NO_METHOD_DESCR)
	ES_DECL_PROP_INFO_RO(				EsStringTokenizer, moreTokens, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RO(				EsStringTokenizer, tokensCount, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RO(				EsStringTokenizer, remainingTokens, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RO(				EsStringTokenizer, nextToken, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO(					EsStringTokenizer, text, EsString, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO(					EsStringTokenizer, separators, EsString, NO_PROPERTY_LABEL, esVT(ES_DEFAULT_SEPARATORS), NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO(					EsStringTokenizer, skipMultipleSeparators, bool, NO_PROPERTY_LABEL, true, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END

EsStringTokenizer::EsStringTokenizer(const EsString& str, const EsString& separators /*= EsString::s_null*/) :
m_str(str),
m_seps(separators),
m_cnt(0),
m_skipMultipleSeparators(true)
{
	if( m_seps.empty() )
		m_seps = esT(ES_DEFAULT_SEPARATORS);

	tokenize();
	rewind();
}

bool EsStringTokenizer::get_moreTokens() const
{
	return m_it != m_tokens.end();
}

ulong EsStringTokenizer::get_tokensCount() const
{
	return m_tokens.size();
}

ulong EsStringTokenizer::get_remainingTokens() const
{
	return m_tokens.size()-m_cnt;
}

bool EsStringTokenizer::get_skipMultipleSeparators() const
{
  return m_skipMultipleSeparators;
}

void EsStringTokenizer::set_skipMultipleSeparators(const bool& skip)
{
  if( m_skipMultipleSeparators != skip )
  {
    m_skipMultipleSeparators = skip;
		tokenize();
		rewind();
  }
}

EsString EsStringTokenizer::get_nextToken() const
{
	if(m_it != m_tokens.end())
	{
		MapT::const_reference v = (*m_it++);
		++m_cnt;
    if( v.second )
		  return m_str.substr(v.first, v.second);
	}

	return EsString::null();
}

EsString EsStringTokenizer::tokenGetAt(ulong idx) const
{
	MapT::const_iterator cit = m_tokens.begin();
	std::advance(cit, idx);
	MapT::const_reference v = (*cit);

  if( v.second )
	  return m_str.substr(v.first, v.second);

	return EsString::null();
}

EsString EsStringTokenizer::get_text() const
{
	return m_str;
}

void EsStringTokenizer::set_text(const EsString& text)
{
	if( m_str != text )
	{
		m_str = text;
		tokenize();
		rewind();
	}
}

EsString EsStringTokenizer::get_separators() const
{
	return m_seps;
}

void EsStringTokenizer::set_separators(const EsString& seps)
{
	if( m_seps != seps )
	{
		m_seps = seps;
		tokenize();
		rewind();
	}
}

EsVariant EsStringTokenizer::NEW()
{
  return NEW(EsString::null(), EsString::null());
}

EsVariant EsStringTokenizer::NEW(cr_EsString str)
{
	return NEW(str, EsString::null());
}

EsVariant EsStringTokenizer::NEW(cr_EsString str, cr_EsString separators)
{
	std::unique_ptr<EsStringTokenizer> obj( new EsStringTokenizer(str, separators) );
	ES_ASSERT(obj.get());
	obj->m_dynamic = true;
	return obj.release()->asBaseIntfPtrDirect();
}

void EsStringTokenizer::rewind()
{
	m_it = m_tokens.begin();
	m_cnt = 0;
}

void EsStringTokenizer::tokenize()
{
	m_tokens.clear();
	if( !m_str.empty() )
	{
		if( m_seps.empty() ) // assign entire string as token, if 'seperators' is empty
			m_tokens[0] = m_str.size();
		else
		{
			// iterate string to find tokens and remember their positions & lengths in map
			size_t tokStart = 0;
			size_t tokEnd = EsString::npos;
			do
			{
				tokEnd = m_str.find_first_of(m_seps, tokStart);
				m_tokens[tokStart] = (EsString::npos == tokEnd) ? m_str.size() : tokEnd-tokStart;

				// skip to the next non-separator
				if( EsString::npos != tokEnd )
				{
					tokStart = tokEnd+1;

					while( tokStart < m_str.size() )
					{
						if( EsString::npos != m_seps.find( m_str[tokStart] ) )
            {
              if( !m_skipMultipleSeparators )
                break;

						  ++tokStart;
            }
						else
							break;
					}
				}

			} while( EsString::npos != tokEnd );
		}
	}
}