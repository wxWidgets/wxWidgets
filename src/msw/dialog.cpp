/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/dialog.cpp
// Purpose:     wxDialog class
// Author:      Julian Smart
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


#include "wx/dialog.h"
#include "wx/modalhook.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/frame.h"
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/settings.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/toolbar.h"
#endif

#include "wx/msw/private.h"
#include "wx/msw/private/custompaint.h"
#include "wx/msw/private/darkmode.h"
#include "wx/msw/wrapcctl.h"

#include "wx/evtloop.h"
#include "wx/scopedptr.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// wxDialogModalData
// ----------------------------------------------------------------------------

// this is simply a container for any data we need to implement modality which
// allows us to avoid changing wxDialog each time the implementation changes
class wxDialogModalData
{
public:
    wxDialogModalData(wxDialog *dialog) : m_evtLoop(dialog) { }

    void RunLoop()
    {
        m_evtLoop.Run();
    }

    void ExitLoop()
    {
        m_evtLoop.Exit();
    }

private:
    wxModalEventLoop m_evtLoop;
};

wxDEFINE_TIED_SCOPED_PTR_TYPE(wxDialogModalData)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// Gripper subclass proc
// ----------------------------------------------------------------------------

namespace wxMSWImpl
{

LRESULT CALLBACK
GripperProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam,
            UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    wxDialog* const self = reinterpret_cast<wxDialog*>(dwRefData);

    switch ( nMsg )
    {
        case WM_PAINT:
            {
                const auto bg = self->GetBackgroundColour();

                wxMSWImpl::CustomPaint
                (
                    hwnd,
                    [](HWND hwnd, WPARAM wParam)
                    {
                        ::DefSubclassProc(hwnd, WM_PAINT, wParam, 0);
                    },
                    [bg](const wxBitmap& bmp)
                    {
                        return wxMSWImpl::PostPaintEachPixel
                               (
                                    bmp,
                                    [bg](unsigned char& r,
                                         unsigned char& g,
                                         unsigned char& b,
                                         unsigned char& a)
                                    {
                                        // Replace all background pixels, which
                                        // are transparent, with the colour we
                                        // want to use.
                                        if ( a == wxALPHA_TRANSPARENT )
                                        {
                                            r = bg.Red();
                                            g = bg.Green();
                                            b = bg.Blue();

                                            a = wxALPHA_OPAQUE;
                                        }
                                    }
                               );
                    }
                );
            }
            return 0;

        case WM_NCDESTROY:
            ::RemoveWindowSubclass(hwnd, GripperProc, uIdSubclass);
            break;
    }

    return ::DefSubclassProc(hwnd, nMsg, wParam, lParam);
}

} // namespace wxMSWImpl

// ----------------------------------------------------------------------------
// wxDialog construction
// ----------------------------------------------------------------------------

void wxDialog::Init()
{
    m_isShown = false;
    m_modalData = nullptr;
    m_hGripper = 0;
}

bool wxDialog::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxString& title,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    SetExtraStyle(GetExtraStyle() | wxTOPLEVEL_EX_DIALOG);

    // All dialogs should really have this style
    style |= wxTAB_TRAVERSAL;

    if ( !wxTopLevelWindow::Create(parent, id, title, pos, size, style, name) )
        return false;

    if ( !m_hasFont )
        SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

    if ( HasFlag(wxRESIZE_BORDER) )
    {
        CreateGripper();

        Bind(wxEVT_CREATE, &wxDialog::OnWindowCreate, this);
    }

    return true;
}

wxDialog::~wxDialog()
{
    // this will also reenable all the other windows for a modal dialog
    Show(false);

    DestroyGripper();
}

// ----------------------------------------------------------------------------
// showing the dialogs
// ----------------------------------------------------------------------------

bool wxDialog::Show(bool show)
{
    if ( show == IsShown() )
        return false;

    if ( !show && m_modalData )
    {
        // we need to do this before calling wxDialogBase version because if we
        // had disabled other app windows, they must be reenabled right now as
        // if they stay disabled Windows will activate another window (one
        // which is enabled, anyhow) when we're hidden in the base class Show()
        // and we will lose activation
        m_modalData->ExitLoop();
    }

    if ( show )
    {
        if (CanDoLayoutAdaptation())
            DoLayoutAdaptation();

        // this usually will result in TransferDataToWindow() being called
        // which will change the controls values so do it before showing as
        // otherwise we could have some flicker
        InitDialog();
    }

    wxDialogBase::Show(show);

    if ( show )
    {
        // dialogs don't get WM_SIZE message from ::ShowWindow() for some
        // reason so generate it ourselves for consistency with frames and
        // dialogs in other ports
        //
        // NB: normally we should call it just the first time but doing it
        //     every time is simpler than keeping a flag
        const wxSize size = GetClientSize();
        ::SendMessage(GetHwnd(), WM_SIZE,
                      SIZE_RESTORED, MAKELPARAM(size.x, size.y));
    }

    return true;
}

// show dialog modally
int wxDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

    wxASSERT_MSG( !IsModal(), wxT("ShowModal() can't be called twice") );

    wxDialogModalDataTiedPtr modalData(&m_modalData,
                                       new wxDialogModalData(this));

    Show();

    // EndModal may have been called from InitDialog handler (called from
    // inside Show()) and hidden the dialog back again
    if ( IsShown() )
        modalData->RunLoop();
    else
        m_modalData->ExitLoop();

    return GetReturnCode();
}

void wxDialog::EndModal(int retCode)
{
    wxASSERT_MSG( IsModal(), wxT("EndModal() called for non modal dialog") );

    SetReturnCode(retCode);

    Hide();
}

// ----------------------------------------------------------------------------
// wxDialog gripper handling
// ----------------------------------------------------------------------------

void wxDialog::SetWindowStyleFlag(long style)
{
    wxDialogBase::SetWindowStyleFlag(style);

    // Don't do anything if we're setting the style before creating the dialog.
    if ( !GetHwnd() )
        return;

    if ( HasFlag(wxRESIZE_BORDER) )
        CreateGripper();
    else
        DestroyGripper();
}

void wxDialog::CreateGripper()
{
    if ( !m_hGripper )
    {
        // just create it here, it will be positioned and shown later
        m_hGripper = (WXHWND)::CreateWindow
                               (
                                    wxT("SCROLLBAR"),
                                    wxT(""),
                                    WS_CHILD |
                                    WS_CLIPSIBLINGS |
                                    SBS_SIZEGRIP |
                                    SBS_SIZEBOX |
                                    SBS_SIZEBOXBOTTOMRIGHTALIGN,
                                    0, 0, 0, 0,
                                    GetHwnd(),
                                    0,
                                    wxGetInstance(),
                                    nullptr
                               );

        wxMSWDarkMode::AllowForWindow((HWND)m_hGripper);

        // Whether we use the dark mode or not, handle WM_PAINT for the gripper
        // ourselves, as even in the light mode its background is wrong if the
        // dialog doesn't use the default background colour -- and in dark mode
        // it's wrong even by default.
        ::SetWindowSubclass(m_hGripper, wxMSWImpl::GripperProc,
                            0, wxPtrToUInt(this));
    }
}

void wxDialog::DestroyGripper()
{
    if ( m_hGripper )
    {
        // we used to have trouble with gripper appearing on top (and hence
        // overdrawing) the other, real, dialog children -- check that this
        // isn't the case automatically (but notice that this could be false if
        // we're not shown at all as in this case ResizeGripper() might not
        // have been called yet)
        wxASSERT_MSG( !IsShown() ||
                      ::GetWindow((HWND)m_hGripper, GW_HWNDNEXT) == 0,
            wxT("Bug in wxWidgets: gripper should be at the bottom of Z-order") );
        ::DestroyWindow((HWND) m_hGripper);
        m_hGripper = 0;
    }
}

void wxDialog::ShowGripper(bool show)
{
    wxASSERT_MSG( m_hGripper, wxT("shouldn't be called if we have no gripper") );

    if ( show )
        ResizeGripper();

    ::ShowWindow((HWND)m_hGripper, show ? SW_SHOW : SW_HIDE);
}

void wxDialog::ResizeGripper()
{
    wxASSERT_MSG( m_hGripper, wxT("shouldn't be called if we have no gripper") );

    HWND hwndGripper = (HWND)m_hGripper;

    const wxRect rectGripper = wxRectFromRECT(wxGetWindowRect(hwndGripper));
    const wxSize size = GetClientSize() - rectGripper.GetSize();

    ::SetWindowPos(hwndGripper, HWND_BOTTOM,
                   size.x, size.y,
                   rectGripper.width, rectGripper.height,
                   SWP_NOACTIVATE);
}

void wxDialog::OnWindowCreate(wxWindowCreateEvent& event)
{
    if ( m_hGripper && IsShown() &&
            event.GetWindow() && event.GetWindow()->GetParent() == this )
    {
        // Put gripper below the newly created child window
        ::SetWindowPos((HWND)m_hGripper, HWND_BOTTOM, 0, 0, 0, 0,
                       SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
    }

    event.Skip();
}

// ----------------------------------------------------------------------------
// wxWin event handlers
// ----------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// dialog Windows messages processing
// ---------------------------------------------------------------------------

WXLRESULT wxDialog::MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
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
            switch ( wParam )
            {
                case SIZE_MINIMIZED:
                    m_showCmd = SW_MINIMIZE;
                    break;

                case SIZE_MAXIMIZED:
                    wxFALLTHROUGH;

                case SIZE_RESTORED:
                    if ( m_hGripper )
                        ShowGripper( wParam == SIZE_RESTORED );

                    if ( m_showCmd == SW_MINIMIZE )
                        (void)SendIconizeEvent(false);
                    m_showCmd = SW_RESTORE;

                    break;
            }

            // the Windows dialogs unfortunately are not meant to be resizable
            // at all and their standard class doesn't include CS_[VH]REDRAW
            // styles which means that the window is not refreshed properly
            // after the resize and no amount of WS_CLIPCHILDREN/SIBLINGS can
            // help with it - so we have to refresh it manually which certainly
            // creates flicker but at least doesn't show garbage on the screen
            rc = wxWindow::MSWWindowProc(message, wParam, lParam);
            processed = true;
            if ( HasFlag(wxFULL_REPAINT_ON_RESIZE) )
            {
                ::InvalidateRect(GetHwnd(), nullptr, false /* erase bg */);
            }
            break;

        case WM_CTLCOLORDLG:
            // We need to explicitly set the dark background colour when using
            // dark mode, otherwise we'd be using the default light background.
            if ( wxMSWDarkMode::IsActive() )
                return (WXLRESULT)wxMSWDarkMode::GetBackgroundBrush();
            break;
    }

    if ( !processed )
        rc = wxDialogBase::MSWWindowProc(message, wParam, lParam);

    return rc;
}
