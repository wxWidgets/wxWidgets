///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/uxtheme.h
// Purpose:     wxUxThemeEngine class: support for XP themes
// Author:      John Platts, Vadim Zeitlin
// Modified by:
// Created:     2003
// Copyright:   (c) 2003 John Platts, Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UXTHEME_H_
#define _WX_UXTHEME_H_

#include "wx/defs.h"

#if wxUSE_UXTHEME

#include "wx/msw/private.h"     // we use GetHwndOf()
#include <uxtheme.h>

#if defined(DTPB_WINDOWDC)
// DTPB_WINDOWDC has been added for Vista so it's save to assume that an SDK
// including it has vssym32.h available
#define HAVE_VSSYM32
#endif

#if defined(HAVE_VSSYM32)
#include <vssym32.h>
#else
#include <tmschema.h>
#endif

// ----------------------------------------------------------------------------
// Definitions for legacy Windows SDKs
// ----------------------------------------------------------------------------
// Some definitions introduced with Windows Vista might be missing in older SDKs
// Missing definitions are added here for compatibility

#ifndef VSCLASS_LISTVIEW
#define LISS_NORMAL 1
#define LISS_HOT 2
#define LISS_SELECTED 3
#define LISS_DISABLED 4
#define LISS_SELECTEDNOTFOCUS 5
#define LISS_HOTSELECTED 6
#endif

#ifndef DTT_TEXTCOLOR
#define DTT_TEXTCOLOR       (1UL << 0)      // crText has been specified
#define DTT_STATEID         (1UL << 8)      // IStateId has been specified
#endif

#ifndef DSS_HIDEPREFIX
#define DSS_HIDEPREFIX  0x0200
#define DSS_PREFIXONLY  0x0400
#endif

#ifndef TMT_FONT
#define TMT_FONT    210
#endif

#ifndef HAVE_VSSYM32
enum EXPANDOBUTTONSTATES {
    TDLGEBS_NORMAL = 1,
    TDLGEBS_HOVER = 2,
    TDLGEBS_PRESSED = 3,
    TDLGEBS_EXPANDEDNORMAL = 4,
    TDLGEBS_EXPANDEDHOVER = 5,
    TDLGEBS_EXPANDEDPRESSED = 6,
    TDLGEBS_NORMALDISABLED = 7,
    TDLGEBS_EXPANDEDDISABLED = 8,
};

enum TASKDIALOGPARTS {
    TDLG_PRIMARYPANEL = 1,
    TDLG_MAININSTRUCTIONPANE = 2,
    TDLG_MAINICON = 3,
    TDLG_CONTENTPANE = 4,
    TDLG_CONTENTICON = 5,
    TDLG_EXPANDEDCONTENT = 6,
    TDLG_COMMANDLINKPANE = 7,
    TDLG_SECONDARYPANEL = 8,
    TDLG_CONTROLPANE = 9,
    TDLG_BUTTONSECTION = 10,
    TDLG_BUTTONWRAPPER = 11,
    TDLG_EXPANDOTEXT = 12,
    TDLG_EXPANDOBUTTON = 13,
    TDLG_VERIFICATIONTEXT = 14,
    TDLG_FOOTNOTEPANE = 15,
    TDLG_FOOTNOTEAREA = 16,
    TDLG_FOOTNOTESEPARATOR = 17,
    TDLG_EXPANDEDFOOTERAREA = 18,
    TDLG_PROGRESSBAR = 19,
    TDLG_IMAGEALIGNMENT = 20,
    TDLG_RADIOBUTTONPANE = 21,
};

#define CP_BACKGROUND 2
#define CP_TRANSPARENTBACKGROUND 3
#define CP_BORDER 4
#define CP_READONLY 5
#define CP_DROPDOWNBUTTONRIGHT 6
#define CP_DROPDOWNBUTTONLEFT 7
#define CP_CUEBANNER 8

#define RP_BACKGROUND 6
#define RP_SPLITTER 7
#define RP_SPLITTERVERT 8

enum BORDERSTATES {
    CBB_NORMAL = 1,
    CBB_HOT = 2,
    CBB_FOCUSED = 3,
    CBB_DISABLED = 4,
};

// The MENUPARTS enum is defined in MSVS 2005 SDK, even though it doesn't have
// vssym32.h, but it doesn't define the constants we use, so still define them,
// but make the enum unnamed for compatibility.
enum /* MENUPARTS -- FIXME-VC8: uncomment this when support for it is dropped */
{
    MENU_MENUITEM_TMSCHEMA = 1,
    MENU_SEPARATOR_TMSCHEMA = 6,
    MENU_POPUPBACKGROUND = 9,
    MENU_POPUPBORDERS = 10,
    MENU_POPUPCHECK = 11,
    MENU_POPUPCHECKBACKGROUND = 12,
    MENU_POPUPGUTTER = 13,
    MENU_POPUPITEM = 14,
    MENU_POPUPSEPARATOR = 15,
    MENU_POPUPSUBMENU = 16,
};

enum POPUPITEMSTATES
{
    MPI_NORMAL = 1,
    MPI_HOT = 2,
    MPI_DISABLED = 3,
    MPI_DISABLEDHOT = 4,
};

enum POPUPCHECKBACKGROUNDSTATES
{
    MCB_DISABLED = 1,
    MCB_NORMAL = 2,
    MCB_BITMAP = 3,
};

enum POPUPCHECKSTATES
{
    MC_CHECKMARKNORMAL = 1,
    MC_CHECKMARKDISABLED = 2,
    MC_BULLETNORMAL = 3,
    MC_BULLETDISABLED = 4,
};

#endif

// ----------------------------------------------------------------------------
// End definitions for legacy Windows SDKs
// ----------------------------------------------------------------------------

WXDLLIMPEXP_CORE bool wxUxThemeIsActive();

// ----------------------------------------------------------------------------
// wxUxThemeHandle: encapsulates ::Open/CloseThemeData()
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxUxThemeHandle
{
public:
    // For all factory functions, HWND doesn't need to be valid and may be
    // entirely omitted when using NewAtStdDPI(). However DPI must be valid if
    // it's specified, i.e. NewForStdDPI() can be used if the DPI doesn't
    // matter at all (which is the case if the theme is only used to query some
    // colours, for example), but otherwise (e.g. when using the theme to get
    // any metrics) the actual DPI of the window must be passed to NewForDPI().
    static wxUxThemeHandle NewAtDPI(HWND hwnd, const wchar_t *classes, int dpi)
    {
        return wxUxThemeHandle(hwnd, classes, dpi);
    }

    static wxUxThemeHandle NewAtStdDPI(HWND hwnd, const wchar_t *classes)
    {
        return NewAtDPI(hwnd, classes, STD_DPI);
    }

    static wxUxThemeHandle NewAtStdDPI(const wchar_t *classes)
    {
        return NewAtStdDPI(0, classes);
    }

    // wxWindow pointer here must be valid and its DPI is always used.
    wxUxThemeHandle(const wxWindow *win, const wchar_t *classes) :
        wxUxThemeHandle(GetHwndOf(win), classes, win->GetDPI().y)
    {
    }

    wxUxThemeHandle(wxUxThemeHandle&& other)
        : m_hTheme{other.m_hTheme}
    {
        other.m_hTheme = 0;
    }

    operator HTHEME() const { return m_hTheme; }

    ~wxUxThemeHandle()
    {
        if ( m_hTheme )
        {
            ::CloseThemeData(m_hTheme);
        }
    }

    // Return the colour for the given part, property and state.
    //
    // Note that the order of arguments here is _not_ the same as for
    // GetThemeColor() because we want to default the state.
    wxColour GetColour(int part, int prop, int state = 0) const;

    // Return the size of a theme element, either "as is" (TS_TRUE size) or as
    // it would be used for drawing (TS_DRAW size).
    //
    // For now we don't allow specifying the HDC or rectangle as they don't
    // seem to be useful.
    wxSize GetTrueSize(int part, int state = 0) const
    {
        return DoGetSize(part, state, TS_TRUE);
    }

    wxSize GetDrawSize(int part, int state = 0) const
    {
        return DoGetSize(part, state, TS_DRAW);
    }

    // Draw theme background: if the caller already has a RECT, it can be
    // provided directly, otherwise wxRect is converted to it.
    void DrawBackground(HDC hdc, const RECT& rc, int part, int state = 0);
    void DrawBackground(HDC hdc, const wxRect& rect, int part, int state = 0);

private:
    static const int STD_DPI = 96;

    static HTHEME DoOpenThemeData(HWND hwnd, const wchar_t *classes, int dpi);

    wxUxThemeHandle(HWND hwnd, const wchar_t *classes, int dpi) :
        m_hTheme{DoOpenThemeData(hwnd, classes, dpi)}
    {
    }

    wxSize DoGetSize(int part, int state, THEMESIZE ts) const;


    // This is almost, but not quite, const: it's only reset in move ctor.
    HTHEME m_hTheme;

    wxDECLARE_NO_COPY_CLASS(wxUxThemeHandle);
};

#else // !wxUSE_UXTHEME

inline bool wxUxThemeIsActive() { return false; }

#endif // wxUSE_UXTHEME/!wxUSE_UXTHEME

#endif // _WX_UXTHEME_H_

