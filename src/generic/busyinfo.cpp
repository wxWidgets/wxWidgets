/////////////////////////////////////////////////////////////////////////////
// Name:        busyinfo.cpp
// Purpose:     Information window when app is busy
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

#include <wx/wxprec.h>

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include <wx/wx.h>
#endif

#if wxUSE_BUSYINFO

#include "wx/busyinfo.h"





wxInfoFrame::wxInfoFrame(wxWindow *parent, const wxString& message)
    : wxFrame(parent, -1, "", wxPoint(0, 0), wxSize(400, 80), wxTHICK_FRAME | wxSIMPLE_BORDER | wxFRAME_TOOL_WINDOW)
{
    wxPanel *p = new wxPanel(this);
    wxStaticText *s = new wxStaticText(p, -1, message, wxPoint(20, 20), wxSize(360, 40), wxALIGN_CENTER);
    Centre(wxBOTH);
    p -> SetCursor(*wxHOURGLASS_CURSOR);
    s -> SetCursor(*wxHOURGLASS_CURSOR);
}




wxBusyInfo::wxBusyInfo(const wxString& message) : wxObject()
{
    m_InfoFrame = new wxInfoFrame(NULL, message);
    m_InfoFrame -> Show(TRUE);
    wxYield();
    m_InfoFrame -> Refresh();
    wxYield();
}



wxBusyInfo::~wxBusyInfo()
{
    m_InfoFrame -> Show(FALSE);
    m_InfoFrame -> Close();
    wxYield();
}



#endif
