///////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/toplevel.cpp
// Purpose:     implements wxTopLevelWindow for Palm OS
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by: Wlodzimierz ABX Skiba - more than minimal functionality
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne <wbo@freeshell.org>, Wlodzimierz Skiba
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "toplevel.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/toplevel.h"
    #include "wx/dialog.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/frame.h"
    #include "wx/containr.h"        // wxSetFocusToChild()
#endif //WX_PRECOMP

#include "wx/module.h"

#include "wx/display.h"

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// the name of the default wxWidgets class
extern const wxChar *wxCanvasClassName;

// Pointer to the currently active frame for the form event handler.
wxTopLevelWindowPalm* ActiveParentFrame;

// ============================================================================
// wxTopLevelWindowPalm implementation
// ============================================================================

BEGIN_EVENT_TABLE(wxTopLevelWindowPalm, wxTopLevelWindowBase)
    EVT_ACTIVATE(wxTopLevelWindowPalm::OnActivate)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxTopLevelWindowPalm creation
// ----------------------------------------------------------------------------

void wxTopLevelWindowPalm::Init()
{
}

WXDWORD wxTopLevelWindowPalm::PalmGetStyle(long style, WXDWORD *exflags) const
{
    return 0;
}

WXHWND wxTopLevelWindowPalm::PalmGetParent() const
{
    return NULL;
}

bool wxTopLevelWindowPalm::Create(wxWindow *parent,
                                  wxWindowID id,
                                  const wxString& title,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  long style,
                                  const wxString& name)
{
    // this is a check for limitation mentioned before FrameFormHandleEvent() code
    if(wxTopLevelWindows.GetCount()>0)
        return false;

    ActiveParentFrame=NULL;

    wxTopLevelWindows.Append(this);

    if ( parent )
        parent->AddChild(this);

    SetId( id == wxID_ANY ? NewControlId() : id );

    WinConstraintsType constraints;
    memset(&constraints, 0, sizeof(WinConstraintsType));
    // position
    constraints.x_pos = ( pos.x == wxDefaultCoord ) ? winUndefConstraint : pos.x;
    constraints.y_pos = ( pos.y == wxDefaultCoord ) ? winUndefConstraint : pos.y;
    // size
    constraints.x_min = winUndefConstraint;
    constraints.x_max = winMaxConstraint;
    constraints.x_pref = ( size.x == wxDefaultCoord ) ? winUndefConstraint : size.x;
    constraints.y_min = winUndefConstraint;
    constraints.y_max = winMaxConstraint;
    constraints.y_pref = ( size.y == wxDefaultCoord ) ? winUndefConstraint : size.y;

    FrameForm = FrmNewFormWithConstraints(
                    GetId(),
                    title.c_str(),
                    winFlagBackBuffer,
                    &constraints,
                    0,
                    NULL,
                    0,
                    NULL,
                    0
                );

    if(FrameForm==NULL)
        return false;

    FrmSetEventHandler(FrameForm,FrameFormHandleEvent);

    FrmSetActiveForm(FrameForm);

    ActiveParentFrame=this;

    return true;
}

wxTopLevelWindowPalm::~wxTopLevelWindowPalm()
{
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowPalm showing
// ----------------------------------------------------------------------------

void wxTopLevelWindowPalm::DoShowWindow(int nShowCmd)
{
}

bool wxTopLevelWindowPalm::Show(bool show)
{
    FrmDrawForm(FrameForm);

    wxPaintEvent event(m_windowId);
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);

    return true;
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowPalm maximize/minimize
// ----------------------------------------------------------------------------

void wxTopLevelWindowPalm::Maximize(bool maximize)
{
}

bool wxTopLevelWindowPalm::IsMaximized() const
{
    return false;
}

void wxTopLevelWindowPalm::Iconize(bool iconize)
{
}

bool wxTopLevelWindowPalm::IsIconized() const
{
    return false;
}

void wxTopLevelWindowPalm::Restore()
{
}

void wxTopLevelWindowPalm::DoGetSize( int *width, int *height ) const
{
    RectangleType rect;
    FrmGetFormBounds( GetForm() , &rect );
    if(width)
        *width = rect.extent.x;
    if(height)
        *height = rect.extent.y;
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowPalm fullscreen
// ----------------------------------------------------------------------------

bool wxTopLevelWindowPalm::ShowFullScreen(bool show, long style)
{
    return false;
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowPalm misc
// ----------------------------------------------------------------------------

void wxTopLevelWindowPalm::SetIcon(const wxIcon& icon)
{
}

void wxTopLevelWindowPalm::SetIcons(const wxIconBundle& icons)
{
}

bool wxTopLevelWindowPalm::EnableCloseButton(bool enable)
{
    return false;
}

FormType *wxTopLevelWindowPalm::GetForm() const
{
    return FrmGetActiveForm();
}

#ifndef __WXWINCE__

bool wxTopLevelWindowPalm::SetShape(const wxRegion& region)
{
    return false;
}

#endif // !__WXWINCE__

// ----------------------------------------------------------------------------
// wxTopLevelWindow event handling
// ----------------------------------------------------------------------------

void wxTopLevelWindowPalm::OnActivate(wxActivateEvent& event)
{
}

/* Palm OS Event handler for the window
 *
 * This function *must* be located outside of the wxTopLevelWindow class because
 * the Palm OS API expects a standalone C function as a callback.  You cannot
 * pass a pointer to a member function of a C++ class as a callback because the
 * prototypes don't match.  (A member function has a hidden "this" pointer as
 * its first parameter).
 *
 * This event handler uses a global pointer to the current wxFrame to process
 * the events generated by the Palm OS form API.  I know this is ugly, but right
 * now I can't think of any other way to deal with this problem.  If someone
 * finds a better solution, please let me know.  My email address is
 * wbo@freeshell.org
 */
static Boolean FrameFormHandleEvent(EventType* pEvent)
{
    wxFrame*    frame = wxDynamicCast(ActiveParentFrame,wxFrame);
    Boolean     fHandled = false;
    FormType*   pForm;
    WinHandle   hWindow;
    int         ItemID=0;

    switch (pEvent->eType) {
        case ctlSelectEvent:
            break;
#if wxUSE_MENUS_NATIVE
        case menuOpenEvent:
            fHandled = frame->HandleMenuOpen();
            break;
        case menuEvent:
            ItemID = pEvent->data.menu.itemID;
            fHandled = frame->HandleMenuSelect(ItemID);
            break;
#endif
        default:
            break;
    }

    return fHandled;
}
