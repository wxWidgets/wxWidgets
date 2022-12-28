/////////////////////////////////////////////////////////////////////////////
// Name:        wx/panel.h
// Purpose:     Base header for wxPanel
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
//              (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PANEL_H_BASE_
#define _WX_PANEL_H_BASE_

// ----------------------------------------------------------------------------
// headers and forward declarations
// ----------------------------------------------------------------------------

#include "wx/window.h"
#include "wx/containr.h"

extern WXDLLIMPEXP_DATA_CORE(const char) wxPanelNameStr[];

// ----------------------------------------------------------------------------
// wxPanel contains other controls and implements TAB traversal between them
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPanel : public wxNavigationEnabled<wxWindow>
{
public:
    wxPanel() { }

    wxPanel(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = wxASCII_STR(wxPanelNameStr))
    {
        Create(parent, winid, pos, size, style, name);
    }

    // Pseudo ctor
    bool Create(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = wxASCII_STR(wxPanelNameStr));


    // implementation from now on
    // --------------------------

    virtual void InitDialog() override;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxPanel);
};

#endif // _WX_PANELH_BASE_
