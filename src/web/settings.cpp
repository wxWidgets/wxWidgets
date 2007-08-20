/////////////////////////////////////////////////////////////////////////////
// Name:        src/web/settings.cpp
// Purpose:     wxSettings
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: settings.cpp 39797 2006-06-19 20:18:46Z ABX $
// Copyright:   (c) Julian Smart, John Wilmes
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/settings.h"

#ifndef WX_PRECOMP
    #include "wx/gdicmn.h"
#endif

wxColour wxSystemSettingsNative::GetColour(wxSystemColour index)
{
    // Overridden mostly by wxSystemSettings::GetColour in wxUniversal
    // The only way we could do this anyway is with a custom wxColour class
    // that made use of the _depecrated_ CSS system color attributes
    return *wxWHITE;
}

wxFont wxSystemSettingsNative::GetFont(wxSystemFont index)
{
    //TODO
    return wxFont();
}

// Get a system metric, e.g. scrollbar size
int wxSystemSettingsNative::GetMetric(wxSystemMetric index, wxWindow* WXUNUSED(win))
{
    //TODO
    return -1;
}

bool wxSystemSettingsNative::HasFeature(wxSystemFeature index)
{
    // None of the features are supported:
    //  wxSYS_CAN_DRAW_FRAME_DECORATIONS
    //  wxSYS_CAN_ICONIZE_FRAME,
    //  wxSYS_TABLET_PRESENT
    return false;
}
