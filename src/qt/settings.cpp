/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/settings.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/settings.h"

wxColour wxSystemSettingsNative::GetColour(wxSystemColour index)
{
    return wxColour();
}

wxFont wxSystemSettingsNative::GetFont(wxSystemFont index)
{
    return wxFont();
}

int wxSystemSettingsNative::GetMetric(wxSystemMetric index, wxWindow * win)
{
    return 0;
}

bool wxSystemSettingsNative::HasFeature(wxSystemFeature index)
{
    return false;
}

