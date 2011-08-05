#include <stdafx.h>
#pragma hdrstop

#include "EsDateTime.h"

ES_DECL_BASE_CLASS_INFO_BEGIN(EsDateTime, NO_CLASS_DESCRIPTION)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsDateTime, asString, EsString_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CTOR_INFO(EsDateTime, EsVariant_ClassCall, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsDateTime, now, EsVariant_ClassCall, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

EsDateTime::~EsDateTime() {}

EsVariant EsDateTime::now()
{
	EsDateTime dt;
	dt.m_dt = wxDateTime::Now();
	return dt;
}