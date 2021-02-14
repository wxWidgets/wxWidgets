/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/frame.cpp
// Purpose:     wxFrame
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/frame.h"

#ifndef WX_PRECOMP
    #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #include "wx/app.h"
    #include "wx/menu.h"
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/settings.h"
    #include "wx/dcclient.h"
    #include "wx/mdi.h"
    #include "wx/panel.h"
    #include "wx/log.h"
    #include "wx/toolbar.h"
    #include "wx/statusbr.h"
    #include "wx/menuitem.h"
#endif // WX_PRECOMP

#include "wx/msw/private.h"

#include "wx/generic/statusbr.h"

#ifdef __WXUNIVERSAL__
    #include "wx/univ/theme.h"
    #include "wx/univ/colschem.h"
#endif // __WXUNIVERSAL__

#if wxUSE_TASKBARBUTTON
    #include "wx/msw/taskbarbutton.h"
    #include "wx/dynlib.h"

    WXUINT wxMsgTaskbarButtonCreated = 0;
    #define wxTHBN_CLICKED 0x1800
    #define wxMSGFLT_ADD   0x01
#endif  // wxUSE_TASKBARBUTTON


// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

#if wxUSE_MENUS || wxUSE_MENUS_NATIVE
    extern wxMenu *wxCurrentPopupMenu;
#endif // wxUSE_MENUS || wxUSE_MENUS_NATIVE

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxFrame, wxFrameBase)
    EVT_SYS_COLOUR_CHANGED(wxFrame::OnSysColourChanged)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// static class members
// ----------------------------------------------------------------------------

#if wxUSE_STATUSBAR
    #if wxUSE_NATIVE_STATUSBAR
        bool wxFrame::m_useNativeStatusBar = true;
    #else
        bool wxFrame::m_useNativeStatusBar = false;
    #endif
#endif // wxUSE_NATIVE_STATUSBAR

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

void wxFrame::Init()
{
#if wxUSE_MENUS
    m_hMenu = NULL;
    m_menuDepth = 0;
#endif // wxUSE_MENUS

#if wxUSE_TOOLTIPS
    m_hwndToolTip = 0;
#endif

    m_wasMinimized = false;

#if wxUSE_TASKBARBUTTON
    m_taskBarButton = NULL;
#endif
}

bool wxFrame::Create(wxWindow *parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
{
    if ( !wxTopLevelWindow::Create(parent, id, title, pos, size, style, name) )
        return false;

    SetOwnBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));

#if wxUSE_TASKBARBUTTON
    static bool s_taskbarButtonCreatedMsgRegistered = false;
    if ( !s_taskbarButtonCreatedMsgRegistered )
    {
        s_taskbarButtonCreatedMsgRegistered = true;
        wxMsgTaskbarButtonCreated =
            ::RegisterWindowMessage(wxT("TaskbarButtonCreated"));

        // In case the application is run elevated, allow the
        // TaskbarButtonCreated and WM_COMMAND messages through.
#if wxUSE_DYNLIB_CLASS
        typedef BOOL (WINAPI *ChangeWindowMessageFilter_t)(UINT message,
                                                           DWORD dwFlag);
        wxDynamicLibrary dllUser32(wxT("user32.dll"));

        ChangeWindowMessageFilter_t pfnChangeWindowMessageFilter = NULL;
        wxDL_INIT_FUNC(pfn, ChangeWindowMessageFilter, dllUser32);
        if ( pfnChangeWindowMessageFilter )
        {
            pfnChangeWindowMessageFilter(wxMsgTaskbarButtonCreated,
                                           wxMSGFLT_ADD);
            pfnChangeWindowMessageFilter(WM_COMMAND, wxMSGFLT_ADD);
        }
#else
        ChangeWindowMessageFilter(wxMsgTaskbarButtonCreated, wxMSGFLT_ADD);
        ChangeWindowMessageFilter(WM_COMMAND, wxMSGFLT_ADD);
#endif // wxUSE_DYNLIB_CLASS
    }
#endif // wxUSE_TASKBARBUTTON

    return true;
}

wxFrame::~wxFrame()
{
#if wxUSE_TASKBARBUTTON
    delete m_taskBarButton;
#endif
}

// ----------------------------------------------------------------------------
// wxFrame client size calculations
// ----------------------------------------------------------------------------

void wxFrame::DoSetClientSize(int width, int height)
{
    // leave enough space for the status bar if we have (and show) it
#if wxUSE_STATUSBAR
    wxStatusBar *statbar = GetStatusBar();
    if ( statbar && statbar->IsShown() )
    {
        height += statbar->GetSize().y;
    }
#endif // wxUSE_STATUSBAR

    // call GetClientAreaOrigin() to take the toolbar into account
    wxPoint pt = GetClientAreaOrigin();
    width += pt.x;
    height += pt.y;

#if wxUSE_TOOLBAR
    wxToolBar * const toolbar = GetToolBar();
    if ( toolbar )
    {
        if ( toolbar->HasFlag(wxTB_RIGHT | wxTB_BOTTOM) )
        {
            const wxSize sizeTB = toolbar->GetSize();
            if ( toolbar->HasFlag(wxTB_RIGHT) )
                width -= sizeTB.x;
            else // wxTB_BOTTOM
                height -= sizeTB.y;
        }
        //else: toolbar already taken into account by GetClientAreaOrigin()
    }
#endif // wxUSE_TOOLBAR

    wxTopLevelWindow::DoSetClientSize(width, height);
}

// Get size *available for subwindows* i.e. excluding menu bar, toolbar etc.
void wxFrame::DoGetClientSize(int *x, int *y) const
{
    wxTopLevelWindow::DoGetClientSize(x, y);

    // account for the possible toolbar
    wxPoint pt = GetClientAreaOrigin();
    if ( x )
        *x -= pt.x;

    if ( y )
        *y -= pt.y;

#if wxUSE_TOOLBAR
    wxToolBar * const toolbar = GetToolBar();
    if ( toolbar )
    {
        if ( toolbar->HasFlag(wxTB_RIGHT | wxTB_BOTTOM) )
        {
            const wxSize sizeTB = toolbar->GetSize();
            if ( toolbar->HasFlag(wxTB_RIGHT) )
            {
                if ( x )
                    *x -= sizeTB.x;
            }
            else // wxTB_BOTTOM
            {
                if ( y )
                    *y -= sizeTB.y;
            }
        }
        //else: toolbar already taken into account by GetClientAreaOrigin()
    }
#endif // wxUSE_TOOLBAR

#if wxUSE_STATUSBAR
    // adjust client area height to take the status bar into account
    if ( y )
    {
        wxStatusBar *statbar = GetStatusBar();
        if ( statbar && statbar->IsShown() )
        {
            *y -= statbar->GetSize().y;
        }
    }
#endif // wxUSE_STATUSBAR
}

// ----------------------------------------------------------------------------
// wxFrame: various geometry-related functions
// ----------------------------------------------------------------------------

// generate an artificial resize event
void wxFrame::SendSizeEvent(int flags)
{
    if ( !MSWIsIconized() )
    {
        RECT r = wxGetWindowRect(GetHwnd());

        if ( flags & wxSEND_EVENT_POST )
        {
            ::PostMessage(GetHwnd(), WM_SIZE,
                          IsMaximized() ? SIZE_MAXIMIZED : SIZE_RESTORED,
                          MAKELPARAM(r.right - r.left, r.bottom - r.top));
        }
        else // send it
        {
            ::SendMessage(GetHwnd(), WM_SIZE,
                          IsMaximized() ? SIZE_MAXIMIZED : SIZE_RESTORED,
                          MAKELPARAM(r.right - r.left, r.bottom - r.top));
        }
    }
}

#if wxUSE_STATUSBAR
wxStatusBar *wxFrame::OnCreateStatusBar(int number,
                                        long style,
                                        wxWindowID id,
                                        const wxString& name)
{
    wxStatusBar *statusBar wxDUMMY_INITIALIZE(NULL);

#if wxUSE_NATIVE_STATUSBAR
    if ( !UsesNativeStatusBar() )
    {
        statusBar = (wxStatusBar *)new wxStatusBarGeneric(this, id, style);
    }
    else
#endif
    {
        statusBar = new wxStatusBar(this, id, style, name);
    }

    statusBar->SetFieldsCount(number);

    return statusBar;
}

void wxFrame::PositionStatusBar()
{
    if ( !m_frameStatusBar || !m_frameStatusBar->IsShown() )
        return;

    int w, h;
    GetClientSize(&w, &h);

    int x = 0;
#if wxUSE_TOOLBAR
    wxToolBar * const toolbar = GetToolBar();
    if ( toolbar )
    {
        const wxSize sizeTB = toolbar->GetSize();
        const int directionTB = toolbar->GetDirection();

        if ( toolbar->IsVertical() )
        {
            if ( directionTB == wxTB_LEFT )
                x -= sizeTB.x;

            w += sizeTB.x;
        }
        else if ( directionTB == wxTB_BOTTOM )
        {
            // we need to position the status bar below the toolbar
            h += sizeTB.y;
        }
        //else: no adjustments necessary for the toolbar on top
    }
#endif // wxUSE_TOOLBAR

    // GetSize returns the height of the clientSize in which the statusbar
    // height is subtracted (see wxFrame::DoGetClientSize). When the DPI of the
    // window changes, the statusbar height will likely change so we need to
    // account for this difference. If not, the statusbar will be positioned
    // too high or low.
    int shOld;
    m_frameStatusBar->GetSize(NULL, &shOld);

    // Resize the status bar to its default height, as it could have been set
    // to a wrong value before by WM_SIZE sent during the frame creation and
    // our status bars preserve their programmatically set size to avoid being
    // resized by DefWindowProc() to the full window width, so if we didn't do
    // this here, the status bar would retain the possibly wrong current height.
    m_frameStatusBar->SetSize(x, h, w, wxDefaultCoord, wxSIZE_AUTO_HEIGHT);

    int sh;
    m_frameStatusBar->GetSize(NULL, &sh);
    h += shOld - sh;

    // Since we wish the status bar to be directly under the client area,
    // we use the adjusted sizes without using wxSIZE_NO_ADJUSTMENTS.
    m_frameStatusBar->SetSize(x, h, w, sh);
}

#endif // wxUSE_STATUSBAR

#if wxUSE_MENUS_NATIVE

void wxFrame::AttachMenuBar(wxMenuBar *menubar)
{
    wxFrameBase::AttachMenuBar(menubar);

    if ( !menubar )
    {
        // actually remove the menu from the frame
        m_hMenu = (WXHMENU)0;
        InternalSetMenuBar();
    }
    else // set new non NULL menu bar
    {
        // Can set a menubar several times.
        if ( menubar->GetHMenu() )
        {
            m_hMenu = menubar->GetHMenu();
        }
        else // no HMENU yet
        {
            m_hMenu = menubar->Create();

            if ( !m_hMenu )
            {
                wxFAIL_MSG( wxT("failed to create menu bar") );
                return;
            }
        }
        InternalSetMenuBar();
    }
}

void wxFrame::InternalSetMenuBar()
{
    if ( !::SetMenu(GetHwnd(), (HMENU)m_hMenu) )
    {
        wxLogLastError(wxT("SetMenu"));
    }
}

#endif // wxUSE_MENUS_NATIVE

#if wxUSE_MENUS && !defined(__WXUNIVERSAL__)
bool wxFrame::HandleMenuSelect(WXWORD nItem, WXWORD flags, WXHMENU hMenu)
{
    // Unfortunately we need to ignore a message which is sent after
    // closing the currently active submenu of the menu bar by pressing Escape:
    // in this case we get WM_UNINITMENUPOPUP, from which we generate
    // wxEVT_MENU_CLOSE, and _then_ we get WM_MENUSELECT for the top level menu
    // from which we overwrite the help string just restored by OnMenuClose()
    // handler in wxFrameBase. To prevent this from happening we discard these
    // messages but only in the case it's really the top level menu as we still
    // need to clear the help string when a submenu is selected in a menu.
    if ( flags == (MF_POPUP | MF_HILITE) && !m_menuDepth )
        return false;

    return wxWindow::HandleMenuSelect(nItem, flags, hMenu);
}

bool wxFrame::DoSendMenuOpenCloseEvent(wxEventType evtType, wxMenu* menu)
{
    // Update the menu depth when dealing with the top level menus.
    if ( !menu || menu->IsAttached() )
    {
        if ( evtType == wxEVT_MENU_OPEN )
        {
            m_menuDepth++;
        }
        else if ( evtType == wxEVT_MENU_CLOSE )
        {
            wxASSERT_MSG( m_menuDepth > 0, wxS("No open menus?") );

            m_menuDepth--;
        }
        else
        {
            wxFAIL_MSG( wxS("Unexpected menu event type") );
        }
    }

    return wxWindow::DoSendMenuOpenCloseEvent(evtType, menu);
}

wxMenu* wxFrame::MSWFindMenuFromHMENU(WXHMENU hMenu)
{
    if ( wxMenuBar* mbar = GetMenuBar() )
    {
        if ( wxMenu* menu = mbar->MSWGetMenu(hMenu) )
            return menu;
    }

    return wxFrameBase::MSWFindMenuFromHMENU(hMenu);
}
#endif // wxUSE_MENUS && !defined(__WXUNIVERSAL__)

#if wxUSE_TASKBARBUTTON
wxTaskBarButton* wxFrame::MSWGetTaskBarButton()
{
    if ( !m_taskBarButton )
        m_taskBarButton = wxTaskBarButton::New(this);

    return m_taskBarButton;
}
#endif // wxUSE_TASKBARBUTTON

// Responds to colour changes, and passes event on to children.
void wxFrame::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    // Don't override the colour explicitly set by the user, if any.
    if ( !UseBgCol() )
    {
        SetOwnBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
        Refresh();
    }

#if wxUSE_STATUSBAR
    if ( m_frameStatusBar )
    {
        wxSysColourChangedEvent event2;
        event2.SetEventObject( m_frameStatusBar );
        m_frameStatusBar->HandleWindowEvent(event2);
    }
#endif // wxUSE_STATUSBAR

    // Propagate the event to the non-top-level children
    wxWindow::OnSysColourChanged(event);
}

// Pass true to show full screen, false to restore.
bool wxFrame::ShowFullScreen(bool show, long style)
{
    if ( IsFullScreen() == show )
        return false;

    if (show)
    {
        // zap the toolbar, menubar, and statusbar if needed
#if wxUSE_TOOLBAR
        wxToolBar *theToolBar = GetToolBar();

        if ((style & wxFULLSCREEN_NOTOOLBAR) && theToolBar)
        {
            if ( theToolBar->IsShown() )
            {
                theToolBar->SetSize(wxDefaultCoord,0);
                theToolBar->Show(false);
            }
            else // prevent it from being restored later
            {
                style &= ~wxFULLSCREEN_NOTOOLBAR;
            }
        }
#endif // wxUSE_TOOLBAR

        if (style & wxFULLSCREEN_NOMENUBAR)
            SetMenu((HWND)GetHWND(), (HMENU) NULL);

#if wxUSE_STATUSBAR
        wxStatusBar *theStatusBar = GetStatusBar();

        // Save the number of fields in the statusbar
        if ((style & wxFULLSCREEN_NOSTATUSBAR) && theStatusBar)
        {
            if ( theStatusBar->IsShown() )
                theStatusBar->Show(false);
            else
                style &= ~wxFULLSCREEN_NOSTATUSBAR;
        }
#endif // wxUSE_STATUSBAR
    }
    else // restore to normal
    {
        // restore the toolbar, menubar, and statusbar if we had hid them
#if wxUSE_TOOLBAR
        wxToolBar *theToolBar = GetToolBar();

        if ((m_fsStyle & wxFULLSCREEN_NOTOOLBAR) && theToolBar)
        {
            theToolBar->Show(true);
        }
#endif // wxUSE_TOOLBAR

#if wxUSE_MENUS
        if (m_fsStyle & wxFULLSCREEN_NOMENUBAR)
        {
            const WXHMENU hmenu = MSWGetActiveMenu();
            if ( hmenu )
                ::SetMenu(GetHwnd(), (HMENU)hmenu);
        }
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
        wxStatusBar *theStatusBar = GetStatusBar();

        if ((m_fsStyle & wxFULLSCREEN_NOSTATUSBAR) && theStatusBar)
        {
            theStatusBar->Show(true);
            PositionStatusBar();
        }
#endif // wxUSE_STATUSBAR
    }

    return wxFrameBase::ShowFullScreen(show, style);
}

// ----------------------------------------------------------------------------
// tool/status bar stuff
// ----------------------------------------------------------------------------

#if wxUSE_TOOLBAR

wxToolBar* wxFrame::CreateToolBar(long style, wxWindowID id, const wxString& name)
{
    if ( wxFrameBase::CreateToolBar(style, id, name) )
    {
        PositionToolBar();
    }

    return m_frameToolBar;
}

void wxFrame::PositionToolBar()
{
    wxToolBar *toolbar = GetToolBar();
    if ( toolbar && toolbar->IsShown() )
    {
        // don't call our (or even wxTopLevelWindow) version because we want
        // the real (full) client area size, not excluding the tool/status bar
        int width, height;
        wxWindow::DoGetClientSize(&width, &height);

#if wxUSE_STATUSBAR
        wxStatusBar *statbar = GetStatusBar();
        if ( statbar && statbar->IsShown() )
        {
            height -= statbar->GetClientSize().y;
        }
#endif // wxUSE_STATUSBAR

        int tx, ty, tw, th;
        toolbar->GetPosition( &tx, &ty );
        toolbar->GetBestSize( &tw, &th );

        int x, y;
        if ( toolbar->HasFlag(wxTB_BOTTOM) )
        {
            x = 0;
            y = height - th;
        }
        else if ( toolbar->HasFlag(wxTB_RIGHT) )
        {
            x = width - tw;
            y = 0;
        }
        else // left or top
        {
            x = 0;
            y = 0;
        }

        if ( toolbar->HasFlag(wxTB_BOTTOM) )
        {
            if ( ty < 0 && ( -ty == th ) )
                ty = height - th;
            if ( tx < 0 && (-tx == tw ) )
                tx = 0;
        }
        else if ( toolbar->HasFlag(wxTB_RIGHT) )
        {
            if( ty < 0 && ( -ty == th ) )
                ty = 0;
            if( tx < 0 && ( -tx == tw ) )
                tx = width - tw;
        }
        else // left or top
        {
            if (ty < 0 && (-ty == th))
                ty = 0;
            if (tx < 0 && (-tx == tw))
                tx = 0;
        }

        int desiredW,
            desiredH;

        if ( toolbar->IsVertical() )
        {
            desiredW = tw;
            desiredH = height;
        }
        else
        {
            desiredW = width;
            desiredH = th;
        }

        // use the 'real' MSW position here, don't offset relatively to the
        // client area origin
        toolbar->SetSize(x, y, desiredW, desiredH, wxSIZE_NO_ADJUSTMENTS);

    }
}

#endif // wxUSE_TOOLBAR

// ----------------------------------------------------------------------------
// frame state (iconized/maximized/...)
// ----------------------------------------------------------------------------

// propagate our state change to all child frames: this allows us to emulate X
// Windows behaviour where child frames float independently of the parent one
// on the desktop, but are iconized/restored with it
void wxFrame::IconizeChildFrames(bool bIconize)
{
    m_showCmd = bIconize ? SW_MINIMIZE : SW_RESTORE;

    for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow *win = node->GetData();

        // iconizing the frames with this style puts them at the bottom of
        // the screen (as the MDI children) instead of making them appear
        // in the taskbar because they are, by virtue of this style, not
        // managed by the taskbar - instead leave Windows take care of them
        if ( win->GetWindowStyle() & wxFRAME_TOOL_WINDOW )
            continue;

        // the child MDI frames are a special case and should not be touched by
        // the parent frame - instead, they are managed by the user
        wxFrame *frame = wxDynamicCast(win, wxFrame);
        if ( frame
#if wxUSE_MDI_ARCHITECTURE
                && !frame->IsMDIChild()
#endif // wxUSE_MDI_ARCHITECTURE
           )
        {
            // we don't want to restore the child frames which had been
            // iconized even before we were iconized, so save the child frame
            // status when iconizing the parent frame and check it when
            // restoring it
            if ( bIconize )
            {
                frame->m_wasMinimized = frame->IsIconized();
            }

            // note that we shouldn't touch the hidden frames neither because
            // iconizing/restoring them would show them as a side effect
            if ( !frame->m_wasMinimized && frame->IsShown() )
                frame->Iconize(bIconize);
        }
    }
}

WXHICON wxFrame::GetDefaultIcon() const
{
    // we don't have any standard icons (any more)
    return (WXHICON)0;
}

// ===========================================================================
// message processing
// ===========================================================================

// ---------------------------------------------------------------------------
// preprocessing
// ---------------------------------------------------------------------------

bool wxFrame::MSWDoTranslateMessage(wxFrame *frame, WXMSG *pMsg)
{
    if ( wxWindow::MSWTranslateMessage(pMsg) )
        return true;

#if wxUSE_MENUS && wxUSE_ACCEL && !defined(__WXUNIVERSAL__)
    // try the menu bar accelerators
    wxMenuBar *menuBar = GetMenuBar();
    if ( menuBar && menuBar->GetAcceleratorTable()->Translate(frame, pMsg) )
        return true;
#else
    wxUnusedVar(frame);
#endif // wxUSE_MENUS && wxUSE_ACCEL

    return false;
}

// ---------------------------------------------------------------------------
// our private (non virtual) message handlers
// ---------------------------------------------------------------------------

bool wxFrame::HandleSize(int WXUNUSED(x), int WXUNUSED(y), WXUINT id)
{
    // We can get a WM_SIZE when restoring a hidden window using
    // SetWindowPlacement(), don't do anything here in this case as our state
    // will be really updated later, when (and if) we're shown. Still let the
    // base class generate wxEVT_SIZE and perform the layout, however.
    if ( !IsShown() )
        return false;

    switch ( id )
    {
        case SIZE_RESTORED:
        case SIZE_MAXIMIZED:
            // only do it it if we were iconized before, otherwise resizing the
            // parent frame has a curious side effect of bringing it under it's
            // children
            if ( m_showCmd != SW_MINIMIZE )
                break;

            // restore all child frames too
            IconizeChildFrames(false);

            (void)SendIconizeEvent(false);
            break;

        case SIZE_MINIMIZED:
            // iconize all child frames too
            IconizeChildFrames(true);
            break;
    }

    if ( m_showCmd != SW_MINIMIZE )
    {
#if wxUSE_STATUSBAR
        PositionStatusBar();
#endif // wxUSE_STATUSBAR

#if wxUSE_TOOLBAR
        PositionToolBar();
#endif // wxUSE_TOOLBAR

    }

    // call the base class version to generate the appropriate events
    return false;
}

bool wxFrame::HandleCommand(WXWORD id, WXWORD cmd, WXHWND control)
{
#if wxUSE_MENUS

    // we only need to handle the menu and accelerator commands from the items
    // of our menu bar, base wxWindow class already handles the rest
    if ( !control && (cmd == 0 /* menu */ || cmd == 1 /* accel */) )
    {
#if wxUSE_MENUS_NATIVE
        if ( !wxCurrentPopupMenu )
#endif // wxUSE_MENUS_NATIVE
        {
            wxMenuItem * const mitem = FindItemInMenuBar((signed short)id);
            if ( mitem )
                return ProcessCommand(mitem);
        }
    }
#endif // wxUSE_MENUS

#if wxUSE_TASKBARBUTTON
    if ( cmd == wxTHBN_CLICKED && m_taskBarButton )
    {
        wxTaskBarButtonImpl * const
            tbButton = reinterpret_cast<wxTaskBarButtonImpl*>(m_taskBarButton);
        // we use the index as id when adding thumbnail toolbar button.
        wxThumbBarButton * const
            thumbBarButton = tbButton->GetThumbBarButtonByIndex(id);
        wxCommandEvent event(wxEVT_BUTTON, thumbBarButton->GetID());
        event.SetEventObject(thumbBarButton);
        return ProcessEvent(event);
    }
#endif // wxUSE_TASKBARBUTTON

    return wxFrameBase::HandleCommand(id, cmd, control);
}

// ---------------------------------------------------------------------------
// the window proc for wxFrame
// ---------------------------------------------------------------------------

WXLRESULT wxFrame::MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    WXLRESULT rc = 0;
    bool processed = false;

    switch ( message )
    {
        case WM_CLOSE:
            // if we can't close, tell the system that we processed the
            // message - otherwise it would close us
            processed = !Close();
            break;

        case WM_SIZE:
            processed = HandleSize(LOWORD(lParam), HIWORD(lParam), wParam);
            break;

        case WM_COMMAND:
            {
                WORD id, cmd;
                WXHWND hwnd;
                UnpackCommand((WXWPARAM)wParam, (WXLPARAM)lParam,
                              &id, &hwnd, &cmd);

                HandleCommand(id, cmd, (WXHWND)hwnd);

                // don't pass WM_COMMAND to the base class whether we processed
                // it or not because we did generate an event for it (our
                // HandleCommand() calls the base class version) and we must
                // not do it again or the handlers which skip the event would
                // be called twice
                processed = true;
            }
            break;

        case WM_INITMENUPOPUP:
        case WM_UNINITMENUPOPUP:
            // We get these messages from the menu bar even if the menu is
            // disabled, which is unexpected, so ignore them in this case.
            if ( wxMenuBar* mbar = GetMenuBar() )
            {
                const int pos = mbar->MSWGetTopMenuPos((WXHMENU)wParam);
                if ( pos != wxNOT_FOUND && !mbar->IsEnabledTop(pos) )
                {
                    // This event comes from a disabled top level menu, don't
                    // handle it.
                    return MSWDefWindowProc(message, wParam, lParam);
                }
            }
            break;

        case WM_QUERYDRAGICON:
            {
                const wxIcon& icon = GetIcon();
                HICON hIcon = icon.IsOk() ? GetHiconOf(icon)
                                        : (HICON)GetDefaultIcon();
                rc = (WXLRESULT)hIcon;
                processed = rc != 0;
            }
            break;
    }
#if wxUSE_TASKBARBUTTON
    if ( message == wxMsgTaskbarButtonCreated )
    {
        if ( m_taskBarButton )
            m_taskBarButton->Realize();

        processed = true;
    }
#endif

    if ( !processed )
        rc = wxFrameBase::MSWWindowProc(message, wParam, lParam);

    return rc;
}

// ----------------------------------------------------------------------------
// wxFrame size management: we exclude the areas taken by menu/status/toolbars
// from the client area, so the client area is what's really available for the
// frame contents
// ----------------------------------------------------------------------------

// get the origin of the client area in the client coordinates
wxPoint wxFrame::GetClientAreaOrigin() const
{
    wxPoint pt = wxTopLevelWindow::GetClientAreaOrigin();

#if wxUSE_TOOLBAR && !defined(__WXUNIVERSAL__)
    wxToolBar * const toolbar = GetToolBar();
    if ( toolbar && toolbar->IsShown() )
    {
        const wxSize sizeTB = toolbar->GetSize();
        const int directionTB = toolbar->GetDirection();

        if ( directionTB == wxTB_TOP )
        {
            pt.y += sizeTB.y;
        }
        else if ( directionTB == wxTB_LEFT )
        {
            pt.x += sizeTB.x;
        }
    }
#endif // wxUSE_TOOLBAR

    return pt;
}
