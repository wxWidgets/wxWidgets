/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuipropgridpreview.cpp
// Purpose:     Native PropertyGrid controls visible outside the active editor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_PROPGRID && \
    wxUSE_TEXTCTRL && wxUSE_COMBOBOX && wxUSE_CHECKBOX

#include "wx/app.h"
#include "wx/checkbox.h"
#include "wx/combobox.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/props.h"
#include "wx/propgrid/editors.h"
#include "wx/propgrid/advprops.h"
#include "wx/sizer.h"
#include "wx/scopeguard.h"
#include "wx/stopwatch.h"
#include "wx/textctrl.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#if wxUSE_SPINBTN || wxUSE_DATEPICKCTRL
#include <winrt/Windows.Globalization.NumberFormatting.h>
#endif
#if wxUSE_DATEPICKCTRL
#include "wx/datectrl.h"
#include <winrt/Windows.Globalization.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#endif
#include <cmath>
#include <memory>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation;

template<typename Predicate>
bool WaitPreview(Predicate predicate)
{
    auto* const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp ) return false;
    wxStopWatch elapsed;
    while ( elapsed.Time() < 1500 )
    {
        if ( predicate() ) return true;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop ) return false;
        if ( !loop->Pending() && !wxTheApp->HasPendingEvents() )
            wxTheApp->ProcessIdle();
        if ( wxEventLoopBase::GetActive() != loop ) return false;
        if ( predicate() ) return true;
        if ( loop->DispatchTimeout(5) == 0 ) return false;
    }
    return predicate();
}

template<typename T>
T FindPreviewPart(const X::DependencyObject& node, const winrt::hstring& id = {})
{
    if ( !node ) return nullptr;
    if ( const auto part = node.try_as<T>(); part &&
         (id.empty() || A::AutomationProperties::GetAutomationId(part) == id) )
        return part;
    for ( int n = 0; n < X::Media::VisualTreeHelper::GetChildrenCount(node); ++n )
        if ( const auto part = FindPreviewPart<T>(
                X::Media::VisualTreeHelper::GetChild(node, n), id) ) return part;
    return nullptr;
}

X::FrameworkElement WindowContent(wxWindow* window)
{
    auto* const host = window ? wxWinUITopLevelHost::FindSlotOwner(window) : nullptr;
    auto* const slot = host ? host->FindSlot(window) : nullptr;
    return slot ? slot->GetContent().try_as<X::FrameworkElement>() : nullptr;
}

template<typename T>
T Preview(wxPropertyGrid* grid, const wxString& name)
{
    return FindPreviewPart<T>(WindowContent(grid),
        winrt::hstring(("wxProperty.Preview." + name).wc_str()));
}

C::Button PreviewButton(wxPropertyGrid* grid, const wxString& name)
{
    return FindPreviewPart<C::Button>(WindowContent(grid),
        winrt::hstring(("wxProperty.PreviewButton." + name).wc_str()));
}

bool PreviewVisible(const X::FrameworkElement& part)
{
    if ( !part || !part.IsLoaded() || part.ActualWidth() <= 0 ||
         part.ActualHeight() <= 0 ) return false;
    X::DependencyObject node = part;
    while ( node )
    {
        if ( const auto visual = node.try_as<X::UIElement>(); visual &&
             (visual.Visibility() != X::Visibility::Visible || visual.Opacity() <= 0) )
            return false;
        node = X::Media::VisualTreeHelper::GetParent(node);
    }
    return true;
}

bool HasVisiblePreviewText(const X::DependencyObject& root, const wxString& value)
{
    if ( !root ) return false;
    if ( const auto text = root.try_as<C::TextBlock>(); text &&
         wxString(text.Text().c_str()) == value && PreviewVisible(text) ) return true;
    for ( int n = 0; n < X::Media::VisualTreeHelper::GetChildrenCount(root); ++n )
        if ( HasVisiblePreviewText(X::Media::VisualTreeHelper::GetChild(root, n), value) )
            return true;
    return false;
}

void CheckPresentationOnly(const C::Control& preview)
{
    REQUIRE(preview);
    CHECK(PreviewVisible(preview));
    CHECK_FALSE(preview.IsHitTestVisible());
    CHECK_FALSE(preview.IsTabStop());
    CHECK(A::AutomationProperties::GetAccessibilityView(preview) ==
          A::Peers::AccessibilityView::Raw);
}

wxRect PreviewScreenRect(wxPropertyGrid* grid, const X::FrameworkElement& part)
{
    auto* const host = wxWinUITopLevelHost::FindSlotOwner(grid);
    if ( !host || !part || !part.IsLoaded() ) return {};
    const auto bounds = part.TransformToVisual(host->Root()).TransformBounds(
        {0, 0, static_cast<float>(part.ActualWidth()), static_cast<float>(part.ActualHeight())});
    const auto origin = host->GetClientScreenRect();
    const double scale = host->GetScale();
    return wxRect(origin.left + static_cast<int>(std::lround(bounds.X * scale)),
                  origin.top + static_cast<int>(std::lround(bounds.Y * scale)),
                  static_cast<int>(std::lround(bounds.Width * scale)),
                  static_cast<int>(std::lround(bounds.Height * scale)));
}

struct PreviewFixture
{
    explicit PreviewFixture(bool rtl = false)
    {
        wxPoint position(-30000, -30000);
        wchar_t desktop[256]{};
        DWORD needed = 0;
        if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
                UOI_NAME, desktop, sizeof(desktop), &needed) &&
             wxString(desktop).StartsWith("wxWinUITest-") ) position = wxPoint(80, 80);
        frame = std::make_unique<wxFrame>(nullptr, wxID_ANY, "PropertyGrid native previews",
                                         position, wxSize(720, 560));
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
    ~PreviewFixture()
    {
        frame.reset();
        wxTheApp->ProcessPendingEvents();
        wxTheApp->ProcessIdle();
    }
    void Show()
    {
        frame->Layout();
        frame->ShowWithoutActivating();
        REQUIRE(WaitPreview([&]()
        {
            const auto root = WindowContent(grid);
            return root && root.IsLoaded() && root.ActualWidth() > 0;
        }));
    }
    std::unique_ptr<wxFrame> frame;
    wxPropertyGrid* grid = nullptr;
};

// An application editor may intentionally keep the built-in name while
// changing its controls. The preview must classify identity, not this name.
class CustomPreviewEditor final : public wxPGTextCtrlEditor
{
public:
    wxString GetName() const override { return "TextCtrl"; }
};
#if wxUSE_SPINBTN
class CustomNamedSpinEditor final : public wxPGTextCtrlEditor
{
public:
    wxString GetName() const override { return "SpinCtrl"; }
};

// A property's display contract may use a different decimal convention from
// the desktop locale (and need not be NumberBox-parsable). The native preview
// must keep that display, not parse and format its text a second time.
class CommaSpinProperty final : public wxFloatProperty
{
public:
    CommaSpinProperty() : wxFloatProperty("Localized spin", "localized", 12.375) {}
    wxString ValueToString(wxVariant& value, wxPGPropValFormatFlags flags) const override
    {
        wxString text = wxFloatProperty::ValueToString(value, flags);
        text.Replace(".", ",");
        return text + " units";
    }
};
#endif

#if wxUSE_SPINBTN || wxUSE_DATEPICKCTRL
bool SpinTextIs(const C::NumberBox& number, const wxString& expected)
{
    const auto text = FindPreviewPart<C::TextBox>(number);
    return PreviewVisible(number) && text && text.IsLoaded() &&
           wxString(text.Text().c_str()) == expected &&
           wxString(number.Text().c_str()) == expected;
}
#endif
#if wxUSE_DATEPICKCTRL
class CustomNamedDateEditor final : public wxPGTextCtrlEditor
{
public:
    wxString GetName() const override { return "DatePickerCtrl"; }
};

C::TextBlock DatePreviewText(const C::CalendarDatePicker& picker)
{
    const auto control = picker ? picker.try_as<C::IControlProtected>() : nullptr;
    return control ? control.GetTemplateChild(L"DateText").try_as<C::TextBlock>() : nullptr;
}

bool DatePreviewMatches(const C::CalendarDatePicker& picker,
                        const wxDateTime& expected, const wxString& displayed)
{
    const auto text = DatePreviewText(picker);
    if ( !PreviewVisible(picker) || !text || !text.IsLoaded() ||
         wxString(text.Text().c_str()) != displayed ) return false;
    const auto value = picker.Date();
    if ( !expected.IsValid() ) return !value;
    if ( !value ) return false;
    winrt::Windows::Globalization::Calendar calendar;
    calendar.ChangeCalendarSystem(winrt::Windows::Globalization::CalendarIdentifiers::Gregorian());
    calendar.ChangeClock(winrt::Windows::Globalization::ClockIdentifiers::TwentyFourHour());
    calendar.SetDateTime(value.Value());
    return calendar.Year() == expected.GetYear() &&
           calendar.Month() == expected.GetMonth() + 1 &&
           calendar.Day() == expected.GetDay() && calendar.Hour() == 12;
}

wxDateProperty* AppendDropdownDate(wxPropertyGrid* grid, const wxString& name,
                                   const wxDateTime& value)
{
    auto* const property = new wxDateProperty(name, name, value);
    grid->Append(property);
    grid->SetPropertyAttribute(property, wxPG_DATE_PICKER_STYLE,
                               wxDP_DROPDOWN | wxDP_SHOWCENTURY | wxDP_ALLOWNONE);
    return property;
}

wxDateProperty* AppendSpinDate(wxPropertyGrid* grid, const wxString& name,
                               const wxDateTime& value)
{
    auto* const property = new wxDateProperty(name, name, value);
    grid->Append(property);
    grid->SetPropertyAttribute(property, wxPG_DATE_PICKER_STYLE,
                               wxDP_SPIN | wxDP_SHOWCENTURY | wxDP_ALLOWNONE);
    return property;
}

template<typename T = C::NumberBox>
T DateSpinPart(const C::ContentControl& root, const wxString& name, const wxString& part)
{
    return FindPreviewPart<T>(root,
        winrt::hstring(("wxProperty.DateSpin." + name + "." + part).wc_str()));
}

bool DateSpinMatches(const C::ContentControl& root, const wxString& name,
                     const wxDateTime& value)
{
    if ( !PreviewVisible(root) ) return false;
    const auto day = DateSpinPart(root, name, "Day");
    const auto month = DateSpinPart(root, name, "Month");
    const auto year = DateSpinPart(root, name, "Year");
    if ( !PreviewVisible(day) || !PreviewVisible(month) || !PreviewVisible(year) ) return false;
    if ( !value.IsValid() )
        return std::isnan(day.Value()) && std::isnan(month.Value()) && std::isnan(year.Value());
    // Independent native calendar oracle, not the production shared helpers.
    namespace G = winrt::Windows::Globalization;
    G::Calendar calendar;
    calendar.ChangeCalendarSystem(G::CalendarIdentifiers::Gregorian());
    calendar.Day(1);
    calendar.Month(1);
    calendar.Year(value.GetYear());
    calendar.Month(value.GetMonth() + 1);
    calendar.Day(value.GetDay());
    const C::CalendarDatePicker defaults;
    calendar.ChangeCalendarSystem(defaults.CalendarIdentifier());
    return day.Value() == calendar.Day() && month.Value() == calendar.Month() &&
           year.Value() == calendar.Year();
}

C::NumberBox FindDateEditorPart(const X::DependencyObject& node, double value)
{
    if ( !node ) return nullptr;
    if ( const auto number = node.try_as<C::NumberBox>(); number && number.Value() == value )
        return number;
    for ( int n = 0; n < X::Media::VisualTreeHelper::GetChildrenCount(node); ++n )
        if ( const auto part = FindDateEditorPart(X::Media::VisualTreeHelper::GetChild(node, n), value) )
            return part;
    return nullptr;
}
#endif
}

TEST_CASE("WinUIPropertyGrid::NativeControlsRemainVisibleOutsideSelection",
          "[winui-propgrid][native-propgrid][native-propgrid-preview]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    unsigned changing = 0, changed = 0;
    PreviewFixture fixture(rtl);
    auto* const grid = fixture.grid;
    auto* const text = grid->Append(new wxStringProperty("Text", "text", "original"));
    grid->Append(new wxIntProperty("Number", "number", 42));
    auto* const boolean = grid->Append(new wxBoolProperty("Enabled", "boolean", false));
    grid->SetPropertyAttribute(boolean, wxPG_BOOL_USE_CHECKBOX, true);
    wxArrayString labels;
    labels.Add("First"); labels.Add("Second"); labels.Add("Third");
    auto* const choice = grid->Append(new wxEnumProperty("Choice", "choice", labels,
                                                        wxArrayInt(), 1));
    grid->Append(new wxEditEnumProperty("Editable choice", "editchoice", labels,
                                        wxArrayInt(), "Free text"));
    auto* const textButton = grid->Append(new wxStringProperty("Text and button", "textbutton", "path"));
    grid->SetPropertyEditor(textButton, wxPGEditor_TextCtrlAndButton);
    auto* const choiceButton = grid->Append(new wxEnumProperty("Choice and button", "choicebutton", labels,
                                                              wxArrayInt(), 0));
    grid->SetPropertyEditor(choiceButton, wxPGEditor_ChoiceAndButton);
    auto* const disabled = grid->Append(new wxStringProperty("Disabled", "disabled", "still visible"));
    grid->EnableProperty(disabled, false);
    grid->Bind(wxEVT_PG_CHANGING, [&](wxPropertyGridEvent&) { ++changing; });
    grid->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent&) { ++changed; });
    fixture.Show();
    REQUIRE_FALSE(grid->GetSelection());
    REQUIRE_FALSE(grid->GetEditorControl());
    REQUIRE(WaitPreview([&]()
    {
        return PreviewVisible(Preview<C::TextBox>(grid, "text")) &&
               PreviewVisible(Preview<C::TextBox>(grid, "number")) &&
               PreviewVisible(Preview<C::CheckBox>(grid, "boolean")) &&
               PreviewVisible(Preview<C::ComboBox>(grid, "choice")) &&
               PreviewVisible(Preview<C::ComboBox>(grid, "editchoice")) &&
               PreviewVisible(Preview<C::TextBox>(grid, "textbutton")) &&
               PreviewVisible(PreviewButton(grid, "textbutton")) &&
               PreviewVisible(Preview<C::ComboBox>(grid, "choicebutton")) &&
               PreviewVisible(PreviewButton(grid, "choicebutton")) &&
               PreviewVisible(Preview<C::TextBox>(grid, "disabled"));
    }));
    const auto nativeText = Preview<C::TextBox>(grid, "text");
    const auto nativeNumber = Preview<C::TextBox>(grid, "number");
    const auto nativeBool = Preview<C::CheckBox>(grid, "boolean");
    const auto nativeChoice = Preview<C::ComboBox>(grid, "choice");
    const auto nativeEditChoice = Preview<C::ComboBox>(grid, "editchoice");
    for ( const C::Control control : {nativeText.as<C::Control>(), nativeNumber.as<C::Control>(),
            nativeBool.as<C::Control>(), nativeChoice.as<C::Control>(), nativeEditChoice.as<C::Control>(),
            Preview<C::TextBox>(grid, "textbutton").as<C::Control>(),
            PreviewButton(grid, "textbutton").as<C::Control>(),
            Preview<C::ComboBox>(grid, "choicebutton").as<C::Control>(),
            PreviewButton(grid, "choicebutton").as<C::Control>()} )
        CheckPresentationOnly(control);
    CHECK(wxString(nativeText.Text().c_str()) == "original");
    CHECK(wxString(nativeNumber.Text().c_str()) == "42");
    REQUIRE(nativeBool.IsChecked());
    CHECK_FALSE(nativeBool.IsChecked().Value());
    // A passive preview shows the formatted current value only. The real
    // editor, created on selection, owns the full mutable choice model.
    REQUIRE(nativeChoice.SelectedItem());
    CHECK(wxString(winrt::unbox_value<winrt::hstring>(nativeChoice.SelectedItem()).c_str()) == "Second");
    CHECK(nativeEditChoice.IsEditable());
    CHECK(wxString(nativeEditChoice.Text().c_str()) == "Free text");
    CHECK_FALSE(Preview<C::TextBox>(grid, "disabled").IsEnabled());

    grid->SetPropertyValue(text, "updated");
    grid->SetPropertyValue("number", 73L);
    grid->SetPropertyValue(boolean, true);
    grid->SetPropertyValue(choice, 2L);
    REQUIRE(WaitPreview([&]()
    {
        return wxString(nativeText.Text().c_str()) == "updated" &&
               wxString(nativeNumber.Text().c_str()) == "73" &&
               nativeBool.IsChecked() && nativeBool.IsChecked().Value() &&
               nativeChoice.SelectedItem() &&
               wxString(winrt::unbox_value<winrt::hstring>(nativeChoice.SelectedItem()).c_str()) == "Third";
    }));
    CHECK(Preview<C::TextBox>(grid, "text") == nativeText);
    CHECK(Preview<C::ComboBox>(grid, "choice") == nativeChoice);
    grid->SetPropertyValueUnspecified(boolean);
    REQUIRE(WaitPreview([&]() { return !nativeBool.IsChecked(); }));
    CHECK(nativeBool.IsThreeState());
    CHECK(changing == 0);
    CHECK(changed == 0);
    CHECK_FALSE(grid->GetSelection());

    const wxRect oldRect = PreviewScreenRect(grid, nativeText);
    REQUIRE(!oldRect.IsEmpty());
    fixture.frame->SetClientSize(900, 560);
    REQUIRE(WaitPreview([&]()
    {
        const wxRect rect = PreviewScreenRect(grid, nativeText);
        return PreviewVisible(nativeText) && rect.width > oldRect.width;
    }));
    CHECK(Preview<C::TextBox>(grid, "text") == nativeText);
    CHECK(changing == 0);
    CHECK(changed == 0);
}

TEST_CASE("WinUIPropertyGrid::PreviewYieldsToOneRealEditorAndReturns",
          "[winui-propgrid][native-propgrid][native-propgrid-preview]")
{
    const bool rtl = GENERATE(false, true);
    const int kind = GENERATE(0, 1, 2);
    CAPTURE(rtl, kind);
    PreviewFixture fixture(rtl);
    auto* const grid = fixture.grid;
    wxPGProperty* property = nullptr;
    if ( kind == 0 ) property = grid->Append(new wxStringProperty("Value", "value", "original"));
    else if ( kind == 1 )
    {
        property = grid->Append(new wxBoolProperty("Value", "value", true));
        grid->SetPropertyAttribute(property, wxPG_BOOL_USE_CHECKBOX, true);
    }
    else
    {
        wxArrayString labels;
        labels.Add("First"); labels.Add("Second");
        property = grid->Append(new wxEnumProperty("Value", "value", labels, wxArrayInt(), 1));
    }
    fixture.Show();
    C::Control preview{nullptr};
    REQUIRE(WaitPreview([&]()
    {
        preview = Preview<C::Control>(grid, "value");
        return PreviewVisible(preview);
    }));
    REQUIRE(grid->SelectProperty(property));
    REQUIRE(WaitPreview([&]()
    {
        auto* const editor = grid->GetEditorControl();
        const auto content = WindowContent(editor);
        if ( !content || !content.IsLoaded() || !PreviewVisible(content) ) return false;
        if ( kind == 0 ) return !!FindPreviewPart<C::RichEditBox>(content);
        if ( kind == 1 ) return !!FindPreviewPart<C::CheckBox>(content);
        return !!FindPreviewPart<C::ComboBox>(content);
    }));
    CHECK_FALSE(PreviewVisible(preview));
    if ( kind == 0 ) CHECK(wxDynamicCast(grid->GetEditorControl(), wxTextCtrl));
    if ( kind == 1 ) CHECK(wxDynamicCast(grid->GetEditorControl(), wxCheckBox));
    if ( kind == 2 ) CHECK(wxDynamicCast(grid->GetEditorControl(), wxComboBox));
    REQUIRE(grid->ClearSelection());
    REQUIRE(WaitPreview([&]()
    {
        return !grid->GetEditorControl() && PreviewVisible(preview);
    }));
    CHECK(Preview<C::Control>(grid, "value") == preview);
    CheckPresentationOnly(preview);
}

TEST_CASE("WinUIPropertyGrid::CustomEditorDoesNotAcquireGuessedPreview",
          "[winui-propgrid][native-propgrid][native-propgrid-preview]")
{
    static CustomPreviewEditor custom;
    PreviewFixture fixture;
    auto* const grid = fixture.grid;
    auto* const property = grid->Append(new wxStringProperty("Custom", "custom", "custom value"));
    grid->SetPropertyEditor(property, &custom);
    grid->Append(new wxStringProperty("Standard", "standard", "ordinary value"));
    fixture.Show();
    REQUIRE(WaitPreview([&]() { return PreviewVisible(Preview<C::TextBox>(grid, "standard")); }));
    const auto cell = FindPreviewPart<C::Button>(WindowContent(grid), L"wxProperty.Cell.custom.1");
    REQUIRE(cell);
    CHECK(PreviewVisible(cell));
    CHECK_FALSE(Preview<C::Control>(grid, "custom"));
    CHECK_FALSE(PreviewButton(grid, "custom"));
    REQUIRE(grid->SelectProperty(property));
    REQUIRE(WaitPreview([&]()
    {
        const auto editor = FindPreviewPart<C::RichEditBox>(WindowContent(grid->GetEditorControl()));
        return editor && editor.IsLoaded();
    }));
    CHECK(wxDynamicCast(grid->GetEditorControl(), wxTextCtrl));
    REQUIRE(grid->ClearSelection());
    REQUIRE(WaitPreview([&]() { return !grid->GetEditorControl(); }));
    CHECK_FALSE(Preview<C::Control>(grid, "custom"));
}

TEST_CASE("WinUIPropertyGrid::PreviewPreservesPropertyDisplayContracts",
          "[winui-propgrid][native-propgrid][native-propgrid-preview]")
{
    unsigned changed = 0;
    PreviewFixture fixture;
    auto* const grid = fixture.grid;
    auto* const dialog = new wxStringProperty("Dialog value", "dialog", "kept beside button");
    dialog->ChangeFlag(wxPGFlags::NoEditor, true);
    grid->Append(dialog);
    grid->SetPropertyEditor(dialog, wxPGEditor_TextCtrlAndButton);
    wxArrayString labels;
    labels.Add("Off"); labels.Add("On");
    wxArrayInt values;
    values.Add(10); values.Add(20);
    auto* const enumeration = grid->Append(new wxEnumProperty("Check enum", "enumcheck",
                                                              labels, values, 10));
    grid->SetPropertyEditor(enumeration, wxPGEditor_CheckBox);
    auto* const coloured = grid->Append(new wxStringProperty("Coloured", "coloured", "colour contract"));
    const wxColour foreground(193, 37, 71);
    const wxColour background(23, 67, 109);
    grid->SetPropertyTextColour(coloured, foreground);
    grid->SetPropertyBackgroundColour(coloured, background);
    const wxString secret = "not displayed in clear";
    auto* const password = grid->Append(new wxStringProperty("Password", "password", secret));
    grid->SetPropertyAttribute(password, wxPG_STRING_PASSWORD, true);
    grid->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent&) { ++changed; });
    fixture.Show();
    REQUIRE(WaitPreview([&]()
    {
        return PreviewVisible(PreviewButton(grid, "dialog")) &&
               PreviewVisible(Preview<C::CheckBox>(grid, "enumcheck")) &&
               PreviewVisible(Preview<C::TextBox>(grid, "coloured")) &&
               PreviewVisible(Preview<C::TextBox>(grid, "password"));
    }));
    const auto dialogCell = FindPreviewPart<C::Button>(WindowContent(grid), L"wxProperty.Cell.dialog.1");
    REQUIRE(dialogCell);
    CHECK_FALSE(Preview<C::TextBox>(grid, "dialog"));
    CHECK(HasVisiblePreviewText(dialogCell, "kept beside button"));
    CheckPresentationOnly(PreviewButton(grid, "dialog"));

    const auto check = Preview<C::CheckBox>(grid, "enumcheck");
    REQUIRE(check.IsChecked());
    CHECK_FALSE(check.IsChecked().Value());
    CHECK(enumeration->GetValue().GetLong() == 10);
    grid->SetPropertyValue(enumeration, 20L);
    REQUIRE(WaitPreview([&]() { return check.IsChecked() && check.IsChecked().Value(); }));
    CHECK(enumeration->GetValue().GetLong() == 20);
    grid->SetPropertyValueUnspecified(enumeration);
    REQUIRE(WaitPreview([&]() { return !check.IsChecked(); }));

    const auto nativeColour = Preview<C::TextBox>(grid, "coloured");
    const auto nativeForeground = nativeColour.Foreground().try_as<X::Media::SolidColorBrush>();
    const auto nativeBackground = nativeColour.Background().try_as<X::Media::SolidColorBrush>();
    REQUIRE(nativeForeground);
    REQUIRE(nativeBackground);
    const auto fg = nativeForeground.Color();
    const auto bg = nativeBackground.Color();
    CHECK(fg.A == 255); CHECK(fg.R == foreground.Red());
    CHECK(fg.G == foreground.Green()); CHECK(fg.B == foreground.Blue());
    CHECK(bg.A == 255); CHECK(bg.R == background.Red());
    CHECK(bg.G == background.Green()); CHECK(bg.B == background.Blue());

    const auto nativePassword = Preview<C::TextBox>(grid, "password");
    const wxString displayed(nativePassword.Text().c_str());
    CHECK(displayed == password->GetValueAsString());
    CHECK_FALSE(displayed.empty());
    CHECK(displayed != secret);
    CHECK(password->GetValue().GetString() == secret);
    CHECK(changed == 0);
    CHECK_FALSE(grid->GetSelection());
}

#if wxUSE_SPINBTN
TEST_CASE("WinUIPropertyGrid::SpinPreviewPreservesExactNumbersAndDisplay",
          "[winui-propgrid][native-propgrid][native-propgrid-preview][native-propgrid-preview-spin]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    wxPropertyGrid::RegisterAdditionalEditors();
    REQUIRE(wxPGEditor_SpinCtrl);
    unsigned changed = 0;
    PreviewFixture fixture(rtl);
    auto* const grid = fixture.grid;
    const wxLongLong exactSigned(9007199254740993LL);
    const wxULongLong exactUnsigned(18446744073709551615ULL);
    auto* const signedProperty = grid->Append(new wxIntProperty("Signed spin", "signed", exactSigned));
    auto* const unsignedProperty = grid->Append(new wxUIntProperty("Unsigned spin", "unsigned", exactUnsigned));
    auto* const realProperty = grid->Append(new wxFloatProperty("Real spin", "real", 23.625));
    auto* const localizedProperty = grid->Append(new CommaSpinProperty);
    for ( auto* property : {signedProperty, unsignedProperty, realProperty, localizedProperty} )
        grid->SetPropertyEditor(property, wxPGEditor_SpinCtrl);
    grid->SetPropertyAttribute(realProperty, wxPG_FLOAT_PRECISION, 3L);
    grid->SetPropertyAttribute(realProperty, wxPG_ATTR_MIN, 0.0);
    grid->SetPropertyAttribute(realProperty, wxPG_ATTR_MAX, 100.0);
    grid->SetPropertyAttribute(realProperty, wxPG_ATTR_SPINCTRL_STEP, 0.125);
    grid->SetPropertyAttribute(realProperty, wxPG_ATTR_SPINCTRL_WRAP, true);
    grid->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent&) { ++changed; });
    fixture.Show();
    C::NumberBox signedNumber{nullptr}, unsignedNumber{nullptr}, realNumber{nullptr}, localizedNumber{nullptr};
    REQUIRE(WaitPreview([&]()
    {
        signedNumber = Preview<C::NumberBox>(grid, "signed");
        unsignedNumber = Preview<C::NumberBox>(grid, "unsigned");
        realNumber = Preview<C::NumberBox>(grid, "real");
        localizedNumber = Preview<C::NumberBox>(grid, "localized");
        return SpinTextIs(signedNumber, signedProperty->GetValueAsString()) &&
               SpinTextIs(unsignedNumber, unsignedProperty->GetValueAsString()) &&
               SpinTextIs(realNumber, realProperty->GetValueAsString()) &&
               SpinTextIs(localizedNumber, localizedProperty->GetValueAsString());
    }));
    for ( const auto& number : {signedNumber, unsignedNumber, realNumber, localizedNumber} )
    {
        CheckPresentationOnly(number);
        CHECK(number.SpinButtonPlacementMode() == C::NumberBoxSpinButtonPlacementMode::Inline);
    }
    CHECK(wxString(signedNumber.Text().c_str()) == "9007199254740993");
    CHECK(wxString(unsignedNumber.Text().c_str()) == "18446744073709551615");
    CHECK(wxString(localizedNumber.Text().c_str()).Contains(","));
    CHECK(realNumber.Value() == 23.625);
    CHECK(realNumber.Minimum() == 0);
    CHECK(realNumber.Maximum() == 100);
    CHECK(realNumber.SmallChange() == 0.125);
    CHECK(realNumber.IsWrapEnabled());
    const auto signedParser = signedNumber.NumberFormatter().as<winrt::Windows::Globalization::NumberFormatting::INumberParser>();
    const auto unsignedParser = unsignedNumber.NumberFormatter().as<winrt::Windows::Globalization::NumberFormatting::INumberParser>();
    REQUIRE(signedParser.ParseInt(signedNumber.Text()));
    REQUIRE(unsignedParser.ParseUInt(unsignedNumber.Text()));
    CHECK(signedParser.ParseInt(signedNumber.Text()).Value() == exactSigned.GetValue());
    CHECK(unsignedParser.ParseUInt(unsignedNumber.Text()).Value() == exactUnsigned.GetValue());
    const auto formatter = signedNumber.NumberFormatter();
    grid->RefreshProperty(signedProperty);
    fixture.frame->SetClientSize(860, 560);
    REQUIRE(WaitPreview([&]() { return SpinTextIs(signedNumber, "9007199254740993"); }));
    CHECK(Preview<C::NumberBox>(grid, "signed") == signedNumber);
    CHECK(signedNumber.NumberFormatter() == formatter);
    grid->SetPropertyValue(signedProperty, wxVariant(wxLongLong(9007199254740995LL)));
    grid->SetPropertyValue(realProperty, 97.125);
    REQUIRE(WaitPreview([&]()
    {
        return SpinTextIs(signedNumber, "9007199254740995") &&
               SpinTextIs(realNumber, realProperty->GetValueAsString()) && realNumber.Value() == 97.125;
    }));
    CHECK(Preview<C::NumberBox>(grid, "signed") == signedNumber);
    CHECK(Preview<C::NumberBox>(grid, "real") == realNumber);
    CHECK(changed == 0);
    CHECK_FALSE(grid->GetSelection());
}

TEST_CASE("WinUIPropertyGrid::SpinPreviewPreservesEditorAuthorityAndStates",
          "[winui-propgrid][native-propgrid][native-propgrid-preview][native-propgrid-preview-spin]")
{
    wxPropertyGrid::RegisterAdditionalEditors();
    static CustomNamedSpinEditor custom;
    unsigned changed = 0;
    PreviewFixture fixture;
    auto* const grid = fixture.grid;
    auto* const property = grid->Append(new wxIntProperty("Spin", "spin", 24L));
    grid->SetPropertyEditor(property, wxPGEditor_SpinCtrl);
    auto* const hidden = grid->Append(new wxIntProperty("No editor", "noeditor", 42L));
    grid->SetPropertyEditor(hidden, wxPGEditor_SpinCtrl);
    hidden->ChangeFlag(wxPGFlags::NoEditor, true);
    auto* const customProperty = grid->Append(new wxIntProperty("Custom", "customspin", 31L));
    grid->SetPropertyEditor(customProperty, &custom);
    grid->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent&) { ++changed; });
    fixture.Show();
    C::NumberBox number{nullptr};
    REQUIRE(WaitPreview([&]()
    {
        number = Preview<C::NumberBox>(grid, "spin");
        return SpinTextIs(number, "24");
    }));
    CHECK_FALSE(Preview<C::Control>(grid, "noeditor"));
    CHECK_FALSE(Preview<C::Control>(grid, "customspin"));
    grid->SetPropertyReadOnly(property);
    REQUIRE(WaitPreview([&]() { return !number.IsEnabled(); }));
    grid->SetPropertyReadOnly(property, false);
    REQUIRE(WaitPreview([&]() { return number.IsEnabled(); }));
    grid->EnableProperty(property, false);
    REQUIRE(WaitPreview([&]() { return !number.IsEnabled(); }));
    grid->EnableProperty(property, true);
    REQUIRE(WaitPreview([&]() { return number.IsEnabled(); }));
    grid->SetPropertyValueUnspecified(property);
    REQUIRE(WaitPreview([&]() { return std::isnan(number.Value()) && number.Text().empty(); }));
    CHECK(wxString(number.PlaceholderText().c_str()) == property->GetValueAsString());
    grid->SetPropertyValue(property, 53L);
    REQUIRE(WaitPreview([&]() { return SpinTextIs(number, "53"); }));
    REQUIRE(grid->SelectProperty(property));
    REQUIRE(WaitPreview([&]()
    {
        const auto editor = FindPreviewPart<C::RichEditBox>(WindowContent(grid->GetEditorControl()));
        return editor && editor.IsLoaded() && !PreviewVisible(number);
    }));
    CHECK(wxDynamicCast(grid->GetEditorControl(), wxTextCtrl));
    REQUIRE(grid->ClearSelection());
    REQUIRE(WaitPreview([&]() { return !grid->GetEditorControl() && SpinTextIs(number, "53"); }));
    CHECK(Preview<C::NumberBox>(grid, "spin") == number);
    CheckPresentationOnly(number);
    CHECK(changed == 0);
}

TEST_CASE("WinUIPropertyGrid::SpinPreviewReentrantNativePublicationRetiresSafely",
          "[winui-propgrid][native-propgrid][native-propgrid-preview][native-propgrid-preview-spin]")
{
    const int action = GENERATE(0, 1, 2);
    CAPTURE(action);
    wxPropertyGrid::RegisterAdditionalEditors();
    PreviewFixture fixture;
    auto* const grid = fixture.grid;
    wxWeakRef<wxPropertyGrid> weakGrid(grid);
    auto* const property = grid->Append(new wxIntProperty("Spin", "spin", 24L));
    grid->SetPropertyEditor(property, wxPGEditor_SpinCtrl);
    fixture.Show();
    C::NumberBox number{nullptr};
    REQUIRE(WaitPreview([&]()
    {
        number = Preview<C::NumberBox>(grid, "spin");
        return SpinTextIs(number, "24");
    }));
    bool armed = true;
    unsigned calls = 0;
    const auto token = number.RegisterPropertyChangedCallback(C::NumberBox::ValueProperty(),
        [&](const X::DependencyObject&, const X::DependencyProperty&)
        {
            if ( !armed ) return;
            armed = false;
            ++calls;
            if ( action == 0 ) grid->DeleteProperty(property);
            else if ( action == 1 ) grid->Clear();
            else
            {
                fixture.grid = nullptr;
                fixture.frame.reset();
            }
        });
    const auto unhook = wxMakeGuard([&]()
        { number.UnregisterPropertyChangedCallback(C::NumberBox::ValueProperty(), token); });
    grid->SetPropertyValue(property, 67L);
    REQUIRE(WaitPreview([&]() { return calls == 1 && !number.IsLoaded(); }));
    if ( action == 2 ) CHECK_FALSE(weakGrid);
    else
    {
        REQUIRE(weakGrid);
        CHECK_FALSE(grid->GetPropertyByName("spin"));
        auto* const replacement = grid->Append(new wxIntProperty("Replacement", "spin", 89L));
        grid->SetPropertyEditor(replacement, wxPGEditor_SpinCtrl);
        C::NumberBox current{nullptr};
        REQUIRE(WaitPreview([&]()
        {
            current = Preview<C::NumberBox>(grid, "spin");
            return SpinTextIs(current, "89");
        }));
        CHECK(current != number);
        number.Value(12);
        CHECK(replacement->GetValue().GetLong() == 89);
        CHECK(SpinTextIs(current, "89"));
    }
    CHECK(calls == 1);
}
#endif // wxUSE_SPINBTN

#if wxUSE_DATEPICKCTRL
TEST_CASE("WinUIPropertyGrid::DateDropdownPreviewPreservesCivilAndDisplayContracts",
          "[winui-propgrid][native-propgrid][native-propgrid-preview][native-propgrid-preview-date]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    unsigned changed = 0;
    PreviewFixture fixture(rtl);
    auto* const grid = fixture.grid;
    const wxDateTime ancient(9, wxDateTime::Sep, 64, 3, 14, 15);
    const wxDateTime contemporary(29, wxDateTime::Feb, 2024, 21, 34, 56);
    auto* const formatted = AppendDropdownDate(grid, "formatted", ancient);
    auto* const localized = AppendDropdownDate(grid, "localized", contemporary);
    grid->SetPropertyAttribute(formatted, wxPG_DATE_FORMAT, "Epoch {%A} %d/%m/%Y at %H:%M, 100%%");
    grid->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent&) { ++changed; });
    fixture.Show();
    C::CalendarDatePicker custom{nullptr}, locale{nullptr};
    REQUIRE(WaitPreview([&]()
    {
        custom = Preview<C::CalendarDatePicker>(grid, "formatted");
        locale = Preview<C::CalendarDatePicker>(grid, "localized");
        return DatePreviewMatches(custom, ancient, formatted->GetValueAsString()) &&
               DatePreviewMatches(locale, contemporary, localized->GetValueAsString());
    }));
    CheckPresentationOnly(custom);
    CheckPresentationOnly(locale);
    CHECK(formatted->GetValue().GetDateTime() == ancient);
    CHECK(localized->GetValue().GetDateTime() == contemporary);
    CHECK(wxString(DatePreviewText(custom).Text().c_str()).Contains("03:14"));
    const auto nativeTemplate = custom.Template();
    const auto nativeText = DatePreviewText(custom);
    const auto glyph = custom.as<C::IControlProtected>().GetTemplateChild(L"CalendarGlyph");
    REQUIRE(nativeTemplate);
    REQUIRE(glyph.try_as<C::FontIcon>());
    grid->RefreshProperty(formatted);
    fixture.frame->SetClientSize(850, 560);
    REQUIRE(WaitPreview([&]() { return DatePreviewMatches(custom, ancient, formatted->GetValueAsString()); }));
    CHECK(Preview<C::CalendarDatePicker>(grid, "formatted") == custom);
    CHECK(DatePreviewText(custom) == nativeText);
    CHECK(custom.Template() == nativeTemplate);
    CHECK(custom.as<C::IControlProtected>().GetTemplateChild(L"CalendarGlyph") == glyph);
    grid->SetPropertyAttribute(formatted, wxPG_DATE_FORMAT, "literal braces {} | %B %d, %Y | %H:%M");
    grid->SetPropertyValue(formatted, contemporary);
    REQUIRE(WaitPreview([&]() { return DatePreviewMatches(custom, contemporary, formatted->GetValueAsString()); }));
    CHECK(Preview<C::CalendarDatePicker>(grid, "formatted") == custom);
    // A real native template reapplication must refresh the exact DateText
    // part, not keep the old text element alive or lose the wx display format.
    const auto intermediate = X::Markup::XamlReader::Load(LR"xaml(
<ControlTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
 xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml" TargetType="CalendarDatePicker">
 <Grid x:Name="Root"><TextBlock x:Name="DateText" /></Grid>
</ControlTemplate>)xaml").as<C::ControlTemplate>();
    custom.Template(intermediate);
    custom.ApplyTemplate();
    custom.Template(nativeTemplate);
    custom.ApplyTemplate();
    REQUIRE(WaitPreview([&]() { return DatePreviewMatches(custom, contemporary, formatted->GetValueAsString()); }));
    CHECK(DatePreviewText(custom) != nativeText);
    CHECK(custom.Template() == nativeTemplate);
    CHECK(custom.as<C::IControlProtected>().GetTemplateChild(L"CalendarGlyph").try_as<C::FontIcon>());
    CHECK(changed == 0);
    CHECK_FALSE(grid->GetSelection());
}

TEST_CASE("WinUIPropertyGrid::DateDropdownPreviewKeepsEditorAuthorityAndStates",
          "[winui-propgrid][native-propgrid][native-propgrid-preview][native-propgrid-preview-date]")
{
    static CustomNamedDateEditor customEditor;
    unsigned changed = 0;
    PreviewFixture fixture;
    auto* const grid = fixture.grid;
    const wxDateTime value(17, wxDateTime::Nov, 2025);
    auto* const property = AppendDropdownDate(grid, "date", value);
    auto* const hidden = AppendDropdownDate(grid, "noeditor", value);
    hidden->ChangeFlag(wxPGFlags::NoEditor, true);
    auto* const custom = AppendDropdownDate(grid, "customdate", value);
    grid->SetPropertyEditor(custom, &customEditor);
    grid->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent&) { ++changed; });
    fixture.Show();
    C::CalendarDatePicker picker{nullptr};
    REQUIRE(WaitPreview([&]()
    {
        picker = Preview<C::CalendarDatePicker>(grid, "date");
        return DatePreviewMatches(picker, value, property->GetValueAsString());
    }));
    CHECK_FALSE(Preview<C::Control>(grid, "noeditor"));
    CHECK_FALSE(Preview<C::Control>(grid, "customdate"));
    grid->SetPropertyReadOnly(property);
    REQUIRE(WaitPreview([&]() { return !picker.IsEnabled(); }));
    grid->SetPropertyReadOnly(property, false);
    REQUIRE(WaitPreview([&]() { return picker.IsEnabled(); }));
    grid->EnableProperty(property, false);
    REQUIRE(WaitPreview([&]() { return !picker.IsEnabled(); }));
    grid->EnableProperty(property, true);
    REQUIRE(WaitPreview([&]() { return picker.IsEnabled(); }));
    grid->SetPropertyValueUnspecified(property);
    REQUIRE(WaitPreview([&]() { return DatePreviewMatches(picker, wxDefaultDateTime, property->GetValueAsString()); }));
    CHECK(property->IsValueUnspecified());
    grid->SetPropertyValue(property, value);
    REQUIRE(WaitPreview([&]() { return DatePreviewMatches(picker, value, property->GetValueAsString()); }));
    REQUIRE(grid->SelectProperty(property));
    REQUIRE(WaitPreview([&]()
    {
        const auto editor = FindPreviewPart<C::CalendarDatePicker>(WindowContent(grid->GetEditorControl()));
        return editor && editor.IsLoaded() && !PreviewVisible(picker);
    }));
    auto* const editor = wxDynamicCast(grid->GetEditorControl(), wxDatePickerCtrl);
    REQUIRE(editor);
    CHECK(editor->GetValue() == value);
    REQUIRE(grid->ClearSelection());
    REQUIRE(WaitPreview([&]() { return !grid->GetEditorControl() && DatePreviewMatches(picker, value, property->GetValueAsString()); }));
    CHECK(Preview<C::CalendarDatePicker>(grid, "date") == picker);
    CheckPresentationOnly(picker);
    CHECK(changed == 0);
}

TEST_CASE("WinUIPropertyGrid::DateDropdownPublicationCanRetirePropertyPageOrOwner",
          "[winui-propgrid][native-propgrid][native-propgrid-preview][native-propgrid-preview-date]")
{
    const int action = GENERATE(0, 1, 2);
    CAPTURE(action);
    PreviewFixture fixture;
    auto* const grid = fixture.grid;
    wxWeakRef<wxPropertyGrid> weakGrid(grid);
    const wxDateTime initial(4, wxDateTime::Oct, 2026);
    const wxDateTime replacementValue(5, wxDateTime::Oct, 2026);
    auto* const property = AppendDropdownDate(grid, "date", initial);
    fixture.Show();
    C::CalendarDatePicker picker{nullptr};
    REQUIRE(WaitPreview([&]()
    {
        picker = Preview<C::CalendarDatePicker>(grid, "date");
        return DatePreviewMatches(picker, initial, property->GetValueAsString());
    }));
    bool armed = true;
    unsigned calls = 0;
    const auto token = picker.RegisterPropertyChangedCallback(C::CalendarDatePicker::DateProperty(),
        [&](const X::DependencyObject&, const X::DependencyProperty&)
        {
            if ( !armed ) return;
            armed = false;
            ++calls;
            if ( action == 0 ) grid->DeleteProperty(property);
            else if ( action == 1 ) grid->Clear();
            else
            {
                fixture.grid = nullptr;
                fixture.frame.reset();
            }
        });
    const auto unhook = wxMakeGuard([&]()
        { picker.UnregisterPropertyChangedCallback(C::CalendarDatePicker::DateProperty(), token); });
    grid->SetPropertyValue(property, replacementValue);
    REQUIRE(WaitPreview([&]() { return calls == 1 && !picker.IsLoaded(); }));
    if ( action == 2 ) CHECK_FALSE(weakGrid);
    else
    {
        REQUIRE(weakGrid);
        CHECK_FALSE(grid->GetPropertyByName("date"));
        auto* const replacement = AppendDropdownDate(grid, "date", initial);
        C::CalendarDatePicker current{nullptr};
        REQUIRE(WaitPreview([&]()
        {
            current = Preview<C::CalendarDatePicker>(grid, "date");
            return DatePreviewMatches(current, initial, replacement->GetValueAsString());
        }));
        CHECK(current != picker);
        picker.Date(nullptr);
        CHECK(replacement->GetValue().GetDateTime() == initial);
        CHECK(DatePreviewMatches(current, initial, replacement->GetValueAsString()));
    }
    CHECK(calls == 1);
}
TEST_CASE("WinUIPropertyGrid::DateSpinPreviewHasRealCalendarFieldsAndStableGeometry",
          "[winui-propgrid][native-propgrid][native-propgrid-preview][native-propgrid-preview-date-spin]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    unsigned changed = 0;
    PreviewFixture fixture(rtl);
    auto* const grid = fixture.grid;
    const wxDateTime initial(17, wxDateTime::Nov, 2025, 13, 14, 15);
    auto* const property = AppendSpinDate(grid, "date", initial);
    grid->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent&) { ++changed; });
    fixture.Show();
    C::ContentControl root{nullptr};
    REQUIRE(WaitPreview([&]()
    {
        root = Preview<C::ContentControl>(grid, "date");
        return DateSpinMatches(root, "date", initial);
    }));
    CheckPresentationOnly(root);
    const auto day = DateSpinPart(root, "date", "Day");
    const auto month = DateSpinPart(root, "date", "Month");
    const auto year = DateSpinPart(root, "date", "Year");
    const auto clear = DateSpinPart<C::Button>(root, "date", "Clear");
    const auto cell = FindPreviewPart<C::Button>(WindowContent(grid), L"wxProperty.Cell.date.1");
    REQUIRE(cell);
    CHECK(cell.ActualHeight() == Approx(grid->GetRowHeight() / grid->GetDPIScaleFactor()));
    const auto cellBounds = PreviewScreenRect(grid, cell);
    for ( const auto& number : {day, month, year} )
    {
        CheckPresentationOnly(number);
        CHECK(number.SpinButtonPlacementMode() == C::NumberBoxSpinButtonPlacementMode::Compact);
        REQUIRE(FindPreviewPart<C::TextBox>(number));
        CHECK(number.ActualHeight() >= 32);
        const auto bounds = PreviewScreenRect(grid, number);
        CHECK(bounds.GetTop() >= cellBounds.GetTop());
        CHECK(bounds.GetBottom() <= cellBounds.GetBottom());
    }
    CHECK(PreviewVisible(clear));
    CHECK_FALSE(FindPreviewPart<C::CalendarDatePicker>(root));
    const double dayX = PreviewScreenRect(grid, day).GetX();
    const double monthX = PreviewScreenRect(grid, month).GetX();
    const double yearX = PreviewScreenRect(grid, year).GetX();
    // Compare the actual field order to the real wxDP_SPIN editor on this
    // locale/RTL layout, without assuming one particular date convention.
    REQUIRE(grid->SelectProperty(property));
    C::NumberBox activeDay{nullptr}, activeMonth{nullptr}, activeYear{nullptr};
    REQUIRE(WaitPreview([&]()
    {
        const auto editorRoot = WindowContent(grid->GetEditorControl());
        activeDay = FindDateEditorPart(editorRoot, day.Value());
        activeMonth = FindDateEditorPart(editorRoot, month.Value());
        activeYear = FindDateEditorPart(editorRoot, year.Value());
        return PreviewVisible(activeDay) && PreviewVisible(activeMonth) && PreviewVisible(activeYear);
    }));
    auto* const editor = wxDynamicCast(grid->GetEditorControl(), wxDatePickerCtrl);
    REQUIRE(editor);
    CHECK(editor->HasFlag(wxDP_SPIN));
    CHECK((dayX < monthX) == (PreviewScreenRect(grid, activeDay).GetX() < PreviewScreenRect(grid, activeMonth).GetX()));
    CHECK((monthX < yearX) == (PreviewScreenRect(grid, activeMonth).GetX() < PreviewScreenRect(grid, activeYear).GetX()));
    CHECK_FALSE(PreviewVisible(root));
    REQUIRE(grid->ClearSelection());
    REQUIRE(WaitPreview([&]() { return DateSpinMatches(root, "date", initial); }));
    const wxDateTime ancient(9, wxDateTime::Sep, 64, 3, 14, 15);
    grid->SetPropertyValue(property, ancient);
    REQUIRE(WaitPreview([&]() { return DateSpinMatches(root, "date", ancient); }));
    CHECK(property->GetValue().GetDateTime() == ancient);
    grid->RefreshProperty(property);
    fixture.frame->SetClientSize(860, 580);
    REQUIRE(WaitPreview([&]() { return DateSpinMatches(root, "date", ancient); }));
    CHECK(Preview<C::ContentControl>(grid, "date") == root);
    CHECK(DateSpinPart(root, "date", "Day") == day);
    CHECK(DateSpinPart(root, "date", "Month") == month);
    CHECK(DateSpinPart(root, "date", "Year") == year);
    grid->SetPropertyAttribute(property, wxPG_DATE_PICKER_STYLE, wxDP_SPIN | wxDP_ALLOWNONE);
    REQUIRE(WaitPreview([&]()
    {
        const auto text = FindPreviewPart<C::TextBox>(year);
        return text && text.IsLoaded() && text.Text().size() == 2;
    }));
    grid->SetPropertyAttribute(property, wxPG_DATE_PICKER_STYLE, wxDP_SPIN | wxDP_SHOWCENTURY);
    REQUIRE(WaitPreview([&]()
    {
        const auto text = FindPreviewPart<C::TextBox>(year);
        return text && text.IsLoaded() && text.Text().size() >= 4 && !PreviewVisible(clear);
    }));
    CHECK(changed == 0);
}

TEST_CASE("WinUIPropertyGrid::DateSpinPreviewPreservesExactFormatsAndEditorStates",
          "[winui-propgrid][native-propgrid][native-propgrid-preview][native-propgrid-preview-date-spin]")
{
    static CustomNamedDateEditor customEditor;
    unsigned changed = 0;
    PreviewFixture fixture;
    auto* const grid = fixture.grid;
    const wxDateTime initial(17, wxDateTime::Nov, 2025, 3, 14, 15);
    auto* const property = AppendSpinDate(grid, "date", initial);
    auto* const hidden = AppendSpinDate(grid, "noeditor", initial);
    hidden->ChangeFlag(wxPGFlags::NoEditor, true);
    auto* const custom = AppendSpinDate(grid, "customdate", initial);
    grid->SetPropertyEditor(custom, &customEditor);
    grid->SetPropertyAttribute(property, wxPG_DATE_FORMAT, "Epoch {%A} %d/%m/%Y at %H:%M, 100%%");
    grid->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent&) { ++changed; });
    fixture.Show();
    C::ContentControl root{nullptr};
    C::NumberBox formatted{nullptr};
    REQUIRE(WaitPreview([&]()
    {
        root = Preview<C::ContentControl>(grid, "date");
        formatted = DateSpinPart(root, "date", "Formatted");
        return SpinTextIs(formatted, property->GetValueAsString());
    }));
    CheckPresentationOnly(formatted);
    CHECK(formatted.SpinButtonPlacementMode() == C::NumberBoxSpinButtonPlacementMode::Compact);
    CHECK(wxString(formatted.Text().c_str()).Contains("03:14"));
    CHECK_FALSE(PreviewVisible(DateSpinPart(root, "date", "Day")));
    CHECK_FALSE(Preview<C::Control>(grid, "noeditor"));
    CHECK_FALSE(Preview<C::Control>(grid, "customdate"));
    grid->SetPropertyReadOnly(property);
    REQUIRE(WaitPreview([&]() { return !root.IsEnabled() && !formatted.IsEnabled(); }));
    grid->SetPropertyReadOnly(property, false);
    grid->EnableProperty(property, false);
    REQUIRE(WaitPreview([&]() { return !root.IsEnabled(); }));
    grid->EnableProperty(property, true);
    REQUIRE(WaitPreview([&]() { return root.IsEnabled(); }));
    grid->SetPropertyValueUnspecified(property);
    REQUIRE(WaitPreview([&]() { return PreviewVisible(formatted) && std::isnan(formatted.Value()); }));
    CHECK(property->IsValueUnspecified());
    grid->SetPropertyValue(property, initial);
    REQUIRE(WaitPreview([&]() { return SpinTextIs(formatted, property->GetValueAsString()); }));
    REQUIRE(grid->SelectProperty(property));
    REQUIRE(WaitPreview([&]()
    {
        const auto native = FindPreviewPart<C::NumberBox>(WindowContent(grid->GetEditorControl()));
        return PreviewVisible(native) && !PreviewVisible(root);
    }));
    auto* const editor = wxDynamicCast(grid->GetEditorControl(), wxDatePickerCtrl);
    REQUIRE(editor);
    CHECK(editor->HasFlag(wxDP_SPIN));
    REQUIRE(grid->ClearSelection());
    REQUIRE(WaitPreview([&]() { return SpinTextIs(formatted, property->GetValueAsString()); }));
    CHECK(Preview<C::ContentControl>(grid, "date") == root);
    CHECK(DateSpinPart(root, "date", "Formatted") == formatted);
    grid->SetPropertyAttribute(property, wxPG_DATE_FORMAT, wxString());
    REQUIRE(WaitPreview([&]() { return DateSpinMatches(root, "date", initial) && !PreviewVisible(formatted); }));
    grid->SetPropertyValueUnspecified(property);
    REQUIRE(WaitPreview([&]() { return DateSpinMatches(root, "date", wxDefaultDateTime); }));
    grid->SetPropertyValue(property, initial);
    grid->SetPropertyAttribute(property, wxPG_DATE_PICKER_STYLE,
                               wxDP_DROPDOWN | wxDP_SHOWCENTURY | wxDP_ALLOWNONE);
    REQUIRE(WaitPreview([&]()
    {
        return !root.IsLoaded() && DatePreviewMatches(
            Preview<C::CalendarDatePicker>(grid, "date"), initial, property->GetValueAsString());
    }));
    // A retained peer from the previous editor kind is only a snapshot.
    formatted.Value(2030);
    CHECK(property->GetValue().GetDateTime() == initial);
    grid->SetPropertyAttribute(property, wxPG_DATE_PICKER_STYLE,
                               wxDP_SPIN | wxDP_SHOWCENTURY | wxDP_ALLOWNONE);
    REQUIRE(WaitPreview([&]()
    {
        const auto current = Preview<C::ContentControl>(grid, "date");
        return current != root && DateSpinMatches(current, "date", initial);
    }));
    CHECK(changed == 0);
}

TEST_CASE("WinUIPropertyGrid::DateSpinPublicationCanRetirePropertyPageOrOwner",
          "[winui-propgrid][native-propgrid][native-propgrid-preview][native-propgrid-preview-date-spin]")
{
    const int action = GENERATE(0, 1, 2);
    CAPTURE(action);
    PreviewFixture fixture;
    auto* const grid = fixture.grid;
    wxWeakRef<wxPropertyGrid> weakGrid(grid);
    const wxDateTime initial(4, wxDateTime::Oct, 2026);
    const wxDateTime replacementValue(5, wxDateTime::Oct, 2027);
    auto* const property = AppendSpinDate(grid, "date", initial);
    fixture.Show();
    C::ContentControl root{nullptr};
    REQUIRE(WaitPreview([&]()
    {
        root = Preview<C::ContentControl>(grid, "date");
        return DateSpinMatches(root, "date", initial);
    }));
    const auto year = DateSpinPart(root, "date", "Year");
    bool armed = true;
    unsigned calls = 0;
    const auto token = year.RegisterPropertyChangedCallback(C::NumberBox::ValueProperty(),
        [&](const X::DependencyObject&, const X::DependencyProperty&)
        {
            if ( !armed ) return;
            armed = false;
            ++calls;
            if ( action == 0 ) grid->DeleteProperty(property);
            else if ( action == 1 ) grid->Clear();
            else
            {
                fixture.grid = nullptr;
                fixture.frame.reset();
            }
        });
    const auto unhook = wxMakeGuard([&]()
        { year.UnregisterPropertyChangedCallback(C::NumberBox::ValueProperty(), token); });
    grid->SetPropertyValue(property, replacementValue);
    REQUIRE(WaitPreview([&]() { return calls == 1 && !root.IsLoaded() && !year.IsLoaded(); }));
    if ( action == 2 ) CHECK_FALSE(weakGrid);
    else
    {
        REQUIRE(weakGrid);
        CHECK_FALSE(grid->GetPropertyByName("date"));
        auto* const replacement = AppendSpinDate(grid, "date", initial);
        C::ContentControl current{nullptr};
        REQUIRE(WaitPreview([&]()
        {
            current = Preview<C::ContentControl>(grid, "date");
            return DateSpinMatches(current, "date", initial);
        }));
        CHECK(current != root);
        year.Value(2030);
        CHECK(replacement->GetValue().GetDateTime() == initial);
        CHECK(DateSpinMatches(current, "date", initial));
    }
    CHECK(calls == 1);
}
#endif // wxUSE_DATEPICKCTRL

#endif // native PropertyGrid previews
