#include "escorepch.h"
#pragma hdrstop

#include <algorithm>
#include "EsReflection.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

namespace EsReflectionCallProxies {

/// Member method call proxy function pointer
typedef void (*_proxyT)(EsVariant&, EsBaseIntf*, const EsMethodT&, const EsVariant& params);

/// Class method call proxy function pointer
typedef void (*_cproxyT)(EsVariant&, const EsMethodT&, const EsVariant& params);

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsAttributesIntf::Ptr EsAttributes::create(const EsString& ownerName, bool interlocked) ES_NOTHROW
{
	std::unique_ptr<EsAttributes> p(
    new EsAttributes(
      ownerName,
      interlocked
    )
  );
	ES_ASSERT(p.get());

  return p.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

/// EsBaseIntf implementation
EsString EsAttributes::typeNameGet() const ES_NOTHROW
{
	const EsString& owner = m_contents.nameGet();
	if( owner.empty() )
		return esT("EsAttributes");
	else
		return esT("EsAttributes of ") + owner;
}
//---------------------------------------------------------------------------

/// EsAttributesIntf implementation
EsAttributesIntf::Ptr EsAttributes::clone() const
{
	std::unique_ptr<EsAttributes> p(
    new EsAttributes(
      m_contents.nameGet(),
      m_contents.isInterlocked()
    )
  );
	ES_ASSERT(p.get());

	p->m_contents = m_contents;
	return p.release()->asBaseIntfPtrDirect();

}
//---------------------------------------------------------------------------

const EsString& EsAttributes::ownerNameGet() const ES_NOTHROW
{
	return m_contents.nameGet();
}
//---------------------------------------------------------------------------

void EsAttributes::clearAll() ES_NOTHROW
{
	m_contents.clear();
}
//---------------------------------------------------------------------------

EsStringArray EsAttributes::allNamesGet() const ES_NOTHROW
{
	return m_contents.namesGet();
}
//---------------------------------------------------------------------------

const EsVariant& EsAttributes::attributeGet(const EsString& name) const
{
	return m_contents.valueGet(name);
}
//---------------------------------------------------------------------------

bool EsAttributes::attributeExists(const EsString& name) const ES_NOTHROW
{
	return m_contents.itemExists(name);
}
//---------------------------------------------------------------------------

const EsVariant& EsAttributes::attributeGetDef(const EsString& name, const EsVariant& def) const ES_NOTHROW
{
	ulong idx = m_contents.itemFind(name);
	if( EsStringIndexedMap::npos != idx )
		return m_contents.valueGet(idx);
	else
		return def;
}
//---------------------------------------------------------------------------

EsAttributesIntf::Ptr EsAttributes::attributeSet(const EsString& name, const EsVariant& val, bool create /*= false*/)
{
	if( attributeExists(name) )
		m_contents.valueSet(name, val);
	else if( create )
		m_contents.itemAdd(name, val);
	else
		EsException::Throw(
      esT("Attribute '%s' does not exist in '%s'"),
      name,
      m_contents.nameGet()
    );

	return EsAttributesIntf::Ptr(this, false, false);
}
//---------------------------------------------------------------------------

EsAttributesIntf::Ptr EsAttributes::attributeAdd(const EsString& name, const EsVariant& val)
{
	m_contents.itemAdd(name, val);

	return EsAttributesIntf::Ptr(this, false, false);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// method info class implementation
//
EsMethodInfoKeyT::EsMethodInfoKeyT(const EsString& name, const EsVariant& params) ES_NOTHROW :
m_paramsCount(
  params.isEmpty() ?
    0 :
    (params.isVariantCollection() ?
      params.countGet() :
      1
    )
),
m_name(name)
{
  m_name.hashGet();
}
//---------------------------------------------------------------------------

EsMethodInfoKeyT::EsMethodInfoKeyT(const EsString& mangledName)
{
	fromMangledName(mangledName);
}
//---------------------------------------------------------------------------

void EsMethodInfoKeyT::fromMangledName(const EsString& mangledName)
{
	static EsRegEx s_re(
    esT("^([_a-zA-Z][0-9_a-zA-Z]*)\\|([0-9])$")
  );

	s_re.set_text(mangledName);
	if( s_re.get_matches() )
	{
		const EsString& name = s_re.matchGet(1);
		size_t paramsCount = EsString::toULong(s_re.matchGet(2));

		m_paramsCount = paramsCount;
		m_name = name;
	}
	else
		EsException::Throw(
      esT("Invalid method mangled name '%s'"),
      mangledName
    );
}
//---------------------------------------------------------------------------

bool EsMethodInfoKeyT::operator <(const EsMethodInfoKeyT& other) const ES_NOTHROW
{
	long cmpResult = m_name.compare(other.m_name);
	if( EsString::cmpLess == cmpResult )
		return true;
	else if( EsString::cmpEqual == cmpResult )
		return m_paramsCount < other.m_paramsCount;

	return false;
}
//---------------------------------------------------------------------------

EsString EsMethodInfoKeyT::asString() const
{
	ES_ASSERT(isOk());
	return EsString::format(
    esT("%s|%d"),
    m_name,
    m_paramsCount
  );
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

// signature strings map
//
EsMethodInfo::SigStringsT::~SigStringsT() ES_NOTHROW
{
  m_sigs.clear();
}
//---------------------------------------------------------------------------

EsMethodInfo::SigStringsT& EsMethodInfo::SigStringsT::add(MethodSignature sig, const EsString& str)
{
  m_sigs[sig] = str;
  return *this;
}
//---------------------------------------------------------------------------

const EsString& EsMethodInfo::SigStringsT::stringGet(int sig) const ES_NOTHROW
{
	SigsT::const_iterator cit = m_sigs.find(sig);
	if( cit != m_sigs.end() )
		return cit->second;
	else
		return EsString::null();
}
//---------------------------------------------------------------------------

// generate signature-string map
void EsMethodInfo::SigStringsT::init()
{
  (*this)
#include "EsReflectionDefSigStrings.hxx"
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Method info implementation
//
EsMethodInfo::EsMethodInfo(
  EsClassInfo& owner,
  int sig,
  esU8 paramsCount,
  const EsString& name,
  const EsString& descr,
  EsMemberCallT method
) ES_NOTHROW :
m_owner(owner),
m_name(name),
m_attrs(
  EsAttributes::create(
    name,
    false
  )
),
m_signature(sig),
m_paramsCount(paramsCount)
{
  m_method.m_method = method;

	ES_ASSERT( method );
	ES_ASSERT( sig > invalidSignature );
	ES_ASSERT( sig < methodSignaturesEnd );
	// calculate hash
	m_name.hashGet();
	if( !descr.empty() )
		m_attrs->attributeAdd(esT("help"), descr);
	owner.m_methods[ EsMethodInfoKeyT(m_paramsCount, m_name) ] = this;
}
//---------------------------------------------------------------------------

EsMethodInfo::EsMethodInfo(
  EsClassInfo& owner,
  int sig,
  esU8 paramsCount,
  const EsString& name,
  const EsString& descr,
  EsClassCallT classMethod,
  int ES_UNUSED(tagClassCallInfo)
) ES_NOTHROW :
m_owner(owner),
m_name(name),
m_attrs(
  EsAttributes::create(
    name,
    false
  )
),
m_signature(sig),
m_paramsCount(paramsCount)
{
  m_method.m_classMethod = classMethod;

	ES_ASSERT( classMethod );
	ES_ASSERT( sig > invalidSignature );
	ES_ASSERT( maxMethodSignature < sig && sig < classMethodSignaturesEnd );
	// calculate hash
	m_name.hashGet();
	if( !descr.empty() )
		m_attrs->attributeAdd(esT("help"), descr);
	owner.m_classMethods[ EsMethodInfoKeyT(m_paramsCount, m_name) ] = this;
}
//---------------------------------------------------------------------------

// initialize interface method info
EsMethodInfo::EsMethodInfo(
  EsClassInfo& owner,
  int sig,
  const EsIID& iid,
  esU8 paramsCount,
	const EsString& name,
  const EsString& descr,
  EsMemberCallT intfMethod
) ES_NOTHROW :
m_owner(owner),
m_name(name),
m_attrs(
  EsAttributes::create(
    name,
    false
  )
),
m_iid(iid),
m_signature(sig),
m_paramsCount(paramsCount)
{
  m_method.m_method = intfMethod;

	ES_ASSERT( intfMethod );
	ES_ASSERT( sig > invalidSignature );
	ES_ASSERT( sig < methodSignaturesEnd );
	// calculate hash
	m_name.hashGet();
	if( !descr.empty() )
		m_attrs->attributeAdd(esT("help"), descr);
	owner.m_methods[ EsMethodInfoKeyT(m_paramsCount, m_name) ] = this;
}
//---------------------------------------------------------------------------

EsMethodInfo::~EsMethodInfo() ES_NOTHROW
{}
//---------------------------------------------------------------------------

const EsMethodInfo::SigStringsT& EsMethodInfo::sigs() ES_NOTHROW
{
  static SigStringsT s_sigs;
  return s_sigs;
}
//---------------------------------------------------------------------------

EsString EsMethodInfo::descriptionGet() const
{
	EsString result = EsString::format(
    esT("%s(%s)"),
    m_name,
    signatureStringGet()
  );

	EsVariant help = m_attrs->attributeGetDef(
    esT("help"),
    EsVariant::null()
  );

	if( !help.isEmpty() )
		result += esT("\n") + esTranslationGet( help.asString() );

	return result;
}
//---------------------------------------------------------------------------

void EsMethodInfo::checkSignature(int sig)
{
	if( sig <= invalidSignature )
		EsException::Throw(esT("Invalid method signature value: %d"), sig);
	else if( sig >= methodSignaturesEnd && sig <= maxMethodSignature )
		EsException::Throw(esT("Unknown method signature: %d"), sig);
	else if( sig >= classMethodSignaturesEnd )
		EsException::Throw(esT("Unknown class method signature: %d"), sig);
}
//---------------------------------------------------------------------------

EsString EsMethodInfo::fqNameGet() const
{
  return isClassMethod() ?
    (m_owner.nameGet() + esT("::") + mangledNameGet() + esT("|") + signatureStringGet()) :
    (m_owner.nameGet() + esT(".") + mangledNameGet() + esT("|") + signatureStringGet());
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

// Caller methods
//
// Parameter checks
void EsMethodInfo::checkParamsCount(const EsVariant& params) const
{
	if( m_paramsCount )
	{
		if(
      (1 < m_paramsCount && EsVariant::VAR_VARIANT_COLLECTION != params.typeGet()) ||
			(1 == m_paramsCount && params.isEmpty())
    )
			EsException::Throw(
        esT("Parameter list expected in call to %s.%s of type %s"),
				m_owner.nameGet(),
        m_name,
        signatureStringGet()
      );
		else if(
      EsVariant::VAR_VARIANT_COLLECTION == params.typeGet() &&
			m_paramsCount != params.countGet()
    )
			EsException::Throw(
        esT("Wrong parameters count in call to %s.%s of type %s; expected %d, got %d"),
				m_owner.nameGet(),
        m_name,
        signatureStringGet(),
				m_paramsCount,
        params.countGet()
      );
	}
	else if(
    !params.isEmpty() &&
		!(params.typeGet() == EsVariant::VAR_VARIANT_COLLECTION && 0 == params.countGet())
  )
			EsException::Throw(
        esT("Method %s.%s of type %s does not expect parameters"),
				m_owner.nameGet(),
        m_name,
        signatureStringGet()
      );
}
//---------------------------------------------------------------------------

// Generate call proxies
#include "EsReflectionDefCallProxies.hxx"
#include "EsReflectionDefClassCallProxies.hxx"

// Generate call jump tables
#include "EsReflectionDefCalls.hxx"
#include "EsReflectionDefClassCalls.hxx"

// object member function caller
EsVariant EsMethodInfo::call(EsBaseIntf* obj, const EsVariant& params) const
{
	// assert it's normal method
	if( isClassMethod() )
		EsException::Throw(
      esT("The method %s.%s of type %s is class method"),
		  m_owner.nameGet(),
      m_name,
      signatureStringGet()
    );
	else if( !obj )
		EsException::Throw(
      esT("Cannot call method %s.%s of type %s without an object"),
		  m_owner.nameGet(),
      m_name,
      signatureStringGet()
    );

	checkParamsCount(params);

	EsVariant result;

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_MS || \
    ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_GNUC
	// cast to properly aligned interface pointer, if it's reflected interface method.
	if( !m_iid.empty() )
	{
		obj = (EsBaseIntf*)obj->requestIntf(
      m_iid,
      false
    );
		ES_ASSERT(obj);
	}
#endif

  // Call member jump table
  ES_ASSERT(m_signature > invalidSignature);
  ES_ASSERT(m_signature < methodSignaturesEnd);
  EsReflectionCallProxies::sc_memberProxies[m_signature](
    result,
    obj,
    m_method,
    params
  );

	return result;
}
//---------------------------------------------------------------------------

// class function caller
EsVariant EsMethodInfo::classCall(const EsVariant& params) const
{
	// assert it's class method
	if( !isClassMethod() )
		EsException::Throw(
		  esT("The method %s.%s of type %s is not a class method and cannot be called without an object"),
		  m_owner.nameGet(),
      m_name,
      signatureStringGet()
    );

	checkParamsCount(params);

	EsVariant result;

  // Call static members jump table
  ES_ASSERT( m_signature > maxMethodSignature );
  ES_ASSERT( m_signature < classMethodSignaturesEnd );

  int staticSig = m_signature-maxMethodSignature-1;
  EsReflectionCallProxies::sc_staticProxies[staticSig](
    result,
    m_method,
    params
  );

	return result;
}
//---------------------------------------------------------------------------

void EsMethodInfo::fqNameParse(
  const EsString& fqName,
  EsString& className,
  bool& isClassMethod,
  EsString& methodName,
  int& paramCount,
  EsString& signature
)
{
	static EsRegEx s_re(
    esT("^([_a-zA-Z][0-9_a-zA-Z]*)(\\:{2}|\\.)([_a-zA-Z][0-9_a-zA-Z]*)\\|([0-9])\\|([0-9_a-zA-Z]+)$")
  );

	s_re.set_text(fqName);

	if( !s_re.get_matches() )
		EsException::Throw(
      esT("Could not parse FQ name. Invalid FQ method name: '%s'"),
      fqName
    );

  className = s_re.matchGet(1);

  const EsString& qualifier = s_re.matchGet(2);
  isClassMethod = (esT("::") == qualifier);

  paramCount = EsString::toULong(s_re.matchGet(3));
  methodName = s_re.matchGet(4);
  signature = s_re.matchGet(5);
}
//---------------------------------------------------------------------------

const EsMethodInfo* EsMethodInfo::infoFromFqNameGet(const EsString& fqName, bool doThrow, bool allHierarchy /*= true*/)
{
  EsString className;
  bool isClassMethod;
  EsString methodName;
  int paramCount;
  EsString signature;

  EsMethodInfo::fqNameParse(
    fqName,
    className,
    isClassMethod,
    methodName,
    paramCount,
    signature
  );

	const EsClassInfo* info = EsClassInfo::classInfoGet(className, true);
  EsMethodInfoKeyT key(
    paramCount,
    methodName
  );

  const EsMethodInfo* result = isClassMethod ?
    info->classMethodInfoFind( key, allHierarchy ) :
    info->methodInfoFind( key, allHierarchy );

  if( !result && doThrow )
    EsException::Throw(
      esT("Could not find Method Info for specified FQ method name: '%s'"),
      fqName
    );

  return result;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

// prop info name comparison predicate
struct PropertyInfoNameIs
{
	PropertyInfoNameIs(const EsString& name) : m_name(name) {}
	inline bool operator() (const EsPropertyInfo* info) const
	{ ES_ASSERT(info); return m_name == info->nameGet(); }

	const EsString& m_name;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// EsPropertyInfo implementation
//
EsPropertyInfo::EsPropertyInfo(
  EsClassInfo& owner,
  const EsString& name,
  const EsString& label,
  const EsString& descr,
  const EsVariant& def,
  EsMemberCallT reader,
  int readerSig,
  EsMemberCallT writer,
  int writerSig,
  const EsVariant& restriction,
  bool persistent /*= false*/
) :
m_owner(owner),
m_name(name),
m_attrs(
  EsAttributes::create(
    name,
    false
  )
),
m_reader(reader),
m_readerSignature(readerSig),
m_writer(writer),
m_writerSignature(writerSig)
{
	// assert reader & writer signatures
	ES_ASSERT( m_reader ? (invalidSignature < readerSig && readerSig < methodSignaturesEnd) : true );
	ES_ASSERT( m_writer ? (invalidSignature < writerSig && writerSig < methodSignaturesEnd) : true );
	// calculate hash
	m_name.hashGet();
	// validate default value, if set
	if( !def.isEmpty() )
	{
		validate(def, true);
		m_attrs->attributeAdd(esT("default"), def);
	}
	if( !label.empty() )
		m_attrs->attributeAdd(esT("label"), label);
	if( !descr.empty() )
		m_attrs->attributeAdd(esT("help"), descr);
	if( !restriction.isEmpty() )
		m_attrs->attributeAdd(esT("restriction"), restriction);
	// ensure wee have both reader and writer if persistent flag is set
	ES_ASSERT( persistent ? persistent && canRead() && canWrite() : true );
	if( persistent && canRead() && canWrite() )
		m_attrs->attributeAdd(esT("persistent"), EsVariant::null());

	// append reflected property info to the metaclass
	owner.m_propInfos.push_back(this);
}
//---------------------------------------------------------------------------

bool EsPropertyInfo::isPersistent() const ES_NOTHROW
{
	return m_attrs->attributeExists(esT("persistent")) && canRead() && canWrite();
}
//---------------------------------------------------------------------------

// generic value access
EsVariant EsPropertyInfo::get(const EsBaseIntf* obj) const
{
	if( !obj )
		EsException::Throw(esT("Cannot get property without an object"));

	EsVariant result;

	// generate property getters
	#include "EsReflectionDefGetters.hxx"

	return result;
}
//---------------------------------------------------------------------------

void EsPropertyInfo::set(EsBaseIntf* obj, const EsVariant& val) const
{
	if( !obj )
		EsException::Throw(esT("Cannot set property without an object"));

	// generate setters
	#include "EsReflectionDefSetters.hxx"
}
//---------------------------------------------------------------------------

// reset property value to default, if there is one. if property has no default, exception is thrown
void EsPropertyInfo::reset(EsBaseIntf* obj) const
{
	if( defaultExists() )
		set(obj, defaultGet());
	else
		EsException::Throw(
      esT("Property '%s.%s' does not have default value"),
		  m_owner.nameGet(),
      nameGet()
    );
}
//---------------------------------------------------------------------------

const EsVariant& EsPropertyInfo::defaultGet() const
{
	return m_attrs->attributeGet(esT("default"));
}
//---------------------------------------------------------------------------

bool EsPropertyInfo::defaultExists() const ES_NOTHROW
{
	return m_attrs->attributeExists(esT("default"));
}
//---------------------------------------------------------------------------

EsString EsPropertyInfo::labelGet() const ES_NOTHROW
{
	EsVariant label = m_attrs->attributeGetDef(esT("label"), EsVariant::null());
	if( !label.isEmpty() )
		return esTranslationGet( label.asString().c_str() );

	return m_name;
}
//---------------------------------------------------------------------------

EsString EsPropertyInfo::descriptionGet() const ES_NOTHROW
{
	EsVariant help = m_attrs->attributeGetDef(esT("help"), EsVariant::null());
	if( !help.isEmpty() )
		return esT("\n") + esTranslationGet( help.asString() );

	return m_name;
}
//---------------------------------------------------------------------------

const EsVariant& EsPropertyInfo::restrictionGet() const
{
	return m_attrs->attributeGet(esT("restriction"));
}
//---------------------------------------------------------------------------

bool EsPropertyInfo::restrictionExists() const ES_NOTHROW
{
	return m_attrs->attributeExists(esT("restriction"));
}
//---------------------------------------------------------------------------

bool EsPropertyInfo::validate(const EsVariant &val, bool doThrow/* = false*/) const
{
	if( restrictionExists() )
	{
		bool result = restrictionGet().has(val);
		if( !result && doThrow )
			EsException::Throw(
				esT("Property '%s' value does not comply with restriction"),
				m_name
      );

		return result;
	}

	return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Class info metaclass implementation
//
EsClassInfo::EsClassInfo(const EsString& name, const EsString& descr, const EsClassInfo* ancestor) ES_NOTHROW :
m_ancestor(ancestor),
m_name(name),
m_attrs(
  EsAttributes::create(
    name,
    false
  )
)
{
	if( !descr.empty() )
		m_attrs->attributeAdd(esT("help"), descr);

  doRegister();
}
//---------------------------------------------------------------------------

EsClassInfo::~EsClassInfo() ES_NOTHROW
{
  try
  {
    unregister();

    m_propInfos.clear();
    m_methods.clear();
    m_classMethods.clear();
  }
  catch(...)
  {
    // Do not allow anything to escape
  }
}
//---------------------------------------------------------------------------

void EsClassInfo::doRegister(EsClassRegistryAccessorT acc /*= nullptr*/) const
{
  if( !acc )
  {
    if( !classes().itemExists(m_name) )
      classes().itemAdd(
        m_name,
        EsVariant(
          static_cast<void*>(
            const_cast<EsClassInfo*>(this)
          ),
          EsVariant::ACCEPT_POINTER
        )
      );
  }
  else
  {
    if( !acc().itemExists(m_name) )
      acc().itemAdd(
        m_name,
        EsVariant(
          static_cast<void*>(
            const_cast<EsClassInfo*>(this)
          ),
          EsVariant::ACCEPT_POINTER
        )
      );
  }
}
//---------------------------------------------------------------------------

void EsClassInfo::unregister(EsClassRegistryAccessorT acc /*= nullptr*/) const
{
  if( !acc )
  {
    if( classes().itemExists(m_name) )
      classes().itemDelete(m_name);
  }
  else
  {
    if( acc().itemExists(m_name) )
      acc().itemDelete(m_name);
  }
}
//---------------------------------------------------------------------------

// inheritance access
bool EsClassInfo::isKindOf(const EsString& name) const ES_NOTHROW
{
	// check self
	if( name == m_name )
		return true;
	else
	{
		// look-up ancestors
		const EsClassInfo* ancestor = m_ancestor;
		while( ancestor )
		{
			const EsString& aname = ancestor->nameGet();
			if( name == aname )
				return true;
			ancestor = ancestor->m_ancestor;
		}
	}

	return false;
}
//---------------------------------------------------------------------------

 EsString EsClassInfo::descriptionGet() const ES_NOTHROW
{
	EsVariant help = m_attrs->attributeGetDef(esT("help"), EsVariant::null());
	if( help.isEmpty() )
		return m_name;
	else
		return esTranslationGet(help.asString().c_str());
}
//---------------------------------------------------------------------------

// Reflected information access
//
// property names enumeration helper
void EsClassInfo::appendPropertyNames(EsString::Array& out, bool onlyPersistent, bool allHierarchy) const
{
	if( allHierarchy && m_ancestor )
		m_ancestor->appendPropertyNames(out, onlyPersistent, allHierarchy);

	for(PropertyInfosT::const_iterator cit = m_propInfos.begin(); cit != m_propInfos.end(); ++cit)
	{
		if( onlyPersistent )
		{
			if( (*cit)->isPersistent() )
				out.push_back( (*cit)->nameGet() );
		}
		else
			out.push_back( (*cit)->nameGet() );
	}
}
//---------------------------------------------------------------------------

void EsClassInfo::appendPropertyInfos(PropertyInfosT& out, bool onlyPersistent, bool allHierarchy) const
{
	if( allHierarchy && m_ancestor )
		m_ancestor->appendPropertyInfos(out, onlyPersistent, allHierarchy);

	for(PropertyInfosT::const_iterator cit = m_propInfos.begin(); cit != m_propInfos.end(); ++cit)
	{
		if( onlyPersistent )
		{
			if( (*cit)->isPersistent() )
				out.push_back( *cit );
		}
		else
			out.push_back( *cit );
	}
}
//---------------------------------------------------------------------------

EsString::Array EsClassInfo::propertyNamesGet(bool onlyPersistent /*= false*/, bool allHierarchy /*= true*/) const ES_NOTHROW
{
	EsString::Array result;
	appendPropertyNames(result, onlyPersistent, allHierarchy);

	return result;
}
//---------------------------------------------------------------------------

EsClassInfo::PropertyInfosT EsClassInfo::propertyInfosGet(bool onlyPersistent /*= false*/, bool allHierarchy /*= true*/) const ES_NOTHROW
{
	PropertyInfosT result;
	appendPropertyInfos(result, onlyPersistent, allHierarchy);

	return result;
}
//---------------------------------------------------------------------------

const EsPropertyInfo* EsClassInfo::propertyInfoFind(const EsString& name, bool allHierarchy) const ES_NOTHROW
{
	PropertyInfosT::const_iterator cit = std::find_if(m_propInfos.begin(), m_propInfos.end(),
		PropertyInfoNameIs(name) );

	if( cit != m_propInfos.end() )
		return *cit;
	else if( allHierarchy && m_ancestor )
		return m_ancestor->propertyInfoFind(name, allHierarchy);

	return 0;
}
//---------------------------------------------------------------------------

bool EsClassInfo::hasProperty(const EsString& name, bool allHierarchy /*= true*/) const ES_NOTHROW
{
	return propertyInfoFind(name, allHierarchy) != 0;
}
//---------------------------------------------------------------------------

const EsPropertyInfo& EsClassInfo::propertyInfoGet(const EsString& name, bool allHierarchy /*= true*/ ) const
{
	const EsPropertyInfo* result = propertyInfoFind(name, allHierarchy);
	if( !result )
		EsException::Throw(
      esT("There is no '%s' property in class '%s'"),
			name,
      m_name
    );

	return *result;
}
//---------------------------------------------------------------------------

// reflected method infos access
void EsClassInfo::appendMethodKeys(EsMethodInfoKeysT& out, bool allHierarchy) const
{
	if( allHierarchy && m_ancestor )
		m_ancestor->appendMethodKeys(out, allHierarchy);
	else
	{
		for(MethodInfosT::const_iterator cit = m_methods.begin(); cit != m_methods.end(); ++cit)
			out.push_back( cit->first );
	}
}
//---------------------------------------------------------------------------

void EsClassInfo::appendClassMethodKeys(EsMethodInfoKeysT& out, bool allHierarchy) const
{
	if( allHierarchy && m_ancestor )
		m_ancestor->appendClassMethodKeys(out, allHierarchy);
	else
	{
		for(MethodInfosT::const_iterator cit = m_classMethods.begin(); cit != m_classMethods.end(); ++cit)
			out.push_back( cit->first );
	}
}
//---------------------------------------------------------------------------

const EsMethodInfo* EsClassInfo::methodInfoFind(const EsMethodInfoKeyT& key, bool allHierarchy) const ES_NOTHROW
{
	MethodInfosT::const_iterator cit = m_methods.find(key);
	if( cit != m_methods.end() )
		return cit->second;
	else if( allHierarchy && m_ancestor )
		return m_ancestor->methodInfoFind(key, allHierarchy);

	return 0;
}
//---------------------------------------------------------------------------

const EsMethodInfo* EsClassInfo::classMethodInfoFind(const EsMethodInfoKeyT& key, bool allHierarchy) const ES_NOTHROW
{
	MethodInfosT::const_iterator cit = m_classMethods.find(key);
	if( cit != m_classMethods.end() )
		return cit->second;
	else if( allHierarchy && m_ancestor )
		return m_ancestor->methodInfoFind(key, allHierarchy);

	return 0;
}
//---------------------------------------------------------------------------

EsMethodInfoKeysT EsClassInfo::methodKeysGet(bool allHierarchy /*= true*/) const ES_NOTHROW
{
	EsMethodInfoKeysT result;
	result.reserve(m_methods.size() + 32);

	appendMethodKeys(result, allHierarchy);

	return result;
}
//---------------------------------------------------------------------------

bool EsClassInfo::hasMethod( const EsMethodInfoKeyT& key, bool allHierarchy /*= true*/ ) const ES_NOTHROW
{
	const EsMethodInfo* info = methodInfoFind(key, allHierarchy);
	return info != 0;
}
//---------------------------------------------------------------------------

const EsMethodInfo& EsClassInfo::methodInfoGet(const EsMethodInfoKeyT& key, bool allHierarchy /*= true*/ ) const
{
	const EsMethodInfo* info = methodInfoFind(key, allHierarchy);
	if( !info )
		EsException::Throw(
      esT("Method %s, taking %d parameters, is not found in class %s"),
			key.nameGet(),
      key.parametersCountGet(),
      nameGet()
    );

	return *info;
}
//---------------------------------------------------------------------------

EsMethodInfoKeysT EsClassInfo::classMethodKeysGet(bool allHierarchy /*= false*/) const ES_NOTHROW
{
	EsMethodInfoKeysT result;
	result.reserve(m_classMethods.size() + 32);

	appendClassMethodKeys(result, allHierarchy);

	return result;
}
//---------------------------------------------------------------------------

EsString::Array EsClassInfo::fqMethodNamesGet(bool allHierarchy /*= true*/) const ES_NOTHROW
{
	EsMethodInfoKeysT mk;
	EsMethodInfoKeysT cmk;

  mk.reserve(m_methods.size() + 32);
  cmk.reserve(m_classMethods.size() + 32);

  appendMethodKeys(
    mk,
    allHierarchy
  );

  appendClassMethodKeys(
    cmk,
    allHierarchy
  );

  EsString::Array result;
  result.reserve(mk.size() + cmk.size());

  for(auto const& v: cmk)
  {
    const EsMethodInfo& info = classMethodInfoGet(
      v,
      allHierarchy
    );

    result.push_back(
      info.fqNameGet()
    );
  }

  for(auto const& v: mk)
  {
    const EsMethodInfo& info = methodInfoGet(
      v,
      allHierarchy
    );

    result.push_back(
      info.fqNameGet()
    );
  }

  return result;
}
//---------------------------------------------------------------------------

bool EsClassInfo::hasClassMethod( const EsMethodInfoKeyT& key, bool allHierarchy /*= true*/ ) const ES_NOTHROW
{
	const EsMethodInfo* info = classMethodInfoFind(key, allHierarchy);
	return info != nullptr;
}
//---------------------------------------------------------------------------

const EsMethodInfo& EsClassInfo::classMethodInfoGet(const EsMethodInfoKeyT& key, bool allHierarchy /*= false*/ ) const
{
	const EsMethodInfo* info = classMethodInfoFind(key, allHierarchy);
	if( !info )
		EsException::Throw(
      esT("Class method %s, taking %d parameters, is not found in class %s"),
			key.nameGet(),
      key.parametersCountGet(),
      nameGet()
    );

	return *info;
}
//---------------------------------------------------------------------------

// property services simplified
EsVariant EsClassInfo::propertyGet(EsBaseIntf* obj, const EsString& name) const
{
	const EsPropertyInfo& info = propertyInfoGet(name);
	return info.get(obj);
}

void EsClassInfo::propertySet(EsBaseIntf* obj, const EsString& name, const EsVariant& val) const
{
	const EsPropertyInfo& info = propertyInfoGet(name);
	info.set(obj, val);
}

void EsClassInfo::propertyReset(EsBaseIntf* obj, const EsString& name) const
{
	const EsPropertyInfo& info = propertyInfoGet(name);
	return info.reset(obj);
}

void EsClassInfo::resetAllProperties(EsBaseIntf* obj) const
{
	const PropertyInfosT& allProps = propertyInfosGet();
	for(PropertyInfosT::const_iterator cit = allProps.begin(); cit != allProps.end(); ++cit)
	{
		if( (*cit)->defaultExists() )
			(*cit)->reset(obj);
	}
}

// return true if standard indexed services are found
// countGet() and itemGet(index) and | or itemSet(index, value)
//
bool EsClassInfo::isIndexed() const ES_NOTHROW
{
	return hasMethod(
    EsMethodInfoKeyT(
      0,
      EsStdNames::countGet()
    )
  ) &&
	(
    hasMethod(
      EsMethodInfoKeyT(
        1,
        EsStdNames::itemGet()
      )
    ) ||
    hasMethod(
      EsMethodInfoKeyT(
        2,
        EsStdNames::itemSet()
      )
    )
  );
}

// method services simplified
EsVariant EsClassInfo::call(EsBaseIntf* obj, const EsString& name) const
{
	const EsMethodInfo& info = methodInfoGet(EsMethodInfoKeyT(0, name));
	return info.call(obj, EsVariant::null());
}

EsVariant EsClassInfo::call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1) const
{
	const EsMethodInfo& info = methodInfoGet(
    EsMethodInfoKeyT(
      1,
      name
    )
  );

  EsVariant::Array params;
	params.push_back(param1);

	return info.call(
    obj,
    params
  );
}

EsVariant EsClassInfo::call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2) const
{
	const EsMethodInfo& info = methodInfoGet(EsMethodInfoKeyT(2, name));
	EsVariant::Array params;
  params.reserve(2);

	params.push_back(param1);
  params.push_back(param2);

	return info.call(
    obj,
    params
  );
}

EsVariant EsClassInfo::call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) const
{
	const EsMethodInfo& info = methodInfoGet(EsMethodInfoKeyT(3, name));
	EsVariant::Array params;
  params.reserve(3);

	params.push_back(param1);
  params.push_back(param2);
  params.push_back(param3);

	return info.call(
    obj,
    params
  );
}

EsVariant EsClassInfo::call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) const
{
	const EsMethodInfo& info = methodInfoGet(EsMethodInfoKeyT(4, name));
	EsVariant::Array params;
  params.reserve(4);

	params.push_back(param1);
  params.push_back(param2);
  params.push_back(param3);
	params.push_back(param4);

	return info.call(
    obj,
    params
  );
}

EsVariant EsClassInfo::call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) const
{
	const EsMethodInfo& info = methodInfoGet(EsMethodInfoKeyT(5, name));
	EsVariant::Array params;
  params.reserve(5);

	params.push_back(param1);
  params.push_back(param2);
  params.push_back(param3);
	params.push_back(param4);
	params.push_back(param5);

	return info.call(
    obj,
    params
  );
}

EsVariant EsClassInfo::call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6) const
{
	const EsMethodInfo& info = methodInfoGet(EsMethodInfoKeyT(6, name));
	EsVariant::Array params;
  params.reserve(6);

	params.push_back(param1);
  params.push_back(param2);
  params.push_back(param3);
	params.push_back(param4);
	params.push_back(param5);
	params.push_back(param6);

	return info.call(
    obj,
    params
  );
}

EsVariant EsClassInfo::callMethod(EsBaseIntf* obj, const EsString& name, const EsVariant& params) const
{
	int paramsCount = params.isEmpty() ?
    0 :
    (
      params.isVariantCollection() ?
        params.countGet() :
        1
    );

	const EsMethodInfo& info = methodInfoGet(
    EsMethodInfoKeyT(
      paramsCount,
      name
    )
  );

  return info.call(
    obj,
    params
  );
}

EsVariant EsClassInfo::classCall(const EsString& name) const
{
	const EsMethodInfo& info = classMethodInfoGet(EsMethodInfoKeyT(0, name));
	return info.classCall(EsVariant::null());
}

EsVariant EsClassInfo::classCall(const EsString& name, const EsVariant& param1) const
{
	const EsMethodInfo& info = classMethodInfoGet(EsMethodInfoKeyT(1, name));
	EsVariant::Array params;
	params.push_back(param1);

	return info.classCall(params);
}

EsVariant EsClassInfo::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2) const
{
	const EsMethodInfo& info = classMethodInfoGet(EsMethodInfoKeyT(2, name));
	EsVariant::Array params;
  params.reserve(2);

	params.push_back(param1);
  params.push_back(param2);

	return info.classCall(params);
}

EsVariant EsClassInfo::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) const
{
	const EsMethodInfo& info = classMethodInfoGet(EsMethodInfoKeyT(3, name));
	EsVariant::Array params;
  params.reserve(3);

	params.push_back(param1);
  params.push_back(param2);
	params.push_back(param3);

	return info.classCall(params);
}

EsVariant EsClassInfo::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) const
{
	const EsMethodInfo& info = classMethodInfoGet(EsMethodInfoKeyT(4, name));
	EsVariant::Array params;
  params.reserve(4);

	params.push_back(param1);
  params.push_back(param2);
	params.push_back(param3);
	params.push_back(param4);

	return info.classCall(params);
}

EsVariant EsClassInfo::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) const
{
	const EsMethodInfo& info = classMethodInfoGet(EsMethodInfoKeyT(5, name));
	EsVariant::Array params;
  params.reserve(5);

	params.push_back(param1);
  params.push_back(param2);
	params.push_back(param3);
	params.push_back(param4);
  params.push_back(param5);

	return info.classCall(params);
}

EsVariant EsClassInfo::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6) const
{
	const EsMethodInfo& info = classMethodInfoGet(EsMethodInfoKeyT(6, name));
	EsVariant::Array params;
  params.reserve(6);

	params.push_back(param1);
  params.push_back(param2);
	params.push_back(param3);
	params.push_back(param4);
  params.push_back(param5);
  params.push_back(param6);

	return info.classCall(params);
}

EsVariant EsClassInfo::classCallMethod(const EsString& name, const EsVariant& params) const
{
	int paramsCount =
    params.isEmpty() ?
      0 :
      (
        params.isVariantCollection() ?
          params.countGet() :
          1
      );

	const EsMethodInfo& info = classMethodInfoGet(
    EsMethodInfoKeyT(
      paramsCount,
      name
    )
  );

  return info.classCall(params);
}

// static services. registered reflected classes access
//
// use local static init on first use idiom for the class info registry
EsStringIndexedMap& EsClassInfo::classes() ES_NOTHROW
{
	static EsStringIndexedMap s_classes;
	return s_classes;
}
//---------------------------------------------------------------------------

EsString::Array EsClassInfo::classNamesGet() ES_NOTHROW
{
	return classes().namesGet();
}
//---------------------------------------------------------------------------

const EsClassInfo* EsClassInfo::classInfoGet(const EsString& name, bool Throw/* = false*/ )
{
	const EsClassInfo* result = 0;
	ulong idx = classes().itemFind(name);
	if( EsStringIndexedMap::npos != idx )
	{
		result = (const EsClassInfo*)classes().valueGet(idx).asPointer();
		ES_ASSERT(result);
	}
	else if( Throw )
		EsException::Throw(
      esT("Class '%s' is not registered"),
      name
    );

	return result;
}
//---------------------------------------------------------------------------

