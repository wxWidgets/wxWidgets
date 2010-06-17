///////////////////////////////////////////////////////////////////////////////
// Name:        testableframe.cpp
// Purpose:     An improved wxFrame for unit-testing
// Author:      Steven Lamerton
// RCS-ID:      $Id:$
// Copyright:   (c) 2010 Steven Lamerton
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "testprec.h"
 
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "testableframe.h"

wxTestableFrame::wxTestableFrame(wxWindow *parent, 
                                 wxWindowID winid, 
                                 const wxString& title,
                                 const wxPoint& pos,
                                 const wxSize& size, 
                                 long style,
                                 const wxString& name)
{
    wxFrame::Create(parent, winid, title, pos, size, style, name);
    m_count = 0;
}

void wxTestableFrame::OnEvent(wxEvent& WXUNUSED(evt))
{
    m_count++;
}

int wxTestableFrame::GetEventCount()
{
    int count = m_count;
    m_count = 0;
    return count;
}
