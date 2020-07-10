#ifndef _es_path_enumerator_h_
#define _es_path_enumerator_h_

/// @file        EsPathEnumerator.h
///
///  Author:     Vsevolod V Gromov
///  Copyright:  ExacTSoft 2009-2013
///

/// Enumerator flags
enum class EsPathEnumeratorFlag : ulong
{
  Recursive       = 0x0001,     ///< Perform recursive path enumeration
  FollowSymlinks  = 0x0002,     ///< Follow symlinks during enumeration
  Default         = 0           ///< Default behaviour:
                                ///<  - do not recurse
                                ///<  - do not follow symlinks
};

/// Path objects enumerator helper class base.
/// Enumeration starts from the path specified in object ctor,
/// then, for each filesystem object, found in it, the virtual onObject
/// is called. Enumeration is finished when either onObject return false,
/// or all objects are processed.
///
class ESCORE_CLASS EsPathEnumerator
{
public:
  /// Constructor.
  ///
  /// @param  path  EsPath object containing enumeration starting point
  ///
  EsPathEnumerator(const EsPath& path, const EsString& wildcard = EsString::asterisk());

  /// Constructor.
  ///
  /// @param  path  EsString, containing enumeration starting point
  ///
  EsPathEnumerator(const EsString& path, const EsString& wildcard = EsString::asterisk());

  /// Destructor.
  virtual ~EsPathEnumerator() {}

  /// Execute enumeration process.
  ///
  /// @param  flags  (optional) enumeration behaviour flags.
  ///
  void execute(ulong flags = static_cast<ulong>(EsPathEnumeratorFlag::Default));

protected:
  /// Performs the start action at execution start.
  /// Default implementation does nothing
  ///
  virtual void onStart() {}

  /// Performs the ending action at execution completion.
  /// Default implementation does nothing
  ///
  virtual void onEnd() {}

  /// Executes the object action.
  ///
  /// @param  curPath  Full pathname of the current file.
  /// @param  name     The object (file or directory) name.
  /// @param  isDir     true if this object is directory.
  ///
  /// @return  true to continue, false to break enumeration.
  ///
  virtual bool onObject(const EsString& curPath, const EsString& name, bool isDir) = 0;

protected:
  bool internalProcess(const EsString& nestedDir = EsString::null());

protected:
  ulong m_flags;
  EsPath m_curPath;
  EsString m_wildcard;
};

/// Reflected path enumerator class. It's designed to be used in scripting,
/// namely, as member of some specific script object to perform enumeration process.
/// It implements onStart and onObject methods to look up the owning object for
/// actual implementation code. NB! owner is a weak reference to the owning object.
///
class ESCORE_CLASS ES_INTF_IMPL1(EsPathEnumeratorHelper, EsReflectedClassIntf)

protected:
  EsPathEnumeratorHelper(const EsBaseIntfPtr& owner);

public:
  ES_DECL_REFLECTED_CLASS_BASE(EsPathEnumeratorHelper)
  ES_DECL_ATTR_HANDLING_STD

  /// Reflected constructor. Owning object is passed as parameter
  ///
  ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, cr_EsBaseIntfPtr);

  /// Execute enumeration using path and wildcard strings, and flags
  ///
  ES_DECL_REFLECTED_METHOD3(void, enumerate, cr_EsString, cr_EsString, ulong);

protected:
  EsReflectedClassIntf::Ptr m_owner;

  ES_NO_DEFAULT_CTR(EsPathEnumeratorHelper);
  ES_NON_COPYABLE(EsPathEnumeratorHelper);

  friend class EsPathEnumeratorImpl;
};

/// Enumeration flags reflected
///
namespace EsReflection
{

ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCORE_CLASS, EsPathEnumeratorFlag)
  ES_DECL_ENUMERATION_ITEM_LABELLED(Recursive,        static_cast<ulong>(::EsPathEnumeratorFlag::Recursive), esT("Perform recursive path enumeration"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(FollowSymlinks,   static_cast<ulong>(::EsPathEnumeratorFlag::FollowSymlinks), esT("Follow symlinks during enumeration"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Default,          static_cast<ulong>(::EsPathEnumeratorFlag::Default), esT("Default behaviour: do not recurse, do not follow symlinks"))
ES_DECL_ENUMERATION_END

}

#endif // _es_path_enumerator_h_
