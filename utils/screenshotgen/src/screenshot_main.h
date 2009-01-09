/////////////////////////////////////////////////////////////////////////////
// Name:        screenshot_main.h
// Purpose:     Defines the Application Frame
// Author:      Utensil Candel (UtensilCandel@@gmail.com)
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _SCREENSHOT_MAIN_H_
#define _SCREENSHOT_MAIN_H_

#include "guiframe.h"

class ScreenshotFrame : public GUIFrame
{
public:
    ScreenshotFrame(wxFrame *frame);
    ~ScreenshotFrame() {}

protected:      // event handlers

    virtual void OnClose(wxCloseEvent& event);
    virtual void OnQuit(wxCommandEvent& event);
    virtual void OnAbout(wxCommandEvent& event);
    virtual void OnSeeScreenshots( wxCommandEvent& event);

    virtual void OnCaptureFullScreen( wxCommandEvent& event );
    virtual void OnCaptureAllControls( wxCommandEvent& event );


private:

    // Before a config class is written, these two functions are placed here.
    // It's only a transition and they wil be removed soon
    wxString GetDefaultDirectory() const { return _T("screenshots"); }

    wxString GetDefaultDirectoryAbsPath() const
    {
        wxFileName output = wxFileName::DirName(GetDefaultDirectory());
        output.MakeAbsolute();
        return output.GetFullPath();
    }
};

#endif // _SCREENSHOT_MAIN_H_
