/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuipropgrideditors.cpp
// Purpose:     Real native PropertyGrid choice editor transactions
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_PROPGRID && wxUSE_COMBOBOX && !defined(wxNO_RTTI)
#include "wx/app.h"
#include "wx/button.h"
#if wxUSE_CHECKBOX
#include "wx/checkbox.h"
#endif
#include "wx/combobox.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/odcombo.h"
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/props.h"
#include "wx/propgrid/editors.h"
#include "wx/sizer.h"
#include "wx/stopwatch.h"
#include "wx/textctrl.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"
#include "combobox-test-access.h"
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <cmath>
#include <memory>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation;

template<typename Predicate> bool WaitEditor(Predicate predicate, long timeout = 1500)
{
    auto* const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp ) return false;
    wxStopWatch timer;
    while ( timer.Time() < timeout )
    {
        if ( predicate() ) return true;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop ) return false;
        if ( !loop->Pending() && !wxTheApp->HasPendingEvents() ) wxTheApp->ProcessIdle();
        if ( predicate() ) return true;
        if ( loop->DispatchTimeout(5) == 0 ) return false;
    }
    return predicate();
}

template<typename T> T FindEditor(const X::DependencyObject& root)
{
    if ( !root ) return nullptr;
    if ( const auto found = root.try_as<T>() ) return found;
    for ( int i = 0; i < X::Media::VisualTreeHelper::GetChildrenCount(root); ++i )
        if ( const auto found = FindEditor<T>(X::Media::VisualTreeHelper::GetChild(root, i)) ) return found;
    return nullptr;
}

template<typename T> T NativeEditor(wxWindow* window)
{
    auto* const host = window ? wxWinUITopLevelHost::FindSlotOwner(window) : nullptr;
    auto* const slot = host ? host->FindSlot(window) : nullptr;
    return slot ? FindEditor<T>(slot->GetContent()) : nullptr;
}

struct EditorFixture
{
    explicit EditorFixture(bool rtl = false)
    {
        wxPoint position(-30000, -30000);
        wchar_t desktop[256]{};
        DWORD size = 0;
        if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
                UOI_NAME, desktop, sizeof(desktop), &size) &&
             wxString(desktop).StartsWith("wxWinUITest-") ) position = wxPoint(80, 80);
        frame = std::make_unique<wxFrame>(nullptr, wxID_ANY, "PropertyGrid native editors",
                                         position, wxSize(600, 320));
        frame->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        auto pending = std::make_unique<wxPropertyGrid>();
        REQUIRE(pending->Create(frame.get(), wxID_ANY));
        grid = pending.release();
        grid->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        grid->SetValidationFailureBehavior(wxPGVFBFlags::Null);
        auto* const sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(grid, 1, wxEXPAND);
        frame->SetSizer(sizer);
    }
    ~EditorFixture()
    {
        frame.reset();
        wxTheApp->ProcessPendingEvents();
        wxTheApp->ProcessIdle();
    }
    void Show()
    {
        frame->Layout();
        frame->ShowWithoutActivating();
        REQUIRE(WaitEditor([&]
        {
            const auto root = NativeEditor<X::FrameworkElement>(grid);
            return root && root.IsLoaded() && root.ActualWidth() > 0;
        }));
    }
    C::ComboBox Select(wxPGProperty* property)
    {
        REQUIRE(grid->SelectProperty(property));
        auto* const editor = wxDynamicCast(grid->GetEditorControl(), wxComboBox);
        REQUIRE(editor);
        REQUIRE_FALSE(wxDynamicCast(editor, wxOwnerDrawnComboBox));
        REQUIRE(WaitEditor([&]
        {
            const auto peer = NativeEditor<C::ComboBox>(editor);
            return peer && peer.IsLoaded() && peer.ActualWidth() > 0 && peer.ActualHeight() >= 30;
        }));
        return NativeEditor<C::ComboBox>(editor);
    }
    std::unique_ptr<wxFrame> frame;
    wxPropertyGrid* grid = nullptr;
};

A::Provider::ISelectionItemProvider ChoiceProvider(wxComboBox* editor,
                                                  const C::ComboBox& combo, int index)
{
    // As a user does it: the list opens from the focused control. The list
    // of a control without the focus may be dismissed at once.
    editor->SetFocus();
    editor->Popup();
    const bool loaded = WaitEditor([&]
    {
        const auto item = combo.ContainerFromIndex(index).try_as<C::ComboBoxItem>();
        return item && item.IsLoaded();
    });
    INFO("open=" << combo.IsDropDownOpen()
         << " focus=" << int(combo.FocusState())
         << " items=" << combo.Items().Size()
         << " popups=" << X::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(
                              combo.XamlRoot()).Size());
    REQUIRE(loaded);
    // Selection is provided by the parent's item-data peer, not the
    // ComboBoxItem's visual/container peer (as for ListView in the UIA tests).
    const auto comboPeer = A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(combo);
    REQUIRE(comboPeer);
    const auto itemsPeer = comboPeer.try_as<A::Peers::ItemsControlAutomationPeer>();
    REQUIRE(itemsPeer);
    const auto peer = itemsPeer.CreateItemAutomationPeer(combo.Items().GetAt(index));
    REQUIRE(peer);
    const auto provider = peer.GetPattern(A::Peers::PatternInterface::SelectionItem)
                             .try_as<A::Provider::ISelectionItemProvider>();
    REQUIRE(provider);
    return provider;
}

void SetEditableValueViaAutomation(wxComboBox& editor,
                                  const C::TextBox& expectedText,
                                  const wxString& value)
{
    // The native TextBox exposes ValuePattern through UIAutomationCore, not
    // the ComboBox's WinRT IValueProvider (which rejects SetValue). Reuse the
    // existing external-client protocol: it resolves the exact Edit child of
    // this ComboBox and calls IUIAutomationValuePattern::SetValue there.
    editor.SetFocus();
    wxWinUIComboBoxTestAccess::WinUITemplatePeerSnapshot focus;
    REQUIRE(WaitEditor([&]
    {
        return wxWinUIComboBoxTestAccess::GetTemplatePeerSnapshot(&editor, &focus) &&
               wxWindow::FindFocus() == &editor && focus.logicalFocus &&
               focus.nativeFocusInHost && focus.editFocused &&
               !(focus.phase & wxWinUIComboBoxTestAccess::WinUITemplatePhase_PendingText) &&
               NativeEditor<C::TextBox>(&editor) == expectedText;
    }));
    REQUIRE(wxWinUIComboBoxTestAccess::SetPeerTextViaAutomation(&editor, value));
    wxWinUIComboBoxTestAccess::WinUIDiagnosticSnapshot result;
    const bool completed = WaitEditor([&]
    {
        return wxWinUIComboBoxTestAccess::GetDiagnosticSnapshot(&editor, &result) &&
               (result.automationStage ==
                    wxWinUIComboBoxTestAccess::WinUIAutomation_Succeeded ||
                result.automationStage < 0);
    }, 2000);
    CAPTURE(result.automationStage, result.automationHResult);
    REQUIRE(completed);
    REQUIRE(result.automationStage == wxWinUIComboBoxTestAccess::WinUIAutomation_Succeeded);
    REQUIRE(result.automationHResult == 0);
    REQUIRE(WaitEditor([&]
    {
        return editor.GetValue() == value && wxString(expectedText.Text().c_str()) == value;
    }));
    CHECK(NativeEditor<C::TextBox>(&editor) == expectedText);
}
}

TEST_CASE("WinUIPropertyGridEditors::NativeChoicePreservesDynamicChoicesAndVeto",
          "[winui-propgrid][native-propgrid][native-pg-editors]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    bool veto = true;
    unsigned changing = 0, changed = 0;
    EditorFixture fixture(rtl);
    auto* const grid = fixture.grid;
    wxPGChoices choices;
    choices.Add("First", 10); choices.Add("Second", 20); choices.Add("Third", 30);
    auto* const property = grid->Append(new wxEnumProperty("Enum", "enum", choices, 10));
    fixture.Show();
    const auto native = fixture.Select(property);
    auto* const editor = wxDynamicCast(grid->GetEditorControl(), wxComboBox);
    REQUIRE_FALSE(native.IsEditable());
    grid->Bind(wxEVT_PG_CHANGING, [&](wxPropertyGridEvent& event)
    { ++changing; CHECK(event.GetProperty() == property); if ( veto ) event.Veto(); });
    grid->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent& event)
    { ++changed; CHECK(event.GetProperty() == property); });
    REQUIRE(property->InsertChoice("Between", 1, 15) == 1);
    REQUIRE(WaitEditor([&] { return native.Items().Size() == 4; }));
    CHECK(grid->GetEditorControl() == editor);
    CHECK(NativeEditor<C::ComboBox>(editor) == native);
    property->DeleteChoice(1);
    REQUIRE(WaitEditor([&] { return native.Items().Size() == 3; }));
    property->GetEditorClass()->SetItems(editor, property->GetChoices().GetLabels());
    property->GetEditorClass()->UpdateControl(property, editor);
    REQUIRE(WaitEditor([&] { return native.Items().Size() == 3 && native.SelectedIndex() == 0; }));
    CHECK(NativeEditor<C::ComboBox>(editor) == native);
    CHECK(changing == 0); CHECK(changed == 0);
    ChoiceProvider(editor, native, 1).Select();
    REQUIRE(WaitEditor([&] { return changing == 1; }));
    CHECK(grid->GetPropertyValueAsLong(property) == 10);
    CHECK(changed == 0);
    // The existing PropertyGrid validation path restores non-TextCtrl
    // editors after a veto. Observe the real native selection too.
    REQUIRE(WaitEditor([&] { return native.SelectedIndex() == 0; }));
    veto = false;
    ChoiceProvider(editor, native, 1).Select();
    REQUIRE(WaitEditor([&] { return changed == 1; }));
    CHECK(grid->GetPropertyValueAsLong(property) == 20);
    CHECK(changing == 2);
    grid->SetPropertyValue(property, 30L);
    REQUIRE(WaitEditor([&] { return native.SelectedIndex() == 2; }));
    CHECK(changed == 1);
    CHECK(NativeEditor<C::ComboBox>(editor) == native);
    editor->Popup();
    REQUIRE(WaitEditor([&] { return native.IsDropDownOpen(); }));
    grid->DeleteProperty(property);
    REQUIRE(WaitEditor([&] { return !native.IsLoaded(); }));
    CHECK(changed == 1);
    CHECK_FALSE(grid->GetPropertyByName("enum"));
}

TEST_CASE("WinUIPropertyGridEditors::EditableNativeTextUsesPropertyCommit",
          "[winui-propgrid][native-propgrid][native-pg-editors]")
{
    unsigned changing = 0, changed = 0;
    bool veto = true;
    EditorFixture fixture;
    // Keep a veto observable as a rejected commit without showing a dialog.
    fixture.grid->SetValidationFailureBehavior(wxPGVFBFlags::StayInProperty);
    wxPGChoices choices;
    choices.Add("First"); choices.Add("Second");
    auto* const property = fixture.grid->Append(new wxEditEnumProperty("Editable", "editable", choices, "Initial free value"));
    fixture.Show();
    const auto native = fixture.Select(property);
    REQUIRE(native.IsEditable());
    auto* const editor = wxDynamicCast(fixture.grid->GetEditorControl(), wxComboBox);
    REQUIRE(editor);
    editor->SetFocus();
    REQUIRE(WaitEditor([&]
    {
        const auto text = FindEditor<C::TextBox>(native);
        return text && text.IsLoaded() && text.ActualWidth() > 0;
    }));
    const auto text = FindEditor<C::TextBox>(native);
    CHECK(wxString(text.Text().c_str()) == "Initial free value");
    fixture.grid->Bind(wxEVT_PG_CHANGING, [&](wxPropertyGridEvent& event)
    {
        ++changing;
        CHECK(event.GetProperty() == property);
        if ( veto ) event.Veto();
    });
    fixture.grid->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent& event)
    { ++changed; CHECK(event.GetProperty() == property); });

    SetEditableValueViaAutomation(*editor, text, "Rejected UIA value");
    REQUIRE(WaitEditor([&] { return fixture.grid->IsEditorsValueModified(); }));
    CHECK(fixture.grid->GetPropertyValueAsString(property) == "Initial free value");
    CHECK(changing == 0); CHECK(changed == 0);
    CHECK_FALSE(fixture.grid->CommitChangesFromEditor());
    CHECK(fixture.grid->GetPropertyValueAsString(property) == "Initial free value");
    CHECK(changing == 1); CHECK(changed == 0);

    veto = false;
    SetEditableValueViaAutomation(*editor, text, "New native value");
    REQUIRE(WaitEditor([&] { return fixture.grid->IsEditorsValueModified(); }));
    CHECK(wxString(text.Text().c_str()) == "New native value");
    CHECK(fixture.grid->GetPropertyValueAsString(property) == "Initial free value");
    REQUIRE(fixture.grid->CommitChangesFromEditor());
    CHECK(fixture.grid->GetPropertyValueAsString(property) == "New native value");
    CHECK(changing == 2); CHECK(changed == 1);
    fixture.grid->SetPropertyValue(property, "Programmatic value");
    REQUIRE(WaitEditor([&] { return wxString(native.Text().c_str()) == "Programmatic value"; }));
    CHECK(changing == 2); CHECK(changed == 1);
    CHECK(fixture.grid->GetEditorControl() == editor);
    CHECK(NativeEditor<C::ComboBox>(editor) == native);
    CHECK(NativeEditor<C::TextBox>(editor) == text);
    REQUIRE(fixture.grid->ClearSelection());
    REQUIRE(WaitEditor([&] { return !native.IsLoaded(); }));
}

TEST_CASE("WinUIPropertyGridEditors::ChoiceAndButtonKeepsNativeSecondaryAndCustomContract",
          "[winui-propgrid][native-propgrid][native-pg-editors]")
{
    unsigned buttons = 0;
    EditorFixture fixture;
    wxPGChoices choices;
    choices.Add("First", 1); choices.Add("Second", 2);
    auto* const property = fixture.grid->Append(new wxEnumProperty("Button choice", "button", choices, 1));
    fixture.grid->SetPropertyEditor(property, "ChoiceAndButton");
    auto* const custom = fixture.grid->Append(new wxEnumProperty("Image choice", "custom", choices, 1));
    custom->SetValueImage(wxBitmap(12, 12));
    fixture.Show();
    const auto native = fixture.Select(property);
    auto* const secondary = wxDynamicCast(fixture.grid->GetEditorControlSecondary(), wxButton);
    REQUIRE(secondary);
    REQUIRE(WaitEditor([&]
    {
        const auto button = NativeEditor<C::Button>(secondary);
        return button && button.IsLoaded() && button.ActualWidth() > 0;
    }));
    CHECK_FALSE(native.IsEditable());
    fixture.grid->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event)
    { ++buttons; CHECK(event.GetId() == fixture.grid->GetId()); });
    const auto peer = A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(
        NativeEditor<C::Button>(secondary));
    REQUIRE(peer);
    const auto invoke = peer.GetPattern(A::Peers::PatternInterface::Invoke).try_as<A::Provider::IInvokeProvider>();
    REQUIRE(invoke);
    invoke.Invoke();
    REQUIRE(WaitEditor([&] { return buttons == 1; }));
    CHECK(fixture.grid->GetPropertyValueAsLong(property) == 1);
    REQUIRE(fixture.grid->SelectProperty(custom));
    // A custom value image is an explicit owner-draw contract. This slice
    // does not silently discard it to claim every property is already native.
    REQUIRE(wxDynamicCast(fixture.grid->GetEditorControl(), wxOwnerDrawnComboBox));
    REQUIRE(WaitEditor([&] { return !native.IsLoaded(); }));
}

TEST_CASE("WinUIPropertyGridEditors::NativeResizeCallbackCanDestroyGridWithOpenPopup",
          "[winui-propgrid][native-propgrid][native-pg-editors]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    EditorFixture fixture(rtl);
    wxPGChoices choices;
    choices.Add("First", 1); choices.Add("Second", 2);
    auto* const property = fixture.grid->Append(
        new wxEnumProperty("Resize editor", "resize", choices, 1));
    fixture.Show();
    const auto native = fixture.Select(property);
    auto* const editor = wxDynamicCast(fixture.grid->GetEditorControl(), wxComboBox);
    REQUIRE(editor);
    editor->Popup();
    REQUIRE(WaitEditor([&] { return native.IsDropDownOpen(); }));
    const wxWeakRef<wxWindow> weakGrid(fixture.grid);
    const wxWeakRef<wxWindow> weakEditor(editor);
    bool destroyed = false, survivedNestedYield = false;
    editor->Bind(wxEVT_SIZE, [&](wxSizeEvent& event)
    {
        event.Skip();
        if ( destroyed ) return;
        destroyed = true;
        delete fixture.grid;
        fixture.grid = nullptr;
        CHECK_FALSE(weakGrid);
        CHECK(weakEditor);
        wxYield();
        survivedNestedYield = bool(weakEditor);
    });
    wxSizeEvent resize(editor->GetSize(), editor->GetId());
    resize.SetEventObject(editor);
    editor->ProcessWindowEvent(resize);
    CHECK(destroyed);
    CHECK_FALSE(weakGrid);
    CHECK(survivedNestedYield);
    REQUIRE(WaitEditor([&] { return !weakEditor && !native.IsLoaded(); }));
    CHECK_FALSE(native.IsDropDownOpen());
}

TEST_CASE("WinUIPropertyGridEditors::TextIsNativeFieldInsideRowWithoutFormattingCommands",
          "[winui-propgrid][native-propgrid][native-pg-editors][native-pg-text-layout]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    EditorFixture fixture(rtl);
    auto* const label = fixture.grid->Append(new wxStringProperty("Label", "label", "Showcase"));
    auto* const number = fixture.grid->Append(new wxIntProperty("Margin", "margin", 8));
    fixture.Show();
    for ( auto* property : {label, number} )
    {
        REQUIRE(fixture.grid->SelectProperty(property));
        auto* const text = wxDynamicCast(fixture.grid->GetEditorControl(), wxTextCtrl);
        REQUIRE(text);
        REQUIRE_FALSE(text->IsRich());
        REQUIRE(WaitEditor([&]
        {
            const auto peer = NativeEditor<C::RichEditBox>(text);
            return peer && peer.IsLoaded() && peer.ActualHeight() > 0;
        }));
        const auto peer = NativeEditor<C::RichEditBox>(text);
        CHECK(peer.DisabledFormattingAccelerators() == C::DisabledFormattingAccelerators::All);
        // The native text selection flyout still owns clipboard commands;
        // plain editors suppress formatting, not the entire native flyout.
        CHECK(peer.SelectionFlyout());
        // A field of the standard height in the middle of a higher row, as
        // the editor of a cell of a WinUI table.
        const auto insideRow = [&]()
        {
            const int rowHeight = fixture.grid->GetRowHeight();
            const wxRect rect = text->GetRect();
            CHECK(rect.height == fixture.grid->FromDIP(32));
            CHECK(rowHeight == fixture.grid->FromDIP(40));
            CHECK(rect.y % rowHeight == (rowHeight - rect.height) / 2);
        };
        insideRow();
        CHECK(peer.ActualHeight() == Approx(32));
        const auto before = peer;
        fixture.frame->SetClientSize(680, 360);
        fixture.frame->Layout();
        REQUIRE(WaitEditor([&] { return NativeEditor<C::RichEditBox>(text) == before; }));
        insideRow();
        CHECK(fixture.grid->GetEditorControl() == text);
    }
}

TEST_CASE("WinUIPropertyGridEditors::RichFormattingPolicyFollowsPublicTextStyle",
          "[winui-propgrid][native-propgrid][native-pg-editors][winui-text-formatting]")
{
    EditorFixture fixture;
    wxTextCtrl plain(fixture.frame.get(), wxID_ANY, "plain", wxPoint(10, 10), wxSize(180, 32));
    wxTextCtrl rich(fixture.frame.get(), wxID_ANY, "rich", wxPoint(10, 50), wxSize(180, 32), wxTE_RICH2);
    wxTextCtrl password(fixture.frame.get(), wxID_ANY, "secret", wxPoint(10, 90), wxSize(180, 32), wxTE_PASSWORD);
    fixture.Show();
    const auto plainPeer = NativeEditor<C::RichEditBox>(&plain);
    const auto richPeer = NativeEditor<C::RichEditBox>(&rich);
    const auto passwordPeer = NativeEditor<C::RichEditBox>(&password);
    REQUIRE(plainPeer); REQUIRE(richPeer); REQUIRE(passwordPeer);
    CHECK(plainPeer.DisabledFormattingAccelerators() == C::DisabledFormattingAccelerators::All);
    CHECK(richPeer.DisabledFormattingAccelerators() == C::DisabledFormattingAccelerators::None);
    CHECK(passwordPeer.DisabledFormattingAccelerators() == C::DisabledFormattingAccelerators::All);
    plain.SetWindowStyleFlag(plain.GetWindowStyleFlag() | wxTE_READONLY);
    CHECK(plainPeer.DisabledFormattingAccelerators() == C::DisabledFormattingAccelerators::All);
    plain.SetWindowStyleFlag(plain.GetWindowStyleFlag() | wxTE_AUTO_URL);
    CHECK(plain.IsRich());
    CHECK(NativeEditor<C::RichEditBox>(&plain) == plainPeer);
    CHECK(plainPeer.DisabledFormattingAccelerators() == C::DisabledFormattingAccelerators::None);
}

TEST_CASE("WinUIPropertyGridEditors::DefaultBoolUsesNativeChoiceAndValidation",
          "[winui-propgrid][native-pg-editors][native-pg-bool-choices]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    EditorFixture fixture(rtl);
    auto* const grid = fixture.grid;
    auto* const property = grid->Append(new wxBoolProperty("Enabled", "enabled", false));
    fixture.Show();
    const auto native = fixture.Select(property);
    auto* const editor = wxDynamicCast(grid->GetEditorControl(), wxComboBox);
    REQUIRE(editor);
    CHECK_FALSE(native.IsEditable());
    CHECK(native.Items().Size() == 2);
    CHECK(native.SelectedIndex() == 0);
    bool veto = true;
    unsigned changing = 0, changed = 0;
    grid->Bind(wxEVT_PG_CHANGING, [&](wxPropertyGridEvent& event)
    {
        ++changing;
        CHECK(event.GetProperty() == property);
        if ( veto ) event.Veto();
    });
    grid->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent& event)
    { ++changed; CHECK(event.GetProperty() == property); });
    ChoiceProvider(editor, native, 1).Select();
    REQUIRE(WaitEditor([&] { return changing == 1 && native.SelectedIndex() == 0; }));
    CHECK_FALSE(property->GetValue().GetBool());
    CHECK(changed == 0);
    veto = false;
    ChoiceProvider(editor, native, 1).Select();
    REQUIRE(WaitEditor([&] { return changed == 1 && property->GetValue().GetBool(); }));
    CHECK(changing == 2);
    grid->SetPropertyValue(property, false);
    REQUIRE(WaitEditor([&] { return native.SelectedIndex() == 0; }));
    CHECK(changed == 1);
    CHECK(NativeEditor<C::ComboBox>(editor) == native);
    grid->SetPropertyValueUnspecified(property);
    REQUIRE(WaitEditor([&] { return native.SelectedIndex() == -1; }));
    ChoiceProvider(editor, native, 0).Select();
    REQUIRE(WaitEditor([&] { return changed == 2; }));
    CHECK_FALSE(property->IsValueUnspecified());
    CHECK_FALSE(property->GetValue().GetBool());
    const auto stale = ChoiceProvider(editor, native, 1);
    grid->DeleteProperty(property);
    auto* const replacement = grid->Append(new wxBoolProperty("Replacement", "enabled", false));
    REQUIRE(WaitEditor([&] { return !native.IsLoaded(); }));
    try { stale.Select(); }
    catch ( const winrt::hresult_error& error )
    {
        const auto code = static_cast<int32_t>(error.code());
        CHECK((code == static_cast<int32_t>(0x80040200u) || code == static_cast<int32_t>(0x80040201u)));
    }
    CHECK_FALSE(replacement->GetValue().GetBool());
    CHECK(changed == 2);
}

TEST_CASE("WinUIPropertyGridEditors::FlagsChildrenPreserveAggregateCommitAndCheckboxChoice",
          "[winui-propgrid][native-pg-editors][native-pg-bool-choices]")
{
    EditorFixture fixture;
    auto* const grid = fixture.grid;
    wxPGChoices choices;
    choices.Add("Read", 1); choices.Add("Write", 4);
    auto* const flags = grid->Append(new wxFlagsProperty("Permissions", "permissions", choices, 1));
    REQUIRE(flags->GetChildCount() == 2);
    auto* const write = flags->Item(1);
    fixture.Show();
    const auto native = fixture.Select(write);
    auto* const editor = wxDynamicCast(grid->GetEditorControl(), wxComboBox);
    REQUIRE(editor);
    unsigned changing = 0, changed = 0;
    bool veto = true;
    grid->Bind(wxEVT_PG_CHANGING, [&](wxPropertyGridEvent& event)
    {
        ++changing;
        CHECK(event.GetProperty() == flags);
        if ( veto ) event.Veto();
    });
    grid->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent& event)
    { ++changed; CHECK(event.GetProperty() == flags); });
    ChoiceProvider(editor, native, 1).Select();
    REQUIRE(WaitEditor([&] { return changing == 1 && native.SelectedIndex() == 0; }));
    CHECK(flags->GetValue().GetLong() == 1);
    CHECK_FALSE(write->GetValue().GetBool());
    veto = false;
    ChoiceProvider(editor, native, 1).Select();
    REQUIRE(WaitEditor([&] { return changed == 1; }));
    CHECK(flags->GetValue().GetLong() == 5);
    CHECK(write->GetValue().GetBool());
    CHECK(grid->GetEditorControl() == editor);
    grid->SetPropertyValue(flags, 1L);
    REQUIRE(WaitEditor([&] { return native.SelectedIndex() == 0; }));
    CHECK(changed == 1);
    REQUIRE(grid->ClearSelection());
    REQUIRE(WaitEditor([&] { return !native.IsLoaded(); }));
#if wxUSE_CHECKBOX && wxPG_INCLUDE_CHECKBOX
    grid->SetPropertyAttribute(flags, wxPG_BOOL_USE_CHECKBOX, true);
    REQUIRE(grid->SelectProperty(write));
    auto* const check = wxDynamicCast(grid->GetEditorControl(), wxCheckBox);
    REQUIRE(check);
    C::CheckBox checkPeer{nullptr};
    REQUIRE(WaitEditor([&]
    {
        checkPeer = NativeEditor<C::CheckBox>(check);
        return checkPeer && checkPeer.IsLoaded() && checkPeer.ActualHeight() >= 30;
    }));
    const auto automation = A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(checkPeer);
    REQUIRE(automation);
    const auto toggle = automation.GetPattern(A::Peers::PatternInterface::Toggle)
                                  .try_as<A::Provider::IToggleProvider>();
    REQUIRE(toggle);
    toggle.Toggle();
    REQUIRE(WaitEditor([&] { return changed == 2; }));
    CHECK(flags->GetValue().GetLong() == 5);
    CHECK(write->GetValue().GetBool());
#endif
}

TEST_CASE("WinUIPropertyGridEditors::BoolDoubleClickCyclesThroughTheNativeEditorRoute",
          "[winui-propgrid][native-pg-editors][native-pg-bool-choices]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    EditorFixture fixture(rtl);
    auto* const grid = fixture.grid;
    auto* const property = grid->Append(new wxBoolProperty("Cycle", "cycle", false));
    fixture.Show();
    const auto native = fixture.Select(property);
    auto* const editor = wxDynamicCast(grid->GetEditorControl(), wxComboBox);
    REQUIRE(editor);
    C::ContentPresenter presenter{nullptr};
    REQUIRE(WaitEditor([&]
    {
        presenter = native.as<C::IControlProtected>().GetTemplateChild(L"ContentPresenter")
                          .try_as<C::ContentPresenter>();
        return presenter && presenter.IsLoaded() && presenter.ActualWidth() > 0 && presenter.ActualHeight() > 0;
    }));
    const auto position = presenter.TransformToVisual(native).TransformPoint(
        {static_cast<float>(presenter.ActualWidth() / 2), static_cast<float>(presenter.ActualHeight() / 2)});
    const wxPoint textPoint(static_cast<int>(std::lround(position.X * grid->GetDPIScaleFactor())),
                            static_cast<int>(std::lround(position.Y * grid->GetDPIScaleFactor())));
    unsigned changed = 0, changing = 0;
    bool veto = true;
    grid->Bind(wxEVT_PG_CHANGING, [&](wxPropertyGridEvent& event)
    { ++changing; if ( veto ) event.Veto(); });
    grid->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent&) { ++changed; });
    const auto send = [&](wxEventType type)
    {
        wxMouseEvent event(type);
        event.SetEventObject(editor);
        event.SetId(editor->GetId());
        event.SetPosition(textPoint);
        event.SetLeftDown(type != wxEVT_LEFT_UP);
        editor->ProcessWindowEvent(event);
    };
    const auto doubleClick = [&]
    {
        // The first click selected the property and created its editor. Its
        // release then arrives at the new HWND: no matching DOWN and no edit.
        wxFocusEvent focus(wxEVT_SET_FOCUS, editor->GetId());
        focus.SetEventObject(editor);
        editor->ProcessWindowEvent(focus);
        send(wxEVT_LEFT_UP);
        send(wxEVT_LEFT_DCLICK);
        send(wxEVT_LEFT_UP);
    };
    doubleClick();
    REQUIRE(WaitEditor([&] { return changing == 1 && native.SelectedIndex() == 0; }));
    CHECK(changed == 0);
    CHECK_FALSE(native.IsDropDownOpen());
    veto = false;
    doubleClick();
    REQUIRE(WaitEditor([&] { return changed == 1 && native.SelectedIndex() == 1; }));
    CHECK(property->GetValue().GetBool());
    CHECK_FALSE(native.IsDropDownOpen());
    CHECK(NativeEditor<C::ComboBox>(editor) == native);
    grid->SetPropertyAttribute(property, wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING, false);
    doubleClick();
    CHECK(changing == 2);
    CHECK(changed == 1);
    CHECK(property->GetValue().GetBool());
    CHECK(NativeEditor<C::ComboBox>(editor) == native);
}

TEST_CASE("WinUIPropertyGridEditors::BoolChoiceDeletionDuringValidationRetiresNativeEditor",
          "[winui-propgrid][native-pg-editors][native-pg-bool-choices]")
{
    const bool deleteOwner = GENERATE(false, true);
    CAPTURE(deleteOwner);
    EditorFixture fixture;
    auto* const grid = fixture.grid;
    auto* const property = grid->Append(new wxBoolProperty("Delete", "delete", false));
    fixture.Show();
    const auto native = fixture.Select(property);
    auto* const editor = wxDynamicCast(grid->GetEditorControl(), wxComboBox);
    REQUIRE(editor);
    const auto provider = ChoiceProvider(editor, native, 1);
    const wxWeakRef<wxWindow> weakGrid(grid), weakEditor(editor);
    unsigned callbacks = 0;
    grid->Bind(wxEVT_PG_CHANGING, [&](wxPropertyGridEvent& event)
    {
        ++callbacks;
        event.Veto();
        if ( deleteOwner )
        {
            fixture.grid = nullptr;
            delete grid; // No access to the deleted callback storage after this.
        }
        else grid->DeleteProperty(property);
    });
    provider.Select();
    REQUIRE(WaitEditor([&] { return callbacks == 1 && !native.IsLoaded() && !weakEditor; }));
    CHECK(bool(weakGrid) == !deleteOwner);
    if ( !deleteOwner ) CHECK_FALSE(grid->GetPropertyByName("delete"));
}

TEST_CASE("WinUIPropertyGridEditors::BoolChoicePreservesExplicitOwnerDrawContracts",
          "[winui-propgrid][native-pg-editors][native-pg-bool-choices]")
{
    EditorFixture fixture;
    auto* const image = fixture.grid->Append(new wxBoolProperty("Image", "image", false));
    image->SetValueImage(wxBitmap(12, 12));
    auto* const common = fixture.grid->Append(new wxBoolProperty("Common", "common", false));
    common->EnableCommonValue();
    REQUIRE(common->GetDisplayedCommonValueCount() > 0);
    fixture.Show();
    for ( auto* property : {image, common} )
    {
        REQUIRE(fixture.grid->SelectProperty(property));
        REQUIRE(wxDynamicCast(fixture.grid->GetEditorControl(), wxOwnerDrawnComboBox));
        CHECK_FALSE(NativeEditor<C::ComboBox>(fixture.grid->GetEditorControl()));
    }
}
#endif
