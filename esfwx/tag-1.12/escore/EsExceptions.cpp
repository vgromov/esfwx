#include "StdAfx.h"
#pragma hdrstop

#include "EsExceptions.h"

void EsException::Throw(int code, EsException::Type type, EsString::const_pointer fmt, ...)
{
	va_list argptr;
	va_start(argptr, fmt);
	EsException ex(code, type, EsString::formatV(fmt, argptr));

	if( Warning == ex.getType() )
		EsUtilities::logWarning(ex.getMsg());
	else
		EsUtilities::logError(ex.getMsg());
	throw ex;
}

void EsException::ThrowNoValue()
{
	Throw(0, Generic, _("No value exists"));
}

void EsException::ThrowCannotIndexItem()
{
	Throw(0, Generic, _("Item is not of indexed type"));
}

void EsException::ThrowNotSupportedForThisType()
{
	Throw(0, Generic, _("Operation is not supported for this type"));
}

void EsException::ThrowDivisionByZero()
{
	Throw(0, Generic, _("Division by zero encountered"));
}

void EsException::ThrowOverflowInOperation(const EsString& op)
{
	Throw(0, Generic, _("Overflow in operation '%s'"), op.c_str());
}

void EsException::ThrowUnderflowInOperation(const EsString& op)
{
	Throw(0, Generic, _("Underflow in operation '%s'"), op.c_str());
}

void EsException::ThrowObjectTypeIsIncompatibleWith(const EsString& className)
{
	Throw(0, Generic, _("Object type is incompatible with '%s'"), className.c_str());
}

void EsException::ThrowUserDoesNotExist(const EsString& name)
{
	Throw(0, EsException::Generic, _("User '%s' does not exist"), name.c_str());
}

void EsException::ThrowUserAlreadyExists(const EsString& name)
{
	Throw(0, EsException::Generic, _("User '%s' already exists"), name.c_str());
}
	
void EsException::ThrowAddingUsersWithAslIsNotAllowed(AppSecurityLevel asl)
{
	EsException::Throw(0, EsException::Generic, _("Adding users with role '%s' is not allowed"), 
		EsUtilities::getAslString(asl).c_str() );
}

void EsException::ThrowPasswordNotValid()
{
	EsException::Throw(0, EsException::Generic, _("Password is not valid"));
}

void EsException::ThrowUsersMayOnlyChangeThemselves()
{
	EsException::Throw(0, EsException::Generic, _("Users with role '%s' may only change themselves"), 
		EsUtilities::getAslString(AslUser).c_str());
}

void EsException::ThrowPodObjectMayNotContainFields(const EsString& typeName)
{
	EsException::Throw(0, EsException::Generic,	wxT("Object '%s' is of POD type and may not contain fields"), 
		typeName.c_str());
}

void EsException::ThrowFinalObjectMayNotBeModified(const EsString& typeName)
{
	EsException::Throw(0, EsException::Generic,	wxT("Object '%s' is of final type and may not be modified"), 
		typeName.c_str());
}

void EsException::ThrowFieldAlreadyDeclared(const EsString& fieldName, const EsString& typeName)
{
	EsException::Throw(0, EsException::Generic, wxT("Multiple field declration. Field '%s' is already declared in '%s' or one of its base objects"),
		fieldName.c_str(), typeName.c_str());
}

void EsException::ThrowAttributeDoesNotExist(const EsString& attribute, const EsString& typeName)
{
	EsException::Throw(0, EsException::Generic, wxT("Attribute '%s' could not be found in '%s'"),
		attribute.c_str(), typeName.c_str());
}

// ------------------------------------------------------------------------------
// numeric checks
//
void EsNumericCheck::checkRangeFloat(double min, double max, double val, const EsString& name)
{
	if( val < min || val > max )
	{
		if( !name.empty() )
			EsException::Throw(0, EsException::Generic, _("Value '%s'=%f is out of [%f, %f] range"),
				name.c_str(), val, min, max);
		else
			EsException::Throw(0, EsException::Generic, _("Value %f is out of [%f, %f] range"),
				val, min, max);
	}
}

void EsNumericCheck::checkRangeInteger(long min, long max, long val, const EsString& name)
{
	if( val < min || val > max )
	{
		if( !name.empty() )
			EsException::Throw(0, EsException::Generic, _("Value '%s'=%d is out of [%d, %d] range"),
				name.c_str(), val, min, max);
		else
			EsException::Throw(0, EsException::Generic, _("Value %d is out of [%d, %d] range"),
				val, min, max);
	}
}
