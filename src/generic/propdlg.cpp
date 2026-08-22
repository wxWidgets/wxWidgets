/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/propdlg.cpp
// Purpose:     wxPropertySheetDialog
// Author:      Julian Smart
// Created:     2005-03-12
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_BOOKCTRL

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/sizer.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/msgdlg.h"
#endif

#include "wx/bookctrl.h"

#if wxUSE_NOTEBOOK
#include "wx/notebook.h"
#endif
#if wxUSE_CHOICEBOOK
#include "wx/choicebk.h"
#endif
#if wxUSE_TOOLBOOK
#include "wx/toolbook.h"
#endif
#if wxUSE_LISTBOOK
#include "wx/listbook.h"
#endif
#if wxUSE_TREEBOOK
#include "wx/treebook.h"
#endif

#include "wx/generic/propdlg.h"
#include "wx/private/dialogbuttonfactory.h"
#include "wx/private/windowlifetime.h"
#include "wx/scopeguard.h"
#include "wx/sysopt.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace
{

// wxPropertySheetDialog is an exported class whose historical layout contains
// only the raw m_bookCtrl pointer. Keep the weak identity used by the hardened
// implementation in a sidecar instead of changing that layout. New clients
// erase it from the out-of-line destructor below; clients compiled against an
// older inline destructor remain safe because every lookup also verifies the
// dialog weak identity and Init() replaces a stale entry at a reused address.
// ABI compatibility necessarily means that an old binary which inlined the
// former raw GetBookCtrl() itself cannot benefit from the new validation; all
// code rebuilt against this header and all library-internal paths do.
struct PropertySheetRuntimeState
{
    wxWeakRef<wxWindow> dialogLifetime;
    wxBookCtrlBase* bookAddress { nullptr };
    wxWeakRef<wxWindow> bookLifetime;
    wxSizer* containingSizer { nullptr };
};

using PropertySheetRuntimeStates =
    std::unordered_map<const wxPropertySheetDialog*,
                       PropertySheetRuntimeState>;

PropertySheetRuntimeStates& GetPropertySheetRuntimeStates()
{
    // Avoid static-destruction ordering with the objects tracked by wxWeakRef.
    static PropertySheetRuntimeStates* const states =
        new PropertySheetRuntimeStates;
    return *states;
}

void PruneExpiredPropertySheetRuntimeStates()
{
    PropertySheetRuntimeStates& states = GetPropertySheetRuntimeStates();
    for ( PropertySheetRuntimeStates::iterator it = states.begin();
          it != states.end(); )
    {
        if ( !it->second.dialogLifetime.get() )
            it = states.erase(it);
        else
            ++it;
    }
}

PropertySheetRuntimeState& GetPropertySheetRuntimeState(
    wxPropertySheetDialog* dialog)
{
    PruneExpiredPropertySheetRuntimeStates();
    PropertySheetRuntimeState& state =
        GetPropertySheetRuntimeStates()[dialog];
    if ( state.dialogLifetime.get() != dialog )
    {
        state = PropertySheetRuntimeState();
        state.dialogLifetime = wxWeakRef<wxWindow>(dialog);
    }
    return state;
}

const PropertySheetRuntimeState* FindPropertySheetRuntimeState(
    const wxPropertySheetDialog* dialog)
{
    PruneExpiredPropertySheetRuntimeStates();
    const PropertySheetRuntimeStates& states =
        GetPropertySheetRuntimeStates();
    const PropertySheetRuntimeStates::const_iterator it =
        states.find(dialog);
    if ( it == states.end() ||
         it->second.dialogLifetime.get() != dialog )
    {
        return nullptr;
    }

    return &it->second;
}

wxBookCtrlBase* FindLivePropertySheetBookChildByAddress(
    wxPropertySheetDialog* dialog,
    const wxBookCtrlBase* address)
{
    if ( !address || wxWindowIsUnavailableForCallbacks(dialog) )
        return nullptr;

    for ( wxWindowList::compatibility_iterator node =
              dialog->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow* const child = node->GetData();
        if ( static_cast<const wxObject*>(child) !=
             static_cast<const wxObject*>(address) )
        {
            continue;
        }

        wxBookCtrlBase* const book = wxDynamicCast(child, wxBookCtrlBase);
        return book && !wxWindowIsUnavailableForCallbacks(book) &&
                       book->GetParent() == dialog
                   ? book
                   : nullptr;
    }

    return nullptr;
}

void TrackPropertySheetBook(wxPropertySheetDialog* dialog,
                            wxBookCtrlBase* book,
                            wxSizer* containingSizer = nullptr)
{
    PropertySheetRuntimeState& state =
        GetPropertySheetRuntimeState(dialog);
    state.bookAddress = book;
    state.bookLifetime = wxWeakRef<wxWindow>(book);
    state.containingSizer = containingSizer;
}

wxSizer* FindLiveDirectChildSizerByAddress(wxSizer* owner,
                                           const wxSizer* address)
{
    if ( !owner || !address )
        return nullptr;

    for ( wxSizerItem* const item : owner->GetChildren() )
    {
        wxSizer* const child = item->GetSizer();
        if ( child == address )
            return child;
    }

    return nullptr;
}

size_t CountExactWindowItems(wxSizer* sizer, const wxWindow* window)
{
    size_t count = 0;
    if ( sizer && window )
    {
        for ( wxSizerItem* const item : sizer->GetChildren() )
        {
            if ( item->GetWindow() == window )
                ++count;
        }
    }
    return count;
}

#if wxUSE_BUTTON
bool UnlinkExactSizerItemWithoutVirtualDispatch(wxSizer* owner,
                                                wxSizerItem* itemAddress,
                                                wxSizer* child)
{
    if ( !owner || !itemAddress || !child )
        return false;

    wxSizerItemList& items = owner->GetChildren();
    for ( wxSizerItemList::compatibility_iterator node = items.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxSizerItem* const item = node->GetData();
        if ( item != itemAddress || item->GetSizer() != child )
            continue;

        // Unlink before destruction: even a throwing wxSizerItem-derived
        // destructor can no longer leave a dangling item in the live owner.
        item->DetachSizer();
        items.Erase(node);
        wxTRY
        {
            delete item;
        }
        wxCATCH_ALL(;)
        return true;
    }

    return false;
}
#endif // wxUSE_BUTTON

void DetachAllExactWindowItems(wxSizer* sizer, wxWindow* window)
{
    while ( CountExactWindowItems(sizer, window) != 0 )
    {
        if ( !sizer->Detach(window) )
            break;
    }
}

#if wxUSE_BUTTON

// wxSizer::Add(wxSizerItem*) transfers the item to the virtual insertion
// boundary. An application DoInsert() override is allowed to consume the item
// even when it returns nullptr or throws, and destroying an item which still
// contains a child sizer destroys that sizer too. Keep an identity independent
// of both raw addresses so rollback can distinguish this case from an override
// which detached the child before destroying the item.
struct ButtonSizerInsertionState
{
    bool itemIsAlive { true };
    bool itemOwnedSizerAtDestruction { false };
};

class TrackedButtonSizerItem final : public wxSizerItem
{
public:
    TrackedButtonSizerItem(
        wxSizer* sizer,
        const wxSizerFlags& flags,
        const std::shared_ptr<ButtonSizerInsertionState>& state)
        : wxSizerItem(sizer, flags),
          m_state(state)
    {
    }

    ~TrackedButtonSizerItem() override
    {
        // Record this before wxSizerItem::~wxSizerItem() destroys a still
        // attached child. Once this destructor has run, the transaction must
        // never infer the child's lifetime from its historical address.
        m_state->itemOwnedSizerAtDestruction = GetSizer() != nullptr;
        m_state->itemIsAlive = false;
    }

private:
    const std::shared_ptr<ButtonSizerInsertionState> m_state;
};

wxSizerItem* FindLiveExactSizerItem(wxSizer* owner,
                                    const wxSizerItem* itemAddress,
                                    const wxSizer* childAddress)
{
    if ( !owner || !itemAddress || !childAddress )
        return nullptr;

    for ( wxSizerItem* const item : owner->GetChildren() )
    {
        if ( item == itemAddress && item->GetSizer() == childAddress )
            return item;
    }

    return nullptr;
}

#endif // wxUSE_BUTTON

bool IsExactPropertySheetBook(
    const wxWeakRef<wxWindow>& dialogLifetime,
    wxPropertySheetDialog* dialog,
    const wxWeakRef<wxWindow>& bookLifetime,
    wxBookCtrlBase* book,
    wxSizer* expectedContainingSizer = nullptr,
    bool validateContainingSizer = false)
{
    if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) )
        return false;

    // Resolve through the sidecar first. Besides validating the identity this
    // clears the historical raw member if the book died or was reparented.
    wxBookCtrlBase* const currentBook = dialog->GetBookCtrl();
    return currentBook == book &&
           wxWeakWindowIsAvailableForCallbacks(bookLifetime, book) &&
           book->GetParent() == dialog &&
           (!validateContainingSizer ||
            book->GetContainingSizer() == expectedContainingSizer);
}

template <typename T>
wxBookCtrlBase* CreatePropertySheetBook(wxWindow* parent, long style)
{
    T* const book = new T;
    const wxWeakRef<wxWindow> weakBook(book);
    if ( !book->Create(parent,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       style) ||
         !wxWeakWindowIsAvailableForCallbacks(weakBook, book) )
    {
        if ( wxWeakWindowIsAvailableForCallbacks(weakBook, book) )
            delete book;
        return nullptr;
    }

    return book;
}

} // anonymous namespace

//-----------------------------------------------------------------------------
// wxPropertySheetDialog
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxPropertySheetDialog, wxDialog);

wxBEGIN_EVENT_TABLE(wxPropertySheetDialog, wxDialog)
    EVT_IDLE(wxPropertySheetDialog::OnIdle)
wxEND_EVENT_TABLE()

wxPropertySheetDialog::~wxPropertySheetDialog()
{
    GetPropertySheetRuntimeStates().erase(this);
    m_bookCtrl = nullptr;
}

void wxPropertySheetDialog::SetBookCtrl(wxBookCtrlBase* book)
{
    m_bookCtrl = book;
    TrackPropertySheetBook(this, book);
}

wxBookCtrlBase* wxPropertySheetDialog::GetBookCtrl() const
{
    wxPropertySheetDialog* const dialog =
        const_cast<wxPropertySheetDialog*>(this);
    if ( wxWindowIsUnavailableForCallbacks(dialog) )
        return nullptr;

    const PropertySheetRuntimeState* const state =
        FindPropertySheetRuntimeState(this);
    if ( state && state->bookAddress == m_bookCtrl )
    {
        wxWindow* const tracked = state->bookLifetime.get();
        if ( tracked == m_bookCtrl &&
             !wxWindowIsUnavailableForCallbacks(tracked) &&
             tracked->GetParent() == dialog )
        {
            return wxDynamicCast(tracked, wxBookCtrlBase);
        }

        // Reparent() doesn't necessarily detach a window from its former
        // sizer. If application code moved the book to another owner, remove
        // only the exact stale item recorded by this dialog; never touch a new
        // owner's sizer.
        wxBookCtrlBase* const liveTracked =
            tracked == m_bookCtrl &&
                    !wxWindowIsUnavailableForCallbacks(tracked)
                ? wxDynamicCast(tracked, wxBookCtrlBase)
                : nullptr;
        // Sizers don't have weak identities. Resolve the recorded address
        // through the dialog's current live sizer tree before invoking it;
        // m_innerSizer and the sidecar are used only for pointer comparison.
        // If application code replaced either sizer, leave its cleanup to its
        // owner instead of dereferencing a possibly expired raw address.
        wxSizer* const liveContainingSizer =
            state->containingSizer == dialog->m_innerSizer
                ? FindLiveDirectChildSizerByAddress(dialog->GetSizer(),
                                                     state->containingSizer)
                : nullptr;
        if ( liveTracked && liveTracked->GetParent() != dialog &&
             liveContainingSizer &&
             liveTracked->GetContainingSizer() == liveContainingSizer )
        {
            DetachAllExactWindowItems(liveContainingSizer, liveTracked);
        }

        // The weak identity expired, was scheduled for destruction, or was
        // reparented. Never expose the historical raw pointer after any of
        // these transitions.
        dialog->m_bookCtrl = nullptr;
        TrackPropertySheetBook(dialog, nullptr);
        return nullptr;
    }

    // A derived class, or an older binary using the former inline setter, may
    // have written m_bookCtrl without updating the sidecar. Resolve the raw
    // address exclusively through the live direct-child list before creating
    // a weak identity from it; this also prevents ABA resurrection.
    wxBookCtrlBase* const book =
        FindLivePropertySheetBookChildByAddress(dialog, m_bookCtrl);
    dialog->m_bookCtrl = book;
    TrackPropertySheetBook(dialog, book,
                           book ? book->GetContainingSizer() : nullptr);
    return book;
}

wxBookCtrlBase* wxPropertySheetDialog::GetBookCtrlForDestruction() const
{
    const PropertySheetRuntimeState* const state =
        FindPropertySheetRuntimeState(this);
    if ( !state || state->bookAddress != m_bookCtrl )
        return nullptr;

    // The weak identity is authoritative: never fall back to the historical
    // raw address here, as its storage may already have been reused. The
    // dialog is expected to be deleting, but the exact book must still be a
    // live direct child and not have begun an independent deletion itself.
    wxWindow* const tracked = state->bookLifetime.get();
    if ( tracked != m_bookCtrl ||
         wxWindowItselfIsUnavailableForCallbacks(tracked) ||
         tracked->GetParent() != this )
    {
        return nullptr;
    }

    for ( wxWindowList::compatibility_iterator node =
              GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        if ( node->GetData() == tracked )
            return wxDynamicCast(tracked, wxBookCtrlBase);
    }

    return nullptr;
}

bool wxPropertySheetDialog::Create(wxWindow* parent, wxWindowID id, const wxString& title,
                                       const wxPoint& pos, const wxSize& sz, long style,
                                       const wxString& name)
{
    const wxWeakRef<wxWindow> weakThis(this);
    parent = GetParentForModalDialog(parent, style);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return false;
    const wxWeakRef<wxWindow> weakParent(parent);

    if ( parent && !wxWeakWindowIsAvailableForCallbacks(weakParent, parent) )
        return false;

    const bool created = wxDialog::Create(
        parent, id, title, pos, sz, style | wxCLIP_CHILDREN, name);
    if ( !created || !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         GetParent() != parent ||
         (parent &&
          !wxWeakWindowIsAvailableForCallbacks(weakParent, parent)) )
        return false;

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    SetSizer(topSizer);

    // This gives more space around the edges
    m_innerSizer = new wxBoxSizer( wxVERTICAL );

    topSizer->Add(m_innerSizer, 1, wxGROW|wxALL, m_sheetOuterBorder);

    wxSizer* const innerSizer = m_innerSizer;
    const auto hasExactDialogLayout = [&]()
    {
        return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
               GetParent() == parent &&
               (!parent ||
                wxWeakWindowIsAvailableForCallbacks(weakParent, parent)) &&
               GetSizer() == topSizer &&
               m_innerSizer == innerSizer &&
               FindLiveDirectChildSizerByAddress(topSizer, innerSizer) ==
                   innerSizer;
    };

    wxBookCtrlBase* const bookCtrl = CreateBookCtrl();
    if ( !hasExactDialogLayout() )
        return false;

    if ( !bookCtrl || wxWindowIsUnavailableForCallbacks(bookCtrl) ||
         bookCtrl->GetParent() != this ||
         bookCtrl->GetContainingSizer() != nullptr ||
         CountExactWindowItems(innerSizer, bookCtrl) != 0 )
    {
        // A derived CreateBookCtrl() may deliberately transfer/adopt its
        // result while returning through application code. Reject it without
        // claiming or destroying the new owner's window.
        return false;
    }

    SetBookCtrl(bookCtrl);
    const wxWeakRef<wxWindow> weakBook(bookCtrl);
    bool bookCommitted = false;
    const wxScopeGuard rollbackBook = wxMakeGuard([&]()
    {
        if ( bookCommitted || !hasExactDialogLayout() )
            return;

        wxBookCtrlBase* const liveBook =
            wxDynamicCast(weakBook.get(), wxBookCtrlBase);
        if ( m_bookCtrl == bookCtrl )
            SetBookCtrl(nullptr);

        if ( liveBook != bookCtrl ||
             wxWindowIsUnavailableForCallbacks(liveBook) ||
             liveBook->GetParent() != this )
        {
            return;
        }

        wxSizer* const liveInner =
            FindLiveDirectChildSizerByAddress(topSizer, innerSizer);
        if ( liveInner && liveBook->GetContainingSizer() == liveInner )
            DetachAllExactWindowItems(liveInner, liveBook);

        // Delete only the still-autonomous book owned by this unchanged
        // transaction. A callback-adopted book belongs to its latest owner.
        if ( liveBook->GetParent() == this &&
             liveBook->GetContainingSizer() == nullptr )
        {
            delete liveBook;
        }
    });
    wxUnusedVar(rollbackBook);

    // Publish the expected sizer before the virtual call: an override may
    // call the base implementation, reparent the book and re-enter
    // GetBookCtrl() before returning. This lets the getter safely detach the
    // exact stale item in that window.
    TrackPropertySheetBook(this, bookCtrl, innerSizer);
    AddBookCtrl(innerSizer);
    const auto hasExactBookLayout = [&]()
    {
        return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
               topSizer && innerSizer &&
               GetSizer() == topSizer &&
               m_innerSizer == innerSizer &&
               FindLiveDirectChildSizerByAddress(topSizer, innerSizer) ==
                   innerSizer &&
               IsExactPropertySheetBook(weakThis, this, weakBook, bookCtrl,
                                         innerSizer, true) &&
               CountExactWindowItems(innerSizer, bookCtrl) == 1;
    };
    if ( !hasExactBookLayout() )
    {
        return false;
    }

    TrackPropertySheetBook(this, bookCtrl, innerSizer);

    const int selectedPage = bookCtrl->GetSelection();
    if ( !hasExactBookLayout() )
    {
        return false;
    }
    m_selectedPage = selectedPage;

    bookCommitted = true;
    return true;
}

void wxPropertySheetDialog::Init()
{
    GetPropertySheetRuntimeStates().erase(this);
    m_sheetStyle = wxPROPSHEET_DEFAULT;
    m_innerSizer = nullptr;
    m_bookCtrl = nullptr;
    m_sheetOuterBorder = 2;
    m_sheetInnerBorder = 5;
    m_selectedPage = wxNOT_FOUND;
}

// Layout the dialog, to be called after pages have been created
void wxPropertySheetDialog::LayoutDialog(int centreFlags)
{
    wxSizer* const sizer = GetSizer();
    if ( !sizer || wxWindowIsUnavailableForCallbacks(this) )
        return;

    const wxWeakRef<wxWindow> weakThis(this);
    sizer->SetSizeHints(this);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
         GetSizer() != sizer )
        return;

    if (centreFlags)
        Centre(centreFlags);
}

// Creates the buttons, if any
void wxPropertySheetDialog::CreateButtons(int flags)
{
#if !wxUSE_BUTTON
    // wxUSE_BOOKCTRL does not imply wxUSE_BUTTON. Keep property sheets usable
    // in this supported configuration; there is simply no concrete standard
    // button implementation to create.
    wxUnusedVar(flags);
    return;
#else
    if ( wxWindowIsUnavailableForCallbacks(this) || !m_innerSizer )
        return;

    const wxWeakRef<wxWindow> weakThis(this);
    wxWindow* const parent = GetParent();
    const wxWeakRef<wxWindow> weakParent(parent);
    wxSizer* const topSizer = GetSizer();
    wxSizer* const innerSizer = m_innerSizer;
    const auto hasExactOuterTopology = [&]()
    {
        return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
               GetParent() == parent &&
               (!parent ||
                wxWeakWindowIsAvailableForCallbacks(weakParent, parent)) &&
               GetSizer() == topSizer &&
               m_innerSizer == innerSizer &&
               FindLiveDirectChildSizerByAddress(topSizer, innerSizer) ==
                   innerSizer;
    };
    if ( !hasExactOuterTopology() )
        return;

    // FromDIP() reaches the virtual DPI path on some ports. Resolve it as a
    // separate callback boundary before creating any button or publishing any
    // dialog state.
    const int buttonBorder = FromDIP(2);
    if ( !hasExactOuterTopology() )
        return;

    const int originalAffirmativeId = GetAffirmativeId();
    wxWindow* const originalDefaultItem = m_winDefault.get();
    const wxWeakRef<wxWindow> originalDefaultLifetime(originalDefaultItem);
    wxWindow* const originalTmpDefaultItem = m_winTmpDefault.get();
    const wxWeakRef<wxWindow> originalTmpDefaultLifetime(
        originalTmpDefaultItem);
    wxWindow* const originalFocus = wxWindow::FindFocus();
    const wxWeakRef<wxWindow> originalFocusLifetime(originalFocus);

    const wxPrivate::DialogButtonWriterSnapshot writersBeforeFactory =
        wxPrivate::SnapshotDialogButtonWriters(this);
    wxSizer* const buttonSizer = CreateButtonSizer(flags);
    const wxPrivate::DialogButtonWriterSnapshot writersAfterFactory =
        wxPrivate::SnapshotDialogButtonWriters(this);
    if ( !buttonSizer )
        return;

    struct CreatedButton
    {
        wxButton* button;
        wxWeakRef<wxWindow> lifetime;
    };
    std::vector<CreatedButton> buttons;
    bool validButtonSizer = buttonSizer->GetContainingWindow() == nullptr;
    for ( wxSizerItem* const item : buttonSizer->GetChildren() )
    {
        wxWindow* const window = item->GetWindow();
        if ( !window )
            continue;

        const wxWeakRef<wxWindow> weakWindow(window);
        wxButton* const button =
            wxWeakWindowIsAvailableForCallbacks(weakWindow, window)
                ? wxDynamicCast(window, wxButton)
                : nullptr;
        if ( !button )
        {
            validButtonSizer = false;
            continue;
        }

        bool duplicate = false;
        for ( const CreatedButton& existing : buttons )
        {
            if ( existing.button == button )
            {
                duplicate = true;
                break;
            }
        }
        if ( duplicate )
        {
            validButtonSizer = false;
            continue;
        }

        buttons.push_back({button, weakWindow});
        if ( button->GetParent() != this ||
             button->GetContainingSizer() != buttonSizer )
        {
            validButtonSizer = false;
        }
    }

    bool committed = false;
    bool insertionBoundaryEntered = false;
    wxSizerItem* insertionItemAddress = nullptr;
    std::shared_ptr<ButtonSizerInsertionState> insertionState;
    const auto isCreatedButton = [&](const wxWindow* window)
    {
        for ( const CreatedButton& tracked : buttons )
        {
            if ( tracked.button == window &&
                 tracked.lifetime.get() == window )
            {
                return true;
            }
        }
        return false;
    };

    // State restoration is declared below this guard and therefore executes
    // first. If an application override throws while focus is being restored,
    // wxScopeGuard may swallow that exception but this independent guard still
    // removes every item, button and the returned sizer.
    const wxScopeGuard rollbackButtonStructure = wxMakeGuard([&]()
    {
        if ( committed )
            return;

        bool buttonSizerIsLiveAndOwned = !insertionBoundaryEntered;

        if ( insertionBoundaryEntered && insertionState )
        {
            // A destroyed insertion item is an authoritative ownership
            // receipt: DetachSizer() made the child live and autonomous,
            // whereas destruction with the child still attached consumed it.
            if ( !insertionState->itemIsAlive )
            {
                buttonSizerIsLiveAndOwned =
                    !insertionState->itemOwnedSizerAtDestruction;
            }
            else if ( hasExactOuterTopology() &&
                      FindLiveExactSizerItem(innerSizer,
                                             insertionItemAddress,
                                             buttonSizer) )
            {
                // Preserve the dynamic Detach() contract first. Its override
                // may consume the tracked item too, so consult the sentinel
                // again before attempting the qualified fallback.
                wxTRY
                {
                    (void)innerSizer->Detach(buttonSizer);
                }
                wxCATCH_ALL(;)

                if ( !insertionState->itemIsAlive )
                {
                    buttonSizerIsLiveAndOwned =
                        !insertionState->itemOwnedSizerAtDestruction;
                }
                else if ( hasExactOuterTopology() &&
                          FindLiveExactSizerItem(innerSizer,
                                                 insertionItemAddress,
                                                 buttonSizer) )
                {
                    // The dynamic override left the exact tracked item in the
                    // live owner. Bypass it for this transaction only.
                    wxTRY
                    {
                        (void)innerSizer->wxSizer::Detach(buttonSizer);
                    }
                    wxCATCH_ALL(;)

                    if ( insertionState->itemIsAlive &&
                         hasExactOuterTopology() &&
                         FindLiveExactSizerItem(innerSizer,
                                                insertionItemAddress,
                                                buttonSizer) )
                    {
                        // If even the qualified fallback threw, unlink the
                        // exact live item before destruction so the owner can
                        // never retain a pointer to freed transaction data.
                        UnlinkExactSizerItemWithoutVirtualDispatch(
                            innerSizer, insertionItemAddress, buttonSizer);
                    }

                    if ( !insertionState->itemIsAlive )
                    {
                        buttonSizerIsLiveAndOwned =
                            !insertionState->itemOwnedSizerAtDestruction;
                    }
                }
            }
        }

        if ( buttonSizerIsLiveAndOwned )
        {
            for ( const CreatedButton& tracked : buttons )
            {
                wxButton* const button =
                    wxDynamicCast(tracked.lifetime.get(), wxButton);
                if ( button == tracked.button &&
                     !wxWindowItselfIsUnavailableForCallbacks(button) &&
                     button->GetContainingSizer() == buttonSizer )
                {
                    DetachAllExactWindowItems(buttonSizer, button);
                }
            }
        }

        for ( std::vector<CreatedButton>::reverse_iterator it =
                  buttons.rbegin();
              it != buttons.rend();
              ++it )
        {
            wxButton* const button =
                wxDynamicCast(it->lifetime.get(), wxButton);
            if ( button == it->button &&
                 !wxWindowItselfIsUnavailableForCallbacks(button) &&
                 button->GetParent() == this &&
                 button->GetContainingSizer() == nullptr )
            {
                wxTRY
                {
                    delete button;
                }
                wxCATCH_ALL(;)
            }
        }

        if ( buttonSizerIsLiveAndOwned )
        {
            wxTRY
            {
                delete buttonSizer;
            }
            wxCATCH_ALL(;)
        }
    });
    wxUnusedVar(rollbackButtonStructure);

    const wxScopeGuard rollbackButtonState = wxMakeGuard([&]()
    {
        if ( committed )
            return;

        if ( wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        {
            if ( isCreatedButton(wxWindow::FindFocus()) &&
                 (!originalFocus ||
                  wxWeakWindowIsAvailableForCallbacks(
                      originalFocusLifetime, originalFocus)) )
            {
                if ( originalFocus )
                {
                    wxTRY
                    {
                        originalFocus->SetFocus();
                    }
                    wxCATCH_ALL(;)

                    if ( wxWeakWindowIsAvailableForCallbacks(
                             weakThis, this) &&
                         wxWindow::FindFocus() == originalFocus )
                    {
                        wxPrivate::RestoreDialogButtonWriter(
                            this,
                            wxPrivate::DialogButtonWriterField::Focus,
                            writersAfterFactory.focus,
                            writersBeforeFactory.focus);
                    }

                    if ( wxWeakWindowIsAvailableForCallbacks(
                             weakThis, this) &&
                         m_winTmpDefault.get() == originalTmpDefaultItem )
                    {
                        wxPrivate::RestoreDialogButtonWriter(
                            this,
                            wxPrivate::DialogButtonWriterField::
                                TemporaryDefault,
                            writersAfterFactory.temporaryDefault,
                            writersBeforeFactory.temporaryDefault);
                    }
                }
            }

            if ( wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                 isCreatedButton(m_winTmpDefault.get()) &&
                 (!originalTmpDefaultItem ||
                  wxWeakWindowIsAvailableForCallbacks(
                      originalTmpDefaultLifetime,
                      originalTmpDefaultItem)) )
            {
                wxTRY
                {
                    SetTmpDefaultItem(originalTmpDefaultItem);
                }
                wxCATCH_ALL(;)
                if ( wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                     m_winTmpDefault.get() == originalTmpDefaultItem )
                {
                    wxPrivate::RestoreDialogButtonWriter(
                        this,
                        wxPrivate::DialogButtonWriterField::TemporaryDefault,
                        writersAfterFactory.temporaryDefault,
                        writersBeforeFactory.temporaryDefault);
                }
            }

            if ( wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                 isCreatedButton(m_winDefault.get()) &&
                 (!originalDefaultItem ||
                  wxWeakWindowIsAvailableForCallbacks(
                      originalDefaultLifetime, originalDefaultItem)) )
            {
                wxTRY
                {
                    SetDefaultItem(originalDefaultItem);
                }
                wxCATCH_ALL(;)
                if ( wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                     m_winDefault.get() == originalDefaultItem )
                {
                    wxPrivate::RestoreDialogButtonWriter(
                        this,
                        wxPrivate::DialogButtonWriterField::PermanentDefault,
                        writersAfterFactory.permanentDefault,
                        writersBeforeFactory.permanentDefault);
                }
            }

            int publishedAffirmativeId = originalAffirmativeId;
            if ( flags & wxOK )
                publishedAffirmativeId = wxID_OK;
            else if ( flags & wxYES )
                publishedAffirmativeId = wxID_YES;
            else if ( flags & wxCLOSE )
                publishedAffirmativeId = wxID_CLOSE;
            if ( wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                 publishedAffirmativeId != originalAffirmativeId &&
                 GetAffirmativeId() == publishedAffirmativeId )
            {
                wxTRY
                {
                    SetAffirmativeId(originalAffirmativeId);
                }
                wxCATCH_ALL(;)
                if ( wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                     GetAffirmativeId() == originalAffirmativeId )
                {
                    wxPrivate::RestoreDialogButtonWriter(
                        this,
                        wxPrivate::DialogButtonWriterField::Affirmative,
                        writersAfterFactory.affirmative,
                        writersBeforeFactory.affirmative);
                }
            }
        }
    });
    wxUnusedVar(rollbackButtonState);

    if ( !validButtonSizer || !hasExactOuterTopology() ||
         buttonSizer->GetContainingWindow() != nullptr )
    {
        return;
    }

    insertionState = std::make_shared<ButtonSizerInsertionState>();
    insertionItemAddress = new TrackedButtonSizerItem(
        buttonSizer,
        wxSizerFlags().Expand().Border(wxALL, buttonBorder),
        insertionState);
    insertionBoundaryEntered = true;
    wxSizerItem* const buttonItem = innerSizer->Add(insertionItemAddress);

    // Never trust the virtual return value by itself: it may be nullptr, a
    // different item, or the stale address of an item consumed by DoInsert().
    // Publication succeeds only while the sentinel and the current live owner
    // jointly resolve the exact item identity.
    if ( !buttonItem || !insertionState->itemIsAlive ||
         !hasExactOuterTopology() ||
         FindLiveExactSizerItem(innerSizer,
                                insertionItemAddress,
                                buttonSizer) != buttonItem )
    {
        return;
    }

    innerSizer->AddSpacer(2);
    committed = true;
#endif // wxUSE_BUTTON
}

// Creates the book control
wxBookCtrlBase* wxPropertySheetDialog::CreateBookCtrl()
{
    const wxWeakRef<wxWindow> weakThis(this);
    int style = wxCLIP_CHILDREN | wxBK_DEFAULT;

    wxBookCtrlBase* bookCtrl = nullptr;
    const long requestedBookStyles = GetSheetStyle() &
        (wxPROPSHEET_NOTEBOOK |
         wxPROPSHEET_TOOLBOOK |
         wxPROPSHEET_CHOICEBOOK |
         wxPROPSHEET_LISTBOOK |
         wxPROPSHEET_BUTTONTOOLBOOK |
         wxPROPSHEET_TREEBOOK);
    long supportedBookStyles = 0;
#if wxUSE_NOTEBOOK
    supportedBookStyles |= wxPROPSHEET_NOTEBOOK;
#endif
#if wxUSE_TOOLBOOK
    supportedBookStyles |= wxPROPSHEET_TOOLBOOK |
                           wxPROPSHEET_BUTTONTOOLBOOK;
#endif
#if wxUSE_CHOICEBOOK
    supportedBookStyles |= wxPROPSHEET_CHOICEBOOK;
#endif
#if wxUSE_LISTBOOK
    supportedBookStyles |= wxPROPSHEET_LISTBOOK;
#endif
#if wxUSE_TREEBOOK
    supportedBookStyles |= wxPROPSHEET_TREEBOOK;
#endif
    if ( requestedBookStyles & ~supportedBookStyles )
        return nullptr;

    const bool specificBookRequested = requestedBookStyles != 0;

#if wxUSE_TREEBOOK
    if (GetSheetStyle() & wxPROPSHEET_TREEBOOK)
    {
        bookCtrl = CreatePropertySheetBook<wxTreebook>(this, style);
    }
    else
#endif
#if wxUSE_LISTBOOK
    if (GetSheetStyle() & wxPROPSHEET_LISTBOOK)
    {
        bookCtrl = CreatePropertySheetBook<wxListbook>(this, style);
    }
    else
#endif
#if wxUSE_TOOLBOOK
#if defined(__WXMAC__) && wxUSE_TOOLBAR && wxUSE_BMPBUTTON
    if (GetSheetStyle() & wxPROPSHEET_BUTTONTOOLBOOK)
    {
        bookCtrl = CreatePropertySheetBook<wxToolbook>(this,
                                                       style | wxTBK_BUTTONBAR);
    }
    else
#endif
    if ((GetSheetStyle() & wxPROPSHEET_TOOLBOOK) ||
        (GetSheetStyle() & wxPROPSHEET_BUTTONTOOLBOOK))
    {
        bookCtrl = CreatePropertySheetBook<wxToolbook>(this, style);
    }
    else
#endif
#if wxUSE_CHOICEBOOK
    if (GetSheetStyle() & wxPROPSHEET_CHOICEBOOK)
    {
        bookCtrl = CreatePropertySheetBook<wxChoicebook>(this, style);
    }
    else
#endif
#if wxUSE_NOTEBOOK
    if (GetSheetStyle() & wxPROPSHEET_NOTEBOOK)
    {
        bookCtrl = CreatePropertySheetBook<wxNotebook>(this, style);
    }
    else
#endif
    if (!bookCtrl && !specificBookRequested)
        bookCtrl = CreatePropertySheetBook<wxBookCtrl>(this, style);

    if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) )
        return nullptr;

    if ( bookCtrl && (GetSheetStyle() & wxPROPSHEET_SHRINKTOFIT) )
    {
        const wxWeakRef<wxWindow> weakBook(bookCtrl);
        bookCtrl->SetFitToCurrentPage(true);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             !wxWeakWindowIsAvailableForCallbacks(weakBook, bookCtrl) )
        {
            return nullptr;
        }
    }

    return bookCtrl;
}

// Adds the book control to the inner sizer.
void wxPropertySheetDialog::AddBookCtrl(wxSizer* sizer)
{
    wxCHECK_RET( sizer && m_bookCtrl,
                 "property sheet requires a book control and inner sizer" );
    wxCHECK_RET( m_bookCtrl->GetContainingSizer() == nullptr,
                 "property sheet book must be autonomous before adoption" );
    sizer->Add( m_bookCtrl, wxSizerFlags(1).Expand().Border(wxALL, m_sheetInnerBorder) );
}

// Resize dialog if necessary
void wxPropertySheetDialog::OnIdle(wxIdleEvent& event)
{
    event.Skip();

    if (GetSheetStyle() & wxPROPSHEET_SHRINKTOFIT)
    {
        wxBookCtrlBase* const bookCtrl = GetBookCtrl();
        if ( !bookCtrl )
            return;

        const wxWeakRef<wxWindow> weakThis(this);
        const wxWeakRef<wxWindow> weakBook(bookCtrl);
        wxSizer* const topSizer = GetSizer();
        wxSizer* const innerSizer = m_innerSizer;
        const auto hasExactBookLayout = [&]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                   topSizer && innerSizer &&
                   GetSizer() == topSizer &&
                   m_innerSizer == innerSizer &&
                   FindLiveDirectChildSizerByAddress(topSizer, innerSizer) ==
                       innerSizer &&
                   IsExactPropertySheetBook(weakThis, this,
                                             weakBook, bookCtrl,
                                             innerSizer, true);
        };
        if ( !hasExactBookLayout() )
            return;

        int sel = bookCtrl->GetSelection();
        if ( !hasExactBookLayout() )
        {
            return;
        }
        if (sel != -1 && sel != m_selectedPage)
        {
            m_selectedPage = sel;
            bookCtrl->InvalidateBestSize();
            if ( !hasExactBookLayout() )
            {
                return;
            }
            InvalidateBestSize();
            SetSizeHints(-1, -1, -1, -1);
            if ( !hasExactBookLayout() )
            {
                return;
            }
            LayoutDialog(0);
        }
    }
}

// Override function in base
wxWindow* wxPropertySheetDialog::GetContentWindow() const
{
    return GetBookCtrl();
}

#endif // wxUSE_BOOKCTRL
