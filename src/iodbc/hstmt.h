#ifndef _HSTMT_H
#define _HSTMT_H

#include        <config.h>

#include        <isql.h>
#include        <isqlext.h>

typedef struct STMT
{
        int     type;           /* must be 1st field */

        struct STMT*  next;

        HERR    herr;
        HDBC    hdbc;           /* back point to connection object */
        HSTMT   dhstmt;         /* driver's stmt handle */

        int     state;
        int     cursor_state;
        int     prep_state;
        int     asyn_on;        /* async executing which odbc call */
        int     need_on;        /* which call return SQL_NEED_DATA */
} STMT_t;

enum    {
        en_stmt_allocated = 0,
        en_stmt_prepared,
        en_stmt_executed,
        en_stmt_cursoropen,
        en_stmt_fetched,
        en_stmt_xfetched,
        en_stmt_needdata,       /* not call SQLParamData() yet */
        en_stmt_mustput,        /* not call SQLPutData() yet */
        en_stmt_canput  /* SQLPutData() called */
};      /* for statement handle state */

enum    {
        en_stmt_cursor_no = 0,
        en_stmt_cursor_named,
        en_stmt_cursor_opened,
        en_stmt_cursor_fetched,
        en_stmt_cursor_xfetched
};      /* for statement cursor state */

extern  RETCODE _iodbcdm_dropstmt();

#endif
