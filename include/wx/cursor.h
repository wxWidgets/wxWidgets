#ifndef _WX_CURSOR_H_BASE_
#define _WX_CURSOR_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/cursor.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/cursor.h"
#elif defined(__WXGTK__)
#include "wx/gtk/cursor.h"
#elif defined(__WXQT__)
#include "wx/qt/cursor.h"
#elif defined(__WXMAC__)
#include "wx/mac/cursor.h"
#elif defined(__WXPM__)
#include "wx/os2/cursor.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/cursor.h"
#endif

#include "wx/utils.h"
/* This is a small class which can be used by all ports
   to temporarily suspend the busy cursor. Useful in modal
   dialogs.

   Actually that is not (any longer) quite true..  currently it is
   only used in wxGTK Dialog::ShowModal() and now uses static
   wxBusyCursor methods that are only implemented for wxGTK so far.
   The BusyCursor handling code should probably be implemented in
   common code somewhere instead of the separate implementations we
   currently have.  Also the name BusyCursorSuspender is a little
   misleading since it doesn't actually suspend the BusyCursor, just
   masks one that is already showing.
   If another call to wxBeginBusyCursor is made while this is active
   the Busy Cursor will again be shown.  But at least now it doesn't
   interfere with the state of wxIsBusy() -- RL

*/
class wxBusyCursorSuspender
{
public:
    wxBusyCursorSuspender()
    {
        if( wxIsBusy() )
        {
            wxSetCursor( wxBusyCursor::GetStoredCursor() );
            wxYield();
        }
    }
    ~wxBusyCursorSuspender()
    {
        if( wxIsBusy() )
        {
            wxSetCursor( wxBusyCursor::GetBusyCursor() );
            wxYield();
        }
    }
};
#endif
    // _WX_CURSOR_H_BASE_
