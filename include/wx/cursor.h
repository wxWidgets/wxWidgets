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
*/
class wxBusyCursorSuspender
{
public:
   wxBusyCursorSuspender()
      {
         m_wasBusy = wxIsBusy();
         if(m_wasBusy)
            wxEndBusyCursor();
      }
   ~wxBusyCursorSuspender()
      {
         if(m_wasBusy)
            wxBeginBusyCursor();
      }
 private:
   bool m_wasBusy;
};
#endif
    // _WX_CURSOR_H_BASE_
