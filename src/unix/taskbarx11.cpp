/////////////////////////////////////////////////////////////////////////
// File:        src/unix/taskbarx11.cpp
// Purpose:     wxTaskBarIcon class for common Unix desktops
// Author:      Vaclav Slavik
// Modified by:
// Created:     04/04/2003
// Copyright:   (c) Vaclav Slavik, 2003
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////

// NB: This implementation does *not* work with every X11 window manager.
//     Currently only GNOME 1.2 and KDE 1,2,3 methods are implemented here.
//     Freedesktop.org's System Tray specification is implemented in
//     src/gtk/taskbar.cpp and used from here under wxGTK.
//
//     Thanks to Ian Campbell, author of XMMS Status Docklet, for publishing
//     KDE and GNOME 1.2 methods.


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_TASKBARICON

#include "wx/taskbar.h"

#ifndef  WX_PRECOMP
    #include "wx/log.h"
    #include "wx/frame.h"
    #include "wx/dcclient.h"
    #include "wx/statbmp.h"
    #include "wx/sizer.h"
    #include "wx/bmpbndl.h"
    #include "wx/image.h"
#endif

#ifdef __VMS
#pragma message disable nosimpint
#endif
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#ifdef __VMS
#pragma message enable nosimpint
#endif

// ----------------------------------------------------------------------------
// base class that implements toolkit-specific method:
// ----------------------------------------------------------------------------

    class WXDLLIMPEXP_ADV wxTaskBarIconAreaBase : public wxFrame
    {
    public:
        wxTaskBarIconAreaBase()
            : wxFrame(nullptr, wxID_ANY, wxT("systray icon"),
                      wxDefaultPosition, wxDefaultSize,
                      wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                      wxSIMPLE_BORDER | wxFRAME_SHAPED) {}

        static bool IsProtocolSupported() { return false; }
    };

// ----------------------------------------------------------------------------
// toolkit dependent methods to set properties on helper window:
// ----------------------------------------------------------------------------

#if defined(__WXX11__)
    #include "wx/x11/privx.h"
    #define GetDisplay()        ((Display*)wxGlobalDisplay())
    #define GetXWindow(wxwin)   ((Window)(wxwin)->GetHandle())
#else
    #error "You must define X11 accessors for this port!"
#endif


// ----------------------------------------------------------------------------
// wxTaskBarIconArea is the real window that shows the icon:
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxTaskBarIconArea : public wxTaskBarIconAreaBase
{
public:
    wxTaskBarIconArea(wxTaskBarIcon *icon, const wxBitmapBundle &bmp);
    void SetTrayIcon(const wxBitmapBundle& bmp);
    bool IsOk() { return true; }

protected:
    void SetLegacyWMProperties();

    void OnSizeChange(wxSizeEvent& event);
    void OnPaint(wxPaintEvent& evt);
    void OnMouseEvent(wxMouseEvent& event);
    void OnMenuEvent(wxCommandEvent& event);

    wxTaskBarIcon *m_icon;
    wxPoint        m_pos;
    wxBitmapBundle m_bmp;
    wxBitmap       m_bmpReal;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxTaskBarIconArea, wxTaskBarIconAreaBase)
    EVT_SIZE(wxTaskBarIconArea::OnSizeChange)
    EVT_MOUSE_EVENTS(wxTaskBarIconArea::OnMouseEvent)
    EVT_MENU(wxID_ANY, wxTaskBarIconArea::OnMenuEvent)
    EVT_PAINT(wxTaskBarIconArea::OnPaint)
wxEND_EVENT_TABLE()

wxTaskBarIconArea::wxTaskBarIconArea(wxTaskBarIcon *icon, const wxBitmapBundle &bmp)
    : wxTaskBarIconAreaBase(), m_icon(icon), m_bmp(bmp)
{
    // Set initial size to bitmap size (tray manager may and often will
    // change it):
    SetClientSize(bmp.GetPreferredLogicalSizeFor(this));

    SetTrayIcon(bmp);

    if (!IsProtocolSupported())
    {
        wxLogTrace(wxT("systray"),
                   wxT("using legacy KDE1,2 and GNOME 1.2 methods"));
        SetLegacyWMProperties();
    }
}

void wxTaskBarIconArea::SetTrayIcon(const wxBitmapBundle& bmp)
{
    m_bmp = bmp;

    // determine suitable bitmap size:
    const wxSize winsize = GetClientSize();
    wxSize iconsize(m_bmp.GetDefaultSize());
    iconsize.DecTo(winsize);

    m_bmpReal = m_bmp.GetBitmap(iconsize);

    wxRegion region;
    region.Union(m_bmpReal);

    // if the bitmap is smaller than the window, offset it:
    if (winsize != iconsize)
    {
        m_pos.x = (winsize.x - iconsize.x) / 2;
        m_pos.y = (winsize.y - iconsize.y) / 2;
        region.Offset(m_pos.x, m_pos.y);
    }

    // set frame's shape to correct value and redraw:
    SetShape(region);
    Refresh();
}

void wxTaskBarIconArea::SetLegacyWMProperties()
{
    long data[1];

    // KDE 2 & KDE 3:
    Atom _KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR =
        XInternAtom(GetDisplay(), "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", False);
    data[0] = 0;
    XChangeProperty(GetDisplay(), GetXWindow(this),
                    _KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR,
                    XA_WINDOW, 32,
                    PropModeReplace, (unsigned char*)data, 1);

    // GNOME 1.2 & KDE 1:
    Atom KWM_DOCKWINDOW =
        XInternAtom(GetDisplay(), "KWM_DOCKWINDOW", False);
    data[0] = 1;
    XChangeProperty(GetDisplay(), GetXWindow(this),
                    KWM_DOCKWINDOW,
                    KWM_DOCKWINDOW, 32,
                    PropModeReplace, (unsigned char*)data, 1);
}

void wxTaskBarIconArea::OnSizeChange(wxSizeEvent& WXUNUSED(event))
{
    wxLogTrace(wxT("systray"), wxT("icon size changed to %i x %i"),
               GetSize().x, GetSize().y);
    // rescale or reposition the icon as needed:
    SetTrayIcon(m_bmp);
}

void wxTaskBarIconArea::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.DrawBitmap(m_bmpReal, m_pos.x, m_pos.y, true);
}

void wxTaskBarIconArea::OnMouseEvent(wxMouseEvent& event)
{
    wxEventType type = 0;
    wxEventType mtype = event.GetEventType();

    if (mtype == wxEVT_LEFT_DOWN)
        type = wxEVT_TASKBAR_LEFT_DOWN;
    else if (mtype == wxEVT_LEFT_UP)
        type = wxEVT_TASKBAR_LEFT_UP;
    else if (mtype == wxEVT_LEFT_DCLICK)
        type = wxEVT_TASKBAR_LEFT_DCLICK;
    else if (mtype == wxEVT_RIGHT_DOWN)
        type = wxEVT_TASKBAR_RIGHT_DOWN;
    else if (mtype == wxEVT_RIGHT_UP)
        type = wxEVT_TASKBAR_RIGHT_UP;
    else if (mtype == wxEVT_RIGHT_DCLICK)
        type = wxEVT_TASKBAR_RIGHT_DCLICK;
    else if (mtype == wxEVT_MOTION)
        type = wxEVT_TASKBAR_MOVE;
    else
        return;

   wxTaskBarIconEvent e(type, m_icon);
   m_icon->ProcessEvent(e);
}

void wxTaskBarIconArea::OnMenuEvent(wxCommandEvent& event)
{
    m_icon->ProcessEvent(event);
}

// ----------------------------------------------------------------------------
// wxTaskBarIconBase class:
// ----------------------------------------------------------------------------

bool wxTaskBarIconBase::IsAvailable()
{
    return wxTaskBarIconArea::IsProtocolSupported();
}

// ----------------------------------------------------------------------------
// wxTaskBarIcon class:
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxTaskBarIcon, wxEvtHandler);

wxTaskBarIcon::wxTaskBarIcon() : m_iconWnd(nullptr)
{
}

wxTaskBarIcon::~wxTaskBarIcon()
{
    if (m_iconWnd)
    {
        m_iconWnd->Unbind(wxEVT_DESTROY, &wxTaskBarIcon::OnDestroy, this);
        RemoveIcon();
    }
}

bool wxTaskBarIcon::IsOk() const
{
    return true;
}

bool wxTaskBarIcon::IsIconInstalled() const
{
    return m_iconWnd != nullptr;
}

// Destroy event from wxTaskBarIconArea
void wxTaskBarIcon::OnDestroy(wxWindowDestroyEvent&)
{
    // prevent crash if wxTaskBarIconArea is destroyed by something else,
    // for example if panel/kicker is killed
    m_iconWnd = nullptr;
}

bool wxTaskBarIcon::SetIcon(const wxBitmapBundle& icon, const wxString& tooltip)
{
    if (!m_iconWnd)
    {
        m_iconWnd = new wxTaskBarIconArea(this, icon);
        if (m_iconWnd->IsOk())
        {
            m_iconWnd->Bind(wxEVT_DESTROY, &wxTaskBarIcon::OnDestroy, this);
            m_iconWnd->Show();
        }
        else
        {
            m_iconWnd->Destroy();
            m_iconWnd = nullptr;
            return false;
        }
    }
    else
    {
        m_iconWnd->SetTrayIcon(icon);
    }

#if wxUSE_TOOLTIPS
    if (!tooltip.empty())
        m_iconWnd->SetToolTip(tooltip);
    else
        m_iconWnd->SetToolTip(nullptr);
#else
    wxUnusedVar(tooltip);
#endif
    return true;
}

bool wxTaskBarIcon::RemoveIcon()
{
    if (!m_iconWnd)
        return false;
    m_iconWnd->Destroy();
    m_iconWnd = nullptr;
    return true;
}

bool wxTaskBarIcon::PopupMenu(wxMenu *menu)
{
    if (!m_iconWnd)
        return false;
    m_iconWnd->PopupMenu(menu);
    return true;
}

#endif // wxUSE_TASKBARICON
