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
    #define LVSICF_NOINVALIDATEALL  0x0001
    #define LVSICF_NOSCROLL         0x0002
#endif

// mingw32/cygwin don't have declarations for comctl32.dll 4.70+ stuff
#ifndef NM_CACHEHINT
    typedef struct tagNMLVCACHEHINT
    {
        NMHDR   hdr;
        int     iFrom;
        int     iTo;
    } NMLVCACHEHINT;

    #define NM_CACHEHINT NMLVCACHEHINT
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

#if defined(__GNUWIN32__) && !defined(LV_ITEM) \
    && !wxCHECK_W32API_VERSION( 0, 5 )
typedef struct _LVITEMW {
    UINT mask;
    int iItem;
    int iSubItem;
    UINT state;
    UINT stateMask;
    LPWSTR pszText;
    int cchTextMax;
    int iImage;
    LPARAM lParam;
#if (_WIN32_IE >= 0x0300)
    int iIndent;
#endif
} LV_ITEMW;

typedef struct tagLVITEMA
{
    UINT mask;
    int iItem;
    int iSubItem;
    UINT state;
    UINT stateMask;
    LPSTR pszText;
    int cchTextMax;
    int iImage;
    LPARAM lParam;
#if (_WIN32_IE >= 0x0300)
    int iIndent;
#endif
} LV_ITEMA;

#define LV_ITEM LV_ITEMA;
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

#if defined(__GNUWIN32__) && !wxCHECK_W32API_VERSION( 0, 5 )
#ifndef LV_DISPINFOA
typedef struct tagNMLVDISPINFOA {
        NMHDR hdr;
        LV_ITEMA item;
} NMLVDISPINFOA, FAR *LPNMLVDISPINFOA;
#define _LV_DISPINFOA tagNMLVDISPINFOA
#define LV_DISPINFOA NMLVDISPINFOA
#endif
#ifndef LV_DISPINFOW
typedef struct tagNMLVDISPINFOW {
        NMHDR hdr;
        LV_ITEMW item;
} NMLVDISPINFOW, FAR *LPNMLVDISPINFOW;
#define _LV_DISPINFOW tagNMLVDISPINFOW
#define LV_DISPINFOW NMLVDISPINFOW
#endif
#endif

#if ((defined(__WATCOMC__) && __WATCOMC__ >= 1200) || defined(__GNUWIN32__) || defined (__MINGW32__) || defined(__DIGITALMARS__) || defined (__BORLANDC__)) && !defined(HDN_GETDISPINFOW)
#define HDN_GETDISPINFOW (HDN_FIRST-29)
#if !wxCHECK_W32API_VERSION(2, 2)
typedef struct {
        NMHDR hdr;
        int iItem;
        UINT mask;
        LPWSTR pszText;
        int cchTextMax;
        int iImage;
        LPARAM lParam;
} NMHDDISPINFOW, *LPNMHDDISPINFOW;
#endif
#endif

#ifndef LVM_SETUNICODEFORMAT
#define LVM_SETUNICODEFORMAT 0x2005
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
// SHGetFileInfo-related things
// ----------------------------------------------------------------------------

#ifndef SHGetFileInfo
    #ifdef UNICODE
        #define SHGetFileInfo SHGetFileInfoW
    #else
        #define SHGetFileInfo SHGetFileInfoA
    #endif
#endif

#ifndef SHGFI_ATTRIBUTES
    #define SHGFI_ATTRIBUTES 2048
#endif

#ifndef SFGAO_READONLY
    #define SFGAO_READONLY 0x00040000L
#endif

#ifndef SFGAO_REMOVABLE
    #define SFGAO_REMOVABLE 0x02000000L
#endif

#ifndef SHGFI_DISPLAYNAME
    #define SHGFI_DISPLAYNAME 512
#endif

#ifndef SHGFI_ICON
    #define SHGFI_ICON 256
#endif

#ifndef SHGFI_SMALLICON
     #define SHGFI_SMALLICON 1
#endif

#ifndef SHGFI_SHELLICONSIZE
    #define SHGFI_SHELLICONSIZE 4
#endif

#ifndef SHGFI_OPENICON
    #define SHGFI_OPENICON 2
#endif

// ----------------------------------------------------------------------------
// Rich text control
// ----------------------------------------------------------------------------

#if wxUSE_RICHEDIT && defined(MAX_TAB_STOPS)

// old mingw32 doesn't define this
#ifndef CFM_CHARSET
    #define CFM_CHARSET 0x08000000
#endif // CFM_CHARSET

#ifndef CFM_BACKCOLOR
    #define CFM_BACKCOLOR 0x04000000
#endif

// cygwin does not have these defined for richedit
#ifndef ENM_LINK
    #define ENM_LINK 0x04000000
#endif

#ifndef EM_AUTOURLDETECT
    #define EM_AUTOURLDETECT (WM_USER + 91)
#endif

#ifndef EN_LINK
    #define EN_LINK 0x070b

    typedef struct _enlink
    {
        NMHDR nmhdr;
        UINT msg;
        WPARAM wParam;
        LPARAM lParam;
        CHARRANGE chrg;
    } ENLINK;
#endif // ENLINK

#ifndef SF_UNICODE
    #define SF_UNICODE 0x0010
#endif

// Watcom C++ doesn't define this
#ifndef SCF_ALL
    #define SCF_ALL 0x0004
#endif

#ifndef PFA_JUSTIFY
#define PFA_JUSTIFY 4

typedef struct _paraformat2 {
    UINT cbSize;
    DWORD dwMask;
    WORD wNumbering;
    WORD wEffects;
    LONG dxStartIndent;
    LONG dxRightIndent;
    LONG dxOffset;
    WORD wAlignment;
    SHORT cTabCount;
    LONG rgxTabs[MAX_TAB_STOPS];
    LONG dySpaceBefore;
    LONG dySpaceAfter;
    LONG dyLineSpacing;
    SHORT sStype;
    BYTE bLineSpacingRule;
    BYTE bOutlineLevel;
    WORD wShadingWeight;
    WORD wShadingStyle;
    WORD wNumberingStart;
    WORD wNumberingStyle;
    WORD wNumberingTab;
    WORD wBorderSpace;
    WORD wBorderWidth;
    WORD wBorders;
} PARAFORMAT2;
#define wxEffects wReserved

#endif

#endif // wxUSE_RICHEDIT

// ----------------------------------------------------------------------------
// ToolBar
// ----------------------------------------------------------------------------

#if wxUSE_TOOLBAR

#if !defined(TBIF_SIZE)

#define TBIF_SIZE 64
#define TB_SETBUTTONINFO (WM_USER+66)

typedef struct {
    UINT cbSize;
    DWORD dwMask;
    int idCommand;
    int iImage;
    BYTE fsState;
    BYTE fsStyle;
    WORD cx;
    DWORD lParam;
    LPTSTR pszText;
    int cchText;
} TBBUTTONINFO, *LPTBBUTTONINFO;

#endif // !defined(TBIF_SIZE)

#endif // wxUSE_TOOLBAR

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

#ifndef TVS_INFOTIP
    #define TVS_INFOTIP 2048
#endif

#ifndef TVN_GETINFOTIPA
    #define TVN_GETINFOTIPA    (TVN_FIRST-13)
    #define TVN_GETINFOTIPW    (TVN_FIRST-14)
#endif

#ifndef TVN_GETINFOTIP
    #ifdef UNICODE
        #define TVN_GETINFOTIP TVN_GETINFOTIPW
    #else
        #define TVN_GETINFOTIP TVN_GETINFOTIPA
    #endif
#endif

#if !defined(NMTVGETINFOTIP) && defined(TVN_FIRST)
    // NB: Check for TVN_FIRST is done so that this code is not included if
    //     <commctrl.h> (which defined HTREEITEM) wasn't included before.
    struct NMTVGETINFOTIPA
    {
        NMHDR     hdr;
        LPSTR     pszText;
        int       cchTextMax;
        HTREEITEM hItem;
        LPARAM    lParam;
    };
    struct NMTVGETINFOTIPW
    {
        NMHDR     hdr;
        LPWSTR     pszText;
        int       cchTextMax;
        HTREEITEM hItem;
        LPARAM    lParam;
    };
    #ifdef UNICODE
        #define NMTVGETINFOTIP NMTVGETINFOTIPW
    #else
        #define NMTVGETINFOTIP NMTVGETINFOTIPA
    #endif
#endif

// ----------------------------------------------------------------------------
// Misc stuff
// ----------------------------------------------------------------------------

#ifndef CCM_SETUNICODEFORMAT
    #define CCM_SETUNICODEFORMAT 8197
#endif

#ifndef QS_ALLPOSTMESSAGE
    #define QS_ALLPOSTMESSAGE    0x0100
#endif

#ifndef WS_EX_CLIENTEDGE
    #define WS_EX_CLIENTEDGE 0x00000200L
#endif

#ifndef ENDSESSION_LOGOFF
    #define ENDSESSION_LOGOFF    0x80000000
#endif

#ifndef HANGUL_CHARSET
    #define HANGUL_CHARSET  129
#endif

#ifndef TME_HOVER
    #define TME_HOVER     1
#endif

#ifndef TME_LEAVE
    #define TME_LEAVE     2
#endif

#ifndef TME_QUERY
    #define TME_QUERY     0x40000000
#endif

#ifndef TME_CANCEL
    #define TME_CANCEL    0x80000000
#endif

#ifndef HOVER_DEFAULT
    #define HOVER_DEFAULT 0xFFFFFFFF
#endif

#ifdef __DMC__

    #ifndef _TrackMouseEvent
        #define _TrackMouseEvent TrackMouseEvent
    #endif

#endif

// This didn't appear in mingw until 2.95.2
#ifndef SIF_TRACKPOS
#define SIF_TRACKPOS 16
#endif

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

#ifndef WM_UPDATEUISTATE
    #define WM_UPDATEUISTATE    0x128
#endif

#ifndef UIS_INITIALIZE
    #define UIS_INITIALIZE  3
#endif

#ifndef UISF_HIDEFOCUS
    #define UISF_HIDEFOCUS  1
#endif

#ifndef UISF_HIDEACCEL
    #define UISF_HIDEACCEL 2
#endif

#ifndef WC_NO_BEST_FIT_CHARS
    #define WC_NO_BEST_FIT_CHARS 0x400
#endif

#ifndef OFN_EXPLORER
    #define OFN_EXPLORER 0x00080000
#endif

#ifndef OFN_ENABLESIZING
    #define OFN_ENABLESIZING 0x00800000
#endif

// ------------------ For Flashing Window -------------
#if (defined(__BORLANDC__) && (__BORLANDC__ < 550))
typedef struct {
    UINT  cbSize;
    HWND  hwnd;
    DWORD dwFlags;
    UINT  uCount;
    DWORD dwTimeout;
} FLASHWINFO, *PFLASHWINFO;
#endif

// In addition, include stuff not defined in WinCE
#ifdef __WXWINCE__
    #include "wx/msw/wince/missing.h"
#endif

#endif
    // _WX_MISSING_H_
