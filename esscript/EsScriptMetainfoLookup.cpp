#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptMetainfoLookup.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(      EsScriptMetainfoLookup, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(      EsScriptMetainfoLookup, EsBaseIntfPtr_ClassCall_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetainfoLookup, objectNamesFind, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetainfoLookup, globalFunctionsFind, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetainfoLookup, variantServicesFind, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetainfoLookup, objectStaticMethodsFind, EsStringArray_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetainfoLookup, objectMethodsFind, EsStringArray_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetainfoLookup, objectVariablesFind, EsStringArray_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetainfoLookup, objectFieldsFind, EsStringArray_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetainfoLookup, objectPropertiesFind, EsStringArray_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetainfoLookup, enumNamesFind, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetainfoLookup, enumSymbolsFind, EsStringArray_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetainfoLookup, globalVarsFind, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetainfoLookup, systemVarsFind, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetainfoLookup, constantsFind, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetainfoLookup, constantsFindSyntaxRestricted, EsStringArray_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetainfoLookup, localVarsFind, EsStringArray_CallConst_cr_EsString_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptMetainfoLookup, keywordsFind, EsStringArray_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)

  ES_DECL_PROP_INFO_RO(             EsScriptMetainfoLookup, metaclass, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsScriptMetainfoLookup::EsScriptMetainfoLookup(const EsMetaclassIntf::Ptr& meta) ES_NOTHROW :
m_meta(meta)
{
  if( !m_meta )
    EsException::ThrowNoValue(
      _("Could not construct EsScriptMetainfoLookup with an empty metaclass instance")
    );
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsScriptMetainfoLookup::NEW(cr_EsBaseIntfPtr meta)
{
  std::unique_ptr<EsScriptMetainfoLookup> tmp = ES_MAKE_UNIQUE( 
    EsScriptMetainfoLookup,
    meta
  );
  ES_ASSERT(tmp);

  tmp->m_dynamic = true;

  return tmp.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsBaseIntfPtr EsScriptMetainfoLookup::get_metaclass() const
{
  return m_meta;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

static void filterAndSort(EsStringArray& arr, const EsString& filter, const EsString& after = EsString::null(), bool ignoreCase = false)
{
  if( !filter.empty() )
  {
    EsString lafter = after;
    EsString lflt = filter;

    if(ignoreCase)
    {
      lafter = EsString::toLower(after);
      lflt = EsString::toLower(filter);
    }

    auto newEnd = std::remove_if(
      arr.begin(),
      arr.end(),
      [&lflt, &lafter, ignoreCase](const EsString& val) -> bool {

        EsString lval = val;
        if(ignoreCase)
          lval = EsString::toLower(val);

        size_t offs = 0;
        if( !lafter.empty() )
          offs = lval.find(lafter);

        if( EsString::npos != offs )
          offs += lafter.size();
        else 
          offs = 0;

        size_t pos = lval.find(
          lflt,
          offs
        );
        
        if(!lafter.empty())
          return offs != pos; //< If not found filter, of found not next next to 'after', remove
        else
          return EsString::npos == pos; //< Remove, if filter not found
      }
    );

    arr.erase(
      newEnd,
      arr.end()
    );
  }

  std::sort(
    arr.begin(), 
    arr.end()
  );
}
//--------------------------------------------------------------------------------

static EsString tokenizedSigTypeGet(EsStringTokenizer& toksig)
{
  const EsString& token = toksig.get_nextToken();
  if( esT("cr") == token )
    return toksig.get_nextToken() + esT("&");

  return token;
}
//--------------------------------------------------------------------------------

static void fqNameToReadable(EsString& fqName)
{
  EsStringTokenizer tok(fqName, esT("|"));

  EsString result = tok.get_nextToken();
  long paramcnt = EsString::toLong(
    tok.get_nextToken()
  );

  const EsString& sig = tok.get_nextToken(); //< Get signature
  ES_ASSERT(!sig.empty());

  // Replace call specifier with service name
  EsRegEx re(esT("(Object[:.]+Method)"));
  re.set_text(sig);

  fqName = re.replace(result);
}
//--------------------------------------------------------------------------------

static void fqNamesToReadables(EsStringArray& fqNames)
{
  for( size_t idx = 0; idx < fqNames.size(); ++idx )
    fqNameToReadable(
      fqNames[idx]
    );
}
//--------------------------------------------------------------------------------

static void nonStaticRemove(EsStringArray& fqNames)
{
  auto newEnd = std::remove_if(
    fqNames.begin(),
    fqNames.end(),
    [](const EsString& fqName) -> bool {
      return EsString::npos != fqName.find(esT("."));
    }
  );

  fqNames.erase(
    newEnd,
    fqNames.end()
  );
}
//--------------------------------------------------------------------------------

static void staticRemove(EsStringArray& fqNames)
{
  auto newEnd = std::remove_if(
    fqNames.begin(),
    fqNames.end(),
    [](const EsString& fqName) -> bool {
      return EsString::npos != fqName.find(esT("::"));
    }
  );

  fqNames.erase(
    newEnd,
    fqNames.end()
  );
}
//--------------------------------------------------------------------------------

void EsScriptMetainfoLookup::enumerationsRemove(EsStringArray& objs) const
{
  auto newEnd = std::remove_if(
    objs.begin(),
    objs.end(),
    [this](const EsString& obj) -> bool {
      return m_meta->isKindOf(obj, EsEnumeration::classNameGetStatic());
    }
  );

  objs.erase(
    newEnd,
    objs.end()
  );
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetainfoLookup::objectNamesFind(cr_EsString start) const
{
  EsStringArray objs = m_meta->objectNamesGet();

  filterAndSort(objs, start);
  enumerationsRemove(objs);

  return objs;
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetainfoLookup::globalFunctionsFind(cr_EsString start) const
{
  EsStringArray methods = m_meta->fqNamesGet(EsString::null());

  nonStaticRemove(methods);
  fqNamesToReadables(methods);
  filterAndSort(methods, start);

  return methods;
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetainfoLookup::variantServicesFind(cr_EsString start) const
{
  return objectStaticMethodsFind(
    esT("EsVar"),
    start
  );
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetainfoLookup::objectStaticMethodsFind(cr_EsString nsOrObj, cr_EsString start) const
{
  if(!m_meta->hasClass(nsOrObj))
    return EsString::nullArray();

  EsStringArray methods = m_meta->fqNamesGet(nsOrObj);

  nonStaticRemove(methods);
  fqNamesToReadables(methods);
  filterAndSort(
    methods, 
    start, 
    nsOrObj + esT("::")
  );

  return methods;
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetainfoLookup::objectMethodsFind(cr_EsString obj, cr_EsString start) const
{
  if(!m_meta->hasClass(obj))
    return EsString::nullArray();

  EsStringArray methods = m_meta->fqNamesGet(obj);

  staticRemove(methods);
  fqNamesToReadables(methods);
  filterAndSort(
    methods, 
    start,
    obj + esT('.')
  );

  return methods;
}
//--------------------------------------------------------------------------------

EsScriptMachine* EsScriptMetainfoLookup::vm() const
{
  EsReflectedClassIntf::Ptr refl = m_meta;
  ES_ASSERT(refl);

  EsScriptMachine* pvm = reinterpret_cast<EsScriptMachine*>(
    refl->call(esT("scriptMachineGet")).asPointer()
  );
  ES_ASSERT(pvm);

  return pvm;
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetainfoLookup::objectVariablesFind(cr_EsString obj, cr_EsString start) const
{
  EsScriptObjectIntf::Ptr meta = vm()->metaclassFind(obj);
  if(!meta)
    return EsString::nullArray();

  EsStringArray vars = meta->varNamesGet();
  filterAndSort(
    vars,
    start
  );

  return vars;
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetainfoLookup::objectFieldsFind(cr_EsString obj, cr_EsString start) const
{
  EsScriptObjectIntf::Ptr meta = vm()->metaclassFind(obj);

  if(!meta)
    return EsString::nullArray();

  EsStringArray flds = meta->fieldNamesGet();
  filterAndSort(
    flds,
    start
  );

  return flds;
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetainfoLookup::objectPropertiesFind(cr_EsString obj, cr_EsString start) const
{
  EsStringArray props = m_meta->propertyNamesGet(obj);
  filterAndSort(props, start);

  return props;
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetainfoLookup::enumNamesFind(cr_EsString start) const
{
  EsStringArray enums = m_meta->enumerationNamesGet();
  filterAndSort(enums, start);

  return enums;
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetainfoLookup::enumSymbolsFind(cr_EsString enu, cr_EsString start) const
{
  EsStringArray enumSymbols = m_meta->enumerationSymbolsGet(enu);
  filterAndSort(enumSymbols, start);

  return enumSymbols;
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetainfoLookup::globalVarsFind(cr_EsString start) const
{
  EsStringArray vars = vm()->vars().allSymbolNamesGet();
  filterAndSort(vars, start);

  return vars;
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetainfoLookup::systemVarsFind(cr_EsString start) const
{
  EsStringArray vars = vm()->sysVars().allSymbolNamesGet();
  filterAndSort(
    vars, 
    start
  );

  return vars;
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetainfoLookup::constantsFind(cr_EsString start) const
{
  EsStringArray consts = vm()->consts().allSymbolNamesGet();
  filterAndSort(
    consts,
    start
  );

  return consts;
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetainfoLookup::constantsFindSyntaxRestricted(cr_EsString start) const
{
  EsStringArray consts = vm()->consts().allSymbolNamesGet();
  consts.erase(
    std::remove_if(
      consts.begin(),
      consts.end(),
      [](const EsString& item) -> bool {
        return EsString::npos != item.find(esT('?'));
      }
    ),
    consts.end()
  );

  filterAndSort(
    consts,
    start
  );

  return consts;
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetainfoLookup::localVarsFind(cr_EsString ns, cr_EsString mangledName, cr_EsString start) const
{
  EsMethodInfoKeyT key(mangledName);
  if( !key.isOk() )
    return EsString::nullArray();

  EsScriptCodeSection::Ptr pcode;

  if(ns.empty())
  {
    pcode = vm()->globalMethodGet(key, false);
    if( !pcode )
      return EsString::nullArray();
  }
  else
  {
    EsScriptObjectIntf::Ptr meta = vm()->metaclassFind(ns);
    if(!meta)
      return EsString::nullArray();

    pcode = meta->findScriptedMethod(key);
  }

  ES_ASSERT(pcode);

  EsStringArray vars = pcode->variablesAccess().allSymbolNamesGet();
  filterAndSort(vars, start);

  return vars;
}
//--------------------------------------------------------------------------------

EsStringArray EsScriptMetainfoLookup::keywordsFind(cr_EsString type, cr_EsString start) const
{
  static const EsStringArray sc_keywords0 = {
    esT("this"),
    esT("null"),
    esT("read"),
    esT("write"),
    esT("__startup"),
    esT("__value"),
    esT("__exception")
  };

  static const EsStringArray sc_keywords1 = {
    esT("const"),
    esT("var"),
    esT("extern"),
    esT("enum"),
    esT("object"),
    esT("extends")
  };

  static const EsStringArray sc_keywords2 = {
    esT("function"),
    esT("new"),
    esT("delete"),
    esT("switch"),
    esT("case"),
    esT("break"),
    esT("try"),
    esT("catch")
    esT("for"),
    esT("foreach"),
    esT("while"),
    esT("do")
  };

  static const EsStringArray sc_keywords3 = {
    esT("require"),
    esT("link")
  };

  EsStringArray result;

  EsString typesel = type;
  if(typesel.empty())
    typesel = esT("0123");

  if(EsString::npos != typesel.find(esT("0")))
    result.insert(
      result.end(),
      sc_keywords0.begin(),
      sc_keywords0.end()
    );
  if(EsString::npos != typesel.find(esT("1")))
    result.insert(
      result.end(),
      sc_keywords1.begin(),
      sc_keywords1.end()
    );
  if(EsString::npos != typesel.find(esT("2")))
    result.insert(
      result.end(),
      sc_keywords2.begin(),
      sc_keywords2.end()
    );
  if(EsString::npos != typesel.find(esT("3")))
    result.insert(
      result.end(),
      sc_keywords3.begin(),
      sc_keywords3.end()
    );

  filterAndSort(
    result, 
    start
  );

  return result;
}
//--------------------------------------------------------------------------------
