/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuipropgrid.cpp
// Purpose:     Native PropertyGrid presentation and public edit transactions
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_PROPGRID

#include "wx/app.h"
#include "wx/checkbox.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/msw/private.h"
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/manager.h"
#include "wx/propgrid/props.h"
#include "wx/sizer.h"
#include "wx/scopeguard.h"
#include "wx/stopwatch.h"
#include "wx/textctrl.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "wx/winui/winui.h"
#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation;

template<typename Predicate>
bool WaitProperty(Predicate predicate)
{
    auto* const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp ) return false;
    wxStopWatch elapsed;
    while ( elapsed.Time() < 1500 )
    {
        if ( predicate() ) return true;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop ) return false;
        if ( predicate() ) return true;
        // DispatchTimeout() alone does not run wx's idle phase. PropertyGrid
        // retires its deferred editors/properties in OnIdle, just as the real
        // application loop does when the native and wx queues are empty.
        if ( !loop->Pending() && !wxTheApp->HasPendingEvents() )
            wxTheApp->ProcessIdle();
        if ( wxEventLoopBase::GetActive() != loop ) return false;
        if ( predicate() ) return true;
        if ( loop->DispatchTimeout(5) == 0 ) return false;
    }
    return predicate();
}

template<typename T>
T FindPropertyPart(const X::DependencyObject& node, const winrt::hstring& id = {})
{
    if ( !node ) return nullptr;
    if ( const auto found = node.try_as<T>(); found &&
         (id.empty() || A::AutomationProperties::GetAutomationId(found) == id) ) return found;
    for ( int n = 0; n < X::Media::VisualTreeHelper::GetChildrenCount(node); ++n )
        if ( const auto found = FindPropertyPart<T>(X::Media::VisualTreeHelper::GetChild(node, n), id) )
            return found;
    return nullptr;
}

X::DependencyObject PropertyRoot(wxPropertyGrid* grid)
{
    auto* const host = grid ? wxWinUITopLevelHost::FindSlotOwner(grid) : nullptr;
    auto* const slot = host ? host->FindSlot(grid) : nullptr;
    return slot ? slot->GetContent() : nullptr;
}

bool PropertyHasText(const X::DependencyObject& root, const wxString& text)
{
    if ( !root ) return false;
    if ( const auto block = root.try_as<C::TextBlock>(); block && wxString(block.Text().c_str()) == text )
        return true;
    for ( int n = 0; n < X::Media::VisualTreeHelper::GetChildrenCount(root); ++n )
        if ( PropertyHasText(X::Media::VisualTreeHelper::GetChild(root, n), text) ) return true;
    return false;
}

C::TextBlock PropertyText(const X::DependencyObject& root, const wxString& value)
{
    if ( !root ) return nullptr;
    if ( const auto text = root.try_as<C::TextBlock>(); text && wxString(text.Text().c_str()) == value )
        return text;
    for ( int n = 0; n < X::Media::VisualTreeHelper::GetChildrenCount(root); ++n )
        if ( const auto found = PropertyText(X::Media::VisualTreeHelper::GetChild(root, n), value) ) return found;
    return nullptr;
}

C::Control PropertyControl(const X::DependencyObject& root, const wxString& text)
{
    X::DependencyObject node = PropertyText(root, text);
    while ( node )
    {
        if ( const auto control = node.try_as<C::Control>() ) return control;
        node = X::Media::VisualTreeHelper::GetParent(node);
    }
    return nullptr;
}

C::Button PropertyCell(wxPropertyGrid* grid, const wxString& name, unsigned column)
{
    return FindPropertyPart<C::Button>(PropertyRoot(grid),
        winrt::hstring(wxString::Format("wxProperty.Cell.%s.%u", name, column).wc_str()));
}

C::Button PropertyExpander(wxPropertyGrid* grid, const wxString& name)
{
    return FindPropertyPart<C::Button>(PropertyRoot(grid),
        winrt::hstring(("wxProperty.Expand." + name).wc_str()));
}

bool PropertyPartVisible(const X::FrameworkElement& element)
{
    if ( !element || !element.IsLoaded() ) return false;
    for ( X::DependencyObject node = element; node;
          node = X::Media::VisualTreeHelper::GetParent(node) )
        if ( const auto part = node.try_as<X::UIElement>(); part &&
             part.Visibility() != X::Visibility::Visible ) return false;
    return true;
}

unsigned PropertyCellCount(const X::DependencyObject& root)
{
    if ( !root ) return 0;
    unsigned count = 0;
    if ( const auto control = root.try_as<C::Button>(); control &&
         wxString(A::AutomationProperties::GetAutomationId(control).c_str()).StartsWith("wxProperty.Cell.") ) ++count;
    for ( int n = 0; n < X::Media::VisualTreeHelper::GetChildrenCount(root); ++n )
        count += PropertyCellCount(X::Media::VisualTreeHelper::GetChild(root, n));
    return count;
}

A::Provider::IInvokeProvider PropertyInvoke(const C::Control& control)
{
    const auto peer = control ? A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(control) : nullptr;
    return peer ? peer.GetPattern(A::Peers::PatternInterface::Invoke)
                          .try_as<A::Provider::IInvokeProvider>() : nullptr;
}

void InvokeRetiredProperty(const A::Provider::IInvokeProvider& provider)
{
    try { provider.Invoke(); }
    catch ( const winrt::hresult_error& error )
    {
        const auto code = static_cast<int32_t>(error.code());
        CHECK((code == static_cast<int32_t>(0x80040200u) || code == static_cast<int32_t>(0x80040201u)));
    }
}

wxRect PropertyScreenRect(wxPropertyGrid* grid, const X::FrameworkElement& element)
{
    auto* const host = wxWinUITopLevelHost::ForWindow(grid, false);
    if ( !host || !element || !element.IsLoaded() ) return {};
    const auto bounds = element.TransformToVisual(host->Root()).TransformBounds(
        {0, 0, static_cast<float>(element.ActualWidth()), static_cast<float>(element.ActualHeight())});
    const auto origin = host->GetClientScreenRect();
    const double scale = host->GetScale();
    return wxRect(origin.left + static_cast<int>(std::lround(bounds.X * scale)),
                  origin.top + static_cast<int>(std::lround(bounds.Y * scale)),
                  static_cast<int>(std::lround(bounds.Width * scale)),
                  static_cast<int>(std::lround(bounds.Height * scale)));
}

struct PropertyFixture
{
    explicit PropertyFixture(bool rtl = false, bool pages = false)
    {
        wxPoint position(-30000, -30000);
        wchar_t desktop[256]{};
        DWORD needed = 0;
        if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
                UOI_NAME, desktop, sizeof(desktop), &needed) &&
             wxString(desktop).StartsWith("wxWinUITest-") ) position = wxPoint(80, 80);
        frame = std::make_unique<wxFrame>(nullptr, wxID_ANY, "Native PropertyGrid",
                                         position, wxSize(680, 400));
        frame->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        wxWindow* content = nullptr;
        if ( pages )
        {
            manager = new wxPropertyGridManager(frame.get(), wxID_ANY);
            manager->AddPage("First page");
            grid = manager->GetGrid();
            content = manager;
        }
        else
        {
            auto created = std::make_unique<wxPropertyGrid>();
            REQUIRE(created->Create(frame.get(), wxID_ANY));
            grid = created.release();
            content = grid;
        }
        grid->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        grid->SetValidationFailureBehavior(wxPGVFBFlags::Null);
        auto* const sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(content, 1, wxEXPAND);
        frame->SetSizer(sizer);
    }
    ~PropertyFixture()
    {
        frame.reset();
        wxTheApp->ProcessPendingEvents();
    }
    void Show()
    {
        frame->Layout();
        frame->ShowWithoutActivating();
        REQUIRE(WaitProperty([&]
        {
            const auto root = PropertyRoot(grid).try_as<X::FrameworkElement>();
            return root && root.IsLoaded() && root.ActualWidth() > 0;
        }));
    }
    wxWindow* EventTarget() const
    {
        // wxPropertyGridManager installs itself as grid.m_eventObject;
        // SendEvent() dispatches there directly, not through the child grid.
        return manager ? static_cast<wxWindow*>(manager) : grid;
    }
    std::unique_ptr<wxFrame> frame;
    wxPropertyGrid* grid = nullptr;
    wxPropertyGridManager* manager = nullptr;
};

C::RichEditBox PropertyNativeEditor(wxPropertyGrid* grid)
{
    auto* const editor = grid->GetEditorTextCtrl();
    auto* const host = editor ? wxWinUITopLevelHost::FindSlotOwner(editor) : nullptr;
    auto* const slot = host ? host->FindSlot(editor) : nullptr;
    return slot ? slot->GetSemanticTarget().try_as<C::RichEditBox>() : nullptr;
}

class CountedProperty final : public wxStringProperty
{
public:
    CountedProperty(const wxString& name, const std::shared_ptr<unsigned>& calls)
        : wxStringProperty(name, name, "value " + name), m_calls(calls) {}
    wxString ValueToString(wxVariant& value, wxPGPropValFormatFlags flags) const override
    {
        ++*m_calls;
        return wxStringProperty::ValueToString(value, flags);
    }
private:
    std::shared_ptr<unsigned> m_calls;
};

}

TEST_CASE("WinUIPropertyGrid::NativeNamesValuesAndGeometryFollowPublicProperties",
          "[winui-propgrid][native-propgrid]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    PropertyFixture fixture(rtl);
    auto* const grid = fixture.grid;
    grid->Append(new wxPropertyCategory("Appearance", "appearance"));
    auto* const name = grid->Append(new wxStringProperty("Object name", "name", "Initial name"));
    grid->Append(new wxIntProperty("Count", "count", 42));
    fixture.Show();
    REQUIRE(WaitProperty([&]
    {
        const auto root = PropertyRoot(grid);
        return PropertyHasText(root, "Appearance") && PropertyHasText(root, "Object name") &&
               PropertyHasText(root, "Initial name") && PropertyHasText(root, "42");
    }));
    const auto label = PropertyControl(PropertyRoot(grid), "Object name");
    const auto value = PropertyControl(PropertyRoot(grid), "Initial name");
    REQUIRE(label);
    REQUIRE(value);
    CHECK(label != value);
    REQUIRE(WaitProperty([&]
    {
        return !PropertyScreenRect(grid, label).IsEmpty() && !PropertyScreenRect(grid, value).IsEmpty();
    }));
    const auto labelRect = PropertyScreenRect(grid, label);
    const auto valueRect = PropertyScreenRect(grid, value);
    CHECK((rtl ? labelRect.x > valueRect.x : labelRect.x < valueRect.x));
    CHECK(std::abs(labelRect.y - valueRect.y) <= 2);
    grid->SetPropertyValue(name, "Updated name");
    grid->SetPropertyLabel(name, "Renamed label");
    REQUIRE(WaitProperty([&]
    {
        return PropertyHasText(PropertyRoot(grid), "Updated name") &&
               PropertyHasText(PropertyRoot(grid), "Renamed label");
    }));
    CHECK(PropertyControl(PropertyRoot(grid), "Updated name") == value);
    CHECK(PropertyControl(PropertyRoot(grid), "Renamed label") == label);
    fixture.frame->SetClientSize(wxSize(760, 400));
    const bool resized = WaitProperty([&]
    {
        const auto root = PropertyRoot(grid).try_as<X::FrameworkElement>();
        const auto clip = root ? root.Clip().try_as<X::Media::RectangleGeometry>() : nullptr;
        // The host slot spans GetWindowRect (the outer control), while the
        // projection's clip is exactly the drawable client viewport. A PG
        // border makes these intentionally different widths.
        return grid->GetSize().x == 760 && root &&
               std::abs(root.ActualWidth() - grid->GetSize().x / grid->GetDPIScaleFactor()) < 1 &&
               clip && std::abs(clip.Rect().Width - grid->GetClientSize().x / grid->GetDPIScaleFactor()) < 1;
    });
    const auto rootAfterResize = PropertyRoot(grid).try_as<X::FrameworkElement>();
    const auto clipAfterResize = rootAfterResize ? rootAfterResize.Clip().try_as<X::Media::RectangleGeometry>() : nullptr;
    INFO("frame client=" << fixture.frame->GetClientSize().x
         << " grid outer=" << grid->GetSize().x << " client=" << grid->GetClientSize().x
         << " dpi=" << grid->GetDPIScaleFactor()
         << " root loaded=" << (rootAfterResize && rootAfterResize.IsLoaded())
         << " actual=" << (rootAfterResize ? rootAfterResize.ActualWidth() : -1)
         << " clip=" << (clipAfterResize ? clipAfterResize.Rect().Width : -1));
    REQUIRE(resized);
    CHECK(PropertyControl(PropertyRoot(grid), "Updated name") == value);
    CHECK(PropertyControl(PropertyRoot(grid), "Renamed label") == label);
}

TEST_CASE("WinUIPropertyGrid::CollapseAndSplitterRetainUnchangedNativeNeighbours",
          "[winui-propgrid][native-propgrid][native-branch-update]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    unsigned collapsed = 0, expanded = 0;
    PropertyFixture fixture(rtl);
    auto* const grid = fixture.grid;
    auto* const category = grid->Append(new wxPropertyCategory("First category", "first"));
    grid->Append(new wxStringProperty("Child", "child", "child value"));
    grid->Append(new wxPropertyCategory("Other category", "other"));
    grid->Append(new wxStringProperty("Neighbour", "neighbour", "stable neighbour"));
    fixture.Show();
    REQUIRE(WaitProperty([&]
    {
        return PropertyCell(grid, "neighbour", 1) && PropertyCell(grid, "child", 1) &&
               PropertyExpander(grid, "first");
    }));
    const auto neighbour = PropertyCell(grid, "neighbour", 1);
    const auto other = PropertyCell(grid, "other", 0);
    REQUIRE(neighbour.IsLoaded());
    const auto before = PropertyScreenRect(grid, neighbour);
    grid->Bind(wxEVT_PG_ITEM_COLLAPSED, [&](wxPropertyGridEvent& event)
    { ++collapsed; CHECK(event.GetProperty() == category); event.Skip(); });
    grid->Bind(wxEVT_PG_ITEM_EXPANDED, [&](wxPropertyGridEvent& event)
    { ++expanded; CHECK(event.GetProperty() == category); event.Skip(); });
    const auto collapse = PropertyInvoke(PropertyExpander(grid, "first"));
    REQUIRE(collapse);
    collapse.Invoke();
    REQUIRE(WaitProperty([&]
    {
        return collapsed == 1 && !PropertyCell(grid, "child", 1) &&
               PropertyScreenRect(grid, PropertyCell(grid, "neighbour", 1)).y < before.y;
    }));
    CHECK(PropertyCell(grid, "neighbour", 1) == neighbour);
    CHECK(PropertyCell(grid, "other", 0) == other);
    CHECK(PropertyHasText(neighbour, "stable neighbour"));
    const auto expand = PropertyInvoke(PropertyExpander(grid, "first"));
    REQUIRE(expand);
    expand.Invoke();
    REQUIRE(WaitProperty([&]
    {
        return expanded == 1 && PropertyCell(grid, "child", 1) &&
               std::abs(PropertyScreenRect(grid, PropertyCell(grid, "neighbour", 1)).y - before.y) <= 2;
    }));
    CHECK(PropertyCell(grid, "neighbour", 1) == neighbour);
    const auto widthBefore = PropertyScreenRect(grid, neighbour).width;
    grid->SetSplitterPosition(grid->FromDIP(210));
    REQUIRE(WaitProperty([&]
    {
        return PropertyScreenRect(grid, PropertyCell(grid, "neighbour", 1)).width != widthBefore;
    }));
    CHECK(PropertyCell(grid, "neighbour", 1) == neighbour);
    CHECK(PropertyCell(grid, "other", 0) == other);
}

TEST_CASE("WinUIPropertyGrid::ViewportRealizesBoundedCellsAndRetiresOffscreenProviders",
          "[winui-propgrid][native-propgrid][virtual]")
{
    unsigned selected = 0;
    PropertyFixture fixture;
    auto* const grid = fixture.grid;
    std::vector<std::shared_ptr<unsigned>> calls;
    std::vector<wxPGProperty*> properties;
    for ( unsigned index = 0; index < 200; ++index )
    {
        calls.push_back(std::make_shared<unsigned>(0));
        properties.push_back(grid->Append(new CountedProperty(wxString::Format("row%u", index), calls.back())));
    }
    fixture.Show();
    REQUIRE(WaitProperty([&] { return PropertyCell(grid, "row0", 0) != nullptr; }));
    const auto provider = PropertyInvoke(PropertyCell(grid, "row0", 0));
    REQUIRE(provider);
    CHECK(PropertyCellCount(PropertyRoot(grid)) < 80);
    CHECK_FALSE(PropertyCell(grid, "row199", 0));
    const unsigned distantBefore = *calls[199];
    grid->Bind(wxEVT_PG_SELECTED, [&](wxPropertyGridEvent& event)
    { ++selected; event.Skip(); });
    REQUIRE(grid->EnsureVisible(properties[199]));
    REQUIRE(WaitProperty([&]
    {
        return PropertyCell(grid, "row199", 1) &&
               PropertyHasText(PropertyCell(grid, "row199", 1), "value row199") &&
               !PropertyCell(grid, "row0", 0);
    }));
    CHECK(PropertyCellCount(PropertyRoot(grid)) < 80);
    CHECK(*calls[199] > distantBefore);
    const unsigned beforeInvoke = selected;
    InvokeRetiredProperty(provider);
    wxTheApp->ProcessPendingEvents();
    CHECK(selected == beforeInvoke);
    CHECK(grid->GetSelection() != properties[0]);
    const auto current = PropertyInvoke(PropertyCell(grid, "row199", 0));
    REQUIRE(current);
    current.Invoke();
    REQUIRE(WaitProperty([&]
    { return selected == beforeInvoke + 1 && grid->GetSelection() == properties[199]; }));
}

TEST_CASE("WinUIPropertyGrid::RetiredPropertyAndPageProvidersCannotSelectReplacements",
          "[winui-propgrid][native-propgrid][native-lifetime]")
{
    const bool page = GENERATE(false, true);
    CAPTURE(page);
    unsigned selected = 0;
    PropertyFixture fixture(false, page);
    auto* const grid = fixture.grid;
    auto* const original = grid->Append(new wxStringProperty("Original", "same", "old"));
    fixture.Show();
    REQUIRE(WaitProperty([&] { return PropertyCell(grid, "same", 0) != nullptr; }));
    const auto cell = PropertyCell(grid, "same", 0);
    const auto retired = PropertyInvoke(cell);
    REQUIRE(retired);
    wxPGProperty* replacement = nullptr;
    if ( page )
    {
        auto* const second = fixture.manager->AddPage("Second page");
        REQUIRE(second);
        replacement = second->Append(new wxStringProperty("Replacement", "same", "new"));
        fixture.manager->SelectPage(1);
    }
    else
    {
        grid->DeleteProperty(original);
        replacement = grid->Append(new wxStringProperty("Replacement", "same", "new"));
    }
    REQUIRE(WaitProperty([&]
    {
        return PropertyCell(grid, "same", 0) && PropertyCell(grid, "same", 0) != cell &&
               PropertyHasText(PropertyCell(grid, "same", 0), "Replacement");
    }));
    auto* const eventTarget = fixture.EventTarget();
    eventTarget->Bind(wxEVT_PG_SELECTED, [&, eventTarget](wxPropertyGridEvent& event)
    {
        ++selected;
        CHECK(event.GetEventObject() == eventTarget);
        CHECK(event.GetProperty() == replacement);
        event.Skip();
    });
    InvokeRetiredProperty(retired);
    wxTheApp->ProcessPendingEvents();
    CHECK(selected == 0);
    CHECK(grid->GetSelection() != replacement);
    const auto current = PropertyInvoke(PropertyCell(grid, "same", 0));
    REQUIRE(current);
    current.Invoke();
    REQUIRE(WaitProperty([&] { return selected == 1 && grid->GetSelection() == replacement; }));
    fixture.frame.reset();
    fixture.grid = nullptr;
    fixture.manager = nullptr;
    InvokeRetiredProperty(current);
    InvokeRetiredProperty(retired);
    wxTheApp->ProcessPendingEvents();
    CHECK(selected == 1);
}

TEST_CASE("WinUIPropertyGrid::NativeSelectionCallbackCanRetirePropertyPageOrOwner",
          "[winui-propgrid][native-propgrid][native-lifetime]")
{
    const int action = GENERATE(0, 1, 2);
    CAPTURE(action);
    unsigned selected = 0;
    bool insideRetirement = false;
    PropertyFixture fixture(false, action == 1);
    auto* const grid = fixture.grid;
    auto* const property = grid->Append(new wxStringProperty("Callback target", "target", "old"));
    if ( fixture.manager )
    {
        auto* const page = fixture.manager->AddPage("Next page");
        REQUIRE(page);
        page->Append(new wxStringProperty("Page replacement", "replacement", "next"));
        fixture.manager->SelectPage(0);
    }
    fixture.Show();
    REQUIRE(WaitProperty([&] { return PropertyCell(grid, "target", 0) != nullptr; }));
    const auto provider = PropertyInvoke(PropertyCell(grid, "target", 0));
    REQUIRE(provider);
    wxWeakRef<wxWindow> weakGrid(grid);
    auto* const eventTarget = fixture.EventTarget();
    eventTarget->Bind(wxEVT_PG_SELECTED, [&, eventTarget](wxPropertyGridEvent& event)
    {
        // A page transition may synchronously report deselection while the
        // first selection callback is still retiring its page. This is not a
        // second activation of the retained native provider.
        if ( insideRetirement ) return;
        ++selected;
        CHECK(event.GetEventObject() == eventTarget);
        CHECK(event.GetProperty() == property);
        insideRetirement = true;
        // No Skip after retiring the event target: dispatch must not continue
        // through the deleted wx handler chain.
        if ( action == 0 )
        {
            grid->DeleteProperty(property);
            grid->Append(new wxStringProperty("Property replacement", "replacement", "new"));
        }
        else if ( action == 1 ) fixture.manager->SelectPage(1);
        else
        {
            fixture.grid = nullptr;
            insideRetirement = false;
            fixture.frame.reset();
            return;
        }
        insideRetirement = false;
    });
    provider.Invoke();
    REQUIRE(WaitProperty([&] { return selected == 1; }));
    if ( action == 2 ) CHECK_FALSE(weakGrid);
    else
    {
        REQUIRE(WaitProperty([&]
        {
            return !PropertyCell(grid, "target", 0) && PropertyCell(grid, "replacement", 0);
        }));
    }
    InvokeRetiredProperty(provider);
    wxTheApp->ProcessPendingEvents();
    CHECK(selected == 1);
}

#if wxUSE_TEXTCTRL
TEST_CASE("WinUIPropertyGrid::NativeSelectionUsesRealTextEditorAndWxVeto",
          "[winui-propgrid][native-propgrid][native-editor]")
{
    unsigned changing = 0, changed = 0;
    bool veto = true;
    PropertyFixture fixture;
    auto* const grid = fixture.grid;
    auto* const property = grid->Append(new wxStringProperty("Editable string", "edit", "original"));
    // Null cancels an invalid value but deliberately permits leaving the
    // property (CommitChangesFromEditor returns true). This case exercises
    // the blocking-veto contract without opening a validation message box.
    grid->SetValidationFailureBehavior(wxPGVFBFlags::StayInProperty);
    fixture.Show();
    C::Control cell{nullptr};
    REQUIRE(WaitProperty([&]
    {
        cell = PropertyControl(PropertyRoot(grid), "Editable string");
        return cell && cell.IsLoaded();
    }));
    const auto provider = PropertyInvoke(cell);
    REQUIRE(provider);
    provider.Invoke();
    REQUIRE(WaitProperty([&]
    {
        const auto native = PropertyNativeEditor(grid);
        return grid->GetSelection() == property && native && native.IsLoaded();
    }));
    auto* const editor = grid->GetEditorTextCtrl();
    REQUIRE(editor);
    const auto native = PropertyNativeEditor(grid);
    grid->Bind(wxEVT_PG_CHANGING, [&](wxPropertyGridEvent& event)
    {
        ++changing;
        CHECK(event.GetProperty() == property);
        if ( veto ) event.Veto(); else event.Skip();
    });
    grid->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent& event)
    {
        ++changed;
        CHECK(event.GetProperty() == property);
        event.Skip();
    });
    editor->SetValue("candidate");
    grid->EditorsValueWasModified();
    winrt::hstring text;
    native.Document().GetText(winrt::Microsoft::UI::Text::TextGetOptions::None, text);
    CHECK(wxString(text.c_str()).StartsWith("candidate"));
    CHECK_FALSE(grid->CommitChangesFromEditor());
    CHECK(property->GetValue().GetString() == "original");
    CHECK(changing == 1);
    CHECK(changed == 0);
    veto = false;
    REQUIRE(grid->GetEditorTextCtrl());
    grid->GetEditorTextCtrl()->SetValue("accepted");
    grid->EditorsValueWasModified();
    REQUIRE(grid->CommitChangesFromEditor());
    CHECK(property->GetValue().GetString() == "accepted");
    CHECK(changing == 2);
    CHECK(changed == 1);
    grid->ClearSelection();
    REQUIRE(WaitProperty([&] { return PropertyHasText(PropertyRoot(grid), "accepted"); }));
}
#endif // wxUSE_TEXTCTRL

#if wxUSE_CHECKBOX
TEST_CASE("WinUIPropertyGrid::BoolEditorUsesNativeToggleAndCommonValidation",
          "[winui-propgrid][native-propgrid][native-editor][native-toggle]")
{
    unsigned changing = 0, changed = 0;
    bool veto = true;
    PropertyFixture fixture;
    auto* const grid = fixture.grid;
    auto* const property = grid->Append(new wxBoolProperty("Enabled", "enabled", false));
    grid->SetPropertyAttribute(property, wxPG_BOOL_USE_CHECKBOX, true);
    fixture.Show();
    REQUIRE(grid->SelectProperty(property, true));
    auto* const editor = wxDynamicCast(grid->GetEditorControl(), wxCheckBox);
    REQUIRE(editor);
    C::CheckBox native{nullptr};
    REQUIRE(WaitProperty([&]
    {
        auto* const host = wxWinUITopLevelHost::FindSlotOwner(editor);
        auto* const slot = host ? host->FindSlot(editor) : nullptr;
        native = slot ? slot->GetSemanticTarget().try_as<C::CheckBox>() : nullptr;
        return native && native.IsLoaded();
    }));
    const auto peer = A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(native);
    const auto toggle = peer.GetPattern(A::Peers::PatternInterface::Toggle).as<A::Provider::IToggleProvider>();
    grid->Bind(wxEVT_PG_CHANGING, [&](wxPropertyGridEvent& event)
    {
        ++changing;
        CHECK(event.GetProperty() == property);
        if ( veto ) event.Veto(); else event.Skip();
    });
    grid->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent& event)
    { ++changed; CHECK(event.GetProperty() == property); event.Skip(); });
    toggle.Toggle();
    REQUIRE(WaitProperty([&] { return changing == 1; }));
    CHECK_FALSE(property->GetValue().GetBool());
    CHECK(changed == 0);
    REQUIRE(WaitProperty([&]
    {
        return editor->Get3StateValue() == wxCHK_UNCHECKED && native.IsChecked() && !native.IsChecked().Value();
    }));
    veto = false;
    toggle.Toggle();
    REQUIRE(WaitProperty([&] { return changed == 1 && property->GetValue().GetBool(); }));
    CHECK(changing == 2);
    CHECK(editor->Get3StateValue() == wxCHK_CHECKED);
    grid->SetPropertyValueUnspecified(property);
    REQUIRE(WaitProperty([&]
    {
        return editor->Get3StateValue() == wxCHK_UNDETERMINED && !native.IsChecked();
    }));
    CHECK(changing == 2);
    CHECK(changed == 1);
    const wxWeakRef<wxWindow> weakEditor(editor);
    grid->DeleteProperty(property);
    const bool retired = WaitProperty([&]
    { return grid->GetEditorControl() != editor && !native.IsLoaded(); });
    INFO("current editor still old=" << (grid->GetEditorControl() == editor)
         << " wx editor alive=" << !!weakEditor
         << " native loaded=" << native.IsLoaded());
    REQUIRE(retired);
    try { toggle.Toggle(); }
    catch ( const winrt::hresult_error& error )
    {
        const auto code = static_cast<int32_t>(error.code());
        CHECK((code == static_cast<int32_t>(0x80040200u) || code == static_cast<int32_t>(0x80040201u)));
    }
    wxTheApp->ProcessPendingEvents();
    CHECK(changing == 2);
    CHECK(changed == 1);
}
#endif // wxUSE_CHECKBOX

TEST_CASE("WinUIPropertyGrid::NativePaintClearsOnlyInvalidatedLegacyBacking",
          "[winui-propgrid][native-propgrid][winui-propgrid-backing][paint]")
{
    const bool rtl = GENERATE(false, true);
    const bool backdrop = GENERATE(false, true);
    CAPTURE(rtl, backdrop);
    PropertyFixture fixture(rtl);
    auto* const grid = fixture.grid;
    grid->Append(new wxStringProperty("Native row", "nativeRow", "Native value"));
    fixture.Show();
    REQUIRE(WaitProperty([&]
    {
        const auto cell = PropertyCell(grid, "nativeRow", 1);
        return cell && cell.IsLoaded() && PropertyHasText(cell, "Native value");
    }));
    const auto cell = PropertyCell(grid, "nativeRow", 1);
    const auto text = PropertyText(cell, "Native value");
    REQUIRE(text);
    const HWND frameHwnd = GetHwndOf(fixture.frame.get());
    const HWND hwnd = GetHwndOf(grid);
    const HANDLE oldBackdrop = ::GetPropW(frameHwnd, L"wxWinUIBackdropTransparent");
    const HANDLE oldGridBackdrop = ::GetPropW(hwnd, L"wxWinUIBackdropTransparent");
    const auto restoreBackdrop = wxMakeGuard([frameHwnd, hwnd, oldBackdrop, oldGridBackdrop]
    {
        if ( oldBackdrop ) ::SetPropW(frameHwnd, L"wxWinUIBackdropTransparent", oldBackdrop);
        else ::RemovePropW(frameHwnd, L"wxWinUIBackdropTransparent");
        if ( oldGridBackdrop ) ::SetPropW(hwnd, L"wxWinUIBackdropTransparent", oldGridBackdrop);
        else ::RemovePropW(hwnd, L"wxWinUIBackdropTransparent");
    });
    if ( backdrop )
        REQUIRE(::SetPropW(frameHwnd, L"wxWinUIBackdropTransparent", reinterpret_cast<HANDLE>(1)));
    else
    {
        ::RemovePropW(frameHwnd, L"wxWinUIBackdropTransparent");
        ::RemovePropW(hwnd, L"wxWinUIBackdropTransparent");
    }
    REQUIRE(wxWinUIWindowUsesBackdrop(grid) == backdrop);
    grid->Refresh();
    grid->Update();
    REQUIRE_FALSE(::GetUpdateRect(hwnd, nullptr, FALSE));

    // Seed two visible HWND backing pixels below the last row, just like
    // legacy dividers remaining below transparent native cells. Read back
    // the positive control before asking the actual WM_PAINT path to clean.
    const int y = grid->GetClientSize().y - 12;
    REQUIRE(y > grid->GetRowHeight() * 2);
    const COLORREF legacy = RGB(213, 41, 189);
    HDC seedDC = ::GetDC(hwnd);
    REQUIRE(seedDC);
    const COLORREF firstSeed = ::SetPixel(seedDC, 10, y, legacy);
    const COLORREF secondSeed = ::SetPixel(seedDC, 42, y, legacy);
    ::ReleaseDC(hwnd, seedDC);
    REQUIRE(firstSeed != CLR_INVALID);
    REQUIRE(secondSeed != CLR_INVALID);
    const auto pixel = [&](int x)
    {
        HDC dc = ::GetDC(hwnd);
        REQUIRE(dc);
        const COLORREF result = ::GetPixel(dc, x, y);
        ::ReleaseDC(hwnd, dc);
        REQUIRE(result != CLR_INVALID);
        return result;
    };
    REQUIRE(pixel(10) == legacy);
    REQUIRE(pixel(42) == legacy);
    const wxColour background = grid->GetBackgroundColour();
    const COLORREF expected = backdrop ? RGB(0, 0, 0) :
        RGB(background.Red(), background.Green(), background.Blue());
    const RECT dirty{8, y - 2, 13, y + 3};
    REQUIRE(::InvalidateRect(hwnd, &dirty, FALSE));
    REQUIRE(::UpdateWindow(hwnd));
    CHECK(pixel(10) == expected);
    CHECK(pixel(42) == legacy); // PaintDC's exact update clip is respected.
    grid->Refresh();
    grid->Update();
    CHECK(pixel(42) == expected);
    CHECK(wxWinUIWindowUsesBackdrop(grid) == backdrop);
    CHECK(PropertyCell(grid, "nativeRow", 1) == cell);
    CHECK(PropertyText(cell, "Native value") == text);
    CHECK(text.Visibility() == X::Visibility::Collapsed);
    const auto preview = FindPropertyPart<C::TextBox>(cell, L"wxProperty.Preview.nativeRow");
    REQUIRE(preview);
    CHECK(PropertyPartVisible(preview));
    CHECK(preview.Text() == L"Native value");
}

#if wxUSE_TEXTCTRL
TEST_CASE("WinUIPropertyGrid::LiveEditorOwnsTextWithoutDuplicateNativeValue",
          "[winui-propgrid][native-propgrid][winui-propgrid-backing][native-editor]")
{
    PropertyFixture fixture;
    auto* const grid = fixture.grid;
    auto* const property = grid->Append(new wxStringProperty("Editable", "deduplicate", "original"));
    grid->Append(new wxStringProperty("Neighbour", "neighbour", "unchanged"));
    fixture.Show();
    C::TextBlock text{nullptr};
    C::TextBox preview{nullptr};
    REQUIRE(WaitProperty([&]
    {
        text = PropertyText(PropertyCell(grid, "deduplicate", 1), "original");
        preview = FindPropertyPart<C::TextBox>(PropertyCell(grid, "deduplicate", 1),
                                              L"wxProperty.Preview.deduplicate");
        return text && text.Visibility() == X::Visibility::Collapsed &&
               PropertyPartVisible(preview) && preview.Text() == L"original";
    }));
    const auto cell = PropertyCell(grid, "deduplicate", 1);
    const auto neighbour = PropertyCell(grid, "neighbour", 1);
    REQUIRE(grid->SelectProperty(property, true));
    REQUIRE(WaitProperty([&]
    {
        const auto editor = PropertyNativeEditor(grid);
        return editor && editor.IsLoaded() &&
               text.Visibility() == X::Visibility::Collapsed && !PropertyPartVisible(preview);
    }));
    REQUIRE(grid->GetEditorTextCtrl());
    grid->GetEditorTextCtrl()->SetValue("committed");
    grid->EditorsValueWasModified();
    REQUIRE(grid->CommitChangesFromEditor());
    CHECK(property->GetValue().GetString() == "committed");
    CHECK(text.Visibility() == X::Visibility::Collapsed);
    grid->ClearSelection();
    REQUIRE(WaitProperty([&]
    {
        return !grid->GetEditorControl() &&
               text.Visibility() == X::Visibility::Collapsed &&
               PropertyPartVisible(preview) && preview.Text() == L"committed";
    }));
    CHECK(PropertyCell(grid, "deduplicate", 1) == cell);
    CHECK(PropertyText(cell, "committed") == text);
    CHECK(FindPropertyPart<C::TextBox>(cell, L"wxProperty.Preview.deduplicate") == preview);
    CHECK(PropertyCell(grid, "neighbour", 1) == neighbour);
}
#endif // wxUSE_TEXTCTRL

TEST_CASE("WinUIPropertyGrid::NativeThemeRowsRetainCellsAndSelectionIndicator",
          "[winui-propgrid][native-propgrid][winui-propgrid-theme]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    PropertyFixture fixture(rtl);
    auto* const grid = fixture.grid;
    // No border in Win32: the frame of the table is drawn by XAML.
    CHECK((grid->GetWindowStyleFlag() & wxBORDER_MASK) == wxBORDER_NONE);
    CHECK(grid->GetClientSize() == grid->GetSize());
    grid->Append(new wxPropertyCategory("Appearance", "appearance"));
    auto* const property = grid->Append(new wxStringProperty("Label", "theme", "Native value"));
    fixture.Show();
    C::Button cell{nullptr}, category{nullptr};
    C::TextBlock text{nullptr};
    C::Border indicator{nullptr};
    REQUIRE(WaitProperty([&]
    {
        cell = PropertyCell(grid, "theme", 0);
        category = PropertyCell(grid, "appearance", 0);
        text = PropertyText(cell, "Label");
        indicator = FindPropertyPart<C::Border>(cell, L"wxProperty.Selection.theme");
        return cell && category && text && indicator && cell.IsLoaded() && category.IsLoaded();
    }));
    const auto unset = X::DependencyProperty::UnsetValue();
    CHECK(cell.ReadLocalValue(C::Control::BackgroundProperty()) == unset);
    CHECK(category.ReadLocalValue(C::Control::BackgroundProperty()) == unset);
    CHECK(text.ReadLocalValue(C::TextBlock::ForegroundProperty()) == unset);
    // The cell of a table of WinUI: the line of the table under it and,
    // after the name of the property, at its right; the fill of the row is
    // rounded where the row starts.
    CHECK(cell.CornerRadius().TopLeft > 0);
    CHECK(cell.CornerRadius().TopRight == 0);
    CHECK(cell.BorderThickness().Left == 0);
    CHECK(cell.BorderThickness().Bottom == 1);
    CHECK(cell.BorderThickness().Right == 1);
    CHECK(indicator.Visibility() == X::Visibility::Collapsed);
    const auto ordinaryStyle = cell.Style();
    REQUIRE(grid->SelectProperty(property));
    REQUIRE(WaitProperty([&]
    {
        // Visibility changes synchronously; the retained, formerly collapsed
        // Border acquires its real size in the following native layout pass.
        return indicator.Visibility() == X::Visibility::Visible && cell.Style() != ordinaryStyle &&
               indicator.IsLoaded() && indicator.ActualWidth() > 0 && indicator.ActualHeight() > 0;
    }));
    CHECK(cell.ReadLocalValue(C::Control::BackgroundProperty()) == unset);
    CHECK(text.ReadLocalValue(C::TextBlock::ForegroundProperty()) == unset);
    CHECK(indicator.ActualWidth() > 0);
    CHECK(indicator.ActualHeight() > 0);
    const auto root = PropertyRoot(grid).as<X::FrameworkElement>();
    root.RequestedTheme(X::ElementTheme::Light);
    REQUIRE(WaitProperty([&]
    {
        return cell.ActualTheme() == X::ElementTheme::Light &&
               cell.Background().try_as<X::Media::SolidColorBrush>();
    }));
    const auto light = cell.Background().as<X::Media::SolidColorBrush>().Color();
    root.RequestedTheme(X::ElementTheme::Dark);
    REQUIRE(WaitProperty([&]
    {
        return cell.ActualTheme() == X::ElementTheme::Dark &&
               cell.Background().try_as<X::Media::SolidColorBrush>();
    }));
    const auto dark = cell.Background().as<X::Media::SolidColorBrush>().Color();
    HIGHCONTRASTW highContrast{sizeof(HIGHCONTRASTW)};
    REQUIRE(::SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, 0));
    if ( !(highContrast.dwFlags & HCF_HIGHCONTRASTON) )
    {
        // SubtleFill is translucent and changes with the actual native theme;
        // neither state is the legacy opaque selection-blue rectangle.
        CHECK(light.A < 255);
        CHECK(dark.A < 255);
        CHECK((light.A != dark.A || light.R != dark.R || light.G != dark.G || light.B != dark.B));
    }
    fixture.frame->SetClientSize(wxSize(760, 420));
    grid->ClearSelection();
    REQUIRE(WaitProperty([&]
    {
        return indicator.Visibility() == X::Visibility::Collapsed && cell.Style() == ordinaryStyle;
    }));
    CHECK(PropertyCell(grid, "theme", 0) == cell);
    CHECK(PropertyCell(grid, "appearance", 0) == category);
    CHECK(PropertyText(cell, "Label") == text);
    CHECK(FindPropertyPart<C::Border>(cell, L"wxProperty.Selection.theme") == indicator);
    CHECK(cell.Content().as<C::Grid>().FlowDirection() ==
          (rtl ? X::FlowDirection::RightToLeft : X::FlowDirection::LeftToRight));
}

TEST_CASE("WinUIPropertyGrid::ExplicitPaletteOverridesNativeThemeWithoutRemount",
          "[winui-propgrid][native-propgrid][winui-propgrid-theme]")
{
    PropertyFixture fixture;
    auto* const grid = fixture.grid;
    grid->Append(new wxPropertyCategory("Appearance", "appearance"));
    auto* const property = grid->Append(new wxStringProperty("Label", "palette", "Value"));
    auto* const custom = grid->Append(new wxStringProperty("Custom", "customPalette", "Custom value"));
    const wxColour selectionBg(21, 42, 63), selectionFg(234, 213, 192);
    const wxColour captionBg(33, 44, 55), captionFg(155, 166, 177);
    const wxColour cellBg(61, 72, 83), cellFg(184, 195, 206);
    const wxColour customBg(93, 104, 115), customFg(216, 227, 238);
    grid->SetSelectionBackgroundColour(selectionBg);
    grid->SetSelectionTextColour(selectionFg);
    grid->SetCaptionBackgroundColour(captionBg);
    grid->SetCaptionTextColour(captionFg);
    grid->SetCellBackgroundColour(cellBg);
    grid->SetCellTextColour(cellFg);
    grid->SetPropertyCell(custom, 0, wxPG_LABEL, wxBitmapBundle(), customFg, customBg);
    fixture.Show();
    const auto matches = [](const X::Media::Brush& brush, const wxColour& colour)
    {
        const auto solid = brush.try_as<X::Media::SolidColorBrush>();
        if ( !solid ) return false;
        const auto value = solid.Color();
        return value.A == colour.Alpha() && value.R == colour.Red() &&
               value.G == colour.Green() && value.B == colour.Blue();
    };
    C::Button cell{nullptr}, category{nullptr}, customCell{nullptr};
    C::TextBlock text{nullptr}, categoryText{nullptr}, customText{nullptr};
    REQUIRE(WaitProperty([&]
    {
        cell = PropertyCell(grid, "palette", 0);
        category = PropertyCell(grid, "appearance", 0);
        customCell = PropertyCell(grid, "customPalette", 0);
        text = PropertyText(cell, "Label");
        categoryText = PropertyText(category, "Appearance");
        customText = PropertyText(customCell, "Custom");
        return cell && category && customCell && text && categoryText && customText &&
               matches(cell.Background(), cellBg) && matches(text.Foreground(), cellFg) &&
               matches(category.Background(), captionBg) && matches(categoryText.Foreground(), captionFg) &&
               matches(customCell.Background(), customBg) && matches(customText.Foreground(), customFg);
    }));
    REQUIRE(grid->SelectProperty(property));
    REQUIRE(WaitProperty([&]
    {
        return matches(cell.Background(), selectionBg) && matches(text.Foreground(), selectionFg);
    }));
    grid->ClearSelection();
    grid->ResetColours();
    const auto unset = X::DependencyProperty::UnsetValue();
    REQUIRE(WaitProperty([&]
    {
        return cell.ReadLocalValue(C::Control::BackgroundProperty()) == unset &&
               text.ReadLocalValue(C::TextBlock::ForegroundProperty()) == unset &&
               category.ReadLocalValue(C::Control::BackgroundProperty()) == unset &&
               categoryText.ReadLocalValue(C::TextBlock::ForegroundProperty()) == unset;
    }));
    CHECK(matches(customCell.Background(), customBg));
    CHECK(matches(customText.Foreground(), customFg));
    CHECK(PropertyCell(grid, "palette", 0) == cell);
    CHECK(PropertyCell(grid, "appearance", 0) == category);
    CHECK(PropertyCell(grid, "customPalette", 0) == customCell);
    CHECK(PropertyText(cell, "Label") == text);

    // A border asked for gives the frame of a control, which XAML draws
    // around the window: Win32 keeps none, so that the rows XAML shows and
    // the editors, placed in the client area, are at the same place. Asking
    // for no border gives no frame.
    wxPropertyGrid bordered(fixture.frame.get(), wxID_ANY, wxDefaultPosition,
                            wxSize(100, 50), wxBORDER_SIMPLE);
    CHECK((bordered.GetWindowStyleFlag() & wxBORDER_MASK) == wxBORDER_NONE);
    CHECK(bordered.GetClientSize() == bordered.GetSize());
    CHECK(wxWinUIWindowHasRequestedFrame(&bordered));
    CHECK(wxWinUIWindowHasRequestedFrame(grid));

    wxPropertyGrid plain(fixture.frame.get(), wxID_ANY, wxDefaultPosition,
                         wxSize(100, 50), wxBORDER_NONE);
    CHECK_FALSE(wxWinUIWindowHasRequestedFrame(&plain));
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_PROPGRID
