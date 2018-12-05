#ifndef _es_scriptlet_h_
#define _es_scriptlet_h_

#if (1 == ES_USE_COMPILER_IMPLEMENTATION)

ES_DECL_INTF_BEGIN(36900B67, 337F4DF5, 86257376, 454CB972, EsScriptletIntf)
  ES_DECL_INTF_METHOD(bool, isOrphan)() const = 0;
  ES_DECL_INTF_METHOD(bool, isCompiled)() const = 0;
  ES_DECL_INTF_METHOD(const EsString&, nameGet)() const = 0;
  ES_DECL_INTF_METHOD(ulong, parametersCountGet)() const = 0;
  ES_DECL_INTF_METHOD(const EsString::Array&, parameterNamesGet)() const = 0;
  ES_DECL_INTF_METHOD(const EsMethodInfoKeyT&, infoGet)() const = 0;
  ES_DECL_INTF_METHOD(EsScriptMachine&, machineGet)() = 0;
  ES_DECL_INTF_METHOD(bool, compile)(const EsBreakIntf::Ptr& brk) = 0;
  ES_DECL_INTF_METHOD(bool, compile)() = 0;
  ES_DECL_INTF_METHOD(const EsString&, sourceGet)() const = 0;
  ES_DECL_INTF_METHOD(EsVariant, exec)() = 0;
  ES_DECL_INTF_METHOD(EsVariant, exec)(cr_EsVariant) = 0;
  ES_DECL_INTF_METHOD(EsVariant, exec)(cr_EsVariant, cr_EsVariant) = 0;
  ES_DECL_INTF_METHOD(EsVariant, exec)(cr_EsVariant, cr_EsVariant, cr_EsVariant) = 0;
  ES_DECL_INTF_METHOD(EsVariant, exec)(cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) = 0;
  ES_DECL_INTF_METHOD(EsVariant, exec)(cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) = 0;
  ES_DECL_INTF_METHOD(EsVariant, exec)(cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) = 0;
protected:
  ES_DECL_INTF_METHOD(void, discard)() = 0;

  friend class EsScript;
ES_DECL_INTF_END

typedef std::vector<EsScriptletIntf::Ptr> EsScriptletIntfPtrArray;

class ESSCRIPT_CLASS ES_INTF_IMPL2(EsScriptlet, EsScriptletIntf, EsReflectedClassIntf)

protected:
  EsScriptlet(EsScriptMachine& owner, const EsString& name, const EsString& src, const EsString& argNames);

public:
  ES_DECL_REFLECTED_CLASS_BASE( EsScriptlet )
  ES_DECL_ATTR_HANDLING_STD

  virtual ~EsScriptlet();
  
  // EsScriptletIntf implementation
  //
  ES_DECL_INTF_METHOD(bool, isOrphan)() const ES_OVERRIDE { return m_orphan; }
  ES_DECL_INTF_METHOD(bool, isCompiled)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(const EsString&, nameGet)() const ES_OVERRIDE { return m_info.nameGet(); }
  ES_DECL_INTF_METHOD(ulong, parametersCountGet)() const ES_OVERRIDE { return static_cast<ulong>(m_info.parametersCountGet()); }
  ES_DECL_INTF_METHOD(const EsString::Array&, parameterNamesGet)() const ES_OVERRIDE { return m_params; }
  ES_DECL_INTF_METHOD(const EsMethodInfoKeyT&, infoGet)() const ES_OVERRIDE { return m_info; }
  ES_DECL_INTF_METHOD(EsScriptMachine&, machineGet)() ES_OVERRIDE { return m_owner; }
  ES_DECL_INTF_METHOD(bool, compile)(const EsBreakIntf::Ptr& brk) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(bool, compile) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(const EsString&, sourceGet)() const ES_OVERRIDE { return m_source; }
  ES_DECL_REFLECTED_INTF_METHOD0(EsVariant, exec) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(EsVariant, exec, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(EsVariant, exec, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD3(EsVariant, exec, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD4(EsVariant, exec, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD5(EsVariant, exec, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD6(EsVariant, exec, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;

  // reflected properties
  //
  ES_DECL_PROPERTY_RO_STD(orphan, bool)
  ES_DECL_PROPERTY_RO(compiled, bool)
  ES_DECL_PROPERTY_RO(name, EsString)
  ES_DECL_PROPERTY_RO(parametersCount, ulong)
  ES_DECL_PROPERTY_RO(parameterNames, EsStringArray)
  ES_DECL_PROPERTY_RO(source, EsString)

protected:
  ES_DECL_INTF_METHOD(void, discard)() ES_OVERRIDE;

  static EsString nameFormat(const EsString& name, size_t paramsCount);
  void parameterCountCheck(size_t cnt) const;
  void checkIsNotOrphanOperation(const EsString& op) const;

protected:
  EsScriptMachine& m_owner;
  EsMethodInfoKeyT m_info;
  EsString m_source;
  EsString::Array m_params;

  friend class EsScript;
};

#endif // #if (1 == ES_USE_COMPILER_IMPLEMENTATION)

#endif // _es_scriptlet_h_
