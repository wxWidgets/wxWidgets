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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/settings.h"
#include "wx/colour.h"
#include "wx/font.h"
#include "wx/module.h"

// ----------------------------------------------------------------------------
// global data
// ----------------------------------------------------------------------------

static wxFont *gs_fontDefault = NULL;

class wxSystemSettingsModule : public wxModule
{
public:
    virtual bool OnInit() { return TRUE; }
    virtual void OnExit()
    {
        delete gs_fontDefault;
        gs_fontDefault = NULL;
    }

private:
    DECLARE_DYNAMIC_CLASS(wxSystemSettingsModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxSystemSettingsModule, wxModule)



wxColour wxSystemSettings::GetSystemColour(int WXUNUSED(index))
{
    // FIXME_MGL
    return wxColour(0,0,0);
}

wxFont wxSystemSettings::GetSystemFont(int index)
{
    bool isDefaultRequested = (index == wxSYS_DEFAULT_GUI_FONT);

    if ( isDefaultRequested && gs_fontDefault )
    {
        return *gs_fontDefault;
    }

    // FIXME_MGL
    wxFont font(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, "Arial");

    if ( isDefaultRequested )
    {
        // if we got here it means we hadn't cached it yet - do now
        gs_fontDefault = new wxFont(font);
    }

    return font;
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
