#pragma once

#if defined(__UNIX__)
  #include <HIToolbox/lists.h>
#else
  #include <Lists.h>
#endif
#include <stdlib.h> 

#define kExtLDEFID 128	// resource id of our LDEF resource

typedef void (*ExtLDEFDrawProcType)(Rect *r, Cell cell, ListHandle lh, long refcon);

enum {
	uppExtLDEFDrawProcInfo = kCStackBased
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Rect *)))
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(Cell)))
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(ListHandle)))
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(long)))
};

#if USESROUTINEDESCRIPTORS
typedef UniversalProcPtr ExtLDEFDrawProcUPP;

#define CallExtLDEFDrawProc(userRoutine, r, cell, lh , refcon )		\
		CallUniversalProc((UniversalProcPtr)(userRoutine), uppExtLDEFDrawProcInfo, r, cell, lh , refcon )
#define NewExtLDEFDrawProc(userRoutine)		\
		(ExtLDEFDrawProcUPP) NewRoutineDescriptor((ProcPtr)(userRoutine), uppExtLDEFDrawProcInfo, GetCurrentISA())

#else
typedef ExtLDEFDrawProcType ExtLDEFDrawProcUPP;

#define CallExtLDEFDrawProc(userRoutine, r, cell, lh , refcon)		\
		(*(userRoutine))(r, cell, lh, refcon)
#define NewExtLDEFDrawProc(userRoutine)		\
		(ExtLDEFDrawProcUPP)(userRoutine)
#endif

typedef struct
{
	long								refCon ;
	ExtLDEFDrawProcUPP 	drawProc ;
} ExtLDEFInfo ;

static void NewExtLDEFInfo( ListHandle lh , ExtLDEFDrawProcType drawproc , long refcon ) ;
static void NewExtLDEFInfo( ListHandle lh , ExtLDEFDrawProcType drawproc , long refcon ) 
{
	ExtLDEFInfo* info = (ExtLDEFInfo* ) malloc( sizeof( ExtLDEFInfo ) ) ;
	info->drawProc = NewExtLDEFDrawProc( drawproc ) ;
	info->refCon = refcon ;
	(**lh).refCon = (long) info ;
}

static void DisposeExtLDEFInfo( ListHandle lh) ;
static void DisposeExtLDEFInfo( ListHandle lh) 
{
	ExtLDEFInfo* info = (ExtLDEFInfo* )  (**lh).refCon ;
	if ( info )
	{
	#if !TARGET_CARBON
		DisposeRoutineDescriptor( (RoutineDescriptor*) info->drawProc ) ;
	#endif
		free( (void*) (**lh).refCon ) ;
	}
}
