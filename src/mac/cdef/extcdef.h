#pragma once

#include <stdlib.h>
 
#define kExtCDEFID 128	// resource id of our CDEF resource <Controls.h>
const long kExtCDEFMagic = 0xFEEDFEED ; 
typedef struct ExtCDEFInfo
{
	ControlDefProcPtr		defProc ; // this must always be the lowest element
	long								magic ;
	long								contrlRfCon ;
	ControlHandle				containerControl ;
	Handle							children ;
	ControlFontStyleRec fontStyle ;
	Boolean							hasFocus ;
	SInt16							procID ;
	long								privateData ;
} ExtCDEFInfo ;

void NewExtCDEFInfo( ControlHandle theControl , ControlDefProcPtr defproc , SInt16 procID , long refcon ) ;
void DisposeExtCDEFInfo( ControlHandle theControl) ;
