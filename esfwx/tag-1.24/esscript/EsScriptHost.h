#ifndef _es_script_host_h_
#define _es_script_host_h_

// scripting host reflected proxy class
//
class ESSCRIPT_CLASS ES_INTF_IMPL1_NON_RC( EsScriptHost, EsReflectedClassIntf )

protected:
  EsScriptHost(const EsScriptMachine& owner);

public:
  ES_DECL_REFLECTED_CLASS_BASE(EsScriptHost)
  ES_DECL_ATTR_HANDLING_STD

  // todo: misc versions data
  //
  // event exchange services
  //
  // post event from script, given category, id, data
  ES_DECL_REFLECTED_METHOD3(void, eventPost, cr_EsString, ulong, cr_EsVariant);
  // post urget event from script
  ES_DECL_REFLECTED_METHOD3(void, eventPostUrgent, cr_EsString, ulong, cr_EsVariant);
  // same as above, but "generic" is used as category
  ES_DECL_REFLECTED_METHOD2(void, eventPost, ulong, cr_EsVariant);
  // same as above, but "generic" is used as category
  ES_DECL_REFLECTED_METHOD2(void, eventPostUrgent, ulong, cr_EsVariant);

  // marshal an arbitrary data to the master context, if needed
  ES_DECL_REFLECTED_METHOD1(void, marshal, cr_EsVariant);

  // properties
  //
  // this script reference
  ES_DECL_PROPERTY_RO(script, EsVariant)
  // progress monitoring interface access
  ES_DECL_PROPERTY_STD(progressMonitor, EsVariant)
  // Execution aborting flag query
  ES_DECL_PROPERTY_RO(aborting, bool)

protected:
  const EsScriptMachine& m_owner;

private:
  EsScriptHost(const EsScriptHost&) ES_REMOVEDECL;
  const EsScriptHost& operator=(const EsScriptHost&) ES_REMOVEDECL;

  friend class EsScriptMachine;
  friend class EsScript;
};

#endif // _es_script_host_h_