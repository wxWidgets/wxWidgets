/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/collpane.h
// Purpose:     wxCollapsiblePane
// Author:      Francesco Montorsi
// Modified by:
// Created:     8/10/2006
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLLAPSABLE_PANEL_H_GTK_
#define _WX_COLLAPSABLE_PANEL_H_GTK_

// ----------------------------------------------------------------------------
// wxCollapsiblePane
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxCollapsiblePane : public wxCollapsiblePaneBase
{
public:
    wxCollapsiblePane() { Init(); }

    wxCollapsiblePane(wxWindow *parent,
                        wxWindowID winid,
                        const wxString& label,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxCP_DEFAULT_STYLE,
                        const wxValidator& val = wxDefaultValidator,
                        const wxString& name = wxASCII_STR(wxCollapsiblePaneNameStr))
    {
        Init();

        Create(parent, winid, label, pos, size, style, val, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID winid,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCP_DEFAULT_STYLE,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxCollapsiblePaneNameStr));

    virtual void Collapse(bool collapse = true) override;
    virtual bool IsCollapsed() const override;
    virtual void SetLabel(const wxString& str) override;

    virtual wxWindow *GetPane() const override { return m_pPane; }
    virtual wxString GetLabel() const override { return m_strLabel; }

protected:
    virtual wxSize DoGetBestSize() const override;

public:     // used by GTK callbacks
    bool m_bIgnoreNextChange;
    wxSize m_szCollapsed;

    wxWindow *m_pPane;

    // the button label without ">>" or "<<"
    wxString m_strLabel;

private:
    void Init()
    {
        m_bIgnoreNextChange = false;
    }

    void OnSize(wxSizeEvent&);
    virtual void AddChildGTK(wxWindowGTK* child) override;
    GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const override;

    wxDECLARE_DYNAMIC_CLASS(wxCollapsiblePane);
    wxDECLARE_EVENT_TABLE();
};

#endif // _WX_COLLAPSABLE_PANEL_H_GTK_
