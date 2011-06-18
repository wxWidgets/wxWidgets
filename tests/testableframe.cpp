///////////////////////////////////////////////////////////////////////////////
// Name:        testableframe.cpp
// Purpose:     An improved wxFrame for unit-testing
// Author:      Steven Lamerton
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/app.h"
#include "testableframe.h"

wxTestableFrame::wxTestableFrame() : wxFrame(NULL, wxID_ANY, "Test Frame")
{
}

void wxTestableFrame::OnEvent(wxEvent& evt)
{
    m_count[evt.GetEventType()]++;

    if(! evt.IsCommandEvent() )
        evt.Skip();
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

void wxTestableFrame::ClearEventCount(wxEventType type)
{
    m_count[type] = 0;
}

EventCounter::EventCounter(wxWindow* win, wxEventType type) : m_type(type),
                                                              m_win(win)

{
    m_frame = wxStaticCast(wxTheApp->GetTopWindow(),
                           wxTestableFrame);

    m_win->Connect(m_type,
                   wxEventHandler(wxTestableFrame::OnEvent),
                   NULL,
                   m_frame);
}

EventCounter::~EventCounter()
{
    m_win->Disconnect(m_type,
                      wxEventHandler(wxTestableFrame::OnEvent),
                      NULL,
                      m_frame);

    //This stops spurious counts from previous tests
    m_frame->ClearEventCount(m_type);

    m_frame = NULL;
    m_win = NULL;
}
