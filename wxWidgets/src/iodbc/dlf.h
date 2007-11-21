/*
 *  dlf.h
 *
 *  $Id$
 *
 *  Dynamic Library Loader (mapping to SVR4)
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
#ifndef	_DLF_H
#define _DLF_H
#include	"config.h"

#if defined(HAVE_LIBDL)
#define DLDAPI_SVR4_DLFCN
#elif defined(HAVE_SHL_LOAD)
#define DLDAPI_HP_SHL
#endif

#ifdef DLDAPI_SVR4_DLFCN
#include	<dlfcn.h>
#elif DLDAPI_AIX_LOAD
#include	<dlfcn.h>
#else
extern void FAR *dlopen (char FAR * path, int mode);
extern void FAR *dlsym (void FAR * hdll, char FAR * sym);
extern char FAR *dlerror ();
extern int dlclose (void FAR * hdll);
#endif

#ifndef	RTLD_LAZY
#define	RTLD_LAZY       1
#endif

#define	DLL_OPEN(dll)		(void*)dlopen((char*)(path), RTLD_LAZY)
#define	DLL_PROC(hdll, sym)	(void*)dlsym((void*)(hdll), (char*)sym)
#define	DLL_ERROR()		(char*)dlerror()
#define	DLL_CLOSE(hdll)		dlclose((void*)(hdll))
#endif
