#ifndef _grammar_defs_h_
#define _grammar_defs_h_

// boost spirit grammar definition helper macros
//
// define some prerequisites
#define PARSER_CONTEXT_PREREQUISITES_BEGIN( ContextName, NodeDataFactoryClass ) \
	struct ContextName; \
	struct ContextName ## Base { \
		typedef NodeDataFactoryClass NodeDataFactoryT;
#define PARSER_CONTEXT_PREREQUISITES_END };
// top parser context defines
#define PARSER_CONTEXT_IMPL_BEGIN( ContextName ) \
	struct ContextName : public ContextName ## Base { \
		typedef ContextName ## Grammar GrammarT; \
		typedef ContextName ## Skip SkipGrammarT; \
		typedef tree_parse_info< EsString::const_pointer, NodeDataFactoryT > ParseTreeInfoT; \
		typedef tree_match< EsString::const_pointer, NodeDataFactoryT > ParseTreeMatchT; \
		typedef ParseTreeMatchT::tree_iterator ParseTreeIteratorT; \
		typedef ParseTreeMatchT::const_tree_iterator ParseTreeConstIteratorT; \
		typedef ParseTreeMatchT::node_t ParseTreeNodeT; \
		struct ErrorHandler { \
			ContextName& m_ctx; \
			ErrorHandler( ContextName& ctx ) : m_ctx( ctx ) {} \
			template <typename ScannerT, typename ErrorT> \
			error_status<> \
			operator()(ScannerT const& scan, ErrorT const& error) const { \
				m_ctx.setError(error.where, error.descriptor); \
				return error_status<>(error_status<>::fail); } } m_errHandler; \
		ParseTreeInfoT m_parseInfo;	\
		const EsString& m_input; \
		GrammarT m_grammar; \
		SkipGrammarT m_skipGrammar; \
		ContextName( const EsString& input) : m_grammar(*this), m_skipGrammar(*this), m_errHandler(*this), m_input(input) {
// error tags definition macros. must be included before PARSER_CONTEXT_PARSE_BEGIN
#define PARSER_CONTEXT_DEF_ERROR_BEGIN } enum ErrorIDs {
#define PARSER_CONTEXT_DEF_EXPECT( Name ) Expect_ ## Name,
#define PARSER_CONTEXT_DEF_ERROR( Name ) Error_ ## Name,
#define PARSER_CONTEXT_DEF_ERROR_END	ErrorIDsCount };
// error description string getter definition
#define PARSER_CONTEXT_DEF_ERROR_DESCR_BEGIN \
	static EsString getErrorDescr( ErrorIDs id ) { \
		switch( id ) {
#define PARSER_CONTEXT_DEF_EXPECT_DESCR( Name, Descr ) \
		case Expect_ ## Name: \
			return Descr;
#define PARSER_CONTEXT_DEF_ERROR_DESCR( Name, Descr ) \
		case Error_ ## Name: \
			return Descr;
#define PARSER_CONTEXT_DEF_ERROR_DESCR_END \
		default: \
			return _("Unknown parser error.");	}; }
// begin parser entry point block
#define PARSER_CONTEXT_PARSE_BEGIN \
	typedef guard< ErrorIDs > AssertionGuardT; \
	bool doParse() { \
		m_parseInfo = ast_parse<NodeDataFactoryT, EsString::const_pointer, GrammarT, SkipGrammarT>( \
			m_input.c_str(), m_input.c_str()+m_input.size(), m_grammar, m_skipGrammar ); 
#define PARSER_CONTEXT_PARSE_ERROR_HANDLER \
	return m_parseInfo.full; } \
	void setError(EsString::const_pointer pos, ErrorIDs id) {
#define PARSER_CONTEXT_PARSE_END }
// all custom parser context functionality is OK to be defined here,
// befire the PARSER_CONTEXT_IMPL_END
#define PARSER_CONTEXT_IMPL_END	};
// the following may be used to define ids for tagged grammar rules
#define GRAMMAR_RULE_IDS_BEGIN	enum RuleIDs { noRuleID,
#define GRAMMAR_DEF_RULE_ID( Name )	Name ## ID,
#define GRAMMAR_RULE_IDS_END			ruleIdCount };
// grammar parser class definition helper
#define GRAMMAR_PARSER_IMPL_BEGIN( ContextName, ParserName )	\
	struct ContextName ## ParserName : public grammar< ContextName ## ParserName > { \
		ContextName& m_ctx; \
		ContextName ## ParserName( ContextName& ctx ) : m_ctx(ctx) {} \
		template <typename ScannerT> \
		struct definition { \
			typedef ContextName ## ParserName SelfT; \
			typedef ContextName ContextT; \
			typedef ContextName ## Base ContextBaseT;
#define GRAMMAR_PARSER_DEFINITION_IMPL_BEGIN definition(const SelfT& self) { 
#define GRAMMAR_PARSER_USE_ASSERT_GUARD ContextT::AssertionGuardT assertionGuard;
#define GRAMMAR_PARSER_ASSERT_GUARD( x ) \
	assertionGuard(x)[self.m_ctx.m_errHandler]
#define GRAMMAR_PARSER_IMPL_END	}	}; };
// use the following to produce standard-named parser grammar and skip grammar classes
#define GRAMMAR_PARSER_STD_GRAMMAR_IMPL_BEGIN( ContextName ) GRAMMAR_PARSER_IMPL_BEGIN( ContextName, Grammar )
#define GRAMMAR_PARSER_STD_SKIP_IMPL_BEGIN( ContextName ) GRAMMAR_PARSER_IMPL_BEGIN( ContextName, Skip )
// the following will define tagged grammar rule & entry point for use inside grammar::definition structures
#define GRAMMAR_DEF_RULE(Name) rule<ScannerT, parser_context<>, parser_tag< static_cast<int>( ContextBaseT:: ## Name ## ID) > > Name;
#define GRAMMAR_DEF_ENTRY(Name) rule<ScannerT, parser_context<>, parser_tag< static_cast<int>( ContextBaseT:: ## Name ## ID) > > const& \
			start() const { return Name; }

//define the following in the grammar parser's definition body
//to create concrete tagged assertion var
//
#define GRAMMAR_ASSERT_DEF_EXPECT( Name ) \
	assertion<ContextT::ErrorIDs> Expect_ ## Name( ContextT::Expect_ ## Name );
#define GRAMMAR_ASSERT_DEF_ERROR( Name ) \
	assertion<ContextT::ErrorIDs> Error_ ## Name( ContextT::Error_ ## Name );
// apply previously defined assertion to concrete lexemme
#define GRAMMAR_EXPECT( Name ) Expect_ ## Name( Name )
#define GRAMMAR_EXPECT2( Name, Parser ) Expect_ ## Name( Parser )
#define GRAMMAR_ERROR( Name ) Error_ ## Name( Name )
#define GRAMMAR_ERROR2( Name, Parser ) Error_ ## Name( Parser )
// debug helper macros
//
// define debug dump output stream
#define GRAMMAR_DEF_DEBUG_DUMP_DEST( ContextName ) \
	static std::basic_ofstream<std::string::value_type> ContextName ## DumpDest( #ContextName "_dump.txt" );
// dump parse tree as xml file. use inside the top parser|compiler class's "parse" method
#define GRAMMAR_AST_DUMP_MAP_BEGIN \
	typedef std::map<long, std::string> RuleNamesMapT; \
	RuleNamesMapT astRuleNamesMap; \
	astRuleNamesMap[ noRuleID ] = "no-rule";
#define GRAMMAR_AST_DUMP_MAP_ENTRY( RuleName ) \
	astRuleNamesMap[ RuleName ## ID ] = #RuleName;
#define GRAMMAR_AST_DUMP_MAP_END( ContextName ) \
	std::ofstream astDump( #ContextName "_dump.xml" ); \
	if( astDump ) { \
		astToXml<ContextName::ParseTreeInfoT, ContextName::ParseTreeConstIteratorT>(astDump, m_parseInfo, astRuleNamesMap); }
		
#ifdef DUMP_AST_TO_XML
#	include <fstream>
static void inline identation(std::ostream& o, long& indent)
{
	for(long cnt = 0; cnt < indent; ++cnt)
		o << " ";
}

template<typename TreeConstIteratorT, typename RuleNamesMapT> 
static void inline astNodeToXml(std::ostream& o, long& indent, TreeConstIteratorT node, const RuleNamesMapT& astRuleNamesMap)
{
	const std::string c_noname = "noname";
	long id = (*node).value.id().to_long();
	++indent;
	RuleNamesMapT::const_iterator nameIt = astRuleNamesMap.find(id);
	const std::string name = (nameIt != astRuleNamesMap.end()) ? (*nameIt).second : c_noname;
	EsString value((*node).value.begin(), (*node).value.end());
	identation(o, indent);
	o << "<" << name << " value=\"" << EsString::toXMLString(value).toAscii() << "\">\n";
	for(TreeConstIteratorT it = (*node).children.begin(); it != (*node).children.end(); ++it)
		astNodeToXml(o, indent, it, astRuleNamesMap);
	identation(o, indent);
	o << "</" << name << ">\n";
	--indent;
}

template<typename TreeInfoT, typename TreeConstIteratorT, typename RuleNamesMapT> 
static void inline astToXml(std::ostream& o, const TreeInfoT& tree, const RuleNamesMapT& astRuleNamesMap)
{
	long indent = 0;
	o << "<root>\n";
	for( TreeConstIteratorT it = tree.trees.begin(); it != tree.trees.end(); ++it )
		astNodeToXml<TreeConstIteratorT>(o, indent, it, astRuleNamesMap);
	o << "</root>\n";	
}
#endif // DUMP_AST_TO_XML		

#endif // _grammar_defs_h_
