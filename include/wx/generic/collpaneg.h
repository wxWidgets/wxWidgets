/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/collpaneg.h
// Purpose:     wxGenericCollapsiblePane
// Author:      Francesco Montorsi
// Modified by:
// Created:     8/10/2006
// RCS-ID:      $Id$
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLLAPSABLE_PANE_H_GENERIC_
#define _WX_COLLAPSABLE_PANE_H_GENERIC_

#include "wx/button.h"


// the ID of the wxButton used to collapse/expand the panel
#define wxCP_BUTTON_ID      12356

// the number of pixels to leave between the button and the static line and
// between the button and the pane
#define wxCP_MARGIN         10

// forward declared
class WXDLLEXPORT wxStaticLine;

// class name
extern WXDLLEXPORT_DATA(const wxChar) wxGenericCollapsiblePaneNameStr[];



// ----------------------------------------------------------------------------
// wxGenericCollapsiblePane
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGenericCollapsiblePane : public wxCollapsiblePaneBase
{
public:
    wxGenericCollapsiblePane() { Init(); }

    wxGenericCollapsiblePane(wxWindow *parent,
                        wxWindowID winid,
                        const wxString& label,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                        const wxValidator& val = wxDefaultValidator,
                        const wxString& name = wxGenericCollapsiblePaneNameStr)
    {
        Init();

        Create(parent, winid, label, pos, size, style, val, name);
    }

    void Init()
    {
        m_pButton = NULL;
        m_pStatLine = NULL;
        m_pPane = NULL;
    }

    bool Create(wxWindow *parent,
                wxWindowID winid,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxGenericCollapsiblePaneNameStr);


public:     // public API

    void Collapse(bool collapse = true);
    void SetLabel(const wxString &label);

    bool IsCollapsed() const
        { return m_pPane==NULL || !m_pPane->IsShown(); }
    wxWindow *GetPane() const
        { return m_pPane; }
    wxString GetLabel() const
        { return m_strLabel; }

    wxWindow *GetTopLevelParent();

public:         // event handlers

    void OnButton(wxCommandEvent &ev);
    void OnSize(wxSizeEvent &ev);

protected:      // internal utils

    void LayoutChildren();

    wxString GetBtnLabel() const;
    virtual wxSize DoGetBestSize() const;

protected:

    wxButton *m_pButton;
    wxStaticLine *m_pStatLine;
    wxWindow *m_pPane;

    // the button label without ">>" or "<<"
    wxString m_strLabel;

private:
    DECLARE_DYNAMIC_CLASS(wxGenericCollapsiblePane)
    DECLARE_EVENT_TABLE()
};


#endif
    // _WX_COLLAPSABLE_PANE_H_GENERIC_
