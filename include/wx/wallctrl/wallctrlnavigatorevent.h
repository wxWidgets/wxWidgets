/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrlnavigatorevent.h
// Purpose:     Represents wxWallCtrl navigation event (like selection changed)
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     16/06/2008
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WALLCTRLNAVIGATOREVENT_H
#define WX_WALLCTRLNAVIGATOREVENT_H

#include "wx/event.h"

class wxWallCtrlNavigatorEvent :
	public wxNotifyEvent
{
	DECLARE_DYNAMIC_CLASS(wxWallCtrlNavigatorEvent)
public:
	wxWallCtrlNavigatorEvent( wxEventType commandType = wxEVT_NULL, int id = 0 );
	virtual ~wxWallCtrlNavigatorEvent(void);

	// Sets the selected index
	void SetSelectedIndex (unsigned index);

	// Returns the selected index
	unsigned GetSelectedIndex() const;;


	// TODO: See if we need to implement this
	//virtual wxEvent *Clone() const;
private:
	unsigned m_index;
};

DECLARE_EVENT_TYPE( wxEVT_WALLCTRL_SELECTION_CHANGED, -1 )

typedef void (wxEvtHandler::*wxSelectionChangedEventFunction)(wxWallCtrlNavigatorEvent&);

#define EVT_SELECT(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( wxEVT_WALLCTRL_SELECTION_CHANGED, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( wxSelectionChangedEventFunction, & fn ), (wxObject *) NULL ),


#endif