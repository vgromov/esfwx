#ifndef _Es_Dictionary_h_
#define _Es_Dictionary_h_

// dictionary class - flexible named values storage. it may be 
// filled from|saved to the string representation, where name=value pairs are separated with newlines
//
class EKOSF_CORE_CLASS EsDictionary
{
public:
	typedef containers::structured_map<EsString, EsVariant> Contents;

public:
	// named dictionary creation
	EsDictionary(const EsString& name = EsString::s_null, const EsString& str = EsString::s_null);
	// set|get dictionary name
	const EsString& getName() const { return m_name; }
	void setName(const EsString& name) { m_name = name; }
	// read from|get as configuration string
	void setAsString(const EsString& config, bool append = false);
	EsString getAsString() const;
	// dictionary contents
	void clearAll() { m_contents.clear(); }
	EsString::Array getAllNames() const;
	const EsVariant& getValue(const EsString& name) const;
	const EsVariant& getValueDef(const EsString& name, const EsVariant& def) const;
	void setValue(const EsString& name, const EsVariant& val, bool create = false);
	bool valueExists(const EsString& name) const;
	void deleteValue(const EsString& name);
	void renameValue(const EsString& oldName, const EsString& newName);
	// check for empty contents
	bool isEmpty() const { return m_contents.empty(); }

protected:
	// internal helper for exporting array
	static EsString exportArray(const EsVariant& val, const std::locale& loc);

protected:
	EsString m_name;
#pragma warning(push)
#pragma warning(disable: 4251)
	Contents m_contents;
#pragma warning(pop)
};

#endif // _Es_Dictionary_h_