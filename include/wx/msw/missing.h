/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/missing.h
// Purpose:     Declarations for parts of the Win32 SDK that are missing in
//              the versions that come with some compilers
// Created:     2002/04/23
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Mattia Barbon
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MISSING_H_
#define _WX_MISSING_H_

/*
 * The following are required for VC++ 6.
 */

// Needed by strconv.cpp
#ifndef WC_NO_BEST_FIT_CHARS
    #define WC_NO_BEST_FIT_CHARS 0x400
#endif

#ifndef WM_CONTEXTMENU
    #define WM_CONTEXTMENU      0x007B
#endif

// Needed by toplevel.cpp
#ifndef WM_UPDATEUISTATE
    #define WM_UPDATEUISTATE    0x0128
#endif

#ifndef WM_PRINTCLIENT
    #define WM_PRINTCLIENT 0x318
#endif

// Needed by toplevel.cpp
#ifndef UIS_INITIALIZE
    #define UIS_INITIALIZE  3
#endif

#ifndef UISF_HIDEFOCUS
    #define UISF_HIDEFOCUS  1
#endif

#ifndef UISF_HIDEACCEL
    #define UISF_HIDEACCEL 2
#endif

#ifndef OFN_EXPLORER
    #define OFN_EXPLORER 0x00080000
#endif

#ifndef OFN_ENABLESIZING
    #define OFN_ENABLESIZING 0x00800000
#endif

// Needed by window.cpp
#if wxUSE_MOUSEWHEEL
    #ifndef WM_MOUSEWHEEL
        #define WM_MOUSEWHEEL           0x020A
    #endif
    #ifndef WHEEL_DELTA
        #define WHEEL_DELTA             120
    #endif
    #ifndef SPI_GETWHEELSCROLLLINES
        #define SPI_GETWHEELSCROLLLINES 104
    #endif
#endif // wxUSE_MOUSEWHEEL

// Needed by window.cpp
#ifndef VK_OEM_1
    #define VK_OEM_1        0xBA
    #define VK_OEM_2        0xBF
    #define VK_OEM_3        0xC0
    #define VK_OEM_4        0xDB
    #define VK_OEM_5        0xDC
    #define VK_OEM_6        0xDD
    #define VK_OEM_7        0xDE
#endif

#ifndef VK_OEM_COMMA
    #define VK_OEM_PLUS     0xBB
    #define VK_OEM_COMMA    0xBC
    #define VK_OEM_MINUS    0xBD
    #define VK_OEM_PERIOD   0xBE
#endif

// ----------------------------------------------------------------------------
// ListView common control
// Needed by listctrl.cpp
// ----------------------------------------------------------------------------

#ifndef LVS_EX_LABELTIP
    #define LVS_EX_LABELTIP 0x00004000
#endif

 /*
  * In addition to the above, the following are required for BC++ 5.5.
  * (None presently.)
  */

 /*
  * In addition to the above, the following are required for Digital Mars C++
  */

#ifdef __DMC__

#ifndef CCM_SETUNICODEFORMAT
    #define CCM_SETUNICODEFORMAT 8197
#endif

#ifdef __DMC__
    #ifndef _TrackMouseEvent
        #define _TrackMouseEvent TrackMouseEvent
    #endif
#endif

#ifndef LVM_SETEXTENDEDLISTVIEWSTYLE
    #define LVM_SETEXTENDEDLISTVIEWSTYLE (0x1000 + 54)
#endif

#ifndef LVCF_IMAGE
    #define LVCF_IMAGE             0x0010
#endif

#ifndef Header_GetItemRect
    #define Header_GetItemRect(w,i,r) \
            (BOOL)SendMessage((w),HDM_GETITEMRECT,(WPARAM)(i),(LPARAM)(r))
#endif

#ifndef HDM_GETITEMRECT
    #define HDM_GETITEMRECT (HDM_FIRST+7)
#endif

#ifndef HDN_GETDISPINFOW
    #define HDN_GETDISPINFOW (HDN_FIRST-29)
#endif

#ifndef ListView_GetHeader
    #define ListView_GetHeader(w) (HWND)SendMessage((w),LVM_GETHEADER,0,0)
#endif

#ifndef LVM_GETHEADER
    #define LVM_GETHEADER (LVM_FIRST+31)
#endif

#ifndef LVSICF_NOSCROLL
    #define LVSICF_NOINVALIDATEALL  0x0001
    #define LVSICF_NOSCROLL         0x0002
#endif

// ----------------------------------------------------------------------------
// Toolbar define value missing
// ----------------------------------------------------------------------------
#if !defined(CCS_VERT)
#define CCS_VERT                0x00000080L
#endif

#if !defined(TB_SETDISABLEDIMAGELIST)
    #define TB_SETDISABLEDIMAGELIST (WM_USER + 54)
#endif // !defined(TB_SETDISABLEDIMAGELIST)

// ----------------------------------------------------------------------------
// Tree control
// ----------------------------------------------------------------------------

#ifndef TVIS_FOCUSED
    #define TVIS_FOCUSED            0x0001
#endif

#ifndef TV_FIRST
    #define TV_FIRST                0x1100
#endif

#ifndef TVS_CHECKBOXES
    #define TVS_CHECKBOXES          0x0100
#endif

#ifndef TVS_FULLROWSELECT
    #define TVS_FULLROWSELECT       0x1000
#endif

#ifndef TVM_SETBKCOLOR
    #define TVM_SETBKCOLOR          (TV_FIRST + 29)
    #define TVM_SETTEXTCOLOR        (TV_FIRST + 30)
#endif

#endif
    // DMC++

 /*
  * In addition to the declarations for VC++, the following are required for OpenWatcom C++
  */

#if defined(__WATCOMC__)
#ifndef CFM_BACKCOLOR
    #define CFM_BACKCOLOR 0x04000000
#endif

#if !defined(CCS_VERT)
#define CCS_VERT                0x00000080L
#endif

#if !defined(TB_SETDISABLEDIMAGELIST)
    #define TB_SETDISABLEDIMAGELIST (WM_USER + 54)
#endif // !defined(TB_SETDISABLEDIMAGELIST)

#endif

 /*
  * In addition to the declarations for VC++, the following are required for MinGW
  */

#if defined (__MINGW32__)
#ifndef CFM_BACKCOLOR
    #define CFM_BACKCOLOR 0x04000000
#endif
#endif

 /*
  * In addition to the declarations for VC++, the following are required for WinCE
  */

#ifdef __WXWINCE__
    #include "wx/msw/wince/missing.h"
#endif

#endif
    // _WX_MISSING_H_

