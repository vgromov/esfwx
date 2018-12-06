#include "escorepch.h"
#pragma hdrstop

#include "EsRegEx.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#if defined(ES_USE_REGEX)
namespace EsReflection
{
ES_DECL_ENUMERATION_INFO(EsRegExCompileFlag, esT("Regular expression flags used in pattern compilation"))
ES_DECL_ENUMERATION_INFO(EsRegExMatchFlag, esT("Regular expression flags used in pattern matching"))
ES_DECL_ENUMERATION_INFO(EsRegExReplaceFlag, esT("Regular expression replace string format parser flags"))
}

// Use PCRE implementation internally by default
#if !defined(ES_USE_REGEX_STD) && !defined(ES_USE_REGEX_PCRE)
# define ES_USE_REGEX_PCRE
# pragma message( "Using PCRE REG EX Implementation by default" )
#endif

#if defined(ES_USE_REGEX_STD)
# include "EsRegEx.std.cxx"
#elif defined(ES_USE_REGEX_PCRE)
# include "EsRegEx.pcre.cxx"
#else
# error RegEx implementation is not defined!
#endif

// Reflected regex implementation
ES_DECL_BASE_CLASS_INFO_BEGIN(EsRegEx, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(EsRegEx, EsVariant_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(EsRegEx, EsVariant_ClassCall_cr_EsString_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsRegEx, compile, bool_Call_cr_EsString_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsRegEx, matchRangeGet, EsVariant_CallConst_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsRegEx, matchGet, EsString_CallConst_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsRegEx, replace, EsString_Call_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsRegEx, replace, EsString_Call_cr_EsString_ulong, NO_METHOD_DESCR)
  ES_DECL_PROP_INFO_RO(EsRegEx, ok, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsRegEx, matchCount, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsRegEx, matches, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(EsRegEx, text, EsString, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(EsRegEx, offset, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(0), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_PERSISTENT(EsRegEx, pattern, EsString, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_PERSISTENT(EsRegEx, compileFlags, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(EsRegExCompileFlag::DEFAULT), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_PERSISTENT(EsRegEx, matchFlags, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(EsRegExMatchFlag::DEFAULT), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_PERSISTENT(EsRegEx, replaceFlags, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(EsRegExReplaceFlag::DEFAULT), NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END

EsRegEx::EsRegEx() :
m_impl( new EsRegExImpl ),
m_compileFlags(static_cast<ulong>(EsRegExCompileFlag::DEFAULT)),
m_matchFlags(static_cast<ulong>(EsRegExMatchFlag::DEFAULT)),
m_replaceFlags(static_cast<ulong>(EsRegExReplaceFlag::DEFAULT)),
m_compiled(false),
m_offs(0)
{
}

EsRegEx::EsRegEx(const EsString& patt, ulong flags) :
m_impl( new EsRegExImpl ),
m_compileFlags(static_cast<ulong>(EsRegExCompileFlag::DEFAULT)),
m_matchFlags(static_cast<ulong>(EsRegExMatchFlag::DEFAULT)),
m_replaceFlags(static_cast<ulong>(EsRegExReplaceFlag::DEFAULT)),
m_compiled(false),
m_offs(0)
{
  compile(patt, flags);
}

EsRegEx::~EsRegEx()
{
  ES_DELETE(m_impl);
}

// reflected constructors
EsVariant EsRegEx::NEW()
{
  std::unique_ptr<EsRegEx> obj( new EsRegEx() );
  ES_ASSERT(obj);
  obj->m_dynamic = true;
  return obj.release()->asBaseIntfPtr();
}

EsVariant EsRegEx::NEW( cr_EsString patt, ulong flags )
{
  std::unique_ptr<EsRegEx> obj( new EsRegEx(patt, flags) );
  ES_ASSERT(obj);
  obj->m_dynamic = true;
  return obj.release()->asBaseIntfPtr();
}

//regex services
bool EsRegEx::compile(cr_EsString patt, ulong flags)
{
  if( !m_compiled || patt != m_patt || flags != m_compileFlags )
  {
    m_patt = patt;
    m_compileFlags = flags;
    m_compiled = m_impl->compile(m_patt, m_compileFlags);
  }

  return m_compiled;
}

// start-end match range object is returned, or null otherwise
EsVariant  EsRegEx::matchRangeGet(ulong subExprIdx) const
{
  if(m_compiled && !m_text.empty())
  {
    ulong start, len;
    if( m_impl->matchGet(start, len, subExprIdx) )
    {
      EsVariant range(EsVariant::VAR_VARIANT_COLLECTION);
      start += m_offs;
      range.addToVariantCollection(start)
        .addToVariantCollection(start+len);
      return range;
    }
  }

  return EsVariant::null();
}

bool EsRegEx::matchGet(ulong& start, ulong& len, ulong subExprIdx) const
{
  bool result = false;
  if(m_compiled && !m_text.empty())
  {
    result = m_impl->matchGet(
      start, 
      len, 
      subExprIdx
    );
    if( result )
      start += m_offs;
  }

  return result;
}

// Returns the part of string corresponding to the match where index is interpreted as above. 
EsString EsRegEx::matchGet(ulong subExprIdx) const
{
  ulong start, len;
  if(matchGet(start, len, subExprIdx) && len)
    return m_text.substr(start, len);

  return EsString::null();
}

// Matches the precompiled regular expression against the string text, returns true if matches and false otherwise. 
bool EsRegEx::get_matches() const
{
  const_cast<EsRegEx*>(this)->compile(m_patt, m_compileFlags);
  return m_impl->matches(m_text, m_offs, m_matchFlags);
}

// Replaces the current regular expression in the string 'text', with the text in replacement
EsString EsRegEx::replace(cr_EsString replacement)
{
  compile(m_patt, m_compileFlags);
  return m_impl->replace(m_text, m_offs, replacement, m_replaceFlags);
}

EsString EsRegEx::replace(cr_EsString replacement, ulong flags)
{
  m_replaceFlags = flags;
  return replace(replacement);
}

// reflected properties
bool EsRegEx::get_ok() const
{
  return isOk();
}

ulong EsRegEx::get_matchCount() const
{
  return m_impl->matchCountGet();
}

EsString EsRegEx::get_pattern() const
{
  return m_patt;
}

void EsRegEx::set_pattern(const EsString& patt)
{
  compile(patt, m_compileFlags);
}

EsString EsRegEx::get_text() const
{
  return m_text;
}

void EsRegEx::set_text(const EsString& text)
{
  m_text = text;
  m_offs = 0;
}

ulong EsRegEx::get_offset() const
{
  return m_offs;
}

void EsRegEx::set_offset(cr_ulong val)
{
  EsNumericCheck::checkRangeUInteger(
    0, 
    static_cast<ulong>(m_text.size()), 
    val, 
    esT("Text offset")
  );
  m_offs = val;
}

ulong EsRegEx::get_compileFlags() const
{
  return m_compileFlags;
}

void EsRegEx::set_compileFlags(const ulong& flags)
{
   compile(m_patt, flags);
}

ulong EsRegEx::get_matchFlags() const
{
  return m_matchFlags;
}

void EsRegEx::set_matchFlags(const ulong& flags)
{
  m_matchFlags = flags;
}

ulong EsRegEx::get_replaceFlags() const
{
  return m_replaceFlags;
}

void EsRegEx::set_replaceFlags(const ulong& flags)
{
  m_replaceFlags = flags;
}

#endif // #if defined(ES_USE_REGEX)

