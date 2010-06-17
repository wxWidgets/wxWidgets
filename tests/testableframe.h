///////////////////////////////////////////////////////////////////////////////
// Name:        testableframe.h
// Purpose:     An improved wxFrame for unit-testing
// Author:      Steven Lamerton
// RCS-ID:      $Id:$
// Copyright:   (c) 2010 Steven Lamerton
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/frame.h"

class wxTestableFrame : public wxFrame
{
public:
    wxTestableFrame(wxWindow *parent,
                    wxWindowID winid,
                    const wxString& title,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE,
                    const wxString& name = wxFrameNameStr);

    void OnEvent(wxEvent& evt);
    int GetEventCount();

private:
    int m_count;
};
