//#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_MS
# define ES_INTERNAL_THIS_CAST( Intf )        dynamic_cast<Intf*>(this)
# define ES_INTERNAL_THIS_CONSTCAST( Intf )   dynamic_cast<const Intf*>(this)
//#else
//# define ES_INTERNAL_THIS_CAST( Intf )        reinterpret_cast<Intf*>(reinterpret_cast<esU8*>(this) + ES_CONCAT(offs, Intf))
//# define ES_INTERNAL_THIS_CONSTCAST( Intf )   reinterpret_cast<const Intf*>(reinterpret_cast<const esU8*>(this) + ES_CONCAT(offs, Intf))
//#endif

/// Helper macros for base interface refcount trace
///
#ifdef ES_USE_TRACE_BASEINTF_REFCNT
#  define ES_TRACE_BASEINTF_INCREF             ES_DEBUG_TRACE(esT("'%s' before incRef(%d)"), typeNameGet(), (EsAtomicInteger)m_rc );
#  define ES_TRACE_BASEINTF_DECREF             ES_DEBUG_TRACE(esT("'%s' before decRef(%d)"), typeNameGet(), (EsAtomicInteger)m_rc );
#else
#  define ES_TRACE_BASEINTF_INCREF
#  define ES_TRACE_BASEINTF_DECREF
#endif

// helper templated call type caster, primarily for Borland calls resolution stuff
template <typename BaseCallT>
inline EsMemberCallT EsCastCallToMember(BaseCallT in) ES_NOTHROW
{
  union
  {
    BaseCallT m_in;
    EsMemberCallT m_out;

  } caster;

  caster.m_in = in;
  return caster.m_out;
}
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsEsBaseIntf = 0
  };

public:
  EsBaseImpl0() ES_NOTHROW :
  m_dynamic(false),
  m_destroying(false)
  {}
  //virtual ~EsBaseImpl0() ES_NOTHROW {}
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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
  inline EsBaseIntf::Ptr asBaseIntfPtrDirectWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtrDirect() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  bool m_dynamic;               ///< true if instance is dynamically allocated
  volatile bool m_destroying;   ///< true if instance is being destroyed
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = 0,
    offsEsBaseIntf = sizeof(Intf1T)
  };

public:
  EsBaseImpl1() ES_NOTHROW :
  m_dynamic(false),
  m_destroying(false)
  {}
  //virtual ~EsBaseImpl1() ES_NOTHROW {}
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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
  inline EsBaseIntf::Ptr asBaseIntfPtrDirectWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtrDirect() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  bool m_dynamic;               ///< true if instance is dynamically allocated
  volatile bool m_destroying;   ///< true if instance is being destroyed
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = 0,
    offsIntf2T = offsIntf1T+sizeof(Intf1T),
    offsEsBaseIntf = offsIntf2T+sizeof(Intf1T)
  };

public:
  EsBaseImpl2()  ES_NOTHROW :
  m_dynamic(false),
  m_destroying(false)
  {}
  //virtual ~EsBaseImpl2() ES_NOTHROW {}
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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
  inline EsBaseIntf::Ptr asBaseIntfPtrDirectWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtrDirect() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  bool m_dynamic;      ///< true if instance is dynamically allocated
  volatile bool m_destroying;
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = 0,
    offsIntf2T = offsIntf1T+sizeof(Intf1T),
    offsIntf3T = offsIntf2T+sizeof(Intf2T),
    offsEsBaseIntf = offsIntf3T+sizeof(Intf3T)
  };

public:
  EsBaseImpl3() ES_NOTHROW :
  m_dynamic(false)
  {}
  virtual ~EsBaseImpl3() ES_NOTHROW {}
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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
  inline EsBaseIntf::Ptr asBaseIntfPtrDirectWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtrDirect() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  bool m_dynamic;               ///< true if instance is dynamically allocated
  volatile bool m_destroying;   ///< true if instance is being destroyed
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = 0,
    offsIntf2T = offsIntf1T+sizeof(Intf1T),
    offsIntf3T = offsIntf2T+sizeof(Intf2T),
    offsIntf4T = offsIntf3T+sizeof(Intf3T)
    offsEsBaseIntf = offsIntf4T+sizeof(Intf4T)
  };

public:
  EsBaseImpl4() ES_NOTHROW :
  m_dynamic(false),
  m_destroying(false)
  {}
  virtual ~EsBaseImpl4() ES_NOTHROW {}
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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
  inline EsBaseIntf::Ptr asBaseIntfPtrDirectWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtrDirect() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  bool m_dynamic;               ///< true if instance is dynamically allocated
  volatile bool m_destroying;   ///< true if instance is being destroyed
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = 0,
    offsIntf2T = offsIntf1T+sizeof(Intf1T),
    offsIntf3T = offsIntf2T+sizeof(Intf2T),
    offsIntf4T = offsIntf3T+sizeof(Intf3T)
    offsIntf5T = offsIntf4T+sizeof(Intf4T)
    offsEsBaseIntf = offsIntf5T+sizeof(Intf5T)
  };

public:
  EsBaseImpl5() ES_NOTHROW :
  m_dynamic(false),
  m_destroying(false)
  {}
  virtual ~EsBaseImpl5() ES_NOTHROW {}
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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
  inline EsBaseIntf::Ptr asBaseIntfPtrDirectWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtrDirect() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  bool m_dynamic;               ///< true if instance is dynamically allocated
  volatile bool m_destroying;   ///< true if instance is being destroyed
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsEsBaseIntf = 0
  };

public:
  EsBaseImplRc0() ES_NOTHROW :
  m_rc(1),
  m_dynamic(false),
  m_destroying(false)
  {}
  virtual ~EsBaseImplRc0() ES_NOTHROW {}
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
    esAtomicInc(m_rc);
  }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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
  inline EsBaseIntf::Ptr asBaseIntfPtrDirectWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtrDirect() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  EsAtomicInteger m_rc;
  bool m_dynamic;               ///< true if instance is dynamically allocated
  volatile bool m_destroying;   ///< true if instance is being destroyed
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = 0,
    offsEsBaseIntf = offsIntf1T+sizeof(Intf1T)
  };

public:
  EsBaseImplRc1() ES_NOTHROW :
  m_rc(1),
  m_dynamic(false),
  m_destroying(false)
  {}
  virtual ~EsBaseImplRc1() ES_NOTHROW {}
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
    esAtomicInc(m_rc);
  }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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
  inline EsBaseIntf::Ptr asBaseIntfPtrDirectWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtrDirect() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  EsAtomicInteger m_rc;
  bool m_dynamic;             ///< true if instance is dynamically allocated
  volatile bool m_destroying; ///< true if instance is being destroyed
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = 0,
    offsIntf2T = offsIntf1T+sizeof(Intf1T),
    offsEsBaseIntf = offsIntf2T+sizeof(Intf2T)
  };

public:
  EsBaseImplRc2() ES_NOTHROW :
  m_rc(1),
  m_dynamic(false),
  m_destroying(false)
  {}
  virtual ~EsBaseImplRc2() ES_NOTHROW {}
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
    esAtomicInc(m_rc);
  }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {
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
  ES_DECL_INTF_METHOD( void*, requestIntf )(const EsIID& iid, bool incref = true) ES_NOTHROW ES_OVERRIDE
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
  inline EsBaseIntf::Ptr asBaseIntfPtrDirectWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtrDirect() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  EsAtomicInteger m_rc;
  bool m_dynamic;               ///< true if instance is dynamically allocated
  volatile bool m_destroying;   ///< true if instance is being destroyed
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = 0,
    offsIntf2T = offsIntf1T+sizeof(Intf1T),
    offsIntf3T = offsIntf2T+sizeof(Intf2T),
    offsEsBaseIntf = offsIntf3T+sizeof(Intf3T)
  };

public:
  EsBaseImplRc3() ES_NOTHROW :
  m_dynamic(false),
  m_destroying(false),
  m_rc(1)
  {}
  virtual ~EsBaseImplRc3() ES_NOTHROW {}
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
    esAtomicInc(m_rc);
  }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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
  inline EsBaseIntf::Ptr asBaseIntfPtrDirectWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtrDirect() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  EsAtomicInteger m_rc;
  bool m_dynamic;         ///< true if instance is dynamically allocated
  volatile bool m_destroying;    ///< true if instance is being destroyed
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = 0,
    offsIntf2T = offsIntf1T+sizeof(Intf1T),
    offsIntf3T = offsIntf2T+sizeof(Intf2T),
    offsIntf4T = offsIntf3T+sizeof(Intf3T),
    offsEsBaseIntf = offsIntf4T+sizeof(Intf4T)
  };

public:
  EsBaseImplRc4() ES_NOTHROW :
  m_rc(1),
  m_dynamic(false),
  m_destroying(false)
  {}
  virtual ~EsBaseImplRc4() ES_NOTHROW {}
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
    esAtomicInc(m_rc);
  }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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
  inline EsBaseIntf::Ptr asBaseIntfPtrDirectWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtrDirect() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  EsAtomicInteger m_rc;
  bool m_dynamic;               ///< true if instance is dynamically allocated
  volatile bool m_destroying;   ///< true if instance is being destroyed
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = 0,
    offsIntf2T = offsIntf1T+sizeof(Intf1T),
    offsIntf3T = offsIntf2T+sizeof(Intf2T),
    offsIntf4T = offsIntf3T+sizeof(Intf3T),
    offsIntf5T = offsIntf4T+sizeof(Intf4T),
    offsEsBaseIntf = offsIntf5T+sizeof(Intf5T)
  };

public:
  EsBaseImplRc5() ES_NOTHROW :
  m_rc(1),
  m_dynamic(false),
  m_destroying(false)
  {}
  virtual ~EsBaseImplRc5() ES_NOTHROW {}
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
    esAtomicInc(m_rc);
  }
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW ES_OVERRIDE
  {
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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
  inline EsBaseIntf::Ptr asBaseIntfPtrDirectWeak() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf(), false, false );
  }

protected:
  /// return EsBaseIntf::Ptr, NB! non-incremented, so restrict usage to derived classes
  /// construction time only
  ///
  inline EsBaseIntf::Ptr asBaseIntfPtrDirect() ES_NOTHROW
  {
    return EsBaseIntf::Ptr( asBaseIntf() );
  }

protected:
  EsAtomicInteger m_rc;
  bool m_dynamic;                 ///< true if instance is dynamically allocated
  volatile bool m_destroying;     ///< true if instance is being destroyed
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = sizeof(BaseImplT),
  };

public:
  EsDerivedImpl1() ES_NOTHROW :
  BaseImplT()
  {}
  virtual ~EsDerivedImpl1() {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<Intf1T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf1T);

    return BaseImplT::requestIntf(iid, incref);
  }
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = sizeof(BaseImplT),
    offsIntf2T = offsIntf1T+sizeof(Intf1T),
  };

public:
  EsDerivedImpl2() ES_NOTHROW :
  BaseImplT()
  {}
  virtual ~EsDerivedImpl2() ES_NOTHROW {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<Intf1T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf1T);
    else if( EsIID::fromIntf<Intf2T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf2T);

    return BaseImplT::requestIntf(iid, incref);
  }
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = sizeof(BaseImplT),
    offsIntf2T = offsIntf1T+sizeof(Intf1T),
    offsIntf3T = offsIntf2T+sizeof(Intf2T),
  };

public:
  EsDerivedImpl3() ES_NOTHROW :
  BaseImplT()
  {}
  virtual ~EsDerivedImpl3() ES_NOTHROW {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<Intf1T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf1T);
    else if( EsIID::fromIntf<Intf2T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf2T);
    else if( EsIID::fromIntf<Intf3T>() == iid )
      return ES_INTERNAL_THIS_CAST(Intf3T);

    return BaseImplT::requestIntf(iid, incref);
  }
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = sizeof(BaseImplT),
    offsIntf2T = offsIntf1T+sizeof(Intf1T),
    offsIntf3T = offsIntf2T+sizeof(Intf2T),
    offsIntf4T = offsIntf3T+sizeof(Intf3T),
  };

public:
  EsDerivedImpl4() ES_NOTHROW :
  BaseImplT()
  {}
  virtual ~EsDerivedImpl4() ES_NOTHROW {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = sizeof(BaseImplT),
    offsIntf2T = offsIntf1T+sizeof(Intf1T),
    offsIntf3T = offsIntf2T+sizeof(Intf2T),
    offsIntf4T = offsIntf3T+sizeof(Intf3T),
    offsIntf5T = offsIntf4T+sizeof(Intf4T),
  };

public:
  EsDerivedImpl5() ES_NOTHROW :
  BaseImplT()
  {}
  virtual ~EsDerivedImpl5() ES_NOTHROW {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = sizeof(BaseImplT),
  };

public:
  EsDerivedImplRc1() ES_NOTHROW :
  BaseImplT()
  {}
  virtual ~EsDerivedImplRc1() ES_NOTHROW {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
  {
    if( EsIID::fromIntf<Intf1T>() == iid )
    {
      if(incref)
        incRef();

      return ES_INTERNAL_THIS_CAST(Intf1T);
    }

    return BaseImplT::requestIntf(iid, incref);
  }
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = sizeof(BaseImplT),
    offsIntf2T = offsIntf1T+sizeof(Intf1T),
  };

public:
  EsDerivedImplRc2() ES_NOTHROW :
  BaseImplT()
  {}
  virtual ~EsDerivedImplRc2() ES_NOTHROW {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = sizeof(BaseImplT),
    offsIntf2T = offsIntf1T+sizeof(Intf1T),
    offsIntf3T = offsIntf2T+sizeof(Intf2T),
  };

public:
  EsDerivedImplRc3() ES_NOTHROW :
  BaseImplT()
  {}
  virtual ~EsDerivedImplRc3() ES_NOTHROW {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = sizeof(BaseImplT),
    offsIntf2T = offsIntf1T+sizeof(Intf1T),
    offsIntf3T = offsIntf2T+sizeof(Intf2T),
    offsIntf4T = offsIntf3T+sizeof(Intf3T),
  };

public:
  EsDerivedImplRc4() ES_NOTHROW :
  BaseImplT()
  {}
  virtual ~EsDerivedImplRc4() ES_NOTHROW {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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

private:
  /// Values that represent internal interface offsets.
  enum {
    offsIntf1T = sizeof(BaseImplT),
    offsIntf2T = offsIntf1T+sizeof(Intf1T),
    offsIntf3T = offsIntf2T+sizeof(Intf2T),
    offsIntf4T = offsIntf3T+sizeof(Intf3T),
    offsIntf5T = offsIntf4T+sizeof(Intf4T),
  };

public:
  EsDerivedImplRc5() ES_NOTHROW :
  BaseImplT()
  {}
  virtual ~EsDerivedImplRc5() ES_NOTHROW {}
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE
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
  ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid, bool incref = true ) ES_NOTHROW ES_OVERRIDE
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
};
//---------------------------------------------------------------------------
