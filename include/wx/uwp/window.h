/////////////////////////////////////////////////////////////////////////////
// Name:        wx/uwp/window.h
// Purpose:     wxWindowUWP class
// Author:      Yann Clotioloman Yéo
// Modified by: 
// Created:     02/01/2022
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINDOW_H_
#define _WX_WINDOW_H_

#include "wx/settings.h"        // solely for wxSystemColour

class WXDLLIMPEXP_FWD_CORE wxButton;

// ---------------------------------------------------------------------------
// wxWindow declaration for UWP
// ---------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWindowUWP : public wxWindowBase
{
public:
    wxWindowUWP() { Init(); }

    wxWindowUWP(wxWindow* parent,
        wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxASCII_STR(wxPanelNameStr))
    {
        Init();
        Create(parent, id, pos, size, style, name);
    }

    virtual ~wxWindowUWP();

private:
    wxDECLARE_DYNAMIC_CLASS(wxWindowUWP);
    wxDECLARE_NO_COPY_CLASS(wxWindowUWP);
    wxDECLARE_EVENT_TABLE();
};
