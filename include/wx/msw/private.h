/////////////////////////////////////////////////////////////////////////////
// Name:        private.h
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

#ifndef _WX_PRIVATE_H_
#define _WX_PRIVATE_H_

#include "wx/msw/wrapwin.h"

#ifdef __WXMICROWIN__
    // Extra prototypes and symbols not defined by MicroWindows
    #include "wx/msw/microwin.h"
#endif

// Include fixes for MSLU:
#include "wx/msw/mslu.h"

#include "wx/log.h"

class WXDLLEXPORT wxFont;
class WXDLLEXPORT wxWindow;

// ---------------------------------------------------------------------------
// private constants
// ---------------------------------------------------------------------------

// 260 was taken from windef.h
#ifndef MAX_PATH
    #define MAX_PATH  260
#endif

// ---------------------------------------------------------------------------
// standard icons from the resources
// ---------------------------------------------------------------------------

#if wxUSE_GUI

extern WXDLLEXPORT_DATA(HICON) wxSTD_FRAME_ICON;
extern WXDLLEXPORT_DATA(HICON) wxSTD_MDIPARENTFRAME_ICON;
extern WXDLLEXPORT_DATA(HICON) wxSTD_MDICHILDFRAME_ICON;
extern WXDLLEXPORT_DATA(HICON) wxDEFAULT_FRAME_ICON;
extern WXDLLEXPORT_DATA(HICON) wxDEFAULT_MDIPARENTFRAME_ICON;
extern WXDLLEXPORT_DATA(HICON) wxDEFAULT_MDICHILDFRAME_ICON;
extern WXDLLEXPORT_DATA(HFONT) wxSTATUS_LINE_FONT;

#endif // wxUSE_GUI

// ---------------------------------------------------------------------------
// define things missing from some compilers' headers
// ---------------------------------------------------------------------------

#if defined(__WXWINCE__) || (defined(__GNUWIN32__) && !wxUSE_NORLANDER_HEADERS)
#ifndef ZeroMemory
    inline void ZeroMemory(void *buf, size_t len) { memset(buf, 0, len); }
#endif
#endif // old mingw32

// this defines a CASTWNDPROC macro which casts a pointer to the type of a
// window proc
#if defined(STRICT) || defined(__GNUC__)
    typedef WNDPROC WndProcCast;
#else
    typedef FARPROC WndProcCast;
#endif


#define CASTWNDPROC (WndProcCast)



// ---------------------------------------------------------------------------
// some stuff for old Windows versions (FIXME: what does it do here??)
// ---------------------------------------------------------------------------

#if !defined(APIENTRY)  // NT defines APIENTRY, 3.x not
    #define APIENTRY FAR PASCAL
#endif

#ifdef __WIN32__
    #define _EXPORT
#else
    #define _EXPORT _export
#endif

#ifndef __WIN32__
    typedef signed short int SHORT;
#endif

#if !defined(__WIN32__)  // 3.x uses FARPROC for dialogs
#ifndef STRICT
    #define DLGPROC FARPROC
#endif
#endif

#if wxUSE_PENWIN
    WXDLLEXPORT void wxRegisterPenWin();
    WXDLLEXPORT void wxCleanUpPenWin();
    WXDLLEXPORT void wxEnablePenAppHooks (bool hook);
#endif // wxUSE_PENWIN

#if wxUSE_ITSY_BITSY
    #define IBS_HORZCAPTION    0x4000L
    #define IBS_VERTCAPTION    0x8000L

    UINT    WINAPI ibGetCaptionSize( HWND hWnd  ) ;
    UINT    WINAPI ibSetCaptionSize( HWND hWnd, UINT nSize ) ;
    LRESULT WINAPI ibDefWindowProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;
    VOID    WINAPI ibAdjustWindowRect( HWND hWnd, LPRECT lprc ) ;
#endif // wxUSE_ITSY_BITSY

/*
 * Decide what window classes we're going to use
 * for this combination of CTl3D/FAFA settings
 */

#define STATIC_CLASS     wxT("STATIC")
#define STATIC_FLAGS     (SS_LEFT|WS_CHILD|WS_VISIBLE)
#define CHECK_CLASS      wxT("BUTTON")
#define CHECK_FLAGS      (BS_AUTOCHECKBOX|WS_TABSTOP|WS_CHILD)
#define CHECK_IS_FAFA    FALSE
#define RADIO_CLASS      wxT("BUTTON")
#define RADIO_FLAGS      (BS_AUTORADIOBUTTON|WS_CHILD|WS_VISIBLE)
#define RADIO_SIZE       20
#define RADIO_IS_FAFA    FALSE
#define PURE_WINDOWS
#define GROUP_CLASS      wxT("BUTTON")
#define GROUP_FLAGS      (BS_GROUPBOX|WS_CHILD|WS_VISIBLE)

/*
#define BITCHECK_FLAGS   (FB_BITMAP|FC_BUTTONDRAW|FC_DEFAULT|WS_VISIBLE)
#define BITRADIO_FLAGS   (FC_BUTTONDRAW|FB_BITMAP|FC_RADIO|WS_CHILD|WS_VISIBLE)
*/

// ---------------------------------------------------------------------------
// misc macros
// ---------------------------------------------------------------------------

#define MEANING_CHARACTER '0'
#define DEFAULT_ITEM_WIDTH  100
#define DEFAULT_ITEM_HEIGHT 80

// Scale font to get edit control height
//#define EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)    (3*(cy)/2)
#define EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)    (cy+8)

// Generic subclass proc, for panel item moving/sizing and intercept
// EDIT control VK_RETURN messages
extern LONG APIENTRY _EXPORT
  wxSubclassedGenericControlProc(WXHWND hWnd, WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

// ---------------------------------------------------------------------------
// useful macros and functions
// ---------------------------------------------------------------------------

// a wrapper macro for ZeroMemory()
#if defined(__WIN32__) && !defined(__WXMICROWIN__)
#define wxZeroMemory(obj)   ::ZeroMemory(&obj, sizeof(obj))
#else
#define wxZeroMemory(obj)   memset((void*) & obj, 0, sizeof(obj))
#endif

#if wxUSE_GUI

#include <wx/gdicmn.h>
#include <wx/colour.h>

// make conversion from wxColour and COLORREF a bit less painful
inline COLORREF wxColourToRGB(const wxColour& c)
{
    return RGB(c.Red(), c.Green(), c.Blue());
}

inline COLORREF wxColourToPalRGB(const wxColour& c)
{
    return PALETTERGB(c.Red(), c.Green(), c.Blue());
}

inline wxColour wxRGBToColour(COLORREF rgb)
{
    return wxColour(GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
}

inline void wxRGBToColour(wxColour& c, COLORREF rgb)
{
    c.Set(GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
}

// get the standard colour map for some standard colours - see comment in this
// function to understand why is it needed and when should it be used
//
// it returns a wxCOLORMAP (can't use COLORMAP itself here as comctl32.dll
// might be not included/available) array of size wxSTD_COLOUR_MAX
//
// NB: if you change these colours, update wxBITMAP_STD_COLOURS in the
//     resources as well: it must have the same number of pixels!
enum wxSTD_COLOUR
{
    wxSTD_COL_BTNTEXT,
    wxSTD_COL_BTNSHADOW,
    wxSTD_COL_BTNFACE,
    wxSTD_COL_BTNHIGHLIGHT,
    wxSTD_COL_MAX
};

struct WXDLLEXPORT wxCOLORMAP
{
    COLORREF from, to;
};

// this function is implemented in src/msw/window.cpp
extern wxCOLORMAP *wxGetStdColourMap();

// create a wxRect from Windows RECT
inline wxRect wxRectFromRECT(const RECT& r)
{
    return wxRect(r.left, r.top, r.right - r.left, r.bottom - r.top);
}

// copy Windows RECT to our wxRect
inline void wxCopyRECTToRect(const RECT& r, wxRect& rect)
{
    rect = wxRectFromRECT(r);
}

// translations between HIMETRIC units (which OLE likes) and pixels (which are
// liked by all the others) - implemented in msw/utilsexc.cpp
extern void HIMETRICToPixel(LONG *x, LONG *y);
extern void PixelToHIMETRIC(LONG *x, LONG *y);

// Windows convention of the mask is opposed to the wxWidgets one, so we need
// to invert the mask each time we pass one/get one to/from Windows
extern HBITMAP wxInvertMask(HBITMAP hbmpMask, int w = 0, int h = 0);

// Creates an icon or cursor depending from a bitmap
//
// The bitmap must be valid and it should have a mask. If it doesn't, a default
// mask is created using light grey as the transparent colour.
extern HICON wxBitmapToHICON(const wxBitmap& bmp);

// Same requirments as above apply and the bitmap must also have the correct
// size.
extern
HCURSOR wxBitmapToHCURSOR(const wxBitmap& bmp, int hotSpotX, int hotSpotY);

// get (x, y) from DWORD - notice that HI/LOWORD can *not* be used because they
// will fail on system with multiple monitors where the coords may be negative
//
// these macros are standard now (Win98) but some older headers don't have them
#ifndef GET_X_LPARAM
    #define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
    #define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif // GET_X_LPARAM

// get the current state of SHIFT/CTRL keys
inline bool wxIsShiftDown()
{
//    return (::GetKeyState(VK_SHIFT) & 0x100) != 0;
    // Returns different negative values on WinME and WinNT,
    // so simply test for negative value.
    return ::GetKeyState(VK_SHIFT) < 0;
}

inline bool wxIsCtrlDown()
{
//    return (::GetKeyState(VK_CONTROL) & 0x100) != 0;
    // Returns different negative values on WinME and WinNT,
    // so simply test for negative value.
    return ::GetKeyState(VK_CONTROL) < 0;
}

// wrapper around GetWindowRect() and GetClientRect() APIs doing error checking
// for Win32
inline RECT wxGetWindowRect(HWND hwnd)
{
    RECT rect;

    if ( !::GetWindowRect(hwnd, &rect) )
    {
        wxLogLastError(_T("GetWindowRect"));
    }

    return rect;
}

inline RECT wxGetClientRect(HWND hwnd)
{
    RECT rect;

    if ( !::GetClientRect(hwnd, &rect) )
    {
        wxLogLastError(_T("GetClientRect"));
    }

    return rect;
}

// ---------------------------------------------------------------------------
// small helper classes
// ---------------------------------------------------------------------------

// a template to make initializing Windows styructs less painful: it zeroes all
// the struct fields and also sets cbSize member to the correct value (and so
// can be only used with structures which have this member...)
template <class T>
struct WinStruct : public T
{
    WinStruct()
    {
        ::ZeroMemory(this, sizeof(T));

        // explicit qualification is required here for this to be valid C++
        this->cbSize = sizeof(T);
    }
};


// create an instance of this class and use it as the HDC for screen, will
// automatically release the DC going out of scope
class ScreenHDC
{
public:
    ScreenHDC() { m_hdc = ::GetDC(NULL);    }
   ~ScreenHDC() { ::ReleaseDC(NULL, m_hdc); }

    operator HDC() const { return m_hdc; }

private:
    HDC m_hdc;

    DECLARE_NO_COPY_CLASS(ScreenHDC)
};

// the same as ScreenHDC but for window DCs
class WindowHDC
{
public:
    WindowHDC(HWND hwnd) { m_hdc = ::GetDC(m_hwnd = hwnd); }
   ~WindowHDC() { ::ReleaseDC(m_hwnd, m_hdc); }

    operator HDC() const { return m_hdc; }

private:
   HWND m_hwnd;
   HDC m_hdc;

   DECLARE_NO_COPY_CLASS(WindowHDC)
};

// the same as ScreenHDC but for memory DCs: creates the HDC compatible with
// the given one (screen by default) in ctor and destroys it in dtor
class MemoryHDC
{
public:
    MemoryHDC(HDC hdc = 0) { m_hdc = ::CreateCompatibleDC(hdc); }
   ~MemoryHDC() { ::DeleteDC(m_hdc); }

    operator HDC() const { return m_hdc; }

private:
    HDC m_hdc;

    DECLARE_NO_COPY_CLASS(MemoryHDC)
};

// a class which selects a GDI object into a DC in its ctor and deselects in
// dtor
class SelectInHDC
{
public:
    SelectInHDC(HDC hdc, HGDIOBJ hgdiobj) : m_hdc(hdc)
        { m_hgdiobj = ::SelectObject(hdc, hgdiobj); }

   ~SelectInHDC() { ::SelectObject(m_hdc, m_hgdiobj); }

   // return true if the object was successfully selected
   operator bool() const { return m_hgdiobj != 0; }

private:
   HDC m_hdc;
   HGDIOBJ m_hgdiobj;

   DECLARE_NO_COPY_CLASS(SelectInHDC)
};

// a class for temporary bitmaps
class CompatibleBitmap
{
public:
    CompatibleBitmap(HDC hdc, int w, int h)
    {
        m_hbmp = ::CreateCompatibleBitmap(hdc, w, h);
    }

    ~CompatibleBitmap()
    {
        if ( m_hbmp )
            ::DeleteObject(m_hbmp);
    }

    operator HBITMAP() const { return m_hbmp; }

private:
    HBITMAP m_hbmp;
};

// when working with global pointers (which is unfortunately still necessary
// sometimes, e.g. for clipboard) it is important to unlock them exactly as
// many times as we lock them which just asks for using a "smart lock" class
class GlobalPtr
{
public:
    GlobalPtr(HGLOBAL hGlobal) : m_hGlobal(hGlobal)
    {
        m_ptr = GlobalLock(hGlobal);
        if ( !m_ptr )
        {
            wxLogLastError(_T("GlobalLock"));
        }
    }

    ~GlobalPtr()
    {
        if ( !GlobalUnlock(m_hGlobal) )
        {
#ifdef __WXDEBUG__
            // this might happen simply because the block became unlocked
            DWORD dwLastError = ::GetLastError();
            if ( dwLastError != NO_ERROR )
            {
                wxLogApiError(_T("GlobalUnlock"), dwLastError);
            }
#endif // __WXDEBUG__
        }
    }

    operator void *() const { return m_ptr; }

private:
    HGLOBAL m_hGlobal;
    void *m_ptr;

    DECLARE_NO_COPY_CLASS(GlobalPtr)
};

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

#define GetHbrush()             ((HPEN)GetResourceHandle())
#define GetHbrushOf(brush)      ((HPEN)(brush).GetResourceHandle())

#define GetHmenu()              ((HMENU)GetHMenu())
#define GetHmenuOf(menu)        ((HMENU)menu->GetHMenu())

#define GetHcursor()            ((HCURSOR)GetHCURSOR())
#define GetHcursorOf(cursor)    ((HCURSOR)(cursor).GetHCURSOR())

#define GetHfont()              ((HFONT)GetHFONT())
#define GetHfontOf(font)        ((HFONT)(font).GetHFONT())

#define GetHpalette()           ((HPALETTE)GetHPALETTE())
#define GetHpaletteOf(pal)      ((HPALETTE)(pal).GetHPALETTE())

#define GetHpen()               ((HPEN)GetResourceHandle())
#define GetHpenOf(pen)          ((HPEN)(pen).GetResourceHandle())

#define GetHrgn()               ((HRGN)GetHRGN())
#define GetHrgnOf(rgn)          ((HRGN)(rgn).GetHRGN())

#endif // wxUSE_GUI

// ---------------------------------------------------------------------------
// global data
// ---------------------------------------------------------------------------

extern WXDLLIMPEXP_DATA_BASE(HINSTANCE) wxhInstance;

// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

extern "C"
{
    WXDLLIMPEXP_BASE HINSTANCE wxGetInstance();
}

WXDLLIMPEXP_BASE void wxSetInstance(HINSTANCE hInst);

// return the full path of the given module
inline wxString wxGetFullModuleName(HMODULE hmod)
{
    wxString fullname;
    if ( !::GetModuleFileName
            (
                hmod,
                wxStringBuffer(fullname, MAX_PATH),
                MAX_PATH
            ) )
    {
        wxLogLastError(_T("GetModuleFileName"));
    }

    return fullname;
}

// return the full path of the program file
inline wxString wxGetFullModuleName()
{
    return wxGetFullModuleName((HMODULE)wxGetInstance());
}

#if wxUSE_GUI

// cursor stuff
extern HCURSOR wxGetCurrentBusyCursor();    // from msw/utils.cpp
extern const wxCursor *wxGetGlobalCursor(); // from msw/cursor.cpp

WXDLLEXPORT void wxGetCharSize(WXHWND wnd, int *x, int *y, const wxFont& the_font);
WXDLLEXPORT void wxFillLogFont(LOGFONT *logFont, const wxFont *font);
WXDLLEXPORT wxFont wxCreateFontFromLogFont(const LOGFONT *logFont);
WXDLLEXPORT wxFontEncoding wxGetFontEncFromCharSet(int charset);

WXDLLEXPORT void wxSliderEvent(WXHWND control, WXWORD wParam, WXWORD pos);
WXDLLEXPORT void wxScrollBarEvent(WXHWND hbar, WXWORD wParam, WXWORD pos);

// Find maximum size of window/rectangle
extern WXDLLEXPORT void wxFindMaxSize(WXHWND hwnd, RECT *rect);

// Safely get the window text (i.e. without using fixed size buffer)
extern WXDLLEXPORT wxString wxGetWindowText(WXHWND hWnd);

// get the window class name
extern WXDLLEXPORT wxString wxGetWindowClass(WXHWND hWnd);

// get the window id (should be unsigned, hence this is not wxWindowID which
// is, for mainly historical reasons, signed)
extern WXDLLEXPORT WXWORD wxGetWindowId(WXHWND hWnd);

// check if hWnd's WNDPROC is wndProc. Return true if yes, false if they are
// different
extern WXDLLEXPORT bool wxCheckWindowWndProc(WXHWND hWnd, WXFARPROC wndProc);

// Does this window style specify any border?
inline bool wxStyleHasBorder(long style)
{
    return (style & (wxSIMPLE_BORDER | wxRAISED_BORDER |
                     wxSUNKEN_BORDER | wxDOUBLE_BORDER)) != 0;
}

// ----------------------------------------------------------------------------
// functions mapping HWND to wxWindow
// ----------------------------------------------------------------------------

// this function simply checks whether the given hWnd corresponds to a wxWindow
// and returns either that window if it does or NULL otherwise
extern WXDLLEXPORT wxWindow* wxFindWinFromHandle(WXHWND hWnd);

// find the window for HWND which is part of some wxWindow, i.e. unlike
// wxFindWinFromHandle() above it will also work for "sub controls" of a
// wxWindow.
//
// returns the wxWindow corresponding to the given HWND or NULL.
extern WXDLLEXPORT wxWindow *wxGetWindowFromHWND(WXHWND hwnd);

// Get the size of an icon
extern WXDLLEXPORT wxSize wxGetHiconSize(HICON hicon);

// Lines are drawn differently for WinCE and regular WIN32
WXDLLEXPORT void wxDrawLine(HDC hdc, int x1, int y1, int x2, int y2);

// ----------------------------------------------------------------------------
// 32/64 bit helpers
// ----------------------------------------------------------------------------

#ifdef __WIN64__

inline void *wxGetWindowProc(HWND hwnd)
{
    return (void *)::GetWindowLongPtr(hwnd, GWLP_WNDPROC);
}

inline void *wxGetWindowUserData(HWND hwnd)
{
    return (void *)::GetWindowLongPtr(hwnd, GWLP_USERDATA);
}

inline WNDPROC wxSetWindowProc(HWND hwnd, WNDPROC func)
{
    return (WNDPROC)::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)func);
}

inline void *wxSetWindowUserData(HWND hwnd, void *data)
{
    return (void *)::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)data);
}

#else // __WIN32__

#ifdef __VISUALC__
    // strangely enough, VC++ 7.1 gives warnings about 32 -> 64 bit conversions
    // in the functions below, even in spite of the explicit casts
    #pragma warning(disable:4311)
    #pragma warning(disable:4312)
#endif

inline void *wxGetWindowProc(HWND hwnd)
{
    return (void *)::GetWindowLong(hwnd, GWL_WNDPROC);
}

inline void *wxGetWindowUserData(HWND hwnd)
{
    return (void *)::GetWindowLong(hwnd, GWL_USERDATA);
}

inline WNDPROC wxSetWindowProc(HWND hwnd, WNDPROC func)
{
    return (WNDPROC)::SetWindowLong(hwnd, GWL_WNDPROC, (LONG)func);
}

inline void *wxSetWindowUserData(HWND hwnd, void *data)
{
    return (void *)::SetWindowLong(hwnd, GWL_USERDATA, (LONG)data);
}

#ifdef __VISUALC__
    #pragma warning(default:4311)
    #pragma warning(default:4312)
#endif

#endif // __WIN64__/__WIN32__

#endif // wxUSE_GUI

#endif
    // _WX_PRIVATE_H_
