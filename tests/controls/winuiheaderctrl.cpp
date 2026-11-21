/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiheaderctrl.cpp
// Purpose:     Public HeaderCtrl contracts on its real WinUI presentation
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_HEADERCTRL

#include "wx/app.h"
#include "wx/dcmemory.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/headerctrl.h"
#include "wx/image.h"
#include "wx/sizer.h"
#include "wx/stopwatch.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <vector>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation;

template<typename Predicate>
bool WaitHeader(Predicate predicate)
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

template<typename T>
T FindHeader(const X::DependencyObject& node, const winrt::hstring& id = {})
{
    if ( !node ) return nullptr;
    if ( const auto result = node.try_as<T>(); result &&
         (id.empty() || A::AutomationProperties::GetAutomationId(result) == id) ) return result;
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int n = 0; n < count; ++n )
        if ( const auto result = FindHeader<T>(
                 X::Media::VisualTreeHelper::GetChild(node, n), id) ) return result;
    return nullptr;
}

wxPoint HeaderPosition()
{
    wchar_t name[256]{};
    DWORD needed = 0;
    if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
            UOI_NAME, name, sizeof(name), &needed) &&
         wxString(name).StartsWith("wxWinUITest-") ) return wxPoint(80, 80);
    return wxPoint(-30000, -30000);
}

wxArrayInt HeaderOrder(std::initializer_list<int> indices)
{
    wxArrayInt order;
    for ( const int index : indices ) order.Add(index);
    return order;
}

class NativeTestHeader final : public wxHeaderCtrlSimple
{
public:
    const wxHeaderColumn& Column(unsigned int index) const { return GetColumn(index); }
    wxSettableHeaderColumn& MutableColumn(unsigned int index)
    {
        return const_cast<wxHeaderColumnSimple&>(
            static_cast<const wxHeaderColumnSimple&>(GetColumn(index)));
    }
};

struct NativeHeaderFixture
{
    explicit NativeHeaderFixture(bool rtl = false)
    {
        frame = std::make_unique<wxFrame>(nullptr, wxID_ANY, "Native HeaderCtrl",
                                        HeaderPosition(), wxSize(660, 200));
        frame->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        auto candidate = std::make_unique<NativeTestHeader>();
        REQUIRE(candidate->Create(frame.get(), wxID_ANY));
        header = candidate.release();
        header->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        header->SetMinSize(wxSize(-1, header->FromDIP(36)));
        auto* const sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(header, 0, wxEXPAND);
        sizer->AddStretchSpacer();
        frame->SetSizer(sizer);
    }
    ~NativeHeaderFixture()
    {
        frame.reset();
        wxTheApp->ProcessPendingEvents();
    }
    void AddColumns()
    {
        header->AppendColumn(wxHeaderColumnSimple("First", header->FromDIP(160), wxALIGN_LEFT,
            wxCOL_DEFAULT_FLAGS | wxCOL_SORTABLE));
        header->AppendColumn(wxHeaderColumnSimple("Middle", header->FromDIP(130), wxALIGN_CENTER));
        header->AppendColumn(wxHeaderColumnSimple("Last", header->FromDIP(110), wxALIGN_RIGHT));
    }
    void Show()
    {
        frame->Layout();
        frame->ShowWithoutActivating();
        REQUIRE(WaitHeader([&]()
        {
            const auto root = Root();
            const auto first = Button(0);
            return root && root.IsLoaded() && root.ActualWidth() > 0 &&
                   first && first.IsLoaded() && first.ActualWidth() > 0;
        }));
    }
    C::Grid Root() const
    {
        auto* const host = wxWinUITopLevelHost::FindSlotOwner(header);
        auto* const slot = host ? host->FindSlot(header) : nullptr;
        return slot ? FindHeader<C::Grid>(slot->GetContent(), L"wxHeader.NativeRoot") : nullptr;
    }
    C::Button Button(unsigned int column) const
    {
        return FindHeader<C::Button>(Root(),
            winrt::hstring(wxString::Format("wxHeader.Column.%u", column).wc_str()));
    }
    std::unique_ptr<wxFrame> frame;
    NativeTestHeader* header = nullptr;
};

wxRect HeaderScreenRect(wxWindow* header, const X::FrameworkElement& element)
{
    auto* const host = wxWinUITopLevelHost::ForWindow(header, false);
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

wxString HeaderText(const C::Button& button)
{
    const auto text = FindHeader<C::TextBlock>(button);
    return text ? wxString(text.Text().c_str()) : wxString();
}

bool HeaderMatches(const NativeHeaderFixture& fixture, int scrollOffset = 0)
{
    int x = scrollOffset;
    const wxRect viewport(wxPoint(0, 0), fixture.header->GetClientSize());
    for ( const int index : fixture.header->GetColumnsOrder() )
    {
        const auto& column = fixture.header->Column(index);
        const auto button = fixture.Button(index);
        if ( column.IsHidden() )
        {
            if ( button && button.IsLoaded() && button.Visibility() == X::Visibility::Visible )
                return false;
            continue;
        }
        wxRect expected(x, 0, column.GetWidth(), viewport.height);
        expected.Intersect(viewport);
        x += column.GetWidth();
        // Only the visible part is a native interactive surface. The content
        // inside it retains the full logical width and its clipped alignment.
        if ( expected.IsEmpty() )
        {
            if ( button && button.IsLoaded() && button.Visibility() == X::Visibility::Visible )
                return false;
            continue;
        }
        const wxRect rect = HeaderScreenRect(fixture.header, button);
        if ( rect.IsEmpty() || std::abs(rect.width - expected.width) > 2 ||
             HeaderText(button) != column.GetTitle() ) return false;
        const wxPoint centre = fixture.header->ScreenToClient(
            wxPoint(rect.x + rect.width / 2, rect.y + rect.height / 2));
        if ( std::abs(centre.x - (expected.x + expected.width / 2)) > 2 ) return false;
    }
    return true;
}

A::Provider::IInvokeProvider HeaderInvoke(const C::Button& button)
{
    const auto peer = button ? A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(button) : nullptr;
    return peer ? peer.GetPattern(A::Peers::PatternInterface::Invoke)
                          .try_as<A::Provider::IInvokeProvider>() : nullptr;
}

void InvokeInactiveHeader(const A::Provider::IInvokeProvider& provider)
{
    try { provider.Invoke(); }
    catch ( const winrt::hresult_error& error )
    {
        const auto code = static_cast<int32_t>(error.code());
        CHECK((code == static_cast<int32_t>(0x80040200u) ||
               code == static_cast<int32_t>(0x80040201u)));
    }
}

// Enter the existing wx gesture state machine without injecting any physical
// mouse input. XAML publication/geometry is still observed through real peers.
void HeaderMouse(wxHeaderCtrl* header, wxEventType type, int x, bool leftDown = false)
{
    wxMouseEvent event(type);
    event.SetId(header->GetId());
    event.SetEventObject(header);
    event.SetPosition(wxPoint(x, header->GetClientSize().y / 2));
    event.SetLeftDown(leftDown);
    header->ProcessWindowEvent(event);
}

bool HeaderMarkerVisible(const NativeHeaderFixture& fixture, const wchar_t* id)
{
    const auto marker = FindHeader<C::Border>(fixture.Root(), id);
    return marker && marker.IsLoaded() && marker.Visibility() == X::Visibility::Visible &&
           marker.ActualWidth() > 0 && marker.ActualHeight() > 0;
}

wxBitmap HeaderBitmap(unsigned char red, unsigned char green, unsigned char blue)
{
    wxImage pixels(16, 16);
    pixels.SetRGB(wxRect(0, 0, 16, 16), red, green, blue);
    return wxBitmap(pixels);
}

struct HeaderBitmapState
{
    wxBitmap bitmap = HeaderBitmap(255, 0, 0);
    unsigned calls = 0;
};

class MutableHeaderBitmapBundle final : public wxBitmapBundleImpl
{
public:
    explicit MutableHeaderBitmapBundle(const std::shared_ptr<HeaderBitmapState>& state)
        : m_state(state) {}
    wxSize GetDefaultSize() const override { return wxSize(16, 16); }
    wxSize GetPreferredBitmapSizeAtScale(double) const override { return wxSize(16, 16); }
    wxBitmap GetBitmap(const wxSize&) override
    {
        ++m_state->calls;
        return m_state->bitmap;
    }
private:
    std::shared_ptr<HeaderBitmapState> m_state;
};

std::uint32_t HeaderImagePixel(const C::Image& image)
{
    const auto bitmap = image && image.Source()
        ? image.Source().try_as<X::Media::Imaging::WriteableBitmap>() : nullptr;
    if ( !bitmap || bitmap.PixelBuffer().Length() < 4 ) return 0;
    const auto reader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(bitmap.PixelBuffer());
    const std::uint32_t blue = reader.ReadByte();
    const std::uint32_t green = reader.ReadByte();
    const std::uint32_t red = reader.ReadByte();
    const std::uint32_t alpha = reader.ReadByte();
    return (alpha << 24) | (red << 16) | (green << 8) | blue;
}
}

TEST_CASE("WinUIHeaderCtrl::NativeColumnsRetainIdentityAcrossPresentationChanges",
          "[winui-headerctrl][native-headerctrl]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    NativeHeaderFixture fixture(rtl);
    auto* const header = fixture.header;
    fixture.AddColumns();
    fixture.Show();
    REQUIRE(WaitHeader([&]() { return HeaderMatches(fixture); }));
    const auto root = fixture.Root();
    const auto first = fixture.Button(0);
    const auto last = fixture.Button(2);
    CHECK(first.FlowDirection() == (rtl ? X::FlowDirection::RightToLeft : X::FlowDirection::LeftToRight));
    header->MutableColumn(0).SetTitle("Renamed first");
    header->MutableColumn(0).SetWidth(header->FromDIP(180));
    header->UpdateColumn(0);
    wxBitmap bitmap(16, 16, 32);
    {
        wxMemoryDC dc(bitmap);
        dc.SetBackground(*wxBLUE_BRUSH);
        dc.Clear();
        dc.SelectObject(wxNullBitmap);
    }
    header->MutableColumn(2).SetBitmap(wxBitmapBundle::FromBitmap(bitmap));
    header->UpdateColumn(2);
    REQUIRE(WaitHeader([&]() { return HeaderMatches(fixture); }));
    REQUIRE(WaitHeader([&]()
    {
        const auto image = FindHeader<C::Image>(fixture.Button(2));
        return image && image.IsLoaded() && image.ActualWidth() > 0 && image.Source();
    }));
    CHECK(fixture.Button(0) == first);
    CHECK(fixture.Button(2) == last);
    header->ShowSortIndicator(0, true);
    C::FontIcon sort{nullptr};
    REQUIRE(WaitHeader([&]()
    {
        sort = FindHeader<C::FontIcon>(first, L"wxHeader.Sort.0");
        return sort && sort.IsLoaded() && sort.Visibility() == X::Visibility::Visible &&
               !sort.Glyph().empty();
    }));
    const auto ascending = sort.Glyph();
    header->ShowSortIndicator(0, false);
    REQUIRE(WaitHeader([&]()
    {
        const auto current = FindHeader<C::FontIcon>(first, L"wxHeader.Sort.0");
        return current && current.IsLoaded() && current.Glyph() != ascending && !current.Glyph().empty();
    }));
    header->RemoveSortIndicator();
    REQUIRE(WaitHeader([&]()
    {
        const auto current = FindHeader<C::FontIcon>(first, L"wxHeader.Sort.0");
        return !current || current.Visibility() != X::Visibility::Visible || current.Glyph().empty();
    }));
    header->HideColumn(1);
    header->SetColumnsOrder(HeaderOrder({2, 1, 0}));
    REQUIRE(WaitHeader([&]() { return HeaderMatches(fixture); }));
    CHECK(header->GetColumnAt(1) == 1);
    CHECK(header->GetColumnPos(0) == 2);
    CHECK(fixture.Button(0) == first);
    CHECK(fixture.Button(2) == last);
    const auto beforeScroll = HeaderScreenRect(header, first);
    const int dx = -header->FromDIP(24);
    header->ScrollWindow(dx, 0);
    REQUIRE(WaitHeader([&]() { return HeaderMatches(fixture, dx); }));
    const auto afterScroll = HeaderScreenRect(header, first);
    CHECK(std::abs(afterScroll.x - beforeScroll.x - (rtl ? -dx : dx)) <= 2);
    CHECK(fixture.Button(0) == first);
    header->ScrollWindow(-dx, 0);
    header->ShowColumn(1);
    header->ResetColumnsOrder();
    REQUIRE(WaitHeader([&]() { return HeaderMatches(fixture); }));
    CHECK(fixture.Root() == root);
    CHECK(fixture.Button(0) == first);
    CHECK(fixture.Button(2) == last);
}

TEST_CASE("WinUIHeaderCtrl::MutableBundleWithStableIdentityRefreshesNativePixels",
          "[winui-headerctrl][native-headerctrl][native-header-images]")
{
    auto state = std::make_shared<HeaderBitmapState>();
    const auto bundle = wxBitmapBundle::FromImpl(new MutableHeaderBitmapBundle(state));
    NativeHeaderFixture fixture;
    fixture.AddColumns();
    fixture.header->MutableColumn(0).SetBitmap(bundle);
    fixture.header->UpdateColumn(0);
    fixture.Show();
    const auto button = fixture.Button(0);
    REQUIRE(WaitHeader([&]()
    {
        const auto image = FindHeader<C::Image>(button);
        return image && image.IsLoaded() && HeaderImagePixel(image) == 0xffff0000u;
    }));
    const unsigned oldCalls = state->calls;
    state->bitmap = HeaderBitmap(0, 0, 255);
    // The bundle identity and requested dimensions have not changed: updating
    // a column must still resolve the application's current bitmap pixels.
    fixture.header->UpdateColumn(0);
    REQUIRE(WaitHeader([&]()
    {
        return HeaderImagePixel(FindHeader<C::Image>(fixture.Button(0))) == 0xff0000ffu;
    }));
    CHECK(state->calls > oldCalls);
    CHECK(fixture.header->Column(0).GetBitmapBundle().IsSameAs(bundle));
    CHECK(fixture.Button(0) == button);
}

TEST_CASE("WinUIHeaderCtrl::InvokeKeepsLogicalIdentityAndRetiredProvidersAreInert",
          "[winui-headerctrl][native-headerctrl][native-events]")
{
    unsigned clicks = 0;
    int expectedColumn = 1;
    NativeHeaderFixture fixture;
    auto* const header = fixture.header;
    fixture.AddColumns();
    fixture.Show();
    REQUIRE(WaitHeader([&]() { return HeaderMatches(fixture); }));
    header->Bind(wxEVT_HEADER_CLICK, [&](wxHeaderCtrlEvent& event)
    {
        ++clicks;
        CHECK(event.GetColumn() == expectedColumn);
        CHECK(event.GetEventObject() == header);
        CHECK(event.GetId() == header->GetId());
    });
    const auto original = fixture.Button(1);
    const auto provider = HeaderInvoke(original);
    REQUIRE(provider);
    header->SetColumnsOrder(HeaderOrder({2, 0, 1}));
    REQUIRE(WaitHeader([&]() { return HeaderMatches(fixture); }));
    provider.Invoke();
    REQUIRE(WaitHeader([&]() { return clicks == 1; }));
    CHECK_FALSE(header->Column(1).IsSortKey());
    header->Enable(false);
    REQUIRE(WaitHeader([&]() { return !original.IsEnabled(); }));
    InvokeInactiveHeader(provider);
    CHECK(clicks == 1);
    header->Enable(true);
    header->DeleteColumn(1);
    header->InsertColumn(wxHeaderColumnSimple("Replacement", header->FromDIP(130)), 1);
    REQUIRE(WaitHeader([&]()
    {
        return HeaderMatches(fixture) && fixture.Button(1) != original;
    }));
    InvokeInactiveHeader(provider);
    CHECK(clicks == 1);
    const auto replacement = HeaderInvoke(fixture.Button(1));
    REQUIRE(replacement);
    replacement.Invoke();
    REQUIRE(WaitHeader([&]() { return clicks == 2; }));
    fixture.frame.reset();
    fixture.header = nullptr;
    InvokeInactiveHeader(replacement);
    InvokeInactiveHeader(provider);
    wxTheApp->ProcessPendingEvents();
    CHECK(clicks == 2);
}

TEST_CASE("WinUIHeaderCtrl::InvokeCanReplaceColumnsOrDestroyItsOwner",
          "[winui-headerctrl][native-headerctrl][native-lifetime]")
{
    const bool destroy = GENERATE(false, true);
    CAPTURE(destroy);
    unsigned clicks = 0;
    NativeHeaderFixture fixture;
    auto* const header = fixture.header;
    fixture.AddColumns();
    fixture.Show();
    REQUIRE(WaitHeader([&]() { return HeaderMatches(fixture); }));
    const auto original = fixture.Button(0);
    const auto provider = HeaderInvoke(original);
    REQUIRE(provider);
    wxWeakRef<wxWindow> weakHeader(header);
    header->Bind(wxEVT_HEADER_CLICK, [&](wxHeaderCtrlEvent& event)
    {
        ++clicks;
        CHECK(event.GetColumn() == 0);
        // Do not Skip after synchronously destroying the event target: there
        // must be no request to continue wx dispatch through a deleted chain.
        if ( destroy )
        {
            fixture.header = nullptr;
            // Destroying the event owner also destroys this bound functor.
            // Do not read its reference captures after the synchronous delete.
            fixture.frame.reset();
        }
        else
        {
            header->DeleteAllColumns();
            header->AppendColumn(wxHeaderColumnSimple("During invoke", header->FromDIP(120)));
        }
    });
    provider.Invoke();
    REQUIRE(WaitHeader([&]() { return clicks == 1; }));
    if ( destroy ) CHECK_FALSE(weakHeader);
    else
    {
        REQUIRE(WaitHeader([&]()
        {
            return HeaderMatches(fixture) && fixture.Button(0) != original;
        }));
        CHECK(header->GetColumnCount() == 1);
        CHECK(header->Column(0).GetTitle() == "During invoke");
    }
    InvokeInactiveHeader(provider);
    wxTheApp->ProcessPendingEvents();
    CHECK(clicks == 1);
}

TEST_CASE("WinUIHeaderCtrl::ResizeUpdatesRealWidthAndHonoursVeto",
          "[winui-headerctrl][native-headerctrl][native-header-gestures]")
{
    const bool rtl = GENERATE(false, true);
    const int vetoStage = GENERATE(0, 1, 2); // None, BEGIN_RESIZE, RESIZING.
    CAPTURE(rtl, vetoStage);
    unsigned begins = 0, changes = 0, ends = 0, cancelled = 0;
    NativeHeaderFixture fixture(rtl);
    auto* const header = fixture.header;
    fixture.AddColumns();
    const int oldWidth = header->Column(0).GetWidth();
    const int requested = oldWidth + header->FromDIP(35);
    fixture.Show();
    REQUIRE(WaitHeader([&]() { return HeaderMatches(fixture); }));
    const auto first = fixture.Button(0);
    header->Bind(wxEVT_HEADER_BEGIN_RESIZE, [&](wxHeaderCtrlEvent& event)
    {
        ++begins;
        CHECK(event.GetColumn() == 0);
        if ( vetoStage == 1 ) event.Veto(); else event.Skip();
    });
    header->Bind(wxEVT_HEADER_RESIZING, [&](wxHeaderCtrlEvent& event)
    {
        ++changes;
        CHECK(event.GetColumn() == 0);
        CHECK(event.GetWidth() == requested);
        if ( vetoStage == 2 ) event.Veto(); else event.Skip();
    });
    header->Bind(wxEVT_HEADER_END_RESIZE, [&](wxHeaderCtrlEvent& event)
    {
        ++ends;
        CHECK(event.GetColumn() == 0);
        CHECK(event.GetWidth() == requested);
        event.Skip();
    });
    header->Bind(wxEVT_HEADER_DRAGGING_CANCELLED, [&](wxHeaderCtrlEvent& event)
    {
        ++cancelled;
        CHECK(event.GetColumn() == 0);
    });
    HeaderMouse(header, wxEVT_LEFT_DOWN, oldWidth, true);
    CHECK(begins == 1);
    if ( vetoStage == 1 )
    {
        CHECK_FALSE(header->HasCapture());
        CHECK(changes == 0);
    }
    else
    {
        REQUIRE(header->HasCapture());
        HeaderMouse(header, wxEVT_MOTION, requested, true);
        CHECK(changes == 1);
        if ( vetoStage == 0 ) HeaderMouse(header, wxEVT_LEFT_UP, requested);
        CHECK_FALSE(header->HasCapture());
    }
    CHECK(ends == (vetoStage == 0 ? 1u : 0u));
    CHECK(cancelled == (vetoStage == 2 ? 1u : 0u));
    CHECK(header->Column(0).GetWidth() == (vetoStage == 0 ? requested : oldWidth));
    REQUIRE(WaitHeader([&]() { return HeaderMatches(fixture); }));
    CHECK(fixture.Button(0) == first);
}

TEST_CASE("WinUIHeaderCtrl::ReorderHasNativeMarkersAndHonoursEndVeto",
          "[winui-headerctrl][native-headerctrl][native-header-gestures]")
{
    const bool rtl = GENERATE(false, true);
    const bool veto = GENERATE(false, true);
    CAPTURE(rtl, veto);
    unsigned begins = 0, ends = 0;
    NativeHeaderFixture fixture(rtl);
    auto* const header = fixture.header;
    fixture.AddColumns();
    fixture.Show();
    REQUIRE(WaitHeader([&]() { return HeaderMatches(fixture); }));
    const auto first = fixture.Button(0);
    const int target = header->Column(0).GetWidth() + header->Column(1).GetWidth() +
                       header->Column(2).GetWidth() / 2;
    header->Bind(wxEVT_HEADER_BEGIN_REORDER, [&](wxHeaderCtrlEvent& event)
    {
        ++begins;
        CHECK(event.GetColumn() == 0);
        event.Skip();
    });
    header->Bind(wxEVT_HEADER_END_REORDER, [&](wxHeaderCtrlEvent& event)
    {
        ++ends;
        CHECK(event.GetColumn() == 0);
        CHECK(event.GetNewOrder() == 2);
        if ( veto ) event.Veto(); else event.Skip();
    });
    HeaderMouse(header, wxEVT_LEFT_DOWN, header->FromDIP(25), true);
    REQUIRE(header->HasCapture());
    HeaderMouse(header, wxEVT_MOTION, target, true);
    REQUIRE(WaitHeader([&]()
    {
        return HeaderMarkerVisible(fixture, L"wxHeader.ReorderGhost") &&
               HeaderMarkerVisible(fixture, L"wxHeader.ReorderMarker");
    }));
    HeaderMouse(header, wxEVT_LEFT_UP, target);
    CHECK_FALSE(header->HasCapture());
    CHECK(begins == 1);
    CHECK(ends == 1);
    CHECK(header->GetColumnsOrder() == (veto ? HeaderOrder({0, 1, 2}) : HeaderOrder({1, 2, 0})));
    REQUIRE(WaitHeader([&]()
    {
        return HeaderMatches(fixture) &&
               !HeaderMarkerVisible(fixture, L"wxHeader.ReorderGhost") &&
               !HeaderMarkerVisible(fixture, L"wxHeader.ReorderMarker");
    }));
    CHECK(fixture.Button(0) == first);
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_HEADERCTRL
