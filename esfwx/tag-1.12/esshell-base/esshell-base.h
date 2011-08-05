#ifndef _shell_core_h_
#define _shell_core_h_

#ifndef _ekosf_core_h_
#	error "ekosf-core.h must be included prior to this header!"
#endif

#ifndef _ekosf_comm_h_
#	error "ekosf-comm.h must be included prior to this header!"
#endif

// core export-import defines
#ifdef EKOSF_USEDLLS
#	ifdef SHELL_BASE_EXPORTS
#		define EKOSF_SHELL_BASE_CLASS	__declspec(dllexport)
#		define EKOSF_SHELL_CORE_FUNC	__declspec(dllexport)
#		define EKOSF_SHELL_CORE_DATA	__declspec(dllexport)
#	else
#		define EKOSF_SHELL_BASE_CLASS	__declspec(dllimport)
#		define EKOSF_SHELL_CORE_FUNC	__declspec(dllimport)
#		define EKOSF_SHELL_CORE_DATA	__declspec(dllimport)
#	endif
#else
#	define EKOSF_SHELL_BASE_CLASS
#	define EKOSF_SHELL_CORE_FUNC
#	define EKOSF_SHELL_CORE_DATA
#endif

class FrmCommLog;
class RemoteFsTree;
class EsTimeCtrl;
class SetDateTimePane;
class SetDateTimeDlg;
class EsThread;
class ShellApp;
class FrmMain;

#include <esshell-base/FrmLog.h>
#include <esshell-base/RemoteFsTree.h>
#include <esshell-base/NotifierCtl.h>
#include <esshell-base/EsTimeCtrl.h>
#include <esshell-base/SetDateTimePane.h>
#include <esshell-base/SetDateTimeDlg.h>
#include <esshell-base/ShellApp.h>
#include <esshell-base/FrmMain.h>
#include <esshell-base/DataFileView.h>

#endif // _shell_core_h_
