/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/paintsurface.cpp
// Purpose:     retained surface showing, through XAML, what a window paints
// Author:      wxWidgets development team
// Created:     2026-11-22
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// A window which paints itself (EVT_PAINT, wxClientDC) does not paint its
// HWND: it paints a bitmap kept for it here, which images in the canvas
// standing for the window show. What it paints is then composed by XAML in
// the same frame as the controls around it, with a real alpha channel (the
// backdrop shows wherever nothing was painted, on any display), and takes its
// place among them in the order of the windows instead of lying under every
// XAML control of the top-level window.
//
// Windows which never paint have no surface.

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif

#include "wx/msw/private.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"

#include "private.h"

#include <winrt/Microsoft.UI.Xaml.Media.Animation.h>

#include <algorithm>
#include <cmath>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace
{

namespace MUX = winrt::Microsoft::UI::Xaml;

// COM interface giving direct access to the bytes of an IBuffer.
struct __declspec(uuid("905a0fef-bc53-11df-8c49-001e4fc686da"))
IWxSurfaceBufferByteAccess : ::IUnknown
{
    virtual HRESULT __stdcall Buffer(uint8_t **value) = 0;
};

// The bitmap of a surface holds, for each pixel, what the window painted
// there and whether it painted anything:
//
// - alpha 0 and the key colour below: nothing was painted, the pixel is
//   transparent;
// - alpha 0 and any other colour: GDI has just written the pixel (it zeroes
//   the alpha byte of everything it touches); it becomes opaque when the
//   painting is committed;
// - anything else: painted, with that alpha (255 unless the pixels came from
//   a bitmap with an alpha channel of its own).
//
// The key colour, which no one paints by chance and which is black to the
// eye, is what tells a pixel GDI painted from one it only went over without
// changing it: the parts a mask leaves out of a bitmap, the rectangle around
// a shape GDI+ reads and writes back whole. And with no alpha where nothing
// is painted, what AlphaBlend() leaves there has the alpha of its source: the
// soft edge of an icon stays soft instead of turning dark.
//
// As one 32-bit value the alpha byte is the high one, then red, green, blue.
constexpr DWORD kUntouchedPixel = 0x00010302;
constexpr DWORD kAlphaMask = 0xff000000;

// What XAML is given for a pixel of the bitmap: premultiplied BGRA, like the
// bitmap, where only the pixels nothing was painted on differ.
inline DWORD wxWinUISurfaceShown(DWORD pixel)
{
    if ( pixel == kUntouchedPixel )
        return 0;

    const DWORD alpha = pixel >> 24;
    if ( alpha == 255 )
        return pixel;

    // Blended over the key colour, of which a trace remains: no channel may
    // exceed the alpha.
    const DWORD red = wxMin((pixel >> 16) & 0xff, alpha);
    const DWORD green = wxMin((pixel >> 8) & 0xff, alpha);
    const DWORD blue = wxMin(pixel & 0xff, alpha);
    return (alpha << 24) | (red << 16) | (green << 8) | blue;
}

// The bitmap is larger than the window, by up to one grain in each
// direction, and is shown by tiles of a fixed size: a window being resized
// keeps its bitmap from one step to the next, and a painting sends XAML the
// tiles it touched instead of the whole picture.
constexpr LONG kSurfaceGrain = 64;
constexpr LONG kSurfaceTile = 512;

// The size to give the bitmap, along one direction, of a window which needs
// "needed" pixels and has "current" ones: more as soon as it does not fit,
// less only once the difference is worth a new bitmap.
LONG wxWinUISurfaceExtent(LONG current, LONG needed)
{
    const LONG rounded =
        ((needed + kSurfaceGrain - 1) / kSurfaceGrain) * kSurfaceGrain;
    if ( rounded > current || current - rounded >= 4 * kSurfaceGrain )
        return rounded;
    return current;
}

class PaintSurface;
PaintSurface *wxWinUIFindSurface(wxWindow *window);

class PaintSurface
{
public:
    explicit PaintSurface(wxWindow *window)
        : m_window(window)
    {
        // A window which shrinks is not asked to repaint, and its picture
        // must not stick out of it: follow its size as it changes. The
        // binding outlives this object by a moment when the window is
        // destroyed, hence the lookup.
        window->Bind(wxEVT_SIZE, [window](wxSizeEvent& event)
        {
            event.Skip();
            if ( PaintSurface * const surface = wxWinUIFindSurface(window) )
                surface->OnWindowResized();
        });
    }

    void OnWindowResized()
    {
        if ( !m_depth )
            (void)EnsureSize();
    }

    ~PaintSurface()
    {
        Free();
    }

    PaintSurface(const PaintSurface&) = delete;
    PaintSurface& operator=(const PaintSurface&) = delete;

    HDC GetDC() const { return m_dc; }
    bool IsPainting() const { return m_depth != 0; }

    // Start of a WM_PAINT: rcPaint is the rectangle BeginPaint() reported,
    // in the coordinates of the window. Returns the DC to paint on, or null
    // if the window cannot have a surface right now.
    //
    // With erasing, this is the erasing of the background instead
    // (WM_ERASEBKGND): the rectangle becomes transparent, and whether
    // anything is then drawn on it is found from the bounds GDI accumulates.
    HDC Begin(const RECT& rcPaint, bool erase, bool erasing = false)
    {
        if ( !EnsureSize() )
            return nullptr;

        const int saved = ::SaveDC(m_dc);
        if ( !saved )
            return nullptr;

        ::SetLayout(m_dc, m_rtl ? LAYOUT_RTL : 0);

        // In the pixels of the bitmap, which run left to right whatever the
        // layout of the window: a right-to-left DC is mirrored about the
        // width of its bitmap, whose right edge is the leading edge of the
        // window (see GetClientInBitmap()).
        RECT physical = rcPaint;
        if ( m_rtl )
        {
            physical.left = m_alloc.cx - rcPaint.right;
            physical.right = m_alloc.cx - rcPaint.left;
        }
        const RECT client = GetClientInBitmap();
        if ( !::IntersectRect(&physical, &physical, &client) )
            ::SetRectEmpty(&physical);

        const bool changed = erase && Erase(physical);

        ::IntersectClipRect(m_dc, rcPaint.left, rcPaint.top,
                            rcPaint.right, rcPaint.bottom);
        ExcludeChildren();
        if ( erasing )
            ::SetBoundsRect(m_dc, nullptr, DCB_ENABLE | DCB_RESET);

        m_states.push_back({ saved, physical, false, false,
                             erasing, changed });
        ++m_depth;
        return m_dc;
    }

    // Start of painting outside WM_PAINT (wxClientDC): anywhere in the
    // window, and what was drawn is found from the bounds GDI accumulates.
    HDC BeginDirect()
    {
        if ( !EnsureSize() )
            return nullptr;

        const int saved = ::SaveDC(m_dc);
        if ( !saved )
            return nullptr;

        ::SetLayout(m_dc, m_rtl ? LAYOUT_RTL : 0);
        ::SelectClipRgn(m_dc, nullptr);
        ::SetViewportOrgEx(m_dc, 0, 0, nullptr);
        ::SetWindowOrgEx(m_dc, 0, 0, nullptr);
        ::IntersectClipRect(m_dc, 0, 0, m_size.cx, m_size.cy);
        ExcludeChildren();
        ::SetBoundsRect(m_dc, nullptr, DCB_ENABLE | DCB_RESET);

        m_states.push_back({ saved, RECT{}, true, false, false, false });
        ++m_depth;
        return m_dc;
    }

    void End()
    {
        if ( m_states.empty() )
            return;

        const State state = m_states.back();
        m_states.pop_back();
        --m_depth;

        ::GdiFlush();

        RECT painted = state.rect;
        bool drawn = true;
        if ( state.direct || state.erasing )
        {
            // Bounds are in logical coordinates of the DC as the painting
            // left it: take them to pixels before giving its state back.
            RECT bounds{};
            const UINT flags = ::GetBoundsRect(m_dc, &bounds, DCB_RESET);

            // Reading the bounds resets them for a painting this one was
            // nested in which relies on them too: that one commits all it
            // may have drawn on.
            bool outerTracked = false;
            for ( State& outer : m_states )
            {
                if ( outer.direct || outer.erasing )
                {
                    outer.whole = true;
                    outerTracked = true;
                }
            }
            if ( !outerTracked )
                ::SetBoundsRect(m_dc, nullptr, DCB_DISABLE);

            if ( state.erasing )
            {
                // The rectangle is known; all that matters is whether
                // anything was drawn on it.
                drawn = state.whole || (flags & DCB_SET) == DCB_SET;
            }
            else if ( state.whole )
            {
                painted = GetClientInBitmap();
            }
            else if ( (flags & DCB_SET) == DCB_SET )
            {
                POINT points[2] = { { bounds.left, bounds.top },
                                    { bounds.right, bounds.bottom } };
                if ( ::LPtoDP(m_dc, points, 2) )
                {
                    painted.left = wxMin(points[0].x, points[1].x) - 1;
                    painted.right = wxMax(points[0].x, points[1].x) + 1;
                    painted.top = wxMin(points[0].y, points[1].y) - 1;
                    painted.bottom = wxMax(points[0].y, points[1].y) + 1;
                }
                else
                {
                    painted = GetClientInBitmap();
                }
            }
        }

        ::RestoreDC(m_dc, state.saved);

        const RECT client = GetClientInBitmap();
        if ( !::IntersectRect(&painted, &painted, &client) )
            return;

        // An erasing nothing was drawn after only has to show what it made
        // transparent, if anything.
        if ( drawn )
            Show(painted, true);
        else if ( state.changed )
            Show(painted, false);
    }

    // Makes a rectangle, in the coordinates of the DC being painted on,
    // transparent again: what painting the colour of the backdrop used to
    // stand for.
    void ClearLogicalRect(const RECT& logical)
    {
        if ( !m_depth )
            return;

        ::GdiFlush();
        POINT points[2] = { { logical.left, logical.top },
                            { logical.right, logical.bottom } };
        if ( !::LPtoDP(m_dc, points, 2) )
            return;

        RECT rect = { wxMin(points[0].x, points[1].x),
                      wxMin(points[0].y, points[1].y),
                      wxMax(points[0].x, points[1].x),
                      wxMax(points[0].y, points[1].y) };
        // Whatever was drawn there, during this very painting included,
        // which is not yet counted in what is painted.
        const RECT client = GetClientInBitmap();
        if ( ::IntersectRect(&rect, &rect, &client) )
            EraseRows(rect);
    }

    // A child now covers this rectangle of the window: nothing of what the
    // window painted there may show through it any more.
    void ClearUnderChild(const RECT& childInClient)
    {
        if ( !m_dc || m_depth )
            return;

        const RECT client = GetClientInBitmap();
        RECT rect = childInClient;
        ::OffsetRect(&rect, client.left, 0);
        if ( ::IntersectRect(&rect, &rect, &client) && Erase(rect) )
            Present(rect);
    }

    // Whether what the window shows in this rectangle of its client area,
    // in pixels running left to right, is light rather than dark; fallback
    // if it painted nothing there.
    bool IsLightUnder(const RECT& inClient, bool fallback) const
    {
        if ( !m_bits || m_depth )
            return fallback;

        const RECT client = GetClientInBitmap();
        RECT rect = inClient;
        ::OffsetRect(&rect, client.left, 0);
        if ( !::IntersectRect(&rect, &rect, &client) )
            return fallback;

        ::GdiFlush();
        unsigned long long sum = 0,
                           count = 0;
        for ( LONG y = rect.top; y < rect.bottom; ++y )
        {
            const DWORD *pixel =
                m_bits + static_cast<size_t>(y) * m_alloc.cx + rect.left;
            for ( LONG x = rect.left; x < rect.right; ++x, ++pixel )
            {
                if ( *pixel == kUntouchedPixel )
                    continue;

                sum += 299 * ((*pixel >> 16) & 0xff) +
                       587 * ((*pixel >> 8) & 0xff) +
                       114 * (*pixel & 0xff);
                ++count;
            }
        }
        return count ? sum / count >= 128000 : fallback;
    }

    // The window scrolled its contents by (dx, dy) window pixels, inside
    // rect if there is one.
    void Scroll(int dx, int dy, const wxRect *rect)
    {
        if ( !m_dc || m_depth || !EnsureSize() )
            return;

        const RECT client = GetClientInBitmap();
        RECT area = client;
        if ( rect )
        {
            area.left = client.left +
                (m_rtl ? m_size.cx - rect->GetRight() - 1 : rect->x);
            area.top = rect->y;
            area.right = area.left + rect->width;
            area.bottom = area.top + rect->height;
            if ( !::IntersectRect(&area, &area, &client) )
                return;
        }
        if ( m_rtl )
            dx = -dx;

        // Plain pixels: the layout of the DC must not mirror this.
        const int saved = ::SaveDC(m_dc);
        ::SetLayout(m_dc, 0);
        ::SelectClipRgn(m_dc, nullptr);
        ::SetViewportOrgEx(m_dc, 0, 0, nullptr);
        ::ScrollDC(m_dc, dx, dy, &area, &area, nullptr, nullptr);
        ::GdiFlush();
        if ( saved )
            ::RestoreDC(m_dc, saved);

        // What was painted in the area may now be anywhere in it.
        RECT moved;
        if ( ::IntersectRect(&moved, &area, &m_painted) )
            ::UnionRect(&m_painted, &m_painted, &area);

        Present(area);
    }

private:
    struct State
    {
        int saved;
        RECT rect;      // of a WM_PAINT, in pixels of the bitmap
        bool direct;    // outside WM_PAINT: what was drawn is not known
        bool whole;     // nor can it be found: everything is committed
        bool erasing;   // WM_ERASEBKGND: often nothing is drawn at all
        bool changed;   // erasing made painted pixels transparent
    };

    // One of the bitmaps XAML shows, for a square of the picture.
    struct Tile
    {
        RECT rect{};            // in pixels of the bitmap
        MUX::Media::Imaging::WriteableBitmap bitmap{ nullptr };
        MUX::Controls::Image image{ nullptr };
    };

    bool IsRTL() const
    {
        return (::GetWindowLongPtr(GetHwndOf(m_window), GWL_EXSTYLE) &
                    WS_EX_LAYOUTRTL) != 0;
    }

    // The client area of the window in the pixels of the bitmap, which is
    // larger than it: at its left edge, or at its right edge for a
    // right-to-left window, whose contents hold to that edge when it is
    // resized. Whatever lies outside is never painted.
    RECT GetClientInBitmap() const
    {
        const LONG left = m_rtl ? m_alloc.cx - m_size.cx : 0;
        return RECT{ left, 0, left + m_size.cx, m_size.cy };
    }

    void Free()
    {
        if ( m_dc )
        {
            if ( m_oldBitmap )
                ::SelectObject(m_dc, m_oldBitmap);
            ::DeleteDC(m_dc);
        }
        if ( m_dib )
            ::DeleteObject(m_dib);
        m_dc = nullptr;
        m_dib = nullptr;
        m_oldBitmap = nullptr;
        m_bits = nullptr;
        m_size = SIZE{};
        m_alloc = SIZE{};
    }

    // Makes the bitmap, the tiles XAML shows and their place in the canvas
    // of the window match the client area. What was painted is kept.
    bool EnsureSize()
    {
        // The bitmap cannot change under a painting in progress.
        if ( m_depth )
            return m_dc != nullptr;

        const HWND hwnd = GetHwndOf(m_window);
        RECT client{};
        if ( !hwnd || !::GetClientRect(hwnd, &client) )
            return false;

        const SIZE size = { client.right - client.left,
                            client.bottom - client.top };
        if ( size.cx <= 0 || size.cy <= 0 )
            return false;

        wxWinUITopLevelHost * const host =
            wxWinUITopLevelHost::ForWindow(m_window, false);
        if ( !host )
            return false;
        const double scale = host->GetScale();
        if ( scale <= 0 )
            return false;

        const bool rtl = IsRTL();
        const SIZE alloc = { wxWinUISurfaceExtent(m_alloc.cx, size.cx),
                             wxWinUISurfaceExtent(m_alloc.cy, size.cy) };

        try
        {
            if ( !m_holder )
            {
                m_holder = MUX::Controls::Canvas();
                m_holder.IsHitTestVisible(false);
            }

            if ( !m_dc || rtl != m_rtl ||
                 alloc.cx != m_alloc.cx || alloc.cy != m_alloc.cy )
            {
                if ( !Reallocate(alloc, size, rtl) )
                    return false;
                PlaceTiles(scale);
            }
            else
            {
                const bool resized =
                    size.cx != m_size.cx || size.cy != m_size.cy;
                if ( resized )
                    SetClientSize(size);

                if ( scale != m_scale )
                    PlaceTiles(scale);
                else if ( resized && m_rtl )
                    PlaceHolder();
            }

            const auto kind =
                wxWinUITopLevelHost::WindowPartKind::PaintSurface;
            host->SetWindowPart(m_window, kind, m_holder);
            return host->GetWindowPart(m_window, kind) == m_holder;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI paint surface", e);
            return false;
        }
    }

    static Tile MakeTile(const RECT& rect)
    {
        Tile tile;
        tile.rect = rect;
        tile.bitmap = MUX::Media::Imaging::WriteableBitmap(
            rect.right - rect.left, rect.bottom - rect.top);

        tile.image = MUX::Controls::Image();
        tile.image.IsHitTestVisible(false);
        tile.image.Stretch(MUX::Media::Stretch::Fill);
        tile.image.Source(tile.bitmap);
        return tile;
    }

    // Gives the surface a bitmap of another size, for a client area of the
    // given size and layout.
    bool Reallocate(const SIZE& alloc, const SIZE& size, bool rtl)
    {
        // First what can throw: the tiles. Those which cover the same pixels
        // as before are kept, with the texture XAML made of them.
        std::vector<Tile> tiles;
        std::vector<bool> kept(m_tiles.size(), false);
        std::vector<bool> fresh;
        for ( LONG top = 0; top < alloc.cy; top += kSurfaceTile )
        {
            for ( LONG left = 0; left < alloc.cx; left += kSurfaceTile )
            {
                const RECT rect = { left, top,
                                    wxMin(left + kSurfaceTile, alloc.cx),
                                    wxMin(top + kSurfaceTile, alloc.cy) };
                size_t n = 0;
                while ( n < m_tiles.size() &&
                        !::EqualRect(&m_tiles[n].rect, &rect) )
                {
                    ++n;
                }
                if ( n < m_tiles.size() )
                {
                    kept[n] = true;
                    tiles.push_back(m_tiles[n]);
                }
                else
                {
                    tiles.push_back(MakeTile(rect));
                }
                fresh.push_back(n == m_tiles.size());
            }
        }

        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(info.bmiHeader);
        info.bmiHeader.biWidth = alloc.cx;
        info.bmiHeader.biHeight = -alloc.cy;    // top-down
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;

        void *bits = nullptr;
        const HBITMAP dib = ::CreateDIBSection(
            nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
        if ( !dib || !bits )
            return false;

        const HDC dc = ::CreateCompatibleDC(nullptr);
        if ( !dc )
        {
            ::DeleteObject(dib);
            return false;
        }
        const HBITMAP oldBitmap =
            static_cast<HBITMAP>(::SelectObject(dc, dib));

        DWORD * const pixels = static_cast<DWORD *>(bits);
        std::fill_n(pixels, static_cast<size_t>(alloc.cx) * alloc.cy,
                    kUntouchedPixel);

        // What was painted stays where the window keeps its contents when
        // it is resized: at its leading edge. A plain copy of the four
        // bytes of each pixel, marks included.
        const bool keep = m_dc && rtl == m_rtl;
        const SIZE before = m_size;
        RECT painted{};
        if ( keep )
        {
            painted = m_painted;
            if ( rtl )
                ::OffsetRect(&painted, alloc.cx - m_alloc.cx, 0);
        }
        if ( keep )
        {
            ::GdiFlush();
            const LONG width = wxMin(m_size.cx, size.cx);
            const LONG height = wxMin(m_size.cy, size.cy);
            const LONG from = rtl ? m_alloc.cx - width : 0;
            const LONG to = rtl ? alloc.cx - width : 0;
            for ( LONG y = 0; y < height; ++y )
            {
                std::copy_n(m_bits + static_cast<size_t>(y) * m_alloc.cx +
                                from,
                            width,
                            pixels + static_cast<size_t>(y) * alloc.cx + to);
            }
        }

        Free();
        m_dc = dc;
        m_dib = dib;
        m_oldBitmap = oldBitmap;
        m_bits = pixels;
        m_alloc = alloc;
        m_size = size;
        m_rtl = rtl;

        const RECT clientNow = GetClientInBitmap();
        if ( !::IntersectRect(&m_painted, &painted, &clientNow) )
            ::SetRectEmpty(&m_painted);

        const std::vector<Tile> former = std::move(m_tiles);
        m_tiles = std::move(tiles);

        const auto children = m_holder.Children();
        for ( size_t n = 0; n < former.size(); ++n )
        {
            std::uint32_t index = 0;
            if ( !kept[n] && children.IndexOf(former[n].image, index) )
                children.RemoveAt(index);
        }
        for ( size_t n = 0; n < m_tiles.size(); ++n )
        {
            if ( fresh[n] )
                children.Append(m_tiles[n].image);
        }

        // The HWND keeps nothing of what it may have been painted with
        // before it had a surface: black with no alpha, which is transparent.
        if ( !m_clearedWindow )
        {
            m_clearedWindow = true;
            const HWND hwnd = GetHwndOf(m_window);
            if ( const HDC windowDC = ::GetDC(hwnd) )
            {
                RECT client{};
                ::GetClientRect(hwnd, &client);
                ::FillRect(windowDC, &client,
                           static_cast<HBRUSH>(
                               ::GetStockObject(BLACK_BRUSH)));
                ::ReleaseDC(hwnd, windowDC);
            }
        }

        if ( keep && !rtl )
        {
            // The tiles which were kept already show their pixels, except
            // those the window no longer has.
            for ( size_t n = 0; n < m_tiles.size(); ++n )
            {
                if ( fresh[n] )
                    Present(m_tiles[n].rect);
            }
            if ( size.cy < before.cy )
                Present(RECT{ 0, size.cy, before.cx, before.cy });
            if ( size.cx < before.cx )
                Present(RECT{ size.cx, 0, before.cx, before.cy });
        }
        else
        {
            // Everything moved with the leading edge, or is gone.
            Present(RECT{ 0, 0, alloc.cx, alloc.cy });
        }
        return true;
    }

    // The window changed size inside its bitmap: what it no longer shows
    // becomes transparent again.
    void SetClientSize(const SIZE& size)
    {
        const RECT before = GetClientInBitmap();
        m_size = size;
        const RECT now = GetClientInBitmap();

        ::GdiFlush();
        if ( now.bottom < before.bottom )
        {
            Discard(RECT{ before.left, now.bottom,
                          before.right, before.bottom });
        }
        if ( now.left > before.left )
            Discard(RECT{ before.left, 0, now.left, before.bottom });
        if ( now.right < before.right )
            Discard(RECT{ now.right, 0, before.right, before.bottom });
    }

    void Discard(const RECT& rect)
    {
        if ( Erase(rect) )
            Present(rect);
    }

    // The holder of the tiles puts the client area of the bitmap on the
    // canvas of the window, whose origin is the left edge of that area.
    void PlaceHolder()
    {
        MUX::Controls::Canvas::SetLeft(
            m_holder, -GetClientInBitmap().left / m_scale);
    }

    void PlaceTiles(double scale)
    {
        using MUX::Controls::Canvas;

        for ( const Tile& tile : m_tiles )
        {
            Canvas::SetLeft(tile.image, tile.rect.left / scale);
            Canvas::SetTop(tile.image, tile.rect.top / scale);
            tile.image.Width((tile.rect.right - tile.rect.left) / scale);
            tile.image.Height((tile.rect.bottom - tile.rect.top) / scale);
        }
        m_scale = scale;
        PlaceHolder();
    }

    // Returns whether anything was painted there: most of the time nothing
    // was (a window paints nothing under its children), and then nothing
    // needs to be shown again.
    bool Erase(const RECT& rect)
    {
        // Nor is there anything to look for outside what was ever painted:
        // a window which lays out children and draws next to nothing is not
        // read through each time one of them moves.
        RECT inside;
        if ( !::IntersectRect(&inside, &rect, &m_painted) )
            return false;

        const bool all = ::EqualRect(&inside, &m_painted) != 0;
        const bool changed = EraseRows(inside);
        if ( all )
            ::SetRectEmpty(&m_painted);
        return changed;
    }

    bool EraseRows(const RECT& rect)
    {
        bool changed = false;
        for ( LONG y = rect.top; y < rect.bottom; ++y )
        {
            DWORD *pixel =
                m_bits + static_cast<size_t>(y) * m_alloc.cx + rect.left;
            for ( LONG x = rect.left; x < rect.right; ++x, ++pixel )
            {
                if ( *pixel != kUntouchedPixel )
                {
                    *pixel = kUntouchedPixel;
                    changed = true;
                }
            }
        }
        return changed;
    }

    // What a window paints stops at its children, as it does on the HWND
    // of a wx window: their pictures are transparent wherever they paint
    // nothing, so anything left under them would show; and those which are
    // real windows painting their own HWND (an OpenGL canvas, a window which
    // is not ours) show through the island. This does not depend on the
    // WS_CLIPCHILDREN style, which these windows no longer have (see
    // wxWindowMSW::MSWGetStyle()). The island itself is a child of the
    // top-level window, and is where this picture is shown.
    void ExcludeChildren()
    {
        const HWND hwnd = GetHwndOf(m_window);
        wxWinUITopLevelHost * const host =
            wxWinUITopLevelHost::ForWindow(m_window, false);
        const HWND bridge = host ? host->GetBridgeHwnd() : nullptr;

        for ( HWND child = ::GetWindow(hwnd, GW_CHILD);
              child;
              child = ::GetWindow(child, GW_HWNDNEXT) )
        {
            if ( child == bridge || !::IsWindowVisible(child) )
                continue;

            // A transparent window (wxBG_STYLE_TRANSPARENT) is the one
            // child its parent does paint under, on a HWND too: it shows
            // what is behind it.
            if ( ::GetWindowLongPtr(child, GWL_EXSTYLE) & WS_EX_TRANSPARENT )
                continue;

            RECT rect{};
            if ( !::GetWindowRect(child, &rect) )
                continue;
            ::MapWindowPoints(nullptr, hwnd,
                              reinterpret_cast<POINT *>(&rect), 2);
            if ( rect.left > rect.right )
                std::swap(rect.left, rect.right);
            ::ExcludeClipRect(m_dc, rect.left, rect.top,
                              rect.right, rect.bottom);
        }
    }

    void Present(const RECT& rect)
    {
        Show(rect, false);
    }

    // Copies a rectangle of the bitmap to the tiles it lies on, which XAML
    // then draws again; the other tiles are not sent anew. With commit,
    // what GDI has just written there becomes opaque on the way.
    void Show(const RECT& rect, bool commit)
    {
        if ( !m_bits )
            return;

        try
        {
            for ( const Tile& tile : m_tiles )
            {
                RECT part;
                if ( !::IntersectRect(&part, &rect, &tile.rect) )
                    continue;

                // The buffer is asked for each time: XAML is free to give
                // the bitmap other memory once it has drawn it.
                uint8_t *bytes = nullptr;
                if ( FAILED(tile.bitmap.PixelBuffer()
                                .as<IWxSurfaceBufferByteAccess>()
                                ->Buffer(&bytes)) || !bytes )
                {
                    continue;
                }
                DWORD * const pixels = reinterpret_cast<DWORD *>(bytes);

                // Premultiplied BGRA on both sides: only the pixels nothing
                // was painted on differ, they are transparent for XAML.
                const LONG tileWidth = tile.rect.right - tile.rect.left;
                DWORD seen = 0;
                for ( LONG y = part.top; y < part.bottom; ++y )
                {
                    DWORD *from =
                        m_bits + static_cast<size_t>(y) * m_alloc.cx +
                        part.left;
                    DWORD *to =
                        pixels +
                        static_cast<size_t>(y - tile.rect.top) * tileWidth +
                        (part.left - tile.rect.left);
                    DWORD * const end = from + (part.right - part.left);
                    if ( commit )
                    {
                        for ( ; from != end; ++from, ++to )
                        {
                            DWORD value = *from;
                            if ( !(value & kAlphaMask) &&
                                 value != kUntouchedPixel )
                            {
                                value |= kAlphaMask;
                                *from = value;
                            }
                            seen |= value ^ kUntouchedPixel;
                            *to = wxWinUISurfaceShown(value);
                        }
                    }
                    else
                    {
                        for ( ; from != end; ++from, ++to )
                        {
                            seen |= *from ^ kUntouchedPixel;
                            *to = wxWinUISurfaceShown(*from);
                        }
                    }
                }

                // Something is painted here: Erase() has to look at it.
                if ( seen )
                    ::UnionRect(&m_painted, &m_painted, &part);

                tile.bitmap.Invalidate();
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI paint surface update", e);
        }
    }

    wxWindow * const m_window;
    HDC m_dc = nullptr;
    HBITMAP m_dib = nullptr;
    HBITMAP m_oldBitmap = nullptr;
    DWORD *m_bits = nullptr;
    SIZE m_size{};          // of the client area of the window
    SIZE m_alloc{};         // of the bitmap, which is larger
    RECT m_painted{};       // bounds of all that is painted, in the bitmap
    bool m_rtl = false;     // the layout the bitmap is laid out for
    double m_scale = 0;     // the scale the tiles are placed for
    std::vector<Tile> m_tiles;
    MUX::Controls::Canvas m_holder{ nullptr };
    std::vector<State> m_states;
    int m_depth = 0;
    bool m_clearedWindow = false;
};

std::map<wxWindow *, std::unique_ptr<PaintSurface>> gs_surfaces;

// Windows which were asked to erase their background since their last
// painting: the update region of the next one starts transparent.
std::set<wxWindow *> gs_erasePending;

// The windows which paint their HWND although surfaces are in use (an
// OpenGL canvas): the only ones whose pixels the island has to mind.
std::set<wxWindow *> gs_hwndPainters;

PaintSurface *wxWinUIFindSurface(wxWindow *window)
{
    const auto it = gs_surfaces.find(window);
    return it == gs_surfaces.end() ? nullptr : it->second.get();
}

// Whether what this window paints goes to a surface.
bool wxWinUIWindowTakesSurface(wxWindow *window)
{
    if ( !window || !wxWinUIPaintSurfacesEnabled() || !wxIsMainThread() )
        return false;

    const HWND hwnd = GetHwndOf(window);
    if ( !hwnd || window->IsBeingDeleted() )
        return false;

    // A window shown by a XAML control paints nothing itself; one which
    // owns its DC (an OpenGL canvas) keeps its own pixels.
    if ( wxWinUITopLevelHost::FindSlotOwner(window) ||
         (::GetClassLongPtr(hwnd, GCL_STYLE) & CS_OWNDC) )
    {
        return false;
    }

    return wxWinUITopLevelHost::ForWindow(window, false) != nullptr;
}

} // anonymous namespace

bool wxWinUIPaintSurfacesEnabled()
{
    // On unless WX_WINUI_PAINT_SURFACES is 0, which brings back the
    // painting of the HWND to tell the two apart when something looks wrong.
    static const bool s_enabled = []()
    {
        wchar_t text[8];
        const DWORD length = ::GetEnvironmentVariableW(
            L"WX_WINUI_PAINT_SURFACES", text, WXSIZEOF(text));
        return !(length == 1 && text[0] == L'0');
    }();

    return s_enabled;
}

WXHDC wxWinUIPaintSurfaceBegin(wxWindow *window,
                               int left, int top, int right, int bottom)
{
    // A transparent window is never asked to erase its background, which
    // is what lies behind it: what it painted before goes each time, or what
    // it draws again would pile up on itself.
    const bool erase =
        (gs_erasePending.erase(window) != 0) ||
        (window && window->GetBackgroundStyle() == wxBG_STYLE_TRANSPARENT);

    HDC dc = nullptr;
    if ( wxWinUIWindowTakesSurface(window) )
    {
        PaintSurface *surface = wxWinUIFindSurface(window);
        if ( !surface )
        {
            surface = new PaintSurface(window);
            gs_surfaces[window].reset(surface);
        }

        const RECT rcPaint = { left, top, right, bottom };
        dc = surface->Begin(rcPaint, erase);
    }

    if ( !dc && window && wxWinUIPaintSurfacesEnabled() &&
         !wxWinUITopLevelHost::FindSlotOwner(window) )
    {
        gs_hwndPainters.insert(window);
    }
    return reinterpret_cast<WXHDC>(dc);
}

bool wxWinUIPaintSurfaceHasHwndPainters(WXHWND root)
{
    for ( wxWindow *window : gs_hwndPainters )
    {
        const HWND hwnd = GetHwndOf(window);
        if ( hwnd && ::GetAncestor(hwnd, GA_ROOT) == (HWND)root )
            return true;
    }
    return false;
}

WXHDC wxWinUIPaintSurfaceBeginDirect(wxWindow *window)
{
    // Only a window which already paints through a surface: a wxClientDC is
    // more often made to measure text than to draw, and must not give a
    // surface to a window which never paints.
    PaintSurface * const surface = wxWinUIFindSurface(window);
    if ( !surface || !wxWinUIWindowTakesSurface(window) )
        return nullptr;

    return reinterpret_cast<WXHDC>(surface->BeginDirect());
}

bool wxWinUIPaintSurfaceOwnsDC(wxWindow *window, WXHDC hdc)
{
    PaintSurface * const surface = wxWinUIFindSurface(window);
    return surface && hdc &&
           surface->GetDC() == reinterpret_cast<HDC>(hdc) &&
           surface->IsPainting();
}

void wxWinUIPaintSurfaceEnd(wxWindow *window)
{
    if ( PaintSurface * const surface = wxWinUIFindSurface(window) )
        surface->End();
}

void wxWinUIPaintSurfaceNoteErase(wxWindow *window)
{
    // Only for a window which has a surface: the others have nothing to
    // erase, and get an empty surface if they ever paint.
    if ( window && wxWinUIFindSurface(window) )
        gs_erasePending.insert(window);
}

WXHDC wxWinUIPaintSurfaceBeginErase(wxWindow *window, WXHDC hdcErase)
{
    PaintSurface * const surface = wxWinUIFindSurface(window);
    const HDC hdc = reinterpret_cast<HDC>(hdcErase);
    if ( !surface || !hdc || !wxWinUIWindowTakesSurface(window) )
        return nullptr;

    // The message is also sent with the DC of something else, to have the
    // background drawn there (printing a window, a child asking for what
    // lies behind it): that is not an erasing of the window.
    if ( ::WindowFromDC(hdc) != GetHwndOf(window) )
        return nullptr;

    // The DC of the message is clipped to what is to be erased.
    RECT rect{};
    if ( ::GetClipBox(hdc, &rect) == ERROR )
        return nullptr;

    // Done here and now: the painting which follows keeps what the handler
    // of the event draws.
    gs_erasePending.erase(window);
    return reinterpret_cast<WXHDC>(surface->Begin(rect, true, true));
}

void wxWinUIPaintSurfaceAdopt(wxWindow *window)
{
    if ( !wxWinUIFindSurface(window) && wxWinUIWindowTakesSurface(window) )
        gs_surfaces[window].reset(new PaintSurface(window));
}

void wxWinUIPaintSurfaceScroll(wxWindow *window,
                               int dx, int dy, const wxRect *rect)
{
    if ( PaintSurface * const surface = wxWinUIFindSurface(window) )
        surface->Scroll(dx, dy, rect);
}

bool wxWinUIPaintSurfaceClearRect(WXHDC hdc,
                                  int x, int y, int width, int height)
{
    for ( const auto& entry : gs_surfaces )
    {
        PaintSurface& surface = *entry.second;
        if ( surface.IsPainting() &&
             surface.GetDC() == reinterpret_cast<HDC>(hdc) )
        {
            surface.ClearLogicalRect(RECT{ x, y, x + width, y + height });
            return true;
        }
    }

    return false;
}

// ----------------------------------------------------------------------------
// borders
// ----------------------------------------------------------------------------

namespace
{

// The windows whose border XAML draws.
std::set<wxWindow *> gs_frames;

// Those which have a frame without having a border in Win32.
std::set<wxWindow *> gs_wantedFrames;

} // anonymous namespace

bool wxWinUIWindowFrameSync(wxWindow *window)
{
    if ( !window || window->IsTopLevel() )
        return false;

    // Any window of the island but one which keeps the pixels of its HWND
    // (an OpenGL canvas): a window shown by a XAML control which has a
    // border of wx around it (wxPropertyGrid) had it painted on its HWND
    // like the others.
    const HWND hwnd = GetHwndOf(window);
    wxWinUITopLevelHost * const host =
        hwnd && wxWinUIPaintSurfacesEnabled() && wxIsMainThread() &&
        !window->IsBeingDeleted() &&
        !(::GetClassLongPtr(hwnd, GCL_STYLE) & CS_OWNDC)
            ? wxWinUITopLevelHost::ForWindow(window, false)
            : nullptr;

    // What Win32 keeps around the client area. The scrollbars of a window
    // are not part of it here: they are drawn over the edge of the client
    // area (see WM_NCCALCSIZE), and those USER32 still owns are painted by
    // it, inside the border.
    RECT rw{};
    RECT rc{};
    bool bordered = false;
    if ( host && hwnd && ::GetWindowRect(hwnd, &rw) &&
         ::GetClientRect(hwnd, &rc) )
    {
        ::MapWindowPoints(hwnd, nullptr, reinterpret_cast<POINT *>(&rc), 2);
        const LONG left = wxMin(rc.left, rc.right);
        bordered = (left > rw.left && rc.top > rw.top) ||
                   gs_wantedFrames.count(window) != 0;
    }

    if ( !bordered )
    {
        if ( gs_frames.erase(window) && host )
            host->SetWindowFrame(window, nullptr);
        return false;
    }

    gs_frames.insert(window);
    if ( ::GetWindowLong(hwnd, GWL_STYLE) & WS_VISIBLE )
    {
        const wxRect rect(rw.left, rw.top,
                          rw.right - rw.left, rw.bottom - rw.top);
        host->SetWindowFrame(window, &rect);
    }
    else
    {
        host->SetWindowFrame(window, nullptr);
    }
    return true;
}

void wxWinUISetDialogContentTone(wxWindow *dialog, int height)
{
    wxWinUITopLevelHost * const host =
        dialog ? wxWinUITopLevelHost::ForWindow(dialog, false) : nullptr;
    if ( !host )
        return;

    const auto kind = wxWinUITopLevelHost::WindowPartKind::Ground;
    try
    {
        auto ground = host->GetWindowPart(dialog, kind).try_as<MUX::Controls::Grid>();
        if ( height <= 0 )
        {
            if ( ground )
            {
                host->SetWindowPart(dialog, kind, nullptr);
                wxWinUISetDialogCaptionTone(dialog, false);
            }
            return;
        }

        if ( !ground )
        {
            // The brushes of a ContentDialog, which is opaque: its
            // background under everything, the overlay of its top part and
            // the line which separates it from its commands.
            ground = MUX::Markup::XamlReader::Load(
                L"<Grid"
                L" xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
                L" IsHitTestVisible='False'"
                L" Background='{ThemeResource SolidBackgroundFillColorBaseBrush}'>"
                L"<Border VerticalAlignment='Top' BorderThickness='0,0,0,1'"
                L" Background='{ThemeResource LayerFillColorAltBrush}'"
                L" BorderBrush='{ThemeResource CardStrokeColorDefaultBrush}'/>"
                L"</Grid>")
                    .as<MUX::Controls::Grid>();
            host->SetWindowPart(dialog, kind, ground);
        }

        const double scale = host->GetScale();
        if ( scale <= 0 )
            return;

        // A dialog shown over its owner has its title above its client
        // area, in the letters of the title of a ContentDialog, and
        // margins around it: the ground is under all of them.
        const int band = wxWinUIGetDialogCardInsets(dialog).top;
        RECT client = {};
        ::GetClientRect(reinterpret_cast<HWND>(dialog->GetHandle()), &client);
        ground.Width(client.right / scale);
        ground.Height(client.bottom / scale);
        MUX::Controls::Canvas::SetLeft(ground, 0);
        MUX::Controls::Canvas::SetTop(ground, 0);
        if ( ground.Children().Size() )
        {
            ground.Children().GetAt(0).as<MUX::FrameworkElement>().Height(
                height / scale);
        }

        MUX::Controls::TextBlock title{ nullptr };
        if ( ground.Children().Size() > 1 )
        {
            title = ground.Children().GetAt(1)
                        .try_as<MUX::Controls::TextBlock>();
        }
        if ( band > 0 )
        {
            if ( !title )
            {
                title = MUX::Markup::XamlReader::Load(
                    L"<TextBlock"
                    L" xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
                    L" FontSize='20' FontWeight='SemiBold'"
                    L" Margin='24,22,24,0' VerticalAlignment='Top'"
                    L" TextTrimming='CharacterEllipsis'/>")
                        .as<MUX::Controls::TextBlock>();
                ground.Children().Append(title);
            }
            title.Text(wxWinUIToHString(dialog->GetLabel()));
        }
        else if ( title )
        {
            ground.Children().RemoveAt(1);
        }

        // The title bar is part of the top of the dialog: the same tone.
        wxWinUISetDialogCaptionTone(dialog, true);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI dialog content tone", e);
    }
}

void wxWinUISetWindowSmoke(wxWindow *tlw, bool on)
{
    wxWinUITopLevelHost * const host =
        tlw ? wxWinUITopLevelHost::ForWindow(tlw, false) : nullptr;
    if ( !host )
        return;

    const auto kind = wxWinUITopLevelHost::WindowPartKind::Smoke;
    try
    {
        auto smoke =
            host->GetWindowPart(tlw, kind).try_as<MUX::Controls::Border>();
        if ( !on )
        {
            if ( smoke )
                host->SetWindowPart(tlw, kind, nullptr);
            return;
        }

        if ( !smoke )
        {
            // The veil of a ContentDialog.
            smoke = MUX::Markup::XamlReader::Load(
                L"<Border"
                L" xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
                L" IsHitTestVisible='False'"
                L" Background='{ThemeResource SmokeFillColorDefaultBrush}'/>")
                    .as<MUX::Controls::Border>();
            host->SetWindowPart(tlw, kind, smoke);
        }

        const double scale = host->GetScale();
        if ( scale <= 0 )
            return;

        // All of the client area: the one of a frame of wx leaves its tool
        // bar and its status bar out.
        RECT client = {};
        ::GetClientRect(reinterpret_cast<HWND>(tlw->GetHandle()), &client);
        smoke.Width(client.right / scale);
        smoke.Height(client.bottom / scale);
        MUX::Controls::Canvas::SetLeft(smoke, 0);
        MUX::Controls::Canvas::SetTop(smoke, 0);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI window veil", e);
    }
}

bool wxWinUIWindowHasRequestedFrame(const wxWindow *window)
{
    return gs_wantedFrames.count(const_cast<wxWindow *>(window)) != 0;
}

void wxWinUIRequestWindowFrame(wxWindow *window)
{
    if ( !window || window->IsTopLevel() )
        return;

    // Drawn now if the window is in its island already, and when its
    // geometry is next told otherwise.
    gs_wantedFrames.insert(window);
    wxWinUIWindowFrameSync(window);
}

void wxWinUIPaintSurfaceNoteChildGeometry(wxWindow *child)
{
    // Its border follows the window, and goes while it is hidden.
    if ( child && (gs_frames.count(child) || gs_wantedFrames.count(child)) )
        wxWinUIWindowFrameSync(child);

    if ( gs_surfaces.empty() || !child || child->IsTopLevel() )
        return;

    wxWindow * const parent = child->GetParent();
    PaintSurface * const surface =
        parent ? wxWinUIFindSurface(parent) : nullptr;
    const HWND hwnd = GetHwndOf(child);
    if ( !surface || !hwnd || !::IsWindowVisible(hwnd) ||
         (::GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_TRANSPARENT) )
    {
        return;
    }

    // In pixels of the picture of the parent, which run left to right.
    RECT rect{};
    RECT origin{};
    if ( !::GetWindowRect(hwnd, &rect) ||
         !::GetClientRect(GetHwndOf(parent), &origin) )
    {
        return;
    }
    ::MapWindowPoints(GetHwndOf(parent), nullptr,
                      reinterpret_cast<POINT *>(&origin), 2);
    const LONG left = wxMin(origin.left, origin.right);
    ::OffsetRect(&rect, -left, -origin.top);
    surface->ClearUnderChild(rect);
}

// ----------------------------------------------------------------------------
// caret
// ----------------------------------------------------------------------------

namespace
{

// The caret of Win32 is drawn on the HWND, which a window painting through
// a surface does not show: its caret is an element of its canvas, blinking
// at the rate of the system. One at a time, as in Win32.
struct SurfaceCaret
{
    wxWindow *window = nullptr;
    MUX::Controls::Border element{ nullptr };
    MUX::Media::Animation::Storyboard blink{ nullptr };
    UINT blinkTime = 0;
};

SurfaceCaret gs_caret;

void wxWinUICaretBlink()
{
    using namespace MUX::Media::Animation;

    if ( gs_caret.blink )
        gs_caret.blink.Stop();

    // A caret which does not blink stays shown.
    const UINT time = ::GetCaretBlinkTime();
    if ( !time || time == INFINITE )
    {
        gs_caret.blink = nullptr;
        return;
    }

    if ( !gs_caret.blink || gs_caret.blinkTime != time )
    {
        const auto at = [](UINT ms)
        {
            return winrt::Windows::Foundation::TimeSpan(
                std::chrono::milliseconds(ms));
        };

        DiscreteDoubleKeyFrame shown;
        shown.KeyTime(KeyTimeHelper::FromTimeSpan(at(0)));
        shown.Value(1.);
        DiscreteDoubleKeyFrame hidden;
        hidden.KeyTime(KeyTimeHelper::FromTimeSpan(at(time)));
        hidden.Value(0.);

        DoubleAnimationUsingKeyFrames animation;
        animation.KeyFrames().Append(shown);
        animation.KeyFrames().Append(hidden);
        animation.Duration(MUX::DurationHelper::FromTimeSpan(at(2 * time)));
        animation.RepeatBehavior(RepeatBehaviorHelper::Forever());
        Storyboard::SetTarget(animation, gs_caret.element);
        Storyboard::SetTargetProperty(animation, L"Opacity");

        Storyboard storyboard;
        storyboard.Children().Append(animation);
        gs_caret.blink = storyboard;
        gs_caret.blinkTime = time;
    }

    gs_caret.blink.Begin();
}

void wxWinUICaretRemove()
{
    wxWindow * const window = gs_caret.window;
    gs_caret.window = nullptr;
    if ( !window )
        return;

    try
    {
        if ( gs_caret.blink )
            gs_caret.blink.Stop();

        if ( wxWinUITopLevelHost * const host =
                wxWinUITopLevelHost::ForWindow(window, false) )
        {
            host->SetWindowPart(
                window, wxWinUITopLevelHost::WindowPartKind::Caret, nullptr);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI caret removal", e);
    }
}

} // anonymous namespace

bool wxWinUICaretShow(wxWindow *window, int x, int y, int width, int height)
{
    if ( width <= 0 || height <= 0 || !wxWinUIWindowTakesSurface(window) )
        return false;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(window, false);
    const double scale = host ? host->GetScale() : 0.;
    if ( scale <= 0 )
        return false;

    // In pixels of the client area running left to right, like the canvas
    // of the window.
    const HWND hwnd = GetHwndOf(window);
    RECT client{};
    ::GetClientRect(hwnd, &client);
    const bool rtl =
        (::GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0;
    const int left = rtl ? client.right - x - width : x;

    try
    {
        using MUX::Controls::Canvas;

        if ( gs_caret.window && gs_caret.window != window )
            wxWinUICaretRemove();

        if ( !gs_caret.element )
        {
            gs_caret.element = MUX::Controls::Border();
            gs_caret.element.IsHitTestVisible(false);
        }

        // The caret of Win32 inverts what it covers: dark on a light
        // ground, light on a dark one.
        bool light = !wxWinUIIsDarkTheme();
        if ( const PaintSurface * const surface = wxWinUIFindSurface(window) )
        {
            light = surface->IsLightUnder(
                RECT{ left, y, left + width, y + height }, light);
        }
        gs_caret.element.Background(MUX::Media::SolidColorBrush(
            light ? winrt::Microsoft::UI::Colors::Black()
                  : winrt::Microsoft::UI::Colors::White()));

        Canvas::SetLeft(gs_caret.element, left / scale);
        Canvas::SetTop(gs_caret.element, y / scale);
        gs_caret.element.Width(width / scale);
        gs_caret.element.Height(height / scale);
        gs_caret.element.Opacity(1.);

        const auto kind = wxWinUITopLevelHost::WindowPartKind::Caret;
        host->SetWindowPart(window, kind, gs_caret.element);
        if ( host->GetWindowPart(window, kind) != gs_caret.element )
            return false;

        gs_caret.window = window;
        wxWinUICaretBlink();
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI caret", e);
        return false;
    }
}

bool wxWinUICaretHide(wxWindow *window)
{
    if ( !window || gs_caret.window != window )
        return false;

    wxWinUICaretRemove();
    return true;
}

bool wxWinUICaretIsShown(wxWindow *window)
{
    return window && gs_caret.window == window;
}

void wxWinUIPaintSurfaceForget(wxWindow *window)
{
    // The host takes the element out of the canvas of a window being
    // destroyed, see wxWinUITopLevelHost::OnAncestorDestroy().
    if ( gs_caret.window == window )
    {
        gs_caret.window = nullptr;
        try
        {
            if ( gs_caret.blink )
                gs_caret.blink.Stop();
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    gs_erasePending.erase(window);
    gs_hwndPainters.erase(window);
    gs_surfaces.erase(window);

    // Its border is in the canvas of its parent, which stays.
    gs_wantedFrames.erase(window);
    if ( gs_frames.erase(window) )
    {
        if ( wxWinUITopLevelHost * const host =
                wxWinUITopLevelHost::ForWindow(window, false) )
        {
            host->SetWindowFrame(window, nullptr);
        }
    }
}

#endif // wxUSE_WINUI3
