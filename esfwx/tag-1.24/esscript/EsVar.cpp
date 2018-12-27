#include "esscriptpch.h"
#pragma hdrstop

#include "EsVar.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// variant reflected services
//
ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsVar, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, as, EsVariant_ClassCall_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, as, EsVariant_ClassCall_ulong_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, typeGet, ulong_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, countGet, ulong_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, itemGet, EsVariant_ClassCall_cr_EsVariant_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, itemDelete, void_ClassCall_cr_EsVariant_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, itemSet, void_ClassCall_cr_EsVariant_ulong_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, setToNull, void_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, value, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, asString, EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, asEscapedString, EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, asBinBuffer, EsBinBuffer_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, asBool, bool_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, asByte, esU8_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, asChar, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, asLong, long_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, asULong, ulong_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, asLLong, llong_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, asULLong, ullong_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, asDouble, double_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, isCollection, bool_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, isIndexed, bool_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, isObject, bool_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, isNumeric, bool_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, isEmpty, bool_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, isString, bool_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, isKindOf, bool_ClassCall_cr_EsVariant_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, countSet, void_ClassCall_cr_EsVariant_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, sliceGet, EsVariant_ClassCall_cr_EsVariant_long_long, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, find, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, findFirstOf, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, findLastOf, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, replace, void_ClassCall_cr_EsVariant_cr_EsVariant_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, append, void_ClassCall_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, prepend, void_ClassCall_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, appendUnique, void_ClassCall_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, sortAscending, void_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, sortDescending, void_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, reverse, void_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsVar, trace, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_REFLECTED_SERVICES_INFO_END

EsVariant EsVar::as(ulong type)
{
  if( !EsVariantType::instanceGet()->valueExists(type) )
    EsScriptException::ThrowValueIsNotMemberOfEnum(type, esT("EsVariantType"));

  EsVariant::Type t = static_cast<EsVariant::Type>(type);
  return EsVariant(t);
}

EsVariant EsVar::as(ulong type, ulong size)
{
  EsVariant result = as(type);
  if( !result.isIndexed() )
    EsException::ThrowCannotIndexItem();

  result.countSet(size);
  return result;
}

ulong EsVar::typeGet(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().typeGet();
  }

  return var.typeGet();
}

ulong EsVar::countGet(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().countGet();
  }

  return var.countGet();
}

EsVariant EsVar::itemGet(const EsVariant& var, ulong idx)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().itemGet(idx);
  }

  return var.itemGet(idx);
}

void EsVar::itemSet(const EsVariant& var, ulong idx, const EsVariant& val)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
    {
      if( acc->isReadOnly() )
        EsScriptException::ThrowPerformingOperationOnReadOnly(esT("itemSet"));

      if( !acc->getReturnsTemporary() )
      {
        EsVariant& v = acc->get();
        v.itemSet(idx, val);
        return;
      }
    }
  }

  EsScriptException::ThrowPerformingOperationOnTemporary(esT("itemSet"));
}

void EsVar::itemDelete(cr_EsVariant var, ulong idx)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
    {
      if( acc->isReadOnly() )
        EsScriptException::ThrowPerformingOperationOnReadOnly(esT("itemDelete"));

      if( !acc->getReturnsTemporary() )
      {
        EsVariant& v = acc->get();
        v.itemDelete(idx);
        return;
      }
    }
  }

  EsScriptException::ThrowPerformingOperationOnTemporary(esT("itemDelete"));
}

void EsVar::setToNull(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
    {
      if( acc->isReadOnly() )
        EsScriptException::ThrowPerformingOperationOnReadOnly(esT("setToNull"));

      if( !acc->getReturnsTemporary() )
      {
        acc->get().setToNull();
        return;
      }
    }
  }

  EsScriptException::ThrowPerformingOperationOnTemporary(esT("setToNull"));
}

EsVariant EsVar::value(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get();
  }

  return var;
}

EsString EsVar::asString(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().asString();
  }

  return var.asString();
}

EsString EsVar::asEscapedString(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().asEscapedString();
  }

  return var.asEscapedString();
}

EsBinBuffer EsVar::asBinBuffer(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().asBinBuffer();
  }

  return var.asBinBuffer();
}

bool EsVar::asBool(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().asBool();
  }

  return var.asBool();
}

esU8 EsVar::asByte(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().asByte();
  }

  return var.asByte();
}

EsVariant EsVar::asChar(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return EsVariant(
        acc->get().asChar(),
        EsVariant::ACCEPT_CHAR
      );
  }

  return EsVariant(
    var.asChar(),
    EsVariant::ACCEPT_CHAR
  );
}

long EsVar::asLong(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().asLong();
  }

  return var.asLong();
}

ulong EsVar::asULong(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().asULong();
  }

  return var.asULong();
}

llong EsVar::asLLong(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().asLLong();
  }

  return var.asLLong();
}

ullong EsVar::asULLong(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().asULLong();
  }

  return var.asULLong();
}

double EsVar::asDouble(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().asDouble();
  }

  return var.asDouble();
}

bool EsVar::isCollection(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().isCollection();
  }

  return var.isCollection();
}

bool EsVar::isIndexed(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().isIndexed();
  }

  return var.isIndexed();
}

bool EsVar::isObject(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().isObject();
  }

  return var.isObject();
}

bool EsVar::isNumeric(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().isNumeric();
  }

  return var.isNumeric();
}

bool EsVar::isEmpty(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().isEmpty();
  }

  return var.isEmpty();
}

bool EsVar::isString(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().isString();
  }

  return var.isString();
}

bool EsVar::isKindOf(const EsVariant& var, const EsString& type)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    EsReflectedClassIntf::Ptr ro;
    if( acc )
    {
      const EsVariant& v = acc->get();
      if( v.isObject() )
        ro = v.asObject();
    }
    else
      ro = var.asObject();

    return ro && ro->isKindOf(type);
  }

  return false;
}

void EsVar::countSet(const EsVariant& var, ulong newCount)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
    {
      if( acc->isReadOnly() )
        EsScriptException::ThrowPerformingOperationOnReadOnly(esT("countSet"));

      if( !acc->getReturnsTemporary() )
      {
        acc->get().countSet(newCount);
        return;
      }
    }
  }

  EsScriptException::ThrowPerformingOperationOnTemporary(esT("countSet"));
}

EsVariant EsVar::sliceGet(const EsVariant& var, long from, long to)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().sliceGet(from, to);
  }

  return var.sliceGet(from, to);
}

// extract
static const EsVariant* valueExtract(EsScriptValAccessorIntf::Ptr& acc, const EsVariant& val)
{
  const EsVariant* result = &val;
  if( val.isObject() )
  {
    acc = val.asObject();
    if( acc )
      result = &acc->get();
  }

  return result;
}

EsVariant EsVar::find(cr_EsVariant obj, cr_EsVariant subj)
{
  EsScriptValAccessorIntf::Ptr acc;
  const EsVariant* pobj = valueExtract(acc, obj);

  EsScriptValAccessorIntf::Ptr sacc;
  const EsVariant* psubj = valueExtract(sacc, subj);

  return pobj->find(*psubj);
}

EsVariant EsVar::findFirstOf(cr_EsVariant obj, cr_EsVariant subj)
{
  EsScriptValAccessorIntf::Ptr acc;
  const EsVariant* pobj = valueExtract(acc, obj);

  EsScriptValAccessorIntf::Ptr sacc;
  const EsVariant* psubj = valueExtract(sacc, subj);

  return pobj->findFirstOf(*psubj);
}

EsVariant EsVar::findLastOf(cr_EsVariant obj, cr_EsVariant subj)
{
  EsScriptValAccessorIntf::Ptr acc;
  const EsVariant* pobj = valueExtract(acc, obj);

  EsScriptValAccessorIntf::Ptr sacc;
  const EsVariant* psubj = valueExtract(sacc, subj);

  return pobj->findLastOf(*psubj);
}

// replace part of sequence, with another one, startimg from specified offset
void EsVar::replace(cr_EsVariant obj, cr_EsVariant subj, ulong offs)
{
  if( obj.isObject() )
  {
    EsScriptValAccessorIntf::Ptr sacc;
    const EsVariant* psubj = valueExtract(sacc, subj);

    EsScriptValAccessorIntf::Ptr acc = obj.asObject();
    if( acc )
    {
      if( acc->isReadOnly() )
        EsScriptException::ThrowPerformingOperationOnReadOnly(esT("replace"));

      if( !acc->getReturnsTemporary() )
      {
        EsVariant& v = acc->get();
        v.replace(*psubj, offs);
        return;
      }
    }
  }

  EsScriptException::ThrowPerformingOperationOnTemporary(esT("replace"));
}

// append an element to the end of the sequence. Behaves the same way as += operator
// but for variant collections, in which case it does not concat sequences, but
// inserts right sequence as an element of left one
void EsVar::append(cr_EsVariant obj, cr_EsVariant subj)
{
  if( obj.isObject() )
  {
    EsScriptValAccessorIntf::Ptr sacc;
    const EsVariant* psubj = valueExtract(sacc, subj);

    EsScriptValAccessorIntf::Ptr acc = obj.asObject();
    if( acc )
    {
      if( acc->isReadOnly() )
        EsScriptException::ThrowPerformingOperationOnReadOnly(esT("append"));

      if( !acc->getReturnsTemporary() )
      {
        EsVariant& v = acc->get();
        switch( v.typeGet() )
        {
        case EsVariant::VAR_STRING:
        case EsVariant::VAR_BIN_BUFFER:
        case EsVariant::VAR_STRING_COLLECTION:
          v += *psubj;
          break;
        case EsVariant::VAR_VARIANT_COLLECTION:
          v.addToVariantCollection(*psubj);
          break;
        default:
          EsScriptException::ThrowNotSupportedForThisType();
        }
        return;
      }
    }
  }

  EsScriptException::ThrowPerformingOperationOnTemporary(esT("append"));
}

void EsVar::prepend(cr_EsVariant obj, cr_EsVariant subj)
{
  if( obj.isObject() )
  {
    EsScriptValAccessorIntf::Ptr sacc;
    const EsVariant* psubj = valueExtract(sacc, subj);

    EsScriptValAccessorIntf::Ptr acc = obj.asObject();
    if( acc )
    {
      if( acc->isReadOnly() )
        EsScriptException::ThrowPerformingOperationOnReadOnly(esT("prepend"));

      if( !acc->getReturnsTemporary() )
      {
        EsVariant& v = acc->get();
        switch( v.typeGet() )
        {
        case EsVariant::VAR_STRING:
        case EsVariant::VAR_BIN_BUFFER:
        case EsVariant::VAR_STRING_COLLECTION:
          v = *psubj + v;
          break;
        case EsVariant::VAR_VARIANT_COLLECTION:
          v.addToVariantCollection(*psubj, true);
          break;
        default:
          EsScriptException::ThrowNotSupportedForThisType();
        }
        return;
      }
    }
  }

  EsScriptException::ThrowPerformingOperationOnTemporary(esT("prepend"));
}

/// Append unique element to the end of collection
void EsVar::appendUnique(cr_EsVariant obj, cr_EsVariant subj)
{
  if( obj.isObject() )
  {
    EsScriptValAccessorIntf::Ptr sacc;
    const EsVariant* psubj = valueExtract(sacc, subj);

    EsScriptValAccessorIntf::Ptr acc = obj.asObject();
    if( acc )
    {
      if( acc->isReadOnly() )
        EsScriptException::ThrowPerformingOperationOnReadOnly(esT("appendUnique"));

      if( !acc->getReturnsTemporary() )
      {
        EsVariant& v = acc->get();
        switch( v.typeGet() )
        {
        case EsVariant::VAR_STRING:
          {
            const EsString& ssubj = psubj->asString();
            const EsString& sobj = v.doInterpretAsString();

            if( EsString::npos != sobj.find(ssubj) )
              EsException::Throw(
                esT("String '%s' is already present in '%s'"),
                ssubj,
                sobj
              );
          }
          v += *psubj;
          break;
        case EsVariant::VAR_BIN_BUFFER:
          {
            const EsBinBuffer& bsubj = psubj->asBinBuffer();
            const EsBinBuffer& bobj = v.doInterpretAsBinBuffer();

            if( bobj.end() != std::search(bobj.begin(), bobj.end(), bsubj.begin(), bsubj.end()) )
              EsException::Throw(esT("Sequence is already present in buffer"));
          }
          v += *psubj;
          break;
        case EsVariant::VAR_STRING_COLLECTION:
          {
            const EsString& ssubj = psubj->asString();
            const EsString::Array& aobj = v.doInterpretAsStringCollection();

            if( aobj.end() != std::find(aobj.begin(), aobj.end(), ssubj) )
              EsException::Throw(
                esT("String '%s' is already present in string collection"),
                ssubj
              );
          }
          v += *psubj;
          break;
        case EsVariant::VAR_VARIANT_COLLECTION:
          {
            const EsVariant::Array& aobj = v.doInterpretAsVariantCollection();
            if( aobj.end() != std::find(aobj.begin(), aobj.end(), *psubj) )
              EsException::Throw(esT("Item is already present in collection"));
          }
          v.addToVariantCollection(*psubj);
          break;
        default:
          EsScriptException::ThrowNotSupportedForThisType();
        }
        return;
      }
    }
  }

  EsScriptException::ThrowPerformingOperationOnTemporary(esT("appendUnique"));
}

/// Sort elements collection
void EsVar::sortAscending(cr_EsVariant obj)
{
  if( obj.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = obj.asObject();
    if( acc )
    {
      if( acc->isReadOnly() )
        EsScriptException::ThrowPerformingOperationOnReadOnly(esT("sortAscending"));

      if( !acc->getReturnsTemporary() )
      {
        EsVariant& v = acc->get();
        v.sortAscending();
        return;
      }
    }
  }

  EsScriptException::ThrowPerformingOperationOnTemporary(esT("sortAscending"));
}

void EsVar::sortDescending(cr_EsVariant obj)
{
  if( obj.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = obj.asObject();
    if( acc )
    {
      if( acc->isReadOnly() )
        EsScriptException::ThrowPerformingOperationOnReadOnly(esT("sortDescending"));

      if( !acc->getReturnsTemporary() )
      {
        EsVariant& v = acc->get();
        v.sortDescending();
        return;
      }
    }
  }

  EsScriptException::ThrowPerformingOperationOnTemporary(esT("sortDescending"));
}

void EsVar::reverse(cr_EsVariant var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
    {
      if( acc->isReadOnly() )
        EsScriptException::ThrowPerformingOperationOnReadOnly(esT("reverse"));

      if( !acc->getReturnsTemporary() )
      {
        EsVariant& v = acc->get();
        v.reverse();
        return;
      }
    }
  }

  EsScriptException::ThrowPerformingOperationOnTemporary(esT("reverse"));
}

EsVariant EsVar::trace(cr_EsVariant var)
{
  if( var.isObject() )
  {
    EsScriptValAccessorIntf::Ptr acc = var.asObject();
    if( acc )
      return acc->get().trace();
  }

  return var.trace();
}
