/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/combobox.cpp
// Purpose:     wxWinUI wxComboBox implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_COMBOBOX

#include "wx/combobox.h"

#ifdef WXWINUI_TEST_SUPPORT
#include "combobox-test-access.h"
#include "choice-test-access.h"
#include "combobox-uia-helper.h"
#include <UIAutomation.h>
#endif

#ifndef WX_PRECOMP
    #include "wx/event.h"
#endif

#include "wx/clipbrd.h"
#include "wx/dataobj.h"
#include "wx/dir.h"
#include "wx/filename.h"
#include "private.h"
#include "wx/scopeguard.h"
#include "wx/textcompleter.h"
#include "wx/weakref.h"
#include "wx/winui/winui.h"
#include "wx/winui/private/inputtest.h"
#include "wx/winui/private/textpeer.h"

#include <algorithm>
#include <cmath>
#include <exception>
#include <limits>
#include <vector>

#include <winrt/Microsoft.UI.Text.h>
#ifdef WXWINUI_TEST_SUPPORT
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#endif

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXT = winrt::Microsoft::UI::Text;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace MUXD = winrt::Microsoft::UI::Dispatching;
#ifdef WXWINUI_TEST_SUPPORT
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
#endif

namespace
{

#ifdef WXWINUI_TEST_SUPPORT
struct wxWinUIComboLayoutTestRequest;
wxWinUIComboLayoutTestRequest *gs_comboLayoutTestRequest = nullptr;

// Lexically owned by the accessor, including any early-return/destruction.
// This leaves no test parameter or storage in an installed/shared header.
struct wxWinUIComboLayoutTestRequest
{
    wxWinUIComboLayoutTestRequest(
        wxComboBox *control,
        wxWinUIChoiceImpl *implementation,
        const std::shared_ptr<wxWinUIChoiceCallbackState>& choice,
        const std::shared_ptr<wxWinUITextCallbackState>& text)
        : owner(control), impl(implementation), choiceState(choice),
          textState(text), previous(gs_comboLayoutTestRequest)
    {
        gs_comboLayoutTestRequest = this;
    }

    ~wxWinUIComboLayoutTestRequest()
    {
        gs_comboLayoutTestRequest = previous;
    }

    wxComboBox * const owner;
    wxWinUIChoiceImpl * const impl;
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState;
    const std::shared_ptr<wxWinUITextCallbackState> textState;
    wxWinUIComboLayoutTestRequest * const previous;
    bool pending = true;

    wxDECLARE_NO_COPY_CLASS(wxWinUIComboLayoutTestRequest);
};

bool wxWinUITakeComboLayoutTestRequest(
    wxComboBox *owner, wxWinUIChoiceImpl *impl)
{
    for ( auto *request = gs_comboLayoutTestRequest;
          request; request = request->previous )
    {
        if ( request->owner == owner && request->impl == impl &&
             request->choiceState == impl->callbackState &&
             request->textState == impl->textCallbackState &&
             request->choiceState->GetOwner() == owner &&
             request->textState->GetOwner<wxComboBox>() == owner )
        {
            const bool pending = request->pending;
            request->pending = false;
            return pending;
        }
    }
    return false;
}
#endif // WXWINUI_TEST_SUPPORT

template <typename T>
winrt::Windows::Foundation::IUnknown
wxWinUIComboObjectIdentity(const T& object) noexcept
{
    return object
        ? object.template try_as<winrt::Windows::Foundation::IUnknown>()
        : nullptr;
}

long wxWinUIClampComboTextPos(long pos, long len)
{
    if ( pos < 0 )
        return len;
    if ( pos > len )
        return len;
    return pos;
}

bool wxWinUIComboIsUTF16Boundary(const wxString& value,
                                 std::size_t offset)
{
    if ( offset == 0 || offset >= value.length() )
        return true;

    const unsigned int before =
        static_cast<unsigned int>(value[offset - 1]);
    const unsigned int after =
        static_cast<unsigned int>(value[offset]);
    return !(before >= 0xd800 && before <= 0xdbff &&
             after >= 0xdc00 && after <= 0xdfff);
}

std::size_t wxWinUIComboFitUTF16Prefix(const wxString& value,
                                      std::size_t available)
{
    std::size_t offset = 0;
    while ( offset < value.length() )
    {
        std::size_t units = 1;
        const unsigned int first =
            static_cast<unsigned int>(value[offset]);
        if ( first >= 0xd800 && first <= 0xdbff &&
             offset + 1 < value.length() )
        {
            const unsigned int second =
                static_cast<unsigned int>(value[offset + 1]);
            if ( second >= 0xdc00 && second <= 0xdfff )
                units = 2;
        }

        if ( offset >= available || units > available - offset )
            break;
        offset += units;
    }
    return offset;
}

#ifdef WXWINUI_TEST_SUPPORT
int wxWinUIComboThemeForTesting(MUX::ElementTheme theme)
{
    switch ( theme )
    {
        case MUX::ElementTheme::Default:
            return wxWinUIComboBoxTestAccess::WinUITheme_Default;
        case MUX::ElementTheme::Light:
            return wxWinUIComboBoxTestAccess::WinUITheme_Light;
        case MUX::ElementTheme::Dark:
            return wxWinUIComboBoxTestAccess::WinUITheme_Dark;
    }

    return wxWinUIComboBoxTestAccess::WinUITheme_Unknown;
}
#endif

struct wxWinUIComboConstrainedEdit
{
    wxString value;
    bool wasTruncated{false};
    long proposedStart{0};
    long proposedEnd{0};
    long acceptedStart{0};
    long acceptedEnd{0};

    long RemapProposedPosition(long position) const
    {
        if ( !wasTruncated || position < 0 )
            return position;
        if ( position <= proposedStart )
        {
            return wxMax(0L,
                         acceptedStart - (proposedStart - position));
        }
        if ( position >= proposedEnd )
            return acceptedEnd + (position - proposedEnd);
        return acceptedStart +
               wxMin(position - proposedStart,
                     acceptedEnd - acceptedStart);
    }
};

wxWinUIComboConstrainedEdit wxWinUIConstrainComboPeerEdit(
    const wxString& before,
    const wxString& proposed,
    unsigned long configuredLimit)
{
    wxWinUIComboConstrainedEdit result;
    result.value = proposed;
    if ( !configuredLimit )
        return result;

    const long proposedLength =
        wxWinUITextPositionMap(proposed, false, false).GetLastPosition();
    if ( proposedLength >= 0 &&
         static_cast<unsigned long>(proposedLength) <= configuredLimit )
    {
        return result;
    }

    std::size_t prefix = 0;
    const std::size_t common = wxMin(before.length(), proposed.length());
    while ( prefix < common && before[prefix] == proposed[prefix] )
        ++prefix;
    while ( prefix &&
            (!wxWinUIComboIsUTF16Boundary(before, prefix) ||
             !wxWinUIComboIsUTF16Boundary(proposed, prefix)) )
    {
        --prefix;
    }

    std::size_t suffix = 0;
    while ( suffix < before.length() - prefix &&
            suffix < proposed.length() - prefix &&
            before[before.length() - suffix - 1] ==
                proposed[proposed.length() - suffix - 1] )
    {
        ++suffix;
    }
    while ( suffix &&
            (!wxWinUIComboIsUTF16Boundary(
                 before, before.length() - suffix) ||
             !wxWinUIComboIsUTF16Boundary(
                 proposed, proposed.length() - suffix)) )
    {
        --suffix;
    }

    const std::size_t insertionLength =
        proposed.length() - prefix - suffix;
    // Deletion remains possible when a programmatic value already exceeds
    // the user-input limit. There is no inserted span to reject or report.
    if ( insertionLength == 0 )
        return result;

    const std::size_t preservedLength = prefix + suffix;
    const std::size_t available =
        preservedLength < configuredLimit
            ? static_cast<std::size_t>(configuredLimit - preservedLength)
            : 0;
    const wxString proposedInsertion =
        proposed.substr(prefix, insertionLength);
    const std::size_t acceptedUnits =
        wxWinUIComboFitUTF16Prefix(proposedInsertion, available);

    result.value = before.substr(0, prefix);
    result.value += proposedInsertion.substr(0, acceptedUnits);
    if ( suffix )
        result.value += before.substr(before.length() - suffix);
    result.proposedStart = static_cast<long>(prefix);
    result.proposedEnd = static_cast<long>(prefix + insertionLength);
    result.acceptedStart = static_cast<long>(prefix);
    result.acceptedEnd = static_cast<long>(prefix + acceptedUnits);
    result.wasTruncated = acceptedUnits != insertionLength;
    return result;
}

MUXC::TextBox wxWinUIFindTextBox(MUX::DependencyObject const& root)
{
    if ( !root )
        return nullptr;

    if ( const MUXC::TextBox textBox = root.try_as<MUXC::TextBox>() )
        return textBox;

    using MUX::Media::VisualTreeHelper;

    const int count = VisualTreeHelper::GetChildrenCount(root);
    for ( int i = 0; i < count; ++i )
    {
        auto child = VisualTreeHelper::GetChild(root, i);
        if ( auto tb = child.try_as<MUXC::TextBox>() )
            return tb;
        if ( auto tb = wxWinUIFindTextBox(child) )
            return tb;
    }

    return nullptr;
}

MUXC::ScrollViewer wxWinUIFindScrollViewer(
    MUX::DependencyObject const& root)
{
    if ( !root )
        return nullptr;

    if ( const MUXC::ScrollViewer scroll =
             root.try_as<MUXC::ScrollViewer>() )
    {
        return scroll;
    }

    using MUX::Media::VisualTreeHelper;
    const int count = VisualTreeHelper::GetChildrenCount(root);
    for ( int i = 0; i < count; ++i )
    {
        if ( const MUXC::ScrollViewer scroll =
                 wxWinUIFindScrollViewer(
                     VisualTreeHelper::GetChild(root, i)) )
        {
            return scroll;
        }
    }

    return nullptr;
}

wxWinUIKeyboardModifiers wxWinUIComboGetModifiers()
{
    wxWinUIKeyboardModifiers modifiers = {};
    if ( wxWinUI3GetKeyboardModifiersOverrideForTesting(&modifiers) )
        return modifiers;

    modifiers.shiftDown = wxIsShiftDown();
    modifiers.controlDown = wxIsCtrlDown();
    modifiers.leftAltDown = (::GetKeyState(VK_LMENU) & 0x8000) != 0;
    modifiers.rightAltDown = (::GetKeyState(VK_RMENU) & 0x8000) != 0;
    return modifiers;
}

bool wxWinUIComboHasModifiers()
{
    const wxWinUIKeyboardModifiers modifiers =
        wxWinUIComboGetModifiers();
    return modifiers.shiftDown || modifiers.controlDown ||
           modifiers.leftAltDown || modifiers.rightAltDown;
}

class wxWinUIComboFileNameCompleter final : public wxTextCompleterSimple
{
public:
    explicit wxWinUIComboFileNameCompleter(int flags)
        : m_flags(flags)
    {
    }

    void GetCompletions(const wxString& prefix,
                        wxArrayString& completions) override
    {
        wxFileName name(prefix);
        wxString directory = name.GetPath();
        const wxString leaf = name.GetFullName();
        if ( directory.empty() )
            directory = wxS(".");

        wxDir dir(directory);
        if ( !dir.IsOpened() )
            return;

        const int dirFlags = m_flags == wxDIR
            ? wxDIR_DIRS
            : wxDIR_DIRS | wxDIR_FILES;
        wxString entry;
        bool found = dir.GetFirst(&entry, leaf + wxS("*"), dirFlags);
        while ( found )
        {
            wxFileName completion(directory, entry);
            wxString value = completion.GetFullPath();
            if ( name.GetPath().empty() )
                value = entry;
            if ( m_flags == wxDIR )
                value += wxFileName::GetPathSeparator();
            completions.Add(value);
            found = dir.GetNext(&entry);
        }
    }

private:
    const int m_flags;
};

void wxWinUIRevokeComboEditHandlers(
    const MUXC::TextBox& editBox,
    const MUX::UIElement& keyTarget,
    const MUX::UIElement& outerKeyTarget,
    long long textChangedCallbackToken,
    winrt::event_token selectionToken,
    winrt::event_token layoutUpdatedToken,
    winrt::event_token keyDownToken,
    winrt::event_token outerKeyDownToken,
    winrt::event_token pointerPressedToken,
    winrt::event_token copyingToken,
    winrt::event_token cuttingToken,
    winrt::event_token pasteToken)
{
    if ( !editBox )
        return;

    if ( textChangedCallbackToken )
    {
        try
        {
            editBox.UnregisterPropertyChangedCallback(
                MUXC::TextBox::TextProperty(), textChangedCallbackToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI ComboBox edit text callback removal", e);
        }
    }
    if ( selectionToken.value )
    {
        try
        {
            editBox.SelectionChanged(selectionToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI ComboBox edit selection removal", e);
        }
    }
    if ( layoutUpdatedToken.value )
    {
        try
        {
            editBox.LayoutUpdated(layoutUpdatedToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI ComboBox edit layout removal", e);
        }
    }
    if ( keyTarget && keyDownToken.value )
    {
        try
        {
            keyTarget.PreviewKeyDown(keyDownToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox edit KeyDown removal", e);
        }
    }
    if ( outerKeyTarget && outerKeyDownToken.value )
    {
        try
        {
            outerKeyTarget.PreviewKeyDown(outerKeyDownToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI ComboBox outer edit KeyDown removal", e);
        }
    }
    if ( pointerPressedToken.value )
    {
        try
        {
            editBox.PointerPressed(pointerPressedToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI ComboBox edit PointerPressed removal", e);
        }
    }
    if ( copyingToken.value )
    {
        try
        {
            editBox.CopyingToClipboard(copyingToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI ComboBox CopyingToClipboard removal", e);
        }
    }
    if ( cuttingToken.value )
    {
        try
        {
            editBox.CuttingToClipboard(cuttingToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI ComboBox CuttingToClipboard removal", e);
        }
    }
    if ( pasteToken.value )
    {
        try
        {
            editBox.Paste(pasteToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox Paste removal", e);
        }
    }
}

} // namespace

wxComboBox::wxComboBox() = default;

wxComboBox::wxComboBox(wxWindow *parent,
                       wxWindowID id,
                       const wxString& value,
                       const wxPoint& pos,
                       const wxSize& size,
                       int n,
                       const wxString choices[],
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
    : wxComboBox()
{
    Create(parent, id, value, pos, size, n, choices, style, validator, name);
}

wxComboBox::wxComboBox(wxWindow *parent,
                       wxWindowID id,
                       const wxString& value,
                       const wxPoint& pos,
                       const wxSize& size,
                       const wxArrayString& choices,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
    : wxComboBox()
{
    Create(parent, id, value, pos, size, choices, style, validator, name);
}

wxComboBox::~wxComboBox()
{
    // Revoke derived-class delegates while m_value and the text-selection
    // state are still alive. The base Close() then revokes item selection and
    // detaches the peer from its host.
    CloseTextPeer();
    if ( m_winui )
        m_winui->Close();
}

void wxComboBox::ClearPendingTextValue()
{
    if ( !m_winui )
        return;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    impl->hasPendingTextValue = false;
    impl->pendingTextValue.clear();
    impl->pendingTextEditGeneration = 0;
    impl->pendingTextItemId = 0;
}

void wxComboBox::ArmPendingTextValue(const wxString& value,
                                     std::uint64_t selectedItemId)
{
    if ( !m_winui )
        return;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    if ( !impl->textCallbackState || !impl->callbackState )
        return;

    impl->pendingTextValue = value;
    impl->pendingTextEditGeneration = impl->editGeneration;
    impl->pendingTextItemId = selectedItemId;
    impl->hasPendingTextValue = true;
}

void wxComboBox::ReconcilePendingTextValueAtPeerEdge()
{
    if ( !m_winui || !m_winui->hasPendingTextValue ||
         !m_winui->editBox || !m_winui->callbackState ||
         m_winui->callbackState->IsPeerMutationInProgress() ||
         m_winui->comboTemplateTransition )
    {
        return;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    if ( impl->pendingTextEditGeneration != impl->editGeneration ||
         impl->pendingTextItemId != m_selectedItemId ||
         impl->pendingTextValue != m_value )
    {
        ClearPendingTextValue();
        return;
    }

    try
    {
        const wxString peerValue =
            wxWinUITextPositionMap::NormalizeNewlines(
                wxWinUIFromHString(impl->editBox.Text()));
        if ( peerValue == impl->pendingTextValue )
        {
            // LayoutUpdated is the concrete end of the exact editor's
            // projection transaction. Unlike a posted callback, it cannot run
            // ahead of a later SelectedIndex -> Text layout pass.
            ClearPendingTextValue();
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox pending text layout read", e);
    }
}

void wxComboBox::ClearPendingTextSelection()
{
    if ( !m_winui )
        return;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    impl->hasPendingTextSelection = false;
    impl->pendingTextSelectionStart = 0;
    impl->pendingTextSelectionEnd = 0;
    impl->pendingTextSelectionGeneration = 0;
    impl->pendingTextSelectionRevision = 0;
    impl->pendingTextSelectionEditIdentity = 0;
    impl->pendingRangeProjectionHookForTesting = nullptr;
    impl->pendingRangeFinalizationHookForTesting = nullptr;
    impl->independentRangeObservationHookForTesting = nullptr;
}

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIComboBoxTestAccess::RunIndependentRangeObservationHook(wxComboBox* control)
{
    if ( !control->m_winui ||
         !control->m_winui->independentRangeObservationHookForTesting )
    {
        return;
    }

    // This is called only after the ordinary handler imported the live range.
    // The hook may delete the owner, so move it out and make invocation the
    // final owner operation in the native delegate.
    std::function<void(wxComboBox *)> hook =
        std::move(control->m_winui->independentRangeObservationHookForTesting);
    hook(control);
}
#endif // WXWINUI_TEST_SUPPORT

bool wxComboBox::SuppressPendingTextSelectionChange()
{
    if ( !m_winui || !m_winui->hasPendingTextSelection )
        return false;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const MUXC::TextBox editBox = impl->editBox;
    const std::uintptr_t editIdentity = editBox
        ? reinterpret_cast<std::uintptr_t>(
              winrt::get_abi(wxWinUIComboObjectIdentity(editBox)))
        : 0;
    if ( !editBox ||
         impl->pendingTextSelectionGeneration != impl->editGeneration ||
         impl->pendingTextSelectionEditIdentity != editIdentity )
    {
        ClearPendingTextSelection();
        return false;
    }

    // The transition ticket, not the transient peer range, remains
    // authoritative until the exact delayed SelectAll edge has been replayed
    // and passively confirmed on the editor's own dispatcher. In particular,
    // a getter must never import or repair that transient peer range.
    return true;
}

bool wxComboBox::HandlePendingTextSelectionChange()
{
    if ( !SuppressPendingTextSelectionChange() || !m_winui )
        return false;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const MUXC::TextBox editBox = impl->editBox;
    const long expectedFrom = wxMin(impl->pendingTextSelectionStart,
                                    impl->pendingTextSelectionEnd);
    const long expectedTo = wxMax(impl->pendingTextSelectionStart,
                                  impl->pendingTextSelectionEnd);
    try
    {
        const long actualFrom = editBox.SelectionStart();
        const long actualTo = actualFrom + editBox.SelectionLength();
        if ( actualFrom == expectedFrom && actualTo == expectedTo )
            return true;

        const long peerLast = static_cast<long>(editBox.Text().size());
        if ( actualFrom != 0 || actualTo != peerLast )
        {
            // Physical keys, pointer placement, clipboard operations and wx
            // writers retire the ticket before their native range event. A
            // remaining non-SelectAll divergence is therefore an independent
            // peer writer (including accessibility selection): accept it.
            ClearPendingTextSelection();
            return false;
        }

        // This exact divergent SelectAll is the causal template-projection
        // edge for which the ticket was armed. Replay synchronously, then
        // confirm only this revision on the same XAML dispatcher. A later
        // projection increments the revision and makes this continuation a
        // no-op; no LayoutUpdated count or artificial arrange is involved.
        if ( ++impl->pendingTextSelectionRevision == 0 )
            ++impl->pendingTextSelectionRevision;
        ++impl->diagnosticRangeProjectionMismatches;

        const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
            impl->callbackState;
        const std::shared_ptr<wxWinUITextCallbackState> textState =
            impl->textCallbackState;
        const std::uint64_t generation = impl->editGeneration;
        const std::uint64_t epoch = impl->pendingTextSelectionEpoch;
        const std::uint64_t revision =
            impl->pendingTextSelectionRevision;
        const std::uintptr_t editIdentity =
            impl->pendingTextSelectionEditIdentity;
        wxComboBox * const expectedOwner = this;
        if ( !choiceState || !textState )
        {
            ClearPendingTextSelection();
            return false;
        }

        ReplayPendingTextSelection();

        wxChoice * const choiceOwner = choiceState->GetOwner();
        wxComboBox *owner = textState->GetOwner<wxComboBox>();
        if ( owner != expectedOwner || choiceOwner != owner ||
             !owner->m_winui || owner->m_winui.get() != impl ||
             owner->m_winui->callbackState != choiceState ||
             owner->m_winui->textCallbackState != textState ||
             owner->m_winui->editGeneration != generation ||
             !owner->m_winui->hasPendingTextSelection ||
             owner->m_winui->pendingTextSelectionEpoch != epoch ||
             owner->m_winui->pendingTextSelectionRevision != revision ||
             owner->m_winui->pendingTextSelectionEditIdentity !=
                 editIdentity )
        {
            return true;
        }

        const MUXD::DispatcherQueue queue = editBox.DispatcherQueue();
        if ( !queue )
            return true;

        const std::weak_ptr<wxWinUIChoiceCallbackState> weakChoiceState =
            choiceState;
        const std::weak_ptr<wxWinUITextCallbackState> weakTextState =
            textState;
        const bool confirmationQueued = queue.TryEnqueue(
            MUXD::DispatcherQueuePriority::Low,
            [weakChoiceState, weakTextState, generation, epoch, revision,
             editIdentity, expectedFrom, expectedTo]()
            {
                const std::shared_ptr<wxWinUIChoiceCallbackState>
                    queuedChoiceState = weakChoiceState.lock();
                const std::shared_ptr<wxWinUITextCallbackState>
                    queuedTextState = weakTextState.lock();
                if ( !queuedChoiceState || !queuedTextState )
                    return;

                wxChoice * const queuedChoiceOwner =
                    queuedChoiceState->GetOwner();
                wxComboBox * const queuedOwner =
                    queuedTextState->GetOwner<wxComboBox>();
                if ( !queuedOwner || queuedChoiceOwner != queuedOwner ||
                     !queuedOwner->m_winui )
                {
                    return;
                }

                wxWinUIChoiceImpl * const queuedImpl =
                    queuedOwner->m_winui.get();
                if ( queuedImpl->callbackState !=
                          queuedChoiceState ||
                     queuedImpl->textCallbackState !=
                          queuedTextState ||
                     queuedImpl->editGeneration != generation ||
                     !queuedImpl->hasPendingTextSelection ||
                     queuedImpl->pendingTextSelectionEpoch != epoch ||
                     queuedImpl->pendingTextSelectionRevision != revision ||
                     queuedImpl->pendingTextSelectionEditIdentity !=
                         editIdentity )
                {
                    return;
                }

                const MUXC::TextBox queuedEdit =
                    queuedImpl->editBox;
                const std::uintptr_t queuedIdentity = queuedEdit
                    ? reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                          wxWinUIComboObjectIdentity(queuedEdit)))
                    : 0;
                if ( !queuedEdit || queuedIdentity != editIdentity )
                    return;

                try
                {
                    const long queuedFrom = queuedEdit.SelectionStart();
                    const long queuedTo =
                        queuedFrom + queuedEdit.SelectionLength();
                    if ( queuedFrom == expectedFrom &&
                         queuedTo == expectedTo )
                    {
                        // A staged independent authority may begin only after
                        // the current revision is passively confirmed. Move
                        // its intent before ClearPendingTextSelection() erases
                        // all test hooks, then invoke it as the final operation:
                        // this Low continuation is already outside the routed
                        // SelectionChanged callback.
                        std::function<void(wxComboBox *)> finalizationHook =
                            std::move(queuedImpl->
                                pendingRangeFinalizationHookForTesting);
                        ++queuedImpl->diagnosticRangeFinalizations;
                        queuedOwner->ClearPendingTextSelection();
                        if ( finalizationHook )
                        {
                            finalizationHook(queuedOwner);
                            return;
                        }
                    }
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI ComboBox pending selection confirmation", e);
                }
            });

        // The lifetime oracle installs this only for one raw SelectAll. Run
        // it as the final action of the genuine projection callback, after
        // the exact weak confirmation has been accepted by the dispatcher.
        // The hook may delete this control: move it out and never touch the
        // owner/implementation again.
        if ( confirmationQueued &&
             impl->pendingRangeProjectionHookForTesting )
        {
            std::function<void(wxComboBox *)> hook =
                std::move(impl->pendingRangeProjectionHookForTesting);
            hook(this);
            return true;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox pending selection read", e);
    }
    return true;
}

void wxComboBox::ReplayPendingTextSelection()
{
    if ( !SuppressPendingTextSelectionChange() || !m_winui )
        return;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    const MUXC::TextBox editBox = impl->editBox;
    const std::uint64_t generation = impl->editGeneration;
    const std::uint64_t epoch = impl->pendingTextSelectionEpoch;
    const std::uint64_t revision =
        impl->pendingTextSelectionRevision;
    const long from = wxMin(impl->pendingTextSelectionStart,
                            impl->pendingTextSelectionEnd);
    const long to = wxMax(impl->pendingTextSelectionStart,
                          impl->pendingTextSelectionEnd);
    wxComboBox * const expectedOwner = this;
    if ( !choiceState || !textState )
    {
        ClearPendingTextSelection();
        return;
    }

    choiceState->BeginPeerMutation();
    wxScopeGuard mutationGuard = wxMakeGuard([choiceState]()
    {
        choiceState->EndPeerMutation();
    });
    wxUnusedVar(mutationGuard);

    try
    {
        editBox.Select(static_cast<int32_t>(from),
                       static_cast<int32_t>(to - from));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox transition selection reconciliation", e);
    }

    wxChoice * const choiceOwner = choiceState->GetOwner();
    wxComboBox * const owner = textState->GetOwner<wxComboBox>();
    if ( owner != expectedOwner || choiceOwner != owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->textCallbackState != textState ||
         owner->m_winui->callbackState != choiceState ||
         owner->m_winui->editGeneration != generation ||
         owner->m_winui->editBox != editBox ||
         !owner->m_winui->hasPendingTextSelection ||
         owner->m_winui->pendingTextSelectionEpoch != epoch ||
         owner->m_winui->pendingTextSelectionRevision != revision )
    {
        return;
    }

}

void wxComboBox::CloseTextPeer()
{
    if ( !m_winui )
        return;

    wxWinUIChoiceImpl * const impl = m_winui.get();

    // Invalidate before the first revocation attempt: each removal is allowed
    // to throw, but any queued delegate then observes a null owner.
    if ( impl->textCallbackState )
        impl->textCallbackState->Invalidate();

    ++impl->autoCompleteGeneration;
    impl->autoCompleteSelection = wxNOT_FOUND;
    MUXC::MenuFlyout autoCompleteFlyout =
        std::move(impl->autoCompleteFlyout);
    impl->autoCompleteFlyout = nullptr;
    std::shared_ptr<wxWinUIFlyoutPopupRetirementState>
        autoCompleteRetirementState =
            std::move(impl->autoCompleteRetirementState);
    // The composite gate already owns the retirement child independently.
    // Drop the control-side alias before Hide() so the final peer release is
    // governed only by the exact popup/focus gate.
    impl->autoCompleteRetirementPending = false;
    impl->autoCompleteReplayPending = false;
    std::shared_ptr<wxTextCompleter> autoCompleter =
        std::move(impl->autoCompleter);

    impl->comboPartPending = false;
    impl->comboPartResolving = false;
    impl->comboLayoutResolveQueued = false;
    impl->comboLayoutResolveRunning = false;
    impl->comboLayoutForcedRetryAttempted = false;
    impl->comboLayoutCallbackDepth = 0;
    if ( ++impl->comboLayoutResolveRevision == 0 )
        ++impl->comboLayoutResolveRevision;
    impl->comboTemplateReplayPending = false;
    if ( ++impl->editGeneration == 0 )
        ++impl->editGeneration;

    wxWinUIRevokeComboEditHandlers(
        impl->editBox,
        impl->editBox.try_as<MUX::UIElement>(),
        impl->simpleRoot
            ? impl->simpleRoot.try_as<MUX::UIElement>()
            : impl->comboBox.try_as<MUX::UIElement>(),
        impl->editTextChangedCallbackToken,
        impl->editSelectionChangedToken,
        impl->editLayoutUpdatedToken,
        impl->editKeyDownToken,
        impl->editOuterKeyDownToken,
        impl->editPointerPressedToken,
        impl->editCopyingToken,
        impl->editCuttingToken,
        impl->editPasteToken);
    impl->editTextChangedCallbackToken = 0;
    impl->editSelectionChangedToken = {};
    impl->editLayoutUpdatedToken = {};
    impl->editKeyDownToken = {};
    impl->editOuterKeyDownToken = {};
    impl->editPointerPressedToken = {};
    impl->editCopyingToken = {};
    impl->editCuttingToken = {};
    impl->editPasteToken = {};

    if ( impl->editBox && impl->simpleTextChangedCallbackToken )
    {
        try
        {
            impl->editBox.UnregisterPropertyChangedCallback(
                MUXC::TextBox::TextProperty(),
                impl->simpleTextChangedCallbackToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI simple ComboBox text callback removal", e);
        }
    }
    impl->simpleTextChangedCallbackToken = 0;

    if ( impl->editBox && impl->simpleKeyDownToken.value )
    {
        try
        {
            impl->editBox.PreviewKeyDown(impl->simpleKeyDownToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI simple ComboBox KeyDown removal", e);
        }
    }
    impl->simpleKeyDownToken = {};
    impl->simpleLayoutQueued = false;
    impl->simpleLayoutInProgress = false;
    impl->simpleLayoutReplayPending = false;
    if ( ++impl->simpleLayoutRevision == 0 )
        ++impl->simpleLayoutRevision;
    if ( ++impl->simpleLayoutEpoch == 0 )
        ++impl->simpleLayoutEpoch;

    if ( impl->comboBox )
    {
        if ( impl->textSubmittedToken.value )
        {
            try
            {
                impl->comboBox.TextSubmitted(
                    impl->textSubmittedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI ComboBox TextSubmitted removal", e);
            }
        }
        impl->textSubmittedToken = {};

        if ( impl->textChangedCallbackToken )
        {
            try
            {
                impl->comboBox.UnregisterPropertyChangedCallback(
                    MUXC::ComboBox::TextProperty(),
                    impl->textChangedCallbackToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI ComboBox text callback removal", e);
            }
        }
        impl->textChangedCallbackToken = 0;

        if ( impl->comboLoadedToken.value )
        {
            try
            {
                impl->comboBox.Loaded(impl->comboLoadedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI ComboBox Loaded removal", e);
            }
        }
        impl->comboLoadedToken = {};

        if ( impl->comboLayoutUpdatedToken.value )
        {
            try
            {
                impl->comboBox.LayoutUpdated(
                    impl->comboLayoutUpdatedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI ComboBox LayoutUpdated removal", e);
            }
        }
        impl->comboLayoutUpdatedToken = {};

        const MUX::FrameworkElement themeBoundary =
            impl->comboThemeBoundaryRoot;
        const winrt::event_token themeBoundaryToken =
            impl->comboThemeBoundaryChangedToken;
        impl->comboThemeBoundaryRoot = nullptr;
        impl->comboThemeBoundaryChangedToken = {};
        if ( themeBoundary && themeBoundaryToken.value )
        {
            try
            {
                themeBoundary.ActualThemeChanged(themeBoundaryToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI ComboBox theme-boundary removal", e);
            }
        }

        if ( impl->comboTemplateChangedCallbackToken )
        {
            try
            {
                impl->comboBox.UnregisterPropertyChangedCallback(
                    MUXC::Control::TemplateProperty(),
                    impl->comboTemplateChangedCallbackToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI ComboBox Template callback removal", e);
            }
        }
        impl->comboTemplateChangedCallbackToken = 0;

        if ( impl->dropDownOpenedToken.value )
        {
            try
            {
                impl->comboBox.DropDownOpened(
                    impl->dropDownOpenedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI ComboBox DropDownOpened removal", e);
            }
        }
        impl->dropDownOpenedToken = {};

        if ( impl->dropDownClosedToken.value )
        {
            try
            {
                impl->comboBox.DropDownClosed(
                    impl->dropDownClosedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI ComboBox DropDownClosed removal", e);
            }
        }
        impl->dropDownClosedToken = {};
    }

    impl->retiredEditBoxForTesting = nullptr;
    impl->editBox = nullptr;
    impl->textCallbackState.reset();
    ClearPendingTextValue();
    ClearPendingTextSelection();
    impl->clipboardOperationInProgress = false;
    impl->comboTemplateTransition = false;
    impl->dropDownOpen = false;
    if ( ++impl->dropDownGeneration == 0 )
        ++impl->dropDownGeneration;

    // Both boundaries may synchronously enter application code. They remain
    // the final operations after every delegate and owner pointer is retired.
    if ( autoCompleteFlyout )
    {
        if ( !autoCompleteRetirementState )
            std::terminate();
        try
        {
            autoCompleteFlyout.Hide();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI ComboBox auto-complete flyout removal", e);
            wxWinUIFailFlyoutPopupShow(
                autoCompleteRetirementState);
        }
    }
    autoCompleteFlyout = nullptr;
    autoCompleter.reset();
}

void wxComboBox::SetFocus()
{
    const wxWeakRef<wxWindow> self(this);
    wxWinUIChoiceImpl * const impl = m_winui.get();

    // Publish the logical/native wx focus request first. In particular, a
    // cross-top-level Reparent() may still be completing its one-shot slot
    // migration here; resolving XAML before this request is authoritative can
    // nominate an editor in the destination tree without giving that tree
    // native focus authority.
    wxChoice::SetFocus();

    if ( !self || !m_winui || m_winui.get() != impl )
        return;

    // Rebind the effective-theme boundary only after the destination slot owns
    // the focus intent, then resolve/navigate to its exact current editor.
    if ( !m_winui->simpleRoot && !EnsureThemeTransitionBoundary() )
        return;
    if ( !self || !m_winui || m_winui.get() != impl ||
         !ResolveEditPart() )
    {
        return;
    }

    if ( !self || !m_winui || m_winui.get() != impl )
        return;

    m_winui->host.SynchronizeForFocus();

    // SynchronizeForFocus() can publish a slot which did not yet exist when
    // wxChoice::SetFocus() recorded the logical focus intent (notably just
    // after a cross-TLW Reparent()). Re-enter the normal slot focus path once
    // with the now-current preferred editor. This is also required when the
    // old slot already had native focus: merely replacing PreferredFocus does
    // not move XAML focus to the replacement template part.
    wxComboBox * const synchronizedOwner =
        wxDynamicCast(self.get(), wxComboBox);
    if ( synchronizedOwner && synchronizedOwner->m_winui &&
         synchronizedOwner->m_winui.get() == impl )
    {
        // FocusSlot() is an arbitrary re-entry boundary, so keep it final.
        synchronizedOwner->m_winui->host.NavigateFocus(true);
    }
}

bool wxComboBox::MSWShouldPreProcessMessage(WXMSG* msg)
{
    wxComboBox *owner = this;
    if ( msg && msg->message == WM_KEYDOWN && msg->wParam == VK_TAB &&
         m_winui && m_winui->autoCompleteFlyout )
    {
        // Mixed HWND/XAML Tab arbitration runs before the TextBox KeyDown
        // delegate. Close the transient suggestion surface here, but leave
        // the message unhandled so ordinary forward/backward wx traversal
        // remains authoritative.
        const wxWeakRef<wxWindow> self(this);
        DismissAutoComplete();
        owner = wxDynamicCast(self.get(), wxComboBox);
        if ( !owner )
            return false;
    }

    // Choice owns F4 and an open dropdown's Escape/Enter. Editable combos
    // additionally keep their text-editing shortcuts in the XAML TextBox.
    return owner->wxChoice::MSWShouldPreProcessMessage(msg) &&
           (owner->HasFlag(wxCB_READONLY) ||
            owner->wxTextEntry::MSWShouldPreProcessMessage(msg));
}

bool wxComboBox::AttachEditPeerHandlers()
{
    if ( !m_winui || !m_winui->editBox ||
         !m_winui->textCallbackState )
    {
        return false;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    const std::uint64_t editGeneration = impl->editGeneration;
    const MUXC::TextBox editBox = impl->editBox;
    // WinAppSDK splits editable-combo keys between two routed owners: the
    // semantic ComboBox consumes list navigation during the tunnel, while
    // ordinary edit commands reach EditableText. SIMPLE has the same split
    // between its composite Grid and TextBox. Observe both exact peers; a
    // handled routed event won't be delivered to the second registration.
    const MUX::UIElement keyTarget = editBox.try_as<MUX::UIElement>();
    const MUX::UIElement outerKeyTarget = impl->simpleRoot
        ? impl->simpleRoot.try_as<MUX::UIElement>()
        : impl->comboBox.try_as<MUX::UIElement>();
    const auto getLiveOwner = [&]() -> wxComboBox *
    {
        wxComboBox * const owner =
            callbackState->GetOwner<wxComboBox>();
        return owner && owner->m_winui && owner->m_winui.get() == impl &&
                       owner->m_winui->textCallbackState == callbackState &&
                       owner->m_winui->editGeneration == editGeneration &&
                       owner->m_winui->editBox == editBox
                 ? owner
                 : nullptr;
    };
    long long textChangedCallbackToken = 0;
    winrt::event_token layoutUpdatedToken{};
    winrt::event_token keyDownToken{};
    winrt::event_token outerKeyDownToken{};
    winrt::event_token pointerPressedToken{};
    winrt::event_token copyingToken{};
    winrt::event_token cuttingToken{};
    winrt::event_token pasteToken{};
    try
    {
        if ( !impl->simpleListBox )
        {
            textChangedCallbackToken =
                editBox.RegisterPropertyChangedCallback(
                    MUXC::TextBox::TextProperty(),
                    [callbackState, impl, editGeneration, editBox](
                        MUX::DependencyObject const& sender,
                        MUX::DependencyProperty const&)
                    {
                        wxComboBox * const owner =
                            callbackState->GetOwner<wxComboBox>();
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != impl ||
                             owner->m_winui->textCallbackState !=
                                 callbackState ||
                             owner->m_winui->editGeneration !=
                                 editGeneration ||
                             owner->m_winui->editBox != editBox ||
                             sender.try_as<MUXC::TextBox>() != editBox )
                        {
                            return;
                        }

                        owner->OnPeerTextChanged();
                    });
        }

        layoutUpdatedToken = editBox.LayoutUpdated(
            [callbackState, impl, editGeneration, editBox](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&)
            {
                wxComboBox *owner =
                    callbackState->GetOwner<wxComboBox>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->textCallbackState != callbackState ||
                     owner->m_winui->editGeneration != editGeneration ||
                     owner->m_winui->editBox != editBox ||
                     owner->m_winui->callbackState->
                         IsPeerMutationInProgress() ||
                     owner->m_winui->comboTemplateTransition )
                {
                    return;
                }

                ++owner->m_winui->diagnosticEditLayoutEdges;
                owner->ReconcilePendingTextValueAtPeerEdge();
                owner = callbackState->GetOwner<wxComboBox>();
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->textCallbackState == callbackState &&
                     owner->m_winui->editGeneration == editGeneration &&
                     owner->m_winui->editBox == editBox &&
                     owner->m_winui->simpleRoot )
                {
                    // Initial SIMPLE realization happens while the shared
                    // slot is still collapsed, when this editor reports a
                    // zero intrinsic height.  This real post-Loaded layout
                    // edge is the causal point at which its Auto row becomes
                    // measurable.  Only inspect here; the queued Low performs
                    // the explicit layout outside the routed callback.
                    owner->QueueSimplePeerLayoutAtPeerEdge();
                }
            });

        impl->diagnosticKeyTargetIdentity =
            reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                wxWinUIComboObjectIdentity(outerKeyTarget)));
        const auto keyHandler =
            [callbackState, impl, editGeneration, editBox, keyTarget,
             outerKeyTarget](
                winrt::Windows::Foundation::IInspectable const& sender,
                MUX::Input::KeyRoutedEventArgs const& args)
            {
                wxComboBox * const owner =
                    callbackState->GetOwner<wxComboBox>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->textCallbackState != callbackState ||
                     owner->m_winui->editGeneration != editGeneration ||
                     owner->m_winui->editBox != editBox )
                {
                    return;
                }

                ++owner->m_winui->diagnosticKeyPreviewCallbacks;
                owner->m_winui->diagnosticLastPreviewKey =
                    static_cast<unsigned>(args.Key());
                owner->m_winui->diagnosticKeySenderIdentity =
                    reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                        wxWinUIComboObjectIdentity(
                            sender.try_as<MUX::UIElement>())));
                owner->m_winui->diagnosticLastPreviewHandledOnEntry =
                    args.Handled();
                const MUX::UIElement senderElement =
                    sender.try_as<MUX::UIElement>();
                if ( args.Handled() ||
                     (senderElement != keyTarget &&
                      senderElement != outerKeyTarget) )
                {
                    return;
                }

                // This routed key is a physical-input boundary. It cannot be
                // one of the delayed template/selection projections guarded
                // by either programmatic writer ticket.
                owner->ClearPendingTextValue();
                owner->ClearPendingTextSelection();

                const auto key = args.Key();
                const wxWinUIKeyboardModifiers modifiers =
                    wxWinUIComboGetModifiers();
                const unsigned keyCode = static_cast<unsigned>(key);
                const bool controlOnly =
                    modifiers.controlDown && !modifiers.shiftDown &&
                    !modifiers.leftAltDown && !modifiers.rightAltDown;
                const bool shiftOnly =
                    modifiers.shiftDown && !modifiers.controlDown &&
                    !modifiers.leftAltDown && !modifiers.rightAltDown;

                // When both transient surfaces are open, Escape belongs to
                // the ComboBox popup first. Restore the accepted stable ID
                // and leave the event unhandled so XAML closes the popup.
                if ( key == winrt::Windows::System::VirtualKey::Escape &&
                     owner->m_winui->choiceDropDownOpen )
                {
                    owner->WinUICancelPendingPeerSelection();
                    return;
                }

                // Route the standard native edit shortcuts through the same
                // cancellable wx transaction as the public commands. Mark the
                // routed event handled before entering application code: any
                // command handler is allowed to destroy this control.
                if ( controlOnly &&
                     (keyCode == static_cast<unsigned>('C') ||
                      key == winrt::Windows::System::VirtualKey::Insert) )
                {
                    ++owner->m_winui->diagnosticKeyPreviewHandled;
                    args.Handled(true);
                    owner->Copy();
                    return;
                }
                if ( (controlOnly &&
                      keyCode == static_cast<unsigned>('X')) ||
                     (shiftOnly &&
                      key == winrt::Windows::System::VirtualKey::Delete) )
                {
                    ++owner->m_winui->diagnosticKeyPreviewHandled;
                    args.Handled(true);
                    owner->Cut();
                    return;
                }
                if ( (controlOnly &&
                      keyCode == static_cast<unsigned>('V')) ||
                     (shiftOnly &&
                      key == winrt::Windows::System::VirtualKey::Insert) )
                {
                    ++owner->m_winui->diagnosticKeyPreviewHandled;
                    args.Handled(true);
                    owner->Paste();
                    return;
                }

                if ( !owner->m_winui->autoCompleteFlyout )
                    return;

                if ( key == winrt::Windows::System::VirtualKey::Tab )
                {
                    if ( !modifiers.controlDown &&
                         !modifiers.leftAltDown &&
                         !modifiers.rightAltDown )
                    {
                        const bool previous = modifiers.shiftDown;
                        const wxWeakRef<wxWindow> ownerRef(owner);
                        if ( !owner->DismissAutoComplete() )
                            return;
                        wxComboBox * const liveOwner =
                            wxDynamicCast(ownerRef.get(), wxComboBox);
                        if ( !liveOwner || !liveOwner->m_winui ||
                             liveOwner->m_winui.get() != impl ||
                             liveOwner->m_winui->textCallbackState !=
                                 callbackState ||
                             liveOwner->m_winui->editGeneration !=
                                 editGeneration ||
                             liveOwner->m_winui->editBox != editBox )
                        {
                            return;
                        }

                        // The normal message hook usually moves focus before
                        // XAML sees Tab. Native island dispatch (including
                        // accessibility/action simulators) reaches this
                        // fallback instead; both normal and SIMPLE combos are
                        // one wx tab stop and must use the same mixed arbiter.
                        WXMSG tabMessage{};
                        tabMessage.hwnd = ::GetFocus();
                        tabMessage.message = WM_KEYDOWN;
                        tabMessage.wParam = VK_TAB;
                        if ( wxWinUI3ProcessTabNavigationWithModifiers(
                                 &tabMessage, previous, false, false) )
                        {
                            args.Handled(true);
                        }
                    }
                    return;
                }

                if ( modifiers.shiftDown || modifiers.controlDown ||
                     modifiers.leftAltDown || modifiers.rightAltDown )
                {
                    return;
                }

                switch ( key )
                {
                    case winrt::Windows::System::VirtualKey::Down:
                        ++owner->m_winui->diagnosticKeyPreviewHandled;
                        args.Handled(true);
                        owner->NavigateAutoComplete(1);
                        return;

                    case winrt::Windows::System::VirtualKey::Up:
                        ++owner->m_winui->diagnosticKeyPreviewHandled;
                        args.Handled(true);
                        owner->NavigateAutoComplete(-1);
                        return;

                    case winrt::Windows::System::VirtualKey::Enter:
                        if ( owner->m_winui->autoCompleteSelection !=
                                 wxNOT_FOUND )
                        {
                            ++owner->m_winui->diagnosticKeyPreviewHandled;
                            args.Handled(true);
                            owner->AcceptAutoCompleteSelection();
                        }
                        return;

                    case winrt::Windows::System::VirtualKey::Escape:
                        ++owner->m_winui->diagnosticKeyPreviewHandled;
                        args.Handled(true);
                        owner->DismissAutoComplete();
                        return;

                    default:
                        return;
                }
            };
        outerKeyDownToken = outerKeyTarget.PreviewKeyDown(keyHandler);
        keyDownToken = keyTarget.PreviewKeyDown(keyHandler);

        pointerPressedToken = editBox.PointerPressed(
            [callbackState, impl, editGeneration, editBox](
                winrt::Windows::Foundation::IInspectable const& sender,
                MUX::Input::PointerRoutedEventArgs const&)
            {
                wxComboBox * const owner =
                    callbackState->GetOwner<wxComboBox>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->textCallbackState != callbackState ||
                     owner->m_winui->editGeneration != editGeneration ||
                     owner->m_winui->editBox != editBox ||
                     sender.try_as<MUXC::TextBox>() != editBox )
                {
                    return;
                }

                // Pointer placement is the other native caret writer. Retire
                // any programmatic projection tickets before TextBox raises
                // its resulting SelectionChanged event.
                owner->ClearPendingTextValue();
                owner->ClearPendingTextSelection();
            });

        copyingToken = editBox.CopyingToClipboard(
            [callbackState, impl, editGeneration, editBox](
                winrt::Windows::Foundation::IInspectable const& sender,
                MUXC::TextControlCopyingToClipboardEventArgs const& args)
            {
                wxComboBox *owner =
                    callbackState->GetOwner<wxComboBox>();
                const bool exact = owner && owner->m_winui &&
                    owner->m_winui.get() == impl &&
                    owner->m_winui->textCallbackState == callbackState &&
                    owner->m_winui->editGeneration == editGeneration &&
                    owner->m_winui->editBox == editBox &&
                    sender.try_as<MUXC::TextBox>() == editBox;
                if ( !exact )
                {
                    args.Handled(true);
                    return;
                }
                owner->ClearPendingTextValue();
                owner->ClearPendingTextSelection();
                if ( owner->m_winui->clipboardOperationInProgress )
                    return;

                const bool handled =
                    owner->SendClipboardEvent(wxEVT_TEXT_COPY);
                owner = callbackState->GetOwner<wxComboBox>();
                if ( handled || !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->textCallbackState != callbackState ||
                     owner->m_winui->editGeneration != editGeneration ||
                     owner->m_winui->editBox != editBox )
                {
                    args.Handled(true);
                }
            });

        cuttingToken = editBox.CuttingToClipboard(
            [callbackState, impl, editGeneration, editBox](
                winrt::Windows::Foundation::IInspectable const& sender,
                MUXC::TextControlCuttingToClipboardEventArgs const& args)
            {
                wxComboBox *owner =
                    callbackState->GetOwner<wxComboBox>();
                const bool exact = owner && owner->m_winui &&
                    owner->m_winui.get() == impl &&
                    owner->m_winui->textCallbackState == callbackState &&
                    owner->m_winui->editGeneration == editGeneration &&
                    owner->m_winui->editBox == editBox &&
                    sender.try_as<MUXC::TextBox>() == editBox;
                if ( !exact )
                {
                    args.Handled(true);
                    return;
                }
                owner->ClearPendingTextValue();
                owner->ClearPendingTextSelection();
                if ( owner->m_winui->clipboardOperationInProgress )
                    return;

                const bool handled =
                    owner->SendClipboardEvent(wxEVT_TEXT_CUT);
                owner = callbackState->GetOwner<wxComboBox>();
                if ( handled || !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->textCallbackState != callbackState ||
                     owner->m_winui->editGeneration != editGeneration ||
                     owner->m_winui->editBox != editBox )
                {
                    args.Handled(true);
                }
            });

        pasteToken = editBox.Paste(
            [callbackState, impl, editGeneration, editBox](
                winrt::Windows::Foundation::IInspectable const& sender,
                MUXC::TextControlPasteEventArgs const& args)
            {
                wxComboBox *owner =
                    callbackState->GetOwner<wxComboBox>();
                const bool exact = owner && owner->m_winui &&
                    owner->m_winui.get() == impl &&
                    owner->m_winui->textCallbackState == callbackState &&
                    owner->m_winui->editGeneration == editGeneration &&
                    owner->m_winui->editBox == editBox &&
                    sender.try_as<MUXC::TextBox>() == editBox;
                if ( !exact )
                {
                    args.Handled(true);
                    return;
                }
                owner->ClearPendingTextValue();
                owner->ClearPendingTextSelection();
                if ( owner->m_winui->clipboardOperationInProgress )
                    return;

                const bool handled =
                    owner->SendClipboardEvent(wxEVT_TEXT_PASTE);
                owner = callbackState->GetOwner<wxComboBox>();
                if ( handled || !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->textCallbackState != callbackState ||
                     owner->m_winui->editGeneration != editGeneration ||
                     owner->m_winui->editBox != editBox )
                {
                    args.Handled(true);
                }
            });
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox edit handler setup", e);
        wxWinUIRevokeComboEditHandlers(
            editBox, keyTarget, outerKeyTarget,
            textChangedCallbackToken, {}, layoutUpdatedToken,
            keyDownToken, outerKeyDownToken, pointerPressedToken,
            copyingToken, cuttingToken, pasteToken);
        return false;
    }

    wxComboBox * const owner = getLiveOwner();
    if ( !owner || impl->editTextChangedCallbackToken ||
         impl->editLayoutUpdatedToken.value ||
         impl->editKeyDownToken.value ||
         impl->editOuterKeyDownToken.value ||
         impl->editPointerPressedToken.value ||
         impl->editCopyingToken.value || impl->editCuttingToken.value ||
         impl->editPasteToken.value )
    {
        wxWinUIRevokeComboEditHandlers(
            editBox, keyTarget, outerKeyTarget,
            textChangedCallbackToken, {}, layoutUpdatedToken,
            keyDownToken, outerKeyDownToken, pointerPressedToken,
            copyingToken, cuttingToken, pasteToken);
        return false;
    }

    impl->editTextChangedCallbackToken = textChangedCallbackToken;
    impl->editLayoutUpdatedToken = layoutUpdatedToken;
    impl->editKeyDownToken = keyDownToken;
    impl->editOuterKeyDownToken = outerKeyDownToken;
    impl->editPointerPressedToken = pointerPressedToken;
    impl->editCopyingToken = copyingToken;
    impl->editCuttingToken = cuttingToken;
    impl->editPasteToken = pasteToken;
    return true;
}

bool wxComboBox::CreateSimplePeer()
{
    if ( !m_winui || !m_winui->comboBox ||
         !m_winui->callbackState || !m_winui->textCallbackState )
    {
        return false;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    wxComboBox * const expectedOwner = this;

    try
    {
        // The base Choice initially creates its normal ComboBox peer. Retire
        // all of its delegates before replacing that visual with the native
        // wxCB_SIMPLE topology. No hidden ComboBox remains to proxy either
        // text or selection after this transaction.
        const MUXC::ComboBox oldCombo = impl->comboBox;
        if ( impl->selectionChangedToken.value )
            oldCombo.SelectionChanged(impl->selectionChangedToken);
        impl->selectionChangedToken = {};
        if ( impl->choiceKeyDownToken.value )
            oldCombo.KeyDown(impl->choiceKeyDownToken);
        impl->choiceKeyDownToken = {};
        if ( impl->choiceDropDownOpenedToken.value )
            oldCombo.DropDownOpened(impl->choiceDropDownOpenedToken);
        impl->choiceDropDownOpenedToken = {};
        if ( impl->choiceDropDownClosedToken.value )
            oldCombo.DropDownClosed(impl->choiceDropDownClosedToken);
        impl->choiceDropDownClosedToken = {};

        MUXC::Grid root;
        MUXC::RowDefinition editRow;
        editRow.Height(MUX::GridLengthHelper::Auto());
        MUXC::RowDefinition listRow;
        // The persistent SIMPLE viewport is sized explicitly by
        // RealizeSimplePeerLayout(). Starting at zero prevents the first
        // unconstrained host arrange from publishing a stale full-height
        // ListBox before the editor's Auto row has been measured.
        listRow.Height(MUX::GridLengthHelper::FromPixels(0.0));
        root.RowDefinitions().Append(editRow);
        root.RowDefinitions().Append(listRow);

        MUXC::TextBox edit;
        edit.AcceptsReturn(false);
        edit.TextWrapping(MUX::TextWrapping::NoWrap);
        edit.IsReadOnly(!m_editable);
        // The native property must remain unlimited: wxMSW permits
        // programmatic values above SetMaxLength(), while peer-originated
        // edits are constrained from their complete proposed value below.
        edit.MaxLength(0);
        if ( m_forceUpper )
            edit.CharacterCasing(MUXC::CharacterCasing::Upper);
        if ( m_horizontalMargin >= 0 )
        {
            MUX::Thickness padding = edit.Padding();
            const double horizontal = static_cast<double>(
                ToDIP(wxSize(m_horizontalMargin, 0)).x);
            padding.Left = horizontal;
            padding.Right = horizontal;
            edit.Padding(padding);
        }
        if ( !m_hint.empty() )
            edit.PlaceholderText(wxWinUIToHString(m_hint));

        MUXC::ListBox list;
        list.SelectionMode(MUXC::SelectionMode::Single);
        list.MinHeight(0.0);
        // Before the first constrained realization, neither the intrinsic
        // item extent nor a host arrange may publish a full-height viewport.
        // RealizeSimplePeerLayout() replaces both bounds atomically with the
        // exact persistent-list extent.
        list.MaxHeight(0.0);
        list.Height(0.0);
        // CBS_SIMPLE is one tab stop. Pointer focus and ListBoxItem keyboard
        // handling remain available, but Tab leaves the composite instead of
        // exposing a second public control.
        list.IsTabStop(false);
        list.HorizontalContentAlignment(MUX::HorizontalAlignment::Stretch);
        MUXC::Grid::SetRow(edit, 0);
        MUXC::Grid::SetRow(list, 1);
        root.Children().Append(edit);
        root.Children().Append(list);

        impl->simpleRoot = root;
        impl->simpleListBox = list;
        impl->editBox = edit;

        impl->simpleSelectionChangedToken = list.SelectionChanged(
            [choiceState](
                winrt::Windows::Foundation::IInspectable const&,
                MUXC::SelectionChangedEventArgs const&)
            {
                wxComboBox * const owner =
                    static_cast<wxComboBox *>(choiceState->GetOwner());
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != choiceState )
                {
                    return;
                }

                owner->WinUIOnPeerSelectionChanged();
            });

        // TextBox::TextChanged can be deferred until a composition turn for
        // an initially collapsed shared-host slot. The wx text contract is
        // synchronous, so observe the dependency property directly, exactly
        // as the normal editable ComboBox path does.
        impl->simpleTextChangedCallbackToken =
            edit.RegisterPropertyChangedCallback(
                MUXC::TextBox::TextProperty(),
                [textState](
                    MUX::DependencyObject const&,
                    MUX::DependencyProperty const&)
                {
                    wxComboBox * const owner =
                        textState->GetOwner<wxComboBox>();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->textCallbackState != textState )
                    {
                        return;
                    }

                    owner->OnPeerTextChanged();
                });

        if ( ++impl->editGeneration == 0 )
            ++impl->editGeneration;
        const std::uint64_t editGeneration = impl->editGeneration;
        impl->editSelectionChangedToken = edit.SelectionChanged(
            [textState, editGeneration](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                wxComboBox *owner =
                    textState->GetOwner<wxComboBox>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->textCallbackState != textState ||
                     owner->m_winui->editGeneration != editGeneration )
                {
                    return;
                }

                // A template replacement can publish its final SelectAll
                // while the compound replay still owns the peer-mutation
                // guard. The exact generation/identity ticket must see that
                // causal edge; the guard continues to suppress every
                // ordinary selection notification below.
                if ( owner->HandlePendingTextSelectionChange() )
                    return;

                if ( owner->m_winui->callbackState->
                         IsPeerMutationInProgress() )
                {
                    return;
                }

                if ( !owner->ReadTextSelectionFromPeer() )
                    return;
                owner = textState->GetOwner<wxComboBox>();
                if ( owner && owner->m_winui &&
                     owner->m_winui->textCallbackState == textState &&
                     owner->m_winui->editGeneration == editGeneration )
                {
                    owner->RefreshAutoComplete();
                }
                owner = textState->GetOwner<wxComboBox>();
                if ( owner && owner->m_winui &&
                     owner->m_winui->textCallbackState == textState &&
                     owner->m_winui->editGeneration == editGeneration )
                {
#ifdef WXWINUI_TEST_SUPPORT
                    wxWinUIComboBoxTestAccess::
                        RunIndependentRangeObservationHook(owner);
#endif
                }
            });

        // Register autocomplete and clipboard command handlers first. XAML
        // invokes same-source handlers in registration order, so an active
        // completion popup gets the navigation key before the persistent list
        // handler below sees it.
        if ( !AttachEditPeerHandlers() )
            return false;

        impl->simpleKeyDownToken = edit.PreviewKeyDown(
            [textState](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::Input::KeyRoutedEventArgs const& args)
            {
                if ( args.Handled() )
                    return;

                wxComboBox *owner =
                    textState->GetOwner<wxComboBox>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->textCallbackState != textState )
                {
                    return;
                }

                int direction = 0;
                bool pageNavigation = false;
                switch ( args.Key() )
                {
                    case winrt::Windows::System::VirtualKey::Up:
                        direction = -1;
                        break;
                    case winrt::Windows::System::VirtualKey::Down:
                        direction = 1;
                        break;
                    case winrt::Windows::System::VirtualKey::PageUp:
                        direction = -1;
                        pageNavigation = true;
                        break;
                    case winrt::Windows::System::VirtualKey::PageDown:
                        direction = 1;
                        pageNavigation = true;
                        break;
                    case winrt::Windows::System::VirtualKey::Enter:
                        if ( owner->HasFlag(wxTE_PROCESS_ENTER) )
                        {
                            const int selection = owner->GetSelection();
                            wxCommandEvent event(
                                wxEVT_TEXT_ENTER, owner->GetId());
                            owner->InitCommandEventWithItems(
                                event, selection);
                            event.SetInt(selection);
                            event.SetString(owner->GetValue());
                            if ( owner->ProcessCommand(event) )
                                args.Handled(true);
                        }
                        return;
                    default:
                        return;
                }

                // Modified navigation belongs to the TextBox (selection,
                // accessibility and Alt/AltGr handling), never to the list.
                if ( wxWinUIComboHasModifiers() )
                    return;

                int delta = direction;
                if ( pageNavigation )
                {
                    const wxWeakRef<wxWindow> ownerRef(owner);
                    const int pageSize = owner->GetSimpleListPageSize();
                    owner = wxDynamicCast(ownerRef.get(), wxComboBox);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->textCallbackState != textState )
                    {
                        return;
                    }
                    delta *= pageSize;
                }

                // Mark the navigation key before changing selection: the
                // resulting wx event is allowed to destroy the control.
                args.Handled(true);
                owner->NavigateSimpleList(delta);
            });

        // The ListBox now owns the only item containers. Rebuild them using
        // the existing stable IDs, then release the construction-time ComboBox
        // before publishing the composite visual.
        oldCombo.Items().Clear();
        impl->comboBox = nullptr;
        impl->peerItemIds.clear();
        impl->peerItemsValid = false;
        if ( !ApplyItemsToPeer() )
            return false;

        if ( !impl->host.SetContent(root, edit) )
            return false;

        wxChoice * const choiceOwner = choiceState->GetOwner();
        wxComboBox *liveOwner =
            textState->GetOwner<wxComboBox>();
        if ( choiceOwner != expectedOwner || liveOwner != expectedOwner ||
             !liveOwner->m_winui || liveOwner->m_winui.get() != impl ||
             liveOwner->m_winui->simpleRoot != root )
        {
            return false;
        }

        liveOwner->m_winui->host.SetPreferredFocus(edit);
        liveOwner = textState->GetOwner<wxComboBox>();
        if ( liveOwner != expectedOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != impl ||
             liveOwner->m_winui->simpleRoot != root )
        {
            return false;
        }

        // Creation can publish the composite after wxChoice already committed
        // its initial HWND size, so no later DoSetSize() is guaranteed. Run the
        // same constrained peer layout now; otherwise the first snapshot sees
        // ListBox's unconstrained desired viewport and the first resize appears
        // to lose height which never belonged to the wx slot.
        (void)liveOwner->RealizeSimplePeerLayout();
        liveOwner = textState->GetOwner<wxComboBox>();
        return liveOwner == expectedOwner && liveOwner->m_winui &&
               liveOwner->m_winui.get() == impl &&
               liveOwner->m_winui->simpleRoot == root;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI simple ComboBox creation", e);
        return false;
    }
}

bool wxComboBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        int n,
                        const wxString choices[],
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    // The combobox-specific flags would trip wxChoice's style assert; they are
    // handled here, not by the base class.
    const long comboFlags = style & (wxCB_DROPDOWN | wxCB_SIMPLE | wxCB_READONLY);
    // Declare editable construction before wxChoice::Create(). It builds the
    // native peer but now leaves its first SetContent()/Loaded boundary to the
    // derived setup below, so WinAppSDK creates x:Load=False EditableText only
    // after all text/template delegates are authoritative.
    m_editable = (comboFlags & wxCB_READONLY) == 0;
    if ( !wxChoice::Create(parent, id, pos, size, n, choices,
                           style & ~comboFlags, validator, name) )
        return false;

    // Restore them so that HasFlag(wxCB_READONLY) etc. work as under wxMSW.
    m_windowStyle |= comboFlags;

    m_value.clear();
    m_insertionPoint = 0;
    m_selectionStart = 0;
    m_selectionEnd = 0;
    m_allowTextEvents = true;
    m_winui->textCallbackState =
        std::make_shared<wxWinUITextCallbackState>(this);

    const bool useSimplePeer =
        (comboFlags & wxCB_SIMPLE) && !(comboFlags & wxCB_READONLY);
    if ( useSimplePeer )
    {
        // Publishing a new XAML content root can synchronously re-enter wx
        // and delete this control. Never touch its members after that point
        // unless the weak reference confirms that it is still alive.
        const wxWeakRef<wxWindow> self(this);
        const bool simpleCreated = CreateSimplePeer();
        if ( !self )
            return false;

        if ( !simpleCreated )
        {
            CloseTextPeer();
            if ( m_winui )
            {
                m_winui->Close();
                m_winui.reset();
            }
            return false;
        }
    }
    else
    {
      try
      {
        auto& combo = m_winui->comboBox;
        combo.IsEditable(m_editable);

        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            m_winui->textCallbackState;
        // The outer ComboBox.Text dependency property is a template
        // projection, not the editable document. It transiently becomes empty
        // while x:Load materializes or replaces EditableText and can therefore
        // never be authoritative for an editable combo. Observe the exact
        // generation of the realized TextBox in AttachEditPeerHandlers()
        // instead. Read-only combos keep the outer callback so an unexpected
        // peer write can still be restored from the wx model.
        if ( !m_editable )
        {
            m_winui->textChangedCallbackToken =
                combo.RegisterPropertyChangedCallback(
                    MUXC::ComboBox::TextProperty(),
                    [callbackState](
                        MUX::DependencyObject const&,
                        MUX::DependencyProperty const&)
                    {
                        wxComboBox *owner =
                            callbackState->GetOwner<wxComboBox>();
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->textCallbackState !=
                                 callbackState )
                        {
                            return;
                        }

                        owner->OnPeerTextChanged();
                    });
        }

        m_winui->textSubmittedToken = combo.TextSubmitted(
            [callbackState](
                MUXC::ComboBox const&,
                MUXC::ComboBoxTextSubmittedEventArgs const& args)
            {
                wxComboBox * const owner =
                    callbackState->GetOwner<wxComboBox>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->textCallbackState != callbackState )
                {
                    return;
                }

                if ( owner->HasFlag(wxTE_PROCESS_ENTER) )
                {
                    const int selection = owner->GetSelection();
                    wxCommandEvent event(wxEVT_TEXT_ENTER, owner->GetId());
                    owner->InitCommandEventWithItems(event, selection);
                    event.SetInt(selection);
                    event.SetString(owner->GetValue());
                    if ( owner->ProcessCommand(event) )
                        args.Handled(true);
                }
            });

        m_winui->comboLoadedToken = combo.Loaded(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                wxComboBox *owner =
                    callbackState->GetOwner<wxComboBox>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->textCallbackState != callbackState )
                {
                    return;
                }

                // Bind the stable, explicitly-themed editable subtree to its
                // owning root before resolving x:Load. A global theme change
                // can then retire the old edit generation while this subtree
                // is still physically on the old theme.
                if ( !owner->EnsureThemeTransitionBoundary() )
                    return;
                owner = callbackState->GetOwner<wxComboBox>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->textCallbackState != callbackState )
                {
                    return;
                }

                // The shared carrier may materialize this editable peer on a
                // later dispatcher turn. Resolve the x:Load editor first,
                // then replay the accepted selection and document into that
                // exact template generation. Publishing selection before
                // resolution lets EditableText creation reset SelectedIndex;
                // reading outer ComboBox.Text here would likewise import its
                // transient empty template projection into the wx model.
                if ( !owner->WinUIEnsurePeerConsistent() )
                    return;
                owner = callbackState->GetOwner<wxComboBox>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->textCallbackState != callbackState )
                {
                    return;
                }

                if ( !owner->ResolveEditPart() )
                    return;
                owner = callbackState->GetOwner<wxComboBox>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->textCallbackState != callbackState ||
                     !owner->ApplySelectionToPeer() )
                {
                    return;
                }

                owner = callbackState->GetOwner<wxComboBox>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->textCallbackState != callbackState )
                {
                    return;
                }
                owner->ApplyTextToPeer();
                wxComboBox * const liveOwner =
                    callbackState->GetOwner<wxComboBox>();
                if ( liveOwner && liveOwner->m_winui &&
                     liveOwner->m_winui->textCallbackState == callbackState )
                {
                    if ( !liveOwner->ApplyTextSelectionToPeer() )
                        return;
                    wxComboBox * const refreshedOwner =
                        callbackState->GetOwner<wxComboBox>();
                    if ( refreshedOwner && refreshedOwner->m_winui &&
                         refreshedOwner->m_winui->textCallbackState ==
                             callbackState &&
                         refreshedOwner->m_winui->autoCompleter )
                    {
                        refreshedOwner->RefreshAutoComplete();
                    }
                }
            });

        if ( m_editable )
        {
            // Effective Template changes and inherited theme changes are the
            // two production boundaries which can dismantle EditableText.
            // Observe both before LayoutUpdated: by the time layout discovers
            // a new identity, the outgoing TextBox may already have published
            // its transient teardown document.
            m_winui->comboTemplateChangedCallbackToken =
                combo.RegisterPropertyChangedCallback(
                    MUXC::Control::TemplateProperty(),
                    [callbackState](
                        MUX::DependencyObject const& sender,
                        MUX::DependencyProperty const&)
                    {
                        wxComboBox * const owner =
                            callbackState->GetOwner<wxComboBox>();
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->textCallbackState !=
                                 callbackState ||
                             sender.try_as<MUXC::ComboBox>() !=
                                 owner->m_winui->comboBox )
                        {
                            return;
                        }

                        owner->OnPeerTemplateTransition(false);
                    });

        }

        // Theme changes can replace the template TextBox without re-raising
        // Loaded on the outer ComboBox. Identity comparison on layout edges
        // revalidates it with a strict two-pass, non-scheduling budget.
        m_winui->comboLayoutUpdatedToken = combo.LayoutUpdated(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&)
            {
                wxComboBox *owner =
                    callbackState->GetOwner<wxComboBox>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->textCallbackState != callbackState )
                {
                    return;
                }
                owner->OnPeerLayoutUpdated();
            });

        m_winui->dropDownOpenedToken = combo.DropDownOpened(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&)
            {
                if ( wxWinUIIsPhysicalDisconnectPublicationPoisoned() )
                    return;

                wxComboBox * const owner =
                    callbackState->GetOwner<wxComboBox>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->textCallbackState != callbackState )
                {
                    return;
                }

                owner->OnPeerDropDownChanged(true);
            });

        m_winui->dropDownClosedToken = combo.DropDownClosed(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&)
            {
                wxComboBox * const owner =
                    callbackState->GetOwner<wxComboBox>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->textCallbackState != callbackState )
                {
                    return;
                }

                owner->OnPeerDropDownChanged(false);
            });

        if ( m_editable )
        {
            // The shared slot's visual root is intentionally a plain layout
            // element. Hosting the x:Load-backed ComboBox itself makes the
            // slot's common Style/LayoutUpdated observers participate in its
            // first template transaction before it has joined a XamlRoot.
            // The Grid joins the carrier deterministically; the real ComboBox
            // remains both its only child and the slot semantic target.
            MUXC::Grid editableRoot;
            // Seed the current policy before attachment. Loaded binds the
            // owning XamlRoot and replaces Default/System with its resolved
            // Light/Dark value, producing a stable effective-theme pin. The
            // root notification can then invalidate the outgoing editor before
            // changing this property dismantles the child template.
            editableRoot.RequestedTheme(wxWinUIGetCurrentElementTheme());
            editableRoot.Children().Append(combo);
            m_winui->editableRoot = editableRoot;
        }
      }
      catch ( const winrt::hresult_error& e )
      {
          wxWinUILogException("WinUI ComboBox setup", e);
          CloseTextPeer();
          if ( m_winui )
          {
              m_winui->Close();
              m_winui.reset();
          }
          return false;
      }
    }

    if ( !m_winui )
        return false;

    if ( !useSimplePeer && m_editable )
    {
        // wxChoice intentionally deferred this publication. All derived
        // callbacks are authoritative now, so the real ComboBox reaches the
        // carrier for the first time at the correct Loaded/x:Load boundary.
        wxWinUIChoiceImpl * const publishImpl = m_winui.get();
        const MUXC::ComboBox publishCombo = publishImpl->comboBox;
        const MUXC::Grid publishRoot = publishImpl->editableRoot;
        if ( !publishRoot )
            return false;
        const wxWeakRef<wxWindow> publishSelf(this);
        const bool contentSet =
            publishImpl->host.SetContent(publishRoot, publishCombo);
        wxComboBox * const publishOwner =
            wxDynamicCast(publishSelf.get(), wxComboBox);
        if ( !publishOwner || !publishOwner->m_winui ||
             publishOwner->m_winui.get() != publishImpl ||
             publishOwner->m_winui->comboBox != publishCombo ||
             publishOwner->m_winui->editableRoot != publishRoot )
        {
            return false;
        }
        if ( !contentSet )
        {
            publishOwner->CloseTextPeer();
            if ( publishOwner->m_winui &&
                 publishOwner->m_winui.get() == publishImpl )
            {
                publishOwner->m_winui->Close();
                publishOwner->m_winui.reset();
            }
            return false;
        }
    }

    const wxWeakRef<wxWindow> createSelf(this);
    const int sel = FindString(value);
    if ( sel != wxNOT_FOUND )
    {
        wxChoice::SetSelection(sel);
        if ( !createSelf )
            return false;
    }
    if ( HasFlag(wxCB_READONLY) )
    {
        if ( sel != wxNOT_FOUND )
        {
            DoSetValue(GetString(sel), SetValue_NoEvent);
            if ( !createSelf )
                return false;
        }
    }
    else
    {
        DoSetValue(value, SetValue_NoEvent);
        if ( !createSelf )
            return false;
    }
    const bool editResolved = ResolveEditPart();
    if ( !createSelf )
        return false;
    if ( !editResolved )
        return false;

    // The base Choice was initially sized for a one-row ComboBox. Reapply the
    // caller's request after the simple composite exists so a default size
    // reserves the permanently-visible list rows.
    if ( useSimplePeer )
    {
        wxWinUIChoiceImpl * const simpleImpl = m_winui.get();
        const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
            simpleImpl->callbackState;
        const std::shared_ptr<wxWinUITextCallbackState> textState =
            simpleImpl->textCallbackState;
        const std::uint64_t editGeneration = simpleImpl->editGeneration;
        const MUXC::Grid root = simpleImpl->simpleRoot;
        const MUXC::TextBox edit = simpleImpl->editBox;
        const MUXC::ListBox list = simpleImpl->simpleListBox;
        wxComboBox * const expectedOwner = this;
        const auto getExactOwner = [&]() -> wxComboBox *
        {
            wxChoice * const choiceOwner =
                choiceState ? choiceState->GetOwner() : nullptr;
            wxComboBox * const textOwner = textState
                ? textState->GetOwner<wxComboBox>() : nullptr;
            return choiceOwner == expectedOwner &&
                           textOwner == expectedOwner &&
                           textOwner->m_winui &&
                           textOwner->m_winui.get() == simpleImpl &&
                           textOwner->m_winui->callbackState == choiceState &&
                           textOwner->m_winui->textCallbackState == textState &&
                           textOwner->m_winui->editGeneration == editGeneration &&
                           textOwner->m_winui->simpleRoot == root &&
                           textOwner->m_winui->editBox == edit &&
                           textOwner->m_winui->simpleListBox == list
                     ? textOwner
                     : nullptr;
        };

        InvalidateBestSize();
        SetInitialSize(size);
        wxComboBox *liveOwner = getExactOwner();
        if ( !liveOwner )
            return false;

        // Item projection suppressed rendering while the composite root was
        // still off-tree. Materialize the two real rows against the final wx
        // size before Create() reports a usable wxCB_SIMPLE peer.
        liveOwner->RealizeSimplePeerLayout();
        liveOwner = getExactOwner();
        if ( !liveOwner )
            return false;
    }

    return true;
}

bool wxComboBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        const wxArrayString& choices,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    wxCArrayString chs(choices);
    return Create(parent, id, value, pos, size, chs.GetCount(), chs.GetStrings(),
                  style, validator, name);
}

void wxComboBox::Clear()
{
    wxChoice::Clear();
    DoSetValue(wxString(),
               HasFlag(wxCB_READONLY)
                   ? SetValue_NoEvent
                   : SetValue_SendEvent);
}

wxString wxComboBox::GetValue() const
{
    return HasFlag(wxCB_READONLY)
               ? wxChoice::GetStringSelection()
               : m_value;
}

void wxComboBox::SetValue(const wxString& value)
{
    if ( HasFlag(wxCB_READONLY) )
    {
        const int selection = FindString(value);
        if ( selection != wxNOT_FOUND )
            SetSelection(selection);
        return;
    }

    DoSetValue(value, SetValue_SendEvent);
}

void wxComboBox::SetString(unsigned int n, const wxString& value)
{
    const bool wasSelected =
        n < GetCount() &&
        m_itemModel.At(n).id == m_selectedItemId;
    if ( wasSelected && m_winui )
    {
        wxString peerValue =
            wxWinUITextPositionMap::NormalizeNewlines(value);
        if ( m_forceUpper )
            peerValue.MakeUpper();
        ArmPendingTextValue(peerValue, m_selectedItemId);
    }

    // Keep the complete programmatic item/value transaction closed to peer
    // callbacks. wxChoice protects its item projection itself, but a normal
    // editable ComboBox can deliver the resulting Text dependency-property
    // callback in the gap between that projection and DoSetValue() below.
    // In particular, a reentrant SetString() from wxEVT_COMBOBOX must not
    // append a second live-value wxEVT_TEXT after SendSelectionEvent() has
    // emitted its accepted-item snapshot.
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        m_winui ? m_winui->callbackState : nullptr;
    if ( choiceState )
        choiceState->BeginPeerMutation();
    wxScopeGuard mutationGuard = wxMakeGuard([choiceState]()
    {
        if ( choiceState )
            choiceState->EndPeerMutation();
    });
    wxUnusedVar(mutationGuard);

    const wxWeakRef<wxWindow> self(this);
    wxChoice::SetString(n, value);
    if ( !self )
        return;

    if ( wasSelected )
    {
        const int selection = wxChoice::GetSelection();
        if ( selection != wxNOT_FOUND )
        {
            DoSetValue(wxChoice::GetString(selection),
                       SetValue_NoEvent);
            if ( !self )
                return;
            m_insertionPoint = GetLastPosition();
            m_selectionStart = m_selectionEnd = m_insertionPoint;
            if ( !ApplyTextSelectionToPeer() )
                return;
        }
    }
}

void wxComboBox::Popup()
{
    if ( m_winui && m_winui->simpleRoot )
    {
        wxFAIL_MSG(wxT("can't popup the list for simple combo box"));
        return;
    }

    if ( !m_winui || !m_winui->comboBox )
        return;

    if ( WinUICoalescePopupReopen(
             [](wxChoice *owner)
             {
                 static_cast<wxComboBox *>(owner)->Popup();
             }) )
    {
        return;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> callbackState =
        impl->callbackState;
    const MUXC::ComboBox combo = impl->comboBox;
    const std::shared_ptr<wxWinUIComboPopupRetirementState>
        popupRetirementState = impl->popupRetirementState;
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxComboBox *
    {
        wxComboBox * const owner = wxDynamicCast(self.get(), wxComboBox);
        return owner && owner->m_winui && owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState &&
                       owner->m_winui->comboBox == combo
                 ? owner
                 : nullptr;
    };
    const auto getPublishableOwner = [&]() -> wxComboBox *
    {
        wxComboBox * const owner = getLiveOwner();
        return owner && !owner->m_winui->comboTemplateReplayPending &&
                       !owner->m_winui->comboTemplateTransition
                 ? owner
                 : nullptr;
    };
    bool setterAttempted = false;
    try
    {
        // A programmatic Popup() can be called in the same stack as Show(), or
        // while a theme pass is replacing the editable part. Never force the
        // ComboBox template/layout from this public operation: preparation
        // below is debt-free and simply rejects a peer which is not naturally
        // realized yet. A caller may retry Popup() at a later layout edge.
        wxComboBox *live = getPublishableOwner();
        if ( !live )
            return;

        // Do not publish IsDropDownOpen until this peer belongs to an island
        // and its closed template exposes the exact Popup/Child graph. This
        // preparation stores only non-owning identities and creates no gate
        // debt: an immediate Popup() before XamlRoot attachment is a normal
        // no-op, not a process-wide publication fault.
        if ( !wxWinUIPrepareComboPopupOpen(popupRetirementState) )
            return;
        live = getPublishableOwner();
        if ( !live )
        {
            wxWinUICancelPreparedComboPopupOpen(popupRetirementState);
            return;
        }

        // Rendering can run arbitrary application callbacks. Recheck the
        // publication barrier and open generation immediately before touching
        // the native property.
        if ( live->WinUICoalescePopupReopen(
                 [](wxChoice *owner)
                 {
                     static_cast<wxComboBox *>(owner)->Popup();
                 }) )
        {
            wxWinUICancelPreparedComboPopupOpen(popupRetirementState);
            return;
        }
        live = getPublishableOwner();
        if ( !live )
        {
            wxWinUICancelPreparedComboPopupOpen(popupRetirementState);
            return;
        }

        const std::uint64_t generation = live->m_winui->dropDownGeneration;
        const bool wasOpen = combo.IsDropDownOpen();
        live = getPublishableOwner();
        if ( !live )
        {
            wxWinUICancelPreparedComboPopupOpen(popupRetirementState);
            return;
        }
        if ( !wasOpen )
        {
            setterAttempted = true;
            combo.IsDropDownOpen(true);
            live = getPublishableOwner();
            if ( !live )
            {
                wxWinUIAbortPreparedComboPopupOpen(popupRetirementState);
                return;
            }
        }

        if ( !wxWinUIFinishPreparedComboPopupOpen(popupRetirementState) )
        {
            return;
        }
        live = getPublishableOwner();
        if ( !live )
            return;

        // WinUI normally raises DropDownOpened inline, but this is not
        // guaranteed once another native popup HWND has participated in the
        // thread. wxMSW's public Popup() contract is synchronous, so publish
        // the transition ourselves if the peer accepted it without invoking
        // its delegate. A later delegate observes the new logical state and
        // is idempotent.
        const bool isOpen = combo.IsDropDownOpen();
        live = getPublishableOwner();
        if ( live && isOpen )
        {
            // Keep the accepted-vs-current selection state synchronous too.
            // Either native delegate can be deferred independently.
            live->WinUIOnDropDownChanged(true);
            live = getPublishableOwner();
            if ( live &&
                 live->m_winui->dropDownGeneration == generation )
            {
                live->OnPeerDropDownChanged(true);
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        if ( setterAttempted )
            wxWinUIAbortPreparedComboPopupOpen(popupRetirementState);
        else
            wxWinUICancelPreparedComboPopupOpen(popupRetirementState);
        wxWinUILogException("WinUI ComboBox popup", e);
    }
    catch ( ... )
    {
        if ( setterAttempted )
            wxWinUIAbortPreparedComboPopupOpen(popupRetirementState);
        else
            wxWinUICancelPreparedComboPopupOpen(popupRetirementState);
    }
}

void wxComboBox::Dismiss()
{
    if ( m_winui && m_winui->simpleRoot )
    {
        wxFAIL_MSG(wxT("can't dismiss the list for simple combo box"));
        return;
    }

    if ( !m_winui || !m_winui->comboBox )
        return;

    const wxWeakRef<wxWindow> self(this);
    try
    {
        const MUXC::ComboBox combo = m_winui->comboBox;
        const std::uint64_t generation = m_winui->dropDownGeneration;
        const bool wasLogicallyOpen = m_winui->dropDownOpen;
        if ( combo.IsDropDownOpen() )
            combo.IsDropDownOpen(false);

        // DropDownClosed can be deferred (or the peer may already have
        // auto-closed) after a native popup such as the shared tooltip HWND
        // was created. Preserve wxMSW's synchronous Dismiss() contract. The
        // generation check also makes this safe when a close handler
        // re-enters Popup(): that newer transition must win.
        wxComboBox *live = wxDynamicCast(self.get(), wxComboBox);
        if ( live && live->m_winui && !combo.IsDropDownOpen() )
        {
            // Commit Choice's pending selection before CLOSEUP, matching the
            // native CBN_CLOSEUP ordering. The selection event may delete or
            // reopen this control, so revalidate before publishing CLOSEUP.
            live->WinUIOnDropDownChanged(false);
            live = wxDynamicCast(self.get(), wxComboBox);
            if ( live && live->m_winui && wasLogicallyOpen &&
                 live->m_winui->dropDownGeneration == generation )
            {
                live->OnPeerDropDownChanged(false);
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox dismiss", e);
    }
}

void wxComboBox::OnPeerDropDownChanged(bool open)
{
    if ( !m_winui || m_winui->dropDownOpen == open )
        return;

    m_winui->dropDownOpen = open;
    if ( ++m_winui->dropDownGeneration == 0 )
        ++m_winui->dropDownGeneration;

    wxCommandEvent event(
        open ? wxEVT_COMBOBOX_DROPDOWN : wxEVT_COMBOBOX_CLOSEUP,
        GetId());
    event.SetEventObject(this);
    ProcessCommand(event);
}

void wxComboBox::SetSelection(int n)
{
    if ( n < 0 || static_cast<unsigned int>(n) >= GetCount() )
        n = wxNOT_FOUND;

    const wxWeakRef<wxWindow> self(this);
    // Programmatic selection updates the text without sending any event, as
    // under wxMSW. Stage both wx model fields before the first XAML boundary:
    // WinUI can defer the editor's dependency-property callback beyond the
    // native selection-mutation guard, and it must then observe the already
    // accepted value instead of reporting a spurious user text event. The
    // actual peer text is still written only after wxChoice has committed the
    // stable selected ID, so synchronous application re-entry never sees a
    // half-updated public model.
    wxString selectionValue;
    if ( n != wxNOT_FOUND )
    {
        selectionValue = wxWinUITextPositionMap::NormalizeNewlines(
            wxChoice::GetString(n));
        if ( m_forceUpper )
            selectionValue.MakeUpper();
        const bool valueChanged = selectionValue != m_value;
        m_value = selectionValue;
        m_insertionPoint =
            wxWinUITextPositionMap(m_value, false, false).GetLastPosition();
        m_selectionStart = m_selectionEnd = m_insertionPoint;
        if ( valueChanged )
            InvalidateBestSize();
    }

    // SelectedIndex can synchronously project a new editable document, but
    // its dependency-property callback is allowed to arrive after the native
    // mutation guard has unwound. Mark the exact programmatic value before
    // entering wxChoice so that delayed projection is never mistaken for a
    // user edit. Clearing a selection must likewise preserve the existing
    // editable value.
    if ( m_winui && m_editable )
    {
        const std::uint64_t selectedItemId =
            n == wxNOT_FOUND ? 0 : m_itemModel.At(n).id;
        ArmPendingTextValue(
            n == wxNOT_FOUND ? m_value : selectionValue,
            selectedItemId);
    }

    // wxChoice guards SelectedIndex itself, while the editable document is
    // replayed afterwards. Hold one outer guard over both phases so a XAML
    // Text callback dispatched between them cannot publish a transient value
    // (or overwrite the snapshot TEXT event during reentrant selection).
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        m_winui ? m_winui->callbackState : nullptr;
    if ( choiceState )
        choiceState->BeginPeerMutation();
    wxScopeGuard mutationGuard = wxMakeGuard([choiceState]()
    {
        if ( choiceState )
            choiceState->EndPeerMutation();
    });
    wxUnusedVar(mutationGuard);

    wxChoice::SetSelection(n);
    if ( !self )
        return;

    if ( n != wxNOT_FOUND )
    {
        DoSetValue(selectionValue, SetValue_NoEvent);
        if ( !self )
            return;
        m_insertionPoint = GetLastPosition();
        m_selectionStart = m_selectionEnd = m_insertionPoint;
        if ( !ApplyTextSelectionToPeer() )
            return;
        // Setting Text on an editable WinUI ComboBox can clear SelectedIndex.
        // Publish text first and restore the authoritative stable item ID as
        // the final item mutation, matching CB_SETCURSEL under wxMSW. Native
        // item projection can in turn select the entire editable document, so
        // replay the already-committed caret/range once more afterwards.
        if ( !ApplySelectionToPeer() )
            return;
        if ( !self || !ApplyTextSelectionToPeer() )
            return;
    }
    else if ( HasFlag(wxCB_READONLY) )
    {
        DoSetValue(wxString(), SetValue_NoEvent);
    }
}

void wxComboBox::SetSelection(long from, long to)
{
    if ( HasFlag(wxCB_READONLY) )
        return;

    const long len =
        wxWinUITextPositionMap(m_value, false, false).GetLastPosition();
    if ( from == -1 && to == -1 )
    {
        from = 0;
        to = len;
    }
    else
    {
        from = wxWinUIClampComboTextPos(from, len);
        to = to == -1 ? len : wxWinUIClampComboTextPos(to, len);
    }
    if ( to < from )
        wxSwap(from, to);

    m_selectionStart = from;
    m_selectionEnd = to;
    m_insertionPoint = from;
    ApplyTextSelectionToPeer();
}

void wxComboBox::GetSelection(long *from, long *to) const
{
    if ( HasFlag(wxCB_READONLY) )
    {
        if ( from )
            *from = -1;
        if ( to )
            *to = -1;
        return;
    }

    wxComboBox * const self = const_cast<wxComboBox *>(this);
    if ( !self->ReadTextSelectionFromPeer() )
        return;
    if ( from )
        *from = m_selectionStart;
    if ( to )
        *to = m_selectionEnd;
}

bool wxComboBox::IsEditable() const
{
    return !HasFlag(wxCB_READONLY) && m_editable;
}

void wxComboBox::SetEditable(bool editable)
{
    if ( HasFlag(wxCB_READONLY) )
    {
        m_editable = false;
        DismissAutoComplete();
        return;
    }

    m_editable = editable;
    wxComboBox *owner = this;
    if ( !editable )
    {
        wxWinUIChoiceImpl * const impl = m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl ? impl->textCallbackState : nullptr;
        const wxWeakRef<wxWindow> self(this);
        if ( !DismissAutoComplete() )
            return;
        owner = wxDynamicCast(self.get(), wxComboBox);
        if ( !owner || owner->m_editable || !owner->m_winui ||
             owner->m_winui.get() != impl ||
             owner->m_winui->textCallbackState != callbackState )
        {
            return;
        }
    }

    if ( !owner->ResolveEditPart() )
        return;
    if ( owner->m_winui && owner->m_winui->editBox )
    {
        try
        {
            owner->m_winui->editBox.IsReadOnly(!editable);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox editable", e);
        }
    }
}

void wxComboBox::SetMaxLength(unsigned long len)
{
    // EM_LIMITTEXT treats values at and above 64K as the platform maximum.
    m_maxLength = len >= 0xffff ? 0 : len;

    if ( !ResolveEditPart() )
        return;
    if ( !m_winui || !m_winui->editBox )
        return;

    try
    {
        m_winui->editBox.MaxLength(0);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox maximum length", e);
    }
}

void wxComboBox::ForceUpper()
{
    m_forceUpper = true;

    // Preserve the public selection while converting the existing value, as
    // wxTextEntryBase does, then project the policy to the real XAML editor.
    const wxWeakRef<wxWindow> self(this);
    ConvertToUpperCase();
    wxComboBox * const live = wxDynamicCast(self.get(), wxComboBox);
    if ( !live || !live->ResolveEditPart() ||
         !live->m_winui || !live->m_winui->editBox )
    {
        return;
    }

    try
    {
        live->m_winui->editBox.CharacterCasing(
            MUXC::CharacterCasing::Upper);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox character casing", e);
    }
}

bool wxComboBox::DoSetMargins(const wxPoint& margins)
{
    const bool fullySupported = margins.y == -1;
    if ( margins.x == -1 )
        return fullySupported;
    if ( margins.x < 0 )
        return false;

    m_horizontalMargin = margins.x;
    if ( !ResolveEditPart() )
        return false;
    if ( !m_winui || !m_winui->editBox )
        return fullySupported;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    const MUXC::TextBox editBox = impl->editBox;
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxComboBox *
    {
        wxComboBox * const owner = wxDynamicCast(self.get(), wxComboBox);
        return owner && owner->m_winui && owner->m_winui.get() == impl &&
                       owner->m_winui->textCallbackState == callbackState &&
                       owner->m_winui->editBox == editBox
                 ? owner
                 : nullptr;
    };

    try
    {
        MUX::Thickness padding = editBox.Padding();
        wxComboBox *owner = getLiveOwner();
        if ( !owner )
            return false;

        const double horizontal = static_cast<double>(
            owner->ToDIP(wxSize(margins.x, 0)).x);
        padding.Left = horizontal;
        padding.Right = horizontal;
        editBox.Padding(padding);
        owner = getLiveOwner();
        if ( !owner )
            return false;
        owner->InvalidateBestSize();
        return fullySupported;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox margins", e);
        return false;
    }
}

wxPoint wxComboBox::DoGetMargins() const
{
    wxComboBox * const mutableThis = const_cast<wxComboBox *>(this);
    const wxWeakRef<wxWindow> self(mutableThis);
    if ( !mutableThis->ResolveEditPart() )
        return wxPoint(-1, -1);

    wxComboBox *owner = wxDynamicCast(self.get(), wxComboBox);
    if ( !owner || !owner->m_winui || !owner->m_winui->editBox )
        return wxPoint(-1, -1);

    const MUXC::TextBox editBox = owner->m_winui->editBox;
    try
    {
        const double left = editBox.Padding().Left;
        owner = wxDynamicCast(self.get(), wxComboBox);
        if ( !owner || !std::isfinite(left) )
            return wxPoint(-1, -1);

        return wxPoint(
            owner->FromDIP(static_cast<int>(std::lround(left))), -1);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox margins query", e);
        return wxPoint(-1, -1);
    }
}

bool wxComboBox::DoAutoCompleteStrings(const wxArrayString& choices)
{
    wxTextCompleterFixed * const completer = new wxTextCompleterFixed;
    completer->SetCompletions(choices);
    return DoAutoCompleteCustom(completer);
}

bool wxComboBox::DoAutoCompleteFileNames(int flags)
{
    if ( flags != wxFILE && flags != wxDIR )
        return false;
    return DoAutoCompleteCustom(
        new wxWinUIComboFileNameCompleter(flags));
}

bool wxComboBox::DoAutoCompleteCustom(wxTextCompleter *completer)
{
    std::shared_ptr<wxTextCompleter> owned(completer);
    if ( HasFlag(wxCB_READONLY) || !m_winui ||
         (!m_winui->comboBox && !m_winui->simpleListBox) )
    {
        return false;
    }

    if ( !ResolveEditPart() || !m_winui )
        return false;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    const std::uint64_t callbackGeneration =
        callbackState ? callbackState->GetGeneration() : 0;
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxComboBox *
    {
        if ( !callbackState ||
             callbackState->GetGeneration() != callbackGeneration )
        {
            return nullptr;
        }
        wxComboBox * const owner = wxDynamicCast(self.get(), wxComboBox);
        return owner && owner->m_winui && owner->m_winui.get() == impl &&
                       owner->m_winui->textCallbackState == callbackState
                 ? owner
                 : nullptr;
    };

    // Publish first: destroying the old application completer is an arbitrary
    // callback boundary and may delete this ComboBox or install a newer one.
    std::shared_ptr<wxTextCompleter> previous =
        std::move(impl->autoCompleter);
    impl->autoCompleter = std::move(owned);
    ++impl->autoCompleteGeneration;
    previous.reset();

    wxComboBox * const owner = getLiveOwner();
    if ( !owner )
        return true;
    owner->RefreshAutoComplete();
    return true;
}

bool wxComboBox::DismissAutoComplete()
{
    if ( !m_winui )
        return true;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    ++impl->autoCompleteGeneration;
    impl->autoCompleteSelection = wxNOT_FOUND;
    impl->autoCompleteReplayPending = false;
    const MUXC::MenuFlyout flyout = impl->autoCompleteFlyout;
    impl->autoCompleteFlyout = nullptr;
    const std::shared_ptr<wxWinUIFlyoutPopupRetirementState>
        retirementState = impl->autoCompleteRetirementState;
    if ( flyout )
    {
        if ( !retirementState )
            std::terminate();
        impl->autoCompleteRetirementPending = true;
    }
    if ( flyout )
    {
        try
        {
            flyout.Hide();
        }
        catch ( const winrt::hresult_error& )
        {
            wxWinUIFailFlyoutPopupShow(retirementState);
        }
    }

    wxComboBox * const owner = callbackState
        ? callbackState->GetOwner<wxComboBox>()
        : nullptr;
    return owner == this && owner->m_winui &&
           owner->m_winui.get() == impl &&
           owner->m_winui->textCallbackState == callbackState;
}

void wxComboBox::RefreshAutoComplete()
{
    if ( !m_editable )
    {
        DismissAutoComplete();
        return;
    }

    if ( !ResolveEditPart() || !m_winui || !m_winui->editBox )
        return;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    if ( wxWinUIIsPhysicalDisconnectPublicationPoisoned() ||
         !impl->physicalDisconnectGate ||
         impl->physicalDisconnectGate->IsSealedForHostShutdown() ||
         impl->physicalDisconnectGate->IsDegraded() )
    {
        return;
    }
    if ( impl->autoCompleteRetirementPending )
    {
        impl->autoCompleteReplayPending = true;
        return;
    }
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    if ( !callbackState )
        return;

    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const std::shared_ptr<wxTextCompleter> completer =
        impl->autoCompleter;
    const std::uint64_t generation = ++impl->autoCompleteGeneration;
    impl->autoCompleteSelection = wxNOT_FOUND;
    const MUXC::TextBox editBox = impl->editBox;
    const wxString prefix = m_value;
    const long selectionStart = m_selectionStart;
    const long selectionEnd = m_selectionEnd;
    const long insertionPoint = m_insertionPoint;
    const long lastPosition = GetLastPosition();
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxComboBox *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;
        wxComboBox * const owner = wxDynamicCast(self.get(), wxComboBox);
        return owner && owner->m_winui && owner->m_winui.get() == impl &&
                       owner->m_winui->textCallbackState == callbackState &&
                       owner->m_winui->editBox == editBox &&
                       owner->m_winui->autoCompleter == completer &&
                       owner->m_winui->autoCompleteGeneration == generation &&
                       owner->m_value == prefix &&
                       owner->m_selectionStart == selectionStart &&
                       owner->m_selectionEnd == selectionEnd &&
                       owner->m_insertionPoint == insertionPoint
                 ? owner
                 : nullptr;
    };

    // Detach before Hide(): closing a popup can synchronously move focus and
    // dispatch application code which starts a newer refresh.
    MUXC::MenuFlyout oldFlyout = impl->autoCompleteFlyout;
    impl->autoCompleteFlyout = nullptr;
    if ( oldFlyout )
    {
        if ( !impl->autoCompleteRetirementState )
        {
            // A published popup without its causal child would be an
            // invisible self-retention hazard at Close().
            std::terminate();
        }
        impl->autoCompleteRetirementPending = true;
        impl->autoCompleteReplayPending = true;
        try
        {
            oldFlyout.Hide();
        }
        catch ( const winrt::hresult_error& )
        {
            wxWinUIFailFlyoutPopupShow(
                impl->autoCompleteRetirementState);
        }
        // Whether Hide() completed the exact gate inline or asynchronously,
        // its observer owns the sole coalesced replay. Never publish another
        // flyout while the old popup/focus generation is still retiring.
        return;
    }

    // Only complete at the end of a non-empty, unselected prefix, matching
    // the native editable-combo interaction.
    if ( !completer || prefix.empty() ||
         selectionStart != selectionEnd ||
         insertionPoint != lastPosition )
    {
        return;
    }

    // Start()/GetNext() are application callbacks. The shared local keeps the
    // completer alive while exact generation checks protect the wx owner.
    if ( !completer->Start(prefix) || !getLiveOwner() )
        return;

    std::vector<wxString> suggestions;
    const wxString foldedPrefix = prefix.Lower();
    constexpr std::size_t MaximumVisibleSuggestions = 256;
    while ( suggestions.size() < MaximumVisibleSuggestions )
    {
        const wxString candidate = completer->GetNext();
        if ( !getLiveOwner() || candidate.empty() )
            break;
        if ( !candidate.Lower().StartsWith(foldedPrefix) )
            continue;
        if ( std::find(suggestions.begin(), suggestions.end(), candidate) ==
             suggestions.end() )
        {
            suggestions.push_back(candidate);
        }
    }
    if ( !getLiveOwner() || suggestions.empty() )
        return;

    std::shared_ptr<wxWinUIFlyoutPopupRetirementState>
        popupRetirementState;
    try
    {
        if ( !editBox.XamlRoot() || !getLiveOwner() )
            return;

        MUXC::MenuFlyout flyout;
        flyout.Closed(
            [callbackState, callbackGeneration, impl, generation](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&)
            {
                if ( callbackState->GetGeneration() != callbackGeneration )
                    return;

                wxComboBox * const owner =
                    callbackState->GetOwner<wxComboBox>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->textCallbackState != callbackState ||
                     owner->m_winui->autoCompleteGeneration != generation )
                {
                    return;
                }

                // A transient flyout closes when focus leaves the editor
                // (including Tab). Retire its items immediately so neither
                // the production owner nor the real-peer test seam can keep
                // treating a hidden popup as the active suggestion surface.
                owner->m_winui->autoCompleteFlyout = nullptr;
                owner->m_winui->autoCompleteRetirementPending = true;
                owner->m_winui->autoCompleteSelection = wxNOT_FOUND;
                ++owner->m_winui->autoCompleteGeneration;
            });
        for ( const wxString& suggestion : suggestions )
        {
            MUXC::MenuFlyoutItem item;
            item.Text(wxWinUIToHString(suggestion));
            item.Click(
                [callbackState, callbackGeneration, impl, generation,
                 suggestion](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUX::RoutedEventArgs const&)
                {
                    if ( callbackState->GetGeneration() !=
                         callbackGeneration )
                    {
                        return;
                    }
                    wxComboBox *owner =
                        callbackState->GetOwner<wxComboBox>();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != impl ||
                         owner->m_winui->textCallbackState != callbackState ||
                         owner->m_winui->autoCompleteGeneration != generation )
                    {
                        return;
                    }

                    // Mouse/UIA and keyboard acceptance deliberately share
                    // one transaction so value, caret, popup retirement and
                    // the final wxEVT_TEXT have identical ordering.
                    owner->AcceptAutoCompleteSuggestion(
                        suggestion, generation);
                });
            flyout.Items().Append(item);
            if ( !getLiveOwner() )
                return;
        }

        wxComboBox * const owner = getLiveOwner();
        if ( !owner )
            return;

        const MUX::DependencyObject exactItem =
            flyout.Items().GetAt(0).try_as<MUX::DependencyObject>();
        popupRetirementState =
            wxWinUICreateFlyoutPopupRetirementState(
                flyout, editBox.XamlRoot(), exactItem);
        if ( !popupRetirementState ||
             !owner->m_winui->physicalDisconnectGate ||
             !wxWinUIAddCompositePhysicalDisconnectGate(
                 owner->m_winui->physicalDisconnectGate,
                 wxWinUIGetFlyoutPopupRetirementGate(
                     popupRetirementState)) )
        {
            // All hooks were installed before ShowAt(), so no popup graph has
            // escaped yet. A fan-in allocation failure cannot be allowed to
            // drop that state without an owner; fail closed deterministically.
            std::terminate();
        }

        const std::weak_ptr<wxWinUIFlyoutPopupRetirementState>
            weakRetirement = popupRetirementState;
        if ( !wxWinUISetFlyoutPopupRetirementObserver(
                 popupRetirementState,
                 [callbackState, callbackGeneration, impl,
                  weakRetirement]()
                 {
                     if ( callbackState->GetGeneration() !=
                          callbackGeneration )
                     {
                         return;
                     }
                     wxComboBox * const owner =
                         callbackState->GetOwner<wxComboBox>();
                     const auto exactRetirement = weakRetirement.lock();
                     if ( !owner || !owner->m_winui ||
                          owner->m_winui.get() != impl ||
                          owner->m_winui->textCallbackState !=
                              callbackState ||
                          !exactRetirement ||
                          owner->m_winui->autoCompleteRetirementState !=
                              exactRetirement )
                     {
                         return;
                     }

                     owner->m_winui->autoCompleteRetirementState.reset();
                     owner->m_winui->autoCompleteRetirementPending = false;
                     const bool replay =
                         owner->m_winui->autoCompleteReplayPending;
                     owner->m_winui->autoCompleteReplayPending = false;
                     if ( replay )
                         owner->RefreshAutoComplete();
                 }) )
        {
            std::terminate();
        }
        owner->m_winui->autoCompleteFlyout = flyout;
        owner->m_winui->autoCompleteRetirementState =
            popupRetirementState;
        owner->m_winui->autoCompleteRetirementPending = false;
        owner->m_winui->autoCompleteReplayPending = false;
        owner->m_winui->autoCompleteSelection = wxNOT_FOUND;

        const std::shared_ptr<wxWinUIPhysicalDisconnectGate>
            popupGate = wxWinUIGetFlyoutPopupRetirementGate(
                popupRetirementState);
        if ( !popupGate || popupGate->IsDegraded() )
        {
            // Arm/hook failure happened before ShowAt(). Keep its partially
            // installed callbacks owned by the composite, but publish no
            // MenuFlyout graph. The process poison prevents any replacement.
            owner->m_winui->autoCompleteFlyout = nullptr;
            owner->m_winui->autoCompleteRetirementPending = true;
            return;
        }

        MUXCP::FlyoutShowOptions options;
        options.Placement(MUXCP::FlyoutPlacementMode::Bottom);
        options.ShowMode(MUXCP::FlyoutShowMode::Transient);
        flyout.ShowAt(editBox, options);
        wxWinUIResolveFlyoutPopupAfterShow(popupRetirementState);
        if ( popupGate->IsDegraded() )
        {
            if ( wxComboBox * const liveOwner = getLiveOwner() )
                liveOwner->m_winui->autoCompleteRetirementPending = true;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUIFailFlyoutPopupShow(popupRetirementState);
        wxWinUILogException("WinUI ComboBox auto-completion", e);
        if ( wxComboBox * const owner = getLiveOwner() )
        {
            owner->m_winui->autoCompleteFlyout = nullptr;
            owner->m_winui->autoCompleteRetirementPending =
                popupRetirementState != nullptr;
            owner->m_winui->autoCompleteSelection = wxNOT_FOUND;
        }
    }
}

void wxComboBox::NavigateAutoComplete(int delta)
{
    if ( !delta || !m_winui || !m_winui->autoCompleteFlyout ||
         !m_winui->textCallbackState )
    {
        return;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const std::uint64_t generation = impl->autoCompleteGeneration;
    const MUXC::MenuFlyout flyout = impl->autoCompleteFlyout;
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxComboBox *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;
        wxComboBox * const owner = wxDynamicCast(self.get(), wxComboBox);
        return owner && owner->m_winui && owner->m_winui.get() == impl &&
                       owner->m_winui->textCallbackState == callbackState &&
                       owner->m_winui->autoCompleteGeneration == generation &&
                       owner->m_winui->autoCompleteFlyout == flyout
                 ? owner
                 : nullptr;
    };

    try
    {
        const auto items = flyout.Items();
        const int count = static_cast<int>(items.Size());
        if ( count <= 0 || !getLiveOwner() )
            return;

        const int previous = impl->autoCompleteSelection;
        int selected;
        if ( previous < 0 || previous >= count )
            selected = delta > 0 ? 0 : count - 1;
        else
            selected = wxMax(0, wxMin(count - 1, previous + delta));

        // Publish before touching XAML: every property call is treated as a
        // callback boundary, so a reentrant key/click observes one coherent
        // active index and a later generation always wins.
        impl->autoCompleteSelection = selected;
        if ( previous >= 0 && previous < count && previous != selected )
        {
            const MUXC::MenuFlyoutItem previousItem =
                items.GetAt(static_cast<unsigned>(previous)).
                    try_as<MUXC::MenuFlyoutItem>();
            if ( previousItem )
                previousItem.FontWeight(MUXT::FontWeights::Normal());
            if ( !getLiveOwner() )
                return;
        }

        const MUXC::MenuFlyoutItem selectedItem =
            items.GetAt(static_cast<unsigned>(selected)).
                try_as<MUXC::MenuFlyoutItem>();
        if ( !selectedItem )
        {
            if ( wxComboBox * const owner = getLiveOwner() )
                owner->m_winui->autoCompleteSelection = wxNOT_FOUND;
            return;
        }

        selectedItem.FontWeight(MUXT::FontWeights::SemiBold());
        if ( !getLiveOwner() )
            return;

        // MenuFlyout has no selected-item contract of its own. Keep the
        // visibly active suggestion inside its transient scroll viewport.
        selectedItem.StartBringIntoView();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox auto-complete navigation", e);
        if ( wxComboBox * const owner = getLiveOwner() )
            owner->m_winui->autoCompleteSelection = wxNOT_FOUND;
    }
}

void wxComboBox::AcceptAutoCompleteSelection()
{
    if ( !m_editable || !m_winui || !m_winui->autoCompleteFlyout ||
         !m_winui->textCallbackState )
    {
        return;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    const std::uint64_t generation = impl->autoCompleteGeneration;
    const MUXC::MenuFlyout flyout = impl->autoCompleteFlyout;
    const int selected = impl->autoCompleteSelection;
    if ( selected < 0 )
        return;

    try
    {
        const auto items = flyout.Items();
        if ( static_cast<unsigned>(selected) >= items.Size() )
            return;
        const MUXC::MenuFlyoutItem item =
            items.GetAt(static_cast<unsigned>(selected)).
                try_as<MUXC::MenuFlyoutItem>();
        if ( !item )
            return;
        const wxString suggestion = wxWinUIFromHString(item.Text());

        wxComboBox * const owner =
            callbackState->GetOwner<wxComboBox>();
        if ( owner == this && owner->m_winui &&
             owner->m_winui.get() == impl &&
             owner->m_winui->textCallbackState == callbackState &&
             owner->m_winui->autoCompleteGeneration == generation &&
             owner->m_winui->autoCompleteFlyout == flyout &&
             owner->m_winui->autoCompleteSelection == selected )
        {
            owner->AcceptAutoCompleteSuggestion(suggestion, generation);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox auto-complete acceptance", e);
    }
}

void wxComboBox::AcceptAutoCompleteSuggestion(
    const wxString& suggestion,
    std::uint64_t generation)
{
    if ( !m_editable || !m_winui || !m_winui->autoCompleteFlyout ||
         m_winui->autoCompleteGeneration != generation ||
         !m_winui->textCallbackState )
    {
        return;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const std::uint64_t retiredGeneration = generation + 1;
    wxString accepted =
        wxWinUITextPositionMap::NormalizeNewlines(suggestion);
    if ( m_forceUpper )
        accepted.MakeUpper();
    const wxString previousValue = m_value;
    const wxWinUIComboConstrainedEdit constrainedEdit =
        wxWinUIConstrainComboPeerEdit(
            previousValue, accepted, m_maxLength);
    accepted = constrainedEdit.value;
    const bool valueChanged = accepted != previousValue;

    const wxWeakRef<wxWindow> self(this);
    DoSetValue(accepted, SetValue_NoEvent);

    wxComboBox *owner = wxDynamicCast(self.get(), wxComboBox);
    if ( !owner || callbackState->GetGeneration() != callbackGeneration ||
         !owner->m_winui || owner->m_winui.get() != impl ||
         owner->m_winui->textCallbackState != callbackState ||
         owner->m_winui->autoCompleteGeneration != retiredGeneration ||
         owner->m_winui->autoCompleteFlyout ||
         owner->m_value != accepted )
    {
        return;
    }

    owner->SetInsertionPointEnd();
    owner = wxDynamicCast(self.get(), wxComboBox);
    if ( !owner || callbackState->GetGeneration() != callbackGeneration ||
         !owner->m_winui || owner->m_winui.get() != impl ||
         owner->m_winui->textCallbackState != callbackState ||
         owner->m_winui->autoCompleteGeneration != retiredGeneration ||
         owner->m_winui->autoCompleteFlyout ||
         owner->m_value != accepted )
    {
        return;
    }

    // The accepted text and end caret are observable before notification,
    // matching the native edit transaction. Overflow is reported after the
    // accepted text event, exactly as for a peer typing/paste transaction.
    if ( valueChanged )
    {
        owner->SendTextEvent();
        owner = wxDynamicCast(self.get(), wxComboBox);
        if ( !owner || callbackState->GetGeneration() != callbackGeneration ||
             !owner->m_winui || owner->m_winui.get() != impl ||
             owner->m_winui->textCallbackState != callbackState ||
             owner->m_winui->autoCompleteGeneration != retiredGeneration ||
             owner->m_winui->autoCompleteFlyout ||
             owner->m_value != accepted )
        {
            return;
        }
    }
    if ( constrainedEdit.wasTruncated )
        owner->SendMaxLengthEvent();
}

bool wxComboBox::SetHint(const wxString& hint)
{
    m_hint = hint;

    if ( !m_winui || (!m_winui->comboBox && !m_winui->editBox) )
        return true;

    try
    {
        // Do not use the MSW EM_SETCUEBANNER fallback on the HWND shell:
        // wxTextEntryHintData temporarily writes the hint as the value, which
        // is observable through our XAML-backed model. ComboBox owns a native
        // placeholder dependency property, so keep value and hint disjoint.
        if ( m_winui->simpleListBox )
            m_winui->editBox.PlaceholderText(wxWinUIToHString(hint));
        else
            m_winui->comboBox.PlaceholderText(wxWinUIToHString(hint));
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox hint", e);
        return false;
    }
}

wxString wxComboBox::GetHint() const
{
    return m_hint;
}

void wxComboBox::WriteText(const wxString& text)
{
    if ( !m_editable )
        return;

    if ( !ReadTextSelectionFromPeer() )
        return;
    long from = m_selectionStart;
    long to = m_selectionEnd;
    if ( from > to )
        wxSwap(from, to);
    if ( from == to )
        from = to = m_insertionPoint;

    wxString value;
    long insertionPoint = from;
    const wxWinUITextPositionMap map(m_value, false, false);
    const wxString replacement = m_forceUpper ? text.Upper() : text;
    if ( !map.ReplaceRange(
             from, to, replacement, &value, &insertionPoint) )
        return;

    const bool valueChanged = value != m_value;
    m_value = value;
    if ( valueChanged )
        InvalidateBestSize();
    m_insertionPoint = insertionPoint;
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    const wxWeakRef<wxWindow> self(this);
    ApplyTextToPeer();
    if ( !self || !ApplyTextSelectionToPeer() )
        return;
    SendTextEvent();
}

void wxComboBox::Remove(long from, long to)
{
    if ( !m_editable )
        return;

    const wxWinUITextPositionMap map(m_value, false, false);
    const long len = map.GetLastPosition();
    from = wxWinUIClampComboTextPos(from, len);
    to = to < 0 ? len : wxWinUIClampComboTextPos(to, len);
    if ( to < from )
        wxSwap(from, to);

    wxString value;
    long insertionPoint = from;
    if ( !map.ReplaceRange(
             from, to, wxString(), &value, &insertionPoint) )
    {
        return;
    }

    const bool valueChanged = value != m_value;
    m_value = value;
    if ( valueChanged )
        InvalidateBestSize();
    m_insertionPoint = insertionPoint;
    m_selectionStart = m_selectionEnd = insertionPoint;
    const wxWeakRef<wxWindow> self(this);
    ApplyTextToPeer();
    if ( !self || !ApplyTextSelectionToPeer() )
        return;
    SendTextEvent();
}

bool wxComboBox::SendClipboardEvent(wxEventType type)
{
    wxClipboardTextEvent event(type, GetId());
    event.SetEventObject(this);
    return HandleWindowEvent(event);
}

void wxComboBox::Copy()
{
    if ( !ReadTextSelectionFromPeer() ||
         m_selectionStart == m_selectionEnd )
        return;

    wxWinUIChoiceImpl * const eventImpl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> eventState =
        eventImpl ? eventImpl->textCallbackState : nullptr;
    const std::uint64_t eventEditGeneration =
        eventImpl ? eventImpl->editGeneration : 0;
    const MUXC::TextBox eventEdit =
        eventImpl ? eventImpl->editBox : nullptr;
    const wxWeakRef<wxWindow> self(this);
    if ( SendClipboardEvent(wxEVT_TEXT_COPY) )
        return;

    wxComboBox *live = wxDynamicCast(self.get(), wxComboBox);
    if ( !live || live->m_winui.get() != eventImpl ||
         (eventState &&
          (live->m_winui->textCallbackState != eventState ||
           live->m_winui->editGeneration != eventEditGeneration ||
           live->m_winui->editBox != eventEdit)) )
    {
        // Application code replaced/reparented the exact command sender.
        // Never continue a native clipboard operation on its successor.
        return;
    }
    if ( !live || !live->ResolveEditPart() )
        return;

    live = wxDynamicCast(self.get(), wxComboBox);
    if ( !live )
        return;
    if ( live->m_winui && live->m_winui->editBox )
    {
        wxWinUIChoiceImpl * const impl = live->m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->textCallbackState;
        const MUXC::TextBox editBox = impl->editBox;
        const std::uint64_t editGeneration = impl->editGeneration;
        const bool wasInProgress = impl->clipboardOperationInProgress;
        impl->clipboardOperationInProgress = true;
        wxScopeGuard operationGuard = wxMakeGuard(
            [callbackState, impl, editBox, editGeneration, wasInProgress]()
            {
                wxComboBox * const owner = callbackState
                    ? callbackState->GetOwner<wxComboBox>()
                    : nullptr;
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->textCallbackState == callbackState &&
                     owner->m_winui->editGeneration == editGeneration &&
                     owner->m_winui->editBox == editBox )
                {
                    owner->m_winui->clipboardOperationInProgress =
                        wasInProgress;
                }
            });
        wxUnusedVar(operationGuard);

        try
        {
            editBox.CopySelectionToClipboard();
            return;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox copy", e);
        }
        return;
    }

#if wxUSE_CLIPBOARD
    if ( !wxTheClipboard )
        return;
    const wxString selected =
        wxWinUITextPositionMap(live->m_value, false, false).
            GetRange(live->m_selectionStart, live->m_selectionEnd);
    if ( wxTheClipboard->Open() )
    {
        wxTheClipboard->SetData(new wxTextDataObject(selected));
        wxTheClipboard->Close();
    }
#endif // wxUSE_CLIPBOARD
}

void wxComboBox::Cut()
{
    if ( !m_editable || !ReadTextSelectionFromPeer() ||
         m_selectionStart == m_selectionEnd )
        return;

    wxWinUIChoiceImpl * const eventImpl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> eventState =
        eventImpl ? eventImpl->textCallbackState : nullptr;
    const std::uint64_t eventEditGeneration =
        eventImpl ? eventImpl->editGeneration : 0;
    const MUXC::TextBox eventEdit =
        eventImpl ? eventImpl->editBox : nullptr;
    const wxWeakRef<wxWindow> self(this);
    if ( SendClipboardEvent(wxEVT_TEXT_CUT) )
        return;

    wxComboBox *live = wxDynamicCast(self.get(), wxComboBox);
    if ( !live || live->m_winui.get() != eventImpl ||
         (eventState &&
          (live->m_winui->textCallbackState != eventState ||
           live->m_winui->editGeneration != eventEditGeneration ||
           live->m_winui->editBox != eventEdit)) )
    {
        return;
    }
    if ( !live || !live->ResolveEditPart() )
        return;

    live = wxDynamicCast(self.get(), wxComboBox);
    if ( !live )
        return;
    if ( live->m_winui && live->m_winui->editBox )
    {
        wxWinUIChoiceImpl * const impl = live->m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->textCallbackState;
        const MUXC::TextBox editBox = impl->editBox;
        const std::uint64_t editGeneration = impl->editGeneration;
        const bool wasInProgress = impl->clipboardOperationInProgress;
        impl->clipboardOperationInProgress = true;
        wxScopeGuard operationGuard = wxMakeGuard(
            [callbackState, impl, editBox, editGeneration, wasInProgress]()
            {
                wxComboBox * const owner = callbackState
                    ? callbackState->GetOwner<wxComboBox>()
                    : nullptr;
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->textCallbackState == callbackState &&
                     owner->m_winui->editGeneration == editGeneration &&
                     owner->m_winui->editBox == editBox )
                {
                    owner->m_winui->clipboardOperationInProgress =
                        wasInProgress;
                }
            });
        wxUnusedVar(operationGuard);

        try
        {
            editBox.CutSelectionToClipboard();
            return;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox cut", e);
        }
        return;
    }

#if wxUSE_CLIPBOARD
    const long selectionStart = live->m_selectionStart;
    const long selectionEnd = live->m_selectionEnd;
    const long from = wxMin(live->m_selectionStart, live->m_selectionEnd);
    const long to = wxMax(live->m_selectionStart, live->m_selectionEnd);
    const wxString value = live->m_value;
    wxWinUIChoiceImpl * const impl =
        live->m_winui ? live->m_winui.get() : nullptr;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl ? impl->textCallbackState : nullptr;
    const std::uint64_t callbackGeneration =
        callbackState ? callbackState->GetGeneration() : 0;
    const std::uint64_t editGeneration =
        impl ? impl->editGeneration : 0;
    const auto getExactOwner = [&]() -> wxComboBox *
    {
        wxComboBox * const owner = wxDynamicCast(self.get(), wxComboBox);
        if ( !owner || owner->m_winui.get() != impl ||
             owner->m_value != value ||
             owner->m_selectionStart != selectionStart ||
             owner->m_selectionEnd != selectionEnd )
        {
            return nullptr;
        }
        if ( callbackState &&
             (callbackState->GetGeneration() != callbackGeneration ||
              owner->m_winui->textCallbackState != callbackState ||
              owner->m_winui->editGeneration != editGeneration) )
        {
            return nullptr;
        }
        return owner;
    };
    const wxString selected =
        wxWinUITextPositionMap(value, false, false).GetRange(from, to);
    if ( wxTheClipboard && wxTheClipboard->Open() )
    {
        const bool copied =
            wxTheClipboard->SetData(new wxTextDataObject(selected));
        wxTheClipboard->Close();
        live = getExactOwner();
        if ( copied && live )
            live->Remove(from, to);
    }
#else
    live->RemoveSelection();
#endif // wxUSE_CLIPBOARD
}

void wxComboBox::Paste()
{
    if ( !m_editable )
        return;

    wxWinUIChoiceImpl * const eventImpl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> eventState =
        eventImpl ? eventImpl->textCallbackState : nullptr;
    const std::uint64_t eventEditGeneration =
        eventImpl ? eventImpl->editGeneration : 0;
    const MUXC::TextBox eventEdit =
        eventImpl ? eventImpl->editBox : nullptr;
    const wxWeakRef<wxWindow> self(this);
    if ( SendClipboardEvent(wxEVT_TEXT_PASTE) )
        return;

    wxComboBox *live = wxDynamicCast(self.get(), wxComboBox);
    if ( !live || live->m_winui.get() != eventImpl ||
         (eventState &&
          (live->m_winui->textCallbackState != eventState ||
           live->m_winui->editGeneration != eventEditGeneration ||
           live->m_winui->editBox != eventEdit)) )
    {
        return;
    }
    if ( !live || !live->ResolveEditPart() )
        return;

    live = wxDynamicCast(self.get(), wxComboBox);
    if ( !live )
        return;
    if ( live->m_winui && live->m_winui->editBox )
    {
        wxWinUIChoiceImpl * const impl = live->m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->textCallbackState;
        const MUXC::TextBox editBox = impl->editBox;
        const std::uint64_t editGeneration = impl->editGeneration;
        const bool wasInProgress = impl->clipboardOperationInProgress;
        impl->clipboardOperationInProgress = true;
        wxScopeGuard operationGuard = wxMakeGuard(
            [callbackState, impl, editBox, editGeneration, wasInProgress]()
            {
                wxComboBox * const owner = callbackState
                    ? callbackState->GetOwner<wxComboBox>()
                    : nullptr;
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->textCallbackState == callbackState &&
                     owner->m_winui->editGeneration == editGeneration &&
                     owner->m_winui->editBox == editBox )
                {
                    owner->m_winui->clipboardOperationInProgress =
                        wasInProgress;
                }
            });
        wxUnusedVar(operationGuard);

        try
        {
            editBox.PasteFromClipboard();
            return;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox paste", e);
        }
        return;
    }

#if wxUSE_CLIPBOARD
    if ( !wxTheClipboard )
        return;

    // Snapshot every control-owned value before opening the OLE clipboard:
    // delayed-rendering IDataObject implementations may synchronously enter
    // application code from Open()/GetData().
    const wxString value = live->m_value;
    const long selectionStart = live->m_selectionStart;
    const long selectionEnd = live->m_selectionEnd;
    wxWinUIChoiceImpl * const impl =
        live->m_winui ? live->m_winui.get() : nullptr;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl ? impl->textCallbackState : nullptr;
    const std::uint64_t callbackGeneration =
        callbackState ? callbackState->GetGeneration() : 0;
    const std::uint64_t editGeneration =
        impl ? impl->editGeneration : 0;
    const auto getExactOwner = [&]() -> wxComboBox *
    {
        wxComboBox * const owner = wxDynamicCast(self.get(), wxComboBox);
        if ( !owner || owner->m_winui.get() != impl ||
             owner->m_value != value ||
             owner->m_selectionStart != selectionStart ||
             owner->m_selectionEnd != selectionEnd )
        {
            return nullptr;
        }
        if ( callbackState &&
             (callbackState->GetGeneration() != callbackGeneration ||
              owner->m_winui->textCallbackState != callbackState ||
              owner->m_winui->editGeneration != editGeneration) )
        {
            return nullptr;
        }
        return owner;
    };

    if ( !wxTheClipboard->Open() )
        return;

    wxString text;
    if ( wxTheClipboard->IsSupported(wxDF_TEXT) )
    {
        wxTextDataObject data;
        if ( wxTheClipboard->GetData(data) )
            text = data.GetText();
    }
    wxTheClipboard->Close();
    live = getExactOwner();
    if ( live && !text.empty() )
        live->WriteText(text);
#endif // wxUSE_CLIPBOARD
}

void wxComboBox::Undo()
{
    if ( !m_editable )
        return;

    if ( !ResolveEditPart() )
        return;
    if ( m_winui && m_winui->editBox )
    {
        try
        {
            m_winui->editBox.Undo();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox undo", e);
        }
    }
}

void wxComboBox::Redo()
{
    if ( !m_editable )
        return;

    if ( !ResolveEditPart() )
        return;
    if ( m_winui && m_winui->editBox )
    {
        try
        {
            m_winui->editBox.Redo();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox redo", e);
        }
    }
}

bool wxComboBox::CanUndo() const
{
    if ( !m_editable )
        return false;

    wxComboBox * const self = const_cast<wxComboBox *>(this);
    if ( !self->ResolveEditPart() )
        return false;
    if ( m_winui && m_winui->editBox )
    {
        try
        {
            return m_winui->editBox.CanUndo();
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    return false;
}

bool wxComboBox::CanRedo() const
{
    if ( !m_editable )
        return false;

    wxComboBox * const self = const_cast<wxComboBox *>(this);
    if ( !self->ResolveEditPart() )
        return false;
    if ( m_winui && m_winui->editBox )
    {
        try
        {
            return m_winui->editBox.CanRedo();
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    return false;
}

void wxComboBox::SetInsertionPoint(long pos)
{
    if ( HasFlag(wxCB_READONLY) )
        return;

    const long len =
        wxWinUITextPositionMap(m_value, false, false).GetLastPosition();
    m_insertionPoint = wxWinUIClampComboTextPos(pos, len);
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    ApplyTextSelectionToPeer();
}

long wxComboBox::GetInsertionPoint() const
{
    if ( HasFlag(wxCB_READONLY) )
        return -1;

    if ( !const_cast<wxComboBox *>(this)->ReadTextSelectionFromPeer() )
        return -1;
    return m_insertionPoint;
}

long wxComboBox::GetLastPosition() const
{
    return wxWinUITextPositionMap(
               GetValue(), false, false).GetLastPosition();
}

void wxComboBox::Command(wxCommandEvent& event)
{
    const wxWeakRef<wxWindow> self(this);
    if ( event.GetInt() != wxNOT_FOUND )
    {
        SetSelection(event.GetInt());
        if ( !self )
            return;
    }
    ProcessCommand(event);
}

wxSize wxComboBox::DoGetBestSize() const
{
    wxSize size = wxChoice::DoGetBestSize();

    wxCoord valueWidth = 0;
    wxCoord valueHeight = 0;
    GetTextExtent(m_value, &valueWidth, &valueHeight);

    // Keep the established WinUI editable-combo minimum, but never clip a
    // value which is wider than it. wxChoice already accounts for the widest
    // item and the dropdown chrome.
    const wxSize chrome = FromDIP(wxSize(48, 14));
    const wxSize minimum = FromDIP(wxSize(180, 32));
    size.x = wxMax(size.x, wxMax(valueWidth + chrome.x, minimum.x));
    size.y = wxMax(size.y, wxMax(valueHeight + chrome.y, minimum.y));

    if ( m_winui && m_winui->simpleRoot )
    {
        // Match wxMSW's CBS_SIMPLE sizing policy: the list always reserves
        // between three and six visible rows, independent of popup metrics.
        const int rows = wxMin(wxMax(static_cast<int>(GetCount()), 3), 6);
        const int editHeight =
            wxMax(valueHeight + FromDIP(14), FromDIP(32));
        int itemContentHeight = GetCharHeight();
        for ( unsigned int n = 0; n < GetCount(); ++n )
        {
            const wxBitmap bitmap = WinUIGetItemBitmap(n);
            if ( bitmap.IsOk() )
                itemContentHeight = wxMax(itemContentHeight,
                                          bitmap.GetHeight());
        }
        const int itemHeight =
            wxMax(itemContentHeight + FromDIP(8), FromDIP(24));
        size.y = editHeight + rows * itemHeight;
    }
    return size;
}

void wxComboBox::DoSetSize(int x, int y, int width, int height,
                           int sizeFlags)
{
    // Publish the ordinary wx geometry first. For wxCB_SIMPLE, immediately
    // measure and arrange its two-row XAML surface against that current slot:
    // merely waiting for the shared host's coalesced geometry pass leaves
    // PageUp/PageDown observing the previous viewport for an entire loop.
    const wxWeakRef<wxWindow> self(this);
    wxChoice::DoSetSize(x, y, width, height, sizeFlags);
    wxComboBox * const owner = wxDynamicCast(self.get(), wxComboBox);
    if ( owner && owner->m_winui && owner->m_winui->simpleRoot )
        owner->RealizeSimplePeerLayout();
}

wxString wxComboBox::DoGetValue() const
{
    return GetValue();
}

void wxComboBox::DoSetValue(const wxString& value, int flags)
{
    wxString normalized =
        wxWinUITextPositionMap::NormalizeNewlines(value);
    if ( m_forceUpper )
        normalized.MakeUpper();
    const bool valueChanged = normalized != m_value;
    if ( valueChanged )
        m_insertionPoint = 0;
    else
        ClampTextState();

    m_value = normalized;
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    if ( valueChanged )
        InvalidateBestSize();

    if ( !DismissAutoComplete() )
        return;

    const wxWeakRef<wxWindow> self(this);
    ApplyTextToPeer();
    if ( !self || !ApplyTextSelectionToPeer() )
        return;

    if ( flags & SetValue_SendEvent )
        SendTextEvent();
}

void wxComboBox::ApplyTextToPeer()
{
    if ( !m_winui ||
         (!m_winui->comboBox &&
          !(m_winui->simpleListBox && m_winui->editBox)) )
        return;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    choiceState->BeginPeerMutation();
    wxScopeGuard mutationGuard = wxMakeGuard([choiceState]()
    {
        choiceState->EndPeerMutation();
    });
    wxUnusedVar(mutationGuard);

    bool setTextAttempted = false;
    bool setTextCompleted = false;
    try
    {
        const wxString comboValue = impl->comboBox
            ? wxWinUITextPositionMap::NormalizeNewlines(
                  wxWinUIFromHString(impl->comboBox.Text()))
            : m_value;
        const wxString editValue = impl->editBox
            ? wxWinUITextPositionMap::NormalizeNewlines(
                  wxWinUIFromHString(impl->editBox.Text()))
            : m_value;
        const int selected = wxChoice::GetSelection();
        const bool synchronizeOuter = impl->comboBox &&
            (selected == wxNOT_FOUND ||
             wxWinUITextPositionMap::NormalizeNewlines(
                 wxChoice::GetString(selected)) == m_value);
        // Readback equality doesn't prove that the property callback already
        // ran: WinUI may defer it until the surrounding selection/template
        // transaction unwinds. Only the exact callback consumes the marker.
        if ( (!synchronizeOuter || comboValue == m_value) &&
             editValue == m_value )
            return;

        ArmPendingTextValue(m_value, m_selectedItemId);
        setTextAttempted = true;
        // ComboBox.Text remains a template projection, but leaving the selected
        // item's outer value stale lets a later layout overwrite the inner
        // document. A custom editable value can intentionally differ from the
        // selected item, so keep the outer projection untouched in that case.
        if ( synchronizeOuter && comboValue != m_value )
            impl->comboBox.Text(wxWinUIToHString(m_value));
        if ( synchronizeOuter && selected != wxNOT_FOUND &&
             impl->comboBox.SelectedIndex() != selected )
        {
            // Setting Text can transiently clear SelectedIndex. Restore the
            // stable item before the final inner-document write.
            impl->comboBox.SelectedIndex(selected);
        }
        if ( impl->editBox && editValue != m_value )
            impl->editBox.Text(wxWinUIToHString(m_value));
        setTextCompleted = true;
    }
    catch ( const winrt::hresult_error& e )
    {
        if ( setTextAttempted && !setTextCompleted &&
             m_winui && m_winui.get() == impl )
        {
            ClearPendingTextValue();
        }
        wxWinUILogException("WinUI ComboBox text", e);
    }
}

void wxComboBox::OnPeerTextChanged()
{
    if ( !m_winui ||
         (!m_winui->comboBox &&
          !(m_winui->simpleListBox && m_winui->editBox)) )
        return;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    wxString text;
    try
    {
        const winrt::hstring peerText =
            impl->editBox ? impl->editBox.Text()
                          : impl->comboBox.Text();
        text = wxWinUITextPositionMap::NormalizeNewlines(
            wxWinUIFromHString(peerText));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox peer text read", e);
        return;
    }

    ++impl->diagnosticPeerTextCallbacks;
    impl->diagnosticLastPeerText = text;

    const bool peerMutationInProgress =
        impl->callbackState->IsPeerMutationInProgress();
    if ( impl->hasPendingTextValue && impl->pendingTextValue == text )
    {
        // TextProperty is not the end of a ComboBox selection projection: the
        // framework can still publish an older outer value later in the same
        // layout transaction. Keep the exact writer authoritative until the
        // current editor raises its real post-mutation LayoutUpdated edge.
        return;
    }

    // A dependency-property callback can run synchronously in the middle of a
    // compound native mutation. A transient value from that mutation must not
    // consume the pending authoritative document: a later exact callback (or
    // the next peer reconciliation) still has to recognize it.
    if ( peerMutationInProgress )
        return;

    if ( impl->hasPendingTextValue )
    {
        // TextProperty notifications carry no value or mutation generation.
        // A callback queued for the old item can therefore run after a
        // re-entrant handler has committed a newer selection/text writer. Keep
        // the newer marker while all independently observable authorities --
        // wx document, edit generation, stable model ID and peer ID -- still
        // attest it. The exact callback consumes it above; template teardown,
        // a superseding peer writer, or a genuinely different peer selection
        // invalidates it explicitly.
        bool pendingStillAuthoritative =
            impl->pendingTextEditGeneration == impl->editGeneration &&
            impl->pendingTextItemId == m_selectedItemId &&
            impl->pendingTextValue == m_value;

        if ( pendingStillAuthoritative )
        {
            try
            {
                const int peerSelection =
                    impl->simpleListBox
                        ? impl->simpleListBox.SelectedIndex()
                        : impl->comboBox.SelectedIndex();
                if ( peerSelection >= 0 )
                {
                    const std::size_t peerIndex =
                        static_cast<std::size_t>(peerSelection);
                    if ( !impl->peerItemsValid ||
                         peerIndex >= impl->peerItemIds.size() )
                    {
                        pendingStillAuthoritative = false;
                    }
                    else
                    {
                        const std::uint64_t peerItemId =
                            impl->peerItemIds[peerIndex];
                        pendingStillAuthoritative =
                            peerItemId != 0 &&
                            peerItemId == impl->pendingTextItemId;
                    }
                }
                else if ( impl->pendingTextItemId != 0 )
                {
                    // Editable ComboBox.Text may transiently clear
                    // SelectedIndex while projecting the same model writer.
                    // The exact text callback remains authoritative in that
                    // state and ApplySelectionToPeer() will restore the ID.
                    pendingStillAuthoritative = true;
                }
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI ComboBox pending text authority", e);
                pendingStillAuthoritative = false;
            }
        }

        if ( pendingStillAuthoritative )
            return;

        ClearPendingTextValue();
    }

    if ( text == m_value )
    {
        if ( SuppressPendingTextSelectionChange() )
            return;
        ReadTextSelectionFromPeer();
        return;
    }

    // A genuine document mutation supersedes a template-transition range
    // ticket. Its native caret belongs to the new document and must not be
    // replaced later by the old projection correction.
    ClearPendingTextSelection();

    // A realized WinUI ComboBox can project the newly selected item's text
    // through TextProperty before raising SelectionChanged.  wxMSW's contract
    // is nevertheless one ordered COMBOBOX -> TEXT pair, and
    // SendSelectionEvent() owns that pair (including its re-entrancy-safe
    // client-data lookup). Recognize only a valid stable peer ID which has not
    // yet been published by SelectionChanged; ordinary edits keep the already
    // published selection ID and continue through the text path below. This
    // identity test also works for bitmap-item templates whose projected
    // content is not a plain string.
    try
    {
        const int peerSelection =
            impl->simpleListBox ? impl->simpleListBox.SelectedIndex()
                                : impl->comboBox.SelectedIndex();
        if ( peerSelection >= 0 && impl->peerItemsValid )
        {
            const std::size_t peerIndex =
                static_cast<std::size_t>(peerSelection);
            if ( peerIndex < impl->peerItemIds.size() )
            {
                const wxWinUIItemModel::Id selectedId =
                    impl->peerItemIds[peerIndex];
                if ( selectedId != 0 &&
                     selectedId != m_selectedItemId &&
                     m_itemModel.IndexOf(selectedId) !=
                         wxWinUIItemModel::npos )
                {
                    return;
                }
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        // A transient peer read must not lose a genuine text edit.
        wxWinUILogException(
            "WinUI ComboBox pending selection text read", e);
    }

    if ( !m_editable )
    {
        ApplyTextToPeer();
        return;
    }

    const wxString previousValue = m_value;
    const wxWinUIComboConstrainedEdit constrainedEdit =
        wxWinUIConstrainComboPeerEdit(
            previousValue, text, m_maxLength);
    const wxString acceptedValue = constrainedEdit.value;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    const std::uint64_t editGeneration = impl->editGeneration;
    const MUXC::TextBox editBox = impl->editBox;
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxComboBox *
    {
        wxComboBox * const owner = wxDynamicCast(self.get(), wxComboBox);
        return owner && owner->m_winui && owner->m_winui.get() == impl &&
                       owner->m_winui->textCallbackState == callbackState &&
                       owner->m_winui->editGeneration == editGeneration &&
                       owner->m_winui->editBox == editBox
                 ? owner
                 : nullptr;
    };

    // Read the native selection in the proposed document, remap it around
    // any rejected UTF-16 units, and only then publish the accepted model.
    m_value = text;
    m_insertionPoint =
        wxWinUITextPositionMap(text, false, false).GetLastPosition();
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    if ( !ReadTextSelectionFromPeer() )
    {
        if ( wxComboBox * const owner = getLiveOwner() )
            owner->m_value = previousValue;
        return;
    }
    m_selectionStart = constrainedEdit.RemapProposedPosition(
        m_selectionStart);
    m_selectionEnd = constrainedEdit.RemapProposedPosition(
        m_selectionEnd);
    m_insertionPoint = constrainedEdit.RemapProposedPosition(
        m_insertionPoint);
    m_value = acceptedValue;
    ClampTextState();

    const bool valueChanged = acceptedValue != previousValue;
    if ( valueChanged )
        InvalidateBestSize();

    wxComboBox *owner = this;
    if ( constrainedEdit.wasTruncated )
    {
        ApplyTextToPeer();
        owner = getLiveOwner();
        if ( !owner || !owner->ApplyTextSelectionToPeer() )
            return;
        owner = getLiveOwner();
        if ( !owner )
            return;
    }

    if ( valueChanged )
    {
        ++impl->diagnosticPeerTextDeliveries;
        owner->SendTextEvent();
        owner = getLiveOwner();
        if ( !owner )
            return;
    }

    if ( constrainedEdit.wasTruncated )
    {
        owner->SendMaxLengthEvent();
        owner = getLiveOwner();
        if ( !owner )
            return;
    }

    if ( valueChanged )
        owner->RefreshAutoComplete();
}

bool wxComboBox::ApplyTextSelectionToPeer()
{
    if ( HasFlag(wxCB_READONLY) )
        return true;

    if ( !ResolveEditPart() )
        return false;
    if ( !m_winui || !m_winui->editBox )
        return true;

    ClampTextState();
    const long from = wxMin(m_selectionStart, m_selectionEnd);
    const long to = wxMax(m_selectionStart, m_selectionEnd);
    wxWinUIChoiceImpl * const impl = m_winui.get();
    if ( impl->hasPendingTextSelection &&
         !impl->comboTemplateTransition )
    {
        // A later public text/range writer supersedes any post-template
        // correction which has not reached its dispatcher boundary yet.
        ClearPendingTextSelection();
    }
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    const MUXC::TextBox editBox = impl->editBox;
    wxComboBox * const expectedOwner = this;
    choiceState->BeginPeerMutation();
    wxScopeGuard mutationGuard = wxMakeGuard([choiceState]()
    {
        choiceState->EndPeerMutation();
    });
    wxUnusedVar(mutationGuard);

    try
    {
        editBox.Select(
            static_cast<int32_t>(from),
            static_cast<int32_t>(to - from));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox text selection", e);
    }

    wxChoice * const choiceOwner = choiceState->GetOwner();
    wxComboBox * const textOwner =
        textState ? textState->GetOwner<wxComboBox>() : nullptr;
    return choiceOwner == expectedOwner && textOwner == expectedOwner &&
           textOwner->m_winui && textOwner->m_winui.get() == impl &&
           textOwner->m_winui->callbackState == choiceState &&
           textOwner->m_winui->textCallbackState == textState &&
           textOwner->m_winui->editBox == editBox;
}

bool wxComboBox::ReadTextSelectionFromPeer()
{
    if ( HasFlag(wxCB_READONLY) )
        return true;

    // Do this before ResolveEditPart(): resolving may force the template to
    // project a range. A public getter must remain purely observational while
    // the exact replacement-editor ticket is pending.
    if ( SuppressPendingTextSelectionChange() )
        return true;

    if ( !ResolveEditPart() )
        return false;
    if ( !m_winui || !m_winui->editBox )
        return true;

    // While a replacement editor still owns a projection ticket, getters
    // must expose the authoritative wx snapshot.
    // Reading the peer here would import WinUI's transient SelectAll before
    // the generation-scoped continuation has replayed the exact range.
    // ResolveEditPart() may itself have discovered a new editor transition,
    // so recheck before reading any peer dependency property.
    if ( SuppressPendingTextSelectionChange() )
        return true;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    const MUXC::TextBox editBox = impl->editBox;
    wxComboBox * const expectedOwner = this;
    try
    {
        const long selectionStart = editBox.SelectionStart();
        const long selectionLength = editBox.SelectionLength();
        wxComboBox * const owner =
            callbackState
                ? callbackState->GetOwner<wxComboBox>()
                : nullptr;
        if ( owner != expectedOwner || !owner->m_winui ||
             owner->m_winui.get() != impl ||
             owner->m_winui->textCallbackState != callbackState ||
             owner->m_winui->editBox != editBox )
        {
            return false;
        }

        owner->m_selectionStart = selectionStart;
        owner->m_selectionEnd = selectionStart + selectionLength;
        owner->m_insertionPoint = selectionStart;
        owner->ClampTextState();
    }
    catch ( const winrt::hresult_error& )
    {
    }

    wxComboBox * const owner =
        callbackState
            ? callbackState->GetOwner<wxComboBox>()
            : nullptr;
    return owner == expectedOwner && owner->m_winui &&
           owner->m_winui.get() == impl &&
           owner->m_winui->textCallbackState == callbackState &&
           owner->m_winui->editBox == editBox;
}

void wxComboBox::QueueSimplePeerLayoutAtPeerEdge()
{
    if ( !m_winui || !m_winui->simpleRoot || !m_winui->editBox ||
         !m_winui->simpleListBox || !m_winui->callbackState ||
         !m_winui->textCallbackState )
    {
        return;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    if ( impl->simpleLayoutInProgress )
        return;

    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    const std::uint64_t editGeneration = impl->editGeneration;
    const MUXC::Grid root = impl->simpleRoot;
    const MUXC::TextBox edit = impl->editBox;
    const MUXC::ListBox list = impl->simpleListBox;
    const std::uintptr_t rootIdentity =
        reinterpret_cast<std::uintptr_t>(winrt::get_abi(
            wxWinUIComboObjectIdentity(root)));
    const std::uintptr_t editIdentity =
        reinterpret_cast<std::uintptr_t>(winrt::get_abi(
            wxWinUIComboObjectIdentity(edit)));
    const std::uintptr_t listIdentity =
        reinterpret_cast<std::uintptr_t>(winrt::get_abi(
            wxWinUIComboObjectIdentity(list)));
    wxComboBox * const expectedOwner = this;
    const auto getExactOwner = [&]() -> wxComboBox *
    {
        wxChoice * const choiceOwner = choiceState->GetOwner();
        wxComboBox * const textOwner =
            textState->GetOwner<wxComboBox>();
        if ( choiceOwner != expectedOwner || textOwner != expectedOwner ||
             !textOwner->m_winui || textOwner->m_winui.get() != impl )
        {
            return nullptr;
        }
        wxWinUIChoiceImpl * const liveImpl = textOwner->m_winui.get();
        const auto identity = [](const auto& object)
        {
            return reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                wxWinUIComboObjectIdentity(object)));
        };
        return liveImpl->callbackState == choiceState &&
                       liveImpl->textCallbackState == textState &&
                       liveImpl->editGeneration == editGeneration &&
                       identity(liveImpl->simpleRoot) == rootIdentity &&
                       identity(liveImpl->editBox) == editIdentity &&
                       identity(liveImpl->simpleListBox) == listIdentity
                 ? textOwner
                 : nullptr;
    };
    if ( !getExactOwner() )
        return;

    std::uint64_t revision = 0;
    const auto clearExactRevision = [&]()
    {
        wxComboBox * const owner = getExactOwner();
        if ( owner && revision &&
             owner->m_winui->simpleLayoutRevision == revision )
        {
            owner->m_winui->simpleLayoutQueued = false;
        }
    };

    try
    {
        // LayoutUpdated is useful only once this exact composite is attached
        // and the editor's Auto row has a real intrinsic extent.  All reads
        // in this callback are passive: no template/layout operation occurs
        // until the dispatcher continuation below.
        const wxSize sourcePixels = GetSize();
        wxComboBox *owner = getExactOwner();
        if ( !owner )
            return;
        const MUX::XamlRoot xamlRoot = root.XamlRoot();
        owner = getExactOwner();
        if ( !owner )
            return;
        const double editHeight = edit.ActualHeight();
        owner = getExactOwner();
        if ( !owner )
            return;
        const double scale = xamlRoot
            ? xamlRoot.RasterizationScale()
            : 0.0;
        owner = getExactOwner();
        if ( !owner )
            return;
        const bool rootLoaded = root.IsLoaded();
        owner = getExactOwner();
        if ( !owner )
            return;
        const bool editLoaded = edit.IsLoaded();
        owner = getExactOwner();
        if ( !owner )
            return;
        const bool listLoaded = list.IsLoaded();
        owner = getExactOwner();
        if ( !owner )
            return;
        if ( !rootLoaded || !editLoaded || !listLoaded ||
             !xamlRoot || !std::isfinite(editHeight) || editHeight <= 0.0 ||
             !std::isfinite(scale) || scale <= 0.0 ||
             sourcePixels.x <= 0 || sourcePixels.y <= 0 )
        {
            return;
        }

        const double requestedWidth = sourcePixels.x / scale;
        const double requestedHeight = sourcePixels.y / scale;
        const double listExtent = wxMax(0.0, requestedHeight - editHeight);
        const auto rows = root.RowDefinitions();
        if ( rows.Size() != 2 )
            return;
        const MUX::GridLength rowHeight = rows.GetAt(1).Height();
        owner = getExactOwner();
        if ( !owner )
            return;
        constexpr double tolerance = 0.5;
        const auto differs = [tolerance](double left, double right)
        {
            return !std::isfinite(left) || !std::isfinite(right) ||
                   std::abs(left - right) > tolerance;
        };
        const auto scaleDiffers = [](double left, double right)
        {
            return !std::isfinite(left) || !std::isfinite(right) ||
                   std::abs(left - right) > 0.000001;
        };
        const bool stale =
            impl->simpleLayoutAppliedEpoch != impl->simpleLayoutEpoch ||
            rowHeight.GridUnitType != MUX::GridUnitType::Pixel ||
            differs(rowHeight.Value, listExtent) ||
            differs(root.Width(), requestedWidth) ||
            differs(root.Height(), requestedHeight) ||
            differs(root.ActualHeight(), requestedHeight) ||
            differs(list.Height(), listExtent) ||
            differs(list.MaxHeight(), listExtent) ||
            differs(list.ActualHeight(), listExtent);
        if ( !stale )
            return;

        // A queued request owns the exact physical slot, XamlRoot scale and
        // realized editor extent observed on its causal LayoutUpdated edge.
        // A different edge supersedes it with revision N+1; N can neither
        // clear N+1 nor retarget itself to the newer geometry.
        if ( impl->simpleLayoutQueued &&
             impl->simpleLayoutQueuedWidthPixels == sourcePixels.x &&
             impl->simpleLayoutQueuedHeightPixels == sourcePixels.y &&
             !scaleDiffers(impl->simpleLayoutQueuedScale, scale) &&
             !differs(impl->simpleLayoutQueuedEditHeight, editHeight) )
        {
            return;
        }

        revision = ++impl->simpleLayoutRevision;
        if ( revision == 0 )
            revision = ++impl->simpleLayoutRevision;
        impl->simpleLayoutQueued = true;
        impl->simpleLayoutQueuedWidthPixels = sourcePixels.x;
        impl->simpleLayoutQueuedHeightPixels = sourcePixels.y;
        impl->simpleLayoutQueuedScale = scale;
        impl->simpleLayoutQueuedEditHeight = editHeight;

        const MUXD::DispatcherQueue queue = edit.DispatcherQueue();
        owner = getExactOwner();
        if ( !owner || owner->m_winui->simpleLayoutRevision != revision )
            return;
        if ( !queue )
        {
            clearExactRevision();
            return;
        }

        const std::weak_ptr<wxWinUIChoiceCallbackState> weakChoiceState =
            choiceState;
        const std::weak_ptr<wxWinUITextCallbackState> weakTextState =
            textState;
        const bool queued = queue.TryEnqueue(
            MUXD::DispatcherQueuePriority::Low,
            [weakChoiceState, weakTextState, editGeneration, revision,
             rootIdentity, editIdentity, listIdentity,
             sourcePixels, scale, editHeight]()
            {
                const auto queuedChoiceState = weakChoiceState.lock();
                const auto queuedTextState = weakTextState.lock();
                if ( !queuedChoiceState || !queuedTextState )
                    return;

                wxChoice * const choiceOwner =
                    queuedChoiceState->GetOwner();
                wxComboBox * const owner =
                    queuedTextState->GetOwner<wxComboBox>();
                if ( !owner || choiceOwner != owner || !owner->m_winui )
                    return;

                wxWinUIChoiceImpl * const queuedImpl =
                    owner->m_winui.get();
                const auto identity = [](const auto& object)
                {
                    return reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                        wxWinUIComboObjectIdentity(object)));
                };
                if ( queuedImpl->callbackState != queuedChoiceState ||
                     queuedImpl->textCallbackState != queuedTextState ||
                     queuedImpl->editGeneration != editGeneration ||
                     !queuedImpl->simpleLayoutQueued ||
                     queuedImpl->simpleLayoutRevision != revision ||
                     identity(queuedImpl->simpleRoot) != rootIdentity ||
                     identity(queuedImpl->editBox) != editIdentity ||
                     identity(queuedImpl->simpleListBox) != listIdentity )
                {
                    return;
                }
                const auto getQueuedOwner = [&]() -> wxComboBox *
                {
                    wxChoice * const liveChoiceOwner =
                        queuedChoiceState->GetOwner();
                    wxComboBox * const liveOwner =
                        queuedTextState->GetOwner<wxComboBox>();
                    if ( !liveOwner || liveChoiceOwner != liveOwner ||
                         !liveOwner->m_winui )
                    {
                        return nullptr;
                    }
                    wxWinUIChoiceImpl * const liveImpl =
                        liveOwner->m_winui.get();
                    return liveImpl->callbackState == queuedChoiceState &&
                                   liveImpl->textCallbackState ==
                                       queuedTextState &&
                                   liveImpl->editGeneration ==
                                       editGeneration &&
                                   liveImpl->simpleLayoutQueued &&
                                   liveImpl->simpleLayoutRevision == revision &&
                                   identity(liveImpl->simpleRoot) ==
                                       rootIdentity &&
                                   identity(liveImpl->editBox) ==
                                       editIdentity &&
                                   identity(liveImpl->simpleListBox) ==
                                       listIdentity
                             ? liveOwner
                             : nullptr;
                };

                // Revalidate the ticket's complete authority before any
                // writer.  These are passive reads on the same UI thread; a
                // newer LayoutUpdated request has already advanced revision.
                try
                {
                    wxComboBox *liveOwner = getQueuedOwner();
                    if ( !liveOwner )
                        return;
                    const wxSize currentPixels = liveOwner->GetSize();
                    liveOwner = getQueuedOwner();
                    if ( !liveOwner )
                        return;
                    const MUXC::Grid currentSimpleRoot =
                        liveOwner->m_winui->simpleRoot;
                    const MUX::XamlRoot currentRoot =
                        currentSimpleRoot.XamlRoot();
                    liveOwner = getQueuedOwner();
                    if ( !liveOwner )
                        return;
                    const double currentScale = currentRoot
                        ? currentRoot.RasterizationScale()
                        : 0.0;
                    liveOwner = getQueuedOwner();
                    if ( !liveOwner )
                        return;
                    const MUXC::TextBox currentEdit =
                        liveOwner->m_winui->editBox;
                    const double currentEditHeight =
                        currentEdit.ActualHeight();
                    liveOwner = getQueuedOwner();
                    if ( !liveOwner )
                        return;
                    constexpr double tolerance = 0.5;
                    if ( currentPixels != sourcePixels ||
                         !std::isfinite(currentScale) ||
                         std::abs(currentScale - scale) > 0.000001 ||
                         !std::isfinite(currentEditHeight) ||
                         std::abs(currentEditHeight - editHeight) >
                             tolerance )
                    {
                        liveOwner->m_winui->simpleLayoutQueued = false;
                        return;
                    }
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxComboBox * const liveOwner = getQueuedOwner();
                    if ( liveOwner )
                        liveOwner->m_winui->simpleLayoutQueued = false;
                    wxWinUILogException(
                        "WinUI simple ComboBox queued authority", e);
                    return;
                }

                wxComboBox * const liveOwner = getQueuedOwner();
                if ( !liveOwner )
                    return;
                liveOwner->m_winui->simpleLayoutQueued = false;
                liveOwner->RealizeSimplePeerLayout();
            });
        if ( !queued )
            clearExactRevision();
    }
    catch ( const winrt::hresult_error& e )
    {
        clearExactRevision();
        wxWinUILogException(
            "WinUI simple ComboBox layout-edge queue", e);
    }
}

bool wxComboBox::RealizeSimplePeerLayout()
{
    if ( !m_winui || !m_winui->simpleRoot || !m_winui->editBox ||
         !m_winui->simpleListBox || !m_winui->callbackState ||
         !m_winui->textCallbackState )
    {
        return false;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    ++impl->diagnosticSimpleRealizeAttempts;
    std::uint64_t layoutEpoch = ++impl->simpleLayoutEpoch;
    if ( layoutEpoch == 0 )
        layoutEpoch = ++impl->simpleLayoutEpoch;
    if ( impl->simpleLayoutInProgress )
    {
        // A re-entrant DoSetSize() publishes newer wx geometry. Invalidate
        // the outer writer and ask it to queue one current-size replay when
        // it unwinds; the nested call itself must never race the XAML tree.
        impl->simpleLayoutReplayPending = true;
        return false;
    }
    impl->simpleLayoutInProgress = true;
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    const std::uint64_t editGeneration = impl->editGeneration;
    const MUXC::Grid root = impl->simpleRoot;
    const MUXC::TextBox edit = impl->editBox;
    const MUXC::ListBox list = impl->simpleListBox;
    wxComboBox * const expectedOwner = this;
    const auto getExactOwner = [&]() -> wxComboBox *
    {
        wxChoice * const choiceOwner = choiceState->GetOwner();
        wxComboBox * const textOwner =
            textState->GetOwner<wxComboBox>();
        return choiceOwner == expectedOwner && textOwner == expectedOwner &&
                       textOwner->m_winui &&
                       textOwner->m_winui.get() == impl &&
                       textOwner->m_winui->callbackState == choiceState &&
                       textOwner->m_winui->textCallbackState == textState &&
                       textOwner->m_winui->editGeneration == editGeneration &&
                       textOwner->m_winui->simpleRoot == root &&
                       textOwner->m_winui->editBox == edit &&
                       textOwner->m_winui->simpleListBox == list &&
                       textOwner->m_winui->simpleLayoutEpoch == layoutEpoch
                 ? textOwner
                 : nullptr;
    };
    wxScopeGuard layoutGuard = wxMakeGuard(
        [choiceState, textState, impl]()
        {
            wxChoice * const choiceOwner = choiceState->GetOwner();
            wxComboBox * const owner = textState->GetOwner<wxComboBox>();
            if ( !owner || choiceOwner != owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != choiceState ||
                 owner->m_winui->textCallbackState != textState )
            {
                return;
            }

            const bool replay = impl->simpleLayoutReplayPending;
            impl->simpleLayoutReplayPending = false;
            impl->simpleLayoutInProgress = false;
            if ( replay )
                owner->QueueSimplePeerLayoutAtPeerEdge();
        });
    wxUnusedVar(layoutGuard);

    wxComboBox *owner = getExactOwner();
    if ( !owner )
        return false;

    // Sample the physical wx slot before any XAML call can re-enter sizing.
    // The shared host projects the same pixels through the attached
    // XamlRoot scale, so this pair is the stable geometry authority.
    const wxSize clientPixels = owner->GetSize();
    owner = getExactOwner();
    if ( !owner )
        return false;

    try
    {
        const MUX::XamlRoot attachedRoot = root.XamlRoot();
        const auto editDesiredBefore = edit.DesiredSize();
        const double editActualBefore = edit.ActualHeight();
        const bool editExtentKnown =
            (std::isfinite(editActualBefore) && editActualBefore > 0.0) ||
            (std::isfinite(editDesiredBefore.Height) &&
             editDesiredBefore.Height > 0.0f);
        if ( !root.IsLoaded() || !edit.IsLoaded() || !list.IsLoaded() ||
             !attachedRoot || !editExtentKnown ||
             clientPixels.x <= 0 || clientPixels.y <= 0 )
        {
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI simple ComboBox pre-layout observation", e);
        return false;
    }

    // This runs outside LayoutUpdated (the edge only queued us), so forcing
    // the shared slot to publish its current rectangle can't recurse into the
    // routed callback body. Epoch validation makes a nested resize win.
    owner = getExactOwner();
    if ( !owner )
        return false;
    owner->m_winui->host.ForceRender();
    owner = getExactOwner();
    if ( !owner )
        return false;

    try
    {
        const MUX::XamlRoot attachedRoot = root.XamlRoot();
        const double scale = attachedRoot
            ? attachedRoot.RasterizationScale()
            : 0.0;
        if ( !root.IsLoaded() || !edit.IsLoaded() || !list.IsLoaded() ||
             !attachedRoot || !std::isfinite(scale) || scale <= 0.0 )
        {
            return false;
        }
        const double requestedWidth = clientPixels.x / scale;
        const double requestedHeight = clientPixels.y / scale;
        if ( !std::isfinite(requestedWidth) || requestedWidth <= 0.0 ||
             !std::isfinite(requestedHeight) || requestedHeight <= 0.0 )
        {
            return false;
        }

        owner = getExactOwner();
        if ( !owner )
            return false;

        impl->diagnosticSimpleSourceWidthPixels = clientPixels.x;
        impl->diagnosticSimpleSourceHeightPixels = clientPixels.y;
        impl->diagnosticSimpleRasterizationScale = scale;
        impl->diagnosticSimpleComputedWidth = requestedWidth;
        impl->diagnosticSimpleComputedHeight = requestedHeight;
        impl->diagnosticSimpleRequestedWidth =
            static_cast<int>(std::lround(requestedWidth));
        impl->diagnosticSimpleRequestedHeight =
            static_cast<int>(std::lround(requestedHeight));

        edit.ApplyTemplate();
        owner = getExactOwner();
        if ( !owner )
            return false;
        list.ApplyTemplate();
        owner = getExactOwner();
        if ( !owner )
            return false;

        const float inf = std::numeric_limits<float>::infinity();
        root.Measure({ inf, inf });
        owner = getExactOwner();
        if ( !owner )
            return false;

        const float width = static_cast<float>(wxMin(
            requestedWidth,
            static_cast<double>(std::numeric_limits<float>::max())));
        const float height = static_cast<float>(wxMin(
            requestedHeight,
            static_cast<double>(std::numeric_limits<float>::max())));

        // wxCB_SIMPLE exposes the requested wx height as a real two-row
        // viewport contract. The RowDefinition, rather than a suggested child
        // Height inside a Star row, must own the remaining extent.
        root.Width(width);
        owner = getExactOwner();
        if ( !owner )
            return false;
        root.Height(height);
        owner = getExactOwner();
        if ( !owner )
            return false;

        const auto rows = root.RowDefinitions();
        if ( rows.Size() != 2 )
            return false;
        const MUXC::RowDefinition listRow = rows.GetAt(1);
        owner = getExactOwner();
        if ( !owner )
            return false;

        // A Pixel Grid row owns the cell, but XAML is allowed to retain an
        // unclipped child DesiredSize larger than that cell. Reset the prior
        // local extent before publishing the new row and direct child bounds.
        list.ClearValue(MUX::FrameworkElement::HeightProperty());
        owner = getExactOwner();
        if ( !owner )
            return false;

        // Measure the Auto editor row explicitly at the final composite
        // width. During the first realization Grid can report a valid root
        // DesiredSize while its TextBox child still reports zero, which made
        // the initial list consume the whole slot and the same size restore
        // to a smaller viewport once the editor had been realized.
        edit.Measure({ width, inf });
        owner = getExactOwner();
        if ( !owner )
            return false;
        const auto editDesired = edit.DesiredSize();
        const double desiredEditHeight =
            std::isfinite(editDesired.Height) && editDesired.Height > 0.0f
                ? static_cast<double>(editDesired.Height)
                : 0.0;
        const double desiredListHeight = wxMax(
            0.0, static_cast<double>(height) - desiredEditHeight);
        listRow.Height(
            MUX::GridLengthHelper::FromPixels(desiredListHeight));
        owner = getExactOwner();
        if ( !owner )
            return false;
        list.MinHeight(0.0);
        owner = getExactOwner();
        if ( !owner )
            return false;
        list.MaxHeight(desiredListHeight);
        owner = getExactOwner();
        if ( !owner )
            return false;
        list.Height(desiredListHeight);
        owner = getExactOwner();
        if ( !owner )
            return false;

        root.Measure({ width, height });
        owner = getExactOwner();
        if ( !owner )
            return false;
        root.Arrange({ 0.0f, 0.0f, width, height });
        owner = getExactOwner();
        if ( !owner )
            return false;
        root.UpdateLayout();
        owner = getExactOwner();
        if ( !owner )
            return false;
        list.UpdateLayout();
        owner = getExactOwner();
        if ( !owner )
            return false;

        // ActualHeight is the final authority after the first constrained
        // pass. Always run one second pass from that realized Auto-row extent:
        // even when it numerically matches DesiredSize, the first layout can
        // have completed a deferred TextBox template during UpdateLayout().
        const double realizedEditHeight = edit.ActualHeight();
        const double finalEditHeight =
            std::isfinite(realizedEditHeight) && realizedEditHeight > 0.0
                ? realizedEditHeight
                : desiredEditHeight;
        const double finalListHeight = wxMax(
            0.0, static_cast<double>(height) - finalEditHeight);
        list.ClearValue(MUX::FrameworkElement::HeightProperty());
        owner = getExactOwner();
        if ( !owner )
            return false;
        listRow.Height(MUX::GridLengthHelper::FromPixels(finalListHeight));
        owner = getExactOwner();
        if ( !owner )
            return false;
        list.MinHeight(0.0);
        owner = getExactOwner();
        if ( !owner )
            return false;
        list.MaxHeight(finalListHeight);
        owner = getExactOwner();
        if ( !owner )
            return false;
        list.Height(finalListHeight);
        owner = getExactOwner();
        if ( !owner )
            return false;
        root.InvalidateMeasure();
        root.InvalidateArrange();
        owner = getExactOwner();
        if ( !owner )
            return false;
        root.Measure({ width, height });
        owner = getExactOwner();
        if ( !owner )
            return false;
        root.Arrange({ 0.0f, 0.0f, width, height });
        owner = getExactOwner();
        if ( !owner )
            return false;
        root.UpdateLayout();
        owner = getExactOwner();
        if ( !owner )
            return false;
        list.UpdateLayout();
        owner = getExactOwner();
        if ( !owner )
            return false;

        const MUX::XamlRoot rootXaml = root.XamlRoot();
        const MUX::XamlRoot editXaml = edit.XamlRoot();
        const MUX::XamlRoot listXaml = list.XamlRoot();
        const double editWidth = edit.ActualWidth();
        const double editHeight = edit.ActualHeight();
        const double listWidth = list.ActualWidth();
        const double listHeight = list.ActualHeight();
        owner = getExactOwner();
        if ( !owner )
            return false;

        wxUnusedVar(editWidth);
        wxUnusedVar(editHeight);
        wxUnusedVar(listWidth);
        impl->diagnosticSimpleArrangedHeight = root.ActualHeight();
        impl->diagnosticSimpleListHeight = listHeight;
        const MUXC::ScrollViewer scroll = wxWinUIFindScrollViewer(list);
        impl->diagnosticSimpleViewportHeight =
            scroll ? scroll.ActualHeight() : 0.0;
        const bool sharesRoot = rootXaml && editXaml && listXaml &&
            wxWinUIComboObjectIdentity(rootXaml) ==
                wxWinUIComboObjectIdentity(editXaml) &&
            wxWinUIComboObjectIdentity(rootXaml) ==
                wxWinUIComboObjectIdentity(listXaml);
        if ( sharesRoot )
            impl->simpleLayoutAppliedEpoch = layoutEpoch;
        return sharesRoot;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI simple ComboBox peer realization", e);
        return false;
    }
}

bool wxComboBox::ResolveEditPart(bool updateLayout)
{
    if ( m_winui && m_winui->simpleRoot )
    {
        wxWinUIChoiceImpl * const impl = m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->textCallbackState;
        const std::uint64_t editGeneration = impl->editGeneration;
        const MUXC::Grid root = impl->simpleRoot;
        const MUXC::TextBox edit = impl->editBox;
        const MUXC::ListBox list = impl->simpleListBox;
        wxComboBox *owner = callbackState
            ? callbackState->GetOwner<wxComboBox>()
            : nullptr;
        const auto isExact = [&]()
        {
            return owner == this && owner->m_winui &&
                   owner->m_winui.get() == impl &&
                   owner->m_winui->textCallbackState == callbackState &&
                   owner->m_winui->editGeneration == editGeneration &&
                   owner->m_winui->simpleRoot == root &&
                   owner->m_winui->editBox == edit && edit &&
                   owner->m_winui->simpleListBox == list && list;
        };
        if ( !isExact() )
            return false;

        impl->host.SetPreferredFocus(edit);
        owner = callbackState->GetOwner<wxComboBox>();
        return isExact();
    }

    if ( !m_winui || !m_winui->comboBox )
        return false;

    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        m_winui->textCallbackState;
    wxWinUIChoiceImpl * const impl = m_winui.get();
    if ( !callbackState )
        return false;

    if ( impl->comboPartResolving )
    {
        impl->comboPartPending = true;
        return true;
    }

    impl->comboPartResolving = true;
    bool queuePassiveFollowup = false;
    // ResolveEditPartOnce() calls into XAML. A template/layout/property
    // callback is allowed to destroy the wx control synchronously, so the
    // guard must never retain and later dereference the raw impl pointer. Any
    // residual request is published only after this resolving flag is clear,
    // through the ordinary weak, revisioned Low-priority mechanism.
    wxScopeGuard resolvingGuard = wxMakeGuard(
        [callbackState, impl, &queuePassiveFollowup]()
    {
        wxComboBox * const owner =
            callbackState->GetOwner<wxComboBox>();
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == impl &&
             owner->m_winui->textCallbackState == callbackState )
        {
            owner->m_winui->comboPartResolving = false;
            if ( queuePassiveFollowup )
            {
                // This can be the unwinding edge of ResolveEditPart() inside
                // SetSelection()'s outer peer-mutation guard. Calling the
                // public LayoutUpdated handler would be rejected there and
                // strand comboPartPending. Queue the already-latched residual
                // directly; this path only performs passive identity reads
                // and publishes the existing weak, revisioned Low ticket.
                owner->QueueEditPartResolutionAtLayoutEdge();
            }
        }
    });
    wxUnusedVar(resolvingGuard);

    for ( unsigned pass = 0; pass != 2; ++pass )
    {
        wxComboBox * const owner =
            callbackState->GetOwner<wxComboBox>();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != impl ||
             owner->m_winui->textCallbackState != callbackState )
        {
            break;
        }

        owner->m_winui->comboPartPending = false;
        // Once a transition has retired its editor, every resolver is
        // observation-only until the natural WinUI layout pass exposes the
        // post-transition part. This also covers a transition which begins
        // re-entrantly between the two bounded passes.
        const bool forceLayout =
            updateLayout && pass == 0 &&
            !owner->m_winui->comboTemplateReplayPending &&
            !owner->m_winui->comboTemplateTransition;
        owner->ResolveEditPartOnce(forceLayout);

        wxComboBox * const liveOwner =
            callbackState->GetOwner<wxComboBox>();
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != impl ||
             liveOwner->m_winui->textCallbackState != callbackState ||
             !liveOwner->m_winui->comboPartPending )
        {
            break;
        }
    }

    wxComboBox *owner = callbackState->GetOwner<wxComboBox>();
    const auto isExactOwner = [&]()
    {
        return owner == this && owner->m_winui &&
               owner->m_winui.get() == impl &&
               owner->m_winui->textCallbackState == callbackState;
    };
    if ( !isExactOwner() )
        return false;

    if ( owner->m_winui->comboTemplateReplayPending &&
         owner->m_winui->editBox &&
         !owner->m_winui->comboTemplateTransition )
    {
        // A nested theme/template transition asked this already-running
        // resolver to install the editor. Complete the state replay on this
        // concrete resolver edge, before exposing the generation as ready.
        owner->m_winui->comboTemplateReplayPending = false;
        if ( !owner->ReplayTemplateStateToEditPart() )
        {
            owner = callbackState->GetOwner<wxComboBox>();
            if ( isExactOwner() )
            {
                owner->m_winui->comboTemplateReplayPending = true;
                owner->m_winui->comboPartPending = true;
                queuePassiveFollowup = true;
            }
        }
        owner = callbackState->GetOwner<wxComboBox>();
    }

    if ( isExactOwner() &&
         (owner->m_winui->comboPartPending ||
          (owner->m_winui->comboTemplateReplayPending &&
           !owner->m_winui->editBox)) )
    {
        // The two synchronous passes are a hard recursion budget. Transfer
        // any remaining work into one weak Low ticket instead of leaving
        // phase PendingResolve orphaned or starting a third pass here. When
        // this resolver itself runs inside such a ticket, the enqueue first
        // coalesces and its ticket guard republishes only after retiring the
        // current revision.
        owner->m_winui->comboPartPending = true;
        queuePassiveFollowup = true;
    }

    return isExactOwner();
}

bool wxComboBox::EnsureThemeTransitionBoundary()
{
    if ( !m_winui || !m_winui->comboBox || !m_winui->editableRoot ||
         m_winui->simpleRoot || !m_winui->textCallbackState ||
         !m_winui->callbackState )
    {
        return false;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const MUXC::ComboBox combo = impl->comboBox;
    const MUXC::Grid editableRoot = impl->editableRoot;
    wxComboBox * const expectedOwner = this;
    const auto getExactOwner = [&]() -> wxComboBox *
    {
        wxChoice * const choiceOwner = choiceState->GetOwner();
        wxComboBox * const textOwner =
            textState->GetOwner<wxComboBox>();
        return choiceOwner == expectedOwner && textOwner == expectedOwner &&
                       textOwner->m_winui &&
                       textOwner->m_winui.get() == impl &&
                       textOwner->m_winui->callbackState == choiceState &&
                       textOwner->m_winui->textCallbackState == textState &&
                       textOwner->m_winui->comboBox == combo &&
                       textOwner->m_winui->editableRoot == editableRoot
                 ? textOwner
                 : nullptr;
    };

    MUX::FrameworkElement boundary{ nullptr };
    try
    {
        const MUX::XamlRoot xamlRoot = editableRoot.XamlRoot();
        if ( !xamlRoot )
        {
            // A detached subtree cannot inherit a root transition.
            // Loaded/LayoutUpdated will bind and pin its effective Light/Dark
            // theme before resolving a callback-bearing edit generation.
            return getExactOwner() != nullptr;
        }
        boundary = xamlRoot.Content().try_as<MUX::FrameworkElement>();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox theme-boundary discovery", e);
        return false;
    }

    if ( !boundary )
        return false;

    const auto getEffectiveTheme = [&]() -> MUX::ElementTheme
    {
        MUX::ElementTheme theme = boundary.ActualTheme();
        if ( theme == MUX::ElementTheme::Default )
            theme = editableRoot.ActualTheme();
        return theme;
    };

    if ( impl->comboThemeBoundaryRoot == boundary &&
         impl->comboThemeBoundaryChangedToken.value )
    {
        try
        {
            const MUX::ElementTheme effectiveTheme = getEffectiveTheme();
            if ( effectiveTheme != MUX::ElementTheme::Default &&
                 editableRoot.RequestedTheme() != effectiveTheme &&
                 !impl->comboTemplateTransition )
            {
                OnPeerTemplateTransition(true);
                // The transition deliberately leaves realization to the
                // natural layout pass. Callers must not append a synchronous
                // ApplyTemplate()/UpdateLayout() while that pass is pending.
                return false;
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI ComboBox theme-boundary reconciliation", e);
            return false;
        }
        return getExactOwner() != nullptr;
    }

    const MUX::FrameworkElement oldBoundary =
        impl->comboThemeBoundaryRoot;
    const winrt::event_token oldToken =
        impl->comboThemeBoundaryChangedToken;
    // Publish detachment before crossing the old root. A copied callback from
    // that root can no longer claim this control after a host migration.
    impl->comboThemeBoundaryRoot = nullptr;
    impl->comboThemeBoundaryChangedToken = {};
    if ( oldBoundary && oldToken.value )
    {
        try
        {
            oldBoundary.ActualThemeChanged(oldToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI ComboBox old theme-boundary removal", e);
        }
    }
    if ( !getExactOwner() )
        return false;

    try
    {
        impl->comboThemeBoundaryRoot = boundary;
        impl->comboThemeBoundaryChangedToken =
            boundary.ActualThemeChanged(
                [textState](
                    MUX::FrameworkElement const& sender,
                    winrt::Windows::Foundation::IInspectable const&)
                {
                    wxComboBox * const owner =
                        textState->GetOwner<wxComboBox>();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->textCallbackState != textState ||
                         owner->m_winui->comboThemeBoundaryRoot != sender ||
                         owner->m_winui->comboTemplateTransition )
                    {
                        return;
                    }

                    // editableRoot is still pinned to the previous explicit
                    // theme here. Retire that editor before allowing the new
                    // root theme to enter the subtree.
                    owner->OnPeerTemplateTransition(true);
                });
    }
    catch ( const winrt::hresult_error& e )
    {
        impl->comboThemeBoundaryRoot = nullptr;
        impl->comboThemeBoundaryChangedToken = {};
        wxWinUILogException(
            "WinUI ComboBox theme-boundary registration", e);
        return false;
    }

    wxComboBox *owner = getExactOwner();
    if ( !owner )
        return false;

    try
    {
        const MUX::ElementTheme effectiveTheme = getEffectiveTheme();
        if ( effectiveTheme != MUX::ElementTheme::Default &&
             editableRoot.RequestedTheme() != effectiveTheme )
        {
            owner->OnPeerTemplateTransition(true);
            // See the already-bound branch above: the replacement editor is
            // committed by the queued natural-layout continuation.
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox attached theme reconciliation", e);
        return false;
    }

    return getExactOwner() != nullptr;
}

bool wxComboBox::RetireEditPartForTemplateTransition()
{
    if ( !m_winui || !m_winui->comboBox || m_winui->simpleRoot ||
         !m_winui->textCallbackState || !m_winui->callbackState )
    {
        return false;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const MUXC::ComboBox combo = impl->comboBox;
    const MUXC::TextBox oldEditBox = impl->editBox;
    wxComboBox * const expectedOwner = this;
    const auto getExactOwner = [&]() -> wxComboBox *
    {
        wxChoice * const choiceOwner = choiceState->GetOwner();
        wxComboBox * const textOwner =
            textState->GetOwner<wxComboBox>();
        return choiceOwner == expectedOwner && textOwner == expectedOwner &&
                       textOwner->m_winui &&
                       textOwner->m_winui.get() == impl &&
                       textOwner->m_winui->callbackState == choiceState &&
                       textOwner->m_winui->textCallbackState == textState &&
                       textOwner->m_winui->comboBox == combo
                 ? textOwner
                 : nullptr;
    };

    if ( !oldEditBox )
        return getExactOwner() != nullptr;

    const long long oldTextChangedCallbackToken =
        impl->editTextChangedCallbackToken;
    const winrt::event_token oldSelectionToken =
        impl->editSelectionChangedToken;
    const winrt::event_token oldLayoutUpdatedToken =
        impl->editLayoutUpdatedToken;
    const winrt::event_token oldKeyDownToken = impl->editKeyDownToken;
    const winrt::event_token oldOuterKeyDownToken =
        impl->editOuterKeyDownToken;
    const winrt::event_token oldPointerPressedToken =
        impl->editPointerPressedToken;
    const winrt::event_token oldCopyingToken = impl->editCopyingToken;
    const winrt::event_token oldCuttingToken = impl->editCuttingToken;
    const winrt::event_token oldPasteToken = impl->editPasteToken;

    // Publish invalidation before the first XAML boundary. Queued callbacks
    // from the outgoing editor now fail both generation and exact-identity
    // checks even if its teardown Text mutation arrives before Unloaded.
    impl->editBox = nullptr;
    impl->editTextChangedCallbackToken = 0;
    impl->editSelectionChangedToken = {};
    impl->editLayoutUpdatedToken = {};
    impl->editKeyDownToken = {};
    impl->editOuterKeyDownToken = {};
    impl->editPointerPressedToken = {};
    impl->editCopyingToken = {};
    impl->editCuttingToken = {};
    impl->editPasteToken = {};
    if ( ++impl->editGeneration == 0 )
        ++impl->editGeneration;
    ClearPendingTextValue();
    ClearPendingTextSelection();
    impl->clipboardOperationInProgress = false;

    ++impl->autoCompleteGeneration;
    impl->autoCompleteSelection = wxNOT_FOUND;
    MUXC::MenuFlyout oldAutoCompleteFlyout =
        std::move(impl->autoCompleteFlyout);
    impl->autoCompleteFlyout = nullptr;
    const std::shared_ptr<wxWinUIFlyoutPopupRetirementState>
        oldAutoCompleteRetirementState =
            impl->autoCompleteRetirementState;
    if ( oldAutoCompleteFlyout )
    {
        if ( !oldAutoCompleteRetirementState )
            std::terminate();
        impl->autoCompleteRetirementPending = true;
        impl->autoCompleteReplayPending = true;
    }

    wxWinUIRevokeComboEditHandlers(
        oldEditBox,
        oldEditBox.try_as<MUX::UIElement>(),
        combo.try_as<MUX::UIElement>(),
        oldTextChangedCallbackToken,
        oldSelectionToken,
        oldLayoutUpdatedToken,
        oldKeyDownToken,
        oldOuterKeyDownToken,
        oldPointerPressedToken,
        oldCopyingToken,
        oldCuttingToken,
        oldPasteToken);
    wxComboBox *owner = getExactOwner();
    if ( !owner )
        return false;

    // Until the replacement editor is committed, focus belongs to the stable
    // semantic ComboBox rather than to the retiring template descendant.
    impl->host.SetPreferredFocus(combo);
    owner = getExactOwner();
    if ( !owner )
        return false;

    if ( oldAutoCompleteFlyout )
    {
        try
        {
            oldAutoCompleteFlyout.Hide();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI ComboBox transition flyout removal", e);
            wxWinUIFailFlyoutPopupShow(
                oldAutoCompleteRetirementState);
        }
    }

    return getExactOwner() != nullptr;
}

bool wxComboBox::ReplayTemplateStateToEditPart()
{
    if ( !m_editable || !m_winui || !m_winui->comboBox ||
         !m_winui->editBox || !m_winui->callbackState ||
         !m_winui->textCallbackState )
    {
        return false;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    ++impl->diagnosticTemplateReplayAttempts;
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    wxComboBox * const expectedOwner = this;
    const auto getExactOwner = [&]() -> wxComboBox *
    {
        wxChoice * const choiceOwner = choiceState->GetOwner();
        wxComboBox * const textOwner = textState->GetOwner<wxComboBox>();
        return choiceOwner == expectedOwner && textOwner == expectedOwner &&
                       textOwner->m_winui &&
                       textOwner->m_winui.get() == impl &&
                       textOwner->m_winui->callbackState == choiceState &&
                       textOwner->m_winui->textCallbackState == textState &&
                       textOwner->m_winui->comboBox &&
                       textOwner->m_winui->editBox
                 ? textOwner
                 : nullptr;
    };

    const bool installedTransitionGuard = !impl->comboTemplateTransition;
    if ( installedTransitionGuard )
        impl->comboTemplateTransition = true;
    wxScopeGuard transitionGuard = wxMakeGuard(
        [textState, impl, installedTransitionGuard]()
        {
            if ( !installedTransitionGuard )
                return;
            wxComboBox * const owner =
                textState->GetOwner<wxComboBox>();
            if ( owner && owner->m_winui && owner->m_winui.get() == impl &&
                 owner->m_winui->textCallbackState == textState )
            {
                owner->m_winui->comboTemplateTransition = false;
            }
        });
    wxUnusedVar(transitionGuard);

    choiceState->BeginPeerMutation();
    wxScopeGuard mutationGuard = wxMakeGuard([choiceState]()
    {
        choiceState->EndPeerMutation();
    });
    wxUnusedVar(mutationGuard);

    wxComboBox *owner = getExactOwner();
    if ( !owner || !owner->ApplySelectionToPeer() )
        return false;
    owner = getExactOwner();
    if ( !owner )
        return false;

    owner->ApplyTextToPeer();
    owner = getExactOwner();
    if ( !owner )
        return false;

    wxWinUIChoiceImpl * const replayImpl = owner->m_winui.get();
    const MUXC::TextBox replayEdit = replayImpl->editBox;
    const std::uint64_t rangeGeneration = replayImpl->editGeneration;
    const long rangeFrom = wxMin(owner->m_selectionStart,
                                 owner->m_selectionEnd);
    const long rangeTo = wxMax(owner->m_selectionStart,
                               owner->m_selectionEnd);
    long peerRangeFrom = 0;
    long peerRangeTo = 0;
    try
    {
        peerRangeFrom = replayEdit.SelectionStart();
        owner = getExactOwner();
        if ( !owner || owner->m_winui.get() != replayImpl ||
             owner->m_winui->editGeneration != rangeGeneration ||
             owner->m_winui->editBox != replayEdit )
        {
            return false;
        }

        peerRangeTo = peerRangeFrom + replayEdit.SelectionLength();
        owner = getExactOwner();
        if ( !owner || owner->m_winui.get() != replayImpl ||
             owner->m_winui->editGeneration != rangeGeneration ||
             owner->m_winui->editBox != replayEdit )
        {
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox replay range discovery", e);
        return false;
    }

    const bool needsRangeTicket =
        peerRangeFrom != rangeFrom || peerRangeTo != rangeTo;
    std::uint64_t rangeEpoch = 0;
    if ( needsRangeTicket )
    {
        if ( ++replayImpl->pendingTextSelectionEpoch == 0 )
            ++replayImpl->pendingTextSelectionEpoch;
        rangeEpoch = replayImpl->pendingTextSelectionEpoch;
        replayImpl->pendingTextSelectionStart = rangeFrom;
        replayImpl->pendingTextSelectionEnd = rangeTo;
        replayImpl->pendingTextSelectionGeneration = rangeGeneration;
        replayImpl->pendingTextSelectionRevision = 0;
        replayImpl->pendingTextSelectionEditIdentity =
            reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                wxWinUIComboObjectIdentity(replayEdit)));
        replayImpl->hasPendingTextSelection = true;
    }
    else
    {
        // A natural theme pass may keep the same TextBox and its exact wx
        // range. Selecting that range again need not raise SelectionChanged,
        // so a ticket would have no causal callback capable of retiring it.
        owner->ClearPendingTextSelection();
    }

    if ( !owner->ApplyTextSelectionToPeer() )
        return false;

    owner = getExactOwner();
    if ( !owner || owner->m_winui.get() != replayImpl ||
         owner->m_winui->editGeneration != rangeGeneration ||
         owner->m_winui->editBox != replayEdit ||
         (needsRangeTicket &&
          (!owner->m_winui->hasPendingTextSelection ||
           owner->m_winui->pendingTextSelectionEpoch != rangeEpoch)) )
    {
        return false;
    }

    owner = getExactOwner();
    const bool replayed = owner && owner->m_winui.get() == replayImpl &&
           owner->m_winui->editGeneration == rangeGeneration &&
           owner->m_winui->editBox == replayEdit &&
           (!needsRangeTicket ||
            (owner->m_winui->hasPendingTextSelection &&
             owner->m_winui->pendingTextSelectionEpoch == rangeEpoch));
    if ( replayed )
        ++owner->m_winui->diagnosticTemplateReplaySuccesses;
    return replayed;
}

void wxComboBox::OnPeerTemplateTransition(bool forceEditableReload)
{
    if ( !m_editable || !m_winui || !m_winui->comboBox ||
         m_winui->simpleRoot || !m_winui->textCallbackState ||
         !m_winui->callbackState || m_winui->comboTemplateTransition )
    {
        return;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    ++impl->diagnosticTemplateTransitions;
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const MUXC::ComboBox combo = impl->comboBox;
    wxComboBox * const expectedOwner = this;
    const auto getExactOwner = [&]() -> wxComboBox *
    {
        wxChoice * const choiceOwner = choiceState->GetOwner();
        wxComboBox * const textOwner =
            textState->GetOwner<wxComboBox>();
        return choiceOwner == expectedOwner && textOwner == expectedOwner &&
                       textOwner->m_winui &&
                       textOwner->m_winui.get() == impl &&
                       textOwner->m_winui->callbackState == choiceState &&
                       textOwner->m_winui->textCallbackState == textState &&
                       textOwner->m_winui->comboBox == combo
                 ? textOwner
                 : nullptr;
    };

    MUX::ElementTheme effectiveTheme = MUX::ElementTheme::Default;
    if ( forceEditableReload )
    {
        try
        {
            const MUX::FrameworkElement boundary =
                impl->comboThemeBoundaryRoot;
            if ( boundary )
                effectiveTheme = boundary.ActualTheme();
            if ( effectiveTheme == MUX::ElementTheme::Default &&
                 impl->editableRoot )
            {
                effectiveTheme = impl->editableRoot.ActualTheme();
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI ComboBox effective-theme capture", e);
            return;
        }

        // ActualTheme on an attached root resolves to Light or Dark. Do not
        // retire a working editor until that concrete pin is available.
        if ( effectiveTheme == MUX::ElementTheme::Default ||
             !getExactOwner() )
        {
            return;
        }
    }

    // Queue only after both the peer-mutation and transition guards have
    // unwound. This guarantees that the continuation observes the natural
    // post-theme tree and that no caller nests forced layout in the theme or
    // TemplateProperty callback which initiated this transaction.
    bool queueNaturalResolution = false;
    wxScopeGuard resolutionGuard = wxMakeGuard(
        [textState, choiceState, impl, combo, expectedOwner,
         &queueNaturalResolution]()
        {
            if ( !queueNaturalResolution )
                return;
            wxChoice * const choiceOwner = choiceState->GetOwner();
            wxComboBox * const textOwner =
                textState->GetOwner<wxComboBox>();
            if ( choiceOwner == expectedOwner &&
                 textOwner == expectedOwner && textOwner->m_winui &&
                 textOwner->m_winui.get() == impl &&
                 textOwner->m_winui->callbackState == choiceState &&
                 textOwner->m_winui->textCallbackState == textState &&
                 textOwner->m_winui->comboBox == combo )
            {
                textOwner->OnPeerLayoutUpdated();
            }
        });
    wxUnusedVar(resolutionGuard);

    const bool hadCommittedEdit = impl->editBox != nullptr;
    impl->comboTemplateTransition = true;
    if ( hadCommittedEdit )
        impl->comboTemplateReplayPending = false;
    wxScopeGuard transitionGuard = wxMakeGuard([textState, impl]()
    {
        wxComboBox * const owner =
            textState->GetOwner<wxComboBox>();
        if ( owner && owner->m_winui && owner->m_winui.get() == impl &&
             owner->m_winui->textCallbackState == textState )
        {
            owner->m_winui->comboTemplateTransition = false;
        }
    });
    wxUnusedVar(transitionGuard);

    // Suppress native selection/text projections for the complete physical
    // teardown, but do not rely on the guard for safety: the outgoing editor
    // generation is revoked first by RetireEditPartForTemplateTransition().
    choiceState->BeginPeerMutation();
    wxScopeGuard mutationGuard = wxMakeGuard([choiceState]()
    {
        choiceState->EndPeerMutation();
    });
    wxUnusedVar(mutationGuard);

    if ( !hadCommittedEdit )
    {
        // There is no callback-bearing generation to retire or replay yet.
        // Pin the concrete theme when requested and let the ordinary natural
        // layout path perform the initial passive bind. comboPartPending is a
        // latch for the revisioned weak Low ticket if this notification was
        // itself raised from inside the currently-running ticket.
        impl->comboPartPending = true;
        queueNaturalResolution = true;
        try
        {
            if ( forceEditableReload )
            {
                const MUXC::Grid editableRoot = impl->editableRoot;
                if ( !editableRoot )
                    return;
                editableRoot.RequestedTheme(effectiveTheme);
                if ( !getExactOwner() )
                    return;
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI ComboBox initial theme bind", e);
        }
        return;
    }

    if ( !RetireEditPartForTemplateTransition() )
        return;

    wxComboBox *owner = getExactOwner();
    if ( !owner )
        return;

    // Retiring the old generation is authoritative even when WinUI keeps the
    // same TextBox object across a theme pass: its old delegates can no longer
    // project state while RequestedTheme updates the subtree. The queued
    // resolver installs the exact post-transition generation and replays the
    // complete wx text/selection state.
    owner->m_winui->comboTemplateReplayPending = true;
    queueNaturalResolution = true;

    try
    {
        if ( forceEditableReload )
        {
            const MUXC::Grid editableRoot = impl->editableRoot;
            if ( !editableRoot )
                return;
            // This is the only writer needed to let the new root theme enter
            // the editable subtree. Explicitly clearing/restoring Template and
            // forcing ApplyTemplate()/UpdateLayout() here can leave deferred
            // E_UNEXPECTED errors in WinUI's ComboBox/TextBox internals. Let
            // WinUI's ordinary theme/layout pass own that reconstruction.
            editableRoot.RequestedTheme(effectiveTheme);
            owner = getExactOwner();
            if ( !owner )
                return;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox template transition", e);
        return;
    }
}

void wxComboBox::QueueEditPartResolutionAtLayoutEdge()
{
    if ( !m_editable || !m_winui || m_winui->simpleRoot ||
         !m_winui->editableRoot || !m_winui->comboBox ||
         !m_winui->callbackState || !m_winui->textCallbackState ||
         m_winui->comboTemplateTransition )
    {
        return;
    }

    // Ordinary LayoutUpdated callbacks remain excluded during a peer
    // mutation by OnPeerLayoutUpdated(). The sole exception here is the
    // internal handoff of already-latched residual resolver work: scheduling
    // a Low continuation cannot execute it inline on this dispatcher thread,
    // so the enclosing mutation necessarily unwinds first.
    if ( m_winui->callbackState->IsPeerMutationInProgress() &&
         !m_winui->comboPartPending )
    {
        return;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
#ifdef WXWINUI_TEST_SUPPORT
    // Consume before any XAML getter can re-enter a layout callback.
    const bool forceTransitionForTesting =
        wxWinUITakeComboLayoutTestRequest(this, impl);
#endif
    if ( impl->comboLayoutResolveQueued )
    {
        ++impl->diagnosticComboLayoutCoalescedEdges;
        return;
    }

    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    const MUXC::Grid editableRoot = impl->editableRoot;
    const MUXC::ComboBox combo = impl->comboBox;
    const std::uint64_t editGeneration = impl->editGeneration;
    const bool templateReplayPendingAtQueue =
        impl->comboTemplateReplayPending;
    const bool partResolutionPendingAtQueue =
        impl->comboPartPending;
    const std::uintptr_t editableRootIdentity =
        reinterpret_cast<std::uintptr_t>(winrt::get_abi(
            wxWinUIComboObjectIdentity(editableRoot)));
    const std::uintptr_t comboIdentity =
        reinterpret_cast<std::uintptr_t>(winrt::get_abi(
            wxWinUIComboObjectIdentity(combo)));
    const std::uintptr_t editIdentity =
        reinterpret_cast<std::uintptr_t>(winrt::get_abi(
            wxWinUIComboObjectIdentity(impl->editBox)));
    wxComboBox * const expectedOwner = this;
    const auto identity = [](const auto& object)
    {
        return reinterpret_cast<std::uintptr_t>(winrt::get_abi(
            wxWinUIComboObjectIdentity(object)));
    };
    const auto getExactOwner = [&]() -> wxComboBox *
    {
        wxChoice * const choiceOwner = choiceState->GetOwner();
        wxComboBox * const textOwner =
            textState->GetOwner<wxComboBox>();
        if ( choiceOwner != expectedOwner || textOwner != expectedOwner ||
             !textOwner->m_winui || textOwner->m_winui.get() != impl )
        {
            return nullptr;
        }

        wxWinUIChoiceImpl * const liveImpl = textOwner->m_winui.get();
        return liveImpl->callbackState == choiceState &&
                       liveImpl->textCallbackState == textState &&
                       liveImpl->editGeneration == editGeneration &&
                       identity(liveImpl->editableRoot) ==
                           editableRootIdentity &&
                       identity(liveImpl->comboBox) == comboIdentity &&
                       identity(liveImpl->editBox) == editIdentity
                 ? textOwner
                 : nullptr;
    };
    if ( !getExactOwner() )
        return;

    std::uint64_t revision = 0;
    const auto clearExactRevision = [&]()
    {
        wxChoice * const choiceOwner = choiceState->GetOwner();
        wxComboBox * const textOwner =
            textState->GetOwner<wxComboBox>();
        if ( choiceOwner == textOwner && textOwner && textOwner->m_winui &&
             textOwner->m_winui.get() == impl &&
             textOwner->m_winui->callbackState == choiceState &&
             textOwner->m_winui->textCallbackState == textState && revision &&
             textOwner->m_winui->comboLayoutResolveRevision == revision )
        {
            textOwner->m_winui->comboLayoutResolveRunning = false;
            textOwner->m_winui->comboLayoutResolveQueued = false;
        }
    };

    try
    {
        // This method runs in LayoutUpdated. Every XAML operation before the
        // dispatcher enqueue is a getter: template/theme mutation and forced
        // layout are owned exclusively by the continuation below.
        const MUX::XamlRoot rootXaml = editableRoot.XamlRoot();
        if ( !getExactOwner() )
            return;
        const MUX::XamlRoot comboXaml = combo.XamlRoot();
        if ( !getExactOwner() || !rootXaml || !comboXaml ||
             identity(rootXaml) != identity(comboXaml) )
        {
            return;
        }
        const std::uintptr_t xamlRootIdentity = identity(rootXaml);

        const MUXC::TextBox candidate = wxWinUIFindTextBox(combo);
        if ( !getExactOwner() )
            return;
        const std::uintptr_t candidateIdentity = identity(candidate);

        const MUX::UIElement rootContent = rootXaml.Content();
        if ( !getExactOwner() )
            return;
        const MUX::FrameworkElement boundary =
            rootContent.try_as<MUX::FrameworkElement>();
        if ( !getExactOwner() || !boundary )
            return;

        MUX::ElementTheme effectiveTheme = boundary.ActualTheme();
        if ( !getExactOwner() )
            return;
        if ( effectiveTheme == MUX::ElementTheme::Default )
        {
            effectiveTheme = editableRoot.ActualTheme();
            if ( !getExactOwner() )
                return;
        }
        const MUX::ElementTheme requestedTheme =
            editableRoot.RequestedTheme();
        wxComboBox *owner = getExactOwner();
        if ( !owner )
            return;

        const bool partChanged = candidateIdentity != editIdentity;
        const bool editorMissing = !candidate && !owner->m_winui->editBox;
        const bool boundaryChanged =
            owner->m_winui->comboThemeBoundaryRoot != boundary ||
            !owner->m_winui->comboThemeBoundaryChangedToken.value;
        const bool themeChanged =
            effectiveTheme != MUX::ElementTheme::Default &&
            requestedTheme != effectiveTheme;
        if (
#ifdef WXWINUI_TEST_SUPPORT
             !forceTransitionForTesting &&
#endif
             !templateReplayPendingAtQueue &&
             !partResolutionPendingAtQueue && !partChanged &&
             !editorMissing && !boundaryChanged && !themeChanged )
        {
            return;
        }

        // One forced x:Load retry is allowed for an unchanged edit generation.
        // A later concrete candidate identity or theme boundary can still queue
        // a passive reconciliation without reopening that force-layout budget.
        if (
#ifdef WXWINUI_TEST_SUPPORT
             !forceTransitionForTesting &&
#endif
             !partResolutionPendingAtQueue && editorMissing &&
             !partChanged && !boundaryChanged && !themeChanged &&
             owner->m_winui->comboLayoutForcedRetryAttempted &&
             owner->m_winui->comboLayoutForcedRetryGeneration ==
                 editGeneration )
        {
            return;
        }

        revision = ++owner->m_winui->comboLayoutResolveRevision;
        if ( revision == 0 )
            revision = ++owner->m_winui->comboLayoutResolveRevision;
        owner->m_winui->comboLayoutResolveQueued = true;
        ++owner->m_winui->diagnosticComboLayoutResolveRequests;

        const MUXD::DispatcherQueue queue = combo.DispatcherQueue();
        owner = getExactOwner();
        if ( !owner ||
             owner->m_winui->comboLayoutResolveRevision != revision )
        {
            clearExactRevision();
            return;
        }
        if ( !queue )
        {
            clearExactRevision();
            return;
        }

        const std::weak_ptr<wxWinUIChoiceCallbackState> weakChoiceState =
            choiceState;
        const std::weak_ptr<wxWinUITextCallbackState> weakTextState =
            textState;
        const bool queued = queue.TryEnqueue(
            MUXD::DispatcherQueuePriority::Low,
            [weakChoiceState, weakTextState, expectedOwner, impl,
             editGeneration, revision, editableRootIdentity, comboIdentity,
             editIdentity, xamlRootIdentity,
#ifdef WXWINUI_TEST_SUPPORT
             forceTransitionForTesting,
#endif
             templateReplayPendingAtQueue,
             partResolutionPendingAtQueue]()
            {
                const auto queuedChoiceState = weakChoiceState.lock();
                const auto queuedTextState = weakTextState.lock();
                if ( !queuedChoiceState || !queuedTextState )
                    return;

                const auto queuedIdentity = [](const auto& object)
                {
                    return reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                        wxWinUIComboObjectIdentity(object)));
                };
                const auto getTicketOwner = [&](bool exactGeneration)
                    -> wxComboBox *
                {
                    wxChoice * const choiceOwner =
                        queuedChoiceState->GetOwner();
                    wxComboBox * const textOwner =
                        queuedTextState->GetOwner<wxComboBox>();
                    if ( choiceOwner != expectedOwner ||
                         textOwner != expectedOwner || !textOwner->m_winui ||
                         textOwner->m_winui.get() != impl )
                    {
                        return nullptr;
                    }
                    wxWinUIChoiceImpl * const liveImpl =
                        textOwner->m_winui.get();
                    if ( liveImpl->callbackState != queuedChoiceState ||
                         liveImpl->textCallbackState != queuedTextState ||
                         !liveImpl->comboLayoutResolveQueued ||
                         liveImpl->comboLayoutResolveRevision != revision ||
                         queuedIdentity(liveImpl->editableRoot) !=
                             editableRootIdentity ||
                         queuedIdentity(liveImpl->comboBox) != comboIdentity )
                    {
                        return nullptr;
                    }
                    if ( exactGeneration &&
                         (liveImpl->editGeneration != editGeneration ||
                          queuedIdentity(liveImpl->editBox) != editIdentity) )
                    {
                        return nullptr;
                    }
                    return textOwner;
                };

                bool scheduleBoundedReplaySuccessor = false;
                wxScopeGuard ticketGuard = wxMakeGuard(
                    [weakChoiceState, weakTextState, impl, revision,
                     templateReplayPendingAtQueue,
                     partResolutionPendingAtQueue,
                     &scheduleBoundedReplaySuccessor]()
                    {
                        const auto choiceState = weakChoiceState.lock();
                        const auto textState = weakTextState.lock();
                        if ( !choiceState || !textState )
                            return;
                        wxChoice * const choiceOwner = choiceState->GetOwner();
                        wxComboBox * const textOwner =
                            textState->GetOwner<wxComboBox>();
                        if ( choiceOwner == textOwner && textOwner &&
                             textOwner->m_winui &&
                             textOwner->m_winui.get() == impl &&
                             textOwner->m_winui->callbackState == choiceState &&
                             textOwner->m_winui->textCallbackState ==
                                 textState &&
                             textOwner->m_winui->comboLayoutResolveRevision ==
                                 revision )
                        {
                            textOwner->m_winui->comboLayoutResolveRunning =
                                false;
                            textOwner->m_winui->comboLayoutResolveQueued =
                                false;

                            // A transition, replay failure or residual nested
                            // resolver can ask for another passive edge while
                            // this revision is still live, so its immediate
                            // request correctly coalesces. Retire the revision
                            // first, then publish one successor only for a
                            // false-to-true latch created by this ticket. The
                            // successor captures that latch as true and cannot
                            // self-republish for the same cause.
                            const bool transitionNeedsSuccessor =
                                !templateReplayPendingAtQueue &&
                                textOwner->m_winui->
                                    comboTemplateReplayPending;
                            const bool partNeedsSuccessor =
                                !partResolutionPendingAtQueue &&
                                textOwner->m_winui->comboPartPending;
                            if ( scheduleBoundedReplaySuccessor ||
                                 transitionNeedsSuccessor ||
                                 partNeedsSuccessor )
                            {
                                textOwner->OnPeerLayoutUpdated();
                            }
                        }
                    });
                wxUnusedVar(ticketGuard);

                try
                {
                    wxComboBox *owner = getTicketOwner(true);
                    if ( !owner )
                        return;
                    const MUX::XamlRoot currentRoot =
                        owner->m_winui->editableRoot.XamlRoot();
                    owner = getTicketOwner(true);
                    if ( !owner || !currentRoot ||
                         queuedIdentity(currentRoot) != xamlRootIdentity )
                    {
                        return;
                    }

                    owner->m_winui->comboLayoutResolveRunning = true;
                    ++owner->m_winui->diagnosticComboLayoutResolveRuns;

#ifdef WXWINUI_TEST_SUPPORT
                    if ( forceTransitionForTesting )
                    {
                        owner->OnPeerTemplateTransition(true);
                        return;
                    }
#endif

                    // Preserve the old LayoutUpdated theme responsibility, but
                    // run it only after the callback has unwound. It may itself
                    // replace and fully resolve the editor generation.
                    if ( !owner->EnsureThemeTransitionBoundary() )
                        return;
                    owner = getTicketOwner(false);
                    if ( !owner || owner->m_winui->comboTemplateTransition )
                        return;
                    const MUX::XamlRoot reconciledRoot =
                        owner->m_winui->editableRoot.XamlRoot();
                    owner = getTicketOwner(false);
                    if ( !owner || !reconciledRoot ||
                         queuedIdentity(reconciledRoot) != xamlRootIdentity ||
                         owner->m_winui->editGeneration != editGeneration )
                    {
                        // A theme transaction owns the replacement generation
                        // it created. Do not append a second forced pass to the
                        // same dispatcher continuation.
                        return;
                    }

                    MUXC::TextBox currentCandidate =
                        wxWinUIFindTextBox(owner->m_winui->comboBox);
                    owner = getTicketOwner(false);
                    if ( !owner )
                        return;

                    const bool partResolutionPending =
                        owner->m_winui->comboPartPending;
                    const bool replayResolutionPending =
                        owner->m_winui->comboTemplateReplayPending;
                    if ( partResolutionPending || replayResolutionPending ||
                         queuedIdentity(currentCandidate) !=
                         queuedIdentity(owner->m_winui->editBox) )
                    {
                        if ( !owner->ResolveEditPart(false) )
                            return;
                        owner = getTicketOwner(false);
                        if ( !owner )
                            return;

                        // A failed replay or a nested resolver can reassert
                        // comboPartPending from inside ResolveEditPart(). If
                        // this ticket already captured the latch as true, the
                        // edge comparison in the guard cannot see the new
                        // request. Grant one passive successor for this
                        // concrete edit generation. A repeated failure keeps
                        // the natural-edge latches set but cannot schedule
                        // another successor by itself.
                        if ( owner->m_winui->editBox &&
                             owner->m_winui->comboPartPending )
                        {
                            wxWinUIChoiceImpl * const replayImpl =
                                owner->m_winui.get();
                            if ( !replayImpl->
                                    comboLayoutForcedRetryAttempted ||
                                 replayImpl->
                                    comboLayoutForcedRetryGeneration !=
                                     replayImpl->editGeneration )
                            {
                                replayImpl->comboLayoutForcedRetryAttempted =
                                    true;
                                replayImpl->comboLayoutForcedRetryGeneration =
                                    replayImpl->editGeneration;
                                scheduleBoundedReplaySuccessor = true;
                            }
                        }
                    }

                    if ( owner->m_winui->editBox )
                        return;

                    currentCandidate =
                        wxWinUIFindTextBox(owner->m_winui->comboBox);
                    owner = getTicketOwner(false);
                    if ( !owner )
                        return;
                    if ( currentCandidate )
                    {
                        (void)owner->ResolveEditPart(false);
                        return;
                    }

                    if ( owner->m_winui->comboTemplateReplayPending )
                    {
                        // A theme/template transition deliberately waits for
                        // WinUI's natural layout pass. If this Low callback
                        // wins that race, keep the replay latched: the next
                        // genuine LayoutUpdated edge will resolve the part
                        // passively. Never re-enter the failing forced
                        // ApplyTemplate()/UpdateLayout() path here.
                        return;
                    }

                    const MUX::XamlRoot retryRoot =
                        owner->m_winui->editableRoot.XamlRoot();
                    owner = getTicketOwner(false);
                    if ( !owner || !retryRoot ||
                         queuedIdentity(retryRoot) != xamlRootIdentity )
                    {
                        return;
                    }

                    wxWinUIChoiceImpl * const liveImpl = owner->m_winui.get();
                    if ( liveImpl->comboLayoutForcedRetryAttempted &&
                         liveImpl->comboLayoutForcedRetryGeneration ==
                             liveImpl->editGeneration )
                    {
                        return;
                    }
                    liveImpl->comboLayoutForcedRetryAttempted = true;
                    liveImpl->comboLayoutForcedRetryGeneration =
                        liveImpl->editGeneration;
                    (void)owner->ResolveEditPart(true);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI ComboBox queued layout resolution", e);
                }
            });
        if ( !queued )
            clearExactRevision();
    }
    catch ( const winrt::hresult_error& e )
    {
        clearExactRevision();
        wxWinUILogException(
            "WinUI ComboBox layout-edge resolution queue", e);
    }
}

void wxComboBox::OnPeerLayoutUpdated()
{
    if ( !m_winui || m_winui->simpleRoot || !m_winui->comboBox ||
         !m_winui->callbackState || !m_winui->textCallbackState ||
         m_winui->comboTemplateTransition ||
         m_winui->callbackState->IsPeerMutationInProgress() )
    {
        return;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    ++impl->diagnosticComboLayoutEdges;
    ++impl->comboLayoutCallbackDepth;
    wxScopeGuard callbackGuard = wxMakeGuard([textState, impl]()
    {
        wxComboBox * const owner =
            textState->GetOwner<wxComboBox>();
        if ( owner && owner->m_winui && owner->m_winui.get() == impl &&
             owner->m_winui->textCallbackState == textState &&
             owner->m_winui->comboLayoutCallbackDepth )
        {
            --owner->m_winui->comboLayoutCallbackDepth;
        }
    });
    wxUnusedVar(callbackGuard);

    // Strictly passive: this path performs only identity/theme reads and a
    // coalesced enqueue. ApplyTemplate/Measure/Arrange/UpdateLayout and theme
    // replay are forbidden until the Low-priority continuation runs.
    QueueEditPartResolutionAtLayoutEdge();
}

void wxComboBox::ResolveEditPartOnce(bool updateLayout)
{
    if ( !m_winui || !m_winui->comboBox )
        return;

    wxComboBox * const expectedOwner = this;
    wxWinUIChoiceImpl * const impl = m_winui.get();
    if ( updateLayout )
    {
        if ( impl->comboLayoutCallbackDepth )
        {
            ++impl->diagnosticComboLayoutSynchronousRealizations;
        }
        if ( impl->comboLayoutResolveRunning )
        {
            ++impl->diagnosticComboLayoutRealizations;
            if ( !impl->comboLayoutResolveQueued )
                ++impl->diagnosticComboLayoutUnlatchedRealizations;
        }
    }
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    if ( !callbackState || !choiceState )
        return;

    const MUXC::ComboBox combo = impl->comboBox;
    const auto getLiveOwner = [&]() -> wxComboBox *
    {
        wxComboBox * const owner =
            callbackState->GetOwner<wxComboBox>();
        return owner == expectedOwner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->textCallbackState == callbackState &&
                       owner->m_winui->callbackState == choiceState &&
                       owner->m_winui->comboBox == combo
                 ? owner
                 : nullptr;
    };
    const auto getForcedOwner = [&]() -> wxComboBox *
    {
        wxComboBox * const owner = getLiveOwner();
        return owner && !owner->m_winui->comboTemplateReplayPending &&
                       !owner->m_winui->comboTemplateTransition
                 ? owner
                 : nullptr;
    };

    try
    {
        wxComboBox *liveOwner = getLiveOwner();
        if ( !liveOwner )
            return;

        if ( updateLayout )
        {
            liveOwner = getForcedOwner();
            if ( !liveOwner )
                return;

            // Always publish the current wx slot geometry.
            impl->host.ForceRender();
            liveOwner = getForcedOwner();
            if ( !liveOwner )
                return;

            const bool rendered = combo.XamlRoot() != nullptr;
            liveOwner = getForcedOwner();
            if ( !liveOwner || !rendered )
                return;

            combo.ApplyTemplate();
            liveOwner = getForcedOwner();
            if ( !liveOwner )
                return;

            const float inf = std::numeric_limits<float>::infinity();
            combo.Measure({ inf, inf });
            liveOwner = getForcedOwner();
            if ( !liveOwner )
                return;

            const auto desired = combo.DesiredSize();
            liveOwner = getForcedOwner();
            if ( !liveOwner )
                return;
            const double actualWidth = combo.ActualWidth();
            liveOwner = getForcedOwner();
            if ( !liveOwner )
                return;
            const double actualHeight = combo.ActualHeight();
            liveOwner = getForcedOwner();
            if ( !liveOwner )
                return;

            const wxSize clientPixels = liveOwner->GetClientSize();
            liveOwner = getForcedOwner();
            if ( !liveOwner )
                return;
            const wxSize clientDips = liveOwner->ToDIP(clientPixels);
            liveOwner = getForcedOwner();
            if ( !liveOwner )
                return;

            const auto usableExtent = [](int requested,
                                         float desiredExtent,
                                         double actualExtent) -> float
            {
                double extent = requested > 0
                                    ? static_cast<double>(requested)
                                    : static_cast<double>(desiredExtent);
                if ( (!std::isfinite(extent) || extent <= 0.0) &&
                     std::isfinite(actualExtent) && actualExtent > 0.0 )
                {
                    extent = actualExtent;
                }
                if ( !std::isfinite(extent) || extent <= 0.0 )
                    extent = 1.0;
                return static_cast<float>(wxMin(
                    extent,
                    static_cast<double>(
                        std::numeric_limits<float>::max())));
            };
            const float width = usableExtent(
                clientDips.x, desired.Width, actualWidth);
            const float height = usableExtent(
                clientDips.y, desired.Height, actualHeight);

            combo.Measure({ width, height });
            liveOwner = getForcedOwner();
            if ( !liveOwner )
                return;
            combo.Arrange({ 0.0f, 0.0f, width, height });
            liveOwner = getForcedOwner();
            if ( !liveOwner )
                return;
            combo.UpdateLayout();
            liveOwner = getForcedOwner();
            if ( !liveOwner )
                return;

            MUXC::TextBox realized = wxWinUIFindTextBox(combo);
            liveOwner = getForcedOwner();
            if ( !liveOwner )
                return;

            if ( !realized && liveOwner->m_editable )
            {
                // EditableText is an x:Load element controlled by ComboBox's
                // IsEditable transition. If attachment happened while the
                // slot was unrealized, replay the real native transition once
                // under the ordinary peer-mutation guard; the final public
                // property remains editable and no synthetic TextBox is used.
                choiceState->BeginPeerMutation();
                wxScopeGuard mutationGuard = wxMakeGuard([choiceState]()
                {
                    choiceState->EndPeerMutation();
                });
                wxUnusedVar(mutationGuard);

                combo.IsEditable(false);
                liveOwner = getForcedOwner();
                if ( !liveOwner )
                    return;
                combo.IsEditable(true);
                liveOwner = getForcedOwner();
                if ( !liveOwner )
                    return;
                combo.ApplyTemplate();
                liveOwner = getForcedOwner();
                if ( !liveOwner )
                    return;
                combo.InvalidateMeasure();
                liveOwner = getForcedOwner();
                if ( !liveOwner )
                    return;
                combo.InvalidateArrange();
                liveOwner = getForcedOwner();
                if ( !liveOwner )
                    return;
                combo.Measure({ width, height });
                liveOwner = getForcedOwner();
                if ( !liveOwner )
                    return;
                combo.Arrange({ 0.0f, 0.0f, width, height });
                liveOwner = getForcedOwner();
                if ( !liveOwner )
                    return;
                combo.UpdateLayout();
                liveOwner = getForcedOwner();
                if ( !liveOwner )
                    return;
            }
        }

        const MUXC::TextBox candidate = wxWinUIFindTextBox(combo);
        liveOwner = getLiveOwner();
        if ( !liveOwner )
            return;

        if ( candidate != impl->editBox )
        {
            const MUXC::TextBox oldEditBox = impl->editBox;
            const long long oldTextChangedCallbackToken =
                impl->editTextChangedCallbackToken;
            const winrt::event_token oldSelectionToken =
                impl->editSelectionChangedToken;
            const winrt::event_token oldLayoutUpdatedToken =
                impl->editLayoutUpdatedToken;
            const winrt::event_token oldKeyDownToken =
                impl->editKeyDownToken;
            const winrt::event_token oldOuterKeyDownToken =
                impl->editOuterKeyDownToken;
            const winrt::event_token oldPointerPressedToken =
                impl->editPointerPressedToken;
            const winrt::event_token oldCopyingToken =
                impl->editCopyingToken;
            const winrt::event_token oldCuttingToken =
                impl->editCuttingToken;
            const winrt::event_token oldPasteToken =
                impl->editPasteToken;
            impl->editBox = nullptr;
            impl->editTextChangedCallbackToken = 0;
            impl->editSelectionChangedToken = {};
            impl->editLayoutUpdatedToken = {};
            impl->editKeyDownToken = {};
            impl->editOuterKeyDownToken = {};
            impl->editPointerPressedToken = {};
            impl->editCopyingToken = {};
            impl->editCuttingToken = {};
            impl->editPasteToken = {};
            if ( ++impl->editGeneration == 0 )
                ++impl->editGeneration;
            const std::uint64_t generation = impl->editGeneration;

            ++impl->autoCompleteGeneration;
            impl->autoCompleteSelection = wxNOT_FOUND;
            MUXC::MenuFlyout oldAutoCompleteFlyout =
                impl->autoCompleteFlyout;
            impl->autoCompleteFlyout = nullptr;
            const std::shared_ptr<wxWinUIFlyoutPopupRetirementState>
                oldAutoCompleteRetirementState =
                    impl->autoCompleteRetirementState;
            if ( oldAutoCompleteFlyout )
            {
                if ( !oldAutoCompleteRetirementState )
                    std::terminate();
                impl->autoCompleteRetirementPending = true;
                impl->autoCompleteReplayPending = true;
                try
                {
                    oldAutoCompleteFlyout.Hide();
                }
                catch ( const winrt::hresult_error& )
                {
                    wxWinUIFailFlyoutPopupShow(
                        oldAutoCompleteRetirementState);
                }
                liveOwner = getLiveOwner();
                if ( !liveOwner ||
                     liveOwner->m_winui->editGeneration != generation )
                {
                    return;
                }
            }

            wxWinUIRevokeComboEditHandlers(
                oldEditBox,
                oldEditBox.try_as<MUX::UIElement>(),
                impl->comboBox.try_as<MUX::UIElement>(),
                oldTextChangedCallbackToken,
                oldSelectionToken,
                oldLayoutUpdatedToken,
                oldKeyDownToken,
                oldOuterKeyDownToken,
                oldPointerPressedToken,
                oldCopyingToken,
                oldCuttingToken,
                oldPasteToken);
            liveOwner = getLiveOwner();
            if ( !liveOwner ||
                 liveOwner->m_winui->editGeneration != generation )
            {
                return;
            }

            winrt::event_token selectionToken{};
            if ( candidate )
            {
                try
                {
                    // Setting the newly discovered template part can
                    // synchronously propagate ComboBox.Text. Suppress that
                    // peer-originated notification while installing the
                    // model value, and revalidate ownership before touching
                    // the impl again.
                    choiceState->BeginPeerMutation();
                    wxScopeGuard mutationGuard =
                        wxMakeGuard([choiceState]()
                        {
                            choiceState->EndPeerMutation();
                        });
                    wxUnusedVar(mutationGuard);

                    candidate.Text(
                        wxWinUIToHString(liveOwner->m_value));
                    liveOwner = getLiveOwner();
                    if ( !liveOwner ||
                         liveOwner->m_winui->editGeneration != generation )
                    {
                        return;
                    }

                    candidate.IsReadOnly(!liveOwner->m_editable);
                    liveOwner = getLiveOwner();
                    if ( !liveOwner ||
                         liveOwner->m_winui->editGeneration != generation )
                    {
                        return;
                    }

                    candidate.MaxLength(0);
                    liveOwner = getLiveOwner();
                    if ( !liveOwner ||
                         liveOwner->m_winui->editGeneration != generation )
                    {
                        return;
                    }

                    if ( liveOwner->m_forceUpper )
                    {
                        candidate.CharacterCasing(
                            MUXC::CharacterCasing::Upper);
                        liveOwner = getLiveOwner();
                        if ( !liveOwner ||
                             liveOwner->m_winui->editGeneration != generation )
                        {
                            return;
                        }
                    }

                    if ( liveOwner->m_horizontalMargin >= 0 )
                    {
                        MUX::Thickness padding = candidate.Padding();
                        liveOwner = getLiveOwner();
                        if ( !liveOwner ||
                             liveOwner->m_winui->editGeneration != generation )
                        {
                            return;
                        }

                        const double horizontal = static_cast<double>(
                            liveOwner->ToDIP(wxSize(
                                liveOwner->m_horizontalMargin, 0)).x);
                        padding.Left = horizontal;
                        padding.Right = horizontal;
                        candidate.Padding(padding);
                        liveOwner = getLiveOwner();
                        if ( !liveOwner ||
                             liveOwner->m_winui->editGeneration != generation )
                        {
                            return;
                        }
                    }

                    if ( !liveOwner->m_hint.empty() )
                    {
                        candidate.PlaceholderText(
                            wxWinUIToHString(liveOwner->m_hint));
                        liveOwner = getLiveOwner();
                        if ( !liveOwner ||
                             liveOwner->m_winui->editGeneration != generation )
                        {
                            return;
                        }
                    }

                    selectionToken = candidate.SelectionChanged(
                        [callbackState, generation](
                            winrt::Windows::Foundation::IInspectable const&
                                sender,
                            MUX::RoutedEventArgs const&)
                        {
                            wxComboBox * const owner =
                                callbackState->GetOwner<wxComboBox>();
                            if ( !owner || !owner->m_winui ||
                                 owner->m_winui->textCallbackState !=
                                     callbackState ||
                                 owner->m_winui->editGeneration !=
                                     generation )
                            {
                                return;
                            }

                            // See the simple-editor handler above: a pending
                            // replacement ticket is narrower than the broad
                            // peer-mutation guard and must classify its exact
                            // SelectAll edge before ordinary events are
                            // suppressed.
                            if ( owner->HandlePendingTextSelectionChange() )
                                return;

                            if ( owner->m_winui->callbackState->
                                     IsPeerMutationInProgress() )
                            {
                                return;
                            }

                            try
                            {
                                const MUXC::TextBox editBox =
                                    sender.as<MUXC::TextBox>();
                                owner->m_selectionStart =
                                    editBox.SelectionStart();
                                owner->m_selectionEnd =
                                    owner->m_selectionStart +
                                    editBox.SelectionLength();
                                owner->m_insertionPoint =
                                    owner->m_selectionStart;
                                owner->ClampTextState();
                                owner->RefreshAutoComplete();
                            }
                            catch ( const winrt::hresult_error& )
                            {
                                return;
                            }

                            wxComboBox * const observedOwner =
                                callbackState->GetOwner<wxComboBox>();
                            if ( observedOwner && observedOwner->m_winui &&
                                 observedOwner->m_winui->textCallbackState ==
                                     callbackState &&
                                 observedOwner->m_winui->editGeneration ==
                                     generation )
                            {
#ifdef WXWINUI_TEST_SUPPORT
                                wxWinUIComboBoxTestAccess::
                                    RunIndependentRangeObservationHook(
                                        observedOwner);
#endif
                            }
                        });

                    liveOwner = getLiveOwner();
                    if ( !liveOwner ||
                         liveOwner->m_winui->editGeneration != generation )
                    {
                        candidate.SelectionChanged(selectionToken);
                        return;
                    }

                    liveOwner->m_winui->editBox = candidate;
                    liveOwner->m_winui->editSelectionChangedToken =
                        selectionToken;
                    if ( !liveOwner->AttachEditPeerHandlers() )
                    {
                        wxWinUIRevokeComboEditHandlers(
                            candidate, MUX::UIElement{ nullptr },
                            MUX::UIElement{ nullptr }, 0, selectionToken, {},
                            {}, {}, {}, {}, {}, {});
                        liveOwner = getLiveOwner();
                        if ( liveOwner &&
                             liveOwner->m_winui->editGeneration ==
                                 generation &&
                             liveOwner->m_winui->editBox == candidate )
                        {
                            liveOwner->m_winui->editBox = nullptr;
                            liveOwner->m_winui->
                                editSelectionChangedToken = {};
                        }
                        return;
                    }
                }
                catch ( const winrt::hresult_error& e )
                {
                    if ( selectionToken.value )
                    {
                        try
                        {
                            candidate.SelectionChanged(selectionToken);
                        }
                        catch ( const winrt::hresult_error& )
                        {
                        }
                    }
                    wxWinUILogException(
                        "WinUI ComboBox edit-part setup", e);
                }
            }
        }

        liveOwner = getLiveOwner();
        if ( liveOwner && liveOwner->m_winui->editBox )
        {
            liveOwner->m_winui->host.SetPreferredFocus(
                liveOwner->m_winui->editBox);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox template-part resolution", e);
    }
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIComboBoxTestAccess::SetPeerText(wxComboBox* control,
    const wxString& text)
{
    if ( !control->m_winui ||
         (!control->m_winui->comboBox &&
          !(control->m_winui->simpleListBox && control->m_winui->editBox)) )
    {
        return false;
    }

    // A peer edit is a focused-user transaction. Establish the public wx
    // focus intent before capturing an edit generation: SetFocus() can finish
    // a host migration or a theme/template replacement synchronously.
    if ( control->m_editable || control->m_winui->editBox )
    {
        const wxWeakRef<wxWindow> self(control);
        control->SetFocus();
        wxComboBox * const owner =
            wxDynamicCast(self.get(), wxComboBox);
        if ( !owner || !owner->m_winui || !owner->ResolveEditPart() ||
             !owner->m_winui || !owner->m_winui->editBox )
        {
            return false;
        }
    }

    wxWinUIChoiceImpl * const impl = control->m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const std::uint64_t editGeneration = impl->editGeneration;
    const MUXC::TextBox editBox = impl->editBox;
    wxComboBox * const expectedOwner = control;

    const auto getExactOwner = [&]() -> wxComboBox *
    {
        wxComboBox * const owner = callbackState
            ? callbackState->GetOwner<wxComboBox>()
            : nullptr;
        return owner == expectedOwner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == choiceState &&
                       owner->m_winui->textCallbackState == callbackState &&
                       owner->m_winui->editGeneration == editGeneration &&
                       owner->m_winui->editBox == editBox
                 ? owner
                 : nullptr;
    };

    try
    {
        wxString peerValue =
            wxWinUITextPositionMap::NormalizeNewlines(text);
        // This seam represents a user-originated mutation of the real edit
        // peer. CharacterCasing is applied by TextBox to typed input but not
        // to programmatic Text property writes, so mirror that native input
        // transformation before assigning the dependency property.
        if ( control->m_forceUpper )
            peerValue.MakeUpper();
        const winrt::hstring peerText = wxWinUIToHString(peerValue);
        if ( editBox )
        {
            // A completed user edit necessarily targets the focused native
            // editor. Flush a possibly new/reparented slot and focus that
            // exact generation before changing Text; MenuFlyout::ShowAt()
            // otherwise closes immediately and the next island key is routed
            // to a stale semantic target.
            impl->host.SetPreferredFocus(editBox);
            wxComboBox *owner = getExactOwner();
            if ( !owner )
                return false;
            owner->m_winui->host.SynchronizeForFocus();
            owner = getExactOwner();
            if ( !owner )
                return false;

            owner->m_winui->host.NavigateFocus(true);
            owner = getExactOwner();
            if ( !owner )
                return false;

            // UIElement::Focus() returns false when focus is already inside a
            // templated descendant. The shared slot reconciles authority from
            // FocusManager instead of this advisory boolean for the same
            // reason; an already-focused editor is a valid peer-edit target.
            (void)editBox.Focus(MUX::FocusState::Programmatic);
            owner = getExactOwner();
            if ( !owner || !choiceState )
                return false;

            owner->ClearPendingTextValue();
            owner->ClearPendingTextSelection();

            // A programmatic Text assignment raises its dependency-property
            // callback before TextBox lets us put the caret where a completed
            // user edit would already have left it. Close that artificial
            // half-state under the ordinary peer-mutation guard, then invoke
            // the real production reader once against the coherent Text+
            // selection snapshot. This also lets autocomplete observe the end
            // caret instead of opening and immediately retiring generation 0.
            choiceState->BeginPeerMutation();
            {
                wxScopeGuard mutationGuard = wxMakeGuard([choiceState]()
                {
                    choiceState->EndPeerMutation();
                });
                wxUnusedVar(mutationGuard);

                editBox.Text(peerText);
                owner = getExactOwner();
                if ( !owner )
                    return true;

                editBox.Select(static_cast<int32_t>(peerText.size()), 0);
                owner = getExactOwner();
                if ( !owner )
                    return true;
            }

            owner = getExactOwner();
            if ( !owner )
                return true;
            owner->OnPeerTextChanged();
        }
        else
        {
            control->ClearPendingTextValue();
            control->ClearPendingTextSelection();
            impl->comboBox.Text(peerText);
        }
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox peer text test mutation", e);
        return false;
    }
}

bool wxWinUIComboBoxTestAccess::SetPeerTextViaAutomation(wxComboBox* control,
    const wxString& text)
{
    if ( !control->m_winui )
        return false;

    // Keep a single UIAutomation client operation in flight. In particular,
    // don't replace its diagnostic authority while ResolveEditPart() or a
    // worker-side pattern call is still pending: the worker is deliberately
    // independent of the UI object and can't be cancelled without racing its
    // final SetValue(). Failed and completed operations may be replaced.
    if ( const auto active = control->m_winui->diagnosticAutomationOperation )
    {
        const int stage = active->stage.load(std::memory_order_acquire);
        if ( stage > 0 && stage != WinUIAutomation_Succeeded )
            return false;
    }

    wxComboBox * const expectedOwner = control;
    const wxWeakRef<wxWindow> ownerRef(control);
    const std::shared_ptr<wxWinUIComboAutomationTestState> operation =
        std::make_shared<wxWinUIComboAutomationTestState>();
    control->m_winui->diagnosticAutomationOperation = operation;
    control->m_winui->diagnosticAutomationStage = 1;
    operation->stage.store(1, std::memory_order_release);
    const auto failOnUIThread =
        [operation, ownerRef, expectedOwner](int stage, HRESULT hr) -> bool
    {
        operation->hresult.store(static_cast<long>(hr),
                                 std::memory_order_relaxed);
        operation->stage.store(stage, std::memory_order_release);
        wxComboBox * const owner =
            wxDynamicCast(ownerRef.get(), wxComboBox);
        if ( owner == expectedOwner && owner->m_winui &&
             owner->m_winui->diagnosticAutomationOperation == operation )
        {
            owner->m_winui->diagnosticAutomationStage = stage;
        }
        return false;
    };

    if ( !control->m_editable )
        return failOnUIThread(-2, UIA_E_NOTSUPPORTED);
    const bool resolved = control->ResolveEditPart();
    wxComboBox *liveOwner = wxDynamicCast(ownerRef.get(), wxComboBox);
    if ( !resolved || liveOwner != expectedOwner || !liveOwner->m_winui ||
         liveOwner->m_winui->diagnosticAutomationOperation != operation )
        return failOnUIThread(-3, UIA_E_ELEMENTNOTAVAILABLE);
    liveOwner->m_winui->diagnosticAutomationStage = 3;
    operation->stage.store(3, std::memory_order_release);
    if ( !liveOwner->m_winui->comboBox ||
         !liveOwner->m_winui->editBox ||
         !liveOwner->m_winui->callbackState ||
         !liveOwner->m_winui->textCallbackState )
        return failOnUIThread(-4, UIA_E_ELEMENTNOTAVAILABLE);

    const MUXC::ComboBox comboBox = liveOwner->m_winui->comboBox;
    const MUXC::TextBox editBox = liveOwner->m_winui->editBox;
    wxWinUIChoiceImpl * const impl = liveOwner->m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        liveOwner->m_winui->callbackState;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        liveOwner->m_winui->textCallbackState;
    const std::uint64_t editGeneration =
        liveOwner->m_winui->editGeneration;
    const auto getExactOwner = [&]() -> wxComboBox *
    {
        wxChoice * const choiceOwner = choiceState->GetOwner();
        wxComboBox * const textOwner =
            callbackState->GetOwner<wxComboBox>();
        return choiceOwner == expectedOwner && textOwner == expectedOwner &&
                       textOwner->m_winui &&
                       textOwner->m_winui.get() == impl &&
                       textOwner->m_winui->callbackState == choiceState &&
                       textOwner->m_winui->textCallbackState ==
                           callbackState &&
                       textOwner->m_winui->diagnosticAutomationOperation ==
                           operation &&
                       textOwner->m_winui->editGeneration ==
                           editGeneration &&
                       textOwner->m_winui->comboBox == comboBox &&
                       textOwner->m_winui->editBox == editBox
                 ? textOwner
                 : nullptr;
    };

    const HWND bridge = impl->host.GetBridgeHWND();
    if ( !bridge || !::IsWindow(bridge) )
        return failOnUIThread(-5, UIA_E_ELEMENTNOTAVAILABLE);

    // UIAutomationCore's ValuePattern synchronously enters provider focus.
    // A worker apartment in this same process still deadlocks there, so give
    // both exact elements unique semantic identities and launch the isolated
    // desktop runner in its external-client mode. The watcher owns only OS
    // handles and completion atomics, never wx or apartment-bound objects.
    static std::atomic<std::uint64_t> nextAutomationId{0};
    std::uint64_t id = nextAutomationId.fetch_add(
        1, std::memory_order_relaxed) + 1;
    if ( id == 0 )
        id = nextAutomationId.fetch_add(1, std::memory_order_relaxed) + 1;
    const std::wstring idBase =
        L"wxWidgets.ComboBox.UIA." +
        std::to_wstring(static_cast<unsigned long>(::GetCurrentProcessId())) +
        L"." + std::to_wstring(id);
    const std::wstring outerAutomationId = idBase + L".Outer";
    const std::wstring editAutomationId = idBase + L".Edit";
    const std::wstring requestedValue(text.wc_str());

    try
    {
        MUXA::AutomationProperties::SetAutomationId(
            comboBox, winrt::hstring(outerAutomationId));
        if ( !getExactOwner() )
            return failOnUIThread(-6, UIA_E_ELEMENTNOTAVAILABLE);
        MUXA::AutomationProperties::SetAutomationId(
            editBox, winrt::hstring(editAutomationId));
        if ( !getExactOwner() )
            return failOnUIThread(-7, UIA_E_ELEMENTNOTAVAILABLE);
    }
    catch ( const winrt::hresult_error& e )
    {
        return failOnUIThread(-7, e.code());
    }

    operation->hresult.store(static_cast<long>(S_OK),
                             std::memory_order_relaxed);
    operation->stage.store(WinUIAutomation_Scheduled,
                           std::memory_order_release);
    impl->diagnosticAutomationStage = WinUIAutomation_Scheduled;

    const wxWinUIUiaHelperLaunchResult launch =
        wxWinUILaunchUiaSetValueHelper(
            operation, bridge, outerAutomationId, editAutomationId,
            requestedValue);
    if ( !launch.scheduled )
        return failOnUIThread(-15, launch.hresult);
    return true;

}

bool wxWinUIComboBoxTestAccess::SelectPeerItem(wxComboBox* control,
    int selection)
{
    if ( !control->m_winui || (!control->m_winui->comboBox && !control->m_winui->simpleListBox) )
        return false;

    const std::shared_ptr<wxWinUIChoiceCallbackState> callbackState =
        control->m_winui->callbackState;
    wxWinUIChoiceImpl * const impl = control->m_winui.get();
    wxComboBox * const expectedOwner = control;

    try
    {
        // Realize the peer before mutation so the deterministic seam always
        // exercises WinUI's production ordering (TextProperty can precede
        // SelectionChanged). ForceRender() is a re-entrant boundary.
        impl->host.ForceRender();
        wxChoice * const choiceOwner = callbackState->GetOwner();
        if ( choiceOwner != expectedOwner )
            return false;
        wxComboBox * const owner =
            static_cast<wxComboBox *>(choiceOwner);
        if ( !owner->m_winui ||
             owner->m_winui.get() != impl ||
             owner->m_winui->callbackState != callbackState )
        {
            return false;
        }

        if ( impl->simpleListBox )
            impl->simpleListBox.SelectedIndex(selection);
        else
            impl->comboBox.SelectedIndex(selection);
        // SelectionChanged is allowed to destroy the control. Do not inspect
        // the owner or implementation after this call.
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox peer selection test mutation", e);
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

bool wxComboBox::NavigateSimpleList(int delta)
{
    if ( !m_winui || !m_winui->simpleListBox || GetCount() == 0 ||
         delta == 0 )
    {
        return false;
    }

    try
    {
        const int count = static_cast<int>(GetCount());
        const int current = m_winui->simpleListBox.SelectedIndex();
        int target;
        if ( current == wxNOT_FOUND )
            target = delta < 0 ? count - 1 : 0;
        else
            target = wxMax(0, wxMin(current + delta, count - 1));

        if ( target != current )
        {
            // SelectionChanged can synchronously delete this control. This is
            // intentionally the final operation before returning.
            m_winui->simpleListBox.SelectedIndex(target);
        }
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI simple ComboBox navigation", e);
        return false;
    }
}

int wxComboBox::GetSimpleListPageSize() const
{
    if ( !m_winui || !m_winui->simpleListBox )
        return 1;

    wxComboBox * const expectedOwner =
        const_cast<wxComboBox *>(this);
    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    const MUXC::ListBox list = impl->simpleListBox;
    const auto getLiveOwner = [&]() -> wxComboBox *
    {
        wxComboBox * const owner = callbackState
            ? callbackState->GetOwner<wxComboBox>()
            : nullptr;
        return owner == expectedOwner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->textCallbackState == callbackState &&
                       owner->m_winui->simpleListBox == list
                 ? owner
                 : nullptr;
    };

    try
    {
        double viewport = list.ActualHeight();
        const MUXC::ScrollViewer scroll = wxWinUIFindScrollViewer(list);
        if ( scroll )
        {
            // ItemsStackPanel uses logical scrolling: ViewportHeight is then
            // expressed in item units (e.g. 3.7), not DIPs. ActualHeight is
            // always a physical layout extent and can be compared with the
            // measured item heights below for both logical and pixel modes.
            const double scrollHeight = scroll.ActualHeight();
            if ( std::isfinite(scrollHeight) && scrollHeight > 0.0 )
                viewport = scrollHeight;
        }

        const MUXC::ItemCollection items = list.Items();
        const unsigned count = items.Size();
        double itemHeight = 0.0;
        for ( unsigned n = 0; n < count; ++n )
        {
            const MUX::FrameworkElement container =
                list.ContainerFromIndex(static_cast<int32_t>(n))
                    .try_as<MUX::FrameworkElement>();
            if ( !container )
                continue;

            const double height = container.ActualHeight();
            if ( std::isfinite(height) && height > 0.0 )
            {
                // Every item uses the same template and row policy. One real
                // realized ListBoxItem is therefore the stable line metric;
                // averaging content elements makes the result depend on which
                // rows virtualization happened to measure while compact.
                itemHeight = height;
                break;
            }
        }

        wxComboBox * const owner = getLiveOwner();
        if ( !owner || !std::isfinite(viewport) || viewport <= 0.0 )
            return 1;

        if ( itemHeight <= 0.0 )
        {
            const int charHeightDip = owner->ToDIP(
                wxSize(0, wxMax(1, owner->GetCharHeight()))).y;
            itemHeight = wxMax(24.0,
                               static_cast<double>(charHeightDip + 8));
        }

        const int pageSize = static_cast<int>(
            std::floor(viewport / wxMax(1.0, itemHeight)));
        return wxMax(1, wxMin(pageSize, static_cast<int>(count)));
    }
    catch ( const winrt::hresult_error& )
    {
        return 1;
    }
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIComboBoxTestAccess::NavigateSimpleList(wxComboBox* control,
    int delta)
{
    return control->NavigateSimpleList(delta);
}

int wxWinUIComboBoxTestAccess::GetSimplePageSize(const wxComboBox* control)
{
    return control->GetSimpleListPageSize();
}

unsigned wxWinUIComboBoxTestAccess::GetSimplePeerState(const wxComboBox* control,
    std::uintptr_t *editIdentity,
    std::uintptr_t *listIdentity)
{
    if ( editIdentity )
        *editIdentity = 0;
    if ( listIdentity )
        *listIdentity = 0;
    WinUISimplePeerSnapshot snapshot;
    if ( !wxWinUIComboBoxTestAccess::GetSimplePeerSnapshot(control, &snapshot) )
        return 0;
    if ( editIdentity )
        *editIdentity = snapshot.editIdentity;
    if ( listIdentity )
        *listIdentity = snapshot.listIdentity;
    return snapshot.state;
}

bool wxWinUIComboBoxTestAccess::GetSimplePeerSnapshot(const wxComboBox* control,
    WinUISimplePeerSnapshot *snapshot)
{
    if ( !snapshot )
        return false;
    *snapshot = WinUISimplePeerSnapshot();
    // This is deliberately passive. Creation and DoSetSize() own realization;
    // a polling diagnostic must never resolve, focus or arrange the surface it
    // is measuring.
    if ( !control->m_winui || !control->m_winui->simpleRoot || !control->m_winui->editBox ||
         !control->m_winui->simpleListBox )
    {
        return false;
    }

    wxComboBox * const expectedOwner =
        const_cast<wxComboBox *>(control);
    wxWinUIChoiceImpl * const impl = control->m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    const std::uint64_t editGeneration = impl->editGeneration;
    const MUXC::Grid root = impl->simpleRoot;
    const MUXC::TextBox edit = impl->editBox;
    const MUXC::ListBox list = impl->simpleListBox;
    const unsigned itemCount = control->GetCount();
    const auto getLiveOwner = [&]() -> wxComboBox *
    {
        wxChoice * const choiceOwner =
            choiceState ? choiceState->GetOwner() : nullptr;
        wxComboBox * const textOwner =
            textState ? textState->GetOwner<wxComboBox>() : nullptr;
        return choiceOwner == expectedOwner && textOwner == expectedOwner &&
                       textOwner->m_winui &&
                       textOwner->m_winui.get() == impl &&
                       textOwner->m_winui->callbackState == choiceState &&
                       textOwner->m_winui->textCallbackState == textState &&
                       textOwner->m_winui->editGeneration == editGeneration &&
                       textOwner->m_winui->simpleRoot == root &&
                       textOwner->m_winui->editBox == edit &&
                       textOwner->m_winui->simpleListBox == list
                 ? textOwner
                 : nullptr;
    };

    WinUISimplePeerSnapshot result;
    result.state = WinUISimple_HasRoot | WinUISimple_HasEdit |
                   WinUISimple_HasList;
    if ( choiceState->IsPeerMutationInProgress() )
        result.phase |= WinUITemplatePhase_PeerMutation;
    if ( impl->hasPendingTextValue )
        result.phase |= WinUITemplatePhase_PendingText;
    if ( impl->hasPendingTextSelection )
        result.phase |= WinUITemplatePhase_PendingRange;
    result.peerItemsValid = impl->peerItemsValid;
    result.sourceWidthPixels = impl->diagnosticSimpleSourceWidthPixels;
    result.sourceHeightPixels = impl->diagnosticSimpleSourceHeightPixels;
    result.requestedWidth = impl->diagnosticSimpleComputedWidth;
    result.requestedHeight = impl->diagnosticSimpleComputedHeight;
    result.layoutEpoch = impl->simpleLayoutEpoch;
    result.layoutAppliedEpoch = impl->simpleLayoutAppliedEpoch;
    result.layoutQueued = impl->simpleLayoutQueued;
    result.layoutInProgress = impl->simpleLayoutInProgress;
    result.logicalFocus = wxWindow::FindFocus() == control;
    result.nativeFocusInHost = impl->host.ContainsFocus(::GetFocus());
    result.editIdentity = reinterpret_cast<std::uintptr_t>(
        winrt::get_abi(wxWinUIComboObjectIdentity(edit)));
    result.listIdentity = reinterpret_cast<std::uintptr_t>(
        winrt::get_abi(wxWinUIComboObjectIdentity(list)));

    try
    {
        result.editFocused =
            edit.FocusState() != MUX::FocusState::Unfocused;
        result.peerTextSelectionStart = edit.SelectionStart();
        result.peerTextSelectionLength = edit.SelectionLength();
        result.rootHeight = root.ActualHeight();
        result.peerSelection = list.SelectedIndex();
        if ( list.Visibility() == MUX::Visibility::Visible )
            result.state |= WinUISimple_ListVisible;
        if ( root.RowDefinitions().Size() == 2 )
            result.state |= WinUISimple_HasTwoRows;

        const auto children = root.Children();
        if ( children.Size() == 2 &&
             wxWinUIComboObjectIdentity(children.GetAt(0)) ==
                 wxWinUIComboObjectIdentity(edit) &&
             wxWinUIComboObjectIdentity(children.GetAt(1)) ==
                 wxWinUIComboObjectIdentity(list) )
        {
            result.state |= WinUISimple_ChildOrder;
        }
        if ( !impl->comboBox )
            result.state |= WinUISimple_NoComboPeer;
        if ( list.Items().Size() == itemCount )
            result.state |= WinUISimple_ItemsMatch;

        const MUX::UIElement hosted = impl->host.GetContent();
        if ( hosted &&
             wxWinUIComboObjectIdentity(hosted) ==
                 wxWinUIComboObjectIdentity(root) )
            result.state |= WinUISimple_HostsRoot;

        result.editRow = MUXC::Grid::GetRow(edit);
        result.listRow = MUXC::Grid::GetRow(list);
        if ( root.RowDefinitions().Size() == 2 )
        {
            const MUX::GridLength listRowHeight =
                root.RowDefinitions().GetAt(1).Height();
            result.listRowIsPixel =
                listRowHeight.GridUnitType == MUX::GridUnitType::Pixel;
            result.listRowHeight = listRowHeight.Value;
        }
        if ( result.editRow == 0 && result.listRow == 1 )
            result.state |= WinUISimple_RowsAssigned;

        const auto editPoint =
            edit.TransformToVisual(root).TransformPoint({ 0.0f, 0.0f });
        const auto listPoint =
            list.TransformToVisual(root).TransformPoint({ 0.0f, 0.0f });
        result.editX = editPoint.X;
        result.editY = editPoint.Y;
        result.editWidth = edit.ActualWidth();
        result.editHeight = edit.ActualHeight();
        result.listX = listPoint.X;
        result.listY = listPoint.Y;
        result.listWidth = list.ActualWidth();
        result.listHeight = list.ActualHeight();
        result.listDesiredHeight = list.DesiredSize().Height;
        result.listLocalHeight = list.Height();
        result.listMinHeight = list.MinHeight();
        result.listMaxHeight = list.MaxHeight();
        if ( std::isfinite(result.editX) &&
             std::isfinite(result.editY) &&
             std::isfinite(result.editWidth) &&
             std::isfinite(result.editHeight) &&
             std::isfinite(result.listX) &&
             std::isfinite(result.listY) &&
             std::isfinite(result.listWidth) &&
             std::isfinite(result.listHeight) &&
             result.editWidth > 0.0 && result.editHeight > 0.0 &&
             result.listWidth > 0.0 && result.listHeight > 0.0 )
        {
            result.state |= WinUISimple_RectsValid |
                            WinUISimple_ListArranged;
            if ( result.listY + 0.5 >=
                 result.editY + result.editHeight )
            {
                result.state |= WinUISimple_ListBelowEdit;
            }
            if ( std::abs(result.editX - result.listX) <= 1.0 &&
                 std::abs(result.editWidth - result.listWidth) <= 1.0 )
            {
                result.state |= WinUISimple_WidthsAligned;
            }
        }

        const MUXC::ScrollViewer scroll = wxWinUIFindScrollViewer(list);
        if ( scroll )
            result.viewportHeight = scroll.ActualHeight();
        if ( std::isfinite(result.viewportHeight) &&
             result.viewportHeight > 0.0 )
        {
            result.state |= WinUISimple_HasViewport;
        }

        const MUX::XamlRoot rootXaml = root.XamlRoot();
        const MUX::XamlRoot editXaml = edit.XamlRoot();
        const MUX::XamlRoot listXaml = list.XamlRoot();
        if ( rootXaml && editXaml && listXaml &&
             wxWinUIComboObjectIdentity(rootXaml) ==
                 wxWinUIComboObjectIdentity(editXaml) &&
             wxWinUIComboObjectIdentity(rootXaml) ==
                 wxWinUIComboObjectIdentity(listXaml) )
        {
            result.xamlRootIdentity = reinterpret_cast<std::uintptr_t>(
                winrt::get_abi(wxWinUIComboObjectIdentity(rootXaml)));
            result.state |= WinUISimple_HasXamlRoot;
            result.rasterizationScale = rootXaml.RasterizationScale();
            if ( std::isfinite(result.rasterizationScale) &&
                 result.rasterizationScale > 0.0 )
            {
                result.state |= WinUISimple_ScaleValid;
            }
        }

        const MUX::ElementTheme rootTheme = root.ActualTheme();
        const MUX::ElementTheme editTheme = edit.ActualTheme();
        const MUX::ElementTheme listTheme = list.ActualTheme();
        result.actualTheme = static_cast<int>(rootTheme);
        if ( rootTheme == editTheme && rootTheme == listTheme )
            result.state |= WinUISimple_ThemeObserved;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }

    if ( !getLiveOwner() )
        return false;
    *snapshot = result;
    return true;
}

bool wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(const wxComboBox* control,
    WinUITemplatePeerSnapshot *snapshot)
{
    if ( !snapshot )
        return false;
    *snapshot = WinUITemplatePeerSnapshot();

    // This is deliberately a passive observation seam. Realization and retry
    // belong to Loaded/LayoutUpdated and explicit production operations; a
    // polling diagnostic must not call ResolveEditPart() and thereby change
    // the very transition phase it is reporting.
    if ( !control->m_winui || control->m_winui->simpleRoot || !control->m_winui->comboBox ||
         !control->m_winui->callbackState || !control->m_winui->textCallbackState )
    {
        return false;
    }

    wxComboBox * const expectedOwner = const_cast<wxComboBox *>(control);
    wxWinUIChoiceImpl * const impl = control->m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    const MUXC::Grid visualRoot = impl->editableRoot;
    const MUXC::ComboBox combo = impl->comboBox;
    const MUXC::TextBox edit = impl->editBox;
    const std::uint64_t editGeneration = impl->editGeneration;
    const auto getExactOwner = [&]() -> wxComboBox *
    {
        wxChoice * const choiceOwner = choiceState->GetOwner();
        wxComboBox * const textOwner =
            textState->GetOwner<wxComboBox>();
        return choiceOwner == expectedOwner && textOwner == expectedOwner &&
                       textOwner->m_winui &&
                       textOwner->m_winui.get() == impl &&
                        textOwner->m_winui->callbackState == choiceState &&
                        textOwner->m_winui->textCallbackState == textState &&
                        textOwner->m_winui->editableRoot == visualRoot &&
                        textOwner->m_winui->comboBox == combo &&
                       textOwner->m_winui->editGeneration == editGeneration &&
                       textOwner->m_winui->editBox == edit
                 ? textOwner
                 : nullptr;
    };
    if ( !getExactOwner() )
        return false;

    WinUITemplatePeerSnapshot result;
    result.state = WinUITemplate_HasCombo;
    result.editGeneration = editGeneration;
    if ( impl->comboPartResolving )
        result.phase |= WinUITemplatePhase_Resolving;
    if ( impl->comboPartPending )
        result.phase |= WinUITemplatePhase_PendingResolve;
    if ( impl->comboLayoutResolveQueued )
        result.phase |= WinUITemplatePhase_QueuedResolve;
    if ( impl->comboTemplateTransition )
        result.phase |= WinUITemplatePhase_Transition;
    if ( choiceState->IsPeerMutationInProgress() )
        result.phase |= WinUITemplatePhase_PeerMutation;
    if ( impl->hasPendingTextValue )
        result.phase |= WinUITemplatePhase_PendingText;
    if ( impl->hasPendingTextSelection )
        result.phase |= WinUITemplatePhase_PendingRange;
    result.logicalFocus = wxWindow::FindFocus() == control;
    result.nativeFocusInHost = impl->host.ContainsFocus(::GetFocus());
    result.hostContentIdentity = reinterpret_cast<std::uintptr_t>(
        winrt::get_abi(wxWinUIComboObjectIdentity(
            impl->host.GetContent())));
    if ( visualRoot )
    {
        result.visualRootIdentity = reinterpret_cast<std::uintptr_t>(
            winrt::get_abi(wxWinUIComboObjectIdentity(visualRoot)));
        result.state |= WinUITemplate_HasVisualRoot;
    }
    result.comboIdentity = reinterpret_cast<std::uintptr_t>(
        winrt::get_abi(wxWinUIComboObjectIdentity(combo)));
    if ( edit )
    {
        result.editIdentity = reinterpret_cast<std::uintptr_t>(
            winrt::get_abi(wxWinUIComboObjectIdentity(edit)));
        result.state |= WinUITemplate_HasEdit;
    }

    try
    {
        if ( combo.IsEditable() )
            result.state |= WinUITemplate_IsEditable;

        if ( edit )
        {
            result.peerSelectionStart = edit.SelectionStart();
            result.peerSelectionLength = edit.SelectionLength();
            result.editFocused =
                edit.FocusState() != MUX::FocusState::Unfocused;
        }

        if ( visualRoot )
        {
            result.visualRootLogicalParentIdentity =
                reinterpret_cast<std::uintptr_t>(
                    winrt::get_abi(wxWinUIComboObjectIdentity(
                        visualRoot.Parent())));
            result.visualRootVisualParentIdentity =
                reinterpret_cast<std::uintptr_t>(
                    winrt::get_abi(wxWinUIComboObjectIdentity(
                        MUX::Media::VisualTreeHelper::GetParent(
                            visualRoot))));
            result.visualRootChildCount =
                MUX::Media::VisualTreeHelper::GetChildrenCount(visualRoot);
            if ( result.visualRootChildCount == 1 &&
                 MUX::Media::VisualTreeHelper::GetChild(visualRoot, 0) == combo )
            {
                result.state |= WinUITemplate_RootOwnsCombo;
            }

            const MUX::XamlRoot rootXaml = visualRoot.XamlRoot();
            if ( rootXaml )
            {
                result.visualRootXamlRootIdentity =
                    reinterpret_cast<std::uintptr_t>(
                        winrt::get_abi(
                            wxWinUIComboObjectIdentity(rootXaml)));
                result.state |= WinUITemplate_RootHasXamlRoot;
            }
        }

        const MUX::XamlRoot comboXaml = combo.XamlRoot();
        if ( comboXaml )
        {
            result.xamlRootIdentity = reinterpret_cast<std::uintptr_t>(
                winrt::get_abi(
                    wxWinUIComboObjectIdentity(comboXaml)));
            result.state |= WinUITemplate_HasXamlRoot;
        }

        const MUXC::ControlTemplate controlTemplate = combo.Template();
        if ( controlTemplate )
        {
            result.templateIdentity = reinterpret_cast<std::uintptr_t>(
                winrt::get_abi(
                    wxWinUIComboObjectIdentity(controlTemplate)));
            result.state |= WinUITemplate_HasTemplate;
        }

        result.visualChildCount =
            MUX::Media::VisualTreeHelper::GetChildrenCount(combo);
        if ( result.visualChildCount > 0 )
            result.state |= WinUITemplate_HasVisualChild;

        result.actualWidth = combo.ActualWidth();
        result.actualHeight = combo.ActualHeight();
        if ( std::isfinite(result.actualWidth) &&
             std::isfinite(result.actualHeight) &&
             result.actualWidth > 0.0 && result.actualHeight > 0.0 )
        {
            result.state |= WinUITemplate_IsArranged;
        }

        const auto desired = combo.DesiredSize();
        result.desiredWidth = desired.Width;
        result.desiredHeight = desired.Height;
        if ( std::isfinite(result.desiredWidth) &&
             std::isfinite(result.desiredHeight) &&
             result.desiredWidth > 0.0 && result.desiredHeight > 0.0 )
        {
            result.state |= WinUITemplate_HasDesiredSize;
        }

        if ( comboXaml && edit )
        {
            const MUX::XamlRoot editXaml = edit.XamlRoot();
            if ( editXaml &&
                 wxWinUIComboObjectIdentity(editXaml) ==
                     wxWinUIComboObjectIdentity(comboXaml) )
            {
                result.state |= WinUITemplate_EditSharesRoot;
            }
        }

        const HWND hwnd = static_cast<HWND>(control->GetHWND());
        const HWND bridge = impl->host.GetBridgeHWND();
        RECT rect{};
        if ( hwnd && ::GetWindowRect(hwnd, &rect) )
        {
            result.windowWidth = rect.right - rect.left;
            result.windowHeight = rect.bottom - rect.top;
        }
        if ( bridge && ::GetClientRect(bridge, &rect) )
        {
            result.bridgeWidth = rect.right - rect.left;
            result.bridgeHeight = rect.bottom - rect.top;
        }
        result.windowVisible = hwnd && ::IsWindowVisible(hwnd);
        result.windowShownOnScreen = control->IsShownOnScreen();
        result.bridgeVisible = bridge && ::IsWindowVisible(bridge);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox template peer snapshot", e);
        return false;
    }

    if ( !getExactOwner() )
        return false;
    *snapshot = result;
    return true;
}

bool wxWinUIComboBoxTestAccess::Retemplate(wxComboBox* control)
{
    if ( !control->m_winui || !control->m_winui->callbackState ||
         !control->m_winui->textCallbackState || control->m_winui->simpleRoot ||
         !control->m_winui->comboBox || !control->m_winui->editBox )
        return false;

    wxWinUIChoiceImpl * const impl = control->m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    const MUXC::ComboBox combo = impl->comboBox;
    if ( !choiceState )
        return false;

    wxComboBox * const expectedOwner = control;
    const auto getLiveOwner = [&]() -> wxComboBox *
    {
        wxChoice * const choiceOwner =
            choiceState ? choiceState->GetOwner() : nullptr;
        wxComboBox * const textOwner =
            textState ? textState->GetOwner<wxComboBox>() : nullptr;
        return choiceOwner == expectedOwner && textOwner == expectedOwner &&
                       textOwner->m_winui &&
                       textOwner->m_winui.get() == impl &&
                       textOwner->m_winui->callbackState == choiceState &&
                       textOwner->m_winui->textCallbackState == textState &&
                       textOwner->m_winui->comboBox == combo
                 ? textOwner
                 : nullptr;
    };

    const auto transitionAcceptedSince =
        [&](std::uint64_t generation) -> bool
        {
            wxComboBox * const owner = getLiveOwner();
            if ( !owner || owner->m_winui->comboTemplateTransition ||
                 owner->m_winui->editGeneration == generation )
            {
                return false;
            }

            const wxWinUIChoiceImpl * const liveImpl = owner->m_winui.get();
            return liveImpl->comboTemplateReplayPending || liveImpl->editBox;
        };

    // Theme-boundary reconciliation can itself accept and begin the natural
    // transition. Its false return means "deferred", not necessarily failure;
    // recognize the retired generation before attempting any second request.
    const MUXC::TextBox editBeforeBoundary = impl->editBox;
    const std::uint64_t generationBeforeBoundary = impl->editGeneration;
    const auto acceptBoundaryTransition = [&]() -> bool
    {
        if ( !transitionAcceptedSince(generationBeforeBoundary) )
            return false;

        wxComboBox * const owner = getLiveOwner();
        if ( !owner )
            return false;

        // EnsureThemeTransitionBoundary() and ResolveEditPart() can themselves
        // accept the deferred transition. Preserve the exact outgoing editor
        // before either boundary so the stale-sender seam never falls back to
        // a projection retained by an earlier generation.
        owner->m_winui->retiredEditBoxForTesting = editBeforeBoundary;
        return true;
    };
    if ( !control->EnsureThemeTransitionBoundary() )
        return acceptBoundaryTransition();

    if ( !control->ResolveEditPart() )
        return acceptBoundaryTransition();

    wxComboBox *owner = getLiveOwner();
    if ( !owner || !owner->m_winui->editBox )
        return false;

    const MUXC::TextBox oldEdit = owner->m_winui->editBox;
    const std::uint64_t oldGeneration = owner->m_winui->editGeneration;

    // Exercise the exact production retirement/rebuild transaction. The seam
    // retains the outgoing part only so tests can prove that its revoked
    // callbacks cannot mutate the committed selection afterwards.
    owner->m_winui->retiredEditBoxForTesting = oldEdit;
    owner->OnPeerTemplateTransition(true);

    owner = getLiveOwner();
    if ( !owner || owner->m_winui->comboTemplateTransition )
        return false;

    // Acceptance is identified by retirement of the outgoing callback
    // generation, not by an immediate change of TextBox identity. WinUI may
    // reuse the same template part when the queued natural-layout resolver
    // commits the replacement generation.
    return transitionAcceptedSince(oldGeneration);
}

bool wxWinUIComboBoxTestAccess::RunTemplateLayoutEdge(wxComboBox* control,
    bool forceTransition)
{
    if ( !control->m_winui || !control->m_winui->callbackState ||
         !control->m_winui->textCallbackState || control->m_winui->simpleRoot ||
         !control->m_winui->comboBox || !control->m_winui->editBox )
    {
        return false;
    }

    wxWinUIChoiceImpl * const impl = control->m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    const MUXC::TextBox oldEdit = impl->editBox;
    const std::uint64_t oldGeneration = impl->editGeneration;
    wxComboBox * const expectedOwner = control;
    const auto getExactOwner = [&]() -> wxComboBox *
    {
        wxChoice * const choiceOwner = choiceState->GetOwner();
        wxComboBox * const textOwner = textState->GetOwner<wxComboBox>();
        return choiceOwner == expectedOwner && textOwner == expectedOwner &&
                       textOwner->m_winui &&
                       textOwner->m_winui.get() == impl &&
                       textOwner->m_winui->callbackState == choiceState &&
                       textOwner->m_winui->textCallbackState == textState
                 ? textOwner
                 : nullptr;
    };

    // A forced edge enters the exact production transition. Success means the
    // outgoing generation was retired and its deferred replay was accepted;
    // it does not promise synchronous replacement-part realization.
    if ( forceTransition )
    {
        impl->retiredEditBoxForTesting = oldEdit;
        control->OnPeerTemplateTransition(true);
    }
    else
    {
        control->OnPeerLayoutUpdated();
    }

    wxComboBox * const owner = getExactOwner();
    if ( !owner || owner->m_winui->comboTemplateTransition ||
         (!forceTransition && !owner->m_winui->editBox) )
    {
        return false;
    }

    if ( !forceTransition )
        return true;

    const wxWinUIChoiceImpl * const liveImpl = owner->m_winui.get();
    const bool replayAccepted = liveImpl->comboTemplateReplayPending;
    const bool replayCompleted =
        liveImpl->editBox && !liveImpl->comboTemplateReplayPending;
    return liveImpl->editGeneration != oldGeneration &&
           (replayAccepted || replayCompleted);
}

bool wxWinUIComboBoxTestAccess::QueueTemplateLayoutResolution(wxComboBox* control)
{
    if ( !control->m_winui || !control->m_winui->callbackState ||
         !control->m_winui->textCallbackState || control->m_winui->simpleRoot ||
         !control->m_winui->editableRoot || !control->m_winui->comboBox ||
         !control->m_winui->editBox || control->m_winui->comboLayoutResolveQueued )
    {
        return false;
    }

    wxWinUIChoiceImpl * const impl = control->m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    const MUXC::Grid editableRoot = impl->editableRoot;
    const MUXC::ComboBox combo = impl->comboBox;
    const MUXC::TextBox edit = impl->editBox;
    const std::uint64_t editGeneration = impl->editGeneration;
    const std::uint64_t revisionBefore =
        impl->comboLayoutResolveRevision;
    const std::uint64_t requestsBefore =
        impl->diagnosticComboLayoutResolveRequests;
    const std::uint64_t runsBefore =
        impl->diagnosticComboLayoutResolveRuns;
    const std::uint64_t edgesBefore =
        impl->diagnosticComboLayoutEdges;
    const std::uint64_t coalescedBefore =
        impl->diagnosticComboLayoutCoalescedEdges;
    const std::uint64_t synchronousBefore =
        impl->diagnosticComboLayoutSynchronousRealizations;
    wxComboBox * const expectedOwner = control;
    const auto getExactOwner = [&]() -> wxComboBox *
    {
        wxChoice * const choiceOwner = choiceState->GetOwner();
        wxComboBox * const textOwner =
            textState->GetOwner<wxComboBox>();
        return choiceOwner == expectedOwner && textOwner == expectedOwner &&
                       textOwner->m_winui &&
                       textOwner->m_winui.get() == impl &&
                       textOwner->m_winui->callbackState == choiceState &&
                       textOwner->m_winui->textCallbackState == textState &&
                       textOwner->m_winui->editableRoot == editableRoot &&
                       textOwner->m_winui->comboBox == combo &&
                       textOwner->m_winui->editBox == edit &&
                       textOwner->m_winui->editGeneration == editGeneration
                 ? textOwner
                 : nullptr;
    };
    if ( !getExactOwner() )
        return false;

    // Keep the outgoing COM object alive across the deferred teardown. Without
    // this pin the allocator may legitimately reuse its ABI address for the
    // replacement TextBox, turning the test's identity oracle into a false
    // timeout even though editGeneration and template replay both advanced.
    impl->retiredEditBoxForTesting = edit;

    // Force a destructive retemplate in the deferred continuation, then enter
    // the genuine callback a second time before the dispatcher can run. On
    // return the old editor must still be authoritative, the second edge must
    // have coalesced into the first ticket, and no forced realization may have
    // run synchronously inside either LayoutUpdated callback.
    wxWinUIComboLayoutTestRequest request(
        control, impl, choiceState, textState);
    control->OnPeerLayoutUpdated();
    control->OnPeerLayoutUpdated();

    wxComboBox * const owner = getExactOwner();
    return owner && owner->m_winui->comboLayoutResolveQueued &&
           !owner->m_winui->comboLayoutResolveRunning &&
           owner->m_winui->comboLayoutCallbackDepth == 0 &&
           owner->m_winui->comboLayoutResolveRevision != revisionBefore &&
           owner->m_winui->diagnosticComboLayoutEdges == edgesBefore + 2 &&
           owner->m_winui->diagnosticComboLayoutResolveRequests ==
               requestsBefore + 1 &&
           owner->m_winui->diagnosticComboLayoutResolveRuns == runsBefore &&
           owner->m_winui->diagnosticComboLayoutCoalescedEdges ==
               coalescedBefore + 1 &&
           owner->m_winui->diagnosticComboLayoutSynchronousRealizations ==
               synchronousBefore;
}

bool wxWinUIComboBoxTestAccess::SetEditSelection(wxComboBox* control,
    long from,
    long to,
    bool retiredPart)
{
    if ( (!retiredPart && !control->ResolveEditPart()) || !control->m_winui )
        return false;

    const MUXC::TextBox editBox =
        retiredPart ? control->m_winui->retiredEditBoxForTesting
                    : control->m_winui->editBox;
    if ( !editBox )
        return false;

    const long len =
        wxWinUITextPositionMap(control->m_value, false, false).
            GetLastPosition();
    from = wxWinUIClampComboTextPos(from, len);
    to = to < 0 ? len : wxWinUIClampComboTextPos(to, len);
    if ( to < from )
        wxSwap(from, to);

    // This seam is a real peer-side writer. The current replacement editor's
    // native range supersedes its template-transition correction ticket;
    // mutating the retained retired editor must remain completely passive.
    if ( !retiredPart )
        control->ClearPendingTextSelection();

    try
    {
        editBox.Select(
            static_cast<int32_t>(from),
            static_cast<int32_t>(to - from));
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox edit selection test mutation", e);
        return false;
    }
}

bool wxWinUIComboBoxTestAccess::SetRawEditSelection(wxComboBox* control,
    long from, long to)
{
    if ( !control->ResolveEditPart() || !control->m_winui || !control->m_winui->editBox )
        return false;

    const long len =
        wxWinUITextPositionMap(control->m_value, false, false).
            GetLastPosition();
    from = wxWinUIClampComboTextPos(from, len);
    to = to < 0 ? len : wxWinUIClampComboTextPos(to, len);
    if ( to < from )
        wxSwap(from, to);

    try
    {
        // Deliberately do not clear the transition ticket here. The real
        // TextBox.SelectionChanged callback must classify this raw peer edge
        // as either the template's SelectAll or an independent writer.
        control->m_winui->editBox.Select(
            static_cast<int32_t>(from),
            static_cast<int32_t>(to - from));
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox raw edit selection test mutation", e);
        return false;
    }
}

bool wxWinUIComboBoxTestAccess::ChainPendingRangeCallbacks(wxComboBox* control,
    long independentFrom,
    long independentTo,
    const std::shared_ptr<WinUIRangeSequenceSnapshot>& snapshot)
{
    if ( !snapshot )
        return false;
    *snapshot = WinUIRangeSequenceSnapshot();

    // Do not resolve or arrange here: the transition creator established the
    // exact editor and its ticket, and its deferred SelectionChanged may
    // already be queued. Crossing another layout boundary would change the
    // event whose ordering this seam is intended to prove.
    if ( !control->m_winui || !control->m_winui->editBox || !control->m_winui->callbackState ||
         !control->m_winui->textCallbackState ||
         !control->m_winui->hasPendingTextSelection ||
         control->m_winui->pendingRangeProjectionHookForTesting )
    {
        return false;
    }

    const long peerLast =
        wxWinUITextPositionMap(control->m_value, false, false).GetLastPosition();
    independentFrom = wxWinUIClampComboTextPos(independentFrom, peerLast);
    independentTo = independentTo < 0
                        ? peerLast
                        : wxWinUIClampComboTextPos(independentTo, peerLast);
    if ( independentTo < independentFrom )
        wxSwap(independentFrom, independentTo);

    wxWinUIChoiceImpl * const impl = control->m_winui.get();
    const long expectedFrom = wxMin(impl->pendingTextSelectionStart,
                                    impl->pendingTextSelectionEnd);
    const long expectedTo = wxMax(impl->pendingTextSelectionStart,
                                  impl->pendingTextSelectionEnd);
    if ( (independentFrom == 0 && independentTo == peerLast) ||
         (independentFrom == expectedFrom &&
          independentTo == expectedTo) )
    {
        return false;
    }

    const std::shared_ptr<wxWinUIChoiceCallbackState> choiceState =
        impl->callbackState;
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    const MUXC::TextBox editBox = impl->editBox;
    const std::uint64_t generation = impl->editGeneration;
    const std::uint64_t epoch = impl->pendingTextSelectionEpoch;
    const std::uintptr_t editIdentity =
        impl->pendingTextSelectionEditIdentity;
    wxComboBox * const expectedOwner = control;
    const auto getLiveOwner =
        [choiceState, textState, impl, editBox, generation,
         expectedOwner]() -> wxComboBox *
    {
        wxChoice * const choiceOwner = choiceState->GetOwner();
        wxComboBox * const textOwner =
            textState->GetOwner<wxComboBox>();
        return choiceOwner == expectedOwner && textOwner == expectedOwner &&
                       textOwner->m_winui &&
                       textOwner->m_winui.get() == impl &&
                       textOwner->m_winui->callbackState == choiceState &&
                       textOwner->m_winui->textCallbackState == textState &&
                       textOwner->m_winui->editGeneration == generation &&
                       textOwner->m_winui->editBox == editBox
                 ? textOwner
                 : nullptr;
    };
    const auto hasExactTicket =
        [impl, epoch, editIdentity](wxComboBox *owner)
    {
        return owner && owner->m_winui &&
               owner->m_winui.get() == impl &&
               owner->m_winui->hasPendingTextSelection &&
               owner->m_winui->pendingTextSelectionEpoch == epoch &&
               owner->m_winui->pendingTextSelectionEditIdentity ==
                   editIdentity;
    };

    long currentFrom = -1;
    long currentTo = -1;
    try
    {
        currentFrom = editBox.SelectionStart();
        currentTo = currentFrom + editBox.SelectionLength();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox staged range initial read", e);
        return false;
    }
    const bool currentIsExpected =
        currentFrom == expectedFrom && currentTo == expectedTo;
    const bool currentIsSelectAll =
        currentFrom == 0 && currentTo == peerLast;
    if ( !currentIsExpected && !currentIsSelectAll )
        return false;

    std::function<void(wxComboBox *)> secondProjectionHook =
        [snapshot, getLiveOwner, hasExactTicket, impl, choiceState,
         textState, editBox, generation, epoch, editIdentity, expectedOwner,
         expectedFrom, expectedTo, independentFrom, independentTo](
            wxComboBox *callbackOwner)
        {
            wxComboBox * const owner = getLiveOwner();
            if ( owner != callbackOwner || !hasExactTicket(owner) )
                return;
            const std::uint64_t revision =
                impl->pendingTextSelectionRevision;

            try
            {
                snapshot->secondRangeProjectionMismatches =
                    impl->diagnosticRangeProjectionMismatches;
                snapshot->secondRangeFinalizations =
                    impl->diagnosticRangeFinalizations;
                snapshot->secondPendingRangeRevision =
                    revision;
                snapshot->secondPeerSelectionStart =
                    editBox.SelectionStart();
                wxComboBox *liveOwner = getLiveOwner();
                if ( !hasExactTicket(liveOwner) ||
                     liveOwner->m_winui->pendingTextSelectionRevision !=
                         revision )
                {
                    return;
                }
                snapshot->secondPeerSelectionLength =
                    editBox.SelectionLength();
                liveOwner = getLiveOwner();
                if ( !hasExactTicket(liveOwner) ||
                     liveOwner->m_winui->pendingTextSelectionRevision !=
                         revision )
                {
                    return;
                }
                snapshot->secondProjectionObserved = true;
                if ( snapshot->secondPeerSelectionStart != expectedFrom ||
                     snapshot->secondPeerSelectionLength !=
                         expectedTo - expectedFrom )
                {
                    return;
                }

                if ( impl->pendingRangeFinalizationHookForTesting ||
                     impl->independentRangeObservationHookForTesting )
                {
                    return;
                }
                const std::weak_ptr<wxWinUIChoiceCallbackState>
                    weakChoiceState = choiceState;
                const std::weak_ptr<wxWinUITextCallbackState>
                    weakTextState = textState;

                // Do not schedule the independent writer here. Any number of
                // later SelectAll projections may advance the revision first.
                // The exact current confirmation moves this intent only after
                // it has observed and retired the final corrected range.
                impl->pendingRangeFinalizationHookForTesting =
                    [weakChoiceState, weakTextState, snapshot, expectedOwner,
                     impl, generation, epoch, editIdentity, independentFrom,
                     independentTo](wxComboBox *callbackOwner)
                    {
                        const std::shared_ptr<wxWinUIChoiceCallbackState>
                            dispatchedChoiceState = weakChoiceState.lock();
                        const std::shared_ptr<wxWinUITextCallbackState>
                            dispatchedTextState = weakTextState.lock();
                        if ( !dispatchedChoiceState || !dispatchedTextState )
                            return;

                        const auto getDispatchedOwner = [&]() -> wxComboBox *
                        {
                            wxChoice * const choiceOwner =
                                dispatchedChoiceState->GetOwner();
                            wxComboBox * const textOwner =
                                dispatchedTextState->GetOwner<wxComboBox>();
                            if ( textOwner != callbackOwner ||
                                 choiceOwner != expectedOwner ||
                                 textOwner != expectedOwner ||
                                 !textOwner->m_winui ||
                                 textOwner->m_winui.get() != impl )
                            {
                                return nullptr;
                            }

                            wxWinUIChoiceImpl * const liveImpl =
                                textOwner->m_winui.get();
                            if ( liveImpl->callbackState !=
                                     dispatchedChoiceState ||
                                 liveImpl->textCallbackState !=
                                     dispatchedTextState ||
                                 liveImpl->editGeneration != generation ||
                                 !liveImpl->editBox )
                            {
                                return nullptr;
                            }

                            const std::uintptr_t liveIdentity =
                                reinterpret_cast<std::uintptr_t>(
                                    winrt::get_abi(
                                        wxWinUIComboObjectIdentity(
                                            liveImpl->editBox)));
                            return liveIdentity == editIdentity
                                     ? textOwner
                                     : nullptr;
                        };
                        const auto hasFinalizedAuthority =
                            [&](wxComboBox *liveOwner)
                        {
                            return liveOwner && liveOwner->m_winui &&
                                   liveOwner->m_winui.get() == impl &&
                                   !liveOwner->m_winui->
                                       hasPendingTextSelection &&
                                   liveOwner->m_winui->
                                       pendingTextSelectionEpoch == epoch &&
                                   !liveOwner->m_winui->
                                       independentRangeObservationHookForTesting;
                        };
                        const auto clearDispatchedObservation = [&]()
                        {
                            wxComboBox * const liveOwner =
                                getDispatchedOwner();
                            if ( liveOwner && liveOwner->m_winui &&
                                 liveOwner->m_winui.get() == impl )
                            {
                                liveOwner->m_winui->
                                    independentRangeObservationHookForTesting =
                                        nullptr;
                            }
                        };

                        wxComboBox *dispatchedOwner = getDispatchedOwner();
                        if ( !hasFinalizedAuthority(dispatchedOwner) )
                            return;

                        const MUXC::TextBox dispatchedEdit =
                            dispatchedOwner->m_winui->editBox;
                        try
                        {
                            dispatchedOwner = getDispatchedOwner();
                            if ( !hasFinalizedAuthority(dispatchedOwner) )
                                return;

                            snapshot->finalizedRangeProjectionMismatches =
                                dispatchedOwner->m_winui->
                                    diagnosticRangeProjectionMismatches;
                            snapshot->finalizedRangeFinalizations =
                                dispatchedOwner->m_winui->
                                    diagnosticRangeFinalizations;
                            snapshot->independentIntentConsumed = true;
                            dispatchedOwner->m_winui->
                                independentRangeObservationHookForTesting =
                                    [weakChoiceState, weakTextState, snapshot,
                                     expectedOwner, impl, generation,
                                     editIdentity](
                                        wxComboBox *callbackOwner)
                                    {
                                        const std::shared_ptr<
                                            wxWinUIChoiceCallbackState>
                                            observedChoiceState =
                                                weakChoiceState.lock();
                                        const std::shared_ptr<
                                            wxWinUITextCallbackState>
                                            observedTextState =
                                                weakTextState.lock();
                                        if ( !observedChoiceState ||
                                             !observedTextState )
                                        {
                                            return;
                                        }

                                        const auto getObservedOwner =
                                            [&]() -> wxComboBox *
                                        {
                                            wxChoice * const choiceOwner =
                                                observedChoiceState->GetOwner();
                                            wxComboBox * const textOwner =
                                                observedTextState->
                                                    GetOwner<wxComboBox>();
                                            if ( textOwner != callbackOwner ||
                                                 choiceOwner != expectedOwner ||
                                                 textOwner != expectedOwner ||
                                                 !textOwner->m_winui ||
                                                 textOwner->m_winui.get() !=
                                                     impl )
                                            {
                                                return nullptr;
                                            }

                                            wxWinUIChoiceImpl * const liveImpl =
                                                textOwner->m_winui.get();
                                            if ( liveImpl->callbackState !=
                                                     observedChoiceState ||
                                                 liveImpl->textCallbackState !=
                                                     observedTextState ||
                                                 liveImpl->editGeneration !=
                                                     generation ||
                                                 !liveImpl->editBox )
                                            {
                                                return nullptr;
                                            }

                                            const std::uintptr_t liveIdentity =
                                                reinterpret_cast<
                                                    std::uintptr_t>(
                                                    winrt::get_abi(
                                                        wxWinUIComboObjectIdentity(
                                                            liveImpl->editBox)));
                                            return liveIdentity == editIdentity
                                                     ? textOwner
                                                     : nullptr;
                                        };

                                        wxComboBox *observedOwner =
                                            getObservedOwner();
                                        if ( !observedOwner )
                                            return;
                                        const MUXC::TextBox observedEdit =
                                            observedOwner->m_winui->editBox;

                                        long observedStart = 0;
                                        long observedLength = 0;
                                        try
                                        {
                                            observedStart =
                                                observedEdit.SelectionStart();
                                            observedOwner = getObservedOwner();
                                            if ( !observedOwner )
                                                return;
                                            observedLength =
                                                observedEdit.SelectionLength();
                                            observedOwner = getObservedOwner();
                                            if ( !observedOwner )
                                                return;
                                        }
                                        catch ( const winrt::hresult_error& e )
                                        {
                                            wxWinUILogException(
                                                "WinUI ComboBox staged "
                                                "independent read", e);
                                            return;
                                        }
                                        wxWinUIChoiceImpl * const observedImpl =
                                            observedOwner->m_winui.get();
                                        snapshot->
                                            independentPeerSelectionStart =
                                                observedStart;
                                        snapshot->
                                            independentPeerSelectionLength =
                                                observedLength;
                                        snapshot->
                                            independentModelSelectionStart =
                                                observedOwner->m_selectionStart;
                                        snapshot->
                                            independentModelSelectionEnd =
                                                observedOwner->m_selectionEnd;
                                        snapshot->independentPendingRange =
                                            observedImpl->
                                                hasPendingTextSelection;
                                        snapshot->
                                            independentRangeProjectionMismatches =
                                                observedImpl->
                                                    diagnosticRangeProjectionMismatches;
                                        snapshot->
                                            independentRangeFinalizations =
                                                observedImpl->
                                                    diagnosticRangeFinalizations;
                                        snapshot->independentRangeObserved =
                                            true;
                                    };
                            snapshot->independentObservationArmed = true;

                            // Mark the authority before Select(): the native
                            // ordinary callback can run synchronously and is
                            // the only consumer of the observer above. Select
                            // is deliberately the final owner operation.
                            snapshot->independentRangeInjected = true;
                            dispatchedEdit.Select(
                                static_cast<int32_t>(independentFrom),
                                static_cast<int32_t>(independentTo -
                                                     independentFrom));
                        }
                        catch ( const winrt::hresult_error& e )
                        {
                            clearDispatchedObservation();
                            snapshot->independentRangeInjected = false;
                            wxWinUILogException(
                                "WinUI ComboBox staged independent range", e);
                        }
                    };
                snapshot->independentIntentArmed = true;
            }
            catch ( const winrt::hresult_error& e )
            {
                wxComboBox * const liveOwner = getLiveOwner();
                if ( liveOwner && hasExactTicket(liveOwner) &&
                     liveOwner->m_winui->pendingTextSelectionRevision ==
                         revision )
                {
                    liveOwner->ClearPendingTextSelection();
                }
                wxWinUILogException(
                    "WinUI ComboBox staged second range", e);
            }
        };

    impl->pendingRangeProjectionHookForTesting =
        [snapshot, getLiveOwner, hasExactTicket, impl, editBox, peerLast,
         expectedFrom, expectedTo,
         secondProjectionHook = std::move(secondProjectionHook)](
            wxComboBox *callbackOwner) mutable
        {
            wxComboBox * const owner = getLiveOwner();
            if ( owner != callbackOwner || !hasExactTicket(owner) )
                return;

            try
            {
                snapshot->firstRangeProjectionMismatches =
                    impl->diagnosticRangeProjectionMismatches;
                snapshot->firstRangeFinalizations =
                    impl->diagnosticRangeFinalizations;
                snapshot->firstPendingRangeRevision =
                    impl->pendingTextSelectionRevision;
                snapshot->firstPeerSelectionStart =
                    editBox.SelectionStart();
                snapshot->firstPeerSelectionLength =
                    editBox.SelectionLength();
                snapshot->firstProjectionObserved = true;
                if ( snapshot->firstPeerSelectionStart != expectedFrom ||
                     snapshot->firstPeerSelectionLength !=
                         expectedTo - expectedFrom ||
                     impl->pendingRangeProjectionHookForTesting )
                {
                    return;
                }

                impl->pendingRangeProjectionHookForTesting =
                    std::move(secondProjectionHook);
                editBox.Select(0, static_cast<int32_t>(peerLast));
                snapshot->secondSelectAllInjected = true;
            }
            catch ( const winrt::hresult_error& e )
            {
                wxComboBox * const liveOwner = getLiveOwner();
                if ( liveOwner && liveOwner->m_winui.get() == impl )
                {
                    liveOwner->m_winui->
                        pendingRangeProjectionHookForTesting = nullptr;
                }
                wxWinUILogException(
                    "WinUI ComboBox staged first range", e);
            }
        };

    if ( currentIsSelectAll )
    {
        // The transition already changed the dependency property and its real
        // deferred event is the first edge in the chain.
        snapshot->awaitedExistingProjection = true;
        return true;
    }

    try
    {
        editBox.Select(0, static_cast<int32_t>(peerLast));
        snapshot->firstSelectAllInjected = true;
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxComboBox * const owner = getLiveOwner();
        if ( owner && owner->m_winui.get() == impl )
            owner->m_winui->pendingRangeProjectionHookForTesting = nullptr;
        wxWinUILogException(
            "WinUI ComboBox staged range activation", e);
        return false;
    }
}

bool wxWinUIComboBoxTestAccess::SetRawEditSelectionAndDestroy(wxComboBox* control,
    long from,
    long to,
    const std::shared_ptr<WinUIRangeDestructionSnapshot>& snapshot)
{
    if ( !snapshot )
        return false;
    *snapshot = WinUIRangeDestructionSnapshot();

    // This oracle must not force a template/layout edge: the exact current
    // editor and its armed ticket were established by the preceding passive
    // snapshots, and the deferred SelectionChanged is the event under test.
    if ( !control->m_winui || !control->m_winui->editBox ||
         !control->m_winui->textCallbackState ||
         !control->m_winui->hasPendingTextSelection )
    {
        return false;
    }

    const long len =
        wxWinUITextPositionMap(control->m_value, false, false).GetLastPosition();
    from = wxWinUIClampComboTextPos(from, len);
    to = to < 0 ? len : wxWinUIClampComboTextPos(to, len);
    if ( to < from )
        wxSwap(from, to);

    // This lifetime seam is deliberately limited to the causal template
    // projection shape. A native non-SelectAll range remains an independent
    // selection authority and must never trigger destruction here.
    if ( from != 0 || to != len )
        return false;

    wxWinUIChoiceImpl * const impl = control->m_winui.get();
    const MUXC::TextBox editBox = impl->editBox;
    const std::shared_ptr<wxWinUITextCallbackState> textState =
        impl->textCallbackState;
    const std::uint64_t generation = impl->editGeneration;
    try
    {
        const long currentFrom = editBox.SelectionStart();
        const long currentTo = currentFrom + editBox.SelectionLength();
        if ( currentFrom == from && currentTo == to )
            return false;

        impl->pendingRangeProjectionHookForTesting =
            [snapshot](wxComboBox *owner)
            {
                wxWinUIChoiceImpl * const hookImpl = owner->m_winui.get();
                snapshot->rangeProjectionMismatches =
                    hookImpl->diagnosticRangeProjectionMismatches;
                snapshot->rangeFinalizations =
                    hookImpl->diagnosticRangeFinalizations;
                snapshot->pendingRangeRevision =
                    hookImpl->pendingTextSelectionRevision;

                // The normal-priority observation runs after the current
                // routed callback has unwound but before the already queued
                // Low confirmation. It therefore measures whether that Low
                // retained callback state, without sleeping or repairing it.
                try
                {
                    const MUXD::DispatcherQueue queue =
                        hookImpl->editBox.DispatcherQueue();
                    if ( queue )
                    {
                        (void)queue.TryEnqueue(
                            MUXD::DispatcherQueuePriority::Normal,
                            [snapshot]()
                            {
                                snapshot->liveCountAfterDestroy =
                                    wxWinUITextCallbackState::
                                        GetLiveCountForTesting();
                            });
                        // The confirmation was queued at Low before this
                        // hook. A second Low sentinel is therefore a causal
                        // FIFO proof that the weak confirmation has drained
                        // after deletion.
                        (void)queue.TryEnqueue(
                            MUXD::DispatcherQueuePriority::Low,
                            [snapshot]()
                            {
                                snapshot->lowDrainObserved = true;
                            });
                    }
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI ComboBox lifetime observation queue", e);
                }
                snapshot->causalHookEntered = true;

                // This must be the final owner operation: CloseTextPeer()
                // invalidates every state which the queued Low can lock.
                delete owner;
            };

        // Do not pre-classify this range. The real deferred
        // TextBox.SelectionChanged event must enter HandlePending... and
        // consume the one-shot only after its weak confirmation is queued.
        editBox.Select(static_cast<int32_t>(from),
                       static_cast<int32_t>(to - from));
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxComboBox * const owner = textState->GetOwner<wxComboBox>();
        if ( owner && owner->m_winui && owner->m_winui.get() == impl &&
             owner->m_winui->textCallbackState == textState &&
             owner->m_winui->editGeneration == generation &&
             owner->m_winui->editBox == editBox )
        {
            owner->m_winui->pendingRangeProjectionHookForTesting = nullptr;
        }
        wxWinUILogException(
            "WinUI ComboBox raw lifetime selection test mutation", e);
        return false;
    }
}

unsigned wxWinUIComboBoxTestAccess::GetTemplateState(const wxComboBox* control)
{
    if ( !control->m_winui )
        return 0;

    unsigned state = 0;
    if ( control->m_winui->editBox )
        state |= 0x1;
    if ( control->m_winui->retiredEditBoxForTesting )
        state |= 0x2;
    if ( control->m_winui->editSelectionChangedToken.value )
        state |= 0x4;
    return state;
}

bool wxWinUIComboBoxTestAccess::GetTextEntryPeerState(wxComboBox* control,
    unsigned long *maxLength,
    bool *forceUpper,
    wxPoint *margins,
    std::uintptr_t *editIdentity,
    std::uint64_t *editGeneration,
    int *editableRootActualTheme,
    int *editableRootRequestedTheme,
    int *editActualTheme,
    bool realize)
{
    if ( editIdentity )
        *editIdentity = 0;
    if ( editGeneration )
        *editGeneration = 0;
    if ( editableRootActualTheme )
        *editableRootActualTheme = WinUITheme_Unknown;
    if ( editableRootRequestedTheme )
        *editableRootRequestedTheme = WinUITheme_Unknown;
    if ( editActualTheme )
        *editActualTheme = WinUITheme_Unknown;
    if ( realize && !control->ResolveEditPart() )
        return false;
    if ( !control->m_winui || !control->m_winui->editBox )
        return false;

    wxComboBox * const expectedOwner = control;
    wxWinUIChoiceImpl * const impl = control->m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    const MUXC::TextBox editBox = impl->editBox;

    try
    {
        const int32_t peerMaxLength = editBox.MaxLength();
        const bool peerForceUpper =
            editBox.CharacterCasing() == MUXC::CharacterCasing::Upper;
        const double peerLeftMargin = editBox.Padding().Left;
        const int peerRootTheme = impl->editableRoot
            ? wxWinUIComboThemeForTesting(
                  impl->editableRoot.ActualTheme())
            : WinUITheme_Unknown;
        const int peerRootRequestedTheme = impl->editableRoot
            ? wxWinUIComboThemeForTesting(
                  impl->editableRoot.RequestedTheme())
            : WinUITheme_Unknown;
        const int peerEditTheme =
            wxWinUIComboThemeForTesting(editBox.ActualTheme());

        wxComboBox * const owner =
            callbackState
                ? callbackState->GetOwner<wxComboBox>()
                : nullptr;
        if ( owner != expectedOwner || !owner->m_winui ||
             owner->m_winui.get() != impl ||
             owner->m_winui->textCallbackState != callbackState ||
             owner->m_winui->editBox != editBox ||
             peerMaxLength < 0 || !std::isfinite(peerLeftMargin) )
        {
            return false;
        }

        if ( maxLength )
            *maxLength = static_cast<unsigned long>(peerMaxLength);
        if ( forceUpper )
            *forceUpper = peerForceUpper;
        if ( margins )
        {
            *margins = wxPoint(
                owner->FromDIP(
                    static_cast<int>(std::lround(peerLeftMargin))),
                -1);
        }
        if ( editIdentity )
        {
            *editIdentity = reinterpret_cast<std::uintptr_t>(
                winrt::get_abi(
                    wxWinUIComboObjectIdentity(editBox)));
        }
        if ( editGeneration )
            *editGeneration = owner->m_winui->editGeneration;
        if ( editableRootActualTheme )
            *editableRootActualTheme = peerRootTheme;
        if ( editableRootRequestedTheme )
            *editableRootRequestedTheme = peerRootRequestedTheme;
        if ( editActualTheme )
            *editActualTheme = peerEditTheme;
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox text-entry peer test query", e);
        return false;
    }
}

unsigned wxWinUIComboBoxTestAccess::GetAutoCompleteSuggestionCount(const wxComboBox* control)
{
    if ( !control->m_winui || !control->m_winui->autoCompleteFlyout )
        return 0;

    wxWinUIChoiceImpl * const impl = control->m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    const MUXC::MenuFlyout flyout = impl->autoCompleteFlyout;
    try
    {
        const unsigned count = flyout.Items().Size();
        const wxComboBox * const owner = callbackState
            ? callbackState->GetOwner<wxComboBox>()
            : nullptr;
        return owner == control && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->textCallbackState == callbackState &&
                       owner->m_winui->autoCompleteFlyout == flyout
                 ? count
                 : 0;
    }
    catch ( const winrt::hresult_error& )
    {
        return 0;
    }
}

wxString wxWinUIComboBoxTestAccess::GetAutoCompleteSuggestion(const wxComboBox* control,
    unsigned n)
{
    if ( !control->m_winui || !control->m_winui->autoCompleteFlyout )
        return wxString();

    wxWinUIChoiceImpl * const impl = control->m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    const MUXC::MenuFlyout flyout = impl->autoCompleteFlyout;
    try
    {
        const auto items = flyout.Items();
        if ( n >= items.Size() )
            return wxString();
        const MUXC::MenuFlyoutItem item =
            items.GetAt(n).try_as<MUXC::MenuFlyoutItem>();
        const wxString value =
            item ? wxWinUIFromHString(item.Text()) : wxString();
        const wxComboBox * const owner = callbackState
            ? callbackState->GetOwner<wxComboBox>()
            : nullptr;
        return owner == control && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->textCallbackState == callbackState &&
                       owner->m_winui->autoCompleteFlyout == flyout
                 ? value
                 : wxString();
    }
    catch ( const winrt::hresult_error& )
    {
        return wxString();
    }
}

int wxWinUIComboBoxTestAccess::GetAutoCompleteActiveSuggestion(const wxComboBox* control)
{
    if ( !control->m_winui || !control->m_winui->autoCompleteFlyout ||
         !control->m_winui->textCallbackState )
    {
        return wxNOT_FOUND;
    }

    wxWinUIChoiceImpl * const impl = control->m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    const MUXC::MenuFlyout flyout = impl->autoCompleteFlyout;
    const std::uint64_t generation = impl->autoCompleteGeneration;
    const int selected = impl->autoCompleteSelection;
    if ( selected < 0 )
        return wxNOT_FOUND;

    try
    {
        const auto items = flyout.Items();
        if ( static_cast<unsigned>(selected) >= items.Size() )
            return wxNOT_FOUND;
        const MUXC::MenuFlyoutItem item =
            items.GetAt(static_cast<unsigned>(selected)).
                try_as<MUXC::MenuFlyoutItem>();
        if ( !item || item.FontWeight().Weight !=
                         MUXT::FontWeights::SemiBold().Weight )
        {
            return wxNOT_FOUND;
        }

        const wxComboBox * const owner =
            callbackState->GetOwner<wxComboBox>();
        return owner == control && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->textCallbackState == callbackState &&
                       owner->m_winui->autoCompleteFlyout == flyout &&
                       owner->m_winui->autoCompleteGeneration == generation &&
                       owner->m_winui->autoCompleteSelection == selected
                 ? selected
                 : wxNOT_FOUND;
    }
    catch ( const winrt::hresult_error& )
    {
        return wxNOT_FOUND;
    }
}

bool wxWinUIComboBoxTestAccess::InvokeAutoCompleteSuggestion(wxComboBox* control,
    unsigned n)
{
    if ( !control->m_editable || !control->m_winui || !control->m_winui->autoCompleteFlyout )
        return false;

    try
    {
        const auto items = control->m_winui->autoCompleteFlyout.Items();
        if ( n >= items.Size() )
            return false;
        const MUXC::MenuFlyoutItem item =
            items.GetAt(n).try_as<MUXC::MenuFlyoutItem>();
        if ( !item )
            return false;

        // Exercise the real MenuFlyoutItem Invoke provider, and hence its real
        // Click delegate. The callback may synchronously delete this control.
        MUXAP::MenuFlyoutItemAutomationPeer(item).Invoke();
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox auto-complete test invocation", e);
        return false;
    }
}

bool wxWinUIComboBoxTestAccess::InvokeClipboardCommand(wxComboBox* control,
    wxEventType type)
{
    if ( !control->m_winui || !control->m_winui->editBox ||
         !control->m_winui->textCallbackState ||
         wxWindow::FindFocus() != control )
        return false;

    // The seam only injects input into an already focused editor. Focus and
    // layout are established by production SetFocus() and observed passively
    // by the caller; declaring success while XAML focus is still pending made
    // a dispatched HWND message look like a routed clipboard command.
    wxWinUIChoiceImpl * const impl = control->m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->textCallbackState;
    const std::uint64_t editGeneration = impl->editGeneration;
    const MUXC::TextBox editBox = impl->editBox;
    wxComboBox * const expectedOwner = control;
    const auto getExactOwner = [&]() -> wxComboBox *
    {
        wxComboBox * const owner = callbackState
            ? callbackState->GetOwner<wxComboBox>()
            : nullptr;
        return owner == expectedOwner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->textCallbackState == callbackState &&
                       owner->m_winui->editGeneration == editGeneration &&
                       owner->m_winui->editBox == editBox
                 ? owner
                 : nullptr;
    };
    try
    {
        const HWND focusedHwnd = ::GetFocus();
        if ( !focusedHwnd || !impl->host.ContainsFocus(focusedHwnd) ||
             editBox.FocusState() == MUX::FocusState::Unfocused ||
             !getExactOwner() )
        {
            return false;
        }

        WXWPARAM key = 0;
        if ( type == wxEVT_TEXT_COPY )
            key = 'C';
        else if ( type == wxEVT_TEXT_CUT )
            key = 'X';
        else if ( type == wxEVT_TEXT_PASTE )
            key = 'V';
        else
            return false;

        wxWinUIKeyboardModifiers previousModifiers{};
        const bool hadModifierOverride =
            wxWinUI3GetKeyboardModifiersOverrideForTesting(
                &previousModifiers);
        wxWinUIKeyboardModifiers modifiers{};
        modifiers.controlDown = true;
        wxWinUI3SetKeyboardModifiersForTesting(&modifiers);
        wxScopeGuard modifierGuard = wxMakeGuard(
            [hadModifierOverride, previousModifiers]()
            {
                wxWinUI3SetKeyboardModifiersForTesting(
                    hadModifierOverride ? &previousModifiers : nullptr);
            });
        wxUnusedVar(modifierGuard);

        wxComboBox * const owner = getExactOwner();
        if ( !owner )
            return false;
        const std::uint64_t callbacksBefore =
            owner->m_winui->diagnosticKeyPreviewCallbacks;
        const std::uint64_t handledBefore =
            owner->m_winui->diagnosticKeyPreviewHandled;
        const std::uintptr_t keyTargetIdentity =
            owner->m_winui->diagnosticKeyTargetIdentity;
        const std::uintptr_t editIdentity =
            reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                wxWinUIComboObjectIdentity(editBox)));
        if ( !keyTargetIdentity ||
             callbacksBefore ==
                 std::numeric_limits<std::uint64_t>::max() ||
             handledBefore ==
                 std::numeric_limits<std::uint64_t>::max() )
        {
            return false;
        }

        MSG down{};
        down.hwnd = focusedHwnd;
        down.message = WM_KEYDOWN;
        down.wParam = key;
        down.lParam = 1 |
            (static_cast<LPARAM>(::MapVirtualKeyW(
                 static_cast<UINT>(key), MAPVK_VK_TO_VSC)) << 16);
        // The test modifier snapshot is the production routed-handler's Ctrl
        // authority, but it deliberately doesn't alter process-global Win32
        // key state. Dispatch KEYDOWN directly: TranslateMessage() would see
        // Ctrl physically up and enqueue a bogus WM_CHAR ('c', 'x' or 'v')
        // even after XAML marks PreviewKeyDown handled.
        (void)::DispatchMessageW(reinterpret_cast<MSG *>(&down));

        // The key handler may synchronously delete this control. KeyUp carries
        // no command and is safe to deliver to the captured HWND; the handled
        // result of KeyDown remains the seam's success authority.
        MSG up = down;
        up.message = WM_KEYUP;
        up.lParam |= static_cast<LPARAM>(0xC0000000u);
        (void)::DispatchMessageW(reinterpret_cast<MSG *>(&up));

        // A routed wx handler may synchronously destroy or retemplate the
        // control. Destruction is itself proven by the callback state's owner
        // retirement; otherwise require telemetry from the exact TextBox
        // PreviewKeyDown and its handled command branch before reporting
        // success.
        wxComboBox * const survivingOwner =
            callbackState->GetOwner<wxComboBox>();
        if ( !survivingOwner )
            return true;
        return survivingOwner == expectedOwner &&
               survivingOwner->m_winui &&
               survivingOwner->m_winui.get() == impl &&
               impl->diagnosticKeyPreviewCallbacks == callbacksBefore + 1 &&
               impl->diagnosticKeyPreviewHandled == handledBefore + 1 &&
               impl->diagnosticLastPreviewKey ==
                   static_cast<unsigned>(key) &&
               (impl->diagnosticKeySenderIdentity == keyTargetIdentity ||
                impl->diagnosticKeySenderIdentity == editIdentity) &&
               !impl->diagnosticLastPreviewHandledOnEntry;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ComboBox clipboard test invocation", e);
        return false;
    }
}

bool wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(const wxComboBox* control,
    WinUIDiagnosticSnapshot *snapshot)
{
    if ( !snapshot )
        return false;
    *snapshot = WinUIDiagnosticSnapshot();
    if ( !control->m_winui || !control->m_winui->callbackState ||
         !control->m_winui->textCallbackState )
    {
        return false;
    }

    wxWinUIChoiceImpl * const impl = control->m_winui.get();
    wxChoice * const choiceOwner = impl->callbackState->GetOwner();
    wxComboBox * const textOwner =
        impl->textCallbackState->GetOwner<wxComboBox>();
    if ( choiceOwner != control || textOwner != control || !textOwner->m_winui ||
         textOwner->m_winui.get() != impl )
    {
        return false;
    }

    WinUIDiagnosticSnapshot result;
    result.selectionDispatches = impl->diagnosticSelectionDispatches;
    result.selectionSnapshotDeliveries =
        impl->diagnosticSelectionSnapshotDeliveries;
    result.peerTextCallbacks = impl->diagnosticPeerTextCallbacks;
    result.peerTextDeliveries = impl->diagnosticPeerTextDeliveries;
    result.selectionBeforeHandler =
        impl->diagnosticSelectionBeforeHandler;
    result.selectionAfterHandler = impl->diagnosticSelectionAfterHandler;
    result.lastPeerText = impl->diagnosticLastPeerText;
    result.selectionTextAllowed = impl->diagnosticSelectionTextAllowed;
    result.selectionOwnerSurvived =
        impl->diagnosticSelectionOwnerSurvived;

    result.keyPreviewCallbacks = impl->diagnosticKeyPreviewCallbacks;
    result.keyPreviewHandled = impl->diagnosticKeyPreviewHandled;
    result.lastPreviewKey = impl->diagnosticLastPreviewKey;
    result.keyTargetIdentity = impl->diagnosticKeyTargetIdentity;
    result.keySenderIdentity = impl->diagnosticKeySenderIdentity;
    result.lastPreviewHandledOnEntry =
        impl->diagnosticLastPreviewHandledOnEntry;

    if ( impl->diagnosticAutomationOperation )
    {
        result.automationStage =
            impl->diagnosticAutomationOperation->stage.load(
                std::memory_order_acquire);
        result.automationHResult =
            impl->diagnosticAutomationOperation->hresult.load(
                std::memory_order_relaxed);
    }
    else
    {
        result.automationStage = impl->diagnosticAutomationStage;
    }

    result.templateTransitions = impl->diagnosticTemplateTransitions;
    result.templateReplayRequests = impl->diagnosticTemplateReplayRequests;
    result.templateReplayAttempts = impl->diagnosticTemplateReplayAttempts;
    result.templateReplaySuccesses = impl->diagnosticTemplateReplaySuccesses;
    result.comboLayoutEdges = impl->diagnosticComboLayoutEdges;
    result.comboLayoutResolveRequests =
        impl->diagnosticComboLayoutResolveRequests;
    result.comboLayoutResolveRuns = impl->diagnosticComboLayoutResolveRuns;
    result.comboLayoutCoalescedEdges =
        impl->diagnosticComboLayoutCoalescedEdges;
    result.comboLayoutRealizations =
        impl->diagnosticComboLayoutRealizations;
    result.comboLayoutSynchronousRealizations =
        impl->diagnosticComboLayoutSynchronousRealizations;
    result.comboLayoutUnlatchedRealizations =
        impl->diagnosticComboLayoutUnlatchedRealizations;
    result.editLayoutEdges = impl->diagnosticEditLayoutEdges;
    result.rangeProjectionMismatches =
        impl->diagnosticRangeProjectionMismatches;
    result.rangeFinalizations = impl->diagnosticRangeFinalizations;
    result.templateReplayPending = impl->comboTemplateReplayPending;
    result.pendingText = impl->hasPendingTextValue;
    result.pendingRange = impl->hasPendingTextSelection;
    result.pendingRangeEpoch = impl->pendingTextSelectionEpoch;
    result.pendingRangeGeneration =
        impl->pendingTextSelectionGeneration;
    result.pendingRangeRevision =
        impl->pendingTextSelectionRevision;
    result.pendingRangeStart = impl->pendingTextSelectionStart;
    result.pendingRangeEnd = impl->pendingTextSelectionEnd;

    result.simpleRealizeAttempts = impl->diagnosticSimpleRealizeAttempts;
    result.simpleRequestedWidth = impl->diagnosticSimpleRequestedWidth;
    result.simpleRequestedHeight = impl->diagnosticSimpleRequestedHeight;
    result.simpleSourceWidthPixels =
        impl->diagnosticSimpleSourceWidthPixels;
    result.simpleSourceHeightPixels =
        impl->diagnosticSimpleSourceHeightPixels;
    result.simpleRasterizationScale =
        impl->diagnosticSimpleRasterizationScale;
    result.simpleComputedWidth = impl->diagnosticSimpleComputedWidth;
    result.simpleComputedHeight = impl->diagnosticSimpleComputedHeight;
    result.simpleLayoutEpoch = impl->simpleLayoutEpoch;
    result.simpleLayoutAppliedEpoch = impl->simpleLayoutAppliedEpoch;
    result.simpleArrangedHeight = impl->diagnosticSimpleArrangedHeight;
    result.simpleListHeight = impl->diagnosticSimpleListHeight;
    result.simpleViewportHeight = impl->diagnosticSimpleViewportHeight;
    *snapshot = result;
    return true;
}
#endif // WXWINUI_TEST_SUPPORT

void wxComboBox::SendSelectionEvent()
{
    wxWinUIChoiceImpl * const diagnosticImpl = m_winui.get();
    if ( diagnosticImpl )
        ++diagnosticImpl->diagnosticSelectionDispatches;

    const int sel = wxChoice::GetSelection();
    wxWinUIItemModel::Id selectedId = 0;
    if ( sel != wxNOT_FOUND )
    {
        selectedId = m_itemModel.At(sel).id;
        const wxString oldValue = m_value;
        m_value = wxChoice::GetString(sel);
        if ( m_value != oldValue )
            InvalidateBestSize();
        m_insertionPoint = m_value.length();
        m_selectionStart = m_selectionEnd = m_insertionPoint;

        // A normal ComboBox projects the selected item's text itself. The
        // simple peer's ListBox and TextBox are independent visuals, so make
        // the accepted value visible before delivering wxEVT_COMBOBOX, just
        // as wxMSW does before its notification handler can call GetValue().
        if ( m_winui && m_winui->simpleListBox )
        {
            const wxWeakRef<wxWindow> self(this);
            ApplyTextToPeer();
            if ( !self || !ApplyTextSelectionToPeer() )
                return;
        }
    }

    // wxMSW snapshots both fields before delivering wxEVT_COMBOBOX. A
    // reentrant handler may select another item or mutate/sort the model, but
    // the following wxEVT_TEXT still describes this original notification.
    // Force an owning copy too: this value crosses arbitrary application
    // re-entry and must not share writable storage with the live document.
    const wxString selectionValue(m_value.c_str(), m_value.length());
    if ( diagnosticImpl )
        diagnosticImpl->diagnosticSelectionBeforeHandler = selectionValue;

    // wxMSW sends wxEVT_COMBOBOX for the selection followed by wxEVT_TEXT for
    // the resulting text change.
    const bool sendSnapshotText = m_allowTextEvents;
    if ( diagnosticImpl )
    {
        diagnosticImpl->diagnosticSelectionTextAllowed = sendSnapshotText;
        diagnosticImpl->diagnosticSelectionOwnerSurvived = false;
    }
    const std::shared_ptr<wxWinUITextCallbackState> snapshotState =
        m_winui ? m_winui->textCallbackState : nullptr;
    wxCommandEvent event(wxEVT_COMBOBOX, GetId());
    InitCommandEventWithItems(event, sel);
    event.SetInt(sel);
    if ( sel != wxNOT_FOUND )
        event.SetString(selectionValue);

    const wxWeakRef<wxWindow> self(this);
    ProcessCommand(event);
    wxComboBox *owner = snapshotState
        ? snapshotState->GetOwner<wxComboBox>()
        : wxDynamicCast(self.get(), wxComboBox);
    if ( owner != this || !owner->m_winui || !sendSnapshotText )
        return;
    owner->m_winui->diagnosticSelectionOwnerSurvived = true;
    owner->m_winui->diagnosticSelectionAfterHandler = owner->m_value;

    // The COMBOBOX handler may delete, clear or sort the item model. Never
    // reuse the stale numeric index (and hence potentially freed client
    // data) when constructing the following TEXT event.
    int textItem = wxNOT_FOUND;
    if ( selectedId )
    {
        const std::size_t index = owner->m_itemModel.IndexOf(selectedId);
        if ( index != wxWinUIItemModel::npos )
            textItem = static_cast<int>(index);
    }

    // Construct and deliver the accepted-selection TEXT directly from the
    // pre-handler snapshot. Reentrant SetSelection()/SetString() is allowed to
    // alter both the live value and the ordinary text-event enable flag, but
    // neither can replace this already-accepted wxMSW notification pair.
    wxCommandEvent textEvent(wxEVT_TEXT, owner->GetId());
    owner->InitCommandEventWithItems(textEvent, textItem);
    textEvent.SetString(selectionValue);
    ++owner->m_winui->diagnosticSelectionSnapshotDeliveries;
    wxWinUIChoiceImpl * const snapshotImpl = owner->m_winui.get();
    ++owner->m_snapshotTextDispatchDepth;
    wxComboBox * const expectedSnapshotOwner = owner;
    wxScopeGuard snapshotDispatchGuard = wxMakeGuard(
        [snapshotState, snapshotImpl, expectedSnapshotOwner]()
        {
            wxComboBox * const liveOwner = snapshotState
                ? snapshotState->GetOwner<wxComboBox>()
                : nullptr;
            if ( liveOwner == expectedSnapshotOwner && liveOwner->m_winui &&
                 liveOwner->m_winui.get() == snapshotImpl &&
                 liveOwner->m_snapshotTextDispatchDepth )
            {
                --liveOwner->m_snapshotTextDispatchDepth;
            }
        });
    wxUnusedVar(snapshotDispatchGuard);
    owner->ProcessCommand(textEvent);
}

void wxComboBox::EnableTextChangedEvents(bool enable)
{
    m_allowTextEvents = enable;
}

void wxComboBox::SendTextEvent(int item)
{
    SendTextEvent(item, m_value);
}

void wxComboBox::SendTextEvent(int item, const wxString& value)
{
    if ( !m_allowTextEvents )
        return;

    wxCommandEvent event(wxEVT_TEXT, GetId());
    InitCommandEventWithItems(event, item);
    event.SetString(value);
    ProcessCommand(event);
}

void wxComboBox::SendMaxLengthEvent()
{
    wxCommandEvent event(wxEVT_TEXT_MAXLEN, GetId());
    event.SetEventObject(this);
    event.SetString(m_value);
    ProcessCommand(event);
}

void wxComboBox::ClampTextState()
{
    const long len =
        wxWinUITextPositionMap(m_value, false, false).GetLastPosition();
    m_insertionPoint =
        wxWinUIClampComboTextPos(m_insertionPoint, len);
    m_selectionStart =
        wxWinUIClampComboTextPos(m_selectionStart, len);
    m_selectionEnd =
        wxWinUIClampComboTextPos(m_selectionEnd, len);
}

#endif // wxUSE_COMBOBOX
