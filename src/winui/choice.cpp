/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/choice.cpp
// Purpose:     wxWinUI wxChoice implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CHOICE

#include "wx/choice.h"

#ifdef WXWINUI_TEST_SUPPORT
#include "choice-test-access.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/validate.h"
#endif

#include "choiceimpl.h"
#include "wx/weakref.h"
#include "keyboard.h"

#include <cmath>
#include <utility>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

namespace
{

template <typename T>
winrt::Windows::Foundation::IUnknown
wxWinUIChoiceObjectIdentity(const T& object) noexcept
{
    return object
        ? object.template try_as<winrt::Windows::Foundation::IUnknown>()
        : nullptr;
}

class wxWinUIPeerMutationGuard final
{
public:
    explicit wxWinUIPeerMutationGuard(
        const std::shared_ptr<wxWinUIChoiceCallbackState>& state)
        : m_state(state)
    {
        if ( m_state )
            m_state->BeginPeerMutation();
    }

    ~wxWinUIPeerMutationGuard()
    {
        if ( m_state )
            m_state->EndPeerMutation();
    }

private:
    std::shared_ptr<wxWinUIChoiceCallbackState> m_state;
};

bool wxWinUIChoiceHasModifiers(WXMSG *msg)
{
    wxWinUIKeyboardModifiers modifiers;
    if ( wxWinUIGetKeyboardModifiersOverride(&modifiers) )
    {
        return modifiers.shiftDown || modifiers.controlDown ||
               modifiers.leftAltDown || modifiers.rightAltDown;
    }

    return wxIsShiftDown() || wxIsCtrlDown() ||
           (msg && (HIWORD(msg->lParam) & KF_ALTDOWN));
}

bool wxWinUIChoiceHasItemPeer(const wxWinUIChoiceImpl *impl)
{
    return impl && (impl->simpleListBox || impl->comboBox);
}

winrt::Microsoft::UI::Xaml::Controls::ItemCollection
wxWinUIChoiceGetPeerItems(const wxWinUIChoiceImpl *impl)
{
    if ( impl->simpleListBox )
        return impl->simpleListBox.Items();

    return impl->comboBox.Items();
}

int wxWinUIChoiceGetPeerSelection(const wxWinUIChoiceImpl *impl)
{
    return impl->simpleListBox ? impl->simpleListBox.SelectedIndex()
                               : impl->comboBox.SelectedIndex();
}

void wxWinUIChoiceSetPeerSelection(wxWinUIChoiceImpl *impl, int selection)
{
    if ( impl->simpleListBox )
        impl->simpleListBox.SelectedIndex(selection);
    else
        impl->comboBox.SelectedIndex(selection);
}

winrt::Windows::Foundation::IInspectable
wxWinUICreateChoicePeerItem(const wxWinUIChoiceImpl *impl)
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
    if ( impl->simpleListBox )
        return MUXC::ListBoxItem();

    return MUXC::ComboBoxItem();
}

} // anonymous namespace

namespace
{
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
}

wxWinUIChoiceImpl::~wxWinUIChoiceImpl()
{
    Close();
}

void wxWinUIChoiceImpl::Close()
{
    // Invalidate before the first revocation attempt. Revocation can throw,
    // and detaching a focused editable ComboBox can synchronously (or on the
    // dispatcher) commit pending text. Every delegate owns only this shared
    // state, so either path observes a null owner instead of a dying control.
    if ( callbackState )
        callbackState->Invalidate();

    // Keep the exact popup/focus observation live while requesting closure.
    // The ComboBox can raise DropDownClosed inline; that delegate records the
    // logical close even though its wx owner was invalidated above.
    if ( popupRetirementState )
        wxWinUIRequestComboPopupClose(popupRetirementState);

    if ( comboBox )
    {
        if ( textSubmittedToken.value )
        {
            try
            {
                comboBox.TextSubmitted(textSubmittedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException("WinUI ComboBox TextSubmitted removal", e);
            }
        }
        textSubmittedToken = {};

        if ( textChangedCallbackToken )
        {
            try
            {
                comboBox.UnregisterPropertyChangedCallback(
                    winrt::Microsoft::UI::Xaml::Controls::ComboBox::
                        TextProperty(),
                    textChangedCallbackToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI ComboBox text callback removal", e);
            }
        }
        textChangedCallbackToken = 0;

        if ( selectionChangedToken.value )
        {
            try
            {
                comboBox.SelectionChanged(selectionChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI ComboBox SelectionChanged removal", e);
            }
        }
        selectionChangedToken = {};

        if ( choiceKeyDownToken.value )
        {
            try
            {
                comboBox.KeyDown(choiceKeyDownToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI Choice KeyDown removal", e);
            }
        }
        choiceKeyDownToken = {};

        if ( choiceDropDownOpenedToken.value )
        {
            try
            {
                comboBox.DropDownOpened(choiceDropDownOpenedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI Choice DropDownOpened removal", e);
            }
        }
        choiceDropDownOpenedToken = {};

        if ( choiceDropDownClosedToken.value )
        {
            try
            {
                comboBox.DropDownClosed(choiceDropDownClosedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI Choice DropDownClosed removal", e);
            }
        }
        choiceDropDownClosedToken = {};
    }

    if ( simpleListBox && simpleSelectionChangedToken.value )
    {
        try
        {
            simpleListBox.SelectionChanged(simpleSelectionChangedToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI simple ComboBox SelectionChanged removal", e);
        }
    }
    simpleSelectionChangedToken = {};

    // Unregistering the slot invalidates its shared callback state before the
    // XAML element is detached. Only then release our final peer references.
    // WinUI can dispatch its popup-child LostFocus after this Close() returns.
    // Only an actually open/closing popup carries a gate: a never-opened or
    // already causally retired ComboBox disconnects synchronously.
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate> retirementGate =
        physicalDisconnectGate && !physicalDisconnectGate->IsSatisfied()
            ? physicalDisconnectGate
            : nullptr;
    host.Close(retirementGate);
    editBox = nullptr;
    simpleListBox = nullptr;
    simpleRoot = nullptr;
    editableRoot = nullptr;
    comboBox = nullptr;
    popupRetirementState.reset();
    physicalDisconnectGate.reset();
    pendingItemId = 0;
    hasPendingSelection = false;
    choiceDropDownOpen = false;
    callbackState.reset();
}

wxChoice::wxChoice()
{
}

wxChoice::wxChoice(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   int n,
                   const wxString choices[],
                   long style,
                   const wxValidator& validator,
                   const wxString& name)
    : wxChoice()
{
    Create(parent, id, pos, size, n, choices, style, validator, name);
}

wxChoice::wxChoice(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   const wxArrayString& choices,
                   long style,
                   const wxValidator& validator,
                   const wxString& name)
    : wxChoice()
{
    Create(parent, id, pos, size, choices, style, validator, name);
}

wxChoice::~wxChoice()
{
    // wxComboBox's derived members (notably m_value) have already been
    // destroyed when this base destructor starts. A delayed TextSubmitted
    // callback must therefore be disconnected before Clear() mutates the
    // peer, and the impl must be destroyed before m_items is torn down.
    if ( m_winui )
    {
        m_winui->Close();
        m_winui.reset();
    }

    Clear();
}

bool wxChoice::MSWShouldPreProcessMessage(WXMSG *msg)
{
    if ( msg && msg->message == WM_KEYDOWN &&
            !wxWinUIChoiceHasModifiers(msg) )
    {
        switch ( msg->wParam )
        {
            case VK_F4:
                // F4 belongs to the XAML ComboBox even while it is closed.
                // wxCB_SIMPLE has no popup and leaves F4 to ordinary dialog
                // preprocessing, matching the native CBS_SIMPLE control.
                if ( !HasFlag(wxCB_SIMPLE) || HasFlag(wxCB_READONLY) )
                    return false;
                break;

            case VK_ESCAPE:
            case VK_RETURN:
                // Escape/Enter belong to an open dropdown, not to a parent
                // accelerator or dialog default button.
                try
                {
                    if ( m_winui && m_winui->comboBox &&
                            m_winui->comboBox.IsDropDownOpen() )
                    {
                        return false;
                    }
                }
                catch ( const winrt::hresult_error& )
                {
                    // During teardown, fall through to normal preprocessing:
                    // there is no usable peer left to own this key.
                }
                break;
        }
    }

    return wxControl::MSWShouldPreProcessMessage(msg);
}

bool wxChoice::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      int n,
                      const wxString choices[],
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    wxASSERT_MSG( !(style & wxCB_DROPDOWN) &&
                  !(style & wxCB_READONLY) &&
                  !(style & wxCB_SIMPLE),
                  wxT("this style flag is ignored by wxChoice") );

    // The WinUI ComboBox draws its own border, so suppress the native control
    // border to avoid an extra grey frame around the island.
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_winui.reset(new wxWinUIChoiceImpl);
    m_winui->callbackState =
        std::make_shared<wxWinUIChoiceCallbackState>(this);
    wxWinUIChoiceImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> callbackState =
        createImpl->callbackState;
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->comboBox = winrt::Microsoft::UI::Xaml::Controls::ComboBox();
        m_winui->popupRetirementState =
            wxWinUICreateComboPopupRetirementState(
                m_winui->comboBox);
        m_winui->physicalDisconnectGate =
            wxWinUICreateCompositePhysicalDisconnectGate();
        if ( !m_winui->popupRetirementState ||
             !m_winui->physicalDisconnectGate ||
             !wxWinUIAddCompositePhysicalDisconnectGate(
                 m_winui->physicalDisconnectGate,
                 wxWinUIGetComboPopupRetirementGate(
                     m_winui->popupRetirementState),
                 true) )
        {
            return false;
        }
        m_winui->host.SetPhysicalDisconnectGate(
            m_winui->physicalDisconnectGate);
        // An editable derived ComboBox must install its text/template
        // delegates before the one and only host publication. Publishing it
        // here would consume the initial Loaded/template transition in the
        // wxChoice half of construction; a later same-content SetContent()
        // deliberately doesn't touch the carrier and cannot replay it.
        const bool deferEditablePeerPublication =
            WinUIWantsEditablePeerDuringCreate();
        m_winui->comboBox.IsEditable(deferEditablePeerPublication);
        m_winui->comboBox.MaxDropDownHeight(240);
        // wxChoice::GetCurrentSelection() must expose the highlighted item
        // while the popup is open, before GetSelection() is committed.  The
        // default Committed trigger doesn't publish keyboard navigation.
        m_winui->comboBox.SelectionChangedTrigger(
            winrt::Microsoft::UI::Xaml::Controls::
                ComboBoxSelectionChangedTrigger::Always);

        m_winui->selectionChangedToken = m_winui->comboBox.SelectionChanged(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Microsoft::UI::Xaml::Controls::
                    SelectionChangedEventArgs const&)
            {
                wxChoice * const owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState ||
                     callbackState->IsPeerMutationInProgress() )
                {
                    return;
                }

                owner->WinUIOnPeerSelectionChanged();
            });

        m_winui->choiceKeyDownToken = m_winui->comboBox.KeyDown(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const&
                    args)
            {
                if ( args.Handled() ||
                     args.Key() !=
                         winrt::Windows::System::VirtualKey::Escape )
                {
                    return;
                }

                wxChoice * const owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState ||
                     !owner->m_winui->choiceDropDownOpen )
                {
                    return;
                }

                // Leave Escape unhandled so the real ComboBox closes its
                // popup, but discard its transient highlight first.  The
                // ensuing DropDownClosed callback then has nothing to commit.
                owner->WinUICancelPendingPeerSelection();
            });

        const std::shared_ptr<wxWinUIComboPopupRetirementState>
            popupRetirementState = m_winui->popupRetirementState;
        m_winui->choiceDropDownOpenedToken =
            m_winui->comboBox.DropDownOpened(
                [callbackState, popupRetirementState](
                    winrt::Windows::Foundation::IInspectable const&,
                    winrt::Windows::Foundation::IInspectable const&)
                {
                    // WinUI raises this event before moving focus into the
                    // popup. Arm exact-popup and FocusManager observation
                    // before any wx callback can run or destroy the owner.
                    if ( !wxWinUIObserveComboPopupOpened(popupRetirementState) )
                        return;
                    wxChoice * const owner = callbackState->GetOwner();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState )
                    {
                        return;
                    }
                    owner->WinUIOnDropDownChanged(true);
                });

        m_winui->choiceDropDownClosedToken =
            m_winui->comboBox.DropDownClosed(
                [callbackState, popupRetirementState](
                    winrt::Windows::Foundation::IInspectable const&,
                    winrt::Windows::Foundation::IInspectable const&)
                {
                    wxWinUIObserveComboPopupClosed(popupRetirementState);
                    wxChoice * const owner = callbackState->GetOwner();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState )
                    {
                        return;
                    }
                    owner->WinUIOnDropDownChanged(false);
                });

        const bool contentSet = deferEditablePeerPublication ||
            createImpl->host.SetContent(createImpl->comboBox);
        wxChoice * const owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }
        if ( !contentSet )
        {
            owner->m_winui.reset();
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox creation", e);
        wxChoice * const owner = callbackState->GetOwner();
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == createImpl )
        {
            owner->m_winui.reset();
        }
        return false;
    }

    wxChoice *owner = callbackState->GetOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != createImpl )
    {
        return false;
    }

    if ( n && choices )
    {
        owner->Append(n, choices);
        owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }
    }

    owner->SetInitialSize(size);
    owner = callbackState->GetOwner();
    return owner && owner->m_winui &&
           owner->m_winui.get() == createImpl;
}

bool wxChoice::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      const wxArrayString& choices,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    wxCArrayString chs(choices);
    return Create(parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
                  style, validator, name);
}

void wxChoice::SetLabel(const wxString& label)
{
    const int sel = FindString(label);
    const wxWeakRef<wxWindow> self(this);
    SetSelection(sel);
    if ( self )
        wxControl::SetLabel(label);
}

unsigned int wxChoice::GetCount() const
{
    return static_cast<unsigned int>(m_itemModel.GetCount());
}

int wxChoice::GetSelection() const
{
    if ( !m_selectedItemId )
        return wxNOT_FOUND;

    const std::size_t index = m_itemModel.IndexOf(m_selectedItemId);
    return index == wxWinUIItemModel::npos
               ? wxNOT_FOUND
               : static_cast<int>(index);
}

int wxChoice::GetCurrentSelection() const
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) )
        return GetSelection();

    // The selection delegate translates the native index to a stable item ID
    // synchronously. Keep that transaction authoritative instead of reading
    // ComboBox.SelectedIndex again: an editable ComboBox can transiently
    // clear SelectedIndex while projecting its TextBox document even though
    // both the accepted model and the popup highlight remain valid.
    if ( !m_winui->choiceDropDownOpen ||
         !m_winui->hasPendingSelection )
    {
        return GetSelection();
    }

    if ( !m_winui->pendingItemId )
        return wxNOT_FOUND;

    const std::size_t index =
        m_itemModel.IndexOf(m_winui->pendingItemId);
    return index == wxWinUIItemModel::npos
               ? GetSelection()
               : static_cast<int>(index);
}

void wxChoice::WinUIOnPeerSelectionChanged()
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) ||
         !m_winui->callbackState ||
         m_winui->callbackState->IsPeerMutationInProgress() )
    {
        return;
    }

    int selection = wxNOT_FOUND;
    try
    {
        selection = wxWinUIChoiceGetPeerSelection(m_winui.get());
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI item peer selection read", e);
        return;
    }
    wxWinUIItemModel::Id selectedId = 0;
    if ( selection >= 0 )
    {
        const std::size_t peerIndex =
            static_cast<std::size_t>(selection);
        if ( !m_winui->peerItemsValid ||
             peerIndex >= m_winui->peerItemIds.size() )
        {
            return;
        }

        selectedId = m_winui->peerItemIds[peerIndex];
        if ( m_itemModel.IndexOf(selectedId) ==
                wxWinUIItemModel::npos )
        {
            return;
        }
    }

    // A simple list has no transient popup state: keyboard and pointer
    // selection are accepted immediately. Normal Choice/ComboBox peers retain
    // the highlighted-vs-accepted split while their dropdown is open.
    if ( !m_winui->simpleListBox && m_winui->choiceDropDownOpen )
    {
        m_winui->pendingItemId = selectedId;
        m_winui->hasPendingSelection = true;
        return;
    }

    WinUICommitPeerSelection(selectedId);
}

void wxChoice::WinUICommitPeerSelection(
    wxWinUIItemModel::Id selectedId)
{
    if ( selectedId &&
         m_itemModel.IndexOf(selectedId) == wxWinUIItemModel::npos )
    {
        return;
    }

    const bool changed = m_selectedItemId != selectedId;
    m_selectedItemId = selectedId;
    for ( std::size_t n = 0; n < m_itemModel.GetCount(); ++n )
    {
        m_itemModel.At(n).selected =
            selectedId && m_itemModel.At(n).id == selectedId;
    }
    if ( changed )
        SendSelectionEvent();
}

void wxChoice::WinUIOnDropDownChanged(bool open)
{
    if ( !m_winui || m_winui->choiceDropDownOpen == open )
        return;

    m_winui->choiceDropDownOpen = open;
    if ( open )
    {
        m_winui->pendingItemId = 0;
        m_winui->hasPendingSelection = false;
        return;
    }

    const bool commit = m_winui->hasPendingSelection;
    const wxWinUIItemModel::Id pending = m_winui->pendingItemId;
    m_winui->pendingItemId = 0;
    m_winui->hasPendingSelection = false;
    if ( commit )
        WinUICommitPeerSelection(pending);
}

bool wxChoice::WinUICoalescePopupReopen(
    std::function<void (wxChoice *)> replay) noexcept
{
    if ( !m_winui || !m_winui->popupRetirementState )
        return false;

    // The first degraded gate closes publication process-wide. Existing
    // controls must reject public Popup() before setting the peer property;
    // DropDownOpened remains the fail-close backstop for a native/user open.
    if ( wxWinUIIsPhysicalDisconnectPublicationPoisoned() )
        return true;

    // Shutdown sealing is a permanent publication barrier. Refuse a public
    // reopen before touching IsDropDownOpen, without poisoning a peer which
    // never actually published a second popup graph.
    if ( wxWinUIIsComboPopupRetirementSealed(
             m_winui->popupRetirementState) )
    {
        return true;
    }

    // Repeating Popup() while this generation is still logically open is an
    // idempotent public operation, not a request to replay after retirement.
    // Dismiss() clears this bit before delivering CLOSEUP, so a Popup() from
    // that synchronous handler still enters the coalesced-reopen path below.
    if ( m_winui->choiceDropDownOpen )
        return true;

    if ( wxWinUIIsComboPopupRetirementSatisfied(
             m_winui->popupRetirementState) )
    {
        return false;
    }

    if ( m_winui->popupReopenPending )
        return true;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const auto state = impl->popupRetirementState;
    const std::weak_ptr<wxWinUIComboPopupRetirementState> weakState = state;
    const std::shared_ptr<wxWinUIChoiceCallbackState> callbackState =
        impl->callbackState;
    if ( ++impl->popupReopenGeneration == 0 )
        ++impl->popupReopenGeneration;
    const std::uint64_t generation = impl->popupReopenGeneration;
    impl->popupReopenPending = true;
#ifdef WXWINUI_TEST_SUPPORT
    ++impl->popupReopenSchedulesForTesting;
#endif

    try
    {
        if ( !wxWinUIArmComboPopupRetirementCompletion(
                 state,
                 [weakState, callbackState, generation,
                  replay = std::move(replay)]() mutable
                 {
                     const auto exactState = weakState.lock();
                     wxChoice * const owner = callbackState->GetOwner();
                     if ( !exactState || !owner || !owner->m_winui ||
                          owner->m_winui->callbackState != callbackState ||
                          owner->m_winui->popupRetirementState !=
                              exactState ||
                          !owner->m_winui->popupReopenPending ||
                          owner->m_winui->popupReopenGeneration !=
                              generation )
                     {
                         return;
                     }

                     owner->m_winui->popupReopenPending = false;
#ifdef WXWINUI_TEST_SUPPORT
                     ++owner->m_winui->popupReopenRunsForTesting;
#endif
                     replay(owner);
                 }) )
        {
            impl->popupReopenPending = false;
        }
    }
    catch ( ... )
    {
        impl->popupReopenPending = false;
        wxWinUINotePhysicalDisconnectGateDegraded();
    }
    return true;
}

void wxChoice::WinUICancelPendingPeerSelection()
{
    if ( !m_winui || !m_winui->comboBox ||
         !m_winui->choiceDropDownOpen )
    {
        return;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> state =
        impl->callbackState;
    const int accepted = GetSelection();
    impl->pendingItemId = 0;
    impl->hasPendingSelection = false;

    wxWinUIPeerMutationGuard guard(state);
    try
    {
        impl->comboBox.SelectedIndex(accepted);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI Choice cancelled selection restore", e);
    }
}

void wxChoice::SetSelection(int n)
{
    if ( n < 0 || static_cast<std::size_t>(n) >= m_itemModel.GetCount() )
        n = wxNOT_FOUND;

    m_selectedItemId =
        n == wxNOT_FOUND ? 0 : m_itemModel.At(n).id;
    for ( std::size_t i = 0; i < m_itemModel.GetCount(); ++i )
        m_itemModel.At(i).selected = static_cast<int>(i) == n;

    // A direct selection is authoritative even while the dropdown is open.
    if ( m_winui )
    {
        m_winui->pendingItemId = 0;
        m_winui->hasPendingSelection = false;
    }

    if ( !ApplySelectionToPeer() )
        return;
    InvalidateBestSize();
}

int wxChoice::FindString(const wxString& s, bool bCase) const
{
    for ( std::size_t i = 0; i < m_itemModel.GetCount(); ++i )
    {
        if ( m_itemModel.At(i).text.IsSameAs(s, bCase) )
            return static_cast<int>(i);
    }

    return wxNOT_FOUND;
}

wxString wxChoice::GetString(unsigned int n) const
{
    wxCHECK_MSG( n < m_itemModel.GetCount(), wxString(),
                 wxT("invalid choice index") );
    return m_itemModel.At(n).text;
}

void wxChoice::SetString(unsigned int n, const wxString& s)
{
    wxCHECK_RET( n < m_itemModel.GetCount(), wxT("invalid choice index") );

    if ( !WinUIEnsurePeerConsistent() )
        return;
    const auto change = m_itemModel.Rename(n, s, IsSorted());
    if ( change.kind == wxWinUIItemModel::Change::Kind::Move )
    {
        WinUIOnItemMoved(
            static_cast<unsigned int>(change.oldIndex),
            static_cast<unsigned int>(change.newIndex));
        WinUIMovePeerItem(
            static_cast<unsigned int>(change.oldIndex),
            static_cast<unsigned int>(change.newIndex));
        WinUIUpdatePeerItem(static_cast<unsigned int>(change.newIndex));
    }
    else
    {
        WinUIUpdatePeerItem(n);
    }

    if ( !ApplySelectionToPeer() )
        return;
    InvalidateBestSize();
}

void wxChoice::DoDeleteOneItem(unsigned int n)
{
    wxCHECK_RET( n < m_itemModel.GetCount(), wxT("invalid choice index") );

    if ( !WinUIEnsurePeerConsistent() )
        return;
    const wxWinUIItemModel::Id erasedId = m_itemModel.At(n).id;
    if ( m_winui && m_winui->hasPendingSelection &&
         m_winui->pendingItemId == erasedId )
    {
        m_winui->pendingItemId = 0;
        m_winui->hasPendingSelection = false;
    }
    WinUIOnItemErased(n);
    m_itemModel.Erase(n);
    if ( m_selectedItemId == erasedId )
        m_selectedItemId = 0;
    WinUIErasePeerItem(n);

    if ( !ApplySelectionToPeer() )
        return;
    InvalidateBestSize();
}

void wxChoice::DoClear()
{
    WinUIOnItemsCleared();
    m_itemModel.Clear();
    m_selectedItemId = 0;
    if ( m_winui )
    {
        m_winui->pendingItemId = 0;
        m_winui->hasPendingSelection = false;
    }

    if ( wxWinUIChoiceHasItemPeer(m_winui.get()) )
    {
        wxWinUIPeerMutationGuard guard(m_winui->callbackState);
        m_winui->peerItemsValid = false;
        try
        {
            // Drop the selection first: removing the item a ComboBox or a
            // ListBox is currently showing makes the control reselect during
            // the mutation, and that reselection fails with E_INVALIDARG once
            // the collection no longer holds the index it computed. Clearing
            // the whole collection at once then has nothing left to fight
            // with -- and this is a clear, so the peer selection is going away
            // in any case.
            if ( wxWinUIChoiceGetPeerSelection(m_winui.get()) != wxNOT_FOUND )
                wxWinUIChoiceSetPeerSelection(m_winui.get(), wxNOT_FOUND);

            const auto items = wxWinUIChoiceGetPeerItems(m_winui.get());
            items.Clear();
            m_winui->peerItemIds.clear();
            m_winui->peerItemsValid = true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox item clear", e);
            m_winui->peerItemIds.clear();
            m_winui->peerItemsValid = false;
        }
    }

    InvalidateBestSize();
}

int wxChoice::DoInsertItems(const wxArrayStringsAdapter& items,
                            unsigned int pos,
                            void **clientData,
                            wxClientDataType type)
{
    wxCHECK_MSG( pos <= m_itemModel.GetCount(), wxNOT_FOUND,
                 wxT("invalid choice index") );

    int last = wxNOT_FOUND;
    for ( unsigned int i = 0; i < items.GetCount(); ++i )
    {
        if ( !WinUIEnsurePeerConsistent() )
            return wxNOT_FOUND;
        const auto change = m_itemModel.Insert(
            items[i],
            IsSorted() ? m_itemModel.GetCount() : pos + i,
            IsSorted());
        const unsigned int insertPos =
            static_cast<unsigned int>(change.newIndex);

        AssignNewItemClientData(insertPos, clientData, i, type);
        WinUIOnItemInserted(insertPos);
        WinUIInsertPeerItem(insertPos);
        last = static_cast<int>(change.newIndex);
    }

    if ( !ApplySelectionToPeer() )
        return wxNOT_FOUND;
    InvalidateBestSize();
    return last;
}

void wxChoice::DoSetItemClientData(unsigned int n, void* clientData)
{
    wxCHECK_RET( n < m_itemModel.GetCount(), wxT("invalid choice index") );
    m_itemModel.At(n).clientData = clientData;
}

void* wxChoice::DoGetItemClientData(unsigned int n) const
{
    wxCHECK_MSG( n < m_itemModel.GetCount(), nullptr,
                 wxT("invalid choice index") );
    return m_itemModel.At(n).clientData;
}

wxSize wxChoice::DoGetBestSize() const
{
    wxCoord width = 0;
    wxCoord height = GetCharHeight();
    for ( std::size_t n = 0; n < m_itemModel.GetCount(); ++n )
    {
        wxCoord itemWidth = 0;
        wxCoord itemHeight = 0;
        GetTextExtent(m_itemModel.At(n).text, &itemWidth, &itemHeight);
        width = wxMax(width, itemWidth);
        height = wxMax(height, itemHeight);
    }

    // The stock ComboBox template: 12 DIPs before the text, a 38 DIP glyph
    // column after it and a 1 DIP border on each side.
    const wxSize chrome = FromDIP(wxSize(54, 14));
    const wxSize minimum = FromDIP(wxSize(80, 32));
    return wxSize(wxMax(width + chrome.x, minimum.x),
                  wxMax(height + chrome.y, minimum.y));
}

wxBitmap wxChoice::WinUIGetItemBitmap(
    unsigned int WXUNUSED(n),
    double WXUNUSED(requestedScale)) const
{
    return wxBitmap();
}

void wxChoice::WinUIRefreshItems(double requestedBitmapScale)
{
    if ( !WinUIEnsurePeerConsistent() )
        return;
    for ( unsigned int n = 0; n < GetCount(); ++n )
        WinUIUpdatePeerItem(n, requestedBitmapScale);
}

void wxChoice::WinUIOnItemInserted(unsigned int WXUNUSED(n))
{
}

void wxChoice::WinUIOnItemErased(unsigned int WXUNUSED(n))
{
}

void wxChoice::WinUIOnItemMoved(unsigned int WXUNUSED(oldIndex),
                                unsigned int WXUNUSED(newIndex))
{
}

void wxChoice::WinUIOnItemsCleared()
{
}

bool wxChoice::WinUIEnsurePeerConsistent()
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) )
        return true;

    bool valid = m_winui->peerItemsValid;
    try
    {
        const std::size_t count = m_itemModel.GetCount();
        valid = valid &&
                wxWinUIChoiceGetPeerItems(m_winui.get()).Size() == count &&
                m_winui->peerItemIds.size() == count;
        for ( std::size_t n = 0; valid && n < count; ++n )
        {
            valid = m_winui->peerItemIds[n] ==
                    m_itemModel.At(n).id;
        }
    }
    catch ( const winrt::hresult_error& )
    {
        valid = false;
    }

    if ( !valid )
        return ApplyItemsToPeer();

    return true;
}

bool wxChoice::ApplyItemsToPeer()
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) )
        return true;

    wxWinUIPeerMutationGuard guard(m_winui->callbackState);
    m_winui->peerItemsValid = false;
    try
    {
        auto items = wxWinUIChoiceGetPeerItems(m_winui.get());
        while ( items.Size() )
            items.RemoveAtEnd();
        m_winui->peerItemIds.clear();

        for ( unsigned int n = 0; n < GetCount(); ++n )
        {
            items.Append(wxWinUICreateChoicePeerItem(m_winui.get()));
            m_winui->peerItemIds.push_back(m_itemModel.At(n).id);
        }

        m_winui->peerItemsValid =
            items.Size() == m_itemModel.GetCount() &&
            m_winui->peerItemIds.size() ==
                m_itemModel.GetCount();
        if ( m_winui->peerItemsValid )
        {
            for ( unsigned int n = 0; n < GetCount(); ++n )
            {
                WinUIUpdatePeerItem(n);
                if ( !m_winui->peerItemsValid )
                    break;
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox item update", e);
        m_winui->peerItemIds.clear();
        m_winui->peerItemsValid = false;
    }

    return ApplySelectionToPeer();
}

bool wxChoice::ApplySelectionToPeer()
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) )
        return true;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> state =
        impl->callbackState;

    if ( !impl->peerItemsValid )
        return true;

    int peerSelection = GetSelection();
    if ( impl->choiceDropDownOpen && impl->hasPendingSelection )
    {
        if ( impl->pendingItemId )
        {
            const std::size_t pendingIndex =
                m_itemModel.IndexOf(impl->pendingItemId);
            peerSelection = pendingIndex == wxWinUIItemModel::npos
                ? wxNOT_FOUND
                : static_cast<int>(pendingIndex);
        }
        else
        {
            peerSelection = wxNOT_FOUND;
        }
    }

    wxWinUIPeerMutationGuard guard(state);
    try
    {
        // While wxComboBox is replacing the construction-time ComboBox with
        // its wxCB_SIMPLE composite, the new item peer is deliberately built
        // off-tree. Rendering the old hosted peer here could re-enter wx and
        // destroy the control before the transaction publishes simpleRoot.
        wxWinUIChoiceSetPeerSelection(impl, peerSelection);

        // SelectedIndex is a re-entrant XAML boundary. It can start an
        // editable-template transition (or destroy this control), so no
        // precomputed render decision and no unvalidated impl access may cross
        // it. Re-evaluate the exact live generation before each further host
        // boundary and never force layout while a natural template replay owns
        // the subtree.
        wxChoice *live = state ? state->GetOwner() : nullptr;
        if ( live != this || !live->m_winui ||
             live->m_winui.get() != impl ||
             live->m_winui->callbackState != state )
        {
            return false;
        }

        bool shouldRender = !impl->comboTemplateTransition &&
                            !impl->comboTemplateReplayPending;
        if ( shouldRender && impl->simpleRoot )
        {
            const auto simpleRoot = impl->simpleRoot;
            const auto hostedContent = impl->host.GetContent();
            live = state ? state->GetOwner() : nullptr;
            if ( live != this || !live->m_winui ||
                 live->m_winui.get() != impl ||
                 live->m_winui->callbackState != state ||
                 live->m_winui->simpleRoot != simpleRoot )
            {
                return false;
            }
            shouldRender = !impl->comboTemplateTransition &&
                           !impl->comboTemplateReplayPending &&
                           wxWinUIChoiceObjectIdentity(hostedContent) ==
                               wxWinUIChoiceObjectIdentity(simpleRoot);
        }
        if ( shouldRender )
            impl->host.ForceRender();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox selection update", e);
    }

    wxChoice * const live = state ? state->GetOwner() : nullptr;
    return live == this && live->m_winui &&
           live->m_winui.get() == impl &&
           live->m_winui->callbackState == state;
}

void wxChoice::SendSelectionEvent()
{
    const int selection = GetSelection();
    wxCommandEvent event(wxEVT_CHOICE, GetId());
    InitCommandEventWithItems(event, selection);
    event.SetInt(selection);
    if ( selection != wxNOT_FOUND )
        event.SetString(m_itemModel.At(selection).text);
    ProcessCommand(event);
}

void wxChoice::WinUIInsertPeerItem(unsigned int n)
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) ||
            n >= m_itemModel.GetCount() )
    {
        return;
    }

    bool peerReady =
        m_winui->peerItemsValid &&
        m_winui->peerItemIds.size() + 1 ==
            m_itemModel.GetCount();
    try
    {
        peerReady = peerReady &&
                    wxWinUIChoiceGetPeerItems(m_winui.get()).Size() ==
                        m_winui->peerItemIds.size();
    }
    catch ( const winrt::hresult_error& )
    {
        peerReady = false;
    }
    if ( !peerReady )
    {
        ApplyItemsToPeer();
        return;
    }

    {
        bool failed = false;
        wxWinUIPeerMutationGuard guard(m_winui->callbackState);
        try
        {
            const auto items = wxWinUIChoiceGetPeerItems(m_winui.get());
            items.InsertAt(n, wxWinUICreateChoicePeerItem(m_winui.get()));
            m_winui->peerItemIds.insert(
                m_winui->peerItemIds.begin() + n,
                m_itemModel.At(n).id);
            m_winui->peerItemsValid =
                items.Size() ==
                    m_itemModel.GetCount() &&
                m_winui->peerItemIds.size() ==
                    m_itemModel.GetCount();
            WinUIUpdatePeerItem(n);
            failed = !m_winui->peerItemsValid;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox item insertion", e);
            m_winui->peerItemsValid = false;
            failed = true;
        }

        if ( !failed )
            return;
    }

    ApplyItemsToPeer();
}

void wxChoice::WinUIErasePeerItem(unsigned int n)
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) )
        return;

    bool peerReady =
        m_winui->peerItemsValid &&
        m_winui->peerItemIds.size() ==
            m_itemModel.GetCount() + 1;
    try
    {
        peerReady = peerReady &&
                    wxWinUIChoiceGetPeerItems(m_winui.get()).Size() ==
                        m_winui->peerItemIds.size();
    }
    catch ( const winrt::hresult_error& )
    {
        peerReady = false;
    }
    if ( !peerReady )
    {
        ApplyItemsToPeer();
        return;
    }

    {
        bool failed = false;
        wxWinUIPeerMutationGuard guard(m_winui->callbackState);
        try
        {
            const auto items = wxWinUIChoiceGetPeerItems(m_winui.get());
            items.RemoveAt(n);
            m_winui->peerItemIds.erase(
                m_winui->peerItemIds.begin() + n);
            m_winui->peerItemsValid =
                items.Size() == m_itemModel.GetCount() &&
                m_winui->peerItemIds.size() ==
                    m_itemModel.GetCount();
            failed = !m_winui->peerItemsValid;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox item removal", e);
            m_winui->peerItemsValid = false;
            failed = true;
        }

        if ( !failed )
            return;
    }

    ApplyItemsToPeer();
}

void wxChoice::WinUIMovePeerItem(unsigned int oldIndex,
                                 unsigned int newIndex)
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) ||
         oldIndex == newIndex )
        return;

    bool peerReady =
        m_winui->peerItemsValid &&
        m_winui->peerItemIds.size() == m_itemModel.GetCount();
    try
    {
        peerReady = peerReady &&
                    wxWinUIChoiceGetPeerItems(m_winui.get()).Size() ==
                        m_itemModel.GetCount() &&
                    oldIndex < m_winui->peerItemIds.size() &&
                    newIndex < m_itemModel.GetCount() &&
                    m_winui->peerItemIds[oldIndex] ==
                        m_itemModel.At(newIndex).id;
    }
    catch ( const winrt::hresult_error& )
    {
        peerReady = false;
    }
    if ( !peerReady )
    {
        ApplyItemsToPeer();
        return;
    }

    {
        bool failed = false;
        wxWinUIPeerMutationGuard guard(m_winui->callbackState);
        try
        {
            const auto items = wxWinUIChoiceGetPeerItems(m_winui.get());
            const auto item = items.GetAt(oldIndex);
            items.RemoveAt(oldIndex);
            items.InsertAt(newIndex, item);

            const std::uint64_t id =
                m_winui->peerItemIds.at(oldIndex);
            m_winui->peerItemIds.erase(
                m_winui->peerItemIds.begin() + oldIndex);
            m_winui->peerItemIds.insert(
                m_winui->peerItemIds.begin() + newIndex, id);
            m_winui->peerItemsValid = true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox item move", e);
            m_winui->peerItemsValid = false;
            failed = true;
        }

        if ( !failed )
            return;
    }

    ApplyItemsToPeer();
}

void wxChoice::WinUIUpdatePeerItem(unsigned int n,
                                   double requestedBitmapScale)
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) ||
            n >= m_itemModel.GetCount() )
    {
        return;
    }

    if ( !m_winui->peerItemsValid ||
         n >= m_winui->peerItemIds.size() ||
         m_winui->peerItemIds[n] != m_itemModel.At(n).id )
    {
        ApplyItemsToPeer();
        return;
    }

    wxWinUIPeerMutationGuard guard(m_winui->callbackState);
    try
    {
        namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
        const auto items = wxWinUIChoiceGetPeerItems(m_winui.get());
        if ( n >= items.Size() )
            return;

        const MUXC::ContentControl item =
            items.GetAt(n).as<MUXC::ContentControl>();
        const wxBitmap bmp =
            WinUIGetItemBitmap(n, requestedBitmapScale);
        if ( bmp.IsOk() )
        {
            MUXC::StackPanel panel;
            panel.Orientation(MUXC::Orientation::Horizontal);
            panel.Spacing(8);

            if ( auto source = wxWinUIWriteableBitmapFromBitmap(bmp) )
            {
                MUXC::Image image;
                image.Source(source);
                // wxMSW's historical GetLogicalWidth()/Height() aliases
                // deliberately return physical pixels. XAML layout is in
                // DIPs, and wxBitmapBundle::GetBitmap() records the selected
                // representation's scale factor, so GetDIPSize() is the
                // portable, non-double-scaled size here.
                const wxSize dipSize = bmp.GetDIPSize();
                image.Width(dipSize.x);
                image.Height(dipSize.y);
                panel.Children().Append(image);
            }

            MUXC::TextBlock text;
            text.Text(wxWinUIToHString(m_itemModel.At(n).text));
            text.VerticalAlignment(
                winrt::Microsoft::UI::Xaml::VerticalAlignment::Center);
            panel.Children().Append(text);
            item.Content(panel);
        }
        else
        {
            item.Content(winrt::box_value(
                wxWinUIToHString(m_itemModel.At(n).text)));
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox item content update", e);
        m_winui->peerItemsValid = false;
    }
}

#ifdef WXWINUI_TEST_SUPPORT
std::uint64_t wxWinUIChoiceTestAccess::GetItemId(const wxChoice* control,
    unsigned int n)
{
    return n < control->m_itemModel.GetCount() ? control->m_itemModel.At(n).id : 0;
}

std::uintptr_t wxWinUIChoiceTestAccess::GetItemPeerIdentity(const wxChoice* control,
    unsigned int n)
{
    if ( !wxWinUIChoiceHasItemPeer(control->m_winui.get()) )
        return 0;

    try
    {
        const auto items = wxWinUIChoiceGetPeerItems(control->m_winui.get());
        if ( n >= items.Size() )
            return 0;
        return reinterpret_cast<std::uintptr_t>(
            winrt::get_abi(
                wxWinUIChoiceObjectIdentity(items.GetAt(n))));
    }
    catch ( const winrt::hresult_error& )
    {
        return 0;
    }
}

bool wxWinUIChoiceTestAccess::GetItemPeerBitmapState(const wxChoice* control,
    unsigned int n,
    wxSize *pixelSize,
    wxSize *dipSize)
{
    if ( !wxWinUIChoiceHasItemPeer(control->m_winui.get()) ||
            !control->m_winui->peerItemsValid ||
            n >= control->m_winui->peerItemIds.size() )
    {
        return false;
    }

    try
    {
        namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
        namespace MUXMI =
            winrt::Microsoft::UI::Xaml::Media::Imaging;

        const auto items = wxWinUIChoiceGetPeerItems(control->m_winui.get());
        if ( n >= items.Size() )
            return false;

        const MUXC::ContentControl item =
            items.GetAt(n).try_as<MUXC::ContentControl>();
        const MUXC::StackPanel panel =
            item ? item.Content().try_as<MUXC::StackPanel>() : nullptr;
        if ( !panel )
            return false;

        const auto children = panel.Children();
        for ( std::uint32_t i = 0; i < children.Size(); ++i )
        {
            const MUXC::Image image =
                children.GetAt(i).try_as<MUXC::Image>();
            if ( !image )
                continue;

            const MUXMI::WriteableBitmap bitmap =
                image.Source().try_as<MUXMI::WriteableBitmap>();
            if ( !bitmap )
                return false;

            if ( pixelSize )
            {
                *pixelSize =
                    wxSize(bitmap.PixelWidth(), bitmap.PixelHeight());
            }
            if ( dipSize )
            {
                *dipSize =
                    wxSize(static_cast<int>(std::lround(image.Width())),
                           static_cast<int>(std::lround(image.Height())));
            }
            return true;
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return false;
}

bool wxWinUIChoiceTestAccess::SelectPeerItem(wxChoice* control,
    int selection)
{
    if ( !wxWinUIChoiceHasItemPeer(control->m_winui.get()) )
        return false;

    if ( !control->WinUIEnsurePeerConsistent() ||
         !control->m_winui || !control->m_winui->peerItemsValid )
        return false;

    try
    {
        wxWinUIChoiceSetPeerSelection(control->m_winui.get(), selection);
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI Choice peer selection test mutation", e);
        return false;
    }
}

bool wxWinUIChoiceTestAccess::SetDropDown(wxChoice* control,
    bool open)
{
    if ( !control->m_winui || !control->m_winui->comboBox )
        return false;

    if ( open && control->WinUICoalescePopupReopen(
             [](wxChoice *owner)
             {
                 wxWinUIChoiceTestAccess::SetDropDown(owner, true);
             }) )
    {
        return true;
    }

    wxWinUIChoiceImpl * const impl = control->m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> state =
        impl->callbackState;
    const MUXC::ComboBox combo = impl->comboBox;
    const std::shared_ptr<wxWinUIComboPopupRetirementState>
        popupRetirementState = impl->popupRetirementState;
    const wxWeakRef<wxWindow> self(control);
    const auto getLiveOwner = [&]() -> wxChoice *
    {
        wxChoice * const owner = wxDynamicCast(self.get(), wxChoice);
        return owner && owner->m_winui && owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == state &&
                       owner->m_winui->comboBox == combo
                 ? owner
                 : nullptr;
    };
    bool setterAttempted = false;
    try
    {
        if ( impl->comboBox.SelectionChangedTrigger() !=
             winrt::Microsoft::UI::Xaml::Controls::
                 ComboBoxSelectionChangedTrigger::Always )
        {
            return false;
        }

        if ( open )
        {
            // Match wxComboBox::Popup(): apply the template while the peer is
            // closed, then render that exact tree before opening it. Reversing
            // this order leaves the named Popup part unrealized; applying a
            // template after the setter closes the dropdown inside WinUI.
            combo.ApplyTemplate();
            wxChoice *live = getLiveOwner();
            if ( !live )
                return false;
            impl->host.ForceRender();
            live = getLiveOwner();
            if ( !live )
                return false;
            if ( !wxWinUIPrepareComboPopupOpen(popupRetirementState) )
                return false;
            live = getLiveOwner();
            if ( !live )
            {
                wxWinUICancelPreparedComboPopupOpen(popupRetirementState);
                return false;
            }
            if ( live->WinUICoalescePopupReopen(
                     [](wxChoice *owner)
                     {
                         wxWinUIChoiceTestAccess::SetDropDown(owner, true);
                     }) )
            {
                wxWinUICancelPreparedComboPopupOpen(popupRetirementState);
                return true;
            }
        }

        setterAttempted = open;
        combo.IsDropDownOpen(open);
        wxChoice *live = getLiveOwner();
        if ( !live )
        {
            return false;
        }
        if ( open &&
             !wxWinUIFinishPreparedComboPopupOpen(popupRetirementState) )
        {
            return false;
        }

        live = getLiveOwner();
        if ( !live )
        {
            return false;
        }

        // The peer normally raises this transition inline, but a retained
        // popup can defer it. Keep the deterministic seam synchronous while
        // leaving the later native event idempotent.
        live->WinUIOnDropDownChanged(open);
        live = wxDynamicCast(self.get(), wxChoice);
        return live && live->m_winui && live->m_winui.get() == impl;
    }
    catch ( const winrt::hresult_error& e )
    {
        if ( setterAttempted )
            wxWinUIAbortPreparedComboPopupOpen(popupRetirementState);
        else
            wxWinUICancelPreparedComboPopupOpen(popupRetirementState);
        wxWinUILogException("WinUI Choice dropdown test mutation", e);
        return false;
    }
    catch ( ... )
    {
        if ( setterAttempted )
            wxWinUIAbortPreparedComboPopupOpen(popupRetirementState);
        else
            wxWinUICancelPreparedComboPopupOpen(popupRetirementState);
        return false;
    }
}

bool wxWinUIChoiceTestAccess::IsPeerDropDownOpen(const wxChoice* control)
{
    if ( !control->m_winui || !control->m_winui->comboBox )
        return false;
    try
    {
        return control->m_winui->comboBox.IsDropDownOpen();
    }
    catch ( ... )
    {
        return false;
    }
}

bool wxWinUIChoiceTestAccess::GetPopupReopenSnapshot(const wxChoice* control,
    bool *pending,
    std::uint64_t *generation,
    unsigned *schedules,
    unsigned *runs)
{
    if ( !control->m_winui || !pending || !generation || !schedules || !runs )
        return false;
    *pending = control->m_winui->popupReopenPending;
    *generation = control->m_winui->popupReopenGeneration;
    *schedules = control->m_winui->popupReopenSchedulesForTesting;
    *runs = control->m_winui->popupReopenRunsForTesting;
    return true;
}

#endif // WXWINUI_TEST_SUPPORT

#endif // wxUSE_CHOICE
