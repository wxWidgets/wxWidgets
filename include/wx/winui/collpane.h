/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/collpane.h
// Purpose:     wxCollapsiblePane shown by a WinUI Expander
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_COLLPANE_H_
#define _WX_WINUI_COLLPANE_H_

#include "wx/containr.h"

#include <memory>

class wxWinUICollapsiblePaneImpl;

// The header and the frame of the pane are those of an Expander. The pane
// itself stays a window of wx, a child of this one, placed over the content
// area of the Expander: what a program puts in it is unchanged.
class WXDLLIMPEXP_CORE wxCollapsiblePane :
    public wxNavigationEnabled<wxCollapsiblePaneBase>
{
public:
    wxCollapsiblePane();
    wxCollapsiblePane(wxWindow *parent,
                      wxWindowID winid,
                      const wxString& label,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxCP_DEFAULT_STYLE,
                      const wxValidator& val = wxDefaultValidator,
                      const wxString& name = wxASCII_STR(wxCollapsiblePaneNameStr));
    ~wxCollapsiblePane() override;

    bool Create(wxWindow *parent,
                wxWindowID winid,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCP_DEFAULT_STYLE,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxCollapsiblePaneNameStr));

    void Collapse(bool collapse = true) override;
    bool IsCollapsed() const override { return m_collapsed; }
    wxWindow *GetPane() const override { return m_pane; }
    wxString GetLabel() const override { return m_label; }
    void SetLabel(const wxString& label) override;

    bool Layout() override;

    // implementation only: the user expanded or collapsed the Expander
    void WinUIUserToggled(bool expanded);

protected:
    wxSize DoGetBestClientSize() const override;

private:
    void OnSize(wxSizeEvent& event);
    void OnStateChange(const wxSize& sizeNew);
    void UpdateHeaderMetrics() const;

    std::unique_ptr<wxWinUICollapsiblePaneImpl> m_winui;
    wxWindow *m_pane = nullptr;
    wxString m_label;
    bool m_collapsed = true;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxCollapsiblePane);
};

#endif // _WX_WINUI_COLLPANE_H_
