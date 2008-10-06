/////////////////////////////////////////////////////////////////////////////
// Name:        screenshot_main.h
// Purpose:     Defines the Application Frame
// Author:      Utensil Candel (UtensilCandel@@gmail.com)
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef WXSCREENSHOTMAIN_H
#define WXSCREENSHOTMAIN_H

#include "guiframe.h"

// when defined to 1, wxAui for the main frame
#define SCREENSHOTGEN_USE_AUI       0


class CtrlMaskOut;


class ScreenshotFrame
#if SCREENSHOTGEN_USE_AUI
: public AuiGUIFrame
#else
: public GUIFrame
#endif
{
public:
    ScreenshotFrame(wxFrame *frame);
    ~ScreenshotFrame();

protected:      // event handlers

    virtual void OnClose(wxCloseEvent& event);
    virtual void OnQuit(wxCommandEvent& event);
    virtual void OnAbout(wxCommandEvent& event);
    virtual void OnSeeScreenshots( wxCommandEvent& event);

    virtual void OnCaptureFullScreen( wxCommandEvent& event );
    virtual void OnCaptureRect( wxCommandEvent& event );
    virtual void OnEndCaptureRect( wxCommandEvent& event );
    virtual void OnCaptureAllControls( wxCommandEvent& event );

#if SCREENSHOTGEN_USE_AUI
    virtual void OnNotebookPageChanged( wxAuiNotebookEvent& event );
    virtual void OnNotebookPageChanging( wxAuiNotebookEvent& event );
#else
    virtual void OnNotebookPageChanged( wxNotebookEvent& event );
    virtual void OnNotebookPageChanging( wxNotebookEvent& event );
#endif

private:
    // Helper functions
    void InitFBControls();

    // Data members
    bool capturingRect;
    CtrlMaskOut * m_maskout;
};

#endif // WXSCREENSHOTMAIN_H
