/////////////////////////////////////////////////////////////////////////////
// Name:        settings.h
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "settings.h"
#endif

#include "wx/settings.h"
#include "wx/colour.h"
#include "wx/font.h"

wxColour wxSystemSettings::GetSystemColour(int WXUNUSED(index))
{
    // FIXME_MGL
    return wxColour(255,255,0);
}

wxFont wxSystemSettings::GetSystemFont(int WXUNUSED(index))
{
    // FIXME_MGL
    return wxFont(12, wxMODERN, wxNORMAL, wxNORMAL);
}

int wxSystemSettings::GetSystemMetric(int WXUNUSED(index))
{
    // FIXME_MGL
    return 1;
}
