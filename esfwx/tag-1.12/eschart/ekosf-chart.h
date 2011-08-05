#ifndef _ekosf_chart_h_
#define _ekosf_chart_h_

#ifndef _ekosf_core_h_
#	error "ekosf-core.h must be included prior to this header!"
#endif

// communication stuff
//
// core export-import defines
#ifdef EKOSF_USEDLLS
#	ifdef ESCHART_EXPORTS
#		define EKOSF_CHART_CLASS	__declspec(dllexport)
#		define EKOSF_CHART_FUNC		__declspec(dllexport)
#		define EKOSF_CHART_DATA		__declspec(dllexport)
#	else
#		define EKOSF_CHART_CLASS	__declspec(dllimport)
#		define EKOSF_CHART_FUNC		__declspec(dllimport)
#		define EKOSF_CHART_DATA		__declspec(dllimport)
#	endif
// freechart linkage
# ifdef _DEBUG
#		pragma comment(lib, "wxcode_msw29ud_freechart")
# else
#		pragma comment(lib, "wxcode_msw29u_freechart")
# endif
#else
#	define EKOSF_CHART_CLASS
#	define EKOSF_CHART_FUNC
#	define EKOSF_CHART_DATA
#endif

// boost includes
#include <boost/shared_ptr.hpp>

// freechart includes
#include <wx/wxfreechartdefs.h>
#include <wx/chartpanel.h>
#include <wx/chart.h>
#include <wx/xy/xyplot.h>
#include <wx/xy/xyhistorenderer.h>
#include <wx/xy/vectordataset.h>

// our stuff
#include <eschart/EsDsUpdateLocker.h>
#include <eschart/EsSpecDataEvent.h>
#include <eschart/EsTransparentAreaDraw.h>
#include <eschart/EsSpecDataset.h>
#include <eschart/EsSpecView.h>

#endif // _ekosf_chart_h_