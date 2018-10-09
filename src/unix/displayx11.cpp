///////////////////////////////////////////////////////////////////////////
// Name:        src/unix/displayx11.cpp
// Purpose:     Unix/X11 implementation of wxDisplay class
// Author:      Brian Victor, Vadim Zeitlin
// Modified by:
// Created:     12/05/02
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif /* WX_PRECOMP */

#include "wx/private/display.h"
#include "wx/unix/private/displayx11.h"

static wxRect wxGetMainScreenWorkArea();

namespace
{

inline int wxGetMainScreenDepth()
{
    Display* const dpy = wxGetX11Display();

    return DefaultDepth(dpy, DefaultScreen (dpy));
}

inline wxSize wxGetMainScreenSizeMM()
{
    Display* const dpy = wxGetX11Display();

    return wxSize
           (
                DisplayWidthMM(dpy, DefaultScreen(dpy)),
                DisplayHeightMM(dpy, DefaultScreen(dpy))
           );
}

class wxDisplayImplSingleX11 : public wxDisplayImplSingle
{
public:
    virtual wxRect GetGeometry() const wxOVERRIDE
    {
        Display* const dpy = wxGetX11Display();

        return wxRect(0, 0,
                      DisplayWidth(dpy, DefaultScreen (dpy)),
                      DisplayHeight(dpy, DefaultScreen (dpy)));
    }

    virtual wxRect GetClientArea() const wxOVERRIDE
    {
        return wxGetMainScreenWorkArea();
    }

    virtual int GetDepth() const wxOVERRIDE
    {
        return wxGetMainScreenDepth();
    }

    virtual wxSize GetSizeMM() const wxOVERRIDE
    {
        return wxGetMainScreenSizeMM();
    }
};

class wxDisplayFactorySingleX11 : public wxDisplayFactorySingle
{
protected:
    virtual wxDisplayImpl *CreateSingleDisplay()
    {
        return new wxDisplayImplSingleX11;
    }
};

} // anonymous namespace

#if wxUSE_DISPLAY

#include <X11/extensions/Xinerama.h>

typedef XineramaScreenInfo ScreenInfo;

// ----------------------------------------------------------------------------
// helper class storing information about all screens
// ----------------------------------------------------------------------------

class ScreensInfoBase
{
public:
    operator const ScreenInfo *() const { return m_screens; }

    unsigned GetCount() const { return static_cast<unsigned>(m_num); }

protected:
    ScreenInfo *m_screens;
    int m_num;
};

class ScreensInfo : public ScreensInfoBase
{
public:
    ScreensInfo()
    {
        m_screens = XineramaQueryScreens((Display *)wxGetDisplay(), &m_num);
    }

    ~ScreensInfo()
    {
        XFree(m_screens);
    }
};

// ----------------------------------------------------------------------------
// display and display factory classes
// ----------------------------------------------------------------------------

class wxDisplayImplX11 : public wxDisplayImpl
{
public:
    wxDisplayImplX11(unsigned n, const ScreenInfo& info)
        : wxDisplayImpl(n),
          m_rect(info.x_org, info.y_org, info.width, info.height)
    {
    }

    virtual wxRect GetGeometry() const wxOVERRIDE { return m_rect; }
    virtual wxRect GetClientArea() const wxOVERRIDE
    {
        // we intentionally don't cache the result here because the client
        // display area may change (e.g. the user resized or hid a panel) and
        // we don't currently react to its changes
        return IsPrimary() ? wxGetMainScreenWorkArea() : m_rect;
    }
    virtual int GetDepth() const wxOVERRIDE
    {
        const wxVideoMode& mode = GetCurrentMode();
        if ( mode.bpp )
            return mode.bpp;

        return wxGetMainScreenDepth();
    }
    virtual wxSize GetSizeMM() const wxOVERRIDE
    {
        // TODO: how to get physical size or resolution of the other monitors?
        return IsPrimary() ? wxGetMainScreenSizeMM() : wxSize(0, 0);
    }

    virtual wxArrayVideoModes GetModes(const wxVideoMode& mode) const wxOVERRIDE;
    virtual wxVideoMode GetCurrentMode() const wxOVERRIDE;
    virtual bool ChangeMode(const wxVideoMode& mode) wxOVERRIDE;

private:
    wxRect m_rect;

    wxDECLARE_NO_COPY_CLASS(wxDisplayImplX11);
};

class wxDisplayFactoryX11 : public wxDisplayFactory
{
public:
    wxDisplayFactoryX11() { }

    virtual wxDisplayImpl *CreateDisplay(unsigned n) wxOVERRIDE;
    virtual unsigned GetCount() wxOVERRIDE;
    virtual int GetFromPoint(const wxPoint& pt) wxOVERRIDE;

protected:
    wxDECLARE_NO_COPY_CLASS(wxDisplayFactoryX11);
};

// ============================================================================
// wxDisplayFactoryX11 implementation
// ============================================================================

unsigned wxDisplayFactoryX11::GetCount()
{
    return ScreensInfo().GetCount();
}

int wxDisplayFactoryX11::GetFromPoint(const wxPoint& p)
{
    ScreensInfo screens;

    const unsigned numscreens(screens.GetCount());
    for ( unsigned i = 0; i < numscreens; ++i )
    {
        const ScreenInfo& s = screens[i];
        if ( p.x >= s.x_org && p.x < s.x_org + s.width &&
                p.y >= s.y_org && p.y < s.y_org + s.height )
        {
            return i;
        }
    }

    return wxNOT_FOUND;
}

wxDisplayImpl *wxDisplayFactoryX11::CreateDisplay(unsigned n)
{
    ScreensInfo screens;

    return n < screens.GetCount() ? new wxDisplayImplX11(n, screens[n]) : NULL;
}

// ============================================================================
// wxDisplayImplX11 implementation
// ============================================================================

#ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H

wxArrayVideoModes wxDisplayImplX11::GetModes(const wxVideoMode& modeMatch) const
{
    Display* display = static_cast<Display*>(wxGetDisplay());
    int nScreen = DefaultScreen(display);
    return wxXF86VidMode_GetModes(modeMatch, display, nScreen);
}

wxVideoMode wxDisplayImplX11::GetCurrentMode() const
{
    Display* display = static_cast<Display*>(wxGetDisplay());
    int nScreen = DefaultScreen(display);
    return wxXF86VidMode_GetCurrentMode(display, nScreen);
}

bool wxDisplayImplX11::ChangeMode(const wxVideoMode& mode)
{
    Display* display = static_cast<Display*>(wxGetDisplay());
    int nScreen = DefaultScreen(display);
    return wxXF86VidMode_ChangeMode(mode, display, nScreen);
}

#else // !HAVE_X11_EXTENSIONS_XF86VMODE_H

wxArrayVideoModes wxDisplayImplX11::GetModes(const wxVideoMode& modeMatch) const
{
    Display* display = static_cast<Display*>(wxGetDisplay());
    return wxX11_GetModes(this, modeMatch, display);
}

wxVideoMode wxDisplayImplX11::GetCurrentMode() const
{
    // Not implemented
    return wxVideoMode();
}

bool wxDisplayImplX11::ChangeMode(const wxVideoMode& WXUNUSED(mode))
{
    // Not implemented
    return false;
}

#endif // !HAVE_X11_EXTENSIONS_XF86VMODE_H

#endif // wxUSE_DISPLAY

// ============================================================================
// wxDisplay::CreateFactory()
// ============================================================================

/* static */ wxDisplayFactory *wxDisplay::CreateFactory()
{
#if wxUSE_DISPLAY
    if ( XineramaIsActive((Display*)wxGetDisplay()) )
        return new wxDisplayFactoryX11;
#endif // wxUSE_DISPLAY

    return new wxDisplayFactorySingleX11;
}

wxRect wxGetMainScreenWorkArea()
{
    wxRect rect;

    Display * const dpy = wxGetX11Display();
    wxCHECK_MSG( dpy, rect, "can't be called before initializing the GUI" );

    wxGetWorkAreaX11(DefaultScreenOfDisplay(dpy),
                     rect.x, rect.y, rect.width, rect.height);

    const wxRect rectFull = wxDisplay().GetGeometry();

    if ( !rect.width || !rect.height )
    {
        // _NET_WORKAREA not available or didn't work, fall back to the total
        // display size.
        rect = rectFull;
    }
    else
    {
        // Although _NET_WORKAREA is supposed to return the client size of the
        // screen, not all implementations are conforming, apparently, see
        // #14419, so make sure we return a subset of the primary display.
        rect = rect.Intersect(rectFull);
    }

    return rect;
}
