/** trace functions
       
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

#include	<../iodbc/itrace.h>

#include	<../iodbc/herr.h>
#include	<../iodbc/henv.h>
#include	"../iodbc/henv.ci"

#include	<stdio.h>

static  int printreturn(void FAR* istm, int ret )
{
	FILE FAR*	stm = (FILE FAR*)istm;
	char FAR*	ptr = "Invalid return value";

	switch( ret )
	{
		case SQL_SUCCESS:
			ptr = "SQL_SUCCESS";
			break;
		
		case SQL_SUCCESS_WITH_INFO:
			ptr = "SQL_SUCCESS_WITH_INFO";
			break;

		case SQL_NO_DATA_FOUND:
			ptr = "SQL_NO_DATA_FOUND";
			break;

		case SQL_NEED_DATA:
			ptr = "SQL_NEED_DATA";
			break;

		case SQL_INVALID_HANDLE:
			ptr = "SQL_INVALID_HANDLE";
			break;

		case SQL_ERROR:
			ptr = "SQL_ERROR";
			break;

		case SQL_STILL_EXECUTING:
			ptr = "SQL_STILL_EXECUTING";
			break;

		default:
			break;
	}

	fprintf( stm, "%s\n", ptr);
	fflush( stm );

	return 0;
}

HPROC	_iodbcdm_gettrproc(void FAR* istm, int procid, int type)
{
	FILE FAR*	stm = (FILE FAR*)istm;

	if( type == TRACE_TYPE_DM2DRV )
	{
		int i,j = 0;

		for(i=0;j != en_NullProc ;i++)
		{
			j = odbcapi_symtab[i].en_idx;

			if( j == procid )
			{
				fprintf( stm, "\n%s ( ... )\n",
					odbcapi_symtab[i].symbol);

				fflush( stm );
			}
		}
	}

	if( type == TRACE_TYPE_RETURN )
	{
		return (HPROC)printreturn;
	}

	return SQL_NULL_HPROC;
}
