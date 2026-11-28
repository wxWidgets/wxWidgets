/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/toolbar.h
// Purpose:     wxToolBar for wxWinUI, made of a WinUI CommandBar
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
class wxDPIChangedEvent;

// wx keeps what it has always had: the list of the tools, their state, the
// events. XAML does the rest with the control made for it: a CommandBar lays
// the tools out, shows their states and moves those which do not fit into
// its "..." menu.
//
// - a tool is an AppBarButton, a tool which can be checked an
//   AppBarToggleButton, a separator an AppBarSeparator;
// - a tool with a menu (wxITEM_DROPDOWN) is followed by a small button with
//   a chevron, which sends wxEVT_TOOL_DROPDOWN;
// - a control (AddControl) keeps its own window, laid over a place the bar
//   reserves for it;
// - labels (wxTB_TEXT) are beside the icons, as in the command bars of
//   Windows 11: a CommandBar only shows labels under the icons while it is
//   open;
// - the tools after the first stretchable space are at the end of the bar;
// - a vertical bar (wxTB_LEFT, wxTB_RIGHT) is a column of the same buttons:
//   a CommandBar is horizontal only;
// - there are no rows (SetRows): what does not fit is in the "..." menu.
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

    void SetWindowStyleFlag(long style) override;

    void SetToolShortHelp(int toolid,
                          const wxString& helpString) override;
    void SetToolNormalBitmap(int toolid,
                             const wxBitmapBundle& bitmap) override;
    void SetToolDisabledBitmap(int toolid,
                               const wxBitmapBundle& bitmap) override;
    void SetToolBitmapSize(const wxSize& size) override;
    wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const override;

    bool CanApplyThemeBorder() const override { return false; }

protected:
    bool MSWOnEffectiveLayoutDirectionChanged() override;

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

    // wxToolbook names the tool of a page by its position: two pages may
    // have the same identifier.
    void DoSetToolNormalBitmapByPos(size_t pos,
                                    const wxBitmapBundle& bitmap) override;

    bool DoInsertTool(size_t pos, wxToolBarToolBase *tool) override;
    bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool) override;

    void DoEnableTool(wxToolBarToolBase *tool, bool enable) override;
    void DoToggleTool(wxToolBarToolBase *tool, bool toggle) override;
    void DoSetToggle(wxToolBarToolBase *tool, bool toggle) override;
    void DoEnable(bool enable) override;

    wxSize DoGetBestSize() const override;

private:
    friend class wxWinUIToolBarImpl;
    friend class wxToolBarTool;
    friend class wxWinUIToolBarTestAccess;

    // Makes the XAML elements again from the list of the tools: now, or
    // once the code which is changing the list has finished.
    void Rebuild();
    void ScheduleRebuild();

    // What the user did.
    void WinUIOnClick(wxToolBarToolBase *tool, bool checked);
    void WinUIOnDropdown(wxToolBarToolBase *tool);

    // The label of a tool changed (wxToolBarToolBase::SetLabel()).
    void WinUIUpdateLabel(wxToolBarToolBase *tool);

    void WinUISetNormalBitmap(wxToolBarToolBase *tool,
                              const wxBitmapBundle& bitmap);

    // Puts the windows of the control tools over the places kept for them.
    void WinUIPlaceControls();

    void OnDPIChanged(wxDPIChangedEvent& event);

    std::unique_ptr<wxWinUIToolBarImpl> m_winui;

    wxDECLARE_DYNAMIC_CLASS(wxToolBar);
    wxDECLARE_NO_COPY_CLASS(wxToolBar);
};

// ----------------------------------------------------------------------------
// wxToolBarTool: public in all ports, so that a tool can be created outside
// of wxToolBar::CreateTool() and given to AddTool() or InsertTool().
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(wxToolBar *tbar,
                  int id,
                  const wxString& label,
                  const wxBitmapBundle& bmpNormal,
                  const wxBitmapBundle& bmpDisabled,
                  wxItemKind kind,
                  wxObject *clientData,
                  const wxString& shortHelp,
                  const wxString& longHelp);

    wxToolBarTool(wxToolBar *tbar, wxControl *control, const wxString& label);

    virtual ~wxToolBarTool();

    // wxToolbook changes the label of the tool of a page this way.
    virtual void SetLabel(const wxString& label) override;

private:
    wxDECLARE_NO_COPY_CLASS(wxToolBarTool);
    wxDECLARE_CLASS(wxToolBarTool);
};

#endif // wxUSE_TOOLBAR

#endif // _WX_WINUI_TOOLBAR_H_
