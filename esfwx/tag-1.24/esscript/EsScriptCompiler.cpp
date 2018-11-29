#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptCompiler.h"

#ifndef ES_USE_COMPILER_IMPLEMENTATION
# define ES_USE_COMPILER_IMPLEMENTATION 0
#endif

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#if (1 == ES_USE_COMPILER_IMPLEMENTATION)
#include "EsVar.h"
#include "EsStr.h"
#include "EsScriptThreadContext.h"

// generate AST structure in xml at the end of successfull compilation
# ifdef ESSCRIPT_COMPILER_USE_TRACE
#   ifndef ES_DUMP_AST_TO_XML
#     define ES_DUMP_AST_TO_XML
#   endif
# endif

#include <escore/EsGrammarDefs.h>

#ifdef _DEBUG
# ifdef BOOST_SPIRIT_DEBUG
# define BOOST_SPIRIT_DEBUG_PRINT_SOME 64
#   include <fstream>
    GRAMMAR_DEF_DEBUG_DUMP_DEST(EsScriptCompiler)
#   define BOOST_SPIRIT_DEBUG_OUT   EsScriptCompilerDumpDest
# endif // BOOST_SPIRIT_DEBUG
#endif

#ifdef ESSCRIPT_COMPILER_USE_TRACE
# define ESSCRIPT_COMPILER_TRACE1(a)                  ES_DEBUG_TRACE(a);
# define ESSCRIPT_COMPILER_TRACE2(a, b)               ES_DEBUG_TRACE((a), (b));
# define ESSCRIPT_COMPILER_TRACE3(a, b, c)            ES_DEBUG_TRACE((a), (b), (c));
# define ESSCRIPT_COMPILER_TRACE4(a, b, c, d)         ES_DEBUG_TRACE((a), (b), (c), (d));
# define ESSCRIPT_COMPILER_TRACE5(a, b, c, d, e)      ES_DEBUG_TRACE((a), (b), (c), (d), (e));
#else
# define ESSCRIPT_COMPILER_TRACE1(a)
# define ESSCRIPT_COMPILER_TRACE2(a, b)
# define ESSCRIPT_COMPILER_TRACE3(a, b, c)
# define ESSCRIPT_COMPILER_TRACE4(a, b, c, d)
# define ESSCRIPT_COMPILER_TRACE5(a, b, c, d, e)
#endif

// avoid compilation error in BCC32
#if (ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND) && \
    (ES_OS == ES_OS_WINDOWS) && !defined(ES_CLANG)
# define BOOST_NO_IS_ABSTRACT
#endif

// spirit-boost includes (current boost installation)
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_error_handling.hpp>
#include <boost/spirit/include/classic_ast.hpp>
#include <boost/spirit/include/classic_confix.hpp>
#include <boost/spirit/include/classic_lists.hpp>
#include <boost/spirit/include/classic_escape_char.hpp>
#include <boost/spirit/include/classic_symbols.hpp>
#include <boost/spirit/include/classic_chset.hpp>
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// Handle non-standard char types cases
#if !defined(ES_USE_NARROW_ES_CHAR) && !defined(ES_CHAR_IS_WCHAR_T)
namespace boost { namespace spirit {

BOOST_SPIRIT_CLASSIC_NAMESPACE_BEGIN

  inline chset<EsString::value_type>
  chset_p(EsString::value_type const* init)
  { return chset<EsString::value_type>(init); }

  inline chset<EsString::value_type>
  chset_p(EsString::value_type ch)
  { return chset<EsString::value_type>(ch); }

  struct eschar_as_parser
  {
      typedef chlit<EsString::value_type> type;
      static type convert(EsString::value_type ch)
      {
          return type(ch);
      }
  };

  struct esstring_as_parser
  {
      typedef strlit<EsString::value_type const*> type;
      static type convert(EsString::value_type const* str)
      {
          return type(str);
      }
  };

  template<>
  struct as_parser<EsString::value_type> : eschar_as_parser {};

  template<>
  struct as_parser<EsString::value_type*> : esstring_as_parser {};

  template<>
  struct as_parser<EsString::value_type const*> : esstring_as_parser {};

  template<int N>
  struct as_parser<EsString::value_type[N]> : esstring_as_parser {};

  template<int N>
  struct as_parser<EsString::value_type const[N]> : esstring_as_parser {};

BOOST_SPIRIT_CLASSIC_NAMESPACE_END

}} // namespace BOOST_SPIRIT_CLASSIC_NS

#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

using namespace boost::spirit::classic;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// context prerequisites
enum { EsScriptCompiler_errCntMax = 255 };
PARSER_CONTEXT_PREREQUISITES_BEGIN( EsScriptCompiler, node_iter_data_factory<> )
  GRAMMAR_RULE_IDS_BEGIN
    GRAMMAR_DEF_RULE_ID(skip)
#   define GRAMMAR_RULE GRAMMAR_DEF_RULE_ID
#   include "EsScriptCompilerRuleNames.hxx"
  GRAMMAR_RULE_IDS_END
  PARSER_CONTEXT_DEF_ERROR_BEGIN
    PARSER_CONTEXT_DEF_ERROR(Parser)
    PARSER_CONTEXT_DEF_EXPECT(COLON)
    PARSER_CONTEXT_DEF_EXPECT(SEMICOLON)
    PARSER_CONTEXT_DEF_EXPECT(RCBRACKET)
    PARSER_CONTEXT_DEF_EXPECT(RSBRACKET)
    PARSER_CONTEXT_DEF_EXPECT(RRBRACKET)
    PARSER_CONTEXT_DEF_EXPECT(SNGLQUOTE)
    PARSER_CONTEXT_DEF_EXPECT(DBLQUOTE)
    PARSER_CONTEXT_DEF_EXPECT(OPATTRACC)
    PARSER_CONTEXT_DEF_EXPECT(OPIN)
    PARSER_CONTEXT_DEF_EXPECT(value)
    PARSER_CONTEXT_DEF_EXPECT(ident)
  PARSER_CONTEXT_DEF_ERROR_END
  PARSER_CONTEXT_DEF_ERROR_DESCR_BEGIN
    PARSER_CONTEXT_DEF_ERROR_DESCR(Parser,      esT("parser error"))
    PARSER_CONTEXT_DEF_EXPECT_DESCR(COLON,      esT("expected :"))
    PARSER_CONTEXT_DEF_EXPECT_DESCR(SEMICOLON,  esT("expected ;"))
    PARSER_CONTEXT_DEF_EXPECT_DESCR(RCBRACKET,  esT("expected }"))
    PARSER_CONTEXT_DEF_EXPECT_DESCR(RSBRACKET,  esT("expected ]"))
    PARSER_CONTEXT_DEF_EXPECT_DESCR(RRBRACKET,  esT("expected )"))
    PARSER_CONTEXT_DEF_EXPECT_DESCR(SNGLQUOTE,  esT("expected '"))
    PARSER_CONTEXT_DEF_EXPECT_DESCR(DBLQUOTE,   esT("expected \""))
    PARSER_CONTEXT_DEF_EXPECT_DESCR(OPATTRACC,  esT("expected @"))
    PARSER_CONTEXT_DEF_EXPECT_DESCR(OPIN,       esT("expected 'in'"))
    PARSER_CONTEXT_DEF_EXPECT_DESCR(value,      esT("expected integer, float, string, or array value"))
    PARSER_CONTEXT_DEF_EXPECT_DESCR(ident,      esT("expected identifier"))
  PARSER_CONTEXT_DEF_ERROR_DESCR_END
PARSER_CONTEXT_PREREQUISITES_END

// grammar parser
GRAMMAR_PARSER_IMPL_BEGIN( EsScriptCompiler, Grammar )
// named char literals
# define SEMICOLON  ch_p(esT(';'))
# define COMMA      ch_p(esT(','))
# define SNGLQUOTE  ch_p(esT('\''))
# define DBLQUOTE   ch_p(esT('"'))
# define LCBRACKET  ch_p(esT('{'))
# define RCBRACKET  ch_p(esT('}'))
# define LSBRACKET  ch_p(esT('['))
# define RSBRACKET  ch_p(esT(']'))
# define LRBRACKET  ch_p(esT('('))
# define RRBRACKET  ch_p(esT(')'))
# define UNDERSCORE ch_p(esT('_'))
# define COLON      ch_p(esT(':'))
# define QUESTION   ch_p(esT('?'))
// operators
# define OPINC      str_p(esT("++"))
# define OPDEC      str_p(esT("--"))
# define OPLOG_OR   str_p(esT("||"))
# define OPLOG_AND  str_p(esT("&&"))
# define OPBW_AND   ch_p(esT('&'))
# define OPBW_OR    ch_p(esT('|'))
# define OPBW_XOR   ch_p(esT('^'))
# define OPBW_NOT   ch_p(esT('~'))
# define OPLSH      str_p(esT("<<"))
# define OPRSH      str_p(esT(">>"))
# define PLUS       ch_p(esT('+'))
# define MINUS      ch_p(esT('-'))
# define OPMUL      ch_p(esT('*'))
# define OPDIV      ch_p(esT('/'))
# define OPMOD      ch_p(esT('%'))
# define OPNOT      ch_p(esT('!'))
// access
# define OPMTDACC   ch_p(esT('.'))
# define OPFLDACC   ch_p(esT('.'))
# define OPPROPACC  ch_p(esT('$'))
# define OPENUMACC  str_p(esT("$$"))
# define OPVSVCACC  ch_p(esT('#'))
# define OPATTRACC  ch_p(esT('@'))
# define OPSCOPEACC str_p(esT("::"))
// misc
# define OPRANGE    str_p(esT(".."))
# define PREPROC    str_p(esT("##"))
// comparison
# define OPLE       ch_p(esT('<'))
# define OPGR       ch_p(esT('>'))
# define OPEQ       str_p(esT("=="))
# define OPNEQ      str_p(esT("!="))
# define OPIN       str_p(esT("in"))
# define OPLEEQ     str_p(esT("<="))
# define OPGREQ     str_p(esT(">="))
// assignments
# define ASN        ch_p(esT('='))
# define ASNDEC     str_p(esT("-="))
# define ASNINC     str_p(esT("+="))
# define ASNDIV     str_p(esT("/="))
# define ASNMOD     str_p(esT("%="))
# define ASNMUL     str_p(esT("*="))
# define ASNOR      str_p(esT("|="))
# define ASNAND     str_p(esT("&="))
# define ASNXOR     str_p(esT("^="))
# define ASNLSH     str_p(esT("<<="))
# define ASNRSH     str_p(esT(">>="))
// reserved words
//
# define REQUIRE    str_p(esT("require"))
# define LINKBIN    str_p(esT("link"))
# define VARIABLE   str_p(esT("var"))
# define CONSTANT   str_p(esT("const"))
# define CTOR       str_p(EsStdNames::reflectedCtr().c_str())
# define DTOR       str_p(EsStdNames::reflectedDtr().c_str())
# define ENUM       str_p(esT("enum"))
# define FUNCTION   str_p(esT("function"))
# define RETURN     str_p(esT("return"))
# define NEWOBJ     str_p(EsStdNames::reflectedCtr().c_str())
# define EXTSYM     str_p(esT("extern"))
# define OBJECT     str_p(esT("object"))
# define EXTENDS    str_p(esT("extends"))
# define BTRUE      str_p(esT("true"))
# define BFALSE     str_p(esT("false"))
# define NULLC      str_p(esT("null"))
# define IF         str_p(esT("if"))
# define ELSE       str_p(esT("else"))
# define FOR        str_p(esT("for"))
# define FOREACH    str_p(esT("foreach"))
# define WHILE      str_p(esT("while"))
# define DO         str_p(esT("do"))
# define BREAK      str_p(esT("break"))
# define CONTINUE   str_p(esT("continue"))
# define SWITCH     str_p(esT("switch"))
# define CASE       str_p(esT("case"))
# define DEFAULT    str_p(esT("default"))
# define LABEL      str_p(esT("label"))
# define GOTO       str_p(esT("goto"))
# define THROWEX    str_p(esT("throw"))
# define RETHROWEX  str_p(esT("rethrow"))
# define TRYEX      str_p(esT("try"))
# define CATCHEX    str_p(esT("catch"))
# define PROPERTY   str_p(esT("property"))
# define READER     str_p(esT("read"))
# define WRITER     str_p(esT("write"))
// rules
//
# define GRAMMAR_RULE GRAMMAR_DEF_RULE
# include "EsScriptCompilerRuleNames.hxx"
  symbols<int, EsString::value_type> reservedWords;
  // Full script grammar entry point
  GRAMMAR_DEF_ENTRY(script)

  GRAMMAR_PARSER_DEFINITION_IMPL_BEGIN
    // error support
    GRAMMAR_PARSER_USE_ASSERT_GUARD
    GRAMMAR_ASSERT_DEF_EXPECT(COLON)
    GRAMMAR_ASSERT_DEF_EXPECT(SEMICOLON)
    GRAMMAR_ASSERT_DEF_EXPECT(RCBRACKET)
    GRAMMAR_ASSERT_DEF_EXPECT(RSBRACKET)
    GRAMMAR_ASSERT_DEF_EXPECT(RRBRACKET)
    GRAMMAR_ASSERT_DEF_EXPECT(SNGLQUOTE)
    GRAMMAR_ASSERT_DEF_EXPECT(DBLQUOTE)
    GRAMMAR_ASSERT_DEF_EXPECT(OPATTRACC)
    GRAMMAR_ASSERT_DEF_EXPECT(OPIN)
    GRAMMAR_ASSERT_DEF_EXPECT(value)
    GRAMMAR_ASSERT_DEF_EXPECT(ident)

    // UINT char parser
    static const escape_char_parser<c_escapes, uint8_t> c_escape_uint8_p = escape_char_parser<c_escapes, uint8_t>();

    // init reserved words symbol table
    const EsStringIndexedMap& rw = EsScript::reservedWordsGet();
    for(ulong idx = 0; idx < rw.countGet(); ++idx )
      reservedWords.add( rw.nameGet(idx).c_str() );

    // values
    nullVal =
      NULLC;

    boolVal =
      BTRUE |
      BFALSE;

    intBinVal =
      lexeme_d[
        no_node_d[ch_p(esT('0'))] >> no_node_d[ch_p(esT('b'))] >>
        leaf_node_d[
          +chset_p(esT("01"))
        ]
      ];

    intOctVal =
      lexeme_d[
        no_node_d[ch_p(esT('0'))] >> no_node_d[ch_p(esT('o'))] >>
        leaf_node_d[
          +chset_p(esT("0-7"))
        ]
      ];

    intHexVal =
      lexeme_d[
        no_node_d[ch_p(esT('0'))] >> no_node_d[ch_p(esT('x'))] >>
        leaf_node_d[
          +xdigit_p
        ]
      ];

    intVal  =
      intBinVal |
      intOctVal |
      intHexVal |
      int_p;

    floatVal = longest_d[
      strict_real_p |
      strict_ureal_p
      ];

    charVal= lexeme_d[leaf_node_d[
      !ch_p(esT('B')) >> SNGLQUOTE >>
        (c_escape_uint8_p - SNGLQUOTE) >>
      GRAMMAR_EXPECT(SNGLQUOTE)
      ]];

    strVal = lexeme_d[leaf_node_d[
      !(ch_p(esT('I'))|ch_p(esT('B'))) >> DBLQUOTE >>
        *(
          c_escape_uint8_p -
          DBLQUOTE
          ) >>
      GRAMMAR_EXPECT(DBLQUOTE)
      ]];

    // enumerations values access support
    //
    enumVal =
      (ident-reservedWords) >> root_node_d[OPENUMACC] >>
        ident;

    enumValLbl =
      (ident-reservedWords) >> root_node_d[OPENUMACC] >>
        ident >> no_node_d[OPENUMACC] >> no_node_d[LABEL];
    //
    // enumerations support end

    numericVal =
      charVal |
      floatVal |
      intVal;

    simpleVal =
      enumValLbl |
      enumVal |
      strVal |
      nullVal |
      boolVal |
      numericVal;

    arrayVal =
      root_node_d[LSBRACKET] >>
        infix_node_d[
          !list_p(expr, COMMA)
        ] >>
      no_node_d[GRAMMAR_EXPECT(RSBRACKET)];

    rangeRightVal =
      root_node_d[OPRANGE] >>
      expr;

    rangeLeftVal =
      expr >>
      root_node_d[OPRANGE];

    rangeLeftRightVal =
      expr >>
      root_node_d[OPRANGE] >>
      expr;

    rangeVal =
      no_node_d[LSBRACKET] >>
        ( rangeLeftRightVal |
          rangeLeftVal |
          rangeRightVal ) >>
      no_node_d[GRAMMAR_EXPECT(RSBRACKET)];

    value =
      rangeVal |
      arrayVal |
      simpleVal;

    // lhs|rhs = (lrhs) (AKA left|right-hand-side rule)
    idxAcc = root_node_d[LSBRACKET] >>
      expr >>
      no_node_d[GRAMMAR_EXPECT(RSBRACKET)];

    fieldOrMemberVarAcc = root_node_d[OPFLDACC] >>
      ident >> !idxAcc;

    propAcc = root_node_d[OPPROPACC] >>
      ident >> !idxAcc;

    attrAcc = root_node_d[OPATTRACC] >>
      ident >> !idxAcc;

    objMethodCall =
      root_node_d[OPMTDACC] >>
      exprFuncCall;

    varSvcCall =
      root_node_d[OPVSVCACC] >>
      exprFuncCall;

    lrhs =
      (
        ( exprCall |
          ident
        ) >>
        *idxAcc >>
        *(  objMethodCall |
            varSvcCall |
            fieldOrMemberVarAcc |
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
      ASN    |
      ASNDEC |
      ASNINC |
      ASNDIV |
      ASNMOD |
      ASNMUL |
      ASNOR  |
      ASNAND |
      ASNXOR |
      ASNLSH |
      ASNRSH
    ];

    opIncDec = OPINC | OPDEC;

    opConditional = QUESTION;

    opLogical =
      OPLOG_OR |
      OPLOG_AND;

    opBitwise =
      OPBW_AND |
      OPBW_OR |
      OPBW_XOR;

    opEqNeq =
      OPEQ |
      OPNEQ |
      OPIN;

    opCmp =  longest_d[
      OPLE |
      OPGR |
      OPLEEQ |
      OPGREQ
      ];

    opSh  =
      OPLSH |
      OPRSH;

    opAddSub =
      PLUS |
      MINUS;

    opMulDiv =
      OPMUL |
      OPDIV |
      OPMOD;

    opUnary =
      OPNOT |
      MINUS |
      OPBW_NOT;

    // identifier
    ident = lexeme_d[leaf_node_d[
      (alpha_p | UNDERSCORE) >> *(alnum_p | UNDERSCORE)
      ]];

    // expressions
    //
    expr = exprLogical >>
      !(
        root_node_d[opConditional] >>
          exprLogical >>
          no_node_d[COLON] >>
          exprLogical
      );

    exprLogical = exprBitwise >> *(root_node_d[opLogical] >> exprBitwise);

    exprBitwise = exprEqNeq >> *(root_node_d[opBitwise] >> exprEqNeq);

    exprEqNeq = exprCmp >> *(root_node_d[opEqNeq] >> exprCmp);

    exprCmp = exprSh >> *(root_node_d[opCmp] >> exprSh);

    exprSh = exprAddSub >> *(root_node_d[opSh] >> exprAddSub);

    exprAddSub = exprMulDiv >> *(root_node_d[opAddSub] >> exprMulDiv);

    exprMulDiv = exprUnary >> *(root_node_d[opMulDiv] >> exprUnary);

    exprUnary =
      exprIncDecPfx |
      exprIncDec |
      (!root_node_d[opUnary] >> exprAtom);

    exprIncDec = opIncDec >> lrhs;

    exprIncDecPfx = lrhs >> opIncDec;

    exprWildcard = root_node_d[OPMUL];

    exprFuncCall =
      ident >> root_node_d[ LRBRACKET ] >>
        infix_node_d[
          !list_p(
            (
              expr |
              exprWildcard
            ),
            COMMA
          )
        ] >>
      no_node_d[ RRBRACKET ];

    exprObjCtrCall = root_node_d[NEWOBJ] >> exprFuncCall;
    exprStaticOrBaseCall =
      ident >>
      root_node_d[OPSCOPEACC] >>
      exprFuncCall;

    exprCall =
      exprStaticOrBaseCall |
      exprFuncCall;

    exprAtom =
      value |
      exprObjCtrCall |
      lrhs |
      inner_node_d[
        LRBRACKET >>
        expr >>
        GRAMMAR_EXPECT(RRBRACKET)
      ];

    exprAssign = lrhs >> root_node_d[opAssign] >> expr;

    // special assign initializer expression, which is used in var decls only
    exprInitAsn =
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

    stmtEnumItemDecl =
      ident >>
        root_node_d[ASN] >>
      expr >>
      !(no_node_d[COMMA] >> strVal) >>
        no_node_d[GRAMMAR_EXPECT(SEMICOLON)];

    stmtEnumDecl =
      root_node_d[ENUM] >>
        GRAMMAR_EXPECT(ident) >>
        !stmtAttributeList >>
      no_node_d[LCBRACKET] >>
        *stmtEnumItemDecl >>
      no_node_d[GRAMMAR_EXPECT(RCBRACKET)];

    stmtFuncDecl =
      root_node_d[FUNCTION] >>
        GRAMMAR_EXPECT(ident) >>
      no_node_d[LRBRACKET] >>
        infix_node_d[ !list_p(ident, COMMA) ] >>
      no_node_d[GRAMMAR_EXPECT(RRBRACKET)] >>
      !stmtAttributeList >>
      *stmtVarDecl >>
      stmtBlock;

    stmtObjCtorDecl =
      root_node_d[CTOR] >>
      no_node_d[LRBRACKET] >>
        infix_node_d[ !list_p(ident, COMMA) ] >>
      no_node_d[GRAMMAR_EXPECT(RRBRACKET)] >>
      !stmtAttributeList >>
      *stmtVarDecl >>
      stmtBlock;

    stmtObjDtorDecl =
      root_node_d[DTOR] >>
      no_node_d[LRBRACKET] >>
      no_node_d[GRAMMAR_EXPECT(RRBRACKET)] >>
      !stmtAttributeList >>
      *stmtVarDecl >>
      stmtBlock;

    stmtReturn =
      root_node_d[RETURN] >>
        !expr >>
          no_node_d[GRAMMAR_EXPECT(SEMICOLON)];

    stmtAttributeDecl =
      root_node_d[OPATTRACC] >> GRAMMAR_EXPECT(ident) >>
        !(no_node_d[ASN] >> (value|ident)) >>
        no_node_d[GRAMMAR_EXPECT(SEMICOLON)];

    stmtAttributeList =
      +stmtAttributeDecl;

    stmtObjPlainFldDecl =
      (ident-reservedWords) >> ident;

    stmtObjArrayFldDecl =
      (ident-reservedWords) >> ident >> no_node_d[LSBRACKET] >>
        expr >> no_node_d[GRAMMAR_EXPECT(RSBRACKET)];

    // complex conditional field grammar begin
    stmtObjFldElse = root_node_d[ ELSE ] >>
      (stmtObjFldDecl | stmtObjDeclBlock);

    stmtObjFldIfDecl = root_node_d[ IF ] >>
      inner_node_d[ LRBRACKET >> expr >> GRAMMAR_EXPECT(RRBRACKET) ] >>
        (stmtObjFldDecl | stmtObjDeclBlock) >>
        !stmtObjFldElse;
    // complex conditional field grammar end

    stmtObjFldDecl =
      (
        (
          stmtObjArrayFldDecl |
          stmtObjPlainFldDecl
        ) >>
        no_node_d[SEMICOLON] >>
        !stmtAttributeList

      ) | stmtObjFldIfDecl;

    stmtObjMemberVarDecl =
      root_node_d[VARIABLE] >>
        infix_node_d[
          list_p(ident, COMMA)
        ] >> no_node_d[GRAMMAR_EXPECT(SEMICOLON)];

    exprObjPropertyReaderDecl =
      root_node_d[READER] >> no_node_d[COLON] >>
      *stmtVarDecl >>
      stmtBlock;

    exprObjPropertyWriterDecl =
      root_node_d[WRITER] >> no_node_d[COLON] >>
      *stmtVarDecl >>
      stmtBlock;

    stmtObjPropertyDecl =
      root_node_d[PROPERTY] >> ident >> no_node_d[SEMICOLON] >>
        !stmtAttributeList >>
        (exprObjPropertyReaderDecl || exprObjPropertyWriterDecl);

    stmtObjMethodDecl =
      stmtFuncDecl |
      stmtObjDtorDecl |
      stmtObjCtorDecl;

    stmtObjDeclBlock = inner_node_d[
      LCBRACKET >>
        *stmtObjMemberVarDecl >>
        *stmtObjFldDecl >>
        *stmtObjMethodDecl >>
        *stmtObjPropertyDecl >>
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

    exprCase = value | lrhs;

    stmtExprCaseBlock = root_node_d[ GRAMMAR_EXPECT( COLON ) ] >>
      stmt;

    stmtCase = root_node_d[ CASE ] >>
      infix_node_d[ list_p(exprCase, COMMA) ] >>
      stmtExprCaseBlock;

    stmtDefault = root_node_d[ DEFAULT ] >>
      stmtExprCaseBlock;

    stmtSwitch = root_node_d[ SWITCH ] >>
      inner_node_d[ LRBRACKET >> exprCase >> GRAMMAR_EXPECT(RRBRACKET) ] >>
      no_node_d[ LCBRACKET ] >>
      +stmtCase >> !stmtDefault >>
      no_node_d[ GRAMMAR_EXPECT(RCBRACKET) ];

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

    exprForEachCheck = no_node_d[LRBRACKET] >>
        ident >> root_node_d[GRAMMAR_EXPECT(OPIN)] >> expr >>
      no_node_d[GRAMMAR_EXPECT(RRBRACKET)];

    stmtForEach = root_node_d[ FOREACH ] >>
      exprForEachCheck >>
      stmt;

    exprWhileCheck = root_node_d[WHILE] >>
      no_node_d[LRBRACKET] >>
        expr >>
      no_node_d[GRAMMAR_EXPECT(RRBRACKET)];

    stmtWhile = exprWhileCheck >> stmt;

    stmtDoWhile = root_node_d[DO] >>
      stmt >>
      exprWhileCheck >> no_node_d[GRAMMAR_EXPECT(SEMICOLON)];

    stmtBreak = root_node_d[BREAK] >> no_node_d[GRAMMAR_EXPECT(SEMICOLON)];

    stmtContinue = root_node_d[CONTINUE] >> no_node_d[GRAMMAR_EXPECT(SEMICOLON)];

    stmtLabel = root_node_d[LABEL] >> ident >> no_node_d[GRAMMAR_EXPECT(COLON)];

    stmtGoto = root_node_d[GOTO] >> ident >> no_node_d[GRAMMAR_EXPECT(SEMICOLON)];

    // try-catch blocks
    stmtTry = root_node_d[TRYEX] >> stmtBlock;
    stmtCatch = root_node_d[CATCHEX] >> stmtBlock;
    stmtTryCatch =
      stmtTry >> stmtCatch;

    // as a statement, prefix and postfis incdecs are the same
    stmtIncDec =
      (exprIncDec |
      exprIncDecPfx) >>
        root_node_d[SEMICOLON];

    stmtLrhs = lrhs >> root_node_d[SEMICOLON];

    stmtNoop = root_node_d[SEMICOLON];

    stmtThrow = root_node_d[THROWEX] >>
      expr >> no_node_d[SEMICOLON];

    stmtRethrow = root_node_d[RETHROWEX] >>
      no_node_d[SEMICOLON];

    stmtRequire = root_node_d[PREPROC] >>
      no_node_d[REQUIRE] >>
      no_node_d[LRBRACKET] >>
        strVal >>
      no_node_d[RRBRACKET] >>
      no_node_d[GRAMMAR_EXPECT(SEMICOLON)];

    stmtLink = root_node_d[PREPROC] >>
      no_node_d[LINKBIN] >>
      no_node_d[LRBRACKET] >>
        strVal >>
      no_node_d[RRBRACKET] >>
      no_node_d[GRAMMAR_EXPECT(SEMICOLON)];

   stmtExternDecl = root_node_d[EXTSYM] >>
      (ENUM | OBJECT ) >>
      GRAMMAR_EXPECT(ident) >>
      no_node_d[GRAMMAR_EXPECT(SEMICOLON)];

    // top-level grammar
    //
    stmtList =
      *( stmtExternDecl |
         stmtConstDecl |
         stmtVarDecl |
         stmtEnumDecl |
         stmtObjDecl |
         stmtFuncDecl |
         stmt );

    stmtBlock = root_node_d[LCBRACKET] >>
      *stmt >>
      no_node_d[GRAMMAR_EXPECT(RCBRACKET)];

    stmt =
      stmtReturn |
      stmtTryCatch |
      stmtRethrow |
      stmtThrow |
      stmtRequire |
      stmtLink |
      stmtIncDec |
      stmtAssign |
      stmtBlock |
      stmtIf |
      stmtSwitch |
      stmtFor |
      stmtForEach |
      stmtDoWhile |
      stmtWhile |
      stmtContinue |
      stmtBreak |
      stmtLabel |
      stmtGoto |
      stmtCall |
      stmtLrhs |
      stmtNoop; // noop statement

    // Main entry points
    script = GRAMMAR_PARSER_ASSERT_GUARD(
      stmtList
      ) >> end_p;

#   ifdef BOOST_SPIRIT_DEBUG
#     define GRAMMAR_RULE(r)  BOOST_SPIRIT_DEBUG_RULE(r);
#     include "EsScriptCompilerRuleNames.hxx"
      // root rules
      BOOST_SPIRIT_DEBUG_RULE(script);
#   endif
GRAMMAR_PARSER_IMPL_END

// skip parser
GRAMMAR_PARSER_IMPL_BEGIN( EsScriptCompiler, Skip )
  GRAMMAR_DEF_RULE(skip)
  // grammar entry point
  GRAMMAR_DEF_ENTRY(skip)
  GRAMMAR_PARSER_DEFINITION_IMPL_BEGIN
    skip =
      space_p |
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
      (esT("/*") >> *(anychar_p - esT("*/")) >> esT("*/")) |
      (esT("//") >> *(anychar_p - esT('\n')));
#else
      comment_p( esT("/*"), esT("*/") ) |
      comment_p( esT("//") );
#endif

# ifdef BOOST_SPIRIT_DEBUG
    BOOST_SPIRIT_DEBUG_RULE(skip);
# endif
GRAMMAR_PARSER_IMPL_END

// Custom parser(s), based on Grammar parser
template <typename CtxT>
struct EsScriptCompilerExprParser : public EsScriptCompilerGrammar<CtxT>
{
  EsScriptCompilerExprParser(CtxT& ctx) :
    EsScriptCompilerGrammar<CtxT>(ctx)
  {}

  template <typename ScannerT>
  struct definition
  {
    typedef EsScriptCompilerExprParser<CtxT> SelfT;
    typedef EsScriptCompilerGrammar<CtxT> ScriptCompilerGrammarT;
    typedef typename ScriptCompilerGrammarT::template definition< ScannerT > BaseDefinitionT;
    BaseDefinitionT m_basedef;

    definition(const SelfT& self) : m_basedef(self) {}

    GRAMMAR_DEF_ENTRY_OVERRIDE(m_basedef, expr)
  };
};

// Customized Parser context impl
PARSER_CONTEXT_CUSTOM_IMPL_BEGIN(
  EsScriptCompiler,
  typedef EsScriptCompilerExprParser<EsScriptCompiler> ExprGrammarT;
  ExprGrammarT m_grammar_expr;
)
, m_grammar_expr(*this)
  PARSER_CONTEXT_IMPL_INIT_BEGIN

    if( fromString == inputType )
      EsEventDispatcher::eventPost(
        ES_EVTC_SCRIPT,
        static_cast<ulong>(EsScript::evtCompilerInfo)
      );
    else
      EsEventDispatcher::eventPost(
        ES_EVTC_SCRIPT,
        static_cast<ulong>(EsScript::evtCompilerInfo),
        input
      );

    scopesReset();
    PARSER_CONTEXT_IMPL_INIT_END

      PARSER_CONTEXT_PARSE_BEGIN
# ifdef ES_DUMP_AST_TO_XML
      // dump parse tree as XML. each entry must be GRAMMAR_XML_DUMP_MAP_ENTRY
      GRAMMAR_AST_DUMP_MAP_BEGIN
      GRAMMAR_AST_DUMP_MAP_ENTRY(skip)
#     define GRAMMAR_RULE GRAMMAR_AST_DUMP_MAP_ENTRY
#     include "EsScriptCompilerRuleNames.hxx"
      // root rule
      GRAMMAR_AST_DUMP_MAP_ENTRY(script)
      GRAMMAR_AST_DUMP_MAP_END(EsScriptCompiler)
# endif // ES_DUMP_AST_TO_XML
      PARSER_CONTEXT_PARSE_END

      PARSER_SELECTED_CONTEXT_PARSE_BEGIN(expr)
# ifdef ES_DUMP_AST_TO_XML
      // dump parse tree as XML. each entry must be GRAMMAR_XML_DUMP_MAP_ENTRY
      GRAMMAR_AST_DUMP_MAP_BEGIN
      GRAMMAR_AST_DUMP_MAP_ENTRY(skip)
#     define GRAMMAR_RULE GRAMMAR_AST_DUMP_MAP_ENTRY
#     include "EsScriptCompilerRuleNames.hxx"
      // root rule
      GRAMMAR_AST_DUMP_MAP_ENTRY(expr)
      GRAMMAR_AST_DUMP_MAP_END(EsScriptCompiler)
# endif // ES_DUMP_AST_TO_XML
      PARSER_CONTEXT_PARSE_END

      PARSER_CONTEXT_PARSE_ERROR_HANDLER_BEGIN
      // parsing error handling goes here, error position is passed
      // in 'pos' parameter, its identifier goes in 'id' parameter
      if(doThrow)
        EsScriptException::Throw(
          getErrorDescr(id),
          debugInfoCreateAt(pos),
          m_doLogErrors
        );
      else
      {
        std::unique_ptr<EsScriptException> tmp(
          new EsScriptException( //< Just create temporaty object instance - it posts an error event, without interrupting compilation thread
            getErrorDescr(id),
            debugInfoCreateAt(pos),
            m_errCnt,
            m_doLogErrors
          )
        );

        return true;
      }
    PARSER_CONTEXT_PARSE_ERROR_HANDLER_END

    PARSER_CONTEXT_ERROR_RECOVERY_HANDLER_BEGIN

      return *scannerPos == esT('\n');

    PARSER_CONTEXT_ERROR_RECOVERY_HANDLER_END
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------

  void scopesReset()
  {
    m_codeScope = nullptr;
    m_objectScope = nullptr;
    m_breakGuard = nullptr;
    m_continueGuard = nullptr;
    m_labelGuard = nullptr;
    m_switchScope = nullptr;
  }
  //---------------------------------------------------------------------------

  inline EsScriptDebugInfoIntf::Ptr debugInfoCreateAt(EsString::const_pointer pos) const
  {
    return EsScriptDebugInfo::create(
      m_input,
      pos,
      m_file
    );
  }
  //---------------------------------------------------------------------------

  inline EsScriptDebugInfoIntf::Ptr debugInfoCreateAt(ParseTreeConstIteratorT cit) const
  {
    return debugInfoCreateAt(
      (*cit).value.begin()
    );
  }
  //---------------------------------------------------------------------------

  EsScriptObjectIntf::Ptr currentMetaclassGet()
  {
    if( m_objectScope )
      return m_objectScope->metaclassGet();

    return nullptr;
  }
  //---------------------------------------------------------------------------

  inline bool isBreaking() const
  {
    return m_brk && m_brk->isBreaking();
  }
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------

  static inline bool isSimpleVal(long id)
  {
    return simpleValID <= id &&
      enumValLblID >= id;
  }
  //---------------------------------------------------------------------------

  static inline bool isRangeVal(long id)
  {
    return rangeValID <= id &&
      rangeRightValID >= id;
  }
  //---------------------------------------------------------------------------

  static inline bool isBinOp(long id)
  {
    return opLogicalID <= id && opMulDivID >= id;
  }
  //---------------------------------------------------------------------------

  static inline bool isIncDec(long id)
  {
    return  // opIncDecID is never used by itself - it's being superseeded by corresponding expr IDs
      exprIncDecID == id ||
      exprIncDecPfxID == id;
  }
  //---------------------------------------------------------------------------

  static inline bool isUnaryOp(long id)
  {
    return opUnaryID == id ||
      isIncDec(id);
  }
  //---------------------------------------------------------------------------

  static inline bool isFunctionOrMemberCall(long id)
  {
    return exprFuncCallID == id ||
      exprStaticOrBaseCallID == id;
  }
  //---------------------------------------------------------------------------

  static inline bool isCompoundStatement(long id)
  {
    return stmtListID <= id;
  }
  //---------------------------------------------------------------------------

  static inline bool isFunctionOrMethodDecl(long id)
  {
    return stmtFuncDeclID <= id &&
      stmtObjDtorDeclID >= id;
  }
  //---------------------------------------------------------------------------

  static inline bool isWhile(long id)
  {
    return stmtWhileID == id ||
      stmtDoWhileID == id;
  }
  //---------------------------------------------------------------------------

  static inline bool isBreakContinue(long id)
  {
    return stmtBreakID == id ||
      stmtContinueID == id;
  }
  //---------------------------------------------------------------------------

  static inline bool isLabelGoto(long id)
  {
    return stmtLabelID == id ||
      stmtGotoID == id;
  }
  //---------------------------------------------------------------------------

  static inline bool isNakedFieldDecl(long id)
  {
    return stmtObjPlainFldDeclID <= id &&
      stmtObjFldIfDeclID >= id;
  }
  //---------------------------------------------------------------------------

  static inline bool isAttrListOrAttrDecl(long id)
  {
    return stmtAttributeListID == id ||
      stmtAttributeDeclID == id;
  }
  //---------------------------------------------------------------------------

  // These cases may be treated as Left-or-Right hand side expressions
  static inline bool isKindOfLrhs(long id)
  {
    return identID == id ||
      isFunctionOrMemberCall(id) ||
      exprObjCtrCallID == id ||
      idxAccID == id ||
      fieldOrMemberVarAccID == id ||
      propAccID == id ||
      attrAccID == id ||
      objMethodCallID == id ||
      varSvcCallID == id ||
      lrhsID == id;
  }
  //---------------------------------------------------------------------------

  // These cases may be treated as compound Right hand side expressions in
  // constexpr evaluator
  //
  static inline bool isKindOfCompoundRhs(long id)
  {
    return lrhsID == id ||
      attrAccID == id ||
      idxAccID == id ||
      isFunctionOrMemberCall(id);
  }
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------

  void compileTo(EsScriptMachine& script)
  {
    scopesReset();
    m_execRanges.clear();

    {
      // Create startup code section scope
      CodeSectionScope startup(
        *this,
        script.startupMethodGet()
      );

      // create global-scope label guard
      LabelGuard breakGotoGuard(
        *this,
        startup.sectionGet()
      );

      ParseTreeConstIteratorT cit = m_parseInfo.trees.begin();
      while( cit != m_parseInfo.trees.end() )
        compileStmt(
          cit++,
          script
        );
    }

    if( m_fileRefs )
    {
      // Calculate input hash
      // Assign hash and executable ranges to m_fileRefs
      //
      const EsVariant::Array& payload = {
        m_input.hashGet(),
        m_execRanges
      };

      m_fileRefs->valueSet(
        m_file,
        payload
      );
    }
  }
  //---------------------------------------------------------------------------

  void compileToScriptlet(EsScriptMachine& script, const EsMethodInfoKeyT& info, const EsString::Array& params)
  {
    // force script function context
    scopesReset();
    m_execRanges.clear();

    {
      // Create startup code section scope
      CodeSectionScope startup(
        *this,
        script.startupMethodGet()
      );

      // Create global-scope label guard
      LabelGuard breakGotoGuard(
        *this,
        startup.sectionGet()
      );

      ParseTreeConstIteratorT cit = m_parseInfo.trees.begin();
      if( stmtListID == (*cit).value.id().to_long() )
        cit = (*cit).children.begin();

      compileFunctionDecl(
        cit,
        script,
        script.globalMethodDeclare(
          info.nameGet(),
          params
        )
      );
    }

    if( m_fileRefs )
    {
      // Calculate input hash
      // Assign hash and executable ranges to m_fileRefs
      //
      const EsVariant::Array& payload = {
        m_input.hashGet(),
        m_execRanges
      };

      m_fileRefs->valueSet(
        m_file,
        payload
      );
    }
  }
  //---------------------------------------------------------------------------

protected:
  // Statement compiler entry point
  void compileStmt(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    if(scriptID == id) //< An empty script
      return;

    ES_ASSERT(opAssignID == id || (stmtID <= id && id <= stmtCatchID));

    if( isCompoundStatement(id) )
      for( ParseTreeConstIteratorT child = (*cit).children.begin(); child != (*cit).children.end(); ++child )
        compileStmt(
          child,
          script
        );
    else if( stmtAssignID == id )
      compileOpAssign(
        cit,
        script,
        true
      );
    else if( stmtIncDecID == id )
      compileIncDec(
        cit,
        script
      );
    else if( stmtConstDeclID == id )
      compileConstDecl(
        cit,
        script
      );
    else if( stmtVarDeclID == id )
      compileVarsDecl(
        cit,
        script
      );
    else if( stmtExternDeclID == id )
      compileStmtExternDecl(
        cit,
        script
      );
    else if( isFunctionOrMethodDecl(id) )
      compileStmtFuncDecl(
        cit,
        script
      );
    else if( stmtReturnID == id )
      compileReturn(
        cit,
        script
      );
    else if( stmtCallID == id )
      compileStmtCall(
        cit,
        script
      );
    else if( stmtIfID == id )
      compileStmtIf(
        cit,
        script
      );
    else if( stmtSwitchID == id )
      compileStmtSwitch(
        cit,
        script
      );
    else if( stmtForID == id )
      compileStmtFor(
        cit,
        script
      );
    else if( stmtForEachID == id )
      compileStmtForEach(
        cit,
        script
      );
    else if( isWhile(id) )
      compileStmtWhile(
        cit,
        script
      );
    else if( isBreakContinue(id) )
      compileStmtBreakContinue(
        cit,
        script
      );
    else if( isLabelGoto(id) )
      compileStmtLabelGoto(
        cit,
        script
      );
    else if( stmtObjDeclID == id )
      compileStmtObjDecl(
        cit,
        script
      );
    else if( stmtObjFldDeclID == id ) //< Generic field decl with attributes
      compileStmtObjFldWithAttrsDecl(
        cit,
        script
      );
    else if( isNakedFieldDecl(id) )  //< Naked field decl (no attributes, or attrinutes already compiled)
      compileStmtObjFldDecl(
        cit,
        script
      );
    else if( stmtObjMemberVarDeclID == id )
      compileStmtObjMemberVarDecl(
        cit,
        script
      );
    else if( stmtObjPropertyDeclID == id )
      compileStmtObjPropDecl(
        cit,
        script
      );
    else if( stmtLrhsID == id )
      compileStmtLrhs(
        cit,
        script
      );
    else if( stmtEnumDeclID == id )
      compileStmtEnumDecl(
        cit,
        script
      );
    else if( stmtTryCatchID == id )
      compileStmtTryCatch(
        cit,
        script
      );
    else if( stmtThrowID == id )
      compileStmtThrow(
        cit,
        script
      );
    else if( stmtRethrowID == id )
      compileStmtRethrow(
        cit,
        script
      );
    else if( stmtRequireID == id )
      compileStmtRequire(
        cit,
        script
      );
    else if( stmtLinkID == id )
      compileStmtLink(
        cit,
        script
      );
    else if( stmtNoopID == id )
      return; //< do nothing
    else //< we should not be here!
      EsScriptException::ThrowUnhandledAstNodeId(
        id,
        debugInfoCreateAt(
          cit
        )
      );
  }
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------

  // single statement compilers
  //
  void compileStmtTryCatch(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(stmtTryCatchID == (*cit).value.id().to_long());
    ES_ASSERT(2 == (*cit).children.size());

    ES_ASSERT(m_codeScope);
    EsScriptCodeSection::Ptr code = m_codeScope->sectionGet();

    // later we need to update this instruction's parameter with real try-catch block index
    ulong tryCatchEnterPos;
    code->instructionAdd(
      tryCatchEnterPos,
      iEnterTryCatch,
      debugInfoCreateAt(
        (*cit).value.begin()
      )
    );
    ulong tryCatchIdx = code->tryCatchBlockCreate();

    code->instructionModifyAt(
      tryCatchEnterPos
    ).tryCatchIdxSet(
      tryCatchIdx
    );

    ParseTreeConstIteratorT child = (*cit).children.begin();

    // compile try block
    ES_ASSERT(stmtTryID == (*child).value.id().to_long());
    code->tryBlockSelect();
    compileStmt(
      child++,
      script
    );

    // compile catch block
    code->catchBlockSelect();
    compileStmt(
      child,
      script
    );

    // exit currently active try-catch block compilation
    code->activeTryCatchBlockDeselect();
  }
  //---------------------------------------------------------------------------

  void compileStmtThrow(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(stmtThrowID == (*cit).value.id().to_long());
    ES_ASSERT(1 == (*cit).children.size());

    compileExpr(
      (*cit).children.begin(),
      script
    );

    ES_ASSERT(m_codeScope);
    EsScriptCodeSection::Ptr code = m_codeScope->sectionGet();

    code->instructionAdd(
      iThrow,
      debugInfoCreateAt(
        (*cit).value.begin()
      )
    );
  }
  //---------------------------------------------------------------------------

  void compileStmtRethrow(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(stmtRethrowID == (*cit).value.id().to_long());
    ES_ASSERT((*cit).children.empty());

    ES_ASSERT(m_codeScope);
    EsScriptCodeSection::Ptr code = m_codeScope->sectionGet();

    EsScriptDebugInfoIntf::Ptr dbg = debugInfoCreateAt(cit);

    // endure we're inside catch section, otherwise, rethrow is unexpected
    if( !code->isCatchBlockSelected() )
      EsScriptException::Throw(
        esT("Statement 'rethrow' is unexpected outside the 'catch' block"),
        dbg
      );

    code->instructionAdd(
      iRethrow,
      dbg
    );
  }
  //---------------------------------------------------------------------------

  EsString stringValCompile(ParseTreeConstIteratorT cit, EsScriptMachine& script, bool expectPfx)
  {
    EsString strVal(
      (*cit).value.begin(),
      (*cit).value.end()
    );

    return compileStrValHelper(
      cit,
      script,
      strVal,
      expectPfx
    ).asString();
  }
  //---------------------------------------------------------------------------

  void compileStmtRequire(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(stmtRequireID == (*cit).value.id().to_long());
    ES_ASSERT(1 == (*cit).children.size());

    // Enter file scope
    ParseTreeConstIteratorT child = (*cit).children.begin();
    const EsString& fileName = stringValCompile(
      child,
      script,
      false
    );

    // convert file name to the absolute path
    const EsString& fileNameAbs = EsScript::absoluteFileNameGet(
      fileName,
      script.ownerGet().includePathsGet(),
      false,
      m_file
    );

    // check source file existence
    if( fileNameAbs.empty() ) // throw compiler positional exception
      EsScriptException::Throw(
        EsString::format(
          script.loc(),
          esT("Source file '%s' could not be found"),
          fileName
        ),
        debugInfoCreateAt(cit)
      );

    // check if source file is already referenced, if so, do not compile it (eliminate circular references)
    ES_ASSERT(m_fileRefs);
    if( m_fileRefs->keyExists(fileNameAbs) )
      return; //< Skip compilation - already comiled before

    if( isBreaking() )
      EsScriptException::Throw( esT( "Compilation aborted" ) );

    ESSCRIPT_COMPILER_TRACE2(
      esT( "Compiling '%s'" ),
      fileNameAbs
    )

    EsScriptCompiler fileCompiler(
      fileNameAbs,
      m_fileRefs,
      fromSourceFile,
      m_brk
    );

    if( fileCompiler.doParse() )
      fileCompiler.compileTo(script);
    else
      fileCompiler.errorHandle(
        fileCompiler.m_parseInfo.stop,
        EsScriptCompiler::Error_Parser,
        true
      );
  }
  //---------------------------------------------------------------------------

  void compileStmtLink(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(stmtLinkID == (*cit).value.id().to_long());
    ES_ASSERT(1 == (*cit).children.size());

    ParseTreeConstIteratorT child = (*cit).children.begin();
    const EsString& fileName = stringValCompile(
      child,
      script,
      false
    );

    script.linkAdd(fileName);
  }
  //---------------------------------------------------------------------------

  void compileConstDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT(stmtConstDeclID == id);
    ES_ASSERT((*cit).children.size() == 2);

    ParseTreeConstIteratorT child = (*cit).children.begin();

    const EsString& name = compileIdent(child++);
    const EsVariant& val = compileConstExpr(
      child,
      script
    );

    script.constDeclare(
      name,
      val
    );
  }
  //---------------------------------------------------------------------------

  void compileVarsDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(stmtVarDeclID == (*cit).value.id().to_long());
    // compile variablesAccess
    for(ParseTreeConstIteratorT child = (*cit).children.begin(); child != (*cit).children.end(); ++child)
    {
      long id = static_cast<long>((*child).value.id().to_long());
      ES_ASSERT(identID == id || exprInitAsnID == id);
      if( identID == id ) // just declaration
      {
        ES_ASSERT(m_codeScope);
        EsScriptCodeSection::Ptr code = m_codeScope->sectionGet();

        // add variable to the current section's symtable & create memslot for it
        code->variableDeclare(
          compileIdent(child),
          debugInfoCreateAt(child)
        );
      }
      else // declaration + initialization with assignment
        compileInitAsn(child, script);
    }
  }
  //---------------------------------------------------------------------------

  void compileStmtExternDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(stmtExternDeclID == (*cit).value.id().to_long());
    // Compile external decls
    EsString::Array excluding =
      script.enumNamesGet() +
      script.consts().allSymbolNamesGet() +
      script.vars().allSymbolNamesGet() +
      script.sysVars().allSymbolNamesGet() +
      script.metaGet()->objectNamesGet();

    ES_ASSERT( 2 == (*cit).children.size() );
    ParseTreeConstIteratorT child = (*cit).children.begin();

    EsScriptDebugInfoIntf::Ptr dbg = debugInfoCreateAt(child);
    EsString externSubj( (*child).value.begin(), (*child).value.end() );
    ++child;

    ES_ASSERT(identID == (*child).value.id().to_long());
    const EsString& name = compileIdent(
      child,
      true,
      excluding
    );

    // Add identifier to the global externals list (if it's not already there)
    // Check against the currently known objects, enums, constants, global variables,
    // and global methods
    //
    if( esT("enum") == externSubj )
      script.externEnumDeclare(name);
    else if( esT("object") == externSubj )
      script.externMetaclassDeclare(name);
  }
  //---------------------------------------------------------------------------

  // compile variable declaration with initialization expression
  void compileInitAsn(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT((*cit).value.id().to_long() == exprInitAsnID);
    ES_ASSERT((*cit).children.size() == 2);

    ParseTreeConstIteratorT child = (*cit).children.begin();

    ES_ASSERT(m_codeScope);
    EsScriptCodeSection::Ptr code = m_codeScope->sectionGet();

    // declare var, pushing its reference as lhs
    const EsString& name = compileIdent(child++);
    code->variableDeclare(
      name,
      debugInfoCreateAt(child)
    );

    // compile and push right-side expr(s)
    compileExpr(child, script);

    // issue var init instruction, parameter contains reference to variable
    code->instructionAdd(
      iVarInit,
      debugInfoCreateAt(cit)
    ).nameSet(
      name
    );
  }
  //---------------------------------------------------------------------------

  void compileStmtFunctionAttrDecls(EsScriptCodeSection::Ptr function, ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT( isAttrListOrAttrDecl(id) );

    if(stmtAttributeListID == id)
    {
      for( ParseTreeConstIteratorT attrIt = (*cit).children.begin(); attrIt != (*cit).children.end(); ++attrIt )
        compileStmtFunctionAttrDecls(
          function,
          attrIt,
          script
        );
    }
    else
    {
      ES_ASSERT(0 < (*cit).children.size() && (*cit).children.size() <= 2 );

      ParseTreeConstIteratorT child = (*cit).children.begin();
      const EsString& name = compileIdent(
        child++,
        false
      );

      script.functionAttributeDeclare(
        function,
        name,
        ( child != (*cit).children.end() ) ?
          compileConstExpr(
            child,
            script
          ) :
          EsVariant::null()
      );
    }
  }
  //---------------------------------------------------------------------------

  void compileFunctionDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script, const EsScriptCodeSection::Ptr& function)
  {
    CodeSectionScope functionScope(
      *this,
      function
    );

    // compile optional attribute(s)
    long id = static_cast<long>((*cit).value.id().to_long());
    if( isAttrListOrAttrDecl(id) )
    {
      compileStmtFunctionAttrDecls(
        function,
        cit,
        script
      );

      ++cit;
    }

    // compile optional variable declarations
    while(stmtVarDeclID == (*cit).value.id().to_long())
    {
      compileVarsDecl(
        cit,
        script
      );

      ++cit;
    }

    // set-up label|goto guard
    LabelGuard lblGuard(
      *this,
      functionScope.sectionGet()
    );

    // compile section's code
    ES_ASSERT(stmtBlockID == (*cit).value.id().to_long());
    compileStmt(
      cit,
      script
    );
  }
  //---------------------------------------------------------------------------

  void compileMethodDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script, const EsScriptCodeSection::Ptr& function, const EsScriptObjectIntf::Ptr& metaclass)
  {
    ES_ASSERT(metaclass);

    compileFunctionDecl(
      cit,
      script,
      function
    );
  }
  //---------------------------------------------------------------------------

  void compileStmtFuncDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT(id == stmtFuncDeclID || id == stmtObjCtorDeclID || id == stmtObjDtorDeclID);
    ES_ASSERT((*cit).children.size() > 0);

    ParseTreeConstIteratorT child = (*cit).children.begin();

    // get function identifier
    EsString funcName;
    if( id == stmtObjCtorDeclID )
      funcName = EsStdNames::reflectedCtr();
    else if( id == stmtObjDtorDeclID )
      funcName = EsStdNames::reflectedDtr();
    else
      funcName = compileIdent(child++);

    EsString::Array params;
    while(
      child != (*cit).children.end() &&
      identID == (*child).value.id().to_long()
    )
      params.push_back(
        compileIdent(child++)
      );

    // declare new global script function or object method
    EsScriptObjectIntf::Ptr metaclass = currentMetaclassGet();
    if( metaclass )
      compileMethodDecl(
        child,
        script,
        script.methodDeclare(
          funcName,
          params,
          metaclass
        ),
        metaclass
      );
    else
      compileFunctionDecl(
        child,
        script,
        script.globalMethodDeclare(
          funcName,
          params
        )
      );
  }
  //---------------------------------------------------------------------------

  void compileReturn(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT((*cit).value.id().to_long() == stmtReturnID);

    // issue return instruction, possibly preceeded by some expr code
    EsScriptDebugInfoIntf::Ptr dbg = debugInfoCreateAt(cit);

    ES_ASSERT(m_codeScope);

    if( (*cit).children.empty() )
    {
      // push empty const value to the data stack on empty return
      m_codeScope->sectionGet()->instructionAdd(
        iLoadConstVal,
        dbg
      ).nameSet(
        EsScriptStdNames::null()
      );
    }
    else
      for(ParseTreeConstIteratorT child = (*cit).children.begin(); child != (*cit).children.end(); ++child)
        compileExpr(child, script);

    m_codeScope->sectionGet()->instructionAdd(
      iRet,
      dbg
    );
  }
  //---------------------------------------------------------------------------

  void compileStmtCall(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT((*cit).value.id().to_long() == stmtCallID);
    ES_ASSERT((*cit).children.size() == 1);
    // compile function call expr as usual
    compileExprCall((*cit).children.begin(), script);

    ES_ASSERT(m_codeScope);
    // issue additional iRetCleanup instruction after call return
    m_codeScope->sectionGet()->instructionAdd(
      iRetCleanup,
      debugInfoCreateAt(cit)
    );
  }
  //---------------------------------------------------------------------------

  void compileStmtLrhs(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT((*cit).value.id().to_long() == stmtLrhsID);
    ES_ASSERT((*cit).children.size() == 1);

    // compile lrhs
    cit = (*cit).children.begin();
    compileLrhs(cit, script);

    // if the last lrhs child is object|variant service call - add iRetCleanup, otherwise - complain on senseless instruction
    long id = static_cast<long>((*cit).children.back().value.id().to_long());
    EsScriptDebugInfoIntf::Ptr dbg = debugInfoCreateAt(cit);

    if( objMethodCallID != id && varSvcCallID != id )
      EsScriptException::Throw(
        esT("Statement expression is meaningless"),
        dbg
      );
    else
    {
      ES_ASSERT(m_codeScope);

      m_codeScope->sectionGet()->instructionAdd(
        iRetCleanup,
        dbg
      );
    }
  }
  //---------------------------------------------------------------------------

  void compileOpAssign(ParseTreeConstIteratorT cit, EsScriptMachine& script, bool asStmt = false )
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    if( asStmt && id == stmtAssignID )
    {
      ES_ASSERT((*cit).children.size() == 1);
      cit = (*cit).children.begin();
      id = static_cast<long>((*cit).value.id().to_long());
    }

    ES_ASSERT(id == opAssignID);
    ES_ASSERT((*cit).children.size() == 2);
    EsString opstr((*cit).value.begin(), (*cit).value.end());
    // find which exactly assign operation to use
    EsScriptOperatorIds opid = EsScriptOpIds::instanceGet().getId(opstr);
    // assert opid is one of assignment ids
    ES_ASSERT(opid >= opidAsn && opid <= opidAsnRsh);

    ParseTreeConstIteratorT child = (*cit).children.begin();
    // evaluate and push lhs
    compileLrhs(child++, script);
    // evaluate rhs
    compileExpr(child, script);

    // finally, push assignment instruction
    ES_ASSERT(m_codeScope);
    m_codeScope->sectionGet()->instructionAdd(
      iAsn,
      debugInfoCreateAt(cit)
    ).operatorIdSet(
      opid
    ).callAsStmtSet(
      asStmt
    );
  }
  //---------------------------------------------------------------------------

  // evaluate and push l|r-hs. lrhs kind is defined by the last part of lrhs compiled
  void compileLrhsNested(ParseTreeConstIteratorT cit, EsScriptMachine& script, size_t level)
  {
    long id = static_cast<long>((*cit).value.id().to_long());

    if( identID == id )
      compileConstOrVarOrEnumAccOrClassAcc(
        cit,
        script
      );
    else if(
      isFunctionOrMemberCall(id) ||
      exprObjCtrCallID == id
    )
      compileExprCall(
        cit,
        script
      );
    else if( idxAccID == id )
      compileIdxAcc(
        cit,
        script
      );
    else if( fieldOrMemberVarAccID == id )
      compileFieldOrMemberVarAcc(
        cit,
        script
      );
    else if( propAccID == id )
      compilePropAcc(
        cit,
        script,
        0 == level
      );
    else if( attrAccID == id )
      compileAttrAcc(
        cit,
        script,
        0 == level
      );
    else if( objMethodCallID == id )
      compileObjMethodCall(
        cit,
        script
      );
    else if( varSvcCallID == id )
      compileVsvcCall(
        cit,
        script
      );
    else if( lrhsID == id )
    {
      // any nested lhrs got to be compiled as rhs
      // compound l-or-r-value, recursively resolve from leftmost to rightmost
      for(ParseTreeConstIteratorT child = (*cit).children.begin(); child != (*cit).children.end(); ++child)
        compileLrhsNested(
          child,
          script,
          ++level
        );
    }
    else
      EsScriptException::ThrowUnhandledAstNodeId(
        id,
        debugInfoCreateAt(cit)
      );
  }
  //---------------------------------------------------------------------------

  inline void compileLrhs(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    // Start with nesting level 0
    compileLrhsNested(
      cit,
      script,
      0
    );
  }
  //---------------------------------------------------------------------------

  // var access common internal helper
  bool internalCompileVarAcc(const EsString& name, EsScriptMachine& script, const EsScriptDebugInfoIntf::Ptr& dbg)
  {
    ES_ASSERT(m_codeScope);
    EsScriptCodeSection::Ptr activeMethod = m_codeScope->sectionGet();

    // first, lookup local symbol
    bool found = activeMethod->variableFind(name);
    if( found )
    {
      activeMethod->instructionAdd(
        iLoadVarRef,
        dbg
      ).nameSet(
        name
      ).variableTypeSet(
        EsVarType::vtLocal //< local symbol
      );
    }
    else if( activeMethod->isObjectMethod() )
    {
      EsScriptObjectIntf* activeBranch = activeMethod->immediateMetaclassGet();
      ES_ASSERT(activeBranch);

      // lookup object fields
      EsScriptObjectIntf::Ptr fld;
      while( !fld && activeBranch )
      {
        EsScriptObjectFieldFastFinder finder(
          activeBranch,
          name
        );

        if( finder.found() )
          fld = EsScriptObject::fnodeFieldGet( finder.resultGet() );

        activeBranch = activeBranch->parentGet();
      }

      if( fld )
      {
        found = true;

        // lookup object context for field name
        activeMethod->instructionAdd(
          iLoadMemberRef,
          dbg
        ).nameSet(
          name
        ).memberRefTypeSet(
          EsMemberRefType::mrThisField
        );

        // add field name to the code section dependency list
        activeMethod->thisFieldDependencyAdd(name);
      }
      else
      {
        // lookup object member variables
        activeBranch = activeMethod->immediateMetaclassGet();
        ES_ASSERT(activeBranch);

        while( activeBranch && !found )
        {
          EsScriptObjectVarFastFinder finder(
            activeBranch,
            name
          );
          found = finder.found();
          activeBranch = activeBranch->parentGet();
        }

        if( found )
        {
          // lookup object context for field name
          activeMethod->instructionAdd(
            iLoadMemberRef,
            dbg
          ).nameSet(
            name
          ).memberRefTypeSet(
            EsMemberRefType::mrThisVar
          );

          // add member var name to the code section dependency list
          activeMethod->thisMemberVarDependencyAdd(name);
        }
      }
    }

    if( !found )
    {
      // try global variables
      found = script.startupMethodGet()->variableFind(name);
      if( found )
      {
        activeMethod->instructionAdd(
          iLoadVarRef,
          dbg
        ).nameSet(
          name
        ).variableTypeSet(
          EsVarType::vtGlobal
        );
      }
      else
      {
        // try system variables
        found = script.sysVars().symbolExists(name);
        if( found )
        {
          activeMethod->instructionAdd(
            iLoadVarRef,
            dbg
          ).nameSet(
            name
          ).variableTypeSet(
            EsVarType::vtSystem //< flag symbol as system
          );
        }
      }
    }

    return found;
  }
  //---------------------------------------------------------------------------

  bool internalCompileConstAcc(const EsString& name, EsScriptMachine& script, const EsScriptDebugInfoIntf::Ptr& dbg)
  {
    if( script.consts().symbolGet(name, false) )
    {
      ES_ASSERT(m_codeScope);
      m_codeScope->sectionGet()->instructionAdd(
        iLoadConstVal,
        dbg
      ).nameSet(
        name
      );

      return true;
    }

    return false;
  }
  //---------------------------------------------------------------------------

  bool internalCompileEnumInstAcc(const EsString& name, EsScriptMachine& script, const EsScriptDebugInfoIntf::Ptr& dbg)
  {
    if( script.enumFind(name) ||
        script.isExternalEnum(name)
    )
    {
      ES_ASSERT(m_codeScope);

      m_codeScope->sectionGet()->instructionAdd(
        iLoadImmediateVal,
        dbg
      ).nameSet(
        name
      ).immValTypeSet(
        EsIvalType::ivEnum
      );

      return true;
    }

    return false;
  }
  //---------------------------------------------------------------------------

  bool internalCompileMetaclassInstAcc(const EsString& name, EsScriptMachine& script, const EsScriptDebugInfoIntf::Ptr& dbg)
  {
    if( script.metaclassFind(name) ||
        script.isExternalMetaclass(name)
    )
    {
      ES_ASSERT(m_codeScope);

      m_codeScope->sectionGet()->instructionAdd(
        iLoadImmediateVal,
        dbg
      ).nameSet(
        name
      ).immValTypeSet(
        EsIvalType::ivMetaclass
      );

      return true;
    }

    return false;
  }
  //---------------------------------------------------------------------------

  void compileConstOrVarOrEnumAccOrClassAcc(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT((*cit).value.id().to_long() == identID);

    const EsString& name = compileIdent(cit);
    EsScriptDebugInfoIntf::Ptr dbg = debugInfoCreateAt(cit);

    // Search constants
    if( internalCompileConstAcc(name, script, dbg) )
      return;

    // Not const - try local or global variable
    if( internalCompileVarAcc(name, script, dbg) )
      return;

    // Symbol is neither local, global, nor system variable, try to find declared enumeration
    if( internalCompileEnumInstAcc(name, script, dbg) )
      return;

    // Not enum, try script metaclass or c++ reflected class
    if( internalCompileMetaclassInstAcc(name, script, dbg) )
      return;

    // Not script metaclass as well, give up - throw compile time exception
    EsScriptException::Throw(
      EsString::format(
        esT("Symbol '%s' is neither constant, enum, object, local, global, or system variable"),
        name
      ),
      dbg
    );
  }
  //---------------------------------------------------------------------------

  void compileIdxAcc(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT((*cit).value.id().to_long() == idxAccID);
    ES_ASSERT((*cit).children.size() == 1);

    compileExpr(
      (*cit).children.begin(),
      script
    );

    // issue index access instruction
    ES_ASSERT(m_codeScope);
    m_codeScope->sectionGet()->instructionAdd(
      iLoadItemRef,
      debugInfoCreateAt(cit)
    );
  }
  //---------------------------------------------------------------------------

  void compileFieldOrMemberVarAcc(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT((*cit).value.id().to_long() == fieldOrMemberVarAccID);
    ES_ASSERT((*cit).children.size() > 0 && (*cit).children.size() < 3 );

    // retrieve field name from the first child
    ParseTreeConstIteratorT child = (*cit).children.begin();
    const EsString& name = compileIdent(child++);

    ES_ASSERT(m_codeScope);
    m_codeScope->sectionGet()->instructionAdd(
      iLoadMemberRef,
      debugInfoCreateAt(cit)
    ).nameSet(
      name
    ).memberRefTypeSet(
      EsMemberRefType::mrFieldOrVar
    );

    // handle possible index access instruction
    if( child != (*cit).children.end() )
      compileIdxAcc(
        child,
        script
      );
  }
  //---------------------------------------------------------------------------

  void compilePropAcc(ParseTreeConstIteratorT cit, EsScriptMachine& script, bool thisAcc)
  {
    ES_ASSERT((*cit).value.id().to_long() == propAccID);
    ES_ASSERT((*cit).children.size() > 0 && (*cit).children.size() < 3 );

    // retrieve property name from the first child
    ParseTreeConstIteratorT child = (*cit).children.begin();

    ES_ASSERT(m_codeScope);
    m_codeScope->sectionGet()->instructionAdd(
      thisAcc ?
        iLoadThisPropRef :
        iLoadPropRef,
      debugInfoCreateAt(cit)
    ).nameSet(
      compileIdent(child++)
    );

    // handle possible index access instruction
    if( child != (*cit).children.end() )
      compileIdxAcc(
        child,
        script
      );
  }
  //---------------------------------------------------------------------------

  void compileAttrAcc(ParseTreeConstIteratorT cit, EsScriptMachine& script, bool thisAcc)
  {
    ES_ASSERT(attrAccID == (*cit).value.id().to_long());
    ES_ASSERT((*cit).children.size() < 2);

    // retrieve attribute name from the first child
    ParseTreeConstIteratorT child = (*cit).children.begin();

    ES_ASSERT(m_codeScope);
    m_codeScope->sectionGet()->instructionAdd(
      thisAcc ?
        iLoadThisAttributeVal :
        iLoadAttributeVal,
      debugInfoCreateAt(cit)
    ).nameSet(
      compileIdent(
        child++,
        false
      )
    );

    // handle possible index access instruction
    if( child != (*cit).children.end() )
      compileIdxAcc(
        child,
        script
      );
  }
  //---------------------------------------------------------------------------

  void compileObjMethodCall(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());

    ES_ASSERT(objMethodCallID == id);
    ES_ASSERT((*cit).children.size() == 1);

    compileExprCall(
      (*cit).children.begin(),
      script,
      id
    );
  }
  //---------------------------------------------------------------------------

  void compileVsvcCall(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(varSvcCallID == (*cit).value.id().to_long());
    ES_ASSERT((*cit).children.size() == 1);

    // compile lhs expr
    compileExprCall(
      (*cit).children.begin(),
      script,
      varSvcCallID
    );
  }
  //---------------------------------------------------------------------------

  void compileIncDec(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT((*cit).value.id().to_long() == stmtIncDecID);
    ES_ASSERT((*cit).children.size() == 1);

    cit = (*cit).children.begin();

    // compile inc dec expression, but do not push result on data stack, as it's statement,
    // so do not care about pre- or postfix ++ or -- as well.
    compileExprIncDec(
      cit,
      script,
      true
    );
  }
  //---------------------------------------------------------------------------

  void compileStmtIf(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(stmtIfID == (*cit).value.id().to_long());
    ES_ASSERT((*cit).children.size() > 1);

    ParseTreeConstIteratorT child = (*cit).children.begin();

    ES_ASSERT(m_codeScope);
    EsScriptCodeSection::Ptr code = m_codeScope->sectionGet();

    // compile if expression
    compileExpr(
      child++,
      script
    );

    // issue iJumpFalse instruction, saving its positon
    ulong jumpFalsePos;
    code->instructionAdd(
      jumpFalsePos,
      iJumpFalse,
      debugInfoCreateAt(child)
    );

    // compile 'true' code section
    compileStmt(
      child,
      script
    );

    // add instruction to jump to the ending of entire if
    ulong jumpOutOfIfPos;
    code->instructionAdd(
      jumpOutOfIfPos,
      iJump,
      debugInfoCreateAt(child)
    );
    ++child;

    // modify iJumpFalse instruction, so as its target is instruction following the last instruction of the true block
    code->instructionModifyAt(
      jumpFalsePos
    ).jumpOpcodeSet(
      joAbs
    ).jumpOffsSet(
      code->instructionEndPosGet()
    );

    // next is optional else section
    if( child != (*cit).children.end() )
    {
      ES_ASSERT((*child).value.id().to_long());

      child = (*child).children.begin();

      compileStmt(
        child,
        script
      );
    }

    // finalize out-of-if jump instruction
    code->instructionModifyAt(
      jumpOutOfIfPos
    ).jumpOpcodeSet(
      joAbs
    ).jumpOffsSet(
      code->instructionEndPosGet()
    );
  }
  //---------------------------------------------------------------------------

  void compileStmtSwitch(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT(stmtSwitchID == id);
    ES_ASSERT((*cit).children.size() > 1);

    ParseTreeConstIteratorT child = (*cit).children.begin();

    ES_ASSERT(m_codeScope);
    EsScriptCodeSection::Ptr code = m_codeScope->sectionGet();

    // Open new switch scope
    SwitchScope scope(
      *this,
      code
    );

    // Create|use case-specific private temporary var, assigning switch argument to it
    // var will have syntax-impossible name to prevent it from clashing with other
    // script vars.
    //
    code->variableDeclarePrivateReusable(
      scope.argnameGet(),
      debugInfoCreateAt(child)
    );

    // Compile switch argument expression
    compileExpr(
      child++,
      script
    );

    // Issue reusable scope variable init instruction from the compiled switch arg expr result
    code->instructionAdd(
      iVarInit,
      debugInfoCreateAt(cit)
    ).nameSet(
      scope.argnameGet()
    );

    // Compile cases|default statements
    m_switchScope->caseEntriesCntSet(
      static_cast<ulong>((*cit).children.size() - 1) //< Children count is cases+default+switch arg expr, so use size-1 here
    );

    while( child != (*cit).children.end() )
    {
      compileCaseOrDefault(
        child++,
        script
      );

      scope.caseEntryCntInc();
    }
  }
  //---------------------------------------------------------------------------

  void compileCaseOrDefault(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT(id == stmtCaseID || id == stmtDefaultID);

    // Fixup previous entry's unconditional jump instruction
    m_switchScope->previousCaseEntryJumpHere();

    ES_ASSERT(m_codeScope);
    EsScriptCodeSection::Ptr code = m_codeScope->sectionGet();

    EsScriptDebugInfoIntf::Ptr dbg;
    auto child = (*cit).children.begin();

    if( stmtCaseID == id ) //< Case stmt compilation
    {
      // Issue compare instruction for each case condition list entry
      std::vector<ulong> trueJumps;
      trueJumps.reserve(
        (*cit).children.size()
      );

      id = static_cast<long>((*child).value.id().to_long());
      while(id != stmtExprCaseBlockID)
      {
        dbg = debugInfoCreateAt(child);

        // Issue argval variable push instruction
        code->instructionAdd(
          iLoadVarRef,
          dbg
        ).nameSet(
          m_switchScope->argnameGet()
        ).variableTypeSet(
          EsVarType::vtLocal
        );

        // Compile case condition list entry
        compileExpr(
          child++,
          script
        );

        // Issue 'in' binop compare instruction. The parameters are argval[s_top-1], expr[s_top]
        code->instructionAdd(
          iBinOpExpr,
          dbg
        ).operatorIdSet(
          EsScriptOperatorIds::opidIn
        );

        // Issue iJumpTrue instruction, saving its positon for later fixup
        ulong jmpPos;
        code->instructionAdd(
          jmpPos,
          iJumpTrue,
          dbg
        );
        trueJumps.push_back( jmpPos );

        id = static_cast<long>((*child).value.id().to_long());
      }

      // Issue unconditional jump instruction to the beginning of the next case entry,
      // if this is not the last case entry in the list. Otherwise, add an end jump.
      // This instruction will be executed if no true conditions are met
      //
      m_switchScope->nextCaseEntryOrEndJumpAdd(dbg);

      // Apply all true jumps fixups to jump here, to the beginning of the case code block
      for(auto jmp: trueJumps)
      {
        code->instructionModifyAt(
          jmp
        ).jumpOpcodeSet(
          joAbs
        ).jumpOffsSet(
          code->instructionEndPosGet()
        );
      }

      // Compile case entry code block
      ES_ASSERT(id == stmtExprCaseBlockID);
      compileStmt(
        child,
        script
      );

      // Issue unconditional jump instruction to the end of switch statement
      m_switchScope->switchEndJumpAdd(
        debugInfoCreateAt(child)
      );
    }
    else //< Default stmt compilation
    {
      id = static_cast<long>((*child).value.id().to_long());
      ES_ASSERT(id == stmtExprCaseBlockID);

      compileStmt(
        child,
        script
      );
    }
  }
  //---------------------------------------------------------------------------

  void compileForCountExpr(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT((*cit).value.id().to_long() == exprForCntID);

    if( !(*cit).children.empty() )
    {
      cit = (*cit).children.begin();
      long id = static_cast<long>((*cit).value.id().to_long());
      ES_ASSERT( opAssignID == id || exprIncDecID == id || exprIncDecPfxID == id );

      if(opAssignID == id)
        compileOpAssign(
          cit,
          script,
          true
        );
      else
        compileExprIncDec(
          cit,
          script,
          true
        );
    }
  }
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------

  // Code section automatic compilation scope
  class CodeSectionScope
  {
  public:
    enum RangeState {
      None,
      RangeStarted
    };

  public:
    // try to declare object and enter its scope
    CodeSectionScope(EsScriptCompiler& compiler, const EsScriptCodeSection::Ptr& cs) ES_NOTHROW :
    m_compiler(compiler),
    m_prev(compiler.m_codeScope),
    m_cs(cs)
    {
      ES_ASSERT(m_cs);

      m_compiler.m_codeScope = this;
      ESSCRIPT_COMPILER_TRACE2( esT("CodeSectionScope(compiler, cs='%s')"), m_cs->nameGet() )
    }

    ~CodeSectionScope() ES_NOTHROW
    {
      const EsScriptInstructions& code = m_cs->codeAccess();
      if( !code.empty() )
      {
        EsScriptDebugInfoIntf::Ptr dbgStart;
        EsScriptDebugInfoIntf::Ptr dbgEnd;

        ulong line = static_cast<ulong>(-1);
        ulong lineBeg = line;
        ulong linePrev = line;
        RangeState state = RangeState::None;

        for(size_t idx = 0; idx < code.size(); ++idx)
        {
          EsScriptDebugInfoIntf::Ptr dbg = code[idx].debugInfoGet();
          if(!dbg)
            continue;

          line = dbg->lineGet()-1;

          if(!dbgStart)
          {
            dbgStart = dbg;
            lineBeg = line;
            linePrev = line;
            state = RangeState::RangeStarted;
          }
          else if(line-linePrev > 1)
          {
            if(RangeState::RangeStarted == state)
            {
              m_compiler.m_execRanges.push_back(
                EsRange::createMinInclMaxIncl(
                  lineBeg,
                  linePrev
                )
              );
              state = RangeState::None;
            }

            dbgStart = dbg;
            lineBeg = line;
            state = RangeState::RangeStarted;
          }

          linePrev = line;
        }

        if(RangeState::RangeStarted == state && static_cast<ulong>(-1) != lineBeg)
          m_compiler.m_execRanges.push_back(
            EsRange::createMinInclMaxIncl(
              lineBeg,
              linePrev
            )
          );
      }

      ESSCRIPT_COMPILER_TRACE2( esT("~CodeSectionScope() '%s'"), m_cs->nameGet() )
      m_compiler.m_codeScope = m_prev;
    }

    inline EsScriptCodeSection::Ptr sectionGet() const ES_NOTHROW { return m_cs; }

  protected:
    // data members
    EsScriptCompiler& m_compiler;
    CodeSectionScope* m_prev;
    EsScriptCodeSection::Ptr m_cs;

  private:
    // prohibited functionality
    CodeSectionScope() ES_REMOVEDECL;
    CodeSectionScope(const CodeSectionScope&) ES_REMOVEDECL;
    CodeSectionScope& operator=(const CodeSectionScope&) ES_REMOVEDECL;
  };
  friend class CodeSectionScope;
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------

  // object|namespace automatic compilation scope
  class ObjectScope
  {
  public:
    // try to declare object and enter its scope
    ObjectScope(EsScriptCompiler& compiler, EsScriptMachine& script, const EsString& obj, const EsString& baseObj) :
    m_compiler(compiler),
    m_prev(compiler.m_objectScope)
    {
      ES_ASSERT(!obj.empty());

      m_compiler.m_objectScope = this;
      m_metaclass = script.metaclassDeclare(obj, baseObj);
      ESSCRIPT_COMPILER_TRACE3(esT("ObjectScope(compiler, script, obj='%s', baseObj='%s')"), obj, baseObj)
    }

    ObjectScope(EsScriptCompiler& compiler, const EsScriptObjectIntf::Ptr& fld) :
    m_compiler(compiler),
    m_prev(m_compiler.m_objectScope),
    m_metaclass(fld)
    {
      ES_ASSERT(m_prev);
      ES_ASSERT(m_metaclass);
      m_compiler.m_objectScope = this;
      ESSCRIPT_COMPILER_TRACE2(esT("ObjectScope(compiler, fld='%s')"), m_metaclass->typeNameGet())
    }

    ~ObjectScope()
    {
      ESSCRIPT_COMPILER_TRACE2(esT("~ObjectScope() '%s'"), m_metaclass->typeNameGet())
      m_compiler.m_objectScope = m_prev;
    }

    EsScriptObjectIntf::Ptr metaclassGet() const
    {
      return m_metaclass;
    }

    EsScriptObjectIntf::Ptr topMetaclassGet() const
    {
      const ObjectScope* top = this;
      const ObjectScope* prev = m_prev;
      while( prev )
      {
        top = prev;
        prev = prev->m_prev;
      }

      ES_ASSERT(top);
      return top->metaclassGet();
    }

  private:
    // prohibited functionality
    ObjectScope() ES_REMOVEDECL;
    ObjectScope(const ObjectScope&) ES_REMOVEDECL;
    ObjectScope& operator=(const ObjectScope&) ES_REMOVEDECL;

  protected:
    // data members
    EsScriptCompiler& m_compiler;
    ObjectScope* m_prev;
    EsScriptObjectIntf::Ptr m_metaclass;
  };
  friend class ObjectScope;
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------

  // break|continue automatic guard objects
  class BreakContinueGuard
  {
  public:
    enum Guard {
      breakGuard,
      continueGuard
    };

  public:
    BreakContinueGuard(EsScriptCompiler& compiler, Guard guard, EsScriptCodeSection::Ptr code) :
    m_compiler(compiler),
    m_guard(guard),
    m_prev((breakGuard == m_guard) ? m_compiler.m_breakGuard : m_compiler.m_continueGuard),
    m_code(code),
    m_fixup(0)
    {
      m_instrPositions.reserve(16);
      if(breakGuard == m_guard)
        m_compiler.m_breakGuard = this;
      else
        m_compiler.m_continueGuard = this;
    }

    ~BreakContinueGuard()
    {
      // apply instruction fixups
      while(!m_instrPositions.empty())
      {
        ulong pos = m_instrPositions.back();
        m_instrPositions.pop_back();

        m_code->instructionModifyAt(
          pos
        ).jumpOpcodeSet(
          joAbs
        ).jumpOffsSet(
          m_fixup
        );
      }

      if(breakGuard == m_guard)
        m_compiler.m_breakGuard = m_prev;
      else
        m_compiler.m_continueGuard = m_prev;
    }

    void instructionAddPos(ulong pos)
    {
      m_instrPositions.push_back(pos);
    }

    void setFixup(ulong fixup)
    {
      m_fixup = fixup;
    }

  private:
    // prohibited functionality
    BreakContinueGuard() ES_REMOVEDECL;
    BreakContinueGuard(const BreakContinueGuard&) ES_REMOVEDECL;
    BreakContinueGuard& operator =(const BreakContinueGuard&) ES_REMOVEDECL;

  protected:
    EsScriptCompiler& m_compiler;
    Guard m_guard;
    BreakContinueGuard* m_prev;
    EsScriptCodeSection::Ptr m_code;
    std::vector<ulong> m_instrPositions;
    ulong m_fixup;
  };
  friend class BreakContinueGuard;
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------

  class LabelGuard
  {
  public:
    LabelGuard(EsScriptCompiler& compiler, EsScriptCodeSection::Ptr code) :
    m_compiler(compiler),
    m_code(code),
    m_prev(m_compiler.m_labelGuard)
    {
      m_compiler.m_labelGuard = this;
    }

    ~LabelGuard()
    {
      for(Gotos::const_iterator cit = m_gotos.begin(); cit != m_gotos.end(); ++cit)
      {
        const EsString& name = (*cit).first;
        ulong instrPos = (*cit).second;

        // lookup label
        auto lblIt = m_labels.find(name);
        if( m_labels.end() != lblIt )
        {
          m_code->instructionModifyAt(
            instrPos
          ).jumpOpcodeSet(
            joAbs
          ).jumpOffsSet(
            (*lblIt).second
          );
        }
        // todo: !!!!! else
        //  EsScriptException::Throw( EsString::format(esT("goto target '%s' is not declared"), name.c_str()),
        //  EsScriptDebugInfo::create( );
      }

      m_compiler.m_labelGuard = m_prev;
    }

    void addLabel(const EsString& name)
    {
      if( m_labels.end() != m_labels.find(name) )
        EsScriptException::Throw(
          EsString::format(
            esT("label '%s' is already declared"),
            name
          )
        );

      m_labels[name] = m_code->instructionEndPosGet();
    }

    void addGoto(const EsString& name, EsString::const_pointer pos)
    {
      ulong gotoPos;
      m_code->instructionAdd(
        gotoPos,
        iJump,
        m_compiler.debugInfoCreateAt(pos)
      );

      m_gotos.push_back(
        Goto(
          name,
          gotoPos
        )
      );
    }

  private:
    // prohibited functionality
    LabelGuard() ES_REMOVEDECL;
    LabelGuard(const LabelGuard&) ES_REMOVEDECL;
    LabelGuard& operator =(const LabelGuard&) ES_REMOVEDECL;

  protected:
    typedef std::map<EsString, ulong> Map;
    typedef std::pair<EsString, ulong> Goto;
    typedef std::list<Goto> Gotos;

    EsScriptCompiler& m_compiler;
    EsScriptCodeSection::Ptr m_code;
    Map m_labels;
    Gotos m_gotos;
    LabelGuard* m_prev;
  };
  friend class LabelGuard;
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------

  class SwitchScope
  {
  private:
    enum {
      defJumpoverSize = 16
    };

  public:
    SwitchScope(EsScriptCompiler& compiler, EsScriptCodeSection::Ptr code) :
    m_compiler(compiler),
    m_code(code),
    m_prev(m_compiler.m_switchScope),
    m_nesting(0),
    m_caseEntryCnt(0),
    m_caseEntries(0)
    {
      m_compiler.m_switchScope = this;

      if( m_prev )
        m_nesting = m_prev->m_nesting+1;

      m_argname = EsString::format(
        esT("%d||ss_arg"),
        m_nesting
      );

      m_jumpovers.reserve(
        defJumpoverSize
      );
      m_endjumps.reserve(
        defJumpoverSize
      );
    }

    ~SwitchScope()
    {
      ESSCRIPT_COMPILER_TRACE1(esT("~SwitchScope: Applying endjumps fixups"))

      // Apply all end jump fixups
      for(auto jmp: m_endjumps)
      {
        m_code->instructionModifyAt(
          jmp
        ).jumpOpcodeSet(
          joAbs
        ).jumpOffsSet(
          m_code->instructionEndPosGet()
        );
      }

      m_compiler.m_switchScope = m_prev;
    }

    // Access switch-specific argname
    inline const EsString& argnameGet() const { return m_argname; }

    // Increment case entry counter
    inline void caseEntryCntInc() { ++m_caseEntryCnt; }

    // Access case entry counter
    inline ulong caseEntryCntGet() const { return m_caseEntryCnt; }

    // Assign case entries count
    inline void caseEntriesCntSet(ulong cnt) { m_caseEntries = cnt; }

    void previousCaseEntryJumpHere()
    {
      if(0 == m_caseEntryCnt) //< Skip for the first entry
        return;

      ulong prevCaseJumpOverPos = m_jumpovers[m_caseEntryCnt-1];
      ulong here = m_code->instructionEndPosGet();

      m_code->instructionModifyAt(
        prevCaseJumpOverPos
      ).jumpOpcodeSet(
        joAbs
      ).jumpOffsSet(
        here
      );

      ESSCRIPT_COMPILER_TRACE3(
        esT("SwitchScope::previousCaseEntryJumpHere fixup applied for case entry %d, jump pos is %d"),
        m_caseEntryCnt-1,
        here
      )
    }

    void nextCaseEntryOrEndJumpAdd(const EsScriptDebugInfoIntf::Ptr& dbg)
    {
      if( m_caseEntryCnt == m_caseEntries-1 ) //< Add an end jump for the last entry
      {
        switchEndJumpAdd(dbg);
        return;
      }

      ulong pos;
      m_code->instructionAdd(
        pos,
        iJump,
        dbg
      );

      m_jumpovers.push_back(
        pos
      );

      ESSCRIPT_COMPILER_TRACE3(
        esT("SwitchScope::nextCaseEntryJumpAdd for entry %d, jump pos is %d"),
        m_jumpovers.size()-1,
        pos
      )
    }

    void switchEndJumpAdd(const EsScriptDebugInfoIntf::Ptr& dbg)
    {
      ulong pos;
      m_code->instructionAdd(
        pos,
        iJump,
        dbg
      );

      m_endjumps.push_back(
        pos
      );

      ESSCRIPT_COMPILER_TRACE3(
        esT("SwitchScope::switchEndJumpAdd for entry %d, jump pos is %d"),
        m_endjumps.size()-1,
        pos
      )
    }

  private:
    // prohibited functionality
    SwitchScope() ES_REMOVEDECL;
    SwitchScope(const SwitchScope&) ES_REMOVEDECL;
    SwitchScope& operator =(const SwitchScope&) ES_REMOVEDECL;

  protected:
    EsScriptCompiler& m_compiler;
    EsScriptCodeSection::Ptr m_code;
    std::vector<ulong> m_jumpovers;
    std::vector<ulong> m_endjumps;
    SwitchScope* m_prev;
    EsString m_argname;
    ulong m_nesting;
    ulong m_caseEntryCnt;
    ulong m_caseEntries;
  };
  friend class SwitchScope;
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------

  // Script element compilers
  //
  void compileStmtFor(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(stmtForID == (*cit).value.id().to_long());
    ES_ASSERT(4 == (*cit).children.size());

    ES_ASSERT(m_codeScope);
    EsScriptCodeSection::Ptr code = m_codeScope->sectionGet();

    // set-up break and continue guards
    BreakContinueGuard breakGuard(*this, BreakContinueGuard::breakGuard, code);
    BreakContinueGuard continueGuard(*this, BreakContinueGuard::continueGuard, code);
    // compile for init statement
    ParseTreeConstIteratorT child = (*cit).children.begin();
    if(!(*child).children.empty())
      compileStmt(
        (*child).children.begin(),
        script
      );
    ++child;
    // save current instruction position as loop start position
    ulong loopStartPos = code->instructionEndPosGet();
    // compile check expression
    ulong forCheckPos = static_cast<ulong>(-1);
    if(!(*child).children.empty())
    {
      compileExpr((*child).children.begin(), script);
      // issue loop check instruction, save position for later
      ulong checkPos;
      code->instructionAdd(
        checkPos,
        iJumpFalse,
        debugInfoCreateAt(child)
      );

      forCheckPos = checkPos;
    }
    ++child;
    // save counter expression block for later
    ParseTreeConstIteratorT cntExpr = child++;
    // compile _for_ code block
    compileStmt(child, script);

    // finally, compile _for_ counter expression
    ulong forCounterStartPos = code->instructionEndPosGet(); //< remember for counter jump pos
    compileForCountExpr(
      cntExpr,
      script
    );

    // add jump to the loop start point
    code->instructionAdd(
      iJump,
      debugInfoCreateAt(child)
    ).jumpOpcodeSet(
      joAbs
    ).jumpOffsSet(
      loopStartPos
    );

    // set break|continue fixups
    breakGuard.setFixup(
      code->instructionEndPosGet()
    );
    continueGuard.setFixup(
      forCounterStartPos
    );

    // modify iJumpFalse to jump out of the _for_ loop
    if( static_cast<ulong>(-1) != forCheckPos )
    {
      code->instructionModifyAt(
        forCheckPos
      ).jumpOpcodeSet(
        joAbs
      ).jumpOffsSet(
        code->instructionEndPosGet()
      );
    }
  }
  //---------------------------------------------------------------------------

  void compileExprForEachCheck(
    ParseTreeConstIteratorT cit,
    const EsScriptCodeSection::Ptr& code,
    ulong& loopStartPos,
    ulong& loopCheckPos,
    EsScriptMachine& script)
  {
    ES_ASSERT(exprForEachCheckID == (*cit).value.id().to_long());
    ES_ASSERT(2 == (*cit).children.size());

    ParseTreeConstIteratorT child = (*cit).children.begin();
    // item holder variable
    EsString varname = compileIdent(child++);

    // foreach subject expression compile
    compileExpr(
      child,
      script
    );

    code->instructionAdd(
      iAutoItemRefCreate,
      debugInfoCreateAt(child)
    );
    // here is the actual loop entry point
    loopStartPos = code->instructionEndPosGet();

    // load the loop check expression
    code->instructionAdd(
      iIsOkLoad,
      debugInfoCreateAt(child)
    );

    code->instructionAdd(
      loopCheckPos,
      iJumpFalse,
      debugInfoCreateAt(child)
    );

    // the loop iterator variable assignment
    code->instructionAdd(
      iAutoItemLoad,
      debugInfoCreateAt(child)
    );

    code->instructionAdd(
      iVarInit,
      debugInfoCreateAt(child)
    ).nameSet(
      varname
    );
  }
  //---------------------------------------------------------------------------

  void compileStmtForEach(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(stmtForEachID == (*cit).value.id().to_long());
    ES_ASSERT(2 == (*cit).children.size());

    ES_ASSERT(m_codeScope);
    EsScriptCodeSection::Ptr code = m_codeScope->sectionGet();

    // set-up break and continue guards
    BreakContinueGuard breakGuard(
      *this,
      BreakContinueGuard::breakGuard,
      code
    );
    BreakContinueGuard continueGuard(
      *this,
      BreakContinueGuard::continueGuard,
      code
    );

    ulong loopStartPos, loopCheckPos;
    // compile foreach check with assign expression
    // compile foreach init
    ParseTreeConstIteratorT child = (*cit).children.begin();
    compileExprForEachCheck(
      child++,
      code,
      loopStartPos,
      loopCheckPos,
      script
    );

    // compile _foreach_ code block
    compileStmt(child, script);

    // do jump to the loop start point
    code->instructionAdd(
      iJump,
      debugInfoCreateAt(child)
    ).jumpOpcodeSet(
      joAbs
    ).jumpOffsSet(
      loopStartPos
    );

    // set break|continue fixups
    breakGuard.setFixup(
      code->instructionEndPosGet()
    );
    continueGuard.setFixup(
      loopStartPos
    );

    // modify iJumpFalse to jump out of the _foreach_ loop
    code->instructionModifyAt(
      loopCheckPos
    ).jumpOpcodeSet(
      joAbs
    ).jumpOffsSet(
      code->instructionEndPosGet()
    );

    // add instruction to cleanup auto indexed item from data stack
    code->instructionAdd(
      iRetCleanup,
      debugInfoCreateAt(child)
    );
  }
  //---------------------------------------------------------------------------

  void compileStmtWhile(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT(stmtWhileID == id || stmtDoWhileID == id);
    ES_ASSERT((*cit).children.size()==2);

    ES_ASSERT(m_codeScope);
    EsScriptCodeSection::Ptr code = m_codeScope->sectionGet();

    // set-up break and continue guards
    BreakContinueGuard breakGuard(
      *this,
      BreakContinueGuard::breakGuard,
      code
    );
    BreakContinueGuard continueGuard(
      *this,
      BreakContinueGuard::continueGuard,
      code
    );

    // save loop start point
    ulong loopStart = code->instructionEndPosGet();
    ParseTreeConstIteratorT child = (*cit).children.begin();

    ulong whileCheckPos;
    if( stmtWhileID == id )
    {
      // compile check expression
      ES_ASSERT(!(*child).children.empty());
      compileExpr(
        (*child).children.begin(),
        script
      );

      // issue loop check instruction, saving position for later
      code->instructionAdd(
        whileCheckPos,
        iJumpFalse,
        debugInfoCreateAt(child)
      );
      ++child;
    }

    // compile _while_ code block
    compileStmt(
      child,
      script
    );

    if( stmtDoWhileID == id )
    {
      whileCheckPos = code->instructionEndPosGet(); //< We fill in pos before check expr here
      ++child;

      // compile check expression
      ES_ASSERT(!(*child).children.empty());
      compileExpr(
        (*child).children.begin(),
        script
      );

      // jump, if true, to the beginning of the loop
      code->instructionAdd(
        iJumpTrue,
        debugInfoCreateAt(child)
      ).jumpOpcodeSet(
        joAbs
      ).jumpOffsSet(
        loopStart
      );
    }
    else
    {
      // issue iJump instruction to the start of the loop section
      code->instructionAdd(
        iJump,
        debugInfoCreateAt(cit)
      ).jumpOpcodeSet(
        joAbs
      ).jumpOffsSet(
        loopStart
      );

      // modify while check instruction
      code->instructionModifyAt(
        whileCheckPos
      ).jumpOpcodeSet(
        joAbs
      ).jumpOffsSet(
        code->instructionEndPosGet()
      );
    }

    // set break|continue fixups
    breakGuard.setFixup(
      code->instructionEndPosGet()
    );

    continueGuard.setFixup(
      (stmtWhileID == id) ?
        loopStart :
        whileCheckPos
    );
  }
  //---------------------------------------------------------------------------

  // compile break and continue
  void compileStmtBreakContinue(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT(stmtBreakID == id || stmtContinueID == id);

    ES_ASSERT(m_codeScope);
    EsScriptCodeSection::Ptr code = m_codeScope->sectionGet();

    if(stmtBreakID == id)
    {
      if (m_breakGuard)
      {
        ulong pos;
        code->instructionAdd(
          pos,
          iJump,
          debugInfoCreateAt(cit)
        );

        m_breakGuard->instructionAddPos( pos );
      }
      else
        EsScriptException::Throw(
          esT("'break' without loop block"),
          debugInfoCreateAt(cit)
        );
    }
    else if (m_continueGuard)
    {
      ulong pos;
      code->instructionAdd(
        pos,
        iJump,
        debugInfoCreateAt(cit)
      );

      m_continueGuard->instructionAddPos( pos );
    }
    else
        EsScriptException::Throw(
          esT("'continue' without loop block"),
          debugInfoCreateAt(cit)
        );
  }
  //---------------------------------------------------------------------------

  void compileStmtLabelGoto(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT(stmtLabelID == id || stmtGotoID == id);
    ES_ASSERT(nullptr != m_labelGuard);
    EsString name = compileIdent((*cit).children.begin());
    if( stmtLabelID == id )
      m_labelGuard->addLabel( name );
    else
      m_labelGuard->addGoto( name, (*cit).value.begin() );
  }
  //---------------------------------------------------------------------------

  void compileAttribute(ParseTreeConstIteratorT cit, EsScriptMachine& script, const EsAttributesIntf::Ptr& attrs)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT(stmtAttributeDeclID == id);
    ES_ASSERT(0 < (*cit).children.size() && (*cit).children.size() <= 2 );

    ParseTreeConstIteratorT child = (*cit).children.begin();
    const EsString& name = compileIdent(
      child++,
      false
    );

    attrs->attributeAdd(
      name,
      (child != (*cit).children.end()) ?
        compileConstExpr(
          child,
          script
        ) :
        EsVariant::null()
    );
  }
  //---------------------------------------------------------------------------

  void compileAttributesList(ParseTreeConstIteratorT cit, EsScriptMachine& script, const EsAttributesIntf::Ptr& attrs)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT(stmtAttributeListID == id);

    for( ParseTreeConstIteratorT attrIt = (*cit).children.begin(); attrIt != (*cit).children.end(); ++attrIt )
      compileAttribute(
        attrIt,
        script,
        attrs
      );
  }
  //---------------------------------------------------------------------------

  void compileAttributes(ParseTreeConstIteratorT cit, EsScriptMachine& script, const EsAttributesIntf::Ptr& attrs)
  {
    ES_ASSERT(attrs);

    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT( isAttrListOrAttrDecl(id) );

    if( stmtAttributeListID == id )
      compileAttributesList(
        cit,
        script,
        attrs
      );
    else
      compileAttribute(
        cit,
        script,
        attrs
      );
  }
  //---------------------------------------------------------------------------

  void compileStmtEnumItemDecl(const EsEnumerationIntf::Ptr& enu, ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT(stmtEnumItemDeclID == id);

    ParseTreeConstIteratorT child = (*cit).children.begin();
    // item name
    const EsString& name = compileIdent(child++);
    // item value
    const EsVariant& val = compileConstExpr(
      child,
      script
    );

    if( val.isEmpty() )
      EsScriptException::Throw(
        EsString::format(
          esT("Could not declare enumeration item %s$$%s with null value"),
          enu->typeNameGet(),
          name
        ),
        debugInfoCreateAt(child)
      );

    ++child; //< Optional label expression, if not == end
    script.enumItemDeclare(
      enu,
      name,
      val,
      (child != (*cit).children.end()) ?
        stringValCompile(
          child,
          script,
          true
        ) :
        EsString::null()
    );
  }
  //---------------------------------------------------------------------------

  void compileStmtEnumDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT(stmtEnumDeclID == id);

    ParseTreeConstIteratorT child = (*cit).children.begin();
    // compile enumeration name && declare enumeration
    const EsString& name = compileIdent(child++);
    EsEnumerationIntf::Ptr enu = script.enumDeclare( name );
    EsReflectedClassIntf::Ptr r = enu;
    ES_ASSERT(r);

    // compile optional attribute(s)
    if(
      child != (*cit).children.end() &&
      isAttrListOrAttrDecl( static_cast<long>((*child).value.id().to_long()) )
    )
    {
      EsAttributesIntf::Ptr attrs = EsAttributes::create(
        enu->typeNameGet(),
        false
      );
      ES_ASSERT(attrs);

      compileAttributes(
        child++,
        script,
        attrs
      );

      r->attributesAssign(attrs);
    }

    // compile optional items
    while( child != (*cit).children.end() )
      compileStmtEnumItemDecl(
        enu,
        child++,
        script
      );
  }
  //---------------------------------------------------------------------------

  void compileStmtObjDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT( stmtObjDeclID == id );
    ES_ASSERT( (*cit).children.size() > 0 );

    ParseTreeConstIteratorT child = (*cit).children.begin();
    // compile object
    const EsString& obj = compileIdent(child++);
    // compile optional base class spec
    EsString baseObj;
    if(identID == (*child).value.id().to_long())
      baseObj = compileIdent(child++);

    // declare new object and enter its scope
    ObjectScope scope(
      *this,
      script,
      obj,
      baseObj
    );

    // optionally compile attributes
    if(
      child != (*cit).children.end() &&
      isAttrListOrAttrDecl( static_cast<long>((*child).value.id().to_long()) )
    )
    {
      EsAttributesIntf::Ptr attrs = EsAttributes::create(
        obj,
        false
      );
      ES_ASSERT(attrs);

      compileAttributes(
        child++,
        script,
        attrs
      );

      EsReflectedClassIntf::Ptr robj = scope.metaclassGet();
      ES_ASSERT(robj);
      robj->attributesAssign(attrs);
    }

    // dive in metaclass compilation
    if( child != (*cit).children.end() )
      compileStmt(
        child,
        script
      );
  }
  //---------------------------------------------------------------------------

  void compileFldExpr(ParseTreeConstIteratorT cit, EsScriptMachine& script, const EsScriptObjectIntf::Ptr& fldMetaclass, const EsScriptCodeSection::Ptr& code)
  {
    // enter field expression scope
    CodeSectionScope exprScope(
      *this,
      code
    );

    // compile conditional expression
    compileExpr(
      cit,
      script
    );
  }
  //---------------------------------------------------------------------------

  void compileIfFldBranch(ParseTreeConstIteratorT cit, EsScriptMachine& script, const EsScriptObjectIntf::Ptr& ifMetaclass, int branchSelection)
  {
    EsScriptObjectIntf::Ptr branch = ifMetaclass->conditionalBranchGet(branchSelection);
    ES_ASSERT(branch);

    ObjectScope branchScope(*this, branch);

    compileStmt(
      cit,
      script
    );
  }
  //---------------------------------------------------------------------------

  EsScriptObjectIntf::Ptr compileStmtObjFldIfDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(stmtObjFldIfDeclID == (*cit).value.id().to_long());

    ES_ASSERT((*cit).children.size() >= 2);
    ParseTreeConstIteratorT child = (*cit).children.begin();

    // use names autogeneration for conditional fields
    EsScriptMachine::CompoundFieldCreationResult conditional = script.metaclassIfFieldDeclare(
      m_objectScope->metaclassGet()
    );

    // enter if metaclass scope
    ObjectScope ifScope(*this, conditional.m_fieldMetaclass);

    // compile conditional expression
    compileFldExpr(child++, script, conditional.m_fieldMetaclass, conditional.m_fieldExpr);

    // proceed to the 'if' internals
    // compile 'true' branch as single field or nested object declaration block statement
    compileIfFldBranch(child++, script, conditional.m_fieldMetaclass, 1);

    // optionally, compile false branch
    if( (*cit).children.end() != child )
    {
      ES_ASSERT( stmtObjFldElseID == (*child).value.id().to_long() );
      ES_ASSERT( !(*child).children.empty() );

      // compile 'false' branch as single field or nested object declaration block statement
      compileIfFldBranch(
        (*child).children.begin(),
        script,
        conditional.m_fieldMetaclass,
        0
      );
    }

    return conditional.m_fieldMetaclass;
  }
  //---------------------------------------------------------------------------

  EsScriptObjectIntf::Ptr compileStmtObjFldDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script, EsString* fldName = 0)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT(stmtObjPlainFldDeclID == id || stmtObjArrayFldDeclID == id || stmtObjFldIfDeclID == id);
    ES_ASSERT((*cit).children.size() >= 2);

    if( stmtObjFldIfDeclID == id )
      // compile conditional object field
      return compileStmtObjFldIfDecl(cit, script);
    else
    {
      ParseTreeConstIteratorT child = (*cit).children.begin();
      // compile field type
      const EsString& fieldType = compileIdent(child++);
      // compile field name
      const EsString& fieldName = compileIdent(child++);
      if( fldName )
        *fldName = fieldName;

      if( stmtObjPlainFldDeclID == id )
        // add field to metaclass currently being compiled
        return script.metaclassFieldDeclare(m_objectScope->metaclassGet(), fieldType, fieldName);
      else // compile array field
      {
        EsScriptMachine::CompoundFieldCreationResult arrayFldResult =
          script.metaclassArrayFieldDeclare(m_objectScope->metaclassGet(), fieldType, fieldName);
        // compile array field size expression
        compileFldExpr(child, script, arrayFldResult.m_fieldMetaclass, arrayFldResult.m_fieldExpr);
        return arrayFldResult.m_fieldMetaclass;
      }
    }
  }
  //---------------------------------------------------------------------------

  void compileStmtObjMemberVarDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(stmtObjMemberVarDeclID == (*cit).value.id().to_long());
    ES_ASSERT(m_objectScope);

    // compile member variables
    for(ParseTreeConstIteratorT child = (*cit).children.begin(); child != (*cit).children.end(); ++child)
    {
      long id = static_cast<long>((*child).value.id().to_long());
      ES_ASSERT(identID == id);
      // add variable to the current object's member symtable & create memslot for it
      script.metaclassMemberVarDeclare(
        m_objectScope->metaclassGet(),
        compileIdent(child),
        debugInfoCreateAt(child)
      );
    }
  }
  //---------------------------------------------------------------------------

  EsString compileExprObjPropertyReaderDecl(const EsString& name, ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(exprObjPropertyReaderDeclID == (*cit).value.id().to_long());
    ES_ASSERT(m_objectScope);
    ParseTreeConstIteratorT child = (*cit).children.begin();

    // declare property reader as current object method
    EsScriptObjectIntf::Ptr metaclass = currentMetaclassGet();
    ES_ASSERT( metaclass );
    EsString readerName = name+esT("_read");
    EsScriptCodeSection::Ptr reader = script.methodDeclare(readerName, EsString::nullArray(), metaclass);
    compileMethodDecl(child, script, reader, metaclass);

    return readerName;
  }
  //---------------------------------------------------------------------------

  EsString compileExprObjPropertyWriterDecl(const EsString& name, ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(exprObjPropertyWriterDeclID == (*cit).value.id().to_long());
    ES_ASSERT(m_objectScope);
    ParseTreeConstIteratorT child = (*cit).children.begin();

    // declare new global script function or object method
    EsScriptObjectIntf::Ptr metaclass = currentMetaclassGet();
    ES_ASSERT( metaclass );
    EsStringArray params;
    params.push_back(esT("__value"));
    EsString writerName = name+esT("_write");
    EsScriptCodeSection::Ptr writer = script.methodDeclare(writerName, params, metaclass);
    compileMethodDecl(child, script, writer, metaclass);

    return writerName;
  }
  //---------------------------------------------------------------------------

  void compileStmtObjPropDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT(stmtObjPropertyDeclID == id);
    ES_ASSERT(m_objectScope);

    ParseTreeConstIteratorT child = (*cit).children.begin();

    // property name
    const EsString& name = compileIdent(child++);

    EsAttributesIntf::Ptr attrs;

    id = static_cast<long>((*child).value.id().to_long());
    if( isAttrListOrAttrDecl(id) )
    {
      attrs = EsAttributes::create(
        name,
        false
      );
      ES_ASSERT(attrs);

      compileAttributes(
        child++,
        script,
        attrs
      );
    }

    // compile property reader|writer declarations
    EsString readerName;
    EsString writerName;
    while(child != (*cit).children.end())
    {
      id = static_cast<long>((*child).value.id().to_long());
      if( exprObjPropertyReaderDeclID == id )
        readerName = compileExprObjPropertyReaderDecl(
          name,
          child,
          script
        );
      else
        writerName = compileExprObjPropertyWriterDecl(
          name,
          child,
          script
        );

      ++child;
    }

    EsScriptObjectIntf::Ptr metaclass = currentMetaclassGet();
    ES_ASSERT( metaclass );
    metaclass->propertyDeclare(
      name,
      attrs,
      readerName,
      writerName
    );
  }
  //---------------------------------------------------------------------------

  void compileStmtObjAttrDecls(EsScriptObjectIntf::Ptr obj, ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT( isAttrListOrAttrDecl(id) );

    if(stmtAttributeListID == id)
    {
      for( ParseTreeConstIteratorT attrIt = (*cit).children.begin(); attrIt != (*cit).children.end(); ++attrIt )
        compileStmtObjAttrDecls(
          obj,
          attrIt,
          script
        );
    }
    else
    {
      ES_ASSERT(0 < (*cit).children.size() && (*cit).children.size() <= 2 );

      ParseTreeConstIteratorT child = (*cit).children.begin();
      EsString name = compileIdent(
        child++,
        false
      );

      EsVariant val;
      // compile optional value|enumeration reference
      if( child != (*cit).children.end() )
        val = compileConstExpr(
          child,
          script
        );

      script.objectAttributeDeclare(
        obj,
        name,
        val
      );
    }
  }
  //---------------------------------------------------------------------------

  void compileStmtObjFldWithAttrsDecl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT(stmtObjFldDeclID == id);
    ES_ASSERT((*cit).children.size() == 2);

    ParseTreeConstIteratorT child = (*cit).children.begin();

    EsString fldName;
    EsScriptObjectIntf::Ptr field = compileStmtObjFldDecl(child++, script, &fldName);
    ES_ASSERT(field);

    // compile field attributes
    EsAttributesIntf::Ptr attrs = EsAttributes::create(
      fldName,
      false
    );
    ES_ASSERT(attrs);

    compileAttributes(
      child,
      script,
      attrs
    );

    m_objectScope->metaclassGet()->thisFieldAttributesAssign(attrs);
  }
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------

  // Expression compiler entry point
  //
  void compileExpr(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());

    if( isBinOp(id) )
      compileExprBinaryOp(
        cit,
        script
      );
    else if( isUnaryOp(id) )
      compileExprUnaryOp(
        cit,
        script
      );
    else if( opConditionalID == id )
      compileExprTernary(
        cit,
        script
      );
    else if( isSimpleVal(id) ) //< value - just push it onto data stack as unnamed constant
      compileLiteralSimpleValLoad(
        cit,
        script
      );
    else if( isRangeVal(id) )
      compileLiteralRangeValLoad(
        cit,
        script
      );
    else if(arrayValID == id) //< Decide if we're having constexpr or temporary collection expr here
      compileLiteralArrayValLoadOrTemporaryCollection(
        cit,
        script
      );
    else if( isKindOfLrhs(id) )
      compileLrhs(
        cit,
        script
      );
    else if( opAssignID == id )
      compileOpAssign(
        cit,
        script
      );
    else if( exprWildcardID == id ) //< Wildcard argument expression.
      compileWildcardArgExpr(
        cit,
        script
      );
    else //< should not be here! - unhandled AST node ID encountered
      EsScriptException::ThrowUnhandledAstNodeId(
        id,
        debugInfoCreateAt(
          cit
        )
      );
  }
  //---------------------------------------------------------------------------

  void compileLiteralSimpleValLoad(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(m_codeScope);

    m_codeScope->sectionGet()->instructionAdd(
      iLoadConstVal
    ).nameSet(
      (nullValID == (*cit).value.id().to_long()) ?
        EsScriptStdNames::null() :
        script.unnamedDeclare(
          compileSimpleValue(
            cit,
            script
          )
        )
    );
  }
  //---------------------------------------------------------------------------

  void compileLiteralRangeValLoad(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(m_codeScope);

    m_codeScope->sectionGet()->instructionAdd(
      iLoadConstVal
    ).nameSet(
      script.unnamedDeclare(
        compileRangeValue(
          cit,
          script
        )
      )
    );
  }
  //---------------------------------------------------------------------------

  void compileLiteralArrayValLoadOrTemporaryCollection(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT(arrayValID == id);

    if( isConstExpr( cit, script ) )
    {
      ES_ASSERT(m_codeScope);

      m_codeScope->sectionGet()->instructionAdd(
        iLoadConstVal
      ).nameSet(
        script.unnamedDeclare(
          compileArrayValue(
            cit,
            script
          )
        )
      );
    }
    else
    {
      // Push all item exprs to stack, and make temporary collection creation call
      for(ParseTreeConstIteratorT child = (*cit).children.begin(); child != (*cit).children.end(); ++child )
        compileExpr(
          child,
          script
        );

      ES_ASSERT(m_codeScope);

      m_codeScope->sectionGet()->instructionAdd(
        iLoadImmediateVal,
        debugInfoCreateAt(cit)
      ).immValTypeSet(
        EsIvalType::ivInplaceCollection
      ).immValArgsCntSet(
        static_cast<ulong>((*cit).children.size())
      );
    }
  }
  //---------------------------------------------------------------------------

  void compileWildcardArgExpr(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(exprWildcardID == (*cit).value.id().to_long());
    ES_ASSERT(m_codeScope);

    // TODO: extract the index of currently compiled call argument from parent context
    // So far, the only meaning of the wildcard argument is null value substitution
    //
    m_codeScope->sectionGet()->instructionAdd(
      iLoadConstVal,
      debugInfoCreateAt(cit)
    ).nameSet(
      EsScriptStdNames::null()
    );
  }
  //---------------------------------------------------------------------------

  void compileExprCall(ParseTreeConstIteratorT cit, EsScriptMachine& script, long explicitCallId = -1)
  {
    long id = static_cast<long>((*cit).value.id().to_long());

    ES_ASSERT(id >= exprFuncCallID && id < exprCallID);
    ES_ASSERT((*cit).children.size() > 0);

    if(
      objMethodCallID == explicitCallId ||
      varSvcCallID == explicitCallId
    )
      compileExplicitMethodCall(
        cit,
        script,
        explicitCallId
      );
    else if( exprObjCtrCallID == id )
      compileObjectCtrCall(
        (*cit).children.begin(),
        script
      );
    else if( exprStaticOrBaseCallID == id )
      compileStaticOrBaseCall(
        (*cit).children.begin(),
        script
      );
    else if( exprFuncCallID == id )
      compileFunctionOrMethodCall(
        cit,
        script
      );
    else
      EsScriptException::ThrowUnhandledAstNodeId(
        id,
        debugInfoCreateAt(
          cit
        )
      );
  }
  //---------------------------------------------------------------------------

  EsVariant compileEnumVal(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(enumValID == (*cit).value.id().to_long());
    ParseTreeConstIteratorT child = (*cit).children.begin();
    EsString enuName = compileIdent(child++);
    EsString sym = compileIdent(child);

    return script.enumValueGet(
      enuName,
      sym
    );
  }
  //---------------------------------------------------------------------------

  EsVariant compileEnumValLbl(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(enumValLblID == (*cit).value.id().to_long());
    ParseTreeConstIteratorT child = (*cit).children.begin();
    EsString enuName = compileIdent(child++);
    EsString sym = compileIdent(child);

    return script.enumValueLabelGet(
      enuName,
      sym
    );
  }
  //---------------------------------------------------------------------------

  void compileObjectCtrCall(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    // compile object constructor expression
    // move to the function expression at once
    ParseTreeConstIteratorT child = (*cit).children.begin();
    EsString objectName = compileIdent(child++);

    // get call parameters count
    ulong paramsCount = static_cast<ulong>((*cit).children.size()-1);

    // compile parameter expressions
    while(child != (*cit).children.end())
      compileExpr(
        child++,
        script
      );

    EsScriptDebugInfoIntf::Ptr dbg = debugInfoCreateAt(cit);

    // If the class name we use in CTR call is an external symbol - do not check
    // any signatures on compile time at all, postpone until the code is run
    //
    if( !script.isExternalMetaclass(objectName) )
    {
      EsMethodInfoKeyT ctrKey(paramsCount, EsStdNames::reflectedCtr());

      EsScriptObjectIntf::Ptr metaclass = script.metaclassFind(objectName);
      if( metaclass )
      {
        // check scripted constructor signature, if non-0 parameter count specified
        // (script object will always have implicit ctor with 0 parameters)
        if( paramsCount && !metaclass->findScriptedMethod( ctrKey ) )
          EsScriptException::ThrowCtorIsNotDeclared(
            objectName,
            paramsCount,
            dbg
          );
      }
      else // check reflected classes for any signature
      {
        const EsClassInfo* info = EsClassInfo::classInfoGet(
          objectName,
          false
        );

        if( info )
        {
          if(
            !info->classMethodInfoFind(
              ctrKey,
              false
            )
          )
            EsScriptException::ThrowCtorIsNotDeclared(
              objectName,
              paramsCount,
              dbg
            );
        }
        else
          EsScriptException::ThrowMetaclassDoesNotExist(
            objectName,
            dbg
          );
      }
    }

    // issue constructor call instruction
    ES_ASSERT(m_codeScope);

    m_codeScope->sectionGet()->instructionAdd(
      iConstructorCall,
      dbg
    ).nameSet(
      objectName
    ).argsCountSet(
      paramsCount
    );
  }
  //---------------------------------------------------------------------------

  void compileExplicitMethodCall(ParseTreeConstIteratorT cit, EsScriptMachine& script, long explicitCallId)
  {
    // The target object namespace is known only at runtime, when explicit method call
    // syntax is used, so issue reduced form of iMethodCall instruction, without namespace parameter
    //
    // move to the function expression
    ParseTreeConstIteratorT child = (*cit).children.begin();
    const EsString& name = compileIdent(child++);
    // get call parameters count
    ulong paramsCount = static_cast<ulong>((*cit).children.size()-1);

    // compile parameter expressions
    while(child != (*cit).children.end())
      compileExpr(
        child++,
        script
      );

    // in case of variant service call, we already can check service existence
    if( varSvcCallID == explicitCallId )
    {
      // increment parameters count, because variant services take the variant
      // itself as the first expected parameter
      ++paramsCount;

      EsMethodInfoKeyT key(
        paramsCount,
        name
      );

      if( !EsVar::classInfoGetStatic().hasClassMethod(key) )
        EsScriptException::Throw(
          EsString::format(
            script.loc(),
            esT("'%s' taking %d parameters is not a valid variant service"),
            name,
            paramsCount
          ),
          debugInfoCreateAt(cit)
        );
    }

    // finally, issue call instruction, specifying call parameters
    ES_ASSERT(m_codeScope);
    m_codeScope->sectionGet()->instructionAdd(
      (objMethodCallID == explicitCallId) ?
        iMethodCall :
        iVsvcCall,
      debugInfoCreateAt(cit)
    ).nameSet(
      name
    ).argsCountSet(
      paramsCount
    );
  }
  //---------------------------------------------------------------------------

  void compileStaticOrBaseCall(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(m_codeScope);
    EsScriptCodeSection::Ptr code = m_codeScope->sectionGet();

    // If the call namespace is script object, retrieve pointer to its metaclass
    EsScriptObjectIntf::Ptr obj;
    obj.reset( code->metaclassGet(), false );

    const EsString& targetNamespace = compileIdent(cit++);
    EsScriptObjectIntf::Ptr targetObj = script.metaclassFind(targetNamespace);

    // move to the function expression
    ParseTreeConstIteratorT child = (*cit).children.begin();
    EsString::Array allowedWords;
    if( m_objectScope )
    {
      if( code && 0 == code->nameGet().compare( esT("new") ) )
        allowedWords.push_back( esT("new") );
    }

    const EsString& name = compileIdent(
      child++,
      true,
      allowedWords
    );

    // get call parameters count
    ulong paramsCount = static_cast<ulong>((*cit).children.size()-1);
    EsScriptInstructionOpcode opcode = iFuncCall;
    EsMethodInfoKeyT methodKey(paramsCount, name);
    EsScriptCodeSection::Ptr method;
    EsScriptDebugInfoIntf::Ptr dbg = debugInfoCreateAt(cit);

    // decide on static call|override call
    if( targetObj )
    {
      if( obj )
      {
        EsReflectedClassIntf::Ptr robj = obj;
        ES_ASSERT(robj);

        if( robj->isKindOf( targetObj ) )
        {
          // lookup object method
          method = targetObj->findScriptedMethod( methodKey );
          if( method )
            // set call instruction opcode to the method call
            opcode = iMethodCall;
          else
            EsScriptException::Throw(
              EsString::format(
                script.loc(),
                esT("'%s' is not method of '%s' or its base objects"),
                name,
                targetNamespace
              ),
              dbg
            );
        }
        else
          EsScriptException::Throw(
            EsString::format(
              script.loc(),
              esT("'%s' is not base object of '%s'"),
              targetNamespace,
              obj->typeNameGet()
            ),
            dbg
          );
      }
      else
        EsScriptException::Throw(
          EsString::format(
            script.loc(),
            esT("Cannot call object method '%s' without an object"),
            name
          ),
        dbg
      );
    }
    else
    {
      // lookup external static services
      if( !targetNamespace.empty() )
      {
        if( script.isExternalMetaclass(targetNamespace) )
          opcode = iExtFuncCall;
        else
        {
          const EsClassInfo* info = EsClassInfo::classInfoGet(targetNamespace, false);
          if( info )
          {
            const EsMethodInfo* methodInfo = info->classMethodInfoFind(methodKey);
            if( methodInfo )
              opcode = iExtFuncCall;
            else
              EsScriptException::Throw(
                EsString::format(
                  script.loc(),
                  esT("'%s::%s', taking %d parameters, is not a known method"),
                  targetNamespace,
                  name,
                  paramsCount
                ),
                dbg
              );
          }
          else
            EsScriptException::ThrowUnknownNamespaceOrService(targetNamespace, dbg);
        }
      }
      else
        // Lookup global namespace, throw an exception on error
        method = script.globalMethodGet(methodKey);
    }

    // compile parameter expressions
    while(child != (*cit).children.end())
      compileExpr(
        child++,
        script
      );

    // format instruction parameters
    EsString::Array qname = {
      name
    };

    if( !targetNamespace.empty() )
      qname.push_back(
        targetNamespace
      );

    // finally, issue call instruction, specifying call parameters
    code->instructionAdd(
      opcode,
      dbg
    ).qnameSet(
      qname
    ).argsCountSet(
      paramsCount
    );
  }
  //---------------------------------------------------------------------------

  void compileFunctionOrMethodCall(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    bool found = false;
    ParseTreeConstIteratorT child = (*cit).children.begin();

    // get function or method name
    const EsString& name = compileIdent(child++);

    // get call parameters count
    ulong paramsCount = static_cast<ulong>((*cit).children.size()-1);

    // create method search key
    EsMethodInfoKeyT methodKey(
      paramsCount,
      name
    );

    // initialize instruction opcode
    EsScriptInstructionOpcode opcode = iFuncCall;

    // find if current namespace is scripted object
    EsScriptObjectIntf::Ptr obj = currentMetaclassGet();
    EsScriptCodeSection::Ptr method;
    if( obj )
    {
      // if so, try to find the object's method
      EsScriptObjectMethodFastFinder ffinder(obj.get(), methodKey);
      method = ffinder.resultGet();
      // if there is such method - call it
      if( method )
      {
        opcode = iMethodCall;
        found = true;
      }
      else
      {
        EsReflectedClassIntf::Ptr robj = obj;
        ES_ASSERT(robj);
        const EsClassInfo& info = robj->classInfoGet();

        // otherwise, try to find c++ reflected method
        const EsMethodInfo* methodInfo = info.methodInfoFind(methodKey);
        if( methodInfo )
        {
          opcode = iMethodCall;
          found = true;
        }
      }
    }

    if( !found )
    {
      // lookup global methods
      method = script.globalMethodGet(methodKey, false);
      if( method )
        found = true;
    }

    // create debug information
    EsScriptDebugInfoIntf::Ptr dbg = debugInfoCreateAt(cit);

    if( !found )
      EsScriptException::Throw(
        EsString::format(
          script.loc(),
          esT("Method '%s', taking %d parameters, is not declared"),
          name,
          paramsCount
        ),
        dbg
      );

    ES_ASSERT(m_codeScope);
    EsScriptCodeSection::Ptr code = m_codeScope->sectionGet();

    // for implicit method call = push 'this' variable on stack before other parameters
    if(iMethodCall == opcode)
    {
      code->instructionAdd(
        iLoadVarRef,
        dbg
      ).nameSet(
        EsScriptStdNames::_this() //< 'this' symbol is always local to current context
      ).variableTypeSet(
        EsVarType::vtLocal
      );
    }

    // compile parameters expressions
    while(child != (*cit).children.end())
      compileExpr(
        child++,
        script
      );

    // issue call instruction
    code->instructionAdd(
      opcode,
      dbg
    ).nameSet(
      name
    ).argsCountSet(
      paramsCount
    );
  }
  //---------------------------------------------------------------------------

  // binary operation expression compiler
  void compileExprBinaryOp(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    EsString op(
      (*cit).value.begin(),
      (*cit).value.end()
    );

    ES_ASSERT(opConditionalID <= id && id <= opMulDivID);
    ES_ASSERT((*cit).children.size() == 2);

    ParseTreeConstIteratorT child = (*cit).children.begin();

    // check for short logical expression eval,
    // perform full eval for others
    // evaluate left part
    compileExpr(
      child++,
      script
    );

    ulong instrPos = 0;
    ulong jumpOffs = 0;

    ES_ASSERT(m_codeScope);
    EsScriptCodeSection::Ptr code = m_codeScope->sectionGet();

    if( opLogicalID == id ) // issue short logical eval check instruction between left & right evals
      code->instructionAdd(
        instrPos,
        iLogicCheck,
        debugInfoCreateAt(child)
      ); // save current instruction idx

    // compile right part
    compileExpr(
      child,
      script
    );

    // issue binary operation instruction, calculate jump offset for logical check
    EsScriptOperatorIds opid = EsScriptOpIds::instanceGet().getId(op);
    ES_ASSERT( EsScriptOperatorIds::opidInvalid != opid );

    code->instructionAdd(
      jumpOffs,
      iBinOpExpr,
      debugInfoCreateAt(cit)
    ).operatorIdSet(
      opid
    );

    // finally, modify logcheck instruction at instrPos, if needed
    if( opLogicalID == id )
      code->instructionModifyAt(
        instrPos
      ).jumpOpcodeSet(
        joRel
      ).jumpOffsSet(
        jumpOffs-instrPos
      );
  }
  //---------------------------------------------------------------------------

  void compileExprUnaryOp(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());

    ES_ASSERT( isUnaryOp(id) );

    if( isIncDec(id) )
    {
      compileExprIncDec(
        cit,
        script
      );

      return;
    }

    EsScriptOperatorIds opid = EsScriptOpIds::instanceGet().getId(
      EsString(
        (*cit).value.begin(),
        (*cit).value.end()
      )
    );
    ES_ASSERT(opid == opidBwNot || opid == opidNot || opid == opidSub || opid == opidScopeAcc);
    ES_ASSERT((*cit).children.size()==1);

    // compile operand expression
    compileExpr(
      (*cit).children.begin(),
      script
    );

    ES_ASSERT(m_codeScope);
    // issue unary operator instruction
    m_codeScope->sectionGet()->instructionAdd(
      iUnaryOpExpr,
      debugInfoCreateAt(cit)
    ).operatorIdSet(
      opid
    );
  }
  //---------------------------------------------------------------------------

  void compileExprIncDec(ParseTreeConstIteratorT cit, EsScriptMachine& script, bool asStmt = false)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT(id == exprIncDecID || id == exprIncDecPfxID);

    ParseTreeConstIteratorT incDec;
    ParseTreeConstIteratorT operand;

    if( exprIncDecID == id )
    {
      operand = incDec = (*cit).children.begin();
      ++operand;
    }
    else
    {
      incDec = operand = (*cit).children.begin();
      ++incDec;
    }

    EsString op((*incDec).value.begin(), (*incDec).value.end());
    EsScriptOperatorIds opid = EsScriptOpIds::instanceGet().getId(op);
    ES_ASSERT(opidInc == opid || opidDec == opid);

    // first, evaluate operand as lhs
    compileLrhs(
      operand,
      script
    );

    ES_ASSERT(m_codeScope);
    m_codeScope->sectionGet()->instructionAdd(
      iIncDec,
      debugInfoCreateAt(cit)
    ).operatorIdSet(
      opid
    ).callAsStmtSet(
      asStmt
    ).isPostfixSet(
      static_cast<bool>(id == exprIncDecPfxID)
    );
  }
  //---------------------------------------------------------------------------

  void compileExprTernary(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(opConditionalID == (*cit).value.id().to_long());
    ES_ASSERT(3 == (*cit).children.size());

    ParseTreeConstIteratorT child = (*cit).children.begin();

    ES_ASSERT(m_codeScope);
    EsScriptCodeSection::Ptr code = m_codeScope->sectionGet();

    // compile if expression
    compileExpr(child++, script);

    // issue iJumpFalse instruction, saving its positon
    ulong jumpFalsePos;
    code->instructionAdd(
      jumpFalsePos,
      iJumpFalse,
      debugInfoCreateAt(child)
    );

    // compile 'true' expression
    compileExpr(
      child++,
      script
    );

    // add instruction to jump to the ending of entire if
    ulong jumpOutOfIfPos;
    code->instructionAdd(
      jumpOutOfIfPos,
      iJump,
      debugInfoCreateAt(child)
    );

    // modify iJumpFalse instruction, so as its target is instruction following the last instruction of the true block
    code->instructionModifyAt(
      jumpFalsePos
    ).jumpOpcodeSet(
      joAbs
    ).jumpOffsSet(
      code->instructionEndPosGet()
    );

    // next is 'false' expression section
    compileExpr(
      child,
      script
    );

    // finalize out-of-if jump instruction
    code->instructionModifyAt(
      jumpOutOfIfPos
    ).jumpOpcodeSet(
      joAbs
    ).jumpOffsSet(
      code->instructionEndPosGet()
    );
  }
  //---------------------------------------------------------------------------

  // Identifier and value compilers
  //
  EsString compileIdent(
    ParseTreeConstIteratorT cit,
    bool checkReserved = true,
    const EsString::Array& exceptions = EsString::nullArray()
  )
  {
    ES_ASSERT((*cit).value.id().to_long() == identID);
    EsString result((*cit).value.begin(), (*cit).value.end());

    if( checkReserved && exceptions.end() == std::find(exceptions.begin(), exceptions.end(), result) )
      EsScript::checkNotReservedWord(
        result,
        debugInfoCreateAt(cit)
      );

    return result;
  }
  //---------------------------------------------------------------------------

  EsVariant compileStrValHelper(ParseTreeConstIteratorT cit, EsScriptMachine& script, EsString& strVal, bool expectPfx = true)
  {
    EsString::value_type pfx = strVal[0];

    // handle special prefix for string
    if( esT('"') != pfx )
    {
      if(expectPfx)
        strVal = strVal.substr(1);
      else
        EsScriptException::Throw(
          esT("String prefix is not expected here"),
          debugInfoCreateAt(cit)
        );
    }

    strVal = EsString::fromString(
      strVal,
      EsString::StrQuote,
      script.loc()
    );

    if( esT('I') == pfx )
      script.translatableAdd(strVal);

    if( esT('B') == pfx )
      return EsStr::toByteString(strVal);

    return strVal;
  }
  //---------------------------------------------------------------------------

  EsVariant compileSimpleValue(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    EsString strVal(
      (*cit).value.begin(),
      (*cit).value.end()
    );

    switch( id )
    {
    case nullValID:
      ES_ASSERT(EsScriptStdNames::null() == strVal);
      return EsVariant::null();
    case boolValID:
      if(EsStdNames::_false() == strVal)
        return false;
      else if(EsStdNames::_true() == strVal)
        return true;
      ES_FAIL; // we should never be here
      return 0; // pacify compilers
    case intBinValID:
      return EsString::toULong(
        strVal,
        2,
        script.loc()
      );
    case intOctValID:
      return EsString::toULong(
        strVal,
        8,
        script.loc()
      );
    case intHexValID:
      return EsString::toULong(
        strVal,
        16,
        script.loc()
      );
    case intValID:
      return EsString::toLong(
        strVal,
        10,
        script.loc()
      );
    case floatValID:
      return EsString::toDouble(
        strVal,
        script.loc()
      );
    case charValID:
      {
        EsString::value_type pfx = strVal[0];

        // handle special prefix for string
        if( esT('\'') != pfx )
          strVal = strVal.substr(1);

        strVal = EsString::fromString(
          strVal,
          EsString::StrQuote,
          script.loc()
        );

        if( esT('B') == pfx )
          return EsVariant(
            static_cast<esU8>(strVal[0])
          );
        else
          return EsVariant(
            strVal[0],
            EsVariant::ACCEPT_CHAR
          );
      }
    case strValID:
      return compileStrValHelper(
        cit,
        script,
        strVal
      );
    case enumValID:
      return compileEnumVal(
        cit,
        script
      );
    case enumValLblID:
      return compileEnumValLbl(
        cit,
        script
      );
    default:
      EsScriptException::ThrowUnhandledAstNodeId(
        id,
        debugInfoCreateAt(cit)
      );
      break;
    }

    // pacify compiler
    return EsVariant::null();
  }
  //---------------------------------------------------------------------------

  // compile array value
  EsVariant compileArrayValue(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT( arrayValID == id );

    EsVariant::Array result;
    result.reserve( (*cit).children.size() );

    for(ParseTreeConstIteratorT child = (*cit).children.begin(); child != (*cit).children.end(); ++child)
    {
      result.push_back(
        compileConstExpr(
          child,
          script
        )
      );
    }

    return result;
  }
  //---------------------------------------------------------------------------

  // Compile range value
  EsVariant compileRangeValue(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT((*cit).children.size() > 0);

    ParseTreeConstIteratorT child = (*cit).children.begin();
    switch( id )
    {
    case rangeLeftValID:
      return EsRange::createMinIncl(
        compileConstExpr(
          child,
          script
        )
      );
    case rangeLeftRightValID:
      {
        // Perform separate left-right expr compilation here -
        // to maintain proper compilation order
        //
        ES_ASSERT(2 == (*cit).children.size());
        const EsVariant& min = compileConstExpr(
          child++,
          script
        );
        const EsVariant& max = compileConstExpr(
          child,
          script
        );

        try
        {
          return EsRange::createMinInclMaxIncl(
            min,
            max
          );
        }
        catch(const EsException& ex)
        {
          EsScriptException::ReThrow(
            ex,
            debugInfoCreateAt(cit)
          );
        }
      }
    case rangeRightValID:
      return EsRange::createMaxIncl(
        compileConstExpr(
          child,
          script
        )
      );
    default:
      EsScriptException::ThrowUnhandledAstNodeId(
        id,
        debugInfoCreateAt(cit)
      );
      break;
    }

    return EsVariant::null();
  }
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------

  // Const expr checking
  //
  bool isConstExpr(ParseTreeConstIteratorT cit, EsScriptMachine& script, bool isLrhsSubj = false)
  {
    long id = static_cast<long>((*cit).value.id().to_long());

    // Literal values
    if( simpleValID < id && id < arrayValID )
      return true;

    // Check if array expression is constexpr
    if( arrayValID == id )
    {
      for(ParseTreeConstIteratorT child = (*cit).children.begin(); child != (*cit).children.end(); ++child)
        if(
          !isConstExpr(
            child,
            script
          )
        )
          return false;

      return true;
    }

    // Stand-alone identifier - only const is allowed, otherwise,
    // enums and classes are OK
    //
    if( identID == id )
    {
      const EsString& name = compileIdent(
        cit,
        false
      );

      if( isLrhsSubj )
        return script.consts().symbolExists(name) ||
          script.enumFind(name) ||
          script.metaGet()->hasClass(name);
      else
        return script.consts().symbolExists(name);
    }

    // Compound expressions
    //

    // Ternary operator - all parts are constexprs
    if( opConditionalID == id )
    {
      ParseTreeConstIteratorT child = (*cit).children.begin();
      return isConstExpr(
        child++,
        script
      ) &&
      isConstExpr(
        child++,
        script
      ) &&
      isConstExpr(
        child,
        script
      );
    }

    // Binary operators - all parts are constexprs
    if( isBinOp(id) )
    {
      ParseTreeConstIteratorT child = (*cit).children.begin();
      return isConstExpr(
        child++,
        script
      ) &&
      isConstExpr(
        child,
        script
      );
    }

    // Unary operators (exclude pre-and-postfix incdec, these have side-effects)
    if( opUnaryID == id )
      return isConstExpr(
        (*cit).children.begin(),
        script
      );

    // RHS - ext. function calls with constexpr parameters,
    // variant service calls on constexpr, with constexpr parameters,
    // index access on constexpr with constexpr index
    // attribute access on class
    //
    if( isKindOfCompoundRhs(id) )
      return isConstExpr(
        (*cit).children.begin(),
        script,
        lrhsID == id
      );

    // The rest defaults to non-constexpr
    return false;
  }
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------

  // Constexpr evaluators
  //
  EsVariant compileConstExprBinaryOp(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());

    ES_ASSERT( isBinOp(id) );
    ES_ASSERT((*cit).children.size() == 2);

    ParseTreeConstIteratorT child = (*cit).children.begin();

    // Evaluate left part
    const EsVariant& _1 = compileConstExpr(
      child++,
      script
    );

    const EsVariant& _2 = compileConstExpr(
      child,
      script
    );

    // Evaluate binary operation
    EsScriptOperatorIds opid = EsScriptOpIds::instanceGet().getId(
      EsString(
        (*cit).value.begin(),
        (*cit).value.end()
      )
    );

    EsVariant result;
    EsScriptThreadContext::binOpEval(
      opid,
      _1,
      _2,
      result
    );

    return result;
  }
  //---------------------------------------------------------------------------

  EsVariant compileConstExprUnaryOp(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    if( opUnaryID == id )
    {
      EsScriptOperatorIds opid = EsScriptOpIds::instanceGet().getId(
        EsString(
          (*cit).value.begin(),
          (*cit).value.end()
        )
      );
      ES_ASSERT(opid == opidBwNot || opid == opidNot || opid == opidSub);
      ES_ASSERT((*cit).children.size()==1);

      // Compile operand expression
      EsVariant var = compileConstExpr(
        (*cit).children.begin(),
        script
      );

      EsScriptThreadContext::unaryOpEval(
        opid,
        var
      );

      return var;
    }
    else if( isIncDec(id) )
      EsScriptException::ThrowConstexprExpected(
        debugInfoCreateAt(
          cit
        )
      );

    // Pacify compiler
    return EsVariant::null();
  }
  //---------------------------------------------------------------------------

  EsVariant compileConstExprTernary(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    ES_ASSERT(opConditionalID == (*cit).value.id().to_long());
    ES_ASSERT(3 == (*cit).children.size());

    ParseTreeConstIteratorT child = (*cit).children.begin();

    // Evaluate test expression
    const EsVariant& test = compileConstExpr(
      child++,
      script
    );

    if( test.asBool() )  //< Eval 'true' const expression
      return compileConstExpr(
        child,
        script
      );
    else //< Eval 'false' const expression
      return compileConstExpr(
        ++child,
        script
      );
  }
  //---------------------------------------------------------------------------

  EsVariant compileConstExprCall(ParseTreeConstIteratorT cit, EsScriptMachine& script, const EsString& nameSpace)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    ES_ASSERT((*cit).children.size() > 0);

    if( exprStaticOrBaseCallID == id )
    {
      ParseTreeConstIteratorT child = (*cit).children.begin();
      const EsString& nameSpace = compileIdent(child++);

      return compileConstExprCall(
        child,
        script,
        nameSpace
      );
    }
    else if( exprFuncCallID == id )
    {
      ParseTreeConstIteratorT child = (*cit).children.begin();
      const EsString& name = compileIdent(child++);

      EsVariant result;
      EsVariant::Array params;

      while( child != (*cit).children.end() )
        params.push_back(
          compileConstExpr(
            child++,
            script
          )
        );

      if( !nameSpace.empty() )
      {
        EsScriptThreadContext::callExtMethod(
          result,
          name,
          params,
          nameSpace,
          debugInfoCreateAt(cit)
        );

        return result;
      }
    }

    // Report generic compile time constexpr error
    EsScriptException::ThrowConstexprExpected(
      debugInfoCreateAt(cit)
    );

    return EsVariant::null();
  }
  //---------------------------------------------------------------------------

  EsVariant compileConstExprVsvcCall( const EsVariant& lhs, ParseTreeConstIteratorT cit, EsScriptMachine& script )
  {
    ES_ASSERT( varSvcCallID == (*cit).value.id().to_long() );

    ParseTreeConstIteratorT child = (*cit).children.begin();
    ES_ASSERT( exprFuncCallID == (*child).value.id().to_long() );

    cit = child;
    child = (*cit).children.begin();
    ES_ASSERT( identID == (*child).value.id().to_long() );

    EsScriptDebugInfoIntf::Ptr dbg = debugInfoCreateAt(child);
    const EsString& name = compileIdent(child++);

    EsVariant::Array args;
    args.push_back(lhs);

    while(child != (*cit).children.end())
      args.push_back(
        compileConstExpr(
          child++,
          script
        )
      );

    EsVariant result;
    EsScriptThreadContext::variantServiceCall(
      result,
      name,
      args,
      dbg
    );

    return result;
  }
  //---------------------------------------------------------------------------

  EsVariant compileConstExprRhs(ParseTreeConstIteratorT cit, EsScriptMachine& script)
  {
    long id = static_cast<long>((*cit).value.id().to_long());

    if( isFunctionOrMemberCall(id) )
      return compileConstExprCall(
        cit,
        script,
        EsString::null()
      );

    ES_ASSERT( lrhsID == id );

    cit = (*cit).children.begin();
    const EsVariant& lhs = compileConstExpr(
      cit++,
      script,
      true
    );

    id = static_cast<long>((*cit).value.id().to_long());

    if( varSvcCallID == id )
      return compileConstExprVsvcCall(
        lhs,
        cit,
        script
      );
    else if( idxAccID == id )
      return lhs[
        compileConstExpr(
          (*cit).children.begin(),
          script
        )
      ];
    else if( attrAccID == id )
    {
      const EsString& attrName = compileIdent((*cit).children.begin());

      if( lhs.isVariantCollection() && 2 == lhs.countGet() )
      {
        EsMetaclassIntf::Ptr meta = lhs[0].asExistingObject();
        if( meta )
          return meta->attributeGet(
            attrName,
            lhs[1].asString()
          );
      }

      if( lhs.isObject() )
      {
        // Otherwise, use reflected class interface to access (enumeration) attributes
        EsReflectedClassIntf::Ptr renu = lhs.asExistingObject();

        if( renu )
          return renu->attributeGet(attrName);
      }
    }

    // Report generic compile time constexpr error
    EsScriptException::ThrowConstexprExpected(
      debugInfoCreateAt(cit)
    );

    return EsVariant::null();
  }
  //---------------------------------------------------------------------------

  EsVariant compileConstExprIdentifier(ParseTreeConstIteratorT cit, EsScriptMachine& script, bool lhsConstExpr)
  {
    const EsString& ident = compileIdent( cit );

    if( script.consts().symbolExists(ident) )
      return script.constantGet(ident);

    EsEnumerationIntf::Ptr enu = script.enumFind(ident);
    if( enu )
      return enu;

    if( !lhsConstExpr ) //< If not compiling as a lhs part of compileConstExprRhs - allow only const and enum names here
      EsScriptException::Throw(
        esT("Existing enumeration, or constant is expected"),
        debugInfoCreateAt(cit)
      );

    // Otherwise - check if it's metaclass, enum instances, or const name
    EsMetaclassIntf::Ptr meta = script.metaGet();
    if( meta->hasClass(ident) )
    {
      EsVariant::Array ret;
      ret.push_back(meta);
      ret.push_back(ident);

      return ret;
    }

    EsScriptException::Throw(
      esT("Existing class name, enumeration, or constant is expected"),
      debugInfoCreateAt(cit)
    );

    // Pacify compiler
    return EsVariant::null();
  }
  //---------------------------------------------------------------------------

  // Constexpr compile-time evaluation entry point
  // Compile constexpression, with optional compile-time execution and evaluation.
  // lhsConstExpr is set to true if it's called to compile a left part from within compileConstExprRhs
  // This flags changes the way identID compiler behave
  //
  EsVariant compileConstExpr(ParseTreeConstIteratorT cit, EsScriptMachine& script, bool lhsConstExpr = false)
  {
    long id = static_cast<long>((*cit).value.id().to_long());
    if( isSimpleVal(id) )
      return compileSimpleValue(
        cit,
        script
      );
    else if( isRangeVal(id) )
      return compileRangeValue(
        cit,
        script
      );
    else if( arrayValID == id )
      return compileArrayValue(
        cit,
        script
      );
    else if( identID == id )            //< This identifier may be class name, enum name, or const name
      return compileConstExprIdentifier(
        cit,
        script,
        lhsConstExpr
      );
    else if( isBinOp(id) )              //< binary operation expressions
      return compileConstExprBinaryOp(
        cit,
        script
      );
    else if( isUnaryOp(id) )            //< unary operation expressions
      return compileConstExprUnaryOp(
        cit,
        script
      );
    else if( opConditionalID == id )    //< ternary operator
      return compileConstExprTernary(
        cit,
        script
      );
    else if( isKindOfCompoundRhs(id) )  //< compound RHS expression(s)
      return compileConstExprRhs(
        cit,
        script
      );

    // Report generic compile time constexpr error
    EsScriptException::ThrowConstexprExpected(
      debugInfoCreateAt(cit)
    );

    return EsVariant::null();
  }
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------

protected:
  // Compilation scopes
  CodeSectionScope* m_codeScope;
  ObjectScope* m_objectScope;
  BreakContinueGuard* m_breakGuard;
  BreakContinueGuard* m_continueGuard;
  LabelGuard* m_labelGuard;
  SwitchScope* m_switchScope;

PARSER_CONTEXT_IMPL_END
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#else

static void EsScriptThrowNoCompilerFunctionality()
{
  EsException::Throw(esT("EsScript source compiler functionality is disabled"));
}
//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#if (0 == ES_USE_COMPILER_IMPLEMENTATION)
// Dummy source type definition for the case we're not using compiler functionality
enum SourceInputType {
  fromString,
  fromSourceFile
};
#endif

static bool doCompile(
  const EsString& src,
  EsScriptMachine& machine,
  const EsAssocContainerIntf::Ptr& modulesInfo,
  SourceInputType inputType,
  const EsBreakIntf::Ptr& brk
)
{
#if (1 == ES_USE_COMPILER_IMPLEMENTATION)
  // reset code && file refs
  machine.reset();

  EsScriptCompiler compiler(
    src,
    modulesInfo,
    inputType,
    brk
  );

  if( compiler.doParse() )
  {
#ifndef ES_EXCLUDE_ESS_CODEGEN
    compiler.compileTo(machine);
#endif
    return true;
  }
  else
  {
    // throw parse stop info
    return compiler.errorHandle(
      compiler.m_parseInfo.stop,
      EsScriptCompiler::Error_Parser
    );
  }

#else
  EsScriptThrowNoCompilerFunctionality();
#endif

  return false;
}
//---------------------------------------------------------------------------

bool EsScriptUtilities::compileFromString(
  const EsString& src,
  EsScriptMachine& machine,
  const EsAssocContainerIntf::Ptr& modulesInfo,
  const EsBreakIntf::Ptr& brk
)
{
  return doCompile(
    src,
    machine,
    modulesInfo,
    fromString,
    brk
  );
}
//---------------------------------------------------------------------------

bool EsScriptUtilities::compileFromFile(
  const EsString& src,
  EsScriptMachine& machine,
  const EsAssocContainerIntf::Ptr& modulesInfo,
  const EsBreakIntf::Ptr& brk
)
{
  return doCompile(
    src,
    machine,
    modulesInfo,
    fromSourceFile,
    brk
  );
}
//---------------------------------------------------------------------------

bool EsScriptUtilities::compileFromScriptlet(EsScriptlet& src, const EsBreakIntf::Ptr& brk)
{
#if (1 == ES_USE_COMPILER_IMPLEMENTATION)
  EsAssocContainerIntf::Ptr dummyFiles;

  EsScriptCompiler compiler(
    EsString::fromEscapedString(
      src.sourceGet(),
      src.machineGet().loc()
    ),
    dummyFiles,
    fromString,
    brk
  );

  try
  {
    if( compiler.doParse() )
    {
      compiler.compileToScriptlet(
        src.machineGet(),
        src.infoGet(),
        src.parameterNamesGet()
      );

      return true;
    }
    else
    {
      return compiler.errorHandle(
        compiler.m_parseInfo.stop,
        EsScriptCompiler::Error_Parser
      );
    }
  }
  catch(...)
  {
    EsUtilities::logError( _("Unknown Scriptlet parser error") );
  }
#else
  EsScriptThrowNoCompilerFunctionality();
#endif

  return false;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

template <typename ChildrenT>
void EsScriptParser::Node::childrenNodesAdd(
  EsString::const_pointer inStart,
  const EsScriptParser::Node::PtrT& node,
  const ChildrenT& astChildren
)
{
  ES_ASSERT(node);
  for(auto const& astChild : astChildren)
  {
    EsScriptParser::Node::PtrT childNode = node->childAdd(
      static_cast<long>(astChild.value.id().to_long()),
      static_cast<ulong>(astChild.value.begin()-inStart),
      static_cast<ulong>(astChild.value.end()-inStart),
      static_cast<ulong>(astChild.children.size())
    );

    if(!astChild.children.empty())
      childrenNodesAdd(
        inStart,
        childNode,
        astChild.children
      );
  }
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

template <typename AstNodeT>
void EsScriptParser::rootInit(
  EsString::const_pointer inStart,
  const AstNodeT& astRoot
)
{
  Node::PtrT rootNode(
    new Node(
      *this,
      static_cast<long>(astRoot.value.id().to_long()),
      static_cast<ulong>(astRoot.value.begin()-inStart),
      static_cast<ulong>(astRoot.value.end()-inStart),
      nullptr,
      static_cast<ulong>(astRoot.children.size())
    )
  );
  ES_ASSERT(rootNode);

  Node::childrenNodesAdd(
    inStart,
    rootNode,
    astRoot.children
  );

  m_roots.push_back(
    rootNode
  );
}
//--------------------------------------------------------------------------------

template <typename AstTreesT>
void EsScriptParser::rootsInit(
  EsString::const_pointer inStart,
  AstTreesT& trees
)
{
  m_roots.reserve(
    m_roots.size() +
    trees.size()
  );

  for(auto const& branch : trees)
  {
    if(EsScriptParser::stmtListId == branch.value.id().to_long())
    {
      m_roots.reserve(
        m_roots.size() +
        branch.children.size()
      );

      for(auto const& stmt : branch.children)
        rootInit(
          inStart,
          stmt
        );
    }
    else
    {
      rootInit(
        inStart,
        branch
      );
    }
  }
}
//--------------------------------------------------------------------------------

bool EsScriptParser::parse(const EsString& in)
{
  if(m_isParsed && m_hashPrev == in.hashGet()) //< Do not re-parse what was already parsed
    return m_isParsed;

  if(in.empty())
  {
    reset();
    m_isParsed = true;

    return m_isParsed;
  }

  EsBreakIntf::Ptr brk; //< == TODO

#if (1 == ES_USE_COMPILER_IMPLEMENTATION)
  EsAssocContainerIntf::Ptr dummyFiles;

  EsScriptCompiler compiler(
    in,
    dummyFiles,
    fromString,
    brk,
    false //< Do not log errors
  );

  try
  {
    EsString::const_pointer inStart = compiler.m_input.data();
    bool fullyParsed = compiler.doParse();
    bool needPartialParse = false;

    ulong falseStop = 0;
    m_stop = static_cast<ulong>(compiler.m_parseInfo.stop - inStart);
    if(!fullyParsed)
    {
      falseStop = m_stop;

      fullyParsed = compiler.doParse(
        0,
        m_stop
      );

      needPartialParse = fullyParsed;
    }

    m_roots.clear();

    m_isParsed = fullyParsed ||
      !compiler.m_parseInfo.trees.empty();

    rootsInit(
      inStart,
      compiler.m_parseInfo.trees
    );

    if(
      needPartialParse &&
      compiler.doParse_expr(
        falseStop
      )
    )
      rootsInit(
        inStart,
        compiler.m_parseInfo.trees
      );
  }
  catch(...)
  {
    m_hashPrev = 0;
    m_isParsed = false;
  }
#endif

  if(m_isParsed)
    m_hashPrev = in.hashGet();

  return m_isParsed;
}
//--------------------------------------------------------------------------------
