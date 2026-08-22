/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/radiobox.h
// Purpose:     wxWinUI wxRadioBox declaration (WinUI RadioButtons)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_RADIOBOX_H_
#define _WX_WINUI_RADIOBOX_H_

#include "wx/arrstr.h"

#include <memory>
#include <vector>

class wxWinUIRadioBoxImpl;
struct wxWinUIAppearanceSnapshot;

using wxWinUIRadioBoxPeerWriteHookForTesting = void (*)(void *);

class WXDLLIMPEXP_CORE wxRadioBox : public wxControl,
                                    public wxRadioBoxBase
{
public:
    wxRadioBox();
    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0, const wxString choices[] = nullptr,
               int majorDim = 0,
               long style = wxRA_SPECIFY_COLS,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxRadioBoxNameStr));
    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               int majorDim = 0,
               long style = wxRA_SPECIFY_COLS,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxRadioBoxNameStr));
    ~wxRadioBox() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = nullptr,
                int majorDim = 0,
                long style = wxRA_SPECIFY_COLS,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxRadioBoxNameStr));
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                int majorDim = 0,
                long style = wxRA_SPECIFY_COLS,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxRadioBoxNameStr));

    // wxItemContainerImmutable
    unsigned int GetCount() const override;
    wxString GetString(unsigned int n) const override;
    void SetString(unsigned int n, const wxString& s) override;
    void SetSelection(int n) override;
    int GetSelection() const override;
    void SetLabel(const wxString& label) override;
    bool SetFont(const wxFont& font) override;
    bool SetForegroundColour(const wxColour& colour) override;
    bool SetBackgroundColour(const wxColour& colour) override;

    // wxRadioBoxBase
    bool Enable(unsigned int n, bool enable = true) override;
    bool Show(unsigned int n, bool show = true) override;
    bool IsItemEnabled(unsigned int n) const override;
    bool IsItemShown(unsigned int n) const override;

    // bring the wxWindow versions back into scope
    bool Enable(bool enable = true) override;
    bool Show(bool show = true) override;
    bool CanBeFocused() const override;
    void SetFocus() override;
    int GetItemFromPoint(const wxPoint& pt) const override;

#if wxUSE_TOOLTIPS
    bool HasToolTips() const override;
#endif // wxUSE_TOOLTIPS
#if wxUSE_HELP
    wxString GetHelpTextAtPoint(const wxPoint& pt,
                                wxHelpEvent::Origin origin) const override
    {
        return wxRadioBoxBase::DoGetHelpTextAtPoint(this, pt, origin);
    }
#endif // wxUSE_HELP

    bool WinUIGetAppearanceForTesting(
        wxWinUIAppearanceSnapshot *snapshot,
        bool *titleIsRaw = nullptr) const;
    bool WinUIGetPeerStateForTesting(
        wxArrayString *strings,
        int *selection,
        unsigned long long *generation = nullptr) const;
    bool WinUISelectItemForTesting(unsigned int item);
    // One-shot deterministic seam invoked immediately before SetContent().
    void WinUISetNextPeerWriteHookForTesting(
        wxWinUIRadioBoxPeerWriteHookForTesting hook,
        void *context);
    bool WinUIHasDeferredPeerWriteForTesting() const;
    bool WinUIIsPeerProjectionQuarantinedForTesting() const;
    unsigned long long WinUIGetModelRevisionForTesting() const;
    void WinUIGetCheckedHandlerCountsForTesting(
        unsigned long long *added,
        unsigned long long *revoked) const;

#if wxUSE_TOOLTIPS
    void DoSetToolTipText(const wxString& tip) override;
    void DoSetToolTip(wxToolTip *tip) override;
#endif // wxUSE_TOOLTIPS

protected:
    void DoEnable(bool enable) override;
    wxSize DoGetBestSize() const override;

    bool DoCreate(wxWindow *parent, wxWindowID id, const wxString& title,
                  const wxPoint& pos, const wxSize& size, int majorDim,
                  long style, const wxValidator& validator,
                  const wxString& name);
#if wxUSE_TOOLTIPS
    void DoSetItemToolTip(unsigned int item, wxToolTip *tooltip) override;
#endif // wxUSE_TOOLTIPS
    bool SyncItemState();
    bool RebuildItems();
    void SendSelectionEvent();
    int FindSelectedItem() const;

    std::unique_ptr<wxWinUIRadioBoxImpl> m_winui;
    wxArrayString m_strings;
    std::vector<bool> m_itemEnabled;
    std::vector<bool> m_itemShown;
    int m_selection = wxNOT_FOUND;
#if wxUSE_TOOLTIPS
    wxString m_tooltipText;
#endif // wxUSE_TOOLTIPS

private:
    unsigned long long BumpWinUIModelRevision();
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxRadioBox);
};

#endif // _WX_WINUI_RADIOBOX_H_
