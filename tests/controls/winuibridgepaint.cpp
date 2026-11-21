///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuibridgepaint.cpp
// Purpose:     Backdrop erase ownership and native bridge repaint coverage
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "wx/app.h"
#if wxUSE_AUI
    #include "wx/aui/auibook.h"
    #include "wx/aui/dockart.h"
#endif
#include "wx/button.h"
#include "wx/dcclient.h"
#include "wx/frame.h"
#if wxUSE_GRID
    #include "wx/grid.h"
#endif
#if wxUSE_HTML
    #include "wx/html/htmlwin.h"
#endif
#if wxUSE_RICHTEXT
    #include "wx/richtext/richtextctrl.h"
#endif
#if wxUSE_STC
    #include "wx/stc/stc.h"
#endif
#include "wx/msw/wrapwin.h"
#include "wx/utils.h"
#include "wx/window.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "opaque-paint-test-access.h"
#include "host-geometry-test-access.h"

#include <uxtheme.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

#include <algorithm>
#include <array>

namespace
{

class BridgePaintDib final
{
public:
    ~BridgePaintDib()
    {
        if ( m_oldBitmap )
            ::SelectObject(dc, m_oldBitmap);
        if ( m_bitmap )
            ::DeleteObject(m_bitmap);
        if ( dc )
            ::DeleteDC(dc);
    }

    bool Create(int width, int height)
    {
        if ( width <= 0 || height <= 0 )
            return false;
        dc = ::CreateCompatibleDC(nullptr);
        if ( !dc )
            return false;
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = width;
        info.bmiHeader.biHeight = -height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        void *bits = nullptr;
        m_bitmap = ::CreateDIBSection(
            dc, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
        if ( !m_bitmap || !bits )
            return false;
        m_oldBitmap = ::SelectObject(dc, m_bitmap);
        if ( !m_oldBitmap || m_oldBitmap == HGDI_ERROR )
        {
            m_oldBitmap = nullptr;
            return false;
        }
        pixels = static_cast<RGBQUAD *>(bits);
        pixelCount = static_cast<size_t>(width) * height;
        return true;
    }

    HDC dc = nullptr;
    RGBQUAD *pixels = nullptr;
    size_t pixelCount = 0;

private:
    HBITMAP m_bitmap = nullptr;
    HGDIOBJ m_oldBitmap = nullptr;
};

void DrainBridgePaint()
{
    for ( unsigned i = 0; i != 24; ++i )
    {
        wxYield();
        wxMilliSleep(2);
    }
}

HWND NativeHandle(const wxWindow& window)
{
    return reinterpret_cast<HWND>(window.GetHWND());
}

} // namespace

TEST_CASE("WinUIBridgePaint::BackdropErasePreservesOwnedPixels",
          "[opaque-gdi][winui-bridge-paint]")
{
    const wxBackgroundStyle style =
        GENERATE(wxBG_STYLE_PAINT, wxBG_STYLE_TRANSPARENT);
    CAPTURE(style);
    wxFrame frame(nullptr, wxID_ANY, "backdrop erase ownership",
                  wxDefaultPosition, wxSize(240, 160));
    wxWindow canvas;
    // Transparent background must be requested before native creation.
    REQUIRE(canvas.SetBackgroundStyle(style));
    REQUIRE(canvas.Create(&frame, wxID_ANY, wxPoint(10, 10), wxSize(80, 60)));
    const HWND frameHwnd = NativeHandle(frame);
    const HWND canvasHwnd = NativeHandle(canvas);
    REQUIRE(::SetPropW(frameHwnd, L"wxWinUIBackdropTransparent",
                       reinterpret_cast<HANDLE>(1)));
    RECT client{};
    REQUIRE(::GetClientRect(canvasHwnd, &client));
    BridgePaintDib buffer;
    REQUIRE(buffer.Create(client.right, client.bottom));
    const RGBQUAD red{0, 0, 255, 255};
    std::fill_n(buffer.pixels, buffer.pixelCount, red);

    CHECK(::SendMessage(canvasHwnd, WM_ERASEBKGND,
                        reinterpret_cast<WPARAM>(buffer.dc), 0) == TRUE);
    ::GdiFlush();
    CHECK(std::all_of(buffer.pixels, buffer.pixels + buffer.pixelCount,
        [](const RGBQUAD& pixel)
        {
            return pixel.rgbRed == 255 && pixel.rgbGreen == 0 &&
                   pixel.rgbBlue == 0 && pixel.rgbReserved == 255;
        }));
    CHECK(::GetPropW(frameHwnd, L"wxWinUIBackdropTransparent") != nullptr);
    CHECK(::GetPropW(canvasHwnd, L"wxWinUIBackdropTransparent") != nullptr);
}

TEST_CASE("WinUIBridgePaint::EraseInvalidatesOnlyIntersectingClassicClients",
          "[opaque-gdi][winui-bridge-paint]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    wxFrame frame(nullptr, wxID_ANY, "bridge erase coverage",
                  wxDefaultPosition, wxSize(420, 260));
    frame.SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    unsigned paints = 0;
    wxWindow canvasA(&frame, wxID_ANY, wxPoint(20, 20), wxSize(80, 60));
    wxWindow canvasB(&frame, wxID_ANY, wxPoint(160, 20), wxSize(80, 60));
    wxWindow hidden(&frame, wxID_ANY, wxPoint(20, 20), wxSize(80, 60));
    REQUIRE(canvasA.SetBackgroundStyle(wxBG_STYLE_ERASE));
    REQUIRE(canvasB.SetBackgroundStyle(wxBG_STYLE_PAINT));
    REQUIRE(hidden.SetBackgroundStyle(wxBG_STYLE_PAINT));
    hidden.Hide();
    canvasA.Bind(wxEVT_PAINT, [&](wxPaintEvent&)
    {
        wxPaintDC dc(&canvasA);
        ++paints;
    });
    // Overlap the erased area deliberately: the XAML slot must be excluded
    // by ownership, not just because its rectangle happens to be elsewhere.
    wxButton native(&frame, wxID_ANY, "native", wxPoint(20, 20), wxSize(80, 60));
    const HWND frameHwnd = NativeHandle(frame);
    ::SetWindowLongPtr(frameHwnd, GWL_EXSTYLE,
        ::GetWindowLongPtr(frameHwnd, GWL_EXSTYLE) |
        WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
    frame.Move(wxPoint(-32000, -32000));
    frame.ShowWithoutActivating();
    DrainBridgePaint();
    REQUIRE(::SetPropW(frameHwnd, L"wxWinUIBackdropTransparent",
                       reinterpret_cast<HANDLE>(1)));

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(&frame);
    REQUIRE(host);
    const HWND bridge = host->GetBridgeHwnd();
    REQUIRE(bridge);
    RECT client{};
    REQUIRE(::GetClientRect(bridge, &client));
    BridgePaintDib buffer;
    REQUIRE(buffer.Create(client.right, client.bottom));
    const DWORD layout =
        (::GetWindowLongPtr(bridge, GWL_EXSTYLE) & WS_EX_LAYOUTRTL)
            ? LAYOUT_RTL : 0;
    REQUIRE(::SetLayout(buffer.dc, layout) != GDI_ERROR);

    const HWND a = NativeHandle(canvasA);
    const HWND b = NativeHandle(canvasB);
    const HWND hiddenHwnd = NativeHandle(hidden);
    const HWND nativeHwnd = NativeHandle(native);
    for ( HWND hwnd : {frameHwnd, bridge, a, b, hiddenHwnd, nativeHwnd} )
        REQUIRE(::ValidateRect(hwnd, nullptr));
    const unsigned paintsBefore = paints;
    const RECT expected{5, 7, 25, 22};
    RECT bridgeClip = expected;
    ::MapWindowPoints(a, bridge, reinterpret_cast<POINT *>(&bridgeClip), 2);
    const int intersection = ::IntersectClipRect(buffer.dc,
        bridgeClip.left, bridgeClip.top, bridgeClip.right, bridgeClip.bottom);
    // Windows can report COMPLEXREGION from IntersectClipRect even when the
    // installed clip contains one rectangle. Check the actual clip shape and
    // logical coordinates instead of relying on that return classification.
    REQUIRE((intersection == SIMPLEREGION || intersection == COMPLEXREGION));
    RECT actualClip{};
    REQUIRE(::GetClipBox(buffer.dc, &actualClip) == SIMPLEREGION);
    REQUIRE(::EqualRect(&actualClip, &bridgeClip));

    // Exercise the installed native bridge subclass, not the erase helper.
    REQUIRE(::SendMessage(bridge, WM_ERASEBKGND,
                          reinterpret_cast<WPARAM>(buffer.dc), 0) == TRUE);
    CHECK(paints == paintsBefore);
    RECT invalid{};
    REQUIRE(::GetUpdateRect(a, &invalid, FALSE));
    CHECK(::EqualRect(&invalid, &expected));
    CHECK_FALSE(::GetUpdateRect(b, nullptr, FALSE));
    CHECK_FALSE(::GetUpdateRect(hiddenHwnd, nullptr, FALSE));
    CHECK_FALSE(::GetUpdateRect(nativeHwnd, nullptr, FALSE));
    REQUIRE(::UpdateWindow(a));
    CHECK(paints == paintsBefore + 1);
    CHECK_FALSE(::GetUpdateRect(a, nullptr, FALSE));

    REQUIRE(::IntersectClipRect(buffer.dc, 0, 0, 0, 0) == NULLREGION);
    ::SendMessage(bridge, WM_ERASEBKGND,
                  reinterpret_cast<WPARAM>(buffer.dc), 0);
    CHECK(paints == paintsBefore + 1);
    for ( HWND hwnd : {a, b, hiddenHwnd, nativeHwnd} )
        CHECK_FALSE(::GetUpdateRect(hwnd, nullptr, FALSE));
}

TEST_CASE("WinUIBridgePaint::ErasePreservesVisibleOpaqueClients",
          "[opaque-gdi][winui-bridge-paint]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    wxFrame frame(nullptr, wxID_ANY, "bridge opaque coverage",
                  wxDefaultPosition, wxSize(420, 260));
    frame.SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    wxWindow opaque(&frame, wxID_ANY, wxPoint(20, 20), wxSize(80, 60));
    REQUIRE(opaque.SetBackgroundStyle(wxBG_STYLE_PAINT));
    wxWindow transparent;
    REQUIRE(transparent.SetBackgroundStyle(wxBG_STYLE_TRANSPARENT));
    REQUIRE(transparent.Create(&frame, wxID_ANY,
                               wxPoint(160, 20), wxSize(80, 60)));
    wxButton native(&frame, wxID_ANY, "native",
                    wxPoint(270, 130), wxSize(80, 30));
    unsigned opaquePaints = 0;
    unsigned transparentPaints = 0;
    opaque.Bind(wxEVT_PAINT, [&](wxPaintEvent&)
    {
        wxPaintDC dc(&opaque);
        ++opaquePaints;
    });
    transparent.Bind(wxEVT_PAINT, [&](wxPaintEvent&)
    {
        wxPaintDC dc(&transparent);
        ++transparentPaints;
    });
    const HWND frameHwnd = NativeHandle(frame);
    const HWND opaqueHwnd = NativeHandle(opaque);
    const HWND transparentHwnd = NativeHandle(transparent);
    const HWND nativeHwnd = NativeHandle(native);
    ::SetWindowLongPtr(frameHwnd, GWL_EXSTYLE,
        ::GetWindowLongPtr(frameHwnd, GWL_EXSTYLE) |
        WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
    frame.Move(wxPoint(-32000, -32000));
    frame.ShowWithoutActivating();
    DrainBridgePaint();
    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(&frame);
    REQUIRE(host);
    const HWND bridge = host->GetBridgeHwnd();
    REQUIRE(bridge);
    RECT client{};
    REQUIRE(::GetClientRect(bridge, &client));
    BridgePaintDib buffer;
    REQUIRE(buffer.Create(client.right, client.bottom));
    const DWORD layout =
        (::GetWindowLongPtr(bridge, GWL_EXSTYLE) & WS_EX_LAYOUTRTL)
            ? LAYOUT_RTL : 0;
    REQUIRE(::SetLayout(buffer.dc, layout) != GDI_ERROR);
    RECT clip{10, 10, 250, 170};
    ::MapWindowPoints(frameHwnd, bridge, reinterpret_cast<POINT *>(&clip), 2);
    REQUIRE(::IntersectClipRect(buffer.dc, clip.left, clip.top,
                                clip.right, clip.bottom) != ERROR);
    struct ClipRegion
    {
        HRGN handle = ::CreateRectRgn(0, 0, 0, 0);
        ~ClipRegion() { if ( handle ) ::DeleteObject(handle); }
    } before, after;
    REQUIRE(before.handle);
    REQUIRE(after.handle);
    REQUIRE(::GetClipRgn(buffer.dc, before.handle) == 1);
    RECT formerOpaque{};
    REQUIRE(::GetClientRect(opaqueHwnd, &formerOpaque));
    ::MapWindowPoints(opaqueHwnd, bridge,
                      reinterpret_cast<POINT *>(&formerOpaque), 2);

    // Map a one-pixel rectangle, not an edge coordinate: RTL reflection of
    // a pixel is width-right, whereas reflection of its left edge is width-x.
    // Raw DIB access also checks alpha, including pixels outside the clip.
    const auto checkPixel = [&](HWND source, POINT point, bool preserved)
    {
        RECT pixelRect{point.x, point.y, point.x + 1, point.y + 1};
        ::MapWindowPoints(source, bridge,
                          reinterpret_cast<POINT *>(&pixelRect), 2);
        const LONG x = (layout & LAYOUT_RTL)
                        ? client.right - pixelRect.right : pixelRect.left;
        const LONG y = pixelRect.top;
        REQUIRE((x >= 0 && x < client.right));
        REQUIRE((y >= 0 && y < client.bottom));
        const RGBQUAD& pixel = buffer.pixels[
            static_cast<size_t>(y) * client.right + x];
        CHECK(pixel.rgbRed == (preserved ? 255 : 0));
        CHECK(pixel.rgbGreen == 0);
        CHECK(pixel.rgbBlue == 0);
        if ( preserved )
            CHECK(pixel.rgbReserved == 255);
    };
    const auto checkRectangle = [&](const RECT& rect, bool preserved)
    {
        for ( POINT point : {POINT{rect.left, rect.top},
                             POINT{rect.right - 1, rect.top},
                             POINT{rect.left, rect.bottom - 1},
                             POINT{rect.right - 1, rect.bottom - 1},
                             POINT{(rect.left + rect.right) / 2,
                                   (rect.top + rect.bottom) / 2}} )
        {
            REQUIRE(::PtInRect(&clip, point));
            checkPixel(bridge, point, preserved);
        }
    };

    for ( unsigned phase = 0; phase != 3; ++phase )
    {
        CAPTURE(phase);
        if ( phase == 1 )
            opaque.Hide();
        else if ( phase == 2 )
        {
            opaque.Move(wxPoint(20, 95));
            opaque.Show();
        }
        DrainBridgePaint();
        REQUIRE(::SetPropW(frameHwnd, L"wxWinUIBackdropTransparent",
                           reinterpret_cast<HANDLE>(1)));
        ::GdiFlush();
        const RGBQUAD red{0, 0, 255, 255};
        std::fill_n(buffer.pixels, buffer.pixelCount, red);
        for ( HWND hwnd : {frameHwnd, bridge, opaqueHwnd,
                           transparentHwnd, nativeHwnd} )
            REQUIRE(::ValidateRect(hwnd, nullptr));
        const unsigned opaqueBefore = opaquePaints;
        const unsigned transparentBefore = transparentPaints;

        REQUIRE(::SendMessage(bridge, WM_ERASEBKGND,
                              reinterpret_cast<WPARAM>(buffer.dc), 0) == TRUE);
        ::GdiFlush();
        CHECK(opaquePaints == opaqueBefore);
        CHECK(transparentPaints == transparentBefore);
        CHECK(::GetLayout(buffer.dc) == layout);
        REQUIRE(::GetClipRgn(buffer.dc, after.handle) == 1);
        CHECK(::EqualRgn(before.handle, after.handle));
        // Corners and centre catch both too-wide and off-by-one exclusions.
        checkRectangle(formerOpaque, phase == 0);
        if ( phase == 2 )
        {
            RECT movedOpaque{};
            REQUIRE(::GetClientRect(opaqueHwnd, &movedOpaque));
            ::MapWindowPoints(opaqueHwnd, bridge,
                              reinterpret_cast<POINT *>(&movedOpaque), 2);
            checkRectangle(movedOpaque, true);
        }
        checkPixel(transparentHwnd, POINT{40, 30}, false);
        checkPixel(frameHwnd, POINT{120, 110}, false);
        checkPixel(frameHwnd, POINT{5, 5}, true);
        CHECK_FALSE(::GetUpdateRect(opaqueHwnd, nullptr, FALSE));
        CHECK_FALSE(::GetUpdateRect(nativeHwnd, nullptr, FALSE));
        RECT transparentClient{};
        RECT invalid{};
        REQUIRE(::GetClientRect(transparentHwnd, &transparentClient));
        REQUIRE(::GetUpdateRect(transparentHwnd, &invalid, FALSE));
        CHECK(::EqualRect(&invalid, &transparentClient));
        REQUIRE(::UpdateWindow(transparentHwnd));
        CHECK(transparentPaints == transparentBefore + 1);
        CHECK(opaquePaints == opaqueBefore);
        CHECK_FALSE(::GetUpdateRect(transparentHwnd, nullptr, FALSE));
    }
}

#if wxUSE_GRID

TEST_CASE("WinUIBridgePaint::GridSurfacesStayPaintedDuringBridgeErase",
          "[opaque-gdi][winui-bridge-paint][winui-grid-repaint]")
{
    const bool rtl = GENERATE(false, true);
    const bool empty = GENERATE(false, true);
    CAPTURE(rtl, empty);
    std::array<unsigned, 4> paints{};
    wxFrame frame(nullptr, wxID_ANY, "grid bridge erase ownership",
                  wxDefaultPosition, wxSize(680, 380));
    frame.SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    wxGrid grid(&frame, wxID_ANY, wxPoint(20, 20), wxSize(460, 260));
    REQUIRE(grid.CreateGrid(empty ? 0 : 2, empty ? 0 : 2));
    grid.SetRowLabelSize(60);
    grid.SetColLabelSize(30);
    // Keep an independent native peer in the same production island. The
    // grid's four real public subwindows now own native visual slots too.
    wxButton native(&frame, wxID_ANY, "native",
                    wxPoint(520, 290), wxSize(90, 30));
    const std::array<wxWindow *, 4> surfaces = {
        grid.GetGridWindow(), grid.GetGridRowLabelWindow(),
        grid.GetGridColLabelWindow(), grid.GetGridCornerLabelWindow()
    };
    const std::array<const char *, 4> names = {"body", "row", "column", "corner"};
    for ( size_t i = 0; i < surfaces.size(); ++i )
    {
        REQUIRE(surfaces[i]);
        surfaces[i]->Bind(wxEVT_PAINT, [&, i](wxPaintEvent& event)
        {
            ++paints[i];
            event.Skip(); // retain the real wxGrid paint implementation
        });
    }
    const HWND frameHwnd = NativeHandle(frame);
    ::SetWindowLongPtr(frameHwnd, GWL_EXSTYLE,
        ::GetWindowLongPtr(frameHwnd, GWL_EXSTYLE) |
        WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
    frame.Move(wxPoint(-32000, -32000));
    frame.ShowWithoutActivating();
    DrainBridgePaint();
    REQUIRE(::SetPropW(frameHwnd, L"wxWinUIBackdropTransparent",
                       reinterpret_cast<HANDLE>(1)));
    const auto host = wxWinUITopLevelHost::FindForTLW(&frame);
    REQUIRE(host);
    const HWND bridge = host->GetBridgeHwnd();
    REQUIRE(bridge);
    RECT bridgeClient{};
    REQUIRE(::GetClientRect(bridge, &bridgeClient));
    BridgePaintDib buffer;
    REQUIRE(buffer.Create(bridgeClient.right, bridgeClient.bottom));
    const DWORD layout =
        (::GetWindowLongPtr(bridge, GWL_EXSTYLE) & WS_EX_LAYOUTRTL)
            ? LAYOUT_RTL : 0;
    REQUIRE(::SetLayout(buffer.dc, layout) != GDI_ERROR);

    for ( size_t i = 0; i < surfaces.size(); ++i )
    {
        INFO("grid surface: " << names[i]);
        CAPTURE(surfaces[i]->GetBackgroundStyle());
        const HWND surface = NativeHandle(*surfaces[i]);
        REQUIRE(::IsWindowVisible(surface));
        REQUIRE(wxWinUITopLevelHost::FindSlotOwner(surfaces[i]));
        RECT client{};
        REQUIRE(::GetClientRect(surface, &client));
        REQUIRE(client.right > 2);
        REQUIRE(client.bottom > 2);
        ::GdiFlush();
        const RGBQUAD red{0, 0, 255, 255};
        std::fill_n(buffer.pixels, buffer.pixelCount, red);
        for ( HWND hwnd : {frameHwnd, NativeHandle(grid), bridge,
                           NativeHandle(native)} )
            REQUIRE(::ValidateRect(hwnd, nullptr));
        for ( wxWindow *window : surfaces )
            REQUIRE(::ValidateRect(NativeHandle(*window), nullptr));
        const auto paintsBefore = paints;
        RECT clip = client;
        ::MapWindowPoints(surface, bridge,
                          reinterpret_cast<POINT *>(&clip), 2);
        const int saved = ::SaveDC(buffer.dc);
        REQUIRE(saved != 0);
        REQUIRE(::IntersectClipRect(buffer.dc, clip.left, clip.top,
                                    clip.right, clip.bottom) != ERROR);
        REQUIRE(::SendMessage(bridge, WM_ERASEBKGND,
                              reinterpret_cast<WPARAM>(buffer.dc), 0) == TRUE);
        REQUIRE(::RestoreDC(buffer.dc, saved));
        ::GdiFlush();

        // Native slots must expose the Mica backing surface without a GDI
        // repair or an opaque legacy repaint covering their XAML children.
        // Inspect BEFORE dispatching any WM_PAINT.
        CHECK(paints == paintsBefore);
        for ( const POINT point : {
                  POINT{1, 1}, POINT{client.right - 2, 1},
                  POINT{1, client.bottom - 2},
                  POINT{client.right - 2, client.bottom - 2},
                  POINT{client.right / 2, client.bottom / 2}} )
        {
            RECT pixelRect{point.x, point.y, point.x + 1, point.y + 1};
            ::MapWindowPoints(surface, bridge,
                              reinterpret_cast<POINT *>(&pixelRect), 2);
            const LONG x = (layout & LAYOUT_RTL)
                ? bridgeClient.right - pixelRect.right : pixelRect.left;
            const LONG y = pixelRect.top;
            REQUIRE((x >= 0 && x < bridgeClient.right));
            REQUIRE((y >= 0 && y < bridgeClient.bottom));
            const RGBQUAD& pixel = buffer.pixels[
                static_cast<size_t>(y) * bridgeClient.right + x];
            CHECK(pixel.rgbRed == 0);
            CHECK(pixel.rgbGreen == 0);
            CHECK(pixel.rgbBlue == 0);
            CHECK(pixel.rgbReserved == 0);
        }
        for ( wxWindow *window : surfaces )
            CHECK_FALSE(::GetUpdateRect(NativeHandle(*window), nullptr, FALSE));
    }
}

TEST_CASE("WinUIBridgePaint::NativeGridLabelsCoverTheirWholeSurface",
          "[opaque-gdi][winui-bridge-paint][winui-grid-repaint]")
{
    const bool rtl = GENERATE(false, true);
    const bool empty = GENERATE(false, true);
    CAPTURE(rtl, empty);
    wxFrame frame(nullptr, wxID_ANY, "grid label opaque painting",
                  wxDefaultPosition, wxSize(680, 380));
    frame.SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    wxGrid grid(&frame, wxID_ANY, wxPoint(20, 20), wxSize(460, 260));
    REQUIRE(grid.CreateGrid(empty ? 0 : 2, empty ? 0 : 2));
    grid.SetRowLabelSize(60);
    grid.SetColLabelSize(30);
    grid.SetUseNativeColLabels(false);
    wxButton native(&frame, wxID_ANY, "native",
                    wxPoint(520, 290), wxSize(90, 30));
    frame.ShowWithoutActivating();
    DrainBridgePaint();
    REQUIRE(::SetPropW(NativeHandle(frame), L"wxWinUIBackdropTransparent",
                       reinterpret_cast<HANDLE>(1)));

    struct Observation
    {
        WXHWND target = nullptr;
        unsigned count = 0;
    } observation;
    const auto observer = [](WXHWND window, WXHDC, void *,
                             const wxRect&, void *context)
    {
        auto& state = *static_cast<Observation *>(context);
        if ( window == state.target )
            ++state.count;
    };
    struct ObserverGuard
    {
        ~ObserverGuard()
        {
            wxWinUIOpaquePaintTestAccess::SetObserver(nullptr, nullptr);
        }
    } observerGuard;
    wxWinUIOpaquePaintTestAccess::SetObserver(observer, &observation);

    const std::array<wxWindow *, 3> surfaces = {
        grid.GetGridRowLabelWindow(), grid.GetGridColLabelWindow(),
        grid.GetGridCornerLabelWindow()
    };
    const std::array<const char *, 3> names = {"row", "column", "corner"};
    for ( size_t i = 0; i < surfaces.size(); ++i )
    {
        INFO(names[i]);
        const HWND surface = NativeHandle(*surfaces[i]);
        auto host = wxWinUITopLevelHost::FindSlotOwner(surfaces[i]);
        REQUIRE(host);
        auto slot = host->FindSlot(surfaces[i]);
        REQUIRE(slot);
        auto root = slot->GetContent().try_as<
            winrt::Microsoft::UI::Xaml::Controls::Grid>();
        REQUIRE(root);
        REQUIRE(root.Children().Size() == 1);
        auto canvas = root.Children().GetAt(0).try_as<
            winrt::Microsoft::UI::Xaml::Controls::Canvas>();
        REQUIRE(canvas);
        RECT client{};
        REQUIRE(::GetClientRect(surface, &client));
        REQUIRE(client.right > 20);
        REQUIRE(client.bottom > 20);
        // In a nonempty grid the row/column samples are beyond the last label,
        // not on a renderer's border/text. The empty grid exercises its early
        // return, and the corner sample exercises a border-only renderer.
        const POINT logicalSample{
            i == 1 ? client.right - 10 : client.right / 2,
            i == 0 ? client.bottom - 10 : client.bottom / 2
        };
        if ( !empty && i == 0 )
            REQUIRE(logicalSample.y > grid.GetRowSize(0) + grid.GetRowSize(1));
        if ( !empty && i == 1 )
            REQUIRE(logicalSample.x > grid.GetColSize(0) + grid.GetColSize(1));
        for ( const bool partial : {false, true} )
        {
            CAPTURE(partial);
            // Changing colour also rejects a buffer that merely republishes
            // pixels seeded from the previous frame without painting them.
            const wxColour colour = partial ? wxColour(113, 47, 19)
                                             : wxColour(17, 61, 113);
            grid.SetLabelBackgroundColour(colour);
            REQUIRE(::ValidateRect(surface, nullptr));
            observation = {};
            observation.target = surfaces[i]->GetHWND();
            const wxRect invalid = partial
                ? wxRect(logicalSample.x - 3, logicalSample.y - 3, 8, 6)
                : wxRect(0, 0, client.right, client.bottom);
            surfaces[i]->Refresh(true, partial ? &invalid : nullptr);
            REQUIRE(::UpdateWindow(surface)); // actual wxGrid WM_PAINT handler
            DrainBridgePaint();
            // No classic opaque buffer is republished over the real XAML
            // surface. Its retained background covers even the empty area
            // beyond the last label, for full and partial invalidations.
            CHECK(observation.count == 0);
            CHECK(slot->GetContent() == root);
            CHECK(root.Children().GetAt(0) == canvas);
            auto brush = canvas.Background().try_as<
                winrt::Microsoft::UI::Xaml::Media::SolidColorBrush>();
            REQUIRE(brush);
            CHECK(brush.Color().R == colour.Red());
            CHECK(brush.Color().G == colour.Green());
            CHECK(brush.Color().B == colour.Blue());
            CHECK(brush.Color().A == 255);
            CHECK_FALSE(::GetUpdateRect(surface, nullptr, FALSE));
        }
    }
}

#endif // wxUSE_GRID

#if wxUSE_AUI

TEST_CASE("WinUIBridgePaint::AuiNotebookOwnsItsDockBackground",
          "[opaque-gdi][winui-bridge-paint][winui-aui-repaint]")
{
    const bool rtl = GENERATE(false, true);
    const bool empty = GENERATE(false, true);
    CAPTURE(rtl, empty);
    unsigned paints = 0;
    unsigned paintRenders = 0;
    unsigned directRenders = 0;
    wxFrame frame(nullptr, wxID_ANY, "AUI dock background on Mica",
                  wxDefaultPosition, wxSize(580, 320));
    frame.SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    wxAuiNotebook book(&frame, wxID_ANY, wxPoint(20, 20), wxSize(380, 220));
    wxWindow *page = nullptr;
    if ( !empty )
    {
        page = new wxWindow(&book, wxID_ANY);
        REQUIRE(book.AddPage(page, "active", true));
    }
    wxButton native(&frame, wxID_ANY, "native", wxPoint(430, 240));
    book.Bind(wxEVT_PAINT, [&](wxPaintEvent& event)
    {
        ++paints;
        event.Skip(); // retain the real wxAuiManager painter
    });
    book.Bind(wxEVT_AUI_RENDER, [&](wxAuiManagerEvent& event)
    {
        // wxPaintDC derives from wxClientDC: testing for wxClientDC would
        // accept the bypass too. Observe the actual paint-only DC subclass.
        if ( wxDynamicCast(event.GetDC(), wxPaintDC) )
            ++paintRenders;
        else
            ++directRenders;
        event.Skip(); // retain the real dock-art rendering
    });
    frame.ShowWithoutActivating();
    DrainBridgePaint();
    REQUIRE(::SetPropW(NativeHandle(frame), L"wxWinUIBackdropTransparent",
                       reinterpret_cast<HANDLE>(1)));
    const HWND bookHwnd = NativeHandle(book);
    RECT client{};
    REQUIRE(::GetClientRect(bookHwnd, &client));
    REQUIRE(client.right > 40);
    REQUIRE(client.bottom > 40);
    const bool mirrored =
        (::GetWindowLongPtr(bookHwnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0;

    BridgePaintDib direct;
    REQUIRE(direct.Create(client.right, client.bottom));
    REQUIRE(::SetLayout(direct.dc, mirrored ? LAYOUT_RTL : 0) != GDI_ERROR);
    std::fill_n(direct.pixels, direct.pixelCount, RGBQUAD{0, 0, 255, 255});
    const unsigned beforeErase = paints;
    REQUIRE(::ValidateRect(bookHwnd, nullptr));
    REQUIRE(::SendMessage(bookHwnd, WM_ERASEBKGND,
                          reinterpret_cast<WPARAM>(direct.dc), 0) == TRUE);
    ::GdiFlush();
    CHECK(paints == beforeErase);
    const auto isRed = [](const RGBQUAD& pixel)
    {
        return pixel.rgbRed == 255 && pixel.rgbGreen == 0 &&
               pixel.rgbBlue == 0 && pixel.rgbReserved == 255;
    };
    CHECK(std::all_of(direct.pixels, direct.pixels + direct.pixelCount, isRed));

    const auto host = wxWinUITopLevelHost::FindForTLW(&frame);
    REQUIRE(host);
    const HWND bridge = host->GetBridgeHwnd();
    REQUIRE(bridge);
    RECT bridgeClient{};
    REQUIRE(::GetClientRect(bridge, &bridgeClient));
    BridgePaintDib island;
    REQUIRE(island.Create(bridgeClient.right, bridgeClient.bottom));
    const bool bridgeMirrored =
        (::GetWindowLongPtr(bridge, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0;
    REQUIRE(::SetLayout(island.dc, bridgeMirrored ? LAYOUT_RTL : 0) != GDI_ERROR);
    std::fill_n(island.pixels, island.pixelCount, RGBQUAD{0, 0, 255, 255});
    RECT bookClip = client;
    ::MapWindowPoints(bookHwnd, bridge, reinterpret_cast<POINT *>(&bookClip), 2);
    REQUIRE(::IntersectClipRect(island.dc, bookClip.left, bookClip.top,
                                bookClip.right, bookClip.bottom) != ERROR);
    REQUIRE(::SendMessage(bridge, WM_ERASEBKGND,
                          reinterpret_cast<WPARAM>(island.dc), 0) == TRUE);
    ::GdiFlush();
    CHECK(paints == beforeErase);
    CHECK(std::all_of(island.pixels, island.pixels + island.pixelCount, isRed));
    CHECK_FALSE(::GetUpdateRect(bookHwnd, nullptr, FALSE));

    // Expose real manager-owned background in the populated notebook as
    // during a page hide/layout transition. The tab remains selected; no
    // test handler replaces the notebook's actual WM_PAINT implementation.
    if ( page )
    {
        REQUIRE(book.GetSelection() == 0);
        page->Hide();
    }
    struct Observation
    {
        WXHWND target = nullptr;
        unsigned count = 0;
        bool valid = false;
        bool opaque = false;
        RGBQUAD centre{};
        wxRect bounds;
    } observation;
    const auto observer = [](WXHWND window, WXHDC, void *bufferArg,
                             const wxRect& bounds, void *context)
    {
        auto& state = *static_cast<Observation *>(context);
        if ( window != state.target )
            return;
        ++state.count;
        state.bounds = bounds;
        const auto buffer = reinterpret_cast<HPAINTBUFFER>(bufferArg);
        RGBQUAD *bits = nullptr;
        int stride = 0;
        RECT target{};
        if ( FAILED(::GetBufferedPaintBits(buffer, &bits, &stride)) || !bits ||
             FAILED(::GetBufferedPaintTargetRect(buffer, &target)) )
            return;
        const LONG width = target.right - target.left;
        const LONG height = target.bottom - target.top;
        if ( width <= 0 || height <= 0 || stride < width )
            return;
        state.opaque = true;
        for ( LONG y = 0; y < height; ++y )
            for ( LONG x = 0; x < width; ++x )
                if ( bits[static_cast<size_t>(y) * stride + x].rgbReserved != 255 )
                    state.opaque = false;
        state.centre = bits[static_cast<size_t>(height / 2) * stride + width / 2];
        state.valid = true;
    };
    struct ObserverGuard
    {
        ~ObserverGuard() { wxWinUIOpaquePaintTestAccess::SetObserver(nullptr, nullptr); }
    } observerGuard;
    wxWinUIOpaquePaintTestAccess::SetObserver(observer, &observation);
    wxAuiDockArt * const dockArt = book.GetAuiManager().GetArtProvider();
    REQUIRE(dockArt);
    for ( const bool partial : {false, true} )
    {
        CAPTURE(partial);
        const wxColour colour = partial ? wxColour(89, 31, 117)
                                         : wxColour(23, 79, 41);
        dockArt->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, colour);
        REQUIRE(::ValidateRect(bookHwnd, nullptr));
        observation = {};
        observation.target = book.GetHWND();
        // The active notebook retains visible tabs/borders. Its central
        // exposed background is used rather than demanding that the manager
        // overwrite child HWNDs. The empty notebook must paint its full area.
        const wxRect invalid = partial || !empty
            ? wxRect(client.right / 2 - 12, client.bottom / 2 - 8, 24, 16)
            : wxRect(0, 0, client.right, client.bottom);
        const wxRect physicalInvalid(
            mirrored ? client.right - invalid.GetRight() - 1 : invalid.x,
            invalid.y, invalid.width, invalid.height);
        book.Refresh(true, &invalid);
        const unsigned beforePaint = paints;
        REQUIRE(::UpdateWindow(bookHwnd));
        CHECK(paints == beforePaint + 1);
        REQUIRE(observation.count == 1);
        REQUIRE(observation.valid);
        CHECK(observation.bounds == physicalInvalid);
        CHECK(observation.opaque);
        CHECK(observation.centre.rgbRed == colour.Red());
        CHECK(observation.centre.rgbGreen == colour.Green());
        CHECK(observation.centre.rgbBlue == colour.Blue());
        CHECK(observation.centre.rgbReserved == 255);
    }

    // Exercise the native WM_SIZE -> manager OnSize -> Repaint(nullptr)
    // front door with unchanged geometry. There is deliberately no Refresh,
    // UpdateWindow or dispatcher pump here to hide direct ClientDC drawing
    // behind an unrelated later WM_PAINT.
    const wxColour resizeColour(37, 101, 173);
    dockArt->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, resizeColour);
    REQUIRE(wxWinUIShouldUseOpaquePaintBuffer(&book));
    REQUIRE(::ValidateRect(bookHwnd, nullptr));
    REQUIRE(::GetClientRect(bookHwnd, &client));
    observation = {};
    observation.target = book.GetHWND();
    paintRenders = directRenders = 0;
    const unsigned beforeSizePaint = paints;
    ::SendMessage(bookHwnd, WM_SIZE, SIZE_RESTORED,
                  MAKELPARAM(client.right, client.bottom));
    CHECK(directRenders == 0);
    CHECK(paintRenders > 0);
    CHECK(paints > beforeSizePaint);
    REQUIRE(observation.count > 0);
    REQUIRE(observation.valid);
    CHECK(observation.opaque);
    if ( empty )
    {
        CHECK(observation.centre.rgbRed == resizeColour.Red());
        CHECK(observation.centre.rgbGreen == resizeColour.Green());
        CHECK(observation.centre.rgbBlue == resizeColour.Blue());
        CHECK(observation.centre.rgbReserved == 255);
    }
    CHECK(wxWinUIShouldUseOpaquePaintBuffer(&book));
}

#endif // wxUSE_AUI

#if wxUSE_RICHTEXT || wxUSE_STC || wxUSE_HTML

TEST_CASE("WinUIBridgePaint::ClassicEditorsPublishOpaqueRefreshAndResize",
          "[opaque-gdi][winui-bridge-paint][winui-classic-editor-paint]")
{
    const char* const families[] = {
#if wxUSE_RICHTEXT
        "RichText",
#endif
#if wxUSE_STC
        "STC",
#endif
#if wxUSE_HTML
        "HTML",
#endif
    };
    const auto family = GENERATE_REF(Catch::Generators::from_range(
        families, families + WXSIZEOF(families)));
    CAPTURE(family);
    unsigned paints = 0, sizes = 0;
    wxFrame frame(nullptr, wxID_ANY, "classic editor opaque painting",
                  wxPoint(80, 80), wxSize(650, 400));
    wxWindow* editor = nullptr;
#if wxUSE_RICHTEXT
    if ( wxString(family) == "RichText" )
        editor = new wxRichTextCtrl(&frame, wxID_ANY, "Real rich text\nSecond line",
            wxPoint(20, 20), wxSize(400, 240), wxRE_MULTILINE);
#endif
#if wxUSE_STC
    if ( wxString(family) == "STC" )
    {
        auto* const stc = new wxStyledTextCtrl(&frame, wxID_ANY,
            wxPoint(20, 20), wxSize(400, 240));
        stc->SetText("Real styled text\nSecond line");
        editor = stc;
    }
#endif
#if wxUSE_HTML
    if ( wxString(family) == "HTML" )
    {
        auto* const html = new wxHtmlWindow(&frame, wxID_ANY,
            wxPoint(20, 20), wxSize(400, 240));
        REQUIRE(html->SetPage("<p>Real <b>HTML</b> content</p>"));
        editor = html;
    }
#endif
    REQUIRE(editor);
    // Also create the real island, and avoid the frame's single-child sizing
    // policy obscuring the editor's own WM_SIZE/WM_PAINT sequence.
    wxButton native(&frame, wxID_ANY, "native", wxPoint(490, 280));
    editor->Bind(wxEVT_PAINT, [&](wxPaintEvent& event)
    {
        ++paints;
        event.Skip(); // Do not replace any editor's actual paint handler.
    });
    editor->Bind(wxEVT_SIZE, [&](wxSizeEvent& event)
    {
        ++sizes;
        event.Skip();
    });
    frame.ShowWithoutActivating();
    DrainBridgePaint();
    const HWND frameHwnd = NativeHandle(frame);
    const HWND editorHwnd = NativeHandle(*editor);
    REQUIRE(::SetPropW(frameHwnd, L"wxWinUIBackdropTransparent",
                       reinterpret_cast<HANDLE>(1)));
    REQUIRE(editor->GetBackgroundStyle() == wxBG_STYLE_PAINT);
    REQUIRE(wxWinUIShouldUseOpaquePaintBuffer(editor));

    struct Observation
    {
        WXHWND target = nullptr;
        unsigned count = 0;
        bool valid = true;
        bool opaque = true;
        bool sampled = false;
        bool coversRequested = false;
        wxPoint sample;
        wxRect requested;
        wxRect bounds, targetBounds;
        RGBQUAD background{};
    } observation;
    const auto observer = [](WXHWND window, WXHDC, void* bufferArg,
                             const wxRect& bounds, void* context)
    {
        auto& state = *static_cast<Observation*>(context);
        if ( window != state.target ) return;
        ++state.count;
        state.bounds = bounds;
        const auto buffer = reinterpret_cast<HPAINTBUFFER>(bufferArg);
        RGBQUAD* bits = nullptr;
        int stride = 0;
        RECT target{};
        if ( FAILED(::GetBufferedPaintBits(buffer, &bits, &stride)) || !bits ||
             FAILED(::GetBufferedPaintTargetRect(buffer, &target)) )
        {
            state.valid = false;
            return;
        }
        const LONG width = target.right - target.left;
        const LONG height = target.bottom - target.top;
        state.targetBounds = wxRect(target.left, target.top, width, height);
        if ( width <= 0 || height <= 0 || stride < width || bounds.IsEmpty() ||
             !state.targetBounds.Contains(bounds) )
        {
            state.valid = false;
            return;
        }
        state.coversRequested |= !state.requested.IsEmpty() &&
            bounds.Contains(state.requested) && state.targetBounds.Contains(state.requested);
        for ( LONG y = 0; y < height; ++y )
            for ( LONG x = 0; x < width; ++x )
                if ( bits[static_cast<size_t>(y) * stride + x].rgbReserved != 255 )
                    state.opaque = false;
        if ( state.targetBounds.Contains(state.sample) )
        {
            state.background = bits[static_cast<size_t>(state.sample.y - target.top) *
                                    stride + state.sample.x - target.left];
            state.sampled = true;
        }
    };
    struct ObserverGuard
    {
        ~ObserverGuard() { wxWinUIOpaquePaintTestAccess::SetObserver(nullptr, nullptr); }
    } observerGuard;
    wxWinUIOpaquePaintTestAccess::SetObserver(observer, &observation);

    const std::array<wxColour, 3> colours = {
        wxColour(23, 79, 41), wxColour(54, 66, 78), wxColour(85, 53, 115)
    };
    for ( unsigned phase = 0; phase != 3; ++phase )
    {
        CAPTURE(phase);
        const wxColour colour = colours[phase];
        editor->SetBackgroundColour(colour);
#if wxUSE_STC
        if ( auto* const stc = wxDynamicCast(editor, wxStyledTextCtrl) )
        {
            stc->StyleSetBackground(wxSTC_STYLE_DEFAULT, colour);
            stc->StyleSetBackground(0, colour);
        }
#endif
        REQUIRE(::ValidateRect(editorHwnd, nullptr));
        RECT client{};
        REQUIRE(::GetClientRect(editorHwnd, &client));
        REQUIRE(client.right > 100);
        REQUIRE(client.bottom > 100);
        observation = {};
        observation.target = editor->GetHWND();
        observation.sample = wxPoint(client.right - 12, client.bottom - 12);
        const unsigned beforePaint = paints;
        const unsigned beforeSize = sizes;
        if ( phase != 2 )
        {
            observation.requested = phase == 0
                ? wxRect(0, 0, client.right, client.bottom)
                : wxRect(observation.sample.x - 4, observation.sample.y - 3, 8, 6);
            editor->Refresh(true, &observation.requested);
        }
        else
        {
            // Actual public resize, then consume its pending native paint.
            // No compensating Refresh can conceal a missing resize repaint.
            observation.sample += wxPoint(40, 24);
            editor->SetSize(editor->GetSize() + wxSize(40, 24));
        }
        REQUIRE(::UpdateWindow(editorHwnd));
        CAPTURE(observation.count, observation.bounds, observation.targetBounds);
        CHECK(paints > beforePaint);
        if ( phase == 2 ) CHECK(sizes > beforeSize);
        REQUIRE(observation.count > 0);
        REQUIRE(observation.valid);
        CHECK(observation.opaque);
        if ( phase != 2 ) CHECK(observation.coversRequested);
        REQUIRE(observation.sampled);
        CHECK(observation.background.rgbRed == colour.Red());
        CHECK(observation.background.rgbGreen == colour.Green());
        CHECK(observation.background.rgbBlue == colour.Blue());
        CHECK(observation.background.rgbReserved == 255);
        CHECK(::GetPropW(frameHwnd, L"wxWinUIBackdropTransparent") ==
              reinterpret_cast<HANDLE>(1));
        CHECK(wxWinUIShouldUseOpaquePaintBuffer(editor));
    }
}

#endif // wxUSE_RICHTEXT || wxUSE_STC || wxUSE_HTML

TEST_CASE("WinUIBridgePaint::UncutBridgeResizeDoesNotReinstallRegion",
          "[opaque-gdi][winui-bridge-paint][winui-resize-paint-budget]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    wxPoint position(-30000, -30000);
    wchar_t desktop[256]{};
    DWORD needed = 0;
    if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
            UOI_NAME, desktop, sizeof(desktop), &needed) &&
         wxString(desktop).StartsWith("wxWinUITest-") )
        position = wxPoint(80, 80);
    wxFrame frame(nullptr, wxID_ANY, "uncut bridge resize",
                  position, wxSize(500, 350));
    frame.SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    wxButton button(&frame, wxID_ANY, "native", wxPoint(20, 20), wxSize(90, 35));
    wxWindow classic(&frame, wxID_ANY, wxPoint(130, 20), wxSize(150, 140));
    frame.ShowWithoutActivating();
    DrainBridgePaint();
    auto* const host = wxWinUITopLevelHost::ForWindow(&frame, false);
    REQUIRE(host);
    const HWND bridge = host->GetBridgeHwnd();
    REQUIRE(bridge);
    HRGN region = ::CreateRectRgn(0, 0, 0, 0);
    REQUIRE(region);
    struct RegionOwner
    {
        HRGN value;
        ~RegionOwner() { ::DeleteObject(value); }
    } regionOwner{region};
    CHECK(::GetWindowRgn(bridge, region) == ERROR); // no explicit region
    const auto before = wxWinUIHostGeometryTestAccess::GetBridgeRegionChangeCount();
    for ( int step = 0; step != 16; ++step )
    {
        frame.SetClientSize(440 + step * 7, 300 + (step % 4) * 9);
        DrainBridgePaint();
        CHECK(::GetWindowRgn(bridge, region) == ERROR);
    }
    CHECK(wxWinUIHostGeometryTestAccess::GetBridgeRegionChangeCount() == before);

    // A genuinely native (not wx-managed/projected) scrollbar still needs a
    // physical bridge hole. Switching to it and back must not lose coverage.
    const HWND hwnd = NativeHandle(classic);
    ::SetWindowLongPtr(hwnd, GWL_STYLE,
        ::GetWindowLongPtr(hwnd, GWL_STYLE) | WS_VSCROLL);
    SCROLLINFO info{sizeof(SCROLLINFO), SIF_RANGE | SIF_PAGE | SIF_POS,
                    0, 999, 100, 0, 0};
    ::SetScrollInfo(hwnd, SB_VERT, &info, TRUE);
    REQUIRE(::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED));
    SCROLLBARINFO nativeBar{sizeof(SCROLLBARINFO)};
    REQUIRE(::GetScrollBarInfo(hwnd, OBJID_VSCROLL, &nativeBar));
    REQUIRE_FALSE(nativeBar.rgstate[0] & (STATE_SYSTEM_INVISIBLE | STATE_SYSTEM_OFFSCREEN));
    wxWinUITLWHostNotifyNativeLayout(&classic, false);
    DrainBridgePaint();
    CHECK(::GetWindowRgn(bridge, region) != ERROR);
    CHECK(wxWinUIHostGeometryTestAccess::GetBridgeRegionChangeCount() > before);
    ::ShowScrollBar(hwnd, SB_VERT, FALSE);
    wxWinUITLWHostNotifyNativeLayout(&classic, false);
    DrainBridgePaint();
    CHECK(::GetWindowRgn(bridge, region) == ERROR);
}

#endif // __WXWINUI__ && wxUSE_WINUI3
