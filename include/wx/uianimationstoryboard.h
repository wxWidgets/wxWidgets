///////////////////////////////////////////////////////////////////////////////
// Name:        wx/uianimationstoryboard.h
// Purpose:     Declaration of the storyboard base classe
// Author:      Chiciu Bogdan Gabriel
// Created:     2011-05-11
// RCS-ID:      $Id$
// Copyright:   (c) 2011 wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UI_ANIMATION_STORYBOARD_H_BASE_
#define _WX_UI_ANIMATION_STORYBOARD_H_BASE_

enum wxStoryboardStatus
{
    wxSTORYBOARD_STATUS_FINISHED,
    wxSTORYBOARD_STATUS_STARTED,
    // Initial storyboard status. Before Start is called.
    wxSTORYBOARD_STATUS_IDLE
};

enum wxStoryboardFillMode
{
    wxSTORYBOARD_FILL_MODE_HOLD_END,
    wxSTORYBOARD_FILL_MODE_STOP
};

enum wxStoryboardRepeat
{
    wxSTORYBOARD_REPEAT_FOREVER = -1
};

class WXDLLIMPEXP_ANIMATION wxUIAnimationStoryboardBase : public wxEvtHandler
{
public:
    wxUIAnimationStoryboardBase(wxControl* targetControl = NULL)
        : m_targetControl(targetControl), m_status(wxSTORYBOARD_STATUS_IDLE),
        m_fillMode(wxSTORYBOARD_FILL_MODE_HOLD_END), m_repeatCount(0)
    {
    }

    virtual ~wxUIAnimationStoryboardBase()
    {
    }
    
    // Return the status of our storyboard.
    wxStoryboardStatus GetStoryboardStatus() const
    {
        return m_status;
    }

    void SetFillMode(wxStoryboardFillMode fillMode)
    {
        m_fillMode = fillMode;
    }

    wxStoryboardFillMode GetFillMode() const
    {
        return m_fillMode;
    }

    int GetRepeatCount()
    {
        return m_repeatCount;
    }

    void SetRepeatCount(int repeatCount)
    {
        m_repeatCount = repeatCount;
    }

    void SetAnimationTarget(wxControl* targetControl)
    {
        m_targetControl = targetControl;
    }

    virtual void Start() = 0;

    virtual void Stop() = 0;
protected:
    void SetStoryboardStatus(wxStoryboardStatus status)
    {
        m_status = status;
    }

    wxControl* m_targetControl;
    wxStoryboardStatus m_status;
    wxStoryboardFillMode m_fillMode;
    int m_repeatCount;
};

// ----------------------------------------------------------------------------
// Storyboard events.
// ----------------------------------------------------------------------------

DECLARE_EVENT_TYPE(wxEVT_STORYBOARD, -1)

// Basic storyboard event. Fired when a storyboard starts or finishes.
class wxUIAnimationStoryboardEvent : public wxEvent
{
public:
    // Since this event will mostly be used to delete the storyboard when
    // animations finish we are passing the storyboard that sends this event
    // as well.
    wxUIAnimationStoryboardEvent(wxUIAnimationStoryboardBase* sender)
        : wxEvent(0, wxEVT_STORYBOARD)
    {
        m_sender = sender;
    }

    wxUIAnimationStoryboardBase* GetSender() const
    {
        return m_sender;
    }

    // Short hand accessor.
    wxStoryboardStatus GetStoryboardStatus() const
    {
        return m_sender->GetStoryboardStatus();
    }

    wxEvent* Clone() const
    {
        return new wxUIAnimationStoryboardEvent(*this);
    }

private:
    wxUIAnimationStoryboardEvent(const wxUIAnimationStoryboardEvent& event)
        : wxEvent(event)
    {
        m_sender = event.GetSender();
    }

    wxUIAnimationStoryboardBase* m_sender;
};

typedef void (wxEvtHandler::*wxStoryboardEventFunction)(wxUIAnimationStoryboardEvent&);

#define EVT_STORYBOARD(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_STORYBOARD, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( wxStoryboardEventFunction, & fn ), (wxObject *) NULL ),

#define wxStoryboardEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxStoryboardEventFunction, func)

// ----------------------------------------------------------------------------
// Platform-specific storyboard classes.
// ----------------------------------------------------------------------------
#if defined(__WXMSW__)
    #define wxUIAnimationStoryboardMSW wxUIAnimationStoryboard
    #include "wx/msw/uianimationstoryboard.h"
#elif defined(__WXCOCOA__)
    #define wxUIAnimationStoryboardCocoa wxUIAnimationStoryboard
    #include "wx/cocoa/uianimationstoryboard.h"
#endif

#endif//_WX_UI_ANIMATION_STORYBOARD_H_BASE_
