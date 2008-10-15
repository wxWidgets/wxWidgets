/////////////////////////////////////////////////////////////////////////////
// Name:        ctrlmaskout.h
// Purpose:     Defines the CtrlMaskOut class
// Author:      Utensil Candel (UtensilCandel@@gmail.com)
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _CTRLMASKOUT_H_
#define _CTRLMASKOUT_H_

#include "wx/filename.h"


// ----------------------------------------------------------------------------
// class CtrlMaskOut
// ----------------------------------------------------------------------------

class CtrlMaskOut : public wxEvtHandler
{
public:
    CtrlMaskOut();
    ~CtrlMaskOut();

public:
    void OnLeftButtonDown(wxMouseEvent& event);
    void OnLeftButtonUp(wxMouseEvent& event);
    void OnMouseMoving(wxMouseEvent& event);

    void Capture(wxRect rect, wxString fileName);
    void Capture(int x, int y, int width, int height, wxString fileName);

    wxString GetDefaultDirectory() const
        { return m_defaultDir; }
    wxString GetDefaultDirectoryAbsPath() const
        {
            wxFileName output = wxFileName::DirName(GetDefaultDirectory());
            output.MakeAbsolute();
            return output.GetFullPath();
        }

private:
    // Helper functions
    void CreateMask(wxWindow* parent);
    void DestroyMask();
    void DetermineCtrlNameAndRect();

    void OnTimingFinished(wxTimerEvent& event);


    // Data members
    wxString m_defaultDir;

    wxString m_controlName;
    wxRect m_currentRect;
    wxCoord m_inflateBorder;

    wxFrame * m_mask;

    bool m_isTiming;
};

#endif // _CTRLMASKOUT_H_


