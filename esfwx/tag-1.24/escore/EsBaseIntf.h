#ifndef _base_intf_h_
#define _base_intf_h_

// interface method calling convention macros
//
#define ES_DECL_INTF_METHOD(Retval, MethodName) \
  virtual Retval MethodName

#define ES_IMPL_INTF_METHOD(Retval, MethodName) \
  Retval MethodName

/// Interface unique ID lightweight structure
struct EsIID
{
  /// Data members
  esU32 id0, id1, id2, id3;

  /// Constructors
  inline EsIID() ES_NOTHROW : id0(0), id1(0), id2(0), id3(0) {}
  inline EsIID(const EsIID& other) ES_NOTHROW : id0(other.id0), id1(other.id1), id2(other.id2), id3(other.id3) {}
  inline EsIID(esU32 _0, esU32 _1, esU32 _2, esU32 _3) ES_NOTHROW : id0(_0), id1(_1), id2(_2), id3(_3) {}

  /// Copy operator
  inline EsIID& operator=( const EsIID& other ) ES_NOTHROW
  {
    id0 = other.id0; id1 = other.id1; id2 = other.id2; id3 = other.id3;
    return *this;
  }

  /// Templated EsIID generator
  template <typename IntfT>
  static inline EsIID fromIntf() ES_NOTHROW
  {
    return EsIID(
      static_cast<esU32>(IntfT::uid0), 
      static_cast<esU32>(IntfT::uid1), 
      static_cast<esU32>(IntfT::uid2), 
      static_cast<esU32>(IntfT::uid3)
    );
  }

  /// Check EsIID for emptyness
  inline bool empty() const ES_NOTHROW
  {
    return id0 == 0 && id1 == 0 && id2 == 0 && id3 == 0;
  }

  /// EsIID equality check
  inline bool operator== (const EsIID& other) const ES_NOTHROW
  {
    return id0 == other.id0 && id1 == other.id1 && id2 == other.id2 && id3 == other.id3;
  }

  /// Type conversion to GUID
  inline operator GUID () const ES_NOTHROW
  {
    GUID result;
    ES_COMPILE_TIME_ASSERT(sizeof(GUID) == (sizeof(EsIID)), _sizeofGUID_equalto_sizeofIID);

    memcpy(
      &result,
      this,
      sizeof(EsIID)
    );

    return result;
  }
};

// template for smart pointer to interface declaration
//
template<typename IntfT>
class EsIntfPtr
{
public:
  typedef IntfT Intf;

public:
  inline EsIntfPtr(IntfT* intf = nullptr, bool inc = false, bool own = true) ES_NOTHROW :
  m_intf(intf),
  m_own(own)
  {
    if( inc && m_intf )
      m_intf->incRef();
  }

  inline ~EsIntfPtr() ES_NOTHROW
  {
    EsCriticalSectionLocker lock(m_cs);
    clean();
  }

  inline bool own() const ES_NOTHROW
  {
    return m_own;
  }

  inline void reset(IntfT* intf = nullptr, bool own = true) ES_NOTHROW
  {
    EsCriticalSectionLocker lock(m_cs);
    resetNonInterlocked(
      intf,
      own
    );
  }

  // peek contained interface pointer
  inline IntfT* get() const ES_NOTHROW
  {
    EsCriticalSectionLocker lock(m_cs);
    return m_intf;
  }

  // return contained interface id, or empty id if contained pointer is 0
  inline EsIID getID() const ES_NOTHROW
  {
    EsCriticalSectionLocker lock(m_cs);
    if( m_intf )
      return EsIID( IntfT::uid0, IntfT::uid1, IntfT::uid2, IntfT::uid3 );

    return EsIID();
  }

  // detach interface pointer, lose ownership of it, do not refcount any more
  inline IntfT* detach() ES_NOTHROW
  {
    EsCriticalSectionLocker lock(m_cs);
    IntfT* result = m_intf;
    m_intf = nullptr;
    return result;
  }

  inline IntfT* operator->() const ES_NOTHROW
  {
    EsCriticalSectionLocker lock(m_cs);
    ES_ASSERT(m_intf);
    return m_intf;
  }

  inline operator bool() const ES_NOTHROW
  {
    EsCriticalSectionLocker lock(m_cs);
    return nullptr != m_intf;
  }

  // ref-counted copy from other instance of the same interface pointer type
  inline EsIntfPtr(const EsIntfPtr<IntfT>& src) ES_NOTHROW :
  m_intf(nullptr),
  m_own(false)
  {
    EsCriticalSectionLocker locksrc(src.m_cs);
    m_intf = src.m_intf;
    m_own = src.m_own;
    if( m_own && m_intf )
      m_intf->incRef();
  }

  // ref-counted copy from other instance of other interface pointer type
  template < typename OtherIntfT >
  inline EsIntfPtr(const EsIntfPtr<OtherIntfT>& src) ES_NOTHROW :
  m_intf(nullptr),
  m_own(false)
  {
    EsIntfPtr<IntfT> tmp = src.template request<IntfT>();
    m_intf = tmp.m_intf;
    m_own = tmp.m_own;
    if( m_own && m_intf )
      m_intf->incRef();
  }

  // ref-counted assign from other instance of the same interface pointer type
  inline EsIntfPtr<IntfT>& operator= (const EsIntfPtr<IntfT>& src) ES_NOTHROW
  {
    if( this != &src ) // if we're not assigning ouselves to ourselves
    {
      EsCriticalSectionLocker locksrc(src.m_cs);
      EsCriticalSectionLocker lock(m_cs);

      resetNonInterlocked(
        src.m_intf,
        src.m_own
      );

      if( m_own && m_intf )
        m_intf->incRef();
    }

    return *this;
  }

  // ref-counted assign from other instance of the other interface pointer type
  template < typename OtherIntfT >
  inline EsIntfPtr<IntfT>& operator= (const EsIntfPtr<OtherIntfT>& src) ES_NOTHROW
  {
    EsIntfPtr<IntfT> tmp = src.template request<IntfT>();
    EsCriticalSectionLocker lock(m_cs);

    resetNonInterlocked(
      tmp.m_intf,
      tmp.m_own
    );

    if( m_own && m_intf )
      m_intf->incRef();

    return *this;
  }

  // request and return other interface smartptr type
  template < typename OtherIntfT >
  inline EsIntfPtr< OtherIntfT > request() const ES_NOTHROW
  {
    EsCriticalSectionLocker lock(m_cs);
    return requestNonInterlocked<OtherIntfT>();
  }

  // EsIntfPtr templated equality check operator
  //
  template < typename OtherIntfT >
  bool operator== (const EsIntfPtr< OtherIntfT >& other) const ES_NOTHROW;

  // EsIntfPtr templated inequality check operator
  //
  template < typename OtherIntfT >
  inline bool operator!= (const EsIntfPtr< OtherIntfT >& other) const ES_NOTHROW
  {
    return ! this->operator== (other);
  }

  /// Semi-private API
  ///

  /// Non-interlocked version of smartptr reset
  inline void resetNonInterlocked( IntfT* intf = nullptr, bool own = true )  ES_NOTHROW
  {
    clean();
    m_intf = intf;
    m_own = own;
  }

  /// Request other interface type without interlock
  template < typename OtherIntfT >
  inline EsIntfPtr< OtherIntfT > requestNonInterlocked() const ES_NOTHROW
  {
    EsIntfPtr< OtherIntfT > result;
    if( m_intf )
    {
      OtherIntfT* other = reinterpret_cast< OtherIntfT* >(m_intf->requestIntf( EsIID::fromIntf< OtherIntfT >() ));
      result.resetNonInterlocked(
        other,
        m_own
      );

      if( other && !m_own ) // compensate the implicit incRef in requestIntf, if not owning interface
        m_intf->decRef();
    }

    return result;
  }

  // ref-counted assign from other instance of the same interface pointer type
  inline EsIntfPtr<IntfT>& assignNonInterlocked( const EsIntfPtr<IntfT>& src ) ES_NOTHROW
  {
    if( this != &src ) // if we're not assigning ouselves to ourselves
    {
      resetNonInterlocked(
        src.m_intf,
        src.m_own
      );

      if( m_own && m_intf )
        m_intf->incRef();
    }

    return *this;
  }

  // ref-counted assign from other instance of the other interface pointer type
  template < typename OtherIntfT >
  inline EsIntfPtr<IntfT>& assignNonInterlocked( const EsIntfPtr<OtherIntfT>& src ) ES_NOTHROW
  {
    EsIntfPtr<IntfT> tmp;
    tmp.assignNonInterlocked(
      src.template requestNonInterlocked<IntfT>()
    );

    resetNonInterlocked(
      tmp.m_intf,
      tmp.m_own
    );

    if( m_own && m_intf )
      m_intf->incRef();

    return *this;
  }

#ifdef ES_MODERN_CPP
  // C11 move from other instance of the same interface pointer type
  inline EsIntfPtr( EsIntfPtr<IntfT>&& src ) ES_NOTHROW
  {
    if( this == &src ) // if we're not assigning ouselves to ourselves
      return;

// REM    EsCriticalSectionLocker locksrc( src.m_cs );
    m_intf = src.m_intf;
    m_own = src.m_own;

    src.m_intf = nullptr;
    src.m_own = false;
  }

  // Move sematics assign from other instance of the same interface pointer type
  inline EsIntfPtr<IntfT>& operator= ( EsIntfPtr<IntfT>&& src ) ES_NOTHROW
  {
    if( this == &src ) // if we're not assigning ouselves to ourselves
      return *this;

// REM   EsCriticalSectionLocker locksrc( src.m_cs );
// REM   EsCriticalSectionLocker lock( m_cs );
    resetNonInterlocked( src.m_intf, src.m_own );
    src.m_intf = nullptr;
    src.m_own = false;

    return *this;
  }
#endif

private:
  inline void clean() ES_NOTHROW
  {
    if(m_intf && m_own)
      m_intf->decRef();
    m_intf = nullptr;
  }

protected:
  mutable EsCriticalSection m_cs;
  IntfT* m_intf;
  bool m_own;
};
//---------------------------------------------------------------------------

/// Generic implicit base for all Esxxx classes hierarchy
///
class EsBaseIntf; ///< Forward decl
class ESCORE_CLASS EsBase
{
public:
  virtual ~EsBase() {}

  /// To be implemented in templated implementor classes, see EsBaseIntfImpl.hxx
  ///
  virtual EsBaseIntf* asBaseIntf() ES_NOTHROW = 0;
  virtual const EsBaseIntf* asBaseIntf() const ES_NOTHROW = 0;

  /// Perform typed intfptr request with optional incref control
  ///
  template <typename IntfT>
  IntfT* asIntfT(bool doIncref = true) ES_NOTHROW;

  template <typename IntfT>
  const IntfT* asIntfT(bool doIncref = true) const ES_NOTHROW; //< Contness handling
};
//---------------------------------------------------------------------------

/// Interface declaration helper macro
///

/// UID0-3 four 4-byte parts of iterface's GUID
#define ES_DECL_INTF_BEGIN( UID0, UID1, UID2, UID3, InterfaceName ) \
class ES_ABSTRACT InterfaceName : virtual public EsBaseIntf \
{ \
public: \
  enum { uid0 = 0x ## UID0, uid1 = 0x ## UID1, uid2 = 0x ## UID2, uid3 = 0x ## UID3 }; \
  typedef EsIntfPtr<InterfaceName> Ptr;

#define ES_DECL_INTF_END };

/// Base interface definition. this class must be the base class
/// for any interface used throughout the framework, to allow
/// kina COM-QueryInterface feature impementation. NB! interface implementation
/// used here is not refcounted, so beware.
///
class ES_ABSTRACT EsBaseIntf
{
public:
  enum { uid0 = 0x61233F3D, uid1 = 0x2A86471D, uid2 = 0xB73DAE05, uid3 = 0x51A4E46C };
  typedef EsIntfPtr<EsBaseIntf> Ptr;

  /// Analog to COM's query interface method. if requested interface is not supported,
  /// return NULL pointer, otherwise, return properly cast requested interface pointer,
  /// wrapped in void pointer.
  /// @param  incref if set (default), object is incref-ed, otherwise, refcount is left as-is
  ///
  ES_DECL_INTF_METHOD(void*, requestIntf)(const EsIID& iid, bool incref = true) ES_NOTHROW = 0;

  /// Basic ESFWX RTTI support
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const ES_NOTHROW = 0;

  /// Refcount support
  ES_DECL_INTF_METHOD(void, incRef)() ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(void, decRef)() ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(size_t, refGet)() const ES_NOTHROW = 0;

  /// Special deletion method
  ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW = 0;

  /// Access to interface implementor's object instance
  ES_DECL_INTF_METHOD(EsBase*, implementorGet)() ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(const EsBase*, implementorGet)() const ES_NOTHROW = 0;
};

/// Generic member service call type
typedef void (EsBase:: *EsMemberCallT)(void);
/// Generic class service call type
typedef void (*EsClassCallT)(void);

/// Generic method (class or member)
union EsMethodT {
  EsMemberCallT m_method;
  EsClassCallT m_classMethod;
};

// Multiple interface templated implementation macros
#include "EsBaseIntfImpl.hxx"

//---------------------------------------------------------------------------
/// A helper macros that defines helper functionality for
/// templated EsBaseImplN implementors
///
#define ES_INTERNAL_INTF_IMPL( ClassName ) \
public: \
  static inline EsString classNameGetStatic() ES_NOTHROW { return ES_STRINGIZE(ClassName); }

//---------------------------------------------------------------------------
/// Macros to define class which implements interfaces
/// Usage: class [optional export spec] ES_INTF_IMPLN( ClassName [, ...] )
///
#define ES_INTF_IMPL0( ClassName ) \
  ClassName : public EsBaseImplRc0< ClassName > { \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL1( ClassName, Intf1 ) \
  ClassName : public EsBaseImplRc1< ClassName, Intf1 > { \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL2( ClassName, Intf1, Intf2 ) \
  ClassName : public EsBaseImplRc2< ClassName, Intf1, Intf2 > { \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL3( ClassName, Intf1, Intf2, Intf3 ) \
  ClassName : public EsBaseImplRc3< ClassName, Intf1, Intf2, Intf3 > { \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL4( ClassName, Intf1, Intf2, Intf3, Intf4 ) \
  ClassName : public EsBaseImplRc4< ClassName, Intf1, Intf2, Intf3, Intf4 > { \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL5( ClassName, Intf1, Intf2, Intf3, INtf4, Intf5 ) \
  ClassName : public EsBaseImplRc5< ClassName, Intf1, Intf2, Intf3, Intf4, Intf5 > { \
  ES_INTERNAL_INTF_IMPL( ClassName )

//---------------------------------------------------------------------------
/// Non-refcounted macros counterparts
///
#define ES_INTF_IMPL0_NON_RC( ClassName ) \
  ClassName : public EsBaseImpl0< ClassName > { \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL1_NON_RC( ClassName, Intf1 ) \
  ClassName : public EsBaseImpl1< ClassName, Intf1 > { \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL2_NON_RC( ClassName, Intf1, Intf2 ) \
  ClassName : public EsBaseImpl2< ClassName, Intf1, Intf2 > { \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL3_NON_RC( ClassName, Intf1, Intf2, Intf3 ) \
  ClassName : public EsBaseImpl3< ClassName, Intf1, Intf2, Intf3 > { \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL4_NON_RC( ClassName, Intf1, Intf2, Intf3, Intf4 ) \
  ClassName : public EsBaseImpl4< ClassName, Intf1, Intf2, Intf3, Intf4 > { \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL5_NON_RC( ClassName, Intf1, Intf2, Intf3, INtf4, Intf5 ) \
  ClassName : public EsBaseImpl5< ClassName, Intf1, Intf2, Intf3, Intf4, Intf5 > { \
  ES_INTERNAL_INTF_IMPL( ClassName )

//---------------------------------------------------------------------------
/// Macro to define class which is derived from interface implementing base
/// usage: class [optional export spec] ES_INTF_IMPL_DERIVED(ClassName, BaseClassName)
#define ES_INTF_IMPL_DERIVED( ClassName, BaseName ) \
ClassName : public BaseName { \
public: \
  static inline EsString classNameGetStatic() ES_NOTHROW { return ES_STRINGIZE(ClassName); } \
  ES_DECL_INTF_METHOD(EsString, classNameGet)() const ES_NOTHROW ES_OVERRIDE { return ES_STRINGIZE(ClassName); }

//---------------------------------------------------------------------------
/// Macros to declare class which is derived from base interface implementation class,
/// and implements additional interfaces.
/// Usage: class [optional export spec] ES_INTF_IMPL_DERIVEDN( ClassName, BaseName, Intf1[, ...] )
///
#define ES_INTF_IMPL_DERIVED1( ClassName, BaseName, Intf1 ) \
  ClassName : public EsDerivedImplRc1< ClassName, BaseName, Intf1 > { \
  protected: typedef EsDerivedImplRc1< ClassName, BaseName, Intf1 > BaseImplT; \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL_DERIVED2( ClassName, BaseName, Intf1, Intf2 ) \
  ClassName : public EsDerivedImplRc2< ClassName, BaseName, Intf1, Intf2 > { \
  protected: typedef EsDerivedImplRc2< ClassName, BaseName, Intf1, Intf2 > BaseImplT; \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL_DERIVED3( ClassName, BaseName, Intf1, Intf2, Intf3 ) \
  ClassName : public EsDerivedImplRc3< ClassName, BaseName, Intf1, Intf2, Intf3 > { \
  protected: typedef EsDerivedImplRc3< ClassName, BaseName, Intf1, Intf2, Intf3 > BaseImplT; \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL_DERIVED4( ClassName, BaseName, Intf1, Intf2, Intf3, Intf4 ) \
  ClassName : public EsDerivedImplRc4< ClassName, BaseName, Intf1, Intf2, Intf3, Intf4 > { \
  protected: typedef EsDerivedImplRc4< ClassName, BaseName, Intf1, Intf2, Intf3, Intf4 > BaseImplT; \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL_DERIVED5( ClassName, BaseName, Intf1, Intf2, Intf3, Intf4, Intf5 ) \
  ClassName : public EsDerivedImplRc5< ClassName, BaseName, Intf1, Intf2, Intf3, Intf4, Intf5 > { \
  protected: typedef EsDerivedImplRc5< ClassName, BaseName, Intf1, Intf2, Intf3, Intf4, Intf5 > BaseImplT; \
  ES_INTERNAL_INTF_IMPL( ClassName )

//---------------------------------------------------------------------------
/// Macros to declare class which is derived from base interface implementation class,
/// and implements additional interfaces, without refcounting capability.
/// Usage: class [optional export spec] ES_INTF_IMPL_DERIVEDN_NON_RC( ClassName, BaseName, Intf1[, ...] )
///
#define ES_INTF_IMPL_DERIVED1_NON_RC( ClassName, BaseName, Intf1 ) \
  ClassName : public EsDerivedImpl1< ClassName, BaseName, Intf1 > { \
  protected: typedef EsDerivedImpl1< ClassName, BaseName, Intf1 > BaseImplT; \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL_DERIVED2_NON_RC( ClassName, BaseName, Intf1, Intf2 ) \
  ClassName : public EsDerivedImpl2< ClassName, BaseName, Intf1, Intf2 > { \
  protected: typedef EsDerivedImpl2< ClassName, BaseName, Intf1, Intf2 > BaseImplT; \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL_DERIVED_NON_RC3( ClassName, BaseName, Intf1, Intf2, Intf3 ) \
  ClassName : public EsDerivedImpl3< ClassName, BaseName, Intf1, Intf2, Intf3 > { \
  protected: typedef EsDerivedImpl3< ClassName, BaseName, Intf1, Intf2, Intf3 > BaseImplT; \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL_DERIVED4_NON_RC( ClassName, BaseName, Intf1, Intf2, Intf3, Intf4 ) \
  ClassName : public EsDerivedImpl4< ClassName, BaseName, Intf1, Intf2, Intf3, Intf4 > { \
  protected: typedef EsDerivedImpl4< ClassName, BaseName, Intf1, Intf2, Intf3, Intf4 > BaseImplT; \
  ES_INTERNAL_INTF_IMPL( ClassName )

#define ES_INTF_IMPL_DERIVED5_NON_RC( ClassName, BaseName, Intf1, Intf2, Intf3, Intf4, Intf5 ) \
  ClassName : public EsDerivedImpl5< ClassName, BaseName, Intf1, Intf2, Intf3, Intf4, Intf5 > { \
  protected: typedef EsDerivedImpl5< ClassName, BaseName, Intf1, Intf2, Intf3, Intf4, Intf5 > BaseImplT; \
  ES_INTERNAL_INTF_IMPL( ClassName )

//---------------------------------------------------------------------------
/// Optional interfaced implementation closure
#define ES_INTF_IMPL_END };

//---------------------------------------------------------------------------
/// EsBaseIntf::Ptr equality operator.
///
/// @param  IntfT  Type of the typename intf t.
/// @param  _2  The second EsIntfPtr&lt;OtherIntfT&gt;
///
/// @return  true if contained interfaces are equal (shallow comparison).
///
template < typename IntfT >
template < typename OtherIntfT >
inline bool EsIntfPtr< IntfT >::operator== (const EsIntfPtr< OtherIntfT >& _2) const ES_NOTHROW
{
  EsBaseIntf::Ptr p2 = _2.template request< EsBaseIntf >();
  EsBaseIntf::Ptr p1 = request< EsBaseIntf >();

  return  p1 &&
          p2 &&
          p1.get() == p2.get();
}
#endif // _base_intf_h_
