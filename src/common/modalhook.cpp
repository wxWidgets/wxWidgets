///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/modalhook.cpp
// Purpose:     wxModalDialogHook implementation
// Author:      Vadim Zeitlin
// Created:     2013-05-19
// Copyright:   (c) 2013 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/modalhook.h"

#ifndef WX_PRECOMP
    #include "wx/dialog.h"
    #include "wx/event.h"
#endif // WX_PRECOMP

#include "wx/weakref.h"

wxModalDialogHook::Hooks wxModalDialogHook::ms_hooks;
int wxModalDialogHook::ms_countOpen = 0;

// Keep the modal-hook exit notification tied to the dialog lifetime rather
// than blindly retaining its raw address until ShowModal() unwinds. WinUI
// presenters run a nested event loop in which application code can legally
// Destroy() the source dialog.
class wxModalDialogHookExitGuard::Impl final : public wxEvtHandler
{
public:
    explicit Impl(wxDialog *dialog)
        : m_dialog(dialog),
          m_identity(dialog)
    {
        if ( dialog )
        {
            dialog->Bind(
                wxEVT_DESTROY,
                &wxModalDialogHookExitGuard::Impl::OnDestroy,
                this);
        }
    }

    ~Impl() override
    {
        Finish(false);
    }

private:
    void Finish(bool fromDestroy)
    {
        if ( m_finished )
            return;

        wxDialog * const dialog = m_dialog.get();
        m_finished = true;

        if ( dialog && !fromDestroy )
        {
            dialog->Unbind(
                wxEVT_DESTROY,
                &wxModalDialogHookExitGuard::Impl::OnDestroy,
                this);
        }

        // Mark the transaction complete before invoking user hooks: Exit()
        // itself may destroy the dialog and synchronously re-enter OnDestroy.
        // During wxEVT_DESTROY the object is still alive, so its pointer is
        // valid for the established hook contract.
        wxModalDialogHook::CallExit(dialog);
    }

    void OnDestroy(wxWindowDestroyEvent& event)
    {
        event.Skip();
        if ( event.GetWindow() != m_identity )
            return;

        Finish(true);
    }

    wxWeakRef<wxDialog> m_dialog;
    wxDialog * const m_identity;
    bool m_finished = false;
};

wxModalDialogHookExitGuard::wxModalDialogHookExitGuard(wxDialog *dialog)
    : m_impl(new Impl(dialog))
{
}

wxModalDialogHookExitGuard::~wxModalDialogHookExitGuard()
{
    delete m_impl;
}

// ============================================================================
// wxModalDialogHook implementation
// ============================================================================

// ----------------------------------------------------------------------------
// Hooks management
// ----------------------------------------------------------------------------

void wxModalDialogHook::Register()
{
#if wxDEBUG_LEVEL
    for ( auto hook : ms_hooks )
    {
        if ( hook == this )
        {
            wxFAIL_MSG( wxS("Registering already registered hook?") );
            return;
        }
    }
#endif // wxDEBUG_LEVEL

    ms_hooks.insert(ms_hooks.begin(), this);
}

void wxModalDialogHook::Unregister()
{
    if ( !DoUnregister() )
    {
        wxFAIL_MSG( wxS("Unregistering not registered hook?") );
    }
}

bool wxModalDialogHook::DoUnregister()
{
    for ( auto it = ms_hooks.begin(); it != ms_hooks.end(); ++it )
    {
        if ( *it == this )
        {
            ms_hooks.erase(it);
            return true;
        }
    }

    return false;
}

// ----------------------------------------------------------------------------
// Invoking hooks methods
// ----------------------------------------------------------------------------

/* static */
int wxModalDialogHook::CallEnter(wxDialog* dialog)
{
    // Make a copy of the hooks list to avoid problems if it's modified while
    // we're iterating over it: this is unlikely to happen in our case, but
    // quite possible in CallExit() as the hooks may remove themselves after
    // the call to their Exit(), so do it here for symmetry as well.
    const Hooks hooks = ms_hooks;

    for ( auto hook : hooks )
    {
        const int rc = hook->Enter(dialog);
        if ( rc != wxID_NONE )
        {
            // Skip calling all the rest of the hooks if one of them preempts
            // showing the dialog completely.
            return rc;
        }
    }

    ms_countOpen++;

    return wxID_NONE;
}

/* static */
void wxModalDialogHook::CallExit(wxDialog* dialog)
{
    ms_countOpen--;

    // See comment in CallEnter() for the reasons for making a copy here.
    const Hooks hooks = ms_hooks;

    for ( auto hook : hooks )
    {
        hook->Exit(dialog);
    }
}
