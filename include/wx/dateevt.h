///////////////////////////////////////////////////////////////////////////////
// Name:        wx/dateevt.h
// Purpose:     declares wxDateEvent class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2005-01-10
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DATEEVT_H_
#define _WX_DATEEVT_H_

#include "wx/event.h"
#include "wx/datetime.h"
#include "wx/window.h"

// ----------------------------------------------------------------------------
// wxDateEvent: used by wxCalendarCtrl and wxDatePickerCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDateEvent : public wxCommandEvent
{
public:
    wxDateEvent() { }
    wxDateEvent(wxWindow *win, const wxDateTime& dt, wxEventType type)
        : wxCommandEvent(type, win->GetId()),
          m_date(dt)
    {
        SetEventObject(win);
    }

    const wxDateTime& GetDate() const { return m_date; }
    void SetDate(const wxDateTime &date) { m_date = date; }

private:
    wxDateTime m_date;

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDateEvent)
};

// ----------------------------------------------------------------------------
// event types and macros for handling them
// ----------------------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_DATE_CHANGED, 1101)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxDateEventFunction)(wxDateEvent&);

#define EVT_DATE_CHANGED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_DATE_CHANGED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)  wxStaticCastEvent( wxDateEventFunction, & fn ), (wxObject *) NULL),

#ifdef _WX_DEFINE_DATE_EVENTS_
    DEFINE_EVENT_TYPE(wxEVT_DATE_CHANGED)

    IMPLEMENT_DYNAMIC_CLASS(wxDateEvent, wxCommandEvent)
#endif

#endif // _WX_DATEEVT_H_

