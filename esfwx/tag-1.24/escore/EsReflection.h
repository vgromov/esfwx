#ifndef _ekosf_es_reflection_h_
#define _ekosf_es_reflection_h_

// special marker macro - augment localizable property label extraction into pot file
#define _i    esT

// Forward declaration(s)
//
class EsPropertyInfo;
class EsMethodInfoKeyT;
class EsClassInfo;

#define ES_REFLECTED_PROPERTIES_RESET \
  classInfoGetStatic().resetAllProperties(this->asBaseIntf())

// reflected type aliasing. reflected type names must be single word without spaces
typedef const EsString&        cr_EsString;
#define To_EsString(var)      (var).asString()
#define To_cr_EsString(var)   (var).asString()
#define From_EsString(s)      (s)
#define From_cr_EsString(s)    (s)

typedef const EsString::Array& cr_EsStringArray;
#define To_EsStringArray(var)  (var).asStringCollection()
#define To_cr_EsStringArray(var) (var).asStringCollection()
#define From_EsStringArray(a)  (a)

#define To_p_void(var)        (var).asPointer()
#define To_cp_void(var)        (var).asPointer()
#define From_p_void(p)        EsVariant(p, EsVariant::ACCEPT_POINTER)

#define To_long(var)          (var).asLong()
#define To_cr_long(var)        (var).asLong()
#define From_long(l)          (l)

#define To_ulong(var)          (var).asULong()
#define To_cr_ulong(var)      (var).asULong()
#define From_ulong(u)          (u)

#define To_llong(var)          (var).asLLong()
#define To_cr_llong(var)      (var).asLLong()
#define From_llong(ll)        (ll)

#define To_ullong(var)        (var).asULLong()
#define To_cr_ullong(var)      (var).asULLong()
#define From_ullong(ull)      (ull)

#define To_double(var)        (var).asDouble()
#define To_cr_double(var)      (var).asDouble()
#define From_double(d)        (d)

#define To_bool(var)          (var).asBool()
#define To_cr_bool(var)        (var).asBool()
#define From_bool(b)          (b)

#define To_byte(var)          (var).asByte()
#define To_cr_byte(var)        (var).asByte()
#define From_byte(b)          (b)

typedef EsBaseIntf::Ptr        EsBaseIntfPtr;
typedef const EsBaseIntfPtr&  cr_EsBaseIntfPtr;
#define To_EsBaseIntfPtr(var)  (var).asObject()
#define To_cr_EsBaseIntfPtr(var) (var).asObject()
#define From_EsBaseIntfPtr(ip) (ip)

typedef const EsBinBuffer&    cr_EsBinBuffer;
#define To_cr_EsBinBuffer(var) (var).asBinBuffer()
#define From_EsBinBuffer(bb)  (bb)

// dummy define for variant-to variant casts
typedef const EsVariant&      cr_EsVariant;
#define To_EsVariant(var)      (var)
#define To_cr_EsVariant(var)  (var)
#define From_EsVariant(v)      (v)
#define From_cr_EsVariant(v)  (v)

// misc stubs
#define From_double(d)        (d)
#define From_int(i)            (i)

// RPC reflection stubs
#define To_esU8(var)          (var).asByte()
#define From_esU8(u)          (u)
#define To_esI8(var)          static_cast<esI8>((var).asInt())
#define From_esI8(i)          static_cast<int>(i)
#define To_esU16(var)          static_cast<esU16>((var).asULong())
#define From_esU16(u)          static_cast<ulong>(u)
#define To_WORD                To_esU16
#define To_esI16(var)          static_cast<esI16>((var).asLong())
#define From_esI16(i)          static_cast<int>(i)
#define To_esU32(var)          (var).asULong()
#define From_esU32(u)          (u)
#define To_esI32(var)          (var).asLong()
#define From_esI32(i)          (i)
#define To_esU64(var)          (var).asULLong()
#define From_esU64(u)          (u)
#define To_esI64(var)          (var).asLLong()
#define From_esI64(i)          (i)
#define To_esF(var)            static_cast<esF>((var).asDouble())
#define From_esF(f)            static_cast<double>(f)
#define To_esD(var)            static_cast<esD>((var).asDouble())
#define From_esD(d)            static_cast<double>(d)
#define To_esBL(var)          static_cast<esBL>((var).asBool() == true)
#define From_esBL(b)          static_cast<bool>((b) == TRUE)
#define To_esDT(var)          EsDateTime::fromVariant(var)
#define From_esDT(dt)          EsDateTime::toVariant(dt)
#define To_esBA(var)          static_cast<esBA>((var).asBinBuffer())
#define From_esBA(ba)          EsBinBuffer(ba)
// RPC return types stubs (for EsRpcServices thread-safe wrappers, namely for esBA safe copying on return)
#define esU8_TS                 esU8
#define esI8_TS                esI8
#define esU16_TS              esU16
#define esI16_TS               esI16
#define esU32_TS              esU32
#define esI32_TS               esI32
#define esU64_TS              esU64
#define esI64_TS               esI64
#define esF_TS                 esF
#define esD_TS                 esD
#define esBL_TS                esBL
#define esDT_TS                esDT
#define esBA_TS                EsBinBuffer
#define To_TS_TYPE(x)          x ##_TS
// POD reflection type standard range checks
#define Check_esU8_Range(var)  EsNumericCheck::checkRangeUInteger(0, std::numeric_limits<uint8_t>::max(), var.asULong())
#define Check_esI8_Range(var)  EsNumericCheck::checkRangeInteger(std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max(), var.asLong())
#define Check_esU16_Range(var)   EsNumericCheck::checkRangeUInteger(0, std::numeric_limits<uint16_t>::max(), var.asULong())
#define Check_esI16_Range(var)  EsNumericCheck::checkRangeInteger(std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max(), var.asLong())
#define Check_esU32_Range(var)  EsNumericCheck::checkRangeUInteger(0, std::numeric_limits<uint32_t>::max(), var.asULong())
#define Check_esI32_Range(var)  EsNumericCheck::checkRangeInteger(std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max(), var.asLong())
#define Check_esU64_Range(var)  EsNumericCheck::checkRangeULLInteger(0UL, std::numeric_limits<uint64_t>::max(), var.asULLong())
#define Check_esI64_Range(var)  EsNumericCheck::checkRangeLLInteger(std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max(), var.asLLong())
#define Check_esDT_Range      Check_esI64_Range
#define Check_esF_Range(var)  EsNumericCheck::checkRangeFloat(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), var.asDouble())
#define Check_esD_Range(var)  EsNumericCheck::checkRangeFloat(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), var.asDouble())

// reflection helpers
//
#include <escore/EsReflectionGenDefs.h>
//---------------------------------------------------------------------------

/// Metaclass services interface declaration
///
ES_DECL_INTF_BEGIN2( 364770DE, FD234A83, 8DD78971, 90410D5C, EsMetaclassIntf, EsBaseIntf )
  /// Inheritance access
  ///
  /// Return names of objects, acessible via reflection interfaces
  ES_DECL_INTF_METHOD(EsStringArray, objectNamesGet)() const ES_NOTHROW = 0;
  /// Return names of object(s), which are the base for the specified one
  ES_DECL_INTF_METHOD(EsStringArray, baseObjectNamesGet)(cr_EsString objName) const = 0;
  /// Return true if class in registered in reflection engine
  ES_DECL_INTF_METHOD(bool, hasClass)(cr_EsString objName) const ES_NOTHROW = 0;
  /// Query if 'className' has the 'otherClassName' as one of its base classes
  ES_DECL_INTF_METHOD(bool, isKindOf)(cr_EsString className, cr_EsString otherClassName) const = 0;

  /// Class attributes access
  ///
  /// Query if class has attribute
  ES_DECL_INTF_METHOD(bool, hasAttribute)(cr_EsString className, cr_EsString attrName) const = 0;
  /// Query names of all attributes for the specified class
  ES_DECL_INTF_METHOD(EsStringArray, attributeNamesGet)(cr_EsString className) const = 0;
  /// Return value associated with specified attribute
  ES_DECL_INTF_METHOD(EsVariant, attributeGet)(cr_EsString className, cr_EsString attrName) const =0;

  /// Method info access
  ///
  /// Query mangled names of object methods, including all its bases
  ES_DECL_INTF_METHOD(EsStringArray, methodNamesGet)(cr_EsString className) const = 0;
  /// Query mangled names of class methods, including all its bases
  ES_DECL_INTF_METHOD(EsStringArray, classMethodNamesGet)(cr_EsString className) const = 0;
  /// Query mangled names of object methods, this object only
  ES_DECL_INTF_METHOD(EsStringArray, thisMethodNamesGet)(cr_EsString className) const = 0;
  /// Query mangled names of class methods, this class only
  ES_DECL_INTF_METHOD(EsStringArray, thisClassMethodNamesGet)(cr_EsString className) const = 0;
  /// Query if object has specified method. methodName is expected to be mangled name
  ES_DECL_INTF_METHOD(bool, hasMethod)(cr_EsString className, cr_EsString methodName) const = 0;
  /// Query if class has specified method. methodName is expected to be mangled name
  ES_DECL_INTF_METHOD(bool, hasClassMethod)(cr_EsString className, cr_EsString methodName) const = 0;
  /// Query Fully Qualified method names for entire hierarchy
  ES_DECL_INTF_METHOD(EsStringArray, fqNamesGet)(cr_EsString className) const = 0;
  /// Query Fully Qualified method names for this class only
  ES_DECL_INTF_METHOD(EsStringArray, thisFqNamesGet)(cr_EsString className) const = 0;

  /// Method attributes access.
  ///

  /// Return method attribute based on Fully Qualified method name
  ES_DECL_INTF_METHOD(EsStringArray, methodAttributeNamesGet)(cr_EsString fqMethodName) const = 0;
  ES_DECL_INTF_METHOD(bool, methodHasAttribute)(cr_EsString fqMethodName, cr_EsString attrName) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, methodAttributeGet)(cr_EsString fqMethodName, cr_EsString attrName) const = 0;
  ES_DECL_INTF_METHOD(EsString, methodDescriptionGet)(cr_EsString fqMethodName) const = 0;

  /// Property info access
  ///
  /// Return names of properties of object and its bases
  ES_DECL_INTF_METHOD(EsStringArray, propertyNamesGet)(cr_EsString className) const = 0;
  ES_DECL_INTF_METHOD(EsStringArray, thisPropertyNamesGet)(cr_EsString className) const = 0;
  ES_DECL_INTF_METHOD(EsStringArray, persistentPropertyNamesGet)(cr_EsString className) const = 0;
  ES_DECL_INTF_METHOD(EsStringArray, thisPersistentPropertyNamesGet)(cr_EsString className) const = 0;
  ES_DECL_INTF_METHOD(bool, hasProperty)(cr_EsString className, cr_EsString propName) const = 0;
  ES_DECL_INTF_METHOD(bool, propertyCanRead)(cr_EsString className, cr_EsString propName) const = 0;
  ES_DECL_INTF_METHOD(bool, propertyCanWrite)(cr_EsString className, cr_EsString propName) const = 0;
  /// Property attributes access
  ///
  ES_DECL_INTF_METHOD(EsStringArray, propertyAttributeNamesGet)(cr_EsString className, cr_EsString propName) const = 0;
  ES_DECL_INTF_METHOD(bool, propertyHasAttribute)(cr_EsString className, cr_EsString propName, cr_EsString attrName) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, propertyAttributeGet)(cr_EsString className, cr_EsString propName, cr_EsString attrName) const = 0;
  ES_DECL_INTF_METHOD(EsString, propertyLabelGet)(cr_EsString className, cr_EsString propName) const = 0;
  ES_DECL_INTF_METHOD(EsString, propertyDescriptionGet)(cr_EsString className, cr_EsString propName) const = 0;

  /// Enumeration info access
  ///
  ES_DECL_INTF_METHOD(EsStringArray, enumerationNamesGet)() const = 0;
  ES_DECL_INTF_METHOD(EsStringArray, enumerationSymbolsGet)(cr_EsString enu) const = 0;

  /// Shortcuts for creating objects with different count of parameters passed to ctor
  ///
  ES_DECL_INTF_METHOD(EsVariant, objectCreate)(cr_EsString className) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, objectCreate)(cr_EsString className, cr_EsVariant p0) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, objectCreate)(cr_EsString className, cr_EsVariant p0, cr_EsVariant p1) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, objectCreate)(cr_EsString className, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, objectCreate)(cr_EsString className, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, objectCreate)(cr_EsString className, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4) const = 0;
ES_DECL_INTF_END
//---------------------------------------------------------------------------

/// Attributes container interface and its implementation
///
ES_DECL_INTF_BEGIN2(1072B084, AD7E4AD7, 8A468399, FF6FDFBB, EsAttributesIntf, EsBaseIntf)
  ES_DECL_INTF_METHOD(EsAttributesIntf::Ptr, clone)() const = 0;
  ES_DECL_INTF_METHOD(const EsString&, ownerNameGet)() const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(void, clearAll)() ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(EsStringArray, allNamesGet)() const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(const EsVariant&, attributeGet)(const EsString& name) const = 0;
  ES_DECL_INTF_METHOD(const EsVariant&, attributeGetDef)(const EsString& name, const EsVariant& def) const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(EsAttributesIntf::Ptr, attributeSet)(const EsString& name, const EsVariant& val, bool create = false) = 0;
  ES_DECL_INTF_METHOD(bool, attributeExists)(const EsString& name) const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(EsAttributesIntf::Ptr, attributeAdd)(const EsString& name, const EsVariant& val) = 0;
  ES_DECL_INTF_METHOD(bool, isEmpty)() const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(bool, isInterlocked)() const ES_NOTHROW = 0;
ES_DECL_INTF_END
//---------------------------------------------------------------------------

class ESCORE_CLASS ES_INTF_IMPL1(EsAttributes, EsAttributesIntf)

protected:
  EsAttributes(const EsString& ownerName, bool interlocked) ES_NOTHROW :
  m_contents(
    ownerName,
    interlocked ?
      EsStringIndexedMap::ContainerUsesInterlock :
      EsStringIndexedMap::ContainerWithoutInterlock
  )
  {}

public:
  static EsAttributesIntf::Ptr create(const EsString& ownerName, bool interlocked ) ES_NOTHROW;

  /// EsBaseIntf implementation
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE;

  /// EsAttributesIntf implementation
  ES_DECL_INTF_METHOD(EsAttributesIntf::Ptr, clone)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(const EsString&, ownerNameGet)() const ES_NOTHROW ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, clearAll)() ES_NOTHROW ES_OVERRIDE;
  ES_DECL_INTF_METHOD(EsStringArray, allNamesGet)() const ES_NOTHROW ES_OVERRIDE;
  ES_DECL_INTF_METHOD(const EsVariant&, attributeGet)(const EsString& name) const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(const EsVariant&, attributeGetDef)(const EsString& name, const EsVariant& def) const ES_NOTHROW ES_OVERRIDE;
  ES_DECL_INTF_METHOD(EsAttributesIntf::Ptr, attributeSet)(const EsString& name, const EsVariant& val, bool create = false) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(bool, attributeExists)(const EsString& name) const ES_NOTHROW ES_OVERRIDE;
  ES_DECL_INTF_METHOD(EsAttributesIntf::Ptr, attributeAdd)(const EsString& name, const EsVariant& val) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(bool, isEmpty)() const ES_NOTHROW ES_OVERRIDE { return m_contents.isEmpty(); }
   ES_DECL_INTF_METHOD(bool, isInterlocked)() const ES_NOTHROW ES_OVERRIDE { return m_contents.isInterlocked(); }

protected:
  EsStringIndexedMap m_contents;

private:
  // prohibited functionality
  EsAttributes() ES_REMOVEDECL;
  EsAttributes(const EsAttributes& other) ES_REMOVEDECL;
  EsAttributes& operator=(const EsAttributes& other) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

/// EsReflectedClassIntf declaration - core of reflection API
///
ES_DECL_INTF_BEGIN2( 97C0CB8C, 421B4ef5, A85E85C3, 107B1948, EsReflectedClassIntf, EsBaseIntf )
  /// Reflection interface methods
  ///
  ES_DECL_INTF_METHOD(const EsClassInfo&, classInfoGet)() const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(EsMetaclassIntf::Ptr, metaGet)() const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(bool, isKindOf)(const EsReflectedClassIntf::Ptr& other) const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(bool, isKindOf)(const EsString& other) const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(bool, isKindOf)(const EsClassInfo& other) const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(void, copyFrom)(const EsBaseIntfPtr& other) = 0;
  ES_DECL_INTF_METHOD(EsBaseIntfPtr, asWeakReference)() const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(bool, isIndexed)() const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(bool, isScripted)() const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(void, attributesAssign)(const EsAttributesIntf::Ptr& attrs) = 0;
  ES_DECL_INTF_METHOD(void, attributeAdd)(const EsString& name, const EsVariant& val = EsVariant::null()) = 0;
  ES_DECL_INTF_METHOD(bool, hasAttribute)(const EsString& name) const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(EsString::Array, attributeNamesGet)() const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(EsVariant, attributeGet)(const EsString& name) const = 0;
  ES_DECL_INTF_METHOD(EsString::Array, propertyNamesGet)() const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(EsString::Array, persistentPropertyNamesGet)() const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(bool, hasProperty)(const EsString& name) const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(bool, propertyCanRead)(const EsString& name) const = 0;
  ES_DECL_INTF_METHOD(bool, propertyCanWrite)(const EsString& name) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, propertyGet)(const EsString& name) const = 0;
  ES_DECL_INTF_METHOD(void, propertySet)(const EsString& name, const EsVariant& val) = 0;
  ES_DECL_INTF_METHOD(EsStringArray, propertyAttributeNamesGet)(const EsString& name) const = 0;
  ES_DECL_INTF_METHOD(bool, propertyHasAttribute)(const EsString& name, const EsString& attrName) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, propertyAttributeGet)(const EsString& name, const EsString& attrName) const = 0;
  ES_DECL_INTF_METHOD(EsString, propertyLabelGet)(const EsString& name) const = 0;
  ES_DECL_INTF_METHOD(EsString, propertyDescriptionGet)(const EsString& name) const = 0;
  ES_DECL_INTF_METHOD(bool, hasMethod)(const EsMethodInfoKeyT& key) const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(bool, hasClassMethod)(const EsMethodInfoKeyT& key) const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(EsVariant, callMethod)(const EsString& name, const EsVariant& params) = 0;
  ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name) = 0;
  ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1) = 0;
  ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2) = 0;
  ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) = 0;
  ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) = 0;
  ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) = 0;
  ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6) = 0;
  ES_DECL_INTF_METHOD(EsVariant, callClassMethod)(const EsString& name, const EsVariant& params) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6) const = 0;
ES_DECL_INTF_END
//---------------------------------------------------------------------------

// generate reflection signatures
#include <escore/EsReflectionDefSignatures.h>
// generate reflected methods parameter count
#include <escore/EsReflectionDefParamCount.h>
// generate reflected method type casts
#include <escore/EsReflectionDefTypeCasts.h>
//---------------------------------------------------------------------------

/// Method info map key. both name, and parameters count used as id info
///
class ESCORE_CLASS EsMethodInfoKeyT
{
public:
  EsMethodInfoKeyT(ulong paramsCount = 0, const EsString& name = EsString::null()) ES_NOTHROW :
  m_paramsCount(paramsCount), m_name(name)
  { m_name.hashGet();  }

  EsMethodInfoKeyT(const EsString& name, const EsVariant& params) ES_NOTHROW;

  EsMethodInfoKeyT(const EsMethodInfoKeyT& other) ES_NOTHROW :
  m_paramsCount(other.m_paramsCount), m_name(other.m_name)
  { m_name.hashGet();  }

  EsMethodInfoKeyT(const EsString& mangledName);

  inline bool isOk() const ES_NOTHROW { return !m_name.empty(); }
  inline const EsString& nameGet() const ES_NOTHROW { return m_name; }
  inline ulong parametersCountGet() const ES_NOTHROW { return m_paramsCount; }

  // Assignment
  EsMethodInfoKeyT& operator=(const EsMethodInfoKeyT& other) ES_NOTHROW
  {
    m_paramsCount = other.m_paramsCount;
    m_name = other.m_name;
    m_name.hashGet();

    return *this;
  }

  // Comparison operators, augmenting map key requirements
  bool operator< (const EsMethodInfoKeyT& other) const ES_NOTHROW;
  inline bool operator ==(const EsMethodInfoKeyT& other) const ES_NOTHROW
  {  return m_name == other.m_name && m_paramsCount == other.m_paramsCount; }
  // return method info key as string
  EsString asString() const;

#ifdef ES_MODERN_CPP
  // Move constructor
  EsMethodInfoKeyT(EsMethodInfoKeyT&& other) ES_NOTHROW :
  m_paramsCount(other.m_paramsCount),
  m_name(other.m_name)
  { m_name.hashGet(); }

  // Move assignment
  EsMethodInfoKeyT& operator=(EsMethodInfoKeyT&& other) ES_NOTHROW
  {
    m_paramsCount = other.m_paramsCount;
    m_name = other.m_name;
    m_name.hashGet();

    return *this;
  }
#endif

protected:
  void fromMangledName(const EsString& mangledName);

protected:
  ulong m_paramsCount;
  EsString m_name;
};
typedef std::vector<EsMethodInfoKeyT> EsMethodInfoKeysT;
//---------------------------------------------------------------------------

/// Method information metaclass
///
class ESCORE_CLASS EsMethodInfo
{
protected:
  // signature strings container
  class ESCORE_CLASS SigStringsT
  {
  public:
    SigStringsT() { init(); }
    ~SigStringsT() ES_NOTHROW;

    const EsString& stringGet(long sig) const ES_NOTHROW;

  protected:
    SigStringsT& add(MethodSignature sig, const EsString& str);
    void init();

  protected:
    typedef std::map<long, EsString> SigsT;
    SigsT m_sigs;
  };

public:
  EsMethodInfo(EsClassInfo& owner, long sig,  ulong paramsCount, const EsString& name,
    const EsString& descr, EsMemberCallT method) ES_NOTHROW;
  EsMethodInfo(EsClassInfo& owner, long sig, ulong paramsCount, const EsString& name,
    const EsString& descr, EsClassCallT classMethod, long tagClassCallInfo) ES_NOTHROW;
  EsMethodInfo(EsClassInfo& owner, long sig, const EsIID& iid, ulong paramsCount,
    const EsString& name, const EsString& descr, EsMemberCallT intfMethod) ES_NOTHROW;
  ~EsMethodInfo() ES_NOTHROW;

  /// Member access
  inline const EsClassInfo& classInfoGet() const ES_NOTHROW { return m_owner; }
  inline long signatureGet() const ES_NOTHROW { return m_signature; }
  inline long parametersCountGet() const ES_NOTHROW { return m_paramsCount; }
  inline const EsString& nameGet() const ES_NOTHROW { return m_name; }
  EsString descriptionGet() const;

  /// Access attribute storage
  inline EsAttributesIntf::Ptr attributesAccess() ES_NOTHROW { return m_attrs; }
  inline const EsAttributesIntf::Ptr attributesAccess() const ES_NOTHROW { return m_attrs; }

  /// Signature checks
  inline bool isClassMethod() const ES_NOTHROW { return m_signature > maxMethodSignature && m_signature < classMethodSignaturesEnd; }
  static void checkSignature(long sig);

  /// Return signature string by signature id
  inline const EsString& signatureStringGet() const { return signatureStringGet(m_signature); }
  static inline const EsString& signatureStringGet(long sig) { return sigs().stringGet(sig); }

  /// Return mangled method name MethodName|ParamCount
  inline EsString mangledNameGet() const { return m_name + esT("|") + EsString::fromLong(m_paramsCount); }

  /// Return Fully Qualified method name
  /// Class method FQ names are ClassName::MethodName|ParamCount
  /// Method FQ names are ClassName.MethodName|ParamCount
  ///
  EsString fqNameGet() const;

  /// Parse FQ name into ClassName, MethodName, ParamCount, and Signature
  static void fqNameParse(const EsString& fqName, EsString& className, bool& isClassMethod,
    EsString& methodName, ulong& paramCount, EsString& signature);

  /// Parse FQ name and try to obtain corresponding method info record
  static const EsMethodInfo* infoFromFqNameGet(const EsString& fqName, bool doThrow, bool allHierarchy = true);

  /// Caller methods
  ///

  /// Object member function caller
  EsVariant call(EsBaseIntf* obj, const EsVariant& params) const;

  /// Class function caller
  EsVariant classCall(const EsVariant& params) const;

protected:
  // Parameter checks
  void checkParamsCount(const EsVariant& params) const;

  // Signature strings access
  static const SigStringsT& sigs() ES_NOTHROW;

private:
  // prohibited functionality
  EsMethodInfo() ES_REMOVEDECL;
  EsMethodInfo(const EsMethodInfo&) ES_REMOVEDECL;
  EsMethodInfo& operator=(const EsMethodInfo&) ES_REMOVEDECL;

protected:
  const EsClassInfo& m_owner;
  // method name & attributes.
  EsString m_name;
  EsAttributesIntf::Ptr m_attrs;
  // method signature, interface IID (optional) & param count
  EsIID m_iid;
  long m_signature;
  ulong m_paramsCount;
  // method pointer
  EsMethodT m_method;
};
//---------------------------------------------------------------------------

/// Property info metaclass
///
class ESCORE_CLASS EsPropertyInfo
{
public:
  EsPropertyInfo(EsClassInfo& owner, const EsString& name, const EsString& label, const EsString& descr,
    const EsVariant& def, EsMemberCallT reader, long readerSig, EsMemberCallT writer, long writerSig,
    const EsVariant& restriction, bool persistent = false
  );

  // Property info services
  //
  inline const EsClassInfo& classInfoGet() const ES_NOTHROW { return m_owner; }
  bool validate(const EsVariant& val, bool doThrow = false) const;
  const EsVariant& defaultGet() const;
  bool defaultExists() const ES_NOTHROW;
  const EsVariant& restrictionGet() const;
  bool restrictionExists() const ES_NOTHROW;
  inline const EsString& nameGet() const ES_NOTHROW { return m_name; }
  EsString labelGet() const ES_NOTHROW;
  EsString descriptionGet() const ES_NOTHROW;
  bool isPersistent() const ES_NOTHROW;
  inline bool canRead() const ES_NOTHROW { return 0 != m_reader; }
  inline bool canWrite() const ES_NOTHROW { return 0 != m_writer; }
  // attributes access
  inline EsAttributesIntf::Ptr attributesAccess() ES_NOTHROW { return m_attrs; }
  inline const EsAttributesIntf::Ptr attributesAccess() const ES_NOTHROW { return m_attrs; }
  // generic value access
  EsVariant get(const EsBaseIntf* obj) const;
  void set(EsBaseIntf* obj, const EsVariant& val) const;
  // reset property value to default, if there is one. if property has no default, exception is thrown
  void reset(EsBaseIntf* obj) const;

private:
  // prohibited services
  EsPropertyInfo() ES_REMOVEDECL;
  EsPropertyInfo( const EsPropertyInfo& ) ES_REMOVEDECL;
  EsPropertyInfo& operator=( const EsPropertyInfo& ) ES_REMOVEDECL;

protected:
  const EsClassInfo& m_owner;
  EsString m_name;
  EsAttributesIntf::Ptr m_attrs;
  EsMemberCallT m_reader;
  int m_readerSignature;
  EsMemberCallT m_writer;
  int m_writerSignature;
};
//---------------------------------------------------------------------------

/// Metaclass information holder
///
typedef EsStringIndexedMap& (*EsClassRegistryAccessorT)();

class ESCORE_CLASS EsClassInfo
{
public:
  typedef std::list<const EsPropertyInfo*> PropertyInfosT;
  typedef std::map<EsMethodInfoKeyT, const EsMethodInfo*> MethodInfosT;

public:
  EsClassInfo(const EsString& name, const EsString& descr, const EsClassInfo* ancestor) ES_NOTHROW;
  ~EsClassInfo() ES_NOTHROW;

  /// Inheritance access
  bool isKindOf(const EsString& name) const ES_NOTHROW;
  inline bool isKindOf(const EsClassInfo& info) const ES_NOTHROW { return isKindOf( info.nameGet() ); }

  /// Class info access
  inline const EsString& nameGet() const ES_NOTHROW { return m_name; }
  EsString descriptionGet() const ES_NOTHROW;

  /// Unregister class info from classinfo reistry
  void doRegister(EsClassRegistryAccessorT acc = nullptr) const;
  void unregister(EsClassRegistryAccessorT acc = nullptr) const;

  /// Ancestor info access
  inline bool hasAncestor() const ES_NOTHROW { return 0 != m_ancestor; }
  inline const EsClassInfo* ancestorGet() const ES_NOTHROW { return m_ancestor; }

  /// Attributes access
  inline void attributesAssign(const EsAttributesIntf::Ptr& attrs) ES_NOTHROW { m_attrs = attrs; }
  inline void attributeAdd(const EsString& name, const EsVariant& val) { ES_ASSERT(m_attrs); m_attrs->attributeAdd(name, val); }
  inline EsAttributesIntf::Ptr attributesAccess() ES_NOTHROW { return m_attrs; }
  inline const EsAttributesIntf::Ptr attributesAccess() const ES_NOTHROW { return m_attrs; }

  /// Reflected property information access
  EsString::Array propertyNamesGet(bool onlyPersistent = false, bool allHierarchy = true) const ES_NOTHROW;
  EsString::Array persistentPropertyNamesGet(bool allHierarchy = true) const ES_NOTHROW
  { return propertyNamesGet(true, allHierarchy); }

  bool hasProperty(const EsString& name, bool allHierarchy = true) const ES_NOTHROW;
  const EsPropertyInfo* propertyInfoFind(const EsString& name, bool allHierarchy = true ) const ES_NOTHROW;
  const EsPropertyInfo& propertyInfoGet(const EsString& name, bool allHierarchy = true ) const;
  PropertyInfosT propertyInfosGet(bool onlyPersistent = false, bool allHierarchy = true) const ES_NOTHROW;
  PropertyInfosT persistentPropertyInfosGet(bool allHierarchy = true) const ES_NOTHROW
  { return propertyInfosGet(true, allHierarchy); }

  /// Reflected method infos access
  EsMethodInfoKeysT methodKeysGet(bool allHierarchy = true) const ES_NOTHROW;
  bool hasMethod( const EsMethodInfoKeyT& key, bool allHierarchy = true ) const ES_NOTHROW;
  const EsMethodInfo* methodInfoFind(const EsMethodInfoKeyT& key, bool allHierarchy = true ) const ES_NOTHROW;
  const EsMethodInfo& methodInfoGet(const EsMethodInfoKeyT& key, bool allHierarchy = true ) const;
  EsMethodInfoKeysT classMethodKeysGet(bool allHierarchy = false) const ES_NOTHROW;
  bool hasClassMethod( const EsMethodInfoKeyT& key, bool allHierarchy = true ) const ES_NOTHROW;
  const EsMethodInfo* classMethodInfoFind(const EsMethodInfoKeyT& key, bool allHierarchy = true ) const ES_NOTHROW;
  const EsMethodInfo& classMethodInfoGet(const EsMethodInfoKeyT& key, bool allHierarchy = false ) const;

  /// FQ names access
  EsString::Array fqMethodNamesGet(bool allHierarchy = true) const ES_NOTHROW;

  /// Property services simplified
  EsVariant propertyGet(EsBaseIntf* obj, const EsString& name) const;
  void propertySet(EsBaseIntf* obj, const EsString& name, const EsVariant& val) const;
  void propertyReset(EsBaseIntf* obj, const EsString& name) const;
  void resetAllProperties(EsBaseIntf* obj) const;

  /// Generic checks
  ///

  /// isIndexed returns true if standard indexed access services are registered for the class:
  /// countGet() and itemGet(index) and | or itemSet(index, value)
  ///
  bool isIndexed() const ES_NOTHROW;

  /// Method services simplified
  EsVariant call(EsBaseIntf* obj, const EsString& name) const;
  EsVariant call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1) const;
  EsVariant call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2) const;
  EsVariant call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) const;
  EsVariant call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) const;
  EsVariant call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) const;
  EsVariant call(EsBaseIntf* obj, const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6) const;
  EsVariant callMethod(EsBaseIntf* obj, const EsString& name, const EsVariant& params) const;
  EsVariant classCall(const EsString& name) const;
  EsVariant classCall(const EsString& name, const EsVariant& param1) const;
  EsVariant classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2) const;
  EsVariant classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) const;
  EsVariant classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) const;
  EsVariant classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) const;
  EsVariant classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6) const;
  EsVariant classCallMethod(const EsString& name, const EsVariant& params) const;

  /// Static services. registered reflected classes access
  ///
  static EsString::Array classNamesGet() ES_NOTHROW;
  static const EsClassInfo* classInfoGet(const EsString& name, bool Throw = false );

  // Use "local static init on first use" idiom for the class info registry
  static EsStringIndexedMap& classes() ES_NOTHROW;

protected:
  // misc registration helpers
  void appendPropertyNames(EsString::Array& out, bool onlyPersistent, bool allHierarchy) const;
  void appendPropertyInfos(PropertyInfosT& out, bool onlyPersistent, bool allHierarchy) const;
  void appendMethodKeys(EsMethodInfoKeysT& out, bool allHierarchy) const;
  void appendClassMethodKeys(EsMethodInfoKeysT& out, bool allHierarchy) const;

protected:
  // Ancestor classes
  const EsClassInfo* m_ancestor;

  // Class name
  EsString m_name;
  // class attributes
  EsAttributesIntf::Ptr m_attrs;

  // reflected properties
  PropertyInfosT m_propInfos;
  // normal reflected services
  MethodInfosT m_methods;
  // reflected class services
  MethodInfosT m_classMethods;

private:
  EsClassInfo() ES_REMOVEDECL;
  EsClassInfo(const EsClassInfo&) ES_REMOVEDECL;
  EsClassInfo& operator=(const EsClassInfo&) ES_REMOVEDECL;

  friend class EsPropertyInfo;
  friend class EsMethodInfo;
};
//---------------------------------------------------------------------------

#include <escore/EsReflectionMethodDeclDefs.h>
#include <escore/EsReflectionMethodInfoDeclDefs.h>
#include <escore/EsReflectionPropertyDeclDefs.h>
#include <escore/EsReflectionReflectedClassDefs.h>

#define ES_THIS_REFLECTED_PROP_INFO_GET(Name) \
  (classInfoGetStatic().propertyInfoGet( ES_STRINGIZE(Name) ))

#define ES_REFLECTED_PROP_INFO_GET(Class, Name) \
  (Class ::classInfoGetStatic().propertyInfoGet( ES_STRINGIZE(Name) ))

/// Reflection registrar declaration and implementation macros
///
#  define ES_DECL_REFLECTION_REGISTRAR(ExportSpec, ModuleName) \
  namespace ModuleName { ExportSpec( void, reflectionInit() ); \
  ExportSpec( void, reflectionInitDynamic(EsClassRegistryAccessorT) ); \
  ExportSpec( void, reflectionUninitDynamic() ); } \
  extern "C" ExportSpec( void, _##ModuleName##_##reflectionInit() ); \
  extern "C" ExportSpec( void, _##ModuleName##_##reflectionInitDynamic(void* accPfn) ); \
  extern "C" ExportSpec( void, _##ModuleName##_##reflectionUninitDynamic() );

#  define ES_DECL_REFLECTION_REGISTRAR_NO_EXPORT(ModuleName) \
  namespace ModuleName { void reflectionInit(); \
  void reflectionInitDynamic(EsClassRegistryAccessorT); \
  void reflectionUninitDynamic(); } \
  extern "C" void _##ModuleName##_##reflectionInit();

#  define ES_REFLECTION_REGISTRAR_BEGIN(ModuleName) \
  void _##ModuleName##_##reflectionInit() { ModuleName ::reflectionInit(); } \
  void _##ModuleName##_##reflectionInitDynamic(void* accPfn) { ModuleName ::reflectionInitDynamic( reinterpret_cast<EsClassRegistryAccessorT>(accPfn) ); } \
  void _##ModuleName##_##reflectionUninitDynamic() { ModuleName ::reflectionUninitDynamic(); } \
  namespace ModuleName { \
  static EsClassRegistryAccessorT& dynInitAccess() { static EsClassRegistryAccessorT s_acc = nullptr; return s_acc; } \
  static std::list<const EsClassInfo*>& dynClassInfosAccess() { static std::list<const EsClassInfo*> s_infos; return s_infos; } \
  } \
  void ModuleName ::reflectionInitDynamic(EsClassRegistryAccessorT acc) { ES_ASSERT(acc); dynInitAccess() = acc; reflectionInit(); } \
  void ModuleName ::reflectionUninitDynamic() { \
    if( !dynInitAccess() ) return; \
    try { \
    while( !dynClassInfosAccess().empty() ) { \
      dynClassInfosAccess().back()->unregister( dynInitAccess() ); \
      dynClassInfosAccess().pop_back(); \
    } } \
    catch( ... ) {} \
    dynInitAccess() = nullptr; \
  } \
  void ModuleName ::reflectionInit() {

#define ES_REFLECTION_REGISTRAR_ENTRY(ClassName) \
  if( dynInitAccess() ) { \
    const EsClassInfo& ci = ClassName ::classInfoGetStatic(); \
    dynClassInfosAccess().push_back( &ci ); \
    ci.doRegister( dynInitAccess() ); \
  } else { \
    ClassName ::classInfoGetStatic(); \
  }

#define ES_REFLECTION_REGISTRAR_END }

// specific macros family, allowing to utilize dualistic nature of classes implementing reflected interfaces
// the specific object calls on specific interfaces may be tried first on native (C++) interface,
// if one is implemented by the object, otherwise, if object implements reflection interface,
// the named service call attempt is performed
//
#include <escore/EsReflectionDualCallDefs.h>

#endif // _ekosf_es_reflection_h_
