#ifndef _es_script_compiler_h_
#define _es_script_compiler_h_

namespace EsScriptUtilities
{

/// Compile script into machine instructions.
/// @param src      [in]      string containing script text
/// @param machine  [in, out] reference to the script machine object, which is populated
///                           by compiled instructions and data
/// @param brk      [in]      optional parameter which allow passing breaker interface to the
///                           compiler, and aborting compilation process
///
ESSCRIPT_FUNC(bool, compileFromString(
  const EsString& src,
  EsScriptMachine& machine, 
  const EsAssocContainerIntf::Ptr& modulesInfo,
  const EsBreakIntf::Ptr& brk = EsBreakIntf::Ptr()
) );

/// Compile script file into machine instructions.
/// @param src      [in]      string containing input script file
/// @param machine  [in, out] reference to the script machine object, which is populated
///                           by compiled instructions and data
/// @param brk      [in]      optional parameter which allow passing breaker interface to the
///                           compiler, and aborting compilation process
ESSCRIPT_FUNC(bool, compileFromFile(
  const EsString& src,
  EsScriptMachine& machine, 
  const EsAssocContainerIntf::Ptr& modulesInfo,
  const EsBreakIntf::Ptr& brk = EsBreakIntf::Ptr()
) );

/// Compile scriptlet contents into machine instructions.
/// @param src      [in]      reference to scriptlet to be compiled
/// @param brk      [in]      parameter which allow passing breaker interface to the
///                           compiler, and aborting compilation process
///
ESSCRIPT_FUNC(bool, compileFromScriptlet(EsScriptlet& src, const EsBreakIntf::Ptr& brk) );

}

#endif // _es_script_compiler_h_

