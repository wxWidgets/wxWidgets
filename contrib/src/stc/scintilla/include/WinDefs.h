// Scintilla source code edit control
/** @file WinDefs.h
 ** The subset of definitions from Windows needed by Scintilla for GTK+.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef WINDEFS_H
#define WINDEFS_H

/* Running GTK version on win32 */
#if PLAT_GTK_WIN32
#include "Windows.h"
#include "Richedit.h"

/* Name conflicts */
#undef DrawText
#undef FindText
#else

#define WORD short
#define WPARAM unsigned long
#define LPARAM long
#define LRESULT long
#define DWORD long

#define UINT unsigned int
#define LPSTR char *
#define LONG long

//#if 0
/* RTF control */
#define EM_CANPASTE	(1074)
#define EM_CANUNDO	(198)
#define EM_CHARFROMPOS	(215)
#define EM_EMPTYUNDOBUFFER	(205)
#define EM_EXGETSEL	(1076)
#define EM_EXLINEFROMCHAR	(1078)
#define EM_EXSETSEL	(1079)
#define EM_FINDTEXT	(1080)
#define EM_FINDTEXTEX	(1103)
#define EM_FORMATRANGE	(1081)
#define EM_GETFIRSTVISIBLELINE	(206)
#define EM_GETLINE	(196)
#define EM_GETLINECOUNT	(186)
#define EM_GETMARGINS	(212)
#define EM_GETMODIFY	(184)
#define EM_GETRECT	(178)
#define EM_GETSEL	(176)
#define EM_GETSELTEXT	(1086)
#define EM_GETTEXTRANGE	(1099)
#define EM_HIDESELECTION	(1087)
#define EM_LINEFROMCHAR	(201)
#define EM_LINEINDEX	(187)
#define EM_LINELENGTH	(193)
#define EM_LINESCROLL	(182)
#define EM_POSFROMCHAR	(214)
#define EM_REPLACESEL	(194)
#define EM_SCROLLCARET	(183)
#define EM_SELECTIONTYPE	(1090)
#define EM_SETMARGINS	(211)
#define EM_SETREADONLY	(207)
#define EM_SETSEL	(177)
#define EM_UNDO	(199)

#define WM_NULL		(0)
#define WM_CLEAR	(771)
#define WM_COPY	(769)
#define WM_CUT	(768)
#define WM_GETTEXT	(13)
#define WM_GETTEXTLENGTH	(14)
#define WM_PASTE	(770)
#define WM_SETTEXT	(12)
#define WM_UNDO	(772)

#define EN_CHANGE	(768)
#define EN_KILLFOCUS (512)
#define EN_SETFOCUS (256)

#define EC_LEFTMARGIN 1
#define EC_RIGHTMARGIN 2
#define EC_USEFONTINFO 0xffff

//#endif

#if 0
#if PLAT_GTK
#define VK_DOWN GDK_Down
#define VK_UP GDK_Up
#define VK_LEFT GDK_Left
#define VK_RIGHT GDK_Right
#define VK_HOME GDK_Home
#define VK_END GDK_End
#define VK_PRIOR GDK_Page_Up
#define VK_NEXT GDK_Page_Down
#define VK_DELETE GDK_Delete
#define VK_INSERT GDK_Insert
#define VK_ESCAPE GDK_Escape
#define VK_BACK GDK_BackSpace
#define VK_TAB GDK_Tab
#define VK_RETURN GDK_Return
#define VK_ADD GDK_KP_Add
#define VK_SUBTRACT GDK_KP_Subtract
#define VK_DIVIDE GDK_KP_Divide
#endif

#if PLAT_WX
#define VK_DOWN         WXK_DOWN
#define VK_UP           WXK_UP
#define VK_LEFT         WXK_LEFT
#define VK_RIGHT        WXK_RIGHT
#define VK_HOME         WXK_HOME
#define VK_END          WXK_END
#define VK_PRIOR        WXK_PRIOR
#define VK_NEXT         WXK_NEXT
#define VK_DELETE       WXK_DELETE
#define VK_INSERT       WXK_INSERT
#define VK_ESCAPE       WXK_ESCAPE
#define VK_BACK         WXK_BACK
#define VK_TAB          WXK_TAB
#define VK_RETURN       WXK_RETURN
#define VK_ADD          WXK_ADD
#define VK_SUBTRACT     WXK_SUBTRACT
//TODO:
#define VK_DIVIDE	WXK_DIVIDE
#endif

#define SHIFT_PRESSED 1
#define LEFT_CTRL_PRESSED 2
#define LEFT_ALT_PRESSED 4

// Are these needed any more
#define LPSTR char *
#define LONG long
#define LPDWORD (long *)

/* SELCHANGE structure */
#define SEL_EMPTY	(0)
#define SEL_TEXT	(1)
#define SEL_OBJECT	(2)
#define SEL_MULTICHAR	(4)
#define SEL_MULTIOBJECT	(8)

struct RECT {
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
};

/* FINDREPLACE structure */

#define FR_MATCHCASE	(0x4)
#define FR_WHOLEWORD	(0x2)
#define FR_DOWN		(0x1)

#endif

#if 0
struct CHARRANGE {
	LONG cpMin;
	LONG cpMax;
};

struct TEXTRANGE {
	CHARRANGE chrg;
	LPSTR lpstrText;
};

struct FINDTEXTEX {
	CHARRANGE chrg;
	LPSTR lpstrText;
	CHARRANGE chrgText;
};

struct NMHDR {
	WindowID hwndFrom;
	UINT idFrom;
	UINT code;
};

struct FORMATRANGE {
	SurfaceID hdc;
	SurfaceID hdcTarget;
	RECT rc;
	RECT rcPage;
	CHARRANGE chrg;
};
#endif

//#define MAKELONG(a, b) ((a) | ((b) << 16))
//#define LOWORD(x) (x & 0xffff)
//#define HIWORD(x) (x >> 16)

#endif /* !_MSC_VER */

#endif
