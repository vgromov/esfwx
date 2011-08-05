#include <escore/grammar_defs.h>

#ifdef _DEBUG

// comment out the following define BOOST_SPIRIT_DEBUG to turn off debug text dump
//#	define BOOST_SPIRIT_DEBUG
#	ifdef BOOST_SPIRIT_DEBUG
#	define BOOST_SPIRIT_DEBUG_PRINT_SOME 64
#		include <fstream>
		GRAMMAR_DEF_DEBUG_DUMP_DEST(EsDictionaryCompiler)
#		define BOOST_SPIRIT_DEBUG_OUT		EsDictionaryCompilerDumpDest
// comment out the following to exclude debug xml parsetree dump
#		define BOOST_SPIRIT_DUMP_PARSETREE_AS_XML
#	endif // BOOST_SPIRIT_DEBUG
#endif

// use fast implementation of boost pool objects allocator for tree nodes
// if no CRT memleak detection is active
#ifndef USE_MEMLEAK_DETECTION
#	define BOOST_SPIRIT_USE_BOOST_ALLOCATOR_FOR_TREES
#endif
// avoid compilation error on BCC32
//#define BOOST_NO_IS_ABSTRACT
// spirit-boost includes
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_error_handling.hpp>
#include <boost/spirit/include/classic_ast.hpp>
#include <boost/spirit/include/classic_confix.hpp>
#include <boost/spirit/include/classic_lists.hpp>
#include <boost/spirit/include/classic_escape_char.hpp>
#include <boost/spirit/include/classic_if.hpp>

using namespace boost::spirit::classic;

#	ifdef BOOST_SPIRIT_DUMP_PARSETREE_AS_XML
#		include <boost/spirit/include/classic_tree_to_xml.hpp>
template<typename CharT, typename TreeNodeT, typename AssocContainerT>
static inline void my_tree_to_xml( std::basic_ofstream<CharT>& out, const TreeNodeT& tree, const std::basic_string<CharT> input, const AssocContainerT& names )
{
	basic_tree_to_xml(out, tree, input, names);
}
#	endif

// context prerequisites
PARSER_CONTEXT_PREREQUISITES_BEGIN( EsDictionaryCompiler, node_iter_data_factory<> )
	GRAMMAR_RULE_IDS_BEGIN
		GRAMMAR_DEF_RULE_ID(skip)
		GRAMMAR_DEF_RULE_ID(boolVal)
		GRAMMAR_DEF_RULE_ID(intVal)
		GRAMMAR_DEF_RULE_ID(floatVal)
		GRAMMAR_DEF_RULE_ID(strVal)
		GRAMMAR_DEF_RULE_ID(simpleVal)
		GRAMMAR_DEF_RULE_ID(arrayVal)
		GRAMMAR_DEF_RULE_ID(value)
		GRAMMAR_DEF_RULE_ID(key)
		GRAMMAR_DEF_RULE_ID(keyAssign)
		GRAMMAR_DEF_RULE_ID(dicName)
		GRAMMAR_DEF_RULE_ID(dictionary)
	GRAMMAR_RULE_IDS_END
PARSER_CONTEXT_PREREQUISITES_END

// grammar parser
GRAMMAR_PARSER_STD_GRAMMAR_IMPL_BEGIN( EsDictionaryCompiler )
	// named char literals
#	define SEMICOLON	ch_p(wxT(';'))
#	define COMMA			ch_p(wxT(','))
#	define DBLQUOTE		ch_p(wxT('"'))
#	define LCBRACKET	ch_p(wxT('{'))
#	define RCBRACKET	ch_p(wxT('}'))
#	define LSBRACKET	ch_p(wxT('['))
#	define RSBRACKET	ch_p(wxT(']'))
#	define UNDERSCORE	ch_p(wxT('_'))
#	define EQ					ch_p(wxT('='))
	// string literals
	//
	// hex prefix
#	define HEXPFX			str_p(wxT("0x"))
	// reserved words
#	define BTRUE			str_p(wxT("true"))
#	define BFALSE			str_p(wxT("false"))
	// rules
	GRAMMAR_DEF_RULE(boolVal)
	GRAMMAR_DEF_RULE(intVal)
	GRAMMAR_DEF_RULE(floatVal)
	GRAMMAR_DEF_RULE(strVal)
	GRAMMAR_DEF_RULE(simpleVal)
	GRAMMAR_DEF_RULE(arrayVal)
	GRAMMAR_DEF_RULE(value)
	GRAMMAR_DEF_RULE(key)
	GRAMMAR_DEF_RULE(keyAssign)
	GRAMMAR_DEF_RULE(dicName)
	GRAMMAR_DEF_RULE(dictionary)
	// grammar entry point
	GRAMMAR_DEF_ENTRY(dictionary)
	
	GRAMMAR_PARSER_DEFINITION_IMPL_BEGIN
		// error support
		GRAMMAR_PARSER_USE_ASSERT_GUARD
		GRAMMAR_ASSERT_DEF_EXPECT(SEMICOLON)
		GRAMMAR_ASSERT_DEF_EXPECT(RCBRACKET)
		GRAMMAR_ASSERT_DEF_EXPECT(RSBRACKET)
		GRAMMAR_ASSERT_DEF_EXPECT(EQ)
		GRAMMAR_ASSERT_DEF_EXPECT(value)

		// values
		boolVal = 
			BTRUE | 
			BFALSE;
			
		intVal	= 
			if_p(as_lower_d[ HEXPFX ])[
				hex_p
			].else_p[
				int_p
			];
		
		floatVal = longest_d[
			strict_real_p |
			strict_ureal_p
			];
		
		strVal = lexeme_d[leaf_node_d[
			DBLQUOTE >> 
				*( 
					c_escape_ch_p -	
					DBLQUOTE 
					)	>> 
			DBLQUOTE
      ]];
  
    simpleVal = 
			strVal |
			boolVal |
			floatVal |			
			intVal;
			
		arrayVal = inner_node_d[ 
			LCBRACKET >> list_p(value, no_node_d[COMMA]) >> GRAMMAR_EXPECT(RCBRACKET) 
			];
		
		value = 
			simpleVal |
			arrayVal;
		
		// value key & its assignment
		key = lexeme_d[leaf_node_d[
			(alpha_p | UNDERSCORE) >>	*(alnum_p | UNDERSCORE)
			]];
			
		keyAssign = 
			key >> root_node_d[GRAMMAR_EXPECT(EQ)] >> GRAMMAR_EXPECT(value) 
				>> no_node_d[ GRAMMAR_EXPECT(SEMICOLON) ]
		;
		
		// dictionary name
		dicName = 
			no_node_d[LSBRACKET] >>
				lexeme_d[leaf_node_d[
					(alpha_p | UNDERSCORE) >>	*(alnum_p | UNDERSCORE)
				]] >>
			no_node_d[ GRAMMAR_EXPECT(RSBRACKET) ] 
			>> no_node_d[ GRAMMAR_EXPECT(SEMICOLON) ]
		;
		
		// entry point
		dictionary = GRAMMAR_PARSER_ASSERT_GUARD(
			dicName || *keyAssign
			);
		
#		ifdef BOOST_SPIRIT_DEBUG
		BOOST_SPIRIT_DEBUG_RULE(boolVal);
		BOOST_SPIRIT_DEBUG_RULE(intVal);
		BOOST_SPIRIT_DEBUG_RULE(floatVal);
		BOOST_SPIRIT_DEBUG_RULE(strVal);
		BOOST_SPIRIT_DEBUG_RULE(simpleVal);
		BOOST_SPIRIT_DEBUG_RULE(arrayVal);
		BOOST_SPIRIT_DEBUG_RULE(value);
		BOOST_SPIRIT_DEBUG_RULE(key);
		BOOST_SPIRIT_DEBUG_RULE(keyAssign);
		BOOST_SPIRIT_DEBUG_RULE(dicName);
		BOOST_SPIRIT_DEBUG_RULE(dictionary);
#		endif	
GRAMMAR_PARSER_IMPL_END

// skip parser
GRAMMAR_PARSER_STD_SKIP_IMPL_BEGIN( EsDictionaryCompiler )
	GRAMMAR_DEF_RULE(skip)
	// grammar entry point
	GRAMMAR_DEF_ENTRY(skip)
	GRAMMAR_PARSER_DEFINITION_IMPL_BEGIN
		skip = no_node_d[
			space_p |
			comment_p( wxT("//") ) |
			comment_p( wxT("/*"), wxT("*/") )
			];
				
#	ifdef BOOST_SPIRIT_DEBUG
		BOOST_SPIRIT_DEBUG_RULE(skip);
#	endif					
GRAMMAR_PARSER_IMPL_END

// parser context impl
PARSER_CONTEXT_IMPL_BEGIN(EsDictionaryCompiler)
	// error ids
	PARSER_CONTEXT_DEF_ERROR_BEGIN
		PARSER_CONTEXT_DEF_EXPECT(SEMICOLON)
		PARSER_CONTEXT_DEF_EXPECT(RCBRACKET)
		PARSER_CONTEXT_DEF_EXPECT(RSBRACKET)
		PARSER_CONTEXT_DEF_EXPECT(EQ)
		PARSER_CONTEXT_DEF_EXPECT(value)
	PARSER_CONTEXT_DEF_ERROR_END
	PARSER_CONTEXT_DEF_ERROR_DESCR_BEGIN
		PARSER_CONTEXT_DEF_EXPECT_DESCR(SEMICOLON,	_("Expected ;"))
		PARSER_CONTEXT_DEF_EXPECT_DESCR(RCBRACKET,	_("Expected }"))
		PARSER_CONTEXT_DEF_EXPECT_DESCR(RSBRACKET,	_("Expected ]"))
		PARSER_CONTEXT_DEF_EXPECT_DESCR(EQ,					_("Expected ="))
		PARSER_CONTEXT_DEF_EXPECT_DESCR(value,			_("Expected integer, float, string, or array value"))
	PARSER_CONTEXT_DEF_ERROR_DESCR_END
	PARSER_CONTEXT_PARSE_BEGIN
#	ifdef BOOST_SPIRIT_DUMP_PARSETREE_AS_XML	
		// dump parse tree as XML
		GRAMMAR_XML_DUMP_MAP_BEGIN
			GRAMMAR_XML_DUMP_MAP_ENTRY(boolVal)
			GRAMMAR_XML_DUMP_MAP_ENTRY(intVal)
			GRAMMAR_XML_DUMP_MAP_ENTRY(floatVal)
			GRAMMAR_XML_DUMP_MAP_ENTRY(strVal)
			GRAMMAR_XML_DUMP_MAP_ENTRY(simpleVal)
			GRAMMAR_XML_DUMP_MAP_ENTRY(arrayVal)
			GRAMMAR_XML_DUMP_MAP_ENTRY(value)
			GRAMMAR_XML_DUMP_MAP_ENTRY(key)
			GRAMMAR_XML_DUMP_MAP_ENTRY(keyAssign)
			GRAMMAR_XML_DUMP_MAP_ENTRY(dicName)
			GRAMMAR_XML_DUMP_MAP_ENTRY(dictionary)		
			GRAMMAR_XML_DUMP_MAP_ENTRY(skip)
		GRAMMAR_XML_DUMP_MAP_END(EsDictionaryCompiler)		
#	endif
	PARSER_CONTEXT_PARSE_ERROR_HANDLER	
		// parsing error handling goes here, error position is passed 
		// in pos parameter, its identifier goes in id parameter
		EsException::Throw(0, EsException::Generic,
			_("EsDictionary parsing error at position %d; Reason: '%s'\n"), 
			pos-m_input.c_str(), getErrorDescr(id).c_str());
	PARSER_CONTEXT_PARSE_END
	// EsDictionary compilation using existing m_parseInfo
	// main compilation entry point
	void compileTo(EsDictionary& dic, const std::locale& loc) const
	{
		// traverse through ast, compiling its nodes depending on id
		ParseTreeConstIteratorT node = m_parseInfo.trees.begin();
		while( node != m_parseInfo.trees.end() )
		{
			compileTopNode( node, dic, loc );
			++node;
		}
	}
private:
	// node compiler
	void compileTopNode(ParseTreeConstIteratorT node, EsDictionary& dic, const std::locale& loc) const
	{
		int id = node->value.id().to_long();
		switch( id )
		{
		case dicNameID:
			compileDictionaryName( node, dic );
			break;
		case keyAssignID:
			compileKeyAssignment( node, dic, loc );
			break;
		case dictionaryID:
			// dive into dicitonary root node
			for( ParseTreeConstIteratorT child = node->children.begin(); child != node->children.end(); ++child )
				compileTopNode( child, dic, loc );
			break;
		default:
			// we should never be here
			wxASSERT(false);
			break;
		}
	}
	// key assignment compiler
	void compileKeyAssignment(ParseTreeConstIteratorT node, EsDictionary& dic, const std::locale& loc) const
	{		
		wxASSERT(keyAssignID == node->value.id().to_long());
		wxASSERT(2 == node->children.size());
		// get key
		EsString key = compileKey( node->children[0] );
		// get value
		EsVariant val = compileValue( node->children[1], loc );
		// update dictionary
		dic.setValue(key, val, true);
	}
	// dictionary name node compiler
	void compileDictionaryName(ParseTreeConstIteratorT node, EsDictionary& dic) const
	{
		wxASSERT(dicNameID == node->value.id().to_long());
		wxASSERT(0 == node->children.size());
		dic.setName( EsString(node->value.begin(), node->value.end()) );
	}
	// compile key name
	EsString compileKey(ParseTreeConstIteratorT::reference node) const
	{
		wxASSERT(keyID == node.value.id().to_long());
		return EsString(node.value.begin(), node.value.end());
	}
	// compile value
	EsVariant compileValue(ParseTreeConstIteratorT::reference node, const std::locale& loc) const
	{
		int id = node.value.id().to_long();
		wxASSERT( boolValID <= id && valueID >= id );
		if( simpleValID > id )
			return compileSimpleValue(node, loc);
		else if( arrayValID == id )
			return compileArrayValue(node, loc);

		// we should never be here
		wxASSERT(false);
		return EsVariant::s_null;
	}
	// compile simple value
	EsVariant compileSimpleValue(ParseTreeConstIteratorT::reference node, const std::locale& loc) const
	{
		int id = node.value.id().to_long();
		wxASSERT( boolValID <= id && simpleValID > id );
		EsString data(node.value.begin(), node.value.end());
		EsVariant var( data );
		
	switch( id )
	{
	case boolValID:
		return var.asBool(loc);
	case intValID:
		return var.asLong(loc);			
	case floatValID:
		return var.asDouble(loc);
	case strValID:
		var = EsString::fromString( data, EsString::StrQuote, loc );
		return var;
	default:
		// we should never be here
		wxFAIL;
		break;
	}

	return EsVariant::s_null;
	}
	// compile array value
	EsVariant compileArrayValue(ParseTreeConstIteratorT::reference node, const std::locale& loc) const
	{
		EsVariant result(EsVariant::VAR_VARIANT_COLLECTION);
		int id = node.value.id().to_long();
		wxASSERT( arrayValID == id );
		ParseTreeConstIteratorT	child = node.children.begin();
		while( child != node.children.end() )
		{
			result.addToVariantCollection( compileValue( *child, loc ) );
			++child;
		}

		return result;
	}
PARSER_CONTEXT_IMPL_END