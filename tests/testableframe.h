///////////////////////////////////////////////////////////////////////////////
// Name:        testableframe.h
// Purpose:     An improved wxFrame for unit-testing
// Author:      Steven Lamerton
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/frame.h"
#include "wx/hashmap.h"
#include "wx/event.h"

class wxTestableFrame : public wxFrame
{
public:
    wxTestableFrame();

    void OnEvent(wxEvent& evt);

    //wxEVT_ANY get the count for all events or a type can be specified
    int GetEventCount(wxEventType type = wxEVT_ANY);

    //Used to clear an event count, after disconnecting a counter for example
    void ClearEventCount(wxEventType type);

private:
    wxLongToLongHashMap m_count;
};

class EventCounter
{
public:
    EventCounter(wxWindow* win, wxEventType type);
    ~EventCounter();

private:
    wxEventType m_type;
    wxTestableFrame* m_frame;
    wxWindow* m_win;
};
