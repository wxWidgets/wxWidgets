/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/panel.h
// Purpose:     wxMoPanel class
// Author:      Julian Smart
// Modified by:
// Created:     2009-06-07
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_PANEL_H_
#define _WX_MOBILE_GENERIC_PANEL_H_

#include "wx/mobile/window.h"
#include "wx/containr.h"

class WXDLLIMPEXP_FWD_CORE wxControlContainer;

extern WXDLLEXPORT_DATA(const wxChar) wxPanelNameStr[];

/**
    @class wxMoPanel

    A class to contain other controls.

    @category{wxMobile}
*/

class WXDLLEXPORT wxMoPanel : public wxMoWindow
{
public:
    /// Default constructor.
    wxMoPanel() { Init(); }

    /// Constructor.
    wxMoPanel(wxWindow *parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxPanelNameStr)
    {
        Init();

        Create(parent, winid, pos, size, style, name);
    }

    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = wxPanelNameStr);

    virtual ~wxMoPanel();

    // calls layout for layout constraints and sizers
    void OnSize(wxSizeEvent& event);

    virtual void InitDialog();

#ifdef __WXUNIVERSAL__
    virtual bool IsCanvasWindow() const { return true; }
#endif

    WX_DECLARE_CONTROL_CONTAINER();

protected:
    // common part of all ctors
    void Init();

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoPanel)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_MOBILE_GENERIC_PANEL_H_

