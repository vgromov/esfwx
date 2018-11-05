// EsScriptCompilerRuleNames.def file
// rule names must go here.
// NB! do not change order of these entries in arbitrary order, or 'by name', or etc.
// Order is important - these entries are used (along other ways) for parser ID enumeration
// generation at compile time. ID ranges (as roughtly shown by commented groups)
// are used for fast AST node classification at ESS compile time.
//

GRAMMAR_RULE(value)       // 2
GRAMMAR_RULE(simpleVal)

// Literal values
GRAMMAR_RULE(nullVal)
GRAMMAR_RULE(boolVal)
GRAMMAR_RULE(charVal)
GRAMMAR_RULE(strVal)
GRAMMAR_RULE(numericVal)
GRAMMAR_RULE(intBinVal)
GRAMMAR_RULE(intOctVal)
GRAMMAR_RULE(intHexVal)
GRAMMAR_RULE(intVal)
GRAMMAR_RULE(floatVal)
GRAMMAR_RULE(enumVal)
GRAMMAR_RULE(enumValLbl)

// Compound values|temporaries
GRAMMAR_RULE(rangeVal)    // 16
GRAMMAR_RULE(rangeLeftVal)
GRAMMAR_RULE(rangeLeftRightVal)
GRAMMAR_RULE(rangeRightVal)
GRAMMAR_RULE(arrayVal)

// Identifier
GRAMMAR_RULE(ident)

// lhs|rhs
GRAMMAR_RULE(idxAcc)
GRAMMAR_RULE(fieldOrMemberVarAcc)
GRAMMAR_RULE(propAcc)
GRAMMAR_RULE(attrAcc)
GRAMMAR_RULE(objMethodCall)
GRAMMAR_RULE(varSvcCall)
GRAMMAR_RULE(lrhs)

// Operators
GRAMMAR_RULE(opAssign)    // 29
GRAMMAR_RULE(opConditional)
GRAMMAR_RULE(opLogical)
GRAMMAR_RULE(opBitwise)
GRAMMAR_RULE(opEqNeq)
GRAMMAR_RULE(opCmp)
GRAMMAR_RULE(opSh)
GRAMMAR_RULE(opAddSub)
GRAMMAR_RULE(opMulDiv)
GRAMMAR_RULE(opUnary)
GRAMMAR_RULE(opIncDec)

// Expressions
GRAMMAR_RULE(expr)        // 40
GRAMMAR_RULE(exprLogical)
GRAMMAR_RULE(exprBitwise)
GRAMMAR_RULE(exprEqNeq)
GRAMMAR_RULE(exprCmp)
GRAMMAR_RULE(exprSh)
GRAMMAR_RULE(exprAddSub)
GRAMMAR_RULE(exprMulDiv)
GRAMMAR_RULE(exprUnary)
GRAMMAR_RULE(exprIncDec)
GRAMMAR_RULE(exprIncDecPfx) // 50
GRAMMAR_RULE(exprWildcard)
GRAMMAR_RULE(exprFuncCall)
GRAMMAR_RULE(exprObjCtrCall)
GRAMMAR_RULE(exprStaticOrBaseCall)
GRAMMAR_RULE(exprCall)
GRAMMAR_RULE(exprAtom)
GRAMMAR_RULE(exprAssign)
GRAMMAR_RULE(exprInitAsn)
GRAMMAR_RULE(exprForInit)
GRAMMAR_RULE(exprForCheck)
GRAMMAR_RULE(exprForCnt)
GRAMMAR_RULE(exprForEachCheck)
GRAMMAR_RULE(exprWhileCheck)
GRAMMAR_RULE(exprCase)
GRAMMAR_RULE(exprObjPropertyReaderDecl)
GRAMMAR_RULE(exprObjPropertyWriterDecl)

// Statements
GRAMMAR_RULE(script)   // 67
GRAMMAR_RULE(stmt)
GRAMMAR_RULE(stmtTryCatch)
GRAMMAR_RULE(stmtThrow)
GRAMMAR_RULE(stmtRethrow)
GRAMMAR_RULE(stmtRequire)
GRAMMAR_RULE(stmtLink)
GRAMMAR_RULE(stmtExternDecl)
GRAMMAR_RULE(stmtAssign)
GRAMMAR_RULE(stmtIncDec)
GRAMMAR_RULE(stmtConstDecl)
GRAMMAR_RULE(stmtVarDecl)
GRAMMAR_RULE(stmtEnumItemDecl)
GRAMMAR_RULE(stmtEnumDecl)
GRAMMAR_RULE(stmtReturn)

// Function decls
GRAMMAR_RULE(stmtFuncDecl)   // 82
GRAMMAR_RULE(stmtObjMethodDecl)
GRAMMAR_RULE(stmtObjCtorDecl)
GRAMMAR_RULE(stmtObjDtorDecl)

// Attributes
GRAMMAR_RULE(stmtAttributeList)
GRAMMAR_RULE(stmtAttributeDecl)

// Object field decls
GRAMMAR_RULE(stmtObjFldDecl)
GRAMMAR_RULE(stmtObjPlainFldDecl)
GRAMMAR_RULE(stmtObjArrayFldDecl)
GRAMMAR_RULE(stmtObjFldIfDecl)
GRAMMAR_RULE(stmtObjFldElse)

GRAMMAR_RULE(stmtObjMemberVarDecl) // 93
GRAMMAR_RULE(stmtObjPropertyDecl)
GRAMMAR_RULE(stmtObjDecl)

GRAMMAR_RULE(stmtCall)
GRAMMAR_RULE(stmtLrhs)
GRAMMAR_RULE(stmtElse)
GRAMMAR_RULE(stmtIf)
GRAMMAR_RULE(stmtSwitch)
GRAMMAR_RULE(stmtDefault)
GRAMMAR_RULE(stmtCase)
GRAMMAR_RULE(stmtFor)
GRAMMAR_RULE(stmtForEach)
GRAMMAR_RULE(stmtWhile)
GRAMMAR_RULE(stmtDoWhile)
GRAMMAR_RULE(stmtBreak)
GRAMMAR_RULE(stmtContinue)
GRAMMAR_RULE(stmtLabel)
GRAMMAR_RULE(stmtGoto)
GRAMMAR_RULE(stmtNoop)

// Block statements (many statements enclosed in block)
GRAMMAR_RULE(stmtList)    // 112
GRAMMAR_RULE(stmtBlock)
GRAMMAR_RULE(stmtObjDeclBlock)
GRAMMAR_RULE(stmtExprCaseBlock)
GRAMMAR_RULE(stmtTry)
GRAMMAR_RULE(stmtCatch)

#undef GRAMMAR_RULE