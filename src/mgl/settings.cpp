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
    return wxColour(0,0,0);
}

wxFont wxSystemSettings::GetSystemFont(int WXUNUSED(index))
{
    // FIXME_MGL
    return wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, "Arial");
}

int wxSystemSettings::GetSystemMetric(int WXUNUSED(index))
{
    // FIXME_MGL
    return 1;
}

bool wxSystemSettings::GetCapability(int index)
{
    switch (index)
    {
        case wxSYS_CAN_ICONIZE_FRAME: 
            return FALSE; break;
        case wxSYS_CAN_DRAW_FRAME_DECORATIONS:
            return FALSE; break;
        default:
            return FALSE;
    }
}
