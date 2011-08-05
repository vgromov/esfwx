#ifndef _Es_Gui_h_
#define _Es_Gui_h_

#include <esgui/include/esgui_types.h>

// some esgui interwork wrappers
//
namespace EsGui
{
// color palette
typedef std::vector<ESGUI_COLOR> PaletteT;

// color convertions
EKOSF_CORE_FUNC ESGUI_COLOR fromWxColor(const wxColor& clr);
EKOSF_CORE_FUNC wxColor toWxColor(ESGUI_COLOR clr);

}

#endif // _Es_Gui_h_
