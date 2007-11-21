/*
 *  itrace.h
 *
 *  $Id$
 *
 *  Trace functions
 *
 *  The iODBC driver manager.
 *  
 *  Copyright (C) 1995 by Ke Jin <kejin@empress.com> 
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef	_ITRACE_H
#define _ITRACE_H

#ifdef	DEBUG

#ifndef NO_TRACE
#define NO_TRACE
#endif

#endif

#define TRACE_TYPE_APP2DM	1
#define TRACE_TYPE_DM2DRV	2
#define TRACE_TYPE_DRV2DM	3

#define TRACE_TYPE_RETURN	4

extern HPROC _iodbcdm_gettrproc (void FAR * stm, int procid, int type);

#ifdef NO_TRACE
#define TRACE_CALL( stm, trace_on, procid, plist )
#else
#define TRACE_CALL( stm, trace_on, plist )\
	{\
		if( trace_on)\
		{\
			HPROC	hproc;\
\
			hproc = _iodbcdm_gettrproc(stm, procid, TRACE_TYPE_APP2DM);\
\
			if( hproc )\
				hproc plist;\
		}\
	}
#endif

#ifdef NO_TRACE
#define TRACE_DM2DRV( stm, procid, plist )
#else
#define TRACE_DM2DRV( stm, procid, plist )\
	{\
		HPROC	hproc;\
\
		hproc = _iodbcdm_gettrproc(stm, procid, TRACE_TYPE_DM2DRV);\
\
		if( hproc )\
			hproc plist;\
	}
#endif

#ifdef NO_TRACE
#define TRACE_DRV2DM( stm, procid, plist )
#else
#define TRACE_DRV2DM( stm, procid, plist ) \
	{\
		HPROC	hproc;\
\
		hproc = _iodbcdm_gettrproc( stm, procid, TRACE_TYPE_DRV2DM);\
\
		if( hproc )\
				hproc plist;\
	}
#endif

#ifdef NO_TRACE
#define TRACE_RETURN( stm, trace_on, ret )
#else
#define TRACE_RETURN( stm, trace_on, ret )\
	{\
		if( trace_on ) {\
			HPROC hproc;\
\
			hproc = _iodbcdm_gettrproc( stm, 0, TRACE_TYPE_RETURN);\
\
			if( hproc )\
				hproc( stm, ret );\
		}\
	}
#endif

#ifdef	NO_TRACE
#define CALL_DRIVER( hdbc, ret, proc, procid, plist )	{ ret = proc plist; }
#else
#define CALL_DRIVER( hdbc, ret, proc, procid, plist )\
	{\
		DBC_t FAR*	pdbc = (DBC_t FAR*)(hdbc);\
\
		if( pdbc->trace ) {\
			TRACE_DM2DRV( pdbc->tstm, procid, plist )\
			ret = proc plist;\
			TRACE_DRV2DM( pdbc->tstm, procid, plist )\
			TRACE_RETURN( pdbc->tstm, 1, ret )\
		}\
		else\
			ret = proc plist;\
	}
#endif

#endif
