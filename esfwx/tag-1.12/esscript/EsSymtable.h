#ifndef _es_sym_table_h_
#define _es_sym_table_h_

// abstract symbol container interface
class EsSymTable
{
public:	
	EsSymTable(bool checkTemplate = true);
	EsSymTable(const EsSymTable& src, const std::locale& loc);

	bool isTemplate() const { return m_template; }
	void reset();
	void addSymbol(const EsString& name);
	void addSymbol(const EsString& name, EsAccessorIntf::Ptr acc);
	bool findSymbol(const EsString& name) const;
	EsAccessorIntf::Ptr getSymbol(const EsString& name, bool doThrow);
	EsAccessorIntf::Ptr getSymbol(const EsString& name, bool doThrow) const
	{ return const_cast<EsSymTable*>(this)->getSymbol(name, doThrow); }
	void setSymbolVal(const EsString& name, const EsVariant& val);

private:
	// internal services
	//
	void checkTemplateOperation() const;
	void checkNonTemplateOperation() const;
	// prohibited functionality
	//
	EsSymTable(const EsSymTable& src);
	const EsSymTable& operator =(const EsSymTable&);
	
protected:
	// check template|non template mode
	bool m_checkTemplate;
	// template flag
	bool m_template;
	// symbols container
	typedef containers::structured_map<EsString, EsAccessorIntf::Ptr> Map;
	Map m_syms;
	// special empty value
	static const EsAccessorIntf::Ptr s_empty;
};

#endif // _es_sym_table_h_
