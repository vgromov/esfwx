#include "escorepch.h"
#pragma hdrstop

#include "EsExceptions.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
__fastcall
#endif
EsException::EsException(const EsException& src) :
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
Exception(src.m_msg.c_str()),
#endif
m_msg(src.m_msg),
m_data(src.m_data),
m_dbg(src.m_dbg),
m_code(src.m_code),
m_severity(src.m_severity),
m_facility(src.m_facility)
{
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  ES_DEBUG_TRACE(
    esT("Exception Stack Trace: %s"),
    esVS( StackTrace )
  );
#endif
}
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
__fastcall
#endif
EsException::EsException(
  ulong code, 
  ulong severity, 
  ulong facility, 
  const EsString& msg,
  const EsScriptDebugInfoIntf::Ptr& dbg /*= EsScriptDebugInfoIntf::Ptr()*/,
  bool doLogErrors /*= true*/,
  const EsVariant& payload /*= EsVariant::null()*/
) :
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
Exception(msg.c_str()),
#endif
m_msg(msg),
m_data(payload),
m_dbg(dbg),
m_code(code),
m_severity(severity),
m_facility(facility)
{
  if( doLogErrors )
    logEsException();

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  ES_DEBUG_TRACE(
    esT("Exception Stack Trace: %s"),
    esVS( StackTrace )
  );
#endif
}
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
__fastcall
#endif
EsException::EsException(
  const EsException& src, 
  const EsScriptDebugInfoIntf::Ptr& dbg
) :
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
Exception(src.m_msg.c_str()),
#endif
m_msg(src.m_msg),
m_data(src.m_data),
m_dbg(dbg),
m_code(src.m_code),
m_severity(src.m_severity),
m_facility(src.m_facility)
{
}
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
__fastcall EsException::~EsException()
{}
#endif
//---------------------------------------------------------------------------

void EsException::logEsException() const ES_NOTHROW
{
  try
  {
    EsScriptDebugInfoIntf::Ptr dbg = debugInfoGet();
    EsString dbgStr;
    if(dbg)
      dbgStr = dbg->asString();
    else
      dbgStr = esT("None");

    const EsString& details = EsString::format(
      esT("Code: %u, severity: %s, facility: %s, debug: '%s'"),
      codeGet(),
      severityStringGet(
        severityGet()
      ),
      facilityStringGet(
        facilityGet()
      ),
      dbgStr
    );

    if( EsException::severityWarning == severityGet() )
      EsUtilities::logWarning(
        messageGet(),
        details
      );
    else
      EsUtilities::logError(
        messageGet(),
        details
      );
  }
  catch(...) //< Do not allow anything to escape this function.
  {}
}
//---------------------------------------------------------------------------

ulong EsException::codeGet() const ES_NOTHROW
{
  return m_code;
}
//--------------------------------------------------------------------------------

ulong EsException::severityGet() const ES_NOTHROW
{
  return m_severity;
}
//--------------------------------------------------------------------------------

ulong EsException::facilityGet() const ES_NOTHROW
{
  return m_facility;
}
//--------------------------------------------------------------------------------

const EsString& EsException::messageGet() const ES_NOTHROW
{
  return m_msg;
}
//--------------------------------------------------------------------------------

const EsVariant& EsException::dataGet() const ES_NOTHROW
{
  return m_data;
}
//--------------------------------------------------------------------------------

EsScriptDebugInfoIntf::Ptr EsException::debugInfoGet() const ES_NOTHROW
{
  return m_dbg;
}
//--------------------------------------------------------------------------------

EsString EsException::severityStringGet(ulong severity) ES_NOTHROW
{
  if( EsEnumHasValue<EsExceptionSeverity>(severity) )
    return EsEnumLabelGet<EsExceptionSeverity>(severity);

  return _("Unknown");
}
//--------------------------------------------------------------------------------

EsString EsException::facilityStringGet(ulong facility) ES_NOTHROW
{
  if( EsEnumHasValue<EsExceptionFacility>(facility) )
    return EsEnumLabelGet<EsExceptionFacility>(facility);

  return _("Unknown");
}
//---------------------------------------------------------------------------

void EsException::Throw(ulong code, ulong severity, ulong facility, const EsString& fmt)
{
  throw EsException(code, severity, facility, EsString::format(fmt));
}

void EsException::Throw(ulong code, ulong severity, ulong facility, const EsString& fmt, const EsVariant& arg0)
{
  throw EsException(code, severity, facility, EsString::format(fmt, arg0));
}

void EsException::Throw(ulong code, ulong severity, ulong facility, const EsString& fmt, const EsVariant& arg0, const EsVariant& arg1)
{
  throw EsException(code, severity, facility, EsString::format(fmt, arg0, arg1));
}

void EsException::Throw(ulong code, ulong severity, ulong facility, const EsString& fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2)
{
  throw EsException(code, severity, facility, EsString::format(fmt, arg0, arg1, arg2));
}

void EsException::Throw(ulong code, ulong severity, ulong facility, const EsString& fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3)
{
  throw EsException(code, severity, facility, EsString::format(fmt, arg0, arg1, arg2, arg3));
}

void EsException::Throw(ulong code, ulong severity, ulong facility, const EsString& fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4)
{
  throw EsException(code, severity, facility, EsString::format(fmt, arg0, arg1, arg2, arg3, arg4));
}

void EsException::Throw(ulong code, ulong severity, ulong facility, const EsString& fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5)
{
  throw EsException(code, severity, facility, EsString::format(fmt, arg0, arg1, arg2, arg3, arg4, arg5));
}

void EsException::Throw(ulong code, ulong severity, ulong facility, const EsString& fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6)
{
  throw EsException(code, severity, facility, EsString::format(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6));
}

void EsException::Throw(ulong code, ulong severity, ulong facility, const EsString& fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7)
{
  throw EsException(code, severity, facility, EsString::format(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7));
}

void EsException::Throw(ulong code, ulong severity, ulong facility, const EsString& fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8)
{
  throw EsException(code, severity, facility, EsString::format(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
}

void EsException::Throw(const EsString& fmt)
{
  throw EsException(0, severityGeneric, facilityEsCommon, EsString::format(fmt));
}

void EsException::Throw(const EsString& fmt, const EsVariant& arg0)
{
  throw EsException(0, severityGeneric, facilityEsCommon, EsString::format(fmt, arg0));
}

void EsException::Throw(const EsString& fmt, const EsVariant& arg0, const EsVariant& arg1)
{
  throw EsException(0, severityGeneric, facilityEsCommon, EsString::format(fmt, arg0, arg1));
}

void EsException::Throw(const EsString& fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2)
{
  throw EsException(0, severityGeneric, facilityEsCommon, EsString::format(fmt, arg0, arg1, arg2));
}

void EsException::Throw(const EsString& fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3)
{
  throw EsException(0, severityGeneric, facilityEsCommon, EsString::format(fmt, arg0, arg1, arg2, arg3));
}

void EsException::Throw(const EsString& fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4)
{
  throw EsException(0, severityGeneric, facilityEsCommon, EsString::format(fmt, arg0, arg1, arg2, arg3, arg4));
}

void EsException::Throw(const EsString& fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5)
{
  throw EsException(0, severityGeneric, facilityEsCommon, EsString::format(fmt, arg0, arg1, arg2, arg3, arg4, arg5));
}

void EsException::Throw(const EsString& fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6)
{
  throw EsException(0, severityGeneric, facilityEsCommon, EsString::format(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6));
}

void EsException::Throw(const EsString& fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7)
{
  throw EsException(0, severityGeneric, facilityEsCommon, EsString::format(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7));
}

void EsException::Throw(const EsString& fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8)
{
  throw EsException(0, severityGeneric, facilityEsCommon, EsString::format(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
}
//---------------------------------------------------------------------------

void EsException::ThrowOsErrorStr(ulong code, const EsString& errstr, const EsString& fmt)
{
  if( !fmt.empty() )
    Throw(
      code,
      severityGeneric,
      facilityEsOS,
      fmt.c_str(),
      errstr
    );
  else
    Throw(
      code,
      severityGeneric,
      facilityEsOS,
      errstr.c_str()
    );
}
//---------------------------------------------------------------------------

void EsException::ThrowOsError(ulong code, const EsString& fmt)
{
  ThrowOsErrorStr(code, EsUtilities::osErrorStringGet(code), fmt);
}
//---------------------------------------------------------------------------

void EsException::ThrowNoValue(const EsString& ctx /*= EsString::null()*/)
{
  if( ctx.empty() )
    Throw(
      0,
      severityGeneric,
      facilityEsCommon,
      _("No value exists")
    );
  else
    Throw(
      0,
      severityGeneric,
      facilityEsCommon,
      esT("%s: '%s'"),
      ctx,
      _("No value exists")
    );
}
//---------------------------------------------------------------------------

void EsException::ThrowCannotIndexItem(const EsString& ctx /*= EsString::null()*/)
{
  if( ctx.empty() )
    Throw(
      0,
      severityGeneric,
      facilityEsCommon,
      _("Item is not of indexed type")
    );
  else
    Throw(
      0,
      severityGeneric,
      facilityEsCommon,
      esT("%s: '%s'"),
      ctx,
      _("Item is not of indexed type")
    );
}
//---------------------------------------------------------------------------

void EsException::ThrowNotSupportedForThisType(const EsString& op /*= EsString::null()*/)
{
  if( op.empty() )
    Throw(
      0,
      severityGeneric,
      facilityEsCommon,
      _("Operation is not supported for this type")
    );
  else
    Throw(
      0,
      severityGeneric,
      facilityEsCommon,
      _("Operation '%s' is not supported for this type"),
      op
    );
}
//---------------------------------------------------------------------------

void EsException::ThrowNotSupportedForThisType(const EsString& type, const EsString& op)
{
  Throw(
    0,
    severityGeneric,
    facilityEsCommon,
    _("Operation '%s' is not supported for type '%s'"),
    op,
    type
  );
}
//---------------------------------------------------------------------------

void EsException::ThrowDivisionByZero(const EsString& ctx /*= EsString::null()*/)
{
  if( ctx.empty() )
    Throw(
      0,
      severityGeneric,
      facilityEsNumeric,
      _("Division by zero encountered")
    );
  else
    Throw(
      0,
      severityGeneric,
      facilityEsNumeric,
      esT("%s: '%s'"),
      ctx,
      _("Division by zero encountered")
    );
}
//---------------------------------------------------------------------------

void EsException::ThrowOverflowInOperation(const EsString& op, const EsString& ctx /*= EsString::null()*/)
{
  if( ctx.empty() )
    Throw(
      0,
      severityGeneric,
      facilityEsNumeric,
      _("Overflow in operation '%s'"),
      op
    );
  else
    Throw(
      0,
      severityGeneric,
      facilityEsNumeric,
      _("%s: Overflow in operation '%s'"),
      ctx,
      op
    );
}
//---------------------------------------------------------------------------

void EsException::ThrowUnderflowInOperation(const EsString& op, const EsString& ctx /*= EsString::null()*/)
{
  if( ctx.empty() )
    Throw(0, severityGeneric, facilityEsNumeric, _("Underflow in operation '%s'"), op);
  else
    Throw(0, severityGeneric, facilityEsNumeric, _("%s: Underflow in operation '%s'"), ctx, op);
}

void EsException::ThrowObjectTypeIsIncompatibleWith(const EsString& className, const EsString& ctx /*= EsString::null()*/)
{
  if( ctx.empty() )
    Throw(0, severityGeneric, facilityEsCommon, _("Object type is incompatible with '%s'"), className);
  else
    Throw(0, severityGeneric, facilityEsCommon, _("%s: Object type is incompatible with '%s'"), ctx, className);
}

void EsException::ThrowUserDoesNotExist(const EsString& name, const EsString& ctx /*= EsString::null()*/)
{
  if( ctx.empty() )
    Throw(0, severityGeneric, facilityEsSecurity, _("User '%s' does not exist"), name);
  else
    Throw(0, severityGeneric, facilityEsSecurity, _("%s: User '%s' does not exist"), ctx, name);
}

void EsException::ThrowUserAlreadyExists(const EsString& name, const EsString& ctx /*= EsString::null()*/)
{
  if( ctx.empty() )
    Throw(0, severityGeneric, facilityEsSecurity, _("User '%s' already exists"), name);
  else
    Throw(0, severityGeneric, facilityEsSecurity, _("%s: User '%s' already exists"), ctx, name);
}

void EsException::ThrowAddingUsersWithAslIsNotAllowed(ulong asl, const EsString& ctx /*= EsString::null()*/)
{
  if( ctx.empty() )
    Throw(
      0,
      severityGeneric,
      facilityEsSecurity,
      _("Adding users with role '%s' is not allowed"),
      EsUtilities::aslStringGet( asl )
    );
  else
    Throw(
      0,
      severityGeneric,
      facilityEsSecurity,
      _("%s: Adding users with role '%s' is not allowed"),
      ctx,
      EsUtilities::aslStringGet( asl )
    );
}

void EsException::ThrowPasswordNotValid(const EsString& ctx /*= EsString::null()*/)
{
  if( ctx.empty() )
    Throw(
      0,
      severityGeneric,
      facilityEsSecurity,
      _("Password is not valid")
    );
  else
    Throw(
      0,
      severityGeneric,
      facilityEsSecurity,
      _("%s: Password is not valid"),
      ctx
    );
}

void EsException::ThrowUsersMayOnlyChangeThemselves(const EsString& ctx /*= EsString::null()*/)
{
  if( ctx.empty() )
    Throw(
      0,
      severityGeneric,
      facilityEsSecurity,
      _("Users with role '%s' may only change themselves"),
      EsUtilities::aslStringGet(static_cast<ulong>(EsAppSecurityLevel::aslUser))
    );
  else
    Throw(
      0,
      severityGeneric,
      facilityEsSecurity,
      _("%s: Users with role '%s' may only change themselves"),
      ctx,
      EsUtilities::aslStringGet(static_cast<ulong>(EsAppSecurityLevel::aslUser))
    );
}

void EsException::ThrowAttributeDoesNotExist(const EsString& attribute, const EsString& typeName, const EsString& ctx /*= EsString::null()*/)
{
  if( ctx.empty() )
    Throw(
      0,
      severityGeneric,
      facilityEsCommon,
      _("Attribute '%s' could not be found in '%s'"),
      attribute,
      typeName
    );
  else
    Throw(
      0,
      severityGeneric,
      facilityEsCommon,
      _("%s: Attribute '%s' could not be found in '%s'"),
      ctx,
      attribute,
      typeName
    );
}

void EsException::ThrowIncompatibleObjectTypes(const EsString& o1, const EsString& o2, const EsString& ctx /*= EsString::null()*/)
{
  if( ctx.empty() )      
    Throw(
      0, 
      severityGeneric, 
      facilityEsCommon, 
      _("Objects types '%s' and '%s' are incompatible"),
      o1,
      o2
    );
  else
    Throw(
      0, 
      severityGeneric, 
      facilityEsCommon, 
      _("%s: Objects types '%s' and '%s' are incompatible"),
      ctx,
      o1,
      o2
    );
}

void EsException::ThrowWrongObjectSize(const EsString& name, size_t expectedSize, size_t size, const EsString& ctx /*= EsString::null()*/)
{
  if( ctx.empty() )      
    Throw(
      0, 
      severityGeneric, 
      facilityEsCommon, 
      _("Object '%s' size is wrong, expected %u, got %u"),
      name,
      expectedSize, 
      size
    );
  else
    Throw(
      0, 
      severityGeneric, 
      facilityEsCommon, 
      _("%s: Object '%s' size is wrong, expected %u, got %u"),
      ctx,
      name,
      expectedSize, 
      size
    );
}

void EsException::ThrowIsNotAnObject(const EsString& ctx /*= EsString::null()*/)
{
  if( ctx.empty() )      
    Throw(
      0,
      severityGeneric,
      facilityEsCommon,
      _("Value is not an object instance")
    );
  else
    Throw(
      0,
      severityGeneric,
      facilityEsCommon,
      _("%s: Value is not an object instance"),
      ctx
    );
}

void EsException::ThrowObjectIsNotReflected(const EsString& ctx /*= EsString::null()*/)
{
  if( ctx.empty() )
    Throw(
      0,
      severityGeneric,
      facilityEsCommon,
      _("Object is not of reflected kind")
    );
  else
    Throw(
      0,
      severityGeneric,
      facilityEsCommon,
      _("%s: Object is not of reflected kind"),
      ctx
    );
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

// numeric checks reflected service
//
ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsNumericCheck, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsNumericCheck, checkRangeFloat, void_ClassCall_double_double_double_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsNumericCheck, checkRangeInteger, void_ClassCall_long_long_long_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsNumericCheck, checkRangeUInteger, void_ClassCall_ulong_ulong_ulong_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsNumericCheck, checkRangeLLInteger, void_ClassCall_llong_llong_llong_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsNumericCheck, checkRangeULLInteger, void_ClassCall_ullong_ullong_ullong_cr_EsString, NO_METHOD_DESCR)
ES_DECL_REFLECTED_SERVICES_INFO_END
//---------------------------------------------------------------------------

void EsNumericCheck::checkRangeFloat(double min, double max, double val, const EsString& name)
{
  if( val < min || val > max )
  {
    if( !name.empty() )
      EsException::Throw(
        0,
        EsException::severityGeneric,
        EsException::facilityEsNumeric,
        _("Value '%s'=%f is out of [%f, %f] range"),
        name,
        val,
        min,
        max
      );
    else
      EsException::Throw(
        0,
        EsException::severityGeneric,
        EsException::facilityEsNumeric,
        _("Value %f is out of [%f, %f] range"),
        val,
        min,
        max
      );
  }
}
//---------------------------------------------------------------------------

void EsNumericCheck::checkRangeInteger(long min, long max, long val, const EsString& name)
{
  if( val < min || val > max )
  {
    if( !name.empty() )
      EsException::Throw(
        0,
        EsException::severityGeneric,
        EsException::facilityEsNumeric,
        _("Value '%s'=%d is out of [%d, %d] range"),
        name,
        val,
        min,
        max
      );
    else
      EsException::Throw(
        0,
        EsException::severityGeneric,
        EsException::facilityEsNumeric,
        _("Value %d is out of [%d, %d] range"),
        val,
        min,
        max
      );
  }
}
//---------------------------------------------------------------------------

void EsNumericCheck::checkRangeUInteger(ulong min, ulong max, ulong val, const EsString& name)
{
  if( val < min || val > max )
  {
    if( !name.empty() )
      EsException::Throw(
        0,
        EsException::severityGeneric,
        EsException::facilityEsNumeric,
        _("Value '%s'=%u is out of [%u, %u] range"),
        name,
        val,
        min,
        max
      );
    else
      EsException::Throw(
        0,
        EsException::severityGeneric,
        EsException::facilityEsNumeric,
        _("Value %u is out of [%u, %u] range"),
        val,
        min,
        max
      );
  }
}
//---------------------------------------------------------------------------

void EsNumericCheck::checkRangeLLInteger(llong min, llong max, llong val, const EsString& name)
{
  if( val < min || val > max )
  {
    if( !name.empty() )
      EsException::Throw(
        0,
        EsException::severityGeneric,
        EsException::facilityEsNumeric,
        _("Value '%s'=%lld is out of [%lld, %lld] range"),
        name,
        val,
        min,
        max
      );
    else
      EsException::Throw(
        0,
        EsException::severityGeneric,
        EsException::facilityEsNumeric,
        _("Value %lld is out of [%lld, %lld] range"),
        val,
        min,
        max
      );
  }
}
//---------------------------------------------------------------------------

void EsNumericCheck::checkRangeULLInteger(ullong min, ullong max, ullong val, const EsString& name)
{
  if( val < min || val > max )
  {
    if( !name.empty() )
      EsException::Throw(
        0,
        EsException::severityGeneric,
        EsException::facilityEsNumeric,
        _("Value '%s'=%llu is out of [%llu, %llu] range"),
        name,
        val,
        min,
        max
      );
    else
      EsException::Throw(
        0,
        EsException::severityGeneric,
        EsException::facilityEsNumeric,
        _("Value %llu is out of [%llu, %llu] range"),
        val,
        min,
        max
      );
  }
}
//---------------------------------------------------------------------------
