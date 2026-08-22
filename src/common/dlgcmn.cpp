/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dlgcmn.cpp
// Purpose:     common (to all ports) wxDialog functions
// Author:      Vadim Zeitlin
// Created:     28.06.99
// Copyright:   (c) Vadim Zeitlin
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

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/dcclient.h"
    #include "wx/intl.h"
    #include "wx/settings.h"
    #include "wx/stattext.h"
    #include "wx/sizer.h"
    #include "wx/containr.h"
#endif

#include "wx/statline.h"
#include "wx/sysopt.h"
#include "wx/module.h"
#include "wx/bookctrl.h"
#include "wx/scrolwin.h"
#include "wx/textwrapper.h"
#include "wx/modalhook.h"
#include "wx/private/dialogbuttonfactory.h"
#include "wx/private/windowlifetime.h"
#include "wx/scopeguard.h"

#include "wx/display.h"

#include <memory>
#include <cstdint>
#include <unordered_map>
#include <vector>

#if wxUSE_BUTTON
namespace
{

struct DialogButtonFactoryWindowPublication
{
    std::uint64_t writer { 0 };
    wxWindow* address { nullptr };
    wxWeakRef<wxWindow> lifetime;
};

struct DialogButtonFactoryAffirmativePublication
{
    std::uint64_t writer { 0 };
    int id { wxID_NONE };
};

struct DialogButtonFactoryState
{
    wxWeakRef<wxWindow> dialogLifetime;
    std::uint64_t epoch { 0 };
    std::uint64_t permanentDefaultWriter { 0 };
    std::uint64_t temporaryDefaultWriter { 0 };
    std::uint64_t focusWriter { 0 };
    std::uint64_t affirmativeWriter { 0 };
    DialogButtonFactoryWindowPublication permanentDefaultPublication;
    DialogButtonFactoryWindowPublication temporaryDefaultPublication;
    DialogButtonFactoryWindowPublication focusPublication;
    DialogButtonFactoryAffirmativePublication affirmativePublication;
};

enum class DialogButtonFactoryField
{
    PermanentDefault,
    TemporaryDefault,
    Focus,
    Affirmative
};

struct DialogButtonFactoryWriterTokens
{
    std::uint64_t transaction { 0 };
    std::uint64_t permanentDefault { 0 };
    std::uint64_t temporaryDefault { 0 };
    std::uint64_t focus { 0 };
    std::uint64_t affirmative { 0 };
    DialogButtonFactoryWindowPublication permanentDefaultPublication;
    DialogButtonFactoryWindowPublication temporaryDefaultPublication;
    DialogButtonFactoryWindowPublication focusPublication;
    DialogButtonFactoryAffirmativePublication affirmativePublication;
};

using DialogButtonFactoryStates =
    std::unordered_map<const wxDialogBase*, DialogButtonFactoryState>;

DialogButtonFactoryStates& GetDialogButtonFactoryStates()
{
    static DialogButtonFactoryStates* const states =
        new DialogButtonFactoryStates;
    return *states;
}

void PruneExpiredDialogButtonFactoryStates()
{
    DialogButtonFactoryStates& states = GetDialogButtonFactoryStates();
    for ( DialogButtonFactoryStates::iterator it = states.begin();
          it != states.end(); )
    {
        if ( !it->second.dialogLifetime.get() )
            it = states.erase(it);
        else
            ++it;
    }
}

std::uint64_t BeginDialogButtonFactoryTransaction(wxDialogBase* dialog)
{
    PruneExpiredDialogButtonFactoryStates();
    DialogButtonFactoryState& state =
        GetDialogButtonFactoryStates()[dialog];
    if ( state.dialogLifetime.get() != dialog )
    {
        state = DialogButtonFactoryState();
        state.dialogLifetime = wxWeakRef<wxWindow>(dialog);
    }
    return ++state.epoch;
}

bool IsCurrentDialogButtonFactoryTransaction(
    wxDialogBase* dialog,
    const wxWeakRef<wxWindow>& dialogLifetime,
    std::uint64_t epoch)
{
    if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) )
        return false;

    DialogButtonFactoryStates& states = GetDialogButtonFactoryStates();
    const DialogButtonFactoryStates::const_iterator it = states.find(dialog);
    return it != states.end() &&
           it->second.dialogLifetime.get() == dialog &&
           it->second.epoch == epoch;
}

std::uint64_t& GetDialogButtonFactoryFieldWriter(
    DialogButtonFactoryState& state,
    DialogButtonFactoryField field)
{
    switch ( field )
    {
        case DialogButtonFactoryField::PermanentDefault:
            return state.permanentDefaultWriter;
        case DialogButtonFactoryField::TemporaryDefault:
            return state.temporaryDefaultWriter;
        case DialogButtonFactoryField::Focus:
            return state.focusWriter;
        case DialogButtonFactoryField::Affirmative:
            return state.affirmativeWriter;
    }

    wxFAIL_MSG("invalid dialog button factory field");
    return state.epoch;
}

std::uint64_t GetDialogButtonFactoryFieldWriter(
    const DialogButtonFactoryState& state,
    DialogButtonFactoryField field)
{
    switch ( field )
    {
        case DialogButtonFactoryField::PermanentDefault:
            return state.permanentDefaultWriter;
        case DialogButtonFactoryField::TemporaryDefault:
            return state.temporaryDefaultWriter;
        case DialogButtonFactoryField::Focus:
            return state.focusWriter;
        case DialogButtonFactoryField::Affirmative:
            return state.affirmativeWriter;
    }

    wxFAIL_MSG("invalid dialog button factory field");
    return state.epoch;
}

DialogButtonFactoryWindowPublication&
GetDialogButtonFactoryWindowPublication(
    DialogButtonFactoryState& state,
    DialogButtonFactoryField field)
{
    switch ( field )
    {
        case DialogButtonFactoryField::PermanentDefault:
            return state.permanentDefaultPublication;
        case DialogButtonFactoryField::TemporaryDefault:
            return state.temporaryDefaultPublication;
        case DialogButtonFactoryField::Focus:
            return state.focusPublication;
        case DialogButtonFactoryField::Affirmative:
            break;
    }

    wxFAIL_MSG("affirmative field has no window publication");
    return state.focusPublication;
}

const DialogButtonFactoryWindowPublication&
GetDialogButtonFactoryWindowPublication(
    const DialogButtonFactoryState& state,
    DialogButtonFactoryField field)
{
    switch ( field )
    {
        case DialogButtonFactoryField::PermanentDefault:
            return state.permanentDefaultPublication;
        case DialogButtonFactoryField::TemporaryDefault:
            return state.temporaryDefaultPublication;
        case DialogButtonFactoryField::Focus:
            return state.focusPublication;
        case DialogButtonFactoryField::Affirmative:
            break;
    }

    wxFAIL_MSG("affirmative field has no window publication");
    return state.focusPublication;
}

void MarkDialogButtonFactoryWindowWriter(
    wxDialogBase* dialog,
    std::uint64_t epoch,
    DialogButtonFactoryField field,
    wxWindow* value)
{
    DialogButtonFactoryStates& states = GetDialogButtonFactoryStates();
    const DialogButtonFactoryStates::iterator it = states.find(dialog);
    if ( it != states.end() &&
         it->second.dialogLifetime.get() == dialog &&
         it->second.epoch == epoch )
    {
        GetDialogButtonFactoryFieldWriter(it->second, field) = epoch;
        DialogButtonFactoryWindowPublication& publication =
            GetDialogButtonFactoryWindowPublication(it->second, field);
        publication.writer = epoch;
        publication.address = value;
        publication.lifetime = wxWeakRef<wxWindow>(value);
    }
}

void MarkDialogButtonFactoryAffirmativeWriter(wxDialogBase* dialog,
                                              std::uint64_t epoch,
                                              int id)
{
    DialogButtonFactoryStates& states = GetDialogButtonFactoryStates();
    const DialogButtonFactoryStates::iterator it = states.find(dialog);
    if ( it != states.end() &&
         it->second.dialogLifetime.get() == dialog &&
         it->second.epoch == epoch )
    {
        it->second.affirmativeWriter = epoch;
        it->second.affirmativePublication.writer = epoch;
        it->second.affirmativePublication.id = id;
    }
}

bool IsDialogButtonFactoryFieldWriter(wxDialogBase* dialog,
                                      std::uint64_t epoch,
                                      DialogButtonFactoryField field)
{
    DialogButtonFactoryStates& states = GetDialogButtonFactoryStates();
    const DialogButtonFactoryStates::iterator it = states.find(dialog);
    return it != states.end() &&
           it->second.dialogLifetime.get() == dialog &&
           GetDialogButtonFactoryFieldWriter(it->second, field) == epoch;
}

DialogButtonFactoryWriterTokens SnapshotDialogButtonFactoryWriters(
    wxDialogBase* dialog)
{
    DialogButtonFactoryWriterTokens tokens;
    DialogButtonFactoryStates& states = GetDialogButtonFactoryStates();
    const DialogButtonFactoryStates::const_iterator it = states.find(dialog);
    if ( it != states.end() &&
         it->second.dialogLifetime.get() == dialog )
    {
        tokens.transaction = it->second.epoch;
        tokens.permanentDefault = it->second.permanentDefaultWriter;
        tokens.temporaryDefault = it->second.temporaryDefaultWriter;
        tokens.focus = it->second.focusWriter;
        tokens.affirmative = it->second.affirmativeWriter;
        tokens.permanentDefaultPublication =
            it->second.permanentDefaultPublication;
        tokens.temporaryDefaultPublication =
            it->second.temporaryDefaultPublication;
        tokens.focusPublication = it->second.focusPublication;
        tokens.affirmativePublication =
            it->second.affirmativePublication;
    }
    return tokens;
}

bool HaveSameDialogButtonFactoryWriters(
    const DialogButtonFactoryWriterTokens& lhs,
    const DialogButtonFactoryWriterTokens& rhs)
{
    return lhs.transaction == rhs.transaction &&
           lhs.permanentDefault == rhs.permanentDefault &&
           lhs.temporaryDefault == rhs.temporaryDefault &&
           lhs.focus == rhs.focus &&
           lhs.affirmative == rhs.affirmative;
}

void RestoreDialogButtonFactoryField(
    wxDialogBase* dialog,
    std::uint64_t epoch,
    DialogButtonFactoryField field,
    const DialogButtonFactoryWriterTokens& previous)
{
    DialogButtonFactoryStates& states = GetDialogButtonFactoryStates();
    const DialogButtonFactoryStates::iterator it = states.find(dialog);
    if ( it == states.end() ||
         it->second.dialogLifetime.get() != dialog ||
         GetDialogButtonFactoryFieldWriter(it->second, field) != epoch )
    {
        return;
    }

    switch ( field )
    {
        case DialogButtonFactoryField::PermanentDefault:
            it->second.permanentDefaultWriter = previous.permanentDefault;
            it->second.permanentDefaultPublication =
                previous.permanentDefaultPublication;
            break;
        case DialogButtonFactoryField::TemporaryDefault:
            it->second.temporaryDefaultWriter = previous.temporaryDefault;
            it->second.temporaryDefaultPublication =
                previous.temporaryDefaultPublication;
            break;
        case DialogButtonFactoryField::Focus:
            it->second.focusWriter = previous.focus;
            it->second.focusPublication = previous.focusPublication;
            break;
        case DialogButtonFactoryField::Affirmative:
            it->second.affirmativeWriter = previous.affirmative;
            it->second.affirmativePublication =
                previous.affirmativePublication;
            break;
    }
}

bool IsCurrentDialogButtonFactoryWindowPublication(
    wxDialogBase* dialog,
    DialogButtonFactoryField field,
    const DialogButtonFactoryWindowPublication& expected)
{
    DialogButtonFactoryStates& states = GetDialogButtonFactoryStates();
    const DialogButtonFactoryStates::const_iterator it = states.find(dialog);
    if ( it == states.end() ||
         it->second.dialogLifetime.get() != dialog ||
         !expected.writer ||
         GetDialogButtonFactoryFieldWriter(it->second, field) !=
             expected.writer )
    {
        return false;
    }

    const DialogButtonFactoryWindowPublication& current =
        GetDialogButtonFactoryWindowPublication(it->second, field);
    return current.writer == expected.writer &&
           current.address == expected.address &&
           current.lifetime.get() == expected.address;
}

void RepairCurrentDialogButtonFactoryPublications(wxDialogBase* dialog)
{
    const wxWeakRef<wxWindow> dialogLifetime(dialog);
    const auto getPublishedWindow =
        [&](DialogButtonFactoryField field,
            const DialogButtonFactoryWindowPublication& publication)
            -> wxWindow*
    {
        if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) ||
             !IsCurrentDialogButtonFactoryWindowPublication(
                 dialog, field, publication) )
        {
            return nullptr;
        }

        wxWindow* const window = publication.lifetime.get();
        return window == publication.address &&
                       !wxWindowIsUnavailableForCallbacks(window)
                   ? window
                   : nullptr;
    };

    // A focus repair is itself an application callback boundary. If it starts
    // another factory, its native continuation can once again publish the
    // older temporary default. Retry from the newer writer snapshot, but keep
    // the repair bounded even under deliberately hostile focus handlers.
    constexpr unsigned MaxRepairPasses = 8;
    for ( unsigned pass = 0; pass != MaxRepairPasses; ++pass )
    {
        if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) )
            return;

        const DialogButtonFactoryWriterTokens published =
            SnapshotDialogButtonFactoryWriters(dialog);

        const DialogButtonFactoryAffirmativePublication& affirmative =
            published.affirmativePublication;
        if ( affirmative.writer &&
             published.affirmative == affirmative.writer &&
             dialog->GetAffirmativeId() != affirmative.id )
        {
            dialog->SetAffirmativeId(affirmative.id);
        }

        if ( wxWindow* const permanent = getPublishedWindow(
                 DialogButtonFactoryField::PermanentDefault,
                 published.permanentDefaultPublication) )
        {
            dialog->SetDefaultItem(permanent);
        }

        if ( wxWindow* const focus = getPublishedWindow(
                 DialogButtonFactoryField::Focus,
                 published.focusPublication) )
        {
            if ( wxWindow::FindFocus() != focus )
            {
                wxTRY
                {
                    focus->SetFocus();
                }
                wxCATCH_ALL(;)

                if ( !wxWeakWindowIsAvailableForCallbacks(
                         dialogLifetime, dialog) )
                {
                    return;
                }

                const DialogButtonFactoryWriterTokens afterFocus =
                    SnapshotDialogButtonFactoryWriters(dialog);
                if ( !HaveSameDialogButtonFactoryWriters(published,
                                                         afterFocus) )
                {
                    continue;
                }

                // The writer is stable but focus could have been refused or
                // thrown. Don't manufacture a temporary-default state which
                // claims a focus publication that never became observable.
                if ( wxWindow::FindFocus() != focus )
                    return;
            }
        }

        // Focus publication may update the temporary default itself. Reassert
        // the exact latest writer last so a stale SetFocus() continuation can't
        // leave the committed nested factory with a null/older value.
        if ( wxWindow* const temporary = getPublishedWindow(
                 DialogButtonFactoryField::TemporaryDefault,
                 published.temporaryDefaultPublication) )
        {
            dialog->SetTmpDefaultItem(temporary);
        }
        return;
    }

    // No more callback boundaries after the retry budget. A final snapshot can
    // still repair the common stale-temporary-default case when the newest
    // factory already owns the actual focus, without permitting an unbounded
    // focus/factory recursion.
    if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) )
        return;

    const DialogButtonFactoryWriterTokens published =
        SnapshotDialogButtonFactoryWriters(dialog);
    wxWindow* const focus = getPublishedWindow(
        DialogButtonFactoryField::Focus,
        published.focusPublication);
    wxWindow* const temporary = getPublishedWindow(
        DialogButtonFactoryField::TemporaryDefault,
        published.temporaryDefaultPublication);
    if ( focus && temporary && wxWindow::FindFocus() == focus )
        dialog->SetTmpDefaultItem(temporary);
}

void RestoreDialogButtonFactoryFieldWriter(
    wxDialogBase* dialog,
    std::uint64_t epoch,
    DialogButtonFactoryField field,
    std::uint64_t previousWriter)
{
    DialogButtonFactoryStates& states = GetDialogButtonFactoryStates();
    const DialogButtonFactoryStates::iterator it = states.find(dialog);
    if ( it != states.end() &&
         it->second.dialogLifetime.get() == dialog &&
         GetDialogButtonFactoryFieldWriter(it->second, field) == epoch )
    {
        GetDialogButtonFactoryFieldWriter(it->second, field) =
            previousWriter;
    }
}

size_t CountExactSizerWindowItems(wxSizer* sizer, const wxWindow* window)
{
    size_t count = 0;
    if ( sizer && window )
    {
        for ( wxSizerItem* const item : sizer->GetChildren() )
        {
            if ( item->GetWindow() == window )
                ++count;
        }
    }
    return count;
}

void DetachAllExactSizerWindowItems(wxSizer* sizer, wxWindow* window)
{
    while ( CountExactSizerWindowItems(sizer, window) != 0 )
    {
        if ( !sizer->Detach(window) )
            break;
    }
}

} // anonymous namespace

wxPrivate::DialogButtonWriterSnapshot
wxPrivate::SnapshotDialogButtonWriters(wxDialogBase* dialog)
{
    const DialogButtonFactoryWriterTokens internal =
        SnapshotDialogButtonFactoryWriters(dialog);
    DialogButtonWriterSnapshot snapshot;
    snapshot.permanentDefault = internal.permanentDefault;
    snapshot.temporaryDefault = internal.temporaryDefault;
    snapshot.focus = internal.focus;
    snapshot.affirmative = internal.affirmative;
    return snapshot;
}

void wxPrivate::RestoreDialogButtonWriter(
    wxDialogBase* dialog,
    DialogButtonWriterField field,
    std::uint64_t expectedWriter,
    std::uint64_t previousWriter)
{
    DialogButtonFactoryField internalField =
        DialogButtonFactoryField::PermanentDefault;
    switch ( field )
    {
        case DialogButtonWriterField::PermanentDefault:
            break;
        case DialogButtonWriterField::TemporaryDefault:
            internalField = DialogButtonFactoryField::TemporaryDefault;
            break;
        case DialogButtonWriterField::Focus:
            internalField = DialogButtonFactoryField::Focus;
            break;
        case DialogButtonWriterField::Affirmative:
            internalField = DialogButtonFactoryField::Affirmative;
            break;
    }

    RestoreDialogButtonFactoryFieldWriter(
        dialog, expectedWriter, internalField, previousWriter);
}
#endif // wxUSE_BUTTON

#if !wxUSE_BUTTON
wxPrivate::DialogButtonWriterSnapshot
wxPrivate::SnapshotDialogButtonWriters(wxDialogBase* WXUNUSED(dialog))
{
    return DialogButtonWriterSnapshot();
}

void wxPrivate::RestoreDialogButtonWriter(
    wxDialogBase* WXUNUSED(dialog),
    DialogButtonWriterField WXUNUSED(field),
    std::uint64_t WXUNUSED(expectedWriter),
    std::uint64_t WXUNUSED(previousWriter))
{
}
#endif // !wxUSE_BUTTON

extern WXDLLEXPORT_DATA(const char) wxDialogNameStr[] = "dialog";

// ----------------------------------------------------------------------------
// XTI
// ----------------------------------------------------------------------------

wxDEFINE_FLAGS( wxDialogStyle )
wxBEGIN_FLAGS( wxDialogStyle )
// new style border flags, we put them first to
// use them for streaming out
wxFLAGS_MEMBER(wxBORDER_SIMPLE)
wxFLAGS_MEMBER(wxBORDER_SUNKEN)
wxFLAGS_MEMBER(wxBORDER_DOUBLE)
wxFLAGS_MEMBER(wxBORDER_RAISED)
wxFLAGS_MEMBER(wxBORDER_STATIC)
wxFLAGS_MEMBER(wxBORDER_NONE)

// old style border flags
wxFLAGS_MEMBER(wxSIMPLE_BORDER)
wxFLAGS_MEMBER(wxSUNKEN_BORDER)
wxFLAGS_MEMBER(wxDOUBLE_BORDER)
wxFLAGS_MEMBER(wxRAISED_BORDER)
wxFLAGS_MEMBER(wxSTATIC_BORDER)
wxFLAGS_MEMBER(wxNO_BORDER)

// standard window styles
wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
wxFLAGS_MEMBER(wxCLIP_CHILDREN)

// dialog styles
wxFLAGS_MEMBER(wxSTAY_ON_TOP)
wxFLAGS_MEMBER(wxCAPTION)
wxFLAGS_MEMBER(wxSYSTEM_MENU)
wxFLAGS_MEMBER(wxRESIZE_BORDER)
wxFLAGS_MEMBER(wxCLOSE_BOX)
wxFLAGS_MEMBER(wxMAXIMIZE_BOX)
wxFLAGS_MEMBER(wxMINIMIZE_BOX)
wxEND_FLAGS( wxDialogStyle )

wxIMPLEMENT_DYNAMIC_CLASS_XTI(wxDialog, wxTopLevelWindow, "wx/dialog.h");

wxBEGIN_PROPERTIES_TABLE(wxDialog)
wxPROPERTY( Title, wxString, SetTitle, GetTitle, wxString(), \
           0 /*flags*/, wxT("Helpstring"), wxT("group"))

wxPROPERTY_FLAGS( WindowStyle, wxDialogStyle, long, SetWindowStyleFlag, \
                 GetWindowStyleFlag, wxEMPTY_PARAMETER_VALUE, 0 /*flags*/, \
                 wxT("Helpstring"), wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxEMPTY_HANDLERS_TABLE(wxDialog)

wxCONSTRUCTOR_6( wxDialog, wxWindow*, Parent, wxWindowID, Id, \
                wxString, Title, wxPoint, Position, wxSize, Size, long, WindowStyle)

// ----------------------------------------------------------------------------
// wxDialogBase
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxDialogBase, wxTopLevelWindow)
    EVT_BUTTON(wxID_ANY, wxDialogBase::OnButton)

    EVT_CLOSE(wxDialogBase::OnCloseWindow)

    EVT_CHAR_HOOK(wxDialogBase::OnCharHook)
wxEND_EVENT_TABLE()

wxDialogLayoutAdapter* wxDialogBase::sm_layoutAdapter = nullptr;
bool wxDialogBase::sm_layoutAdaptation = false;

wxDialogBase::wxDialogBase()
{
    m_returnCode = 0;
    m_affirmativeId = wxID_OK;
    m_escapeId = wxID_ANY;
    m_layoutAdaptationLevel = 3;
    m_layoutAdaptationDone = FALSE;
    m_layoutAdaptationMode = wxDIALOG_ADAPTATION_MODE_DEFAULT;

    // the dialogs have this flag on by default to prevent the events from the
    // dialog controls from reaching the parent frame which is usually
    // undesirable and can lead to unexpected and hard to find bugs
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS);
}

wxWindow *
wxDialogBase::CheckIfCanBeUsedAsParent(wxWindowMode modality,
                                       wxWindow *parent) const
{
    if ( !parent )
        return nullptr;

    extern WXDLLIMPEXP_DATA_BASE(wxList) wxPendingDelete;
    if ( wxPendingDelete.Member(parent) || parent->IsBeingDeleted() )
    {
        // this window is being deleted and we shouldn't create any children
        // under it
        return nullptr;
    }

    if ( parent->HasExtraStyle(wxWS_EX_TRANSIENT) )
    {
        // this window is not being deleted yet but it's going to disappear
        // soon so still don't parent this window under it
        return nullptr;
    }

    // This check is done for modal dialogs only because modeless dialogs can
    // be created before their parent is shown and only shown later.
    switch ( modality )
    {
        case wxWindowMode::Normal:
            break;

        case wxWindowMode::AppModal:
        case wxWindowMode::WindowModal:
            if ( !parent->IsShownOnScreen() )
            {
                // using hidden parent won't work correctly either
                return nullptr;
            }
            break;
    }

    if ( parent == this )
    {
        // not sure if this can really happen but it doesn't hurt to guard
        // against this clearly invalid situation
        return nullptr;
    }

    return parent;
}

wxWindow *
wxDialogBase::DoGetParentForDialog(wxWindowMode modality,
                                   wxWindow *parent,
                                   long style) const
{
    // creating a parent-less modal dialog will result (under e.g. wxGTK2)
    // in an unfocused dialog, so try to find a valid parent for it unless we
    // were explicitly asked not to
    if ( style & wxDIALOG_NO_PARENT )
        return nullptr;

    // first try the given parent
    if ( parent )
        parent = CheckIfCanBeUsedAsParent(modality, wxGetTopLevelParent(parent));

    // then the currently active window
    if ( !parent )
        parent = CheckIfCanBeUsedAsParent(modality,
                    wxGetTopLevelParent(wxGetActiveWindow()));

    // and finally the application main window
    if ( !parent )
        parent = CheckIfCanBeUsedAsParent(modality, wxApp::GetMainTopWindow());

    return parent;
}

#if wxUSE_STATTEXT

wxSizer *wxDialogBase::CreateTextSizer(const wxString& message, int widthMax)
{
    wxTextSizerWrapper wrapper(this);

    return CreateTextSizer(message, wrapper, widthMax);
}

wxSizer *wxDialogBase::CreateTextSizer(const wxString& message,
                                       wxTextSizerWrapper& wrapper,
                                       int widthMax)
{
    // I admit that this is complete bogus, but it makes
    // message boxes work for pda screens temporarily..
    const bool is_pda = wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA;
    if (is_pda)
    {
        widthMax = wxSystemSettings::GetMetric( wxSYS_SCREEN_X, this ) - 25;
    }

    return wrapper.CreateSizer(message, widthMax);
}

#endif // wxUSE_STATTEXT

wxSizer *wxDialogBase::CreateButtonSizer(long flags)
{
#if wxUSE_BUTTON

    return CreateStdDialogButtonSizer(flags);

#else // !wxUSE_BUTTON
    wxUnusedVar(flags);

    return nullptr;
#endif // wxUSE_BUTTON/!wxUSE_BUTTON
}

wxSizer *wxDialogBase::CreateSeparatedSizer(wxSizer *sizer)
{
    // Mac Human Interface Guidelines recommend not to use static lines as
    // grouping elements
#if wxUSE_STATLINE && !defined(__WXMAC__)
    wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);
    topsizer->Add(new wxStaticLine(this),
                   wxSizerFlags().Expand().DoubleBorder(wxBOTTOM));
    topsizer->Add(sizer, wxSizerFlags().Expand());
    sizer = topsizer;
#endif // wxUSE_STATLINE

    return sizer;
}

wxSizer *wxDialogBase::CreateSeparatedButtonSizer(long flags)
{
    wxSizer *sizer = CreateButtonSizer(flags);
    if ( !sizer )
        return nullptr;

    return CreateSeparatedSizer(sizer);
}

#if wxUSE_BUTTON

wxStdDialogButtonSizer *wxDialogBase::CreateStdDialogButtonSizer( long flags )
{
    struct CreatedButton
    {
        explicit CreatedButton(wxButton* button_)
            : button(button_), lifetime(button_)
        {
        }

        wxButton* button;
        wxWeakRef<wxWindow> lifetime;
    };

    const wxWeakRef<wxWindow> dialogLifetime(this);
    if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) )
        return nullptr;
    const std::uint64_t factoryEpoch =
        BeginDialogButtonFactoryTransaction(this);
    const DialogButtonFactoryWriterTokens originalWriterTokens =
        SnapshotDialogButtonFactoryWriters(this);
    std::vector<CreatedButton> buttons;
    buttons.reserve(7);
    wxStdDialogButtonSizer* const sizer =
        new wxStdDialogButtonSizer();
    const int originalAffirmativeId = GetAffirmativeId();
    // Snapshot the two underlying defaults separately. GetDefaultItem()
    // returns the temporary one when present and would lose the permanent
    // identity changed by wxButton::SetDefault() behind that override.
    wxWindow* const originalDefaultItem = m_winDefault.get();
    const wxWeakRef<wxWindow> originalDefaultLifetime(originalDefaultItem);
    wxWindow* const originalTmpDefaultItem = m_winTmpDefault.get();
    const wxWeakRef<wxWindow> originalTmpDefaultLifetime(
        originalTmpDefaultItem);
    wxWindow* const originalFocus = wxWindow::FindFocus();
    const wxWeakRef<wxWindow> originalFocusLifetime(originalFocus);
    wxButton* publishedDefaultButton = nullptr;
    bool focusPublished = false;
    bool focusBoundaryEntered = false;
    int publishedAffirmativeId = originalAffirmativeId;
    bool affirmativePublished = false;
    bool buttonsRealized = false;
    bool committed = false;
    bool finalFocusRollbackRequired = false;
    std::uint64_t finalFocusRollbackTransaction { 0 };

    const auto exactTopologyIsAvailable = [&]()
    {
        if ( !IsCurrentDialogButtonFactoryTransaction(
                 this, dialogLifetime, factoryEpoch) )
            return false;

        for ( const CreatedButton& tracked : buttons )
        {
            if ( !wxWeakWindowIsAvailableForCallbacks(
                     tracked.lifetime, tracked.button) ||
                 tracked.button->GetParent() != this ||
                 tracked.button->GetContainingSizer() !=
                     (buttonsRealized ? sizer : nullptr) ||
                 CountExactSizerWindowItems(sizer, tracked.button) !=
                     (buttonsRealized ? 1u : 0u) )
            {
                return false;
            }
        }

        return true;
    };

    const auto isLiveCreatedButton = [&](const wxWindow* candidate)
    {
        for ( const CreatedButton& tracked : buttons )
        {
            if ( tracked.button == candidate &&
                 tracked.lifetime.get() == candidate &&
                 !wxWindowItselfIsUnavailableForCallbacks(candidate) )
            {
                return true;
            }
        }

        return false;
    };

    // State restoration normally runs before structural cleanup. Keep one
    // final, generation-checked focus attempt after it as well: destroying the
    // provisional buttons/sizer can complete a pending WinUI host transition
    // and otherwise leave physical focus on the aborted factory even though
    // all dialog default identities have already been restored.
    const wxScopeGuard rollbackFinalFocus = wxMakeGuard([&]()
    {
        if ( committed || !finalFocusRollbackRequired || !originalFocus ||
             !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) ||
             !wxWeakWindowIsAvailableForCallbacks(
                 originalFocusLifetime, originalFocus) ||
             SnapshotDialogButtonFactoryWriters(this).transaction !=
                 finalFocusRollbackTransaction ||
             wxWindow::FindFocus() == originalFocus )
        {
            return;
        }

        wxTRY
        {
            originalFocus->SetFocus();
        }
        wxCATCH_ALL(;)

        if ( wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) &&
             SnapshotDialogButtonFactoryWriters(this).transaction ==
                 finalFocusRollbackTransaction &&
             wxWindow::FindFocus() == originalFocus )
        {
            RestoreDialogButtonFactoryField(
                this,
                factoryEpoch,
                DialogButtonFactoryField::Focus,
                originalWriterTokens);
            if ( m_winTmpDefault.get() == originalTmpDefaultItem )
            {
                RestoreDialogButtonFactoryField(
                    this,
                    factoryEpoch,
                    DialogButtonFactoryField::TemporaryDefault,
                    originalWriterTokens);
            }
        }
    });
    wxUnusedVar(rollbackFinalFocus);

    // Keep structural cleanup in its own guard, declared first so that the
    // state guard below runs before it. wxScopeGuard deliberately swallows an
    // escaping exception; independent guards ensure that even an unexpected
    // state-restore failure can never strand buttons or their sizer.
    const wxScopeGuard rollbackStructure = wxMakeGuard([&]()
    {
        if ( committed )
            return;

        // First detach every exact item published by this sizer. No button may
        // be destroyed while an item still points at it, and a button adopted
        // by another sizer is deliberately preserved for its latest owner.
        for ( const CreatedButton& tracked : buttons )
        {
            wxButton* const button =
                wxDynamicCast(tracked.lifetime.get(), wxButton);
            if ( button == tracked.button &&
                 !wxWindowItselfIsUnavailableForCallbacks(button) &&
                 button->GetContainingSizer() == sizer )
            {
                DetachAllExactSizerWindowItems(sizer, button);
            }
        }

        // A parent TLW may already be queued for destruction while its child
        // buttons are still individually live. Delete only autonomous exact
        // children still owned by this factory; any adopted button belongs to
        // its latest sizer/owner. Keep every destruction boundary local so a
        // user exception cannot skip the remaining cleanup.
        for ( std::vector<CreatedButton>::reverse_iterator it =
                  buttons.rbegin();
              it != buttons.rend();
              ++it )
        {
            wxButton* const button =
                wxDynamicCast(it->lifetime.get(), wxButton);
            if ( button == it->button &&
                 !wxWindowItselfIsUnavailableForCallbacks(button) &&
                 button->GetParent() == this &&
                 button->GetContainingSizer() == nullptr )
            {
                wxTRY
                {
                    delete button;
                }
                wxCATCH_ALL(;)
            }
        }

        wxTRY
        {
            delete sizer;
        }
        wxCATCH_ALL(;)
    });
    wxUnusedVar(rollbackStructure);

    const wxScopeGuard rollbackState = wxMakeGuard([&]()
    {
        if ( committed )
            return;

        // Restore only values which are still exactly the ones published by
        // this transaction. A nested callback that installed a newer focus,
        // default item, or affirmative id wins.
        if ( wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) )
        {
            const bool publishedButtonEscaped =
                isLiveCreatedButton(publishedDefaultButton) &&
                publishedDefaultButton->GetParent() != this;
            if ( focusBoundaryEntered &&
                 IsDialogButtonFactoryFieldWriter(
                     this, factoryEpoch,
                     DialogButtonFactoryField::Focus) &&
                 (wxWindow::FindFocus() == publishedDefaultButton ||
                  publishedButtonEscaped) &&
                 (!originalFocus ||
                  wxWeakWindowIsAvailableForCallbacks(
                      originalFocusLifetime, originalFocus)) )
            {
                finalFocusRollbackRequired = originalFocus != nullptr;
                finalFocusRollbackTransaction =
                    SnapshotDialogButtonFactoryWriters(this).transaction;
                if ( originalFocus )
                {
                    wxTRY
                    {
                        originalFocus->SetFocus();
                    }
                    wxCATCH_ALL(;)

                    // Check after the exception boundary: an override may
                    // restore focus and only then throw. Hand the writer token
                    // back only when the focus value really committed.
                    if ( wxWeakWindowIsAvailableForCallbacks(
                             dialogLifetime, this) &&
                         wxWindow::FindFocus() == originalFocus )
                    {
                        RestoreDialogButtonFactoryField(
                            this,
                            factoryEpoch,
                            DialogButtonFactoryField::Focus,
                            originalWriterTokens);
                    }

                    // Native focus restoration can also restore the
                    // temporary default before SetTmpDefaultItem() below is
                    // reached. Return that field's token as soon as its exact
                    // prior value is observable, but only if no newer writer
                    // replaced this transaction during the callback.
                    if ( wxWeakWindowIsAvailableForCallbacks(
                             dialogLifetime, this) &&
                         m_winTmpDefault.get() == originalTmpDefaultItem )
                    {
                        RestoreDialogButtonFactoryField(
                            this,
                            factoryEpoch,
                            DialogButtonFactoryField::TemporaryDefault,
                            originalWriterTokens);
                    }
                }
            }

            if ( wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) &&
                 publishedDefaultButton )
            {
                if ( m_winTmpDefault.get() == publishedDefaultButton &&
                     IsDialogButtonFactoryFieldWriter(
                         this, factoryEpoch,
                         DialogButtonFactoryField::TemporaryDefault) &&
                     (!originalTmpDefaultItem ||
                      wxWeakWindowIsAvailableForCallbacks(
                          originalTmpDefaultLifetime,
                          originalTmpDefaultItem)) )
                {
                    wxTRY
                    {
                        SetTmpDefaultItem(originalTmpDefaultItem);
                    }
                    wxCATCH_ALL(;)
                    if ( wxWeakWindowIsAvailableForCallbacks(
                             dialogLifetime, this) &&
                         m_winTmpDefault.get() == originalTmpDefaultItem )
                    {
                        RestoreDialogButtonFactoryField(
                            this,
                            factoryEpoch,
                            DialogButtonFactoryField::TemporaryDefault,
                            originalWriterTokens);
                    }
                }

                if ( wxWeakWindowIsAvailableForCallbacks(
                         dialogLifetime, this) &&
                     m_winDefault.get() == publishedDefaultButton &&
                     IsDialogButtonFactoryFieldWriter(
                         this, factoryEpoch,
                         DialogButtonFactoryField::PermanentDefault) &&
                     (!originalDefaultItem ||
                      wxWeakWindowIsAvailableForCallbacks(
                          originalDefaultLifetime,
                          originalDefaultItem)) )
                {
                    wxTRY
                    {
                        SetDefaultItem(originalDefaultItem);
                    }
                    wxCATCH_ALL(;)
                    if ( wxWeakWindowIsAvailableForCallbacks(
                             dialogLifetime, this) &&
                         m_winDefault.get() == originalDefaultItem )
                    {
                        RestoreDialogButtonFactoryField(
                            this,
                            factoryEpoch,
                            DialogButtonFactoryField::PermanentDefault,
                            originalWriterTokens);
                    }
                }
            }

            if ( wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) &&
                 affirmativePublished &&
                 IsDialogButtonFactoryFieldWriter(
                     this, factoryEpoch,
                     DialogButtonFactoryField::Affirmative) &&
                 GetAffirmativeId() == publishedAffirmativeId )
            {
                wxTRY
                {
                    SetAffirmativeId(originalAffirmativeId);
                }
                wxCATCH_ALL(;)
                if ( wxWeakWindowIsAvailableForCallbacks(
                         dialogLifetime, this) &&
                     GetAffirmativeId() == originalAffirmativeId )
                {
                    RestoreDialogButtonFactoryField(
                        this,
                        factoryEpoch,
                        DialogButtonFactoryField::Affirmative,
                        originalWriterTokens);
                }
            }
        }
    });
    wxUnusedVar(rollbackState);

    bool creationFailed = false;
    const auto createButton = [&](const wxWindowID id) -> wxButton*
    {
        if ( !exactTopologyIsAvailable() )
        {
            creationFailed = true;
            return nullptr;
        }

        wxButton* const button = new wxButton;
        buttons.emplace_back(button);
        const bool created = button->Create(this, id);
        if ( !created || !exactTopologyIsAvailable() )
        {
            creationFailed = true;
            return nullptr;
        }

        sizer->AddButton(button);
        return button;
    };

    wxButton *ok = nullptr;
    wxButton *yes = nullptr;
    wxButton *no = nullptr;

    if (flags & wxOK)
        ok = createButton(wxID_OK);

    if (!creationFailed && (flags & wxCANCEL))
        (void)createButton(wxID_CANCEL);

    if (!creationFailed && (flags & wxYES))
        yes = createButton(wxID_YES);

    if (!creationFailed && (flags & wxNO))
        no = createButton(wxID_NO);

    if (!creationFailed && (flags & wxAPPLY))
        (void)createButton(wxID_APPLY);

    if (!creationFailed && (flags & wxCLOSE))
        (void)createButton(wxID_CLOSE);

    if (!creationFailed && (flags & wxHELP))
        (void)createButton(wxID_HELP);

    if ( creationFailed || !exactTopologyIsAvailable() )
        return nullptr;

    wxButton* defaultButton = nullptr;
    if (flags & wxNO_DEFAULT)
        defaultButton = no;
    else if (ok)
        defaultButton = ok;
    else if (yes)
        defaultButton = yes;

    // Publish the affirmative id before entering SetDefault()/SetFocus().
    // Their native callbacks may start a nested factory, which must snapshot
    // both this value and this transaction's writer token in order to return
    // them together if the nested publication subsequently rolls back.
    bool hasAffirmativeButton = false;
    if (flags & wxOK)
    {
        hasAffirmativeButton = true;
        publishedAffirmativeId = wxID_OK;
    }
    else if (flags & wxYES)
    {
        hasAffirmativeButton = true;
        publishedAffirmativeId = wxID_YES;
    }
    else if (flags & wxCLOSE)
    {
        hasAffirmativeButton = true;
        publishedAffirmativeId = wxID_CLOSE;
    }

    if ( hasAffirmativeButton )
    {
        // Claim the field even if the id is unchanged. A nested factory which
        // deliberately publishes the same affirmative id is still the newer
        // writer and an aborting outer factory must not restore over it.
        MarkDialogButtonFactoryAffirmativeWriter(
            this, factoryEpoch, publishedAffirmativeId);
        if ( publishedAffirmativeId != originalAffirmativeId )
            SetAffirmativeId(publishedAffirmativeId);
        if ( wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) )
        {
            affirmativePublished =
                GetAffirmativeId() == publishedAffirmativeId;
            if ( !affirmativePublished )
            {
                RestoreDialogButtonFactoryField(
                    this,
                    factoryEpoch,
                    DialogButtonFactoryField::Affirmative,
                    originalWriterTokens);
            }
        }
        if ( !exactTopologyIsAvailable() )
            return nullptr;
    }

    if ( defaultButton )
    {
        publishedDefaultButton = defaultButton;
        // Claim the field before entering the native/default callbacks. A
        // nested factory must snapshot this transaction as the previous
        // writer so that a failed nested publication can hand ownership back
        // to us together with the value it restores.
        MarkDialogButtonFactoryWindowWriter(
            this, factoryEpoch,
            DialogButtonFactoryField::PermanentDefault,
            defaultButton);
        defaultButton->SetDefault();
        if ( wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) &&
             m_winDefault.get() != defaultButton )
        {
            RestoreDialogButtonFactoryField(
                this,
                factoryEpoch,
                DialogButtonFactoryField::PermanentDefault,
                originalWriterTokens);
        }
        if ( !IsCurrentDialogButtonFactoryTransaction(
                 this, dialogLifetime, factoryEpoch) )
        {
            RepairCurrentDialogButtonFactoryPublications(this);
        }
        if ( !exactTopologyIsAvailable() )
            return nullptr;

        MarkDialogButtonFactoryWindowWriter(
            this, factoryEpoch, DialogButtonFactoryField::Focus,
            defaultButton);
        MarkDialogButtonFactoryWindowWriter(
            this, factoryEpoch,
            DialogButtonFactoryField::TemporaryDefault,
            defaultButton);
        focusBoundaryEntered = true;
        defaultButton->SetFocus();
        if ( wxWeakWindowIsAvailableForCallbacks(dialogLifetime, this) )
        {
            focusPublished = wxWindow::FindFocus() == defaultButton;
            const bool exactTopologyAfterFocus =
                exactTopologyIsAvailable();
            if ( !focusPublished && exactTopologyAfterFocus )
            {
                RestoreDialogButtonFactoryField(
                    this,
                    factoryEpoch,
                    DialogButtonFactoryField::Focus,
                    originalWriterTokens);
            }
#if defined(__WXWINUI__) && wxUSE_WINUI3
            else if ( focusPublished && exactTopologyAfterFocus &&
                      m_winTmpDefault.get() != defaultButton )
            {
                // Hosted XAML publishes physical focus synchronously without
                // necessarily running the native button path which sets the
                // temporary default. Preserve the wxMSW dialog contract
                // explicitly after the exact focus/topology commit.
                SetTmpDefaultItem(defaultButton);
            }
#endif
            if ( m_winTmpDefault.get() != defaultButton )
            {
                if ( exactTopologyAfterFocus )
                {
                    RestoreDialogButtonFactoryField(
                        this,
                        factoryEpoch,
                        DialogButtonFactoryField::TemporaryDefault,
                        originalWriterTokens);
                }
            }
        }
        if ( !IsCurrentDialogButtonFactoryTransaction(
                 this, dialogLifetime, factoryEpoch) )
        {
            // Native SetFocus() may resume after the nested factory returns
            // and republish the stale outer temporary default. Reassert only
            // values whose writer/publication pair is still the latest one.
            RepairCurrentDialogButtonFactoryPublications(this);
        }
        if ( !exactTopologyIsAvailable() )
            return nullptr;
    }

    sizer->Realize();
    buttonsRealized = true;
    if ( !exactTopologyIsAvailable() )
        return nullptr;

    committed = true;
    return sizer;
}

#endif // wxUSE_BUTTON

// ----------------------------------------------------------------------------
// standard buttons handling
// ----------------------------------------------------------------------------

void wxDialogBase::EndDialog(int rc)
{
    if ( IsModal() )
        EndModal(rc);
    else
        Hide();
}

void wxDialogBase::AcceptAndClose()
{
    const wxWeakRef<wxWindow> weakThis(this);
    const auto isAlive = [this, &weakThis]()
    {
        return wxWeakWindowIsAvailableForCallbacks(weakThis, this);
    };

    // A queued affirmative event can still be delivered after Destroy()
    // scheduled this TLW for deferred deletion.
    if ( !isAlive() )
        return;

    const bool wasModal = IsModal();
    const bool wasShown = IsShown();
    const int originalReturnCode = GetReturnCode();
    const int affirmativeId = GetAffirmativeId();
    const auto isSameDialogTransaction = [&]()
    {
        return isAlive() &&
               IsModal() == wasModal &&
               (!wasShown || IsShown()) &&
               GetReturnCode() == originalReturnCode;
    };

    const bool valid = Validate();
    if ( !isSameDialogTransaction() || !valid )
        return;

    const bool transferred = TransferDataFromWindow();
    if ( !isSameDialogTransaction() || !transferred )
        return;

    EndDialog(affirmativeId);
}

void wxDialogBase::SetAffirmativeId(int affirmativeId)
{
    m_affirmativeId = affirmativeId;
}

void wxDialogBase::SetEscapeId(int escapeId)
{
    m_escapeId = escapeId;
}

bool wxDialogBase::EmulateButtonClickIfPresent(int id)
{
#if wxUSE_BUTTON
    wxButton *btn = wxDynamicCast(FindWindow(id), wxButton);

    if ( !btn || !btn->IsEnabled() || !btn->IsShown() )
        return false;

    wxCommandEvent event(wxEVT_BUTTON, id);
    event.SetEventObject(btn);
    btn->GetEventHandler()->ProcessEvent(event);

    return true;
#else // !wxUSE_BUTTON
    wxUnusedVar(id);
    return false;
#endif // wxUSE_BUTTON/!wxUSE_BUTTON
}

bool wxDialogBase::SendCloseButtonClickEvent()
{
    int idCancel = GetEscapeId();
    switch ( idCancel )
    {
        case wxID_NONE:
            // The user doesn't want this dialog to close "implicitly".
            break;

        case wxID_ANY:
            // this value is special: it means translate Esc to wxID_CANCEL
            // but if there is no such button, then fall back to wxID_OK
            if ( EmulateButtonClickIfPresent(wxID_CANCEL) )
                return true;
            idCancel = GetAffirmativeId();
            wxFALLTHROUGH;

        default:
            // translate Esc to button press for the button with given id
            if ( EmulateButtonClickIfPresent(idCancel) )
                return true;
    }

    return false;
}

bool wxDialogBase::IsEscapeKey(const wxKeyEvent& event)
{
    // For most platforms, Esc key is used to close the dialogs.
    //
    // Notice that we intentionally don't check for modifiers here, Shift-Esc,
    // Alt-Esc and so on still close the dialog, typically.
    return event.GetKeyCode() == WXK_ESCAPE;
}

void wxDialogBase::OnCharHook(wxKeyEvent& event)
{
    if ( IsEscapeKey(event) )
    {
        if ( SendCloseButtonClickEvent() )
        {
            // Skip the call to event.Skip() below, we did handle this key.
            return;
        }
    }

    event.Skip();
}

void wxDialogBase::OnButton(wxCommandEvent& event)
{
    const int id = event.GetId();
    if ( id == GetAffirmativeId() )
    {
        AcceptAndClose();
    }
    else if ( id == wxID_APPLY )
    {
        const wxWeakRef<wxWindow> weakThis(this);
        const auto isAlive = [this, &weakThis]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this);
        };

        if ( !isAlive() )
            return;

        const bool wasModal = IsModal();
        const bool wasShown = IsShown();
        const int originalReturnCode = GetReturnCode();
        const auto isSameDialogTransaction = [&]()
        {
            return isAlive() &&
                   IsModal() == wasModal &&
                   (!wasShown || IsShown()) &&
                   GetReturnCode() == originalReturnCode;
        };

        const bool valid = Validate();
        if ( !isSameDialogTransaction() )
            return;

        if ( valid )
        {
            (void)TransferDataFromWindow();
            if ( !isSameDialogTransaction() )
                return;
        }

        // TODO: disable the Apply button until things change again
    }
    else if ( id == GetEscapeId() ||
                (id == wxID_CANCEL && GetEscapeId() == wxID_ANY) )
    {
        EndDialog(wxID_CANCEL);
    }
    else // not a standard button
    {
        event.Skip();
    }
}

// ----------------------------------------------------------------------------
// compatibility methods for supporting the modality API
// ----------------------------------------------------------------------------

wxDEFINE_EVENT( wxEVT_WINDOW_MODAL_DIALOG_CLOSED , wxWindowModalDialogEvent  );

wxIMPLEMENT_DYNAMIC_CLASS(wxWindowModalDialogEvent, wxCommandEvent);

void wxDialogBase::ShowWindowModal ()
{
    int retval = ShowModal();
    // wxWindowModalDialogEvent relies on GetReturnCode() returning correct
    // code. Rather than doing it manually in all ShowModal() overrides for
    // native dialogs (and getting accidentally broken again), set it here.
    // The worst that can happen is that it will be set twice to the same
    // value.
    SetReturnCode(retval);
    SendWindowModalDialogEvent ( wxEVT_WINDOW_MODAL_DIALOG_CLOSED  );
}

void wxDialogBase::SendWindowModalDialogEvent ( wxEventType type )
{
    wxWindowModalDialogEvent event ( type, GetId());
    event.SetEventObject(this);

    if ( !GetEventHandler()->ProcessEvent(event) )
    {
        // the event is not propagated upwards to the parent automatically
        // because the dialog is a top level window, so do it manually as
        // in 9 cases of 10 the message must be processed by the dialog
        // owner and not the dialog itself
        (void)GetParent()->GetEventHandler()->ProcessEvent(event);
    }
}


wxWindowMode wxDialogBase::GetModality() const
{
    return IsModal() ? wxWindowMode::AppModal : wxWindowMode::Normal;
}

// ----------------------------------------------------------------------------
// other event handlers
// ----------------------------------------------------------------------------

void wxDialogBase::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    // We'll send a Cancel message by default, which may close the dialog.

    // Check for looping if the Cancel event handler calls Close().
    //
    // VZ: this is horrible and MT-unsafe. Can't we reuse some of these global
    //     lists here? don't dare to change it now, but should be done later!
    static wxList closing;

    if ( closing.Member(this) )
        return;

    closing.Append(this);

    // When a previously hidden (necessarily modeless) dialog is being closed,
    // we must not perform the usual validation and data transfer steps as they
    // had been already done when it was hidden and doing it again now would be
    // unexpected and could result in e.g. the dialog asking for confirmation
    // before discarding the changes being shown again, which doesn't make
    // sense as the dialog is not being closed in response to any user action.
    if ( !IsShown() || !SendCloseButtonClickEvent() )
    {
        // If the handler didn't close the dialog (e.g. because there is no
        // button with matching id) we still want to close it when the user
        // clicks the "x" button in the title bar, otherwise we shouldn't even
        // have put it there.
        //
        // Notice that using wxID_CLOSE might have been a better choice but we
        // use wxID_CANCEL for compatibility reasons.
        EndDialog(wxID_CANCEL);
    }

    closing.DeleteObject(this);
}

void wxDialogBase::OnSysColourChanged(wxSysColourChangedEvent& event)
{
#ifndef __WXGTK__
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    Refresh();
#endif

    event.Skip();
}

/// Do the adaptation
bool wxDialogBase::DoLayoutAdaptation()
{
    wxDialogLayoutAdapter* const adapter = GetLayoutAdapter();
    if ( !adapter )
        return false;

    wxDialog* const dialog = static_cast<wxDialog*>(this);
    const wxWeakRef<wxWindow> dialogLifetime(dialog);
    wxWindow* const focusWindow = wxFindFocusDescendant(this); // from event.h
    const wxWeakRef<wxWindow> focusLifetime(focusWindow);

    const bool adapted = adapter->DoLayoutAdaptation(dialog);
    if ( !adapted ||
         !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) )
    {
        return false;
    }

    if ( focusWindow &&
         wxWeakWindowIsAvailableForCallbacks(focusLifetime, focusWindow) &&
         dialog->IsDescendant(focusWindow) )
    {
        focusWindow->SetFocus();
    }

    return wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog);
}

/// Can we do the adaptation?
bool wxDialogBase::CanDoLayoutAdaptation()
{
    // Check if local setting overrides the global setting
    bool layoutEnabled = (GetLayoutAdaptationMode() == wxDIALOG_ADAPTATION_MODE_ENABLED) || (IsLayoutAdaptationEnabled() && (GetLayoutAdaptationMode() != wxDIALOG_ADAPTATION_MODE_DISABLED));

    return (layoutEnabled && !m_layoutAdaptationDone && GetLayoutAdaptationLevel() != 0 && GetLayoutAdapter() != nullptr && GetLayoutAdapter()->CanDoLayoutAdaptation((wxDialog*) this));
}

/// Set scrolling adapter class, returning old adapter
wxDialogLayoutAdapter* wxDialogBase::SetLayoutAdapter(wxDialogLayoutAdapter* adapter)
{
    wxDialogLayoutAdapter* oldLayoutAdapter = sm_layoutAdapter;
    sm_layoutAdapter = adapter;
    return oldLayoutAdapter;
}

/*!
 * Standard adapter
 */

wxIMPLEMENT_CLASS(wxDialogLayoutAdapter, wxObject);

wxIMPLEMENT_CLASS(wxStandardDialogLayoutAdapter, wxDialogLayoutAdapter);

// Allow for caption size on wxWidgets < 2.9
#if defined(__WXGTK__) && !wxCHECK_VERSION(2,9,0)
#define wxEXTRA_DIALOG_HEIGHT 30
#else
#define wxEXTRA_DIALOG_HEIGHT 0
#endif

namespace
{

// ReparentControls() is a public virtual returning void, so its caller can't
// trust that all the requested work was done. Keep a weak snapshot of the
// exact direct-child topology around it: this both makes success observable
// and provides a bounded rollback when an override, or an individual
// wxWindow::Reparent() implementation, stops after moving only some children.
class wxDialogReparentTransaction
{
public:
    wxDialogReparentTransaction(wxWindow* source,
                                wxWindow* destination,
                                wxSizer* excludedSizer = nullptr)
        : m_source(source),
          m_sourceLifetime(source),
          m_destination(destination),
          m_destinationLifetime(destination),
          m_excludedSizer(excludedSizer)
    {
        if ( !IsEndpointAvailable(m_sourceLifetime, m_source) ||
             !IsEndpointAvailable(m_destinationLifetime, m_destination) )
        {
            return;
        }

        SnapshotChildren(m_destination, false);

        for ( wxWindowList::compatibility_iterator node =
                  m_source->GetChildren().GetFirst();
              node;
              node = node->GetNext() )
        {
            wxWindow* const child = node->GetData();
            const bool move =
                child != m_destination &&
                (!excludedSizer || !excludedSizer->GetItem(child));
            m_children.emplace_back(child, m_source, move);
        }

        m_valid = true;
    }

    bool IsValid() const { return m_valid && !m_failed; }

    void MarkFailed() { m_failed = true; }

    bool Matches(wxWindow* source,
                 wxWindow* destination,
                 wxSizer* excludedSizer) const
    {
        return m_source == source &&
               m_destination == destination &&
               m_excludedSizer == excludedSizer;
    }

    bool WasApplied() const
    {
        if ( !m_valid || m_failed ||
             !IsEndpointAvailable(m_sourceLifetime, m_source) ||
             !IsEndpointAvailable(m_destinationLifetime, m_destination) )
        {
            return false;
        }

        size_t expectedSourceCount = 0;
        size_t expectedDestinationCount = 0;
        for ( const Child& child : m_children )
        {
            if ( !IsEndpointAvailable(child.lifetime, child.window) )
                return false;

            wxWindow* const expectedParent =
                child.move ? m_destination : child.originalParent;
            if ( child.window->GetParent() != expectedParent )
                return false;

            if ( expectedParent == m_source )
                ++expectedSourceCount;
            else if ( expectedParent == m_destination )
                ++expectedDestinationCount;
        }

        return m_source->GetChildren().GetCount() == expectedSourceCount &&
               m_destination->GetChildren().GetCount() ==
                   expectedDestinationCount;
    }

    // Rollback is deliberately bounded: every originally tracked child gets
    // at most one virtual Reparent() attempt. The retained-WinUI fallback below
    // may then reverse that exact already-completed intra-TLW move without
    // virtual dispatch. Use the "itself" predicate here because an enclosing
    // dialog can already be queued for destruction while its still-live
    // children must be put back before the provisional scroller is discarded.
    bool Restore() const
    {
        if ( !m_valid )
            return false;

        bool restoredExactly = true;
        for ( std::vector<Child>::const_reverse_iterator it =
                  m_children.rbegin();
              it != m_children.rend();
              ++it )
        {
            const Child& child = *it;
            wxWindow* const window = child.lifetime.get();
            wxWindow* const originalParent = child.originalParentLifetime.get();
            if ( window != child.window ||
                 originalParent != child.originalParent ||
                 wxWindowItselfIsUnavailableForCallbacks(window) ||
                 wxWindowItselfIsUnavailableForCallbacks(originalParent) )
            {
                restoredExactly = false;
                continue;
            }

            if ( window->GetParent() != originalParent )
            {
                bool restored = window->Reparent(originalParent);
                if ( !restored &&
                     child.lifetime.get() == child.window &&
                     child.originalParentLifetime.get() ==
                         child.originalParent &&
                     child.window->GetParent() != child.originalParent )
                {
                    restored = RestoreRetainedWinUIParent(child);
                }
                if ( child.lifetime.get() != child.window ||
                     child.originalParentLifetime.get() !=
                         child.originalParent ||
                     wxWindowItselfIsUnavailableForCallbacks(child.window) ||
                     wxWindowItselfIsUnavailableForCallbacks(
                         child.originalParent) ||
                     !restored ||
                     child.window->GetParent() != child.originalParent )
                {
                    restoredExactly = false;
                }
            }
        }

        return restoredExactly && HasOriginalTopology();
    }

private:
    struct Child
    {
        Child(wxWindow* window_, wxWindow* parent_, bool move_)
            : window(window_),
              lifetime(window_),
              originalParent(parent_),
              originalParentLifetime(parent_),
              move(move_)
        {
        }

        wxWindow* window;
        wxWeakRef<wxWindow> lifetime;
        wxWindow* originalParent;
        wxWeakRef<wxWindow> originalParentLifetime;
        bool move;
    };

    static bool IsEndpointAvailable(const wxWeakRef<wxWindow>& lifetime,
                                    wxWindow* expected)
    {
        return wxWeakWindowIsAvailableForCallbacks(lifetime, expected);
    }

    bool RestoreRetainedWinUIParent(const Child& child) const
    {
#if defined(__WXWINUI__) && wxUSE_WINUI3
        wxWindow* const window = child.lifetime.get();
        wxWindow* const originalParent =
            child.originalParentLifetime.get();
        wxWindow* const currentParent = window ? window->GetParent() : nullptr;
        if ( !child.move || window != child.window ||
             originalParent != child.originalParent ||
             currentParent != m_destination ||
             wxWindowItselfIsUnavailableForCallbacks(window) ||
             wxWindowItselfIsUnavailableForCallbacks(currentParent) ||
             wxWindowItselfIsUnavailableForCallbacks(originalParent) )
        {
            return false;
        }

        // The public WinUI Reparent() correctly rejects all new work once a
        // TLW is queued for destruction. This is narrower: undo the one
        // already-completed intra-TLW move owned by this transaction before
        // its provisional scroller is synchronously deleted. No owner/DPI/host
        // migration is involved because both parents have the same dying TLW.
        wxWindow* const currentTLW = wxGetTopLevelParent(currentParent);
        if ( !currentTLW ||
             currentTLW != wxGetTopLevelParent(originalParent) ||
             !wxWinUITLWHostIsDestroyScheduled(currentTLW) )
        {
            return false;
        }

        const wxWeakRef<wxWindow> currentParentLifetime(currentParent);
        const HWND hwnd = static_cast<HWND>(window->GetHWND());
        const HWND currentParentHwnd =
            static_cast<HWND>(currentParent->GetHWND());
        const HWND originalParentHwnd =
            static_cast<HWND>(originalParent->GetHWND());
        if ( !hwnd || !currentParentHwnd || !originalParentHwnd ||
             !::IsWindow(hwnd) || !::IsWindow(currentParentHwnd) ||
             !::IsWindow(originalParentHwnd) ||
             ::GetParent(hwnd) != currentParentHwnd )
        {
            return false;
        }

        // Match wxWindowMSW::Reparent()'s ordering: publish the logical child
        // graph first, then cross the USER32 boundary, with a bounded logical
        // rollback if that physical operation is refused.
        if ( !window->wxWindowBase::Reparent(originalParent) )
            return false;

        if ( child.lifetime.get() != child.window ||
             child.originalParentLifetime.get() != child.originalParent ||
             currentParentLifetime.get() != currentParent ||
             window->GetParent() != originalParent ||
             ::GetParent(hwnd) != currentParentHwnd )
        {
            if ( child.lifetime.get() == child.window &&
                 currentParentLifetime.get() == currentParent &&
                 child.window->GetParent() == child.originalParent )
            {
                (void)child.window->wxWindowBase::Reparent(currentParent);
            }
            return false;
        }

        ::SetLastError(ERROR_SUCCESS);
        const HWND previousParent = ::SetParent(hwnd, originalParentHwnd);
        const DWORD error = ::GetLastError();
        if ( previousParent != currentParentHwnd ||
             (!previousParent && error != ERROR_SUCCESS) ||
             child.lifetime.get() != child.window ||
             child.originalParentLifetime.get() != child.originalParent ||
             currentParentLifetime.get() != currentParent ||
             ::GetParent(hwnd) != originalParentHwnd )
        {
            if ( child.lifetime.get() == child.window &&
                 currentParentLifetime.get() == currentParent &&
                 child.window->GetParent() == child.originalParent )
            {
                (void)child.window->wxWindowBase::Reparent(currentParent);
            }
            return false;
        }

        return child.lifetime.get() == child.window &&
               child.originalParentLifetime.get() == child.originalParent &&
               currentParentLifetime.get() == currentParent &&
               child.window->GetParent() == child.originalParent &&
               ::GetParent(hwnd) == originalParentHwnd;
#else
        wxUnusedVar(child);
        return false;
#endif
    }

    void SnapshotChildren(wxWindow* parent, bool move)
    {
        for ( wxWindowList::compatibility_iterator node =
                  parent->GetChildren().GetFirst();
              node;
              node = node->GetNext() )
        {
            m_children.emplace_back(node->GetData(), parent, move);
        }
    }

    bool HasOriginalTopology() const
    {
        size_t expectedSourceCount = 0;
        size_t expectedDestinationCount = 0;
        for ( const Child& child : m_children )
        {
            wxWindow* const window = child.lifetime.get();
            if ( window != child.window ||
                 child.originalParentLifetime.get() != child.originalParent ||
                 wxWindowItselfIsUnavailableForCallbacks(window) ||
                 wxWindowItselfIsUnavailableForCallbacks(
                     child.originalParent) ||
                 window->GetParent() != child.originalParent )
            {
                return false;
            }

            if ( child.originalParent == m_source )
                ++expectedSourceCount;
            else if ( child.originalParent == m_destination )
                ++expectedDestinationCount;
        }

        wxWindow* const source = m_sourceLifetime.get();
        wxWindow* const destination = m_destinationLifetime.get();
        return source == m_source && destination == m_destination &&
               !wxWindowItselfIsUnavailableForCallbacks(source) &&
               !wxWindowItselfIsUnavailableForCallbacks(destination) &&
               source->GetChildren().GetCount() == expectedSourceCount &&
               destination->GetChildren().GetCount() ==
                   expectedDestinationCount;
    }

    wxWindow* const m_source;
    wxWeakRef<wxWindow> m_sourceLifetime;
    wxWindow* const m_destination;
    wxWeakRef<wxWindow> m_destinationLifetime;
    wxSizer* const m_excludedSizer { nullptr };
    std::vector<Child> m_children;
    bool m_valid { false };
    bool m_failed { false };
};

thread_local std::vector<wxDialogReparentTransaction*>
    gs_dialogReparentTransactions;

class wxDialogReparentTransactionScope
{
public:
    explicit wxDialogReparentTransactionScope(
        wxDialogReparentTransaction& transaction)
        : m_transaction(&transaction)
    {
        gs_dialogReparentTransactions.push_back(m_transaction);
    }

    ~wxDialogReparentTransactionScope()
    {
        wxASSERT(!gs_dialogReparentTransactions.empty());
        wxASSERT(gs_dialogReparentTransactions.back() == m_transaction);
        gs_dialogReparentTransactions.pop_back();
    }

private:
    wxDialogReparentTransaction* const m_transaction;
};

wxDialogReparentTransaction* FindActiveDialogReparentTransaction(
    wxWindow* source,
    wxWindow* destination,
    wxSizer* excludedSizer)
{
    for ( std::vector<wxDialogReparentTransaction*>::reverse_iterator it =
              gs_dialogReparentTransactions.rbegin();
          it != gs_dialogReparentTransactions.rend();
          ++it )
    {
        if ( (*it)->Matches(source, destination, excludedSizer) )
            return *it;
    }

    return nullptr;
}

struct wxDialogPageSnapshot
{
    explicit wxDialogPageSnapshot(wxWindow* page_)
        : page(page_), lifetime(page_)
    {
    }

    wxWindow* page;
    wxWeakRef<wxWindow> lifetime;
};

bool wxDialogBookTopologyIsAvailable(
    const wxWeakRef<wxWindow>& dialogLifetime,
    wxDialog* dialog,
    wxSizer* dialogSizer,
    const wxWeakRef<wxWindow>& bookLifetime,
    wxBookCtrlBase* book,
    const std::vector<wxDialogPageSnapshot>& pages)
{
    const auto endpointsAreAvailable = [&]()
    {
        return wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) &&
               dialog->GetSizer() == dialogSizer &&
               wxWeakWindowIsAvailableForCallbacks(bookLifetime, book) &&
               book->GetParent() == dialog;
    };

    // GetContentWindow(), GetPageCount() and GetPage() are all virtual. A
    // custom property sheet/book is allowed to destroy or replace any of the
    // tracked objects from them, so never combine one of these calls with a
    // following raw dereference in the same expression.
    const auto contextIsAvailable = [&]()
    {
        if ( !endpointsAreAvailable() )
            return false;

        wxWindow* const contentWindow = dialog->GetContentWindow();
        return endpointsAreAvailable() && contentWindow == book;
    };

    if ( !contextIsAvailable() )
        return false;

    const size_t pageCount = book->GetPageCount();
    if ( !contextIsAvailable() || pageCount != pages.size() )
    {
        return false;
    }

    for ( size_t i = 0; i < pages.size(); ++i )
    {
        const wxDialogPageSnapshot& tracked = pages[i];
        wxWindow* const page = book->GetPage(i);
        if ( !contextIsAvailable() ||
             !wxWeakWindowIsAvailableForCallbacks(tracked.lifetime,
                                                   tracked.page) ||
             page != tracked.page ||
             tracked.page->GetParent() != book )
        {
            return false;
        }
    }

    return true;
}

struct wxAdaptedDialogPage
{
    wxAdaptedDialogPage(wxWindow* page_,
                        wxScrolledWindow* scrolledWindow_,
                        wxSizer* oldSizer_,
                        wxSizer* newSizer_,
                        const wxDialogReparentTransaction& reparenting_)
        : page(page_),
          pageLifetime(page_),
          scrolledWindow(scrolledWindow_),
          scrolledWindowLifetime(scrolledWindow_),
          oldSizer(oldSizer_),
          newSizer(newSizer_),
          reparenting(reparenting_)
    {
    }

    wxWindow* page;
    wxWeakRef<wxWindow> pageLifetime;
    wxScrolledWindow* scrolledWindow;
    wxWeakRef<wxWindow> scrolledWindowLifetime;
    wxSizer* oldSizer;
    wxSizer* newSizer;
    wxDialogReparentTransaction reparenting;
};

bool wxRollbackAdaptedDialogPage(wxAdaptedDialogPage& adapted)
{
    // Never detach the old sizer or destroy its provisional parent while one
    // of the original controls is still below it: that would turn a failed
    // rollback into data loss. Leave this one page adapted if an adversarial
    // Reparent() refuses the single bounded restoration attempt.
    if ( !adapted.reparenting.Restore() )
        return false;

    bool restoredExactly = true;

    wxWindow* const page = adapted.pageLifetime.get();
    wxScrolledWindow* const scrolledWindow =
        wxDynamicCast(adapted.scrolledWindowLifetime.get(), wxScrolledWindow);
    if ( page != adapted.page || scrolledWindow != adapted.scrolledWindow ||
         wxWindowItselfIsUnavailableForCallbacks(page) ||
         wxWindowItselfIsUnavailableForCallbacks(scrolledWindow) )
    {
        return false;
    }

    if ( scrolledWindow->GetSizer() == adapted.oldSizer )
        scrolledWindow->SetSizer(nullptr, false);
    else
        restoredExactly = false;

    if ( page->GetSizer() == adapted.newSizer )
        page->SetSizer(adapted.oldSizer, true);
    else if ( page->GetSizer() != adapted.oldSizer )
        restoredExactly = false;

    if ( scrolledWindow->GetParent() == page )
        delete scrolledWindow;
    else
        restoredExactly = false;

    return restoredExactly &&
           adapted.pageLifetime.get() == adapted.page &&
           !wxWindowItselfIsUnavailableForCallbacks(adapted.page) &&
           adapted.page->GetSizer() == adapted.oldSizer;
}

void wxDiscardProvisionalScrolledWindow(
    wxScrolledWindow* scrolledWindow,
    const wxWeakRef<wxWindow>& scrolledWindowLifetime)
{
    if ( scrolledWindowLifetime.get() == scrolledWindow &&
         !wxWindowItselfIsUnavailableForCallbacks(scrolledWindow) )
    {
        delete scrolledWindow;
    }
}

#if wxUSE_BUTTON
// FindButtonSizer() and FindLooseButtons() predate transactional adaptation and
// mutate the existing tree while searching. Keep the exact wxSizerItem
// wrappers while they are detached: reconstructing them loses userData and is
// not even supported by wxGridBagSizer::Insert().
class wxDialogSizerTopologySnapshot;

thread_local std::vector<wxDialogSizerTopologySnapshot*>
    gs_dialogSizerTopologySnapshots;

class wxDialogSizerTopologySnapshot
{
public:
    explicit wxDialogSizerTopologySnapshot(wxSizer* root)
    {
        Snapshot(root);
        gs_dialogSizerTopologySnapshots.push_back(this);
    }

    ~wxDialogSizerTopologySnapshot()
    {
        wxASSERT( !gs_dialogSizerTopologySnapshots.empty() &&
                  gs_dialogSizerTopologySnapshots.back() == this );
        gs_dialogSizerTopologySnapshots.pop_back();
        DiscardExtractedItems();
    }

    bool ContainsSizer(wxSizer* sizer) const
    {
        for ( wxSizer* const tracked : m_sizers )
        {
            if ( tracked == sizer )
                return true;
        }
        return false;
    }

    bool Tracks(wxSizer* parent, wxWindow* window) const
    {
        return Find(parent, window, nullptr) != nullptr;
    }

    bool Tracks(wxSizer* parent, wxSizer* sizer) const
    {
        return Find(parent, nullptr, sizer) != nullptr;
    }

    bool Extract(wxSizer* parent, wxWindow* window)
    {
        Item* const tracked = Find(parent, window, nullptr);
        if ( !tracked || tracked->extracted ||
             tracked->windowLifetime.get() != window ||
             wxWindowItselfIsUnavailableForCallbacks(window) ||
             window->GetContainingSizer() != parent )
        {
            return false;
        }

        if ( !EraseItemFromParent(*tracked) )
            return false;

        // The retained wxSizerItem still refers to this window, so explicitly
        // clear the live back-pointer while the item is outside every sizer.
        window->SetContainingSizer(nullptr);
        tracked->extracted = true;
        return true;
    }

    bool Extract(wxSizer* parent, wxSizer* sizer)
    {
        Item* const tracked = Find(parent, nullptr, sizer);
        if ( !tracked || tracked->extracted ||
             !EraseItemFromParent(*tracked) )
        {
            return false;
        }

        tracked->extracted = true;
        return true;
    }

    bool Restore(wxSizer* extraSizer = nullptr)
    {
        std::vector<wxSizer*> searchable = m_sizers;
        if ( extraSizer && !ContainsSizer(extraSizer) )
            searchable.push_back(extraSizer);

        bool restoredExactly = true;
        for ( Item& item : m_items )
        {
            if ( !item.extracted )
                continue;

            if ( item.window )
            {
                wxWindow* const window = item.windowLifetime.get();
                if ( window != item.window ||
                     wxWindowItselfIsUnavailableForCallbacks(window) )
                {
                    restoredExactly = false;
                    continue;
                }

                wxSizer* const current = window->GetContainingSizer();
                if ( current && !current->Detach(window) )
                {
                    restoredExactly = false;
                    continue;
                }

                if ( item.windowLifetime.get() != item.window ||
                     wxWindowItselfIsUnavailableForCallbacks(window) ||
                     !RestoreExactItem(item) )
                {
                    restoredExactly = false;
                    continue;
                }

                window->SetContainingSizer(item.parent);
                item.extracted = false;
            }
            else
            {
                wxSizer* const current =
                    FindParentOfSizer(item.sizer, searchable);
                if ( current && !current->Detach(item.sizer) )
                {
                    restoredExactly = false;
                    continue;
                }

                if ( !RestoreExactItem(item) )
                {
                    restoredExactly = false;
                    continue;
                }

                item.extracted = false;
            }
        }

        for ( const Item& item : m_items )
        {
            wxSizerItem* const restored = item.parent->GetItem(item.index);
            if ( restored != item.sizerItem )
            {
                restoredExactly = false;
            }
        }

        return restoredExactly;
    }

private:
    struct Item
    {
        Item(wxSizer* parent_, size_t index_, wxSizerItem* item)
            : parent(parent_),
              index(index_),
              sizerItem(item),
              window(item->GetWindow()),
              windowLifetime(window),
              sizer(item->GetSizer())
        {
        }

        wxSizer* parent;
        size_t index;
        wxSizerItem* sizerItem;
        wxWindow* window;
        wxWeakRef<wxWindow> windowLifetime;
        wxSizer* sizer;
        bool extracted { false };
    };

    Item* Find(wxSizer* parent, wxWindow* window, wxSizer* sizer)
    {
        for ( Item& item : m_items )
        {
            if ( item.parent == parent && item.window == window &&
                 item.sizer == sizer )
            {
                return &item;
            }
        }
        return nullptr;
    }

    const Item* Find(wxSizer* parent,
                     wxWindow* window,
                     wxSizer* sizer) const
    {
        for ( const Item& item : m_items )
        {
            if ( item.parent == parent && item.window == window &&
                 item.sizer == sizer )
            {
                return &item;
            }
        }
        return nullptr;
    }

    void Snapshot(wxSizer* sizer)
    {
        if ( !sizer || ContainsSizer(sizer) )
            return;

        m_sizers.push_back(sizer);
        size_t index = 0;
        for ( wxSizerItemList::compatibility_iterator node =
                  sizer->GetChildren().GetFirst();
              node;
              node = node->GetNext(), ++index )
        {
            wxSizerItem* const item = node->GetData();
            if ( item->IsWindow() || item->IsSizer() )
                m_items.emplace_back(sizer, index, item);

            if ( wxSizer* const childSizer = item->GetSizer() )
                Snapshot(childSizer);
        }
    }

    static bool EraseItemFromParent(const Item& item)
    {
        for ( wxSizerItemList::compatibility_iterator node =
                  item.parent->GetChildren().GetFirst();
              node;
              node = node->GetNext() )
        {
            if ( node->GetData() == item.sizerItem )
            {
                item.parent->GetChildren().Erase(node);
                return true;
            }
        }
        return false;
    }

    static bool RestoreExactItem(const Item& item)
    {
        return item.parent->GetChildren().Insert(item.index,
                                                  item.sizerItem) != nullptr;
    }

    static wxSizer* FindParentOfSizer(
        wxSizer* child,
        const std::vector<wxSizer*>& candidates)
    {
        for ( wxSizer* const candidate : candidates )
        {
            if ( candidate && candidate->GetItem(child, false) )
                return candidate;
        }
        return nullptr;
    }

    void DiscardExtractedItems()
    {
        for ( Item& item : m_items )
        {
            if ( !item.extracted )
                continue;

            if ( item.window )
                item.sizerItem->DetachWindow();
            else
                item.sizerItem->DetachSizer();

            delete item.sizerItem;
            item.sizerItem = nullptr;
            item.extracted = false;
        }
    }

    std::vector<wxSizer*> m_sizers;
    std::vector<Item> m_items;

    wxDECLARE_NO_COPY_CLASS(wxDialogSizerTopologySnapshot);
};

bool wxDetachDialogSizerItem(wxSizer* parent, wxWindow* window)
{
    if ( !gs_dialogSizerTopologySnapshots.empty() )
    {
        wxDialogSizerTopologySnapshot* const snapshot =
            gs_dialogSizerTopologySnapshots.back();
        if ( snapshot->Tracks(parent, window) )
            return snapshot->Extract(parent, window);
    }

    return parent->Detach(window);
}

bool wxDetachDialogSizerItem(wxSizer* parent, wxSizer* sizer)
{
    if ( !gs_dialogSizerTopologySnapshots.empty() )
    {
        wxDialogSizerTopologySnapshot* const snapshot =
            gs_dialogSizerTopologySnapshots.back();
        if ( snapshot->Tracks(parent, sizer) )
            return snapshot->Extract(parent, sizer);
    }

    return parent->Detach(sizer);
}
#endif // wxUSE_BUTTON

} // anonymous namespace

/// Indicate that adaptation should be done
bool wxStandardDialogLayoutAdapter::CanDoLayoutAdaptation(wxDialog* dialog)
{
    if (dialog->GetSizer())
    {
        wxSize windowSize, displaySize;
        return MustScroll(dialog, windowSize, displaySize) != 0;
    }
    else
        return false;
}

bool wxStandardDialogLayoutAdapter::DoLayoutAdaptation(wxDialog* dialog)
{
    const wxWeakRef<wxWindow> dialogLifetime(dialog);
    if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) )
        return false;

    wxSizer* const dialogSizer = dialog->GetSizer();
    if ( !dialogSizer )
        return false;

#if wxUSE_BOOKCTRL
    wxWindow* const contentWindowAddress = dialog->GetContentWindow();
    if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) ||
         dialog->GetSizer() != dialogSizer )
    {
        return false;
    }

    // Resolve a non-null virtual result through the live direct-child list
    // before using RTTI on it: an override may synchronously delete its own
    // result and return the stale address.
    wxWindow* liveContentWindow = nullptr;
    if ( contentWindowAddress )
    {
        for ( wxWindowList::compatibility_iterator node =
                  dialog->GetChildren().GetFirst();
              node;
              node = node->GetNext() )
        {
            if ( node->GetData() == contentWindowAddress )
            {
                liveContentWindow = node->GetData();
                break;
            }
        }

        if ( !liveContentWindow )
            return false;
    }

    wxBookCtrlBase* const bookContentWindow =
        wxDynamicCast(liveContentWindow, wxBookCtrlBase);

    if ( bookContentWindow )
    {
        const wxWeakRef<wxWindow> bookLifetime(bookContentWindow);
        const auto bookContextIsAvailable = [&]()
        {
            if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime,
                                                       dialog) ||
                 dialog->GetSizer() != dialogSizer ||
                 !wxWeakWindowIsAvailableForCallbacks(bookLifetime,
                                                       bookContentWindow) ||
                 bookContentWindow->GetParent() != dialog )
            {
                return false;
            }

            wxWindow* const contentWindow = dialog->GetContentWindow();
            return wxWeakWindowIsAvailableForCallbacks(dialogLifetime,
                                                        dialog) &&
                   dialog->GetSizer() == dialogSizer &&
                   wxWeakWindowIsAvailableForCallbacks(bookLifetime,
                                                        bookContentWindow) &&
                   bookContentWindow->GetParent() == dialog &&
                   contentWindow == bookContentWindow;
        };

        if ( !bookContextIsAvailable() )
        {
            return false;
        }

        std::vector<wxDialogPageSnapshot> pages;
        const size_t pageCount = bookContentWindow->GetPageCount();
        if ( !bookContextIsAvailable() )
            return false;

        pages.reserve(pageCount);
        for ( size_t i = 0; i < pageCount; ++i )
        {
            wxWindow* const page = bookContentWindow->GetPage(i);
            if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime,
                                                       dialog) ||
                 !wxWeakWindowIsAvailableForCallbacks(bookLifetime,
                                                       bookContentWindow) )
            {
                return false;
            }

            // Resolve the address returned by the virtual GetPage() through
            // the live child list before constructing a weak reference from
            // it. An adversarial override can delete the page and still return
            // its stale address.
            wxWindow* livePage = nullptr;
            for ( wxWindowList::compatibility_iterator node =
                      bookContentWindow->GetChildren().GetFirst();
                  node;
                  node = node->GetNext() )
            {
                if ( node->GetData() == page )
                {
                    livePage = node->GetData();
                    break;
                }
            }

            if ( !livePage )
                return false;

            const wxWeakRef<wxWindow> pageLifetime(livePage);
            if ( !bookContextIsAvailable() ||
                 !wxWeakWindowIsAvailableForCallbacks(pageLifetime,
                                                       livePage) ||
                 livePage->GetParent() != bookContentWindow )
            {
                return false;
            }

            pages.emplace_back(livePage);
        }

        if ( !wxDialogBookTopologyIsAvailable(dialogLifetime,
                                               dialog,
                                               dialogSizer,
                                               bookLifetime,
                                               bookContentWindow,
                                               pages) )
        {
            return false;
        }

        const wxSize originalSize = dialog->GetSize();
        const wxSize originalMinSize = dialog->GetMinSize();
        const wxSize originalMaxSize = dialog->GetMaxSize();
        const bool originalAdaptationDone =
            dialog->GetLayoutAdaptationDone();
        if ( !wxDialogBookTopologyIsAvailable(dialogLifetime,
                                               dialog,
                                               dialogSizer,
                                               bookLifetime,
                                               bookContentWindow,
                                               pages) )
        {
            return false;
        }

        wxWindowList scrollingWindows;
        std::vector<wxWeakRef<wxWindow>> scrollingWindowLifetimes;
        std::vector<wxAdaptedDialogPage> adaptedPages;
        bool geometryMayHaveChanged = false;
        bool adaptationCommitted = false;
        const wxScopeGuard rollbackAdaptation = wxMakeGuard(
            [&]()
            {
                if ( adaptationCommitted )
                    return;

                for ( std::vector<wxAdaptedDialogPage>::reverse_iterator it =
                          adaptedPages.rbegin();
                      it != adaptedPages.rend();
                      ++it )
                {
                    (void)wxRollbackAdaptedDialogPage(*it);
                }

                wxDialog* const liveDialog =
                    wxDynamicCast(dialogLifetime.get(), wxDialog);
                if ( liveDialog == dialog &&
                     !wxWindowItselfIsUnavailableForCallbacks(liveDialog) )
                {
                    liveDialog->SetLayoutAdaptationDone(
                        originalAdaptationDone);
                    if ( geometryMayHaveChanged )
                    {
                        liveDialog->SetMinSize(originalMinSize);
                        if ( dialogLifetime.get() == dialog &&
                             !wxWindowItselfIsUnavailableForCallbacks(dialog) )
                        {
                            liveDialog->SetMaxSize(originalMaxSize);
                            liveDialog->SetSize(originalSize);
                        }
                    }
                }
            });
        wxUnusedVar(rollbackAdaptation);

        // If we have a book control, make all the pages that use sizers
        // scrollable. The reparenting callback runs before either sizer is
        // published, so a partial/refused callback leaves the original sizer
        // graph untouched and only requires restoring direct parents.
        for ( const wxDialogPageSnapshot& trackedPage : pages )
        {
            if ( !wxDialogBookTopologyIsAvailable(dialogLifetime,
                                                   dialog,
                                                   dialogSizer,
                                                   bookLifetime,
                                                   bookContentWindow,
                                                   pages) )
            {
                return false;
            }

            wxWindow* const page = trackedPage.page;
            if ( wxScrolledWindow* const existingScroller =
                     wxDynamicCast(page, wxScrolledWindow) )
            {
                scrollingWindows.Append(existingScroller);
                scrollingWindowLifetimes.emplace_back(existingScroller);
                continue;
            }

            wxSizer* const oldSizer = page->GetSizer();
            if ( !oldSizer )
                continue;

            wxScrolledWindow* const scrolledWindow =
                CreateScrolledWindow(page);
            const wxWeakRef<wxWindow> scrolledWindowLifetime(scrolledWindow);
            if ( !wxDialogBookTopologyIsAvailable(dialogLifetime,
                                                   dialog,
                                                   dialogSizer,
                                                   bookLifetime,
                                                   bookContentWindow,
                                                   pages) ||
                 !wxWeakWindowIsAvailableForCallbacks(
                     scrolledWindowLifetime, scrolledWindow) ||
                 scrolledWindow->GetParent() != page ||
                 page->GetSizer() != oldSizer )
            {
                wxDiscardProvisionalScrolledWindow(scrolledWindow,
                                                    scrolledWindowLifetime);
                return false;
            }

            wxDialogReparentTransaction reparenting(page, scrolledWindow);
            if ( !reparenting.IsValid() )
            {
                wxDiscardProvisionalScrolledWindow(scrolledWindow,
                                                    scrolledWindowLifetime);
                return false;
            }

            {
                // ReparentControls() is virtual, but its default
                // implementation must share this transaction instead of
                // independently rolling the same children back a second
                // time. In particular, a retained Destroy() request permits
                // exactly one bounded restoration before the scroller dies.
                wxDialogReparentTransactionScope transactionScope(
                    reparenting);
                ReparentControls(page, scrolledWindow);
            }
            if ( !wxDialogBookTopologyIsAvailable(dialogLifetime,
                                                   dialog,
                                                   dialogSizer,
                                                   bookLifetime,
                                                   bookContentWindow,
                                                   pages) ||
                 !wxWeakWindowIsAvailableForCallbacks(
                     scrolledWindowLifetime, scrolledWindow) ||
                 page->GetSizer() != oldSizer ||
                 !reparenting.WasApplied() )
            {
                if ( reparenting.Restore() )
                {
                    wxDiscardProvisionalScrolledWindow(
                        scrolledWindow, scrolledWindowLifetime);
                }
                return false;
            }

            wxBoxSizer* const newSizer = new wxBoxSizer(wxVERTICAL);
            if ( !newSizer->Add(scrolledWindow, 1, wxEXPAND, 0) )
            {
                delete newSizer;
                if ( reparenting.Restore() )
                {
                    wxDiscardProvisionalScrolledWindow(
                        scrolledWindow, scrolledWindowLifetime);
                }
                return false;
            }

            page->SetSizer(newSizer,
                           false /* don't delete the old sizer */);
            scrolledWindow->SetSizer(oldSizer);
            if ( !wxDialogBookTopologyIsAvailable(dialogLifetime,
                                                   dialog,
                                                   dialogSizer,
                                                   bookLifetime,
                                                   bookContentWindow,
                                                   pages) ||
                 !wxWeakWindowIsAvailableForCallbacks(
                     scrolledWindowLifetime, scrolledWindow) ||
                 page->GetSizer() != newSizer ||
                 scrolledWindow->GetSizer() != oldSizer )
            {
                wxAdaptedDialogPage incomplete(page,
                                               scrolledWindow,
                                               oldSizer,
                                               newSizer,
                                               reparenting);
                (void)wxRollbackAdaptedDialogPage(incomplete);
                return false;
            }

            adaptedPages.emplace_back(page,
                                      scrolledWindow,
                                      oldSizer,
                                      newSizer,
                                      reparenting);
            scrollingWindows.Append(scrolledWindow);
            scrollingWindowLifetimes.emplace_back(scrolledWindow);
        }

        for ( const wxWeakRef<wxWindow>& lifetime :
                  scrollingWindowLifetimes )
        {
            if ( !lifetime ||
                 wxWindowIsUnavailableForCallbacks(lifetime.get()) )
            {
                return false;
            }
        }

        geometryMayHaveChanged = true;
        const bool fitted = FitWithScrolling(dialog, scrollingWindows);
        if ( !fitted ||
             !wxDialogBookTopologyIsAvailable(dialogLifetime,
                                               dialog,
                                               dialogSizer,
                                               bookLifetime,
                                               bookContentWindow,
                                               pages) )
        {
            return false;
        }

        for ( const wxAdaptedDialogPage& adapted : adaptedPages )
        {
            if ( !wxWeakWindowIsAvailableForCallbacks(adapted.pageLifetime,
                                                       adapted.page) ||
                 !wxWeakWindowIsAvailableForCallbacks(
                     adapted.scrolledWindowLifetime,
                     adapted.scrolledWindow) ||
                 adapted.page->GetSizer() != adapted.newSizer ||
                 adapted.scrolledWindow->GetSizer() != adapted.oldSizer )
            {
                return false;
            }
        }

        dialog->SetLayoutAdaptationDone(true);
        if ( !wxDialogBookTopologyIsAvailable(dialogLifetime,
                                               dialog,
                                               dialogSizer,
                                               bookLifetime,
                                               bookContentWindow,
                                               pages) )
        {
            return false;
        }

        adaptationCommitted = true;
        return true;
    }
#endif // wxUSE_BOOKCTRL

#if wxUSE_BUTTON
    // Arbitrary-dialog adaptation uses the same two-stage transaction. The
    // sizer search itself historically detaches button items, hence its own
    // topology snapshot is restored if any later callback refuses or destroys
    // an endpoint.
    wxDialogSizerTopologySnapshot sizerTopology(dialogSizer);
    wxScrolledWindow* const scrolledWindow = CreateScrolledWindow(dialog);
    const wxWeakRef<wxWindow> scrolledWindowLifetime(scrolledWindow);
    if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) ||
         dialog->GetSizer() != dialogSizer ||
         !wxWeakWindowIsAvailableForCallbacks(scrolledWindowLifetime,
                                               scrolledWindow) ||
         scrolledWindow->GetParent() != dialog )
    {
        wxDiscardProvisionalScrolledWindow(scrolledWindow,
                                            scrolledWindowLifetime);
        return false;
    }

    wxSizer* buttonSizer = nullptr;
    bool createdButtonSizer = false;
    wxBoxSizer* newTopSizer = nullptr;
    std::unique_ptr<wxDialogReparentTransaction> reparenting;
    const wxSize originalSize = dialog->GetSize();
    const wxSize originalMinSize = dialog->GetMinSize();
    const wxSize originalMaxSize = dialog->GetMaxSize();
    const bool originalAdaptationDone = dialog->GetLayoutAdaptationDone();
    bool structurePublished = false;
    bool geometryMayHaveChanged = false;
    bool adaptationCommitted = false;
    const wxScopeGuard rollbackAdaptation = wxMakeGuard(
        [&]()
        {
            if ( adaptationCommitted )
                return;

            if ( reparenting && !reparenting->Restore() )
                return;

            wxDialog* const liveDialog =
                wxDynamicCast(dialogLifetime.get(), wxDialog);
            wxScrolledWindow* const liveScroller = wxDynamicCast(
                scrolledWindowLifetime.get(), wxScrolledWindow);
            if ( liveDialog != dialog || liveScroller != scrolledWindow ||
                 wxWindowItselfIsUnavailableForCallbacks(liveDialog) ||
                 wxWindowItselfIsUnavailableForCallbacks(liveScroller) )
            {
                return;
            }

            if ( structurePublished )
            {
                if ( liveDialog->GetSizer() != newTopSizer ||
                     liveScroller->GetSizer() != dialogSizer )
                {
                    return;
                }

                liveScroller->SetSizer(nullptr, false);
                newTopSizer->Detach(liveScroller);
                if ( buttonSizer )
                    newTopSizer->Detach(buttonSizer);
                liveDialog->SetSizer(dialogSizer, true);
            }

            const bool sizersRestored =
                sizerTopology.Restore(createdButtonSizer
                                          ? buttonSizer
                                          : nullptr);
            if ( createdButtonSizer && buttonSizer && sizersRestored )
                delete buttonSizer;

            if ( sizersRestored &&
                 scrolledWindowLifetime.get() == scrolledWindow &&
                 scrolledWindow->GetChildren().empty() )
            {
                delete scrolledWindow;
            }

            if ( dialogLifetime.get() == dialog &&
                 !wxWindowItselfIsUnavailableForCallbacks(dialog) )
            {
                dialog->SetLayoutAdaptationDone(originalAdaptationDone);
                if ( geometryMayHaveChanged )
                {
                    dialog->SetMinSize(originalMinSize);
                    if ( dialogLifetime.get() == dialog &&
                         !wxWindowItselfIsUnavailableForCallbacks(dialog) )
                    {
                        dialog->SetMaxSize(originalMaxSize);
                        dialog->SetSize(originalSize);
                    }
                }
            }
        });
    wxUnusedVar(rollbackAdaptation);

    const auto dialogStateIsAvailable = [&]()
    {
        return wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) &&
               dialog->GetSizer() == dialogSizer &&
               wxWeakWindowIsAvailableForCallbacks(
                   scrolledWindowLifetime, scrolledWindow) &&
               scrolledWindow->GetParent() == dialog;
    };

    int buttonSizerBorder = 0;
    buttonSizer = FindButtonSizer(true,
                                  dialog,
                                  dialogSizer,
                                  buttonSizerBorder);
    if ( !dialogStateIsAvailable() )
        return false;

    if ( !buttonSizer &&
         dialog->GetLayoutAdaptationLevel() >
             wxDIALOG_ADAPTATION_STANDARD_SIZER )
    {
        buttonSizer = FindButtonSizer(false,
                                      dialog,
                                      dialogSizer,
                                      buttonSizerBorder);
        if ( !dialogStateIsAvailable() )
            return false;
    }

    if ( !buttonSizer &&
         dialog->GetLayoutAdaptationLevel() >
             wxDIALOG_ADAPTATION_ANY_SIZER )
    {
        int count = 0;
        wxStdDialogButtonSizer* const stdButtonSizer =
            new wxStdDialogButtonSizer;
        buttonSizer = stdButtonSizer;
        createdButtonSizer = true;

        const bool found = FindLooseButtons(dialog,
                                            stdButtonSizer,
                                            dialogSizer,
                                            count);
        if ( !found || !dialogStateIsAvailable() )
            return false;

        if ( count > 0 )
        {
            stdButtonSizer->Realize();
            if ( !dialogStateIsAvailable() )
                return false;
        }
        else
        {
            delete buttonSizer;
            buttonSizer = nullptr;
            createdButtonSizer = false;
        }
    }

    if ( buttonSizer && !sizerTopology.ContainsSizer(buttonSizer) )
        createdButtonSizer = true;

    if ( buttonSizerBorder == 0 )
        buttonSizerBorder = 5;

    reparenting.reset(new wxDialogReparentTransaction(dialog,
                                                       scrolledWindow,
                                                       buttonSizer));
    if ( !reparenting->IsValid() )
        return false;

    {
        wxDialogReparentTransactionScope transactionScope(*reparenting);
        ReparentControls(dialog, scrolledWindow, buttonSizer);
    }
    if ( !dialogStateIsAvailable() || !reparenting->WasApplied() )
        return false;

    newTopSizer = new wxBoxSizer(wxVERTICAL);
    newTopSizer->Add(scrolledWindow, 1, wxEXPAND | wxALL, 0);
    if ( buttonSizer )
    {
        newTopSizer->Add(buttonSizer,
                         0,
                         wxEXPAND | wxALL,
                         buttonSizerBorder);
    }

    dialog->SetSizer(newTopSizer,
                     false /* don't delete the old sizer */);
    scrolledWindow->SetSizer(dialogSizer);
    structurePublished = true;
    if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) ||
         dialog->GetSizer() != newTopSizer ||
         !wxWeakWindowIsAvailableForCallbacks(scrolledWindowLifetime,
                                               scrolledWindow) ||
         scrolledWindow->GetSizer() != dialogSizer )
    {
        return false;
    }

    geometryMayHaveChanged = true;
    if ( !FitWithScrolling(dialog, scrolledWindow) ||
         !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) ||
         dialog->GetSizer() != newTopSizer ||
         !wxWeakWindowIsAvailableForCallbacks(scrolledWindowLifetime,
                                               scrolledWindow) ||
         scrolledWindow->GetSizer() != dialogSizer )
    {
        return false;
    }

    dialog->SetLayoutAdaptationDone(true);
    if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) ||
         dialog->GetSizer() != newTopSizer )
    {
        return false;
    }

    adaptationCommitted = true;
    return true;
#else
    wxUnusedVar(dialogSizer);
    return false;
#endif // wxUSE_BUTTON
}

// Create the scrolled window
wxScrolledWindow* wxStandardDialogLayoutAdapter::CreateScrolledWindow(wxWindow* parent)
{
    wxScrolledWindow* scrolledWindow = new wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxVSCROLL|wxHSCROLL|wxBORDER_NONE);
    return scrolledWindow;
}

#if wxUSE_BUTTON

/// Find and remove the button sizer, if any
wxSizer* wxStandardDialogLayoutAdapter::FindButtonSizer(bool stdButtonSizer, wxDialog* dialog, wxSizer* sizer, int& retBorder, int accumlatedBorder)
{
    for ( wxSizerItemList::compatibility_iterator node = sizer->GetChildren().GetFirst();
          node; node = node->GetNext() )
    {
        wxSizerItem *item = node->GetData();
        wxSizer *childSizer = item->GetSizer();

        if ( childSizer )
        {
            int newBorder = accumlatedBorder;
            if (item->GetFlag() & wxALL)
                newBorder += item->GetBorder();

            if (stdButtonSizer) // find wxStdDialogButtonSizer
            {
                wxStdDialogButtonSizer* buttonSizer = wxDynamicCast(childSizer, wxStdDialogButtonSizer);
                if (buttonSizer)
                {
                    if ( !wxDetachDialogSizerItem(sizer, childSizer) )
                        return nullptr;
                    retBorder = newBorder;
                    return buttonSizer;
                }
            }
            else // find a horizontal box sizer containing standard buttons
            {
                wxBoxSizer* buttonSizer = wxDynamicCast(childSizer, wxBoxSizer);
                if (buttonSizer && IsOrdinaryButtonSizer(dialog, buttonSizer))
                {
                    if ( !wxDetachDialogSizerItem(sizer, childSizer) )
                        return nullptr;
                    retBorder = newBorder;
                    return buttonSizer;
                }
            }

            wxSizer* s = FindButtonSizer(stdButtonSizer, dialog, childSizer, retBorder, newBorder);
            if (s)
                return s;
        }
    }
    return nullptr;
}

/// Check if this sizer contains standard buttons, and so can be repositioned in the dialog
bool wxStandardDialogLayoutAdapter::IsOrdinaryButtonSizer(wxDialog* dialog, wxBoxSizer* sizer)
{
    if (sizer->GetOrientation() != wxHORIZONTAL)
        return false;

    for ( wxSizerItemList::compatibility_iterator node = sizer->GetChildren().GetFirst();
          node; node = node->GetNext() )
    {
        wxSizerItem *item = node->GetData();
        wxButton *childButton = wxDynamicCast(item->GetWindow(), wxButton);

        if (childButton && IsStandardButton(dialog, childButton))
            return true;
    }
    return false;
}

/// Check if this is a standard button
bool wxStandardDialogLayoutAdapter::IsStandardButton(wxDialog* dialog, wxButton* button)
{
    wxWindowID id = button->GetId();

    return (id == wxID_OK || id == wxID_CANCEL || id == wxID_YES || id == wxID_NO || id == wxID_SAVE ||
            id == wxID_APPLY || id == wxID_HELP || id == wxID_CONTEXT_HELP || dialog->IsMainButtonId(id));
}

/// Find 'loose' main buttons in the existing layout and add them to the standard dialog sizer
bool wxStandardDialogLayoutAdapter::FindLooseButtons(wxDialog* dialog, wxStdDialogButtonSizer* buttonSizer, wxSizer* sizer, int& count)
{
    wxSizerItemList::compatibility_iterator node = sizer->GetChildren().GetFirst();
    while (node)
    {
        wxSizerItemList::compatibility_iterator next = node->GetNext();
        wxSizerItem *item = node->GetData();
        wxSizer *childSizer = item->GetSizer();
        wxButton *childButton = wxDynamicCast(item->GetWindow(), wxButton);

        if (childButton && IsStandardButton(dialog, childButton))
        {
            if ( !wxDetachDialogSizerItem(sizer, childButton) )
                return false;
            buttonSizer->AddButton(childButton);
            count ++;
        }

        if (childSizer &&
            !FindLooseButtons(dialog, buttonSizer, childSizer, count))
        {
            return false;
        }

        node = next;
    }
    return true;
}

#endif // wxUSE_BUTTON

/// Reparent the controls to the scrolled window
void wxStandardDialogLayoutAdapter::ReparentControls(wxWindow* parent, wxWindow* reparentTo, wxSizer* buttonSizer)
{
    DoReparentControls(parent, reparentTo, buttonSizer);
}

void wxStandardDialogLayoutAdapter::DoReparentControls(wxWindow* parent, wxWindow* reparentTo, wxSizer* buttonSizer)
{
    wxDialogReparentTransaction* transaction =
        FindActiveDialogReparentTransaction(parent,
                                            reparentTo,
                                            buttonSizer);
    std::unique_ptr<wxDialogReparentTransaction> localTransaction;
    if ( !transaction )
    {
        localTransaction.reset(
            new wxDialogReparentTransaction(parent,
                                            reparentTo,
                                            buttonSizer));
        transaction = localTransaction.get();
    }

    if ( !transaction->IsValid() )
        return;

    const bool restoreLocally = localTransaction.get() != nullptr;
    const auto restoreIfLocallyOwned = [&]()
    {
        if ( restoreLocally )
            (void)transaction->Restore();
    };

    struct ReparentCandidate
    {
        explicit ReparentCandidate(wxWindow* window_)
            : window(window_), lifetime(window_)
        {
        }

        wxWindow* window;
        wxWeakRef<wxWindow> lifetime;
    };

    const wxWeakRef<wxWindow> parentLifetime(parent);
    const wxWeakRef<wxWindow> destinationLifetime(reparentTo);
    if ( !wxWeakWindowIsAvailableForCallbacks(parentLifetime, parent) ||
         !wxWeakWindowIsAvailableForCallbacks(destinationLifetime,
                                              reparentTo) )
    {
        transaction->MarkFailed();
        return;
    }

    // Snapshot the eligible children before invoking Reparent(): application
    // overrides may destroy or move any sibling and invalidate the live child
    // list as well as buttonSizer.
    std::vector<ReparentCandidate> candidates;
    for ( wxWindowList::compatibility_iterator node =
              parent->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow* const window = node->GetData();
        if ( window != reparentTo &&
             (!buttonSizer || !buttonSizer->GetItem(window)) )
        {
            candidates.emplace_back(window);
        }
    }

    for ( const ReparentCandidate& candidate : candidates )
    {
        if ( !wxWeakWindowIsAvailableForCallbacks(parentLifetime, parent) ||
             !wxWeakWindowIsAvailableForCallbacks(destinationLifetime,
                                                  reparentTo) )
        {
            transaction->MarkFailed();
            restoreIfLocallyOwned();
            return;
        }

        wxWindow* const win = candidate.window;
        if ( !wxWeakWindowIsAvailableForCallbacks(candidate.lifetime, win) ||
             win->GetParent() != parent )
        {
            continue;
        }

        const bool reparented = win->Reparent(reparentTo);
        if ( !wxWeakWindowIsAvailableForCallbacks(parentLifetime, parent) ||
             !wxWeakWindowIsAvailableForCallbacks(destinationLifetime,
                                                  reparentTo) ||
             !wxWeakWindowIsAvailableForCallbacks(candidate.lifetime, win) ||
             !reparented || win->GetParent() != reparentTo )
        {
            transaction->MarkFailed();
            restoreIfLocallyOwned();
            return;
        }

#ifdef __WXMSW__
        // Restore correct tab order, but don't reuse win after its virtual
        // Reparent() without first validating the tracked identity above.
        const HWND hwnd = static_cast<HWND>(win->GetHWND());
        if ( !wxWeakWindowIsAvailableForCallbacks(candidate.lifetime, win) )
        {
            transaction->MarkFailed();
            restoreIfLocallyOwned();
            return;
        }

        if ( hwnd )
        {
            ::SetWindowPos(hwnd, HWND_BOTTOM, -1, -1, -1, -1,
                           SWP_NOMOVE | SWP_NOSIZE);
        }
#endif
    }

    if ( !transaction->WasApplied() )
    {
        transaction->MarkFailed();
        restoreIfLocallyOwned();
    }
}

/// Find whether scrolling will be necessary for the dialog, returning wxVERTICAL, wxHORIZONTAL or both
int wxStandardDialogLayoutAdapter::MustScroll(wxDialog* dialog, wxSize& windowSize, wxSize& displaySize)
{
    return DoMustScroll(dialog, windowSize, displaySize);
}

/// Find whether scrolling will be necessary for the dialog, returning wxVERTICAL, wxHORIZONTAL or both
int wxStandardDialogLayoutAdapter::DoMustScroll(wxDialog* dialog, wxSize& windowSize, wxSize& displaySize)
{
    const wxWeakRef<wxWindow> dialogLifetime(dialog);
    if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) )
        return 0;

    wxSizer* const sizer = dialog->GetSizer();
    if ( !sizer )
        return 0;

    const auto dialogStateIsValid = [&dialogLifetime, dialog, sizer]()
    {
        return wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) &&
               dialog->GetSizer() == sizer;
    };

    wxSize minWindowSize = sizer->GetMinSize();
    if ( !dialogStateIsValid() )
        return 0;

    windowSize = dialog->GetSize();
    if ( !dialogStateIsValid() )
        return 0;

    windowSize = wxSize(wxMax(windowSize.x, minWindowSize.x), wxMax(windowSize.y, minWindowSize.y));
    const wxDisplay display(dialog);
    if ( !dialogStateIsValid() )
        return 0;

    displaySize = display.GetClientArea().GetSize();
    if ( !dialogStateIsValid() )
        return 0;

    int flags = 0;

    if (windowSize.y >= (displaySize.y - wxEXTRA_DIALOG_HEIGHT))
        flags |= wxVERTICAL;
    if (windowSize.x >= displaySize.x)
        flags |= wxHORIZONTAL;

    return flags;
}

// A function to fit the dialog around its contents, and then adjust for screen size.
// If scrolled windows are passed, scrolling is enabled in the required orientation(s).
bool wxStandardDialogLayoutAdapter::FitWithScrolling(wxDialog* dialog, wxWindowList& windows)
{
    return DoFitWithScrolling(dialog, windows);
}

// A function to fit the dialog around its contents, and then adjust for screen size.
// If a scrolled window is passed, scrolling is enabled in the required orientation(s).
bool wxStandardDialogLayoutAdapter::FitWithScrolling(wxDialog* dialog, wxScrolledWindow* scrolledWindow)
{
    return DoFitWithScrolling(dialog, scrolledWindow);
}

// A function to fit the dialog around its contents, and then adjust for screen size.
// If a scrolled window is passed, scrolling is enabled in the required orientation(s).
bool wxStandardDialogLayoutAdapter::DoFitWithScrolling(wxDialog* dialog, wxScrolledWindow* scrolledWindow)
{
    wxWindowList windows;
    windows.Append(scrolledWindow);
    return DoFitWithScrolling(dialog, windows);
}

bool wxStandardDialogLayoutAdapter::DoFitWithScrolling(wxDialog* dialog, wxWindowList& windows)
{
    struct ScrollingWindow
    {
        explicit ScrollingWindow(wxWindow* window_)
            : window(window_), lifetime(window_)
        {
        }

        wxWindow* window;
        wxWeakRef<wxWindow> lifetime;
    };

    const wxWeakRef<wxWindow> dialogLifetime(dialog);
    if ( !wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) )
        return false;

    wxSizer* const sizer = dialog->GetSizer();
    if (!sizer)
        return false;

    std::vector<ScrollingWindow> scrollingWindows;
    scrollingWindows.reserve(windows.GetCount());
    for ( wxWindowList::compatibility_iterator node = windows.GetFirst();
          node;
          node = node->GetNext() )
    {
        scrollingWindows.emplace_back(node->GetData());
    }

    const auto dialogStateIsValid = [&dialogLifetime, dialog, sizer]()
    {
        return wxWeakWindowIsAvailableForCallbacks(dialogLifetime, dialog) &&
               dialog->GetSizer() == sizer;
    };

    sizer->SetSizeHints(dialog);
    if ( !dialogStateIsValid() )
        return false;

    wxSize windowSize, displaySize;
    int scrollFlags = DoMustScroll(dialog, windowSize, displaySize);
    if ( !dialogStateIsValid() )
        return false;

    if (scrollFlags)
    {
        int scrollBarExtraX = 0, scrollBarExtraY = 0;
        bool resizeHorizontally = (scrollFlags & wxHORIZONTAL) != 0;
        bool resizeVertically = (scrollFlags & wxVERTICAL) != 0;

        if (windows.GetCount() != 0)
        {
            // Allow extra for a scrollbar, assuming we resizing in one direction only.
            int scrollBarSize = 20;
            if ((resizeVertically && !resizeHorizontally) && (windowSize.x < (displaySize.x - scrollBarSize)))
                scrollBarExtraX = scrollBarSize;
            if ((resizeHorizontally && !resizeVertically) && (windowSize.y < (displaySize.y - scrollBarSize)))
                scrollBarExtraY = scrollBarSize;
        }

        for ( const ScrollingWindow& tracked : scrollingWindows )
        {
            if ( !dialogStateIsValid() ||
                 !wxWeakWindowIsAvailableForCallbacks(tracked.lifetime,
                                                      tracked.window) )
            {
                return false;
            }

            wxWindow* const win = tracked.window;
            wxScrolledWindow* scrolledWindow = wxDynamicCast(win, wxScrolledWindow);
            if (scrolledWindow)
            {
                scrolledWindow->SetScrollRate(resizeHorizontally ? 10 : 0, resizeVertically ? 10 : 0);
                if ( !dialogStateIsValid() ||
                     !wxWeakWindowIsAvailableForCallbacks(tracked.lifetime,
                                                          win) )
                {
                    return false;
                }

                wxSizer* const scrollingSizer = scrolledWindow->GetSizer();
                if (scrollingSizer)
                {
                    scrollingSizer->Fit(scrolledWindow);
                    if ( !dialogStateIsValid() ||
                         !wxWeakWindowIsAvailableForCallbacks(
                             tracked.lifetime, win) ||
                         scrolledWindow->GetSizer() != scrollingSizer )
                    {
                        return false;
                    }
                }
            }
        }

        wxSize limitTo = windowSize + wxSize(scrollBarExtraX, scrollBarExtraY);
        if (resizeVertically)
            limitTo.y = displaySize.y - wxEXTRA_DIALOG_HEIGHT;
        if (resizeHorizontally)
            limitTo.x = displaySize.x;

        dialog->SetMinSize(limitTo);
        if ( !dialogStateIsValid() )
            return false;

        dialog->SetSize(limitTo);
        if ( !dialogStateIsValid() )
            return false;

        const int maxWidth = dialog->GetMaxWidth();
        const int maxHeight = dialog->GetMaxHeight();
        if ( !dialogStateIsValid() )
            return false;

        dialog->SetSizeHints(limitTo.x, limitTo.y, maxWidth, maxHeight);
        if ( !dialogStateIsValid() )
            return false;
    }

    return true;
}

/*!
 * Module to initialise standard adapter
 */

class wxDialogLayoutAdapterModule: public wxModule
{
    wxDECLARE_DYNAMIC_CLASS(wxDialogLayoutAdapterModule);
public:
    wxDialogLayoutAdapterModule() {}
    virtual void OnExit() override { delete wxDialogBase::SetLayoutAdapter(nullptr); }
    virtual bool OnInit() override { wxDialogBase::SetLayoutAdapter(new wxStandardDialogLayoutAdapter); return true; }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxDialogLayoutAdapterModule, wxModule);
