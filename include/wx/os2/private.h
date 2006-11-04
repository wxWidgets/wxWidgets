/////////////////////////////////////////////////////////////////////////////
// Name:        wx/os2/private.h
// Purpose:     Private declarations: as this header is only included by
//              wxWidgets itself, it may contain identifiers which don't start
//              with "wx".
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OS2_PRIVATE_H_
#define _WX_OS2_PRIVATE_H_

#define INCL_BASE
#define INCL_PM
#define INCL_GPI
#define INCL_WINSYS
#define INCL_SHLERRORS
#define INCL_GPIERRORS
#define INCL_DOS
#define INCL_WINATOM
#define INCL_WIN
#include <os2.h>

#if wxONLY_WATCOM_EARLIER_THAN(1,4)
    inline HATOMTBL APIENTRY WinQuerySystemAtomTable(VOID){return NULL;}
    inline ULONG APIENTRY WinQueryAtomName(HATOMTBL,ATOM,PCSZ,ULONG){return 0;}
    inline LONG APIENTRY GpiPointArc(HPS,PPOINTL){return GPI_ERROR;}
    inline BOOL APIENTRY WinDrawPointer(HPS,LONG,LONG,HPOINTER,ULONG){return FALSE;}
    inline HPOINTER APIENTRY WinCreatePointerIndirect(HWND,PPOINTERINFO){return NULLHANDLE;}
    inline BOOL APIENTRY WinGetMaxPosition(HWND,PSWP){return FALSE;}
    inline BOOL APIENTRY WinGetMinPosition(HWND,PSWP,PPOINTL){return FALSE;}
#endif

#if defined(__WATCOMC__) && defined(__WXMOTIF__)
    #include <os2def.h>
    #define I_NEED_OS2_H
    #include <X11/Xmd.h>

    // include this header from here for many of the GUI related code
    #if wxUSE_GUI
        extern "C" {
            #include <Xm/VendorSP.h>
        }
    #endif

    // provide Unix-like pipe()
    #include <types.h>
    #include <tcpustd.h>
    #include <sys/time.h>
    // Use ::DosCreatePipe or ::DosCreateNPipe under OS/2
    // for more see http://posix2.sourceforge.net/guide.html
    inline int pipe( int WXUNUSED(filedes)[2] )
    {
        wxFAIL_MSG(wxT("Implement first"));
        return -1;
    }
#endif

#if defined (__EMX__) && !defined(USE_OS2_TOOLKIT_HEADERS) && !defined(HAVE_SPBCDATA)

    typedef struct _SPBCDATA {
        ULONG     cbSize;       /*  Size of control block. */
        ULONG     ulTextLimit;  /*  Entryfield text limit. */
        LONG      lLowerLimit;  /*  Spin lower limit (numeric only). */
        LONG      lUpperLimit;  /*  Spin upper limit (numeric only). */
        ULONG     idMasterSpb;  /*  ID of the servant's master spinbutton. */
        PVOID     pHWXCtlData;  /*  Handwriting control data structure flag. */
    } SPBCDATA;

    typedef SPBCDATA *PSPBCDATA;

#endif

#include "wx/dlimpexp.h"
#include "wx/fontenc.h"

class WXDLLEXPORT wxFont;
class WXDLLEXPORT wxWindow;
class WXDLLIMPEXP_BASE wxString;
class WXDLLEXPORT wxBitmap;

// ---------------------------------------------------------------------------
// private constants
// ---------------------------------------------------------------------------

//
// Constant strings for control names and classes
//

//
// Controls
//
WXDLLEXPORT_DATA(extern const wxChar) wxButtonNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxCheckBoxNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxChoiceNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxComboBoxNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxDialogNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxFrameNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxGaugeNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxStaticBoxNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxListBoxNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxStaticTextNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxStaticBitmapNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxPanelNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxRadioBoxNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxRadioButtonNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxBitmapRadioButtonNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxScrollBarNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxSliderNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxTextCtrlNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxToolBarNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxStatusLineNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxGetTextFromUserPromptStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxMessageBoxCaptionStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxFileSelectorPromptStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxFileSelectorDefaultWildcardStr[];
WXDLLEXPORT_DATA(extern const wxChar*) wxInternalErrorStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxFatalErrorStr;
WXDLLEXPORT_DATA(extern const wxChar) wxTreeCtrlNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxDirDialogNameStr[];
WXDLLEXPORT_DATA(extern const wxChar) wxDirDialogDefaultFolderStr[];

//
// Class names
//
WXDLLEXPORT_DATA(extern const wxChar*)  wxFrameClassName;
WXDLLEXPORT_DATA(extern const wxChar*)  wxFrameClassNameNoRedraw;
WXDLLEXPORT_DATA(extern const wxChar*)  wxMDIFrameClassName;
WXDLLEXPORT_DATA(extern const wxChar*)  wxMDIFrameClassNameNoRedraw;
WXDLLEXPORT_DATA(extern const wxChar*)  wxMDIChildFrameClassName;
WXDLLEXPORT_DATA(extern const wxChar*)  wxMDIChildFrameClassNameNoRedraw;
WXDLLEXPORT_DATA(extern const wxChar*)  wxPanelClassName;
WXDLLEXPORT_DATA(extern const wxChar*)  wxPanelClassNameNR;
WXDLLEXPORT_DATA(extern const wxChar*)  wxCanvasClassName;
WXDLLEXPORT_DATA(extern const wxChar*)  wxCanvasClassNameNR;

// ---------------------------------------------------------------------------
// standard icons from the resources
// ---------------------------------------------------------------------------

#ifdef __WXPM__

WXDLLEXPORT_DATA(extern HICON) wxSTD_FRAME_ICON;
WXDLLEXPORT_DATA(extern HICON) wxSTD_MDIPARENTFRAME_ICON;
WXDLLEXPORT_DATA(extern HICON) wxSTD_MDICHILDFRAME_ICON;
WXDLLEXPORT_DATA(extern HICON) wxDEFAULT_FRAME_ICON;
WXDLLEXPORT_DATA(extern HICON) wxDEFAULT_MDIPARENTFRAME_ICON;
WXDLLEXPORT_DATA(extern HICON) wxDEFAULT_MDICHILDFRAME_ICON;
WXDLLEXPORT_DATA(extern HFONT) wxSTATUS_LINE_FONT;

#endif

// ---------------------------------------------------------------------------
// this defines a CASTWNDPROC macro which casts a pointer to the type of a
// window proc for PM.
// MPARAM is a void * but is really a 32-bit value
// ---------------------------------------------------------------------------

typedef MRESULT (APIENTRY * WndProcCast) (HWND, ULONG, MPARAM, MPARAM);
#define CASTWNDPROC (WndProcCast)

/*
 * Decide what window classes we're going to use
 * for this combination of CTl3D/FAFA settings
 */

#define STATIC_CLASS     _T("STATIC")
#define STATIC_FLAGS     (SS_TEXT|DT_LEFT|SS_LEFT|WS_VISIBLE)
#define CHECK_CLASS      _T("BUTTON")
#define CHECK_FLAGS      (BS_AUTOCHECKBOX|WS_TABSTOP)
#define CHECK_IS_FAFA    FALSE
#define RADIO_CLASS      _T("BUTTON" )
#define RADIO_FLAGS      (BS_AUTORADIOBUTTON|WS_VISIBLE)
#define RADIO_SIZE       20
#define RADIO_IS_FAFA    FALSE
#define PURE_WINDOWS
/*  PM has no group box button style
#define GROUP_CLASS      "BUTTON"
#define GROUP_FLAGS      (BS_GROUPBOX|WS_CHILD|WS_VISIBLE)
*/

/*
#define BITCHECK_FLAGS   (FB_BITMAP|FC_BUTTONDRAW|FC_DEFAULT|WS_VISIBLE)
#define BITRADIO_FLAGS   (FC_BUTTONDRAW|FB_BITMAP|FC_RADIO|WS_CHILD|WS_VISIBLE)
*/

// ---------------------------------------------------------------------------
// misc macros
// ---------------------------------------------------------------------------

#define MEANING_CHARACTER '0'
#define DEFAULT_ITEM_WIDTH  200
#define DEFAULT_ITEM_HEIGHT 80

// Scale font to get edit control height
#define EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)    (3*(cy)/2)

#ifdef __WXPM__

// Generic subclass proc, for panel item moving/sizing and intercept
// EDIT control VK_RETURN messages
extern LONG APIENTRY wxSubclassedGenericControlProc(WXHWND hWnd, WXDWORD message, WXWPARAM wParam, WXLPARAM lParam);

#endif

// ---------------------------------------------------------------------------
// constants which might miss from some compilers' headers
// ---------------------------------------------------------------------------

#if !defined(WS_EX_CLIENTEDGE)
    #define WS_EX_CLIENTEDGE 0x00000200L
#endif

#ifndef ENDSESSION_LOGOFF
    #define ENDSESSION_LOGOFF    0x80000000
#endif

#ifndef PMERR_INVALID_PARM
    #define PMERR_INVALID_PARM 0x1303
#endif

#ifndef PMERR_INVALID_PARAMETERS
    #define PMERR_INVALID_PARAMETERS 0x1208
#endif

#ifndef BOOKERR_INVALID_PARAMETERS
    #define BOOKERR_INVALID_PARAMETERS -1
#endif

#ifndef DLGC_ENTRYFIELD
    #define DLGC_ENTRYFIELD  0x0001
#endif

#ifndef DLGC_BUTTON
    #define DLGC_BUTTON      0x0002
#endif

#ifndef DLGC_MLE
    #define DLGC_MLE         0x0400
#endif

#ifndef DP_NORMAL
    #define DP_NORMAL 0
#endif

// ---------------------------------------------------------------------------
// debug messages -- OS/2 has no native debug output system
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// macros to make casting between WXFOO and FOO a bit easier: the GetFoo()
// returns Foo cast to the Windows type for oruselves, while GetFooOf() takes
// an argument which should be a pointer or reference to the object of the
// corresponding class (this depends on the macro)
// ---------------------------------------------------------------------------

#define GetHwnd()               ((HWND)GetHWND())
#define GetHwndOf(win)          ((HWND)((win)->GetHWND()))
// old name
#define GetWinHwnd              GetHwndOf

#define GetHdc()                ((HDC)GetHDC())
#define GetHdcOf(dc)            ((HDC)(dc).GetHDC())

#define GetHbitmap()            ((HBITMAP)GetHBITMAP())
#define GetHbitmapOf(bmp)       ((HBITMAP)(bmp).GetHBITMAP())

#define GetHicon()              ((HICON)GetHICON())
#define GetHiconOf(icon)        ((HICON)(icon).GetHICON())

#define GetHaccel()             ((HACCEL)GetHACCEL())
#define GetHaccelOf(table)      ((HACCEL)((table).GetHACCEL()))

#define GetHmenu()              ((HMENU)GetHMenu())
#define GetHmenuOf(menu)        ((HMENU)menu->GetHMenu())

#define GetHcursor()            ((HCURSOR)GetHCURSOR())
#define GetHcursorOf(cursor)    ((HCURSOR)(cursor).GetHCURSOR())

#define GetHfont()              ((HFONT)GetHFONT())
#define GetHfontOf(font)        ((HFONT)(font).GetHFONT())

// OS/2 convention of the mask is opposed to the wxWidgets one, so we need
// to invert the mask each time we pass one/get one to/from Windows
extern HBITMAP wxInvertMask(HBITMAP hbmpMask, int w = 0, int h = 0);
extern HBITMAP wxCopyBmp(HBITMAP hbmp, bool flip=false, int w=0, int h=0);

// ---------------------------------------------------------------------------
// global data
// ---------------------------------------------------------------------------

#ifdef __WXPM__
// The MakeProcInstance version of the function wxSubclassedGenericControlProc
WXDLLEXPORT_DATA(extern int) wxGenericControlSubClassProc;
WXDLLEXPORT_DATA(extern wxChar*) wxBuffer;
WXDLLEXPORT_DATA(extern HINSTANCE) wxhInstance;
#endif

// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

#ifdef __WXPM__
extern "C"
{
WXDLLEXPORT HINSTANCE wxGetInstance();
}

WXDLLEXPORT void wxSetInstance(HINSTANCE hInst);
#endif

#include "wx/thread.h"
static inline MRESULT MySendMsg(HWND hwnd, ULONG ulMsgid,
                                MPARAM mpParam1, MPARAM mpParam2)
{
    MRESULT vRes;
    vRes = ::WinSendMsg(hwnd, ulMsgid, mpParam1, mpParam2);
#if wxUSE_THREADS
    if (!wxThread::IsMain())
        ::WinPostMsg(hwnd, ulMsgid, mpParam1, mpParam2);
#endif
    return vRes;
}
#define WinSendMsg MySendMsg

#ifdef __WXPM__

WXDLLEXPORT void wxDrawBorder( HPS     hPS
                              ,RECTL&  rRect
                              ,WXDWORD dwStyle
                             );

WXDLLEXPORT wxWindow* wxFindWinFromHandle(WXHWND hWnd);

WXDLLEXPORT void   wxGetCharSize(WXHWND wnd, int *x, int *y,wxFont *the_font);

WXDLLEXPORT void   wxConvertVectorFontSize( FIXED   fxPointSize
                                           ,PFATTRS pFattrs
                                          );
WXDLLEXPORT void   wxFillLogFont( LOGFONT*      pLogFont
                                 ,PFACENAMEDESC pFaceName
                                 ,HPS*          phPS
                                 ,bool*         pbInternalPS
                                 ,long*         pflId
                                 ,wxString&     sFaceName
                                 ,wxFont*       pFont
                                );
WXDLLEXPORT wxFontEncoding wxGetFontEncFromCharSet(int nCharSet);
WXDLLEXPORT void   wxOS2SelectMatchingFontByName( PFATTRS       vFattrs
                                                 ,PFACENAMEDESC pFaceName
                                                 ,PFONTMETRICS  pFM
                                                 ,int           nNumFonts
                                                 ,const wxFont* pFont
                                                );
WXDLLEXPORT wxFont wxCreateFontFromLogFont( LOGFONT*      pLogFont
                                           ,PFONTMETRICS  pFM
                                           ,PFACENAMEDESC pFace
                                          );
WXDLLEXPORT int    wxGpiStrcmp(wxChar* s0, wxChar* s1);

WXDLLEXPORT void wxSliderEvent(WXHWND control, WXWORD wParam, WXWORD pos);
WXDLLEXPORT void wxScrollBarEvent(WXHWND hbar, WXWORD wParam, WXWORD pos);

// Find maximum size of window/rectangle
WXDLLEXPORT extern void wxFindMaxSize(WXHWND hwnd, RECT *rect);

WXDLLEXPORT wxWindow* wxFindControlFromHandle(WXHWND hWnd);
WXDLLEXPORT void wxAddControlHandle(WXHWND hWnd, wxWindow *item);

// Safely get the window text (i.e. without using fixed size buffer)
WXDLLEXPORT extern wxString wxGetWindowText(WXHWND hWnd);

// get the window class name
WXDLLEXPORT extern wxString wxGetWindowClass(WXHWND hWnd);

// get the window id (should be unsigned, hence this is not wxWindowID which
// is, for mainly historical reasons, signed)
WXDLLEXPORT extern WXWORD wxGetWindowId(WXHWND hWnd);

// Convert a PM Error code to a string
WXDLLIMPEXP_BASE extern wxString wxPMErrorToStr(ERRORID vError);

// Does this window style specify any border?
inline bool wxStyleHasBorder(long style)
{
  return (style & (wxSIMPLE_BORDER | wxRAISED_BORDER |
                   wxSUNKEN_BORDER | wxDOUBLE_BORDER)) != 0;
}

inline RECTL wxGetWindowRect(HWND hWnd)
{
    RECTL                           vRect;

    ::WinQueryWindowRect(hWnd, &vRect);
    return vRect;
} // end of wxGetWindowRect

WXDLLEXPORT extern void wxOS2SetFont( HWND          hWnd
                                     ,const wxFont& rFont
                                    );


WXDLLEXPORT extern bool wxCheckWindowWndProc( WXHWND    hWnd
                                             ,WXFARPROC fnWndProc
                                            );
WXDLLEXPORT extern wxBitmap wxDisableBitmap( const wxBitmap& rBmp
                                            ,long            lColor
                                           );

#include "wx/colour.h"

WXDLLEXPORT extern COLORREF wxColourToRGB(const wxColour& rColor);

#endif // __WXPM__

#endif // _WX_OS2_PRIVATE_H_
