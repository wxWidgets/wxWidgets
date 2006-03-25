#include "extcdef.h"  

/*************************************************************************

	main

*************************************************************************/

pascal SInt32 main(SInt16 varCode , ControlHandle theControl , ControlDefProcMessage message, SInt32 param )
{
	long	returnValue = 0L;
	char	state = HGetState((Handle)theControl);
	
	if ( message != initCntl )
	{
		ExtCDEFInfo* info = (ExtCDEFInfo* )  (**theControl).contrlRfCon ;
		if ( info )
		{
			returnValue =	info->defProc( info->procID, theControl, message, param) ;
		}
	}
	else
	{
		 (**theControl).contrlRfCon = NULL ;
	}
	HSetState((Handle)theControl,state);

	return(returnValue);				/* tell them what happened */
}
