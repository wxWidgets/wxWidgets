/////////////////////////////////////////////////////////////////////////////
// Name:        progdlgg.h
// Purpose:     wxProgressDialog class
// Author:      Karsten Ballüder
// Modified by:
// Created:     09.05.1999
// RCS-ID:      $Id$
// Copyright:   (c) Karsten Ballüder
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __PROGDLGH_G__
#define __PROGDLGH_G__

#ifdef __GNUG__
#pragma interface "progdlgg.h"
#endif

#include "wx/setup.h"

#if wxUSE_PROGRESSDLG

#include "wx/frame.h"

/** Progress dialog which shows a moving progress bar.
    Taken from the Mahogany project.*/

class WXDLLEXPORT wxProgressDialog : public wxFrame
{
DECLARE_DYNAMIC_CLASS(wxProgressDialog)
public:
   /** Creates and displays dialog, disables event handling for other
       frames or parent frame to avoid recursion problems.
       @param title title for window
       @param message message to display in window
       @param maximum value for status bar, if <= 0, no bar is shown
       @param parent window or NULL
       @param style is the bit mask of wxPD_XXX constants from wx/defs.h
   */
   wxProgressDialog(const wxString &title, wxString const &message,
                   int maximum = 100,
                   wxWindow *parent = NULL,
                   int style = wxPD_APP_MODAL | wxPD_AUTO_HIDE);
   /** Destructor.
       Re-enables event handling for other windows.
   */
   ~wxProgressDialog();

   /** Update the status bar to the new value.
       @param value new value
       @param newmsg if used, new message to display
       @returns true if ABORT button has not been pressed
   */
   bool Update(int value = -1, const wxString& newmsg = _T(""));

   /** Can be called to continue after the cancel button has been pressed, but
       the program decided to continue the operation (e.g., user didn't
       confirm it)
   */
   void Resume() { m_state = Continue; }

   /// Callback for optional abort button
   void OnCancel(wxCommandEvent& WXUNUSED(event)) { m_state = Canceled; }

   /// callback to disable "hard" window closing
   void OnClose(wxCloseEvent& event);

private:
   /// the status bar
   class wxGauge *m_gauge;
   /// the message displayed
   class wxStaticText *m_msg;
   /// disable all or parent window only
   bool m_disableParentOnly;
   /// parent window
   class wxWindow *m_parent;
   /// continue processing or not (return value for Update())
   enum
   {
      Uncancelable = -1,   // dialog can't be canceled
      Canceled,            // can be cancelled and, in fact, was
      Continue,            // can be cancelled but wasn't
      Finished             // finished, waiting to be removed from screen
   } m_state;
   /// the abort button (or NULL if none)
   class wxButton *m_btnAbort;
   /// the maximum value
   int m_maximum;

   DECLARE_EVENT_TABLE()
};
#endif

#endif
    // __PROGDLGH_G__
