/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/collpane.h
// Purpose:     wxCollapsiblePane
// Author:      Francesco Montorsi
// Modified by:
// Created:     8/10/2006
// RCS-ID:      $Id$
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLLAPSABLE_PANEL_H_GTK_
#define _WX_COLLAPSABLE_PANEL_H_GTK_

#include "wx/generic/collpaneg.h"

extern WXDLLIMPEXP_DATA_CORE(const wxChar) wxCollapsiblePaneNameStr[];

// ----------------------------------------------------------------------------
// wxCollapsiblePane
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxCollapsiblePane : public wxGenericCollapsiblePane
{
public:
    wxCollapsiblePane() { Init(); }

    wxCollapsiblePane(wxWindow *parent,
                        wxWindowID winid,
                        const wxString& label,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                        const wxValidator& val = wxDefaultValidator,
                        const wxString& name = wxCollapsiblePaneNameStr)
    {
        Init();

        Create(parent, winid, label, pos, size, style, val, name);
    }

    void Init()
    {
        m_bIgnoreNextChange = false;
    }

    bool Create(wxWindow *parent,
                wxWindowID winid,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxCollapsiblePaneNameStr);

public:     // public API

    wxSize DoGetBestSize() const;
    void Collapse(bool collapse = true);
    bool IsCollapsed() const;
    void SetLabel(const wxString &str);

    void OnSize(wxSizeEvent&);

public:     // used by GTK callbacks

    bool m_bIgnoreNextChange;
    wxSize m_szCollapsed;

private:
    DECLARE_DYNAMIC_CLASS(wxCollapsiblePane)
    DECLARE_EVENT_TABLE()
};


#endif
    // _WX_COLLAPSABLE_PANEL_H_GTK_
