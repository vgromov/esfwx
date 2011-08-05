#include "stdafx.h"
#pragma hdrstop

#include "EsScriptDebug.h"

// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsScriptDebug, NO_CLASS_DESCRIPTION)
	// reflected service infos declarations
	//
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsScriptDebug, log, void_ClassCall_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsScriptDebug, log, void_ClassCall_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsScriptDebug, log, void_ClassCall_cr_EsString_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsScriptDebug, log, void_ClassCall_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)	
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsScriptDebug, log, void_ClassCall_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)	
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsScriptDebug, log, void_ClassCall_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)	
ES_DECL_CLASS_INFO_END

static void logDebug(const EsString& msg)
{
	wxPrintf( wxT("## %s\n"), msg.c_str() );
}

void EsScriptDebug::log(cr_EsString p1)
{
	logDebug( p1 );
}

void EsScriptDebug::log(cr_EsString p1, cr_EsVariant p2)
{
	EsVariant args(EsVariant::VAR_VARIANT_COLLECTION);
	args.addToVariantCollection(p2);
	logDebug( EsVariant::format(p1, args).asString() );
}

void EsScriptDebug::log(cr_EsString p1, cr_EsVariant p2, cr_EsVariant p3)
{
	EsVariant args(EsVariant::VAR_VARIANT_COLLECTION);
	args.addToVariantCollection(p2)
		.addToVariantCollection(p3);
	logDebug( EsVariant::format(p1, args).asString() );
}

void EsScriptDebug::log(cr_EsString p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4)
{
	EsVariant args(EsVariant::VAR_VARIANT_COLLECTION);
	args.addToVariantCollection(p2)
		.addToVariantCollection(p3)
		.addToVariantCollection(p4);
	logDebug( EsVariant::format(p1, args).asString() );
}

void EsScriptDebug::log(cr_EsString p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4, cr_EsVariant p5)
{
	EsVariant args(EsVariant::VAR_VARIANT_COLLECTION);
	args.addToVariantCollection(p2)
		.addToVariantCollection(p3)
		.addToVariantCollection(p4)
		.addToVariantCollection(p5);
	logDebug( EsVariant::format(p1, args).asString() );
}

void EsScriptDebug::log(cr_EsString p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4, cr_EsVariant p5, cr_EsVariant p6)
{
	EsVariant args(EsVariant::VAR_VARIANT_COLLECTION);
	args.addToVariantCollection(p2)
		.addToVariantCollection(p3)
		.addToVariantCollection(p4)
		.addToVariantCollection(p5)
		.addToVariantCollection(p6);
	logDebug( EsVariant::format(p1, args).asString() );
}
