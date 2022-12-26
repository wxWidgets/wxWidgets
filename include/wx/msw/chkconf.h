/*
 * Name:        wx/msw/chkconf.h
 * Purpose:     Compiler-specific configuration checking
 * Author:      Julian Smart
 * Modified by:
 * Created:     01/02/97
 * Copyright:   (c) Julian Smart
 * Licence:     wxWindows licence
 */

/* THIS IS A C FILE, DON'T USE C++ FEATURES (IN PARTICULAR COMMENTS) IN IT */

#ifndef _WX_MSW_CHKCONF_H_
#define _WX_MSW_CHKCONF_H_

/* ensure that CPU parameter is specified (only nmake .vc makefile) */
#ifdef _MSC_VER
    #if defined(_WIN64) && defined(TARGET_CPU_COMPFLAG) && (TARGET_CPU_COMPFLAG == 0)
        #error CPU must be defined
    #endif
#endif

/* ensure that MSW-specific settings are defined */
#ifndef wxUSE_ACTIVEX
#    ifdef wxABORT_ON_CONFIG_ERROR
#        error "wxUSE_ACTIVEX must be defined."
#    else
#        define wxUSE_ACTIVEX 0
#    endif
#endif /* !defined(wxUSE_ACTIVEX) */

#ifndef wxUSE_WINRT
#    ifdef wxABORT_ON_CONFIG_ERROR
#        error "wxUSE_WINRT must be defined."
#    else
#        define wxUSE_WINRT 0
#    endif
#endif /* !defined(wxUSE_ACTIVEX) */

#ifndef wxUSE_CRASHREPORT
#   ifdef wxABORT_ON_CONFIG_ERROR
#       error "wxUSE_CRASHREPORT must be defined."
#   else
#       define wxUSE_CRASHREPORT 0
#   endif
#endif /* !defined(wxUSE_CRASHREPORT) */

#ifndef wxUSE_DBGHELP
#   ifdef wxABORT_ON_CONFIG_ERROR
#       error "wxUSE_DBGHELP must be defined"
#   else
#       define wxUSE_DBGHELP 1
#   endif
#endif /* wxUSE_DBGHELP */

#ifndef wxUSE_DC_CACHEING
#   ifdef wxABORT_ON_CONFIG_ERROR
#       error "wxUSE_DC_CACHEING must be defined"
#   else
#       define wxUSE_DC_CACHEING 1
#   endif
#endif /* wxUSE_DC_CACHEING */

#ifndef wxUSE_DIALUP_MANAGER
#    ifdef wxABORT_ON_CONFIG_ERROR
#        error "wxUSE_DIALUP_MANAGER must be defined."
#    else
#        define wxUSE_DIALUP_MANAGER 0
#    endif
#endif /* !defined(wxUSE_DIALUP_MANAGER) */

#ifndef wxUSE_MS_HTML_HELP
#    ifdef wxABORT_ON_CONFIG_ERROR
#        error "wxUSE_MS_HTML_HELP must be defined."
#    else
#        define wxUSE_MS_HTML_HELP 0
#    endif
#endif /* !defined(wxUSE_MS_HTML_HELP) */

#ifndef wxUSE_INICONF
#    ifdef wxABORT_ON_CONFIG_ERROR
#        error "wxUSE_INICONF must be defined."
#    else
#        define wxUSE_INICONF 0
#    endif
#endif /* !defined(wxUSE_INICONF) */

#ifndef wxUSE_OLE
#    ifdef wxABORT_ON_CONFIG_ERROR
#        error "wxUSE_OLE must be defined."
#    else
#        define wxUSE_OLE 0
#    endif
#endif /* !defined(wxUSE_OLE) */

#ifndef wxUSE_OLE_AUTOMATION
#    ifdef wxABORT_ON_CONFIG_ERROR
#        error "wxUSE_OLE_AUTOMATION must be defined."
#    else
#        define wxUSE_OLE_AUTOMATION 0
#    endif
#endif /* !defined(wxUSE_OLE_AUTOMATION) */

#ifndef wxUSE_TASKBARICON_BALLOONS
#   ifdef wxABORT_ON_CONFIG_ERROR
#       error "wxUSE_TASKBARICON_BALLOONS must be defined."
#   else
#       define wxUSE_TASKBARICON_BALLOONS 0
#   endif
#endif /* wxUSE_TASKBARICON_BALLOONS */

#ifndef wxUSE_TASKBARBUTTON
#   ifdef wxABORT_ON_CONFIG_ERROR
#       error "wxUSE_TASKBARBUTTON must be defined."
#   else
#       define wxUSE_TASKBARBUTTON 0
#   endif
#endif /* wxUSE_TASKBARBUTTON */

#ifndef wxUSE_UXTHEME
#    ifdef wxABORT_ON_CONFIG_ERROR
#        error "wxUSE_UXTHEME must be defined."
#    else
#        define wxUSE_UXTHEME 0
#    endif
#endif  /* wxUSE_UXTHEME */

#ifndef wxUSE_WINSOCK2
#    ifdef wxABORT_ON_CONFIG_ERROR
#        error "wxUSE_WINSOCK2 must be defined."
#    else
#        define wxUSE_WINSOCK2 0
#    endif
#endif  /* wxUSE_WINSOCK2 */

/*
 * disable the settings which don't work for some compilers
 */

/*
 * All of the settings below require SEH support (__try/__catch) and can't work
 * without it.
 */
#if !defined(_MSC_VER)
#    undef wxUSE_ON_FATAL_EXCEPTION
#    define wxUSE_ON_FATAL_EXCEPTION 0

#    undef wxUSE_CRASHREPORT
#    define wxUSE_CRASHREPORT 0
#endif /* compiler doesn't support SEH */

#if defined(__GNUWIN32__)
    /* These don't work as expected for mingw32 and cygwin32 */
#   undef  wxUSE_MEMORY_TRACING
#   define wxUSE_MEMORY_TRACING            0

#   undef  wxUSE_GLOBAL_MEMORY_OPERATORS
#   define wxUSE_GLOBAL_MEMORY_OPERATORS   0

#   undef  wxUSE_DEBUG_NEW_ALWAYS
#   define wxUSE_DEBUG_NEW_ALWAYS          0

#endif /* __GNUWIN32__ */

/* MinGW32 doesn't provide wincred.h defining the API needed by this */
#ifdef __MINGW32_TOOLCHAIN__
    #undef wxUSE_SECRETSTORE
    #define wxUSE_SECRETSTORE 0
#endif

#if wxUSE_SPINCTRL
#   if !wxUSE_SPINBTN
#       ifdef wxABORT_ON_CONFIG_ERROR
#           error "wxSpinCtrl requires wxSpinButton on MSW"
#       else
#           undef wxUSE_SPINBTN
#           define wxUSE_SPINBTN 1
#       endif
#   endif
#endif

/* wxMSW-specific checks: notice that this file is also used with wxUniv
   and can even be used with wxGTK, when building it under Windows.
 */
#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
#   if !wxUSE_OWNER_DRAWN
#       undef wxUSE_CHECKLISTBOX
#       define wxUSE_CHECKLISTBOX 0
#   endif
#   if !wxUSE_CHECKLISTBOX
#       undef wxUSE_REARRANGECTRL
#       define wxUSE_REARRANGECTRL 0
#   endif
#endif

/* wxMSW implementation requires wxDynamicLibrary. */
#if defined(__WXMSW__) && !wxUSE_DYNLIB_CLASS
#   undef wxUSE_DYNLIB_CLASS
#   define wxUSE_DYNLIB_CLASS 1
#endif  /* !wxUSE_DYNLIB_CLASS */

/*
   un/redefine the options which we can't compile (after checking that they're
   defined
 */
#ifdef __WINE__
#   if wxUSE_ACTIVEX
#       undef wxUSE_ACTIVEX
#       define wxUSE_ACTIVEX 0
#   endif /* wxUSE_ACTIVEX */
#endif /* __WINE__ */

/*
    Currently wxUSE_GRAPHICS_CONTEXT is only enabled with MSVC by default, so
    only check for wxUSE_ACTIVITYINDICATOR dependency on it if it can be
    enabled, otherwise turn the latter off to allow the library to compile.
 */
#if !wxUSE_GRAPHICS_CONTEXT && !defined(_MSC_VER)
#   undef wxUSE_ACTIVITYINDICATOR
#   define wxUSE_ACTIVITYINDICATOR 0
#endif /* !wxUSE_ACTIVITYINDICATOR && !_MSC_VER */

/* MinGW-w64 (32 and 64 bit) has winhttp.h available, legacy MinGW does not. */
#if (!defined(_MSC_VER) && !defined(__MINGW64_VERSION_MAJOR))
    #undef wxUSE_WEBREQUEST_WINHTTP
    #define wxUSE_WEBREQUEST_WINHTTP 0
#endif
/*
    Similarly, turn off wxUSE_WEBREQUEST if we can't enable it because we don't
    have any of its backends to allow the library to compile with the default
    options when using MinGW32 which doesn't come with winhttp.h and so for
    which we have to disable wxUSE_WEBREQUEST_WINHTTP.
 */
#if wxUSE_WEBREQUEST && !wxUSE_WEBREQUEST_CURL && !wxUSE_WEBREQUEST_WINHTTP
#   undef wxUSE_WEBREQUEST
#   define wxUSE_WEBREQUEST 0
#endif /* wxUSE_WEBREQUEST */

/* check settings consistency for MSW-specific ones */
#if wxUSE_CRASHREPORT && !wxUSE_ON_FATAL_EXCEPTION
#   ifdef wxABORT_ON_CONFIG_ERROR
#       error "wxUSE_CRASHREPORT requires wxUSE_ON_FATAL_EXCEPTION"
#   else
#       undef wxUSE_CRASHREPORT
#       define wxUSE_CRASHREPORT 0
#   endif
#endif /* wxUSE_CRASHREPORT */

#if !wxUSE_VARIANT
#   if wxUSE_ACTIVEX
#       ifdef wxABORT_ON_CONFIG_ERROR
#           error "wxActiveXContainer requires wxVariant"
#       else
#           undef wxUSE_ACTIVEX
#           define wxUSE_ACTIVEX 0
#       endif
#   endif

#   if wxUSE_OLE_AUTOMATION
#       ifdef wxABORT_ON_CONFIG_ERROR
#           error "wxAutomationObject requires wxVariant"
#       else
#           undef wxUSE_OLE_AUTOMATION
#           define wxUSE_OLE_AUTOMATION 0
#       endif
#   endif
#endif /* !wxUSE_VARIANT */

#if !wxUSE_DATAOBJ
#   if wxUSE_OLE
#       ifdef wxABORT_ON_CONFIG_ERROR
#           error "wxUSE_OLE requires wxDataObject"
#       else
#           undef wxUSE_OLE
#           define wxUSE_OLE 0
#       endif
#   endif
#endif /* !wxUSE_DATAOBJ */

#if !wxUSE_DYNAMIC_LOADER
#    if wxUSE_MS_HTML_HELP
#        ifdef wxABORT_ON_CONFIG_ERROR
#            error "wxUSE_MS_HTML_HELP requires wxUSE_DYNAMIC_LOADER."
#        else
#            undef wxUSE_MS_HTML_HELP
#            define wxUSE_MS_HTML_HELP 0
#        endif
#    endif
#    if wxUSE_DIALUP_MANAGER
#        ifdef wxABORT_ON_CONFIG_ERROR
#            error "wxUSE_DIALUP_MANAGER requires wxUSE_DYNAMIC_LOADER."
#        else
#            undef wxUSE_DIALUP_MANAGER
#            define wxUSE_DIALUP_MANAGER 0
#        endif
#    endif
#endif  /* !wxUSE_DYNAMIC_LOADER */

#if !wxUSE_OLE
#   if wxUSE_ACTIVEX
#       ifdef wxABORT_ON_CONFIG_ERROR
#           error "wxActiveXContainer requires wxUSE_OLE"
#       else
#           undef wxUSE_ACTIVEX
#           define wxUSE_ACTIVEX 0
#       endif
#   endif

#   if wxUSE_OLE_AUTOMATION
#       ifdef wxABORT_ON_CONFIG_ERROR
#           error "wxAutomationObject requires wxUSE_OLE"
#       else
#           undef wxUSE_OLE_AUTOMATION
#           define wxUSE_OLE_AUTOMATION 0
#       endif
#   endif

#   if wxUSE_DRAG_AND_DROP
#       ifdef wxABORT_ON_CONFIG_ERROR
#           error "wxUSE_DRAG_AND_DROP requires wxUSE_OLE"
#       else
#           undef wxUSE_DRAG_AND_DROP
#           define wxUSE_DRAG_AND_DROP 0
#       endif
#   endif

#   if wxUSE_ACCESSIBILITY
#       ifdef wxABORT_ON_CONFIG_ERROR
#           error "wxUSE_ACCESSIBILITY requires wxUSE_OLE"
#       else
#           undef wxUSE_ACCESSIBILITY
#           define wxUSE_ACCESSIBILITY 0
#       endif
#   endif
#endif /* !wxUSE_OLE */

#if !wxUSE_ACTIVEX
#   if wxUSE_MEDIACTRL
#       ifdef wxABORT_ON_CONFIG_ERROR
#           error "wxMediaCtl requires wxActiveXContainer"
#       else
#           undef wxUSE_MEDIACTRL
#           define wxUSE_MEDIACTRL 0
#       endif
#   endif
#    if wxUSE_WEBVIEW
#       ifdef wxABORT_ON_CONFIG_ERROR
#           error "wxWebView requires wxActiveXContainer under MSW"
#       else
#           undef wxUSE_WEBVIEW
#           define wxUSE_WEBVIEW 0
#       endif
#   endif
#endif /* !wxUSE_ACTIVEX */

#if wxUSE_ACTIVITYINDICATOR && !wxUSE_GRAPHICS_CONTEXT
#   ifdef wxABORT_ON_CONFIG_ERROR
#       error "wxUSE_ACTIVITYINDICATOR requires wxGraphicsContext"
#   else
#       undef wxUSE_ACTIVITYINDICATOR
#       define wxUSE_ACTIVITYINDICATOR 0
#   endif
#endif /* wxUSE_ACTIVITYINDICATOR */

#if wxUSE_STACKWALKER && !wxUSE_DBGHELP
    /*
        Don't give an error in this case because wxUSE_DBGHELP could be 0
        because the compiler just doesn't support it, there is really no other
        choice than to disable wxUSE_STACKWALKER too in this case.

        Unfortunately we can't distinguish between the missing compiler support
        and explicitly disabling wxUSE_DBGHELP (which would ideally result in
        an error if wxUSE_STACKWALKER is not disabled too), but it's better to
        avoid giving a compiler error in the former case even if it means not
        giving it either in the latter one.
     */
    #undef wxUSE_STACKWALKER
    #define wxUSE_STACKWALKER 0
#endif /* wxUSE_STACKWALKER && !wxUSE_DBGHELP */

#if !wxUSE_THREADS
#   if wxUSE_FSWATCHER
#       ifdef wxABORT_ON_CONFIG_ERROR
#           error "wxFileSystemWatcher requires wxThread under MSW"
#       else
#           undef wxUSE_FSWATCHER
#           define wxUSE_FSWATCHER 0
#       endif
#   endif
#   if wxUSE_JOYSTICK
#       ifdef wxABORT_ON_CONFIG_ERROR
#           error "wxJoystick requires wxThread under MSW"
#       else
#           undef wxUSE_JOYSTICK
#           define wxUSE_JOYSTICK 0
#       endif
#   endif
#endif /* !wxUSE_THREADS */


#if !wxUSE_OLE_AUTOMATION
#    if wxUSE_WEBVIEW
#       ifdef wxABORT_ON_CONFIG_ERROR
#           error "wxWebView requires wxUSE_OLE_AUTOMATION under MSW"
#       else
#           undef wxUSE_WEBVIEW
#           define wxUSE_WEBVIEW 0
#       endif
#   endif
#endif /* !wxUSE_OLE_AUTOMATION */

#if defined(__WXUNIVERSAL__) && wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW && !wxUSE_POSTSCRIPT
#   undef wxUSE_POSTSCRIPT
#   define wxUSE_POSTSCRIPT 1
#endif

/*
    IPv6 support requires winsock2.h, but the default of wxUSE_WINSOCK2 is 0.
    Don't require changing it explicitly and just turn it on automatically if
    wxUSE_IPV6 is on.
 */
#if wxUSE_IPV6 && !wxUSE_WINSOCK2
    #undef wxUSE_WINSOCK2
    #define wxUSE_WINSOCK2 1
#endif

#endif /* _WX_MSW_CHKCONF_H_ */
