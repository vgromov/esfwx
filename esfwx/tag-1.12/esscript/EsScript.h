#ifndef _es_script_h_
#define _es_script_h_

// ekosfera script class
class EKOSF_SCRIPT_CLASS EsScript
{
public:
	EsScript(const EsString& script);
	// check compiled status
	bool isCompiled() const
	{
		return m_compiled;
	}
	// run script compilation
	bool compile(bool force = false);
	// access compiled code
	EsScriptMachine& code()
	{
		if( !m_compiled )
			EsException::Throw(0, EsException::Generic, _("Script code is not compiled yet"));
		return m_code;
	}
	
	// identifier checking services
	//
	// make sure identifier is not a reserved word
	static void checkNotReservedWord(const EsString& ident);	
	// make sure identifier is not a registered reflected class name
	static void checkNotRegisteredObject(const EsString& ident);
	// debug services
	//
	static const EsString& getBinOpStr(long id);

protected:
	// data members
	//
	EsString m_text; 
	bool m_compiled;
	// script code - virtual machine executor
	EsScriptMachine m_code;
	// script reserved words
	static const EsString::const_pointer s_reserved[];	
};

#endif // _es_script_h_
