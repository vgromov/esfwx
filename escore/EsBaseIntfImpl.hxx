//#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_MS
#define ES_INTERNAL_THIS_CAST( Intf )         static_cast<Intf*>(this)
#define ES_INTERNAL_THIS_CONSTCAST( Intf )    static_cast<const Intf*>(this)
//---------------------------------------------------------------------------

/// Helper macros for base interface refcount trace
///
#ifdef ES_USE_TRACE_BASEINTF_REFCNT
#  define ES_TRACE_BASEINTF_INCREF             ES_DEBUG_TRACE(esT("'%s' before incRef(%d)"), typeNameGet(), (EsAtomicInteger)m_rc );
#  define ES_TRACE_BASEINTF_DECREF             ES_DEBUG_TRACE(esT("'%s' before decRef(%d)"), typeNameGet(), (EsAtomicInteger)m_rc );
#else
#  define ES_TRACE_BASEINTF_INCREF
#  define ES_TRACE_BASEINTF_DECREF
#endif
//---------------------------------------------------------------------------

template <typename IntfT>
IntfT* EsBase::asIntfT(bool doIncref /*= true*/) ES_NOTHROW
{
  return reinterpret_cast<IntfT*>(
    asBaseIntf()->requestIntf(
      EsIID::fromIntf<IntfT>(),
      doIncref
    )
  );
}
//---------------------------------------------------------------------------

template <typename IntfT>
const IntfT* EsBase::asIntfT(bool doIncref /*= true*/) const ES_NOTHROW //< Contness handling
{
  return reinterpret_cast<const IntfT*>(
    const_cast<EsBaseIntf*>(asBaseIntf())->requestIntf(
      EsIID::fromIntf<IntfT>(),
      doIncref
    )
  );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/// EsBaseImpl template classes, may implement Intf1T, Intf2T, Intf3T,
/// Intf4T, Intf5T, and EsBaseIntf interfaces,
/// using non-refcouted internals.
/// FinalImplT is top-derived class type name, it's used internally
/// for static class name functions access
///
template <typename FinalImplT>
class EsBaseImpl0 : public EsBase, public EsBaseIntf
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsBaseImpl0<FinalImplT> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsBaseImpl0() ES_NOTHROW :
  m_dynamic(false),
  m_destroying(false)
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return FinalImplT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return 0;
  }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE
  {
    m_destroying = true;
    if( m_dynamic )
      delete this;
  }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool ES_UNUSED(incref)) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<EsBaseIntf>() == iid )
      return asBaseIntf();

    return nullptr;
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  virtual EsBaseIntf* asBaseIntf() ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CAST(EsBaseIntf);
  }

  virtual const EsBaseIntf* asBaseIntf() const ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CONSTCAST(EsBaseIntf);
  }

  /// Return weak reference to this object itself
  inline EsBaseIntf::Ptr asBaseIntfPtrWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtr() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  bool m_dynamic;               ///< true if instance is dynamically allocated
  volatile bool m_destroying;   ///< true if instance is being destroyed

  ES_NON_COPYABLE(EsBaseImpl0);
};
//---------------------------------------------------------------------------

template <typename FinalImplT, typename Intf1T >
class EsBaseImpl1 : public EsBase, virtual public EsBaseIntf, public Intf1T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsBaseImpl1<FinalImplT, Intf1T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsBaseImpl1() ES_NOTHROW :
  m_dynamic(false),
  m_destroying(false)
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return FinalImplT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return 0;
  }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE
  {
    m_destroying = true;
    if( m_dynamic )
      delete this;
  }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool ES_UNUSED(incref) ) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<EsBaseIntf>() == iid )
      return asBaseIntf();
    else if( EsIID::fromIntf<Intf1T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf1T);

    return nullptr;
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  virtual EsBaseIntf* asBaseIntf() ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CAST(EsBaseIntf);
  }

  virtual const EsBaseIntf* asBaseIntf() const ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CONSTCAST(EsBaseIntf);
  }

  /// Return weak reference to this object itself
  inline EsBaseIntf::Ptr asBaseIntfPtrWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtr() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  bool m_dynamic;               ///< true if instance is dynamically allocated
  volatile bool m_destroying;   ///< true if instance is being destroyed

  ES_NON_COPYABLE(EsBaseImpl1);
};
//---------------------------------------------------------------------------

template <typename FinalImplT, typename Intf1T, typename Intf2T >
class EsBaseImpl2 : public EsBase, virtual public EsBaseIntf, public Intf1T, public Intf2T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsBaseImpl2<FinalImplT, Intf1T, Intf2T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsBaseImpl2()  ES_NOTHROW :
  m_dynamic(false),
  m_destroying(false)
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return FinalImplT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return 0;
  }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE
  {
    m_destroying = true;
    if( m_dynamic )
      delete this;
  }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool ES_UNUSED(incref) ) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<EsBaseIntf>() == iid )
      return asBaseIntf();
    else if( EsIID::fromIntf<Intf1T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf1T);
    else if( EsIID::fromIntf<Intf2T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf2T);

    return nullptr;
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  virtual EsBaseIntf* asBaseIntf() ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CAST(EsBaseIntf);
  }

  virtual const EsBaseIntf* asBaseIntf() const ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CONSTCAST(EsBaseIntf);
  }

  /// Return weak reference to this object itself
  inline EsBaseIntf::Ptr asBaseIntfPtrWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtr() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  bool m_dynamic;      ///< true if instance is dynamically allocated
  volatile bool m_destroying;

  ES_NON_COPYABLE(EsBaseImpl2);
};
//---------------------------------------------------------------------------

template <typename FinalImplT, typename Intf1T, typename Intf2T, typename Intf3T >
class EsBaseImpl3 : public EsBase, virtual public EsBaseIntf, public Intf1T, public Intf2T, public Intf3T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsBaseImpl3<FinalImplT, Intf1T, Intf2T, Intf3T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsBaseImpl3() ES_NOTHROW :
  m_dynamic(false),
  m_destroying(false)
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return FinalImplT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return 0;
  }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE
  {
    if( m_dynamic )
      delete this;
  }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool ES_UNUSED(incref) ) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<EsBaseIntf>() == iid )
      return asBaseIntf();
    else if( EsIID::fromIntf<Intf1T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf1T);
    else if( EsIID::fromIntf<Intf2T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf2T);
    else if( EsIID::fromIntf<Intf3T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf3T);

    return nullptr;
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  virtual EsBaseIntf* asBaseIntf() ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CAST(EsBaseIntf);
  }

  virtual const EsBaseIntf* asBaseIntf() const ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CONSTCAST(EsBaseIntf);
  }

  /// Return weak reference to this object itself
  inline EsBaseIntf::Ptr asBaseIntfPtrWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtr() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  bool m_dynamic;               ///< true if instance is dynamically allocated
  volatile bool m_destroying;   ///< true if instance is being destroyed

  ES_NON_COPYABLE(EsBaseImpl3);
};
//---------------------------------------------------------------------------

template <typename FinalImplT, typename Intf1T, typename Intf2T, typename Intf3T, typename Intf4T>
class EsBaseImpl4 : public EsBase, virtual public EsBaseIntf, public Intf1T, public Intf2T, public Intf3T, public Intf4T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsBaseImpl4<FinalImplT, Intf1T, Intf2T, Intf3T, Intf4T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsBaseImpl4() ES_NOTHROW :
  m_dynamic(false),
  m_destroying(false)
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return FinalImplT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return 0;
  }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE
  {
    m_destroying = true;
    if( m_dynamic )
      delete this;
  }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool ES_UNUSED(incref)) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<EsBaseIntf>() == iid )
      return asBaseIntf();
    else if( EsIID::fromIntf<Intf1T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf1T);
    else if( EsIID::fromIntf<Intf2T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf2T);
    else if( EsIID::fromIntf<Intf3T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf3T);
    else if( EsIID::fromIntf<Intf4T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf4T);

    return nullptr;
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  virtual EsBaseIntf* asBaseIntf() ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CAST(EsBaseIntf);
  }

  virtual const EsBaseIntf* asBaseIntf() const ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CONSTCAST(EsBaseIntf);
  }

  /// Return weak reference to this object itself
  inline EsBaseIntf::Ptr asBaseIntfPtrWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtr() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  bool m_dynamic;               ///< true if instance is dynamically allocated
  volatile bool m_destroying;   ///< true if instance is being destroyed

  ES_NON_COPYABLE(EsBaseImpl4);
};
//---------------------------------------------------------------------------

template <typename FinalImplT, typename Intf1T, typename Intf2T, typename Intf3T, typename Intf4T, typename Intf5T >
class EsBaseImpl5 : public EsBase, virtual public EsBaseIntf, public Intf1T, public Intf2T, public Intf3T, public Intf4T, public Intf5T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsBaseImpl5<FinalImplT, Intf1T, Intf2T, Intf3T, Intf4T, Intf5T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsBaseImpl5() ES_NOTHROW :
  m_dynamic(false),
  m_destroying(false)
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return FinalImplT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return 0;
  }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE
  {
    m_destroying = true;
    if( m_dynamic )
      delete this;
  }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool ES_UNUSED(incref)) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<EsBaseIntf>() == iid )
      return asBaseIntf();
    else if( EsIID::fromIntf<Intf1T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf1T);
    else if( EsIID::fromIntf<Intf2T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf2T);
    else if( EsIID::fromIntf<Intf3T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf3T);
    else if( EsIID::fromIntf<Intf4T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf4T);
    else if( EsIID::fromIntf<Intf5T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf5T);

    return nullptr;
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  virtual EsBaseIntf* asBaseIntf() ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CAST(EsBaseIntf);
  }

  virtual const EsBaseIntf* asBaseIntf() const ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CONSTCAST(EsBaseIntf);
  }

  /// Return weak reference to this object itself
  inline EsBaseIntf::Ptr asBaseIntfPtrWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtr() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  bool m_dynamic;               ///< true if instance is dynamically allocated
  volatile bool m_destroying;   ///< true if instance is being destroyed

  ES_NON_COPYABLE(EsBaseImpl5);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/// EsBaseImplRc template classes, may implement Intf1T, Intf2T, Intf3T,
/// Intf4T, Intf5T, and EsBaseIntf interfaces,
/// using refcounted internals
/// FinalImplT is top-derived class type name, it's used internally
/// for static class name functions access
///
template <typename FinalImplT>
class EsBaseImplRc0 : public EsBase, public EsBaseIntf
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsBaseImplRc0<FinalImplT> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsBaseImplRc0() ES_NOTHROW :
  m_rc(1),
  m_dynamic(false),
  m_destroying(false)
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return FinalImplT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {
    ES_TRACE_BASEINTF_INCREF

    esAtomicInc(m_rc);
  }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {
    ES_TRACE_BASEINTF_DECREF

    if( 1 > esAtomicDec(m_rc) && !m_destroying )
      destroy();
  }
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return m_rc;
  }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE
  {
    m_destroying = true;
    if( m_dynamic )
      delete this;
  }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<EsBaseIntf>() == iid )
    {
      if(incref)
        incRef();

      return asBaseIntf();
    }

    return nullptr;
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  virtual EsBaseIntf* asBaseIntf() ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CAST(EsBaseIntf);
  }

  virtual const EsBaseIntf* asBaseIntf() const ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CONSTCAST(EsBaseIntf);
  }

  /// Return weak reference to this object itself
  inline EsBaseIntf::Ptr asBaseIntfPtrWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtr() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  EsAtomicInteger m_rc;
  bool m_dynamic;               ///< true if instance is dynamically allocated
  volatile bool m_destroying;   ///< true if instance is being destroyed

  ES_NON_COPYABLE(EsBaseImplRc0);
};
//---------------------------------------------------------------------------

template <typename FinalImplT, typename Intf1T >
class EsBaseImplRc1 : public EsBase, virtual public EsBaseIntf, public Intf1T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsBaseImplRc1<FinalImplT, Intf1T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsBaseImplRc1() ES_NOTHROW :
  m_rc(1),
  m_dynamic(false),
  m_destroying(false)
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return FinalImplT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {
    ES_TRACE_BASEINTF_INCREF
    
    esAtomicInc(m_rc);
  }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {
    ES_TRACE_BASEINTF_DECREF

    if( 1 > esAtomicDec(m_rc) && !m_destroying )
      destroy();
  }
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return m_rc;
  }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE
  {
    m_destroying = true;
    if( m_dynamic )
      delete this;
  }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref ) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<EsBaseIntf>() == iid )
    {
      if(incref)
        incRef();

      return asBaseIntf();
    }
    else if( EsIID::fromIntf<Intf1T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf1T);
    }

    return nullptr;
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  virtual EsBaseIntf* asBaseIntf() ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CAST(EsBaseIntf);
  }

  virtual const EsBaseIntf* asBaseIntf() const ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CONSTCAST(EsBaseIntf);
  }

  /// Return weak reference to this object itself
  inline EsBaseIntf::Ptr asBaseIntfPtrWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtr() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  EsAtomicInteger m_rc;
  bool m_dynamic;             ///< true if instance is dynamically allocated
  volatile bool m_destroying; ///< true if instance is being destroyed

  ES_NON_COPYABLE(EsBaseImplRc1);
};
//---------------------------------------------------------------------------

template <typename FinalImplT, typename Intf1T, typename Intf2T >
class EsBaseImplRc2 : public EsBase, virtual public EsBaseIntf, public Intf1T, public Intf2T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsBaseImplRc2<FinalImplT, Intf1T, Intf2T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsBaseImplRc2() ES_NOTHROW :
  m_rc(1),
  m_dynamic(false),
  m_destroying(false)
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return FinalImplT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {
    ES_TRACE_BASEINTF_INCREF

    esAtomicInc(m_rc);
  }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {
    ES_TRACE_BASEINTF_DECREF

    if( 1 > esAtomicDec(m_rc) && !m_destroying )
      destroy();
  }
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return m_rc;
  }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE
  {
    m_destroying = true;
    if( m_dynamic )
      delete this;
  }
  ES_DECL_INTF_METHOD( void*, requestIntf )(const EsIID& iid, bool incref) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<EsBaseIntf>() == iid )
    {
      if(incref)
        incRef();

      return asBaseIntf();
    }
    else if( EsIID::fromIntf<Intf1T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf1T);
    }
    else if( EsIID::fromIntf<Intf2T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf2T);
    }

    return nullptr;
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  virtual EsBaseIntf* asBaseIntf() ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CAST(EsBaseIntf);
  }

  virtual const EsBaseIntf* asBaseIntf() const ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CONSTCAST(EsBaseIntf);
  }

  /// Return weak reference to this object itself
  inline EsBaseIntf::Ptr asBaseIntfPtrWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtr() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  EsAtomicInteger m_rc;
  bool m_dynamic;               ///< true if instance is dynamically allocated
  volatile bool m_destroying;   ///< true if instance is being destroyed

  ES_NON_COPYABLE(EsBaseImplRc2);
};
//---------------------------------------------------------------------------

template <typename FinalImplT, typename Intf1T, typename Intf2T, typename Intf3T >
class EsBaseImplRc3 : public EsBase, virtual public EsBaseIntf, public Intf1T, public Intf2T, public Intf3T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsBaseImplRc3<FinalImplT, Intf1T, Intf2T, Intf3T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsBaseImplRc3() ES_NOTHROW :
  m_dynamic(false),
  m_destroying(false),
  m_rc(1)
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return FinalImplT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {
    ES_TRACE_BASEINTF_INCREF

    esAtomicInc(m_rc);
  }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {
    ES_TRACE_BASEINTF_DECREF

    if( 1 > esAtomicDec(m_rc) && !m_destroying )
      destroy();
  }
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return m_rc;
  }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE
  {
    m_destroying = true;
    if( m_dynamic )
      delete this;
  }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<EsBaseIntf>() == iid )
    {
      if(incref)
        incRef();

      return asBaseIntf();
    }
    else if( EsIID::fromIntf<Intf1T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf1T);
    }
    else if( EsIID::fromIntf<Intf2T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf2T);
    }
    else if( EsIID::fromIntf<Intf3T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf3T);
    }

    return nullptr;
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  virtual EsBaseIntf* asBaseIntf() ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CAST(EsBaseIntf);
  }

  virtual const EsBaseIntf* asBaseIntf() const ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CONSTCAST(EsBaseIntf);
  }

  /// Return weak reference to this object itself
  inline EsBaseIntf::Ptr asBaseIntfPtrWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtr() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  EsAtomicInteger m_rc;
  bool m_dynamic;                 ///< true if instance is dynamically allocated
  volatile bool m_destroying;     ///< true if instance is being destroyed

  ES_NON_COPYABLE(EsBaseImplRc3);
};
//---------------------------------------------------------------------------

template <typename FinalImplT, typename Intf1T, typename Intf2T, typename Intf3T, typename Intf4T>
class EsBaseImplRc4 : public EsBase, virtual public EsBaseIntf, public Intf1T, public Intf2T, public Intf3T, public Intf4T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsBaseImplRc4<FinalImplT, Intf1T, Intf2T, Intf3T, Intf4T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsBaseImplRc4() ES_NOTHROW :
  m_rc(1),
  m_dynamic(false),
  m_destroying(false)
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return FinalImplT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {
    ES_TRACE_BASEINTF_INCREF

    esAtomicInc(m_rc);
  }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {
    ES_TRACE_BASEINTF_DECREF

    if( 1 > esAtomicDec(m_rc) && !m_destroying )
      destroy();
  }
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return m_rc;
  }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE
  {
    m_destroying = true;
    if( m_dynamic )
      delete this;
  }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<EsBaseIntf>() == iid )
    {
      if(incref)
        incRef();

      return asBaseIntf();
    }
    else if( EsIID::fromIntf<Intf1T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf1T);
    }
    else if( EsIID::fromIntf<Intf2T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf2T);
    }
    else if( EsIID::fromIntf<Intf3T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf3T);
    }
    else if( EsIID::fromIntf<Intf4T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf4T);
    }

    return nullptr;
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  virtual EsBaseIntf* asBaseIntf() ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CAST(EsBaseIntf);
  }

  virtual const EsBaseIntf* asBaseIntf() const ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CONSTCAST(EsBaseIntf);
  }

  /// Return weak reference to this object itself
  inline EsBaseIntf::Ptr asBaseIntfPtrWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtr() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  EsAtomicInteger m_rc;
  bool m_dynamic;               ///< true if instance is dynamically allocated
  volatile bool m_destroying;   ///< true if instance is being destroyed

  ES_NON_COPYABLE(EsBaseImplRc4);
};
//---------------------------------------------------------------------------

template <typename FinalImplT, typename Intf1T, typename Intf2T, typename Intf3T, typename Intf4T, typename Intf5T >
class EsBaseImplRc5 : public EsBase, virtual public EsBaseIntf, public Intf1T, public Intf2T, public Intf3T, public Intf4T, public Intf5T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsBaseImplRc5<FinalImplT, Intf1T, Intf2T, Intf3T, Intf4T, Intf5T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsBaseImplRc5() ES_NOTHROW :
  m_rc(1),
  m_dynamic(false),
  m_destroying(false)
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return FinalImplT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {
    ES_TRACE_BASEINTF_INCREF

    esAtomicInc(m_rc);
  }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {
    ES_TRACE_BASEINTF_DECREF

    if( 1 > esAtomicDec(m_rc) && !m_destroying )
      destroy();
  }
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return m_rc;
  }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE
  {
    m_destroying = true;
    if( m_dynamic )
      delete this;
  }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<EsBaseIntf>() == iid )
    {
      if(incref)
        incRef();

      return asBaseIntf();
    }
    else if( EsIID::fromIntf<Intf1T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf1T);
    }
    else if( EsIID::fromIntf<Intf2T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf2T);
    }
    else if( EsIID::fromIntf<Intf3T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf3T);
    }
    else if( EsIID::fromIntf<Intf4T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf4T);
    }
    else if( EsIID::fromIntf<Intf5T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf5T);
    }

    return nullptr;
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  virtual EsBaseIntf* asBaseIntf() ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CAST(EsBaseIntf);
  }

  virtual const EsBaseIntf* asBaseIntf() const ES_NOTHROW ES_OVERRIDE
  {
    return ES_INTERNAL_THIS_CONSTCAST(EsBaseIntf);
  }

  /// Return weak reference to this object itself
  inline EsBaseIntf::Ptr asBaseIntfPtrWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtr() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  EsAtomicInteger m_rc;
  bool m_dynamic;                 ///< true if instance is dynamically allocated
  volatile bool m_destroying;     ///< true if instance is being destroyed

  ES_NON_COPYABLE(EsBaseImplRc5);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/// Additional Interface implementation in derived class helpers
///
template < typename DerivedT, typename BaseImplT, typename Intf1T >
class EsDerivedImpl1 : public BaseImplT, public Intf1T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsDerivedImpl1<DerivedT, BaseImplT, Intf1T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsDerivedImpl1() ES_NOTHROW :
  BaseImplT()
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return 0;
  }
  ES_DECL_INTF_METHOD(void, destroy)()  ES_NOTHROW ES_OVERRIDE { BaseImplT::destroy(); }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<Intf1T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf1T);

    return BaseImplT::requestIntf(iid, incref);
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  ES_NON_COPYABLE(EsDerivedImpl1);
};
//---------------------------------------------------------------------------

template < typename DerivedT, typename BaseImplT, typename Intf1T, typename Intf2T >
class EsDerivedImpl2 : public BaseImplT, public Intf1T, public Intf2T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsDerivedImpl2<DerivedT, BaseImplT, Intf1T, Intf2T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsDerivedImpl2() ES_NOTHROW :
  BaseImplT()
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return 0;
  }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE
  {
    BaseImplT::destroy();
  }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<Intf1T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf1T);
    else if( EsIID::fromIntf<Intf2T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf2T);

    return BaseImplT::requestIntf(iid, incref);
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  ES_NON_COPYABLE(EsDerivedImpl2);
};
//---------------------------------------------------------------------------

template < typename DerivedT, typename BaseImplT, typename Intf1T, typename Intf2T, typename Intf3T >
class EsDerivedImpl3 : public BaseImplT, public Intf1T, public Intf2T, public Intf3T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsDerivedImpl3<DerivedT, BaseImplT, Intf1T, Intf2T, Intf3T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsDerivedImpl3() ES_NOTHROW :
  BaseImplT()
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return 0;
  }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE
  {
    BaseImplT::destroy();
  }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<Intf1T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf1T);
    else if( EsIID::fromIntf<Intf2T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf2T);
    else if( EsIID::fromIntf<Intf3T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf3T);

    return BaseImplT::requestIntf(iid, incref);
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  ES_NON_COPYABLE(EsDerivedImpl3);
};
//---------------------------------------------------------------------------

template < typename DerivedT, typename BaseImplT, typename Intf1T, typename Intf2T, typename Intf3T, typename Intf4T >
class EsDerivedImpl4 : public BaseImplT, public Intf1T, public Intf2T, public Intf3T, public Intf4T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsDerivedImpl4<DerivedT, BaseImplT, Intf1T, Intf2T, Intf3T, Intf4T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsDerivedImpl4() ES_NOTHROW :
  BaseImplT()
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return 0;
  }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE
  {
    BaseImplT::destroy();
  }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<Intf1T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf1T);
    else if( EsIID::fromIntf<Intf2T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf2T);
    else if( EsIID::fromIntf<Intf3T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf3T);
    else if( EsIID::fromIntf<Intf4T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf4T);

    return BaseImplT::requestIntf(iid, incref);
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  ES_NON_COPYABLE(EsDerivedImpl4);
};
//---------------------------------------------------------------------------

template < typename DerivedT, typename BaseImplT, typename Intf1T, typename Intf2T, typename Intf3T, typename Intf4T, typename Intf5T >
class EsDerivedImpl5 : public BaseImplT, public Intf1T, public Intf2T, public Intf3T, public Intf4T, public Intf5T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsDerivedImpl5<DerivedT, BaseImplT, Intf1T, Intf2T, Intf3T, Intf4T, Intf5T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsDerivedImpl5() ES_NOTHROW :
  BaseImplT()
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {}
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return 0;
  }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE
  {
    BaseImplT::destroy();
  }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<Intf1T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf1T);
    else if( EsIID::fromIntf<Intf2T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf2T);
    else if( EsIID::fromIntf<Intf3T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf3T);
    else if( EsIID::fromIntf<Intf4T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf4T);
    else if( EsIID::fromIntf<Intf5T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf5T);

    return BaseImplT::requestIntf(iid, incref);
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  ES_NON_COPYABLE(EsDerivedImpl5);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/// Refcounted additional interface implementation template classes
///
template < typename DerivedT, typename BaseImplT, typename Intf1T >
class EsDerivedImplRc1 : public BaseImplT, public Intf1T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsDerivedImplRc1<DerivedT, BaseImplT, Intf1T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsDerivedImplRc1() ES_NOTHROW :
  BaseImplT()
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE { BaseImplT::incRef(); }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE { BaseImplT::decRef(); }
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE { return BaseImplT::refGet(); }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE { BaseImplT::destroy(); }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<Intf1T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf1T);
    }

    return BaseImplT::requestIntf(iid, incref);
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  ES_NON_COPYABLE(EsDerivedImplRc1);
};
//---------------------------------------------------------------------------

template < typename DerivedT, typename BaseImplT, typename Intf1T, typename Intf2T >
class EsDerivedImplRc2 : public BaseImplT, public Intf1T, public Intf2T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsDerivedImplRc2<DerivedT, BaseImplT, Intf1T, Intf2T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsDerivedImplRc2() ES_NOTHROW :
  BaseImplT()
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE { BaseImplT::incRef(); }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE { BaseImplT::decRef(); }
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE { return BaseImplT::refGet(); }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE { BaseImplT::destroy(); }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<Intf1T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf1T);
    }
    else if( EsIID::fromIntf<Intf2T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf2T);
    }

    return BaseImplT::requestIntf(iid, incref);
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  ES_NON_COPYABLE(EsDerivedImplRc2);
};
//---------------------------------------------------------------------------

template < typename DerivedT, typename BaseImplT, typename Intf1T, typename Intf2T, typename Intf3T >
class EsDerivedImplRc3 : public BaseImplT, public Intf1T, public Intf2T, public Intf3T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsDerivedImplRc3<DerivedT, BaseImplT, Intf1T, Intf2T, Intf3T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsDerivedImplRc3() ES_NOTHROW :
  BaseImplT()
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE { BaseImplT::incRef(); }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE { BaseImplT::decRef(); }
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE { return BaseImplT::refGet(); }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE { BaseImplT::destroy(); }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<Intf1T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf1T);
    }
    else if( EsIID::fromIntf<Intf2T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf2T);
    }
    else if( EsIID::fromIntf<Intf3T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf3T);
    }

    return BaseImplT::requestIntf(iid, incref);
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  ES_NON_COPYABLE(EsDerivedImplRc3);
};
//---------------------------------------------------------------------------

template < typename DerivedT, typename BaseImplT, typename Intf1T, typename Intf2T, typename Intf3T, typename Intf4T >
class EsDerivedImplRc4 : public BaseImplT, public Intf1T, public Intf2T, public Intf3T, public Intf4T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsDerivedImplRc4<DerivedT, BaseImplT, Intf1T, Intf2T, Intf3T, Intf4T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsDerivedImplRc4() ES_NOTHROW :
  BaseImplT()
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE { BaseImplT::incRef(); }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE { BaseImplT::decRef(); }
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE { return BaseImplT::refGet(); }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE { BaseImplT::destroy(); }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<Intf1T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf1T);
    }
    else if( EsIID::fromIntf<Intf2T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf2T);
    }
    else if( EsIID::fromIntf<Intf3T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf3T);
    }
    else if( EsIID::fromIntf<Intf4T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf4T);
    }

    return BaseImplT::requestIntf(iid, incref);
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  ES_NON_COPYABLE(EsDerivedImplRc4);
};
//---------------------------------------------------------------------------

template < typename DerivedT, typename BaseImplT, typename Intf1T, typename Intf2T, typename Intf3T, typename Intf4T, typename Intf5T >
class EsDerivedImplRc5 : public BaseImplT, public Intf1T, public Intf2T, public Intf3T, public Intf4T, public Intf5T
{
public:
  /// Defines an alias representing this type.
  /// and void call to ThisT method
  typedef EsDerivedImplRc5<DerivedT, BaseImplT, Intf1T, Intf2T, Intf3T, Intf4T, Intf5T> ThisT;
  typedef void (ThisT:: *EsBaseImplCallT)(void);

public:
  EsDerivedImplRc5() ES_NOTHROW :
  BaseImplT()
  {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return DerivedT::classNameGetStatic();
  }
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW ES_OVERRIDE
  {
    return name == classNameGet();
  }
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW ES_OVERRIDE { BaseImplT::incRef(); }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE { BaseImplT::decRef(); }
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW ES_OVERRIDE { return BaseImplT::refGet(); }
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE { BaseImplT::destroy(); }
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<Intf1T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf1T);
    }
    else if( EsIID::fromIntf<Intf2T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf2T);
    }
    else if( EsIID::fromIntf<Intf3T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf3T);
    }
    else if( EsIID::fromIntf<Intf4T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf4T);
    }
    else if( EsIID::fromIntf<Intf5T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf5T);
    }

    return BaseImplT::requestIntf(iid, incref);
  }
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW ES_OVERRIDE
  {
    return this;
  }

  ES_NON_COPYABLE(EsDerivedImplRc5);
};
//---------------------------------------------------------------------------
