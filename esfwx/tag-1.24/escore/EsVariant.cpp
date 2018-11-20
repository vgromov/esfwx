#include "escorepch.h"
#pragma hdrstop

#include "EsVariant.h"
#include <algorithm>
#include <functional>
#include <sstream>

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// reserve defVarCollectionSize for empty collection
#define defVarCollectionSize 16

#ifndef UNICODE
const int sc_ES_CHAR_MIN = std::numeric_limits<signed char>::min(); // cover both signed and unsigned char, -127 ..
const int sc_ES_CHAR_MAX = std::numeric_limits<unsigned char>::max(); // cover both signed and unsigned char, .. 255
#else
const int sc_ES_CHAR_MIN = std::numeric_limits<short>::min(); // cover both signed and unsigned
const int sc_ES_CHAR_MAX = std::numeric_limits<unsigned short>::max();
#endif
//--------------------------------------------------------------------------------

const EsVariant& EsVariant::null() ES_NOTHROW
{
  static const EsVariant sc_null;

  return sc_null;
}
//--------------------------------------------------------------------------------

const EsVariant::Array& EsVariant::nullCollection() ES_NOTHROW
{
  static const EsVariant::Array sc_nullColl;

  return sc_nullColl;
}
//--------------------------------------------------------------------------------

const EsVariant& EsVariant::nullObject() ES_NOTHROW
{
  static const EsVariant sc_nullObject = EsBaseIntf::Ptr();

  return sc_nullObject;
}
//--------------------------------------------------------------------------------

EsString EsVariant::dump(const EsVariant& v)
{
  if(v.isObject())
  {
    EsBaseIntfPtr obj = v.asObject();
    if(obj)
      return EsString::format(
        esT("object '%s'"),
        obj->typeNameGet()
      );
    else
      return esT("null object");
  }
  else if(v.isCollection())
  {
    EsString str;
    int cnt = v.countGet();
    for(int idx = 0; idx < cnt; ++idx)
      str += EsString::format(
        esT("[%d]='%s';"),
        idx,
        dump(
          v[idx]
        )
      );

    return str;
  }
  else if(!v.isEmpty())
    return v.asString();
  else
    return esT("null");
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsVariant::EsVariant() ES_NOTHROW :
m_type(VAR_EMPTY)
{
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(Type type) :
m_type(VAR_EMPTY)
{
  setToNull(type);
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(bool n) ES_NOTHROW :
m_type(VAR_BOOL)
{
  m_value.m_llong = n ? 1 : 0;
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(long n) ES_NOTHROW :
m_type(VAR_INT)
{
  m_value.m_llong = n;
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(long long n) ES_NOTHROW :
m_type(VAR_INT64)
{
  m_value.m_llong = n;
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(unsigned long n) ES_NOTHROW :
m_type(VAR_UINT)
{
  m_value.m_ullong = n;
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(unsigned long long n) ES_NOTHROW :
m_type(VAR_UINT64)
{
  m_value.m_ullong = n;
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(int n) ES_NOTHROW :
m_type(VAR_INT)
{
  m_value.m_llong = n;
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(unsigned n)	ES_NOTHROW :
m_type(VAR_UINT)
{
  m_value.m_ullong = n;
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(esU8 b) ES_NOTHROW:
m_type(VAR_BYTE)
{
  m_value.m_ullong = static_cast<unsigned long long>(b);
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(EsString::value_type c, AcceptCharType) ES_NOTHROW :
m_type(VAR_CHAR)
{
#if defined(ES_USE_WCHAR)
# if 2 == ES_WCHAR_SIZE
		m_value.m_ullong = static_cast<unsigned long long>( static_cast<unsigned short>(c) );
# elif 4 == ES_WCHAR_SIZE
    m_value.m_ullong = static_cast<unsigned long long>( static_cast<unsigned long>(c) );
# else
#   error Unsupported|unknown ES_WCHAR_SIZE!
# endif
#else
		m_value.m_ullong = static_cast<unsigned long long>( static_cast<unsigned char>(c) );
#endif
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(double f)	ES_NOTHROW :
m_type(VAR_DOUBLE)
{
  m_value.m_double = f;
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(EsString::const_pointer p, EsVariant::AcceptStringType) :
m_type(VAR_STRING)
{
  new((void*)&m_value) EsString(p);
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(EsString::const_pointer p, unsigned len, EsVariant::AcceptStringType) :
m_type(VAR_STRING)
{
  new((void*)&m_value) EsString(p, len);
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(const esU8* p, unsigned len)	:
m_type(VAR_BIN_BUFFER)
{
  new((void*)&m_value) EsBinBuffer(p, p+len);
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(const EsBinBuffer& s) :
m_type(VAR_BIN_BUFFER)
{
  new((void*)&m_value) EsBinBuffer(s);
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(const EsString& s) :
m_type(VAR_STRING)
{
  new((void*)&m_value) EsString(s);
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(const EsString::Array& v) :
m_type(VAR_STRING_COLLECTION)
{
  new((void*)&m_value) EsString::Array(v);
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(const EsVariant::Array& v)	:
m_type(VAR_VARIANT_COLLECTION)
{
  new((void*)&m_value) EsVariant::Array(v);
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(void* ptr, AcceptPointerType) ES_NOTHROW :
m_type(VAR_POINTER)
{
  m_value.m_pointer = ptr;
}
//---------------------------------------------------------------------------

EsVariant::EsVariant(const EsVariant& v) :
m_type(VAR_EMPTY)
{
  doAssignToEmpty(v);
}
//---------------------------------------------------------------------------

EsVariant::~EsVariant() ES_NOTHROW
{
  doCleanup();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifdef ES_MODERN_CPP
EsVariant::EsVariant(EsVariant&& other) ES_NOTHROW :
m_type(VAR_EMPTY)
{
  ES_ASSERT(this != &other);
  internalMove(other);
}
//---------------------------------------------------------------------------

EsVariant& EsVariant::operator=(EsVariant&& other) ES_NOTHROW
{
  move(other);
  return *this;
}
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsBaseIntf::Ptr EsVariant::asExistingObject() const
{
  return const_cast<EsVariant*>(this)->asExistingObject();
}
//---------------------------------------------------------------------------

bool EsVariant::isObject() const ES_NOTHROW
{
  return m_type == VAR_OBJECT;
}
//---------------------------------------------------------------------------

bool EsVariant::isEmpty() const ES_NOTHROW
{
  return m_type == VAR_EMPTY ||
    (	m_type == VAR_OBJECT && m_value.m_intf.m_ptr == nullptr );
}
//---------------------------------------------------------------------------

void EsVariant::releaseObject() ES_NOTHROW
{
	ES_ASSERT(VAR_OBJECT == m_type);
	if( m_value.m_intf.m_ptr && m_value.m_intf.m_own )
		m_value.m_intf.m_ptr->decRef();
	m_value.m_intf.m_ptr = 0;
}
//---------------------------------------------------------------------------

void EsVariant::doCleanup() ES_NOTHROW
{
	switch( m_type )
	{
	case VAR_STRING:
		doInterpretAsString().~EsString();
		break;
	case VAR_BIN_BUFFER:
		doInterpretAsBinBuffer().~EsBinBuffer();
		break;
	case VAR_STRING_COLLECTION:
		doInterpretAsStringCollection().~EsStringArray();
		break;
	case VAR_VARIANT_COLLECTION:
		doInterpretAsVariantCollection().~EsVariantArray();
		break;
	case VAR_OBJECT:
		releaseObject();
		break;
	default:
		break;
	}
}
//---------------------------------------------------------------------------

void EsVariant::setEmpty() ES_NOTHROW
{
	doSetType(VAR_EMPTY);
}
//---------------------------------------------------------------------------

void EsVariant::setToNull(Type type)
{
	doSetType(type == (Type)-1 ? m_type : type); // this will always clear the data
	switch( m_type )
	{
	case VAR_EMPTY:
		break;
	case VAR_STRING:
		new(m_value.m_string) EsString;
		break;
	case VAR_BIN_BUFFER:
		new(m_value.m_binBuffer) EsBinBuffer;
		break;
	case VAR_STRING_COLLECTION:
		new(m_value.m_stringCollection) EsStringArray;
		break;
	case VAR_DOUBLE:
		m_value.m_double = 0.0;
		break;
	case VAR_OBJECT:
		releaseObject();
		break;
	case VAR_VARIANT_COLLECTION:
		new(m_value.m_variantCollection) EsVariantArray;
		doInterpretAsVariantCollection().reserve(defVarCollectionSize);
		break;
	default:
		m_value.m_llong = 0; // this surely nullifies all the value types
		break;
	}
}
//---------------------------------------------------------------------------

void EsVariant::setEmptyWithObjectDelete()
{
	switch( m_type )
	{
	case VAR_OBJECT:
		releaseObject();
		m_type = VAR_EMPTY;
		break;
	case VAR_VARIANT_COLLECTION:
		{
			EsVariant::Array& vars = doInterpretAsVariantCollection();
			EsVariant::Array::iterator it = vars.begin();
			EsVariant::Array::iterator itEnd = vars.end();
			for ( ; it != itEnd; ++it )
				(*it).setEmptyWithObjectDelete();
			m_type = VAR_EMPTY;
			break;
		}
	default:
		setEmpty();
	}
}
//---------------------------------------------------------------------------

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
		{
			const EsReflectedClassIntf::Ptr obj = asObject();
			if( obj )
				return obj->isIndexed();
		}
	}
	return false;
}

int EsVariant::countGet() const
{
	switch( m_type )
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
		{
			EsReflectedClassIntf::Ptr obj = asExistingObject();
			ES_ASSERT(obj);
			return obj->call(EsStdNames::countGet()).asInt();
		}
	default:
		EsException::ThrowCannotIndexItem();
		ES_FAIL;
	}
	return 0; // we are never here
}

void EsVariant::countSet(int newCount)
{
	switch( m_type )
	{
	case VAR_BIN_BUFFER:
		doInterpretAsBinBuffer().resize(newCount);
		break;
	case VAR_STRING:
		doInterpretAsString().resize(newCount);
		break;
	case VAR_STRING_COLLECTION:
		doInterpretAsStringCollection().resize(newCount);
		break;
	case VAR_VARIANT_COLLECTION:
		doInterpretAsVariantCollection().resize(newCount);
		break;
	case VAR_OBJECT:
		{
			EsReflectedClassIntf::Ptr obj = asExistingObject();
			ES_ASSERT(obj);
			obj->call(EsStdNames::countSet(), newCount);
		}
		break;
	default:
		EsException::ThrowCannotIndexItem();
		ES_FAIL;
	}
}

EsVariant& EsVariant::operator=(double f)
{
	doSetType(VAR_DOUBLE);
	m_value.m_double = f;
	return *this;
}

EsVariant& EsVariant::operator=(const EsString::value_type* s)
{
	doSetType(VAR_STRING);
	new((void*)&m_value) EsString(s);
	return *this;
}

EsVariant& EsVariant::operator=(const EsString& s)
{
	if( m_type == VAR_STRING )
		doInterpretAsString() = s;
	else
	{
		doSetType(VAR_STRING); // have to call DoSetType ...
		new((void*)&m_value) EsString(s);
	}
	return *this;
}

EsVariant& EsVariant::operator=(const EsString::Array& c)
{
	if( m_type == VAR_STRING_COLLECTION )
		doInterpretAsStringCollection() = c;
	else
	{
		doSetType(VAR_STRING_COLLECTION); // have to call DoSetType ...
		new((void*)&m_value) EsString::Array(c);
	}
	return *this;
}

EsVariant& EsVariant::operator=(const EsVariant::Array& c)
{
	if( m_type == VAR_VARIANT_COLLECTION )
		doInterpretAsVariantCollection() = c;
	else
	{
		doSetType(VAR_VARIANT_COLLECTION); // have to call DoSetType ...
		new((void*)&m_value) EsVariant::Array(c);
	}
	return *this;
}

EsVariant& EsVariant::operator=(const EsBaseIntf::Ptr& obj)
{
	doSetType(VAR_OBJECT);
	m_value.m_intf.m_ptr = obj.get();
	m_value.m_intf.m_own = obj.own();
	if( m_value.m_intf.m_ptr && m_value.m_intf.m_own )
		m_value.m_intf.m_ptr->incRef();
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
	switch( other.m_type )
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

void EsVariant::assign(const esU8* v, size_t len)
{
	if( m_type == VAR_BIN_BUFFER )
		doInterpretAsBinBuffer().assign(v, v+len);
	else
	{
		doSetType(VAR_BIN_BUFFER); // have to call doSetType
		new((void*)&m_value) EsBinBuffer(
      v,
      v+len
    );
	}
}

void EsVariant::assignString(const EsString::value_type* v, size_t len)
{
	if( m_type == VAR_STRING )
		doInterpretAsString().assign(v, len);
	else
	{
		doSetType(VAR_STRING); // have to call DoSetType
		new((void*)&m_value) EsString(
      v,
      len
    );
	}
}

bool EsVariant::asBool(const std::locale& loc /*= EsLocale::locale()*/) const
{
	switch( m_type )
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
			if( buff.empty() || std::equal(buff.begin(), buff.end(), EsStdNames::_bfalse().begin()) )
				return false;
			return !(buff.size() == 1 && (buff[0] == '\0' || buff[0] == '0'));
		}
	case VAR_STRING:
		{  // Perl-like convention
			const EsString& str = doInterpretAsString();
			if( str.empty() || EsStdNames::_false() == str ) // script convention
				return false;
			return !(str.size() == 1 && (str[0] == esT('\0') || str[0] == esT('0')));
		}
	case VAR_STRING_COLLECTION:
		return !doInterpretAsStringCollection().empty();  // convention
	case VAR_OBJECT:
		return m_value.m_intf.m_ptr != nullptr;  // convention
	case VAR_POINTER:
		return m_value.m_pointer != nullptr;  // convention
	case VAR_VARIANT_COLLECTION:
		return !doInterpretAsVariantCollection().empty();  // convention
	default:
		;
	}
	ES_ASSERT(m_type == VAR_EMPTY);
	return false; // by convention, VAR_EMPTY.asBool is false
}

static EsVariant doGetClientValueIfPresent(const EsVariant* self, const EsString& str)
{
  if( !self->isObject() )
  {
    EsException::Throw(
      esT("Non-Object LHS: %s"),
      str
    );
		ES_FAIL;
	}

	EsReflectedClassIntf::Ptr obj = self->asObject();
	if( obj && obj->hasProperty(EsStdNames::value()) )
		return obj->propertyGet(EsStdNames::value());

  if( !obj )
	  EsException::Throw(
      esT("NULL Object LHS: %s"),
      str
    );
  else
	  EsException::Throw(
      esT("LHS Object of type '%s' with no $value property: %s"),
      obj->typeNameGet(),
      str
    );

	ES_FAIL; //< We're never here
	return EsVariant::null();
}

EsString::value_type EsVariant::asChar(const std::locale& loc/* = EsLocale::locale()*/) const
{
	switch( m_type )
	{
	case VAR_BYTE:
	case VAR_BOOL: // by convention...
	case VAR_CHAR:
		return (EsString::value_type)m_value.m_ullong;
	case VAR_INT:
	case VAR_INT64:
	case VAR_UINT:
	case VAR_UINT64:
		{
			long long val = m_value.m_llong;
			if( val < sc_ES_CHAR_MIN || val > sc_ES_CHAR_MAX ) // cover both signed and unsigned
			{
				EsString str = EsString::fromInt64(val, 10, loc);
				EsException::Throw(
          esT("Could not convert '%s' to a single character"),
          str
        );
			}

			return (EsString::value_type)val;
		}
	case VAR_DOUBLE:
		{
			double val = m_value.m_double;
			if( val < double(sc_ES_CHAR_MIN) || val > double(sc_ES_CHAR_MAX) ) // cover both signed and unsigned
			{
				EsString str = EsString::fromDouble(val, loc);
				EsException::Throw(
          esT("Could not convert '%s' to a single character"),
          str
        );
			}

			return EsUtilities::roundTo<EsString::value_type>(val);
		}
	case VAR_BIN_BUFFER:
		{
			EsString str;
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if( !buff.empty() && buff.size() < 5 )
			{
				EsByteString bs(buff.begin(), buff.end());
				str = EsString::fromUtf8(bs);
			}

			if( str.size() != 1 )
				EsException::Throw(
          esT("Could not convert binary buffer %d bytes long to a single character"),
          buff.size()
        );

			return str[0];
		}
	case VAR_STRING:
		{
			const EsString& s = doInterpretAsString();
			if( s.size() != 1 )
				EsException::Throw(
          esT("Could not convert '%s' to a single character"),
          s
        );

			return s[0];
		}
	case VAR_POINTER:
	case VAR_OBJECT:
	case VAR_STRING_COLLECTION:
	case VAR_VARIANT_COLLECTION:
		return doGetClientValueIfPresent(
      this,
			EsString::format(
        esT("Could not convert variant of type '%s' to a single character"),
        EsEnumSymbolGet<EsVariantType>(m_type)
      )
    ).asChar(loc);
	default:
		;
	}
	ES_ASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	ES_FAIL;
	return esT('\0'); // we are never here, pacify compilers in debug mode
}

esU8 EsVariant::asByte(const std::locale& loc /*= EsLocale::locale()*/) const
{
	switch( m_type )
	{
	case VAR_CHAR:
#if defined(ES_UNICODE) && defined(ES_USE_WCHAR)
		{
			EsString::value_type c = (EsString::value_type)m_value.m_ullong;
			if( c > 255 )
				EsException::Throw(
          esT("Could not convert %d to a byte"),
          m_value.m_ullong
        );

			return (esU8)c;
		}
#else
		// otherwise fall through to VAR_CHAR conversion
#endif
	case VAR_BOOL: // by convention...
	case VAR_BYTE:
		ES_ASSERT(m_value.m_ullong < 256);
		return (esU8)m_value.m_ullong;
	case VAR_INT:
	case VAR_UINT:
	case VAR_INT64:
	case VAR_UINT64:
		{
			EsNumericCheck::checkRangeLLInteger(0, 255, m_value.m_llong);
			return (esU8)m_value.m_llong;
		}
	case VAR_DOUBLE:
		EsNumericCheck::checkRangeFloat(0.0, 255.0, m_value.m_double);
		return EsUtilities::roundTo<esU8>(m_value.m_double);
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if( buff.size() != 1 )
				EsException::Throw(
          esT("Could not convert binary buffer %d bytes long to a byte"),
          buff.size()
        );

			return buff[0];
		}
	case VAR_STRING:
		{
			const EsString& str = doInterpretAsString();
			unsigned long val = EsString::toULong(str, 0, loc);
			if( val < 256 )
				return static_cast<esU8>(val);
			else
				EsException::Throw(
          esT("Could not convert '%s' to a byte"),
          str
        );

			return static_cast<esU8>(str[0]);
		}
	case VAR_POINTER:
	case VAR_OBJECT:
	case VAR_STRING_COLLECTION:
	case VAR_VARIANT_COLLECTION:
		return doGetClientValueIfPresent(
      this,
      EsString::format(
        esT("Could not convert variant of type '%s' to a byte"),
        EsEnumSymbolGet<EsVariantType>(m_type)
      )
    ).asByte(loc);
	default:
		;
	}
	ES_ASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	ES_FAIL;
	return (esU8)'\0'; // we are never here, pacify compilers in debug mode
}

esU32 EsVariant::asInternalDWord(const std::locale& loc /*= EsLocale::locale()*/) const
{
	switch( m_type )
	{
	case VAR_POINTER:
		if( sizeof(esU32) != sizeof(void*) )
			EsException::ThrowNotSupportedForThisType();
	case VAR_BOOL:
		ES_ASSERT(m_value.m_ullong <= 1);
	case VAR_BYTE:
	case VAR_CHAR:
#ifdef UNICODE
		ES_ASSERT((m_value.m_ullong & ~0xFFFF) == 0);
#endif
		ES_ASSERT((m_value.m_ullong & ~0xFF) == 0); // this also tests the above VAR_BYTE case
	case VAR_INT:
	case VAR_UINT:
		return (esU32)m_value.m_ullong;
	case VAR_DOUBLE:
		{
			double val = EsUtilities::round0(m_value.m_double);
			if( val < 0.0 )
			{
				if( val < std::numeric_limits<long>::min() )
				{
					EsException::Throw(
            _("Could not convert '%f.0' to double word"),
            val
          );
					ES_FAIL;
				}
				return (unsigned)(long)val; // not (unsigned long)!
			}
			if( val > std::numeric_limits<unsigned long>::max() )
			{
				EsException::Throw(
          _("Could not convert '%f.0' to double word"),
          val
        );
				ES_FAIL;
			}
			return unsigned(val);
		}
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if( buff.empty() || buff.size() > 4 )
			{
				EsException::Throw(
					esT("Could not convert binary buffer %d bytes long to double word"),
          buff.size()
        );
				ES_FAIL;
			}
			esU32 val = 0;
			memcpy(&val, &buff[0], buff.size());
			return val;
		}
	case VAR_STRING:
		{
			const EsString& str = doInterpretAsString();
			unsigned long val = EsString::toULong(str, 0, loc);
			return static_cast<esU32>(val);
		}
	case VAR_OBJECT:
	case VAR_STRING_COLLECTION:
	case VAR_VARIANT_COLLECTION:
		return doGetClientValueIfPresent(
      this,
    	EsString::format(
        esT("Could not convert variant of type '%s' to double word"),
        EsEnumSymbolGet<EsVariantType>(m_type)
      )
    ).asInternalDWord();
	default:
		;
	}
	ES_ASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	ES_FAIL;
	return 0; // we are never here, pacify compilers in debug mode
}

esU64 EsVariant::asInternalQWord(const std::locale& loc /*= EsLocale::locale()*/) const
{
	switch( m_type )
	{
	case VAR_POINTER:
		if(sizeof(unsigned long long) != sizeof(void*))
			EsException::ThrowNotSupportedForThisType();
	case VAR_BOOL:
		ES_ASSERT(m_value.m_ullong <= 1);
	case VAR_BYTE:
	case VAR_CHAR:
#ifdef UNICODE
		ES_ASSERT((m_value.m_ullong & ~0xFFFF) == 0);
#endif
		ES_ASSERT((m_value.m_ullong & ~0xFF) == 0); // this also tests the above VAR_BYTE case
	case VAR_INT:
	case VAR_UINT:
	case VAR_INT64:
	case VAR_UINT64:
		return m_value.m_ullong;
	case VAR_DOUBLE:
		{
			double val = EsUtilities::round0(m_value.m_double);
			if( val < 0.0 )
			{
				if( val < static_cast<double>(std::numeric_limits<long long>::min()) )
				{
					EsException::Throw(
            esT("Could not convert '%f.0' to quad word"),
            val
          );
					ES_FAIL;
				}
				return static_cast<unsigned long long>(static_cast<long long>(val)); // not (unsigned long long)!
			}
			if( val > static_cast<double>(std::numeric_limits<unsigned long long>::max()) )
			{
				EsException::Throw(
          esT("Could not convert '%f.0' to quad word"),
          val
        );
				ES_FAIL;
			}
			return static_cast<unsigned long long>(val);
		}
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if( buff.empty() || buff.size() > 8 )
			{
				EsException::Throw(
					esT("Could not convert binary buffer %d bytes long to quad word"),
          buff.size()
        );
				ES_FAIL;
			}
			esU64 val = 0;
			memcpy(&val, &buff[0], buff.size());
			return val;
		}
	case VAR_STRING:
		{
			const EsString& str = doInterpretAsString();
			unsigned long long val = EsString::toUInt64(str, 0, loc);
			return val;
		}
	case VAR_OBJECT:
	case VAR_STRING_COLLECTION:
	case VAR_VARIANT_COLLECTION:
		return doGetClientValueIfPresent(
      this,
      EsString::format(
        esT("Could not convert variant of type '%s' to quad word"),
        EsEnumSymbolGet<EsVariantType>(m_type)
      )
    ).asInternalDWord();
	default:
		;
	}
	ES_ASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	ES_FAIL;
	return 0; // we are never here, pacify compilers in debug mode
}

long EsVariant::asLong(const std::locale& loc /*= EsLocale::locale()*/) const
{
	switch( m_type )
	{
	case VAR_BOOL:
		ES_ASSERT(m_value.m_ullong <= 1);
	case VAR_INT64:
		ES_ASSERT( m_value.m_llong >= std::numeric_limits<long>::min() && m_value.m_llong <= std::numeric_limits<long>::max() );
	case VAR_BYTE:
	case VAR_CHAR:
	case VAR_INT:
		return (long)m_value.m_llong;
	case VAR_UINT:
	case VAR_UINT64:
		if( m_value.m_ullong > std::numeric_limits<long>::max() ) // unsigned integer bigger than maximum long
		{
			const EsString str = EsString::fromUInt64(m_value.m_ullong, 10, loc);
			EsException::Throw(
        _("Could not convert '%s' to signed integer"),
        str
      );

			ES_FAIL;
		}
		return static_cast<long>(m_value.m_llong);
	case VAR_DOUBLE:
		{
			double val = EsUtilities::round0(m_value.m_double);
			if(
			  val < static_cast<double>(std::numeric_limits<long>::min()) ||
			  val > static_cast<double>(std::numeric_limits<long>::max())
      )
			{
				EsException::Throw(
          _("Could not convert '%.0f' to signed integer"),
          val
        );
				ES_FAIL;
			}
			return static_cast<long>(val);
		}
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if( buff.empty() || buff.size() > 4 )
			{
				EsException::Throw(
					esT("Could not convert binary buffer %d bytes long to signed integer"),
          buff.size()
        );
				ES_FAIL;
			}
			long val = 0;
			memcpy(&val, &buff[0], buff.size());
			return val;
		}
	case VAR_STRING:
	{
		const EsString& str = doInterpretAsString();
		return EsString::toLong(str, 0, loc);
	}
	case VAR_OBJECT:
	case VAR_VARIANT_COLLECTION:
	case VAR_STRING_COLLECTION:
		return doGetClientValueIfPresent(
      this,
			EsString::format(
        esT("Could not convert variant of type '%s' to signed integer"),
        EsEnumSymbolGet<EsVariantType>(m_type)
        )
      ).asLong(loc);
	default:
		;
	}
	ES_ASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	ES_FAIL;
	return 0; // we are never here, pacify compilers in debug mode
}

long long EsVariant::asLLong(const std::locale& loc /*= EsLocale::locale()*/) const
{
	switch( m_type )
	{
	case VAR_BOOL:
		ES_ASSERT(m_value.m_ullong <= 1);
	case VAR_INT64:
	case VAR_BYTE:
	case VAR_CHAR:
	case VAR_INT:
		return m_value.m_llong;
	case VAR_UINT:
	case VAR_UINT64:
		if( m_value.m_ullong > std::numeric_limits<long long>::max() ) // unsigned integer bigger than maximum long
		{
			EsString str = EsString::fromUInt64(m_value.m_ullong, 10, loc);
			EsException::Throw(
        esT("Could not convert '%s' to 64 bit signed integer"),
        str
      );

			ES_FAIL;
		}
		return m_value.m_llong;
	case VAR_DOUBLE:
		{
			double val = EsUtilities::round0(m_value.m_double);
			if( val < (double)std::numeric_limits<long long>::min() || val > (double)std::numeric_limits<long long>::max() )
			{
				EsException::Throw(
          esT("Could not convert '%.0f' to 64 bit signed integer"),
          val
        );
				ES_FAIL;
			}
			return static_cast<long long>(val);
		}
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if(buff.empty() || buff.size() > 8)
			{
				EsException::Throw(
					esT("Could not convert binary buffer %d bytes long to 64 bit signed integer"),
          buff.size()
        );
				ES_FAIL;
			}
			long long val = 0;
			memcpy(&val, &buff[0], buff.size());
			return val;
		}
	case VAR_STRING:
	{
		const EsString& str = doInterpretAsString();
		return EsString::toInt64(str, 0, loc);
	}
	case VAR_OBJECT:
	case VAR_VARIANT_COLLECTION:
	case VAR_STRING_COLLECTION:
		return doGetClientValueIfPresent(
      this,
			EsString::format(
        esT("Could not convert variant of type '%s' to 64 bit signed integer"),
        EsEnumSymbolGet<EsVariantType>(m_type)
      )
    ).asLLong(loc);
	default:
		;
	}
	ES_ASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	ES_FAIL;
	return 0; // we are never here, pacify compilers in debug mode
}

unsigned long EsVariant::asULong(const std::locale& loc /*= EsLocale::locale()*/) const
{
	switch( m_type )
	{
	case VAR_POINTER:
		if( sizeof(unsigned long) != sizeof(void*) )
			EsException::ThrowNotSupportedForThisType();
	case VAR_BOOL:
		ES_ASSERT(m_value.m_ullong <= 1);
	case VAR_UINT64:
		ES_ASSERT( m_value.m_ullong <= std::numeric_limits<unsigned long>::max() );
	case VAR_BYTE:
	case VAR_CHAR:
	case VAR_UINT:
		return (ulong)m_value.m_ullong;
	case VAR_INT:
	case VAR_INT64:
		if( m_value.m_llong < 0 )
		{
			EsString str = EsString::fromInt64(m_value.m_llong, 10, loc);
			EsException::Throw(
        esT("Could not convert '%s' to unsigned integer"),
        str
      );
			ES_FAIL;
		}
		return (ulong)m_value.m_ullong;
	case VAR_DOUBLE:
		{
			double val = EsUtilities::round0(m_value.m_double);
			if( val < 0.0 || val > (double)std::numeric_limits<unsigned long>::max() )
			{
				EsException::Throw(
          _("Could not convert '%.0f' to unsigned integer"),
          val
        );
				ES_FAIL;
			}
			return (ulong)val;
		}
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if( buff.empty() || buff.size() > 4 )
			{
				EsException::Throw(
					esT("Could not convert binary buffer %d bytes long to unsigned integer"),
					buff.size()
        );
				ES_FAIL;
			}
			ulong val = 0;
			memcpy(&val, &buff[0], buff.size());
			return val;
		}
	case VAR_STRING:
		{
			const EsString& str = doInterpretAsString();
			return EsString::toULong(str, 0, loc);
		}
	case VAR_OBJECT:
	case VAR_VARIANT_COLLECTION:
	case VAR_STRING_COLLECTION:
		return doGetClientValueIfPresent(
      this,
		 	EsString::format(
        esT("Could not convert variant of type '%s' to unsigned integer"),
        EsEnumSymbolGet<EsVariantType>(m_type)
      )
    ).asULong(loc);
	default:
		;
	}
	ES_ASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	ES_FAIL;
	return 0uL; // we are never here, pacify compilers in debug mode
}

unsigned long long EsVariant::asULLong(const std::locale& loc /*= EsLocale::locale()*/) const
{
	switch( m_type )
	{
	case VAR_POINTER:
		if( sizeof(unsigned long long) != sizeof(void*) )
			EsException::ThrowNotSupportedForThisType();
	case VAR_BOOL:
		ES_ASSERT(m_value.m_ullong <= 1);
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
			EsException::Throw(
        esT("Could not convert '%s' to 64 bit unsigned integer"),
        str
      );
			ES_FAIL;
		}
		return m_value.m_ullong;
	case VAR_DOUBLE:
		{
			double val = EsUtilities::round0(m_value.m_double);
			if( val < 0.0 || val > (double)std::numeric_limits<unsigned long long>::max() )
			{
				EsException::Throw(
          esT("Could not convert '%.0f' to 64 bit unsigned integer"),
          val
        );
				ES_FAIL;
			}
			return static_cast<unsigned long long>(val);
		}
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer& buff = doInterpretAsBinBuffer();
			if( buff.empty() || buff.size() > 8 )
			{
				EsException::Throw(
					esT("Could not convert binary buffer %d bytes long to 64 bit unsigned integer"),
          buff.size()
        );
				ES_FAIL;
			}
			unsigned long long val = 0;
			memcpy(&val, &buff[0], buff.size());
			return val;
		}
	case VAR_STRING:
		{
			const EsString& str = doInterpretAsString();
			return EsString::toUInt64(str, 0, loc);
		}
	case VAR_OBJECT:
	case VAR_VARIANT_COLLECTION:
	case VAR_STRING_COLLECTION:
		return doGetClientValueIfPresent(
      this,
      EsString::format(
        esT("Could not convert variant of type '%s' to 64 bit unsigned integer"),
        EsEnumSymbolGet<EsVariantType>(m_type)
      )
    ).asULLong(loc);
	default:
		;
	}
	ES_ASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	ES_FAIL;
	return 0ULL; // we are never here, pacify compilers in debug mode
}

double EsVariant::asDouble(const std::locale& loc /*= EsLocale::locale()*/) const
{
	switch( m_type )
	{
	case VAR_BOOL:
		ES_ASSERT(m_value.m_ullong <= 1);
	case VAR_BYTE: // byte is unsigned
	case VAR_CHAR: // char is unsigned by convention
#ifdef UNICODE
		ES_ASSERT((m_value.m_ullong & ~0xFFFF) == 0);
#else
		ES_ASSERT((m_value.m_ullong & ~0xFF) == 0); // this also tests the above VAR_BYTE case
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
			if( buff.size() != sizeof(double) )
			{
				EsException::Throw(
					esT("Could not convert binary buffer %d bytes long to floating point number, %d bytes expected"),
					buff.size(),
          sizeof(double)
        );
				ES_FAIL;
			}
			double val;
			memcpy(&val, &buff[0], sizeof(double));
			return val;
		}
	case VAR_STRING:
		return EsString::toDouble( doInterpretAsString(), loc );
	case VAR_OBJECT:
	case VAR_VARIANT_COLLECTION:
	case VAR_STRING_COLLECTION:
		return doGetClientValueIfPresent(
      this,
      EsString::format(
        esT("Could not convert variant of type '%s' to floating point"),
        EsEnumSymbolGet<EsVariantType>(m_type)
      )
    ).asDouble(loc);
	default:
		;
	}
	ES_ASSERT(m_type == VAR_EMPTY);
	EsException::ThrowNoValue();
	ES_FAIL;
	return 0.0; // we are never here, pacify compilers in debug mode
}

EsBinBuffer EsVariant::asBinBuffer() const
{
	ES_COMPILE_TIME_ASSERT(sizeof(long) == sizeof(unsigned long), longSizeEqUlongSize);
	ES_COMPILE_TIME_ASSERT(sizeof(long long) == sizeof(unsigned long long), llongSizeEqUllongSize);

	EsBinBuffer result;
	switch( m_type )
	{
	case VAR_EMPTY:
		EsException::ThrowNoValue();
		ES_FAIL;
	case VAR_BOOL:
		result.assign(1, (m_value.m_llong != 0) ? '\1' : '\0');
		break;
	case VAR_INT:
	case VAR_UINT: // we assume int and uint are of the same size
		result.assign(reinterpret_cast<const esU8*>(&m_value.m_llong), reinterpret_cast<const esU8*>(&m_value.m_llong) + sizeof(long));
		break;
	case VAR_INT64:
	case VAR_UINT64: // we assume int64 and uint64 are of the same size
		result.assign(reinterpret_cast<const esU8*>(&m_value.m_llong), reinterpret_cast<const esU8*>(&m_value.m_llong) + sizeof(long long));
		break;
	case VAR_DOUBLE:
		result.assign(reinterpret_cast<const esU8*>(&m_value.m_double), reinterpret_cast<const esU8*>(&m_value.m_double) + sizeof(double));
		break;
	case VAR_BYTE:
		result.assign(1, static_cast<esU8>(m_value.m_ullong));
		break;
	case VAR_CHAR:
		{
			const EsString::value_type ch = EsString::value_type(m_value.m_ullong);
			result.assign(reinterpret_cast<const esU8*>(&ch), reinterpret_cast<const esU8*>(&ch) + sizeof(EsString::value_type));
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
		{
			EsReflectedClassIntf::Ptr obj	= asObject();
			if( obj && obj->isKindOf(esT("EsScriptObject")) && obj->hasProperty(esT("buffer")) )
				return obj->propertyGet(esT("buffer")).asBinBuffer();
		}
	case VAR_STRING_COLLECTION:
	case VAR_VARIANT_COLLECTION:
		result = doGetClientValueIfPresent(
      this,
      EsString::format(
        esT("Could not convert variant of type '%s' to binary buffer"),
        EsEnumSymbolGet<EsVariantType>(m_type)
      )
    ).asBinBuffer();
		break;
	default:
		ES_FAIL;
	}
	return result;
}

EsString EsVariant::asString(const std::locale& loc /*= EsLocale::locale()*/) const
{
	EsString result;
	switch( m_type )
	{
	case VAR_EMPTY:
		EsException::ThrowNoValue();
		ES_FAIL;
	case VAR_INT:
	case VAR_BOOL:
		result = EsString::fromLong((long)m_value.m_llong, 10, loc);
		break;
	case VAR_BYTE:
	case VAR_UINT:
		result = EsString::fromULong((ulong)m_value.m_ullong, 10, loc);
		break;
	case VAR_INT64:
		result = EsString::fromInt64(m_value.m_llong, 10, loc);
		break;
	case VAR_UINT64:
		result = EsString::fromUInt64(m_value.m_ullong, 10, loc);
		break;
	case VAR_DOUBLE:
		result = EsString::fromDouble(m_value.m_double, loc);
		break;
	case VAR_CHAR:
		result.push_back( static_cast<EsString::value_type>(m_value.m_ullong) );
		break;
	case VAR_BIN_BUFFER:
		result = EsString::binToHex(doInterpretAsBinBuffer());
		break;
	case VAR_STRING:
		result = doInterpretAsString();
		break;
	case VAR_POINTER:
		EsException::Throw(esT("Could not convert generic pointer to a string value"));
		ES_FAIL;
		break;
	case VAR_OBJECT:
		{
			EsReflectedClassIntf::Ptr obj = asExistingObject();
			if( obj->hasMethod(EsMethodInfoKeyT(0, EsStdNames::asString())) )
				return obj->call(EsStdNames::asString()).doInterpretAsString();
			result = doGetClientValueIfPresent(
        this,
				esT("Could not convert variant to a string value")
      ).asString(loc);
			break;
		}
	case VAR_STRING_COLLECTION:
		result = EsString::fromStringArray(doInterpretAsStringCollection(), esT(";"));
		break;
	case VAR_VARIANT_COLLECTION:
		{
			const EsString::Array& sa = asStringCollection();
			result = EsString::fromStringArray(sa, esT(";"));
		}
		break;
	default:
		ES_FAIL; // we are never here, pacify compilers in debug mode
		break;
	}
	return result;
}

EsString EsVariant::asString(unsigned mask, const std::locale& loc /*= EsLocale::locale()*/) const
{
	EsString result;
	switch( m_type )
	{
	case VAR_CHAR:
		result = EsString::toString( EsString(1, static_cast<EsString::value_type>(m_value.m_ullong)) );
		break;
	case VAR_BIN_BUFFER:
		result = EsString::toString( EsString::binToHex(doInterpretAsBinBuffer()), mask );
		break;
	default:
		result = EsString::toString(asString(loc), mask);
		break;
	}
	return result;
}

EsString EsVariant::asEscapedString(const std::locale& loc /*= EsLocale::locale()*/) const
{
	return asString(0, loc);
}

EsString::Array EsVariant::asStringCollection(const std::locale& loc /*= EsLocale::locale()*/) const
{
	EsString::Array result;
	switch( m_type )
	{
	case VAR_STRING_COLLECTION:
		result = doInterpretAsStringCollection();
		break;
	case VAR_VARIANT_COLLECTION:
		{
			const EsVariant::Array& vars = doInterpretAsVariantCollection();
			result.reserve(vars.size());
			for( size_t idx = 0; idx < vars.size(); ++idx )
      {
        const EsVariant& var = vars[idx];
        const EsString& str = var.asString();
				result.push_back(str);
      }
			break;
		}
	default:
		result.push_back(
      asString(loc)   //< This will throw an error if variant is empty
    );
		break;
	}
	return result;
}

EsVariant::Array EsVariant::asVariantCollection() const
{
	EsVariant::Array result;
	switch( m_type )
	{
	case VAR_EMPTY:
		EsException::ThrowNoValue();
		ES_FAIL;
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
	case VAR_EMPTY:
		return EsBaseIntf::Ptr();
	default:
		EsException::Throw(
      esT("Could not convert variant of type '%s' to interface reference"),
      EsEnumSymbolGet<EsVariantType>(m_type)
    );
		ES_FAIL;
	}

	ES_FAIL;
	return EsBaseIntf::Ptr(); // we are never here, pacify compilers in debug mode
}

EsBaseIntf::Ptr EsVariant::asExistingObject()
{
	EsBaseIntf::Ptr obj = asObject();
	if( !obj )
	{
		EsException::ThrowNoValue();
		ES_FAIL;
	}
	return obj;
}

void* EsVariant::asPointer()
{
	switch( m_type )
	{
	case VAR_OBJECT:
		return reinterpret_cast<void*>(m_value.m_intf.m_ptr);
	case VAR_POINTER:
		return m_value.m_pointer;
	case VAR_EMPTY:
		EsException::ThrowNoValue();
		ES_FAIL;
		break;
	default:
		EsException::Throw(esT("Could not convert variant to generic pointer"));
		ES_FAIL;
		break;
	}
	ES_FAIL;
	return 0; // we are never here, pacify compilers in debug mode
}

void EsVariant::swap(EsVariant& other) ES_NOTHROW
{
  if(this == &other)
    return;

  // If we use binary buffer with internal local storage, we have to perform its
  // fixup after direct copy is complete
  //
  bool fixupOtherBB = (VAR_BIN_BUFFER == m_type) &&
    doInterpretAsBinBuffer().usesLocalStorage();

  bool fixupBB = (VAR_BIN_BUFFER == other.typeGet()) &&
    other.doInterpretAsBinBuffer().usesLocalStorage();

	// We know that Variant is always movable in memory as a whole
	//
  Value tmpval = m_value;
  Type tmptype = m_type;

  m_value = other.m_value;
  m_type = other.m_type;

  other.m_value = tmpval;
  other.m_type = tmptype;

  // Perform optional local storage pointer for vector
  // internally used in binary buffer
  //
  if( fixupBB )
    doInterpretAsBinBuffer().localStorageFixup();

  if( fixupOtherBB )
    other.doInterpretAsBinBuffer().localStorageFixup();
}

void EsVariant::internalMove(EsVariant& other) ES_NOTHROW
{
  m_value = other.m_value;
  m_type = other.m_type;

  // Perform optional local storage pointer for vector
  // internally used in binary buffer
  //
  if( (VAR_BIN_BUFFER == other.typeGet()) &&
      other.doInterpretAsBinBuffer().usesLocalStorage() )
    doInterpretAsBinBuffer().localStorageFixup();

	other.m_type = VAR_EMPTY;
}

void EsVariant::move(EsVariant& other) ES_NOTHROW
{
  if(this == &other)
    return;

	doCleanup();
  internalMove(other);
}

EsVariant EsVariant::pow(const EsVariant& val, const std::locale& loc /*= EsLocale::locale()*/) const
{
	return esPow(asDouble(loc), val.asDouble(loc));
}

void EsVariant::indexAdjust(int& index, unsigned count)
{
	ES_ASSERT((int)count >= 0); // check if we are not too big (16-bit architecture possibility)
	int signedCount = static_cast<int>(count);
	EsNumericCheck::checkRangeInteger(-signedCount, signedCount-1, index);
	if( index < 0 )
		index += signedCount;
}

EsVariant EsVariant::itemGet(const EsVariant& idx) const
{
	EsVariant result;
	if( m_type == VAR_OBJECT )
	{
		EsReflectedClassIntf::Ptr obj = asExistingObject();
		ES_ASSERT(obj);
		result.doAssignToEmpty(obj->call(EsStdNames::itemGet(), idx));
	}
	else
	{
		int index = idx.asInt();
		switch( m_type )
		{
		case VAR_BIN_BUFFER:
			{
				const EsBinBuffer& v = doInterpretAsBinBuffer();
				indexAdjust(index, v.size());
				result.doAssignToEmpty(v[index]);
				break;
			}
		case VAR_STRING:
			{
				const EsString& v = doInterpretAsString();
				indexAdjust(index, v.size());
				result.doAssignToEmpty( static_cast<EsString::value_type>(v[index]) );
				break;
			}
		case VAR_STRING_COLLECTION:
			{
				const EsString::Array& v = doInterpretAsStringCollection();
				indexAdjust(index, v.size());
				result.doAssignToEmpty(v[index]);
				break;
			}
		case VAR_VARIANT_COLLECTION:
			{
				const EsVariant::Array& v = doInterpretAsVariantCollection();
				indexAdjust(index, v.size());
				result.doAssignToEmpty(v[index]);
				break;
			}
		default:
			EsException::ThrowCannotIndexItem();
			ES_FAIL;
		}
	}
	return result;
}

void EsVariant::itemSet(const EsVariant& index, const EsVariant& value, const std::locale& loc /*= EsLocale::locale()*/)
{
	if( m_type == VAR_OBJECT )
	{
		EsReflectedClassIntf::Ptr obj = asExistingObject();
		ES_ASSERT(obj);
		obj->call(EsStdNames::itemSet(), index, value);
	}
	else
	{
		int intIndex = index.asInt();
		switch( m_type )
		{
		case VAR_BIN_BUFFER:
			{
				EsBinBuffer& v = doInterpretAsBinBuffer();
				indexAdjust(intIndex, v.size());
				v[intIndex] = value.asByte(loc);
			}
			break;
		case VAR_STRING:
			{
				EsString& v = doInterpretAsString();
				indexAdjust(intIndex, v.size());
				v[intIndex] = value.asChar(loc);
			}
			break;
		case VAR_STRING_COLLECTION:
			{
				EsString::Array& v = doInterpretAsStringCollection();
				indexAdjust(intIndex, v.size());
				v[intIndex] = value.asString(loc);
			}
			break;
		case VAR_VARIANT_COLLECTION:
			{
				EsVariant::Array& v = doInterpretAsVariantCollection();
				indexAdjust(intIndex, v.size());
				v[intIndex] = value;
			}
			break;
		default:
			EsException::ThrowCannotIndexItem();
			ES_FAIL;
		}
	}
}

void EsVariant::itemDelete(const EsVariant& index)
{
	if( m_type == VAR_OBJECT )
	{
		EsReflectedClassIntf::Ptr obj = asExistingObject();
		ES_ASSERT(obj);
		obj->call(EsStdNames::itemDelete(), index);
	}
	else
	{
		int intIndex = index.asInt();
		switch( m_type )
		{
		case VAR_BIN_BUFFER:
			{
				EsBinBuffer& v = doInterpretAsBinBuffer();
				indexAdjust(intIndex, v.size());
				v.erase( v.begin()+intIndex );
			}
			break;
		case VAR_STRING:
			{
				EsString& v = doInterpretAsString();
				indexAdjust(intIndex, v.size());
				v.erase( v.begin()+intIndex );
			}
			break;
		case VAR_STRING_COLLECTION:
			{
				EsString::Array& v = doInterpretAsStringCollection();
				indexAdjust(intIndex, v.size());
				v.erase( v.begin()+intIndex );
			}
			break;
		case VAR_VARIANT_COLLECTION:
			{
				EsVariant::Array& v = doInterpretAsVariantCollection();
				indexAdjust(intIndex, v.size());
				v.erase( v.begin()+intIndex );
			}
			break;
		default:
			EsException::ThrowCannotIndexItem();
			ES_FAIL;
		}
	}
}

int EsVariant::sliceAdjust(int& from, int& to, unsigned count)
{
	int length = static_cast<int>(count);

	if( from < 0 )
		from += length;
	if( from < 0 )
		from = 0;
	if( from >= length )
		from = length;

	if( to < 0 )
		to += length;
	if( to < 0 )
		to = -1; // here is an assymmetry in the way from and to handled
	if( to > length )
		to = length;

	int size = to - from;
	if( size < 0 )
	{
		to = from;
		return 0;
	}
	return size;
}

EsVariant EsVariant::sliceGet(int from, int to) const
{
	EsVariant result;
	switch( m_type )
	{
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer& v = doInterpretAsBinBuffer();
			int size = sliceAdjust(from, to, v.size());
			EsBinBuffer::const_iterator start = v.begin()+from;
			result.doAssignEsBinBufferToEmpty( EsBinBuffer(start, start+size) );
			break;
		}
	case VAR_STRING:
		{
			const EsString& s = doInterpretAsString();
			int size = sliceAdjust(from, to, s.size());
			result.doAssignToEmpty( s.substr(from, size) );
			break;
		}
	case VAR_STRING_COLLECTION:
		{
			const EsString::Array& a = doInterpretAsStringCollection();
			int size = sliceAdjust(from, to, a.size());
			EsString::Array::const_iterator start = a.begin() + from;
			result.doAssignToEmpty( EsString::Array(start, start + size) );
			break;
		}
	case VAR_VARIANT_COLLECTION:
		{
			const EsVariant::Array& v = doInterpretAsVariantCollection();
			int size = sliceAdjust(from, to, v.size());
			EsVariant::Array::const_iterator start = v.begin() + from;
			result.doAssignToEmpty( EsVariant::Array(start, start + size) );
			break;
		}
	default:
		EsException::ThrowCannotIndexItem();
		ES_FAIL;
	}
	return result;
}

//void EsVariant::SetSlice(int from, int to, const EsVariant& values)
//{
//	switch( m_type )
//	{
//	case VAR_BIN_BUFFER:
//		{
//			EsBinBuffer* v = &doInterpretAsBinBuffer();
//			int size = sliceAdjust(from, to, M_64_CAST(unsigned, v->size()));
//			if( values.IsIndexed() )
//				v->replace(from, size, values.asBinBuffer());
//			else if( values.isEmpty() )
//				v->erase(from, size);
//			else
//				v->replace(from, size, 1, values.asByte());
//		}
//		break;
//	case VAR_STRING:
//		{
//			EsString* v = &doInterpretAsString();
//			int size = sliceAdjust(from, to, M_64_CAST(unsigned, v->size()));
//			if( values.IsIndexed() )
//				v->replace(from, size, values.asString());
//			else if( values.isEmpty() )
//				v->erase(from, size);
//			else
//				v->replace(from, size, 1, (EsString::value_type)values.asChar());
//		}
//		break;
//	case VAR_STRING_COLLECTION:
//		{
//			EsString::Array* v = &doInterpretAsStringCollection();
//			sliceAdjust(from, to, M_64_CAST(unsigned, v->size()));
//			EsString::Array::iterator start = v->begin() + from;
//			Type type = values.GetType();
//			if( type == VAR_STRING_COLLECTION || type == VAR_VARIANT_COLLECTION )
//			{
//				start = v->erase(start, v->begin() + to);
//				const EsString::Array val = values.asStringCollection();
//				v->insert(start, val.begin(), val.end());
//			}
//			else if( values.isEmpty() )
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
//			EsVariant::Array* v = &doInterpretAsVariantCollection();
//			sliceAdjust(from, to, M_64_CAST(unsigned, v->size()));
//			EsVariant::Array::iterator start = v->begin() + from;
//			Type type = values.GetType();
//			if( type == VAR_STRING_COLLECTION || type == VAR_VARIANT_COLLECTION )
//			{
//				start = v->erase(start, v->begin() + to);
//				EsVariant::Array val = values.asVariantCollection();
//				EsVariant::Array::const_iterator it = val.begin();
//				EsVariant::Array::const_iterator itEnd = val.end();
//				v->insert(start, it, itEnd);
//			}
//			else if( values.isEmpty() )
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
//		ES_FAIL;
//	}
//}

static int doCompareObjects(const EsVariant& v1, const EsVariant& v2, bool equality = false)
{
	ES_ASSERT(v1.isObject() || v2.isObject());
	if( v1.isObject() )
	{
		EsReflectedClassIntf::Ptr o1 = v1.asObject();
		if( o1 )
		{
			if( v2.isObject() && o1 == v2.asObject() )
				return 0;
			if( equality && !o1->hasMethod( EsMethodInfoKeyT(1, EsStdNames::compare()) ) )
				return 1; // not equal
			return o1->call(EsStdNames::compare(), v2).asInt();
		}
		// o1 == NULL
		if( v2.isEmpty() )
			return 0; // NULL object is equal to empty variant
		if( !v2.isObject() )
			return -1; // NULL object is less than any other value
	}
	if( v2.isObject() )
	{
		EsReflectedClassIntf::Ptr o2 = v2.asObject();
		if( o2 )
		{
			if( equality && !o2->hasMethod( EsMethodInfoKeyT(1, EsStdNames::compare()) ) )
				return 1; // not equal
			return -(o2->call(EsStdNames::compare(), v1).asInt()); // we changed the order of the arguments => change the sign of the result
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
	// handle empty comparison separately
	if( VAR_EMPTY == m_type )
		return VAR_EMPTY == v.m_type;
	else if( VAR_EMPTY == v.m_type )
		return false;

	switch( (m_type > v.m_type) ? m_type : v.m_type )
	{
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
		if( left[i] < right[i] )
			return true;
		if( left[i] > right[i] )
			return false;
	}
	return left.size() < right.size();
}

bool EsVariant::operator<(const EsVariant& v) const
{
	// handle empty comparison separately
	if( VAR_EMPTY == m_type )
    return VAR_EMPTY != v.m_type;
	else if( VAR_EMPTY == v.m_type )
		return false;

	switch( (m_type > v.m_type) ? m_type : v.m_type )
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
		return doCompareObjects(*this, v) < 0;
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
	if( VAR_EMPTY == m_type )
  	return false;
	else if( VAR_EMPTY == v.m_type )
		return true;

	switch( (m_type > v.m_type) ? m_type : v.m_type )
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
		return doCompareObjects(*this, v) > 0;
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

static void doAndOrXorOnBuffers(EsVariant& result, const EsVariant& v1, const EsVariant& v2, DoAndOrXorEnum andOrXor)
{
	const EsBinBuffer& self = v1.asBinBuffer();
	const EsBinBuffer& other = v2.asBinBuffer();
	if( self.size() != other.size() )
	{
		EsException::Throw(_("Sizes of items are different, %d and %d"), self.size(), other.size());
		ES_FAIL;
	}
	result.doAssignEsBinBufferToEmpty(self);
	EsBinBuffer& resultByteString = result.doInterpretAsBinBuffer();
	EsBinBuffer::iterator it = resultByteString.begin();
	EsBinBuffer::iterator itEnd = resultByteString.end();
	EsBinBuffer::const_iterator otherIt = other.begin();
	if( andOrXor < 0 )
	{
		ES_ASSERT(andOrXor == doAnd);
		for ( ; it != itEnd; ++it, ++otherIt )
			*it &= (unsigned char)*otherIt;
	}
	else if( andOrXor > 0 )
	{
		ES_ASSERT(andOrXor == doXor);
		for ( ; it != itEnd; ++it, ++otherIt )
			*it ^= (unsigned char)*otherIt;
	}
	else
	{
		ES_ASSERT(andOrXor == doOr);
		for ( ; it != itEnd; ++it, ++otherIt )
			*it |= (unsigned char)*otherIt;
	}
}

static void doAndOrXorOnObjects(EsVariant& result, const EsVariant& v1, const EsVariant& v2, DoAndOrXorEnum andOrXor)
{
	EsReflectedClassIntf::Ptr obj = v1.asExistingObject();
	result = obj->propertyGet(EsStdNames::value());
	EsVariant val2 = v2;
	if( v2.isObject() )
	{
		obj = v2.asExistingObject();
		val2 = obj->propertyGet(EsStdNames::value());
	}

	if( andOrXor < 0 )
	{
		ES_ASSERT(andOrXor == doAnd);
		result &= val2;
	}
	else if( andOrXor > 0 )
	{
		ES_ASSERT(andOrXor == doXor);
		result ^= val2;
	}
	else
	{
		ES_ASSERT(andOrXor == doOr);
		result |= val2;
	}
}

EsVariant EsVariant::operator|(const EsVariant& v) const
{
	EsVariant result;
	switch( (m_type > v.m_type) ? m_type : v.m_type )
	{
	case VAR_BOOL:
		result.doAssignToEmpty((bool)(asBool() || v.asBool()));
		break;
	case VAR_OBJECT: // assume object's zero check
		doAndOrXorOnObjects(result, *this, v, doOr);
		break;
	case VAR_POINTER:
		EsException::ThrowNotSupportedForThisType();
		ES_FAIL;
	case VAR_BYTE:
		result.doAssignToEmpty((esU8)(asByte() | v.asByte()));
		break;
	case VAR_CHAR:
		result.doAssignToEmpty((EsString::value_type)(asChar() | v.asChar()));
		break;
	case VAR_BIN_BUFFER:
		doAndOrXorOnBuffers(result, *this, v, doOr);
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
	switch( (m_type > v.m_type) ? m_type : v.m_type )
	{
	case VAR_BOOL:
		result.doAssignToEmpty((bool)(asBool() && v.asBool()));
		break;
	case VAR_OBJECT: // assume object's zero check
		doAndOrXorOnObjects(result, *this, v, doAnd);
		break;
	case VAR_POINTER:
		EsException::ThrowNotSupportedForThisType();
		ES_FAIL;
	case VAR_BYTE:
		result.doAssignToEmpty((esU8)(asByte() & v.asByte()));
		break;
	case VAR_CHAR:
		result.doAssignToEmpty((EsString::value_type)(asChar() & v.asChar()));
		break;
	case VAR_BIN_BUFFER:
		doAndOrXorOnBuffers(result, *this, v, doAnd);
		break;
	case VAR_INT:
		result.doAssignToEmpty(static_cast<long>(asInternalDWord() & v.asInternalDWord())); // and make it back signed
		break;
	case VAR_UINT:
		result.doAssignToEmpty(static_cast<unsigned long>(asInternalDWord() & v.asInternalDWord()));
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
	switch( (m_type > v.m_type) ? m_type : v.m_type )
	{
	case VAR_BOOL:
		result.doAssignToEmpty((bool)(asBool() == v.asBool()));
		break;
	case VAR_OBJECT: // assume object's zero check
		doAndOrXorOnObjects(result, *this, v, doXor);
		break;
	case VAR_POINTER:
		EsException::ThrowNotSupportedForThisType();
		ES_FAIL;
		break;
	case VAR_BYTE:
		result.doAssignToEmpty((esU8)(asByte() ^ v.asByte()));
		break;
	case VAR_CHAR:
		result.doAssignToEmpty((EsString::value_type)(asChar() ^ v.asChar()));
		break;
	case VAR_BIN_BUFFER:
		doAndOrXorOnBuffers(result, *this, v, doXor);
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

EsVariant EsVariant::operator~() const
{
	EsVariant result;
	switch( m_type )
	{
	case VAR_EMPTY:
		result.doAssignToEmpty(true); // by convention
		break;
	case VAR_BOOL:
	case VAR_OBJECT: // assume object's zero check
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
		result.doAssignToEmpty((esU8)~asByte());
		break;
	case VAR_CHAR:
		result.doAssignToEmpty((EsString::value_type)~asChar());
		break;
	default: // VAR_UINT64, all the others....
		result.doAssignToEmpty(static_cast<unsigned long long>(~asInternalQWord()));
		break;
	}
	return result;
}

EsVariant EsVariant::operator!() const
{
	EsVariant result;
	switch( m_type )
	{
	case VAR_EMPTY:
		result.doAssignToEmpty(true); // by convention
		break;
	default: // all the others....
		result.doAssignToEmpty((bool)!asBool());
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
		ES_FAIL;
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
	if( type1 == EsVariant::VAR_BOOL || type2 == EsVariant::VAR_BOOL )
	{
		EsException::ThrowNotSupportedForThisType();
		ES_FAIL;
	}
	switch( (type1 > type2) ? type1 : type2 )
	{
	case EsVariant::VAR_BYTE:
		if( result >= 0.0 && result <= 256.0 ) // hardcode range here, do not use defines
			return (esU8)result;
		break;
	case EsVariant::VAR_CHAR:
		if( result >= -128.0 && result <= 256.0 ) // hardcode range here, do not use defines
			return static_cast<EsString::value_type>(static_cast<char>(result));
		break;
	case EsVariant::VAR_UINT:
		if( result >= 0.0 && result <= static_cast<double>(std::numeric_limits<unsigned int>::max()) )
			return static_cast<unsigned>(result);
		break;
	case EsVariant::VAR_INT:
		if( result >= (double)std::numeric_limits<int>::min() && result <= (double)std::numeric_limits<int>::max() )
			return (int)result;
		break;
	case EsVariant::VAR_UINT64:
		if( result >= 0.0 && result <= (double)std::numeric_limits<unsigned long long>::max() )
			return (unsigned)result;
		break;
	case EsVariant::VAR_INT64:
		if( result >= (double)std::numeric_limits<long long>::min() && result <= (double)std::numeric_limits<long long>::max() )
			return (int)result;
		break;
	default:
		break;
	}
	return result;
}

EsVariant EsVariant::operator+(const EsVariant& v) const
{
	switch( (m_type > v.m_type) ? m_type : v.m_type )
	{
	default:
		break;
	case VAR_BIN_BUFFER:
		return asBinBuffer() + v.asBinBuffer();
	case VAR_STRING:
		return asString() + v.asString();
	case VAR_STRING_COLLECTION:
		{
			EsString::Array result = asStringCollection();
			const EsString::Array& append = v.asStringCollection();
			result.reserve(result.size()+append.size());
			result.insert(result.end(), append.begin(), append.end());
			return result;
		}
	case VAR_VARIANT_COLLECTION:
		{
			EsVariant::Array result = asVariantCollection();
			if( v.isCollection() )
			{
				const EsVariant::Array& append = v.asVariantCollection();
				result.reserve(result.size()+append.size());
				result.insert(result.end(), append.begin(), append.end());
			}
			else
				result.push_back( v );

			return result;
		}
	case VAR_OBJECT:
		{
			EsReflectedClassIntf::Ptr obj;

      if( VAR_OBJECT == m_type )
      {
        obj = const_cast<EsVariant*>(this)->asExistingObject();
			  return obj->call(
          EsStdNames::add(),
          v
        );
      }
      else
      {
        ES_ASSERT(VAR_OBJECT == v.m_type);

        if( VAR_STRING == m_type )
          return asString() + v.asString();
        else if( VAR_BIN_BUFFER == m_type )
          return asBinBuffer() + v.asBinBuffer();
        else
        {
          obj = v.asExistingObject();
          return obj->call(
            EsStdNames::add(),
            *this
          );
        }
      }
		}
	}

	double result = asDouble() + v.asDouble();
	return doReturnTyped(result, m_type, v.m_type);
}

EsVariant& EsVariant::operator+=(const EsVariant& v)
{
	switch( m_type )
	{
	case VAR_BIN_BUFFER:
		doInterpretAsBinBuffer() += v.asBinBuffer();
		break;
	case VAR_STRING:
		doInterpretAsString() += v.asString();
		break;
	case VAR_STRING_COLLECTION:
		{
			EsString::Array& vect = doInterpretAsStringCollection();
			if( v.isCollection() )
			{
				const EsString::Array& vectOther = v.asStringCollection();
				vect.reserve(vect.size()+vectOther.size());
				vect.insert(vect.end(), vectOther.begin(), vectOther.end());
			}
			else
				vect.push_back(v.asString());
		}
		break;
	case VAR_VARIANT_COLLECTION:
		{
			EsVariant::Array& vect = doInterpretAsVariantCollection();
			if( v.isCollection() )
			{
				const EsVariant::Array& vectOther = v.asVariantCollection();
				vect.reserve(vect.size()+vectOther.size());
				vect.insert(vect.end(), vectOther.begin(), vectOther.end());
			}
			else
				vect.push_back(v);
		}
		break;
	case VAR_OBJECT:
    if( hasMethod( EsStdNames::add_inplace(), 1 ) )
    {
      call(EsStdNames::add_inplace(), v);
      return *this;
    }
    // Otherwise, fall-through to default handler
	default: // Also handles NoValue condition:
		*this = *this + v;
		break;
	}
	return *this;
}

EsVariant EsVariant::operator-(const EsVariant& v) const
{
	if( VAR_OBJECT == m_type )
	{
		EsReflectedClassIntf::Ptr obj = const_cast<EsVariant*>(this)->
			asExistingObject();
		ES_ASSERT(obj);
		return obj->call(EsStdNames::subtract(), v);
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
	if( it != c.end() )
		c.erase(it);
}

EsVariant& EsVariant::operator-=(const EsVariant& v)
{
	switch( m_type )
	{
	case VAR_STRING_COLLECTION:
		{
			EsString::Array& vect = doInterpretAsStringCollection();
			if( v.isCollection() )
			{
				const EsString::Array& vectOther = v.asStringCollection();
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
			EsVariant::Array& vect = doInterpretAsVariantCollection();
			if( v.isCollection() )
			{
				const EsVariant::Array& vectOther = v.asVariantCollection();
				EsVariant::Array::const_iterator it = vectOther.begin();
				EsVariant::Array::const_iterator itEnd = vectOther.end();
				for ( ; it != itEnd; ++it )
					doEfficientRemoveIfPresent(vect, *it);
			}
			else
				doEfficientRemoveIfPresent(vect, v);
		}
		break;
	case VAR_OBJECT:
    if( hasMethod( EsStdNames::subtract_inplace(), 1 ) )
    {
      call(EsStdNames::subtract_inplace(), v);
      return *this;
    }
    // Otherwise, fall-through to default handler
	default: // Also handles NoValue condition:
		*this = *this - v;
		break;
	}
	return *this;
}

EsVariant EsVariant::operator*(const EsVariant& v) const
{
	switch( (m_type > v.m_type) ? m_type : v.m_type )
	{
	default:
		break; // break to a double multiplication
	case VAR_BIN_BUFFER:
		{
			const EsBinBuffer* str;
			unsigned num;
			if( m_type == VAR_BIN_BUFFER )
			{
				str = &doInterpretAsBinBuffer();
				num = v.asULong();
			}
			else
			{
				ES_ASSERT(v.m_type == VAR_BIN_BUFFER);
				str = &v.doInterpretAsBinBuffer();
				num = asULong();
			}
			EsBinBuffer result;
			result.reserve(str->size()*num);
			for( unsigned i = num; i > 0; --i )
				result.insert(result.end(), str->begin(), str->end());
			return result;
		}
	case VAR_STRING:
		{
			const EsString* str;
			unsigned num;
			if( m_type == VAR_STRING )
			{
				str = &doInterpretAsString();
				num = v.asULong();
			}
			else
			{
				ES_ASSERT(v.m_type == VAR_STRING);
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
		{
			EsReflectedClassIntf::Ptr obj;
      if( VAR_OBJECT == m_type )
      {
        obj = const_cast<EsVariant*>(this)->asExistingObject();
      	return obj->call(EsStdNames::multiply(), v);
      }
      else
      {
        ES_ASSERT(VAR_OBJECT == v.m_type);
        obj = v.asExistingObject();
   			return obj->call(EsStdNames::multiply(), *this);
      }
		}
	}

	double result = asDouble() * v.asDouble();
	return doReturnTyped(result, m_type, v.m_type);
}

EsVariant EsVariant::operator/(const EsVariant& v) const
{
	if( VAR_OBJECT == m_type )
	{
		EsReflectedClassIntf::Ptr obj = const_cast<EsVariant*>(this)->
			asExistingObject();
		ES_ASSERT(obj);
		return obj->call(EsStdNames::divide(), v);
	}

	double divisor = v.asDouble();
	if( divisor == 0.0 )
	{
		EsException::ThrowDivisionByZero();
		ES_FAIL;
	}
	double result = asDouble() / divisor;
	return doReturnTyped(result, m_type, v.m_type);
}

EsVariant EsVariant::operator%(const EsVariant& v) const
{
	if( m_type == VAR_BOOL || v.m_type == VAR_BOOL )
	{
		EsException::ThrowNotSupportedForThisType();
		ES_FAIL;
	}

	if( m_type == VAR_DOUBLE || v.m_type == VAR_DOUBLE )
	{
		double val = v.asDouble();
		if( val == 0.0 )
		{
			EsException::ThrowDivisionByZero();
			ES_FAIL;
		}
		return fmod(asDouble(), val);
	}
	else
	{
		// this operator is assuming the positive second argument,
		// so the behavior depends only on the first arg

		long val = v.asLong();
		if( val == 0 )
		{
			EsException::ThrowDivisionByZero();
			ES_FAIL;
		}

		if( m_type == VAR_UINT || m_type == VAR_BYTE )
			return asULong() % val;
		else
			return asLong() % val;
	}
}

EsVariant& EsVariant::operator*=(const EsVariant& v)
{
	if( VAR_OBJECT == m_type && hasMethod( EsStdNames::multiply_inplace(), 1 ) )
  {
    call(EsStdNames::multiply_inplace(), v);
    return *this;
  }
  // Otherwise, fall-through to default handler

	return *this = *this * v;
}

EsVariant& EsVariant::operator/=(const EsVariant& v)
{
	if( VAR_OBJECT == m_type && hasMethod( EsStdNames::divide_inplace(), 1 ) )
  {
    call(EsStdNames::divide_inplace(), v);
    return *this;
  }
  // Otherwise, fall-through to default handler

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
	if( m_type == VAR_UINT || m_type == VAR_UINT64 || m_type == VAR_BYTE )
		return asULLong() >> val;
	else
		return asLLong() >> val;
}

const EsString::value_type OPERATOR_AUTOINCREMENT_STRING[] = esT("++");

EsVariant& EsVariant::operator++()
{
	switch( m_type )
	{
	case VAR_EMPTY:
		EsException::ThrowNoValue();
		ES_FAIL;
		// no need to break here
	case VAR_BOOL:
		ES_ASSERT(m_value.m_ullong <= 1);
		if( m_value.m_ullong != 0 )
		{
			EsException::ThrowOverflowInOperation(OPERATOR_AUTOINCREMENT_STRING);
			ES_FAIL;
		}
		++m_value.m_ullong;
		break;
	case VAR_BYTE:
		if( m_value.m_ullong >= std::numeric_limits<unsigned char>::max() )
		{
			EsException::ThrowOverflowInOperation(OPERATOR_AUTOINCREMENT_STRING);
			ES_FAIL;
		}
		++m_value.m_ullong;
		break;
	case VAR_CHAR:
		if( m_value.m_ullong >= sc_ES_CHAR_MAX )
		{
			EsException::ThrowOverflowInOperation(OPERATOR_AUTOINCREMENT_STRING);
			ES_FAIL;
		}
		++m_value.m_ullong;
		break;
	case VAR_INT:
		if( m_value.m_llong == std::numeric_limits<int>::max() )
		{
			EsException::ThrowOverflowInOperation(OPERATOR_AUTOINCREMENT_STRING);
			ES_FAIL;
		}
		++m_value.m_llong;
		break;
	case VAR_UINT:
		if( m_value.m_ullong == std::numeric_limits<unsigned int>::max() )
		{
			EsException::ThrowOverflowInOperation(OPERATOR_AUTOINCREMENT_STRING);
			ES_FAIL;
		}
		++m_value.m_ullong;
		break;
	case VAR_INT64:
		if( m_value.m_llong == std::numeric_limits<long long>::max() )
		{
			EsException::ThrowOverflowInOperation(OPERATOR_AUTOINCREMENT_STRING);
			ES_FAIL;
		}
		++m_value.m_llong;
		break;
	case VAR_UINT64:
		if( m_value.m_ullong == std::numeric_limits<unsigned long long>::max() )
		{
			EsException::ThrowOverflowInOperation(OPERATOR_AUTOINCREMENT_STRING);
			ES_FAIL;
		}
		++m_value.m_ullong;
		break;
	case VAR_DOUBLE:
		++m_value.m_double;
		break;
	default: // all the others....
		EsException::ThrowNotSupportedForThisType();
		ES_FAIL;
	}
	return *this;
}

const EsString::value_type OPERATOR_AUTODECREMENT_STRING[] = esT("--");

EsVariant& EsVariant::operator--()
{
	switch( m_type )
	{
	case VAR_EMPTY:
		EsException::ThrowNoValue();
		ES_FAIL;
		// no need to break here
	case VAR_BOOL:
		ES_ASSERT(m_value.m_ullong <= 1);
	case VAR_BYTE:
	case VAR_CHAR:
	case VAR_UINT:
	case VAR_UINT64:
		if( m_value.m_ullong == 0 )
		{
			EsException::ThrowUnderflowInOperation(OPERATOR_AUTODECREMENT_STRING);
			ES_FAIL;
		}
		--m_value.m_ullong;
		break;
	case VAR_INT:
		if( m_value.m_llong == std::numeric_limits<int>::min() )
		{
			EsException::ThrowUnderflowInOperation(OPERATOR_AUTODECREMENT_STRING);
			ES_FAIL;
		}
		--m_value.m_llong;
		break;
	case VAR_INT64:
		if( m_value.m_llong == std::numeric_limits<long long>::min() )
		{
			EsException::ThrowUnderflowInOperation(OPERATOR_AUTODECREMENT_STRING);
			ES_FAIL;
		}
		--m_value.m_llong;
		break;
	case VAR_DOUBLE:
		--m_value.m_double;
		break;
	default: // all the others....
		EsException::ThrowNotSupportedForThisType();
		ES_FAIL;
	}
	return *this;
}

EsVariant& EsVariant::doSetInt(long long value, Type type) ES_NOTHROW
{
	doSetType(type);
	m_value.m_llong = value;
	return *this;
}

bool EsVariant::has(const EsVariant& v) const
{
  if( isString() && v.isString() )
    return doInterpretAsString() == v.doInterpretAsString(); // TODO: has substring, do the same same for bin buffer

	if( v.isCollection() || v.isIndexed() ) //< Case when the parameter is collection or allows indexed access
	{
		int count = v.countGet();
		for( int i = 0; i < count; ++i )
			if( !has(v.itemGet(i)) ) // recursive call for each item in the parameter
				return false;

		return true;
	}

	if( isCollection() || isIndexed() ) // if we are a collection or allow indexed access
	{
		int count = countGet();
		for( int i = 0; i < count; ++i )
    {
      const EsVariant& item = itemGet(i);
      if( item.has(v) )
        return true;
    }

		return false;
	}
	else if( isObject() )
	{
		EsReflectedClassIntf::Ptr obj = asExistingObject();
		if( obj->hasMethod(
        EsMethodInfoKeyT(
          1,
          esT("has")
        )
      )
    )
  		return obj->call(
        esT("has"),
        v
      ).asBool();
	}

	return v == *this; //< Fallback to values equality comparison
}
//---------------------------------------------------------------------------

EsVariant& EsVariant::addToVariantCollection(const EsVariant& v, bool toFront/* = false*/)
{
  if( toFront )
    doInterpretAsVariantCollection().insert(doInterpretAsVariantCollection().begin(), v);
  else
    doInterpretAsVariantCollection().push_back(v);

  return *this;
}
//---------------------------------------------------------------------------

EsVariant EsVariant::find(const EsVariant& var) const
{
	switch(m_type)
	{
	case EsVariant::VAR_STRING:
		{
      const EsString& str = doInterpretAsString();
			size_t pos = str.find(var.asString());
			if( EsString::npos != pos )
				return pos;
		}
    break;
	case EsVariant::VAR_BIN_BUFFER:
		{
      const EsBinBuffer& bb = doInterpretAsBinBuffer();
      size_t pos;
      if( EsVariant::VAR_BIN_BUFFER == var.typeGet() )
        pos = bb.search( var.asBinBuffer() );
      else
			  pos = bb.find(var.asByte());
			if( EsBinBuffer::npos != pos )
				return pos;
		}
    break;
	case EsVariant::VAR_STRING_COLLECTION:
		{
			const EsStringArray& a = doInterpretAsStringCollection();
			EsStringArray::const_iterator cit = std::find( a.begin(), a.end(), var.asString() );
			if( cit != a.end() )
				return cit-a.begin();
		}
    break;
	case EsVariant::VAR_VARIANT_COLLECTION:
		{
			const EsVariantArray& va = doInterpretAsVariantCollection();
			EsVariantArray::const_iterator cit = std::find( va.begin(), va.end(), var );
			if( cit != va.end() )
				return cit-va.begin();
		}
    break;
	default:
		EsException::ThrowNotSupportedForThisType( esT("find"),
      EsEnumSymbolGet<EsVariantType>(
        static_cast<ulong>(m_type)
      ).c_str()
    );
	}

	return EsVariant::null();
}
//---------------------------------------------------------------------------

EsVariant EsVariant::findFirstOf(const EsVariant& var) const
{
	switch(m_type)
	{
	case EsVariant::VAR_STRING:
		{
      const EsString& str = doInterpretAsString();
			size_t pos = str.find_first_of(var.asString());
			if( EsString::npos != pos )
				return pos;
		}
    break;
	case EsVariant::VAR_BIN_BUFFER:
		{
      const EsBinBuffer& bb = doInterpretAsBinBuffer();
      size_t pos = bb.find(var.asByte());
			if( EsBinBuffer::npos != pos )
				return pos;
		}
    break;
	case EsVariant::VAR_STRING_COLLECTION:
		{
			const EsStringArray& a = doInterpretAsStringCollection();
			EsStringArray::const_iterator cit = std::find( a.begin(), a.end(), var.asString() );
			if( cit != a.end() )
				return a.size()-(cit-a.begin())-1;
		}
    break;
	case EsVariant::VAR_VARIANT_COLLECTION:
		{
			const EsVariantArray& va = doInterpretAsVariantCollection();
			EsVariantArray::const_iterator cit = std::find( va.begin(), va.end(), var );
			if( cit != va.end() )
				return va.size()-(cit-va.begin())-1;
		}
    break;
	default:
		EsException::ThrowNotSupportedForThisType( esT("findFirstOf"),
      EsEnumSymbolGet<EsVariantType>(
        static_cast<ulong>(m_type)
      ).c_str()
    );
	}

	return EsVariant::null();
}
//---------------------------------------------------------------------------

EsVariant EsVariant::findLastOf(const EsVariant& var) const
{
	switch(m_type)
	{
	case EsVariant::VAR_STRING:
		{
      const EsString& str = doInterpretAsString();
			size_t pos = str.find_last_of(var.asString());
			if( EsString::npos != pos )
				return pos;
		}
    break;
	case EsVariant::VAR_BIN_BUFFER:
		{
      const EsBinBuffer& bb = doInterpretAsBinBuffer();
			size_t pos = bb.find_last(var.asByte());
			if( EsBinBuffer::npos != pos )
				return pos;
		}
    break;
	case EsVariant::VAR_STRING_COLLECTION:
		{
			const EsStringArray& a = doInterpretAsStringCollection();
			EsStringArray::const_reverse_iterator crit = std::find( a.rbegin(), a.rend(), var.asString() );
			if( crit != a.rend() )
				return a.size()-(crit-a.rbegin())-1;
		}
    break;
	case EsVariant::VAR_VARIANT_COLLECTION:
		{
			const EsVariantArray& va = doInterpretAsVariantCollection();
			EsVariantArray::const_reverse_iterator crit = std::find( va.rbegin(), va.rend(), var );
			if( crit != va.rend() )
				return va.size()-(crit-va.rbegin())-1;
		}
    break;
	default:
		EsException::ThrowNotSupportedForThisType(
      esT("findLastOf"),
      EsEnumSymbolGet<EsVariantType>(
        static_cast<ulong>(m_type)
      ).c_str()
    );
	}

	return EsVariant::null();
}
//---------------------------------------------------------------------------

template <typename ContainerT>
static void rangeAssign( ContainerT& dest, const ContainerT& src, ulong offs, ulong cnt )
{
  if( !cnt )
    cnt = src.size();
  else
    cnt = esMin(cnt, static_cast<ulong>(src.size()));

  int from = offs;
  int to = offs+cnt;
  int size = EsVariant::sliceAdjust(from, to, dest.size());

  typename ContainerT::const_iterator csrc = src.begin();
  for(typename ContainerT::iterator idest = dest.begin()+from; idest != dest.begin()+from+size; ++idest)
    *idest = *csrc++;
}
//---------------------------------------------------------------------------

void EsVariant::replace(const EsVariant& var, ulong offs, ulong cnt /*= 0*/)
{
  switch(m_type)
  {
  case EsVariant::VAR_STRING:
    {
      EsString& s = doInterpretAsString();
      const EsString& r = var.doInterpretAsString();

      rangeAssign(s, r, offs, cnt);
    }
    break;
  case EsVariant::VAR_BIN_BUFFER:
    {
      EsBinBuffer& s = doInterpretAsBinBuffer();
      const EsBinBuffer& r = var.doInterpretAsBinBuffer();

      rangeAssign(s, r, offs, cnt);
    }
    break;
  case EsVariant::VAR_STRING_COLLECTION:
    {
      EsStringArray& s = doInterpretAsStringCollection();
      const EsStringArray& r = var.doInterpretAsStringCollection();

      rangeAssign(s, r, offs, cnt);
    }
    break;
  case EsVariant::VAR_VARIANT_COLLECTION:
    {
      EsVariantArray& s = doInterpretAsVariantCollection();
      const EsVariantArray& r = var.doInterpretAsVariantCollection();

      rangeAssign(s, r, offs, cnt);
    }
    break;
  default:
    EsException::ThrowNotSupportedForThisType(
      esT("replace"),
      EsEnumSymbolGet<EsVariantType>(
        static_cast<ulong>(m_type)
      ).c_str()
    );
  }
}
//---------------------------------------------------------------------------

void EsVariant::sortAscending()
{
  switch( m_type )
  {
  case EsVariant::VAR_BIN_BUFFER:
    {
      EsBinBuffer& bb = doInterpretAsBinBuffer();
      std::sort(bb.begin(), bb.end());
    }
    break;
  case EsVariant::VAR_STRING_COLLECTION:
    {
      EsString::Array& aobj = doInterpretAsStringCollection();
      std::sort(aobj.begin(), aobj.end());
    }
    break;
  case EsVariant::VAR_VARIANT_COLLECTION:
    {
      EsVariant::Array& aobj = doInterpretAsVariantCollection();
      std::sort(aobj.begin(), aobj.end());
    }
    break;
  default:
    EsException::ThrowNotSupportedForThisType(
      esT("sortAscending"),
      EsEnumSymbolGet<EsVariantType>(
        static_cast<ulong>(m_type)
      ).c_str()
    );
  }
}
//---------------------------------------------------------------------------

void EsVariant::sortDescending()
{
  switch( m_type )
  {
  case EsVariant::VAR_BIN_BUFFER:
    {
      EsBinBuffer& bb = doInterpretAsBinBuffer();
      std::sort(bb.begin(), bb.end(), std::greater<EsBinBuffer::value_type>());
    }
    break;
  case EsVariant::VAR_STRING_COLLECTION:
    {
      EsString::Array& aobj = doInterpretAsStringCollection();
      std::sort(aobj.begin(), aobj.end(), std::greater<EsString>());
    }
    break;
  case EsVariant::VAR_VARIANT_COLLECTION:
    {
      EsVariant::Array& aobj = doInterpretAsVariantCollection();
      std::sort(aobj.begin(), aobj.end(), std::greater<EsVariant>());
    }
    break;
  default:
    EsException::ThrowNotSupportedForThisType(
      esT("sortDescending"),
      EsEnumSymbolGet<EsVariantType>(
        static_cast<ulong>(m_type)
      ).c_str()
    );
  }
}
//---------------------------------------------------------------------------

void EsVariant::reverse()
{
  switch( m_type )
  {
  case EsVariant::VAR_BIN_BUFFER:
    {
      EsBinBuffer& bb = doInterpretAsBinBuffer();
      std::reverse(bb.begin(), bb.end());
    }
    break;
  case EsVariant::VAR_STRING_COLLECTION:
    {
      EsString::Array& aobj = doInterpretAsStringCollection();
      std::reverse(aobj.begin(), aobj.end());
    }
    break;
  case EsVariant::VAR_VARIANT_COLLECTION:
    {
      EsVariant::Array& aobj = doInterpretAsVariantCollection();
      std::reverse(aobj.begin(), aobj.end());
    }
    break;
  default:
    EsException::ThrowNotSupportedForThisType(
      esT("reverse"),
      EsEnumSymbolGet<EsVariantType>(
        static_cast<ulong>(m_type)
      ).c_str()
    );
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

// Get our contents as reflected object intf ptr, performing
// an internal check, if we hold an instance of reflected object.
// If not, an exception is thrown
//
static EsReflectedClassIntf::Ptr getAsReflectedObject(const EsVariant& val, bool doThrow = true)
{
  EsReflectedClassIntf::Ptr ro;
  if( val.isObject() )
  {
    ro = val.asObject();
    if( !ro && doThrow )
      EsException::ThrowObjectIsNotReflected();
  }
  else if( doThrow )
    EsException::ThrowIsNotAnObject();

  return ro;
}
//---------------------------------------------------------------------------

EsString EsVariant::kindGet() const
{
  if( !isObject() )
    return EsEnumSymbolGet<EsVariantType>( typeGet() );
  else if( !isEmpty() )
  {
    EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this, false);
    if( ro )
      return ro->typeNameGet();
  }

  return EsString::null();
}
//---------------------------------------------------------------------------

EsString EsVariant::typeNameGet() const
{
  if(!isObject() || isEmpty())
    EsException::Throw(
      esT("EsVariant does not contain a valid object")
    );

  return kindGet();
}
//--------------------------------------------------------------------------------

bool EsVariant::isKindOf(const EsString& type) const
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this, false);

  if( ro )
    return ro->isKindOf(type);

  return false;
}

bool EsVariant::hasMethod(const EsString& mangledName) const
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this, false);

  if( ro )
    return ro->hasMethod( EsMethodInfoKeyT(mangledName) );

  return false;
}

bool EsVariant::hasMethod(const EsString& name, ulong paramsCnt) const
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this, false);

  if( ro )
    return ro->hasMethod( EsMethodInfoKeyT(paramsCnt, name) );

  return false;
}

bool EsVariant::hasProperty(const EsString& name) const
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this, false);

  if( ro )
    return ro->hasProperty(name);

  return false;
}

EsVariant EsVariant::call(const EsString& method)
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  return ro->call(method);
}

EsVariant EsVariant::call(const EsString& method, const EsVariant& arg0)
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  return ro->call(method, arg0);
}

EsVariant EsVariant::call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1)
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  return ro->call(method, arg0, arg1);
}

EsVariant EsVariant::call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2)
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  return ro->call(method, arg0, arg1, arg2);
}

EsVariant EsVariant::call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3)
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  return ro->call(method, arg0, arg1, arg2, arg3);
}

EsVariant EsVariant::call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4)
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  return ro->call(method, arg0, arg1, arg2, arg3, arg4);
}

EsVariant EsVariant::call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5)
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  return ro->call(method, arg0, arg1, arg2, arg3, arg4, arg5);
}

EsVariant EsVariant::classCall(const EsString& method)
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  return ro->classCall(method);
}

EsVariant EsVariant::classCall(const EsString& method, const EsVariant& arg0)
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  return ro->classCall(method, arg0);
}

EsVariant EsVariant::classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1)
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  return ro->classCall(method, arg0, arg1);
}

EsVariant EsVariant::classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2)
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  return ro->classCall(method, arg0, arg1, arg2);
}

EsVariant EsVariant::classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3)
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  return ro->classCall(method, arg0, arg1, arg2, arg3);
}

EsVariant EsVariant::classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4)
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  return ro->classCall(method, arg0, arg1, arg2, arg3, arg4);
}

EsVariant EsVariant::classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5)
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  return ro->classCall(method, arg0, arg1, arg2, arg3, arg4, arg5);
}

EsVariant EsVariant::propertyGet(const EsString& property) const
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  return ro->propertyGet(property);
}

void EsVariant::propertySet(const EsString& property, const EsVariant& val)
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  ro->propertySet(property, val);
}

EsVariant EsVariant::fieldGet(const EsString& field) const
{
  EsReflectedClassIntf::Ptr ro = getAsReflectedObject(*this);
  ES_ASSERT(ro);

  if( ro->isKindOf(esT("EsScriptObject")) )
    return ro->call(esT("fieldGet"), field);

  return EsVariant::null();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Constness handling stuff
//
EsVariant EsVariant::call(const EsString& method) const
{ return const_cast<EsVariant*>(this)->call(method); }

EsVariant EsVariant::call(const EsString& method, const EsVariant& arg0) const
{ return const_cast<EsVariant*>(this)->call(method, arg0); }

EsVariant EsVariant::call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1) const
{ return const_cast<EsVariant*>(this)->call(method, arg0, arg1); }

EsVariant EsVariant::call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2) const
{ return const_cast<EsVariant*>(this)->call(method, arg0, arg1, arg2); }

EsVariant EsVariant::call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3) const
{ return const_cast<EsVariant*>(this)->call(method, arg0, arg1, arg2, arg3); }

EsVariant EsVariant::call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4) const
{ return const_cast<EsVariant*>(this)->call(method, arg0, arg1, arg2, arg3, arg4); }

EsVariant EsVariant::call(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5) const
{ return const_cast<EsVariant*>(this)->call(method, arg0, arg1, arg2, arg3, arg4, arg5); }

EsVariant EsVariant::classCall(const EsString& method) const
{ return const_cast<EsVariant*>(this)->classCall(method); }

EsVariant EsVariant::classCall(const EsString& method, const EsVariant& arg0) const
{ return const_cast<EsVariant*>(this)->classCall(method, arg0); }

EsVariant EsVariant::classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1) const
{ return const_cast<EsVariant*>(this)->classCall(method, arg0, arg1); }

EsVariant EsVariant::classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2) const
{ return const_cast<EsVariant*>(this)->classCall(method, arg0, arg1, arg2); }

EsVariant EsVariant::classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3) const
{ return const_cast<EsVariant*>(this)->classCall(method, arg0, arg1, arg2, arg3); }

EsVariant EsVariant::classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4) const
{ return const_cast<EsVariant*>(this)->classCall(method, arg0, arg1, arg2, arg3, arg4); }

EsVariant EsVariant::classCall(const EsString& method, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5) const
{ return const_cast<EsVariant*>(this)->classCall(method, arg0, arg1, arg2, arg3, arg4, arg5); }

void EsVariant::propertySet(const EsString& property, const EsVariant& val) const
{ const_cast<EsVariant*>(this)->propertySet(property, val); }
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsVariant EsVariant::clone() const
{
  if( isObject() )
    return call("clone");

  return *this;
}
//---------------------------------------------------------------------------

