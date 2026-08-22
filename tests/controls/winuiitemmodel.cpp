///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiitemmodel.cpp
// Purpose:     deterministic tests for the WinUI item model
// Author:      wxWidgets development team
// Created:     2026-07-25
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"
#include "waitfor.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "testableframe.h"

#include "wx/app.h"
#include "wx/button.h"
#include "wx/choice.h"
#if wxUSE_COMBOBOX
    #include "wx/combobox.h"
    #include "wx/filefn.h"
    #include "wx/filename.h"
    #include "wx/textcompleter.h"
    #if wxUSE_UIACTIONSIMULATOR
    #include "wx/uiaction.h"
    #endif
    #include "wx/winui/private/inputtest.h"
    #include "wx/winui/private/textpeer.h"
    #include "wx/winui/private/tlwhost.h"
#endif
#if wxUSE_BITMAPCOMBOBOX
    #include "wx/bmpcbox.h"
    #include "wx/dcmemory.h"
    #include "wx/image.h"
#endif
#include "wx/frame.h"
#include "wx/scopeguard.h"
#include "wx/msw/wrapwin.h"
#include "wx/panel.h"
#include "wx/winui/private/itemmodel.h"
#include "wx/winui/winui.h"

#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

#if wxUSE_COMBOBOX && wxUSE_UIACTIONSIMULATOR && wxUSE_CLIPBOARD
    #include <ole2.h>
#endif

#include <cstdlib>
#include <cmath>
#include <sstream>
#include <vector>

namespace
{

#if wxUSE_COMBOBOX

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

template <typename T>
winrt::Windows::Foundation::IUnknown
wxWinUITestObjectIdentity(const T& object) noexcept
{
    return object
        ? object.template try_as<winrt::Windows::Foundation::IUnknown>()
        : nullptr;
}

bool wxWinUIIsVisualDescendantOf(
    const MUX::DependencyObject& element,
    const MUX::DependencyObject& root) noexcept
{
    if ( !element || !root )
        return false;

    const auto rootIdentity = wxWinUITestObjectIdentity(root);
    if ( !rootIdentity )
        return false;

    try
    {
        MUX::DependencyObject current = element;
        for ( unsigned depth = 0; current && depth != 512; ++depth )
        {
            if ( wxWinUITestObjectIdentity(current) == rootIdentity )
                return true;
            current = MUXM::VisualTreeHelper::GetParent(current);
        }
    }
    catch ( ... )
    {
    }

    return false;
}

struct wxWinUIExactComboPopupPeer
{
    MUXC::ComboBox combo{ nullptr };
    MUXCP::Popup popup{ nullptr };
    MUX::UIElement child{ nullptr };
    MUX::XamlRoot xamlRoot{ nullptr };
    MUXC::ComboBoxItem item{ nullptr };
};

bool wxWinUIResolveExactComboPopupPeer(
    wxComboBox& owner,
    wxWinUIExactComboPopupPeer *result) noexcept
{
    if ( !result )
        return false;

    *result = {};
    try
    {
        wxWinUITopLevelHost * const host =
            wxWinUITopLevelHost::FindSlotOwner(&owner);
        if ( !host )
            return false;

        wxWinUISlot * const slot = host->FindSlot(&owner);
        if ( !slot )
            return false;

        const MUXC::ComboBox combo =
            slot->GetSemanticTarget().try_as<MUXC::ComboBox>();
        if ( !combo )
            return false;

        const MUXC::IControlProtected control =
            combo.try_as<MUXC::IControlProtected>();
        if ( !control )
            return false;

        const MUXCP::Popup popup =
            control.GetTemplateChild(L"Popup").try_as<MUXCP::Popup>();
        if ( !popup || !popup.IsOpen() )
            return false;

        const MUX::UIElement child = popup.Child();
        const MUX::XamlRoot xamlRoot = combo.XamlRoot();
        if ( !child || !xamlRoot ||
             wxWinUITestObjectIdentity(child.XamlRoot()) !=
                 wxWinUITestObjectIdentity(xamlRoot) )
        {
            return false;
        }

        bool exactPopupIsOpen = false;
        const auto popupIdentity = wxWinUITestObjectIdentity(popup);
        for ( const MUXCP::Popup& openPopup :
              MUXM::VisualTreeHelper::GetOpenPopupsForXamlRoot(xamlRoot) )
        {
            if ( wxWinUITestObjectIdentity(openPopup) == popupIdentity )
            {
                if ( exactPopupIsOpen )
                    return false;
                exactPopupIsOpen = true;
            }
        }
        if ( !exactPopupIsOpen )
            return false;

        result->combo = combo;
        result->popup = popup;
        result->child = child;
        result->xamlRoot = xamlRoot;
        return true;
    }
    catch ( ... )
    {
        return false;
    }
}

MUXC::ComboBoxItem wxWinUIFindRealizedComboPopupItem(
    const MUX::DependencyObject& root) noexcept
{
    if ( !root )
        return nullptr;

    try
    {
        std::vector<MUX::DependencyObject> pending;
        pending.push_back(root);
        for ( std::size_t pos = 0;
              pos != pending.size() && pending.size() <= 2048;
              ++pos )
        {
            const MUX::DependencyObject current = pending[pos];
            if ( const MUXC::ComboBoxItem item =
                     current.try_as<MUXC::ComboBoxItem>() )
            {
                return item;
            }

            const int count = MUXM::VisualTreeHelper::GetChildrenCount(current);
            for ( int child = 0; child != count; ++child )
            {
                pending.push_back(
                    MUXM::VisualTreeHelper::GetChild(current, child));
            }
        }
    }
    catch ( ... )
    {
    }

    return nullptr;
}

bool wxWinUIFocusExactComboPopupItem(
    wxComboBox& owner,
    wxWinUIExactComboPopupPeer *result) noexcept
{
    wxWinUIExactComboPopupPeer peer;
    if ( !wxWinUIResolveExactComboPopupPeer(owner, &peer) )
        return false;

    try
    {
        peer.item = wxWinUIFindRealizedComboPopupItem(peer.child);
        if ( !peer.item ||
             !wxWinUIIsVisualDescendantOf(peer.item, peer.child) ||
             !peer.item.Focus(MUX::FocusState::Keyboard) )
        {
            return false;
        }

        const MUX::DependencyObject focused =
            MUXI::FocusManager::GetFocusedElement(peer.xamlRoot)
                .try_as<MUX::DependencyObject>();
        if ( !wxWinUIIsVisualDescendantOf(focused, peer.child) )
            return false;

        *result = peer;
        return true;
    }
    catch ( ... )
    {
        return false;
    }
}

#endif // wxUSE_COMBOBOX

class wxWinUICountedClientData final : public wxClientData
{
public:
    explicit wxWinUICountedClientData(int *destructionCount)
        : m_destructionCount(destructionCount)
    {
    }

    ~wxWinUICountedClientData() override
    {
        ++*m_destructionCount;
    }

private:
    int *m_destructionCount;
};

#if wxUSE_COMBOBOX

class wxWinUIScopedKeyboardModifiers final
{
public:
    explicit wxWinUIScopedKeyboardModifiers(
        const wxWinUIKeyboardModifiers& modifiers)
    {
        wxWinUI3SetKeyboardModifiersForTesting(&modifiers);
    }

    ~wxWinUIScopedKeyboardModifiers()
    {
        wxWinUI3SetKeyboardModifiersForTesting(nullptr);
    }
};

bool wxWinUIDispatchComboKey(WXHWND hwnd,
                             WXWPARAM key,
                             bool sendKeyUp = true)
{
    MSG down = {};
    down.hwnd = hwnd;
    down.message = WM_KEYDOWN;
    down.wParam = key;
    down.lParam = 1 |
        (static_cast<LPARAM>(::MapVirtualKeyW(
             static_cast<UINT>(key), MAPVK_VK_TO_VSC)) << 16);
    const bool downHandled = wxWinUI3DispatchIslandKeyboard(&down);
    if ( !sendKeyUp )
        return downHandled;

    MSG up = down;
    up.message = WM_KEYUP;
    up.lParam |= static_cast<LPARAM>(0xC0000000u);
    const bool upHandled = wxWinUI3DispatchIslandKeyboard(&up);
    return downHandled && upHandled;
}

std::string wxWinUIComboDiagnostics(const wxComboBox& combo)
{
    wxComboBox::WinUIDiagnosticSnapshot d;
    if ( !combo.WinUIGetDiagnosticSnapshotForTesting(&d) )
        return "combo diagnostics unavailable";

    std::ostringstream s;
    s << "sel=" << d.selectionDispatches << "/" <<
            d.selectionSnapshotDeliveries <<
         " peerText=" << d.peerTextCallbacks << "/" <<
            d.peerTextDeliveries <<
         " before='" << d.selectionBeforeHandler.ToStdString() <<
         "' after='" << d.selectionAfterHandler.ToStdString() <<
         "' peer='" << d.lastPeerText.ToStdString() <<
         "' allowed=" << d.selectionTextAllowed <<
         " owner=" << d.selectionOwnerSurvived <<
         " key=" << d.keyPreviewCallbacks << "/" << d.keyPreviewHandled <<
         " last=" << d.lastPreviewKey <<
         " target=" << d.keyTargetIdentity <<
         " sender=" << d.keySenderIdentity <<
         " enteredHandled=" << d.lastPreviewHandledOnEntry <<
         " uia=" << d.automationStage <<
         " transition=" << d.templateTransitions <<
         " replay=" << d.templateReplayRequests << "/" <<
            d.templateReplayAttempts << "/" << d.templateReplaySuccesses <<
         " layout=" << d.comboLayoutEdges << "/" << d.editLayoutEdges <<
         " rangeMismatch=" << d.rangeProjectionMismatches <<
         " finalize=" << d.rangeFinalizations <<
         " replayPending=" << d.templateReplayPending <<
         " pending=" << d.pendingText << "/" << d.pendingRange <<
         " range=" << d.pendingRangeStart << "," << d.pendingRangeEnd <<
         " epoch/gen=" << d.pendingRangeEpoch << "/" <<
            d.pendingRangeGeneration <<
         " revision=" << d.pendingRangeRevision <<
         " simple=" << d.simpleRealizeAttempts <<
         " requested=" << d.simpleRequestedWidth << "x" <<
            d.simpleRequestedHeight <<
         " sourcePx=" << d.simpleSourceWidthPixels << "x" <<
            d.simpleSourceHeightPixels <<
         " scale/computed=" << d.simpleRasterizationScale << "/" <<
            d.simpleComputedWidth << "x" << d.simpleComputedHeight <<
         " simpleEpoch=" << d.simpleLayoutEpoch << "/" <<
            d.simpleLayoutAppliedEpoch <<
         " arranged/list/viewport=" << d.simpleArrangedHeight << "/" <<
            d.simpleListHeight << "/" << d.simpleViewportHeight;
    return s.str();
}

void RequireWinUIComboTemplate(wxComboBox& combo)
{
    wxComboBox::WinUITemplatePeerSnapshot snapshot;
    bool captured = false;
    const bool realized = WaitFor("WinUI ComboBox template realization", [&]()
    {
        captured = combo.WinUIGetTemplatePeerSnapshotForTesting(&snapshot);
        return captured &&
               snapshot.state == wxComboBox::WinUITemplate_Complete;
    }, 1000);
    INFO("template captured=" << captured <<
         " state=" << snapshot.state <<
         " hostContent=" << snapshot.hostContentIdentity <<
         " visualRoot=" << snapshot.visualRootIdentity <<
         " visualRootXamlRoot=" << snapshot.visualRootXamlRootIdentity <<
         " visualRootLogicalParent=" <<
             snapshot.visualRootLogicalParentIdentity <<
         " visualRootVisualParent=" <<
             snapshot.visualRootVisualParentIdentity <<
         " combo=" << snapshot.comboIdentity <<
         " comboXamlRoot=" << snapshot.xamlRootIdentity <<
         " hwnd=" << snapshot.windowWidth << "x" << snapshot.windowHeight <<
         "/visible=" << snapshot.windowVisible <<
         "/shownOnScreen=" << snapshot.windowShownOnScreen <<
         " bridge=" << snapshot.bridgeWidth << "x" << snapshot.bridgeHeight <<
         "/visible=" << snapshot.bridgeVisible <<
         " edit=" << snapshot.editIdentity <<
         "/generation=" << snapshot.editGeneration <<
         " phase=" << snapshot.phase <<
         " selection=" << snapshot.peerSelectionStart << "+" <<
             snapshot.peerSelectionLength <<
         " focus=" << snapshot.logicalFocus << "/" <<
             snapshot.nativeFocusInHost << "/" << snapshot.editFocused);
    REQUIRE(captured);
    REQUIRE(realized);
    REQUIRE(snapshot.state == wxComboBox::WinUITemplate_Complete);
}

bool WaitForSettledWinUIComboTemplateGeneration(
    wxComboBox& combo,
    std::uint64_t previousGeneration,
    wxComboBox::WinUITemplatePeerSnapshot *snapshot,
    wxComboBox::WinUIDiagnosticSnapshot *diagnostic)
{
    constexpr unsigned inFlightPhases =
        wxComboBox::WinUITemplatePhase_Resolving |
        wxComboBox::WinUITemplatePhase_PendingResolve |
        wxComboBox::WinUITemplatePhase_Transition |
        wxComboBox::WinUITemplatePhase_PeerMutation |
        wxComboBox::WinUITemplatePhase_QueuedResolve;

    return WaitFor("WinUI ComboBox deferred template generation", [&]()
    {
        return combo.WinUIGetTemplatePeerSnapshotForTesting(snapshot) &&
               combo.WinUIGetDiagnosticSnapshotForTesting(diagnostic) &&
               (snapshot->state & wxComboBox::WinUITemplate_HasEdit) != 0 &&
               snapshot->editGeneration > previousGeneration &&
               (snapshot->phase & inFlightPhases) == 0 &&
               !diagnostic->templateReplayPending;
    }, 1000);
}

void RequireWinUIReadOnlyComboPopupPeer(wxComboBox& combo)
{
    constexpr unsigned requiredState =
        wxComboBox::WinUITemplate_HasCombo |
        wxComboBox::WinUITemplate_HasXamlRoot |
        wxComboBox::WinUITemplate_HasTemplate |
        wxComboBox::WinUITemplate_HasVisualChild |
        wxComboBox::WinUITemplate_IsArranged |
        wxComboBox::WinUITemplate_HasDesiredSize;

    wxComboBox::WinUITemplatePeerSnapshot snapshot;
    bool captured = false;
    const bool ready = WaitFor("WinUI read-only ComboBox popup peer readiness",
                               [&]()
    {
        captured = combo.WinUIGetTemplatePeerSnapshotForTesting(&snapshot);
        return captured &&
               (snapshot.state & requiredState) == requiredState;
    }, 1000);
    INFO("popup peer captured=" << captured <<
         " state=" << snapshot.state <<
         " combo=" << snapshot.comboIdentity <<
         " comboXamlRoot=" << snapshot.xamlRootIdentity <<
         " template=" << snapshot.templateIdentity <<
         " visualChildren=" << snapshot.visualChildCount <<
         " actual=" << snapshot.actualWidth << "x" <<
             snapshot.actualHeight <<
         " desired=" << snapshot.desiredWidth << "x" <<
             snapshot.desiredHeight <<
         " hwnd=" << snapshot.windowWidth << "x" << snapshot.windowHeight <<
         " bridge=" << snapshot.bridgeWidth << "x" <<
             snapshot.bridgeHeight);
    REQUIRE(captured);
    REQUIRE(ready);
    REQUIRE((snapshot.state & requiredState) == requiredState);
}

bool wxWinUIComboHasExactEditorFocus(wxComboBox& combo)
{
    if ( combo.HasFlag(wxCB_SIMPLE) )
    {
        wxComboBox::WinUISimplePeerSnapshot snapshot;
        return combo.WinUIGetSimplePeerSnapshotForTesting(&snapshot) &&
               snapshot.logicalFocus && snapshot.nativeFocusInHost &&
               snapshot.editFocused;
    }

    wxComboBox::WinUITemplatePeerSnapshot snapshot;
    return combo.WinUIGetTemplatePeerSnapshotForTesting(&snapshot) &&
           snapshot.logicalFocus && snapshot.nativeFocusInHost &&
           snapshot.editFocused;
}

bool wxWinUIComboHasExactPeerRange(wxComboBox& combo,
                                   long from,
                                   long to)
{
    if ( combo.HasFlag(wxCB_SIMPLE) )
    {
        wxComboBox::WinUISimplePeerSnapshot snapshot;
        return combo.WinUIGetSimplePeerSnapshotForTesting(&snapshot) &&
               snapshot.peerTextSelectionStart == from &&
               snapshot.peerTextSelectionLength == to - from;
    }

    wxComboBox::WinUITemplatePeerSnapshot snapshot;
    return combo.WinUIGetTemplatePeerSnapshotForTesting(&snapshot) &&
           snapshot.peerSelectionStart == from &&
           snapshot.peerSelectionLength == to - from;
}

bool wxWinUISimpleExtentIsExact(
    const wxComboBox::WinUISimplePeerSnapshot& snapshot)
{
    constexpr double tolerance = 0.5;
    return snapshot.sourceWidthPixels > 0 &&
           snapshot.sourceHeightPixels > 0 &&
           snapshot.rasterizationScale > 0.0 &&
           std::abs(snapshot.requestedWidth -
                    snapshot.sourceWidthPixels /
                        snapshot.rasterizationScale) <= tolerance &&
           std::abs(snapshot.requestedHeight -
                    snapshot.sourceHeightPixels /
                        snapshot.rasterizationScale) <= tolerance &&
           std::abs(snapshot.rootHeight - snapshot.requestedHeight) <=
               tolerance &&
           snapshot.layoutEpoch != 0 &&
           snapshot.layoutAppliedEpoch == snapshot.layoutEpoch &&
           !snapshot.layoutQueued && !snapshot.layoutInProgress &&
           snapshot.listRowIsPixel &&
           std::abs(snapshot.listRowHeight - snapshot.listHeight) <=
               tolerance &&
           std::abs(snapshot.listLocalHeight - snapshot.listHeight) <=
               tolerance &&
           std::abs(snapshot.listMaxHeight - snapshot.listHeight) <=
               tolerance &&
           std::abs(snapshot.listMinHeight) <= tolerance &&
           snapshot.listDesiredHeight <= snapshot.listHeight + tolerance &&
           snapshot.viewportHeight > 0.0 &&
           snapshot.viewportHeight <= snapshot.listHeight + tolerance;
}

#if wxUSE_UIACTIONSIMULATOR && wxUSE_CLIPBOARD

class wxWinUIClipboardSnapshot final
{
public:
    wxWinUIClipboardSnapshot()
        : m_snapshotResult(::OleGetClipboard(&m_data))
    {
    }

    ~wxWinUIClipboardSnapshot()
    {
        if ( m_restorePending )
            Restore();
        if ( m_data )
            m_data->Release();
    }

    bool IsOk() const
    {
        return SUCCEEDED(m_snapshotResult);
    }

    bool Restore()
    {
        if ( !m_restorePending )
            return true;
        if ( FAILED(m_snapshotResult) )
            return false;

        // Retain the original IDataObject itself instead of extracting text:
        // this preserves every advertised format and never exposes clipboard
        // contents to the test log. Flush it after restoring so ownership does
        // not depend on this short-lived RAII object.
        const HRESULT setResult = ::OleSetClipboard(m_data);
        if ( FAILED(setResult) )
            return false;
        if ( m_data && FAILED(::OleFlushClipboard()) )
            return false;

        if ( !m_data )
        {
            if ( !::OpenClipboard(nullptr) )
                return false;
            const BOOL emptied = ::EmptyClipboard();
            ::CloseClipboard();
            if ( !emptied )
                return false;
        }

        m_restorePending = false;
        return true;
    }

private:
    IDataObject *m_data = nullptr;
    const HRESULT m_snapshotResult;
    bool m_restorePending = true;
};

#endif // wxUSE_UIACTIONSIMULATOR && wxUSE_CLIPBOARD

class wxWinUIDeleteComboOnStartCompleter final : public wxTextCompleter
{
public:
    explicit wxWinUIDeleteComboOnStartCompleter(wxComboBox **combo)
        : m_combo(combo)
    {
    }

    bool Start(const wxString&) override
    {
        wxComboBox * const doomed = *m_combo;
        *m_combo = nullptr;
        delete doomed;
        return false;
    }

    wxString GetNext() override { return wxString(); }

private:
    wxComboBox **m_combo;
};

class wxWinUIDeleteComboOnDestroyCompleter final : public wxTextCompleter
{
public:
    wxWinUIDeleteComboOnDestroyCompleter(wxComboBox **combo, bool *armed)
        : m_combo(combo), m_armed(armed)
    {
    }

    ~wxWinUIDeleteComboOnDestroyCompleter() override
    {
        if ( *m_armed && *m_combo )
        {
            wxComboBox * const doomed = *m_combo;
            *m_combo = nullptr;
            delete doomed;
        }
    }

    bool Start(const wxString&) override { return false; }
    wxString GetNext() override { return wxString(); }

private:
    wxComboBox **m_combo;
    bool *m_armed;
};

#endif // wxUSE_COMBOBOX

#if wxUSE_BITMAPCOMBOBOX

wxBitmapBundle wxWinUIMakeSolidBitmap(const wxColour& colour,
                                      const wxSize& size = wxSize(8, 8))
{
    wxBitmap bitmap(size);
    {
        wxMemoryDC dc(bitmap);
        dc.SetBackground(wxBrush(colour));
        dc.Clear();
    }
    return wxBitmapBundle::FromBitmap(bitmap);
}

wxColour wxWinUIGetBitmapColour(const wxBitmap& bitmap)
{
    const wxImage image = bitmap.ConvertToImage();
    if ( !image.IsOk() )
        return wxNullColour;
    return wxColour(image.GetRed(0, 0),
                    image.GetGreen(0, 0),
                    image.GetBlue(0, 0));
}

#endif // wxUSE_BITMAPCOMBOBOX

} // anonymous namespace

TEST_CASE("wxWinUI item model deltas", "[winui-itemmodel]")
{
    wxWinUIItemModel model;

    const auto first = model.Insert("one", 0, false);
    const auto second = model.Insert("two", 1, false);
    REQUIRE(first.kind == wxWinUIItemModel::Change::Kind::Insert);
    REQUIRE(second.kind == wxWinUIItemModel::Change::Kind::Insert);
    CHECK(first.oldIndex == wxWinUIItemModel::npos);
    CHECK(first.newIndex == 0);
    CHECK(first.id != 0);
    CHECK(second.id > first.id);

    int clientSentinel = 0;
    model.At(0).clientData = &clientSentinel;
    model.At(0).selected = true;
    model.At(0).checked = true;

    const auto renamed = model.Rename(0, "three", false);
    CHECK(renamed.kind == wxWinUIItemModel::Change::Kind::Update);
    CHECK(renamed.id == first.id);
    CHECK(model.At(0).clientData == &clientSentinel);
    CHECK(model.At(0).selected);
    CHECK(model.At(0).checked);

    const auto erased = model.Erase(1);
    CHECK(erased.kind == wxWinUIItemModel::Change::Kind::Erase);
    CHECK(erased.id == second.id);
    CHECK(erased.oldIndex == 1);
    CHECK(erased.newIndex == wxWinUIItemModel::npos);
    CHECK(model.IndexOf(second.id) == wxWinUIItemModel::npos);

    model.Clear();
    const auto afterClear = model.Insert("new", 0, false);
    CHECK(afterClear.id > second.id);
}

TEST_CASE("wxWinUI item model sorted identity", "[winui-itemmodel]")
{
    wxWinUIItemModel model;

    const auto lower = model.Insert("aaa", 0, true);
    const auto title = model.Insert("Aaa", 0, true);
    const auto upper = model.Insert("AAA", 0, true);
    const auto duplicate = model.Insert("Aaa", 0, true);

    REQUIRE(model.GetCount() == 4);
    CHECK(model.At(0).text == "AAA");
    CHECK(model.At(1).text == "Aaa");
    CHECK(model.At(2).text == "Aaa");
    CHECK(model.At(3).text == "aaa");
    CHECK(model.At(0).id == upper.id);
    CHECK(model.At(1).id == title.id);
    CHECK(model.At(2).id == duplicate.id);
    CHECK(model.At(3).id == lower.id);

    int clientSentinel = 0;
    const std::size_t lowerIndex = model.IndexOf(lower.id);
    model.At(lowerIndex).clientData = &clientSentinel;
    model.At(lowerIndex).selected = true;
    model.At(lowerIndex).checked = true;

    const auto moved = model.Rename(lowerIndex, "AA", true);
    CHECK(moved.kind == wxWinUIItemModel::Change::Kind::Move);
    CHECK(moved.id == lower.id);
    REQUIRE(model.IndexOf(lower.id) == 0);
    CHECK(model.At(0).clientData == &clientSentinel);
    CHECK(model.At(0).selected);
    CHECK(model.At(0).checked);

    const auto unchanged = model.Rename(1, model.At(1).text, true);
    CHECK(unchanged.kind == wxWinUIItemModel::Change::Kind::Update);
    CHECK(unchanged.oldIndex == unchanged.newIndex);
}

#if wxUSE_CHOICE

TEST_CASE("wxWinUI item controls support default construction",
          "[winui-itemmodel][default-create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxChoice choice;
    CHECK(choice.GetCount() == 0);
    CHECK(choice.GetSelection() == wxNOT_FOUND);
    CHECK(choice.GetStringSelection().empty());
    REQUIRE(choice.Create(parent, wxID_ANY));
    CHECK(choice.GetCount() == 0);
    CHECK(choice.GetSelection() == wxNOT_FOUND);
    CHECK(choice.Append("choice") == 0);

#if wxUSE_COMBOBOX
    wxComboBox combo;
    long from = -1;
    long to = -1;
    combo.GetSelection(&from, &to);
    CHECK(combo.IsListEmpty());
    CHECK(combo.IsTextEmpty());
    CHECK(combo.GetSelection() == wxNOT_FOUND);
    CHECK(combo.GetInsertionPoint() == 0);
    CHECK(from == 0);
    CHECK(to == 0);
    CHECK(combo.IsEditable());
    CHECK(combo.GetHint().empty());
    REQUIRE(combo.Create(parent, wxID_ANY));
    CHECK(combo.IsListEmpty());
    CHECK(combo.IsTextEmpty());
    CHECK(combo.GetSelection() == wxNOT_FOUND);
    CHECK(combo.GetInsertionPoint() == 0);
    CHECK(combo.IsEditable());
    combo.ChangeValue("combo");
    CHECK(combo.GetValue() == "combo");
#endif

#if wxUSE_BITMAPCOMBOBOX
    wxBitmapComboBox bitmapCombo;
    REQUIRE(bitmapCombo.Create(parent, wxID_ANY, wxString(),
                               wxDefaultPosition, wxDefaultSize,
                               0, nullptr));
    CHECK(bitmapCombo.Append("bitmap") == 0);
#endif
}

TEST_CASE("wxWinUI Choice applies item deltas",
          "[winui-itemmodel][choice]")
{
    wxFrame frame(nullptr, wxID_ANY, "choice delta owner");
    wxChoice choice(&frame, wxID_ANY);

    REQUIRE(choice.Append("one") == 0);
    REQUIRE(choice.Append("two") == 1);
    REQUIRE(choice.Append("three") == 2);

    const std::uint64_t oneId = choice.WinUIGetItemIdForTesting(0);
    const std::uint64_t twoId = choice.WinUIGetItemIdForTesting(1);
    const std::uint64_t threeId = choice.WinUIGetItemIdForTesting(2);
    const std::uintptr_t onePeer =
        choice.WinUIGetItemPeerIdentityForTesting(0);
    const std::uintptr_t twoPeer =
        choice.WinUIGetItemPeerIdentityForTesting(1);
    const std::uintptr_t threePeer =
        choice.WinUIGetItemPeerIdentityForTesting(2);
    REQUIRE(onePeer != 0);
    REQUIRE(twoPeer != 0);
    REQUIRE(threePeer != 0);

    REQUIRE(choice.Insert("middle", 1) == 1);
    CHECK(choice.WinUIGetItemIdForTesting(0) == oneId);
    CHECK(choice.WinUIGetItemIdForTesting(2) == twoId);
    CHECK(choice.WinUIGetItemIdForTesting(3) == threeId);
    CHECK(choice.WinUIGetItemPeerIdentityForTesting(0) == onePeer);
    CHECK(choice.WinUIGetItemPeerIdentityForTesting(2) == twoPeer);
    CHECK(choice.WinUIGetItemPeerIdentityForTesting(3) == threePeer);

    choice.SetString(2, "two renamed");
    CHECK(choice.WinUIGetItemIdForTesting(2) == twoId);
    CHECK(choice.WinUIGetItemPeerIdentityForTesting(2) == twoPeer);

    choice.Delete(1);
    CHECK(choice.WinUIGetItemIdForTesting(0) == oneId);
    CHECK(choice.WinUIGetItemIdForTesting(1) == twoId);
    CHECK(choice.WinUIGetItemIdForTesting(2) == threeId);
    CHECK(choice.WinUIGetItemPeerIdentityForTesting(0) == onePeer);
    CHECK(choice.WinUIGetItemPeerIdentityForTesting(1) == twoPeer);
    CHECK(choice.WinUIGetItemPeerIdentityForTesting(2) == threePeer);

    const wxSize beforeLongText = choice.GetBestSize();
    choice.Append(wxString(120, 'W'));
    const wxSize withLongText = choice.GetBestSize();
    CHECK(withLongText.x > beforeLongText.x);
    choice.Delete(choice.GetCount() - 1);
    CHECK(choice.GetBestSize().x < withLongText.x);
}

TEST_CASE("wxWinUI Choice distinguishes accepted and popup selection",
          "[winui-itemmodel][choice][current-selection][parity]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxString items[] = { "zero", "one", "two" };
    wxChoice choice(parent, wxID_ANY, wxDefaultPosition,
                    wxDefaultSize, WXSIZEOF(items), items);
    choice.SetSelection(0);
    EventCounter selectionEvents(&choice, wxEVT_CHOICE);

    choice.SetFocus();
    REQUIRE(WaitFor("wxChoice XAML focus", [&choice]()
    {
        return wxWindow::FindFocus() == &choice && ::GetFocus();
    }));
    REQUIRE(choice.WinUISetDropDownForTesting(true));
    CHECK(choice.GetSelection() == 0);
    CHECK(choice.GetCurrentSelection() == 0);

    MSG down = {};
    down.hwnd = ::GetFocus();
    down.message = WM_KEYDOWN;
    down.wParam = VK_DOWN;
    down.lParam = 1 |
        (static_cast<LPARAM>(::MapVirtualKeyW(
             VK_DOWN, MAPVK_VK_TO_VSC)) << 16);
    REQUIRE(wxWinUI3DispatchIslandKeyboard(&down));
    MSG up = down;
    up.message = WM_KEYUP;
    up.lParam |= static_cast<LPARAM>(0xC0000000u);
    REQUIRE(wxWinUI3DispatchIslandKeyboard(&up));
    REQUIRE(WaitFor("wxChoice popup keyboard highlight", [&choice]()
    {
        return choice.GetCurrentSelection() == 1;
    }));
    CHECK(choice.GetSelection() == 0);
    CHECK(choice.GetCurrentSelection() == 1);
    CHECK(selectionEvents.GetCount() == 0);

    REQUIRE(choice.WinUISetDropDownForTesting(false));
    CHECK(choice.GetSelection() == 1);
    CHECK(choice.GetCurrentSelection() == 1);
    CHECK(selectionEvents.GetCount() == 1);

    REQUIRE(choice.WinUISetDropDownForTesting(true));
    REQUIRE(WaitFor("wxChoice causal popup reopen", [&choice]()
    {
        return choice.WinUIIsPeerDropDownOpenForTesting();
    }));
    REQUIRE(choice.WinUISelectPeerItemForTesting(1));
    REQUIRE(choice.WinUISelectPeerItemForTesting(0));
    CHECK(choice.GetSelection() == 1);
    CHECK(choice.GetCurrentSelection() == 0);
    REQUIRE(choice.WinUISetDropDownForTesting(false));
    CHECK(choice.GetSelection() == 0);
    CHECK(selectionEvents.GetCount() == 2);

    // A programmatic selection made while the popup is open is authoritative:
    // closing it must not resurrect an older highlighted item or emit an
    // event for the silent programmatic change.
    REQUIRE(choice.WinUISetDropDownForTesting(true));
    REQUIRE(WaitFor("wxChoice authoritative popup reopen", [&choice]()
    {
        return choice.WinUIIsPeerDropDownOpenForTesting();
    }));
    REQUIRE(choice.WinUISelectPeerItemForTesting(2));
    CHECK(choice.GetSelection() == 0);
    CHECK(choice.GetCurrentSelection() == 2);
    choice.SetSelection(1);
    CHECK(choice.GetSelection() == 1);
    CHECK(choice.GetCurrentSelection() == 1);
    CHECK(selectionEvents.GetCount() == 2);
    REQUIRE(choice.WinUISetDropDownForTesting(false));
    CHECK(choice.GetSelection() == 1);
    CHECK(choice.GetCurrentSelection() == 1);
    CHECK(selectionEvents.GetCount() == 2);
}

TEST_CASE("wxWinUI popup retirement core keeps detached correlation identity",
          "[winui-itemmodel][popup][retirement][seam]")
{
    const wxWinUIPopupRetirementCoreProbeForTesting probe =
        wxChoice::WinUIProbePopupRetirementCoreForTesting();
    CHECK(probe.detachedCorrelationRetired);
    CHECK(probe.reopenRejected);
    CHECK(probe.reopenDegraded);
    CHECK(probe.compositeReentrantCompletion);
    CHECK(probe.sealRemainsClosed);
    CHECK(probe.sealedAddRejectedWithoutPoison);
    CHECK(probe.sealedNativeOpenFailsClosed);
}

#if wxUSE_COMBOBOX

TEST_CASE("wxWinUI ComboBox immediate popup operations are idempotent",
          "[winui-itemmodel][winui-combo][popup][retirement][generation]")
{
    wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    const auto resetPoison = wxMakeGuard([]()
    {
        wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    });
    wxUnusedVar(resetPoison);

    wxFrame frame(nullptr, wxID_ANY, "immediate popup operations");
    const wxString items[] = { "alpha", "beta" };
    wxComboBox combo(&frame, wxID_ANY, wxEmptyString,
                     wxDefaultPosition, wxDefaultSize,
                     WXSIZEOF(items), items, wxCB_READONLY);
    EventCounter dropDowns(&combo, wxEVT_COMBOBOX_DROPDOWN);
    EventCounter closeups(&combo, wxEVT_COMBOBOX_CLOSEUP);
    frame.Show();
    RequireWinUIReadOnlyComboPopupPeer(combo);

    // Peer attachment is complete before this sequence. Do not service the
    // dispatcher between these calls: the first Popup() must synchronously arm
    // the exact Popup.Closed hook, the duplicates are public no-ops, and the
    // same-stack close must remain a normal pending retirement rather than
    // poisoning publication.
    wxWinUIResetComboPopupResolutionDiagnosticForTesting();
    combo.Popup();
    const wxWinUIComboPopupResolutionDiagnosticForTesting popupDiagnostic =
        wxWinUIGetComboPopupResolutionDiagnosticForTesting();
    INFO("popup resolution origin/result=" <<
         static_cast<unsigned>(popupDiagnostic.origin) << "/" <<
         static_cast<unsigned>(popupDiagnostic.result) <<
         " propertyOpen=" << popupDiagnostic.propertyOpen <<
         " root=" << popupDiagnostic.rootAvailable << "/" <<
             popupDiagnostic.rootMatches <<
         " queue=" << popupDiagnostic.queueAvailable <<
         " gate=" << popupDiagnostic.sessionActive << "/" <<
             popupDiagnostic.completionArmed <<
         " control/popup/child=" << popupDiagnostic.controlAvailable << "/" <<
             popupDiagnostic.popupAvailable << "/" <<
             popupDiagnostic.childAvailable <<
         " hooks=" << popupDiagnostic.hooksArmed <<
         " popupOpen=" << popupDiagnostic.popupIsOpen <<
         " openList=" << popupDiagnostic.exactOpenList <<
         " duplicate=" << popupDiagnostic.duplicateOpenList <<
         " failReason/count=" <<
             static_cast<unsigned>(popupDiagnostic.failCloseReason) << "/" <<
             popupDiagnostic.failCloseCount);
    combo.Popup();
    CHECK(dropDowns.GetCount() == 1);
    CHECK(combo.WinUIIsPeerDropDownOpenForTesting());
    combo.Dismiss();
    combo.Dismiss();
    CHECK(closeups.GetCount() == 1);
    CHECK_FALSE(combo.WinUIIsPeerDropDownOpenForTesting());
    CHECK_FALSE(wxWinUIIsPhysicalDisconnectPublicationPoisoned());
}

TEST_CASE("wxWinUI ComboBox coalesces synchronous close-handler reopen",
          "[winui-itemmodel][winui-combo][popup][retirement][generation]")
{
    wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    const auto resetPoison = wxMakeGuard([]()
    {
        wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    });
    wxUnusedVar(resetPoison);

    wxFrame frame(nullptr, wxID_ANY, "coalesced popup reopen");
    const wxString items[] = { "alpha", "beta" };
    wxComboBox combo(&frame, wxID_ANY, wxEmptyString,
                     wxDefaultPosition, wxDefaultSize,
                     WXSIZEOF(items), items, wxCB_READONLY);
    EventCounter dropDowns(&combo, wxEVT_COMBOBOX_DROPDOWN);
    frame.Show();
    RequireWinUIReadOnlyComboPopupPeer(combo);
    // Once the peer is attached, Popup() must own the source-event -> exact
    // popup/open-list publication gap without an intervening dispatcher pump.
    combo.Popup();
    CHECK(dropDowns.GetCount() == 1);
    CHECK_FALSE(wxWinUIIsPhysicalDisconnectPublicationPoisoned());
    combo.Popup();
    CHECK(dropDowns.GetCount() == 1);
    REQUIRE(WaitFor("initial ComboBox popup", [&combo]()
    {
        return combo.WinUIIsPeerDropDownOpenForTesting();
    }));

    unsigned closeups = 0;
    const auto closeupHandler =
        [&combo, &closeups](wxCommandEvent&)
        {
            ++closeups;
            // Dismiss() publishes CLOSEUP synchronously while generation 1's
            // exact Popup.Closed/LostFocus/tail is still pending.
            combo.Popup();
        };
    combo.Bind(wxEVT_COMBOBOX_CLOSEUP, closeupHandler);
    combo.Dismiss();

    bool pending = false;
    std::uint64_t generation = 0;
    unsigned schedules = 0;
    unsigned runs = 0;
    REQUIRE(combo.WinUIGetPopupReopenSnapshotForTesting(
        &pending, &generation, &schedules, &runs));
    CHECK(closeups == 1);
    CHECK(pending);
    CHECK(generation != 0);
    CHECK(schedules == 1);
    CHECK(runs == 0);
    CHECK_FALSE(combo.WinUIIsPeerDropDownOpenForTesting());
    CHECK_FALSE(wxWinUIIsPhysicalDisconnectPublicationPoisoned());

    // WaitFor only services the real dispatcher. The replay is admitted by
    // the exact gate's causal callback, and the counters prove that no idle
    // turn or duplicate public Popup() was used as the oracle.
    REQUIRE(WaitFor("causal ComboBox popup replay", [&combo]()
    {
        bool replayPending = false;
        std::uint64_t replayGeneration = 0;
        unsigned replaySchedules = 0;
        unsigned replayRuns = 0;
        return combo.WinUIGetPopupReopenSnapshotForTesting(
                   &replayPending, &replayGeneration,
                   &replaySchedules, &replayRuns) &&
               !replayPending && replaySchedules == 1 &&
               replayRuns == 1 &&
               combo.WinUIIsPeerDropDownOpenForTesting();
    }));
    REQUIRE(combo.WinUIGetPopupReopenSnapshotForTesting(
        &pending, &generation, &schedules, &runs));
    CHECK_FALSE(pending);
    CHECK(schedules == 1);
    CHECK(runs == 1);
    CHECK_FALSE(wxWinUIIsPhysicalDisconnectPublicationPoisoned());

    REQUIRE(combo.Unbind(wxEVT_COMBOBOX_CLOSEUP, closeupHandler));
    combo.Dismiss();
}

TEST_CASE("wxWinUI ComboBox observes final cancelled LosingFocus",
          "[winui-itemmodel][winui-combo][popup][retirement][focus-cancel]")
{
    wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    const auto resetPoison = wxMakeGuard([]()
    {
        wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    });
    wxUnusedVar(resetPoison);

    wxFrame frame(nullptr, wxID_ANY, "cancelled popup focus");
    const wxString items[] = { "alpha", "beta" };
    wxComboBox combo(&frame, wxID_ANY, wxEmptyString,
                     wxDefaultPosition, wxDefaultSize,
                     WXSIZEOF(items), items, wxCB_READONLY);
    wxButton next(&frame, wxID_ANY, "next");
    frame.Show();
    RequireWinUIReadOnlyComboPopupPeer(combo);
    combo.SetFocus();
    combo.Popup();
    REQUIRE(WaitFor("cancel focus popup", [&combo]()
    {
        return combo.WinUIIsPeerDropDownOpenForTesting();
    }));

    // Resolve the peer from this wx control's authoritative host slot, then
    // walk the exact template Popup.Child. This avoids assuming that a
    // synthetic Down key was routed to the ComboBox rather than its shell.
    wxWinUIExactComboPopupPeer popupPeer;
    REQUIRE(WaitFor("focus exact realized ComboBox popup item", [&]()
    {
        return wxWinUIFocusExactComboPopupItem(combo, &popupPeer);
    }));
    REQUIRE(popupPeer.combo);
    REQUIRE(popupPeer.popup);
    REQUIRE(popupPeer.child);
    REQUIRE(popupPeer.item);
    REQUIRE(wxWinUIIsVisualDescendantOf(popupPeer.item, popupPeer.child));
    const MUX::DependencyObject focusedBefore =
        MUXI::FocusManager::GetFocusedElement(popupPeer.xamlRoot)
            .try_as<MUX::DependencyObject>();
    REQUIRE(wxWinUIIsVisualDescendantOf(focusedBefore, popupPeer.child));

    // Resolve the destination through its own authoritative slot too. A wx
    // SetFocus() can stop at the shell HWND while the popup owns XAML focus;
    // focusing this exact semantic peer produces the real cross-subtree
    // FocusManager transition which the retirement gate must correlate.
    wxWinUITopLevelHost * const nextHost =
        wxWinUITopLevelHost::FindSlotOwner(&next);
    REQUIRE(nextHost != nullptr);
    wxWinUISlot * const nextSlot = nextHost->FindSlot(&next);
    REQUIRE(nextSlot != nullptr);
    const MUX::UIElement nextTarget = nextSlot->GetSemanticTarget();
    REQUIRE(nextTarget);
    REQUIRE(nextTarget.XamlRoot());
    REQUIRE(wxWinUITestObjectIdentity(nextTarget.XamlRoot()) ==
            wxWinUITestObjectIdentity(popupPeer.xamlRoot));
    REQUIRE_FALSE(wxWinUIIsVisualDescendantOf(nextTarget,
                                               popupPeer.child));

    unsigned cancelled = 0;
    bool selectedCorrelation = false;
    winrt::guid cancelledCorrelation{};
    std::uintptr_t cancelledOldFocusedIdentity = 0;
    const winrt::event_token token = MUXI::FocusManager::LosingFocus(
        [&cancelled, &selectedCorrelation, &cancelledCorrelation,
         &cancelledOldFocusedIdentity, popupChild = popupPeer.child](
            const winrt::Windows::Foundation::IInspectable&,
            const MUXI::LosingFocusEventArgs& args)
        {
            const MUX::DependencyObject oldFocused =
                args.OldFocusedElement();
            if ( !wxWinUIIsVisualDescendantOf(oldFocused, popupChild) )
                return;

            // Select exactly the correlation which attempts to leave this
            // Popup.Child. Unrelated global focus changes are never altered.
            if ( !selectedCorrelation )
            {
                selectedCorrelation = true;
                cancelledCorrelation = args.CorrelationId();
                cancelledOldFocusedIdentity =
                    reinterpret_cast<std::uintptr_t>(
                        winrt::get_abi(wxWinUITestObjectIdentity(oldFocused)));
            }
            if ( args.CorrelationId() != cancelledCorrelation )
                return;

            ++cancelled;
            args.Cancel(true);
        });
    bool handlerLive = true;
    const auto revokeHandler = wxMakeGuard([&]()
    {
        if ( handlerLive )
            MUXI::FocusManager::LosingFocus(token);
    });
    wxUnusedVar(revokeHandler);

    const bool nextFocusAccepted = nextTarget.Focus(MUX::FocusState::Keyboard);
    const bool cancellationObserved =
        WaitFor("cancel exact popup focus departure", [&]()
    {
        if ( !selectedCorrelation || cancelled != 1 )
            return false;

        try
        {
            const MUX::DependencyObject focused =
                MUXI::FocusManager::GetFocusedElement(popupPeer.xamlRoot)
                    .try_as<MUX::DependencyObject>();
            return popupPeer.popup.IsOpen() &&
                   wxWinUIIsVisualDescendantOf(focused, popupPeer.child);
        }
        catch ( ... )
        {
            return false;
        }
    });
    MUX::DependencyObject focusedAfterAttempt{ nullptr };
    std::uintptr_t focusedAfterAttemptIdentity = 0;
    bool focusedAfterAttemptInside = false;
    bool popupOpenAfterAttempt = false;
    try
    {
        focusedAfterAttempt =
            MUXI::FocusManager::GetFocusedElement(popupPeer.xamlRoot)
                .try_as<MUX::DependencyObject>();
        const auto focusedIdentity =
            wxWinUITestObjectIdentity(focusedAfterAttempt);
        focusedAfterAttemptIdentity = reinterpret_cast<std::uintptr_t>(
            winrt::get_abi(focusedIdentity));
        focusedAfterAttemptInside =
            wxWinUIIsVisualDescendantOf(focusedAfterAttempt,
                                         popupPeer.child);
        popupOpenAfterAttempt = popupPeer.popup.IsOpen();
    }
    catch ( ... )
    {
    }
    INFO("next focus accepted=" << nextFocusAccepted <<
         " selectedCorrelation=" << selectedCorrelation <<
         " cancelled=" << cancelled <<
         " oldFocus=" << cancelledOldFocusedIdentity <<
         " currentFocus=" << focusedAfterAttemptIdentity <<
         " inside=" << focusedAfterAttemptInside <<
         " popupOpen=" << popupOpenAfterAttempt);
    REQUIRE(cancellationObserved);
    REQUIRE(selectedCorrelation);
    REQUIRE(cancelled == 1);
    REQUIRE(cancelledOldFocusedIdentity != 0);
    const MUX::DependencyObject focusedAfter =
        MUXI::FocusManager::GetFocusedElement(popupPeer.xamlRoot)
            .try_as<MUX::DependencyObject>();
    REQUIRE(wxWinUIIsVisualDescendantOf(focusedAfter, popupPeer.child));
    REQUIRE(popupPeer.popup.IsOpen());
    MUXI::FocusManager::LosingFocus(token);
    handlerLive = false;

    combo.Dismiss();
    combo.Popup();
    REQUIRE(WaitFor("cancelled correlation causal replay", [&combo]()
    {
        return combo.WinUIIsPeerDropDownOpenForTesting();
    }));
    CHECK_FALSE(wxWinUIIsPhysicalDisconnectPublicationPoisoned());
    combo.Dismiss();
}

TEST_CASE("wxWinUI ComboBox poison preflight blocks live peer publication",
          "[winui-itemmodel][winui-combo][popup][retirement][poison]")
{
    wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    const auto resetPoison = wxMakeGuard([]()
    {
        wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    });
    wxUnusedVar(resetPoison);

    wxFrame frame(nullptr, wxID_ANY, "poisoned live combo");
    const wxString items[] = { "alpha", "beta" };
    wxComboBox combo(&frame, wxID_ANY, wxEmptyString,
                     wxDefaultPosition, wxDefaultSize,
                     WXSIZEOF(items), items, wxCB_READONLY);
    frame.Show();

    wxWinUIPoisonPhysicalDisconnectPublicationForTesting();
    REQUIRE(wxWinUIIsPhysicalDisconnectPublicationPoisoned());
    combo.Popup();
    CHECK_FALSE(combo.WinUIIsPeerDropDownOpenForTesting());
}

TEST_CASE("wxWinUI ComboBox shutdown seal permanently rejects Popup",
          "[winui-itemmodel][winui-combo][popup][retirement][seal]")
{
    wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    const auto resetPoison = wxMakeGuard([]()
    {
        wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    });
    wxUnusedVar(resetPoison);

    wxFrame frame(nullptr, wxID_ANY, "sealed live combo");
    const wxString items[] = { "alpha", "beta" };
    wxComboBox combo(&frame, wxID_ANY, wxEmptyString,
                     wxDefaultPosition, wxDefaultSize,
                     WXSIZEOF(items), items, wxCB_READONLY);
    frame.Show();
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(&combo);
    REQUIRE(host != nullptr);
    host->ShutdownForTest();

    combo.Popup();
    CHECK_FALSE(combo.WinUIIsPeerDropDownOpenForTesting());
    CHECK_FALSE(wxWinUIIsPhysicalDisconnectPublicationPoisoned());
}

#endif // wxUSE_COMBOBOX

TEST_CASE("wxWinUI Choice popup highlight follows its stable item ID",
          "[winui-itemmodel][choice][current-selection][sort][parity]")
{
    wxFrame frame(nullptr, wxID_ANY, "sorted popup identity");
    const wxString items[] = { "charlie", "alpha", "mike" };
    wxChoice choice(&frame, wxID_ANY, wxDefaultPosition,
                    wxDefaultSize, WXSIZEOF(items), items, wxCB_SORT);
    frame.Show();

    const auto indexOfId = [&](std::uint64_t id)
    {
        for ( unsigned n = 0; n < choice.GetCount(); ++n )
        {
            if ( choice.WinUIGetItemIdForTesting(n) == id )
                return static_cast<int>(n);
        }
        return wxNOT_FOUND;
    };

    choice.SetSelection(choice.FindString("alpha"));
    EventCounter selectionEvents(&choice, wxEVT_CHOICE);
    REQUIRE(choice.WinUISetDropDownForTesting(true));
    const int mike = choice.FindString("mike");
    REQUIRE(mike != wxNOT_FOUND);
    const std::uint64_t pendingId =
        choice.WinUIGetItemIdForTesting(mike);
    REQUIRE(choice.WinUISelectPeerItemForTesting(mike));

    // Insert, sorted rename/move and unrelated deletion all keep the pending
    // highlight attached to the same logical item, without committing it.
    REQUIRE(choice.Append("aardvark") != wxNOT_FOUND);
    int pendingIndex = indexOfId(pendingId);
    REQUIRE(pendingIndex != wxNOT_FOUND);
    CHECK(choice.GetCurrentSelection() == pendingIndex);
    choice.SetString(pendingIndex, "zulu");
    pendingIndex = indexOfId(pendingId);
    REQUIRE(pendingIndex != wxNOT_FOUND);
    CHECK(choice.GetCurrentSelection() == pendingIndex);
    const int charlie = choice.FindString("charlie");
    REQUIRE(charlie != wxNOT_FOUND);
    choice.Delete(charlie);
    pendingIndex = indexOfId(pendingId);
    REQUIRE(pendingIndex != wxNOT_FOUND);
    CHECK(choice.GetCurrentSelection() == pendingIndex);
    CHECK(selectionEvents.GetCount() == 0);

    REQUIRE(choice.WinUISetDropDownForTesting(false));
    CHECK(choice.GetSelection() == indexOfId(pendingId));
    CHECK(selectionEvents.GetCount() == 1);

    // Only deletion of the highlighted ID invalidates it. The peer falls back
    // to the accepted item and close cannot commit a stale numeric index.
    REQUIRE(choice.WinUISetDropDownForTesting(true));
    REQUIRE(WaitFor("sorted wxChoice causal popup reopen", [&choice]()
    {
        return choice.WinUIIsPeerDropDownOpenForTesting();
    }));
    const int alpha = choice.FindString("alpha");
    REQUIRE(alpha != wxNOT_FOUND);
    REQUIRE(choice.WinUISelectPeerItemForTesting(alpha));
    choice.Delete(alpha);
    CHECK(choice.GetCurrentSelection() == choice.GetSelection());
    REQUIRE(choice.WinUISetDropDownForTesting(false));
    CHECK(choice.WinUIGetItemIdForTesting(choice.GetSelection()) ==
          pendingId);
    CHECK(selectionEvents.GetCount() == 1);
}

TEST_CASE("wxWinUI Choice preserves sorted ownership and selection",
          "[winui-itemmodel][choice]")
{
    wxFrame frame(nullptr, wxID_ANY, "sorted choice owner");
    int destroyed = 0;
    int selectionEvents = 0;

    {
        wxChoice choice(&frame, wxID_ANY, wxDefaultPosition,
                        wxDefaultSize, 0, nullptr, wxCB_SORT);
        choice.Bind(wxEVT_CHOICE,
                    [&selectionEvents](wxCommandEvent&)
                    {
                        ++selectionEvents;
                    });

        choice.Append("aaa");
        choice.Append("Aaa");
        choice.Append("AAA");
        REQUIRE(choice.GetCount() == 3);
        CHECK(choice.GetString(0) == "AAA");
        CHECK(choice.GetString(1) == "Aaa");
        CHECK(choice.GetString(2) == "aaa");

        choice.SetClientObject(
            2, new wxWinUICountedClientData(&destroyed));
        const std::uint64_t selectedId =
            choice.WinUIGetItemIdForTesting(2);
        const std::uintptr_t selectedPeer =
            choice.WinUIGetItemPeerIdentityForTesting(2);
        choice.SetSelection(2);
        CHECK(selectionEvents == 0);

        choice.SetString(2, "AA");
        CHECK(choice.GetSelection() == 0);
        CHECK(choice.WinUIGetItemIdForTesting(0) == selectedId);
        CHECK(choice.WinUIGetItemPeerIdentityForTesting(0) ==
              selectedPeer);
        CHECK(choice.GetClientObject(0) != nullptr);
        CHECK(selectionEvents == 0);

        choice.SetClientObject(
            0, new wxWinUICountedClientData(&destroyed));
        CHECK(destroyed == 1);
        choice.Delete(0);
        CHECK(destroyed == 2);
    }

    CHECK(destroyed == 2);
}

TEST_CASE("wxWinUI Choice peer selection is destruction-safe",
          "[winui-itemmodel][choice][lifetime]")
{
    wxFrame frame(nullptr, wxID_ANY, "choice callback owner");
    wxChoice *choice = new wxChoice(&frame, wxID_ANY);
    choice->Append("zero");
    choice->Append("one");

    int clientSentinel = 0;
    choice->SetClientData(1, &clientSentinel);
    unsigned events = 0;
    void *eventData = nullptr;
    const auto observeSelection = [&](wxCommandEvent& event)
    {
        ++events;
        eventData = event.GetClientData();
    };
    choice->Bind(wxEVT_CHOICE, observeSelection);

    REQUIRE(choice->WinUISelectPeerItemForTesting(1));
    CHECK(events == 1);
    CHECK(eventData == &clientSentinel);
    CHECK(choice->GetSelection() == 1);

    REQUIRE(choice->Unbind(wxEVT_CHOICE, observeSelection));
    choice->Bind(wxEVT_CHOICE, [&](wxCommandEvent&)
    {
        wxChoice * const doomed = choice;
        choice = nullptr;
        delete doomed;
    });
    REQUIRE(choice->WinUISelectPeerItemForTesting(0));
    CHECK(choice == nullptr);
}

#if wxUSE_COMBOBOX

TEST_CASE("wxWinUI ComboBox popup keeps accepted selection authoritative",
          "[winui-itemmodel][winui-combo][popup][parity]")
{
    wxFrame frame(nullptr, wxID_ANY, "combo popup selection");
    wxPanel panel(&frame);
    const wxString items[] = { "zero", "one", "two" };
    wxComboBox combo(&panel, wxID_ANY, wxString(),
                     wxDefaultPosition, wxDefaultSize,
                     WXSIZEOF(items), items);
    combo.SetSelection(0);
    frame.Show();
    RequireWinUIComboTemplate(combo);

    EventCounter dropDownEvents(&combo, wxEVT_COMBOBOX_DROPDOWN);
    EventCounter closeUpEvents(&combo, wxEVT_COMBOBOX_CLOSEUP);
    EventCounter selectionEvents(&combo, wxEVT_COMBOBOX);

    combo.Popup();
    CHECK(dropDownEvents.GetCount() == 1);
    REQUIRE(combo.WinUISelectPeerItemForTesting(2));
    CHECK(combo.GetSelection() == 0);
    CHECK(combo.GetCurrentSelection() == 2);
    CHECK(selectionEvents.GetCount() == 0);

    combo.SetSelection(1);
    CHECK(combo.GetSelection() == 1);
    CHECK(combo.GetCurrentSelection() == 1);
    CHECK(selectionEvents.GetCount() == 0);

    combo.Dismiss();
    CHECK(closeUpEvents.GetCount() == 1);
    CHECK(combo.GetSelection() == 1);
    CHECK(combo.GetCurrentSelection() == 1);
    CHECK(selectionEvents.GetCount() == 0);

    combo.SetFocus();
    REQUIRE(WaitFor("ComboBox Escape popup focus", [&combo]()
    {
        return wxWindow::FindFocus() == &combo && ::GetFocus();
    }));
    combo.Popup();
    REQUIRE(WaitFor("ComboBox causal Escape popup reopen", [&]()
    {
        return dropDownEvents.GetCount() == 2 &&
               combo.WinUIIsPeerDropDownOpenForTesting();
    }));
    REQUIRE(combo.WinUISelectPeerItemForTesting(2));
    CHECK(combo.GetSelection() == 1);
    CHECK(combo.GetCurrentSelection() == 2);
    REQUIRE(wxWinUIDispatchComboKey(::GetFocus(), VK_ESCAPE));
    REQUIRE(WaitFor("ComboBox Escape cancels popup highlight", [&]()
    {
        return closeUpEvents.GetCount() == 2;
    }));
    CHECK(combo.GetSelection() == 1);
    CHECK(combo.GetCurrentSelection() == 1);
    CHECK(selectionEvents.GetCount() == 0);
}

TEST_CASE("wxWinUI ComboBox selection events retain the wxMSW snapshot",
          "[winui-itemmodel][winui-combo][events][reentrant][parity]")
{
    const long style = GENERATE(0, wxCB_SIMPLE);
    wxFrame frame(nullptr, wxID_ANY, "combo event snapshots");
    wxPanel panel(&frame);
    const wxString items[] = { "zero", "one", "two" };
    wxComboBox combo(&panel, wxID_ANY, wxString(),
                     wxDefaultPosition,
                     style == wxCB_SIMPLE ? wxSize(220, 150)
                                          : wxDefaultSize,
                     WXSIZEOF(items), items, style);
    frame.Show();
    if ( style != wxCB_SIMPLE )
        RequireWinUIComboTemplate(combo);
    int oneData = 1;
    int twoData = 2;
    combo.SetClientData(1, &oneData);
    combo.SetClientData(2, &twoData);

    SECTION("reentrant SetSelection")
    {
        wxString textValue;
        void *textData = nullptr;
        int textEventCount = 0;
        combo.Bind(wxEVT_COMBOBOX, [&](wxCommandEvent& event)
        {
            CHECK(event.GetString() == "one");
            CHECK(event.GetClientData() == &oneData);
            combo.SetSelection(2);
        });
        combo.Bind(wxEVT_TEXT, [&](wxCommandEvent& event)
        {
            ++textEventCount;
            textValue = event.GetString();
            textData = event.GetClientData();
        });
        REQUIRE(combo.WinUISelectPeerItemForTesting(1));
        REQUIRE(WaitFor("ComboBox selection snapshot projection drain", [&]()
        {
            if ( style == wxCB_SIMPLE )
            {
                wxComboBox::WinUISimplePeerSnapshot snapshot;
                return combo.WinUIGetSimplePeerSnapshotForTesting(&snapshot) &&
                       !(snapshot.phase &
                         wxComboBox::WinUITemplatePhase_PendingText);
            }
            wxComboBox::WinUITemplatePeerSnapshot snapshot;
            return combo.WinUIGetTemplatePeerSnapshotForTesting(&snapshot) &&
                   !(snapshot.phase &
                     wxComboBox::WinUITemplatePhase_PendingText);
        }));
        INFO(wxWinUIComboDiagnostics(combo));
        CHECK(textEventCount == 1);
        CHECK(textValue == "one");
        CHECK(textData == &oneData);
        CHECK(combo.GetSelection() == 2);
        CHECK(combo.GetValue() == "two");
    }

    SECTION("reentrant deletion")
    {
        wxString textValue;
        void *textData = &oneData;
        combo.Bind(wxEVT_COMBOBOX, [&](wxCommandEvent&)
        {
            combo.Delete(1);
        });
        combo.Bind(wxEVT_TEXT, [&](wxCommandEvent& event)
        {
            textValue = event.GetString();
            textData = event.GetClientData();
        });
        REQUIRE(combo.WinUISelectPeerItemForTesting(1));
        CHECK(textValue == "one");
        CHECK(textData == nullptr);
    }
}

TEST_CASE("wxWinUI ComboBox popup teardown leaves the next template focus clean",
           "[winui-itemmodel][winui-combo][popup][lifetime][order]")
{
    const unsigned physicalGateBaseline =
        wxWinUITopLevelHost::GetPendingPhysicalDisconnectGateCountForTest();
    const unsigned terminalRetirementBaseline =
        wxWinUITopLevelHost::GetPendingTLWTerminalRetirementCountForTest();

    // Preserve the exact popup predecessor which exposed the deferred XAML
    // LostFocus failure in declaration order, including its Escape close.
    {
        wxFrame frame(nullptr, wxID_ANY, "combo popup selection pair");
        wxPanel panel(&frame);
        const wxString items[] = { "zero", "one", "two" };
        wxComboBox combo(&panel, wxID_ANY, wxString(),
                         wxDefaultPosition, wxDefaultSize,
                         WXSIZEOF(items), items);
        combo.SetSelection(0);
        frame.Show();
        RequireWinUIComboTemplate(combo);

        EventCounter dropDownEvents(&combo, wxEVT_COMBOBOX_DROPDOWN);
        EventCounter closeUpEvents(&combo, wxEVT_COMBOBOX_CLOSEUP);
        EventCounter selectionEvents(&combo, wxEVT_COMBOBOX);

        combo.Popup();
        CHECK(dropDownEvents.GetCount() == 1);
        REQUIRE(combo.WinUISelectPeerItemForTesting(2));
        CHECK(combo.GetSelection() == 0);
        CHECK(combo.GetCurrentSelection() == 2);
        CHECK(selectionEvents.GetCount() == 0);

        combo.SetSelection(1);
        CHECK(combo.GetSelection() == 1);
        CHECK(combo.GetCurrentSelection() == 1);
        CHECK(selectionEvents.GetCount() == 0);

        combo.Dismiss();
        CHECK(closeUpEvents.GetCount() == 1);
        CHECK(combo.GetSelection() == 1);
        CHECK(combo.GetCurrentSelection() == 1);
        CHECK(selectionEvents.GetCount() == 0);

        combo.SetFocus();
        REQUIRE(WaitFor("paired ComboBox Escape popup focus", [&combo]()
        {
            return wxWindow::FindFocus() == &combo && ::GetFocus();
        }));
        combo.Popup();
        REQUIRE(WaitFor("paired ComboBox causal Escape popup reopen", [&]()
        {
            return dropDownEvents.GetCount() == 2 &&
                   combo.WinUIIsPeerDropDownOpenForTesting();
        }));
        REQUIRE(combo.WinUISelectPeerItemForTesting(2));
        CHECK(combo.GetSelection() == 1);
        CHECK(combo.GetCurrentSelection() == 2);
        REQUIRE(wxWinUIDispatchComboKey(::GetFocus(), VK_ESCAPE));
        REQUIRE(WaitFor("paired ComboBox Escape cancels popup highlight", [&]()
        {
            return closeUpEvents.GetCount() == 2;
        }));
        CHECK(combo.GetSelection() == 1);
        CHECK(combo.GetCurrentSelection() == 1);
        CHECK(selectionEvents.GetCount() == 0);
    }

    // The deferred error used to surface at the very first Yield() in this
    // next normal-style template realization, before its predicate ran.
    {
        wxFrame frame(nullptr, wxID_ANY, "combo snapshot pair");
        wxPanel panel(&frame);
        const wxString items[] = { "zero", "one", "two" };
        wxComboBox combo(&panel, wxID_ANY, wxString(),
                         wxDefaultPosition, wxDefaultSize,
                         WXSIZEOF(items), items);
        combo.SetSelection(1);
        frame.Show();
        RequireWinUIComboTemplate(combo);
        REQUIRE(
            wxWinUITopLevelHost::
                GetPendingPhysicalDisconnectGateCountForTest() ==
            physicalGateBaseline);
        // The previous case in the paired regression can leave one causal TLW
        // retirement until this first template pump. It is correct for that
        // shared counter to decrease here; this case must only prove that it
        // did not publish any new terminal debt of its own.
        REQUIRE(
            wxWinUITopLevelHost::
                GetPendingTLWTerminalRetirementCountForTest() <=
            terminalRetirementBaseline);
        REQUIRE_FALSE(wxWinUIIsPhysicalDisconnectPublicationPoisoned());
        combo.SetFocus();
        CHECK(wxWindow::FindFocus() == &combo);
        CHECK(combo.GetSelection() == 1);
        CHECK(combo.GetStringSelection() == "one");
    }
}

TEST_CASE("wxWinUI sorted ComboBox resolves snapshot client data by ID",
          "[winui-itemmodel][winui-combo][events][reentrant][sort][parity]")
{
    wxFrame frame(nullptr, wxID_ANY, "sorted combo event snapshot");
    const wxString items[] = { "charlie", "alpha", "bravo" };
    wxComboBox combo(&frame, wxID_ANY, wxString(), wxDefaultPosition,
                     wxDefaultSize, WXSIZEOF(items), items, wxCB_SORT);
    frame.Show();
    RequireWinUIComboTemplate(combo);
    const int bravo = combo.FindString("bravo");
    REQUIRE(bravo != wxNOT_FOUND);
    int stableData = 7;
    combo.SetClientData(bravo, &stableData);
    wxString textValue;
    void *textData = nullptr;
    int textEventCount = 0;
    combo.Bind(wxEVT_COMBOBOX, [&](wxCommandEvent&)
    {
        const int current = combo.FindString("bravo");
        REQUIRE(current != wxNOT_FOUND);
        combo.SetString(current, "zulu");
    });
    combo.Bind(wxEVT_TEXT, [&](wxCommandEvent& event)
    {
        ++textEventCount;
        textValue = event.GetString();
        textData = event.GetClientData();
    });
    REQUIRE(combo.WinUISelectPeerItemForTesting(bravo));
    REQUIRE(WaitFor("sorted ComboBox snapshot projection drain", [&]()
    {
        wxComboBox::WinUITemplatePeerSnapshot snapshot;
        return combo.WinUIGetTemplatePeerSnapshotForTesting(&snapshot) &&
               !(snapshot.phase &
                 wxComboBox::WinUITemplatePhase_PendingText);
    }));
    INFO(wxWinUIComboDiagnostics(combo));
    CHECK(textEventCount == 1);
    CHECK(textValue == "bravo");
    CHECK(textData == &stableData);
    CHECK(combo.GetStringSelection() == "zulu");
}

TEST_CASE("wxWinUI ComboBox text-entry policies target the XAML editor",
           "[winui-itemmodel][winui-combo][textentry][parity]")
{
    wxFrame frame(nullptr, wxID_ANY, "combo text-entry policies");
    wxPanel panel(&frame);
    const wxString items[] = { "zero", "one", "two" };
    const long style = GENERATE(0, wxCB_SIMPLE);
    wxComboBox combo(&panel, wxID_ANY, "MiXeD",
                     wxDefaultPosition,
                     style == wxCB_SIMPLE ? wxSize(220, 150)
                                          : wxDefaultSize,
                     WXSIZEOF(items), items, style);
    frame.Show();
    if ( style != wxCB_SIMPLE )
        RequireWinUIComboTemplate(combo);

    combo.SetSelection(1, 4);
    combo.SetMaxLength(17);
    combo.ForceUpper();
    CHECK(combo.GetValue() == "MIXED");
    long from = -1;
    long to = -1;
    combo.GetSelection(&from, &to);
    CHECK(from == 1);
    CHECK(to == 4);
    CHECK(combo.SetMargins(7, -1));

    unsigned long peerMaxLength = 0;
    bool peerForceUpper = false;
    wxPoint peerMargins(-1, -1);
    REQUIRE(combo.WinUIGetTextEntryPeerStateForTesting(
        &peerMaxLength, &peerForceUpper, &peerMargins));
    // Native MaxLength stays disabled so programmatic wxMSW-compatible writes
    // may exceed the limit; peer-originated proposals are constrained by the
    // complete edit-diff transaction.
    CHECK(peerMaxLength == 0);
    CHECK(peerForceUpper);
    CHECK(std::abs(peerMargins.x - 7) <= 1);
    CHECK(peerMargins.y == -1);

    const wxPoint publicMargins = combo.GetMargins();
    CHECK(std::abs(publicMargins.x - 7) <= 1);
    CHECK(publicMargins.y == -1);

    combo.SetValue("again");
    CHECK(combo.GetValue() == "AGAIN");
    combo.SetInsertionPointEnd();
    combo.WriteText("mixed");
    CHECK(combo.GetValue() == "AGAINMIXED");

    combo.SetMaxLength(4);
    combo.SetValue("programmatic");
    combo.SetInsertionPointEnd();
    combo.WriteText("!");
    CHECK(combo.GetValue() == "PROGRAMMATIC!");

    // Win32 EM_LIMITTEXT applies its budget to the inserted span, not to the
    // untouched programmatic overflow. Replacing one selected character when
    // the preserved text is already over limit deletes it but rejects X.
    combo.SetValue("PROGRAMMATIC");
    combo.SetSelection(2, 3);
    EventCounter textEvents(&combo, wxEVT_TEXT);
    EventCounter maxLengthEvents(&combo, wxEVT_TEXT_MAXLEN);
    std::vector<wxEventType> lengthEventOrder;
    wxString lengthTextValue;
    wxString lengthMaxValue;
    combo.Bind(wxEVT_TEXT, [&](wxCommandEvent& event)
    {
        lengthEventOrder.push_back(event.GetEventType());
        lengthTextValue = event.GetString();
        event.Skip();
    });
    combo.Bind(wxEVT_TEXT_MAXLEN, [&](wxCommandEvent& event)
    {
        lengthEventOrder.push_back(event.GetEventType());
        lengthMaxValue = event.GetString();
        event.Skip();
    });
    REQUIRE(combo.WinUISetPeerTextForTesting("PRXGRAMMATIC"));
    CHECK(combo.GetValue() == "PRGRAMMATIC");
    CHECK(textEvents.GetCount() == 1);
    CHECK(maxLengthEvents.GetCount() == 1);
    REQUIRE(lengthEventOrder.size() == 2);
    CHECK(lengthEventOrder[0] == wxEVT_TEXT);
    CHECK(lengthEventOrder[1] == wxEVT_TEXT_MAXLEN);
    CHECK(lengthTextValue == "PRGRAMMATIC");
    CHECK(lengthMaxValue == "PRGRAMMATIC");

    combo.SetValue("PROGRAMMATIC");
    combo.SetSelection(2, 3);
    textEvents.Clear();
    maxLengthEvents.Clear();
    lengthEventOrder.clear();
    REQUIRE(combo.WinUISetPeerTextForTesting("PRGRAMMATIC"));
    CHECK(combo.GetValue() == "PRGRAMMATIC");
    CHECK(textEvents.GetCount() == 1);
    CHECK(maxLengthEvents.GetCount() == 0);
    REQUIRE(lengthEventOrder.size() == 1);
    CHECK(lengthEventOrder[0] == wxEVT_TEXT);

    combo.SetValue("ab");
    textEvents.Clear();
    maxLengthEvents.Clear();
    lengthEventOrder.clear();
    REQUIRE(combo.WinUISetPeerTextForTesting("abcdef"));
    CHECK(combo.GetValue() == "ABCD");
    CHECK(textEvents.GetCount() == 1);
    CHECK(maxLengthEvents.GetCount() == 1);

    // A fitting prefix must never end between a UTF-16 surrogate pair.
    combo.SetMaxLength(2);
    combo.SetValue(wxString());
    textEvents.Clear();
    maxLengthEvents.Clear();
    const wxString unicodeProposal =
        wxString::FromUTF8("A\xf0\x9f\x98\x80Z");
    REQUIRE(combo.WinUISetPeerTextForTesting(unicodeProposal));
    CHECK(combo.GetValue() == "A");
    CHECK(textEvents.GetCount() == 1);
    CHECK(maxLengthEvents.GetCount() == 1);
}

TEST_CASE("wxWinUI ComboBox constrains autocomplete as user input",
          "[winui-itemmodel][winui-combo][autocomplete][maxlength][parity]")
{
    const long style = GENERATE(0, wxCB_SIMPLE);
    wxFrame frame(nullptr, wxID_ANY, "combo completion length");
    wxComboBox combo(&frame, wxID_ANY, wxString(), wxDefaultPosition,
                     style == wxCB_SIMPLE ? wxSize(220, 150)
                                          : wxDefaultSize,
                     0, nullptr, style);
    frame.Show();
    if ( style != wxCB_SIMPLE )
        RequireWinUIComboTemplate(combo);
    // Auto-completion is driven by a real editor input transaction. In
    // particular, a transient MenuFlyout is allowed to close while XAML focus
    // is still moving into the editor. Wait for the same exact focus authority
    // a user must have before they can type the prefix; wxCB_SIMPLE doesn't
    // otherwise have the template-realization pump used by the normal peer.
    combo.SetFocus();
    REQUIRE(WaitFor("max-length autocomplete editor focus", [&combo]()
    {
        return wxWinUIComboHasExactEditorFocus(combo);
    }));
    combo.SetMaxLength(4);
    REQUIRE(combo.WinUISetPeerTextForTesting("al"));
    wxArrayString values;
    values.Add("alphabet");
    REQUIRE(combo.AutoComplete(values));
    REQUIRE(WaitFor("max-length autocomplete flyout", [&]()
    {
        return combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 1;
    }));
    EventCounter textEvents(&combo, wxEVT_TEXT);
    EventCounter maxLengthEvents(&combo, wxEVT_TEXT_MAXLEN);
    REQUIRE(combo.WinUIInvokeAutoCompleteSuggestionForTesting(0));
    CHECK(combo.GetValue() == "alph");
    CHECK(textEvents.GetCount() == 1);
    CHECK(maxLengthEvents.GetCount() == 1);
}

TEST_CASE("wxWinUI ComboBox clipboard commands use the real XAML editor",
          "[winui-itemmodel][winui-combo][clipboard][lifetime][parity]")
{
    wxFrame frame(nullptr, wxID_ANY, "combo clipboard commands");
    wxPanel panel(&frame);
    const long style = GENERATE(0, wxCB_SIMPLE);
    wxComboBox combo(&panel, wxID_ANY, "abcdef",
                     wxPoint(10, 10),
                     style == wxCB_SIMPLE ? wxSize(220, 150)
                                          : wxSize(220, 32),
                     0, nullptr, style);
    frame.SetClientSize(wxSize(320, 260));
    panel.SetSize(frame.GetClientSize());
    frame.Show();
    if ( style != wxCB_SIMPLE )
        RequireWinUIComboTemplate(combo);

    int copyEvents = 0;
    int cutEvents = 0;
    int pasteEvents = 0;
    combo.Bind(wxEVT_TEXT_COPY, [&](wxClipboardTextEvent&)
    {
        ++copyEvents;
    });
    combo.Bind(wxEVT_TEXT_CUT, [&](wxClipboardTextEvent&)
    {
        ++cutEvents;
    });
    combo.Bind(wxEVT_TEXT_PASTE, [&](wxClipboardTextEvent&)
    {
        ++pasteEvents;
    });

    // Public commands enter the same cancellable wx transaction before the
    // real TextBox operation. A handled event vetoes without changing text.
    combo.SetSelection(1, 4);
    combo.Copy();
    combo.Cut();
    combo.Paste();
    CHECK(copyEvents == 1);
    CHECK(cutEvents == 1);
    CHECK(pasteEvents == 1);
    CHECK(combo.GetValue() == "abcdef");

    // Feed each real Ctrl shortcut to the focused TextBox through the island
    // dispatcher. The routed PreviewKeyDown enters the same veto boundary,
    // and cancellation happens before any clipboard read/write or mutation.
    const auto prepareRoutedClipboard = [&](const char *description)
    {
        combo.SetFocus();
        REQUIRE(WaitFor(description, [&combo]()
        {
            return wxWinUIComboHasExactEditorFocus(combo);
        }));
        combo.SetSelection(1, 4);
        REQUIRE(wxWinUIComboHasExactPeerRange(combo, 1, 4));
    };
    prepareRoutedClipboard("ComboBox exact clipboard editor focus (copy)");
    const bool invokedCopy =
        combo.WinUIInvokeClipboardCommandForTesting(wxEVT_TEXT_COPY);
    INFO(wxWinUIComboDiagnostics(combo));
    REQUIRE(invokedCopy);
    CHECK(copyEvents == 2);
    prepareRoutedClipboard("ComboBox exact clipboard editor focus (cut)");
    const bool invokedCut =
        combo.WinUIInvokeClipboardCommandForTesting(wxEVT_TEXT_CUT);
    INFO(wxWinUIComboDiagnostics(combo));
    REQUIRE(invokedCut);
    CHECK(cutEvents == 2);
    prepareRoutedClipboard("ComboBox exact clipboard editor focus (paste)");
    const bool invokedPaste =
        combo.WinUIInvokeClipboardCommandForTesting(wxEVT_TEXT_PASTE);
    INFO(wxWinUIComboDiagnostics(combo));
    REQUIRE(invokedPaste);
    CHECK(pasteEvents == 2);
    CHECK(combo.GetValue() == "abcdef");

    // Repeat the routed production command after an independent focus/range
    // transaction. The seam supplies Ctrl to the real PreviewKeyDown handler
    // without mutating process-global keyboard state or generating a bogus
    // character message; the UIActionSimulator coverage below retains the
    // physical shortcut oracle.
    prepareRoutedClipboard("ComboBox clipboard focus (copy replay)");
    REQUIRE(combo.WinUIInvokeClipboardCommandForTesting(wxEVT_TEXT_COPY));
    prepareRoutedClipboard("ComboBox clipboard focus (cut replay)");
    REQUIRE(combo.WinUIInvokeClipboardCommandForTesting(wxEVT_TEXT_CUT));
    prepareRoutedClipboard("ComboBox clipboard focus (paste replay)");
    REQUIRE(combo.WinUIInvokeClipboardCommandForTesting(wxEVT_TEXT_PASTE));
    INFO(wxWinUIComboDiagnostics(combo));
    CHECK(copyEvents == 3);
    CHECK(cutEvents == 3);
    CHECK(pasteEvents == 3);
    CHECK(combo.GetValue() == "abcdef");

    // Retemplating from the wx veto boundary retires the exact sender. Even
    // when the handler calls Skip(), the old XAML command must not continue.
    if ( style != wxCB_SIMPLE )
    {
        wxComboBox retemplated(&panel, wxID_ANY, "abcdef",
                               wxPoint(10, 60), wxSize(220, 32));
        RequireWinUIComboTemplate(retemplated);
        retemplated.SetFocus();
        REQUIRE(WaitFor("retemplated ComboBox clipboard focus",
                        [&retemplated]()
        {
            return wxWinUIComboHasExactEditorFocus(retemplated);
        }));
        retemplated.SetSelection(1, 4);
        REQUIRE(wxWinUIComboHasExactPeerRange(retemplated, 1, 4));
        std::uintptr_t originalEditIdentity = 0;
        std::uint64_t originalEditGeneration = 0;
        wxComboBox::WinUIDiagnosticSnapshot beforeClipboardRetemplate;
        REQUIRE(retemplated.WinUIGetTextEntryPeerStateForTesting(
            nullptr, nullptr, nullptr,
            &originalEditIdentity, &originalEditGeneration));
        REQUIRE(retemplated.WinUIGetDiagnosticSnapshotForTesting(
            &beforeClipboardRetemplate));
        REQUIRE(originalEditIdentity != 0);
        REQUIRE(originalEditGeneration != 0);
        bool didRetemplate = false;
        int reentrantCutEvents = 0;
        retemplated.Bind(wxEVT_TEXT_CUT,
            [&](wxClipboardTextEvent& event)
            {
                ++reentrantCutEvents;
                didRetemplate =
                    retemplated.WinUIRetemplateForTesting();
                event.Skip();
            });
        REQUIRE(retemplated.WinUIInvokeClipboardCommandForTesting(
            wxEVT_TEXT_CUT));
        CHECK(reentrantCutEvents == 1);
        CHECK(didRetemplate);
        CHECK(retemplated.GetValue() == "abcdef");
        wxComboBox::WinUIDiagnosticSnapshot acceptedClipboardRetemplate;
        REQUIRE(retemplated.WinUIGetDiagnosticSnapshotForTesting(
            &acceptedClipboardRetemplate));
        CHECK(acceptedClipboardRetemplate.comboLayoutRealizations ==
              beforeClipboardRetemplate.comboLayoutRealizations);
        CHECK(acceptedClipboardRetemplate.
                  comboLayoutSynchronousRealizations ==
              beforeClipboardRetemplate.
                  comboLayoutSynchronousRealizations);

        wxComboBox::WinUITemplatePeerSnapshot replacementPeer;
        wxComboBox::WinUIDiagnosticSnapshot replacementDiagnostic;
        REQUIRE(WaitForSettledWinUIComboTemplateGeneration(
            retemplated, originalEditGeneration,
            &replacementPeer, &replacementDiagnostic));
        const std::uintptr_t replacementEditIdentity =
            replacementPeer.editIdentity;
        CHECK(replacementEditIdentity != 0);
        CHECK(replacementPeer.editGeneration > originalEditGeneration);
        CHECK(replacementDiagnostic.comboLayoutRealizations ==
              beforeClipboardRetemplate.comboLayoutRealizations);
        CHECK(replacementDiagnostic.comboLayoutSynchronousRealizations ==
              beforeClipboardRetemplate.
                  comboLayoutSynchronousRealizations);
        long currentFrom = -1;
        long currentTo = -1;
        retemplated.GetSelection(&currentFrom, &currentTo);
        CHECK(currentFrom == 1);
        CHECK(currentTo == 4);
        if ( replacementEditIdentity != originalEditIdentity )
        {
            REQUIRE(retemplated.WinUISetEditSelectionForTesting(
                0, 1, true));
        }
        else
        {
            // The ABI object was reused, so the retained old projection aliases
            // the current sender and must not be mutated. Generation advancement
            // and unchanged callback/ticket diagnostics prove the old delegate
            // tokens were retired at the wx veto boundary.
            wxComboBox::WinUIDiagnosticSnapshot aliasedSenderDiagnostic;
            REQUIRE(retemplated.WinUIGetDiagnosticSnapshotForTesting(
                &aliasedSenderDiagnostic));
            CHECK((retemplated.WinUIGetTemplateStateForTesting() & 0x7) ==
                  0x7);
            CHECK(aliasedSenderDiagnostic.peerTextCallbacks ==
                  replacementDiagnostic.peerTextCallbacks);
            CHECK(aliasedSenderDiagnostic.rangeProjectionMismatches ==
                  replacementDiagnostic.rangeProjectionMismatches);
            CHECK(aliasedSenderDiagnostic.rangeFinalizations ==
                  replacementDiagnostic.rangeFinalizations);
            CHECK(aliasedSenderDiagnostic.pendingRange ==
                  replacementDiagnostic.pendingRange);
        }
        long afterRetiredFrom = -1;
        long afterRetiredTo = -1;
        retemplated.GetSelection(&afterRetiredFrom, &afterRetiredTo);
        CHECK(afterRetiredFrom == currentFrom);
        CHECK(afterRetiredTo == currentTo);
    }

    // Destruction from the real XAML event invalidates the callback state
    // before the delegate resumes and attempts to cancel the native command.
    wxComboBox *doomed = new wxComboBox(
        &panel, wxID_ANY, "abcdef", wxPoint(10, 210), wxSize(220, 32));
    RequireWinUIComboTemplate(*doomed);
    doomed->SetFocus();
    REQUIRE(WaitFor("doomed ComboBox clipboard focus", [&doomed]()
    {
        return doomed && wxWinUIComboHasExactEditorFocus(*doomed);
    }));
    doomed->SetSelection(0, 3);
    REQUIRE(wxWinUIComboHasExactPeerRange(*doomed, 0, 3));
    doomed->Bind(wxEVT_TEXT_COPY,
        [&doomed](wxClipboardTextEvent&)
        {
            wxComboBox * const deleting = doomed;
            doomed = nullptr;
            delete deleting;
        });
    const bool invoked =
        doomed->WinUIInvokeClipboardCommandForTesting(wxEVT_TEXT_COPY);
    CHECK(invoked);
    CHECK(doomed == nullptr);

    // Programmatic entry has the same destruction boundary even when the
    // platform action simulator is disabled in a build configuration.
    wxComboBox *programmatic = new wxComboBox(
        &panel, wxID_ANY, "abcdef", wxPoint(10, 170), wxSize(220, 32));
    programmatic->Bind(wxEVT_TEXT_PASTE,
        [&programmatic](wxClipboardTextEvent&)
        {
            wxComboBox * const deleting = programmatic;
            programmatic = nullptr;
            delete deleting;
        });
    programmatic->Paste();
    CHECK(programmatic == nullptr);
}

#if wxUSE_UIACTIONSIMULATOR && wxUSE_CLIPBOARD

TEST_CASE("wxWinUI ComboBox physical clipboard shortcut round trip",
          "[.][winui-itemmodel][winui-combo][physical][clipboard]")
{
    // Hidden/manual by design: wxUIActionSimulator uses SendInput and this
    // test intentionally exercises the process clipboard on an isolated
    // desktop. The RAII snapshot restores every original OLE format.
    wxWinUIClipboardSnapshot clipboardSnapshot;
    if ( !clipboardSnapshot.IsOk() )
    {
        WARN("OLE clipboard snapshot is unavailable");
        return;
    }

    wxFrame frame(nullptr, wxID_ANY, "physical combo clipboard");
    wxPanel panel(&frame);
    const long style = GENERATE(0, wxCB_SIMPLE);
    wxComboBox combo(&panel, wxID_ANY, "abcdef", wxPoint(10, 10),
                     style == wxCB_SIMPLE ? wxSize(220, 150)
                                          : wxSize(220, 32),
                     0, nullptr, style);
    frame.SetClientSize(wxSize(320, 220));
    panel.SetSize(frame.GetClientSize());
    frame.Show();

    int copyEvents = 0;
    int cutEvents = 0;
    int pasteEvents = 0;
    combo.Bind(wxEVT_TEXT_COPY, [&](wxClipboardTextEvent& event)
    {
        ++copyEvents;
        event.Skip();
    });
    combo.Bind(wxEVT_TEXT_CUT, [&](wxClipboardTextEvent& event)
    {
        ++cutEvents;
        event.Skip();
    });
    combo.Bind(wxEVT_TEXT_PASTE, [&](wxClipboardTextEvent& event)
    {
        ++pasteEvents;
        event.Skip();
    });

    combo.SetFocus();
    REQUIRE(WaitFor("physical ComboBox clipboard focus", [&combo]()
    {
        return wxWindow::FindFocus() == &combo && ::GetFocus();
    }));

    wxUIActionSimulator simulator;
    combo.SetSelection(1, 4);
    REQUIRE(simulator.Char('C', wxMOD_CONTROL));
    REQUIRE(WaitFor("physical ComboBox Ctrl+C", [&]()
    {
        return copyEvents == 1;
    }));

    combo.SetSelection(1, 4);
    REQUIRE(simulator.Char('X', wxMOD_CONTROL));
    REQUIRE(WaitFor("physical ComboBox Ctrl+X", [&]()
    {
        return cutEvents == 1 && combo.GetValue() == "aef";
    }));

    combo.SetInsertionPointEnd();
    REQUIRE(simulator.Char('V', wxMOD_CONTROL));
    REQUIRE(WaitFor("physical ComboBox Ctrl+V", [&]()
    {
        return pasteEvents == 1 && combo.GetValue() == "aefbcd";
    }));

    CHECK(clipboardSnapshot.Restore());
}

#endif // wxUSE_UIACTIONSIMULATOR && wxUSE_CLIPBOARD

TEST_CASE("wxWinUI ComboBox auto-completion uses its real XAML editor",
          "[winui-itemmodel][winui-combo][autocomplete][reparent][parity]")
{
    wxFrame source(nullptr, wxID_ANY, "combo completion source");
    wxFrame destination(nullptr, wxID_ANY, "combo completion destination");
    wxPanel sourcePanel(&source);
    wxPanel destinationPanel(&destination);
    const wxString items[] = { "zero", "one", "two" };
    const long style = GENERATE(0, wxCB_SIMPLE);
    wxComboBox previousFocus(&sourcePanel, wxID_ANY, wxString(),
                              wxPoint(10, 10), wxSize(120, 30),
                              0, nullptr, wxCB_READONLY);
    wxComboBox combo(&sourcePanel, wxID_ANY, wxString(),
                      wxDefaultPosition,
                      style == wxCB_SIMPLE ? wxSize(220, 150)
                                           : wxDefaultSize,
                      WXSIZEOF(items), items, style);
    wxComboBox nextFocus(&sourcePanel, wxID_ANY, wxString(),
                          wxPoint(10, 220), wxSize(120, 30),
                          0, nullptr, wxCB_READONLY);
    combo.SetPosition(wxPoint(10, 50));
    source.SetClientSize(wxSize(300, 270));
    destination.SetClientSize(wxSize(300, 220));
    sourcePanel.SetSize(source.GetClientSize());
    destinationPanel.SetSize(destination.GetClientSize());
    source.Show();
    destination.Show();
    if ( style != wxCB_SIMPLE )
        RequireWinUIComboTemplate(combo);

    combo.SetFocus();
    REQUIRE(WaitFor("ComboBox completion edit focus", [&combo]()
    {
        return wxWindow::FindFocus() == &combo && ::GetFocus();
    }));
    REQUIRE(combo.WinUISetPeerTextForTesting("al"));

    wxArrayString fixed;
    fixed.Add("alpha");
    fixed.Add("alpine");
    fixed.Add("beta");
    REQUIRE(combo.AutoComplete(fixed));
    REQUIRE(WaitFor("ComboBox fixed completion flyout", [&combo]()
    {
        return combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 2;
    }));
    CHECK(combo.WinUIGetAutoCompleteSuggestionForTesting(0) == "alpha");
    CHECK(combo.WinUIGetAutoCompleteSuggestionForTesting(1) == "alpine");
    CHECK(combo.WinUIGetAutoCompleteActiveSuggestionForTesting() ==
          wxNOT_FOUND);

    combo.SetEditable(false);
    REQUIRE(WaitFor("disabled ComboBox closes completion", [&combo]()
    {
        return combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 0;
    }));
    CHECK(!combo.WinUIInvokeAutoCompleteSuggestionForTesting(0));
    REQUIRE(combo.WinUISetPeerTextForTesting("blocked"));
    CHECK(combo.GetValue() == "al");
    CHECK(combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 0);
    combo.SetEditable(true);
    REQUIRE(combo.WinUISetPeerTextForTesting("a"));
    REQUIRE(WaitFor("re-enabled ComboBox completion", [&combo]()
    {
        return combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 2;
    }));

    EventCounter textEvents(&combo, wxEVT_TEXT);
    REQUIRE(wxWinUIDispatchComboKey(::GetFocus(), VK_DOWN));
    const int activeAfterFirstDown =
        combo.WinUIGetAutoCompleteActiveSuggestionForTesting();
    REQUIRE(wxWinUIDispatchComboKey(::GetFocus(), VK_DOWN));
    const int activeAfterSecondDown =
        combo.WinUIGetAutoCompleteActiveSuggestionForTesting();
    REQUIRE(wxWinUIDispatchComboKey(::GetFocus(), VK_UP));
    const int activeAfterUp =
        combo.WinUIGetAutoCompleteActiveSuggestionForTesting();
    REQUIRE(wxWinUIDispatchComboKey(::GetFocus(), VK_DOWN));
    const int activeAfterFinalDown =
        combo.WinUIGetAutoCompleteActiveSuggestionForTesting();
    REQUIRE(wxWinUIDispatchComboKey(::GetFocus(), VK_RETURN));
    INFO(wxWinUIComboDiagnostics(combo));
    CHECK(activeAfterFirstDown == 0);
    CHECK(activeAfterSecondDown == 1);
    CHECK(activeAfterUp == 0);
    CHECK(activeAfterFinalDown == 1);
    CHECK(combo.GetValue() == "alpine");
    CHECK(combo.GetInsertionPoint() == combo.GetLastPosition());
    CHECK(textEvents.GetCount() == 1);
    CHECK(combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 0);

    // Keep the real MenuFlyoutItem automation route covered as a complement
    // to (not a substitute for) the focused-editor keyboard pipeline above.
    REQUIRE(combo.WinUISetPeerTextForTesting("al"));
    const bool fixedCompletionReopened =
        WaitFor("ComboBox fixed completion reopen", [&combo]()
    {
        return combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 2;
    });
    INFO(wxWinUIComboDiagnostics(combo));
    REQUIRE(fixedCompletionReopened);
    textEvents.Clear();
    REQUIRE(combo.WinUIInvokeAutoCompleteSuggestionForTesting(0));
    CHECK(combo.GetValue() == "alpha");
    CHECK(combo.GetInsertionPoint() == combo.GetLastPosition());
    CHECK(textEvents.GetCount() == 1);
    CHECK(combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 0);

    wxTextCompleterFixed * const custom = new wxTextCompleterFixed;
    wxArrayString customValues;
    customValues.Add("gamma");
    customValues.Add("garden");
    customValues.Add("omega");
    custom->SetCompletions(customValues);
    REQUIRE(combo.WinUISetPeerTextForTesting("ga"));
    REQUIRE(combo.AutoComplete(custom));
    REQUIRE(WaitFor("ComboBox custom completion flyout", [&combo]()
    {
        return combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 2;
    }));

    REQUIRE(wxWinUIDispatchComboKey(::GetFocus(), VK_DOWN));
    CHECK(combo.WinUIGetAutoCompleteActiveSuggestionForTesting() == 0);
    const wxString beforeEscape = combo.GetValue();
    REQUIRE(wxWinUIDispatchComboKey(::GetFocus(), VK_ESCAPE));
    REQUIRE(WaitFor("ComboBox Escape closes completion", [&combo]()
    {
        return combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 0;
    }));
    CHECK(combo.GetValue() == beforeEscape);

    // A subsequent real edit must open a fresh generation after Escape.
    REQUIRE(combo.WinUISetPeerTextForTesting("g"));
    REQUIRE(WaitFor("ComboBox completion reopens after Escape", [&combo]()
    {
        return combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 2;
    }));
    CHECK(combo.WinUIGetAutoCompleteActiveSuggestionForTesting() ==
          wxNOT_FOUND);

    CHECK(wxWindow::FindFocus() == &combo);
    {
        const wxWinUIKeyboardModifiers modifiers{};
        const wxWinUIScopedKeyboardModifiers scopedModifiers(modifiers);
        REQUIRE(wxWinUIDispatchComboKey(::GetFocus(), VK_TAB));
    }
    REQUIRE(WaitFor("ComboBox completion Tab focus exit", [&nextFocus]()
    {
        return wxWindow::FindFocus() == &nextFocus;
    }));
    REQUIRE(WaitFor("ComboBox completion Tab closes flyout", [&combo]()
    {
        return combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 0;
    }));

    // Reopen the same generation and exercise the reverse traversal through
    // the focused TextBox too. The scoped modifier snapshot is the one used
    // by the production ComboBox KeyDown route; no physical SendInput state
    // participates in this automatic gate.
    combo.SetFocus();
    REQUIRE(WaitFor("ComboBox completion reverse refocus", [&combo]()
    {
        return wxWindow::FindFocus() == &combo && ::GetFocus();
    }));
    REQUIRE(combo.WinUISetPeerTextForTesting("ga"));
    REQUIRE(WaitFor("ComboBox reverse completion flyout", [&combo]()
    {
        return combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 2;
    }));
    {
        wxWinUIKeyboardModifiers modifiers{};
        modifiers.shiftDown = true;
        const wxWinUIScopedKeyboardModifiers scopedModifiers(modifiers);
        REQUIRE(wxWinUIDispatchComboKey(::GetFocus(), VK_TAB));
    }
    REQUIRE(WaitFor("ComboBox completion Shift-Tab focus exit",
                    [&previousFocus]()
    {
        return wxWindow::FindFocus() == &previousFocus;
    }));
    REQUIRE(WaitFor("ComboBox completion Shift-Tab closes flyout", [&combo]()
    {
        return combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 0;
    }));

    combo.SetFocus();
    REQUIRE(WaitFor("ComboBox completion refocus", [&combo]()
    {
        return wxWindow::FindFocus() == &combo && ::GetFocus();
    }));
    // Refocus alone is not a text edit. Change the real peer document so the
    // completer is refreshed by the production TextProperty path.
    REQUIRE(combo.WinUISetPeerTextForTesting("g"));
    REQUIRE(WaitFor("ComboBox refocused completion flyout", [&combo]()
    {
        return combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 2;
    }));

    // Keep the same real TextBox and completer across a top-level host move;
    // the next edit must anchor a newly-created flyout in the new XamlRoot.
    REQUIRE(combo.Reparent(&destinationPanel));
    combo.SetFocus();
    REQUIRE(WaitFor("reparented ComboBox completion focus", [&combo]()
    {
        return wxWindow::FindFocus() == &combo && ::GetFocus();
    }));
    REQUIRE(combo.WinUISetPeerTextForTesting("ga"));
    REQUIRE(WaitFor("reparented ComboBox completion flyout", [&combo]()
    {
        return combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 2;
    }));
    REQUIRE(wxWinUIDispatchComboKey(::GetFocus(), VK_DOWN));
    CHECK(combo.WinUIGetAutoCompleteActiveSuggestionForTesting() == 0);
    REQUIRE(wxWinUIDispatchComboKey(::GetFocus(), VK_RETURN));
    CHECK(combo.GetValue() == "gamma");
    CHECK(combo.WinUIGetAutoCompleteSuggestionCountForTesting() == 0);

    struct PathCleanup
    {
        wxString file;
        wxString directory;
        ~PathCleanup()
        {
            if ( !file.empty() && wxFileExists(file) )
                wxRemoveFile(file);
            if ( !directory.empty() && wxDirExists(directory) )
                wxRmdir(directory);
        }
    } cleanup;

    cleanup.file = wxFileName::CreateTempFileName("wxcombo-ac-file");
    REQUIRE(!cleanup.file.empty());
    const wxString filePrefix = cleanup.file.Left(
        cleanup.file.length() - 1);
    REQUIRE(combo.WinUISetPeerTextForTesting(filePrefix));
    REQUIRE(combo.AutoCompleteFileNames());
    REQUIRE(WaitFor("ComboBox filename completion", [&combo]()
    {
        return combo.WinUIGetAutoCompleteSuggestionCountForTesting() != 0;
    }));
    bool foundFile = false;
    for ( unsigned n = 0;
          n < combo.WinUIGetAutoCompleteSuggestionCountForTesting(); ++n )
    {
        if ( combo.WinUIGetAutoCompleteSuggestionForTesting(n).
                 IsSameAs(cleanup.file, false) )
        {
            foundFile = true;
            break;
        }
    }
    CHECK(foundFile);
    REQUIRE(wxRemoveFile(cleanup.file));
    cleanup.file.clear();

    cleanup.directory =
        wxFileName::CreateTempFileName("wxcombo-ac-dir");
    REQUIRE(!cleanup.directory.empty());
    REQUIRE(wxRemoveFile(cleanup.directory));
    REQUIRE(wxMkdir(cleanup.directory));
    const wxString directoryPrefix = cleanup.directory.Left(
        cleanup.directory.length() - 1);
    REQUIRE(combo.WinUISetPeerTextForTesting(directoryPrefix));
    REQUIRE(combo.AutoCompleteDirectories());
    REQUIRE(WaitFor("ComboBox directory completion", [&combo]()
    {
        return combo.WinUIGetAutoCompleteSuggestionCountForTesting() != 0;
    }));
    bool foundDirectory = false;
    const wxString expectedDirectory =
        cleanup.directory + wxFileName::GetPathSeparator();
    for ( unsigned n = 0;
          n < combo.WinUIGetAutoCompleteSuggestionCountForTesting(); ++n )
    {
        const wxString suggestion =
            combo.WinUIGetAutoCompleteSuggestionForTesting(n);
        if ( suggestion.IsSameAs(expectedDirectory, false) )
        {
            foundDirectory = true;
            break;
        }
    }
    CHECK(foundDirectory);
}

TEST_CASE("wxWinUI ComboBox completer callbacks may destroy their owner",
          "[winui-itemmodel][winui-combo][autocomplete][lifetime]")
{
    wxFrame frame(nullptr, wxID_ANY, "combo completion lifetime");
    wxPanel panel(&frame);
    frame.Show();

    wxComboBox *combo = new wxComboBox(&panel, wxID_ANY);
    RequireWinUIComboTemplate(*combo);
    combo->SetFocus();
    REQUIRE(WaitFor("deleting completer ComboBox focus", [&combo]()
    {
        return combo && wxWindow::FindFocus() == combo && ::GetFocus();
    }));
    REQUIRE(combo->WinUISetPeerTextForTesting("al"));
    const bool startResult = combo->AutoComplete(
        new wxWinUIDeleteComboOnStartCompleter(&combo));
    CHECK(startResult);
    const std::string lifetimeDiagnostics = combo
        ? wxWinUIComboDiagnostics(*combo)
        : "combo deleted by completer";
    INFO(lifetimeDiagnostics);
    CHECK(combo == nullptr);

    combo = new wxComboBox(&panel, wxID_ANY);
    RequireWinUIComboTemplate(*combo);
    bool armed = false;
    REQUIRE(combo->AutoComplete(
        new wxWinUIDeleteComboOnDestroyCompleter(&combo, &armed)));
    armed = true;
    wxArrayString replacement;
    replacement.Add("alpha");
    const bool replacementResult = combo->AutoComplete(replacement);
    CHECK(replacementResult);
    CHECK(combo == nullptr);

    combo = new wxComboBox(&panel, wxID_ANY);
    RequireWinUIComboTemplate(*combo);
    combo->SetFocus();
    REQUIRE(WaitFor("accepting completer ComboBox focus", [&combo]()
    {
        return combo && wxWindow::FindFocus() == combo && ::GetFocus();
    }));
    REQUIRE(combo->WinUISetPeerTextForTesting("al"));
    wxArrayString acceptedValues;
    acceptedValues.Add("alpha");
    REQUIRE(combo->AutoComplete(acceptedValues));
    REQUIRE(WaitFor("accepting completer flyout", [&combo]()
    {
        return combo &&
               combo->WinUIGetAutoCompleteSuggestionCountForTesting() == 1;
    }));
    REQUIRE(wxWinUIDispatchComboKey(::GetFocus(), VK_DOWN));
    combo->Bind(wxEVT_TEXT, [&combo](wxCommandEvent&)
    {
        wxComboBox * const doomed = combo;
        combo = nullptr;
        delete doomed;
    });
    const WXHWND editIsland = ::GetFocus();
    REQUIRE(editIsland != nullptr);
    CHECK(wxWinUIDispatchComboKey(editIsland, VK_RETURN, false));
    CHECK(combo == nullptr);
}

TEST_CASE("wxWinUI simple ComboBox is a persistent edit and list surface",
          "[winui-itemmodel][winui-combo][simple][parity]")
{
    wxFrame source(nullptr, wxID_ANY, "simple combo source");
    wxPanel sourcePanel(&source);
    const wxString items[] = { "zero", "one", "two", "three" };
    wxComboBox combo(&sourcePanel, wxID_ANY, wxString(),
                     wxDefaultPosition, wxSize(240, 240),
                     WXSIZEOF(items), items,
                     wxCB_SIMPLE | wxTE_PROCESS_ENTER);
    source.SetClientSize(wxSize(300, 300));
    sourcePanel.SetSize(source.GetClientSize());
    source.Show();

    wxComboBox::WinUISimplePeerSnapshot surface;
    const double persistentRequestedHeight =
        static_cast<double>(combo.ToDIP(combo.GetSize()).y);
    const bool persistentLayout =
        WaitFor("simple ComboBox persistent list layout", [&]()
    {
        return combo.WinUIGetSimplePeerSnapshotForTesting(&surface) &&
               surface.state == wxComboBox::WinUISimple_Complete &&
               std::abs(surface.rootHeight -
                        persistentRequestedHeight) <= 0.5 &&
               std::abs(surface.listY -
                        (surface.editY + surface.editHeight)) <= 0.5 &&
               std::abs(surface.listY + surface.listHeight -
                        surface.rootHeight) <= 0.5 &&
               wxWinUISimpleExtentIsExact(surface);
    });
    INFO("simple state=" << surface.state <<
         " edit=" << surface.editIdentity <<
         " list=" << surface.listIdentity <<
         " xamlRoot=" << surface.xamlRootIdentity <<
         " rows=" << surface.editRow << "," << surface.listRow <<
         " row1Pixel=" << surface.listRowIsPixel << ":" <<
             surface.listRowHeight <<
         " source=" << surface.sourceWidthPixels << "x" <<
             surface.sourceHeightPixels <<
         " requested=" << surface.requestedWidth << "x" <<
             surface.requestedHeight <<
         " layoutEpoch=" << surface.layoutEpoch << "/" <<
             surface.layoutAppliedEpoch <<
         " queued/inProgress=" << surface.layoutQueued << "/" <<
             surface.layoutInProgress <<
         " editRect=" << surface.editX << "," << surface.editY << "," <<
             surface.editWidth << "x" << surface.editHeight <<
         " listRect=" << surface.listX << "," << surface.listY << "," <<
             surface.listWidth << "x" << surface.listHeight <<
         " listDesired/local/min/max=" << surface.listDesiredHeight << "/" <<
             surface.listLocalHeight << "/" << surface.listMinHeight << "/" <<
             surface.listMaxHeight <<
         " viewport=" << surface.viewportHeight <<
         " peerSelection=" << surface.peerSelection <<
         " peerItemsValid=" << surface.peerItemsValid);
    REQUIRE(persistentLayout);
    REQUIRE(surface.editIdentity != 0);
    REQUIRE(surface.listIdentity != 0);
    CHECK(surface.editRow == 0);
    CHECK(surface.listRow == 1);
    CHECK(surface.listY >= surface.editY + surface.editHeight - 0.5);
    CHECK(std::abs(surface.editX - surface.listX) <= 1.0);
    CHECK(std::abs(surface.editWidth - surface.listWidth) <= 1.0);
    CHECK(surface.viewportHeight > 0.0);
    CHECK(surface.xamlRootIdentity != 0);
    CHECK(surface.rasterizationScale > 0.0);
    CHECK(surface.actualTheme >= 0);
    CHECK(surface.peerSelection == wxNOT_FOUND);
    CHECK(surface.peerItemsValid);
    CHECK(combo.GetSize().y == 240);

    wxComboBox ordinary(&sourcePanel, wxID_ANY, wxString(),
                        wxDefaultPosition, wxDefaultSize,
                        WXSIZEOF(items), items);
    CHECK(combo.GetBestSize().y > ordinary.GetBestSize().y);

    int clientSentinel = 0;
    combo.SetClientData(2, &clientSentinel);
    std::vector<wxEventType> order;
    void *selectionData = nullptr;
    void *textData = nullptr;
    int expectedSelection = 2;
    combo.Bind(wxEVT_COMBOBOX, [&](wxCommandEvent& event)
    {
        order.push_back(event.GetEventType());
        selectionData = event.GetClientData();
        CHECK(event.GetInt() == expectedSelection);
        CHECK(combo.GetCurrentSelection() == expectedSelection);
    });
    combo.Bind(wxEVT_TEXT, [&](wxCommandEvent& event)
    {
        order.push_back(event.GetEventType());
        textData = event.GetClientData();
    });

    REQUIRE(combo.WinUISelectPeerItemForTesting(2));
    REQUIRE(order.size() == 2);
    CHECK(order[0] == wxEVT_COMBOBOX);
    CHECK(order[1] == wxEVT_TEXT);
    CHECK(selectionData == &clientSentinel);
    CHECK(textData == &clientSentinel);
    CHECK(combo.GetSelection() == 2);
    CHECK(combo.GetCurrentSelection() == 2);
    CHECK(combo.GetValue() == "two");
    wxComboBox::WinUISimplePeerSnapshot selectedSurface;
    REQUIRE(combo.WinUIGetSimplePeerSnapshotForTesting(&selectedSurface));
    INFO("selected peerSelection=" << selectedSurface.peerSelection <<
         " peerItemsValid=" << selectedSurface.peerItemsValid);
    CHECK(selectedSurface.peerSelection == 2);
    CHECK(selectedSurface.peerItemsValid);

    const std::uint64_t selectedId =
        combo.WinUIGetItemIdForTesting(2);
    const std::uintptr_t selectedPeer =
        combo.WinUIGetItemPeerIdentityForTesting(2);
    REQUIRE(selectedPeer != 0);
    order.clear();
    REQUIRE(combo.Insert("before", 0) == 0);
    CHECK(combo.GetSelection() == 3);
    wxComboBox::WinUISimplePeerSnapshot insertedSurface;
    REQUIRE(combo.WinUIGetSimplePeerSnapshotForTesting(&insertedSurface));
    INFO("inserted peerSelection=" << insertedSurface.peerSelection <<
         " peerItemsValid=" << insertedSurface.peerItemsValid);
    CHECK(insertedSurface.peerSelection == 3);
    CHECK(insertedSurface.peerItemsValid);
    CHECK(combo.GetCurrentSelection() == 3);
    CHECK(combo.WinUIGetItemIdForTesting(3) == selectedId);
    CHECK(combo.WinUIGetItemPeerIdentityForTesting(3) == selectedPeer);
    CHECK(order.empty());

    combo.SetString(3, "renamed");
    CHECK(combo.GetValue() == "renamed");
    CHECK(combo.GetSelection() == 3);
    CHECK(order.empty());

    REQUIRE(combo.WinUISetPeerTextForTesting("typed value"));
    REQUIRE(order.size() == 1);
    CHECK(order[0] == wxEVT_TEXT);
    CHECK(combo.GetValue() == "typed value");
    CHECK(combo.GetSelection() == 3);

    order.clear();
    combo.SetSelection(wxNOT_FOUND);
    expectedSelection = 0;
    combo.SetFocus();
    REQUIRE(WaitFor("simple ComboBox edit focus", [&combo]()
    {
        return wxWindow::FindFocus() == &combo && ::GetFocus();
    }));
    MSG down = {};
    down.hwnd = ::GetFocus();
    down.message = WM_KEYDOWN;
    down.wParam = VK_DOWN;
    down.lParam = 1 |
        (static_cast<LPARAM>(::MapVirtualKeyW(
             VK_DOWN, MAPVK_VK_TO_VSC)) << 16);
    REQUIRE(wxWinUI3DispatchIslandKeyboard(&down));
    MSG up = down;
    up.message = WM_KEYUP;
    up.lParam |= static_cast<LPARAM>(0xC0000000u);
    REQUIRE(wxWinUI3DispatchIslandKeyboard(&up));
    REQUIRE(WaitFor("simple ComboBox keyboard selection", [&combo]()
    {
        return combo.GetSelection() == 0;
    }));
    CHECK(combo.GetCurrentSelection() == 0);
    REQUIRE(order.size() == 2);
    CHECK(order[0] == wxEVT_COMBOBOX);
    CHECK(order[1] == wxEVT_TEXT);

    wxComboBox::WinUISimplePeerSnapshot expandedSurface;
    const wxSize expandedWxSize = combo.GetSize();
    const double expandedRequestedHeight =
        static_cast<double>(combo.ToDIP(expandedWxSize).y);
    REQUIRE(WaitFor("simple ComboBox initial viewport decomposition", [&]()
    {
        return combo.WinUIGetSimplePeerSnapshotForTesting(
                   &expandedSurface) &&
               expandedSurface.viewportHeight > 0.0 &&
               std::abs(expandedSurface.rootHeight -
                        expandedRequestedHeight) <= 0.5 &&
               std::abs(expandedSurface.listY -
                        (expandedSurface.editY +
                         expandedSurface.editHeight)) <= 0.5 &&
               std::abs(expandedSurface.listY +
                        expandedSurface.listHeight -
                        expandedSurface.rootHeight) <= 0.5 &&
               wxWinUISimpleExtentIsExact(expandedSurface);
    }));
    const int expandedPage = combo.WinUIGetSimplePageSizeForTesting();
    REQUIRE(expandedPage >= 1);
    CHECK(expandedPage <= static_cast<int>(combo.GetCount()));
    REQUIRE(expandedSurface.viewportHeight > 0.0);
    REQUIRE(expandedSurface.rootHeight > 0.0);
    REQUIRE(std::abs(expandedSurface.listY +
                     expandedSurface.listHeight -
                     expandedSurface.rootHeight) <= 0.5);
    combo.SetSize(wxSize(240, 90));
    const double compactRequestedHeight =
        static_cast<double>(combo.ToDIP(combo.GetSize()).y);
    wxComboBox::WinUISimplePeerSnapshot compactSurface;
    REQUIRE(WaitFor("simple ComboBox compact viewport page", [&combo,
                                                               expandedPage,
                                                               &compactSurface,
                                                               &expandedSurface,
                                                               compactRequestedHeight]()
    {
        return combo.WinUIGetSimplePeerSnapshotForTesting(&compactSurface) &&
               compactSurface.viewportHeight > 0.0 &&
               std::abs(compactSurface.rootHeight -
                        compactRequestedHeight) <= 0.5 &&
               std::abs(compactSurface.listY -
                        (compactSurface.editY +
                         compactSurface.editHeight)) <= 0.5 &&
               std::abs(compactSurface.listY +
                        compactSurface.listHeight -
                        compactSurface.rootHeight) <= 0.5 &&
               wxWinUISimpleExtentIsExact(compactSurface) &&
               compactSurface.viewportHeight <
                   expandedSurface.viewportHeight &&
               combo.WinUIGetSimplePageSizeForTesting() < expandedPage;
    }));
    CHECK(compactSurface.listHeight < expandedSurface.listHeight);
    const int compactPage = combo.WinUIGetSimplePageSizeForTesting();
    combo.SetSize(expandedWxSize);
    wxComboBox::WinUISimplePeerSnapshot restoredSurface;
    int restoredPage = -1;
    const bool restoredViewport =
        WaitFor("simple ComboBox restored viewport", [&combo,
                                                        &restoredSurface,
                                                        &restoredPage,
                                                        &expandedSurface,
                                                        expandedPage]()
    {
        if ( !combo.WinUIGetSimplePeerSnapshotForTesting(&restoredSurface) )
            return false;
        restoredPage = combo.WinUIGetSimplePageSizeForTesting();
        return std::abs(restoredSurface.viewportHeight -
                        expandedSurface.viewportHeight) <= 0.5 &&
               std::abs(restoredSurface.listY -
                        (restoredSurface.editY +
                         restoredSurface.editHeight)) <= 0.5 &&
               std::abs(restoredSurface.listY +
                        restoredSurface.listHeight -
                        restoredSurface.rootHeight) <= 0.5 &&
               std::abs(restoredSurface.listHeight -
                        expandedSurface.listHeight) <= 0.5 &&
               std::abs(restoredSurface.rootHeight -
                        expandedSurface.rootHeight) <= 0.5 &&
               wxWinUISimpleExtentIsExact(restoredSurface) &&
               restoredPage == expandedPage;
    });
    INFO("simple viewport/page expanded=" <<
         expandedSurface.viewportHeight << "/" << expandedPage <<
         " compact=" << compactSurface.viewportHeight << "/" <<
         compactPage <<
         " restored=" << restoredSurface.viewportHeight << "/" <<
         restoredPage);
    INFO(wxWinUIComboDiagnostics(combo));
    REQUIRE(restoredViewport);
    REQUIRE(restoredPage == expandedPage);

    order.clear();
    const int pageSelection = wxMin(
        restoredPage, static_cast<int>(combo.GetCount()) - 1);
    expectedSelection = pageSelection;
    REQUIRE(wxWinUIDispatchComboKey(::GetFocus(), VK_NEXT));
    REQUIRE(WaitFor("simple ComboBox PageDown selection", [&combo,
                                                            pageSelection]()
    {
        return combo.GetSelection() == pageSelection;
    }));
    REQUIRE(order.size() == 2);
    CHECK(order[0] == wxEVT_COMBOBOX);
    CHECK(order[1] == wxEVT_TEXT);

    // Shift/Ctrl/Alt/AltGr navigation remains owned by the edit peer and must
    // never change the persistent ListBox selection.
    combo.SetSelection(0);
    order.clear();
    const wxWinUIKeyboardModifiers modifiedNavigation[] =
    {
        { true, false, false, false },
        { false, true, false, false },
        { false, false, true, false },
        { false, true, false, true }
    };
    for ( const wxWinUIKeyboardModifiers& modifiers : modifiedNavigation )
    {
        wxWinUIScopedKeyboardModifiers scopedModifiers(modifiers);
        REQUIRE(wxWinUIDispatchComboKey(::GetFocus(), VK_DOWN));
        CHECK(combo.GetSelection() == 0);
        CHECK(combo.GetCurrentSelection() == 0);
        CHECK(order.empty());
    }

    expectedSelection = static_cast<int>(combo.GetCount()) - 1;
    REQUIRE(combo.WinUINavigateSimpleListForTesting(99));
    CHECK(combo.GetSelection() ==
          static_cast<int>(combo.GetCount()) - 1);
    REQUIRE(order.size() == 2);
    order.clear();
    REQUIRE(combo.WinUINavigateSimpleListForTesting(1));
    CHECK(order.empty());
}

TEST_CASE("wxWinUI editable ComboBox pins and retires its effective theme",
           "[winui-itemmodel][winui-combo][template][theme][parity]")
{
    struct ThemeRestorer
    {
        wxWinUIAppTheme saved = wxWinUIGetAppTheme();
        ~ThemeRestorer() { wxWinUISetAppTheme(saved); }
    } restoreTheme;

    // System maps to RequestedTheme(Default). The editable subtree must still
    // pin its resolved Light/Dark theme so a later root transition cannot
    // dismantle the old editor before its generation is retired.
    wxWinUISetAppTheme(wxWinUIAppTheme::System);
    wxFrame frame(nullptr, wxID_ANY, "editable combo theme generation");
    const wxString items[] = { "abcdef", "other" };
    wxComboBox combo(&frame, wxID_ANY, wxString(), wxDefaultPosition,
                     wxSize(220, 32), WXSIZEOF(items), items);
    frame.Show();
    RequireWinUIComboTemplate(combo);
    combo.SetSelection(0);
    combo.SetSelection(1, 4);

    std::uintptr_t oldEditIdentity = 0;
    std::uint64_t oldEditGeneration = 0;
    int oldRootTheme = wxComboBox::WinUITheme_Unknown;
    int oldRootRequestedTheme = wxComboBox::WinUITheme_Unknown;
    int oldEditTheme = wxComboBox::WinUITheme_Unknown;
    REQUIRE(WaitFor("editable ComboBox initial pinned generation", [&]()
    {
        return combo.WinUIGetTextEntryPeerStateForTesting(
            nullptr, nullptr, nullptr,
            &oldEditIdentity, &oldEditGeneration,
            &oldRootTheme, &oldRootRequestedTheme, &oldEditTheme,
            false) &&
               oldEditIdentity != 0 &&
               oldEditGeneration != 0 &&
               (oldRootTheme == wxComboBox::WinUITheme_Light ||
                oldRootTheme == wxComboBox::WinUITheme_Dark) &&
               oldRootRequestedTheme == oldRootTheme &&
               oldEditTheme == oldRootTheme;
    }));
    REQUIRE(oldEditIdentity != 0);
    REQUIRE(oldEditGeneration != 0);
    REQUIRE((oldRootTheme == wxComboBox::WinUITheme_Light ||
             oldRootTheme == wxComboBox::WinUITheme_Dark));
    REQUIRE(oldRootRequestedTheme == oldRootTheme);
    REQUIRE(oldEditTheme == oldRootTheme);

    const wxWinUIAppTheme newPolicy =
        oldRootTheme == wxComboBox::WinUITheme_Light
            ? wxWinUIAppTheme::Dark
            : wxWinUIAppTheme::Light;
    const int newEffectiveTheme =
        oldRootTheme == wxComboBox::WinUITheme_Light
            ? wxComboBox::WinUITheme_Dark
            : wxComboBox::WinUITheme_Light;

    EventCounter textEvents(&combo, wxEVT_TEXT);
    wxComboBox::WinUIDiagnosticSnapshot beforeThemeTransition;
    REQUIRE(combo.WinUIGetDiagnosticSnapshotForTesting(
        &beforeThemeTransition));
    wxWinUISetAppTheme(newPolicy);
    wxComboBox::WinUIDiagnosticSnapshot acceptedThemeTransition;
    REQUIRE(combo.WinUIGetDiagnosticSnapshotForTesting(
        &acceptedThemeTransition));
    CHECK(acceptedThemeTransition.comboLayoutRealizations ==
          beforeThemeTransition.comboLayoutRealizations);
    CHECK(acceptedThemeTransition.comboLayoutSynchronousRealizations ==
          beforeThemeTransition.comboLayoutSynchronousRealizations);

    std::uintptr_t newEditIdentity = 0;
    std::uint64_t newEditGeneration = 0;
    int newRootTheme = wxComboBox::WinUITheme_Unknown;
    int newRootRequestedTheme = wxComboBox::WinUITheme_Unknown;
    int newEditTheme = wxComboBox::WinUITheme_Unknown;
    wxComboBox::WinUITemplatePeerSnapshot settledThemePeer;
    wxComboBox::WinUIDiagnosticSnapshot settledThemeDiagnostic;
    const bool themeGenerationCommitted =
        WaitFor("editable ComboBox theme editor generation", [&]()
    {
        return combo.WinUIGetTextEntryPeerStateForTesting(
                   nullptr, nullptr, nullptr,
                   &newEditIdentity, &newEditGeneration,
                   &newRootTheme, &newRootRequestedTheme,
                   &newEditTheme, false) &&
                newEditIdentity != 0 &&
                newEditGeneration > oldEditGeneration &&
                newRootTheme == newEffectiveTheme &&
                newRootRequestedTheme == newEffectiveTheme &&
                newEditTheme == newEffectiveTheme &&
                combo.WinUIGetTemplatePeerSnapshotForTesting(
                    &settledThemePeer) &&
                combo.WinUIGetDiagnosticSnapshotForTesting(
                    &settledThemeDiagnostic) &&
                !(settledThemePeer.phase &
                  (wxComboBox::WinUITemplatePhase_Resolving |
                   wxComboBox::WinUITemplatePhase_PendingResolve |
                   wxComboBox::WinUITemplatePhase_Transition |
                   wxComboBox::WinUITemplatePhase_PeerMutation |
                   wxComboBox::WinUITemplatePhase_PendingRange |
                   wxComboBox::WinUITemplatePhase_QueuedResolve)) &&
                !settledThemeDiagnostic.templateReplayPending &&
                settledThemePeer.peerSelectionStart == 1 &&
                settledThemePeer.peerSelectionLength == 3 &&
                settledThemeDiagnostic.comboLayoutRealizations ==
                    beforeThemeTransition.comboLayoutRealizations &&
                settledThemeDiagnostic.
                        comboLayoutSynchronousRealizations ==
                    beforeThemeTransition.
                        comboLayoutSynchronousRealizations;
    });
    INFO("theme edit=" << oldEditIdentity << "->" << newEditIdentity <<
         " generation=" << oldEditGeneration << "->" <<
             newEditGeneration <<
         " rootActual=" << oldRootTheme << "->" << newRootTheme <<
         " rootRequested=" << oldRootRequestedTheme << "->" <<
             newRootRequestedTheme <<
         " editActual=" << oldEditTheme << "->" << newEditTheme <<
         " state=" << combo.WinUIGetTemplateStateForTesting());
    INFO(wxWinUIComboDiagnostics(combo));
    REQUIRE(themeGenerationCommitted);
    REQUIRE(newEditIdentity != 0);
    CHECK(newEditGeneration > oldEditGeneration);
    CHECK(settledThemePeer.peerSelectionStart == 1);
    CHECK(settledThemePeer.peerSelectionLength == 3);
    CHECK_FALSE(settledThemeDiagnostic.pendingRange);
    CHECK(settledThemeDiagnostic.pendingRangeRevision == 0);
    const bool divergentThemeRangeDrained =
        settledThemeDiagnostic.rangeFinalizations >
            beforeThemeTransition.rangeFinalizations;
    if ( divergentThemeRangeDrained )
    {
        CHECK(settledThemeDiagnostic.rangeFinalizations ==
              beforeThemeTransition.rangeFinalizations + 1);
        CHECK(settledThemeDiagnostic.rangeProjectionMismatches >
              beforeThemeTransition.rangeProjectionMismatches);
    }
    else
    {
        CHECK(settledThemeDiagnostic.rangeFinalizations ==
              beforeThemeTransition.rangeFinalizations);
        CHECK(settledThemeDiagnostic.rangeProjectionMismatches ==
              beforeThemeTransition.rangeProjectionMismatches);
    }

    CHECK(combo.GetValue() == "abcdef");
    CHECK(combo.GetSelection() == 0);
    long from = -1;
    long to = -1;
    combo.GetSelection(&from, &to);
    CHECK(from == 1);
    CHECK(to == 4);
    CHECK(textEvents.GetCount() == 0);
}

TEST_CASE("wxWinUI simple ComboBox survives host, DPI and theme transitions",
          "[winui-itemmodel][winui-combo][simple][lifetime][dpi][theme]")
{
    struct ThemeRestorer
    {
        wxWinUIAppTheme saved = wxWinUIGetAppTheme();
        ~ThemeRestorer() { wxWinUISetAppTheme(saved); }
    } restoreTheme;

    wxFrame source(nullptr, wxID_ANY, "simple combo source host");
    wxFrame destination(nullptr, wxID_ANY,
                        "simple combo destination host");
    wxPanel sourcePanel(&source);
    wxPanel destinationPanel(&destination);
    const wxString items[] = { "zero", "one", "two" };
    wxComboBox combo(&sourcePanel, wxID_ANY, "initial",
                     wxDefaultPosition, wxSize(220, 160),
                     WXSIZEOF(items), items, wxCB_SIMPLE);
    source.SetClientSize(wxSize(280, 220));
    destination.SetClientSize(wxSize(280, 220));
    sourcePanel.SetSize(source.GetClientSize());
    destinationPanel.SetSize(destination.GetClientSize());
    source.Show();
    destination.Show();

    wxComboBox::WinUISimplePeerSnapshot sourceSurface;
    const bool sourceLayout =
        WaitFor("simple ComboBox source-host layout", [&]()
    {
        return combo.WinUIGetSimplePeerSnapshotForTesting(&sourceSurface) &&
               sourceSurface.state == wxComboBox::WinUISimple_Complete;
    });
    INFO("source simple state=" << sourceSurface.state <<
         " edit=" << sourceSurface.editIdentity <<
         " list=" << sourceSurface.listIdentity <<
         " xamlRoot=" << sourceSurface.xamlRootIdentity <<
         " rows=" << sourceSurface.editRow << "," << sourceSurface.listRow <<
         " editRect=" << sourceSurface.editX << "," <<
             sourceSurface.editY << "," << sourceSurface.editWidth << "x" <<
             sourceSurface.editHeight <<
         " listRect=" << sourceSurface.listX << "," <<
             sourceSurface.listY << "," << sourceSurface.listWidth << "x" <<
             sourceSurface.listHeight <<
         " viewport=" << sourceSurface.viewportHeight);
    REQUIRE(sourceLayout);
    REQUIRE(sourceSurface.xamlRootIdentity != 0);
    const std::uintptr_t itemIdentity =
        combo.WinUIGetItemPeerIdentityForTesting(1);
    REQUIRE(itemIdentity != 0);

    REQUIRE(combo.Reparent(&destinationPanel));
    wxComboBox::WinUISimplePeerSnapshot destinationSurface;
    const bool destinationLayout =
        WaitFor("simple ComboBox destination-host layout", [&]()
    {
        return combo.WinUIGetSimplePeerSnapshotForTesting(
                   &destinationSurface) &&
               destinationSurface.state ==
                   wxComboBox::WinUISimple_Complete &&
               destinationSurface.xamlRootIdentity !=
                   sourceSurface.xamlRootIdentity;
    });
    INFO("destination simple state=" << destinationSurface.state <<
         " edit=" << destinationSurface.editIdentity <<
         " list=" << destinationSurface.listIdentity <<
         " xamlRoot=" << destinationSurface.xamlRootIdentity <<
         " rows=" << destinationSurface.editRow << "," <<
             destinationSurface.listRow <<
         " editRect=" << destinationSurface.editX << "," <<
             destinationSurface.editY << "," <<
             destinationSurface.editWidth << "x" <<
             destinationSurface.editHeight <<
         " listRect=" << destinationSurface.listX << "," <<
             destinationSurface.listY << "," <<
             destinationSurface.listWidth << "x" <<
             destinationSurface.listHeight <<
         " viewport=" << destinationSurface.viewportHeight);
    REQUIRE(destinationLayout);
    CHECK(destinationSurface.editIdentity == sourceSurface.editIdentity);
    CHECK(destinationSurface.listIdentity == sourceSurface.listIdentity);
    CHECK(combo.WinUIGetItemPeerIdentityForTesting(1) == itemIdentity);

    // Observe the real host mapper: the XamlRoot scale and wx DPI must describe
    // the same destination top-level, without synthesizing wxDPIChangedEvent.
    const wxSize dpi = combo.GetDPI();
    CHECK(std::abs(destinationSurface.rasterizationScale * 96.0 -
                   static_cast<double>(dpi.x)) <= 1.5);

    wxWinUISetAppTheme(wxWinUIAppTheme::Light);
    wxComboBox::WinUISimplePeerSnapshot lightSurface;
    REQUIRE(WaitFor("simple ComboBox observed light theme", [&]()
    {
        return combo.WinUIGetSimplePeerSnapshotForTesting(&lightSurface) &&
               lightSurface.state == wxComboBox::WinUISimple_Complete &&
               lightSurface.actualTheme ==
                   static_cast<int>(wxWinUIAppTheme::Light);
    }));
    wxWinUISetAppTheme(wxWinUIAppTheme::Dark);
    wxComboBox::WinUISimplePeerSnapshot darkSurface;
    REQUIRE(WaitFor("simple ComboBox observed dark theme", [&]()
    {
        return combo.WinUIGetSimplePeerSnapshotForTesting(&darkSurface) &&
               darkSurface.state == wxComboBox::WinUISimple_Complete &&
               darkSurface.actualTheme ==
                   static_cast<int>(wxWinUIAppTheme::Dark);
    }));
    CHECK(darkSurface.editIdentity == sourceSurface.editIdentity);
    CHECK(darkSurface.listIdentity == sourceSurface.listIdentity);
    CHECK(darkSurface.xamlRootIdentity ==
          destinationSurface.xamlRootIdentity);

    const std::size_t baseline =
        wxWinUITextCallbackState::GetLiveCountForTesting();
    wxComboBox *doomed =
        new wxComboBox(&destinationPanel, wxID_ANY, wxString(),
                       wxDefaultPosition, wxSize(180, 140),
                       WXSIZEOF(items), items, wxCB_SIMPLE);
    doomed->Bind(wxEVT_COMBOBOX, [&](wxCommandEvent&)
    {
        wxComboBox * const deleting = doomed;
        doomed = nullptr;
        delete deleting;
    });
    wxComboBox * const invoking = doomed;
    REQUIRE(invoking->WinUISelectPeerItemForTesting(1));
    CHECK(doomed == nullptr);
    wxYield();
    CHECK(wxWinUITextCallbackState::GetLiveCountForTesting() == baseline);
}

#endif // wxUSE_COMBOBOX

#endif // wxUSE_CHOICE

#if wxUSE_BITMAPCOMBOBOX

TEST_CASE("wxWinUI BitmapCombo keeps bitmap identity through sorting",
          "[winui-itemmodel][bitmapcombo]")
{
    wxFrame frame(nullptr, wxID_ANY, "bitmap combo delta owner");
    wxBitmapComboBox combo(&frame, wxID_ANY, wxString(),
                           wxDefaultPosition, wxDefaultSize,
                           0, nullptr, wxCB_SORT);

    const wxColour red(220, 20, 20);
    const wxColour green(20, 180, 20);
    const wxColour blue(20, 20, 220);
    REQUIRE(combo.Append("zulu", wxWinUIMakeSolidBitmap(red)) !=
            wxNOT_FOUND);
    REQUIRE(combo.Append("alpha", wxWinUIMakeSolidBitmap(green)) !=
            wxNOT_FOUND);
    REQUIRE(combo.Append("mike", wxWinUIMakeSolidBitmap(blue)) !=
            wxNOT_FOUND);

    REQUIRE(combo.GetCount() == 3);
    CHECK(combo.GetString(0) == "alpha");
    CHECK(combo.GetString(1) == "mike");
    CHECK(combo.GetString(2) == "zulu");
    CHECK(wxWinUIGetBitmapColour(combo.GetItemBitmap(0)) == green);
    CHECK(wxWinUIGetBitmapColour(combo.GetItemBitmap(1)) == blue);
    CHECK(wxWinUIGetBitmapColour(combo.GetItemBitmap(2)) == red);

    const std::uint64_t zuluId =
        combo.WinUIGetItemIdForTesting(2);
    const std::uintptr_t zuluPeer =
        combo.WinUIGetItemPeerIdentityForTesting(2);
    REQUIRE(zuluPeer != 0);

    combo.SetSelection(2);
    combo.SetString(2, "beta");
    REQUIRE(combo.GetSelection() == 1);
    CHECK(combo.WinUIGetItemIdForTesting(1) == zuluId);
    CHECK(combo.WinUIGetItemPeerIdentityForTesting(1) == zuluPeer);
    CHECK(wxWinUIGetBitmapColour(combo.GetItemBitmap(1)) == red);
    CHECK(wxWinUIGetBitmapColour(combo.GetItemBitmap(2)) == blue);

    combo.Delete(0);
    REQUIRE(combo.GetCount() == 2);
    CHECK(combo.GetString(0) == "beta");
    CHECK(wxWinUIGetBitmapColour(combo.GetItemBitmap(0)) == red);
    CHECK(combo.WinUIGetItemPeerIdentityForTesting(0) == zuluPeer);

    wxBitmapComboBox sizing(&frame, wxID_ANY);
    const wxSize beforeLargeBitmap = sizing.GetBestSize();
    sizing.Append("large",
                  wxWinUIMakeSolidBitmap(
                      wxColour(80, 80, 80), wxSize(48, 48)));
    CHECK(sizing.GetBestSize().y > beforeLargeBitmap.y);
}

#endif // wxUSE_BITMAPCOMBOBOX

#endif // __WXWINUI__ && wxUSE_WINUI3
