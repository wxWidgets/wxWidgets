///////////////////////////////////////////////////////////////////////////////
// Name:        msw/toplevel.cpp
// Purpose:     implements wxTopLevelWindow for MSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     24.09.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "toplevel.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/toplevel.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/frame.h"
#endif //WX_PRECOMP

#include "wx/msw/private.h"

#include "wx/popupwin.h"

#ifndef ICON_BIG
    #define ICON_BIG 1
#endif

#ifndef ICON_SMALL
    #define ICON_SMALL 0
#endif

// ----------------------------------------------------------------------------
// stubs for missing functions under MicroWindows
// ----------------------------------------------------------------------------

#ifdef __WXMICROWIN__

// static inline bool IsIconic(HWND WXUNUSED(hwnd)) { return FALSE; }
static inline bool IsZoomed(HWND WXUNUSED(hwnd)) { return FALSE; }

#endif // __WXMICROWIN__

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// list of all frames and modeless dialogs
wxWindowList wxModelessWindows;

// the name of the default wxWindows class
extern const wxChar *wxCanvasClassName;

// the hidden parent for wxFRAME_NO_TASKBAR unowned frames
wxWindow *wxTopLevelWindowMSW::ms_hiddenParent = NULL;

// ============================================================================
// wxTopLevelWindowMSW implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDialog helpers
// ----------------------------------------------------------------------------

// Dialog window proc
LONG APIENTRY _EXPORT
wxDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch ( message )
    {
        case WM_INITDIALOG:
            // for this message, returning TRUE tells system to set focus to the
            // first control in the dialog box
            return TRUE;

        default:
            // for all the other ones, FALSE means that we didn't process the
            // message
            return FALSE;
    }
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW creation
// ----------------------------------------------------------------------------

void wxTopLevelWindowMSW::Init()
{
    m_iconized =
    m_maximizeOnShow = FALSE;

    // unlike (almost?) all other windows, frames are created hidden
    m_isShown = FALSE;

    // Data to save/restore when calling ShowFullScreen
    m_fsStyle = 0;
    m_fsOldWindowStyle = 0;
    m_fsIsMaximized = FALSE;
    m_fsIsShowing = FALSE;
}

WXDWORD wxTopLevelWindowMSW::MSWGetStyle(long style, WXDWORD *exflags) const
{
    // let the base class deal with the common styles but fix the ones which
    // don't make sense for us (we also deal with the borders ourselves)
    WXDWORD msflags = wxWindow::MSWGetStyle
                      (
                        (style & ~wxBORDER_MASK) | wxBORDER_NONE, exflags
                      ) & ~WS_CHILD;

    // first select the kind of window being created
    //
    // note that if we don't set WS_POPUP, Windows assumes WS_OVERLAPPED and
    // creates a window with both caption and border, hence we also test it
    // below in some other cases
    if ( style & wxFRAME_TOOL_WINDOW )
        msflags |= WS_POPUP;
    else
        msflags |= WS_OVERLAPPED;

    // border and caption styles
    if ( style & wxRESIZE_BORDER )
        msflags |= WS_THICKFRAME;
    else if ( !(style & wxBORDER_NONE) )
        msflags |= WS_BORDER;
    else
        msflags |= WS_POPUP;

    if ( style & wxCAPTION )
        msflags |= WS_CAPTION;
    else
        msflags |= WS_POPUP;

    // next translate the individual flags
    if ( style & wxMINIMIZE_BOX )
        msflags |= WS_MINIMIZEBOX;
    if ( style & wxMAXIMIZE_BOX )
        msflags |= WS_MAXIMIZEBOX;
    if ( style & wxSYSTEM_MENU )
        msflags |= WS_SYSMENU;
    if ( style & wxMINIMIZE )
        msflags |= WS_MINIMIZE;
    if ( style & wxMAXIMIZE )
        msflags |= WS_MAXIMIZE;

    // Keep this here because it saves recoding this function in wxTinyFrame
#if wxUSE_ITSY_BITSY && !defined(__WIN32__)
    if ( style & wxTINY_CAPTION_VERT )
        msflags |= IBS_VERTCAPTION;
    if ( style & wxTINY_CAPTION_HORIZ )
        msflags |= IBS_HORZCAPTION;
#else
    if ( style & (wxTINY_CAPTION_VERT | wxTINY_CAPTION_HORIZ) )
        msflags |= WS_CAPTION;
#endif

    if ( exflags )
    {
#if !defined(__WIN16__) && !defined(__SC__)
        if ( !(GetExtraStyle() & wxTOPLEVEL_EX_DIALOG) )
        {
            if ( style & wxFRAME_TOOL_WINDOW )
            {
                // create the palette-like window
                *exflags |= WS_EX_TOOLWINDOW;
            }

            // We have to solve 2 different problems here:
            //
            // 1. frames with wxFRAME_NO_TASKBAR flag shouldn't appear in the
            //    taskbar even if they don't have a parent
            //
            // 2. frames without this style should appear in the taskbar even
            //    if they're owned (Windows only puts non owned windows into
            //    the taskbar normally)
            //
            // The second one is solved here by using WS_EX_APPWINDOW flag, the
            // first one is dealt with in our MSWGetParent() method
            // implementation
            if ( !(style & wxFRAME_NO_TASKBAR) && GetParent() )
            {
                // need to force the frame to appear in the taskbar
                *exflags |= WS_EX_APPWINDOW;
            }
            //else: nothing to do [here]
        }
#endif // !Win16

        if ( style & wxSTAY_ON_TOP )
            *exflags |= WS_EX_TOPMOST;

#ifdef __WIN32__
        if ( GetExtraStyle() & wxFRAME_EX_CONTEXTHELP )
            *exflags |= WS_EX_CONTEXTHELP;
#endif // __WIN32__
    }

    return msflags;
}

WXHWND wxTopLevelWindowMSW::MSWGetParent() const
{
    // for the frames without wxFRAME_FLOAT_ON_PARENT style we should use NULL
    // parent HWND or it would be always on top of its parent which is not what
    // we usually want (in fact, we only want it for frames with the
    // wxFRAME_FLOAT_ON_PARENT flag)
    wxWindow *parent;
    if ( HasFlag(wxFRAME_FLOAT_ON_PARENT) )
    {
        parent = GetParent();

        // this flag doesn't make sense then and will be ignored
        wxASSERT_MSG( parent,
                      _T("wxFRAME_FLOAT_ON_PARENT but no parent?") );
    }
    else // don't float on parent, must not be owned
    {
        parent = NULL;
    }

    // now deal with the 2nd taskbar-related problem (see comments above in
    // MSWGetStyle())
    if ( HasFlag(wxFRAME_NO_TASKBAR) && !parent )
    {
        if ( !ms_hiddenParent )
        {
            ms_hiddenParent = new wxTopLevelWindowMSW(NULL, -1, _T(""));

            // we shouldn't leave it in wxTopLevelWindows or we wouldn't
            // terminate the app when the last user-created frame is deleted --
            // see ~wxTopLevelWindowMSW
            wxTopLevelWindows.DeleteObject(ms_hiddenParent);
        }

        parent = ms_hiddenParent;
    }

    return parent ? parent->GetHWND() : NULL;
}

bool wxTopLevelWindowMSW::CreateDialog(const void *dlgTemplate,
                                       const wxString& title,
                                       const wxPoint& pos,
                                       const wxSize& size)
{
#ifdef __WXMICROWIN__
    // no dialogs support under MicroWin yet
    return CreateFrame(title, pos, size);
#else // !__WXMICROWIN__
    wxWindow *parent = GetParent();

    // for the dialogs without wxDIALOG_NO_PARENT style, use the top level
    // app window as parent - this avoids creating modal dialogs without
    // parent
    if ( !parent && !(GetWindowStyleFlag() & wxDIALOG_NO_PARENT) )
    {
        parent = wxTheApp->GetTopWindow();

        if ( parent )
        {
            // don't use transient windows as parents, this is dangerous as it
            // can lead to a crash if the parent is destroyed before the child
            //
            // also don't use the window which is currently hidden as then the
            // dialog would be hidden as well
            if ( (parent->GetExtraStyle() & wxWS_EX_TRANSIENT) ||
                    !parent->IsShown() )
            {
                parent = NULL;
            }
        }
    }

    m_hWnd = (WXHWND)::CreateDialogIndirect
                       (
                        wxGetInstance(),
                        (DLGTEMPLATE*)dlgTemplate,
                        parent ? GetHwndOf(parent) : NULL,
                        (DLGPROC)wxDlgProc
                       );

    if ( !m_hWnd )
    {
        wxFAIL_MSG(_("Failed to create dialog. Incorrect DLGTEMPLATE?"));

        wxLogSysError(_("Can't create dialog using memory template"));

        return FALSE;
    }

    WXDWORD exflags;
    (void)MSWGetCreateWindowFlags(&exflags);

    if ( exflags )
    {
        ::SetWindowLong(GetHwnd(), GWL_EXSTYLE, exflags);
        ::SetWindowPos(GetHwnd(), NULL, 0, 0, 0, 0,
                       SWP_NOSIZE |
                       SWP_NOMOVE |
                       SWP_NOZORDER |
                       SWP_NOACTIVATE);
    }

#if defined(__WIN95__)
    // For some reason, the system menu is activated when we use the
    // WS_EX_CONTEXTHELP style, so let's set a reasonable icon
    if ( exflags & WS_EX_CONTEXTHELP )
    {
        wxFrame *winTop = wxDynamicCast(wxTheApp->GetTopWindow(), wxFrame);
        if ( winTop )
        {
            wxIcon icon = winTop->GetIcon();
            if ( icon.Ok() )
            {
                ::SendMessage(GetHwnd(), WM_SETICON,
                              (WPARAM)TRUE,
                              (LPARAM)GetHiconOf(icon));
            }
        }
    }
#endif // __WIN95__

    // move the dialog to its initial position without forcing repainting
    int x, y, w, h;
    if ( !MSWGetCreateWindowCoords(pos, size, x, y, w, h) )
    {
        x =
        w = (int)CW_USEDEFAULT;
    }

    // we can't use CW_USEDEFAULT here as we're not calling CreateWindow()
    // and passing CW_USEDEFAULT to MoveWindow() results in resizing the
    // window to (0, 0) size which breaks quite a lot of things, e.g. the
    // sizer calculation in wxSizer::Fit()
    if ( w == (int)CW_USEDEFAULT )
    {
        // the exact number doesn't matter, the dialog will be resized
        // again soon anyhow but it should be big enough to allow
        // calculation relying on "totalSize - clientSize > 0" work, i.e.
        // at least greater than the title bar height
        w =
        h = 100;
    }

    if ( x == (int)CW_USEDEFAULT )
    {
        // centre it on the screen - what else can we do?
        wxSize sizeDpy = wxGetDisplaySize();

        x = (sizeDpy.x - w) / 2;
        y = (sizeDpy.y - h) / 2;
    }

    if ( !::MoveWindow(GetHwnd(), x, y, w, h, FALSE) )
    {
        wxLogLastError(wxT("MoveWindow"));
    }

    if ( !title.empty() )
    {
        ::SetWindowText(GetHwnd(), title);
    }

    SubclassWin(m_hWnd);

    return TRUE;
#endif // __WXMICROWIN__/!__WXMICROWIN__
}

bool wxTopLevelWindowMSW::CreateFrame(const wxString& title,
                                      const wxPoint& pos,
                                      const wxSize& size)
{
    WXDWORD exflags;
    WXDWORD flags = MSWGetCreateWindowFlags(&exflags);

    return MSWCreate(wxCanvasClassName, title, pos, size, flags, exflags);
}

bool wxTopLevelWindowMSW::Create(wxWindow *parent,
                                 wxWindowID id,
                                 const wxString& title,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
{
    // init our fields
    Init();

    m_windowStyle = style;

    SetName(name);

    m_windowId = id == -1 ? NewControlId() : id;

    wxTopLevelWindows.Append(this);

    if ( parent )
        parent->AddChild(this);

    if ( GetExtraStyle() & wxTOPLEVEL_EX_DIALOG )
    {
        // we have different dialog templates to allows creation of dialogs
        // with & without captions under MSWindows, resizeable or not (but a
        // resizeable dialog always has caption - otherwise it would look too
        // strange)

        // we need 3 additional WORDs for dialog menu, class and title (as we
        // don't use DS_SETFONT we don't need the fourth WORD for the font)
        static const int dlgsize = sizeof(DLGTEMPLATE) + (sizeof(WORD) * 3);
        DLGTEMPLATE *dlgTemplate = (DLGTEMPLATE *)malloc(dlgsize);
        memset(dlgTemplate, 0, dlgsize);

        // these values are arbitrary, they won't be used normally anyhow
        dlgTemplate->x  = 34;
        dlgTemplate->y  = 22;
        dlgTemplate->cx = 144;
        dlgTemplate->cy = 75;

        // reuse the code in MSWGetStyle() but correct the results slightly for
        // the dialog
        dlgTemplate->style = MSWGetStyle(style, NULL);

        // all dialogs are popups
        dlgTemplate->style |= WS_POPUP;

        // force 3D-look if necessary, it looks impossibly ugly otherwise
        if ( style & (wxRESIZE_BORDER | wxCAPTION) )
            dlgTemplate->style |= DS_MODALFRAME;

        bool ret = CreateDialog(dlgTemplate, title, pos, size);
        free(dlgTemplate);

        return ret;
    }
    else // !dialog
    {
        return CreateFrame(title, pos, size);
    }
}

wxTopLevelWindowMSW::~wxTopLevelWindowMSW()
{
    if ( this == ms_hiddenParent )
    {
        // stop [infinite] recursion which would otherwise happen when we do
        // "delete ms_hiddenParent" below
        return;
    }

    wxTopLevelWindows.DeleteObject(this);

    if ( wxModelessWindows.Find(this) )
        wxModelessWindows.DeleteObject(this);

    // If this is the last top-level window, exit.
    if ( wxTheApp && (wxTopLevelWindows.Number() == 0) )
    {
        if ( ms_hiddenParent )
        {
            delete ms_hiddenParent;
            ms_hiddenParent = NULL;
        }

        wxTheApp->SetTopWindow(NULL);

        if ( wxTheApp->GetExitOnFrameDelete() )
        {
            ::PostQuitMessage(0);
        }
    }
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW showing
// ----------------------------------------------------------------------------

void wxTopLevelWindowMSW::DoShowWindow(int nShowCmd)
{
    ::ShowWindow(GetHwnd(), nShowCmd);

    m_iconized = nShowCmd == SW_MINIMIZE;
}

bool wxTopLevelWindowMSW::Show(bool show)
{
    // don't use wxWindow version as we want to call DoShowWindow() ourselves
    if ( !wxWindowBase::Show(show) )
        return FALSE;

    int nShowCmd;
    if ( show )
    {
        if ( m_maximizeOnShow )
        {
            // show and maximize
            nShowCmd = SW_MAXIMIZE;

            m_maximizeOnShow = FALSE;
        }
        else // just show
        {
            nShowCmd = SW_SHOW;
        }
    }
    else // hide
    {
        nShowCmd = SW_HIDE;
    }

    DoShowWindow(nShowCmd);

    if ( show )
    {
        ::BringWindowToTop(GetHwnd());

        wxActivateEvent event(wxEVT_ACTIVATE, TRUE, m_windowId);
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent(event);
    }
    else // hide
    {
        // Try to highlight the correct window (the parent)
        if ( GetParent() )
        {
            HWND hWndParent = GetHwndOf(GetParent());
            if (hWndParent)
                ::BringWindowToTop(hWndParent);
        }
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW maximize/minimize
// ----------------------------------------------------------------------------

void wxTopLevelWindowMSW::Maximize(bool maximize)
{
    if ( IsShown() )
    {
        // just maximize it directly
        DoShowWindow(maximize ? SW_MAXIMIZE : SW_RESTORE);
    }
    else // hidden
    {
        // we can't maximize the hidden frame because it shows it as well, so
        // just remember that we should do it later in this case
        m_maximizeOnShow = TRUE;
    }
}

bool wxTopLevelWindowMSW::IsMaximized() const
{
    return ::IsZoomed(GetHwnd()) != 0;
}

void wxTopLevelWindowMSW::Iconize(bool iconize)
{
    DoShowWindow(iconize ? SW_MINIMIZE : SW_RESTORE);
}

bool wxTopLevelWindowMSW::IsIconized() const
{
    // also update the current state
    ((wxTopLevelWindowMSW *)this)->m_iconized = ::IsIconic(GetHwnd()) != 0;

    return m_iconized;
}

void wxTopLevelWindowMSW::Restore()
{
    DoShowWindow(SW_RESTORE);
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW fullscreen
// ----------------------------------------------------------------------------

bool wxTopLevelWindowMSW::ShowFullScreen(bool show, long style)
{
    if (show)
    {
        if (IsFullScreen())
            return FALSE;

        m_fsIsShowing = TRUE;
        m_fsStyle = style;

        // zap the frame borders

        // save the 'normal' window style
        m_fsOldWindowStyle = GetWindowLong((HWND)GetHWND(), GWL_STYLE);

        // save the old position, width & height, maximize state
        m_fsOldSize = GetRect();
        m_fsIsMaximized = IsMaximized();

        // decide which window style flags to turn off
        LONG newStyle = m_fsOldWindowStyle;
        LONG offFlags = 0;

        if (style & wxFULLSCREEN_NOBORDER)
            offFlags |= WS_BORDER | WS_THICKFRAME;
        if (style & wxFULLSCREEN_NOCAPTION)
            offFlags |= (WS_CAPTION | WS_SYSMENU);

        newStyle &= (~offFlags);

        // change our window style to be compatible with full-screen mode
        ::SetWindowLong((HWND)GetHWND(), GWL_STYLE, newStyle);

        // resize to the size of the desktop
        int width, height;

        RECT rect = wxGetWindowRect(::GetDesktopWindow());
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;

        SetSize(width, height);

        // now flush the window style cache and actually go full-screen
        SetWindowPos((HWND)GetHWND(), HWND_TOP, 0, 0, width, height, SWP_FRAMECHANGED);

        wxSizeEvent event(wxSize(width, height), GetId());
        GetEventHandler()->ProcessEvent(event);

        return TRUE;
    }
    else
    {
        if (!IsFullScreen())
            return FALSE;

        m_fsIsShowing = FALSE;

        Maximize(m_fsIsMaximized);
        SetWindowLong((HWND)GetHWND(),GWL_STYLE, m_fsOldWindowStyle);
        SetWindowPos((HWND)GetHWND(),HWND_TOP,m_fsOldSize.x, m_fsOldSize.y,
            m_fsOldSize.width, m_fsOldSize.height, SWP_FRAMECHANGED);

        return TRUE;
    }
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW misc
// ----------------------------------------------------------------------------

void wxTopLevelWindowMSW::SetIcon(const wxIcon& icon)
{
    SetIcons( wxIconBundle( icon ) );
}

void wxTopLevelWindowMSW::SetIcons(const wxIconBundle& icons)
{
    wxTopLevelWindowBase::SetIcons(icons);

#if defined(__WIN95__) && !defined(__WXMICROWIN__)
    const wxIcon& sml = icons.GetIcon( wxSize( 16, 16 ) );
    if( sml.Ok() && sml.GetWidth() == 16 && sml.GetHeight() == 16 )
    {
        ::SendMessage( GetHwndOf( this ), WM_SETICON, ICON_SMALL,
                       (LPARAM)GetHiconOf(sml) );
    }

    const wxIcon& big = icons.GetIcon( wxSize( 32, 32 ) );
    if( big.Ok() && big.GetWidth() == 32 && big.GetHeight() == 32 )
    {
        ::SendMessage( GetHwndOf( this ), WM_SETICON, ICON_BIG,
                       (LPARAM)GetHiconOf(big) );
    }
#endif // __WIN95__
}

bool wxTopLevelWindowMSW::EnableCloseButton(bool enable)
{
#ifndef __WXMICROWIN__
    // get system (a.k.a. window) menu
    HMENU hmenu = ::GetSystemMenu(GetHwnd(), FALSE /* get it */);
    if ( !hmenu )
    {
        wxLogLastError(_T("GetSystemMenu"));

        return FALSE;
    }

    // enabling/disabling the close item from it also automatically
    // disables/enables the close title bar button
    if ( ::EnableMenuItem(hmenu, SC_CLOSE,
                          MF_BYCOMMAND |
                          (enable ? MF_ENABLED : MF_GRAYED)) == -1 )
    {
        wxLogLastError(_T("EnableMenuItem(SC_CLOSE)"));

        return FALSE;
    }

    // update appearance immediately
    if ( !::DrawMenuBar(GetHwnd()) )
    {
        wxLogLastError(_T("DrawMenuBar"));
    }
#endif // !__WXMICROWIN__

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW message processing
// ----------------------------------------------------------------------------

long wxTopLevelWindowMSW::HandleNcActivate(bool activate)
{
#if wxUSE_POPUPWIN
    /*
       Normally, when another top level (whether it is overlapped or popup)
       window is shown, it is activated and the parent window (i.e. we) loses
       the activation. This, however, looks very ugly when the child window is
       a [custom] combobox which we implement using a popup window as surely
       opening a combobox shouldn't result in deactivating the parent window.

       So we don't redraw the title bar in this case, even if we still return
       TRUE to let the change of activation to take place as otherwise the
       controls inside the popup window wouldn't work properly.
     */
    if ( !activate && wxPopupWindow::FindPopupFor(this) )
    {
        return TRUE;
    }
#endif // wxUSE_POPUPWIN

    return FALSE;
}

long
wxTopLevelWindowMSW::MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
{
    if ( msg == WM_NCACTIVATE && HandleNcActivate(wParam != 0) )
    {
        // we processed WM_NCACTIVATE ourselves
        return TRUE;
    }

    return wxTopLevelWindowBase::MSWWindowProc(msg, wParam, lParam);
}
