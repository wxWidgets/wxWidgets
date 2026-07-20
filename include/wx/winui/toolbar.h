/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/toolbar.h
// Purpose:     wxToolBar for wxWinUI (WinUI CommandBar)
// Author:      wxWidgets development team
// Created:     2026-07-20
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_TOOLBAR_H_
#define _WX_WINUI_TOOLBAR_H_

#if wxUSE_TOOLBAR

#include <memory>

class wxWinUIToolBarImpl;

class WXDLLIMPEXP_CORE wxToolBar : public wxToolBarBase
{
public:
    wxToolBar();
    wxToolBar(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxTB_DEFAULT_STYLE,
              const wxString& name = wxASCII_STR(wxToolBarNameStr));

    ~wxToolBar() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxToolBarNameStr));

    bool Realize() override;

    wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const override;

    bool CanApplyThemeBorder() const override { return false; }

protected:
    wxToolBarToolBase *CreateTool(int toolid,
                                  const wxString& label,
                                  const wxBitmapBundle& bmpNormal,
                                  const wxBitmapBundle& bmpDisabled,
                                  wxItemKind kind,
                                  wxObject *clientData,
                                  const wxString& shortHelp,
                                  const wxString& longHelp) override;
    wxToolBarToolBase *CreateTool(wxControl *control,
                                  const wxString& label) override;

    bool DoInsertTool(size_t pos, wxToolBarToolBase *tool) override;
    bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool) override;

    void DoEnableTool(wxToolBarToolBase *tool, bool enable) override;
    void DoToggleTool(wxToolBarToolBase *tool, bool toggle) override;
    void DoSetToggle(wxToolBarToolBase *tool, bool toggle) override;

    wxSize DoGetBestSize() const override;

private:
    // Recreate the CommandBar contents from m_tools.
    void RebuildPeer();

    void OnToolClicked(int toolid);

    std::unique_ptr<wxWinUIToolBarImpl> m_winui;

    wxDECLARE_DYNAMIC_CLASS(wxToolBar);
    wxDECLARE_NO_COPY_CLASS(wxToolBar);
};

#endif // wxUSE_TOOLBAR

#endif // _WX_WINUI_TOOLBAR_H_
