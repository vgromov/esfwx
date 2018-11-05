#include "esscriptpch.h"
#pragma hdrstop

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#include "EsScriptCompiler.h"
#include "EsScriptlet.h"

#if (1 == ES_USE_COMPILER_IMPLEMENTATION)

ES_DECL_BASE_CLASS_INFO_BEGIN(EsScriptlet, NO_CLASS_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptlet, EsScriptletIntf, compile, bool_Call, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptlet, EsScriptletIntf, exec, EsVariant_Call, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptlet, EsScriptletIntf, exec, EsVariant_Call_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptlet, EsScriptletIntf, exec, EsVariant_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptlet, EsScriptletIntf, exec, EsVariant_Call_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptlet, EsScriptletIntf, exec, EsVariant_Call_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptlet, EsScriptletIntf, exec, EsVariant_Call_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsScriptlet, EsScriptletIntf, exec, EsVariant_Call_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	// reflected properties
	ES_DECL_PROP_INFO_RO(EsScriptlet, orphan, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RO(EsScriptlet, compiled, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RO(EsScriptlet, name, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RO(EsScriptlet, parametersCount, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RO(EsScriptlet, parameterNames, EsStringArray, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RO(EsScriptlet, source, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsScriptlet::EsScriptlet(EsScriptMachine& owner, const EsString& name, const EsString& src, const EsString& argNames) :
m_owner(owner),
m_source(src),
m_orphan(false)
{
	m_dynamic = true;

	if( name.empty() ){}
    // TODO: generate automatic name then

	ES_ASSERT(!src.empty());

	EsStringTokenizer tok(
    argNames,
    esT("\t ,;")
  );
	m_params.reserve(
    tok.get_remainingTokens()
  );

	while(tok.get_moreTokens())
	{
		const EsString& param = tok.get_nextToken();
		// todo: check identifier is valid
		m_params.push_back( param );
	}

	m_info = EsMethodInfoKeyT(
    m_params.size(),
    EsScriptlet::nameFormat(
      name,
      m_params.size()
    )
  );

	EsScriptMachine::deleteMethodFromMap(
    EsString::null(),
    m_owner.m_globalMethods,
    m_info
  );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsScriptlet::discard)()
{
	if( !m_orphan )
	{
		EsScriptMachine::deleteMethodFromMap(
      EsString::null(),
      m_owner.m_globalMethods,
      m_info
    );

		m_orphan = true;
	}
}
//---------------------------------------------------------------------------

EsScriptlet::~EsScriptlet()
{
	discard();
}
//---------------------------------------------------------------------------

EsString EsScriptlet::nameFormat(const EsString& name, size_t paramsCount)
{
	return EsString::format(
    esT("?%s&%d"),
    name,
    paramsCount
  );
}
//---------------------------------------------------------------------------

void EsScriptlet::checkIsNotOrphanOperation(const EsString& op) const
{
	if( m_orphan )
		EsScriptException::Throw(
      EsString::format(
        esT("'%s' may not be performed on orphan scriptlet"),
        op
      )
    );
}
//---------------------------------------------------------------------------

bool EsScriptlet::get_orphan() const
{
	return isOrphan();
}
//---------------------------------------------------------------------------

bool EsScriptlet::get_compiled() const
{
	return isCompiled();
}
//---------------------------------------------------------------------------

EsString EsScriptlet::get_name() const
{
	return nameGet();
}
//---------------------------------------------------------------------------

ulong EsScriptlet::get_parametersCount() const
{
	return parametersCountGet();
}
//---------------------------------------------------------------------------

EsStringArray EsScriptlet::get_parameterNames() const
{
	return parameterNamesGet();
}
//---------------------------------------------------------------------------

EsString EsScriptlet::get_source() const
{
	return sourceGet();
}
//---------------------------------------------------------------------------

bool EsScriptlet::isCompiled() const
{
	checkIsNotOrphanOperation(esT("isCompiled"));
	return m_owner.hasGlobalMethod(m_info);
}
//---------------------------------------------------------------------------

bool EsScriptlet::compile(const EsBreakIntf::Ptr& brk)
{
	ES_ASSERT( !m_source.empty() );
	checkIsNotOrphanOperation(esT("compile"));
	if( !isCompiled() )
		return EsScriptUtilities::compileFromScriptlet(
      *this,
      brk
    );

	return true;
}
//---------------------------------------------------------------------------

bool EsScriptlet::compile()
{
  return compile(
    EsBreakIntf::Ptr()
  );
}
//---------------------------------------------------------------------------

void EsScriptlet::parameterCountCheck(size_t cnt) const
{
	if(m_info.parametersCountGet() != cnt)
		EsScriptException::Throw(
      EsString::format(
        esT("Scriptlet wrong parameters count, expected %d, got %d"),
        m_info.parametersCountGet(),
        cnt
      )
    );
}
//---------------------------------------------------------------------------

EsVariant EsScriptlet::exec()
{
	checkIsNotOrphanOperation(esT("exec"));

	parameterCountCheck(0);
	compile();

	return m_owner.callGlobalMethod(
    m_info,
    EsVariant::null()
  );
}
//---------------------------------------------------------------------------

EsVariant EsScriptlet::exec(cr_EsVariant p0)
{
	checkIsNotOrphanOperation(esT("exec(p0)"));

	parameterCountCheck(1);
	compile();

  EsVariant::Array params = {
    p0
  };
	return m_owner.callGlobalMethod(
    m_info,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScriptlet::exec(cr_EsVariant p0, cr_EsVariant p1)
{
	checkIsNotOrphanOperation(esT("exec(p0, p1)"));

	parameterCountCheck(2);
	compile();

	EsVariant::Array params = {
    p0,
		p1
  };

	return m_owner.callGlobalMethod(
    m_info,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScriptlet::exec(cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2)
{
	checkIsNotOrphanOperation(esT("exec(p0, p1, p2)"));

	parameterCountCheck(3);
	compile();

	EsVariant::Array params = {
    p0,
		p1,
		p2
  };

	return m_owner.callGlobalMethod(
    m_info,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScriptlet::exec(cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3)
{
	checkIsNotOrphanOperation(esT("exec(p0, p1, p2, p3)"));

	parameterCountCheck(4);
	compile();

	EsVariant::Array params = {
    p0,
		p1,
		p2,
		p3
  };

	return m_owner.callGlobalMethod(
    m_info,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScriptlet::exec(cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4)
{
	checkIsNotOrphanOperation(esT("exec(p0, p1, p2, p3, p4)"));

	parameterCountCheck(5);
	compile();

	EsVariant::Array params = {
    p0,
		p1,
		p2,
		p3,
		p4
  };

	return m_owner.callGlobalMethod(
    m_info,
    params
  );
}
//---------------------------------------------------------------------------

EsVariant EsScriptlet::exec(cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4, cr_EsVariant p5)
{
	checkIsNotOrphanOperation(esT("exec(p0, p1, p2, p3, p4, p5)"));

	parameterCountCheck(6);
	compile();

	EsVariant::Array params = {
    p0,
		p1,
		p2,
		p3,
		p4,
		p5
  };

	return m_owner.callGlobalMethod(
    m_info,
    params
  );
}
//---------------------------------------------------------------------------

#endif // #if (1 == ES_USE_COMPILER_IMPLEMENTATION)

