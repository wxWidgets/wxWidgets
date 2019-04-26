///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/seh.h
// Purpose:     declarations for SEH (structured exceptions handling) support
// Author:      Vadim Zeitlin
// Created:     2006-04-26
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_SEH_H_
#define _WX_MSW_SEH_H_

#if wxUSE_ON_FATAL_EXCEPTION

    // the exception handler which should be called from the exception filter
    //
    // it calls wxApp::OnFatalException() if wxTheApp object exists
    WXDLLIMPEXP_BASE unsigned long wxGlobalSEHandler(EXCEPTION_POINTERS *pExcPtrs);

    // helper macro for wxSEH_HANDLE
#if defined(__BORLANDC__)
    // some compilers don't understand that this code is unreachable and warn
    // about no value being returned from the function without it, so calm them
    // down
    #define wxSEH_DUMMY_RETURN(rc) return rc;
#else
    #define wxSEH_DUMMY_RETURN(rc)
#endif

    // macros which allow to avoid many #if wxUSE_ON_FATAL_EXCEPTION in the code
    // which uses them
    #define wxSEH_TRY __try
    #define wxSEH_IGNORE __except ( EXCEPTION_EXECUTE_HANDLER ) { }
    #define wxSEH_HANDLE(rc)                                                  \
        __except ( wxGlobalSEHandler(GetExceptionInformation()) )             \
        {                                                                     \
            /* use the same exit code as abort() */                           \
            ::ExitProcess(3);                                                 \
                                                                              \
            wxSEH_DUMMY_RETURN(rc)                                            \
        }

#else // wxUSE_ON_FATAL_EXCEPTION
    #define wxSEH_TRY
    #define wxSEH_IGNORE
    #define wxSEH_HANDLE(rc)
#endif // wxUSE_ON_FATAL_EXCEPTION

#if wxUSE_ON_FATAL_EXCEPTION && defined(__VISUALC__)
    #include <eh.h>

    // C++ exception to structured exceptions translator: we need it in order
    // to prevent VC++ from "helpfully" translating structured exceptions (such
    // as division by 0 or access violation) to C++ pseudo-exceptions
    extern void wxSETranslator(unsigned int code, EXCEPTION_POINTERS *ep);

    // up to VC 12 this warning ("calling _set_se_translator() requires /EHa")
    // is harmless and it's easier to suppress it than deal with it as make/
    // project file level as it seems to be harmless
    #if __VISUALC__ < 2000
        #pragma warning(disable: 4535)
    #endif

    // note that the SE translator must be called wxSETranslator!
    #define DisableAutomaticSETranslator() _set_se_translator(wxSETranslator)
#else // !__VISUALC__
    // the other compilers do nothing as stupid by default so nothing to do for
    // them
    #define DisableAutomaticSETranslator()
#endif // __VISUALC__/!__VISUALC__

#endif // _WX_MSW_SEH_H_
