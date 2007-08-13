/* -------------------------------------------------------------------------
 * Project: Mac Notifier Support
 * Name:    macnotfy.h
 * Author:  Stefan CSomor
 * Purpose: Mac Notifier include file
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */
 
#ifndef MAC_NOTIFYERS
#define MAC_NOTIFYERS

#include "wx/dlimpexp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void (*wxMacNotificationProcPtr)(unsigned long event , void* data ) ;

typedef void *wxMacNotifierTableRef ;
WXDLLIMPEXP_BASE void wxMacCreateNotifierTable() ;
WXDLLIMPEXP_BASE void wxMacDestroyNotifierTable() ;
WXDLLIMPEXP_BASE wxMacNotifierTableRef wxMacGetNotifierTable() ;
WXDLLIMPEXP_BASE void wxMacAddEvent( wxMacNotifierTableRef table , wxMacNotificationProcPtr handler , unsigned long event , void* data , short wakeUp ) ;
WXDLLIMPEXP_BASE void wxMacProcessNotifierEvents() ;
WXDLLIMPEXP_BASE void wxMacProcessNotifierAndPendingEvents() ;
WXDLLIMPEXP_BASE void wxMacRemoveAllNotifiersForData( wxMacNotifierTableRef table , void* data ) ;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* MAC_NOTIFYERS */
