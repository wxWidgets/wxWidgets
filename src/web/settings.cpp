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
    return *wxWHITE;
}

wxFont wxSystemSettingsNative::GetFont(wxSystemFont index)
{
    return wxFont();
}

// Get a system metric, e.g. scrollbar size
int wxSystemSettingsNative::GetMetric(wxSystemMetric index, wxWindow* WXUNUSED(win))
{
    return -1;
}

bool wxSystemSettingsNative::HasFeature(wxSystemFeature index)
{
    return false;
}
