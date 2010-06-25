///////////////////////////////////////////////////////////////////////////////
// Name:        testableframe.cpp
// Purpose:     An improved wxFrame for unit-testing
// Author:      Steven Lamerton
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "testprec.h"
 
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "testableframe.h"

wxTestableFrame::wxTestableFrame() : wxFrame(NULL, wxID_ANY, "Test Frame")
{
}

void wxTestableFrame::OnEvent(wxEvent& evt)
{
    m_count[evt.GetEventType()]++;
}

int wxTestableFrame::GetEventCount(wxEventType type)
{
    if (type == wxEVT_ANY)
    {
        //Get the total event count
        long total = 0;

        for(wxLongToLongHashMap::iterator iter = m_count.begin(); 
            iter != m_count.end(); 
            iter++)
        {
            total += iter->second;
            iter->second = 0;
        }

        return total;
    }
    else
    {
        long count = m_count[type];
        m_count[type] = 0;
        return count;
    }
}

void wxTestableFrame::CountWindowEvents(wxWindow* win, wxEventType type)
{
    win->Connect(type,
                 wxEventHandler(wxTestableFrame::OnEvent),
                 NULL,
                 this);
}
