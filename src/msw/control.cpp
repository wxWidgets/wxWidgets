/////////////////////////////////////////////////////////////////////////////
// Name:        msw/control.cpp
// Purpose:     wxControl class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "control.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CONTROLS

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/dcclient.h"
    #include "wx/log.h"
#endif

#include "wx/control.h"

#include "wx/msw/private.h"

#if defined(__WIN95__) && !(defined(__GNUWIN32_OLD__) && !defined(__CYGWIN10__))
    #include <commctrl.h>
#endif

IMPLEMENT_ABSTRACT_CLASS(wxControl, wxWindow)

BEGIN_EVENT_TABLE(wxControl, wxWindow)
    EVT_ERASE_BACKGROUND(wxControl::OnEraseBackground)
END_EVENT_TABLE()

// Item members
wxControl::wxControl()
{
#if WXWIN_COMPATIBILITY
    m_callback = 0;
#endif // WXWIN_COMPATIBILITY
}

wxControl::~wxControl()
{
    m_isBeingDeleted = TRUE;
}


bool wxControl::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    if ( !wxWindow::Create(parent, id, pos, size, style, name) )
        return FALSE;

#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif

    return TRUE;
}

bool wxControl::MSWCreateControl(const wxChar *classname,
                                 const wxString& label,
                                 const wxPoint& pos,
                                 const wxSize& size)
{
    WXDWORD exstyle;
    WXDWORD msStyle = MSWGetStyle(GetWindowStyle(), &exstyle);

    return MSWCreateControl(classname, msStyle, pos, size, label, exstyle);
}

bool wxControl::MSWCreateControl(const wxChar *classname,
                                 WXDWORD style,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 const wxString& label,
                                 WXDWORD exstyle)
{
    // if no extended style given, determine it ourselves
    if ( exstyle == (WXDWORD)-1 )
    {
        exstyle = 0;
        (void) MSWGetStyle(GetWindowStyle(), &exstyle);
    }

    // all controls should have this style
    style |= WS_CHILD;

    // create the control visible if it's currently shown for wxWindows
    if ( m_isShown )
    {
        style |= WS_VISIBLE;
    }

    int x = pos.x == -1 ? 0 : pos.x,
        y = pos.y == -1 ? 0 : pos.y,
        w = size.x == -1 ? 0 : size.x,
        h = size.y == -1 ? 0 : size.y;

    m_hWnd = (WXHWND)::CreateWindowEx
                       (
                        exstyle,            // extended style
                        classname,          // the kind of control to create
                        label,              // the window name
                        style,              // the window style
                        x, y, w, h,         // the window position and size
                        GetHwndOf(GetParent()),  // parent
                        (HMENU)GetId(),     // child id
                        wxGetInstance(),    // app instance
                        NULL                // creation parameters
                       );

    if ( !m_hWnd )
    {
        wxLogDebug(wxT("Failed to create a control of class '%s'"), classname);
        wxFAIL_MSG(_T("something is very wrong"));

        return FALSE;
    }

#if wxUSE_CTL3D
    if ( want3D )
    {
        Ctl3dSubclassCtl(GetHwnd());
        m_useCtl3D = TRUE;
    }
#endif // wxUSE_CTL3D

    // install wxWindows window proc for this window
    SubclassWin(m_hWnd);

    // controls use the same font and colours as their parent dialog by default
    InheritAttributes();

    // set the size now if no initial size specified
    if ( w <= 0 || h <= 0 )
    {
        SetBestSize(size);
    }

    return TRUE;
}

wxBorder wxControl::GetDefaultBorder() const
{
    // we want to automatically give controls a sunken style (confusingly,
    // it may not really mean sunken at all as we map it to WS_EX_CLIENTEDGE
    // which is not sunken at all under Windows XP -- rather, just the default)
    return wxBORDER_SUNKEN;
}

wxSize wxControl::DoGetBestSize() const
{
    return wxSize(DEFAULT_ITEM_WIDTH, DEFAULT_ITEM_HEIGHT);
}

bool wxControl::ProcessCommand(wxCommandEvent& event)
{
#if WXWIN_COMPATIBILITY
    if ( m_callback )
    {
        (void)(*m_callback)(*this, event);

        return TRUE;
    }
    else
#endif // WXWIN_COMPATIBILITY

    return GetEventHandler()->ProcessEvent(event);
}

#ifdef __WIN95__
bool wxControl::MSWOnNotify(int idCtrl,
                            WXLPARAM lParam,
                            WXLPARAM* result)
{
    wxEventType eventType = wxEVT_NULL;

    NMHDR *hdr = (NMHDR*) lParam;
    switch ( hdr->code )
    {
        case NM_CLICK:
            eventType = wxEVT_COMMAND_LEFT_CLICK;
            break;

        case NM_DBLCLK:
            eventType = wxEVT_COMMAND_LEFT_DCLICK;
            break;

        case NM_RCLICK:
            eventType = wxEVT_COMMAND_RIGHT_CLICK;
            break;

        case NM_RDBLCLK:
            eventType = wxEVT_COMMAND_RIGHT_DCLICK;
            break;

        case NM_SETFOCUS:
            eventType = wxEVT_COMMAND_SET_FOCUS;
            break;

        case NM_KILLFOCUS:
            eventType = wxEVT_COMMAND_KILL_FOCUS;
            break;

        case NM_RETURN:
            eventType = wxEVT_COMMAND_ENTER;
            break;

        default:
            return wxWindow::MSWOnNotify(idCtrl, lParam, result);
    }

    wxCommandEvent event(wxEVT_NULL, m_windowId);
    event.SetEventType(eventType);
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}
#endif // Win95

void wxControl::OnEraseBackground(wxEraseEvent& event)
{
    // notice that this 'dumb' implementation may cause flicker for some of the
    // controls in which case they should intercept wxEraseEvent and process it
    // themselves somehow

    RECT rect;
    ::GetClientRect(GetHwnd(), &rect);

    HBRUSH hBrush = ::CreateSolidBrush(wxColourToRGB(GetBackgroundColour()));

    HDC hdc = GetHdcOf((*event.GetDC()));
    int mode = ::SetMapMode(hdc, MM_TEXT);

    ::FillRect(hdc, &rect, hBrush);
    ::DeleteObject(hBrush);
    ::SetMapMode(hdc, mode);
}

WXHBRUSH wxControl::OnCtlColor(WXHDC pDC, WXHWND WXUNUSED(pWnd), WXUINT WXUNUSED(nCtlColor),
#if wxUSE_CTL3D
                               WXUINT message,
                               WXWPARAM wParam,
                               WXLPARAM lParam
#else
                               WXUINT WXUNUSED(message),
                               WXWPARAM WXUNUSED(wParam),
                               WXLPARAM WXUNUSED(lParam)
#endif
    )
{
#if wxUSE_CTL3D
    if ( m_useCtl3D )
    {
        HBRUSH hbrush = Ctl3dCtlColorEx(message, wParam, lParam);
        return (WXHBRUSH) hbrush;
    }
#endif // wxUSE_CTL3D

    HDC hdc = (HDC)pDC;
    wxColour colBack = GetBackgroundColour();

    ::SetBkColor(hdc, wxColourToRGB(colBack));
    ::SetTextColor(hdc, wxColourToRGB(GetForegroundColour()));

    wxBrush *brush = wxTheBrushList->FindOrCreateBrush(colBack, wxSOLID);

    return (WXHBRUSH)brush->GetResourceHandle();
}

WXDWORD wxControl::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    long msStyle = wxWindow::MSWGetStyle(style, exstyle);

    if ( AcceptsFocus() )
    {
        msStyle |= WS_TABSTOP;
    }

    return msStyle;
}

// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

// Call this repeatedly for several wnds to find the overall size
// of the widget.
// Call it initially with -1 for all values in rect.
// Keep calling for other widgets, and rect will be modified
// to calculate largest bounding rectangle.
void wxFindMaxSize(WXHWND wnd, RECT *rect)
{
    int left = rect->left;
    int right = rect->right;
    int top = rect->top;
    int bottom = rect->bottom;

    GetWindowRect((HWND) wnd, rect);

    if (left < 0)
        return;

    if (left < rect->left)
        rect->left = left;

    if (right > rect->right)
        rect->right = right;

    if (top < rect->top)
        rect->top = top;

    if (bottom > rect->bottom)
        rect->bottom = bottom;
}

#endif // wxUSE_CONTROLS
