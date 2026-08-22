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
#if defined(__WXWINUI__) && wxUSE_WINUI3
    #if wxUSE_CHECKBOX
        #include "wx/checkbox.h"
    #endif
    #if wxUSE_RADIOBTN
        #include "wx/radiobut.h"
    #endif
    #if wxUSE_STATTEXT
        #include "wx/stattext.h"
    #endif
    #include "wx/scopeguard.h"
    #include "wx/weakref.h"
    #include "wx/winui/private/dialogsession.h"
    #include "wx/winui/private/tlwhostmsw.h"
    #include "wx/winui/winui.h"
#endif

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

#if defined(__WXWINUI__) && wxUSE_WINUI3

namespace
{

bool wxWinUIDialogControlIsUsable(const wxWindow *window)
{
    return window && !window->IsBeingDeleted() &&
           window->IsEnabled() && window->IsShownOnScreen();
}

bool wxWinUILabelUsesMnemonic(const wxString& label, wxChar mnemonic)
{
    const wxChar expected = static_cast<wxChar>(wxToupper(mnemonic));

    for ( size_t i = 0; i + 1 < label.length(); ++i )
    {
        if ( label[i] != wxT('&') )
            continue;

        const wxChar next = label[i + 1];
        if ( next == wxT('&') )
        {
            ++i;
            continue;
        }

        return static_cast<wxChar>(wxToupper(next)) == expected;
    }

    return false;
}

#if wxUSE_BUTTON
bool wxWinUIDispatchDialogButton(wxDialog *dialog, wxButton *button)
{
    if ( !wxWinUIDialogControlIsUsable(button) )
        return false;

    // Both the button and the dialog may be destroyed synchronously by the
    // command handler. Command() routes through the normal wx event chain,
    // including wxDialogBase::OnButton(), instead of relying on an HWND button
    // notification which a XAML peer doesn't have.
    wxWeakRef<wxWindow> dialogAlive(dialog);
    wxWeakRef<wxWindow> buttonAlive(button);

    wxCommandEvent event(wxEVT_BUTTON, button->GetId());
    event.SetEventObject(button);
    button->Command(event);

    // Do not dereference either object after the callback. A dispatched,
    // usable button owns the key even when an application handler deliberately
    // vetoes the close or destroys the whole dialog.
    if ( !dialogAlive || !buttonAlive )
        return true;

    return true;
}
#endif // wxUSE_BUTTON

#if wxUSE_CHECKBOX
bool wxWinUIDispatchDialogCheckBox(wxDialog *dialog, wxCheckBox *checkBox)
{
    if ( !wxWinUIDialogControlIsUsable(checkBox) )
        return false;

    wxWeakRef<wxWindow> dialogAlive(dialog);
    wxWeakRef<wxWindow> checkBoxAlive(checkBox);

    const bool checked = !checkBox->GetValue();
    wxCommandEvent event(wxEVT_CHECKBOX, checkBox->GetId());
    event.SetEventObject(checkBox);
    event.SetInt(checked);
    checkBox->Command(event);

    if ( !dialogAlive || !checkBoxAlive )
        return true;

    return true;
}
#endif // wxUSE_CHECKBOX

#if wxUSE_RADIOBTN
bool wxWinUIDispatchDialogRadioButton(wxDialog *dialog,
                                      wxRadioButton *radioButton)
{
    if ( !wxWinUIDialogControlIsUsable(radioButton) )
        return false;

    wxWeakRef<wxWindow> dialogAlive(dialog);
    wxWeakRef<wxWindow> radioAlive(radioButton);

    wxCommandEvent event(wxEVT_RADIOBUTTON, radioButton->GetId());
    event.SetEventObject(radioButton);
    event.SetInt(1);
    radioButton->Command(event);

    if ( !dialogAlive || !radioAlive )
        return true;

    return true;
}
#endif // wxUSE_RADIOBTN

enum class wxWinUIEnterPolicy
{
    ActivateDialogButton,
    ControlOwnsKey,
    DialogWasDestroyed
};

wxWinUIEnterPolicy wxWinUIGetEnterPolicy(wxDialog *dialog, MSG *msg)
{
    wxWindow * const focus = wxWindow::FindFocus();
    if ( !focus || (focus != dialog && !dialog->IsDescendant(focus)) )
        return wxWinUIEnterPolicy::ActivateDialogButton;

    wxWeakRef<wxWindow> dialogAlive(dialog);
    wxWeakRef<wxWindow> focusAlive(focus);

    // Preserve the standard IsDialogMessage() escape hatch for native and
    // hybrid controls. Query every logical ancestor up to the dialog because
    // compound controls can express the policy on their outer HWND.
    for ( wxWindow *window = focus; window && window != dialog;
          window = window->GetParent() )
    {
        const HWND hwnd = static_cast<HWND>(window->GetHWND());
        if ( hwnd && ::IsWindow(hwnd) )
        {
            wxWeakRef<wxWindow> windowAlive(window);
            const LRESULT dialogCode =
                ::SendMessage(hwnd, WM_GETDLGCODE,
                              msg ? msg->wParam : VK_RETURN,
                              reinterpret_cast<LPARAM>(msg));

            if ( !dialogAlive )
                return wxWinUIEnterPolicy::DialogWasDestroyed;

            if ( !focusAlive || !windowAlive ||
                 (focus != dialog && !dialog->IsDescendant(focus)) )
            {
                return wxWinUIEnterPolicy::ControlOwnsKey;
            }

            if ( dialogCode & DLGC_WANTMESSAGE )
                return wxWinUIEnterPolicy::ControlOwnsKey;
        }
    }

    // XAML text peers don't expose their input policy through the placeholder
    // HWND. Match the wx text-entry contract explicitly: multiline controls
    // own Return, and wxTE_PROCESS_ENTER must first get a chance to emit
    // wxEVT_TEXT_ENTER (whose handler may choose whether to propagate).
    return focus->WXGetTextEntry() &&
                   (focus->HasFlag(wxTE_MULTILINE) ||
                    focus->HasFlag(wxTE_PROCESS_ENTER))
        ? wxWinUIEnterPolicy::ControlOwnsKey
        : wxWinUIEnterPolicy::ActivateDialogButton;
}

bool wxWinUIHandleDialogEnter(wxDialog *dialog, MSG *msg)
{
    switch ( wxWinUIGetEnterPolicy(dialog, msg) )
    {
        case wxWinUIEnterPolicy::ControlOwnsKey:
            return false;

        case wxWinUIEnterPolicy::DialogWasDestroyed:
            return true;

        case wxWinUIEnterPolicy::ActivateDialogButton:
            break;
    }

#if wxUSE_BUTTON
    wxButton *button =
        wxDynamicCast(wxWindow::FindFocus(), wxButton);
    if ( !wxWinUIDialogControlIsUsable(button) )
    {
        button = wxDynamicCast(dialog->GetDefaultItem(), wxButton);
    }

    return wxWinUIDispatchDialogButton(dialog, button);
#else
    wxUnusedVar(dialog);
    return false;
#endif // wxUSE_BUTTON
}

bool wxWinUIHandleDialogEscape(wxDialog *dialog)
{
    // The normal WH_KEYBOARD path has already delivered wxEVT_CHAR_HOOK
    // before the island pre-translation pipeline reaches us. Re-synthesizing
    // it here would expose every unhandled Escape twice. Apply the same close
    // button lookup directly, using the public dialog contract.
    return dialog->WinUIHandleEscapeKey();
}

bool wxWinUIHandleMnemonicIn(wxDialog *dialog,
                             wxWindow *parent,
                             wxChar mnemonic)
{
    for ( wxWindowList::compatibility_iterator node =
              parent->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow * const child = node->GetData();
        if ( !child || child->IsBeingDeleted() )
            continue;

        if ( wxWinUIDialogControlIsUsable(child) &&
             wxWinUILabelUsesMnemonic(child->GetLabel(), mnemonic) )
        {
#if wxUSE_BUTTON
            if ( wxButton * const button = wxDynamicCast(child, wxButton) )
                return wxWinUIDispatchDialogButton(dialog, button);
#endif // wxUSE_BUTTON
#if wxUSE_CHECKBOX
            if ( wxCheckBox * const checkBox =
                     wxDynamicCast(child, wxCheckBox) )
            {
                return wxWinUIDispatchDialogCheckBox(dialog, checkBox);
            }
#endif // wxUSE_CHECKBOX
#if wxUSE_RADIOBTN
            if ( wxRadioButton * const radioButton =
                     wxDynamicCast(child, wxRadioButton) )
            {
                return wxWinUIDispatchDialogRadioButton(dialog, radioButton);
            }
#endif // wxUSE_RADIOBTN
#if wxUSE_STATTEXT
            if ( wxDynamicCast(child, wxStaticText) )
            {
                wxWeakRef<wxWindow> dialogAlive(dialog);
                wxWeakRef<wxWindow> labelAlive(child);
                if ( child->Navigate(wxNavigationKeyEvent::IsForward) )
                    return true;
                if ( !dialogAlive || !labelAlive )
                    return true;
            }
#endif // wxUSE_STATTEXT

            // Only replace IsDialogMessage() for a real XAML slot. A native
            // checkbox/radio/etc. must retain its class-specific mnemonic
            // activation in the base implementation below.
            if ( child->CanAcceptFocusFromKeyboard() &&
                 wxWinUITLWHostSetFocus(child) )
            {
                return true;
            }
        }

        if ( wxWinUIHandleMnemonicIn(dialog, child, mnemonic) )
            return true;
    }

    return false;
}

} // anonymous namespace

#endif // __WXWINUI__ && wxUSE_WINUI3

// ----------------------------------------------------------------------------
// Gripper subclass proc
// ----------------------------------------------------------------------------

namespace wxMSWImpl
{

#ifdef __WXWINUI__

int GetWinUIResizeGripSize(const wxDialog* dialog)
{
    return dialog->FromDIP(18);
}

void PaintWinUIResizeGrip(wxDialog* dialog, HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = ::BeginPaint(hwnd, &ps);
    if ( !hdc )
        return;

    RECT rc;
    ::GetClientRect(hwnd, &rc);

    const wxColour bg = dialog->GetBackgroundColour();
    HBRUSH bgBrush = ::CreateSolidBrush(RGB(bg.Red(), bg.Green(), bg.Blue()));
    if ( bgBrush )
    {
        ::FillRect(hdc, &rc, bgBrush);
        ::DeleteObject(bgBrush);
    }

    const wxColour fg = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
    HPEN pen = ::CreatePen(PS_SOLID, wxMax(1, dialog->FromDIP(1)),
                           RGB(fg.Red(), fg.Green(), fg.Blue()));
    HGDIOBJ oldPen = nullptr;
    if ( pen )
        oldPen = ::SelectObject(hdc, pen);

    const int margin = dialog->FromDIP(3);
    const int step = dialog->FromDIP(5);
    for ( int n = 0; n < 3; ++n )
    {
        const int offset = n * step;
        ::MoveToEx(hdc, rc.right - margin - offset, rc.bottom - margin, nullptr);
        ::LineTo(hdc, rc.right - margin, rc.bottom - margin - offset);
    }

    if ( oldPen )
        ::SelectObject(hdc, oldPen);
    if ( pen )
        ::DeleteObject(pen);

    ::EndPaint(hwnd, &ps);
}

#endif // __WXWINUI__

LRESULT CALLBACK
GripperProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam,
            UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    wxDialog* const self = reinterpret_cast<wxDialog*>(dwRefData);

    switch ( nMsg )
    {
#ifdef __WXWINUI__
        case WM_NCHITTEST:
            return HTTRANSPARENT;

        case WM_PAINT:
            PaintWinUIResizeGrip(self, hwnd);
            return 0;
#else
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
#endif // __WXWINUI__

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
#if defined(__WXWINUI__) && wxUSE_WINUI3
    // Regular frame-class dialogs need the same lifetime transaction as the
    // native common-dialog wrappers: destroying their owner must detach them
    // and end modality without deleting the C++ object underneath this stack.
    // Construct this guard before the modal hook; its final action therefore
    // runs only after the hook guard and all guards in
    // WinUIShowModalWithoutHook() (modalData, focus, cursor) have unwound.
    const wxWeakRef<wxDialog> externalLifetimeSelf(this);
    WinUIBeginExternalModalLifetime();
    const wxScopeGuard externalLifetime = wxMakeGuard(
        [externalLifetimeSelf]()
        {
            if ( wxDialog* const live = externalLifetimeSelf.get() )
                live->WinUIEndExternalModalLifetime();
        });
    wxUnusedVar(externalLifetime);

    WX_HOOK_MODAL_DIALOG();

    wxDialog* const live = externalLifetimeSelf.get();
    if ( !live )
        return wxID_CANCEL;

    live->WinUIArmExternalModalLifetime();
    if ( live->IsBeingDeleted() ||
         live->m_winuiExternalDestroyRequested )
    {
        return wxID_CANCEL;
    }

    return live->WinUIShowModalWithoutHook();
}

int wxDialog::WinUIShowModalWithoutHook()
{
#else
    WX_HOOK_MODAL_DIALOG();
#endif
    wxASSERT_MSG( !IsModal(), wxT("ShowModal() can't be called twice") );

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // Declare this before modalData: reverse destruction order guarantees
    // that the exit notification observes IsModal()==false.
    const auto notifyCursorOnModalExit =
        wxMakeGuard([]() { wxWinUINotifyModalCursorChanged(); });

    // Frame-class WinUI dialogs don't have DefDlgProc() to restore the
    // previously focused owner control when their modal loop ends. Snapshot
    // it weakly and restore only after modalData has re-enabled the owner.
    const wxWeakRef<wxWindow> previousFocus(wxWindow::FindFocus());
    const auto restoreFocusOnModalExit =
        wxMakeGuard(
            [previousFocus]()
            {
                wxWindow * const focus = previousFocus.get();
                if ( focus && !focus->IsBeingDeleted() &&
                        focus->IsShownOnScreen() && focus->IsEnabled() )
                {
                    focus->SetFocus();
                }
            });
#endif
    wxDialogModalDataTiedPtr modalData(&m_modalData,
                                       new wxDialogModalData(this));
#if defined(__WXWINUI__) && wxUSE_WINUI3
    // Busy cursors deliberately do not cover an active modal dialog. Update
    // its island at the exact transition instead of waiting for pointer input.
    wxWinUINotifyModalCursorChanged();
#endif

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

#if defined(__WXWINUI__) && wxUSE_WINUI3

bool wxDialog::Destroy()
{
    // Queue the modal completion before crossing the transient-manager
    // boundary below. Cancelling an owned transient is arbitrary application
    // code: it may re-enter Destroy(), end the modal loop or even delete this
    // object. Publishing the weak deferred request first both shields the
    // island from a nested wxYield() and gives every re-entrant request the
    // same authoritative lifetime state.
    const bool deferredModalDestroy =
        IsModal() && !IsBeingDeleted() &&
        wxWinUITLWHostDeferModalDialogDestroy(this);
    const wxWeakRef<wxDialog> self(this);
    wxWinUINotifyTransientOwnerDestroyScheduled(this);

    wxDialog * const liveAfterNotification = self.get();
    if ( !liveAfterNotification )
        return true;

    if ( liveAfterNotification->m_winuiExternalModalDepth )
    {
        // ShowModal() always owns this external lifetime guard. Remember the
        // public request immediately, but when it came from a retained XAML
        // callback leave both wxEVT_DESTROY and EndModal() to the queued
        // completion. Sending either one here would synchronously shut down
        // the island underneath ButtonAutomationPeer::Invoke().
        const bool shouldEndModal =
            liveAfterNotification->m_winuiExternalModalArmed &&
            liveAfterNotification->IsModal();
        liveAfterNotification->m_winuiExternalDestroyRequested = true;
        if ( deferredModalDestroy )
            return true;

        if ( liveAfterNotification->m_winuiExternalModalArmed &&
                !liveAfterNotification->IsBeingDeleted() )
        {
            liveAfterNotification->SendDestroyEvent();
        }

        wxDialog* const liveAfterDestroyEvent = self.get();
        if ( liveAfterDestroyEvent && shouldEndModal &&
                liveAfterDestroyEvent->IsModal() )
        {
            liveAfterDestroyEvent->EndModal(wxID_CANCEL);
        }
        return true;
    }

    // A regular modal dialog sends wxEVT_DESTROY before the common MSW
    // Destroy() hook below. Keep that event (and the host teardown it causes)
    // outside an active XAML delegate just like the common TLW destruction.
    if ( deferredModalDestroy )
        return true;

    // Ensure modal hooks see a still-most-derived dialog when a regular
    // wxDialog is destroyed from its own nested loop.
    if ( liveAfterNotification->IsModal() &&
            !liveAfterNotification->IsBeingDeleted() )
    {
        liveAfterNotification->SendDestroyEvent();
    }

    wxDialog * const liveAfterDestroyEvent = self.get();
    return !liveAfterDestroyEvent ||
           liveAfterDestroyEvent->wxDialogBase::Destroy();
}

bool wxDialog::WinUICompleteDeferredModalDestroy()
{
    const wxWeakRef<wxDialog> self(this);
    if ( !IsBeingDeleted() )
        SendDestroyEvent();

    wxDialog * const live = self.get();
    if ( !live )
        return true;

    if ( live->m_winuiExternalModalDepth )
    {
        // The outer ShowModal() stack owns physical C++ lifetime. Closing the
        // modal loop here is enough: WinUIEndExternalModalLifetime() performs
        // the qualified base Destroy() only after modalData and every modal
        // guard have unwound. In particular, never append this dialog to
        // wxPendingDelete while ShowModal() still has it on its stack.
        if ( live->m_winuiExternalModalArmed && live->IsModal() )
            live->EndModal(wxID_CANCEL);
        return true;
    }

    return wxWinUIMSWCompleteDeferredTopLevelDestroy(
        live,
        true /* hideBeforePending */,
        true /* destroyEventAlreadySent */);
}

void wxDialog::WinUIBeginExternalModalLifetime()
{
    if ( ++m_winuiExternalModalDepth == 1 )
    {
        m_winuiExternalModalArmed = false;
        m_winuiExternalParent = GetParent();
        if ( m_winuiExternalParent )
        {
            m_winuiExternalParent->Bind(
                wxEVT_DESTROY,
                &wxDialog::WinUIOnExternalParentDestroy,
                this);
            m_winuiExternalParentBound = true;
        }
    }
}

void wxDialog::WinUIArmExternalModalLifetime()
{
    wxASSERT(m_winuiExternalModalDepth);
    m_winuiExternalModalArmed = true;
    if ( m_winuiExternalDestroyRequested && !IsBeingDeleted() )
        SendDestroyEvent();
}

void wxDialog::WinUIEndExternalModalLifetime()
{
    wxASSERT(m_winuiExternalModalDepth);
    if ( --m_winuiExternalModalDepth )
        return;

    m_winuiExternalModalArmed = false;
    if ( m_winuiExternalParentBound &&
            m_winuiExternalParent &&
            !m_winuiExternalParentDestroyDispatch &&
            !m_winuiExternalParent->IsBeingDeleted() )
    {
        m_winuiExternalParent->Unbind(
            wxEVT_DESTROY,
            &wxDialog::WinUIOnExternalParentDestroy,
            this);
    }
    m_winuiExternalParent = nullptr;
    m_winuiExternalParentBound = false;

    if ( m_winuiExternalDestroyRequested )
    {
        m_winuiExternalDestroyRequested = false;
        wxDialogBase::Destroy();
    }
}

void wxDialog::WinUIOnExternalParentDestroy(
    wxWindowDestroyEvent& event)
{
    event.Skip();
    if ( event.GetWindow() != m_winuiExternalParent )
        return;

    if ( GetParent() != m_winuiExternalParent )
    {
        // Reparenting transferred the dialog before the former owner's
        // destroy dispatch reached us. Retire only this stale subscription;
        // the external lifetime must not close or destroy the new owner's TLW.
        m_winuiExternalParentBound = false;
        m_winuiExternalParent = nullptr;
        return;
    }

    const wxWeakRef<wxDialog> self(this);
    const bool wasModal = IsModal();
    if ( wasModal && !IsShown() && wxTheApp )
    {
        // Owner destruction may be dispatched from wxEVT_INIT_DIALOG, before
        // wxDialog::Show() performs its physical Show(true). An immediate
        // EndModal() is then intentionally repeated once after the event loop
        // starts; otherwise Show() would resume and expose a doomed dialog.
        wxTheApp->CallAfter(
            [self]()
            {
                wxDialog* const live = self.get();
                if ( live && live->IsModal() )
                    live->EndModal(wxID_CANCEL);
            });
    }

    m_winuiExternalParentDestroyDispatch = true;
    m_winuiExternalDestroyRequested = true;

    // Parent destruction normally calls wxWindowBase::Destroy() explicitly
    // for every child, bypassing our external-modal deferral. Detach while
    // the parent is still alive so the source remains valid until ShowModal
    // unwinds, then let WinUIEndExternalModalLifetime() delete it.
    //
    // Don't call Reparent(nullptr): TLWs are already present in
    // wxTopLevelWindows even when owned, and the generic implementation would
    // append a duplicate. Remove only the logical owner link and clear the
    // native owner in place.
    if ( GetParent() == m_winuiExternalParent )
    {
        m_winuiExternalParent->RemoveChild(this);

        const HWND hwnd = GetHwndOf(this);
        if ( hwnd && ::IsWindow(hwnd) )
        {
            ::SetLastError(ERROR_SUCCESS);
            const LONG_PTR oldOwner =
                ::SetWindowLongPtr(hwnd, GWLP_HWNDPARENT, 0);
            if ( !oldOwner && ::GetLastError() != ERROR_SUCCESS )
            {
                wxLogLastError("SetWindowLongPtr(GWLP_HWNDPARENT)");
            }
        }
    }

    // Publish the fully detached state before either callback boundary below.
    // In particular EndModal()->Hide() may destroy the preferences dialog and
    // its owner observer synchronously, so this method must not access `this`
    // after its final EndModal() call.
    m_winuiExternalParentBound = false;
    m_winuiExternalParent = nullptr;
    m_winuiExternalParentDestroyDispatch = false;
    if ( !IsBeingDeleted() )
        SendDestroyEvent();

    wxDialog* const live = self.get();
    if ( live && wasModal && live->IsModal() )
        live->EndModal(wxID_CANCEL);
}

bool wxDialog::WinUIHandleEscapeKey()
{
    int closeId = GetEscapeId();
    switch ( closeId )
    {
        case wxID_NONE:
            return false;

        case wxID_ANY:
            if ( EmulateButtonClickIfPresent(wxID_CANCEL) )
                return true;
            closeId = GetAffirmativeId();
            break;

        default:
            break;
    }

    return EmulateButtonClickIfPresent(closeId);
}

bool wxDialog::WinUIActivateDefaultButton()
{
    if ( wxWindow* const defaultItem = GetDefaultItem() )
    {
        if ( EmulateButtonClickIfPresent(defaultItem->GetId()) )
            return true;
    }

    return EmulateButtonClickIfPresent(GetAffirmativeId());
}

bool wxDialog::MSWTranslateMessage(WXMSG *message)
{
    // Keep application accelerators ahead of dialog semantics, as in
    // wxGUIEventLoop::PreProcessMessage().
    if ( wxDialogBase::MSWTranslateMessage(message) )
        return true;

    MSG * const msg = reinterpret_cast<MSG *>(message);
    if ( !msg || msg->message != WM_KEYDOWN )
        return false;

    switch ( msg->wParam )
    {
        case VK_RETURN:
            return wxWinUIHandleDialogEnter(this, msg);

        case VK_ESCAPE:
            return wxWinUIHandleDialogEscape(this);
    }

    return false;
}

bool wxDialog::MSWProcessMessage(WXMSG *message)
{
    MSG * const msg = reinterpret_cast<MSG *>(message);
    if ( msg && msg->message == WM_SYSCHAR &&
         wxWinUIHandleMnemonicIn(
             this, this, static_cast<wxChar>(msg->wParam)) )
    {
        return true;
    }

    // Tab/Shift-Tab inside purely native portions and all existing MSW safety
    // checks remain owned by the common implementation.
    return wxDialogBase::MSWProcessMessage(message);
}

#endif // __WXWINUI__ && wxUSE_WINUI3

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
#ifdef __WXWINUI__
        m_hGripper = (WXHWND)::CreateWindow
                               (
                                    wxT("STATIC"),
                                    wxT(""),
                                    WS_CHILD |
                                    WS_CLIPSIBLINGS |
                                    SS_NOTIFY,
                                    0, 0, 0, 0,
                                    GetHwnd(),
                                    0,
                                    wxGetInstance(),
                                    nullptr
                               );
#else
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
#endif // __WXWINUI__

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

#ifdef __WXWINUI__
    const wxSize gripSize(wxMSWImpl::GetWinUIResizeGripSize(this),
                          wxMSWImpl::GetWinUIResizeGripSize(this));
    const wxSize pos = GetClientSize() - gripSize;

    ::SetWindowPos(hwndGripper, HWND_BOTTOM,
                   pos.x, pos.y,
                   gripSize.x, gripSize.y,
                   SWP_NOACTIVATE);
#else
    const wxRect rectGripper = wxRectFromRECT(wxGetWindowRect(hwndGripper));
    const wxSize size = GetClientSize() - rectGripper.GetSize();

    ::SetWindowPos(hwndGripper, HWND_BOTTOM,
                   size.x, size.y,
                   rectGripper.width, rectGripper.height,
                   SWP_NOACTIVATE);
#endif // __WXWINUI__
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
#ifdef __WXWINUI__
        case WM_NCHITTEST:
            if ( HasFlag(wxRESIZE_BORDER) )
            {
                const int gripSize = wxMSWImpl::GetWinUIResizeGripSize(this);
                const POINT pt =
                {
                    static_cast<LONG>(static_cast<short>(LOWORD(lParam))),
                    static_cast<LONG>(static_cast<short>(HIWORD(lParam)))
                };
                RECT rect;
                ::GetWindowRect(GetHwnd(), &rect);

                if ( pt.x >= rect.right - gripSize &&
                        pt.y >= rect.bottom - gripSize )
                {
                    rc = HTBOTTOMRIGHT;
                    processed = true;
                }
            }
            break;
#endif // __WXWINUI__

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
