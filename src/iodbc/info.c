/** Information functions
 
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
#include	<../iodbc/hstmt.h>

#include	<../iodbc/itrace.h>

RETCODE SQL_API	SQLDataSources( 
			HENV		henv,
			UWORD		fDir,
			UCHAR  FAR*	szDSN,
			SWORD		cbDSNMax,
			SWORD  FAR*	pcbDSN,
			UCHAR  FAR* 	szDesc,
			SWORD		cbDescMax,
			SWORD  FAR*	pcbDesc )
{
	GENV_t FAR*	genv	= (GENV_t FAR*)henv;
	
	if( henv == SQL_NULL_HENV )
	{
		return SQL_INVALID_HANDLE;
	}

	/* check argument */
	if( cbDSNMax < 0 || cbDescMax < 0 )
	{
		PUSHSQLERR ( genv->herr, en_S1090 );
		
		return SQL_ERROR;
	}

	if( fDir != SQL_FETCH_FIRST
	 && fDir != SQL_FETCH_NEXT )
	{
		PUSHSQLERR ( genv->herr, en_S1103 );

		return SQL_ERROR;
	}

	/*************************/

	return SQL_NO_DATA_FOUND;
} 

RETCODE SQL_API	SQLDrivers(
			HENV		henv,
			UWORD		fDir,
			UCHAR  FAR*	szDrvDesc,
			SWORD		cbDrvDescMax,
			SWORD  FAR*	pcbDrvDesc,
			UCHAR  FAR*	szDrvAttr,
			SWORD		cbDrvAttrMax,
			SWORD  FAR*	pcbDrvAttr )
{
	GENV_t FAR*	genv	= (GENV_t FAR*)henv;

	if( henv == SQL_NULL_HENV )
	{
		return SQL_INVALID_HANDLE;
	}

	if( cbDrvDescMax <  0
	 || cbDrvAttrMax <  0 
	 || cbDrvAttrMax == 1 )
	{
		PUSHSQLERR ( genv->herr, en_S1090 );

		return SQL_ERROR;
	}

	if( fDir != SQL_FETCH_FIRST
	 || fDir != SQL_FETCH_NEXT )
	{
		PUSHSQLERR ( genv->herr, en_S1103 );

		return SQL_ERROR;
	}
	
	/*********************/
	return SQL_SUCCESS;
}


RETCODE SQL_API	SQLGetInfo(
			HDBC		hdbc,
			UWORD		fInfoType,
			PTR		rgbInfoValue,
			SWORD		cbInfoValueMax,
			SWORD FAR*	pcbInfoValue )
{
	DBC_t  FAR*	pdbc	= (DBC_t FAR*)hdbc;
	ENV_t  FAR*	penv;
	STMT_t FAR*	pstmt	= NULL;
	STMT_t FAR*	tpstmt;
	HPROC		hproc;
	RETCODE		retcode	= SQL_SUCCESS;

	DWORD		dword;
	int		size = 0, len = 0;
	char		buf[16] = { '\0' };

	if( hdbc == SQL_NULL_HDBC 
	 || pdbc->henv == SQL_NULL_HENV )
	{
		return SQL_INVALID_HANDLE;
	}

	if( cbInfoValueMax < 0 )
	{
		PUSHSQLERR ( pdbc->herr, en_S1090 );

		return SQL_ERROR;
	}

	if( /* fInfoType < SQL_INFO_FIRST || */
	    ( fInfoType > SQL_INFO_LAST
	   && fInfoType < SQL_INFO_DRIVER_START ) )
	{
		PUSHSQLERR ( pdbc->herr, en_S1096 );

		return SQL_ERROR;
	}

	if( fInfoType == SQL_ODBC_VER )
	{
		sprintf( buf, "%02d.%02d", 
			(ODBCVER)>>8, 0x00FF&(ODBCVER) );

		
		if( rgbInfoValue != NULL 
		 && cbInfoValueMax > 0 )
		{
			len = STRLEN( buf );

			if( len < cbInfoValueMax - 1 )
			{
				len = cbInfoValueMax - 1;
				PUSHSQLERR ( pdbc->herr, en_01004 );

				retcode = SQL_SUCCESS_WITH_INFO;
			}

			STRNCPY( rgbInfoValue, buf, len );
			((char FAR*)rgbInfoValue)[len] = '\0';
		}

		if( pcbInfoValue != NULL )
		{
			*pcbInfoValue = (SWORD)len;
		}

		return retcode;
	}

	if( pdbc->state == en_dbc_allocated 
	 || pdbc->state == en_dbc_needdata )
	{
		PUSHSQLERR ( pdbc->herr, en_08003 );

		return SQL_ERROR;
	}

	switch( fInfoType )
	{
		case SQL_DRIVER_HDBC:
			dword = (DWORD)(pdbc->dhdbc);
			size  = sizeof(dword);
			break;

		case SQL_DRIVER_HENV:
			penv  = (ENV_t FAR*)(pdbc->henv);
			dword = (DWORD)(penv->dhenv);
			size  = sizeof(dword);
			break;

		case SQL_DRIVER_HLIB:
			penv  = (ENV_t FAR*)(pdbc->henv);
			dword = (DWORD)(penv->hdll);
			size  = sizeof(dword);
			break;

		case SQL_DRIVER_HSTMT:
			if( rgbInfoValue != NULL )
			{
				pstmt = *((STMT_t FAR**)rgbInfoValue);
			}

			for( tpstmt  = (STMT_t FAR*)(pdbc->hstmt);
			     tpstmt != NULL;
			     tpstmt  = tpstmt->next )
			{
				if( tpstmt == pstmt )
				{
					break;
				}
			}

			if( tpstmt == NULL )
			{
				PUSHSQLERR ( pdbc->herr, en_S1009 );
	
				return SQL_ERROR;
			} 

			dword = (DWORD)(pstmt->dhstmt);
			size  = sizeof(dword);
			break;

		default:
			break;
	}

	if( size )
	{
		if( rgbInfoValue != NULL )
		{
			*((DWORD*)rgbInfoValue) = dword;
		}

		if( pcbInfoValue != NULL )
		{
			*(pcbInfoValue) = (SWORD)size;
		}

		return SQL_SUCCESS;
	}

	hproc = _iodbcdm_getproc( hdbc, en_GetInfo );

	if( hproc == SQL_NULL_HPROC )
	{
		PUSHSQLERR ( pdbc->herr, en_IM001 );

		return SQL_ERROR;
	}

	CALL_DRIVER ( hdbc, retcode, hproc, en_GetInfo, (
			pdbc->dhdbc, 
			fInfoType,
			rgbInfoValue,
			cbInfoValueMax,
			pcbInfoValue ) )

#if 0
	retcode = hproc(pdbc->dhdbc, 
			fInfoType,
			rgbInfoValue,
			cbInfoValueMax,
			pcbInfoValue );
#endif

	if( retcode == SQL_ERROR 
	 && fInfoType == SQL_DRIVER_ODBC_VER )
	{
		STRCPY( buf, "01.00" );

		if( rgbInfoValue != NULL 
		 && cbInfoValueMax > 0 )
		{
			len = STRLEN( buf );

			if( len < cbInfoValueMax - 1 )
			{
				len = cbInfoValueMax - 1;
				PUSHSQLERR ( pdbc->herr, en_01004 );
			}

			STRNCPY( rgbInfoValue, buf, len );
			((char FAR*)rgbInfoValue)[len] = '\0';
		}

		if( pcbInfoValue != NULL )
		{
			*pcbInfoValue = (SWORD)len;
		}

		/* what should we return in this case ???? */
	}

	return retcode;
}	

RETCODE SQL_API SQLGetFunctions(
			HDBC		hdbc,
			UWORD		fFunc,
			UWORD FAR*	pfExists )
{
	DBC_t FAR*	pdbc	= (DBC_t FAR*)hdbc;
	HPROC		hproc;
	RETCODE		retcode;

	if( hdbc == SQL_NULL_HDBC )
	{
		return SQL_INVALID_HANDLE;
	}

	if( fFunc > SQL_EXT_API_LAST )
	{
		PUSHSQLERR ( pdbc->herr, en_S1095 );

		return SQL_ERROR;
	}

	if( pdbc->state == en_dbc_allocated 
	 || pdbc->state == en_dbc_needdata )
	{
		PUSHSQLERR ( pdbc->herr, en_S1010 );

		return SQL_ERROR;
	}

	if( pfExists == NULL )
	{
		return SQL_SUCCESS;
	}

	hproc = _iodbcdm_getproc( hdbc, en_GetFunctions );

	if( hproc != SQL_NULL_HPROC )
	{
		CALL_DRIVER ( hdbc, retcode, hproc, en_GetFunctions, (
				pdbc->dhdbc, fFunc, pfExists ) )

#if 0
		retcode = hproc( pdbc->dhdbc, fFunc, pfExists );
#endif
		return retcode;
	}

	if( fFunc == SQL_API_SQLSETPARAM )
	{
		fFunc = SQL_API_SQLBINDPARAMETER;
	}

	if( fFunc != SQL_API_ALL_FUNCTIONS )
	{
		hproc = _iodbcdm_getproc( hdbc, fFunc );

		if( hproc == SQL_NULL_HPROC )
		{
			*pfExists = (UWORD)0;
		}
		else
		{
			*pfExists = (UWORD)1;
		}

		return SQL_SUCCESS;
	}

	for( fFunc=0 ; fFunc < 100; fFunc ++ )
	{
		hproc = _iodbcdm_getproc( hdbc, fFunc );

		if( hproc == SQL_NULL_HPROC )
		{
			pfExists[fFunc] = (UWORD)0;
		}
		else
		{
			pfExists[fFunc] = (UWORD)1;
		}
	}

	return SQL_SUCCESS;
}
