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

#if wxUSE_BUTTON && wxUSE_STATLINE

// forward declared
class WXDLLEXPORT wxButton;
class WXDLLEXPORT wxStaticLine;

// class name
extern WXDLLIMPEXP_DATA_ADV(const wxChar) wxGenericCollapsiblePaneNameStr[];


// ----------------------------------------------------------------------------
// wxGenericCollapsiblePane
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxGenericCollapsiblePane : public wxCollapsiblePaneBase
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


    // public wxCollapsiblePane API
    virtual void Collapse(bool collapse = true);
    virtual void SetLabel(const wxString &label);

    virtual bool IsCollapsed() const
        { return m_pPane==NULL || !m_pPane->IsShown(); }
    virtual wxWindow *GetPane() const
        { return m_pPane; }
    virtual wxString GetLabel() const
        { return m_strLabel; }


    // implementation only, don't use
    void OnStateChange(const wxSize& sizeNew);

protected:
    // overridden methods
    virtual wxSize DoGetBestSize() const;

    // internal helpers
    void LayoutChildren();

    wxString GetBtnLabel() const;


    // child controls
    wxButton *m_pButton;
    wxStaticLine *m_pStatLine;
    wxWindow *m_pPane;

    // the button label without ">>" or "<<"
    wxString m_strLabel;

private:
    // event handlers
    void OnButton(wxCommandEvent &ev);
    void OnSize(wxSizeEvent &ev);

    DECLARE_DYNAMIC_CLASS(wxGenericCollapsiblePane)
    DECLARE_EVENT_TABLE()
};

#endif // wxUSE_BUTTON && wxUSE_STATLINE


#endif // _WX_COLLAPSABLE_PANE_H_GENERIC_
