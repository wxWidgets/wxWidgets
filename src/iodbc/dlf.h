#ifndef _DLF_H
# define _DLF_H

#define DLDAPI_SVR4_DLFCN

# ifdef DLDAPI_SVR4_DLFCN
#    include    <dlfcn.h>
# else
     extern void FAR*   dlopen(char FAR* path, int mode);
     extern void FAR*   dlsym(void FAR* hdll, char FAR* sym);
     extern char FAR*   dlerror();
     extern int         dlclose(void FAR* hdll);
# endif

# ifndef        RTLD_LAZY
#    define     RTLD_LAZY       1
# endif

# define        DLL_OPEN(dll)           (void*)dlopen((char*)(path), RTLD_LAZY)
# define        DLL_PROC(hdll, sym)     (void*)dlsym((void*)(hdll), (char*)sym)
# define        DLL_ERROR()             (char*)dlerror()
# define        DLL_CLOSE(hdll)         dlclose((void*)(hdll))

#endif
