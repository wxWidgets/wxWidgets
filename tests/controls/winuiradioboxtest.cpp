/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiradioboxtest.cpp
// Purpose:     Native wxRadioBox hit testing and command activation
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_RADIOBOX

#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/radiobox.h"
#include "wx/sizer.h"
#include "wx/stopwatch.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"
#if wxUSE_STATBOX
#include "wx/statbox.h"
#endif

#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <memory>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation;

template<typename Predicate>
bool WaitRadioBox(Predicate predicate)
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

wxPoint RadioBoxPosition()
{
    wchar_t desktop[256]{};
    DWORD needed = 0;
    if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
            UOI_NAME, desktop, sizeof(desktop), &needed) &&
         wxString(desktop).StartsWith("wxWinUITest-") ) return wxPoint(80, 80);
    return wxPoint(-30000, -30000);
}

C::RadioButton FindRadioBoxButton(const X::DependencyObject& node,
                                  const wxString& label)
{
    if ( !node ) return nullptr;
    if ( const auto button = node.try_as<C::RadioButton>(); button &&
         wxString(winrt::unbox_value_or<winrt::hstring>(
             button.Content(), {}).c_str()) == label ) return button;
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int i = 0; i < count; ++i )
        if ( const auto found = FindRadioBoxButton(
                X::Media::VisualTreeHelper::GetChild(node, i), label) ) return found;
    return nullptr;
}

bool RadioBoxButtonHasNativeHit(const C::RadioButton& button)
{
    if ( !button || !button.IsLoaded() || !button.XamlRoot() ||
         button.ActualWidth() <= 0 || button.ActualHeight() <= 0 ) return false;
    const auto root = button.XamlRoot().Content();
    const auto point = button.TransformToVisual(root).TransformPoint(
        {float(button.ActualWidth() / 2), float(button.ActualHeight() / 2)});
    // This is XAML's actual hit-test route. A true property on the button
    // alone is insufficient when a containing Border disables its subtree.
    const auto hits = X::Media::VisualTreeHelper::FindElementsInHostCoordinates(point, root);
    for ( const auto& hit : hits )
    {
        X::DependencyObject current = hit;
        for ( int depth = 0; current && depth < 64; ++depth )
        {
            if ( current == button ) return true;
            if ( current == root ) break;
            current = X::Media::VisualTreeHelper::GetParent(current);
        }
    }
    return false;
}

A::Provider::ISelectionItemProvider RadioBoxSelection(const C::RadioButton& button)
{
    const auto peer = A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(button);
    return peer ? peer.GetPattern(A::Peers::PatternInterface::SelectionItem)
                          .try_as<A::Provider::ISelectionItemProvider>() : nullptr;
}

struct NativeRadioBoxFixture
{
    explicit NativeRadioBoxFixture(bool rtl)
    {
        frame = std::make_unique<wxFrame>(nullptr, wxID_ANY,
            "native RadioBox input", RadioBoxPosition(), wxSize(580, 300));
        frame->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        wxWindow* parent = frame.get();
        wxBoxSizer* sizer;
#if wxUSE_STATBOX
        auto* const group = new wxStaticBoxSizer(wxVERTICAL, frame.get(), "Toggles");
        sizer = group;
        parent = group->GetStaticBox();
#else
        sizer = new wxBoxSizer(wxVERTICAL);
#endif
        // Same parentage, labels, and horizontal layout as Showcase.
        const wxString choices[] = {"left", "centre", "right"};
        auto candidate = std::make_unique<wxRadioBox>();
        CHECK_FALSE(candidate->CanBeFocused());
        REQUIRE(candidate->Create(parent, wxID_ANY, "wxRadioBox",
            wxDefaultPosition, wxDefaultSize, WXSIZEOF(choices), choices,
            1, wxRA_SPECIFY_ROWS));
        radio = candidate.release();
        sizer->Add(radio, 0, wxEXPAND | wxALL, 12);
        frame->SetSizer(sizer);
        frame->Layout();
        frame->ShowWithoutActivating();
        REQUIRE(WaitRadioBox([&]
        {
            const auto centre = Button("centre");
            return centre && centre.IsLoaded() &&
                   centre.ActualWidth() > 0 && centre.ActualHeight() > 0;
        }));
        CHECK(radio->CanBeFocused());
    }

    C::RadioButton Button(const wxString& label) const
    {
        auto* const host = wxWinUITopLevelHost::FindSlotOwner(radio);
        auto* const slot = host ? host->FindSlot(radio) : nullptr;
        return slot ? FindRadioBoxButton(slot->GetContent(), label) : nullptr;
    }

    std::unique_ptr<wxFrame> frame;
    wxRadioBox* radio = nullptr;
};
}

TEST_CASE("wxWinUI RadioBox native buttons remain hittable and send one command",
          "[radiobox][winui-radiobox][native-radiobox-input]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    NativeRadioBoxFixture fixture(rtl);
    auto* const radio = fixture.radio;
    const auto centre = fixture.Button("centre");
    const auto right = fixture.Button("right");
    CHECK(RadioBoxButtonHasNativeHit(centre));
    CHECK(RadioBoxButtonHasNativeHit(right));
    const auto provider = RadioBoxSelection(centre);
    REQUIRE(provider);
    int commands = 0;
    radio->Bind(wxEVT_RADIOBOX, [&](wxCommandEvent& event)
    {
        ++commands;
        CHECK(event.GetEventObject() == radio);
        CHECK(event.GetSelection() == 1);
        CHECK(event.GetString() == "centre");
        CHECK(radio->GetSelection() == 1);
    });
    provider.Select();
    REQUIRE(WaitRadioBox([&] { return radio->GetSelection() == 1; }));
    CHECK(centre.IsChecked().Value());
    CHECK(commands == 1);
    provider.Select();
    CHECK(commands == 1);
    radio->SetSelection(2);
    CHECK(right.IsChecked().Value());
    CHECK(commands == 1);

    // A content-generation replacement must preserve the pointer route too.
    radio->SetString(1, "middle");
    REQUIRE(WaitRadioBox([&] { return RadioBoxButtonHasNativeHit(fixture.Button("middle")); }));
    radio->Show(1, false);
    CHECK_FALSE(RadioBoxButtonHasNativeHit(fixture.Button("middle")));
    radio->Show(1, true);
    REQUIRE(WaitRadioBox([&] { return RadioBoxButtonHasNativeHit(fixture.Button("middle")); }));
    CHECK(commands == 1);
}

TEST_CASE("wxWinUI RadioBox native activation may destroy its owner",
          "[radiobox][winui-radiobox][native-radiobox-input][lifetime]")
{
    NativeRadioBoxFixture fixture(false);
    auto* const radio = fixture.radio;
    const auto button = fixture.Button("centre");
    CHECK(RadioBoxButtonHasNativeHit(button));
    const auto provider = RadioBoxSelection(button);
    REQUIRE(provider);
    wxWeakRef<wxRadioBox> weak(radio);
    int commands = 0;
    radio->Bind(wxEVT_RADIOBOX, [&](wxCommandEvent& event)
    {
        CHECK(event.GetSelection() == 1);
        ++commands;
        delete radio;
    });
    provider.Select();
    REQUIRE(WaitRadioBox([&] { return !weak.get() && !button.IsLoaded(); }));
    CHECK(commands == 1);
    try { provider.Select(); }
    catch ( const winrt::hresult_error& ) {}
    CHECK(commands == 1);
}

#endif // WinUI RadioBox
