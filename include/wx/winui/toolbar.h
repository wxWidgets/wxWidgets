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

#include <cstddef>
#include <memory>

class wxWinUIToolBarImpl;
class wxWinUIToolBarTool;
class wxDPIChangedEvent;
class wxToolBar;

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

    wxToolBarToolBase *RemoveTool(int toolid) override;
    bool DeleteToolByPos(size_t pos) override;
    bool DeleteTool(int toolid) override;
    void ClearTools() override;

    void SetToolShortHelp(int toolid,
                          const wxString& helpString) override;
    void SetToolLongHelp(int toolid,
                         const wxString& helpString) override;
    void SetToolNormalBitmap(int toolid,
                             const wxBitmapBundle& bitmap) override;
    void SetToolDisabledBitmap(int toolid,
                               const wxBitmapBundle& bitmap) override;
    void SetToolBitmapSize(const wxSize& size) override;
    void SetMargins(int x, int y) override;
    void SetToolPacking(int packing) override;
    void SetToolSeparation(int separation) override;
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
    void DoSetToolNormalBitmapByPos(
        size_t pos,
        const wxBitmapBundle& bitmap) override;

    bool DoInsertTool(size_t pos, wxToolBarToolBase *tool) override;
    bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool) override;

    void DoEnableTool(wxToolBarToolBase *tool, bool enable) override;
    void DoToggleTool(wxToolBarToolBase *tool, bool toggle) override;
    void DoSetToggle(wxToolBarToolBase *tool, bool toggle) override;
    void DoSetToolBitmapSize(const wxSize& size) override;
    void DoEnable(bool enable) override;

    wxSize DoGetBestSize() const override;

private:
    // wxToolBarToolBase::SetLabel() is virtual and wxToolbook calls it on the
    // exact controller item. Keep the retained label and the XAML generation
    // transactional instead of waiting for an unrelated later rebuild.
    void DoSetToolLabel(wxToolBarToolBase *tool,
                        const wxString& label);
    void SetToolNormalBitmapForTool(
        wxToolBarToolBase *tool,
        const wxBitmapBundle& bitmap);
    bool ApplyToolLabel(unsigned long long peerKey);

    // Build a detached candidate, provisionally publish it for synchronous
    // Loaded callbacks, then either commit it or restore the exact previous
    // generation. `excluded` lets the WinUI deletion overrides publish the
    // peer snapshot before atomically erasing the exact common-model node;
    // `inserted` is the converse required by DoInsertTool(), and `excludeAll`
    // is the single empty candidate used by all-or-nothing ClearTools().
    bool RebuildPeer(wxToolBarToolBase *excluded = nullptr,
                     size_t insertPos = static_cast<size_t>(-1),
                     wxToolBarToolBase *inserted = nullptr,
                     double requestedScale = 0.0,
                     bool excludeAll = false);

    void OnToolClicked(unsigned long long peerKey,
                       unsigned long long generation);
    void OnDropdownClicked(unsigned long long peerKey,
                           unsigned long long generation);
    void OnToolHovered(unsigned long long peerKey,
                       unsigned long long generation,
                       bool entered);
    void OnToolRightClicked(unsigned long long peerKey,
                            unsigned long long generation,
                            const wxPoint& point);
    void ConvergeToolShortHelpUpdates();
    void ConvergeToolEnabledUpdates();
    void SyncControlTools(unsigned long long generation);
    bool UpdateOverflow(double availableExtent,
                        unsigned long long generation);
    void OnDPIChanged(wxDPIChangedEvent& event);

    std::unique_ptr<wxWinUIToolBarImpl> m_winui;

    friend class wxWinUIToolBarTool;
    friend class wxWinUIToolBarTestAccess;

    wxDECLARE_DYNAMIC_CLASS(wxToolBar);
    wxDECLARE_NO_COPY_CLASS(wxToolBar);
};

#endif // wxUSE_TOOLBAR

#endif // _WX_WINUI_TOOLBAR_H_
