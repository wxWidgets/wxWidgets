/////////////////////////////////////////////////////////////////////////////
// Name:        ctrlmaskout.h
// Purpose:     Defines the CtrlMaskOut class
// Author:      Utensil Candel (UtensilCandel@@gmail.com)
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_CTRL_MASK_OUT
#define WX_CTRL_MASK_OUT

#include <wx/frame.h>


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

    wxString GetDefaultDirectory(){return m_defaultDir;}

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

#endif // WX_CTRL_MASK_OUT


