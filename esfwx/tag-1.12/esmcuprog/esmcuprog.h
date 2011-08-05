#ifndef _mcu_prog_h_
#define _mcu_prog_h_

#ifndef _ekosf_core_h_
#	error "ekosf-core.h must be included first"
#endif

#ifndef _ekosf_comm_h_
#	error "ekosf-comm.h must be included first"
#endif

#ifdef EKOSF_USEDLLS
#	ifdef MCUPROG_EXPORTS
#		define MCUPROG_CLASS	__declspec(dllexport)
#		define MCUPROG_FUNC		__declspec(dllexport)
#		define MCUPROG_DATA		__declspec(dllexport)
#	else
#		define MCUPROG_CLASS	__declspec(dllimport)
#		define MCUPROG_FUNC		__declspec(dllimport)
#		define MCUPROG_DATA		__declspec(dllimport)
#	endif
#else
#	define MCUPROG_CLASS
#	define MCUPROG_FUNC
#	define MCUPROG_DATA
#endif

// our forward declarations
class McuProgrammer;
class McuLpcProgrammer;

// our headers
#include <esmcuprog/EsFwMoniker.h>
#include <esmcuprog/McuProgrammer.h>

#endif // _mcu_prog_h_