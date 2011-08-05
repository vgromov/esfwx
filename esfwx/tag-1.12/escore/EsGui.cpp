#include <stdafx.h>
#pragma hdrstop

#include "EsGui.h"

// color convertions
ESGUI_COLOR EsGui::fromWxColor(const wxColor& clr)
{
	ESGUI_COLOR result;
	result.rgba.a = clr.Alpha();
	result.rgba.b = clr.Blue();
	result.rgba.g = clr.Green();
	result.rgba.r = clr.Red();
	
	return result;
}

wxColor EsGui::toWxColor(ESGUI_COLOR clr)
{
	return wxColor(clr.rgba.r, clr.rgba.g, clr.rgba.b, clr.rgba.a);
}


