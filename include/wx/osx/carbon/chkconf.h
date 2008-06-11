/*
 * Name:        wx/mac/chkconf.h
 * Purpose:     Compiler-specific configuration checking
 * Author:      Julian Smart
 * Modified by:
 * Created:     01/02/97
 * RCS-ID:      $Id$
 * Copyright:   (c) Julian Smart
 * Licence:     wxWindows licence
 */

#ifndef _WX_MAC_CHKCONF_H_
#define _WX_MAC_CHKCONF_H_

/*
 * disable the settings which don't work for some compilers
 */

#if defined(__MWERKS__)
    #undef wxUSE_DEBUG_NEW_ALWAYS
    #define wxUSE_DEBUG_NEW_ALWAYS      0

    /* DS: Fixes compilation when wxUSE_ON_FATAL_EXCEPTION is 1 */
    #ifndef wxTYPE_SA_HANDLER
        #define wxTYPE_SA_HANDLER int
    #endif
#endif

/*
 * use OS X CoreGraphics (1) or QuickDraw (0) for rendering
 */

#ifndef wxMAC_USE_CORE_GRAPHICS
    #define wxMAC_USE_CORE_GRAPHICS 1
#endif

/*
 * wxMAC_USE_CORE_GRAPHICS is now implemented in terms of wxUSE_GRAPHICS_CONTEXT
 */
 
#if wxMAC_USE_CORE_GRAPHICS 
	#undef wxUSE_GRAPHICS_CONTEXT
	#define wxUSE_GRAPHICS_CONTEXT 1
#endif

/*
 * check graphics context option
 */
#if wxUSE_GRAPHICS_CONTEXT && !wxMAC_USE_CORE_GRAPHICS
#   error "wxUSE_GRAPHICS_CONTEXT on wxMac requires wxMAC_USE_CORE_GRAPHICS"
#endif

/*
 * native (1) or emulated (0) toolbar
 */

#ifndef wxMAC_USE_NATIVE_TOOLBAR
    #define wxMAC_USE_NATIVE_TOOLBAR 1
#endif

/*
 * using mixins of cocoa functionality
 */
 
#ifndef wxMAC_USE_COCOA
    #define wxMAC_USE_COCOA 0
#endif

/*
 * setting flags according to the platform
 */

#ifdef __LP64__
    #if wxMAC_USE_COCOA == 0
        #undef wxMAC_USE_COCOA
        #define wxMAC_USE_COCOA 1
    #endif
    #define wxMAC_USE_QUICKDRAW 0
    #define wxMAC_USE_CARBON 0
#else
    #ifdef __WXOSX_IPHONE__
        #define wxMAC_USE_QUICKDRAW 0
        #define wxMAC_USE_CARBON 0
    #else
        #define wxMAC_USE_QUICKDRAW 1
        #define wxMAC_USE_CARBON 1
    #endif
#endif

/* 
 * text rendering system 
 */

/* we have different options and we turn on all that make sense 
 * under a certain platform
 */

#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5

#ifdef __WXOSX_IPHONE__
#define wxMAC_USE_CG_TEXT 1
#define wxMAC_USE_CORE_TEXT 0
#define wxMAC_USE_ATSU_TEXT 0
#else
#define wxMAC_USE_CORE_TEXT 1
#define wxMAC_USE_ATSU_TEXT 0
#define wxMAC_USE_CG_TEXT 0
#endif

#else // platform < 10.5

#define wxMAC_USE_CG_TEXT 0

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
#define wxMAC_USE_CORE_TEXT 1
#else
#define wxMAC_USE_CORE_TEXT 0
#endif
#define wxMAC_USE_ATSU_TEXT 1

#endif

/*
 * turning off capabilities that don't work under 64 bit yet
 */

#ifdef __LP64__

#if wxUSE_DRAG_AND_DROP
#undef wxUSE_DRAG_AND_DROP
#define wxUSE_DRAG_AND_DROP 0
#endif

#if wxUSE_TASKBARICON
#undef wxUSE_TASKBARICON
#define wxUSE_TASKBARICON 0
#endif

#if wxUSE_TOOLTIPS
#undef wxUSE_TOOLTIPS
#define wxUSE_TOOLTIPS 0
#endif


#endif


#endif
    /* _WX_MAC_CHKCONF_H_ */

