// Scintilla source code edit control
// WinDefs.h - the subset of definitions from Windows needed by Scintilla for GTK+
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef WINDEFS_H
#define WINDEFS_H

#define WORD short
#define WPARAM unsigned long
#define LPARAM long
#define LRESULT long
#define DWORD long

#define UINT unsigned int
#define LPSTR char *
#define LONG long

/* RTF control */
#define EM_CANPASTE	(1074)
#define EM_CANUNDO	(198)
#define EM_CHARFROMPOS	(215)
#define EM_DISPLAYBAND	(1075)
#define EM_EMPTYUNDOBUFFER	(205)
#define EM_EXGETSEL	(1076)
#define EM_EXLIMITTEXT	(1077)
#define EM_EXLINEFROMCHAR	(1078)
#define EM_EXSETSEL	(1079)
#define EM_FINDTEXT	(1080)
#define EM_FINDTEXTEX	(1103)
#define EM_FINDWORDBREAK	(1100)
#define EM_FMTLINES	(200)
#define EM_FORMATRANGE	(1081)
#define EM_GETCHARFORMAT	(1082)
#define EM_GETEVENTMASK	(1083)
#define EM_GETFIRSTVISIBLELINE	(206)
#define EM_GETHANDLE	(189)
#define EM_GETLIMITTEXT	(213)
#define EM_GETLINE	(196)
#define EM_GETLINECOUNT	(186)
#define EM_GETMARGINS	(212)
#define EM_GETMODIFY	(184)
#define EM_GETIMECOLOR	(1129)
#define EM_GETIMEOPTIONS	(1131)
#define EM_GETOPTIONS	(1102)
#define EM_GETOLEINTERFACE	(1084)
#define EM_GETPARAFORMAT	(1085)
#define EM_GETPASSWORDCHAR	(210)
#define EM_GETPUNCTUATION	(1125)
#define EM_GETRECT	(178)
#define EM_GETSEL	(176)
#define EM_GETSELTEXT	(1086)
#define EM_GETTEXTRANGE	(1099)
#define EM_GETTHUMB	(190)
#define EM_GETWORDBREAKPROC	(209)
#define EM_GETWORDBREAKPROCEX	(1104)
#define EM_GETWORDWRAPMODE	(1127)
#define EM_HIDESELECTION	(1087)
#define EM_LIMITTEXT	(197)
#define EM_LINEFROMCHAR	(201)
#define EM_LINEINDEX	(187)
#define EM_LINELENGTH	(193)
#define EM_LINESCROLL	(182)
#define EM_PASTESPECIAL	(1088)
#define EM_POSFROMCHAR	(214)
#define EM_REPLACESEL	(194)
#define EM_REQUESTRESIZE	(1089)
#define EM_SCROLL	(181)
#define EM_SCROLLCARET	(183)
#define EM_SELECTIONTYPE	(1090)
#define EM_SETBKGNDCOLOR	(1091)
#define EM_SETCHARFORMAT	(1092)
#define EM_SETEVENTMASK	(1093)
#define EM_SETHANDLE	(188)
#define EM_SETIMECOLOR	(1128)
#define EM_SETIMEOPTIONS	(1130)
#define EM_SETLIMITTEXT	(197)
#define EM_SETMARGINS	(211)
#define EM_SETMODIFY	(185)
#define EM_SETOLECALLBACK	(1094)
#define EM_SETOPTIONS	(1101)
#define EM_SETPARAFORMAT	(1095)
#define EM_SETPASSWORDCHAR	(204)
#define EM_SETPUNCTUATION	(1124)
#define EM_SETREADONLY	(207)
#define EM_SETRECT	(179)
#define EM_SETRECTNP	(180)
#define EM_SETSEL	(177)
#define EM_SETTABSTOPS	(203)
#define EM_SETTARGETDEVICE	(1096)
#define EM_SETWORDBREAKPROC	(208)
#define EM_SETWORDBREAKPROCEX	(1105)
#define EM_SETWORDWRAPMODE	(1126)
#define EM_STREAMIN	(1097)
#define EM_STREAMOUT	(1098)
#define EM_UNDO	(199)

#define WM_NULL		(0)
#define WM_CLEAR	(771)
#define WM_COMMAND	(273)
#define WM_COPY	(769)
#define WM_CUT	(768)
#define WM_GETTEXT	(13)
#define WM_GETTEXTLENGTH	(14)
#define WM_NOTIFY	(78)
#define WM_PASTE	(770)
#define WM_SETTEXT	(12)
#define WM_UNDO	(772)

#define EN_CHANGE	(768)
#define EN_KILLFOCUS (512)
#define EN_SETFOCUS (256)

#define EC_LEFTMARGIN 1
#define EC_RIGHTMARGIN 2
#define EC_USEFONTINFO 0xffff

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

// Are these needed any more
#define LPSTR char *
#define LONG long
#define LPDWORD (long *)
#endif

/* SELCHANGE structure */
#define SEL_EMPTY	(0)
#define SEL_TEXT	(1)
#define SEL_OBJECT	(2)
#define SEL_MULTICHAR	(4)
#define SEL_MULTIOBJECT	(8)

/* FINDREPLACE structure */
#define FR_MATCHCASE	(0x4)
#define FR_WHOLEWORD	(0x2)
#define FR_DOWN		(0x1)

#define SHIFT_PRESSED 1
#define LEFT_CTRL_PRESSED 2
#define LEFT_ALT_PRESSED 4

struct RECT {
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
};

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

#define MAKELONG(a, b) ((a) | ((b) << 16))
#define LOWORD(x) (x & 0xffff)
#define HIWORD(x) (x >> 16)

#endif
