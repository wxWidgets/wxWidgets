/////////////////////////////////////////////////////////////////////////////
// Name:        control.cpp
// Purpose:     wxControl class
// Author:      David Webster
// Modified by:
// Created:     09/17/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "control.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/event.h"
#include "wx/app.h"
#include "wx/dcclient.h"
#endif
#include "wx/os2/private.h"
#include "wx/control.h"

IMPLEMENT_ABSTRACT_CLASS(wxControl, wxWindow)

BEGIN_EVENT_TABLE(wxControl, wxWindow)
    EVT_ERASE_BACKGROUND(wxControl::OnEraseBackground)
END_EVENT_TABLE()

// Item members
wxControl::wxControl()
{
  m_backgroundColour = *wxWHITE;
  m_foregroundColour = *wxBLACK;

#if WXWIN_COMPATIBILITY
  m_callback = 0;
#endif // WXWIN_COMPATIBILITY
}

bool wxControl::Create(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size, long style,
#if wxUSE_VALIDATORS
                       const wxValidator& validator,
#endif
                       const wxString& name)
{
    bool rval = wxWindow::Create(parent, id, pos, size, style, name);
    if (rval) {
#if wxUSE_VALIDATORS
        SetValidator(validator);
#endif
    }
    return rval;
}

wxControl::~wxControl()
{
    m_isBeingDeleted = TRUE;
}

bool wxControl::OS2CreateControl(const wxChar *classname,
                                 WXDWORD style,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 const wxString& label,
                                 WXDWORD exstyle)
{
    // VZ: if someone could put a comment here explaining what exactly this is
    //     needed for, it would be nice...
    bool want3D;

    // if no extended style given, determine it ourselves
    if ( exstyle == (WXDWORD)-1 )
    {
        exstyle = GetExStyle(style);
    }

// TODO:
/*
    // all controls have these childs (wxWindows creates all controls visible
    // by default)
    style |= WS_CHILD | WS_VISIBLE;

    m_hWnd = (WXHWND)::CreateWindowEx
                       (
                        exstyle,            // extended style
                        classname,          // the kind of control to create
                        label,              // the window name
                        style,              // the window style
                        pos.x, pos.y,       // the window position
                        size.x, size.y,     //            and size
                        GetHwndOf(GetParent()),  // parent
                        (HMENU)GetId(),     // child id
                        wxGetInstance(),    // app instance
                        NULL                // creation parameters
                       );

    if ( !m_hWnd )
    {
#ifdef __WXDEBUG__
        wxLogError(wxT("Failed to create a control of class '%s'"), classname);
#endif // DEBUG

        return FALSE;
    }

#if wxUSE_CTL3D
    if ( want3D )
    {
        Ctl3dSubclassCtl(GetHwnd());
        m_useCtl3D = TRUE;
    }
#endif // wxUSE_CTL3D

    // subclass again for purposes of dialog editing mode
    SubclassWin(m_hWnd);

    // controls use the same font and colours as their parent dialog by default
    InheritAttributes();
*/
    return TRUE;
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
        (void)(*m_callback)(this, event);

        return TRUE;
    }
    else
#endif // WXWIN_COMPATIBILITY

    return GetEventHandler()->ProcessEvent(event);
}

bool wxControl::OS2OnNotify(int idCtrl,
                            WXLPARAM lParam,
                            WXLPARAM* result)
{
    wxCommandEvent event(wxEVT_NULL, m_windowId);
    wxEventType eventType = wxEVT_NULL;
/* TODO:
    NMHDR *hdr1 = (NMHDR*) lParam;
    switch ( hdr1->code )
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
            return wxWindow::OS2OnNotify(idCtrl, lParam, result);
    }
*/
    event.SetEventType(eventType);
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

void wxControl::OnEraseBackground(wxEraseEvent& event)
{
    // In general, you don't want to erase the background of a control,
    // or you'll get a flicker.
    // TODO: move this 'null' function into each control that
    // might flicker.

    RECT rect;
/*
* below is msw code.
* TODO: convert to PM Code
*   ::GetClientRect((HWND) GetHWND(), &rect);
*
*   HBRUSH hBrush = ::CreateSolidBrush(PALETTERGB(GetBackgroundColour().Red(),
*                                                 GetBackgroundColour().Green(),
*                                                 GetBackgroundColour().Blue()));
*   int mode = ::SetMapMode((HDC) event.GetDC()->GetHDC(), MM_TEXT);
*
*   ::FillRect ((HDC) event.GetDC()->GetHDC(), &rect, hBrush);
*   ::DeleteObject(hBrush);
*   ::SetMapMode((HDC) event.GetDC()->GetHDC(), mode);
*/
}

WXDWORD wxControl::GetExStyle(WXDWORD& style) const
{
    bool want3D;
    WXDWORD exStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &want3D) ;

    // Even with extended styles, need to combine with FS_BORDER
    // for them to look right.  Check it out later, base window style does
    // not designate BORDERS.  Down in Frame and And controls.

    if ( want3D || wxStyleHasBorder(m_windowStyle) )
        style |= FS_BORDER;

    return exStyle;
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
    int left = rect->xLeft;
    int right = rect->xRight;
    int top = rect->yTop;
    int bottom = rect->yBottom;

    ::WinQueryWindowRect((HWND) wnd, rect);

    if (left < 0)
        return;

    if (left < rect->xLeft)
        rect->xLeft = left;

    if (right > rect->xRight)
        rect->xRight = right;

    if (top < rect->yTop)
        rect->yTop = top;

    if (bottom > rect->yBottom)
        rect->yBottom = bottom;
}

