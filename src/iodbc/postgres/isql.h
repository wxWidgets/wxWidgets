#ifndef _INTRINSIC_SQL_H
# define _INTRINSIC_SQL_H

#ifndef OS2
typedef unsigned char           UCHAR;
#endif
typedef long int                SDWORD;
typedef short int               SWORD;
typedef unsigned long int       UDWORD;
typedef unsigned short int      UWORD;

#ifndef FAR
#  define FAR
#endif
#ifndef NEAR                                                                    
#  define NEAR                                                                 
#endif       
#ifndef HANDLE                                                                  
  typedef int                          HANDLE;                                 
  #endif          
#ifndef HGLOBAL                                                                  
  typedef HANDLE                          HGLOBAL;                                 
  #endif          
#ifndef SQL_INDEX_OTHER                                                                  
#define SQL_INDEX_OTHER                 3
  #endif          

#  ifndef BOOL
#    define BOOL                       int
#  endif
#  ifndef CHAR
#    define CHAR                       char
#  endif
#  ifndef FALSE
#    define FALSE                      (0 != 0)
#  endif
#  ifndef HWND
#    define HWND                       int
#  endif
#  ifndef SQLHWND
#    define SQLHWND                       int
#  endif
#  ifndef LONG
#    define LONG                       long
#  endif
#  ifndef PASCAL
#    define PASCAL
#  endif
#  ifndef SHORT
#    define SHORT                      short
#  endif
#  ifndef SQL_API
#    define SQL_API
#  endif
#  ifndef SQL_LOCAL_API
#    define SQL_LOCAL_API
#  endif
#  ifndef TRUE
#    define TRUE                       (0 == 0)
#  endif

typedef struct tagDATE_STRUCT
{
SWORD    year;
UWORD   month;
UWORD   day;
} DATE_STRUCT;

typedef struct tagTIME_STRUCT
{
UWORD   hour;
UWORD   minute;
UWORD   second;
} TIME_STRUCT;

typedef struct tagTIMESTAMP_STRUCT
{
SWORD    year;
UWORD   month;
UWORD   day;
UWORD   hour;
UWORD   minute;
UWORD   second;
UDWORD    fraction;
} TIMESTAMP_STRUCT;



typedef UCHAR FAR*               PTR,
             FAR*               SQLPTR;

typedef void FAR*               HENV,
             FAR*               SQLHENV;
typedef void FAR*               HDBC,
             FAR*               SQLHDBC;
typedef void FAR*               HSTMT,
             FAR*               SQLHSTMT;
typedef SDWORD                  SQLINTEGER;  
typedef signed short            RETCODE;
typedef UCHAR                   SQLCHAR;
typedef UWORD                   SQLUSMALLINT;
typedef PTR              SQLPOINTER;
typedef SWORD                   SQLSMALLINT;
typedef UDWORD                  SQLUINTEGER;

#  ifdef WIN32
#    define SQL_API                     __stdcall
#  else
#    define SQL_API                     /* giovanni EXPORT CALLBACK */
#  endif
#  ifdef OS2
#    ifdef BCPP
#      define _Optlink
#      define _System                  _syscall
#    endif
#    undef  SQL_API
#    define SQL_API                    _System
#  endif

# define ODBCVER                        0x0200

# define SQL_MAX_MESSAGE_LENGTH         512
# define SQL_MAX_DSN_LENGTH             32

/* return code */
# define SQL_INVALID_HANDLE             (-2)
# define SQL_ERROR                      (-1)
# define SQL_SUCCESS                    0
# define SQL_SUCCESS_WITH_INFO          1
# define SQL_NO_DATA_FOUND              100

/* standard SQL datatypes (agree with ANSI type numbering) */
# define SQL_CHAR                       1
# define SQL_NUMERIC                    2
# define SQL_DECIMAL                    3
# define SQL_INTEGER                    4
# define SQL_SMALLINT                   5
# define SQL_FLOAT                      6
# define SQL_REAL                       7
# define SQL_DOUBLE                     8
# define SQL_VARCHAR                    12

# define SQL_TYPE_MIN                   SQL_CHAR
# define SQL_TYPE_NULL                  0
# define SQL_TYPE_MAX                   SQL_VARCHAR

/* C to SQL datatype mapping */
# define SQL_C_CHAR                     SQL_CHAR
# define SQL_C_LONG                     SQL_INTEGER
# define SQL_C_SHORT                    SQL_SMALLINT
# define SQL_C_FLOAT                    SQL_REAL
# define SQL_C_DOUBLE                   SQL_DOUBLE
# define SQL_C_DEFAULT                  99

# define SQL_NO_NULLS                   0
# define SQL_NULLABLE                   1
# define SQL_NULLABLE_UNKNOWN           2

/* some special length values */
# define SQL_NULL_DATA                  (-1)
# define SQL_DATA_AT_EXEC               (-2)
# define SQL_NTS                        (-3)

/* SQLFreeStmt flag values */
# define SQL_CLOSE                      0
# define SQL_DROP                       1
# define SQL_UNBIND                     2
# define SQL_RESET_PARAMS               3

/* SQLTransact flag values */
# define SQL_COMMIT                     0
# define SQL_ROLLBACK                   1

/* SQLColAttributes flag values */
# define SQL_COLUMN_COUNT               0
# define SQL_COLUMN_LABEL               18
# define SQL_COLATT_OPT_MAX             SQL_COLUMN_LABEL
# define SQL_COLUMN_DRIVER_START        1000

# define SQL_COLATT_OPT_MIN             SQL_COLUMN_COUNT

/* Null handles */
# define SQL_NULL_HENV                  0
# define SQL_NULL_HDBC                  0
# define SQL_NULL_HSTMT                 0

#endif

