/////////////////////////////////////////////////////////////////////////
// File:        taskbar.cpp
// Purpose:     wxTaskBarIcon class for common Unix desktops
// Author:      Vaclav Slavik
// Modified by:
// Created:     04/04/2003
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik, 2003
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "taskbarx11.h"
#endif


// NB: This implementation does *not* work with every X11 window manager.
//     Currently only GNOME 1.2 and KDE 1,2,3 methods are implemented.
//
//     FIXME: implement:
//               - GNOME 2 support (see www.freedesktop.org for specification;
//                 KDE 3 uses this method as well, even though legacy KDE
//                 method we implement works as well)
//               - IceWM and XFCE support (?)
//
//     Thanks to Ian Campbell, author of XMMS Status Docklet, for publishing
//     KDE and GNOME 1.2 methods.


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/taskbar.h"
#include "wx/frame.h"
#include "wx/bitmap.h"
#include "wx/statbmp.h"

#ifdef __VMS
#pragma message disable nosimpint
#endif
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#ifdef __VMS
#pragma message enable nosimpint
#endif

// ----------------------------------------------------------------------------
// toolkit dependent methods to set properties on helper window:
// ----------------------------------------------------------------------------

#if defined(__WXGTK__)
    #include <gdk/gdk.h>
    #include <gdk/gdkx.h>
    #include <gtk/gtk.h>
    #define GetDisplay()        GDK_DISPLAY()
    #define GetXWindow(wxwin)   GDK_WINDOW_XWINDOW((wxwin)->m_widget->window)
#elif defined(__WXX11__) || defined(__WXMOTIF__)
    #include "wx/x11/privx.h"
    #define GetDisplay()        ((Display*)wxGlobalDisplay())
    #define GetXWindow(wxwin)   ((Window)(wxwin)->GetHandle())
#else
    #error "You must define X11 accessors for this port!"
#endif

// ----------------------------------------------------------------------------
// code for making wxFrame a toolbar icon by setting appropriate properties:
// ----------------------------------------------------------------------------

static bool wxMakeTaskBarIcon(wxFrame *wnd)
{ 
#ifdef __WXGTK__
    gtk_widget_realize(wnd->m_widget);
#endif
    
    long data[1];
    
    // KDE 2 & KDE 3:
    Atom _KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR =
        XInternAtom(GetDisplay(), "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", False);
    data[0] = 0;
    XChangeProperty(GetDisplay(), GetXWindow(wnd),
                    _KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR,
                    XA_WINDOW, 32,
                    PropModeReplace, (unsigned char*)data, 1);

    // GNOME 1.2 & KDE 1:
    Atom KWM_DOCKWINDOW =
        XInternAtom(GetDisplay(), "KWM_DOCKWINDOW", False);
    data[0] = 1;
    XChangeProperty(GetDisplay(), GetXWindow(wnd),
                    KWM_DOCKWINDOW,
                    KWM_DOCKWINDOW, 32,
                    PropModeReplace, (unsigned char*)data, 1);

    return true;
}
    
// ----------------------------------------------------------------------------
// wxTaskBarIconArea is the real window that shows the icon:
// ----------------------------------------------------------------------------

class wxTaskBarIconArea : public wxStaticBitmap
{
public:
    wxTaskBarIconArea(wxTaskBarIcon *icon,
                      wxWindow *parent, const wxBitmap &bmp)
        : wxStaticBitmap(parent, -1, bmp), m_icon(icon) {}

protected:
    void OnMouseEvent(wxMouseEvent& event);
    void OnMenuEvent(wxCommandEvent& event);

    wxTaskBarIcon *m_icon;
    
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxTaskBarIconArea, wxStaticBitmap)    
    EVT_MOUSE_EVENTS(wxTaskBarIconArea::OnMouseEvent)
    EVT_MENU(-1, wxTaskBarIconArea::OnMenuEvent)
END_EVENT_TABLE()
    
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
// wxTaskBarIcon class:
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTaskBarIcon, wxEvtHandler)

wxTaskBarIcon::wxTaskBarIcon() : m_iconWnd(NULL)
{
}

wxTaskBarIcon::~wxTaskBarIcon()
{
    if (m_iconWnd)
        RemoveIcon();
}

bool wxTaskBarIcon::IsOk() const
{
    return true;
}

bool wxTaskBarIcon::IsIconInstalled() const
{
    return m_iconWnd != NULL;
}

bool wxTaskBarIcon::SetIcon(const wxIcon& icon, const wxString& tooltip)
{
    if (m_iconWnd)
        RemoveIcon();

    m_iconWnd = new wxFrame(NULL, -1, wxT("taskbar icon"),
                            wxDefaultPosition, wxDefaultSize,
                            wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR);
    wxBitmap bmp;
    bmp.CopyFromIcon(icon);
    wxTaskBarIconArea *area = new wxTaskBarIconArea(this, m_iconWnd, bmp);
    m_iconWnd->SetClientSize(area->GetSize());
#if wxUSE_TOOLTIPS
    if (!tooltip.empty())
        area->SetToolTip(tooltip);
#endif
    if (wxMakeTaskBarIcon(m_iconWnd))
    {
        m_iconWnd->Show();
        m_iconArea = area;
        return true;
    }
    else
    {
        m_iconWnd->Destroy();
        m_iconWnd = NULL;
        return false;
    }
}

bool wxTaskBarIcon::RemoveIcon()
{
    if (!m_iconWnd)
        return false;
    m_iconWnd->Destroy();
    m_iconWnd = NULL;
    return true;
}

bool wxTaskBarIcon::PopupMenu(wxMenu *menu)
{
    if (!m_iconWnd)
        return false;
    wxSize size(m_iconArea->GetSize());
    m_iconArea->PopupMenu(menu, size.x/2, size.y/2);
    return true;
}
