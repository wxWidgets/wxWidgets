///////////////////////////////////////////////////////////////////////////////
// Name:        testableframe.h
// Purpose:     An improved wxFrame for unit-testing
// Author:      Steven Lamerton
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

private:
    friend class EventCounter;

    int GetEventCount(wxEventType type);
    void ClearEventCount(wxEventType type);

    wxLongToLongHashMap m_count;
};

class EventCounter
{
public:
    EventCounter(wxWindow* win, wxEventType type);
    ~EventCounter();

    int GetCount() { return m_frame->GetEventCount(m_type); }
    void Clear() { m_frame->ClearEventCount(m_type); }

private:
    wxEventType m_type;
    wxTestableFrame* m_frame;
    wxWindow* m_win;
};
