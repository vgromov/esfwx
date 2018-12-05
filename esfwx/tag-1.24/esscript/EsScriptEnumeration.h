#ifndef _es_script_enumeration_h_
#define _es_script_enumeration_h_

class ES_INTF_IMPL_DERIVED(EsScriptEnumeration, EsEnumeration)

protected:
  EsScriptEnumeration(EsScriptMachine& machine, const EsString& enumTypeName) : 
  EsEnumeration(enumTypeName),
  m_machine(machine) 
  { 
    m_dynamic = true; 
  }
  static EsEnumerationIntf::Ptr create(EsScriptMachine& machine, const EsString& enumTypeName);

public:
  ES_DECL_REFLECTED_CLASS(EsScriptEnumeration, EsEnumeration)
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return m_contents.nameGet(); }
  
private:
  EsScriptMachine& m_machine;
  
  friend class EsScriptMachine;
};

#endif // _es_script_enumeration_h_
