#include <stdafx.h>
#pragma hdrstop

#include <algorithm>
#include "EsVariant.h"

// Reflection-specific standard service names
//
const EsString::pointer EsVariant::s_asString  = wxT("asString");
const EsString::pointer EsVariant::s_compare   = wxT("compare");
const EsString::pointer EsVariant::s_add       = wxT("add");
const EsString::pointer EsVariant::s_subtract  = wxT("subtract");
const EsString::pointer EsVariant::s_multiply  = wxT("multiply");
const EsString::pointer EsVariant::s_divide    = wxT("divide");
const EsString::pointer EsVariant::s_getCount  = wxT("getCount");
const EsString::pointer EsVariant::s_getItem   = wxT("getItem");
const EsString::pointer EsVariant::s_setItem   = wxT("setItem");
const EsString::pointer EsVariant::s_setToNull = wxT("setToNull");
const EsString::pointer EsVariant::s_value     = wxT("value");

#ifndef UNICODE 
const int s_WXCHAR_MIN = SCHAR_MIN; // cover both signed and unsigned char, -127 ..
const int s_WXCHAR_MAX = UCHAR_MAX; // cover both signed and unsigned char, .. 255
#else
const int s_WXCHAR_MIN = SHRT_MIN; // cover both signed and unsigned
const int s_WXCHAR_MAX = USHRT_MAX;
#endif

const EsVariant EsVariant::s_null;

void EsVariant::releaseObject()
{
	wxASSERT(VAR_OBJECT == m_type);
	if( m_value.m_intf )
	{
		m_value.m_intf->decRef();
		m_value.m_intf = 0;
	}
}		

void EsVariant::doCleanup() ES_NOTHROW
{
	switch ( m_type )
	{
	case VAR_STRING:
		doInterpretAsString().EsString::~EsString();
		break;
	case VAR_BIN_BUFFER:
		doInterpretAsBinBuffer().EsBinBuffer::~EsBinBuffer();
		break;
	case VAR_STRING_COLLECTION:
		doInterpretAsStringCollection().EsString::Array::~Array();
		break;
	case VAR_VARIANT_COLLECTION:
		doInterpretAsVariantCollection().EsVariantVector::~EsVariantVector();
		break;	
	case VAR_OBJECT:
		releaseObject();
		break;
	default:
		break;
	}
}

void EsVariant::setEmpty() ES_NOTHROW
{
	doSetType(VAR_EMPTY);
}

void EsVariant::setToNull(Type type)
{
	doSetType(type == (Type)-1 ? m_type : type); // this will always clear the data
	switch ( m_type )
	{
	case VAR_EMPTY:
		break; // no need to copy anything
	case VAR_STRING:
		new((void*)&m_value) EsString();
		break;
	case VAR_BIN_BUFFER:
		new((void*)&m_value) EsBinBuffer();
		break;
	case VAR_STRING_COLLECTION:
		new((void*)&m_value) EsString::Array();
		break;
	case VAR_DOUBLE:
		m_value.m_double = 0.0;
		break;
	case VAR_OBJECT_EMBEDDED:
		{
			EsReflectedClassIntf::Ptr obj = asExistingObject().request<EsReflectedClassIntf>();
			wxASSERT(obj);
			obj->call(s_setToNull);
		}
		break;
	case VAR_OBJECT:
		releaseObject();
		break;	
	case VAR_VARIANT_COLLECTION:
		new((void*)&m_value) EsVariantVector();
		doInterpretAsVariantCollection().reserve(defVarCollectionSize);
		break;
	default:
		m_value.m_llong = 0; // this surely nullifies all the value types
		break;
	}
}

void EsVariant::setEmptyWithObjectDelete()
{
	switch ( m_type )
	{
	case VAR_OBJECT:
		releaseObject();
		m_type = VAR_EMPTY;
		break;
	case VAR_WXOBJECT:
		if( m_value.m_wxObj != 0 )
		{
			delete m_value.m_wxObj;
			m_value.m_wxObj = 0;
		}
		m_type = VAR_EMPTY;
		break;
	case VAR_VARIANT_COLLECTION:
		{
			EsVariantVector& vars = doInterpretAsVariantCollection();
			EsVariantVector::iterator it = vars.begin();
			EsVariantVector::iterator itEnd = vars.end();
			for ( ; it != itEnd; ++it )
				(*it).setEmptyWithObjectDelete();
			m_type = VAR_EMPTY;
			break;
		}
	default:
		setEmpty();
	}
}

bool EsVariant::isIndexed() const
{
	switch(m_type)
	{
	case VAR_BIN_BUFFER:
	case VAR_STRING:
	case VAR_STRING_COLLECTION:
	case VAR_VARIANT_COLLECTION:
		return true;
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:
		{
			const EsReflectedClassIntf::Ptr obj = asObject().request<EsReflectedClassIntf>();
			if( obj )
				return obj->isIndexed();
		}
	}
	return false;
}

int EsVariant::getCount() const
{
	switch ( m_type )
	{
	case VAR_BIN_BUFFER:
		return doInterpretAsBinBuffer().size();
	case VAR_STRING:
		return doInterpretAsString().size();
	case VAR_STRING_COLLECTION:
		return doInterpretAsStringCollection().size();
	case VAR_VARIANT_COLLECTION:
		return doInterpretAsVariantCollection().size();
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:
		{
			const EsReflectedClassIntf::Ptr obj = asExistingObject().request<EsReflectedClassIntf>();
			wxASSERT(obj);
			return obj->call(s_getCount).asInt();
		}
	default:
		EsException::ThrowCannotIndexItem();
		wxFAIL;
	}
	return 0; // we are never here
}

EsVariant& EsVariant::operator=(double f)
{
	doSetType(VAR_DOUBLE);
	m_value.m_double = f;
	return *this;
}

EsVariant& EsVariant::operator=(const wxChar* s)
{
	doSetType(VAR_STRING);
	::new((unsigned char*)&m_value) EsString(s);
	return *this;
}

EsVariant& EsVariant::operator=(const EsString& s)
{
	if( m_type == VAR_STRING )
		doInterpretAsString() = s;
	else
	{
		doSetType(VAR_STRING); // have to call DoSetType ...
		::new((unsigned char*)&m_value) EsString(s);
	}
	return *this;
}

EsVariant& EsVariant::operator=(const EsString::Array& c)
{
	if ( m_type == VAR_STRING_COLLECTION )
		doInterpretAsStringCollection() = c;
	else
	{
		doSetType(VAR_STRING_COLLECTION); // have to call DoSetType ...
		::new((unsigned char*)&m_value) EsString::Array(c);
	}
	return *this;
}

EsVariant& EsVariant::operator=(const EsVariantVector& c)
{
	if ( m_type == VAR_VARIANT_COLLECTION )
		doInterpretAsVariantCollection() = c;
	else
	{
		doSetType(VAR_VARIANT_COLLECTION); // have to call DoSetType ...
		::new((unsigned char*)&m_value) EsVariantVector(c);
	}
	return *this;
}

EsVariant& EsVariant::operator=(const EsBaseIntf::Ptr& obj)
{
	doSetType(VAR_OBJECT);
	m_value.m_intf = obj.get();
	if( m_value.m_intf )
		m_value.m_intf->incRef();
	return *this;
}

EsVariant& EsVariant::operator=(const EsVariant::ObjectByValue& o)
{
   doSetType(VAR_OBJECT_EMBEDDED);
   doCopyObjectEmbedded(&o);
   return *this;
}

EsVariant& EsVariant::operator=(const wxObject* obj)
{
	doSetType(VAR_WXOBJECT);
	m_value.m_wxObj = const_cast<wxObject*>(obj);
	return *this;
}

EsVariant& EsVariant::operator=(const void* ptr)
{
	doSetType(VAR_POINTER);
	m_value.m_pointer = const_cast<void*>(ptr);
	return *this;
}

EsVariant& EsVariant::operator=(const EsVariant& other)
{
	switch ( other.m_type )
	{
	case VAR_STRING:
		return operator=(other.doInterpretAsString());
	case VAR_BIN_BUFFER:
		assignBinBuffer(other.doInterpretAsBinBuffer());
		break;
	case VAR_STRING_COLLECTION:
		return operator=(other.doInterpretAsStringCollection());
	case VAR_OBJECT:
		return operator=(other.doInterpretAsObject());
	case VAR_WXOBJECT:
		return operator=(other.doInterpretAsWxObject());
	case VAR_VARIANT_COLLECTION:
		return operator=(other.doInterpretAsVariantCollection());
	default:
		doSetType(other.m_type);
		m_value = other.m_value; // copy the rest of types in a default way. Other variant can be EMPTY, this is okay.
	}
	return *this;
}

void EsVariant::assignBinBuffer(const EsBinBuffer& v)
{
	if( v.empty() )
		assign(0, 0);
	else
		assign(&v[0], v.size());
}

void EsVariant::assign(const UINT8* v, size_t len)
{
	if ( m_type == VAR_BIN_BUFFER )
		doInterpretAsBinBuffer().assign(v, v+len);
	else
	{
		doSetType(VAR_BIN_BUFFER); // have to call doSetType
		::new((unsigned char*)&m_value) EsBinBuffer(v, v+len);
	}
}

void EsVariant::assignString(const wxChar* v, size_t len)
{
	if ( m_type == VAR_STRING )
		doInterpretAsString().assign(v, len);
	else
	{
		doSetType(VAR_STRING); // have to call DoSetType
		::new((unsigned char*)&m_value) EsString(v, len);
	}
}

bool EsVariant::asBool(const std::locale& loc /*= std::locale()*/) const
{
	switch ( m_type )
	{
	case VAR_CHAR:
		return m_value.m_llong != (long long)'0' && m_value.m_llong != 0; // either '0' or '\0'
	case VAR_BOOL:
	case VAR_BYTE:
	case VAR_INT:
	case VAR_UINT:
	case VAR_INT64:
	case VAR_UINT64:
		return m_value.m_llong != 0;
	case VAR_DOUBLE:
		return m_value.m_double != 0.0;
	case VAR_BIN_BUFFER:
		{  // Perl-like convention
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			const EsBinBuffer _false = EsBinBuffer::fromAscii("false");
			if ( buff.empty() || std::equal(buff.begin(), buff.end(), _false.begin()) )
				return false;
			return !(buff.size() == 1 && (buff[0] == '\0' || buff[0] == '0'));
		}
	case VAR_STRING:
		{  // Perl-like convention
			const EsString& str = doInterpretAsString();
			if ( str.empty() || str.compare(wxT("false"), false, loc) == 0 ) // script convention
				return false;
			return !(str.size() == 1 && (str[0] == wxT('\0') || str[0] == wxT('0')));
		}
	case VAR_STRING_COLLECTION:
		return !doInterpretAsStringCollection().empty();  // convention
	case VAR_OBJECT:
		return m_value.m_intf != 0;  // convention
	case VAR_WXOBJECT:
		return m_value.m_wxObj != 0;  // convention
	case VAR_POINTER:
		return m_value.m_pointer != 0;  // convention
	case VAR_OBJECT_EMBEDDED:
		return true; // always exists
	case VAR_VARIANT_COLLECTION:
		return !doInterpretAsVariantCollection().empty();  // convention
	default:
		;
	}
	wxASSERT(m_type == VAR_EMPTY);
	return false; // by convention, VAR_EMPTY.asBool is false
}

static EsVariant doGetClientValueIfPresent(const EsVariant* self, const EsString& str)
{
	if ( !self->isObject() )
	{
		EsException::Throw(0, EsException::Generic, str.c_str());
		wxFAIL;
	}

	EsReflectedClassIntf::Ptr obj = self->asObject();
	if( obj && obj->hasProperty(EsVariant::s_value) )
		return obj->getProperty(EsVariant::s_value);
	
	EsException::Throw(0, EsException::Generic, str.c_str());
	wxFAIL;
	return EsVariant::s_null;
}

wxChar EsVariant::asChar(const std::locale& loc/* = std::locale()*/) const
{
	switch ( m_type )
	{
	case VAR_BYTE:
#if UNICODE
		return EsString(1, static_cast<wxChar>(m_value.m_ullong))[0];
#else
		// otherwise fall through to VAR_CHAR conversion
#endif
	case VAR_BOOL: // by convention...
	case VAR_CHAR:
		return (wxChar)m_value.m_ullong;
	case VAR_INT:
	case VAR_INT64:
	case VAR_UINT:
	case VAR_UINT64:
		{
			long long val = m_value.m_llong;
			if ( val < s_WXCHAR_MIN || val > s_WXCHAR_MAX ) // cover both signed and unsigned
			{
				EsString str = EsString::fromInt64(val, 10, loc);
				EsException::Throw(0, EsException::Generic, _("Could not convert '%s' to a single character"), str.c_str());
			}
			
			return (wxChar)val;
		}
	case VAR_DOUBLE:
		{
			double val = m_value.m_double;
			if ( val < double(s_WXCHAR_MIN) || val > double(s_WXCHAR_MAX) ) // cover both signed and unsigned
			{
				EsString str = EsString::fromDouble(val, loc);
				EsException::Throw(0, EsException::Generic, _("Could not convert '%s' to a single character"), str.c_str());
			}
			
			return EsUtilities::roundTo<wxChar>(val);
		}
	case VAR_BIN_BUFFER:
		{
			EsString str;
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if( !buff.empty() )
				str = wxString::From8BitData(reinterpret_cast<const char*>(&buff[0]), buff.size());
				
			if ( str.size() != 1 )
				EsException::Throw(0, EsException::Generic, _("Could not convert binary buffer %d bytes long to a single character"), str.size());

			return str[0];
		}
	case VAR_STRING:
		{
			const EsString& s = doInterpretAsString();
			if ( s.size() != 1 )
				EsException::Throw(0, EsException::Generic, _("Could not convert '%s' to a single character"), s.c_str());

			return s[0];
		}
	case VAR_POINTER:
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:
	case VAR_WXOBJECT:
	case VAR_STRING_COLLECTION:
	case VAR_VARIANT_COLLECTION:
		return doGetClientValueIfPresent(this, _("Could not convert variant to a character")).asChar(loc);
	default:
		;
	}
	wxASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	wxFAIL;
	return wxT('\0'); // we are never here, pacify compilers in debug mode
}

UINT8 EsVariant::asByte(const std::locale& loc /*= std::locale()*/) const
{
	switch ( m_type )
	{
	case VAR_CHAR:
#if UNICODE
		{
			wxChar c = (wxChar)m_value.m_ullong;
			if( c > 255 )
				EsException::Throw(0, EsException::Generic, _("Could not convert %d to a byte"), m_value.m_ullong);
	
			return (UINT8)c;
		}
#else
		// otherwise fall through to VAR_CHAR conversion
#endif
	case VAR_BOOL: // by convention...
	case VAR_BYTE:
		wxASSERT(m_value.m_ullong < 256);
		return (UINT8)m_value.m_ullong;
	case VAR_INT:
	case VAR_UINT:
	case VAR_INT64:
	case VAR_UINT64:
		{
			long long val = m_value.m_llong;
			EsNumericCheck::checkRangeInteger(0, 255, val);
			return (UINT8)val;
		}
	case VAR_DOUBLE:
		EsNumericCheck::checkRangeFloat(0.0, 255.0, m_value.m_double);
		return EsUtilities::roundTo<UINT8>(m_value.m_double);
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if ( buff.size() != 1 )
				EsException::Throw(0, EsException::Generic, _("Could not convert binary buffer %d bytes long to a byte"), buff.size());

			return buff[0];
		}
	case VAR_STRING:
		{
			const EsString& str = doInterpretAsString();
			unsigned long val = EsString::toUlong(str, 0, loc);
			if( val < 256 )
				return static_cast<UINT8>(val);
			else
				EsException::Throw(0, EsException::Generic, _("Could not convert '%s' to a byte"), str.c_str());

			return static_cast<UINT8>(str[0]);
		}
	case VAR_POINTER:		
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:	
	case VAR_WXOBJECT:	
	case VAR_STRING_COLLECTION:
	case VAR_VARIANT_COLLECTION:
		return doGetClientValueIfPresent(this, _("Could not convert variant to a byte")).asByte(loc);
	default:
		;
	}
	wxASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	wxFAIL;
	return (UINT8)'\0'; // we are never here, pacify compilers in debug mode
}

UINT32 EsVariant::asInternalDWord(const std::locale& loc /*= std::locale()*/) const
{
	switch ( m_type )
	{
	case VAR_POINTER:
		if( sizeof(UINT32) != sizeof(void*) )
			EsException::ThrowNotSupportedForThisType();
	case VAR_BOOL:
		wxASSERT(m_value.m_ullong <= 1);
	case VAR_BYTE:
	case VAR_CHAR:
#if UNICODE
		wxASSERT((m_value.m_ullong & ~0xFFFF) == 0);
#endif
		wxASSERT((m_value.m_ullong & ~0xFF) == 0); // this also tests the above VAR_BYTE case
	case VAR_INT:
	case VAR_UINT:
		return m_value.m_ullong;
	case VAR_DOUBLE:
		{
			double val = EsUtilities::round0(m_value.m_double);
			if( val < 0.0 )
			{
				if ( val < LONG_MIN )
				{
					EsException::Throw(0, EsException::Generic, _("Could not convert '%f.0' to integer"), val);
					wxFAIL;
				}
				return (unsigned)(long)val; // not (unsigned long)!
			}
			if ( val > ULONG_MAX )
			{
				EsException::Throw(0, EsException::Generic, _("Could not convert '%f.0' to integer"), val);
				wxFAIL;
			}
			return unsigned(val);
		}
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if ( buff.size() != 4 )
			{
				EsException::Throw(0, EsException::Generic, 
					wxT("Could not convert binary buffer %d bytes long to integer, four bytes expected"), buff.size());
				wxFAIL;
			}
			UINT32 val;
			memcpy(&val, &buff[0], sizeof(val));
			return val;
		}
	case VAR_STRING:
		{
			const EsString& str = doInterpretAsString();
			unsigned long val = EsString::toUlong(str, 0, loc);
			return static_cast<UINT32>(val);
		}
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:
	case VAR_WXOBJECT:
	case VAR_STRING_COLLECTION:
	case VAR_VARIANT_COLLECTION:
		return doGetClientValueIfPresent(this, _("Could not convert variant containing object reference to a numeric value")).asInternalDWord();
	default:
		;
	}
	wxASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	wxFAIL;
	return 0; // we are never here, pacify compilers in debug mode
}

UINT64 EsVariant::asInternalQWord(const std::locale& loc /*= std::locale()*/) const
{
	switch ( m_type )
	{
	case VAR_POINTER:
		if(sizeof(unsigned long long) != sizeof(void*))
			EsException::ThrowNotSupportedForThisType();
	case VAR_BOOL:
		wxASSERT(m_value.m_ullong <= 1);
	case VAR_BYTE:
	case VAR_CHAR:
#if UNICODE
		wxASSERT((m_value.m_ullong & ~0xFFFF) == 0);
#endif
		wxASSERT((m_value.m_ullong & ~0xFF) == 0); // this also tests the above VAR_BYTE case
	case VAR_INT:
	case VAR_UINT:
	case VAR_INT64:
	case VAR_UINT64:
		return m_value.m_ullong;
	case VAR_DOUBLE:
		{
			double val = EsUtilities::round0(m_value.m_double);
			if ( val < 0.0 )
			{
				if ( val < (double)LLONG_MIN )
				{
					EsException::Throw(0, EsException::Generic, wxT("Could not convert '%f.0' to 64 bit integer"), val);
					wxFAIL;
				}
				return static_cast<unsigned long long>(static_cast<long long>(val)); // not (unsigned long long)!
			}
			if ( val > (double)ULLONG_MAX )
			{
				EsException::Throw(0, EsException::Generic, wxT("Could not convert '%f.0' to 64 bit integer"), val);
				wxFAIL;
			}
			return static_cast<unsigned long long>(val);
		}
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if ( buff.size() != 8 )
			{
				EsException::Throw(0, EsException::Generic, 
					wxT("Could not convert binary buffer %d bytes long to 64 bit integer, eight bytes expected"), buff.size());
				wxFAIL;
			}
			UINT64 val;
			memcpy(&val, &buff[0], sizeof(val));
			return val;
		}
	case VAR_STRING:
		{
			const EsString& str = doInterpretAsString();
			unsigned long long val = EsString::toUint64(str, 0, loc);
			return val;
		}
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:
	case VAR_WXOBJECT:
	case VAR_STRING_COLLECTION:
	case VAR_VARIANT_COLLECTION:
		return doGetClientValueIfPresent(this, _("Could not convert variant containing object reference to a numeric value")).asInternalDWord();
	default:
		;
	}
	wxASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	wxFAIL;
	return 0; // we are never here, pacify compilers in debug mode
}

long EsVariant::asLong(const std::locale& loc /*= std::locale()*/) const
{
	switch ( m_type )
	{
	case VAR_BOOL:
		wxASSERT(m_value.m_ullong <= 1);
	case VAR_INT64:
		wxASSERT( m_value.m_llong >= LONG_MIN && m_value.m_llong <= LONG_MAX );
	case VAR_BYTE:
	case VAR_CHAR:
	case VAR_INT:
		return m_value.m_llong;
	case VAR_UINT:
	case VAR_UINT64:
		if( m_value.m_ullong > LONG_MAX ) // unsigned integer bigger than maximum long
		{
			EsString str = EsString::fromUint64(m_value.m_ullong, 10, loc);
			EsException::Throw(0, EsException::Generic, _("Could not convert '%s' to signed integer"), str.c_str());

			wxFAIL;
		}
		return m_value.m_llong;
	case VAR_DOUBLE:
		{
			double val = EsUtilities::round0(m_value.m_double);
			if ( val < (double)LONG_MIN || val > (double)LONG_MAX )
			{
				EsException::Throw(0, EsException::Generic, _("Could not convert '%.0f' to integer"), val);
				wxFAIL;
			}
			return static_cast<long>(val);
		}
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if ( buff.size() != 4 )
			{
				EsException::Throw(0, EsException::Generic, 
					_("Could not convert binary buffer %d bytes long to integer, four bytes expected"), buff.size());
				wxFAIL;
			}
			long val;
			memcpy(&val, &buff[0], 4);
			return val;
		}
	case VAR_STRING:
	{
		const EsString& str = doInterpretAsString();
		return EsString::toLong(str, 0, loc);
	}
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:	
	case VAR_WXOBJECT:	
	case VAR_VARIANT_COLLECTION:
	case VAR_STRING_COLLECTION:
		return doGetClientValueIfPresent(this, 
			wxT("Could not convert variant containing object reference to an integer value")).asLong(loc);
	default:
		;
	}
	wxASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	wxFAIL;
	return 0; // we are never here, pacify compilers in debug mode
}

long long EsVariant::asLLong(const std::locale& loc /*= std::locale()*/) const
{
	switch ( m_type )
	{
	case VAR_BOOL:
		wxASSERT(m_value.m_ullong <= 1);
	case VAR_INT64:
	case VAR_BYTE:
	case VAR_CHAR:
	case VAR_INT:
		return m_value.m_llong;
	case VAR_UINT:
	case VAR_UINT64:
		if( m_value.m_ullong > LLONG_MAX ) // unsigned integer bigger than maximum long
		{
			EsString str = EsString::fromUint64(m_value.m_ullong, 10, loc);
			EsException::Throw(0, EsException::Generic, wxT("Could not convert '%s' to signed integer"), str.c_str());

			wxFAIL;
		}
		return m_value.m_llong;
	case VAR_DOUBLE:
		{
			double val = EsUtilities::round0(m_value.m_double);
			if ( val < (double)LLONG_MIN || val > (double)LLONG_MAX )
			{
				EsException::Throw(0, EsException::Generic, wxT("Could not convert '%.0f' to 64 bit integer"), val);
				wxFAIL;
			}
			return static_cast<long long>(val);
		}
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if ( buff.size() != 8 )
			{
				EsException::Throw(0, EsException::Generic, 
					wxT("Could not convert binary buffer %d bytes long to 64bit integer, eight bytes expected"), buff.size());
				wxFAIL;
			}
			long long val;
			memcpy(&val, &buff[0], 8);
			return val;
		}
	case VAR_STRING:
	{
		const EsString& str = doInterpretAsString();
		return EsString::toInt64(str, 0, loc);
	}
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:	
	case VAR_WXOBJECT:	
	case VAR_VARIANT_COLLECTION:
	case VAR_STRING_COLLECTION:
		return doGetClientValueIfPresent(this, 
			wxT("Could not convert variant containing object reference to an integer value")).asLLong(loc);
	default:
		;
	}
	wxASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	wxFAIL;
	return 0; // we are never here, pacify compilers in debug mode
}

unsigned long EsVariant::asULong(const std::locale& loc /*= std::locale()*/) const
{
	switch ( m_type )
	{
	case VAR_POINTER:
		if( sizeof(unsigned long) != sizeof(void*) )
			EsException::ThrowNotSupportedForThisType();
	case VAR_BOOL:
		wxASSERT(m_value.m_ullong <= 1);
	case VAR_UINT64:
		wxASSERT( m_value.m_ullong <= ULONG_MAX );
	case VAR_BYTE:
	case VAR_CHAR:
	case VAR_UINT:
		return m_value.m_ullong;
	case VAR_INT:
	case VAR_INT64:
		if( m_value.m_llong < 0 )
		{
			EsString str = EsString::fromInt64(m_value.m_llong, 10, loc);
			EsException::Throw(0, EsException::Generic, wxT("Could not convert '%s' to unsigned integer"), str.c_str());
			wxFAIL;
		}
		return m_value.m_ullong;
	case VAR_DOUBLE:
		{
			double val = EsUtilities::round0(m_value.m_double);
			if ( val < 0.0 || val > (double)ULONG_MAX )
			{
				EsException::Throw(0, EsException::Generic, _("Could not convert '%.0f' to unsigned integer"), val);
				wxFAIL;
			}
			return (unsigned long)val;
		}
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if ( buff.size() != 4 )
			{
				EsException::Throw(0, EsException::Generic, 
					_("Could not convert binary buffer %d bytes long to unsigned integer, four bytes expected"), buff.size());
				wxFAIL;
			}
			unsigned long val;
			memcpy(&val, &buff[0], sizeof(val));
			return val;
		}
	case VAR_STRING:
		{
			const EsString& str = doInterpretAsString(); 
			return EsString::toUlong(str, 0, loc);
		}
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:	
	case VAR_WXOBJECT:	
	case VAR_VARIANT_COLLECTION:
	case VAR_STRING_COLLECTION:
		return doGetClientValueIfPresent(this, 
			wxT("Could not convert variant containing object reference to a numeric value")).asULong(loc);
	default:
		;
	}
	wxASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	wxFAIL;
	return 0uL; // we are never here, pacify compilers in debug mode
}

unsigned long long EsVariant::asULLong(const std::locale& loc /*= std::locale()*/) const
{
	switch ( m_type )
	{
	case VAR_POINTER:
		if( sizeof(unsigned long long) != sizeof(void*) )
			EsException::ThrowNotSupportedForThisType();
	case VAR_BOOL:
		wxASSERT(m_value.m_ullong <= 1);
	case VAR_UINT64:
	case VAR_BYTE:
	case VAR_CHAR:
	case VAR_UINT:
		return m_value.m_ullong;
	case VAR_INT:
	case VAR_INT64:
		if( m_value.m_llong < 0 )
		{
			EsString str = EsString::fromInt64(m_value.m_llong, 10, loc);
			EsException::Throw(0, EsException::Generic, wxT("Could not convert '%s' to 64 bit unsigned integer"), str.c_str());
			wxFAIL;
		}
		return m_value.m_ullong;
	case VAR_DOUBLE:
		{
			double val = EsUtilities::round0(m_value.m_double);
			if ( val < 0.0 || val > (double)ULLONG_MAX )
			{
				EsException::Throw(0, EsException::Generic, wxT("Could not convert '%.0f' to 64 bit unsigned integer"), val);
				wxFAIL;
			}
			return static_cast<unsigned long long>(val);
		}
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if ( buff.size() != 8 )
			{
				EsException::Throw(0, EsException::Generic, 
					wxT("Could not convert binary buffer %d bytes long to 64 bit unsigned integer, eight bytes expected"), buff.size());
				wxFAIL;
			}
			unsigned long long val;
			memcpy(&val, &buff[0], sizeof(val));
			return val;
		}
	case VAR_STRING:
		{
			const EsString& str = doInterpretAsString(); 
			return EsString::toUint64(str, 0, loc);
		}
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:	
	case VAR_WXOBJECT:	
	case VAR_VARIANT_COLLECTION:
	case VAR_STRING_COLLECTION:
		return doGetClientValueIfPresent(this, 
			wxT("Could not convert variant containing object reference to a numeric value")).asULLong(loc);
	default:
		;
	}
	wxASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	wxFAIL;
	return 0ULL; // we are never here, pacify compilers in debug mode
}

double EsVariant::asDouble(const std::locale& loc /*= std::locale()*/) const
{
	switch ( m_type )
	{
	case VAR_BOOL:
		wxASSERT(m_value.m_ullong <= 1);
	case VAR_BYTE: // byte is unsigned
	case VAR_CHAR: // char is unsigned by convention
#if UNICODE
		wxASSERT((m_value.m_ullong & ~0xFFFF) == 0);
#else
		wxASSERT((m_value.m_ullong & ~0xFF) == 0); // this also tests the above VAR_BYTE case
#endif
	case VAR_INT:
	case VAR_INT64:
		return static_cast<double>(m_value.m_llong);
	case VAR_UINT:
	case VAR_UINT64:
	case VAR_POINTER:	
		return static_cast<double>(m_value.m_ullong);
	case VAR_DOUBLE:
		return m_value.m_double;
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if ( buff.size() != sizeof(double) )
			{
				EsException::Throw(0, EsException::Generic, 
					_("Could not convert binary buffer %d bytes long to floating point number, eight bytes expected"), buff.size());
				wxFAIL;
			}
			double val;
			memcpy(&val, &buff[0], sizeof(double));
			return val;
		}
	case VAR_STRING:
		return EsString::toDouble( doInterpretAsString(), loc );
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:	
	case VAR_WXOBJECT:	
	case VAR_VARIANT_COLLECTION:
	case VAR_STRING_COLLECTION:
		return doGetClientValueIfPresent(this, 
			_("Could not convert variant containing object reference to a numeric value")).asDouble(loc);
	default:
		;
	}
	wxASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	wxFAIL;
	return 0.0; // we are never here, pacify compilers in debug mode
}

EsBinBuffer EsVariant::asBinBuffer() const
{
	wxCOMPILE_TIME_ASSERT(sizeof(long) == sizeof(unsigned long), longSizeEqUlongSize);
	wxCOMPILE_TIME_ASSERT(sizeof(long long) == sizeof(unsigned long long), llongSizeEqUllongSize);

	EsBinBuffer result;
	switch ( m_type )
	{
	case VAR_EMPTY:
		EsException::ThrowNoValue();
		wxFAIL;
	case VAR_BOOL:
		result.assign(1, (m_value.m_llong != 0) ? '\1' : '\0');
		break;
	case VAR_INT:
	case VAR_UINT: // we assume int and uint are of the same size
		result.assign(reinterpret_cast<const UINT8*>(&m_value.m_llong), reinterpret_cast<const UINT8*>(&m_value.m_llong) + sizeof(long));
		break;
	case VAR_INT64:
	case VAR_UINT64: // we assume int64 and uint64 are of the same size
		result.assign(reinterpret_cast<const UINT8*>(&m_value.m_llong), reinterpret_cast<const UINT8*>(&m_value.m_llong) + sizeof(long long));
		break;
	case VAR_DOUBLE:
		result.assign(reinterpret_cast<const UINT8*>(&m_value.m_double), reinterpret_cast<const UINT8*>(&m_value.m_double) + sizeof(double));
		break;
	case VAR_BYTE:
		result.assign(1, static_cast<UINT8>(m_value.m_ullong));
		break;
	case VAR_CHAR:
		{
			const wxChar ch = wxChar(m_value.m_ullong);
			result.assign(reinterpret_cast<const UINT8*>(&ch), reinterpret_cast<const UINT8*>(&ch) + sizeof(wxChar));
			break;
		}
	case VAR_BIN_BUFFER:
		result = doInterpretAsBinBuffer();
		break;
	case VAR_STRING:
		result = EsString::hexToBin(doInterpretAsString());
		break;
	case VAR_POINTER:	
		EsException::ThrowNotSupportedForThisType();
		break;
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:	
	case VAR_WXOBJECT:
	case VAR_STRING_COLLECTION:
	case VAR_VARIANT_COLLECTION:
		result = doGetClientValueIfPresent(this, 
			_("Could not convert variant containing object reference to a binary buffer")).asBinBuffer();
		break;
	default:
		wxFAIL;
	}
	return result;
}

EsString EsVariant::asString(const std::locale& loc /*= std::locale()*/) const
{
	EsString result;
	switch ( m_type )
	{
	case VAR_EMPTY:
		EsException::ThrowNoValue();
		wxFAIL;
	case VAR_INT:
	case VAR_BOOL:
		result = EsString::fromLong(m_value.m_llong, 10, loc);
		break;
	case VAR_BYTE:
	case VAR_UINT:
		result = EsString::fromUlong(m_value.m_ullong, 10, loc);
		break;
	case VAR_INT64:
		result = EsString::fromInt64(m_value.m_llong, 10, loc);
		break;
	case VAR_UINT64:
		result = EsString::fromUint64(m_value.m_ullong, 10, loc);
		break;
	case VAR_DOUBLE:
		result = EsString::fromDouble(m_value.m_double, loc);
		break;
	case VAR_CHAR:
		result = static_cast<EsString::value_type>(m_value.m_ullong);
		break;
	case VAR_BIN_BUFFER:
		result = EsString::binToHex(doInterpretAsBinBuffer());
		break;
	case VAR_STRING:
		result = doInterpretAsString();
		break;
	case VAR_POINTER:		
		EsException::Throw(0, EsException::Generic, _("Could not convert generic pointer to a string value"));
		wxFAIL;
	case VAR_WXOBJECT:
		EsException::Throw(0, EsException::Generic, _("Could not convert wxObject reference to a string value"));
		wxFAIL;
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:
		{
			const EsReflectedClassIntf::Ptr obj = asExistingObject().request<EsReflectedClassIntf>();
			if( obj->hasMethod(MethodInfoKeyT(0, s_asString)) )
				return obj->call(s_asString).doInterpretAsString();
			result = doGetClientValueIfPresent(this, 
				_("Could not convert variant containing object reference to a string value")).asString(loc);
			break;
		}
	case VAR_STRING_COLLECTION:
	case VAR_VARIANT_COLLECTION:
		EsException::Throw(0, EsException::Generic, _("Could not convert collection to a string value"));
		wxFAIL;
	default:
		wxFAIL; // we are never here, pacify compilers in debug mode
	}
	return result; 
}

EsString EsVariant::asString(unsigned mask, const std::locale& loc /*= std::locale()*/) const
{
	EsString result;
	switch ( m_type )
	{
	case VAR_CHAR:
		result = EsString::toString( EsString(1, static_cast<wxChar>(m_value.m_ullong)) );
		break;
	case VAR_BYTE:
		{
			char c = static_cast<char>(m_value.m_ullong);
			result = EsString::toString( wxString::From8BitData(&c, 1) );
			break;
		}
	case VAR_BIN_BUFFER:
		result = EsString::toString( EsString::binToHex(doInterpretAsBinBuffer()), mask );
		break;
	default:
		result = EsString::toString(asString(loc), mask);
		break;
	}
	return result;
}

EsString EsVariant::asEscapedString(const std::locale& loc /*= std::locale()*/) const
{
	return asString(0, loc);
}

EsString::Array EsVariant::asStringCollection(const std::locale& loc /*= std::locale()*/) const
{
	EsString::Array result;
	switch ( m_type )
	{
	case VAR_STRING_COLLECTION:
		result = doInterpretAsStringCollection();
		break;
	case VAR_VARIANT_COLLECTION:
		{
			const EsVariantVector& vars = doInterpretAsVariantCollection();
			result.reserve(vars.size());
			for( EsVariantVector::const_iterator cit = vars.begin(); cit != vars.end(); ++cit )
				result.push_back((*cit).asString());
			break;
		}
	default:
		result.push_back(asString(loc)); // this will throw an error if variant is empty
		break;
	}
	return result;
}

EsVariant::EsVariantVector EsVariant::asVariantCollection() const
{
	EsVariantVector result;
	switch ( m_type )
	{
	case VAR_EMPTY:
		EsException::ThrowNoValue();
		wxFAIL;
	case VAR_STRING_COLLECTION:
		{
			const EsString::Array& vars = doInterpretAsStringCollection();
			result.reserve(vars.size());
			for( EsString::Array::const_iterator cit = vars.begin(); cit != vars.end(); ++cit )
				result.push_back( *cit );
			break;
		}
	case VAR_VARIANT_COLLECTION:
		result = doInterpretAsVariantCollection();
		break;
	default:
		result.push_back(*this);
		break;
	}
	return result;
}

EsBaseIntf::Ptr EsVariant::asObject()
{
	switch( m_type )
	{
	case VAR_OBJECT:
		return doInterpretAsObject();
	case VAR_OBJECT_EMBEDDED:
		return EsBaseIntf::Ptr(doInterpretAsObjectEmbedded());
	case VAR_EMPTY:
		EsException::ThrowNoValue();
		wxFAIL;
	default:
		EsException::Throw(0, EsException::Generic, _("Could not convert variant to interface reference"));
		wxFAIL;
	}
	wxFAIL;
	return EsBaseIntf::Ptr(); // we are never here, pacify compilers in debug mode
}

EsBaseIntf::Ptr EsVariant::asExistingObject()
{
	EsBaseIntf::Ptr obj = asObject();
	if( !obj )
	{
		EsException::ThrowNoValue();
		wxFAIL;
	}
	return obj;
}

wxObject* EsVariant::asWxObject()
{
	switch( m_type )
	{
	case VAR_WXOBJECT:
		return m_value.m_wxObj;
	case VAR_EMPTY:
		EsException::ThrowNoValue();
		wxFAIL;
	default:
		EsException::Throw(0, EsException::Generic, _("Could not convert variant to wxObject reference"));
		wxFAIL;
	}
	wxFAIL;
	return 0; // we are never here, pacify compilers in debug mode
}

wxObject* EsVariant::asExistingWxObject()
{
	wxObject* obj = asWxObject();
	if ( obj == NULL )
	{
		EsException::ThrowNoValue();
		wxFAIL;
	}
	return obj;
}

void* EsVariant::asPointer()
{
	switch( m_type )
	{
	case VAR_OBJECT:
		return reinterpret_cast<void*>(m_value.m_intf);
	case VAR_WXOBJECT:
		return reinterpret_cast<void*>(m_value.m_wxObj);
	case VAR_POINTER:
		return m_value.m_pointer;
	case VAR_EMPTY:
		EsException::ThrowNoValue();
		wxFAIL;
	default:
		EsException::Throw(0, EsException::Generic, _("Could not convert variant to generic pointer"));
		wxFAIL;
	}
	wxFAIL;
	return 0; // we are never here, pacify compilers in debug mode
}

void EsVariant::swap(EsVariant& v) ES_NOTHROW
{
	// We know that Variant is always movable in memory as a whole
	//
	char tmp[ sizeof(EsVariant) ];
	memcpy(tmp, this, sizeof(EsVariant));
	memcpy(this, &v, sizeof(EsVariant));
	memcpy(&v, tmp, sizeof(EsVariant));
}

void EsVariant::moveFrom(EsVariant& other) ES_NOTHROW
{
	doCleanup();
	memcpy(this, &other, sizeof(EsVariant));
	other.m_type = VAR_EMPTY;
}

EsVariant EsVariant::pow(const EsVariant& val, const std::locale& loc /*= std::locale()*/) const
{
	return ::pow(asDouble(loc), val.asDouble(loc));
}

void EsVariant::adjustIndex(int& index, unsigned count)
{
	wxASSERT((int)count >= 0); // check if we are not too big (16-bit architecture possibility)
	int signedCount = int(count);
	EsNumericCheck::checkRangeInteger(-signedCount, signedCount-1, index);
	if ( index < 0 )
		index += signedCount;
}

EsVariant EsVariant::getItem(const EsVariant& idx) const
{
	EsVariant result;
	if ( m_type == VAR_OBJECT || m_type == VAR_OBJECT_EMBEDDED )
	{
		EsReflectedClassIntf::Ptr obj = asExistingObject().request<EsReflectedClassIntf>();
		wxASSERT(obj);
		result.doAssignToEmpty(obj->call(s_getItem, idx));
	}
	else
	{
		int index = idx.asInt();
		switch ( m_type )
		{
		case VAR_BIN_BUFFER:
			{
				const EsBinBuffer& v = doInterpretAsBinBuffer();
				adjustIndex(index, v.size());
				result.doAssignToEmpty(v[index]);
				break;
			}
		case VAR_STRING:
			{
				const EsString& v = doInterpretAsString();
				adjustIndex(index, v.size());
				result.doAssignToEmpty( static_cast<wxChar>(v[index]) );
				break;
			}
		case VAR_STRING_COLLECTION:
			{
				const EsString::Array& v = doInterpretAsStringCollection();
				adjustIndex(index, v.size());
				result.doAssignToEmpty(v[index]);
				break;
			}
		case VAR_VARIANT_COLLECTION:
			{
				const EsVariantVector& v = doInterpretAsVariantCollection();
				adjustIndex(index, v.size());
				result.doAssignToEmpty(v[index]);
				break;
			}
		default:
			EsException::ThrowCannotIndexItem();
			wxFAIL;
		}
	}
	return result;
}

void EsVariant::setItem(const EsVariant& index, const EsVariant& value, const std::locale& loc /*= std::locale()*/)
{
	if ( m_type == VAR_OBJECT || m_type == VAR_OBJECT_EMBEDDED )
	{
		EsReflectedClassIntf::Ptr obj = asExistingObject().request<EsReflectedClassIntf>();
		wxASSERT(obj);
		obj->call(s_setItem, index, value);
	}
	else
	{
		int intIndex = index.asInt();
		switch ( m_type )
		{
		case VAR_BIN_BUFFER:
			{
				EsBinBuffer& v = doInterpretAsBinBuffer();
				adjustIndex(intIndex, v.size());
				v[intIndex] = value.asByte(loc);
			}
			break;
		case VAR_STRING:
			{
				EsString& v = doInterpretAsString();
				adjustIndex(intIndex, v.size());
				v[intIndex] = value.asChar(loc);
			}
			break;
		case VAR_STRING_COLLECTION:
			{
				EsString::Array& v = doInterpretAsStringCollection();
				adjustIndex(intIndex, v.size());
				v[intIndex] = value.asString(loc);
			}
			break;
		case VAR_VARIANT_COLLECTION:
			{
				EsVariantVector& v = doInterpretAsVariantCollection();
				adjustIndex(intIndex, v.size());
				v[intIndex] = value;
			}
			break;
		default:
			EsException::ThrowCannotIndexItem();
			wxFAIL;
		}
	}
}

//int EsVariant::AdjustSlice(int& from, int& to, unsigned count)
//{
//	const int length = static_cast<int>(count);
//
//	if ( from < 0 )
//		from += length;
//	if ( from < 0 )
//		from = 0;
//	if ( from >= length )
//		from = length;
//
//	if ( to < 0 )
//		to += length;
//	if ( to < 0 )
//		to = -1; // here is an assymmetry in the way from and to handled
//	if ( to > length )
//		to = length;
//
//	int size = to - from;
//	if ( size < 0 )
//	{
//		to = from;
//		return 0;
//	}
//	return size;
//}
//
//EsVariant EsVariant::GetSlice(int from, int to) const
//{
//	EsVariant result;
//	switch ( m_type )
//	{
//	default:
//		EsException::ThrowCannotIndexItem();
//		wxFAIL;
//	case VAR_BIN_BUFFER:
//		{
//			const EsBinBuffer* v = &doInterpretAsBinBuffer();
//			int size = AdjustSlice(from, to, M_64_CAST(unsigned, v->size()));
//			result.doAssignEsBinBufferoEmpty(v->substr(from, size));
//			break;
//		}
//	case VAR_STRING:
//		{
//			const EsString* v = &doInterpretAsString();
//			int size = AdjustSlice(from, to, M_64_CAST(unsigned, v->size()));
//			result.doAssignToEmpty(v->substr(from, size));
//			break;
//		}
//	case VAR_STRING_COLLECTION:
//		{
//			const EsString::Array* v = &doInterpretAsStringCollection();
//			int size = AdjustSlice(from, to, M_64_CAST(unsigned, v->size()));
//			EsString::Array::const_iterator start = v->begin() + from;
//			result.doAssignToEmpty(EsString::Array(start, start + size));
//			break;
//		}
//	case VAR_VARIANT_COLLECTION:
//		{
//			const EsVariantVector* v = &doInterpretAsVariantCollection();
//			int size = AdjustSlice(from, to, M_64_CAST(unsigned, v->size()));
//			EsVariantVector::const_iterator start = v->begin() + from;
//			result.doAssignToEmpty(EsVariantVector(start, start + size));
//			break;
//		}
//	}
//	return result;
//}
//
//void EsVariant::SetSlice(int from, int to, const EsVariant& values)
//{
//	switch ( m_type )
//	{
//	case VAR_BIN_BUFFER:
//		{
//			EsBinBuffer* v = &doInterpretAsBinBuffer();
//			int size = AdjustSlice(from, to, M_64_CAST(unsigned, v->size()));
//			if ( values.IsIndexed() )
//				v->replace(from, size, values.asBinBuffer());
//			else if ( values.isEmpty() )
//				v->erase(from, size);
//			else
//				v->replace(from, size, 1, values.asByte());
//		}
//		break;
//	case VAR_STRING:
//		{
//			EsString* v = &doInterpretAsString();
//			int size = AdjustSlice(from, to, M_64_CAST(unsigned, v->size()));
//			if ( values.IsIndexed() )
//				v->replace(from, size, values.asString());
//			else if ( values.isEmpty() )
//				v->erase(from, size);
//			else
//				v->replace(from, size, 1, (wxChar)values.asChar());
//		}
//		break;
//	case VAR_STRING_COLLECTION:
//		{
//			EsString::Array* v = &doInterpretAsStringCollection();
//			AdjustSlice(from, to, M_64_CAST(unsigned, v->size()));
//			EsString::Array::iterator start = v->begin() + from;
//			Type type = values.GetType();
//			if ( type == VAR_STRING_COLLECTION || type == VAR_VARIANT_COLLECTION )
//			{
//				start = v->erase(start, v->begin() + to);
//				const EsString::Array val = values.asStringCollection();
//				v->insert(start, val.begin(), val.end());
//			}
//			else if ( values.isEmpty() )
//				v->erase(v->begin() + from, v->begin() + to);
//			else
//			{
//				EsString str = values.asString(); // do it prior to modifying the vector as it can throw an error
//				v->erase(start, v->begin() + to);
//				v->insert(start, str);
//			}
//		}
//		break;
//	case VAR_VARIANT_COLLECTION:
//		{
//			EsVariantVector* v = &doInterpretAsVariantCollection();
//			AdjustSlice(from, to, M_64_CAST(unsigned, v->size()));
//			EsVariantVector::iterator start = v->begin() + from;
//			Type type = values.GetType();
//			if ( type == VAR_STRING_COLLECTION || type == VAR_VARIANT_COLLECTION )
//			{
//				start = v->erase(start, v->begin() + to);
//				EsVariantVector val = values.asVariantCollection();
//				EsVariantVector::const_iterator it = val.begin();
//				EsVariantVector::const_iterator itEnd = val.end();
//				v->insert(start, it, itEnd);
//			}
//			else if ( values.isEmpty() )
//				v->erase(v->begin() + from, v->begin() + to);
//			else
//			{
//				v->erase(start, v->begin() + to);
//				v->insert(start, values);
//			}
//		}
//		break;
//	default:
//		EsException::ThrowCannotIndexItem();
//		wxFAIL;
//	}
//}

static int doCompareObjects(const EsVariant& v1, const EsVariant& v2, bool equality = false)
{
	wxASSERT(v1.isObject() || v2.isObject());
	if( v1.isObject() )
	{
		EsReflectedClassIntf::Ptr o1 = v1.asObject().request<EsReflectedClassIntf>();
		if( o1 )
		{
			if( v2.isObject() && o1 == v2.asObject() )
				return 0;
			if( equality && !o1->hasMethod( MethodInfoKeyT(1, EsVariant::s_compare) ) )
				return 1; // not equal
			return o1->call(EsVariant::s_compare, v2).asInt();
		}
		// o1 == NULL
		if( v2.isEmpty() )
			return 0; // NULL object is equal to empty variant
		if( !v2.isObject() )
			return -1; // NULL object is less than any other value
	}
	if( v2.isObject() )
	{
		EsReflectedClassIntf::Ptr o2 = v2.asObject().request<EsReflectedClassIntf>();
		if( o2 )
		{
			if ( equality && !o2->hasMethod( MethodInfoKeyT(1, EsVariant::s_compare) ) )
				return 1; // not equal
			return -(o2->call(EsVariant::s_compare, v1).asInt()); // we changed the order of the arguments => change the sign of the result
		}
		// o2 == NULL
		if( v1.isEmpty() )
			return 0; // empty variant is equal to NULL object
		if( !v1.isObject() )
			return 1; // non-object is more than NULL object
	}
	// v1.isObject() && v2.isObject()
	// both are NULL objects
	return 0;
}

bool EsVariant::operator==(const EsVariant& v) const
{
	switch( (m_type > v.m_type) ? m_type : v.m_type )
	{
	case VAR_EMPTY: // both variables are empty, comparison gives true
		return true;
	case VAR_BOOL:
		return asBool() == v.asBool();
	case VAR_BYTE:
	case VAR_CHAR:
	case VAR_UINT:
	case VAR_INT:
	case VAR_UINT64:
	case VAR_INT64:
	case VAR_DOUBLE:
		return asDouble() == v.asDouble(); // works very well even for CHAR, UINT and INT, precision is not decreased
	case VAR_BIN_BUFFER:
		return asBinBuffer() == v.asBinBuffer();
	case VAR_STRING_COLLECTION:
		return asStringCollection() == v.asStringCollection();
	case VAR_WXOBJECT:
		return asExistingWxObject()->IsSameAs(*v.asExistingWxObject());
	case VAR_OBJECT:
		return doCompareObjects(*this, v, true) == 0;
	case VAR_POINTER:
		return asPointer() == v.asPointer(); // only shallow comparison is supported
	case VAR_VARIANT_COLLECTION:
		return asVariantCollection() == v.asVariantCollection();
	default: // case VAR_STRING, also will throw No Value for VAR_EMPTY...
		return asString() == v.asString();
	}
}

template <typename Vect>
static bool doVectorLess(const Vect& left, const Vect& right)
{
	for ( unsigned i = 0 ; i < right.size() && i < left.size() ; ++i )
	{
		if ( left[i] < right[i] )
			return true;
		if ( left[i] > right[i] )
			return false;
	}
	return left.size() < right.size();
}

bool EsVariant::operator<(const EsVariant& v) const
{
	switch ( (m_type > v.m_type) ? m_type : v.m_type )
	{
	case VAR_BOOL:
		return !asBool() && v.asBool();
	case VAR_BYTE:
	case VAR_CHAR:
	case VAR_UINT:
	case VAR_INT:
	case VAR_UINT64:
	case VAR_INT64:
	case VAR_DOUBLE:
	case VAR_POINTER:	
		return asDouble() < v.asDouble(); // works very well even for CHAR, UINT and INT, precision is not decreased
	case VAR_BIN_BUFFER:
		return asBinBuffer() < v.asBinBuffer();
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:	
		return doCompareObjects(*this, v) < 0;
	case VAR_WXOBJECT:
		EsException::ThrowNotSupportedForThisType();
		wxFAIL;
	case VAR_STRING_COLLECTION:
		return doVectorLess(asStringCollection(), v.asStringCollection());
	case VAR_VARIANT_COLLECTION:
		return doVectorLess(asVariantCollection(), v.asVariantCollection());
	default: // case VAR_STRING, also will throw No Value for VAR_EMPTY...
		return asString() < v.asString();
	}
}

bool EsVariant::operator>(const EsVariant& v) const
{
	switch ( (m_type > v.m_type) ? m_type : v.m_type )
	{
	case VAR_BOOL:
		return asBool() && !v.asBool();
	case VAR_BYTE:
	case VAR_CHAR:
	case VAR_UINT:
	case VAR_INT:
	case VAR_UINT64:
	case VAR_INT64:
	case VAR_DOUBLE:
	case VAR_POINTER:	
		return asDouble() > v.asDouble(); // works very well even for CHAR, UINT and INT, precision is not decreased
	case VAR_BIN_BUFFER:
		return asBinBuffer() > v.asBinBuffer();
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:
		return doCompareObjects(*this, v) > 0;
	case VAR_WXOBJECT:
		EsException::ThrowNotSupportedForThisType();
		wxFAIL;		
	case VAR_STRING_COLLECTION:
		return doVectorLess(v.asStringCollection(), asStringCollection());
	case VAR_VARIANT_COLLECTION:
		return doVectorLess(v.asVariantCollection(), asVariantCollection());
	default: // case VAR_STRING, also will throw No Value for VAR_EMPTY...
		return asString() > v.asString();
	}
}

enum DoAndOrXorEnum
{
	doAnd = -1,
	doOr  = 0,
	doXor = 1
};

static void doAndOrXorOnVariants(EsVariant& result, const EsVariant& v1, const EsVariant& v2, DoAndOrXorEnum andOrXor)
{
	const EsBinBuffer& self = v1.asBinBuffer();
	const EsBinBuffer& other = v2.asBinBuffer();
	if ( self.size() != other.size() )
	{
		EsException::Throw(0, EsException::Generic, _("Sizes of items are different, %d and %d"), self.size(), other.size());
		wxFAIL;
	}
	result.doAssignEsBinBufferToEmpty(self);
	EsBinBuffer& resultByteString = result.doInterpretAsBinBuffer();
	EsBinBuffer::iterator it = resultByteString.begin();
	EsBinBuffer::iterator itEnd = resultByteString.end();
	EsBinBuffer::const_iterator otherIt = other.begin();
	if ( andOrXor < 0 )
	{
		wxASSERT(andOrXor == doAnd);
		for ( ; it != itEnd; ++it, ++otherIt )
			*it &= (unsigned char)*otherIt;
	}
	else if ( andOrXor > 0 )
	{
		wxASSERT(andOrXor == doXor);
		for ( ; it != itEnd; ++it, ++otherIt )
			*it ^= (unsigned char)*otherIt;
	}
	else
	{
		wxASSERT(andOrXor == doOr);
		for ( ; it != itEnd; ++it, ++otherIt )
			*it |= (unsigned char)*otherIt;
	}
}

EsVariant EsVariant::operator|(const EsVariant& v) const
{
	EsVariant result;
	switch ( (m_type > v.m_type) ? m_type : v.m_type )
	{
	case VAR_BOOL:
	case VAR_OBJECT: // assume object's zero check
		result.doAssignToEmpty((bool)(asBool() || v.asBool()));
		break;
	case VAR_WXOBJECT:
	case VAR_POINTER:	
		EsException::ThrowNotSupportedForThisType();
		wxFAIL;		
	case VAR_BYTE:
		result.doAssignToEmpty((UINT8)(asByte() | v.asByte()));
		break;
	case VAR_CHAR:
		result.doAssignToEmpty((wxChar)(asChar() | v.asChar()));
		break;
	case VAR_BIN_BUFFER:
		doAndOrXorOnVariants(result, *this, v, doOr);
		break;
	case VAR_INT:
		result.doAssignToEmpty(static_cast<long>(asInternalDWord() | v.asInternalDWord()));
		break;
	case VAR_UINT:
		result.doAssignToEmpty(static_cast<unsigned long>(asInternalDWord() | v.asInternalDWord()));
		break;
	case VAR_INT64:
		result.doAssignToEmpty(static_cast<long long>(asInternalQWord() | v.asInternalQWord()));
		break;
	default: // also will throw No Value for VAR_EMPTY...
		result.doAssignToEmpty(static_cast<unsigned long long>(asInternalQWord() | v.asInternalQWord()));
		break;
	}
	return result;
}

EsVariant EsVariant::operator&(const EsVariant& v) const
{
	EsVariant result;
	switch ( (m_type > v.m_type) ? m_type : v.m_type )
	{
	case VAR_BOOL:
	case VAR_OBJECT: // assume object's zero check
		result.doAssignToEmpty((bool)(asBool() && v.asBool()));
		break;
	case VAR_WXOBJECT:
	case VAR_POINTER:	
		EsException::ThrowNotSupportedForThisType();
		wxFAIL;		
	case VAR_BYTE:
		result.doAssignToEmpty((UINT8)(asByte() & v.asByte()));
		break;
	case VAR_CHAR:
		result.doAssignToEmpty((wxChar)(asChar() & v.asChar()));
		break;
	case VAR_BIN_BUFFER:
		doAndOrXorOnVariants(result, *this, v, doAnd);
		break;
	case VAR_INT:
		result.doAssignToEmpty(static_cast<long>(asInternalDWord() & v.asInternalDWord())); // and make it back signed
		break;
	case VAR_UINT:
		result.doAssignToEmpty(static_cast<unsigned long>(asInternalDWord() & v.asInternalDWord())); // and make it back signed
		break;
	case VAR_INT64:
		result.doAssignToEmpty(static_cast<long long>(asInternalQWord() & v.asInternalQWord())); // and make it back signed
		break;
	default: // will throw No Value for VAR_EMPTY...
		result.doAssignToEmpty(static_cast<unsigned long long>(asInternalQWord() & v.asInternalQWord()));
		break;
	}
	return result;
}

EsVariant EsVariant::operator^(const EsVariant& v) const
{
	EsVariant result;
	switch ( (m_type > v.m_type) ? m_type : v.m_type )
	{
	case VAR_BOOL:
	case VAR_OBJECT: // assume object's zero check
		result.doAssignToEmpty((bool)(asBool() == v.asBool()));
		break;
	case VAR_WXOBJECT:
	case VAR_POINTER:	
		EsException::ThrowNotSupportedForThisType();
		wxFAIL;		
	case VAR_BYTE:
		result.doAssignToEmpty((UINT8)(asByte() ^ v.asByte()));
		break;
	case VAR_CHAR:
		result.doAssignToEmpty((wxChar)(asChar() ^ v.asChar()));
		break;
	case VAR_BIN_BUFFER:
		doAndOrXorOnVariants(result, *this, v, doXor);
		break;
	case VAR_INT:
		result.doAssignToEmpty(static_cast<long>(asInternalDWord() ^ v.asInternalDWord())); // and make it a signed type
		break;
	case VAR_UINT:
		result.doAssignToEmpty(static_cast<unsigned long>(asInternalDWord() ^ v.asInternalDWord())); // and make it a signed type
		break;
	case VAR_INT64:
		result.doAssignToEmpty(static_cast<long long>(asInternalQWord() ^ v.asInternalQWord())); // and make it a signed type
		break;
	default: // also will throw No Value for VAR_EMPTY...
		result.doAssignToEmpty(static_cast<unsigned long long>(asInternalQWord() ^ v.asInternalQWord()));
		break;
	}
	return result;
}

EsVariant EsVariant::operator!() const
{
	EsVariant result;
	switch ( m_type )
	{
	case VAR_EMPTY:
		result.doAssignToEmpty(true); // by convention
		break;
	case VAR_BOOL:
	case VAR_OBJECT: // assume object's zero check
	case VAR_WXOBJECT:
	case VAR_POINTER:	
		result.doAssignToEmpty((bool)!asBool());
		break;
	case VAR_INT:
		result.doAssignToEmpty(static_cast<long>(~asInternalDWord()));
		break;
	case VAR_UINT:
		result.doAssignToEmpty(static_cast<unsigned long>(~asInternalDWord()));
		break;
	case VAR_INT64:
		result.doAssignToEmpty(static_cast<long long>(~asInternalQWord()));
		break;
	case VAR_BYTE:
		result.doAssignToEmpty((UINT8)~asByte());
		break;
	case VAR_CHAR:
		result.doAssignToEmpty((wxChar)~asChar());
		break;
	default: // VAR_UINT64, all the others....
		result.doAssignToEmpty(static_cast<unsigned long long>(~asInternalDWord()));
		break;
	}
	return result;
}

EsVariant EsVariant::operator-() const
{
	EsVariant result;
	if( m_type == VAR_POINTER )
	{
		EsException::ThrowNotSupportedForThisType();
		wxFAIL;
	}
	else if( m_type != VAR_DOUBLE )
	{
		try
		{
			result.doAssignToEmpty(-asLLong());
		}
		catch ( ... ) // an overflow or an conversion error. Fall to a double
		{
			result.doAssignToEmpty(-asDouble());
		}
	}
	else
		result.doAssignToEmpty(-asDouble());
	return result;
}

static EsVariant doReturnTyped(double result, EsVariant::Type type1, EsVariant::Type type2)
{
	if ( type1 == EsVariant::VAR_BOOL || type2 == EsVariant::VAR_BOOL )
	{
		EsException::ThrowNotSupportedForThisType();
		wxFAIL;
	}
	switch ( (type1 > type2) ? type1 : type2 )
	{
	case EsVariant::VAR_BYTE:
		if ( result >= 0.0 && result <= 256.0 ) // hardcode range here, do not use defines
			return (UINT8)result;
		break;
	case EsVariant::VAR_CHAR:
		if ( result >= -128.0 && result <= 256.0 ) // hardcode range here, do not use defines
			return (wxChar)(char)result;
		break;
	case EsVariant::VAR_UINT:
		if ( result >= 0.0 && result <= (double)UINT_MAX )
			return (unsigned)result;
		break;
	case EsVariant::VAR_INT:
		if ( result >= (double)INT_MIN && result <= (double)INT_MAX )
			return (int)result;
		break;
	case EsVariant::VAR_UINT64:
		if ( result >= 0.0 && result <= (double)ULLONG_MAX )
			return (unsigned)result;
		break;
	case EsVariant::VAR_INT64:
		if ( result >= (double)LLONG_MIN && result <= (double)LLONG_MAX )
			return (int)result;
		break;
	default:
		break;
	}
	return result;
}

EsVariant EsVariant::operator+(const EsVariant& v) const
{
	switch ( (m_type > v.m_type) ? m_type : v.m_type )
	{
	default:
		break;
	case VAR_BIN_BUFFER:
		{
			EsBinBuffer self = asBinBuffer();
			EsBinBuffer other = v.asBinBuffer();
			self.reserve(self.size() + other.size());
			self.insert(self.end(), other.begin(), other.end());
			return EsVariant(self);
		}
	case VAR_STRING:
		return asString() + v.asString();
	case VAR_STRING_COLLECTION:
		{
			EsString::Array result = asStringCollection();
			EsString::Array append = v.asStringCollection();
			result.reserve(result.size()+append.size());
			result.insert(result.end(), append.begin(), append.end());
			return result;
		}
	case VAR_VARIANT_COLLECTION:
		{
			EsVariantVector result = asVariantCollection();
			EsVariantVector append = v.asVariantCollection();
			result.reserve(result.size()+append.size());
			result.insert(result.end(), append.begin(), append.end());
			return result;
		}
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:
	{
		EsReflectedClassIntf::Ptr obj = const_cast<EsVariant*>(this)->
			asExistingObject().request<EsReflectedClassIntf>();
		wxASSERT(obj);
		return obj->call(s_add, v);
	}
	case VAR_WXOBJECT:
		EsException::ThrowNotSupportedForThisType();
		wxFAIL;	
	}
	double result = asDouble() + v.asDouble();
	return doReturnTyped(result, m_type, v.m_type);
}

EsVariant& EsVariant::operator+=(const EsVariant& v)
{
	switch ( m_type )
	{
	case VAR_BIN_BUFFER:
		{
			EsBinBuffer& self = doInterpretAsBinBuffer();
			EsBinBuffer other = v.asBinBuffer();
			self.reserve(self.size() + other.size());
			self.insert(self.end(), other.begin(), other.end());
			break;
		}
	case VAR_STRING:
		doInterpretAsString() += v.asString();
		break;
	case VAR_STRING_COLLECTION:
		{
			EsString::Array& vect = doInterpretAsStringCollection();
			if( v.isCollection() )
			{
				EsString::Array vectOther = v.asStringCollection();
				vect.reserve(vect.size()+vectOther.size());
				vect.insert(vect.end(), vectOther.begin(), vectOther.end());
			}
			else
				vect.push_back(v.asString());
		}
		break;
	case VAR_VARIANT_COLLECTION:
		{
			EsVariantVector& vect = doInterpretAsVariantCollection();
			if ( v.isCollection() )
			{
				EsVariantVector vectOther = v.asVariantCollection();
				vect.reserve(vect.size()+vectOther.size());
				vect.insert(vect.end(), vectOther.begin(), vectOther.end());
			}
			else
				vect.push_back(v);
		}
		break;
	default: // Also handles NoValue condition:
		*this = *this + v;
		break;
	}
	return *this;
}

EsVariant EsVariant::operator-(const EsVariant& v) const
{
	switch( m_type )
	{
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:
	{
		EsReflectedClassIntf::Ptr obj = const_cast<EsVariant*>(this)->
			asExistingObject().request<EsReflectedClassIntf>();
		wxASSERT(obj);
		return obj->call(s_subtract, v);
	}
	case VAR_WXOBJECT:
		{
			EsException::ThrowNotSupportedForThisType();
			wxFAIL;
		}
	}
	double result = asDouble() - v.asDouble();
	return doReturnTyped(result, m_type, v.m_type);
}

// Works around a flaw in Visual C++ implementation of remove() from algorithm header.
//
template <typename Container, typename T>
void doEfficientRemoveIfPresent(Container& c, const T& v)
{
	typename Container::iterator it = std::find(c.begin(), c.end(), v);
	if ( it != c.end() )
		c.erase(it);
}

EsVariant& EsVariant::operator-=(const EsVariant& v)
{
	switch ( m_type )
	{
	case VAR_STRING_COLLECTION:
		{
			EsString::Array& vect = doInterpretAsStringCollection();
			if ( v.isCollection() )
			{
				EsString::Array vectOther = v.asStringCollection();
				EsString::Array::const_iterator it = vectOther.begin();
				EsString::Array::const_iterator itEnd = vectOther.end();
				for ( ; it != itEnd; ++it )
					doEfficientRemoveIfPresent(vect, *it);
			}
			else
				doEfficientRemoveIfPresent(vect, v.asString());
		}
		break;
	case VAR_VARIANT_COLLECTION:
		{
			EsVariantVector& vect = doInterpretAsVariantCollection();
			if ( v.isCollection() )
			{
				EsVariantVector vectOther = v.asVariantCollection();
				EsVariantVector::const_iterator it = vectOther.begin();
				EsVariantVector::const_iterator itEnd = vectOther.end();
				for ( ; it != itEnd; ++it )
					doEfficientRemoveIfPresent(vect, *it);
			}
			else
				doEfficientRemoveIfPresent(vect, v);
		}
		break;
	default: // Also handles NoValue condition:
		*this = *this - v;
		break;
	}
	return *this;
}

EsVariant EsVariant::operator*(const EsVariant& v) const
{
	switch ( (m_type > v.m_type) ? m_type : v.m_type )
	{
	default:
		break; // break to a double multiplication
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer* str;
			unsigned num;
			if ( m_type == VAR_BIN_BUFFER ) 
			{
				str = &doInterpretAsBinBuffer();
				num = v.asULong();
			}
			else
			{
				wxASSERT(v.m_type == VAR_BIN_BUFFER);
				str = &v.doInterpretAsBinBuffer();
				num = asULong();
			}
			EsBinBuffer result;
			result.reserve(str->size()*num);
			for( unsigned i = num; i > 0; --i )
				result.insert(result.end(), str->begin(), str->end());
			return EsVariant(result);
		}
	case VAR_STRING:
		{
			const EsString* str;
			unsigned num;
			if ( m_type == VAR_STRING ) 
			{
				str = &doInterpretAsString();
				num = v.asULong();
			}
			else
			{
				wxASSERT(v.m_type == VAR_STRING);
				str = &v.doInterpretAsString();
				num = asULong();
			}
			EsString result;
			result.reserve(str->size()*num);
			for( unsigned i = num; i > 0; --i )
				result += *str;
			return result;
		}
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:
	{
		EsReflectedClassIntf::Ptr obj = const_cast<EsVariant*>(this)->
			asExistingObject().request<EsReflectedClassIntf>();
		wxASSERT(obj);
		return obj->call(s_multiply, v);
	}
	case VAR_WXOBJECT:
		EsException::ThrowNotSupportedForThisType();
		wxFAIL;		
	}

	double result = asDouble() * v.asDouble();
	return doReturnTyped(result, m_type, v.m_type);
}

EsVariant EsVariant::operator/(const EsVariant& v) const
{
	switch( m_type )
	{
	case VAR_OBJECT:
	case VAR_OBJECT_EMBEDDED:
	{
		EsReflectedClassIntf::Ptr obj = const_cast<EsVariant*>(this)->
			asExistingObject().request<EsReflectedClassIntf>();
		wxASSERT(obj);
		return obj->call(s_divide, v);
	}
	case VAR_WXOBJECT:
		EsException::ThrowNotSupportedForThisType();
		wxFAIL;
		break;
	}
	
	double divisor = v.asDouble();
	if ( divisor == 0.0 )
	{
		EsException::ThrowDivisionByZero();
		wxFAIL;
	}
	double result = asDouble() / divisor;
	return doReturnTyped(result, m_type, v.m_type);
}

EsVariant EsVariant::operator%(const EsVariant& v) const
{
	if ( m_type == VAR_BOOL || v.m_type == VAR_BOOL )
	{
		EsException::ThrowNotSupportedForThisType();
		wxFAIL;
	}

	if ( m_type == VAR_DOUBLE || v.m_type == VAR_DOUBLE )
	{
		double val = v.asDouble();
		if ( val == 0.0 )
		{
			EsException::ThrowDivisionByZero();
			wxFAIL;
		}
		return fmod(asDouble(), val);
	}
	else
	{
		// this operator is assuming the positive second argument,
		// so the behavior depends only on the first arg

		long val = v.asLong();
		if ( val == 0 )
		{
			EsException::ThrowDivisionByZero();
			wxFAIL;
		}

		if ( m_type == VAR_UINT || m_type == VAR_BYTE )
			return asULong() % val;
		else
			return asLong() % val;
	}
}

EsVariant& EsVariant::operator*=(const EsVariant& v)
{
	return *this = *this * v;
}

EsVariant& EsVariant::operator/=(const EsVariant& v)
{
	return *this = *this / v;
}

EsVariant& EsVariant::operator%=(const EsVariant& v)
{
	return *this = *this % v;
}

EsVariant& EsVariant::operator>>=(const EsVariant& v)
{
	return *this = *this >> v;
}

EsVariant& EsVariant::operator<<=(const EsVariant& v)
{
	return *this = *this << v;
}

EsVariant& EsVariant::operator|=(const EsVariant& v)
{
	return *this = *this | v;
}

EsVariant& EsVariant::operator&=(const EsVariant& v)
{
	return *this = *this & v;
}

EsVariant& EsVariant::operator^=(const EsVariant& v)
{
	return *this = *this ^ v;
}

EsVariant EsVariant::operator<<(const EsVariant& v) const
{
	// this operator is assuming the positive second argument,
	// so the behavior depends only on the first arg
	unsigned long val = v.asULong();
	if( m_type == VAR_UINT || m_type == VAR_UINT64 || m_type == VAR_BYTE )
		return asULLong() << val;
	else
		return asLLong() << val;
}

EsVariant EsVariant::operator>>(const EsVariant& v) const
{
	// this operator is assuming the positive second argument,
	// so the behavior depends only on the first arg
	unsigned long val = v.asULong();
	if ( m_type == VAR_UINT || m_type == VAR_UINT64 || m_type == VAR_BYTE )
		return asULLong() >> val;
	else
		return asLLong() >> val;
}

const wxChar OPERATOR_AUTOINCREMENT_STRING[] = wxT("++");

EsVariant& EsVariant::operator++()
{
	switch ( m_type )
	{
	case VAR_EMPTY:
		EsException::ThrowNoValue();
		wxFAIL;
		// no need to break here
	case VAR_BOOL:
		wxASSERT(m_value.m_ullong <= 1);
		if ( m_value.m_ullong != 0 )
		{
			EsException::ThrowOverflowInOperation(OPERATOR_AUTOINCREMENT_STRING);
			wxFAIL;
		}
		++m_value.m_ullong;
		break;
	case VAR_BYTE:
		if ( m_value.m_ullong >= UCHAR_MAX )
		{
			EsException::ThrowOverflowInOperation(OPERATOR_AUTOINCREMENT_STRING);
			wxFAIL;
		}
		++m_value.m_ullong;
		break;
	case VAR_CHAR:
		if ( m_value.m_ullong >= s_WXCHAR_MAX )
		{
			EsException::ThrowOverflowInOperation(OPERATOR_AUTOINCREMENT_STRING);
			wxFAIL;
		}
		++m_value.m_ullong;
		break;
	case VAR_INT:
		if ( m_value.m_llong == INT_MAX )
		{
			EsException::ThrowOverflowInOperation(OPERATOR_AUTOINCREMENT_STRING);
			wxFAIL;
		}
		++m_value.m_llong;
		break;
	case VAR_UINT:
		if ( m_value.m_ullong == UINT_MAX )
		{
			EsException::ThrowOverflowInOperation(OPERATOR_AUTOINCREMENT_STRING);
			wxFAIL;
		}
		++m_value.m_ullong;
		break;
	case VAR_INT64:
		if ( m_value.m_llong == LLONG_MAX )
		{
			EsException::ThrowOverflowInOperation(OPERATOR_AUTOINCREMENT_STRING);
			wxFAIL;
		}
		++m_value.m_llong;
		break;
	case VAR_UINT64:
		if ( m_value.m_ullong == ULLONG_MAX )
		{
			EsException::ThrowOverflowInOperation(OPERATOR_AUTOINCREMENT_STRING);
			wxFAIL;
		}
		++m_value.m_ullong;
		break;
	case VAR_DOUBLE:
		++m_value.m_double;
		break;
	default: // all the others....
		EsException::ThrowNotSupportedForThisType();
		wxFAIL;
	}
	return *this;
}

const wxChar OPERATOR_AUTODECREMENT_STRING[] = wxT("--");

EsVariant& EsVariant::operator--()
{
	switch ( m_type )
	{
	case VAR_EMPTY:
		EsException::ThrowNoValue();
		wxFAIL;
		// no need to break here
	case VAR_BOOL:
		wxASSERT(m_value.m_ullong <= 1);
	case VAR_BYTE:
	case VAR_CHAR:
	case VAR_UINT:
	case VAR_UINT64:
		if ( m_value.m_ullong == 0 )
		{
			EsException::ThrowUnderflowInOperation(OPERATOR_AUTODECREMENT_STRING);
			wxFAIL;
		}
		--m_value.m_ullong;
		break;
	case VAR_INT:
		if ( m_value.m_llong == INT_MIN )
		{
			EsException::ThrowUnderflowInOperation(OPERATOR_AUTODECREMENT_STRING);
			wxFAIL;
		}
		--m_value.m_llong;
		break;
	case VAR_INT64:
		if ( m_value.m_llong == LLONG_MIN )
		{
			EsException::ThrowUnderflowInOperation(OPERATOR_AUTODECREMENT_STRING);
			wxFAIL;
		}
		--m_value.m_llong;
		break;
	case VAR_DOUBLE:
		--m_value.m_double;
		break;
	default: // all the others....
		EsException::ThrowNotSupportedForThisType();
		wxFAIL;
	}
	return *this;
}

EsVariant& EsVariant::doSetInt(long long value, Type type) ES_NOTHROW
{
	doSetType(type);
	m_value.m_llong = value;
	return *this;
}

bool EsVariant::isIn(const EsVariant& v) const
{
	if( v.isCollection() ) // case when the parameter is collection
	{
		int count = v.getCount();
		for( int i = 0; i < count; ++i )
			if( !isIn(v.getItem(i)) ) // recursive call for each item in the parameter
				return false;
		return true;
	}

	if( isCollection() ) // if this is a collection
	{
		int count = getCount();
		for( int i = 0; i < count; ++i )
			if( getItem(i) == v )
				return true;
		return false;
	}

	return v == *this;
}

// variant-based string formatter. fmt parameter must contain string variant
// with printf-compatible formatting control codes. 
// args should be collection of variants with values to format.
// returned is variant containing resulting string. exceptions may be thrown if
// arguments count|types do not match the formatting field count|field types
enum {
	fmtField = 1,
	fmtWidth,
	fmtPrecision,
	fmtType
};

static inline bool typeIsInt( EsString::value_type type )
{
	return type == wxT('d') || type == wxT('i');
}

static inline bool typeIsUint( EsString::value_type type )
{
	return type == wxT('o') || type == wxT('u') ||
		type == wxT('x') || type == wxT('X');
}

static inline bool typeIsFloat( EsString::value_type type )
{
	return type == wxT('a') || type == wxT('A') ||
		type == wxT('e') || type == wxT('E') ||
		type == wxT('g') || type == wxT('G') || type == wxT('f');
}

static inline bool typeIsChar( EsString::value_type type )
{
	return type == wxT('c');
}

static inline bool typeIsString( EsString::value_type type )
{
	return type == wxT('s');
}

static inline EsString::const_pointer throwUnknownTypeSpecifier( EsString::value_type type )
{
	EsException::Throw(0, EsException::Generic, wxT("Unsupported format type specifier: '%c'"), type);
	return 0;
}

EsVariant EsVariant::format(const EsVariant& fmt, const EsVariant& args, const std::locale& loc /*= std::locale()*/)
{
	static wxRegEx s_printfRegex(wxT("(%[-+0 #]?([0-9]+|\\*)?(\\.(?:[0-9]+|\\*))?([cdiouxXaAeEgGfs]))"), wxRE_ADVANCED);
	wxASSERT(s_printfRegex.IsValid());
	EsString result;
	if( fmt.getType() != VAR_STRING )
		EsException::Throw(0, EsException::Generic, wxT("Variant containing string expected as format control argument"));
		
	// find format fields in format control string
	EsString fmtStr = fmt.asString();
	EsString::const_pointer pos = fmtStr.c_str();
	EsString::const_pointer end = pos+fmtStr.size();
	size_t argIdx = 0;
	while( s_printfRegex.Matches( pos, 0, end-pos ) )
	{
		// optional width and precision placeholders for * case
		EsVariant width;
		EsVariant precision;
		size_t matchStart, matchLen;
		if( s_printfRegex.GetMatch(&matchStart, &matchLen, fmtWidth) )
		{
			// check if width is *, so we have to extract argument and convert it to integer
			if( *(pos+matchStart) == wxT('*') )
				width = args.getItem(argIdx++);
		}

		if( s_printfRegex.GetMatch(&matchStart, &matchLen, fmtPrecision) )
		{
			// check if width is *, so we have to extract argument and convert it to integer
			if( *(pos+matchStart) == wxT('*') )
				precision = args.getItem(argIdx++);
		}
		
		// format the entire field, append formatted string to result and skip further
		EsString::const_pointer tmpPos = pos;
		bool ok = s_printfRegex.GetMatch(&matchStart, &matchLen, fmtField);
		wxASSERT(ok);
		if( matchStart != 0 )
			// append string prefix to the result, moving position after the field end
			result.append(pos, pos+matchStart);
		pos += matchStart;
		EsString fmtFld(pos, pos+matchLen);
		pos += matchLen;
		
		// extract field type		
		ok = s_printfRegex.GetMatch(&matchStart, &matchLen, fmtType);
		wxASSERT(ok);
		EsString::value_type type = *(tmpPos+matchStart);
		EsVariant val = args.getItem(argIdx++);
	
		// format field, using optional width|precision specifiers
		if( !width.isEmpty() && precision.isEmpty() )
		{
			// only width wildcard is set
			result += 
				(typeIsInt(type) ? EsString::format(loc, fmtFld.c_str(), width.asInt(), val.asLong()) : 
					(typeIsUint(type) ? EsString::format(loc, fmtFld.c_str(), width.asInt(), val.asULong()) :
						(typeIsFloat(type) ? EsString::format(loc, fmtFld.c_str(), width.asInt(), val.asDouble()) :
							(typeIsChar(type) ? EsString::format(loc, fmtFld.c_str(), width.asInt(), val.asChar()) : 
								(typeIsString(type) ? EsString::format(loc, fmtFld.c_str(), width.asInt(), val.asString().c_str()) : 
									throwUnknownTypeSpecifier(type)
								)
							)
						)
					)
				);
		}
		else if( !precision.isEmpty() && width.isEmpty() )
		{
			// only precision wildcard is set	
			result += 				
				(typeIsInt(type) ? EsString::format(loc, fmtFld.c_str(), precision.asInt(), val.asLong()) : 
					(typeIsUint(type) ? EsString::format(loc, fmtFld.c_str(), precision.asInt(), val.asULong()) :
						(typeIsFloat(type) ? EsString::format(loc, fmtFld.c_str(), precision.asInt(), val.asDouble()) :
							(typeIsChar(type) ? EsString::format(loc, fmtFld.c_str(), precision.asInt(), val.asChar()) : 
								(typeIsString(type) ? EsString::format(loc, fmtFld.c_str(), precision.asInt(), val.asString().c_str()) : 
									throwUnknownTypeSpecifier(type)
								)
							)
						)
					) 
				);
		}
		else if( !width.isEmpty() && !precision.isEmpty() )
		{
			// both width and precision wildcard is set
			result += 
				(typeIsInt(type) ? EsString::format(loc, fmtFld.c_str(), width.asInt(), precision.asInt(), val.asLong()) : 
					(typeIsUint(type) ? EsString::format(loc, fmtFld.c_str(), width.asInt(), precision.asInt(), val.asULong()) :
						(typeIsFloat(type) ? EsString::format(loc, fmtFld.c_str(), width.asInt(), precision.asInt(), val.asDouble()) :
							(typeIsChar(type) ? EsString::format(loc, fmtFld.c_str(), width.asInt(), precision.asInt(), val.asChar()) : 
								(typeIsString(type) ? EsString::format(loc, fmtFld.c_str(), width.asInt(), precision.asInt(), val.asString().c_str()) : 
									throwUnknownTypeSpecifier(type)
								)
							)
						)
					) 
				);
		}
		else
		{
			// neither width nor precision wildcard is set
			result += 
				(typeIsInt(type) ? EsString::format(loc, fmtFld.c_str(), val.asLong()) : 
					(typeIsUint(type) ? EsString::format(loc, fmtFld.c_str(), val.asULong()) :
						(typeIsFloat(type) ? EsString::format(loc, fmtFld.c_str(), val.asDouble()) :
							(typeIsChar(type) ? EsString::format(loc, fmtFld.c_str(), val.asChar()) : 
								(typeIsString(type) ? EsString::format(loc, fmtFld.c_str(), val.asString().c_str()) : 
									throwUnknownTypeSpecifier(type)
								)
							)
						)
					) 
				);
		}
	}
	// add the rest of the unmatched string to the result and return
	result.append(pos, end);
	
	return result;
}
