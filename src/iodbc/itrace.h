#ifndef _ITRACE_H
# define _ITRACE_H

# ifdef DEBUG
#  ifndef NO_TRACE
#   define NO_TRACE
#  endif
# endif

# define TRACE_TYPE_APP2DM      1
# define TRACE_TYPE_DM2DRV      2
# define TRACE_TYPE_DRV2DM      3

# define TRACE_TYPE_RETURN      4

extern HPROC    _iodbcdm_gettrproc(void FAR* stm, int procid, int type);

# ifdef NO_TRACE
#  define TRACE_CALL( stm, trace_on, procid, plist )
# else
#  define TRACE_CALL( stm, trace_on, plist )\
        {\
                if( trace_on)\
                {\
                        HPROC   hproc;\
\
                        hproc = _iodbcdm_gettrproc(stm, procid, TRACE_TYPE_APP2DM);\
\
                        if( hproc )\
                                hproc plist;\
                }\
        }
# endif

# ifdef NO_TRACE
#  define TRACE_DM2DRV( stm, procid, plist )
# else
#  define TRACE_DM2DRV( stm, procid, plist )\
        {\
                HPROC   hproc;\
\
                hproc = _iodbcdm_gettrproc(stm, procid, TRACE_TYPE_DM2DRV);\
\
                if( hproc )\
                        hproc plist;\
        }
# endif

# ifdef NO_TRACE
#  define TRACE_DRV2DM( stm, procid, plist )
# else
#  define TRACE_DRV2DM( stm, procid, plist ) \
        {\
                HPROC   hproc;\
\
                hproc = _iodbcdm_gettrproc( stm, procid, TRACE_TYPE_DRV2DM);\
\
                if( hproc )\
                                hproc plist;\
        }
# endif

# ifdef NO_TRACE
#  define TRACE_RETURN( stm, trace_on, ret )
# else
#  define TRACE_RETURN( stm, trace_on, ret )\
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
# endif

# ifdef NO_TRACE
#  define CALL_DRIVER( hdbc, ret, proc, procid, plist ) { ret = proc plist; }
# else
#  define CALL_DRIVER( hdbc, ret, proc, procid, plist )\
        {\
                DBC_t FAR*      pdbc = (DBC_t FAR*)(hdbc);\
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
# endif

#endif
