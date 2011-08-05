// uncomment
#define DUMP_AST_TO_XML

#include <escore/grammar_defs.h>

#ifdef _DEBUG
// comment out the following define BOOST_SPIRIT_DEBUG to turn off debug text dump
//#	define BOOST_SPIRIT_DEBUG
#	ifdef BOOST_SPIRIT_DEBUG
#	define BOOST_SPIRIT_DEBUG_PRINT_SOME 64
#		include <fstream>
		GRAMMAR_DEF_DEBUG_DUMP_DEST(EsScriptCompiler)
#		define BOOST_SPIRIT_DEBUG_OUT		EsScriptCompilerDumpDest
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
#include <boost/spirit/include/classic_symbols.hpp>

using namespace boost::spirit::classic;

// context prerequisites
PARSER_CONTEXT_PREREQUISITES_BEGIN( EsScriptCompiler, node_iter_data_factory<> )
	GRAMMAR_RULE_IDS_BEGIN
		GRAMMAR_DEF_RULE_ID(skip)
#		define GRAMMAR_RULE	GRAMMAR_DEF_RULE_ID
#		include "EsScriptCompilerRuleNames.hxx"
	GRAMMAR_RULE_IDS_END
PARSER_CONTEXT_PREREQUISITES_END

// grammar parser
GRAMMAR_PARSER_STD_GRAMMAR_IMPL_BEGIN( EsScriptCompiler )
	// named char literals
#	define SEMICOLON	ch_p(wxT(';'))
#	define COMMA			ch_p(wxT(','))
#	define DBLQUOTE		ch_p(wxT('"'))
#	define LCBRACKET	ch_p(wxT('{'))
#	define RCBRACKET	ch_p(wxT('}'))
#	define LSBRACKET	ch_p(wxT('['))
#	define RSBRACKET	ch_p(wxT(']'))
#	define LRBRACKET	ch_p(wxT('('))
#	define RRBRACKET	ch_p(wxT(')'))
#	define UNDERSCORE	ch_p(wxT('_'))
# define COLON			ch_p(wxT(':'))
// operators
# define OPINC			str_p(wxT("++"))
# define OPDEC			str_p(wxT("--"))
# define OPLOG_OR		str_p(wxT("||"))
# define OPLOG_AND	str_p(wxT("&&"))
# define OPBW_AND		ch_p(wxT('&'))
# define OPBW_OR		ch_p(wxT('|'))
# define OPBW_XOR		ch_p(wxT('^'))
# define OPBW_NOT		ch_p(wxT('~'))
# define OPLSH			str_p(wxT("<<"))
# define OPRSH			str_p(wxT(">>"))
#	define PLUS				ch_p(wxT('+'))
#	define MINUS			ch_p(wxT('-'))
# define OPMUL			ch_p(wxT('*'))	
# define OPDIV			ch_p(wxT('/'))	
# define OPMOD			ch_p(wxT('%'))
# define OPNOT			ch_p(wxT('!'))
# define OPMTDACC		ch_p(wxT('.'))
# define OPFLDACC		ch_p(wxT('.'))	
# define OPPROPACC	ch_p(wxT('$'))
# define OPATTRACC	ch_p(wxT('@'))
# define OPSCOPEACC str_p(wxT("::"))
# define OPRANGE		str_p(wxT(".."))	
// comparison
#	define OPLE				ch_p(wxT('<'))
#	define OPGR				ch_p(wxT('>'))
#	define OPEQ				str_p(wxT("=="))
#	define OPNEQ			str_p(wxT("!="))
#	define OPLEEQ			str_p(wxT("<="))
#	define OPGREQ			str_p(wxT(">="))
// assignments
#	define ASN				ch_p(wxT('='))
#	define ASNDEC			str_p(wxT("-="))
#	define ASNINC			str_p(wxT("+="))
#	define ASNDIV			str_p(wxT("/="))
#	define ASNMOD			str_p(wxT("%="))
#	define ASNMUL			str_p(wxT("*="))
#	define ASNOR			str_p(wxT("|="))
#	define ASNAND			str_p(wxT("&="))
#	define ASNXOR			str_p(wxT("^="))
#	define ASNLSH			str_p(wxT("<<="))
#	define ASNRSH			str_p(wxT(">>="))
// hex prefix
#	define HEXPFX			str_p(wxT("0x"))
// reserved words
//
#	define VARIABLE		str_p(wxT("var"))	
#	define CONSTANT		str_p(wxT("const"))
# define CTOR				str_p(c_reflectedCtrName)
# define DTOR				str_p(c_reflectedDtrName)
#	define FUNCTION		str_p(wxT("function"))
# define RETURN			str_p(wxT("return"))
# define NEWOBJ			str_p(c_reflectedCtrName)
#	define OBJECT			str_p(wxT("object"))	
# define EXTENDS		str_p(wxT("extends"))
#	define BTRUE			str_p(wxT("true"))
#	define BFALSE			str_p(wxT("false"))
# define IF					str_p(wxT("if"))
# define ELSE				str_p(wxT("else"))
# define FOR				str_p(wxT("for"))
# define WHILE			str_p(wxT("while"))
# define DO					str_p(wxT("do"))
# define BREAK			str_p(wxT("break"))
# define CONTINUE		str_p(wxT("continue"))
# define SWITCH			str_p(wxT("switch"))
# define CASE				str_p(wxT("case"))
# define DEFAULT		str_p(wxT("default"))
# define LABEL			str_p(wxT("label")) 
# define GOTO				str_p(wxT("goto"))
// rules
//
#	define GRAMMAR_RULE	GRAMMAR_DEF_RULE
#	include "EsScriptCompilerRuleNames.hxx"
	// grammar entry point
	GRAMMAR_DEF_ENTRY(script)
	
	GRAMMAR_PARSER_DEFINITION_IMPL_BEGIN
		// error support
		GRAMMAR_PARSER_USE_ASSERT_GUARD
		GRAMMAR_ASSERT_DEF_EXPECT(COLON)
		GRAMMAR_ASSERT_DEF_EXPECT(SEMICOLON)
		GRAMMAR_ASSERT_DEF_EXPECT(RCBRACKET)
		GRAMMAR_ASSERT_DEF_EXPECT(RSBRACKET)
		GRAMMAR_ASSERT_DEF_EXPECT(RRBRACKET)
		GRAMMAR_ASSERT_DEF_EXPECT(DBLQUOTE)
		GRAMMAR_ASSERT_DEF_EXPECT(OPATTRACC)
		GRAMMAR_ASSERT_DEF_EXPECT(value)
		GRAMMAR_ASSERT_DEF_EXPECT(ident)

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
			GRAMMAR_EXPECT(DBLQUOTE)
      ]];
  
    simpleVal = 
			strVal |
			boolVal |
			floatVal |			
			intVal;
			
		arrayVal =  
			root_node_d[LSBRACKET] >> 
				infix_node_d[
					!list_p(value, COMMA)
				] >> no_node_d[
					GRAMMAR_EXPECT(RSBRACKET)
					];
			
		value = 
			simpleVal |
			arrayVal;		
		
		// lhs|rhs = (lrhs) (AKA left|right-hand-side rule)
		idxAcc = root_node_d[LSBRACKET]	>> 
			expr >> 
			no_node_d[GRAMMAR_EXPECT(RSBRACKET)];
				
		fieldAcc = root_node_d[OPFLDACC] >> 
			ident >> !idxAcc;

		propAcc = root_node_d[OPPROPACC] >>
			ident >> !idxAcc;

		attrAcc = root_node_d[OPATTRACC] >>
			ident >> !idxAcc;
			
		objMethodCall = 
			root_node_d[OPMTDACC] >> 
			exprFuncCall;

		lrhs = 
			(ident >> !idxAcc >> 
				*(	objMethodCall | 
						fieldAcc | 
						propAcc |
						attrAcc
				)
			) |	
			(
				propAcc | 
				attrAcc
			);
								
		// operators
		//
		opAssign = longest_d[
			ASN |
      ASNDEC |
      ASNINC |
      ASNDIV |
      ASNMOD |
      ASNMUL |
      ASNOR	|
      ASNAND |
      ASNXOR |
      ASNLSH |
      ASNRSH
    ];	
		
    opIncDec = OPINC | OPDEC;		
    
    opLogical	= 
			OPLOG_OR | 
			OPLOG_AND;
    
		opBitwise	= 
			OPBW_AND | 
			OPBW_OR | 
			OPBW_XOR;    

		opEqNeq = 
			OPEQ |
			OPNEQ;
			
		opCmp	=  longest_d[ 
			OPLE |
			OPGR |
			OPLEEQ |
			OPGREQ
			];
			
		opSh	= 
			OPLSH |
			OPRSH;
			
		opAddSub =   
			PLUS |
			MINUS;	
			
		opMulDiv = 
			OPMUL	|
			OPDIV |
			OPMOD;	
			
		opUnary	=
		  OPNOT	|
			MINUS |
			OPBW_NOT;			
			
		// identifier
		ident = lexeme_d[leaf_node_d[
			(alpha_p | UNDERSCORE) >>	*(alnum_p | UNDERSCORE)
			]];
			
		// expressions
		//
		expr = exprBitwise >> *(root_node_d[opLogical] >> exprBitwise);

		exprBitwise	= exprEqNeq >> *(root_node_d[opBitwise] >> exprEqNeq);

		exprEqNeq = exprCmp >> *(root_node_d[opEqNeq] >> exprCmp);

		exprCmp	= exprSh >> *(root_node_d[opCmp] >> exprSh);

		exprSh = exprAddSub >> *(root_node_d[opSh] >> exprAddSub);

		exprAddSub = exprMulDiv >> *(root_node_d[opAddSub] >> exprMulDiv);

		exprMulDiv = exprUnary >> *(root_node_d[opMulDiv] >> exprUnary);

		exprUnary	= 
			exprIncDec | 
			exprIncDecPfx | 
			(!root_node_d[opUnary] >> exprAtom);

		exprIncDec = opIncDec >> lrhs;

		exprIncDecPfx = lrhs >> opIncDec;
			
		exprFuncCall = 
			ident >> root_node_d[ LRBRACKET ] >> 
				infix_node_d[ 
					!list_p(expr, COMMA) 
				] >> 
			no_node_d[ RRBRACKET ];
			
		exprObjCtrCall = root_node_d[NEWOBJ] >> exprFuncCall;
		
		exprStaticCall = 
			ident >>
			root_node_d[OPSCOPEACC] >> 
			exprFuncCall;

		exprCall = 
			exprObjCtrCall |
			exprStaticCall |
			exprFuncCall;

		exprAtom =
			value	|
			exprCall |
			lrhs |
			inner_node_d[ LRBRACKET >> expr >> RRBRACKET ];	
	  
	  exprAssign = lrhs >> root_node_d[opAssign] >> expr;
	   
	  // special assign initializer expression, which is used in var decls only
	  exprInitAsn	=
			ident >> root_node_d[ASN] >> expr;
			
		// statements	
		//
		stmtAssign = 
			exprAssign >> 
				root_node_d[GRAMMAR_EXPECT(SEMICOLON)];
		
    stmtConstDecl =
			root_node_d[CONSTANT] >> 
				(GRAMMAR_EXPECT(ident) >> no_node_d[ASN] >> GRAMMAR_EXPECT(value)) >> 
					no_node_d[GRAMMAR_EXPECT(SEMICOLON)];
      
    stmtVarDecl =  
			root_node_d[VARIABLE] >> 
				infix_node_d[
					list_p( (exprInitAsn | GRAMMAR_EXPECT(ident)), COMMA)
				] >> no_node_d[GRAMMAR_EXPECT(SEMICOLON)];
      
		stmtFuncDecl = 
			root_node_d[FUNCTION] >> GRAMMAR_EXPECT(ident) >> 
			no_node_d[LRBRACKET] >> 
				infix_node_d[ !list_p(ident, COMMA) ]	>> 
			no_node_d[GRAMMAR_EXPECT(RRBRACKET)] >> 
			!stmtVarDecl >>
			stmtBlock; 

		stmtObjCtorDecl = 
			root_node_d[CTOR] >> 
			no_node_d[LRBRACKET] >> 
				infix_node_d[ !list_p(ident, COMMA) ]	>> 
			no_node_d[GRAMMAR_EXPECT(RRBRACKET)] >> 
			stmtBlock; 

		stmtObjDtorDecl = 
			root_node_d[DTOR] >> 
			no_node_d[LRBRACKET] >>
			no_node_d[GRAMMAR_EXPECT(RRBRACKET)] >> 
			stmtBlock; 
			
		stmtReturn = 
			root_node_d[RETURN] >> !expr >> 
				no_node_d[GRAMMAR_EXPECT(SEMICOLON)];	
				
		stmtAttributeDecl = 
			root_node_d[OPATTRACC] >>	GRAMMAR_EXPECT(ident) >> 
				!(no_node_d[ASN] >> value) >>
				no_node_d[GRAMMAR_EXPECT(SEMICOLON)];
				
		stmtAttributeList = 
			+stmtAttributeDecl;
			
		stmtObjPlainFldDecl = 
			ident >> ident;

		stmtObjArrayFldDecl = 
			ident >> ident >> no_node_d[LSBRACKET] >>
				expr >> no_node_d[GRAMMAR_EXPECT(RSBRACKET)];
		
		stmtObjFldDecl = 
			(stmtObjArrayFldDecl | 
			 stmtObjPlainFldDecl) >>
				no_node_d[SEMICOLON] >>
			!stmtAttributeList;
			
		stmtObjMethodDecl =
			stmtFuncDecl |
			stmtObjDtorDecl |
			stmtObjCtorDecl;
			
		stmtObjDeclBlock = inner_node_d[
      LCBRACKET >> 
				*stmtObjFldDecl >>
				*stmtObjMethodDecl >>
			GRAMMAR_EXPECT(RCBRACKET)
      ];
			
		stmtObjDecl = 
			root_node_d[OBJECT] >> ident >> 
				!(discard_node_d[EXTENDS] >> GRAMMAR_EXPECT(ident)) >>
				!stmtAttributeList >>
					stmtObjDeclBlock;
			
		stmtCall = exprCall >> root_node_d[SEMICOLON];	
		
		stmtElse = root_node_d[ ELSE ] >> stmt;
		
		stmtIf = root_node_d[ IF ] >> 
			inner_node_d[ LRBRACKET >> expr >> GRAMMAR_EXPECT(RRBRACKET) ] >> 
				stmt >> !stmtElse;	
				
		exprForInit = root_node_d[ LRBRACKET ] >>
			(stmtAssign | no_node_d[ GRAMMAR_EXPECT(SEMICOLON) ]);
		
		exprForCheck = (expr >> root_node_d[ GRAMMAR_EXPECT(SEMICOLON) ]) | 
			root_node_d[ GRAMMAR_EXPECT(SEMICOLON) ];
		
		exprForCnt = !(exprAssign|exprIncDec|exprIncDecPfx) >> 
			root_node_d[ GRAMMAR_EXPECT(RRBRACKET) ];
		
    stmtFor = root_node_d[ FOR ] >> 
			exprForInit >>
			exprForCheck >> 
			exprForCnt >> 
			stmt;
			
		exprWhileCheck = root_node_d[WHILE] >> 
			no_node_d[LRBRACKET] >>
				expr >>
			no_node_d[GRAMMAR_EXPECT(RRBRACKET)];
			
		stmtWhile = exprWhileCheck >>	stmt;

		stmtDoWhile = root_node_d[DO] >>
			stmt >>
			exprWhileCheck >>	no_node_d[GRAMMAR_EXPECT(SEMICOLON)];
			
		stmtBreak = root_node_d[BREAK] >> no_node_d[GRAMMAR_EXPECT(SEMICOLON)];	

		stmtContinue = root_node_d[CONTINUE] >> no_node_d[GRAMMAR_EXPECT(SEMICOLON)];	
		
		stmtLabel = root_node_d[LABEL] >> ident >> no_node_d[GRAMMAR_EXPECT(COLON)];
		
		stmtGoto = root_node_d[GOTO] >> ident >> no_node_d[GRAMMAR_EXPECT(SEMICOLON)];
		
		// as a statement, prefix and postfis incdecs are the same
		stmtIncDec = 
			(exprIncDec |
			exprIncDecPfx) >> 
				root_node_d[SEMICOLON];
				
		stmtLrhs = lrhs >> root_node_d[SEMICOLON];
				
		stmtNoop = root_node_d[SEMICOLON];		
				
		// top-level grammar
		//
    stmtList = 
			*( stmtConstDecl |
				 stmtVarDecl |
				 stmtObjDecl |
				 stmt );
			
		stmtBlock = root_node_d[LCBRACKET] >> 
				*stmt >> 
			no_node_d[GRAMMAR_EXPECT(RCBRACKET)];

    stmt = 
			stmtCall |
			stmtIncDec |
      stmtFuncDecl |
      stmtReturn |
      stmtAssign |
      stmtBlock |
      stmtIf |
      stmtFor |
      stmtDoWhile |
      stmtWhile |
      stmtContinue |
      stmtBreak |
      stmtLabel |
      stmtGoto |
      stmtLrhs |
      stmtNoop; // noop statement
		
		// entry point
		script = GRAMMAR_PARSER_ASSERT_GUARD(
			stmtList
			) >> end_p;
		
#		ifdef BOOST_SPIRIT_DEBUG
#			define GRAMMAR_RULE(r)	BOOST_SPIRIT_DEBUG_RULE(r);
#			include "EsScriptCompilerRuleNames.def"
			// root rule
			BOOST_SPIRIT_DEBUG_RULE(script);
#		endif	
GRAMMAR_PARSER_IMPL_END

// skip parser
GRAMMAR_PARSER_STD_SKIP_IMPL_BEGIN( EsScriptCompiler )
	GRAMMAR_DEF_RULE(skip)
	// grammar entry point
	GRAMMAR_DEF_ENTRY(skip)
	GRAMMAR_PARSER_DEFINITION_IMPL_BEGIN
		skip =	space_p |
						comment_p( wxT("//") ) |
						comment_p( wxT("/*"), wxT("*/") );
				
#	ifdef BOOST_SPIRIT_DEBUG
		BOOST_SPIRIT_DEBUG_RULE(skip);
#	endif					
GRAMMAR_PARSER_IMPL_END

// opids static symbol table
class EsScriptCompilerOpIds
{
public:
	EsScriptCompilerOpIds();
	EsScriptOperatorIds getId(const EsString& op) const;
	const EsString& getIdStr(long id) const;
	
protected:
	typedef containers::structured_map<EsString, EsScriptOperatorIds> Map;
	Map m_idsMap;
};

// parser context impl
PARSER_CONTEXT_IMPL_BEGIN(EsScriptCompiler)
	m_breakGuard = 0;
	m_continueGuard = 0;
	m_labelGuard = 0;
	PARSER_CONTEXT_DEF_ERROR_BEGIN
		PARSER_CONTEXT_DEF_ERROR(Parser)
		PARSER_CONTEXT_DEF_EXPECT(COLON)
		PARSER_CONTEXT_DEF_EXPECT(SEMICOLON)
		PARSER_CONTEXT_DEF_EXPECT(RCBRACKET)
		PARSER_CONTEXT_DEF_EXPECT(RSBRACKET)
		PARSER_CONTEXT_DEF_EXPECT(RRBRACKET)
		PARSER_CONTEXT_DEF_EXPECT(DBLQUOTE)
		PARSER_CONTEXT_DEF_EXPECT(OPATTRACC)
		PARSER_CONTEXT_DEF_EXPECT(value)
		PARSER_CONTEXT_DEF_EXPECT(ident)
	PARSER_CONTEXT_DEF_ERROR_END
	PARSER_CONTEXT_DEF_ERROR_DESCR_BEGIN
		PARSER_CONTEXT_DEF_ERROR_DESCR(Parser,			wxT("parser error"))
		PARSER_CONTEXT_DEF_EXPECT_DESCR(COLON,			wxT("expected :"))
		PARSER_CONTEXT_DEF_EXPECT_DESCR(SEMICOLON,	wxT("expected ;"))
		PARSER_CONTEXT_DEF_EXPECT_DESCR(RCBRACKET,	wxT("expected }"))
		PARSER_CONTEXT_DEF_EXPECT_DESCR(RSBRACKET,	wxT("expected ]"))
		PARSER_CONTEXT_DEF_EXPECT_DESCR(RRBRACKET,	wxT("expected )"))
		PARSER_CONTEXT_DEF_EXPECT_DESCR(DBLQUOTE,		wxT("expected \""))
		PARSER_CONTEXT_DEF_EXPECT_DESCR(OPATTRACC,	wxT("expected @"))
		PARSER_CONTEXT_DEF_EXPECT_DESCR(value,			wxT("expected integer, float, string, or array value"))
		PARSER_CONTEXT_DEF_EXPECT_DESCR(ident,			wxT("expected identifier"))
	PARSER_CONTEXT_DEF_ERROR_DESCR_END
	PARSER_CONTEXT_PARSE_BEGIN
#	ifdef DUMP_AST_TO_XML	
		// dump parse tree as XML. each entry must be GRAMMAR_XML_DUMP_MAP_ENTRY
		GRAMMAR_AST_DUMP_MAP_BEGIN
			GRAMMAR_AST_DUMP_MAP_ENTRY(skip)
#			define GRAMMAR_RULE	GRAMMAR_AST_DUMP_MAP_ENTRY
#			include "EsScriptCompilerRuleNames.hxx"
			// root rule
			GRAMMAR_AST_DUMP_MAP_ENTRY(script)
		GRAMMAR_AST_DUMP_MAP_END(EsScriptCompiler)	
# endif // DUMP_AST_TO_XML			
	PARSER_CONTEXT_PARSE_ERROR_HANDLER	
		size_t line = 1;
		EsString::const_pointer start = m_input.c_str();
		EsString::const_pointer cur = start;
		while( cur <= pos )
		{
			if( wxT('\n') == *cur++ )
			{
				++line;
				if( wxT('\r') == *cur )
					++cur;
									
				start = cur;
			}
		}
		size_t col = 1 + ( (pos > start) ? pos-start : 0);
		// parsing error handling goes here, error position is passed 
		// in pos parameter, its identifier goes in id parameter
		EsException::Throw(0, EsException::Generic,
			_("EsScript error at line %d, column %d; Reason: '%s'\n"), 
			line, col, getErrorDescr(id).c_str());
	PARSER_CONTEXT_PARSE_END
	
	void compileTo(EsScriptMachine& script)
	{
		m_breakGuard = 0;
		m_continueGuard = 0;
		m_labelGuard = 0;
		ParseTreeConstIteratorT cit = m_parseInfo.trees.begin();
		while( cit != m_parseInfo.trees.end() )
		{
			m_objectOrNamespace = EsScriptMachine::s_global;
			compileStmt( cit, script );
			++cit;
		}
	}
	
protected:
	// node compiler
	void compileStmt(ParseTreeConstIteratorT citParent, EsScriptMachine& script)
	{
		long id = (*citParent).value.id().to_long();
		wxASSERT(opAssignID == id || (stmtID <= id && id <= stmtBlockID));
		ParseTreeConstIteratorT cit;
		switch( id )
		{
		case stmtListID:
		case stmtBlockID:
		case stmtObjDeclBlockID:
			for(cit = (*citParent).children.begin(); cit != (*citParent).children.end(); ++cit)
				compileStmt(cit, script);
			break;
		case stmtAssignID:
			compileOpAssign(citParent, script, true);
			break;
		case stmtIncDecID:
			compileIncDec(citParent, script);
			break;	
		case stmtConstDeclID:
			compileConstDecl(citParent, script);
			break;
		case stmtVarDeclID:
			compileVarsDecl(citParent, script);
			break;	
		case stmtFuncDeclID:
		case stmtObjCtorDeclID:		
		case stmtObjDtorDeclID:
			compileStmtFuncDecl(citParent, script);
			break;	
		case stmtReturnID:
			compileReturn(citParent, script);
			break;
		case stmtCallID:
			compileStmtCall(citParent, script);
			break;
		case stmtIfID:
			compileStmtIf(citParent, script);
			break;
		case stmtForID:
			compileStmtFor(citParent, script);
			break;	
		case stmtWhileID:
		case stmtDoWhileID:
			compileStmtWhile(citParent, script);
			break;	
		case stmtBreakID:
		case stmtContinueID:
			compileStmtBreakContinue(citParent, script);
			break;	
		case stmtLabelID:
		case stmtGotoID:
			compileStmtLabelGoto(citParent, script);
			break;	
		case stmtObjDeclID:
			compileStmtObjDecl(citParent, script);
			break;
		case stmtObjFldDeclID:
			compileStmtObjFldWithAttrsDecl(citParent, script);
			break;
		case stmtObjPlainFldDeclID:
		case stmtObjArrayFldDeclID:
			compileStmtObjFldDecl(citParent, script);
			break;
		case stmtLrhsID:
			compileStmtLrhs(citParent, script);
			break;
		}
	}
	
	// single statement compilers
	//
	void compileConstDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		wxASSERT((*cit).value.id().to_long() == stmtConstDeclID);
		wxASSERT((*cit).children.size() == 2);
		ParseTreeConstIteratorT child = (*cit).children.begin();
		EsString name = compileIdent(child++);
		EsVariant val = compileValue(child, script);
		script.declConst(name, val);
	}
	
	void compileVarsDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		wxASSERT(stmtVarDeclID == (*cit).value.id().to_long());
		// compile vars 
		for(ParseTreeConstIteratorT child = (*cit).children.begin(); child != (*cit).children.end(); ++child)
		{
			long id = (*child).value.id().to_long();
			wxASSERT(identID == id || exprInitAsnID == id);
			if( identID == id ) // just declaration
				// add variable to the current section's symtable & create memslot for it
				script.getActiveMethod()->declVar( compileIdent(child) );	
			else // declaration + initialization with assignment
				compileInitAsn(child, script);
		}
	}
	
	// compile variable declaration with initialization expression
	void compileInitAsn(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		wxASSERT((*cit).value.id().to_long() == exprInitAsnID);
		wxASSERT((*cit).children.size() == 2);
		ParseTreeConstIteratorT child = (*cit).children.begin();
		EsCodeSection::Ptr code = script.getActiveMethod();
		// declare var, pushing its reference as lhs
		EsString name = compileIdent(child++);
		code->declVar( name );	
		// compile and push right-side expr(s)
		compileExpr(child, script);
		// issue var init instruction, parameter contains reference to variable
		code->addInstruction(iVarInit, name);
	}
	
	void compileStmtFuncDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		long id = (*cit).value.id().to_long();
		wxASSERT(id == stmtFuncDeclID || id == stmtObjCtorDeclID || id == stmtObjDtorDeclID);
		wxASSERT((*cit).children.size() > 0);
		EsCodeSection::Ptr oldCode = script.getActiveMethod();
		ParseTreeConstIteratorT child = (*cit).children.begin();
		// get function identificator
		EsString funcName;
		if( id == stmtObjCtorDeclID )
			funcName = c_reflectedCtrName;
		else if( id == stmtObjDtorDeclID )
			funcName = c_reflectedDtrName;
		else
			funcName = compileIdent(child++);
		// create new script method
		EsCodeSection::Ptr function = script.createMethod(funcName, m_objectOrNamespace);
		while( child != (*cit).children.end() && 
					 identID == (*child).value.id().to_long() )
			// register named function parameters in new section
			function->declParam( compileIdent(child++) );
		// try to add newly created method to the script machine code table
		script.addMethod(function);
		// set this method as an active one 
		script.setActiveMethod(function);
		// compile optional variable declarations
		while(stmtVarDeclID == (*child).value.id().to_long())
		{
			compileVarsDecl(child, script);
			++child;
		}
		// set-up label|goto guard
		LabelGuard lblGuard( script.getActiveMethod(), *this );
		// compile section's code
		wxASSERT(stmtBlockID == (*child).value.id().to_long());
		compileStmt(child, script);
		// finally, restore previous code section, throwing exception if needed
		script.setActiveMethod( oldCode );
	}
	
	void compileReturn(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		wxASSERT((*cit).value.id().to_long() == stmtReturnID);
		// issue return instruction, possibly preceeded by some expr code
		if( (*cit).children.empty() )
			// push empty const value to the data stack on empty return
			script.getActiveMethod()->addInstruction(iLoadConstVal, script.emptyVal());
		else
			for(ParseTreeConstIteratorT child = (*cit).children.begin(); child != (*cit).children.end(); ++child)
				compileExpr(child, script);
		script.getActiveMethod()->addInstruction(iRet);
	}
	
	void compileStmtCall(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		wxASSERT((*cit).value.id().to_long() == stmtCallID);
		wxASSERT((*cit).children.size() == 1);
		// compile function call expr as usual
		compileExprCall((*cit).children.begin(), script);
		// issue additional iRetCleanup instruction after call return
		script.getActiveMethod()->addInstruction(iRetCleanup); 
	}
	
	void compileStmtLrhs(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		wxASSERT((*cit).value.id().to_long() == stmtLrhsID);
		wxASSERT((*cit).children.size() == 1);
		// compile lrhs	
		cit = (*cit).children.begin();
		compileLrhs(cit, script);
		// if the last lrhs child is object call - add iRetCleanup, otherwise - complain on senseless instruction
		if( objMethodCallID == (*cit).children.back().value.id().to_long() )
			script.getActiveMethod()->addInstruction(iRetCleanup); 
		else
			EsException::Throw(0, EsException::Generic, _("Statement expression is meaningless"));	
	}
	
	void compileOpAssign(ParseTreeConstIteratorT cit, EsScriptMachine& script, bool asStmt = false )
	{
		if( asStmt )
		{
			wxASSERT((*cit).value.id().to_long() == stmtAssignID);
			wxASSERT((*cit).children.size() == 1);
			cit = (*cit).children.begin();
		}
		wxASSERT((*cit).value.id().to_long() == opAssignID);
		wxASSERT((*cit).children.size() == 2);
		EsString opstr((*cit).value.begin(), (*cit).value.end());
		// find which exactly assign operation to use
		EsScriptOperatorIds opid = s_opids.getId(opstr); 
		// assert opid is one of assignment ids
		wxASSERT(opid >= opidAsn && opid <= opidAsnRsh);
		
		ParseTreeConstIteratorT child = (*cit).children.begin();
		// evaluate and push lhs
		compileLrhs(child++, script);
		// evaluate rhs
		compileExpr(child, script);
		
		// finally, push assignment instruction
		EsVariant param(EsVariant::VAR_VARIANT_COLLECTION);
		param.addToVariantCollection(opid)
				 .addToVariantCollection(asStmt);
		script.getActiveMethod()->addInstruction(iAsn, param);
	}
	
	// evaluate and push l|r-hs. lrhs kind is defined by the last part of lrhs compiled
	void compileLrhsNested(ParseTreeConstIteratorT cit, EsScriptMachine& script, size_t level)
	{
		long id = (*cit).value.id().to_long();
		wxASSERT(id == lrhsID || id == identID || id == idxAccID || id == fieldAccID || id == propAccID || id == attrAccID || id == objMethodCallID);
		switch( id )
		{
		case identID:
			compileVarOrConstAcc(cit, script);
			break;
		case idxAccID:
			// compile index expression
			compileIdxAcc(cit, script);
			break;	
		case fieldAccID:
			compileFieldAcc(cit, script);
			break;
		case propAccID:
			compilePropAcc(cit, script, 0 == level);
			break;	
		case attrAccID:
			compileAttrAcc(cit, script, 0 == level);
			break;	
		case objMethodCallID:
			compileObjMethodCall(cit, script);
			break;		
		case lrhsID:
			// any nested lhrs got to be compiled as rhs
			// compound l-or-r-value, recursively resolve from leftmost to rightmost
			for(ParseTreeConstIteratorT child = (*cit).children.begin(); child != (*cit).children.end(); ++child)
				compileLrhsNested(child, script, ++level);
			break;
		default:
			wxFAIL; // we're never here
			break;	
		}	
	}
	
	void compileLrhs(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		compileLrhsNested(cit, script, 0);
	}
	
	// var access common internal helper
	bool internalCompileVarAcc(const EsString& name, EsScriptMachine& script)
	{
		EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
		params.addToVariantCollection(name);
		EsCodeSection::Ptr activeMethod = script.getActiveMethod();
		// first, lookup local symbol
		if( activeMethod->findVariable(name) )
		{
			params.addToVariantCollection(EsScriptMachine::vtLocal); // flag symbol not global
			activeMethod->addInstruction(iLoadVarRef, params);
		}
		else if( activeMethod->isObjectMethod() && activeMethod->getThis()->findField(name) )
		{ // lookup object context for field name
			activeMethod->addInstruction(iLoadThisFieldRef, name);
			// add field name to the code section dependency list
			activeMethod->addThisFieldDependency(name);
		}
		else if( script.getStartup()->findVariable(name) )
		{
			params.addToVariantCollection(EsScriptMachine::vtGlobal); // flag symbol as global
			activeMethod->addInstruction( iLoadVarRef, params );
		}
		else
			return false;

		return true;
	}
	
	void compileVarOrConstAcc(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		wxASSERT((*cit).value.id().to_long() == identID);
		EsString name = compileIdent(cit);
		// first, search consts
		EsAccessorIntf::Ptr sym = script.consts().getSymbol(name, false);
		if( !sym && !internalCompileVarAcc(name, script) ) // symbol is neither local nor global variable - throw compile time exception
			EsException::Throw(0, EsException::Generic, wxT("Symbol '%s' is neither either constant, nor local nor global variable"), name.c_str());
		else if( sym )
			// const - load its accessor to instruction parameter
			script.getActiveMethod()->addInstruction(iLoadConstVal, sym);
	}
	
	void compileIdxAcc(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		wxASSERT((*cit).value.id().to_long() == idxAccID);
		wxASSERT((*cit).children.size() == 1);
		compileExpr((*cit).children.begin(), script);
		// issue index access instruction
		script.getActiveMethod()->addInstruction(iLoadItemRef);
	}
	
	void compileFieldAcc(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		wxASSERT((*cit).value.id().to_long() == fieldAccID);
		wxASSERT((*cit).children.size() > 0 && (*cit).children.size() < 3 );
		// retrieve field name from the first child
		ParseTreeConstIteratorT child = (*cit).children.begin();
		script.getActiveMethod()->addInstruction(iLoadFieldRef, compileIdent(child++));
		// handle possible index access instruction
		if( child != (*cit).children.end() )
			compileIdxAcc(child, script);
	}	
	
	void compilePropAcc(ParseTreeConstIteratorT cit, EsScriptMachine& script, bool thisAcc)
	{
		wxASSERT((*cit).value.id().to_long() == propAccID);
		wxASSERT((*cit).children.size() > 0 && (*cit).children.size() < 3 );
		// retrieve property name from the first child
		ParseTreeConstIteratorT child = (*cit).children.begin();
		script.getActiveMethod()->addInstruction(thisAcc ? iLoadThisPropRef : iLoadPropRef, 
			compileIdent(child++));
		// handle possible index access instruction
		if( child != (*cit).children.end() )
			compileIdxAcc(child, script);
	}		
	
	void compileAttrAcc(ParseTreeConstIteratorT cit, EsScriptMachine& script, bool thisAcc)
	{
		wxASSERT(attrAccID == (*cit).value.id().to_long());
		wxASSERT((*cit).children.size() < 2);
		// retrieve attribute name from the first child
		ParseTreeConstIteratorT child = (*cit).children.begin();
		script.getActiveMethod()->addInstruction(thisAcc ? iLoadThisAttributeVal : iLoadAttributeVal, 
			compileIdent(child++, false));
		// handle possible index access instruction
		if( child != (*cit).children.end() )
			compileIdxAcc(child, script);
	}		
		
	void compileObjMethodCall(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		wxASSERT((*cit).value.id().to_long() == objMethodCallID);
		wxASSERT((*cit).children.size() == 1);
		compileExprCall((*cit).children.begin(), script, true);
	}
	
	void compileIncDec(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		wxASSERT((*cit).value.id().to_long() == stmtIncDecID);
		wxASSERT((*cit).children.size() == 1);
		cit = (*cit).children.begin();
		// compile inc dec expression, but do not push result on data stack, as it's statement,
		// so do not care about pre- or postfix ++ or -- as well.
		compileExprIncDec(cit, script, true);
	}
	
	void compileStmtIf(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		wxASSERT((*cit).value.id().to_long() == stmtIfID);
		wxASSERT((*cit).children.size() > 1);
		ParseTreeConstIteratorT child = (*cit).children.begin();
		EsCodeSection::Ptr code = script.getActiveMethod();
		// compile if expression
		compileExpr(child++, script);
		// issue iJumpFalse instruction, saving its positon
		size_t jumpFalsePos = code->addInstruction(iJumpFalse);
		// compile 'true' code section
		compileStmt(child++, script);
		// add instruction to jump to the ending of entire if 
		size_t jumpOutOfIfPos = code->addInstruction(iJump);
		// modify iJumpFalse instruction, so as its target is instruction following the last instruction of the true block
		EsVariant jumpParams(EsVariant::VAR_VARIANT_COLLECTION);
		jumpParams.addToVariantCollection(joAbs)
			.addToVariantCollection(code->instrEndPos());
		code->modifyInstruction(jumpFalsePos, iJumpFalse, jumpParams);
		jumpParams.setToNull();
		// next is optional else section
		if( child != (*cit).children.end() )
		{
			wxASSERT((*child).value.id().to_long());
			child = (*child).children.begin();
			compileStmt(child, script);
		}
		// finalize out-of-if jump instruction
		jumpParams.addToVariantCollection(joAbs)
			.addToVariantCollection(code->instrEndPos());
		code->modifyInstruction(jumpOutOfIfPos, iJump, jumpParams);
	}
	
	void compileForCountExpr(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		wxASSERT((*cit).value.id().to_long() == exprForCntID);
		if(!(*cit).children.empty())
		{
			cit = (*cit).children.begin();
			long id = (*cit).value.id().to_long();
			wxASSERT( opAssignID == id || exprIncDecID == id || exprIncDecPfxID == id );
			if(opAssignID == id)
				compileOpAssign(cit, script, true);
			else
				compileExprIncDec(cit, script, true);
		}
	}
	
	// break|continue automatic guard objects
	class BreakContinueGuard
	{
	protected:
		BreakContinueGuard(EsCodeSection::Ptr code, BreakContinueGuard** guardMember) : 
		m_guardMember(guardMember),
		m_prev(*m_guardMember),
		m_code(code),
		m_fixup(0)
		{
			m_instrPositions.reserve(16);
			*m_guardMember = this;
		}
	
	public:
		~BreakContinueGuard()
		{
			// apply instruction fixups
			while(!m_instrPositions.empty())
			{
				size_t pos = m_instrPositions.back();
				m_instrPositions.pop_back();
				EsVariant jumpParams(EsVariant::VAR_VARIANT_COLLECTION);
				jumpParams.addToVariantCollection(joAbs)
					.addToVariantCollection(m_fixup);
				m_code->modifyInstruction(pos, iJump, jumpParams);
			}
			
			*m_guardMember = m_prev;
		}
		
		void addInstructionPos(size_t pos)
		{
			m_instrPositions.push_back(pos);
		}
		
		void setFixup(size_t fixup)
		{
			m_fixup = fixup;
		}
	
	private:
		// prohibited functionality
		BreakContinueGuard();
		BreakContinueGuard(const BreakContinueGuard&);
		const BreakContinueGuard& operator =(const BreakContinueGuard&);
	
	protected:
		BreakContinueGuard** m_guardMember;
		BreakContinueGuard* m_prev;
		EsCodeSection::Ptr m_code;
		std::vector<size_t> m_instrPositions;
		size_t m_fixup;
			
		friend struct EsScriptCompiler;	
	};
	friend class BreakContinueGuard;
	
	class LabelGuard
	{
	protected:
		LabelGuard(EsCodeSection::Ptr code, EsScriptCompiler& compiler) : 
		m_compiler(compiler),
		m_code(code),
		m_prev(m_compiler.m_labelGuard)
		{
			m_compiler.m_labelGuard = this;
		}	
	
	public:
		~LabelGuard()
		{
			for(Gotos::const_iterator cit = m_gotos.begin(); cit != m_gotos.end(); ++cit)
			{
				const EsString& name = (*cit).first;
				size_t instrPos = (*cit).second;
				// lookup label
				Map::const_iterator lblIt = m_labels.find(name);
				if( m_labels.end() != lblIt )
				{
					EsVariant jumpParams(EsVariant::VAR_VARIANT_COLLECTION);
					jumpParams.addToVariantCollection(joAbs)
						.addToVariantCollection((*lblIt).second);
					m_code->modifyInstruction(instrPos, iJump, jumpParams);
				}
				else
					EsException::Throw(0, EsException::Generic, wxT("goto target '%s' is not declared"));
			}
			
			m_compiler.m_labelGuard = m_prev;
		}
		
		void addLabel(const EsString& name)
		{
			if( m_labels.end() != m_labels.find(name) )
				EsException::Throw(0, EsException::Generic, wxT("label '%s' is already declared"));
			
			m_labels[name] = m_code->instrEndPos();
		}
		
		void addGoto(const EsString& name)
		{
			m_gotos.push_back( Goto(name, m_code->addInstruction(iJump) ) );
		}
		
	private:
		// prohibited functionality
		LabelGuard();
		LabelGuard(const LabelGuard&);
		const LabelGuard& operator =(const LabelGuard&);	
		
	protected:
		typedef containers::structured_map<EsString, size_t> Map;
		typedef std::pair<EsString, size_t> Goto;
		typedef std::list<Goto> Gotos;
		
		EsScriptCompiler& m_compiler;
		EsCodeSection::Ptr m_code;
		Map m_labels;
		Gotos m_gotos;
		LabelGuard* m_prev;

		friend struct EsScriptCompiler;	
	};
	friend class LabelGuard;
	
	void compileStmtFor(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		wxASSERT((*cit).value.id().to_long() == stmtForID);
		wxASSERT((*cit).children.size()==4);
		
		EsCodeSection::Ptr code = script.getActiveMethod();
		// set-up break and continue guards
		BreakContinueGuard breakGuard(code, &m_breakGuard);
		BreakContinueGuard continueGuard(code, &m_continueGuard);
		// compile for init statement
		ParseTreeConstIteratorT child = (*cit).children.begin();
		if(!(*child).children.empty())
			compileStmt((*child).children.begin(), script);
		++child;
		// save current instruction position as loop start position
		size_t loopStartPos = code->instrEndPos();
		// compile check expression
		int forCheckPos = -1;
		if(!(*child).children.empty())
		{
			compileExpr((*child).children.begin(), script);
			// issue loop check instruction, save position for later
			forCheckPos = code->addInstruction(iJumpFalse);
		}
		++child;
		// save counter expression block for later
		ParseTreeConstIteratorT cntExpr = child++;
		// compile _for_ code block
		compileStmt(child, script);
		// save _for_ 'continue' point
		size_t continuePos = code->instrEndPos();
		// finally, compile _for_ counter expression
		compileForCountExpr(cntExpr, script);
		// do jump to the loop start point
		EsVariant jumpParams(EsVariant::VAR_VARIANT_COLLECTION);
		jumpParams.addToVariantCollection(joAbs)
			.addToVariantCollection(loopStartPos);
		code->addInstruction(iJump, jumpParams);
		// set break|continue fixups
		breakGuard.setFixup(code->instrEndPos());
		continueGuard.setFixup(loopStartPos);
		// modify iJumpFalse to jump out of the _for_ loop
		if( -1 < forCheckPos )
		{
			jumpParams.setToNull();
			jumpParams.addToVariantCollection(joAbs)
				.addToVariantCollection(code->instrEndPos());
			code->modifyInstruction(forCheckPos, iJumpFalse, jumpParams);
		}
	}

	void compileStmtWhile(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		long id = (*cit).value.id().to_long();
		wxASSERT(stmtWhileID == id || stmtDoWhileID == id);
		wxASSERT((*cit).children.size()==2);

		EsCodeSection::Ptr code = script.getActiveMethod();
		// set-up break and continue guards
		BreakContinueGuard breakGuard(code, &m_breakGuard);
		BreakContinueGuard continueGuard(code, &m_continueGuard);		
		// save loop start point
		size_t loopStart = code->instrEndPos();
		ParseTreeConstIteratorT child = (*cit).children.begin();
		size_t whileCheckPos;
		if( stmtWhileID == id )
		{
			// compile check expression
			wxASSERT(!(*child).children.empty());
			compileExpr((*child).children.begin(), script);
			// issue loop check instruction, saving position for later
			whileCheckPos = code->addInstruction(iJumpFalse);
			++child;
		}
		// compile _while_ code block
		compileStmt(child, script);
		// compile check expression
		EsVariant jumpParams(EsVariant::VAR_VARIANT_COLLECTION);
		if( stmtDoWhileID == id )
		{
			whileCheckPos = code->instrEndPos();
			++child;
			// compile check expression
			wxASSERT(!(*child).children.empty());
			compileExpr((*child).children.begin(), script);
			// jump, if true, to the beginning of the loop
			jumpParams.addToVariantCollection(joAbs)
				.addToVariantCollection(loopStart);
			whileCheckPos = code->addInstruction(iJumpTrue, jumpParams);
		}
		else
		{
			// issue iJump instruction to the start of the loop section
			jumpParams.addToVariantCollection(joAbs)
				.addToVariantCollection(loopStart);
			code->addInstruction(iJump, jumpParams);
			// modify while check instruction
			jumpParams.setToNull();
			jumpParams.addToVariantCollection(joAbs)
				.addToVariantCollection(code->instrEndPos());
			code->modifyInstruction(whileCheckPos, iJumpFalse, jumpParams);
		}
		// set break|continue fixups
		breakGuard.setFixup(code->instrEndPos());
		continueGuard.setFixup( (stmtWhileID == id) ? loopStart : whileCheckPos );		
	}
	
	// compile break and continue
	void compileStmtBreakContinue(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		long id = (*cit).value.id().to_long();
		wxASSERT(stmtBreakID == id || stmtContinueID == id);
		EsCodeSection::Ptr code = script.getActiveMethod();
		if(stmtBreakID == id)
		{
			if( m_breakGuard ) 
				m_breakGuard->addInstructionPos(code->addInstruction(iJump));
			else
				EsException::Throw(0, EsException::Generic, wxT("'break' without switch or loop block"));
		}
		else if( m_continueGuard ) 
				m_continueGuard->addInstructionPos(code->addInstruction(iJump));
			else
				EsException::Throw(0, EsException::Generic, wxT("'continue' without loop block"));
	}
	
	void compileStmtLabelGoto(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		long id = (*cit).value.id().to_long();
		wxASSERT(stmtLabelID == id || stmtGotoID == id);
		wxASSERT(0 != m_labelGuard);
		EsString name = compileIdent((*cit).children.begin());
		if( stmtLabelID == id )
			m_labelGuard->addLabel( name );
		else
			m_labelGuard->addGoto( name );
	}

	void compileStmtObjDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		long id = (*cit).value.id().to_long();
		wxASSERT( id == stmtObjDeclID );
		wxASSERT( (*cit).children.size() > 0 );
		ParseTreeConstIteratorT child = (*cit).children.begin();
		EsString oldNamespace = m_objectOrNamespace;
		m_objectOrNamespace = compileIdent(child++);
		EsString baseObj;
		if(identID == (*child).value.id().to_long())
			baseObj = compileIdent(child++);
		EsScriptObjectIntf::Ptr metaclass = script.declareMetaclass(m_objectOrNamespace, baseObj);
		
		// optionally compile attributes
		if( child != (*cit).children.end() &&
				(stmtAttributeDeclID == (*child).value.id().to_long() ||
				 stmtAttributeListID == (*child).value.id().to_long()) )
			compileStmtObjAttrDecls(metaclass, child++, script);
		
		// dive in metaclass compilation
		if( child != (*cit).children.end() )
			compileStmt(child, script);
		// restore m_objectOrNamespace
		m_objectOrNamespace = oldNamespace;
	}
	
	EsScriptObjectIntf::Ptr compileStmtObjFldDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		long id = (*cit).value.id().to_long();
		wxASSERT(stmtObjPlainFldDeclID == id || stmtObjArrayFldDeclID == id);
		wxASSERT((*cit).children.size() >= 2);
		ParseTreeConstIteratorT child = (*cit).children.begin();
		// compile field type
		EsString fieldType = compileIdent(child++);
		// compile field name
		EsString fieldName = compileIdent(child++);
		if( stmtObjPlainFldDeclID == id )
			// add field to metaclass currently being compiled
			return script.declareMetaclassField(m_objectOrNamespace, fieldType, fieldName);
		else
		{
			// save old script method
			EsCodeSection::Ptr oldCode = script.getActiveMethod();
			// create new script method
			EsCodeSection::Ptr sizeExpr = script.createMethod(fieldName+wxT("_sizeExpr"), m_objectOrNamespace);
			// try to add newly created method to the script machine code table
			script.addMethod(sizeExpr);
			// set size expression as an active code section
			script.setActiveMethod(sizeExpr);
			// compile array size expression and declare array field
			compileExpr(child++, script);
			EsScriptObjectIntf::Ptr result = script.declareMetaclassArrayField(m_objectOrNamespace, fieldType, fieldName, sizeExpr);
			// restore old code section
			script.setActiveMethod(oldCode);
			return result;
		}
	}
	
	void compileStmtObjAttrDecls(EsScriptObjectIntf::Ptr obj, ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		long id = (*cit).value.id().to_long();
		wxASSERT(stmtAttributeListID == id || stmtAttributeDeclID == id);
		if(stmtAttributeListID == id)
		{
			for( ParseTreeConstIteratorT attrIt = (*cit).children.begin(); attrIt != (*cit).children.end(); ++attrIt )
				compileStmtObjAttrDecls(obj, attrIt, script);
		}
		else
		{
			wxASSERT(0 < (*cit).children.size() && (*cit).children.size() <= 2 );
			ParseTreeConstIteratorT child = (*cit).children.begin();
			EsString name = compileIdent(child++, false);
			EsVariant val;
			// compile optional value
			if( child != (*cit).children.end() )
				val = compileValue(child, script);
			script.declareObjectAttribute(obj, name, val);
		}
	}
	
	void compileStmtObjFldWithAttrsDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		long id = (*cit).value.id().to_long();
		wxASSERT(stmtObjFldDeclID == id);
		wxASSERT((*cit).children.size() == 2);
		ParseTreeConstIteratorT child = (*cit).children.begin();
		EsScriptObjectIntf::Ptr field = compileStmtObjFldDecl(child++, script);
		wxASSERT(field);
		// compile field attributes
		compileStmtObjAttrDecls(field, child, script);
	}
	
	// expression compilers
	//
	void compileExpr(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		long id = (*cit).value.id().to_long();
		switch(id)
		{
		// binary operation expressions
		case opIncDecID:
		case opLogicalID:
		case opBitwiseID:
		case opEqNeqID:
		case opCmpID:
		case opShID:
		case opAddSubID:
		case opMulDivID:
			compileExprBinaryOp(cit, script);
			break;
		case opUnaryID:
			compileExprUnaryOp(cit, script);
			break;
		case exprIncDecID:
		case exprIncDecPfxID:
			compileExprIncDec(cit, script);
			break;
		case exprFuncCallID:
		case exprObjCtrCallID:
		case exprStaticCallID:
			compileExprCall(cit, script);
			break;
		// simple value. just push it onto data stack as constant	
		case boolValID:
		case intValID:
		case floatValID:
		case strValID:
		case arrayValID:
			script.getActiveMethod()->addInstruction(iLoadConstVal, 
				script.declUnnamed(compileValue(cit, script)) );
			break;
		case lrhsID:
		case identID:
		case propAccID: 
		case attrAccID:
			compileLrhs(cit, script);
			break;
		case opAssignID:
			compileOpAssign(cit, script);
			break;
		}
	}
	
	void compileExprCall(ParseTreeConstIteratorT cit, EsScriptMachine& script, bool explicitObjMethodCall = false)
	{
		long id = (*cit).value.id().to_long();
		wxASSERT(id >= exprFuncCallID && id < exprCallID);
		wxASSERT((*cit).children.size() > 0);
		
		if( explicitObjMethodCall )
			compileExplicitMethodCall(cit, script);
		else
		{
			switch(id)
			{
			case exprObjCtrCallID:
				compileObjectCtrCall((*cit).children.begin(), script);
				break;
			case exprStaticCallID:
				// extract object or namespace name
				compileStaticOrBaseCall((*cit).children.begin(), script);
				break;
			case exprFuncCallID:
				compileFunctionOrMethodCall(cit, script);
				break;
			}
		}
	}
	
	void compileObjectCtrCall(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		// compile object constructor expression
		// move to the function expression at once
		ParseTreeConstIteratorT child = (*cit).children.begin();
		EsString objectName = compileIdent(child++);
		// get call parameters count
		size_t paramsCount = (*cit).children.size()-1;
		// compile parameter expressions
		while(child != (*cit).children.end())
			compileExpr(child++, script);
		// format instruction parameters
		EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
		params
			.addToVariantCollection(paramsCount)
			.addToVariantCollection(objectName);			
		// issue constructor call instruction		
		script.getActiveMethod()->addInstruction(iConstructorCall, params);
	}
	
	void compileExplicitMethodCall(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		// the target object namespace is known only at runtime, when explicit method call
		// syntax is used, so issue reduced form of iMethodCall instruction, without namespace parameter
		//
		// move to the function expression
		ParseTreeConstIteratorT child = (*cit).children.begin();
		EsString name = compileIdent(child++);
		// get call parameters count
		size_t paramsCount = (*cit).children.size()-1;
		// compile parameter expressions
		while(child != (*cit).children.end())
			compileExpr(child++, script);
		// format instruction parameters
		EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
		params
			.addToVariantCollection(paramsCount)
			.addToVariantCollection(name);
		// finally, issue call instruction, specifying call parameters
		script.getActiveMethod()->addInstruction(iMethodCall, params);
	}
	
	void compileStaticOrBaseCall(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		// default call namespace is the current one
		EsString objectOrNamespace = script.getActiveMethod()->getNamespace();
		// if the call namespace is script object, retrieve pointer to its metaclass
		EsScriptObjectIntf::Ptr obj = script.findMetaclass(objectOrNamespace);
		EsString targetNamespace = compileIdent(cit++);
		EsScriptObjectIntf::Ptr targetObj = script.findMetaclass(targetNamespace);	
		// move to the function expression
		ParseTreeConstIteratorT child = (*cit).children.begin();
		EsString name = compileIdent(child++);
		// get call parameters count
		size_t paramsCount = (*cit).children.size()-1;
		EsInstructionOpcode opcode = iFuncCall;
		MethodInfoKeyT methodKey(paramsCount, name);
		EsCodeSection::Ptr method;
		// decide on static call|override call
		if( targetObj )
		{
			if( obj )
			{
				if(	obj->isKindOf(targetObj.get()) )
				{
					// lookup base class method
					method = targetObj->findScriptedMethod( methodKey );
					if( method )
						// set call instruction opcode to the method call
						opcode = iMethodCall;
					else
						EsException::Throw(0, EsException::Generic, wxT("'%s' is not method of '%s' or its base objects"),
							name.c_str(), targetNamespace.c_str());
							
				}
				else
					EsException::Throw(0, EsException::Generic, wxT("'%s' is not base object of '%s'"),
						targetNamespace.c_str(), objectOrNamespace.c_str());
			}
			else
				EsException::Throw(0, EsException::Generic, wxT("Cannot call object method '%s' without an object"), 
					name.c_str());
		}
		else
		{
			// lookup script namespace first
			method = script.getMethodTemplate(methodKey, targetNamespace, false);
			if( !method && 
					EsString::cmpEqual != targetNamespace.compare(EsScriptMachine::s_global) )
			{
				// lookup global namespace
				method = script.getMethodTemplate(methodKey, EsScriptMachine::s_global, false);
				if( method )
					targetNamespace = EsScriptMachine::s_global;
			}
			// lookup external static service as last resort
			if( !method )
			{
				const ClassInfo* info = ClassInfo::getClassInfo(targetNamespace, false);
				if( info )
				{
					const MethodInfo* methodInfo = info->findClassMethodInfo(methodKey);
					if( methodInfo )
						opcode = iExtFuncCall;
					else
						EsException::Throw(0, EsException::Generic, wxT("'%s::%s' is not a known method"),
							targetNamespace.c_str(), name.c_str());
				}
			}
		}
			
		// compile parameter expressions
		while(child != (*cit).children.end())
			compileExpr(child++, script);
		// format instruction parameters
		EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
		params
			.addToVariantCollection(paramsCount)
			.addToVariantCollection(name)
			.addToVariantCollection(targetNamespace);
		// finally, issue call instruction, specifying call parameters
		script.getActiveMethod()->addInstruction(opcode, params);				
	}		
	
	void compileFunctionOrMethodCall(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		// get current namespace or object name
		EsString objectOrNamespace = script.getActiveMethod()->getNamespace();
		ParseTreeConstIteratorT child = (*cit).children.begin();
		// get function or method name
		EsString name = compileIdent(child++);
		// get call parameters count
		size_t paramsCount = (*cit).children.size()-1;
		// create method search key
		MethodInfoKeyT methodKey(paramsCount, name);
		// initialize instruction opcode
		EsInstructionOpcode opcode = iFuncCall;
		// find if current namespace is scripted object
		EsScriptObjectIntf::Ptr obj = script.findMetaclass(objectOrNamespace);
		EsCodeSection::Ptr method;
		if( obj )
		{
			// if so, try to find the object's method
			method = obj->findScriptedMethod(methodKey);
			// if there is such method - call it
			if( method )
				opcode = iMethodCall;
		}
		
		if( !method )
		{
			// lookup current namespace first
			method = script.getMethodTemplate(methodKey, objectOrNamespace, false);
			if( !method && 
					EsString::cmpEqual != objectOrNamespace.compare(EsScriptMachine::s_global) ) // otherwise, lookup global namespace
			{		
				method = script.getMethodTemplate(methodKey, EsScriptMachine::s_global, false);
				if( method )
					objectOrNamespace = EsScriptMachine::s_global;
			}
		}
		
		if( !method )
			EsException::Throw(0, EsException::Generic, wxT("'%s' is not declared"), name.c_str());
			
		// compile parameter expressions
		while(child != (*cit).children.end())
			compileExpr(child++, script);
			
		// format instruction parameters
		EsVariant params(EsVariant::VAR_VARIANT_COLLECTION);
		params
			.addToVariantCollection(paramsCount)
			.addToVariantCollection(name)
			.addToVariantCollection(objectOrNamespace);			
		// issue constructor call instruction		
		script.getActiveMethod()->addInstruction(opcode, params);		
	}
	
	// binary operation expression compiler
	void compileExprBinaryOp(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		long id = (*cit).value.id().to_long();
		EsString op((*cit).value.begin(), (*cit).value.end());
		wxASSERT(opIncDecID <= id && id <= opMulDivID);
		wxASSERT((*cit).children.size() == 2);
		ParseTreeConstIteratorT child = (*cit).children.begin();
		// check for short logical expression eval,
		// perform full eval for others
		// evaluate left part
		compileExpr(child++, script);
		size_t instrPos = 0;
		size_t jumpOffs = 0;
		if( opLogicalID == id ) // issue short logical eval check instruction between left & right evals
			instrPos = script.getActiveMethod()->addInstruction(iLogicCheck); // save current instruction idx
		// evaluate right part
		compileExpr(child++, script);
		// issue binary operation instruction, calculate jump offset for logical check
		jumpOffs = script.getActiveMethod()->addInstruction(iBinOpExpr, s_opids.getId(op)) - instrPos;
		// finally, modify logcheck instruction at instrPos, if needed
		if( opLogicalID == id )
			script.getActiveMethod()->modifyInstruction(instrPos, iLogicCheck, jumpOffs);
	}
	
	void compileExprUnaryOp(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		wxASSERT((*cit).value.id().to_long() == opUnaryID);
		EsScriptOperatorIds opid = s_opids.getId( EsString((*cit).value.begin(), (*cit).value.end()) );
		wxASSERT(opid == opidBwNot || opid == opidNot || opid == opidSub || opid == opidExtScope);
		wxASSERT((*cit).children.size()==1);
		// compile operand expression
		compileExpr((*cit).children.begin(), script);
		// issue unary operator instruction
		script.getActiveMethod()->addInstruction(iUnaryOpExpr, opid);
	}
	
	void compileExprIncDec(ParseTreeConstIteratorT cit, EsScriptMachine& script, bool asStmt = false)
	{
		long id = (*cit).value.id().to_long();
		wxASSERT(id == exprIncDecID || id == exprIncDecPfxID);
		ParseTreeConstIteratorT incDec;
		ParseTreeConstIteratorT operand;
		
		if( exprIncDecID == id )
		{
			incDec = (*cit).children.begin();
			operand = incDec+1;
		}
		else
		{
			operand = (*cit).children.begin();		
			incDec = operand+1;
		}
		
		EsString op((*incDec).value.begin(), (*incDec).value.end());
		EsScriptOperatorIds opid = s_opids.getId(op);
		wxASSERT(opidInc == opid || opidDec == opid);
		// first, evaluate operand as lhs
		compileLrhs(operand, script);
		// second, issue incdec instruction specifying statement mode, opid, and pre-postfix, as parameters
		EsVariant param(EsVariant::VAR_VARIANT_COLLECTION);
		param.addToVariantCollection(asStmt)
				 .addToVariantCollection(opid)
				 .addToVariantCollection(id == exprIncDecPfxID);
		script.getActiveMethod()->addInstruction(iIncDec, param);
	}
	
	// identifier and value compilers
	//
	EsString compileIdent(ParseTreeConstIteratorT cit, bool checkReserved = true)
	{
		wxASSERT((*cit).value.id().to_long() == identID);
		EsString result((*cit).value.begin(), (*cit).value.end());
		if( checkReserved ) 
			EsScript::checkNotReservedWord(result);
		
		return result;
	}
	
	EsVariant compileValue(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		long id = (*cit).value.id().to_long();
		wxASSERT( id > simpleValID && id <= arrayValID );
		if( id == arrayValID )
			return compileArrayValue(cit, script);
		else
		{
			EsString strVal((*cit).value.begin(), (*cit).value.end());
			EsVariant var(strVal);
			switch( id )
			{
			case boolValID:
				return var.asBool(script.loc());
			case intValID:
				return var.asLong(script.loc());			
			case floatValID:
				return var.asDouble(script.loc());
			case strValID:
				var = EsString::fromString( strVal, EsString::StrQuote, script.loc() );
				return var;
			default:
				// we should never be here
				wxFAIL;
				break;
			}
			// pacify compiler
			return EsVariant::s_null;
		}
	}
	
	// compile array value
	EsVariant compileArrayValue(ParseTreeConstIteratorT cit, EsScriptMachine& script)
	{
		EsVariant result(EsVariant::VAR_VARIANT_COLLECTION);
		wxASSERT( arrayValID == (*cit).value.id().to_long() );
		for(ParseTreeConstIteratorT	child = (*cit).children.begin(); child != (*cit).children.end(); ++child)
			result.addToVariantCollection( compileValue(child, script) );

		return result;
	}

public:	
	// data members
	//
	// static operator symbol table, to augment compilation
	static EsScriptCompilerOpIds s_opids;	
	
protected:
	// object|namespace declaration scope currently being compiled 
	EsString m_objectOrNamespace;
	BreakContinueGuard* m_breakGuard;
	BreakContinueGuard* m_continueGuard;
	LabelGuard* m_labelGuard;
	
PARSER_CONTEXT_IMPL_END

// s_opids static initializer
EsScriptCompilerOpIds EsScriptCompiler::s_opids;

EsScriptCompilerOpIds::EsScriptCompilerOpIds()
{
	// incdec
	//
	m_idsMap[wxT("++")] = opidInc;
	m_idsMap[wxT("--")] = opidDec;
	// unary operators
	//
	m_idsMap[wxT("~")] = opidBwNot;
	m_idsMap[wxT("!")] = opidNot;
	m_idsMap[wxT("::")] = opidExtScope;
	// binary operators
	//
	// access
	m_idsMap[wxT(".")] = opidFldAcc;
	m_idsMap[wxT("$")] = opidPropAcc;
	m_idsMap[wxT("@")] = opidAttrAcc;
	m_idsMap[wxT("::")] = opidScopeAcc;
	// bitwise
	m_idsMap[wxT("&")] = opidBwAnd;
	m_idsMap[wxT("|")] = opidBwOr;
	m_idsMap[wxT("^")] = opidBwXor;
	// math
	m_idsMap[wxT("<<")] = opidLsh;
	m_idsMap[wxT(">>")] = opidRsh;
	m_idsMap[wxT("+")] = opidAdd;
	m_idsMap[wxT("-")] = opidSub;
	m_idsMap[wxT("*")] = opidMul;
	m_idsMap[wxT("/")] = opidDiv;
	m_idsMap[wxT("%")] = opidMod;
	// logical
	m_idsMap[wxT("||")] = opidLogOr;
	m_idsMap[wxT("&&")] = opidLogAnd;
	m_idsMap[wxT("<")] = opidLess;
	m_idsMap[wxT(">")] = opidGreater;
	m_idsMap[wxT("==")] = opidEqual;
	m_idsMap[wxT("!=")] = opidNotEqual;
	m_idsMap[wxT("<=")] = opidLeEqual;
	m_idsMap[wxT(">=")] = opidGrEqual;
	// assignments
	m_idsMap[wxT("=")] = opidAsn;
	m_idsMap[wxT("-=")] = opidAsnDec;
	m_idsMap[wxT("+=")] = opidAsnInc;
	m_idsMap[wxT("/=")] = opidAsnDiv;
	m_idsMap[wxT("%=")] = opidAsnMod;
	m_idsMap[wxT("*=")] = opidAsnMul;
	m_idsMap[wxT("|=")] = opidAsnOr;
	m_idsMap[wxT("&=")] = opidAsnAnd;
	m_idsMap[wxT("^=")] = opidAsnXor;
	m_idsMap[wxT("<<=")] = opidAsnLsh;
	m_idsMap[wxT(">>=")] = opidAsnRsh;
}

EsScriptOperatorIds EsScriptCompilerOpIds::getId(const EsString& op) const
{
	EsScriptOperatorIds result = opidInvalid;
	Map::const_iterator cit = m_idsMap.find(op);
	if( cit != m_idsMap.end() )
		result = (*cit).second;
	else
		EsException::Throw(0, EsException::Generic, wxT("Could not find script operator id for '%s'"), op.c_str());
		
	return result;
}

const EsString& EsScriptCompilerOpIds::getIdStr(long id) const
{
	for( Map::const_iterator cit = m_idsMap.begin(); cit != m_idsMap.end(); ++cit )
	{
		if( id == (*cit).second )
			return (*cit).first;
	}

	return EsString::s_null;
}