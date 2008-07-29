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

#ifndef wxOSX_USE_CORE_GRAPHICS
    #define wxOSX_USE_CORE_GRAPHICS 1
#endif

/*
 * wxOSX_USE_CORE_GRAPHICS is now implemented in terms of wxUSE_GRAPHICS_CONTEXT
 */
 
#if wxOSX_USE_CORE_GRAPHICS 
	#undef wxUSE_GRAPHICS_CONTEXT
	#define wxUSE_GRAPHICS_CONTEXT 1
#endif

/*
 * check graphics context option
 */
#if wxUSE_GRAPHICS_CONTEXT && !wxOSX_USE_CORE_GRAPHICS
#   error "wxUSE_GRAPHICS_CONTEXT on wxMac requires wxOSX_USE_CORE_GRAPHICS"
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
 
#ifndef wxOSX_USE_COCOA
    #define wxOSX_USE_COCOA 0
#endif

/*
 * setting flags according to the platform
 */

#ifdef __LP64__
    #if wxOSX_USE_COCOA == 0
        #undef wxOSX_USE_COCOA
        #define wxOSX_USE_COCOA 1
    #endif
    #define wxOSX_USE_QUICKDRAW 0
    #define wxOSX_USE_CARBON 0
    #define wxOSX_USE_IPHONE 0
#else
    #ifdef __WXOSX_IPHONE__
        #define wxOSX_USE_QUICKDRAW 0
        #define wxOSX_USE_CARBON 0
        #define wxOSX_USE_IPHONE 1
    #else
        #define wxOSX_USE_IPHONE 0
        #if wxOSX_USE_COCOA
            #define wxOSX_USE_QUICKDRAW 0
            #define wxOSX_USE_CARBON 0
        #else
            #define wxOSX_USE_QUICKDRAW 1
            #define wxOSX_USE_CARBON 1
        #endif
    #endif
#endif

#if wxOSX_USE_COCOA || wxOSX_USE_CARBON
    #define wxOSX_USE_COCOA_OR_CARBON 1
#else
    #define wxOSX_USE_COCOA_OR_CARBON 0
#endif

#if wxOSX_USE_COCOA || wxOSX_USE_IPHONE
    #define wxOSX_USE_COCOA_OR_IPHONE 1
#else
    #define wxOSX_USE_COCOA_OR_IPHONE 0
#endif

/* 
 * text rendering system 
 */

/* we have different options and we turn on all that make sense 
 * under a certain platform
 */

#ifdef __WXOSX_IPHONE__
    #define wxMAC_USE_CG_TEXT 1
    #define wxMAC_USE_CORE_TEXT 0
    #define wxMAC_USE_ATSU_TEXT 0
#else // !__WXOSX_IPHONE__

#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5

    #define wxMAC_USE_CORE_TEXT 1
    #define wxMAC_USE_ATSU_TEXT 0
    #define wxMAC_USE_CG_TEXT 0

#else // platform < 10.5

    #define wxMAC_USE_CG_TEXT 0

    #if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
        #define wxMAC_USE_CORE_TEXT 1
    #else
        #define wxMAC_USE_CORE_TEXT 0
    #endif
    #define wxMAC_USE_ATSU_TEXT 1

#endif

#endif // !__WXOSX_IPHONE__

/*
 * turning off capabilities that don't work under 64 bit yet
 */

#if wxOSX_USE_COCOA_OR_IPHONE

#if wxUSE_MDI
#undef wxUSE_MDI
#define wxUSE_MDI 0
#endif

#if wxUSE_MDI_ARCHITECTURE
#undef wxUSE_MDI_ARCHITECTURE
#define wxUSE_MDI_ARCHITECTURE 0
#endif

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

#if wxUSE_DATAVIEWCTRL
#undef wxUSE_DATAVIEWCTRL
#define wxUSE_DATAVIEWCTRL 0
#endif

#if wxUSE_DRAG_AND_DROP
#undef wxUSE_DRAG_AND_DROP
#define wxUSE_DRAG_AND_DROP 0
#endif

#if wxUSE_TASKBARICON
#undef wxUSE_TASKBARICON
#define wxUSE_TASKBARICON 0
#endif

/*
#if wxUSE_POPUPWIN
#undef wxUSE_POPUPWIN
#define wxUSE_POPUPWIN 0
#endif

#if wxUSE_COMBOBOX
#undef wxUSE_COMBOBOX
#define wxUSE_COMBOBOX 0
#endif


#if wxUSE_MENUS
#undef wxUSE_MENUS
#define wxUSE_MENUS 0
#endif

#if wxUSE_CALENDARCTRL
#undef wxUSE_CALENDARCTRL
#define wxUSE_CALENDARCTRL 0
#endif

#if wxUSE_WXHTML_HELP
#undef wxUSE_WXHTML_HELP
#define wxUSE_WXHTML_HELP 0
#endif

#if wxUSE_DOC_VIEW_ARCHITECTURE
#undef wxUSE_DOC_VIEW_ARCHITECTURE
#define wxUSE_DOC_VIEW_ARCHITECTURE 0
#endif

#if wxUSE_PRINTING_ARCHITECTURE
#undef wxUSE_PRINTING_ARCHITECTURE
#define wxUSE_PRINTING_ARCHITECTURE 0
#endif

*/


#endif // wxOSX_USE_COCOA_OR_IPHON

#if wxOSX_USE_IPHONE

#if wxUSE_CLIPBOARD
#undef wxUSE_CLIPBOARD
#define wxUSE_CLIPBOARD 0
#endif // wxUSE_CLIPBOARD

#endif //wxOSX_USE_IPHONE 

#endif
    /* _WX_MAC_CHKCONF_H_ */

