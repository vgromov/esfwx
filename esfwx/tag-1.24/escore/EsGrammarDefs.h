#ifndef _es_grammar_defs_h_
#define _es_grammar_defs_h_

enum SourceInputType {
  fromString,
  fromSourceFile
};

// boost spirit grammar definition helper macros
//
// define some prerequisites
#define PARSER_CONTEXT_PREREQUISITES_BEGIN( ContextName, NodeDataFactoryClass ) \
  struct ContextName; \
  struct ContextName ## Base { \
    typedef NodeDataFactoryClass NodeDataFactoryT;

#define PARSER_CONTEXT_PREREQUISITES_END \
  };

// top parser context defines
#define PARSER_CONTEXT_CUSTOM_IMPL_BEGIN( ContextName, CustomGrammars ) \
struct ContextName : public ContextName ## Base { \
  typedef ContextName ## Grammar <ContextName> GrammarT; \
  typedef ContextName ## Skip <ContextName> SkipGrammarT; \
  typedef tree_parse_info< EsString::const_pointer, NodeDataFactoryT > ParseTreeInfoT; \
  typedef tree_match< EsString::const_pointer, NodeDataFactoryT > ParseTreeMatchT; \
  typedef ParseTreeMatchT::tree_iterator ParseTreeIteratorT; \
  typedef ParseTreeMatchT::const_tree_iterator ParseTreeConstIteratorT; \
  typedef ParseTreeMatchT::node_t ParseTreeNodeT; \
  typedef ParseTreeNodeT::children_t ParseTreeNodeChildrenT; \
  struct ErrorHandler { \
    ContextName& m_ctx; \
    ErrorHandler( ContextName& ctx ) : m_ctx( ctx ) {} \
    template <typename ScannerT, typename ErrorT> \
    error_status<> operator()(ScannerT const& scan, ErrorT const& error) const { \
      error_status<> stat( ((m_ctx.m_errCnt >= ContextName ##_errCntMax) ? error_status<>::fail : error_status<>::retry) ); \
      ++m_ctx.m_errCnt; \
      ES_DEBUG_TRACE( esT(# ContextName) esT("::ErrorHandler m_ctx.m_errCnt=%d"), (int)m_ctx.m_errCnt); \
      if( stat.result == error_status<>::retry && m_ctx.errorHandle(error.where, error.descriptor) ) { m_ctx.scannerMove(scan); return stat; } \
      return error_status<>(error_status<>::fail); \
    } \
  } m_errHandler; \
  template <typename ScannerT> \
  void scannerMove(ScannerT const& scan) { do { ++scan; if( errorRecoveryIsComplete(scan.first) ) break; } while(!scan.at_end()); } \
  ParseTreeInfoT m_parseInfo; \
  EsAssocContainerIntf::Ptr m_fileRefs; \
  GrammarT m_grammar; \
  SkipGrammarT m_skipGrammar; \
  EsBreakIntf::Ptr m_brk; \
  EsVariant::Array m_execRanges; \
  EsString m_file; \
  EsString m_input; \
  SourceInputType m_inputType; \
  int m_errCnt; \
  CustomGrammars \
  ContextName( const EsString& input, const EsAssocContainerIntf::Ptr& fileRefs, SourceInputType inputType = fromString, \
    const EsBreakIntf::Ptr& brk = EsBreakIntf::Ptr() ) : \
    m_grammar(*this), m_skipGrammar(*this), m_fileRefs(fileRefs), \
    m_errHandler(*this), m_brk(brk), m_inputType(inputType), m_errCnt(0)

#define PARSER_CONTEXT_IMPL_BEGIN( ContextName ) PARSER_CONTEXT_CUSTOM_IMPL_BEGIN(ContextName, )

#define PARSER_CONTEXT_IMPL_INIT_BEGIN {

// all custom parser context initiliation functionality is OK to be defined here,
// before the PARSER_CONTEXT_IMPL_INIT_END

#define PARSER_CONTEXT_IMPL_INIT_END \
  if( fromString == inputType ) { m_file = EsString::null(); m_input = input; } \
  else { m_file = input; \
    EsFile in( m_file, as_<ulong>(EsFileFlag::Read)|as_<ulong>(EsFileFlag::Text) ); \
    if( in.open() ) { \
      ES_DEBUG_TRACE(esT("Parsing '%s'"), m_file); \
      m_input = in.readAllAsString(); } \
  } \
  if( m_fileRefs ) m_fileRefs->newValueSet(m_file, EsVariant::null()); \
}

// all custom parser context functionality is OK to be defined here,
// before the PARSER_CONTEXT_IMPL_END

#define PARSER_CONTEXT_IMPL_END };

// error tags definition macros. must be included in context base
// i.e. in pre-requisites scope
#define PARSER_CONTEXT_DEF_ERROR_BEGIN enum ErrorIDs {
#define PARSER_CONTEXT_DEF_EXPECT( Name ) Expect_ ## Name,
#define PARSER_CONTEXT_DEF_ERROR( Name ) Error_ ## Name,
#define PARSER_CONTEXT_DEF_ERROR_END  ErrorIDsCount };

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
      return _("Unknown parser error.");  }; }

// begin parser entry point block
#define PARSER_CONTEXT_PARSE_BEGIN \
  typedef guard< ErrorIDs > AssertionGuardT; \
  AssertionGuardT assertionGuard; \
  bool doParse(ulong offs = 0, ulong len = 0) { \
    ES_ASSERT(offs < m_input.size()); \
    ES_ASSERT(offs+len <= m_input.size()); \
    if(!len) len = m_input.size()-offs; \
    EsString::const_pointer start = m_input.c_str()+offs; \
    m_parseInfo = ast_parse<NodeDataFactoryT, EsString::const_pointer, GrammarT, SkipGrammarT>( \
      start, start+len, m_grammar, m_skipGrammar );

// begin parser entry point block
#define PARSER_SELECTED_CONTEXT_PARSE_BEGIN(Name) \
  bool doParse_##Name(ulong offs = 0, ulong len = 0) { \
    ES_ASSERT(offs < m_input.size()); \
    ES_ASSERT(offs+len <= m_input.size()); \
    if(!len) len = m_input.size()-offs; \
    EsString::const_pointer start = m_input.c_str()+offs; \
    m_parseInfo = ast_parse<NodeDataFactoryT, EsString::const_pointer, GrammarT, SkipGrammarT>( \
      start, start+len, m_grammar_##Name, m_skipGrammar );

#define PARSER_CONTEXT_PARSE_END \
  return m_parseInfo.full; }

#define PARSER_CONTEXT_PARSE_ERROR_HANDLER_BEGIN \
  bool errorHandle(EsString::const_pointer pos, ErrorIDs id, bool doThrow = false) {

#define PARSER_CONTEXT_PARSE_ERROR_HANDLER_END \
  return false; }

#define PARSER_CONTEXT_ERROR_RECOVERY_HANDLER_BEGIN \
  bool errorRecoveryIsComplete(EsString::const_pointer scannerPos) {

#define PARSER_CONTEXT_ERROR_RECOVERY_HANDLER_END \
  return true; }

// the following may be used to define ids for tagged grammar rules
#define GRAMMAR_RULE_IDS_BEGIN  enum RuleIDs { noRuleID,
#define GRAMMAR_DEF_RULE_ID( Name ) Name ## ID,
#define GRAMMAR_RULE_IDS_END      ruleIdCount };

// grammar parser class definition helper
#define GRAMMAR_PARSER_IMPL_BEGIN( ContextName, ParserName )  \
  template < typename CtxT> \
  struct ContextName ## ParserName : public grammar< ContextName ## ParserName <CtxT> > { \
    typedef CtxT ContextT; \
    typedef ContextName ## Base ContextBaseT; \
    ContextT& m_ctx; \
    ContextName ## ParserName( ContextT& ctx ) : m_ctx(ctx) {} \
    template < typename ScannerT > \
    struct definition { \
      typedef ContextName ## ParserName <CtxT> SelfT; \
      typedef CtxT ContextT; \
      typedef ContextName ## Base ContextBaseT;

#define GRAMMAR_PARSER_DEFINITION_IMPL_BEGIN \
  definition(const SelfT& self) {

#define GRAMMAR_PARSER_USE_ASSERT_GUARD typename ContextT::AssertionGuardT assertionGuard;

#define GRAMMAR_PARSER_ASSERT_GUARD( x ) \
  assertionGuard(x)[self.m_ctx.m_errHandler]

#define GRAMMAR_PARSER_IMPL_END } }; };

// the following will define tagged grammar rule & entry point for use inside grammar::definition structures
#define GRAMMAR_DEF_RULE(Name) rule<ScannerT, parser_context<>, parser_tag< static_cast<int>( ContextBaseT:: Name ## ID) > > Name;

#define GRAMMAR_DEF_ENTRY(Name) rule<ScannerT, parser_context<>, parser_tag< static_cast<int>( ContextBaseT:: Name ## ID) > > const& \
      start() const { return Name; }

#define GRAMMAR_DEF_ENTRY_OVERRIDE(BaseInst, Name) rule<ScannerT, parser_context<>, parser_tag< static_cast<int>( BaseDefinitionT::ContextBaseT:: Name ## ID) > > const& \
      start() const { return BaseInst.Name; }

//define the following in the grammar parser's definition body
//to create concrete tagged assertion var
//
#define GRAMMAR_ASSERT_DEF_EXPECT( Name ) \
  assertion< ContextBaseT::ErrorIDs > Expect_ ## Name( ContextBaseT::Expect_ ## Name );
#define GRAMMAR_ASSERT_DEF_ERROR( Name ) \
  assertion< ContextBaseT::ErrorIDs > Error_ ## Name( ContextBaseT::Error_ ## Name );
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
  EsPath dump_fname = EsPath::createFromFilePath(m_file); EsString dump_sfname = dump_fname.pathGet() + esT(#ContextName "_dump.xml"); \
  std::ofstream astDump( (const char*)EsString::toUtf8(dump_sfname).c_str() ); \
  if( astDump ) { \
    astToXml<ContextName::ParseTreeInfoT, ContextName::ParseTreeConstIteratorT>(astDump, m_parseInfo, astRuleNamesMap); }
    
#ifdef ES_DUMP_AST_TO_XML
# include <fstream>
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

  auto nameIt = astRuleNamesMap.find(id);
  const std::string name = (nameIt != astRuleNamesMap.end()) ?
    (*nameIt).second :
    c_noname;

  EsString value((*node).value.begin(), (*node).value.end());
  identation(o, indent);

  o << "<" << name << " value=\"" << EsString::toAscii(EsString::toXMLString(value)) << "\">\n";
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
#endif // ES_DUMP_AST_TO_XML

#endif // _es_grammar_defs_h_
