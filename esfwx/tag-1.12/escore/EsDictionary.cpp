#include "stdafx.h"
#pragma hdrstop

#include "EsDictionary.h"
#include "EsDictionaryCompiler.hxx"

#define ERR_VALUE_WITH_NAME_S0_DOES_NOT_EXIST		_("Value with name '%s' does not exist.")
#define ERR_VALUE_WITH_NAME_S0_ALREADY_EXISTS		_("Value with name '%s' already exists.")

EsDictionary::EsDictionary(const EsString& name, const EsString& str /*= EsString::s_null*/) :
	m_name(name)
{
	setAsString(str);
}

// read from|get as configuration string
void EsDictionary::setAsString(const EsString& config, bool append /*= false*/)
{
	// use classic locale when importing dictionary from string
	const std::locale& loc = std::locale::classic();
	// reset existing compiled data
	if( !append )
		m_contents.clear();
	
	if( !config.empty() )
	{
		EsDictionaryCompiler compiler(config);
		if( compiler.doParse() )
			compiler.compileTo( *this, loc );
	}
}

// internal helper for exporting array
EsString EsDictionary::exportArray(const EsVariant& val, const std::locale& loc)
{
	wxASSERT(val.getType() == EsVariant::VAR_VARIANT_COLLECTION);
	EsString result;

	for(size_t idx = 0; idx < static_cast<size_t>(val.getCount()); ++idx)
	{
		if( !result.empty() )
			result += wxT(", ");

		EsVariant var = val.getItem(idx);
		switch(var.getType())
		{ 
		case EsVariant::VAR_VARIANT_COLLECTION:
			// export array
			result += exportArray(var, loc);
			break;
		case EsVariant::VAR_STRING:
			result += var.asString(EsString::StrQuote, loc);
			break;	
		default:
			result += var.asString(loc);
			break;
		}
	}

	return wxT('{') + result + wxT('}');
}

// get dictionary contents as string
EsString EsDictionary::getAsString() const
{
	// use classic locale when exporting dictionary to string
	const std::locale& loc = std::locale::classic();
	EsString result;
	EsString::Array allNames = getAllNames();
	if( !m_name.empty() )
		result = wxT('[') + m_name + wxT("];");
	for(size_t idx = 0; idx < allNames.size(); ++idx)
	{
		const EsVariant& val = getValue( allNames[idx] );
		result += allNames[idx] + wxT('=');
		switch(val.getType())
		{ 
		case EsVariant::VAR_VARIANT_COLLECTION:
			// export array
			result += exportArray(val, loc);
			break;
		case EsVariant::VAR_STRING:
			result += val.asString(EsString::StrQuote, loc);
			break;	
		default:
			result += val.asString(loc);
			break;
		}
		
		result += wxT(';');
	}

	return result;
}

// sections manipulation interface
EsString::Array EsDictionary::getAllNames() const
{
	EsString::Array result;
	result.reserve(32);
	for(EsDictionary::Contents::const_iterator cit = m_contents.begin(); cit != m_contents.end(); ++cit )
		result.push_back(cit->first);

	return result;
}

const EsVariant& EsDictionary::getValue(const EsString& name) const
{
	EsDictionary::Contents::const_iterator cit = m_contents.find(name);
	if( cit != m_contents.end() )
		return cit->second;
	else
		EsException::Throw(0, EsException::Generic, ERR_VALUE_WITH_NAME_S0_DOES_NOT_EXIST, name.c_str());
	
	return EsVariant::s_null;
}

const EsVariant& EsDictionary::getValueDef(const EsString& name, const EsVariant& def) const
{
	EsDictionary::Contents::const_iterator cit = m_contents.find(name);
	if( cit != m_contents.end() )
		return cit->second;
	else
		return def;
}

void EsDictionary::setValue(const EsString& name, const EsVariant& val, bool create /*= false*/)
{
	EsDictionary::Contents::iterator it = m_contents.find(name);
	if( create || it != m_contents.end() )
		m_contents[name] = val;
	else
		EsException::Throw(0, EsException::Generic, ERR_VALUE_WITH_NAME_S0_DOES_NOT_EXIST, name.c_str() );
}

bool EsDictionary::valueExists(const EsString& name) const
{
	return m_contents.find(name) != m_contents.end();
}

void EsDictionary::deleteValue(const EsString& name)
{
	EsDictionary::Contents::iterator it = m_contents.find(name);
	if( it != m_contents.end() )
		m_contents.erase(it);
	else
		EsException::Throw(0, EsException::Generic, ERR_VALUE_WITH_NAME_S0_DOES_NOT_EXIST, name.c_str() );	
}

void EsDictionary::renameValue(const EsString& oldName, const EsString& newName)
{
	EsDictionary::Contents::iterator it = m_contents.find(newName);
	if( it == m_contents.end() )
	{
		if( !newName.empty() )
		{
			it = m_contents.find(oldName);
			if( it != m_contents.end() )
			{
				EsVariant val = it->second;
				m_contents.erase(it);
				m_contents[newName] = val;
			}
			else
				EsException::Throw(0, EsException::Generic, ERR_VALUE_WITH_NAME_S0_DOES_NOT_EXIST, oldName.c_str());
		}
		else
			EsException::Throw(0, EsException::Generic, _("Value name may not be the empty string."));
	}
	else
		EsException::Throw(0, EsException::Generic, ERR_VALUE_WITH_NAME_S0_ALREADY_EXISTS, newName.c_str() );	
}
