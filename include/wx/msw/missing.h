/////////////////////////////////////////////////////////////////////////////
// Name:        missing.h
// Purpose:     Declarations for parts of the Win32 SDK that are missing in
//              the version that comes with some compilers
// Created:     2002/04/23
// RCS-ID:      $Id$
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

#endif

// ----------------------------------------------------------------------------
// Misc stuff
// ----------------------------------------------------------------------------

#ifndef QS_ALLPOSTMESSAGE
    #define QS_ALLPOSTMESSAGE    0x0100
#endif

#ifndef WS_EX_CLIENTEDGE
    #define WS_EX_CLIENTEDGE 0x00000200L
#endif

#ifndef ENDSESSION_LOGOFF
    #define ENDSESSION_LOGOFF    0x80000000
#endif

#endif
    // _WX_MISSING_H_
