/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/choice.h
// Purpose:     wxWinUI wxChoice declaration
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_CHOICE_H_
#define _WX_WINUI_CHOICE_H_

#include "wx/bitmap.h"
#include "wx/winui/private/itemmodel.h"

#include <memory>
#include <functional>

class wxWinUIChoiceImpl;

struct wxWinUIPopupRetirementCoreProbeForTesting
{
    bool detachedCorrelationRetired = false;
    bool reopenRejected = false;
    bool reopenDegraded = false;
    bool compositeReentrantCompletion = false;
    bool sealRemainsClosed = false;
    bool sealedAddRejectedWithoutPoison = false;
    bool sealedNativeOpenFailsClosed = false;
};

class WXDLLIMPEXP_CORE wxChoice : public wxChoiceBase
{
public:
    wxChoice();
    wxChoice(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             int n = 0,
             const wxString choices[] = nullptr,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxASCII_STR(wxChoiceNameStr));
    wxChoice(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos,
             const wxSize& size,
             const wxArrayString& choices,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxASCII_STR(wxChoiceNameStr));
    ~wxChoice() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0,
                const wxString choices[] = nullptr,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxChoiceNameStr));
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxChoiceNameStr));

    void SetLabel(const wxString& label) override;

    unsigned int GetCount() const override;
    int GetSelection() const override;
    int GetCurrentSelection() const override;
    void SetSelection(int n) override;
    int FindString(const wxString& s, bool bCase = false) const override;
    wxString GetString(unsigned int n) const override;
    void SetString(unsigned int n, const wxString& s) override;

    // Implementation-only seams used by deterministic WinUI tests.
    std::uint64_t WinUIGetItemIdForTesting(unsigned int n) const;
    std::uintptr_t WinUIGetItemPeerIdentityForTesting(unsigned int n) const;
    bool WinUIGetItemPeerBitmapStateForTesting(unsigned int n,
                                                wxSize *pixelSize,
                                                wxSize *dipSize) const;
    bool WinUISelectPeerItemForTesting(int selection);
    bool WinUISetDropDownForTesting(bool open);
    bool WinUIIsPeerDropDownOpenForTesting() const;
    bool WinUIGetPopupReopenSnapshotForTesting(
        bool *pending,
        std::uint64_t *generation,
        unsigned *schedules,
        unsigned *runs) const;
    static wxWinUIPopupRetirementCoreProbeForTesting
        WinUIProbePopupRetirementCoreForTesting();

protected:
    void DoDeleteOneItem(unsigned int n) override;
    void DoClear() override;
    int DoInsertItems(const wxArrayStringsAdapter& items,
                      unsigned int pos,
                      void **clientData,
                      wxClientDataType type) override;
    void DoSetItemClientData(unsigned int n, void* clientData) override;
    void* DoGetItemClientData(unsigned int n) const override;
    wxSize DoGetBestSize() const override;
    bool MSWShouldPreProcessMessage(WXMSG* msg) override;

    // Hook allowing derived classes (wxBitmapComboBox) to show a per-item
    // image in the dropdown; returns an invalid bitmap by default, in which
    // case the item is rendered as plain text.
    virtual wxBitmap WinUIGetItemBitmap(unsigned int n,
                                        double requestedScale = 0.0) const;

    // wxComboBox declares editable construction through this hook. The base
    // builds an editable peer but defers its host publication so the derived
    // text/template delegates own the first Loaded/x:Load transition.
    virtual bool WinUIWantsEditablePeerDuringCreate() const { return false; }

    // Refresh the existing peer contents without replacing item identities.
    void WinUIRefreshItems(double requestedBitmapScale = 0.0);

    virtual void WinUIOnItemInserted(unsigned int n);
    virtual void WinUIOnItemErased(unsigned int n);
    virtual void WinUIOnItemMoved(unsigned int oldIndex,
                                  unsigned int newIndex);
    virtual void WinUIOnItemsCleared();

    // These helpers can synchronously flush XAML layout. Return false when
    // application re-entry destroyed this control while doing so.
    bool ApplyItemsToPeer();
    bool ApplySelectionToPeer();
    void WinUIInsertPeerItem(unsigned int n);
    void WinUIErasePeerItem(unsigned int n);
    void WinUIMovePeerItem(unsigned int oldIndex, unsigned int newIndex);
    void WinUIUpdatePeerItem(unsigned int n,
                             double requestedBitmapScale = 0.0);
    bool WinUIEnsurePeerConsistent();
    void WinUIOnPeerSelectionChanged();
    void WinUIOnDropDownChanged(bool open);
    void WinUICancelPendingPeerSelection();
    void WinUICommitPeerSelection(wxWinUIItemModel::Id selectedId);
    bool WinUICoalescePopupReopen(
        std::function<void (wxChoice *)> replay) noexcept;

    // Fire the selection event for a user selection; wxComboBox overrides it
    // to send wxEVT_COMBOBOX (+ wxEVT_TEXT) instead of wxEVT_CHOICE.
    virtual void SendSelectionEvent();

    std::unique_ptr<wxWinUIChoiceImpl> m_winui;
    wxWinUIItemModel m_itemModel;
    wxWinUIItemModel::Id m_selectedItemId = 0;

    bool IsSorted() const override { return HasFlag(wxCB_SORT); }

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxChoice);
};

#endif // _WX_WINUI_CHOICE_H_
