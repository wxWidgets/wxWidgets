#ifndef _DLPROC_H
# define        _DLPROC_H

# include       <../iodbc/dlf.h>

# ifdef OS2
  typedef       RETCODE         (FAR* _System HPROC)();
# else
  typedef       RETCODE         (FAR* HPROC)();
# endif

# ifdef DLDAPI_SVR4_DLFCN
#  include      <dlfcn.h>
   typedef void*        HDLL;
# endif

# ifdef DLDAPI_HP_SHL
#  include      <dl.h>
   typedef shl_t        HDLL;
# endif

# ifdef DLDAPI_AIX_LOAD
    typedef void*        HDLL;
# endif

# ifdef DLDAPI_OS2
  typedef HMODULE      HDLL;
# endif

extern  HPROC           _iodbcdm_getproc();
extern  HDLL            _iodbcdm_dllopen(char FAR* dll);
extern  HPROC           _iodbcdm_dllproc(HDLL hdll, char FAR* sym);
extern  char FAR*       _iodbcdm_dllerror();
extern  int             _iodbcdm_dllclose(HDLL hdll);

#define SQL_NULL_HDLL   ((HDLL)NULL)
#define SQL_NULL_HPROC  ((HPROC)NULL)

#endif
