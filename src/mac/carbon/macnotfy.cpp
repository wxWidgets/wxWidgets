/* -------------------------------------------------------------------------
 * Project: Mac Notifier Support
 * Name:    macnotfy.c
 * Author:  Stefan CSomor
 * Purpose: Mac Notifier main file
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */

#include "wx/wx.h"
#include "wx/mac/macnotfy.h"

const short kMaxEvents = 1000 ;

struct wxMacNotificationEvents
{
	short top ;
	short bottom ;
	
	wxMacNotificationProcPtr proc[kMaxEvents] ;
	unsigned long events[kMaxEvents] ;
	void* data[kMaxEvents] ;
} ;

typedef struct wxMacNotificationEvents wxMacNotificationEvents ;
wxMacNotificationEvents gMacNotificationEvents ;

ProcessSerialNumber gAppProcess ;

void wxMacWakeUp()
{
	ProcessSerialNumber psn ;
	Boolean isSame ;
	psn.highLongOfPSN = 0 ;
	psn.lowLongOfPSN = kCurrentProcess ;
	SameProcess( &gAppProcess , &psn , &isSame ) ;
	if ( isSame )
	{
		PostEvent( nullEvent , 0 ) ;
	}
	else
	{
		WakeUpProcess( &gAppProcess ) ;
	}
}

void wxMacCreateNotifierTable()
{
  	GetCurrentProcess(&gAppProcess);
	gMacNotificationEvents.top = 0 ;
	gMacNotificationEvents.bottom = 0 ;
	for ( int i = 0 ; i < kMaxEvents ; ++i )
	{
		gMacNotificationEvents.proc[i] = NULL ;
		gMacNotificationEvents.events[i] = NULL ;
		gMacNotificationEvents.data[i] = NULL ;
	}
}

void wxMacDestroyNotifierTable() 
{
	wxASSERT( gMacNotificationEvents.top == gMacNotificationEvents.bottom ) ;
}

wxMacNotifierTableRef wxMacGetNotifierTable() 
{
	return (wxMacNotifierTableRef) &gMacNotificationEvents ;
}

void wxMacAddEvent( 
	wxMacNotifierTableRef table , 
	wxMacNotificationProcPtr handler , 
	unsigned long event , 
	void* data , 
	short wakeUp ) 
{
	wxMacNotificationEvents *e = (wxMacNotificationEvents *) table ;
	/* this should be protected eventually */
	short index = e->top++ ;
	
	if ( e->top == kMaxEvents )
		e->top = 0 ;

	e->proc[index] = handler ;
	e->events[index] = event ;
	e->data[index] = data ;
	if ( wakeUp )
		wxMacWakeUp() ;
}

bool gInProcessing = false ;

void wxMacRemoveAllNotifiersForData( wxMacNotifierTableRef table , void* data ) 
{
	wxMacNotificationEvents *e = (wxMacNotificationEvents *) table ;
	/* this should be protected eventually */
	short index = e->bottom ;
	
  while ( e->top != index )
  {
    if ( index == kMaxEvents )
      index = 0 ;
    if ( e->data[index] == data )
    	e->data[index] = NULL ;
	index++ ;
  }
}

void wxMacProcessNotifierEvents()
{
//  if ( gInProcessing )
//  	return ;
  	
  gInProcessing = true ;
  while ( gMacNotificationEvents.top != gMacNotificationEvents.bottom )
  {
    // consume event at bottom
	short index = gMacNotificationEvents.bottom++ ;
    if ( gMacNotificationEvents.bottom == kMaxEvents )
      gMacNotificationEvents.bottom = 0 ;
    void* data = gMacNotificationEvents.data[index] ;
    unsigned long event = gMacNotificationEvents.events[index] ;
	wxMacNotificationProcPtr handler =  gMacNotificationEvents.proc[index] ;
	
	gMacNotificationEvents.data[index] = NULL ;
	gMacNotificationEvents.events[index] = NULL ;
	gMacNotificationEvents.proc[index]  = NULL ;
    
	handler( event , data  ) ;
  }
  gInProcessing = false ;
}

void wxMacProcessNotifierAndPendingEvents() 
{
	wxMacProcessNotifierEvents() ;
	wxTheApp->ProcessPendingEvents() ;
}
