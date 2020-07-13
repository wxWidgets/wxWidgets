///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/nonownedwnd.cpp
// Purpose:     wxNonOwnedWindow implementation for MSW.
// Author:      Vadim Zeitlin
// Created:     2011-10-09 (extracted from src/msw/toplevel.cpp)
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/frame.h"       // Only for wxFRAME_SHAPED.
    #include "wx/region.h"
    #include "wx/msw/private.h"
#endif // WX_PRECOMP

#include "wx/nonownedwnd.h"

#if wxUSE_GRAPHICS_CONTEXT
    #include "wx/msw/wrapgdip.h"
    #include "wx/graphics.h"
#endif // wxUSE_GRAPHICS_CONTEXT

#include "wx/dynlib.h"
#include "wx/scopedptr.h"
#include "wx/msw/missing.h"

// ============================================================================
// wxNonOwnedWindow implementation
// ============================================================================

bool wxNonOwnedWindow::DoClearShape()
{
    if (::SetWindowRgn(GetHwnd(), NULL, TRUE) == 0)
    {
        wxLogLastError(wxT("SetWindowRgn"));
        return false;
    }

    return true;
}

bool wxNonOwnedWindow::DoSetRegionShape(const wxRegion& region)
{
    // Windows takes ownership of the region, so
    // we'll have to make a copy of the region to give to it.
    DWORD noBytes = ::GetRegionData(GetHrgnOf(region), 0, NULL);
    RGNDATA *rgnData = (RGNDATA*) new char[noBytes];
    ::GetRegionData(GetHrgnOf(region), noBytes, rgnData);
    HRGN hrgn = ::ExtCreateRegion(NULL, noBytes, rgnData);
    delete[] (char*) rgnData;

    // SetWindowRgn expects the region to be in coordinates
    // relative to the window, not the client area.
    const wxPoint clientOrigin = GetClientAreaOrigin();
    ::OffsetRgn(hrgn, -clientOrigin.x, -clientOrigin.y);

    // Now call the shape API with the new region.
    if (::SetWindowRgn(GetHwnd(), hrgn, TRUE) == 0)
    {
        wxLogLastError(wxT("SetWindowRgn"));
        return false;
    }
    return true;
}

#if wxUSE_GRAPHICS_CONTEXT

#include "wx/msw/wrapgdip.h"

// This class contains data used only when SetPath(wxGraphicsPath) is called.
class wxNonOwnedWindowShapeImpl
{
public:
    wxNonOwnedWindowShapeImpl(wxNonOwnedWindow* win, const wxGraphicsPath& path) :
        m_win(win),
        m_path(path)
    {
        // Create the region corresponding to this path and set it as windows
        // shape.
        wxScopedPtr<wxGraphicsContext> context(wxGraphicsContext::Create(win));
        Region gr(static_cast<GraphicsPath*>(m_path.GetNativePath()));
        win->SetShape(
            wxRegion(
                gr.GetHRGN(static_cast<Graphics*>(context->GetNativeContext()))
            )
        );


        // Connect to the paint event to draw the border.
        //
        // TODO: Do this only optionally?
        m_win->Bind(wxEVT_PAINT, &wxNonOwnedWindowShapeImpl::OnPaint, this);
    }

    virtual ~wxNonOwnedWindowShapeImpl()
    {
        m_win->Unbind(wxEVT_PAINT, &wxNonOwnedWindowShapeImpl::OnPaint, this);
    }

private:
    void OnPaint(wxPaintEvent& event)
    {
        event.Skip();

        wxPaintDC dc(m_win);
        wxScopedPtr<wxGraphicsContext> context(wxGraphicsContext::Create(dc));
        context->SetPen(wxPen(*wxLIGHT_GREY, 2));
        context->StrokePath(m_path);
    }

    wxNonOwnedWindow* const m_win;
    wxGraphicsPath m_path;

    wxDECLARE_NO_COPY_CLASS(wxNonOwnedWindowShapeImpl);
};

bool wxNonOwnedWindow::DoSetPathShape(const wxGraphicsPath& path)
{
    delete m_shapeImpl;
    m_shapeImpl = new wxNonOwnedWindowShapeImpl(this, path);

    return true;
}

#endif // wxUSE_GRAPHICS_CONTEXT

wxNonOwnedWindow::wxNonOwnedWindow()
{
#if wxUSE_GRAPHICS_CONTEXT
    m_shapeImpl = NULL;
#endif // wxUSE_GRAPHICS_CONTEXT

    m_activeDPI = wxDefaultSize;
    m_perMonitorDPIaware = false;
}

wxNonOwnedWindow::~wxNonOwnedWindow()
{
#if wxUSE_GRAPHICS_CONTEXT
    delete m_shapeImpl;
#endif // wxUSE_GRAPHICS_CONTEXT
}

bool wxNonOwnedWindow::Reparent(wxWindowBase* newParent)
{
    // ::SetParent() can't be used for non-owned windows, as they don't have
    // any parent, only the owner, so use a different function for them even
    // if, confusingly, the owner is stored at the same location as the parent
    // and so uses the same GWLP_HWNDPARENT offset.

    // Do not call the base class function here to skip wxWindow reparenting.
    if ( !wxWindowBase::Reparent(newParent) )
        return false;

    const HWND hwndOwner = GetParent() ? GetHwndOf(GetParent()) : 0;

    ::SetWindowLongPtr(GetHwnd(), GWLP_HWNDPARENT, (LONG_PTR)hwndOwner);

    return true;
}

namespace
{

static bool IsPerMonitorDPIAware(HWND hwnd)
{
    bool dpiAware = false;

    // Determine if 'Per Monitor v2' DPI awareness is enabled in the
    // applications manifest.
#if wxUSE_DYNLIB_CLASS
    #define WXDPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((WXDPI_AWARENESS_CONTEXT)-4)
    typedef WXDPI_AWARENESS_CONTEXT(WINAPI * GetWindowDpiAwarenessContext_t)(HWND hwnd);
    typedef BOOL(WINAPI * AreDpiAwarenessContextsEqual_t)(WXDPI_AWARENESS_CONTEXT dpiContextA, WXDPI_AWARENESS_CONTEXT dpiContextB);
    static GetWindowDpiAwarenessContext_t s_pfnGetWindowDpiAwarenessContext = NULL;
    static AreDpiAwarenessContextsEqual_t s_pfnAreDpiAwarenessContextsEqual = NULL;
    static bool s_initDone = false;

    if ( !s_initDone )
    {
        wxLoadedDLL dllUser32("user32.dll");
        wxDL_INIT_FUNC(s_pfn, GetWindowDpiAwarenessContext, dllUser32);
        wxDL_INIT_FUNC(s_pfn, AreDpiAwarenessContextsEqual, dllUser32);
        s_initDone = true;
    }

    if ( s_pfnGetWindowDpiAwarenessContext && s_pfnAreDpiAwarenessContextsEqual )
    {
        WXDPI_AWARENESS_CONTEXT dpiAwarenessContext = s_pfnGetWindowDpiAwarenessContext(hwnd);

        if ( s_pfnAreDpiAwarenessContextsEqual(dpiAwarenessContext, WXDPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2) == TRUE )
        {
            dpiAware = true;
        }
    }
#endif // wxUSE_DYNLIB_CLASS

    return dpiAware;
}

}

void wxNonOwnedWindow::InheritAttributes()
{
    m_activeDPI = GetDPI();
    m_perMonitorDPIaware = IsPerMonitorDPIAware(GetHwnd());

    wxNonOwnedWindowBase::InheritAttributes();
}

WXLRESULT wxNonOwnedWindow::MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    WXLRESULT rc = 0;
    bool processed = false;

    switch ( message )
    {
        case WM_DPICHANGED:
            {
                const RECT* const prcNewWindow =
                                         reinterpret_cast<const RECT*>(lParam);

                processed = HandleDPIChange(wxSize(LOWORD(wParam),
                                                   HIWORD(wParam)),
                                            wxRectFromRECT(*prcNewWindow));
            }
            break;
    }

    if (!processed)
        rc = wxNonOwnedWindowBase::MSWWindowProc(message, wParam, lParam);

    return rc;
}

bool wxNonOwnedWindow::HandleDPIChange(const wxSize& newDPI, const wxRect& newRect)
{
    if ( !m_perMonitorDPIaware )
    {
        return false;
    }

    if ( newDPI != m_activeDPI )
    {
        MSWUpdateOnDPIChange(m_activeDPI, newDPI);
        m_activeDPI = newDPI;
    }

    SetSize(newRect);

    Refresh();

    return true;
}
