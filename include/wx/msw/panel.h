///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/panel.h
// Purpose:     wxMSW-specific wxPanel class.
// Author:      Vadim Zeitlin
// Created:     2011-03-18
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PANEL_H_
#define _WX_MSW_PANEL_H_

class WXDLLIMPEXP_FWD_CORE wxBrush;

// ----------------------------------------------------------------------------
// wxPanel
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPanel : public wxPanelBase
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

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxPanel);
};

#endif // _WX_MSW_PANEL_H_
