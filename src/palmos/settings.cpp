/////////////////////////////////////////////////////////////////////////////
// Name:        palmos/settings.cpp
// Purpose:     wxSystemSettingsNative implementation for Palm OS
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id:
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/gdicmn.h"
#endif

#include "wx/settings.h"

#ifndef SPI_GETFLATMENU
#define SPI_GETFLATMENU                     0x1022
#endif

#include "wx/module.h"
#include "wx/fontutil.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// global data
// ----------------------------------------------------------------------------

// the font returned by GetFont(wxSYS_DEFAULT_GUI_FONT): it is created when
// GetFont() is called for the first time and deleted by wxSystemSettingsModule
static wxFont *gs_fontDefault = NULL;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxSystemSettingsNative
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// colours
// ----------------------------------------------------------------------------

wxColour wxSystemSettingsNative::GetColour(wxSystemColour index)
{
    return wxNullColour;
}

// ----------------------------------------------------------------------------
// fonts
// ----------------------------------------------------------------------------

wxFont wxCreateFontFromStockObject(int index)
{
    wxFont font;
    return font;
}

wxFont wxSystemSettingsNative::GetFont(wxSystemFont index)
{
    wxFont font;
    return font;
}

// ----------------------------------------------------------------------------
// system metrics/features
// ----------------------------------------------------------------------------

// Get a system metric, e.g. scrollbar size
int wxSystemSettingsNative::GetMetric(wxSystemMetric index)
{
    return 0;
}

bool wxSystemSettingsNative::HasFeature(wxSystemFeature index)
{
    return false;
}

// ----------------------------------------------------------------------------
// function from wx/msw/wrapcctl.h: there is really no other place for it...
// ----------------------------------------------------------------------------

#if wxUSE_LISTCTRL || wxUSE_TREECTRL

extern wxFont wxGetCCDefaultFont()
{
    wxFont font;
    return font;
}

#endif // wxUSE_LISTCTRL || wxUSE_TREECTRL
