#include "stdafx.h"
#pragma hdrstop

#include "EsScript.h"
#include "EsScriptCompiler.hxx"

// reserved words
const EsString::const_pointer EsScript::s_reserved[] = {
	wxT("var"),
	wxT("const"),
	wxT("function"),
	wxT("return"),
	wxT("new"),
	wxT("object"),
	wxT("extends"),
	wxT("true"),
	wxT("false"),
	wxT("if"),
	wxT("else"),
	wxT("for"),
	wxT("while"),	
	wxT("do"),	
	wxT("break"),
	wxT("continue"),
	wxT("switch"),
	wxT("case"),
	wxT("default"),
	wxT("label"),
	wxT("goto"),
};

EsScript::EsScript(const EsString& script) :
	m_compiled(false)
,	m_text(script)
{
}

bool EsScript::compile( bool force /*= false*/)
{
	if( !m_compiled || force )
	{
		m_compiled = false; // just in case we force recompilation
		// reset code
		m_code.reset();
		
		EsScriptCompiler compiler(m_text);
		if( compiler.doParse() )
		{
			compiler.compileTo(m_code);
			m_compiled = true;
		}
		else
		{
			// throw parse stop info
			compiler.setError( compiler.m_parseInfo.stop, EsScriptCompiler::Error_Parser );
		}
	}
		
	return m_compiled;
}

// make sure identifier is not a reserved word
void EsScript::checkNotReservedWord(const EsString& ident)
{
	if( !ident.empty() )
	{
		for( size_t idx = 0; idx < ES_CONST_ARRAY_COUNT(s_reserved); ++idx)
			if( EsString::cmpEqual == ident.compare(s_reserved[idx]) )
				EsException::Throw(0, EsException::Generic, wxT("'%s' is a reserved word"), ident.c_str());
	}
}

// make sure identifier is not a registered reflected class name
void EsScript::checkNotRegisteredObject(const EsString& ident)
{
	if( 0 != ClassInfo::getClassInfo(ident) )
		EsException::Throw(0, EsException::Generic, wxT("'%s' is already defined as object type"), ident.c_str());
}

const EsString& EsScript::getBinOpStr(long id)
{
	return EsScriptCompiler::s_opids.getIdStr(id);
}