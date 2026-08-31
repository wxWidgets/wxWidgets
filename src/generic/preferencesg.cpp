///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/preferencesg.cpp
// Purpose:     Implementation of wxPreferencesEditor.
// Author:      Vaclav Slavik
// Created:     2013-02-19
// Copyright:   (c) 2013 Vaclav Slavik <vslavik@fastmail.fm>
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


#if wxUSE_PREFERENCES_EDITOR

#include "wx/private/preferences.h"

#ifndef wxHAS_PREF_EDITOR_NATIVE

#include "wx/dialog.h"
#include "wx/notebook.h"
#include "wx/private/windowlifetime.h"
#include "wx/sizer.h"
#include "wx/scopeguard.h"

#include <memory>

namespace
{

class wxGenericPrefsDialog;

// Use a real event sink instead of adding an anonymous handler to the owner
// for every preferences session. wxEvtHandler tracks source/sink connections,
// and the explicit unbind below also removes the entry as soon as the dialog
// is torn down instead of letting dead weak-capturing lambdas accumulate until
// the owner itself is destroyed.
class wxGenericPrefsOwnerObserver final : public wxEvtHandler
{
public:
    wxGenericPrefsOwnerObserver() = default;
    ~wxGenericPrefsOwnerObserver() override;

    bool BindTo(wxWindow* owner, wxGenericPrefsDialog* dialog);
    void Disconnect();

private:
    void OnDestroy(wxWindowDestroyEvent& event);

    wxWeakRef<wxWindow> m_owner;
    wxWindow* m_ownerIdentity { nullptr };
    wxWeakRef<wxGenericPrefsDialog> m_dialog;

    wxDECLARE_NO_COPY_CLASS(wxGenericPrefsOwnerObserver);
};

class wxGenericPrefsDialog : public wxDialog
{
public:
    wxGenericPrefsDialog() = default;

    bool Create(wxWindow *parent, const wxString& title)
    {
        const wxWeakRef<wxGenericPrefsDialog> dialogLifetime(this);
        const wxWeakRef<wxWindow> parentLifetime(parent);

        m_expectedOwner = parent;
        m_expectedOwnerIdentity = parent;

        const bool created = wxDialog::Create(
            parent, wxID_ANY, title,
            wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_FRAME_STYLE &
                ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX));
        if ( !created ||
             !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) )
        {
            return false;
        }

        if ( !HasExactOwner(parent) )
        {
            return false;
        }

        if ( parentLifetime )
        {
            m_ownerObserver.reset(new wxGenericPrefsOwnerObserver);
            if ( !m_ownerObserver->BindTo(parent, this) )
                return false;

            if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) ||
                 !wxWeakWindowIsAvailableForCallbacks(parentLifetime,
                                                       parent) ||
                 !HasExactOwner(parent) )
            {
                return false;
            }
        }

        SetSizer(new wxBoxSizer(wxVERTICAL));

        long notebookStyle = 0;
#ifndef __WXWINUI__
        // wxNB_MULTILINE is useful for the traditional generic dialog, but
        // isn't part of the wxWinUI notebook contract.
        notebookStyle |= wxNB_MULTILINE;
#endif

        wxNotebook* const notebook = new wxNotebook;
        const wxWeakRef<wxNotebook> notebookLifetime(notebook);
        const bool notebookCreated = notebook->Create(
            this, wxID_ANY, wxDefaultPosition, wxDefaultSize, notebookStyle);
        if ( !notebookCreated ||
             !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) ||
             !wxWeakWindowIsAvailableForCallbacks(notebookLifetime,
                                                   notebook) ||
             !HasExactOwner(parent) )
        {
            if ( wxWeakWindowIsAvailableForCallbacks(notebookLifetime,
                                                      notebook) )
                delete notebook;
            return false;
        }

        m_notebook = notebook;
        m_notebookLifetime = notebook;
        GetSizer()->Add(notebook,
                        wxSizerFlags(1).Expand().DoubleBorder());

#ifdef __WXGTK__
        SetEscapeId(wxID_CLOSE);
        wxSizer* const buttons = CreateButtonSizer(wxCLOSE);
        if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) ||
             !wxWeakWindowIsAvailableForCallbacks(notebookLifetime,
                                                   notebook) ||
             !buttons || !HasExactOwner(parent) )
        {
            delete buttons;
            return false;
        }

        GetSizer()->Add(
            buttons, wxSizerFlags().Expand().DoubleBorder(wxBOTTOM));
#else
        wxSizer* const buttons = CreateButtonSizer(wxOK | wxCANCEL);
        if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) ||
             !wxWeakWindowIsAvailableForCallbacks(notebookLifetime,
                                                   notebook) ||
             !buttons || !HasExactOwner(parent) )
        {
            delete buttons;
            return false;
        }

        GetSizer()->Add(
            buttons,
            wxSizerFlags().Expand().DoubleBorder(wxLEFT|wxRIGHT|wxBOTTOM));
#endif

        wxNotebook* const liveNotebook = GetLiveNotebook();
        if ( !liveNotebook || !HasExactOwner(parent) )
            return false;

        liveNotebook->SetFocus();

        return wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) &&
               HasExactOwner(parent) && GetLiveNotebook() == liveNotebook;
    }

    // Every preferences session owns the dialog only while both its logical
    // parent and the identity passed to Create() remain unchanged. Reparenting
    // a top-level window transfers its ownership: detach our old observer at
    // the first transaction check and let the new owner keep the dialog.
    bool HasExactOwnerIdentity(wxWindow* expectedOwner) const
    {
        return expectedOwner == m_expectedOwnerIdentity &&
               GetParent() == expectedOwner &&
               (!expectedOwner || m_expectedOwner.get() == expectedOwner);
    }

    bool HasExactOwner(wxWindow* expectedOwner)
    {
        const bool exact = HasExactOwnerIdentity(expectedOwner) &&
            (!expectedOwner ||
             wxWeakWindowIsAvailableForCallbacks(m_expectedOwner,
                                                  expectedOwner));
        if ( !exact )
            StopObservingOwner();

        return exact;
    }

    bool HasItsExpectedOwner()
    {
        return HasExactOwner(m_expectedOwnerIdentity);
    }

    bool CanBeDeletedByOwnerTransaction(wxWindow* expectedOwner)
    {
        // Rollback owns the half-built dialog as long as its weak identity and
        // parent topology are unchanged. A destroy-scheduled owner is no
        // longer callback-safe, but that is not an ownership transfer and
        // must not leak the candidate until physical owner teardown.
        if ( HasExactOwnerIdentity(expectedOwner) )
            return true;

        StopObservingOwner();

        // A failed native Create() can leave a default-constructed object with
        // neither handle nor parent. It has not been transferred anywhere and
        // must still be reclaimed by its allocating transaction. Conversely a
        // live TLW reparented to nullptr is present in wxTopLevelWindows and is
        // intentionally left to its new ownership.
        return !GetHandle() && !GetParent();
    }

    template <typename IsTransactionValid>
    bool AddPage(wxPreferencesPage *page,
                 const IsTransactionValid& isTransactionValid)
    {
        wxNotebook* const notebook = GetLiveNotebook();
        if ( !notebook || !isTransactionValid() )
            return false;

        const wxWeakRef<wxGenericPrefsDialog> dialogLifetime(this);
        const wxWeakRef<wxNotebook> notebookLifetime(notebook);

        wxWindow * const returnedWindow = page->CreateWindow(notebook);
        if ( !isTransactionValid() ||
             !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) ||
             GetLiveNotebook() != notebook ||
             notebookLifetime.get() != notebook )
        {
            return false;
        }

        wxCHECK_MSG( returnedWindow, false,
                     "wxPreferencesPage::CreateWindow() returned null" );

        // CreateWindow() is arbitrary application code. Its raw return value
        // can already be stale, or the object can have been transferred to a
        // different parent. Do not even construct a weak reference from it
        // until the live notebook proves that this exact address is one of
        // its current children. In the invalid case we must not delete the raw
        // value either: a different parent may now own it.
        wxWindow* pageWindow = nullptr;
        for ( wxWindow* const child : notebook->GetChildren() )
        {
            if ( child == returnedWindow )
            {
                pageWindow = child;
                break;
            }
        }

        if ( !pageWindow )
        {
            wxFAIL_MSG(
                "wxPreferencesPage::CreateWindow() must return a live child "
                "of its parent argument" );
            return false;
        }

        const wxWeakRef<wxWindow> pageWindowLifetime(pageWindow);
        if ( !wxWeakWindowIsAvailableForCallbacks(pageWindowLifetime,
                                                   pageWindow) ||
             pageWindow->GetParent() != notebook )
        {
            return false;
        }

        const wxString name = page->GetName();
        if ( !isTransactionValid() ||
             !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) ||
             GetLiveNotebook() != notebook ||
             notebookLifetime.get() != notebook ||
             !wxWeakWindowIsAvailableForCallbacks(pageWindowLifetime,
                                                   pageWindow) ||
             pageWindow->GetParent() != notebook )
        {
            return false;
        }

        const bool added = notebook->AddPage(pageWindow, name);
        return added && isTransactionValid() &&
               wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) &&
               GetLiveNotebook() == notebook &&
               notebookLifetime.get() == notebook &&
               wxWeakWindowIsAvailableForCallbacks(pageWindowLifetime,
                                                    pageWindow) &&
               pageWindow->GetParent() == notebook;
    }

#ifndef wxHAS_PREF_EDITOR_MODELESS
    template <typename IsTransactionValid>
    int GetSelectedPage(const IsTransactionValid& isTransactionValid) const
    {
        wxNotebook* const notebook = GetLiveNotebook();
        if ( !notebook || !isTransactionValid() )
            return wxNOT_FOUND;

        const int selection = notebook->GetSelection();
        if ( !isTransactionValid() || GetLiveNotebook() != notebook )
            return wxNOT_FOUND;

        return selection;
    }

    template <typename IsTransactionValid>
    bool SelectPage(int page, const IsTransactionValid& isTransactionValid)
    {
        wxNotebook* const notebook = GetLiveNotebook();
        if ( !notebook || !isTransactionValid() || page < 0 ||
             static_cast<size_t>(page) >= notebook->GetPageCount() )
        {
            return false;
        }

        notebook->ChangeSelection(page);
        return isTransactionValid() && GetLiveNotebook() == notebook &&
               notebook->GetSelection() == page;
    }
#endif // !wxHAS_PREF_EDITOR_MODELESS

     bool ShouldPreventAppExit() const override
     {
         return false;
     }

    template <typename IsTransactionValid>
    bool FitPages(const IsTransactionValid& isTransactionValid)
    {
        const wxWeakRef<wxGenericPrefsDialog> dialogLifetime(this);
        wxNotebook* const notebook = GetLiveNotebook();
        wxSizer* const sizer = GetSizer();
        if ( !notebook || !sizer || !isTransactionValid() )
            return false;

        const wxSize minSize = sizer->GetMinSize();
        if ( !isTransactionValid() ||
             !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) ||
             GetLiveNotebook() != notebook ||
             GetSizer() != sizer )
            return false;

        SetClientSize(minSize);
        return isTransactionValid() &&
               wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) &&
               GetLiveNotebook() == notebook && GetSizer() == sizer;
    }

private:
    void StopObservingOwner()
    {
        m_ownerObserver.reset();
        m_expectedOwner.Release();
        m_expectedOwnerIdentity = nullptr;
    }

    wxNotebook* GetLiveNotebook() const
    {
        wxNotebook* const notebook = m_notebookLifetime.get();
        if ( notebook != m_notebook ||
             !wxWeakWindowIsAvailableForCallbacks(m_notebookLifetime,
                                                   notebook) ||
             notebook->GetParent() != this )
        {
            return nullptr;
        }

        return notebook;
    }

    wxNotebook* m_notebook { nullptr };
    wxWeakRef<wxNotebook> m_notebookLifetime;
    std::unique_ptr<wxGenericPrefsOwnerObserver> m_ownerObserver;
    wxWeakRef<wxWindow> m_expectedOwner;
    wxWindow* m_expectedOwnerIdentity { nullptr };
};


wxGenericPrefsOwnerObserver::~wxGenericPrefsOwnerObserver()
{
    Disconnect();
}

bool wxGenericPrefsOwnerObserver::BindTo(wxWindow* owner,
                                         wxGenericPrefsDialog* dialog)
{
    if ( wxWindowIsUnavailableForCallbacks(owner) ||
         wxWindowIsUnavailableForCallbacks(dialog) )
    {
        return false;
    }

    m_owner = owner;
    m_ownerIdentity = owner;
    m_dialog = dialog;
    owner->Bind(wxEVT_DESTROY,
                &wxGenericPrefsOwnerObserver::OnDestroy,
                this);
    return true;
}

void wxGenericPrefsOwnerObserver::Disconnect()
{
    wxWindow* const owner = m_owner.get();

    // Publish the disconnected state before Unbind(): even though Unbind()
    // normally doesn't dispatch application code, the observer must already
    // be inert if teardown destroys its dialog or event source reentrantly.
    m_owner.Release();
    m_ownerIdentity = nullptr;
    m_dialog.Release();

    if ( owner && !wxWindowItselfIsUnavailableForCallbacks(owner) )
    {
        owner->Unbind(wxEVT_DESTROY,
                      &wxGenericPrefsOwnerObserver::OnDestroy,
                      this);
    }
}

void wxGenericPrefsOwnerObserver::OnDestroy(wxWindowDestroyEvent& event)
{
    event.Skip();
    if ( event.GetWindow() != m_ownerIdentity )
        return;

    wxGenericPrefsDialog* const dialog = m_dialog.get();
    const bool stillOwnsDialog =
        dialog && dialog->GetParent() == m_ownerIdentity;

    // This callback can synchronously hide or destroy the dialog, and doing so
    // destroys this observer with it. Release every observable member before
    // crossing that boundary and deliberately never touch `this` afterwards.
    m_dialog.Release();
    m_owner.Release();
    m_ownerIdentity = nullptr;

    if ( stillOwnsDialog &&
         !wxWindowItselfIsUnavailableForCallbacks(dialog) )
    {
        if ( dialog->IsModal() )
            dialog->EndModal(wxID_CANCEL);
        else
            dialog->Destroy();
    }
}


class wxGenericPreferencesEditorImplBase : public wxPreferencesEditorImpl
{
public:
    void SetTitle(const wxString& title)
    {
        m_title = title;
    }

    virtual void AddPage(wxPreferencesPage* page) override
    {
        m_pages.emplace_back(page);
    }

protected:
    void OnOwnerDestroyed() override
    {
        // This is essential for the modal implementation: deleting the public
        // editor from a page callback must end ShowModal() before its final
        // owning reference can be released.
        Dismiss();
    }

    wxGenericPrefsDialog *CreateDialog(
        wxWindow *parent,
        wxWeakRef<wxGenericPrefsDialog>& activeDialog)
    {
        if ( !IsOwnerAlive() ||
             (parent && wxWindowIsUnavailableForCallbacks(parent)) )
            return nullptr;

        const wxWeakRef<wxWindow> parentLifetime(parent);

        if ( m_title.empty() )
        {
            // Use the default title, which should include the application name
            // under both MSW and GTK (and OSX uses its own native
            // implementation anyhow).
            if ( wxTheApp )
                m_title.Printf(_("%s Preferences"),
                               wxTheApp->GetAppDisplayName());
            else
                m_title = _("Preferences");
        }

        wxGenericPrefsDialog * const dlg = new wxGenericPrefsDialog;
        const wxWeakRef<wxGenericPrefsDialog> dialogLifetime(dlg);
        wxScopeGuard rollbackDialog = wxMakeGuard(
            [&activeDialog, dialogLifetime, dlg, parent]()
            {
                if ( activeDialog.get() == dlg )
                    activeDialog.Release();

                wxGenericPrefsDialog* const live = dialogLifetime.get();
                if ( live &&
                     !wxWindowItselfIsUnavailableForCallbacks(live) &&
                     live->CanBeDeletedByOwnerTransaction(parent) )
                {
                    delete live;
                }
            });

        if ( !dlg->Create(parent, m_title) ||
             !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dlg) ||
             !IsOwnerAlive() ||
             (parent &&
              !wxWeakWindowIsAvailableForCallbacks(parentLifetime,
                                                    parent)) ||
             !dlg->HasExactOwner(parent) )
        {
            return nullptr;
        }

        // Publish the dialog before invoking any page callback so Dismiss()
        // can abort this exact half-built session. Show() itself is guarded by
        // the derived implementation's session state and must not expose an
        // incomplete notebook.
        activeDialog = dlg;

        // TODO: Don't create all pages immediately like this, do it on demand
        //       when a page is selected in the notebook (as is done on OS X).
        //
        //       Currently, creating all pages is necessary so that the notebook
        //       can determine its best size. We'll need to extend
        //       wxPreferencesPage with a GetBestSize() virtual method to make
        //       it possible to defer the creation.
        std::vector<wxPreferencesPage*> pages;
        pages.reserve(m_pages.size());
        for ( const auto& page : m_pages )
            pages.push_back(page.get());

        // Use a stable snapshot because CreateWindow() and GetName() are
        // application callbacks and may add another page to this editor.
        const auto transactionIsValid =
            [this, dlg, parent, &activeDialog, dialogLifetime,
             parentLifetime]()
            {
                return IsOwnerAlive() &&
                       wxWeakWindowIsAvailableForCallbacks(dialogLifetime,
                                                           dlg) &&
                       activeDialog.get() == dlg &&
                       (!parent ||
                        wxWeakWindowIsAvailableForCallbacks(parentLifetime,
                                                            parent)) &&
                       dlg->HasExactOwner(parent);
            };
        for ( wxPreferencesPage* const page : pages )
        {
            if ( !dlg->AddPage(page, transactionIsValid) ||
                 !transactionIsValid() )
                return nullptr;
        }

        if ( !dlg->FitPages(transactionIsValid) || !transactionIsValid() )
            return nullptr;

        rollbackDialog.Dismiss();
        return dlg;
    }

    bool Activate(wxGenericPrefsDialog* dlg, wxWindow* expectedOwner)
    {
        if ( !IsOwnerAlive() || !dlg ||
             wxWindowIsUnavailableForCallbacks(dlg) ||
             !dlg->HasExactOwner(expectedOwner) )
        {
            return false;
        }

        const wxWeakRef<wxGenericPrefsDialog> dialogLifetime(dlg);
        dlg->Show();
        if ( IsOwnerAlive() &&
             wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dlg) &&
             dialogLifetime->HasExactOwner(expectedOwner) &&
             dialogLifetime->IsShown() )
        {
            dialogLifetime->Raise();
            return IsOwnerAlive() &&
                   wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dlg) &&
                   dialogLifetime->HasExactOwner(expectedOwner);
        }

        return false;
    }

    std::vector<std::unique_ptr<wxPreferencesPage>> m_pages;

private:
    wxString m_title;
};


#ifdef wxHAS_PREF_EDITOR_MODELESS

class wxModelessPreferencesEditorImpl : public wxGenericPreferencesEditorImplBase
{
public:
    virtual ~wxModelessPreferencesEditorImpl()
    {
        // m_win may already be destroyed if this destructor is called from
        // wxApp's destructor. In that case, all windows -- including this
        // one -- would already be destroyed by now.
        wxGenericPrefsDialog* const dialog = m_win.get();
        const wxWeakRef<wxGenericPrefsDialog> dialogLifetime(dialog);
        m_win.Release();
        if ( wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) &&
             dialogLifetime->HasItsExpectedOwner() )
        {
            dialogLifetime->Destroy();
        }
    }

    virtual void Show(wxWindow* parent) override
    {
        if ( m_dismissing || !IsOwnerAlive() )
            return;

        // In particular on GTK this generic implementation is modeless: the
        // same-owner hidden dialog is shown again, while changing owner is a
        // destroy-and-create transaction. Capture the requested owner before
        // either Destroy() or Show() can dispatch application code.
        const wxWeakRef<wxWindow> parentLifetime(parent);
        if ( parent &&
             !wxWeakWindowIsAvailableForCallbacks(parentLifetime, parent) )
        {
            return;
        }

        // CreateDialog() publishes m_win so a callback can dismiss it, but a
        // nested Show() must never make the half-built dialog visible.
        if ( m_creating )
            return;

        if ( wxGenericPrefsDialog* const existing = m_win.get() )
        {
            if ( wxWindowIsUnavailableForCallbacks(existing) )
            {
                m_win.Release();
            }
            else if ( !existing->HasItsExpectedOwner() )
            {
                // The application transferred this TLW. Stop this Show()
                // transaction and never destroy or reclaim the new owner's
                // dialog.
                m_win.Release();
                return;
            }
            // A nested Show() and a reopen for the same owner reuse the one
            // dialog. A hidden session requested by another owner is rebuilt:
            // reparenting an existing TLW isn't a portable operation.
            else if ( existing->IsShown() || existing->GetParent() == parent )
            {
                Activate(existing, existing->GetParent());
                return;
            }
        }

        // This guard is the transaction gate for the complete replacement,
        // including destruction of the previous hidden dialog. Destroy() can
        // dispatch application code: a nested Show() must see m_creating and
        // return instead of publishing a replacement which this outer call
        // would subsequently forget and overwrite with a second dialog.
        m_creating = true;
        wxON_BLOCK_EXIT_SET(m_creating, false);

        if ( m_win && !wxWindowIsUnavailableForCallbacks(m_win.get()) )
        {
            const wxWeakRef<wxGenericPrefsDialog> previous(m_win);
            if ( !previous->HasItsExpectedOwner() )
            {
                m_win.Release();
                return;
            }

            m_win.Release();
            previous->Destroy();
            if ( !IsOwnerAlive() ||
                 (parent &&
                  !wxWeakWindowIsAvailableForCallbacks(parentLifetime,
                                                        parent)) )
            {
                return;
            }
        }

        m_win.Release();

        wxGenericPrefsDialog * const dlg = CreateDialog(parent, m_win);
        if ( !dlg )
            return;

        const wxWeakRef<wxGenericPrefsDialog> dialogLifetime(dlg);
        wxScopeGuard rollbackDialog = wxMakeGuard(
            [this, dialogLifetime, dlg, parent]()
            {
                if ( m_win.get() == dlg )
                    m_win.Release();

                wxGenericPrefsDialog* const live = dialogLifetime.get();
                if ( live &&
                     !wxWindowItselfIsUnavailableForCallbacks(live) &&
                     live->HasExactOwner(parent) )
                {
                    live->Destroy();
                }
            });

        if ( !Activate(dlg, parent) ||
             m_win.get() != dlg || !IsOwnerAlive() ||
             (parent &&
              !wxWeakWindowIsAvailableForCallbacks(parentLifetime, parent)) ||
             !dlg->HasExactOwner(parent) )
        {
            return;
        }

        rollbackDialog.Dismiss();
    }

    virtual void Dismiss() override
    {
        if ( m_dismissing )
            return;

        const wxWeakRef<wxGenericPrefsDialog> dialogLifetime(m_win);
        wxGenericPrefsDialog* const dlg = dialogLifetime.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dlg) ||
             !dialogLifetime->HasItsExpectedOwner() )
        {
            m_win.Release();
            return;
        }

        if ( m_creating )
        {
            // Dismiss() from CreateWindow()/GetName() aborts publication of
            // the half-built session; merely hiding it would let the outer
            // Show() make it visible again when construction resumes.
            m_win.Release();
            dialogLifetime->Destroy();
            return;
        }

        m_dismissing = true;
        wxON_BLOCK_EXIT_SET(m_dismissing, false);
        dialogLifetime->Hide();
    }

private:
    wxWeakRef<wxGenericPrefsDialog> m_win;
    bool m_creating { false };
    bool m_dismissing { false };
};

inline
wxGenericPreferencesEditorImplBase* NewGenericImpl()
{
    return new wxModelessPreferencesEditorImpl;
}

#else // !wxHAS_PREF_EDITOR_MODELESS

class wxModalPreferencesEditorImpl : public wxGenericPreferencesEditorImplBase
{
public:
    wxModalPreferencesEditorImpl() = default;

    virtual void Show(wxWindow* parent) override
    {
        if ( m_dismissing || !IsOwnerAlive() )
            return;

        const wxWeakRef<wxWindow> parentLifetime(parent);
        if ( parent &&
             !wxWeakWindowIsAvailableForCallbacks(parentLifetime, parent) )
        {
            return;
        }

        // m_dlg is published throughout construction and the complete modal
        // call. A nested Show() from wxEVT_INIT_DIALOG runs before the TLW is
        // physically shown: never call Show() again in that state (which would
        // dispatch InitDialog recursively). Once visible, raising is the only
        // permitted nested operation and cannot start another modal session.
        if ( m_state != SessionState::Idle )
        {
            wxGenericPrefsDialog* const active = m_dlg.get();
            if ( !active || wxWindowIsUnavailableForCallbacks(active) ||
                 !active->HasItsExpectedOwner() )
            {
                m_dlg.Release();
                return;
            }

            if ( active->IsShown() )
                active->Raise();
            return;
        }

        m_dlg.Release();
        m_state = SessionState::Creating;
        wxScopeGuard resetSession = wxMakeGuard(
            [this]()
            {
                m_preShowDismissScheduled = false;
                m_state = SessionState::Idle;
            });
        wxUnusedVar(resetSession);

        wxGenericPrefsDialog * const dlg = CreateDialog(parent, m_dlg);
        if ( !dlg )
            return;

        const wxWeakRef<wxGenericPrefsDialog> dialogLifetime(dlg);
        wxScopeGuard releaseDialog = wxMakeGuard(
            [this, dialogLifetime, dlg, parent]()
            {
                if ( m_dlg.get() == dlg )
                    m_dlg.Release();

                wxGenericPrefsDialog* const live = dialogLifetime.get();
                if ( live &&
                     !wxWindowItselfIsUnavailableForCallbacks(live) &&
                     live->HasExactOwner(parent) )
                {
                    delete live;
                }
            });
        wxUnusedVar(releaseDialog);

        const auto transactionIsValid =
            [this, dlg, parent, dialogLifetime, parentLifetime]()
            {
                return IsOwnerAlive() &&
                       m_state == SessionState::Creating &&
                       m_dlg.get() == dlg &&
                       wxWeakWindowIsAvailableForCallbacks(dialogLifetime,
                                                           dlg) &&
                       (!parent ||
                        wxWeakWindowIsAvailableForCallbacks(parentLifetime,
                                                            parent)) &&
                       dlg->HasExactOwner(parent);
            };

        // Restore the previously selected page, if any.
        if ( m_currentPage != -1 )
        {
            if ( !dlg->SelectPage(m_currentPage, transactionIsValid) )
                return;
        }

        if ( !transactionIsValid() )
            return;

        // Selection restoration can synchronously invoke platform callbacks.
        // Keep the explicit Creating state until it and its postcondition have
        // completed, then enter the sole modal call for this transaction.
        m_state = SessionState::Modal;
        ScheduleModalOwnerValidation(dialogLifetime);

        // Don't remember the last selected page if the dialog was cancelled.
        const int rc = dlg->ShowModal();
        if ( wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dlg) &&
             m_dlg.get() == dialogLifetime.get() &&
             dlg->HasExactOwner(parent) )
        {
            if ( rc != wxID_CANCEL && IsOwnerAlive() )
            {
                const auto selectionTransactionIsValid =
                    [this, dlg, parent, dialogLifetime, parentLifetime]()
                    {
                        return IsOwnerAlive() &&
                               m_state == SessionState::Modal &&
                               m_dlg.get() == dlg &&
                               wxWeakWindowIsAvailableForCallbacks(
                                   dialogLifetime, dlg) &&
                               (!parent ||
                                wxWeakWindowIsAvailableForCallbacks(
                                    parentLifetime, parent)) &&
                               dlg->HasExactOwner(parent);
                    };
                const int selection = dialogLifetime->GetSelectedPage(
                    selectionTransactionIsValid);
                if ( selection != wxNOT_FOUND )
                    m_currentPage = selection;
            }
        }
    }

    virtual void Dismiss() override
    {
        if ( m_dismissing )
            return;

        const wxWeakRef<wxGenericPrefsDialog> dialogLifetime(m_dlg);
        wxGenericPrefsDialog* const dlg = dialogLifetime.get();
        if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dlg) ||
             !dialogLifetime->HasItsExpectedOwner() )
        {
            m_dlg.Release();
            return;
        }

        m_dismissing = true;
        wxON_BLOCK_EXIT_SET(m_dismissing, false);

        if ( dialogLifetime->IsModal() && dialogLifetime->IsShown() )
        {
            // Keep m_dlg published across EndModal(): Hide() dispatches user
            // code, and a nested Show() must observe m_dismissing instead of
            // publishing a second preferences session.
            dialogLifetime->EndModal(wxID_CANCEL);
            return;
        }

        SchedulePreShowDismiss(dialogLifetime);
    }

private:
    enum class SessionState
    {
        Idle,
        Creating,
        Modal
    };

    void SchedulePreShowDismiss(
        const wxWeakRef<wxGenericPrefsDialog>& dialogLifetime)
    {
        if ( m_preShowDismissScheduled || !wxTheApp )
            return;

        wxGenericPrefsDialog* const dlg = dialogLifetime.get();
        if ( !dlg || !dlg->HasItsExpectedOwner() )
            return;

        wxWindow* const expectedOwner = dlg->GetParent();
        const wxWeakRef<wxWindow> ownerLifetime(expectedOwner);
        m_preShowDismissScheduled = true;

        // EndModal() called from wxEVT_INIT_DIALOG is too early: wxDialog::Show
        // has not physically shown the TLW yet and resumes showing it when the
        // handler returns. Queue exactly one weak request; the modal loop is
        // entered before this callback can run, and a transferred/destroyed
        // dialog makes it a no-op rather than affecting a later session.
        wxTheApp->CallAfter(
            [dialogLifetime, expectedOwner, ownerLifetime]()
            {
                wxGenericPrefsDialog* const live = dialogLifetime.get();
                if ( !live ||
                     wxWindowItselfIsUnavailableForCallbacks(live) ||
                     (expectedOwner &&
                      !wxWeakWindowIsAvailableForCallbacks(ownerLifetime,
                                                            expectedOwner)) ||
                     !live->HasExactOwner(expectedOwner) ||
                     !live->IsModal() )
                {
                    return;
                }

                live->EndModal(wxID_CANCEL);
            });
    }

    void ScheduleModalOwnerValidation(
        const wxWeakRef<wxGenericPrefsDialog>& dialogLifetime)
    {
        if ( !wxTheApp )
            return;

        wxGenericPrefsDialog* const dlg = dialogLifetime.get();
        if ( !dlg || !dlg->HasItsExpectedOwner() )
            return;

        wxWindow* const expectedOwner = dlg->GetParent();
        const wxWeakRef<wxWindow> ownerLifetime(expectedOwner);
        wxTheApp->CallAfter(
            [dialogLifetime, expectedOwner, ownerLifetime]()
            {
                wxGenericPrefsDialog* const live = dialogLifetime.get();
                if ( !live ||
                     wxWindowItselfIsUnavailableForCallbacks(live) )
                {
                    return;
                }

                const bool ownerIsLive =
                    !expectedOwner ||
                    wxWeakWindowIsAvailableForCallbacks(ownerLifetime,
                                                        expectedOwner);
                const bool ownerIsExact =
                    live->HasExactOwner(expectedOwner);
                if ( ownerIsLive && ownerIsExact )
                    return;

                // HasExactOwner() also releases the observer from the former
                // owner. Ending modality is the final callback boundary: the
                // transferred TLW itself remains owned by its new parent.
                if ( live->IsModal() )
                    live->EndModal(wxID_CANCEL);
            });
    }

    wxWeakRef<wxGenericPrefsDialog> m_dlg;
    int m_currentPage { -1 };
    SessionState m_state { SessionState::Idle };
    bool m_dismissing { false };
    bool m_preShowDismissScheduled { false };

    wxDECLARE_NO_COPY_CLASS(wxModalPreferencesEditorImpl);
};

inline
wxGenericPreferencesEditorImplBase* NewGenericImpl()
{
    return new wxModalPreferencesEditorImpl;
}

#endif // !wxHAS_PREF_EDITOR_MODELESS

} // anonymous namespace

/*static*/
wxPreferencesEditorImpl* wxPreferencesEditorImpl::Create(const wxString& title)
{
    wxGenericPreferencesEditorImplBase* const impl = NewGenericImpl();

    impl->SetTitle(title);

    return impl;
}

#endif // !wxHAS_PREF_EDITOR_NATIVE

#endif // wxUSE_PREFERENCES_EDITOR
