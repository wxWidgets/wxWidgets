/////////////////////////////////////////////////////////////////////////////
// Name:        missing.h
// Purpose:     Declarations for parts of the Win32 SDK that are missing in
//              the version that comes with some compilers
// Created:     2002/04/23
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Mattia Barbon
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MISSING_H_
#define _WX_MISSING_H_

// ----------------------------------------------------------------------------
// ListView common control
// ----------------------------------------------------------------------------

#ifndef LVHT_ONITEM
    #define LVHT_ONITEM \
                (LVHT_ONITEMICON | LVHT_ONITEMLABEL | LVHT_ONITEMSTATEICON)
#endif

#ifndef LVM_SETEXTENDEDLISTVIEWSTYLE
    #define LVM_SETEXTENDEDLISTVIEWSTYLE (0x1000 + 54)
#endif

#ifndef LVS_EX_FULLROWSELECT
    #define LVS_EX_FULLROWSELECT 0x00000020
#endif

#ifndef LVS_OWNERDATA
    #define LVS_OWNERDATA 0x1000
#endif

#ifndef LVM_FIRST
    #define LVM_FIRST 0x1000
#endif

#ifndef HDM_FIRST
    #define HDM_FIRST 0x1200
#endif

#ifndef LVCFMT_JUSTIFYMASK
    #define LVCFMT_JUSTIFYMASK 0x0003
#endif

#ifndef LVSICF_NOSCROLL
    #define LVSICF_NOSCROLL 0x0002
#endif

#ifndef LVN_ODCACHEHINT
    #define LVN_ODCACHEHINT (-113)
#endif

#ifndef ListView_GetHeader
    #define ListView_GetHeader(w) (HWND)SendMessage((w),LVM_GETHEADER,0,0)
#endif

#ifndef LVM_GETHEADER
    #define LVM_GETHEADER (LVM_FIRST+31)
#endif

#ifndef Header_GetItemRect
    #define Header_GetItemRect(w,i,r) \
            (BOOL)SendMessage((w),HDM_GETITEMRECT,(WPARAM)(i),(LPARAM)(r))
#endif

#ifndef HDM_GETITEMRECT
    #define HDM_GETITEMRECT (HDM_FIRST+7)
#endif

#ifndef LVCF_IMAGE
    #define LVCF_IMAGE             0x0010
#endif

#ifndef LVCFMT_BITMAP_ON_RIGHT
    #define LVCFMT_BITMAP_ON_RIGHT 0x1000
#endif

#ifndef ListView_GetColumnWidth
#define ListView_GetColumnWidth(hwnd, iCol) \
    (int)SNDMSG((hwnd), LVM_GETCOLUMNWIDTH, (WPARAM)(int)(iCol), 0)
#endif

#ifndef ListView_SetColumnWidth
#define ListView_SetColumnWidth(hwnd, iCol, cx) \
    (BOOL)SNDMSG((hwnd), LVM_SETCOLUMNWIDTH, (WPARAM)(int)(iCol), MAKELPARAM((cx), 0))
#endif

#ifndef ListView_GetTextColor
#define ListView_GetTextColor(hwnd)  \
    (COLORREF)SNDMSG((hwnd), LVM_GETTEXTCOLOR, 0, 0L)
#endif

#ifndef ListView_FindItem
#define ListView_FindItem(hwnd, iStart, plvfi) \
    (int)SNDMSG((hwnd), LVM_FINDITEM, (WPARAM)(int)(iStart), (LPARAM)(const LV_FINDINFO FAR*)(plvfi))
#endif

#ifndef HDN_GETDISPINFOW
#define HDN_GETDISPINFOW (HDN_FIRST-29)
#endif

// ----------------------------------------------------------------------------
// Toolbar define value missing 
// ----------------------------------------------------------------------------
#if !defined(CCS_VERT)
#define CCS_VERT                0x00000080L
#endif

// ----------------------------------------------------------------------------
// MS HTML Help
// ----------------------------------------------------------------------------

// instead of including htmlhelp.h, duplicate the things from it we need here:

enum
{
    HH_DISPLAY_TOPIC,
    HH_DISPLAY_TOC,
    HH_DISPLAY_INDEX,
    HH_DISPLAY_SEARCH,
    HH_SET_WIN_TYPE,
    HH_GET_WIN_TYPE,
    HH_GET_WIN_HANDLE,
    HH_ENUM_INFO_TYPE,
    HH_SET_INFO_TYPE,
    HH_SYNC,
    HH_RESERVED1,
    HH_RESERVED2,
    HH_RESERVED3,
    HH_KEYWORD_LOOKUP,
    HH_DISPLAY_TEXT_POPUP,
    HH_HELP_CONTEXT,
    HH_TP_HELP_CONTEXTMENU,
    HH_TP_HELP_WM_HELP,
    HH_CLOSE_ALL,
    HH_ALINK_LOOKUP,
    HH_GET_LAST_ERROR,
    HH_ENUM_CATEGORY,
    HH_ENUM_CATEGORY_IT,
    HH_RESET_IT_FILTER,
    HH_SET_INCLUSIVE_FILTER,
    HH_SET_EXCLUSIVE_FILTER
};

struct HH_POPUP
{
    int       cbStruct;
    HINSTANCE hinst;
    UINT      idString;
    LPCTSTR   pszText;
    POINT     pt;
    COLORREF  clrForeground;
    COLORREF  clrBackground;
    RECT      rcMargins;
    LPCTSTR   pszFont;
};

struct HH_AKLINK
{
    int       cbStruct;
    BOOL      fReserved;
    LPCTSTR   pszKeywords;
    LPCTSTR   pszUrl;
    LPCTSTR   pszMsgText;
    LPCTSTR   pszMsgTitle;
    LPCTSTR   pszWindow;
    BOOL      fIndexOnFail;
};


// ----------------------------------------------------------------------------
// Misc stuff
// ----------------------------------------------------------------------------

#ifndef QS_ALLPOSTMESSAGE
    #define QS_ALLPOSTMESSAGE    0x0100
#endif

#ifdef __WXWINE__
#define LV_ITEMA LVITEMA
#define LV_ITEMW LVITEMW
#define LV_DISPINFOA NMLVDISPINFOA
#define LV_DISPINFOW NMLVDISPINFOW

#if wxUSE_UNICODE
#define LV_FINDINFO LVFINDINFOW
#else
#define LV_FINDINFO LVFINDINFOA
#endif

#endif

#endif
    // _WX_MISSING_H_
