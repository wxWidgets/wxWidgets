/** Load driver and resolve driver's function entry point 
      
    Copyright (C) 1995 by Ke Jin <kejin@empress.com> 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
**/

#include	<../iodbc/iodbc.h>

#include	<../iodbc/isql.h>
#include	<../iodbc/isqlext.h>

#include        <../iodbc/dlproc.h>

#include	<../iodbc/herr.h>
#include	<../iodbc/henv.h>
#include	<../iodbc/hdbc.h>

#include	<../iodbc/itrace.h>

#include	"../iodbc/henv.ci"

HPROC	_iodbcdm_getproc( HDBC hdbc, int idx )
{
	DBC_t FAR*	pdbc	= (DBC_t FAR*)hdbc;
	ENV_t FAR*	penv;
/*	HDLL		hdll; */
	HPROC FAR*	phproc;

	if( idx <= 0 || idx > SQL_EXT_API_LAST )
	/* first entry naver used */
	{
		return SQL_NULL_HPROC;
	}
	
	penv = (ENV_t FAR*)(pdbc->henv);

	if( penv == NULL )
	{
		return SQL_NULL_HPROC;
	}

	phproc = penv->dllproc_tab + idx;

	if( *phproc == SQL_NULL_HPROC )
	{
		int	i, en_idx;

		for( i=0 ; ; i++ )
		{
			en_idx = odbcapi_symtab[i].en_idx;
			
			if( en_idx == en_NullProc )
			{
				break;
			}

			if( en_idx == idx )
			{
				*phproc = _iodbcdm_dllproc( penv->hdll, 
					odbcapi_symtab[i].symbol );

				break;
			}
		}
	}

	return *phproc;
}

HDLL	_iodbcdm_dllopen( char FAR* path )
{
	return	(HDLL)DLL_OPEN( path );
}

HPROC	_iodbcdm_dllproc( HDLL hdll, char FAR* sym )
{
	return (HPROC)DLL_PROC( hdll, sym );
}

int	_iodbcdm_dllclose( HDLL hdll )
{
	DLL_CLOSE( hdll );

	return 0;
}

char*	_iodbcdm_dllerror( )
{
	return DLL_ERROR();
}
