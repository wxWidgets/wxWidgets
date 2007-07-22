/////////////////////////////////////////////////////////////////////////////
// Name:        src/web/utils.cpp
// Purpose:     Various utilities
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: utils.cpp 46158 2007-05-22 02:30:01Z VZ $
// Copyright:   (c) Julian Smart, John Wilmes
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/utils.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/window.h"
    #include "wx/cursor.h"
    #include "wx/msgdlg.h"
#endif

#include "wx/apptrait.h"
#include "wx/evtloop.h"
#include "wx/unix/execute.h"
#include "wx/unix/private/timer.h"

// ----------------------------------------------------------------------------
// wxExecute stuff
// ----------------------------------------------------------------------------

WX_DECLARE_HASH_MAP( int, wxEndProcessData*, wxIntegerHash, wxIntegerEqual, wxProcMap );

static wxProcMap *gs_procmap;

int wxAddProcessCallback(wxEndProcessData *proc_data, int fd)
{
    if (!gs_procmap) gs_procmap = new wxProcMap();
    (*gs_procmap)[fd] = proc_data;
    return 1;
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

void wxBell()
{
    wxFAIL_MSG(wxT("wxBell unsupported in wxWeb port"));
}

wxPortId wxGUIAppTraits::GetToolkitVersion(int *verMaj, int *verMin) const
{
    *verMaj = 0;
    *verMin = 0;
    return wxPORT_WEB;
}

wxEventLoopBase* wxGUIAppTraits::CreateEventLoop()
{
    return new wxEventLoop;
}

wxTimerImpl* wxGUIAppTraits::CreateTimerImpl(wxTimer* timer) {
    return new wxUnixTimerImpl(timer);
}

// ----------------------------------------------------------------------------
// display info
// ----------------------------------------------------------------------------

bool wxGetKeyState(wxKeyCode key) {
    //TODO
    return false;
}

wxMouseState wxGetMouseState() {
    //TODO
    wxMouseState ms;
    return ms;
}

wxPoint wxGetMousePosition() {
    //TODO
    return wxPoint(0,0);
}

void wxGetMousePosition( int* x, int* y )
{
    wxPoint p = wxGetMousePosition();
    *x = p.x;
    *y = p.y;
};

// Return true if we have a colour display
bool wxColourDisplay()
{
    // TODO
    // collect this information on with the first request from the client
    return true;
}

// Returns depth of screen
int wxDisplayDepth()
{
    //TODO
    //same approach as above
    return 16;
}

void wxDisplaySize(int *width, int *height)
{
    //TODO - see above
    if (width)
        *width = 800;
    if (height)
        *height = 600;
}

void wxDisplaySizeMM(int *width, int *height)
{
    //TODO - see above
    if (width)
        *width = -1;
    if (height)
        *height = -1;
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    // This is supposed to return desktop dimensions minus any window
    // manager panels, menus, taskbars, etc.  If there is a way to do that
    // for this platform please fix this function, otherwise it defaults
    // to the entire desktop.
    if (x) *x = 0;
    if (y) *y = 0;
    wxDisplaySize(width, height);
}

wxWindow* wxFindWindowAtPoint(const wxPoint& pt)
{
    return wxGenericFindWindowAtPoint(pt);
}

wxWindow* wxFindWindowAtPointer(wxPoint& pt) {
    wxGetMousePosition(&pt.x, &pt.y);
    return wxFindWindowAtPoint(pt);
}
