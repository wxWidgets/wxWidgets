/////////////////////////////////////////////////////////////////////////////
// Name:        common/appcmn.cpp
// Purpose:     wxAppConsole and wxAppBase methods common to all platforms
// Author:      Vadim Zeitlin
// Modified by:
// Created:     18.10.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "appbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/bitmap.h"
    #include "wx/intl.h"
    #include "wx/list.h"
    #include "wx/log.h"
    #include "wx/msgdlg.h"
    #include "wx/bitmap.h"
    #include "wx/confbase.h"
#endif

#include "wx/apptrait.h"
#if wxUSE_FONTMAP
    #include "wx/fontmap.h"
#endif // wxUSE_FONTMAP
#include "wx/msgout.h"
#include "wx/thread.h"
#include "wx/utils.h"

// ============================================================================
// wxAppBase implementation
// ============================================================================

// ----------------------------------------------------------------------------
// initialization
// ----------------------------------------------------------------------------

wxAppBase::wxAppBase()
{
    m_topWindow = (wxWindow *)NULL;
    m_useBestVisual = FALSE;
    m_isActive = TRUE;

    // We don't want to exit the app if the user code shows a dialog from its
    // OnInit() -- but this is what would happen if we set m_exitOnFrameDelete
    // to Yes initially as this dialog would be the last top level window.
    // OTOH, if we set it to No initially we'll have to overwrite it with Yes
    // when we enter our OnRun() because we do want the default behaviour from
    // then on. But this would be a problem if the user code calls
    // SetExitOnFrameDelete(FALSE) from OnInit().
    //
    // So we use the special "Later" value which is such that
    // GetExitOnFrameDelete() returns FALSE for it but which we know we can
    // safely (i.e. without losing the effect of the users SetExitOnFrameDelete
    // call) overwrite in OnRun()
    m_exitOnFrameDelete = Later;
}

bool wxAppBase::Initialize(int& argc, wxChar **argv)
{
    if ( !wxAppConsole::Initialize(argc, argv) )
        return false;

#if wxUSE_THREADS
    wxPendingEventsLocker = new wxCriticalSection;
#endif

    wxTheColourDatabase = new wxColourDatabase(wxKEY_STRING);
    wxTheColourDatabase->Initialize();

    wxInitializeStockLists();
    wxInitializeStockObjects();

    wxBitmap::InitStandardHandlers();

    return true;
}

// ----------------------------------------------------------------------------
// cleanup
// ----------------------------------------------------------------------------

wxAppBase::~wxAppBase()
{
    // this destructor is required for Darwin
}

void wxAppBase::CleanUp()
{
    // one last chance for pending objects to be cleaned up
    DeletePendingObjects();

    wxBitmap::CleanUpHandlers();

    wxDeleteStockObjects();

    wxDeleteStockLists();

    delete wxTheColourDatabase;
    wxTheColourDatabase = NULL;

#if wxUSE_THREADS
    delete wxPendingEvents;
    wxPendingEvents = NULL;

    delete wxPendingEventsLocker;
    wxPendingEventsLocker = NULL;

#if wxUSE_VALIDATORS
    // If we don't do the following, we get an apparent memory leak.
    ((wxEvtHandler&) wxDefaultValidator).ClearEventLocker();
#endif // wxUSE_VALIDATORS
#endif // wxUSE_THREADS
}

// ----------------------------------------------------------------------------
// OnXXX() hooks
// ----------------------------------------------------------------------------

bool wxAppBase::OnInitGui()
{
#ifdef __WXUNIVERSAL__
    if ( !wxTheme::Get() && !wxTheme::CreateDefault() )
        return FALSE;
#endif // __WXUNIVERSAL__

    return TRUE;
}

int wxAppBase::OnRun()
{
    // see the comment in ctor: if the initial value hasn't been changed, use
    // the default Yes from now on
    if ( m_exitOnFrameDelete == Later )
    {
        m_exitOnFrameDelete = Yes;
    }
    //else: it has been changed, assume the user knows what he is doing

    return MainLoop();
}

void wxAppBase::Exit()
{
    ExitMainLoop();
}

wxAppTraits *wxAppBase::CreateTraits()
{
    return new wxGUIAppTraits;
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

void wxAppBase::SetActive(bool active, wxWindow * WXUNUSED(lastFocus))
{
    if ( active == m_isActive )
        return;

    m_isActive = active;

    wxActivateEvent event(wxEVT_ACTIVATE_APP, active);
    event.SetEventObject(this);

    (void)ProcessEvent(event);
}

void wxAppBase::DeletePendingObjects()
{
    wxNode *node = wxPendingDelete.GetFirst();
    while (node)
    {
        wxObject *obj = node->GetData();

        delete obj;

        if (wxPendingDelete.Member(obj))
            delete node;

        // Deleting one object may have deleted other pending
        // objects, so start from beginning of list again.
        node = wxPendingDelete.GetFirst();
    }
}

// ----------------------------------------------------------------------------
// wxGUIAppTraitsBase
// ----------------------------------------------------------------------------

#if wxUSE_LOG

wxLog *wxGUIAppTraitsBase::CreateLogTarget()
{
    return new wxLogGui;
}

#endif // wxUSE_LOG

wxMessageOutput *wxGUIAppTraitsBase::CreateMessageOutput()
{
    // The standard way of printing help on command line arguments (app --help)
    // is (according to common practice):
    //     - console apps: to stderr (on any platform)
    //     - GUI apps: stderr on Unix platforms (!)
    //                 message box under Windows and others
#ifdef __UNIX__
    return new wxMessageOutputStderr;
#else // !__UNIX__
    // wxMessageOutputMessageBox doesn't work under Motif
    #ifdef __WXMOTIF__
        return new wxMessageOutputLog;
    #else
        return new wxMessageOutputMessageBox;
    #endif
#endif // __UNIX__/!__UNIX__
}

#if wxUSE_FONTMAP

wxFontMapper *wxGUIAppTraitsBase::CreateFontMapper()
{
    return new wxFontMapper;
}

#endif // wxUSE_FONTMAP

#ifdef __WXDEBUG__

bool wxGUIAppTraitsBase::ShowAssertDialog(const wxString& msg)
{
    // under MSW we prefer to use the base class version using ::MessageBox()
    // even if wxMessageBox() is available because it has less chances to
    // double fault our app than our wxMessageBox()
#if defined(__WXMSW__) || !wxUSE_MSGDLG
    return wxAppTraitsBase::ShowAssertDialog(msg);
#else // wxUSE_MSGDLG
    // this message is intentionally not translated -- it is for
    // developpers only
    wxString msgDlg(msg);
    msgDlg += wxT("\nDo you want to stop the program?\n")
              wxT("You can also choose [Cancel] to suppress ")
              wxT("further warnings.");

    switch ( wxMessageBox(msgDlg, wxT("wxWindows Debug Alert"),
                          wxYES_NO | wxCANCEL | wxICON_STOP ) )
    {
        case wxYES:
            wxTrap();
            break;

        case wxCANCEL:
            // no more asserts
            return true;

        //case wxNO: nothing to do
    }

    return false;
#endif // !wxUSE_MSGDLG/wxUSE_MSGDLG
}

#endif // __WXDEBUG__

bool wxGUIAppTraitsBase::HasStderr()
{
    // we consider that under Unix stderr always goes somewhere, even if the
    // user doesn't always see it under GUI desktops
#ifdef __UNIX__
    return true;
#else
    return false;
#endif
}

void wxGUIAppTraitsBase::ScheduleForDestroy(wxObject *object)
{
    if ( !wxPendingDelete.Member(object) )
        wxPendingDelete.Append(object);
}

void wxGUIAppTraitsBase::RemoveFromPendingDelete(wxObject *object)
{
    wxPendingDelete.DeleteObject(object);
}

