/* -------------------------------------------------------------------------
 * Project: Mac Notifier Support
 * Name:    macnotfy.c
 * Author:  Stefan CSomor
 * Purpose: Mac Notifier main file
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */

#include "wx/wxprec.h"

#include "wx/wx.h"

#include "wx/mac/private.h"

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
static wxMacNotificationEvents gMacNotificationEvents ;

ProcessSerialNumber gAppProcess ;

void wxMacCreateNotifierTable()
{
      GetCurrentProcess(&gAppProcess);
    gMacNotificationEvents.top = 0 ;
    gMacNotificationEvents.bottom = 0 ;
    for ( int i = 0 ; i < kMaxEvents ; ++i )
    {
        gMacNotificationEvents.proc[i] = NULL ;
        gMacNotificationEvents.events[i] = 0 ;
        gMacNotificationEvents.data[i] = NULL ;
    }
}

void wxMacDestroyNotifierTable() 
{
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
    wxASSERT_MSG( handler != NULL , wxT("illegal notification proc ptr") ) ;
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
        if ( e->data[index] == data )
            e->data[index] = NULL ;
        index++ ;
        if ( index == kMaxEvents )
            index = 0 ;
    }
}

void wxMacProcessNotifierEvents()
{
    //  if ( gInProcessing )
    //      return ;
    
    gInProcessing = true ;
    if ( gMacNotificationEvents.top != gMacNotificationEvents.bottom )
    {
        // we only should process the notifiers that were here when we entered it
        // otherwise we might never get out...  
        short count = gMacNotificationEvents.top - gMacNotificationEvents.bottom ;
        if ( count < 0 )
            count += kMaxEvents ;
        
        while ( count-- )
        {
            // consume event at bottom
            short index = gMacNotificationEvents.bottom++ ;
            if ( gMacNotificationEvents.bottom == kMaxEvents )
                gMacNotificationEvents.bottom = 0 ;
            void* data = gMacNotificationEvents.data[index] ;
            unsigned long event = gMacNotificationEvents.events[index] ;
            wxMacNotificationProcPtr handler =  gMacNotificationEvents.proc[index] ;
            
            gMacNotificationEvents.data[index] = NULL ;
            gMacNotificationEvents.events[index] = 0 ;
            gMacNotificationEvents.proc[index]  = NULL ;
            
            if ( handler )
                handler( event , data  ) ;
        }
    }
    gInProcessing = false ;
}

void wxMacProcessNotifierAndPendingEvents() 
{
    wxMacProcessNotifierEvents() ;
    wxTheApp->ProcessPendingEvents() ;
}
