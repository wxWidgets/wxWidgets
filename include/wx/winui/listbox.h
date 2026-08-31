/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/listbox.h
// Purpose:     wxWinUI wxListBox declaration
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_LISTBOX_H_
#define _WX_WINUI_LISTBOX_H_

#include "wx/winui/private/itemmodel.h"

#include <cstdint>
#include <memory>

class wxWinUIListBoxImpl;

#if wxUSE_OWNER_DRAWN
class WXDLLIMPEXP_FWD_CORE wxOwnerDrawn;

#include "wx/dynarray.h"

WX_DEFINE_EXPORTED_ARRAY_PTR(wxOwnerDrawn *, wxListBoxItemsArray);
#endif // wxUSE_OWNER_DRAWN

class WXDLLIMPEXP_CORE wxListBox : public wxListBoxBase
{
public:
    wxListBox();
    wxListBox(wxWindow *parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              int n = 0, const wxString choices[] = nullptr,
              long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxASCII_STR(wxListBoxNameStr));
    wxListBox(wxWindow *parent, wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              const wxArrayString& choices,
              long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxASCII_STR(wxListBoxNameStr));

    ~wxListBox() override;

    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = nullptr,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxListBoxNameStr));
    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxListBoxNameStr));

    unsigned int GetCount() const override;
    wxString GetString(unsigned int n) const override;
    void SetString(unsigned int n, const wxString& s) override;
    int FindString(const wxString& s, bool bCase = false) const override;

    bool IsSelected(int n) const override;
    int GetSelection() const override;
    int GetSelections(wxArrayInt& aSelections) const override;

    int GetTopItem() const override;
    int GetCountPerPage() const override;
    void EnsureVisible(int n) override;

#if wxUSE_OWNER_DRAWN
    // Keep the Windows owner-drawn ListBox contract available when the HWND
    // peer is replaced by a WinUI ListView. The drawing callback is projected
    // to the XAML item content by the implementation.
    bool SetFont(const wxFont& font) override;
    virtual wxOwnerDrawn *CreateLboxItem(size_t n);
    wxOwnerDrawn *GetItem(size_t n) const { return m_aItems[n]; }
    int GetItemIndex(wxOwnerDrawn *item) const
        { return m_aItems.Index(item); }
    bool GetItemRect(size_t n, wxRect& rect) const;
    bool RefreshItem(size_t n);
#endif // wxUSE_OWNER_DRAWN

    // Windows-compatible horizontal list semantics. Tab stops use quarters
    // of the current average character width, exactly like LB_SETTABSTOPS.
    virtual void SetHorizontalExtent(
        const wxString& s = wxEmptyString);
    virtual bool MSWSetTabStops(const wxVector<int>& tabStops);


protected:
    void DoSetFirstItem(int n) override;
    void DoSetSelection(int n, bool select) override;
    int DoListHitTest(const wxPoint& point) const override;

    void DoClear() override;
    void DoDeleteOneItem(unsigned int n) override;
    int DoInsertItems(const wxArrayStringsAdapter& items,
                      unsigned int pos,
                      void **clientData,
                      wxClientDataType type) override;
    void DoSetItemClientData(unsigned int n, void* clientData) override;
    void* DoGetItemClientData(unsigned int n) const override;

    wxSize DoGetBestSize() const override;
    void DoSetSize(int x, int y, int width, int height,
                   int sizeFlags = wxSIZE_AUTO) override;
    void DoSetClientSize(int width, int height) override;
    void DoEnable(bool enable) override;
    bool MSWOnEffectiveLayoutDirectionChanged() override;

    // Hooks used by wxCheckListBox. The callback receives a stable item ID,
    // never a mutable index, because sorting or deletion can happen while a
    // queued XAML notification is pending.
    virtual bool WinUIIsCheckable() const { return false; }
    virtual void WinUIOnItemToggled(wxWinUIItemModel::Id id, bool check);
    void WinUISendDoubleClick(wxWinUIItemModel::Id id);

    // Refresh existing peer contents without replacing their identities.
    void WinUIRefreshItems();

    void Init();

    void ApplyItemsToPeer();
    void ApplySelectionToPeer();
    void WinUIInsertPeerItem(unsigned int n);
    void WinUIErasePeerItem(unsigned int n, wxWinUIItemModel::Id id);
    void WinUIMovePeerItem(unsigned int oldIndex, unsigned int newIndex);
    void WinUIUpdatePeerItem(unsigned int n);
    bool WinUIUpdatePeerItemById(wxWinUIItemModel::Id id,
                                 bool ensureConsistent = true);
    void WinUIEnsurePeerConsistent();
    void WinUISyncOldSelections();
    int WinUIGetHorizontalExtentPixels() const;
    bool WinUIEnsureHorizontalPresentation();
    bool WinUISynchronizeHorizontalPresentation();
    bool WinUIResetHorizontalExtent();
    void WinUIApplyScrollPolicy();
    void WinUIApplyControlSizeToPeer();
#if wxUSE_OWNER_DRAWN
    void WinUIApplyOwnerDrawFont();
#endif
    void OnDPIChanged(wxDPIChangedEvent& event);
    void SendSelectionEvent();

    std::unique_ptr<wxWinUIListBoxImpl> m_winui;
    wxWinUIItemModel m_itemModel;
    wxWinUIItemModel::Id m_selectedItemId = 0;
    bool m_updatingPeer = false;
    int m_horizontalExtent = 0;

#if wxUSE_OWNER_DRAWN
    wxListBoxItemsArray m_aItems;
#endif

private:
    friend class wxWinUIListBoxTestAccess;

    bool WinUIHandleCheckKey(int virtualKey,
                             bool shiftDown,
                             std::uintptr_t keyboardLayout,
                             bool useLiveKeyboardState);

    enum class WinUITabStopsState
    {
        NeverSet,
        ExplicitDefault,
        ExplicitStops
    };

    struct WinUITabLayout
    {
        wxVector<wxString> runs;
        wxVector<int> runOffsets;
        int width = 0;
    };

    WinUITabLayout WinUIBuildTabLayout(const wxString& text) const;
    int WinUIMeasureTextWidth(const wxString& text) const;

    WinUITabStopsState m_tabStopsState = WinUITabStopsState::NeverSet;
    wxVector<int> m_tabStops;
    double m_controlWidthDIPs = 0.0;
    double m_controlHeightDIPs = 0.0;
    double m_ownerDrawProjectionScale = 0.0;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxListBox);
};

#endif // _WX_WINUI_LISTBOX_H_
