///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/toplevel.cpp
// Purpose:     implements wxTopLevelWindow for MSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     24.09.01
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/toplevel.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dialog.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/containr.h"        // wxSetFocusToChild()
    #include "wx/module.h"
#endif //WX_PRECOMP

#include "wx/dynlib.h"
#include "wx/scopeguard.h"
#include "wx/tooltip.h"

#include "wx/msw/private.h"
#include "wx/msw/private/darkmode.h"
#include "wx/msw/private/winstyle.h"

#include "wx/msw/winundef.h"
#include "wx/msw/missing.h"

#include "wx/display.h"

// NB: wxDlgProc must be defined here and not in dialog.cpp because the latter
//     is not included by wxUniv build which does need wxDlgProc
INT_PTR APIENTRY
wxDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// ----------------------------------------------------------------------------
// wxTLWHiddenParentModule: used to manage the hidden parent window (we need a
// module to ensure that the window is always deleted)
// ----------------------------------------------------------------------------

class wxTLWHiddenParentModule : public wxModule
{
public:
    // module init/finalize
    virtual bool OnInit() override;
    virtual void OnExit() override;

    // get the hidden window (creates on demand)
    static HWND GetHWND();

private:
    // the HWND of the hidden parent
    static HWND ms_hwnd;

    // the class used to create it
    static const wxChar *ms_className;

    wxDECLARE_DYNAMIC_CLASS(wxTLWHiddenParentModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxTLWHiddenParentModule, wxModule);

// ============================================================================
// wxTopLevelWindowMSW implementation
// ============================================================================

wxBEGIN_EVENT_TABLE(wxTopLevelWindowMSW, wxTopLevelWindowBase)
    EVT_ACTIVATE(wxTopLevelWindowMSW::OnActivate)
wxEND_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW creation
// ----------------------------------------------------------------------------

void wxTopLevelWindowMSW::Init()
{
    m_showCmd = SW_SHOWNORMAL;

    // Data to save/restore when calling ShowFullScreen
    m_fsStyle = 0;
    m_fsOldWindowStyle = 0;
    m_fsIsMaximized = false;
    m_fsIsShowing = false;

    m_menuSystem = nullptr;
}

WXDWORD wxTopLevelWindowMSW::MSWGetStyle(long style, WXDWORD *exflags) const
{
    // let the base class deal with the common styles but fix the ones which
    // don't make sense for us (we also deal with the borders ourselves)
    WXDWORD msflags = wxWindow::MSWGetStyle
                      (
                        (style & ~wxBORDER_MASK) | wxBORDER_NONE, exflags
                      ) & ~WS_CHILD & ~WS_VISIBLE;

    // first select the kind of window being created
    //
    // note that if we don't set WS_POPUP, Windows assumes WS_OVERLAPPED and
    // creates a window with both caption and border, hence we need to use
    // WS_POPUP in a few cases just to avoid having caption/border which we
    // don't want

    // border and caption styles
    if ( ( style & wxRESIZE_BORDER ) && !IsAlwaysMaximized())
        msflags |= WS_THICKFRAME;
    else if ( exflags && ((style & wxBORDER_DOUBLE) || (style & wxBORDER_RAISED)) )
        *exflags |= WS_EX_DLGMODALFRAME;
    else if ( !(style & wxBORDER_NONE) )
        msflags |= WS_BORDER;
    else
        msflags |= WS_POPUP;

    // We need to use WS_CAPTION to show any of the minimize/maximize/close
    // buttons, so enable it if any of these styles is specified.
    if ( style & (wxCAPTION | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX) )
        msflags |= WS_CAPTION;
    else
        msflags |= WS_POPUP;

    // next translate the individual flags

    // WS_EX_CONTEXTHELP is incompatible with WS_MINIMIZEBOX and WS_MAXIMIZEBOX
    // and is ignored if we specify both of them, but chances are that if we
    // use wxWS_EX_CONTEXTHELP, we really do want to have the context help
    // button while wxMINIMIZE/wxMAXIMIZE are included by default, so the help
    // takes precedence
    if ( !(GetExtraStyle() & wxWS_EX_CONTEXTHELP) )
    {
        if ( style & wxMINIMIZE_BOX )
            msflags |= WS_MINIMIZEBOX;
        if ( style & wxMAXIMIZE_BOX )
            msflags |= WS_MAXIMIZEBOX;
    }

    // notice that if wxCLOSE_BOX is specified we need to use WS_SYSMENU too as
    // otherwise the close box doesn't appear
    if ( style & (wxSYSTEM_MENU | wxCLOSE_BOX) )
        msflags |= WS_SYSMENU;

    if ( style & wxMINIMIZE )
        msflags |= WS_MINIMIZE;

    if ( style & wxMAXIMIZE )
        msflags |= WS_MAXIMIZE;

    // Keep this here because it saves recoding this function in wxTinyFrame
    if ( style & wxTINY_CAPTION )
        msflags |= WS_CAPTION;

    if ( exflags )
    {
        if ( !(GetExtraStyle() & wxTOPLEVEL_EX_DIALOG) )
        {
            if ( style & wxFRAME_TOOL_WINDOW )
            {
                // create the palette-like window
                *exflags |= WS_EX_TOOLWINDOW;

                // tool windows shouldn't appear on the taskbar (as documented)
                style |= wxFRAME_NO_TASKBAR;
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

        if ( GetExtraStyle() & wxWS_EX_CONTEXTHELP )
            *exflags |= WS_EX_CONTEXTHELP;

        if ( style & wxSTAY_ON_TOP )
            *exflags |= WS_EX_TOPMOST;
    }

    return msflags;
}

WXHWND wxTopLevelWindowMSW::MSWGetParent() const
{
    // for the frames without wxFRAME_FLOAT_ON_PARENT style we should use null
    // parent HWND or it would be always on top of its parent which is not what
    // we usually want (in fact, we only want it for frames with the
    // wxFRAME_FLOAT_ON_PARENT flag)
    HWND hwndParent = nullptr;
    if ( HasFlag(wxFRAME_FLOAT_ON_PARENT) )
    {
        const wxWindow *parent = GetParent();

        if ( !parent )
        {
            // this flag doesn't make sense then and will be ignored
            wxFAIL_MSG( wxT("wxFRAME_FLOAT_ON_PARENT but no parent?") );
        }
        else
        {
            hwndParent = GetHwndOf(parent);
        }
    }
    //else: don't float on parent, must not be owned

    // now deal with the 2nd taskbar-related problem (see comments above in
    // MSWGetStyle())
    if ( HasFlag(wxFRAME_NO_TASKBAR) && !hwndParent )
    {
        // use hidden parent
        hwndParent = wxTLWHiddenParentModule::GetHWND();
    }

    return (WXHWND)hwndParent;
}

WXLRESULT wxTopLevelWindowMSW::MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    WXLRESULT rc = 0;
    bool processed = false;

    switch ( message )
    {
        case WM_SYSCOMMAND:
            {
                // From MSDN:
                //
                //      ... the four low-order bits of the wParam parameter are
                //      used internally by the system. To obtain the correct
                //      result when testing the value of wParam, an application
                //      must combine the value 0xFFF0 with the wParam value by
                //      using the bitwise AND operator.
                unsigned id = wParam & 0xfff0;

                // Preserve the focus when minimizing/restoring the window:
                // surprisingly, DefWindowProc() doesn't do it automatically
                // and so we need to it ourselves. Moreover, our code in
                // OnActivate() doesn't work in this case as we receive the
                // deactivation event too late when the window is being
                // minimized and the focus is already null by then. Similarly,
                // we receive the activation event too early and restoring
                // focus in it fails because the window is still minimized. So
                // we need to do it here.
                if ( id == SC_MINIMIZE )
                {
                    // For minimization, it's simple enough: just save the
                    // focus as usual. The important thing is that we're not
                    // minimized yet, so this works correctly.
                    DoSaveLastFocus();
                }
                else if ( id == SC_RESTORE )
                {
                    // For restoring, it's trickier as DefWindowProc() sets
                    // focus to the window itself. So run it first and restore
                    // our saved focus only afterwards.
                    processed = true;
                    rc = wxTopLevelWindowBase::MSWWindowProc(message,
                                                             wParam, lParam);

                    DoRestoreLastFocus();
                }
                else if ( id == SC_KEYMENU )
                {
                    // Alt-Backspace is understood as an accelerator for "Undo"
                    // by the native EDIT control, but pressing it results in a
                    // beep by default when the resulting SC_KEYMENU is handled
                    // by DefWindowProc(), so pretend to handle it ourselves if
                    // we're editing a text control to avoid the annoying beep.
                    if ( lParam == VK_BACK )
                    {
                        if ( wxWindow* const focus = FindFocus() )
                        {
                            if ( focus->WXGetTextEntry() )
                                processed = true;
                        }
                    }
                }

#ifndef __WXUNIVERSAL__
                // We need to generate events for the custom items added to the
                // system menu if it had been created (and presumably modified).
                // As SC_SIZE is the first of the system-defined commands, we
                // only do this for the custom commands before it and leave
                // SC_SIZE and everything after it to DefWindowProc().
                if ( m_menuSystem && id < SC_SIZE )
                {
                    if ( m_menuSystem->MSWCommand(0 /* unused anyhow */, id) )
                        processed = true;
                }
#endif // #ifndef __WXUNIVERSAL__
            }
            break;
    }

    if ( !processed )
        rc = wxTopLevelWindowBase::MSWWindowProc(message, wParam, lParam);

    return rc;
}

void
wxTopLevelWindowMSW::MSWUpdateFontOnDPIChange(const wxSize& WXUNUSED(newDPI))
{
    if ( !m_icons.IsEmpty() )
        DoSetIcons();
}

bool wxTopLevelWindowMSW::CreateDialog(const void *dlgTemplate,
                                       const wxString& title,
                                       const wxPoint& pos,
                                       const wxSize& size)
{
    // static cast is valid as we're only ever called for dialogs
    wxWindow * const
        parent = static_cast<wxDialog *>(this)->GetParentForModalDialog();

    m_hWnd = (WXHWND)::CreateDialogIndirect
                       (
                        wxGetInstance(),
                        static_cast<const DLGTEMPLATE*>(dlgTemplate),
                        parent ? GetHwndOf(parent) : nullptr,
                        (DLGPROC)wxDlgProc
                       );

    if ( !m_hWnd )
    {
        wxFAIL_MSG(wxT("Failed to create dialog. Incorrect DLGTEMPLATE?"));

        wxLogSysError(wxT("Can't create dialog using memory template"));

        return false;
    }

    // For some reason, the system menu is activated when we use the
    // WS_EX_CONTEXTHELP style, so let's set a reasonable icon
    if ( HasExtraStyle(wxWS_EX_CONTEXTHELP) )
    {
        wxFrame *winTop = wxDynamicCast(wxTheApp->GetTopWindow(), wxFrame);
        if ( winTop )
        {
            wxIcon icon = winTop->GetIcon();
            if ( icon.IsOk() )
            {
                ::SendMessage(GetHwnd(), WM_SETICON,
                              (WPARAM)TRUE,
                              (LPARAM)GetHiconOf(icon));
            }
        }
    }

    if ( !title.empty() )
    {
        ::SetWindowText(GetHwnd(), title.t_str());
    }

    SubclassWin(m_hWnd);

    // move the dialog to its initial position without forcing repainting
    int x, y, w, h;
    (void)MSWGetCreateWindowCoords(pos, size, x, y, w, h);

    if ( x == (int)CW_USEDEFAULT )
    {
        // Let the system position the window, just set its size.
        ::SetWindowPos(GetHwnd(), 0,
                       0, 0, w, h,
                       SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    else // Move the window to the desired location and set its size too.
    {
        if ( !::MoveWindow(GetHwnd(), x, y, w, h, FALSE) )
        {
            wxLogLastError(wxT("MoveWindow"));
        }
    }

    return true;
}

bool wxTopLevelWindowMSW::CreateFrame(const wxString& title,
                                      const wxPoint& pos,
                                      const wxSize& size)
{
    WXDWORD exflags;
    WXDWORD flags = MSWGetCreateWindowFlags(&exflags);

    const wxSize sz = IsAlwaysMaximized() ? wxDefaultSize : size;

    if ( wxApp::MSWGetDefaultLayout(m_parent) == wxLayout_RightToLeft )
        exflags |= WS_EX_LAYOUTRTL;

    return MSWCreate(GetMSWClassName(GetWindowStyle()), title.t_str(), pos, sz, flags, exflags);
}

bool wxTopLevelWindowMSW::Create(wxWindow *parent,
                                 wxWindowID id,
                                 const wxString& title,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
{
    wxSize sizeReal = size;
    if ( !sizeReal.IsFullySpecified() )
    {
        sizeReal.SetDefaults(GetDefaultSize());
    }

    // notice that we should append this window to wxTopLevelWindows list
    // before calling CreateBase() as it behaves differently for TLW and
    // non-TLW windows
    wxTopLevelWindows.Append(this);

    if ( !CreateBase(parent, id, pos, sizeReal, style, name) )
        return false;

    if ( parent )
        parent->AddChild(this);

    if ( GetExtraStyle() & wxTOPLEVEL_EX_DIALOG )
    {
        // we have different dialog templates to allows creation of dialogs
        // with & without captions under MSWindows, resizable or not (but a
        // resizable dialog always has caption - otherwise it would look too
        // strange)

        // we need 3 additional WORDs for dialog menu, class and title (as we
        // don't use DS_SETFONT we don't need the fourth WORD for the font)
        static const int dlgsize = sizeof(DLGTEMPLATE) + (sizeof(WORD) * 3);
        DLGTEMPLATE *dlgTemplate = (DLGTEMPLATE *)malloc(dlgsize);
        wxON_BLOCK_EXIT1(free, dlgTemplate);

        memset(dlgTemplate, 0, dlgsize);

        // these values are arbitrary, they won't be used normally anyhow
        const LONG baseUnits = ::GetDialogBaseUnits();
        dlgTemplate->x = 34;
        dlgTemplate->y  = 22;
        dlgTemplate->cx = ::MulDiv(sizeReal.x, 4, LOWORD(baseUnits));
        dlgTemplate->cy = ::MulDiv(sizeReal.y, 8, HIWORD(baseUnits));

        // reuse the code in MSWGetStyle() but correct the results slightly for
        // the dialog
        //
        // NB: we need a temporary variable as we can't pass pointer to
        //     dwExtendedStyle directly, it's not aligned correctly for 64 bit
        //     architectures
        WXDWORD dwExtendedStyle;
        dlgTemplate->style = MSWGetStyle(style, &dwExtendedStyle);
        dlgTemplate->dwExtendedStyle = dwExtendedStyle;

        // all dialogs are popups
        dlgTemplate->style |= WS_POPUP;

        if ( wxApp::MSWGetDefaultLayout(m_parent) == wxLayout_RightToLeft )
        {
            dlgTemplate->dwExtendedStyle |= WS_EX_LAYOUTRTL;
        }

        // force 3D-look if necessary, it looks impossibly ugly otherwise
        if ( style & (wxRESIZE_BORDER | wxCAPTION) )
            dlgTemplate->style |= DS_MODALFRAME;

        if ( !CreateDialog(dlgTemplate, title, pos, sizeReal) )
            return false;
    }
    else // !dialog
    {
        if ( !CreateFrame(title, pos, sizeReal) )
            return false;
    }

    if ( !(GetWindowStyleFlag() & wxCLOSE_BOX) )
    {
        EnableCloseButton(false);
    }

    // for standard dialogs the dialog manager generates WM_CHANGEUISTATE
    // itself but for custom windows we have to do it ourselves in order to
    // make the keyboard indicators (such as underlines for accelerators and
    // focus rectangles) work under Win2k+
    MSWUpdateUIState(UIS_INITIALIZE);

    wxMSWDarkMode::EnableForTLW(GetHwnd());

    return true;
}

wxTopLevelWindowMSW::~wxTopLevelWindowMSW()
{
    delete m_menuSystem;

    SendDestroyEvent();
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW showing
// ----------------------------------------------------------------------------

void wxTopLevelWindowMSW::DoShowWindow(int nShowCmd)
{
    // After hiding or minimizing an owned window, Windows activates the next
    // top level window in Z order but it may be different from our owner (to
    // reproduce this simply Alt-TAB to another application and back before
    // closing the owned frame) whereas we always want to yield activation to
    // our parent, so do it explicitly _before_ yielding activation.
    switch ( nShowCmd )
    {
        case SW_HIDE:
        case SW_MINIMIZE:
            if ( HasFlag(wxFRAME_FLOAT_ON_PARENT) )
            {
                wxWindow *parent = GetParent();
                if ( parent )
                {
                    ::BringWindowToTop(GetHwndOf(parent));
                }
            }
            break;
    }

    ::ShowWindow(GetHwnd(), nShowCmd);

#if wxUSE_TOOLTIPS
    // Don't leave a tooltip hanging around if TLW is hidden now.
    wxToolTip::UpdateVisibility();
#endif // wxUSE_TOOLTIPS
}

void wxTopLevelWindowMSW::ShowWithoutActivating()
{
    if ( !wxWindowBase::Show(true) )
        return;

    // We can't show the window in a maximized state without activating it, so
    // the sequence of hiding the window, calling Maximize() and this function
    // will end up with the window not being maximized -- but this is arguably
    // better than activating it and is compatible with the historical
    // behaviour of this function.
    DoShowWindow(m_showCmd == SW_MINIMIZE ? SW_SHOWMINNOACTIVE : SW_SHOWNA);
}

bool wxTopLevelWindowMSW::Show(bool show)
{
    // don't use wxWindow version as we want to call DoShowWindow() ourselves
    if ( !wxWindowBase::Show(show) )
        return false;

    int nShowCmd;
    if ( show )
    {
        // If we need to minimize or maximize the window, do it now.
        if ( m_showCmd == SW_MAXIMIZE || m_showCmd == SW_MINIMIZE )
        {
            nShowCmd = m_showCmd;
        }
        else if ( ::IsIconic(GetHwnd()) )
        {
            // We were restored while we were hidden, so now we need to show
            // the window in its normal state.
            //
            // As below, don't activate some kinds of windows.
            if ( HasFlag(wxFRAME_TOOL_WINDOW) || !IsEnabled() )
                nShowCmd = SW_SHOWNOACTIVATE;
            else
                nShowCmd = SW_RESTORE;
        }
        else // just show
        {
            // we shouldn't use SW_SHOW which also activates the window for
            // tool frames (as they shouldn't steal focus from the main window)
            // nor for the currently disabled windows as they would be enabled
            // as a side effect
            if ( HasFlag(wxFRAME_TOOL_WINDOW) || !IsEnabled() )
                nShowCmd = SW_SHOWNA;
            else
                nShowCmd = SW_SHOW;
        }
    }
    else // hide
    {
        // When hiding the window, remember if it was maximized or iconized in
        // order to return the correct value from Is{Maximized,Iconized}().
        if ( ::IsZoomed(GetHwnd()) )
            m_showCmd = SW_MAXIMIZE;
        else if ( ::IsIconic(GetHwnd()) )
            m_showCmd = SW_MINIMIZE;
        else
            m_showCmd = SW_SHOW;

        nShowCmd = SW_HIDE;
    }

#if wxUSE_DEFERRED_SIZING
    // we only set pending size if we're maximized before being shown, now that
    // we're shown we don't need it any more (it is reset in size event handler
    // for child windows but we have to do it ourselves for this parent window)
    //
    // make sure to reset it before actually showing the window as this will
    // generate WM_SIZE events and we want to use the correct client size from
    // them, not the size returned by WM_NCCALCSIZE in DoGetClientSize() which
    // turns out to be wrong for maximized windows (see #11762)
    m_pendingSize = wxDefaultSize;
#endif // wxUSE_DEFERRED_SIZING

    DoShowWindow(nShowCmd);

    if ( show && nShowCmd == SW_MAXIMIZE )
    {
        // We don't receive WM_SHOWWINDOW when shown in the maximized state,
        // cf. https://docs.microsoft.com/en-us/windows/desktop/winmsg/wm-showwindow
        // and so we have to issue the event ourselves in this case.
        wxShowEvent event(GetId(), true);
        event.SetEventObject(this);
        AddPendingEvent(event);
    }

    return true;
}

void wxTopLevelWindowMSW::Raise()
{
    ::SetForegroundWindow(GetHwnd());
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW maximize/minimize
// ----------------------------------------------------------------------------

void wxTopLevelWindowMSW::Maximize(bool maximize)
{
    // Update m_showCmd to ensure that the window is maximized when it's shown
    // later even if it's currently hidden.
    m_showCmd = maximize ? SW_MAXIMIZE : SW_RESTORE;

    if ( IsShown() )
    {
        // just maximize it directly
        DoShowWindow(m_showCmd);
    }
    else // hidden
    {
        // we can't maximize the hidden frame because it shows it as well,
        // so don't do anything other than updating m_showCmd for now

#if wxUSE_DEFERRED_SIZING
        // after calling Maximize() the client code expects to get the frame
        // "real" size and doesn't want to know that, because of implementation
        // details, the frame isn't really maximized yet but will be only once
        // it's shown, so return our size as it will be then in this case
        if ( maximize )
        {
            // we must only change pending size here, and not call SetSize()
            // because otherwise Windows would think that this (full screen)
            // size is the natural size for the frame and so would use it when
            // the user clicks on "restore" title bar button instead of the
            // correct initial frame size
            //
            // NB: unfortunately we don't know which display we're on yet so we
            //     have to use the default one
            m_pendingSize = wxGetClientDisplayRect().GetSize();
        }
        //else: can't do anything in this case, we don't have the old size
#endif // wxUSE_DEFERRED_SIZING
    }
}

bool wxTopLevelWindowMSW::IsMaximized() const
{
    if ( IsAlwaysMaximized() )
        return true;

    // If the window is shown, just ask Windows if it's maximized. But hidden
    // windows are not really maximized, even after Maximize() is called on
    // them, so for them we check if we're scheduled to maximize the window
    // when it's shown instead.
    return IsShown() ? ::IsZoomed(GetHwnd()) != 0 : m_showCmd == SW_MAXIMIZE;
}

void wxTopLevelWindowMSW::Iconize(bool iconize)
{
    if ( iconize == MSWIsIconized() )
    {
        // Do nothing, in particular don't restore non-iconized windows when
        // Iconize(false) is called as this would wrongly un-maximize them.
        return;
    }

    // Note that we can't change m_showCmd yet as wxFrame WM_SIZE handler uses
    // its value to determine whether the frame had been iconized before or not
    // and this handler will be called from inside DoShowWindow() below.
    const UINT showCmd = iconize ? SW_MINIMIZE : SW_RESTORE;

    // We can't actually iconize the window if it's currently hidden, as this
    // would also show it unexpectedly.
    if ( IsShown() )
    {
        DoShowWindow(showCmd);
    }

    // Update the internal flag in any case, so that IsIconized() returns the
    // correct value, for example. And if the window is currently hidden, this
    // also ensures that the next call to Show() will show it in an iconized
    // state instead of showing it normally.
    m_showCmd = showCmd;
}

bool wxTopLevelWindowMSW::IsIconized() const
{
    if ( !IsShown() )
    {
        // Hidden windows are never actually iconized at MSW level, but can be
        // in wx, so use m_showCmd to determine our status.
        return m_showCmd == SW_MINIMIZE;
    }

    // don't use m_showCmd, it may be briefly out of sync with the real state
    // as it's only modified when we receive a WM_SIZE and we could be called
    // from an event handler from one of the messages we receive before it,
    // such as WM_MOVE
    return MSWIsIconized();
}

bool wxTopLevelWindowMSW::MSWIsIconized() const
{
    return ::IsIconic(GetHwnd()) != 0;
}

void wxTopLevelWindowMSW::Restore()
{
    // Forget any previous minimized/maximized status.
    m_showCmd = SW_SHOW;

    // And actually restore the window to its normal state. Note that here,
    // unlike in Maximize() and Iconize(), we do it even if the window is
    // currently hidden, i.e. Restore() is supposed to show it in this case.
    DoShowWindow(SW_RESTORE);
}

bool wxTopLevelWindowMSW::Destroy()
{
    if ( !wxTopLevelWindowBase::Destroy() )
        return false;

    // Under Windows 10 iconized windows don't get any messages, so delayed
    // destruction doesn't work for them if we don't force a message dispatch
    // here (and it doesn't seem useful to test for MSWIsIconized() as doing
    // this doesn't do any harm for non-iconized windows either). For that
    // matter, doing this shouldn't do any harm under previous OS versions
    // either, so checking for the OS version doesn't seem useful too.
    wxWakeUpIdle();

    return true;
}

void wxTopLevelWindowMSW::SetLayoutDirection(wxLayoutDirection dir)
{
    if ( dir == wxLayout_Default )
        dir = wxApp::MSWGetDefaultLayout(m_parent);

    if ( dir != wxLayout_Default )
        wxTopLevelWindowBase::SetLayoutDirection(dir);
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW geometry
// ----------------------------------------------------------------------------

void wxTopLevelWindowMSW::DoGetPosition(int *x, int *y) const
{
    if ( IsIconized() )
    {
        WINDOWPLACEMENT wp;
        wp.length = sizeof(WINDOWPLACEMENT);
        if ( ::GetWindowPlacement(GetHwnd(), &wp) )
        {
            RECT& rc = wp.rcNormalPosition;

            // the position returned by GetWindowPlacement() is in workspace
            // coordinates except for windows with WS_EX_TOOLWINDOW style
            if ( !HasFlag(wxFRAME_TOOL_WINDOW) )
            {
                // we must use the correct display for the translation as the
                // task bar might be shown on one display but not the other one
                wxDisplay dpy(this);
                const wxPoint ptOfs = dpy.GetClientArea().GetPosition() -
                                      dpy.GetGeometry().GetPosition();

                rc.left += ptOfs.x;
                rc.top += ptOfs.y;
            }

            if ( x )
                *x = rc.left;
            if ( y )
                *y = rc.top;

            return;
        }

        wxLogLastError(wxT("GetWindowPlacement"));
    }
    //else: normal case

    wxTopLevelWindowBase::DoGetPosition(x, y);
}

void wxTopLevelWindowMSW::DoGetSize(int *width, int *height) const
{
    if ( IsIconized() )
    {
        WINDOWPLACEMENT wp;
        wp.length = sizeof(WINDOWPLACEMENT);
        if ( ::GetWindowPlacement(GetHwnd(), &wp) )
        {
            const RECT& rc = wp.rcNormalPosition;

            if ( width )
                *width = rc.right - rc.left;
            if ( height )
                *height = rc.bottom - rc.top;

            return;
        }

        wxLogLastError(wxT("GetWindowPlacement"));
    }
    //else: normal case

    wxTopLevelWindowBase::DoGetSize(width, height);
}

void
wxTopLevelWindowMSW::MSWGetCreateWindowCoords(const wxPoint& pos,
                                              const wxSize& size,
                                              int& x, int& y,
                                              int& w, int& h) const
{
    // let the system position the window if no explicit position was specified
    if ( pos.x == wxDefaultCoord )
    {
        // if x is set to CW_USEDEFAULT, y parameter is ignored anyhow so we
        // can just as well set it to CW_USEDEFAULT as well
        x =
        y = CW_USEDEFAULT;
    }
    else
    {
        // OTOH, if x is not set to CW_USEDEFAULT, y shouldn't be set to it
        // either because it is not handled as a special value by Windows then
        // and so we have to choose some default value for it, even if a
        // completely arbitrary one
        static const int DEFAULT_Y = 200;

        x = pos.x;
        y = pos.y == wxDefaultCoord ? DEFAULT_Y : pos.y;
    }

    if ( size.x == wxDefaultCoord || size.y == wxDefaultCoord )
    {
        // We don't use CW_USEDEFAULT here for several reasons:
        //
        //  1. It results in huge frames on modern screens (1000*800 is not
        //     uncommon on my 1280*1024 screen) which is way too big for a half
        //     empty frame of most of wxWidgets samples for example)
        //
        //  2. It is buggy for frames with wxFRAME_TOOL_WINDOW style for which
        //     the default is for whatever reason 8*8 which breaks client <->
        //     window size calculations (it would be nice if it didn't, but it
        //     does and the simplest way to fix it seemed to change the broken
        //     default size anyhow)
        //
        //  3. There is just no advantage in doing it: with x and y it is
        //     possible that [future versions of] Windows position the new top
        //     level window in some smart way which we can't do, but we can
        //     guess a reasonably good size for a new window just as well
        //     ourselves
        //
        wxSize sizeReal = size;
        sizeReal.SetDefaults(GetDefaultSize());

        w = sizeReal.x;
        h = sizeReal.y;
    }
    else
    {
        w = size.x;
        h = size.y;
    }
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW fullscreen
// ----------------------------------------------------------------------------

bool wxTopLevelWindowMSW::ShowFullScreen(bool show, long style)
{
    if ( show == IsFullScreen() )
    {
        // nothing to do
        return true;
    }

    m_fsIsShowing = show;

    if ( show )
    {
        m_fsStyle = style;

        // zap the frame borders

        // save the 'normal' window style
        wxMSWWinStyleUpdater updateStyle(GetHwnd());
        m_fsOldWindowStyle = updateStyle.Get();

        // save the old position, width & height, maximize state
        m_fsOldSize = GetRect();
        m_fsIsMaximized = IsMaximized();

        // decide which window style flags to turn off
        LONG offFlags = 0;

        if (style & wxFULLSCREEN_NOBORDER)
        {
            offFlags |= WS_BORDER;
            offFlags |= WS_THICKFRAME;
        }
        if (style & wxFULLSCREEN_NOCAPTION)
            offFlags |= WS_CAPTION | WS_SYSMENU;

        updateStyle.TurnOff(offFlags);

        // Full screen windows should logically be popups as they don't have
        // decorations (and are definitely not children) and while not using
        // this style doesn't seem to make any difference for most windows, it
        // breaks wxGLCanvas in some cases, see #15434, so just always use it.
        updateStyle.TurnOn(WS_POPUP);

        // change our window style to be compatible with full-screen mode
        updateStyle.Apply();

        // resize to the size of the display containing us, falling back to the
        // primary one
        const wxRect rect = wxDisplay(this).GetGeometry();

        SetSize(rect);

        // now flush the window style cache and actually go full-screen
        long flags = SWP_FRAMECHANGED;

        // showing the frame full screen should also show it if it's still
        // hidden
        if ( !IsShown() )
        {
            // don't call wxWindow version to avoid flicker from calling
            // ::ShowWindow() -- we're going to show the window at the correct
            // location directly below -- but do call the wxWindowBase version
            // to sync the internal m_isShown flag
            wxWindowBase::Show();

            flags |= SWP_SHOWWINDOW;
        }

        SetWindowPos(GetHwnd(), HWND_TOP,
                     rect.x, rect.y, rect.width, rect.height,
                     flags);

        // finally send an event allowing the window to relayout itself &c
        wxSizeEvent event(rect.GetSize(), GetId());
        event.SetEventObject(this);
        HandleWindowEvent(event);
    }
    else // stop showing full screen
    {
        Maximize(m_fsIsMaximized);
        SetWindowLong(GetHwnd(),GWL_STYLE, m_fsOldWindowStyle);
        SetWindowPos(GetHwnd(),HWND_TOP,m_fsOldSize.x, m_fsOldSize.y,
            m_fsOldSize.width, m_fsOldSize.height, SWP_FRAMECHANGED);
    }

    return true;
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW misc
// ----------------------------------------------------------------------------

void wxTopLevelWindowMSW::SetTitle( const wxString& title)
{
    SetLabel(title);
}

wxString wxTopLevelWindowMSW::GetTitle() const
{
    return GetLabel();
}

bool wxTopLevelWindowMSW::DoSelectAndSetIcon(const wxIconBundle& icons,
                                             int smX,
                                             int smY,
                                             int i)
{
    const wxSize size(wxGetSystemMetrics(smX, this), wxGetSystemMetrics(smY, this));

    wxIcon icon = icons.GetIcon(size, wxIconBundle::FALLBACK_NEAREST_LARGER);

    if ( !icon.IsOk() )
        return false;

    ::SendMessage(GetHwnd(), WM_SETICON, i, (LPARAM)GetHiconOf(icon));
    return true;
}

void wxTopLevelWindowMSW::SetIcons(const wxIconBundle& icons)
{
    wxTopLevelWindowBase::SetIcons(icons);

    if ( icons.IsEmpty() )
    {
        // FIXME: SetIcons(wxNullIconBundle) should unset existing icons,
        //        but we currently don't do that
        wxASSERT_MSG( m_icons.IsEmpty(), "unsetting icons doesn't work" );
        return;
    }

    DoSetIcons();
}

void wxTopLevelWindowMSW::DoSetIcons()
{
    DoSelectAndSetIcon(m_icons, SM_CXSMICON, SM_CYSMICON, ICON_SMALL);
    DoSelectAndSetIcon(m_icons, SM_CXICON, SM_CYICON, ICON_BIG);
}

wxContentProtection wxTopLevelWindowMSW::GetContentProtection() const
{
    typedef BOOL(WINAPI *GetWindowDisplayAffinity_t)(HWND, DWORD *);

    wxDynamicLibrary dllUser32("user32.dll");
    GetWindowDisplayAffinity_t pfnGetWindowDisplayAffinity =
        (GetWindowDisplayAffinity_t)dllUser32.RawGetSymbol("GetWindowDisplayAffinity");
    if (pfnGetWindowDisplayAffinity)
    {
        DWORD affinity = 0;
        if (!pfnGetWindowDisplayAffinity(GetHWND(), &affinity))
            wxLogLastError("GetWindowDisplayAffinity");
        else if (affinity & WDA_MONITOR)
            return wxCONTENT_PROTECTION_ENABLED;
    }

    return wxCONTENT_PROTECTION_NONE;
}

bool wxTopLevelWindowMSW::SetContentProtection(wxContentProtection contentProtection)
{
    typedef BOOL(WINAPI *SetWindowDisplayAffinity_t)(HWND, DWORD);

    wxDynamicLibrary dllUser32("user32.dll");
    SetWindowDisplayAffinity_t pfnSetWindowDisplayAffinity =
        (SetWindowDisplayAffinity_t)dllUser32.RawGetSymbol("SetWindowDisplayAffinity");
    if (pfnSetWindowDisplayAffinity)
    {
        if (pfnSetWindowDisplayAffinity(GetHWND(),
            (contentProtection == wxCONTENT_PROTECTION_ENABLED) ?
            WDA_MONITOR : WDA_NONE))
            return true;
        else
            wxLogLastError("SetWindowDisplayAffinity");
    }

    return false;
}

// static
bool wxTopLevelWindowMSW::MSWEnableCloseButton(WXHWND hwnd, bool enable)
{
    // get system (a.k.a. window) menu
    HMENU hmenu = GetSystemMenu(hwnd, FALSE /* get it */);
    if ( !hmenu )
    {
        // no system menu at all -- ok if we want to remove the close button
        // anyhow, but bad if we want to show it
        return !enable;
    }

    // enabling/disabling the close item from it also automatically
    // disables/enables the close title bar button
    if ( ::EnableMenuItem(hmenu, SC_CLOSE,
                          MF_BYCOMMAND |
                          (enable ? MF_ENABLED : MF_GRAYED)) == -1 )
    {
        wxLogLastError(wxT("EnableMenuItem(SC_CLOSE)"));

        return false;
    }
    // update appearance immediately
    if ( !::DrawMenuBar(hwnd) )
    {
        wxLogLastError(wxT("DrawMenuBar"));
    }

    return true;
}

bool wxTopLevelWindowMSW::EnableCloseButton(bool enable)
{
    return MSWEnableCloseButton(GetHwnd(), enable);
}

// Window must have wxCAPTION and either wxCLOSE_BOX or wxSYSTEM_MENU for the
// button to be visible. Also check for wxMAXIMIZE_BOX because we don't want
// to enable a button that is excluded from the current style.

bool wxTopLevelWindowMSW::EnableMaximizeButton(bool enable)
{
    if ( ( HasFlag(wxCAPTION) &&
         ( HasFlag(wxCLOSE_BOX) || HasFlag(wxSYSTEM_MENU) ) ) &&
         HasFlag(wxMAXIMIZE_BOX) )
    {
        if ( enable )
        {
            SetWindowStyleFlag(GetWindowStyleFlag() | wxMAXIMIZE_BOX);
        }
        else
        {
            SetWindowStyleFlag(GetWindowStyleFlag() ^ wxMAXIMIZE_BOX);
            // Restore the style to our internal store
            wxWindowBase::SetWindowStyleFlag(GetWindowStyle() | wxMAXIMIZE_BOX);
        }

        return true;
    }

    return false;
}

bool wxTopLevelWindowMSW::EnableMinimizeButton(bool enable)
{
    if ( ( HasFlag(wxCAPTION) &&
         ( HasFlag(wxCLOSE_BOX) || HasFlag(wxSYSTEM_MENU) ) ) &&
         HasFlag(wxMINIMIZE_BOX) )
    {
        if ( enable )
        {
            SetWindowStyleFlag(GetWindowStyleFlag() | wxMINIMIZE_BOX);
        }
        else
        {
            SetWindowStyleFlag(GetWindowStyleFlag() ^ wxMINIMIZE_BOX);
            // Restore the style to our internal store
            wxWindowBase::SetWindowStyleFlag(GetWindowStyle() | wxMINIMIZE_BOX);
        }

        return true;
    }

    return false;
}

void wxTopLevelWindowMSW::RequestUserAttention(int flags)
{
#if defined(FLASHW_STOP)
    WinStruct<FLASHWINFO> fwi;
    fwi.hwnd = GetHwnd();
    fwi.dwFlags = FLASHW_ALL;
    if ( flags & wxUSER_ATTENTION_INFO )
    {
        // just flash a few times
        fwi.uCount = 3;
    }
    else // wxUSER_ATTENTION_ERROR
    {
        // flash until the user notices it
        fwi.dwFlags |= FLASHW_TIMERNOFG;
    }

    ::FlashWindowEx(&fwi);
#else
    {
        wxUnusedVar(flags);
        ::FlashWindow(GetHwnd(), TRUE);
    }
#endif // defined(FLASHW_STOP)
}

wxMenu *wxTopLevelWindowMSW::MSWGetSystemMenu() const
{
#ifndef __WXUNIVERSAL__
    if ( !m_menuSystem )
    {
        HMENU hmenu = ::GetSystemMenu(GetHwnd(), FALSE);
        if ( !hmenu )
        {
            wxLogLastError(wxT("GetSystemMenu()"));
            return nullptr;
        }

        wxTopLevelWindowMSW * const
            self = const_cast<wxTopLevelWindowMSW *>(this);

        self->m_menuSystem = wxMenu::MSWNewFromHMENU(hmenu);

        // We need to somehow associate this menu with this window to ensure
        // that we get events from it. A natural idea would be to pretend that
        // it's attached to our menu bar but this wouldn't work if we don't
        // have any menu bar which is a common case for applications using
        // custom items in the system menu (they mostly do it exactly because
        // they don't have any other menus).
        //
        // So reuse the invoking window pointer instead, this is not exactly
        // correct but doesn't seem to have any serious drawbacks.
        m_menuSystem->SetInvokingWindow(self);
    }
#endif // #ifndef __WXUNIVERSAL__

    return m_menuSystem;
}

// ----------------------------------------------------------------------------
// Transparency support
// ---------------------------------------------------------------------------

bool wxTopLevelWindowMSW::SetTransparent(wxByte alpha)
{
    wxMSWWinExStyleUpdater updateExStyle(GetHwnd());

    // if setting alpha to fully opaque then turn off the layered style
    if (alpha == 255)
    {
        updateExStyle.TurnOff(WS_EX_LAYERED).Apply();
        Refresh();
        return true;
    }

    // Otherwise, set the layered style if needed and set the alpha value
    updateExStyle.TurnOn(WS_EX_LAYERED).Apply();

    if ( ::SetLayeredWindowAttributes(GetHwnd(), 0, (BYTE)alpha, LWA_ALPHA) )
        return true;

    return false;
}

bool wxTopLevelWindowMSW::CanSetTransparent()
{
    return true;
}

void wxTopLevelWindowMSW::DoFreeze()
{
    // do nothing: freezing toplevel window causes paint and mouse events
    // to go through it any TLWs under it, so the best we can do is to freeze
    // all children -- and wxWindowBase::Freeze() does that
}

void wxTopLevelWindowMSW::DoThaw()
{
    // intentionally empty -- see DoFreeze()
}


// ----------------------------------------------------------------------------
// wxTopLevelWindow event handling
// ----------------------------------------------------------------------------

void wxTopLevelWindowMSW::DoSaveLastFocus()
{
    if ( MSWIsIconized() )
        return;

    // remember the last focused child if it is our child
    wxWindow* const winFocus = FindFocus();

    m_winLastFocused = IsDescendant(winFocus) ? winFocus : nullptr;
}

void wxTopLevelWindowMSW::DoRestoreLastFocus()
{
    wxWindow *parent = m_winLastFocused ? m_winLastFocused->GetParent()
                                        : nullptr;
    if ( !parent )
    {
        parent = this;
    }

    wxWindow* winPtr = m_winLastFocused;
    wxSetFocusToChild(parent, &winPtr);
    m_winLastFocused = winPtr;
}

void wxTopLevelWindowMSW::OnActivate(wxActivateEvent& event)
{
    if ( event.GetActive() )
    {
        // We get WM_ACTIVATE before being restored from iconized state, so we
        // can be still iconized here. In this case, avoid restoring the focus
        // as it doesn't work anyhow and we will do when we're really restored.
        if ( MSWIsIconized() )
        {
            event.Skip();
            return;
        }

        // restore focus to the child which was last focused unless one of our
        // children already has it (the frame having focus on itself does not
        // count, if only because this would be always the case for an MDI
        // child frame as the MDI parent sets focus to it before it's
        // activated)
        wxLogTrace(wxT("focus"), wxT("wxTLW %p activated."), m_hWnd);

        wxWindow* const winFocus = FindFocus();
        if ( winFocus == this || !IsDescendant(winFocus) )
            DoRestoreLastFocus();
    }
    else // deactivating
    {
        DoSaveLastFocus();

        wxLogTrace(wxT("focus"),
                   wxT("wxTLW %p deactivated, last focused: %p."),
                   m_hWnd,
                   m_winLastFocused ? GetHwndOf(m_winLastFocused) : nullptr);

        event.Skip();
    }
}

// the DialogProc for all wxWidgets dialogs
INT_PTR APIENTRY
wxDlgProc(HWND WXUNUSED(hDlg),
          UINT message,
          WPARAM WXUNUSED(wParam),
          LPARAM WXUNUSED(lParam))
{
    switch ( message )
    {
        case WM_INITDIALOG:
        {
            // for WM_INITDIALOG, returning TRUE tells system to set focus to
            // the first control in the dialog box, but as we set the focus
            // ourselves, we return FALSE for it as well
            return FALSE;
        }
    }

    // for almost all messages, returning FALSE means that we didn't process
    // the message
    return FALSE;
}

// ============================================================================
// wxTLWHiddenParentModule implementation
// ============================================================================

HWND wxTLWHiddenParentModule::ms_hwnd = nullptr;

const wxChar *wxTLWHiddenParentModule::ms_className = nullptr;

bool wxTLWHiddenParentModule::OnInit()
{
    ms_hwnd = nullptr;
    ms_className = nullptr;

    return true;
}

void wxTLWHiddenParentModule::OnExit()
{
    if ( ms_hwnd )
    {
        if ( !::DestroyWindow(ms_hwnd) )
        {
            wxLogLastError(wxT("DestroyWindow(hidden TLW parent)"));
        }

        ms_hwnd = nullptr;
    }

    if ( ms_className )
    {
        if ( !::UnregisterClass(ms_className, wxGetInstance()) )
        {
            wxLogLastError(wxT("UnregisterClass(\"wxTLWHiddenParent\")"));
        }

        ms_className = nullptr;
    }
}

/* static */
HWND wxTLWHiddenParentModule::GetHWND()
{
    if ( !ms_hwnd )
    {
        if ( !ms_className )
        {
            static const wxChar *HIDDEN_PARENT_CLASS = wxT("wxTLWHiddenParent");

            WNDCLASS wndclass;
            wxZeroMemory(wndclass);

            wndclass.lpfnWndProc   = DefWindowProc;
            wndclass.hInstance     = wxGetInstance();
            wndclass.lpszClassName = HIDDEN_PARENT_CLASS;

            if ( !::RegisterClass(&wndclass) )
            {
                wxLogLastError(wxT("RegisterClass(\"wxTLWHiddenParent\")"));
            }
            else
            {
                ms_className = HIDDEN_PARENT_CLASS;
            }
        }

        ms_hwnd = ::CreateWindow(ms_className, wxEmptyString, 0, 0, 0, 0, 0, nullptr,
                                 nullptr, wxGetInstance(), nullptr);
        if ( !ms_hwnd )
        {
            wxLogLastError(wxT("CreateWindow(hidden TLW parent)"));
        }
    }

    return ms_hwnd;
}
