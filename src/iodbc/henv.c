/** Environment object managment functions

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

#include	<../iodbc/dlproc.h>

#include	<../iodbc/herr.h>
#include	<../iodbc/henv.h>

#include	<../iodbc/itrace.h>

RETCODE SQL_API SQLAllocEnv( HENV FAR* phenv )
{
        GENV_t FAR*     genv;

        genv = (GENV_t*)MEM_ALLOC( sizeof(GENV_t) );

        if( genv == NULL )
        {
                *phenv = SQL_NULL_HENV;

                return SQL_ERROR;
        }

#if (ODBCVER >= 0x0300 )
        genv->type = SQL_HANDLE_ENV;
#endif

        genv->henv = SQL_NULL_HENV;     /* driver's env list */
        genv->hdbc = SQL_NULL_HDBC;     /* driver's dbc list */
        genv->herr = SQL_NULL_HERR;     /* err list          */

        *phenv = (HENV)genv;

        return SQL_SUCCESS;
}

RETCODE SQL_API SQLFreeEnv ( HENV henv )
{
        GENV_t FAR*     genv = (GENV_t*)henv;

        if( henv == SQL_NULL_HENV )
        {
                return SQL_INVALID_HANDLE;
        }

        if( genv->hdbc != SQL_NULL_HDBC )
        {
                PUSHSQLERR ( genv->herr, en_S1010 );

                return SQL_ERROR;
        }

        _iodbcdm_freesqlerrlist( genv->herr );

        MEM_FREE( henv );

        return SQL_SUCCESS;
}
