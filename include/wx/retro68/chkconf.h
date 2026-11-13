/*
 * Name:        wx/retro68/chkconf.h
 * Purpose:     Classic Mac OS (Retro68) specific config settings checks
 * Created:     2026-11-12
 * Copyright:   (c) 2026 wxWidgets development team
 * Licence:     wxWindows licence
 */

/* THIS IS A C FILE, DON'T USE C++ FEATURES (IN PARTICULAR COMMENTS) IN IT */

#ifndef _WX_RETRO68_CHKCONF_H_
#define _WX_RETRO68_CHKCONF_H_

/*
    Classic Mac OS uses cooperative multitasking only and there are no
    preemptive threads usable by the applications (MP tasks exist on PowerPC
    but can't call any Toolbox functions and are not supported by wx).
 */
#if wxUSE_THREADS
#   undef wxUSE_THREADS
#   define wxUSE_THREADS 0
#endif

/* Networking would require Open Transport/MacTCP support, not done yet. */
#if wxUSE_SOCKETS
#   undef wxUSE_SOCKETS
#   define wxUSE_SOCKETS 0
#endif

#if wxUSE_IPV6
#   undef wxUSE_IPV6
#   define wxUSE_IPV6 0
#endif

#if wxUSE_WEBREQUEST
#   undef wxUSE_WEBREQUEST
#   define wxUSE_WEBREQUEST 0
#endif

/* These features need OS functionality which doesn't exist at all. */
#if wxUSE_DYNLIB_CLASS
#   undef wxUSE_DYNLIB_CLASS
#   define wxUSE_DYNLIB_CLASS 0
#endif

#if wxUSE_DYNAMIC_LOADER
#   undef wxUSE_DYNAMIC_LOADER
#   define wxUSE_DYNAMIC_LOADER 0
#endif

#if wxUSE_FSWATCHER
#   undef wxUSE_FSWATCHER
#   define wxUSE_FSWATCHER 0
#endif

#if wxUSE_SNGLINST_CHECKER
#   undef wxUSE_SNGLINST_CHECKER
#   define wxUSE_SNGLINST_CHECKER 0
#endif

#if wxUSE_STACKWALKER
#   undef wxUSE_STACKWALKER
#   define wxUSE_STACKWALKER 0
#endif

#if wxUSE_ON_FATAL_EXCEPTION
#   undef wxUSE_ON_FATAL_EXCEPTION
#   define wxUSE_ON_FATAL_EXCEPTION 0
#endif

#if wxUSE_CRASHREPORT
#   undef wxUSE_CRASHREPORT
#   define wxUSE_CRASHREPORT 0
#endif

#if wxUSE_DEBUGREPORT
#   undef wxUSE_DEBUGREPORT
#   define wxUSE_DEBUGREPORT 0
#endif

#if wxUSE_MIMETYPE
#   undef wxUSE_MIMETYPE
#   define wxUSE_MIMETYPE 0
#endif

#if wxUSE_SECRETSTORE
#   undef wxUSE_SECRETSTORE
#   define wxUSE_SECRETSTORE 0
#endif

#if wxUSE_CONSOLE_EVENTLOOP
#   undef wxUSE_CONSOLE_EVENTLOOP
#   define wxUSE_CONSOLE_EVENTLOOP 0
#endif

#if wxUSE_XLOCALE
#   undef wxUSE_XLOCALE
#   define wxUSE_XLOCALE 0
#endif

#if wxUSE_FSVOLUME
#   undef wxUSE_FSVOLUME
#   define wxUSE_FSVOLUME 0
#endif

#if wxUSE_DIALUP_MANAGER
#   undef wxUSE_DIALUP_MANAGER
#   define wxUSE_DIALUP_MANAGER 0
#endif

/* Only wchar_t-based Unicode build is supported, there is no iconv. */
#ifdef wxUSE_LIBICONV
#   undef wxUSE_LIBICONV
#endif
#define wxUSE_LIBICONV 0

#if wxUSE_GUI
#   if wxUSE_GRAPHICS_CONTEXT
#       undef wxUSE_GRAPHICS_CONTEXT
#       define wxUSE_GRAPHICS_CONTEXT 0
#   endif

#   if wxUSE_DRAG_AND_DROP
#       undef wxUSE_DRAG_AND_DROP
#       define wxUSE_DRAG_AND_DROP 0
#   endif

    /*
        Generic file control is not used as we have native file dialogs and
        it doesn't support classic Mac OS paths yet.
     */
#   if wxUSE_FILECTRL
#       undef wxUSE_FILECTRL
#       define wxUSE_FILECTRL 0
#   endif

    /* Tooltips are not implemented yet. */
#   if wxUSE_TOOLTIPS
#       undef wxUSE_TOOLTIPS
#       define wxUSE_TOOLTIPS 0
#   endif

    /* Scrap Manager-based clipboard is not implemented yet. */
#   if wxUSE_CLIPBOARD
#       undef wxUSE_CLIPBOARD
#       define wxUSE_CLIPBOARD 0
#   endif

#   if wxUSE_DATAOBJ
#       undef wxUSE_DATAOBJ
#       define wxUSE_DATAOBJ 0
#   endif

#   ifndef __WXUNIVERSAL__
#       error "wxRetro68 GUI port must be built with wxUniversal"
#   endif

#   if wxUSE_PRINTING_ARCHITECTURE
#       undef wxUSE_PRINTING_ARCHITECTURE
#       define wxUSE_PRINTING_ARCHITECTURE 0
#   endif

#   if wxUSE_POSTSCRIPT
#       undef wxUSE_POSTSCRIPT
#       define wxUSE_POSTSCRIPT 0
#   endif

#   if wxUSE_ACCESSIBILITY
#       undef wxUSE_ACCESSIBILITY
#       define wxUSE_ACCESSIBILITY 0
#   endif

#   if wxUSE_JOYSTICK
#       undef wxUSE_JOYSTICK
#       define wxUSE_JOYSTICK 0
#   endif

#   if wxUSE_HOTKEY
#       undef wxUSE_HOTKEY
#       define wxUSE_HOTKEY 0
#   endif

#   if wxUSE_MEDIACTRL
#       undef wxUSE_MEDIACTRL
#       define wxUSE_MEDIACTRL 0
#   endif

#   if wxUSE_WEBVIEW
#       undef wxUSE_WEBVIEW
#       define wxUSE_WEBVIEW 0
#   endif

#   if wxUSE_OPENGL
#       undef wxUSE_OPENGL
#       define wxUSE_OPENGL 0
#   endif

#   if wxUSE_TASKBARICON
#       undef wxUSE_TASKBARICON
#       define wxUSE_TASKBARICON 0
#   endif

#   if wxUSE_NOTIFICATION_MESSAGE
#       undef wxUSE_NOTIFICATION_MESSAGE
#       define wxUSE_NOTIFICATION_MESSAGE 0
#   endif

#   if wxUSE_UIACTIONSIMULATOR
#       undef wxUSE_UIACTIONSIMULATOR
#       define wxUSE_UIACTIONSIMULATOR 0
#   endif

#   if wxUSE_PRIVATE_FONTS
#       undef wxUSE_PRIVATE_FONTS
#       define wxUSE_PRIVATE_FONTS 0
#   endif
#endif /* wxUSE_GUI */

#endif /* _WX_RETRO68_CHKCONF_H_ */
