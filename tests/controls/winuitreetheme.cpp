/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuitreetheme.cpp
// Purpose:     Realized TreeView label theme and explicit-colour contracts
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_TREECTRL

#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/sizer.h"
#include "wx/stopwatch.h"
#include "wx/treectrl.h"
#include "wx/winui/winui.h"
#include "wx/winui/private/tlwhost.h"

#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <memory>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace M = X::Media;

template<typename Predicate>
bool WaitTreeTheme(Predicate predicate)
{
    auto* const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp ) return false;
    wxStopWatch elapsed;
    while ( elapsed.Time() < 1000 )
    {
        if ( predicate() ) return true;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop ) return false;
        if ( predicate() ) return true;
        if ( loop->DispatchTimeout(5) == 0 ) return false;
    }
    return predicate();
}

wxApp::Appearance TreeAppearance(wxWinUIAppTheme theme)
{
    switch ( theme )
    {
        case wxWinUIAppTheme::Light: return wxApp::Appearance::Light;
        case wxWinUIAppTheme::Dark: return wxApp::Appearance::Dark;
        default: return wxApp::Appearance::System;
    }
}

struct TreeThemeGuard
{
    TreeThemeGuard() : previous(wxWinUIGetAppTheme()) {}
    ~TreeThemeGuard()
    {
        if ( wxTheApp ) wxTheApp->SetAppearance(TreeAppearance(previous));
    }
    wxWinUIAppTheme previous;
};

wxPoint TreeThemePosition()
{
    wchar_t desktop[256]{};
    DWORD needed = 0;
    if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
            UOI_NAME, desktop, sizeof(desktop), &needed) &&
         wxString(desktop).StartsWith("wxWinUITest-") ) return wxPoint(80, 80);
    return wxPoint(-30000, -30000);
}

C::TextBlock FindTreeThemeLabel(const X::DependencyObject& root,
                                const wchar_t* text)
{
    if ( !root ) return nullptr;
    if ( const auto label = root.try_as<C::TextBlock>(); label &&
         label.Name() == L"wxTreeLabel" && label.Text() == text ) return label;
    const int count = M::VisualTreeHelper::GetChildrenCount(root);
    for ( int i = 0; i < count; ++i )
        if ( auto found = FindTreeThemeLabel(
                M::VisualTreeHelper::GetChild(root, i), text) ) return found;
    return nullptr;
}

C::TreeViewItem TreeThemeContainer(const C::TextBlock& label)
{
    X::DependencyObject node = label;
    for ( int depth = 0; node && depth < 64; ++depth )
    {
        if ( auto item = node.try_as<C::TreeViewItem>() ) return item;
        node = M::VisualTreeHelper::GetParent(node);
    }
    return nullptr;
}

bool HasTreeColour(const C::TextBlock& label, const wxColour& colour)
{
    if ( !label || !label.IsLoaded() ) return false;
    const auto brush = label.Foreground().try_as<M::SolidColorBrush>();
    if ( !brush ) return false;
    const auto actual = brush.Color();
    return actual.A == colour.Alpha() && actual.R == colour.Red() &&
           actual.G == colour.Green() && actual.B == colour.Blue();
}

bool HasReadableTreeTheme(const C::TextBlock& label, X::ElementTheme theme)
{
    if ( !label || !label.IsLoaded() || label.ActualWidth() <= 0 ||
         label.ActualTheme() != theme ) return false;
    const auto brush = label.Foreground().try_as<M::SolidColorBrush>();
    if ( !brush ) return false;
    const auto colour = brush.Color();
    // Do not pin theme-palette RGB values, but catch the reported white-on-light
    // failure using the actual rendered label brush, not the app preference.
    const unsigned lightness = unsigned(colour.R) + colour.G + colour.B;
    return colour.A != 0 && (theme == X::ElementTheme::Dark
        ? lightness > 3 * 128 : lightness < 3 * 128);
}

} // namespace

TEST_CASE("WinUITreeTheme::RealizedLabelsFollowApplicationAppearance",
          "[winui-treemodel][winui-tree-theme]")
{
    const bool rtl = GENERATE(false, true);
    TreeThemeGuard restoreTheme;
    REQUIRE(wxTheApp->SetAppearance(wxApp::Appearance::Dark) ==
            wxApp::AppearanceResult::Ok);

    auto frame = std::make_unique<wxFrame>(nullptr, wxID_ANY,
        "TreeView theme", TreeThemePosition(), wxSize(480, 300));
    frame->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    auto* const tree = new wxTreeCtrl(frame.get(), wxID_ANY,
        wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS);
    auto* const sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(tree, 1, wxEXPAND);
    frame->SetSizer(sizer);
    const auto root = tree->AddRoot("root");
    const auto normalItem = tree->AppendItem(root, "Default navigation label");
    const auto explicitItem = tree->AppendItem(root, "Explicit colour label");
    const wxColour explicitColour(28, 101, 185);
    tree->SetItemTextColour(explicitItem, explicitColour);
    frame->ShowWithoutActivating();

    C::TreeView native{nullptr};
    C::TextBlock normal{nullptr}, explicitLabel{nullptr};
    REQUIRE(WaitTreeTheme([&]
    {
        auto* const host = wxWinUITopLevelHost::FindSlotOwner(tree);
        auto* const slot = host ? host->FindSlot(tree) : nullptr;
        native = slot ? slot->GetContent().try_as<C::TreeView>() : nullptr;
        normal = FindTreeThemeLabel(native, L"Default navigation label");
        explicitLabel = FindTreeThemeLabel(native, L"Explicit colour label");
        return HasReadableTreeTheme(normal, X::ElementTheme::Dark) &&
               HasTreeColour(explicitLabel, explicitColour);
    }));
    const auto normalIdentity = normal;
    const auto explicitIdentity = explicitLabel;
    const auto containerIdentity = TreeThemeContainer(normal);
    REQUIRE(containerIdentity);

    for ( const auto theme : {wxApp::Appearance::Light, wxApp::Appearance::Dark} )
    {
        const auto nativeTheme = theme == wxApp::Appearance::Light
            ? X::ElementTheme::Light : X::ElementTheme::Dark;
        REQUIRE(wxTheApp->SetAppearance(theme) == wxApp::AppearanceResult::Ok);
        REQUIRE(WaitTreeTheme([&]
        {
            return HasReadableTreeTheme(normal, nativeTheme) &&
                   HasTreeColour(explicitLabel, explicitColour);
        }));
        CHECK(FindTreeThemeLabel(native, L"Default navigation label") == normalIdentity);
        CHECK(FindTreeThemeLabel(native, L"Explicit colour label") == explicitIdentity);
        CHECK(TreeThemeContainer(normal) == containerIdentity);
        CHECK_FALSE(tree->GetItemTextColour(normalItem).IsOk());
        CHECK(tree->GetItemTextColour(explicitItem) == explicitColour);
    }

    // Removing a local item colour must resume the native theme, including
    // subsequent application changes, without replacing the realized label.
    tree->SetItemTextColour(explicitItem, wxNullColour);
    REQUIRE(WaitTreeTheme([&]
    {
        return HasReadableTreeTheme(explicitLabel, X::ElementTheme::Dark);
    }));
    REQUIRE(wxTheApp->SetAppearance(wxApp::Appearance::Light) ==
            wxApp::AppearanceResult::Ok);
    REQUIRE(WaitTreeTheme([&]
    {
        return HasReadableTreeTheme(normal, X::ElementTheme::Light) &&
               HasReadableTreeTheme(explicitLabel, X::ElementTheme::Light);
    }));
    CHECK(FindTreeThemeLabel(native, L"Explicit colour label") == explicitIdentity);
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_TREECTRL
