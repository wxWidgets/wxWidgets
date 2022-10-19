/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/progdlg.cpp
// Purpose:     wxProgressDialog
// Author:      Rickard Westerlund
// Created:     2010-07-22
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// Declarations
// ============================================================================

// ----------------------------------------------------------------------------
// Headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_PROGRESSDLG && wxUSE_THREADS && wxUSE_NATIVE_PROGRESSDLG

#include "wx/progdlg.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/msgdlg.h"
    #include "wx/stopwatch.h"
    #include "wx/msw/private.h"
#endif

#include "wx/msw/private/msgdlg.h"
#include "wx/evtloop.h"

using namespace wxMSWMessageDialog;

#ifdef wxHAS_MSW_TASKDIALOG

// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------

namespace
{

// Notification values of wxProgressDialogSharedData::m_notifications
const int wxSPDD_VALUE_CHANGED     = 0x0001;
const int wxSPDD_RANGE_CHANGED     = 0x0002;
const int wxSPDD_PBMARQUEE_CHANGED = 0x0004;
const int wxSPDD_TITLE_CHANGED     = 0x0008;
const int wxSPDD_MESSAGE_CHANGED   = 0x0010;
const int wxSPDD_EXPINFO_CHANGED   = 0x0020;
const int wxSPDD_ENABLE_SKIP       = 0x0040;
const int wxSPDD_ENABLE_ABORT      = 0x0080;
const int wxSPDD_DISABLE_SKIP      = 0x0100;
const int wxSPDD_FINISHED          = 0x0400;
const int wxSPDD_DESTROYED         = 0x0800;
const int wxSPDD_ICON_CHANGED      = 0x1000;
const int wxSPDD_WINDOW_MOVED      = 0x2000;

const int Id_SkipBtn = wxID_HIGHEST + 1;

} // anonymous namespace

// ============================================================================
// Helper classes
// ============================================================================

// Class used to share data between the main thread and the task dialog runner.
class wxProgressDialogSharedData
{
public:
    wxProgressDialogSharedData()
    {
        m_hwnd = 0;
        m_value = 0;
        m_progressBarMarquee = false;
        m_skipped = false;
        m_msgChangeElementText = TDM_UPDATE_ELEMENT_TEXT;
        m_notifications = 0;
        m_parent = nullptr;
    }

    wxCriticalSection m_cs;

    wxWindow *m_parent;     // Parent window only used to center us over it.
    HWND m_hwnd;            // Task dialog handler
    long m_style;           // wxProgressDialog style
    int m_value;
    int m_range;
    wxString m_title;
    wxString m_message;
    wxString m_expandedInformation;
    wxString m_labelCancel; // Privately used by callback.
    unsigned long m_timeStop;
    wxIcon m_iconSmall;
    wxIcon m_iconBig;
    wxPoint m_winPosition;

    wxProgressDialog::State m_state;
    bool m_progressBarMarquee;
    bool m_skipped;

    // The task dialog message to use for changing the text of its elements:
    // it is set to TDM_SET_ELEMENT_TEXT by Fit() to let the dialog adjust
    // itself to the size of its elements during the next update, but otherwise
    // TDM_UPDATE_ELEMENT_TEXT is used in order to prevent the dialog from
    // performing a layout on each update, which is annoying as it can result
    // in its size constantly changing.
    int m_msgChangeElementText;

    // Bit field that indicates fields that have been modified by the
    // main thread so the task dialog runner knows what to update.
    int m_notifications;


    // Helper function to split a single message, passed via our public API,
    // into the title and the main content body used by the native dialog.
    //
    // Note that it uses m_message and so must be called with m_cs locked.
    void SplitMessageIntoTitleAndBody(wxString& title, wxString& body) const
    {
        title = m_message;

        const size_t posNL = title.find('\n');
        if ( posNL != wxString::npos )
        {
            // There can an extra new line between the first and subsequent
            // lines to separate them as it looks better with the generic
            // version -- but in this one, they're already separated by the use
            // of different dialog elements, so suppress the extra new line.
            int numNLs = 1;
            if ( posNL < title.length() - 1 && title[posNL + 1] == '\n' )
                numNLs++;

            body.assign(title, posNL + numNLs, wxString::npos);
            title.erase(posNL);
        }
        else // A single line
        {
            // Don't use title without the body, this doesn't make sense.
            body.clear();
            title.swap(body);
        }
    }
};

// Runner thread that takes care of displaying and updating the
// task dialog.
class wxProgressDialogTaskRunner : public wxThread
{
public:
    wxProgressDialogTaskRunner()
        : wxThread(wxTHREAD_JOINABLE)
        { }

    wxProgressDialogSharedData* GetSharedDataObject()
        { return &m_sharedData; }

private:
    wxProgressDialogSharedData m_sharedData;

    virtual void* Entry() override;

    static HRESULT CALLBACK TaskDialogCallbackProc(HWND hwnd,
                                                   UINT uNotification,
                                                   WPARAM wParam,
                                                   LPARAM lParam,
                                                   LONG_PTR dwRefData);
};

namespace
{

// A custom event loop which runs until the state of the dialog becomes
// "Dismissed".
class wxProgressDialogModalLoop : public wxEventLoop
{
public:
    wxProgressDialogModalLoop(wxProgressDialogSharedData& data)
        : m_data(data)
    {
    }

protected:
    virtual void OnNextIteration() override
    {
        wxCriticalSectionLocker locker(m_data.m_cs);

        if ( m_data.m_state == wxProgressDialog::Dismissed )
            Exit();
    }

    wxProgressDialogSharedData& m_data;

    wxDECLARE_NO_COPY_CLASS(wxProgressDialogModalLoop);
};

// ============================================================================
// Helper functions
// ============================================================================

BOOL CALLBACK DisplayCloseButton(HWND hwnd, LPARAM lParam)
{
    wxProgressDialogSharedData *sharedData =
        (wxProgressDialogSharedData *) lParam;

    if ( wxGetWindowText( hwnd ) == sharedData->m_labelCancel )
    {
        sharedData->m_labelCancel = _("Close");
        SendMessage( hwnd, WM_SETTEXT, 0,
                     wxMSW_CONV_LPARAM(sharedData->m_labelCancel) );

        return FALSE;
    }

    return TRUE;
}

// This function enables or disables both the cancel button in the task dialog
// and the close button in its title bar, as they perform the same function and
// so should be kept in the same state.
void EnableCloseButtons(HWND hwnd, bool enable)
{
    ::SendMessage(hwnd, TDM_ENABLE_BUTTON, IDCANCEL, enable ? TRUE : FALSE);

    wxTopLevelWindow::MSWEnableCloseButton(hwnd, enable);
}

void PerformNotificationUpdates(HWND hwnd,
                                wxProgressDialogSharedData *sharedData)
{
    // Update the appropriate dialog fields.
    if ( sharedData->m_notifications & wxSPDD_RANGE_CHANGED )
    {
        ::SendMessage( hwnd,
                       TDM_SET_PROGRESS_BAR_RANGE,
                       0,
                       MAKELPARAM(0, sharedData->m_range) );
    }

    if ( sharedData->m_notifications & wxSPDD_VALUE_CHANGED )
    {
        // Use a hack to avoid progress bar animation: we can't afford to use
        // it because animating the gauge movement smoothly requires a
        // constantly running message loop and while it does run in this (task
        // dialog) thread, it is often blocked from proceeding by some lock
        // held by the main thread which is busy doing something and may not
        // dispatch events frequently enough. So, in practice, the animation
        // can lag far behind the real value and results in showing a wrong
        // value in the progress bar.
        //
        // To prevent this from happening, set the progress bar value to
        // something greater than its maximal value and then move it back: in
        // the current implementations of the progress bar control, moving its
        // position backwards does it directly, without using the animation,
        // which is exactly what we want here.
        //
        // Finally notice that this hack doesn't really work for the last
        // value, but while we could use a nested hack and temporarily increase
        // the progress bar range when the value is equal to it, it isn't
        // actually necessary in practice because when we reach the end of the
        // bar the dialog is either hidden immediately anyhow or the main
        // thread enters a modal event loop which does dispatch events and so
        // it's not a problem to have an animated transition in this particular
        // case.
        ::SendMessage( hwnd,
                       TDM_SET_PROGRESS_BAR_POS,
                       sharedData->m_value + 1,
                       0 );
        ::SendMessage( hwnd,
                       TDM_SET_PROGRESS_BAR_POS,
                       sharedData->m_value,
                       0 );
    }

    if ( sharedData->m_notifications & wxSPDD_PBMARQUEE_CHANGED )
    {
        BOOL val = sharedData->m_progressBarMarquee ? TRUE : FALSE;
        ::SendMessage( hwnd,
                       TDM_SET_MARQUEE_PROGRESS_BAR,
                       val,
                       0 );
        ::SendMessage( hwnd,
                       TDM_SET_PROGRESS_BAR_MARQUEE,
                       val,
                       0 );
    }

    if ( sharedData->m_notifications & wxSPDD_TITLE_CHANGED )
        ::SetWindowText( hwnd, sharedData->m_title.t_str() );

    if ( sharedData->m_notifications & wxSPDD_ICON_CHANGED )
    {
        ::SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)GetHiconOf(sharedData->m_iconSmall));
        ::SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)GetHiconOf(sharedData->m_iconBig));
    }

    if ( sharedData->m_notifications & wxSPDD_WINDOW_MOVED )
    {
        ::SetWindowPos(hwnd, nullptr, sharedData->m_winPosition.x, sharedData->m_winPosition.y,
                       -1, -1, // ignored
                       SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);
    }

    if ( sharedData->m_notifications & wxSPDD_MESSAGE_CHANGED )
    {
        // Split the message in the title string and the rest if it has
        // multiple lines.
        wxString title, body;
        sharedData->SplitMessageIntoTitleAndBody(title, body);

        ::SendMessage( hwnd,
                       sharedData->m_msgChangeElementText,
                       TDE_MAIN_INSTRUCTION,
                       wxMSW_CONV_LPARAM(title) );

        ::SendMessage( hwnd,
                       sharedData->m_msgChangeElementText,
                       TDE_CONTENT,
                       wxMSW_CONV_LPARAM(body) );

        // After using TDM_SET_ELEMENT_TEXT once, we don't want to use it for
        // the subsequent updates as it could result in dialog size changing
        // unexpectedly, so reset it (which does nothing if we had already done
        // it, of course, but it's not a problem).
        //
        // Notice that, contrary to its documentation, even using this message
        // still increases the dialog size if the new text is longer (at least
        // under Windows 7), but it doesn't shrink back if the text becomes
        // shorter later and stays at the bigger size which is still a big gain
        // as it prevents jumping back and forth between the smaller and larger
        // sizes.
        sharedData->m_msgChangeElementText = TDM_UPDATE_ELEMENT_TEXT;
    }

    if ( sharedData->m_notifications & wxSPDD_EXPINFO_CHANGED )
    {
        const wxString& expandedInformation =
            sharedData->m_expandedInformation;
        if ( !expandedInformation.empty() )
        {
            // Here we never need to use TDM_SET_ELEMENT_TEXT as the size of
            // the expanded information doesn't change drastically.
            //
            // Notice that TDM_UPDATE_ELEMENT_TEXT for this element only works
            // when using TDF_EXPAND_FOOTER_AREA, as we do. Without this flag,
            // only TDM_SET_ELEMENT_TEXT could be used as otherwise the dialog
            // layout becomes completely mangled (at least under Windows 7).
            ::SendMessage( hwnd,
                           TDM_UPDATE_ELEMENT_TEXT,
                           TDE_EXPANDED_INFORMATION,
                           wxMSW_CONV_LPARAM(expandedInformation) );
        }
    }

    if ( sharedData->m_notifications & wxSPDD_ENABLE_SKIP )
        ::SendMessage( hwnd, TDM_ENABLE_BUTTON, Id_SkipBtn, TRUE );

    if ( sharedData->m_notifications & wxSPDD_ENABLE_ABORT )
        EnableCloseButtons(hwnd, true);

    if ( sharedData->m_notifications & wxSPDD_DISABLE_SKIP )
        ::SendMessage( hwnd, TDM_ENABLE_BUTTON, Id_SkipBtn, FALSE );

    // Is the progress finished?
    if ( sharedData->m_notifications & wxSPDD_FINISHED )
    {
        sharedData->m_state = wxProgressDialog::Finished;

        if ( !(sharedData->m_style & wxPD_AUTO_HIDE) )
        {
            // Change Cancel into Close and activate the button.
            ::SendMessage( hwnd, TDM_ENABLE_BUTTON, Id_SkipBtn, FALSE );
            EnableCloseButtons(hwnd, true);
            ::EnumChildWindows( hwnd, DisplayCloseButton,
                                (LPARAM) sharedData );
        }
    }
}

} // anonymous namespace

#endif // wxHAS_MSW_TASKDIALOG

// ============================================================================
// wxProgressDialog implementation
// ============================================================================

wxProgressDialog::wxProgressDialog( const wxString& title,
                                    const wxString& message,
                                    int maximum,
                                    wxWindow *parent,
                                    int style )
    : wxGenericProgressDialog(),
      m_taskDialogRunner(nullptr),
      m_sharedData(nullptr),
      m_message(message),
      m_title(title)
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
    {
        SetTopParent(parent);
        SetPDStyle(style);
        SetMaximum(maximum);

        EnsureActiveEventLoopExists();
        Show();
        DisableOtherWindows();

        return;
    }
#endif // wxHAS_MSW_TASKDIALOG

    Create(title, message, maximum, parent, style);
}

wxProgressDialog::~wxProgressDialog()
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( !m_taskDialogRunner )
        return;

    if ( m_sharedData )
    {
        wxCriticalSectionLocker locker(m_sharedData->m_cs);
        m_sharedData->m_notifications |= wxSPDD_DESTROYED;
    }

    // We can't use simple wxThread::Wait() here as we would deadlock because
    // the task dialog thread expects this thread to process some messages
    // (presumably those the task dialog sends to its parent during its
    // destruction).
    const WXHANDLE hThread = m_taskDialogRunner->MSWGetHandle();
    for ( bool cont = true; cont; )
    {
        DWORD rc = ::MsgWaitForMultipleObjects
                     (
                       1,                   // number of objects to wait for
                       &hThread,            // the objects
                       false,               // wait for any objects, not all
                       INFINITE,            // no timeout
                       QS_ALLINPUT |        // return as soon as there are any events
                       QS_ALLPOSTMESSAGE
                     );

        switch ( rc )
        {
            case 0xFFFFFFFF:
                // This is unexpected, but we can't do anything about it and
                // probably shouldn't continue waiting as we risk doing it
                // forever.
                wxLogLastError("MsgWaitForMultipleObjectsEx");
                cont = false;
                break;

            case WAIT_OBJECT_0:
                // Thread has terminated.
                cont = false;
                break;

            default:
                // An event has arrive, so dispatch it.
                wxEventLoop::GetActive()->Dispatch();
        }
    }

    // Enable the windows before deleting the task dialog to ensure that we
    // can regain the activation.
    ReenableOtherWindows();

    delete m_taskDialogRunner;
#endif // wxHAS_MSW_TASKDIALOG
}

bool wxProgressDialog::Update(int value, const wxString& newmsg, bool *skip)
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
    {
        if ( !DoNativeBeforeUpdate(skip) )
        {
            // Dialog was cancelled.
            return false;
        }

        value /= m_factor;

        wxASSERT_MSG( value <= m_maximum, wxT("invalid progress value") );

        {
            wxCriticalSectionLocker locker(m_sharedData->m_cs);

            if ( value != m_sharedData->m_value )
            {
                m_sharedData->m_value = value;
                m_sharedData->m_notifications |= wxSPDD_VALUE_CHANGED;
            }

            if ( !newmsg.empty() && newmsg != m_message )
            {
                m_message = newmsg;
                m_sharedData->m_message = newmsg;
                m_sharedData->m_notifications |= wxSPDD_MESSAGE_CHANGED;
            }

            if ( m_sharedData->m_progressBarMarquee )
            {
                m_sharedData->m_progressBarMarquee = false;
                m_sharedData->m_notifications |= wxSPDD_PBMARQUEE_CHANGED;
            }

            UpdateExpandedInformation( value );

            // If we didn't just reach the finish, all we have to do is to
            // return true if the dialog wasn't cancelled and false otherwise.
            if ( value != m_maximum || m_state == Finished )
                return m_sharedData->m_state != Canceled;


            // On finishing, the dialog without wxPD_AUTO_HIDE style becomes a
            // modal one meaning that we must block here until the user
            // dismisses it.
            m_state = Finished;
            m_sharedData->m_state = Finished;
            m_sharedData->m_notifications |= wxSPDD_FINISHED;
            if ( HasPDFlag(wxPD_AUTO_HIDE) )
                return true;

            if ( newmsg.empty() )
            {
                // Provide the finishing message if the application didn't.
                m_message = _("Done.");
                m_sharedData->m_message = m_message;
                m_sharedData->m_notifications |= wxSPDD_MESSAGE_CHANGED;
            }
        } // unlock m_sharedData->m_cs

        // We only get here when we need to wait for the dialog to terminate so
        // do just this by running a custom event loop until the dialog is
        // dismissed.
        wxProgressDialogModalLoop loop(*m_sharedData);
        loop.Run();
        return true;
    }
#endif // wxHAS_MSW_TASKDIALOG

    return wxGenericProgressDialog::Update( value, newmsg, skip );
}

bool wxProgressDialog::Pulse(const wxString& newmsg, bool *skip)
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
    {
        if ( !DoNativeBeforeUpdate(skip) )
        {
            // Dialog was cancelled.
            return false;
        }

        wxCriticalSectionLocker locker(m_sharedData->m_cs);

        if ( !m_sharedData->m_progressBarMarquee )
        {
            m_sharedData->m_progressBarMarquee = true;
            m_sharedData->m_notifications |= wxSPDD_PBMARQUEE_CHANGED;
        }

        if ( !newmsg.empty() && newmsg != m_message )
        {
            m_message = newmsg;
            m_sharedData->m_message = newmsg;
            m_sharedData->m_notifications |= wxSPDD_MESSAGE_CHANGED;
        }

        // Value of 0 is special and is used when we can't estimate the
        // remaining and total times, which is exactly what we need here.
        UpdateExpandedInformation(0);

        return m_sharedData->m_state != Canceled;
    }
#endif // wxHAS_MSW_TASKDIALOG

    return wxGenericProgressDialog::Pulse( newmsg, skip );
}

void wxProgressDialog::DispatchEvents()
{
#ifdef wxHAS_MSW_TASKDIALOG
    // No need for HasNativeTaskDialog() check, we're only called when this is
    // the case.

    // We don't need to dispatch the user input events as the task dialog
    // handles its own ones in its thread and we shouldn't react to any
    // other user actions while the dialog is shown.
    wxEventLoop::GetActive()->
        YieldFor(wxEVT_CATEGORY_ALL & ~wxEVT_CATEGORY_USER_INPUT);
#else // !wxHAS_MSW_TASKDIALOG
    wxFAIL_MSG( "unreachable" );
#endif // wxHAS_MSW_TASKDIALOG/!wxHAS_MSW_TASKDIALOG
}

bool wxProgressDialog::DoNativeBeforeUpdate(bool *skip)
{
#ifdef wxHAS_MSW_TASKDIALOG
    DispatchEvents();

    wxCriticalSectionLocker locker(m_sharedData->m_cs);

    if ( m_sharedData->m_skipped  )
    {
        if ( skip && !*skip )
        {
            *skip = true;
            m_sharedData->m_skipped = false;
            m_sharedData->m_notifications |= wxSPDD_ENABLE_SKIP;
        }
    }

    if ( m_sharedData->m_state == Canceled )
        m_timeStop = m_sharedData->m_timeStop;

    return m_sharedData->m_state != Canceled;
#else // !wxHAS_MSW_TASKDIALOG
    wxUnusedVar(skip);
    wxFAIL_MSG( "unreachable" );

    return false;
#endif // wxHAS_MSW_TASKDIALOG/!wxHAS_MSW_TASKDIALOG
}

void wxProgressDialog::Resume()
{
    wxGenericProgressDialog::Resume();

#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
    {
        HWND hwnd;

        {
            wxCriticalSectionLocker locker(m_sharedData->m_cs);
            m_sharedData->m_state = Continue;

            // "Skip" was disabled when "Cancel" had been clicked, so re-enable
            // it now.
            m_sharedData->m_notifications |= wxSPDD_ENABLE_SKIP;

            // Also re-enable "Cancel" itself
            if ( HasPDFlag(wxPD_CAN_ABORT) )
                m_sharedData->m_notifications |= wxSPDD_ENABLE_ABORT;

            hwnd = m_sharedData->m_hwnd;
        } // Unlock m_cs, we can't call any function operating on a dialog with
          // it locked as it can result in a deadlock if the dialog callback is
          // called by Windows.

        // After resuming we need to bring the window on top of the Z-order as
        // it could be hidden by another window shown from the main thread,
        // e.g. a confirmation dialog asking whether the user really wants to
        // abort.
        //
        // Notice that this must be done from the main thread as it owns the
        // currently active window and attempts to do this from the task dialog
        // thread would simply fail.
        ::BringWindowToTop(hwnd);
    }
#endif // wxHAS_MSW_TASKDIALOG
}

WXWidget wxProgressDialog::GetHandle() const
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
    {
        wxCriticalSectionLocker locker(m_sharedData->m_cs);
        return m_sharedData->m_hwnd;
    }
#endif // wxHAS_MSW_TASKDIALOG

    return wxGenericProgressDialog::GetHandle();
}

int wxProgressDialog::GetValue() const
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
    {
        wxCriticalSectionLocker locker(m_sharedData->m_cs);
        return m_sharedData->m_value;
    }
#endif // wxHAS_MSW_TASKDIALOG

    return wxGenericProgressDialog::GetValue();
}

wxString wxProgressDialog::GetMessage() const
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
        return m_message;
#endif // wxHAS_MSW_TASKDIALOG

    return wxGenericProgressDialog::GetMessage();
}

void wxProgressDialog::SetRange(int maximum)
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
    {
        SetMaximum(maximum);

        wxCriticalSectionLocker locker(m_sharedData->m_cs);

        m_sharedData->m_range = maximum;
        m_sharedData->m_notifications |= wxSPDD_RANGE_CHANGED;

        return;
    }
#endif // wxHAS_MSW_TASKDIALOG

    wxGenericProgressDialog::SetRange( maximum );
}

bool wxProgressDialog::WasSkipped() const
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
    {
        if ( !m_sharedData )
        {
            // Couldn't be skipped before being shown.
            return false;
        }

        wxCriticalSectionLocker locker(m_sharedData->m_cs);
        return m_sharedData->m_skipped;
    }
#endif // wxHAS_MSW_TASKDIALOG

    return wxGenericProgressDialog::WasSkipped();
}

bool wxProgressDialog::WasCancelled() const
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
    {
        wxCriticalSectionLocker locker(m_sharedData->m_cs);
        return m_sharedData->m_state == Canceled;
    }
#endif // wxHAS_MSW_TASKDIALOG

    return wxGenericProgressDialog::WasCancelled();
}

void wxProgressDialog::SetTitle(const wxString& title)
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
    {
        m_title = title;

        if ( m_sharedData )
        {
            wxCriticalSectionLocker locker(m_sharedData->m_cs);
            m_sharedData->m_title = title;
            m_sharedData->m_notifications |= wxSPDD_TITLE_CHANGED;
        }
    }
#endif // wxHAS_MSW_TASKDIALOG

    wxGenericProgressDialog::SetTitle(title);
}

wxString wxProgressDialog::GetTitle() const
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
        return m_title;
#endif // wxHAS_MSW_TASKDIALOG

    return wxGenericProgressDialog::GetTitle();
}

void wxProgressDialog::SetIcons(const wxIconBundle& icons)
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
    {
        m_icons = icons; // We can't just call to parent's SetIcons()
                         // (wxGenericProgressDialog::SetIcons == wxTopLevelWindowMSW::SetIcons)
                         // because it does too many things.
        wxIcon iconSmall;
        wxIcon iconBig;
        if (!icons.IsEmpty())
        {
            const wxSize sizeSmall(wxGetSystemMetrics(SM_CXSMICON, this), wxGetSystemMetrics(SM_CYSMICON, this));
            iconSmall = icons.GetIcon(sizeSmall, wxIconBundle::FALLBACK_NEAREST_LARGER);

            const wxSize sizeBig(wxGetSystemMetrics(SM_CXICON, this), wxGetSystemMetrics(SM_CYICON, this));
            iconBig = icons.GetIcon(sizeBig, wxIconBundle::FALLBACK_NEAREST_LARGER);
        }

        if (m_sharedData)
        {
            wxCriticalSectionLocker locker(m_sharedData->m_cs);
            m_sharedData->m_iconSmall = iconSmall;
            m_sharedData->m_iconBig = iconBig;
            m_sharedData->m_notifications |= wxSPDD_ICON_CHANGED;
        }

        return;
    }
#endif // wxHAS_MSW_TASKDIALOG

    wxGenericProgressDialog::SetIcons(icons);
}

void wxProgressDialog::DoMoveWindow(int x, int y, int width, int height)
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
    {
        if ( m_sharedData )
        {
            wxCriticalSectionLocker locker(m_sharedData->m_cs);
            m_sharedData->m_winPosition = wxPoint(x, y);
            m_sharedData->m_notifications |= wxSPDD_WINDOW_MOVED;
        }

        return;
    }
#endif // wxHAS_MSW_TASKDIALOG

    wxGenericProgressDialog::DoMoveWindow(x, y, width, height);
}

wxRect wxProgressDialog::GetTaskDialogRect() const
{
    wxRect r;

#ifdef wxHAS_MSW_TASKDIALOG
    if ( m_sharedData )
    {
        wxCriticalSectionLocker locker(m_sharedData->m_cs);
        r = wxRectFromRECT(wxGetWindowRect(m_sharedData->m_hwnd));
    }
#else // !wxHAS_MSW_TASKDIALOG
    wxFAIL_MSG( "unreachable" );
#endif // wxHAS_MSW_TASKDIALOG/!wxHAS_MSW_TASKDIALOG

    return r;
}

void wxProgressDialog::DoGetPosition(int *x, int *y) const
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
    {
        const wxRect r = GetTaskDialogRect();
        if (x)
            *x = r.x;
        if (y)
            *y = r.y;

        return;
    }
#endif // wxHAS_MSW_TASKDIALOG

    wxGenericProgressDialog::DoGetPosition(x, y);
}

void wxProgressDialog::DoGetSize(int *width, int *height) const
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
    {
        const wxRect r = GetTaskDialogRect();
        if ( width )
            *width = r.width;
        if ( height )
            *height = r.height;

        return;
    }
#endif // wxHAS_MSW_TASKDIALOG

    wxGenericProgressDialog::DoGetSize(width, height);
}

void wxProgressDialog::Fit()
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
    {
        wxCriticalSectionLocker locker(m_sharedData->m_cs);

        // Force the task dialog to use this message to adjust it layout.
        m_sharedData->m_msgChangeElementText = TDM_SET_ELEMENT_TEXT;

        // Don't change the message, but pretend that it did change.
        m_sharedData->m_notifications |= wxSPDD_MESSAGE_CHANGED;
    }
#endif // wxHAS_MSW_TASKDIALOG

    wxGenericProgressDialog::Fit();
}

bool wxProgressDialog::Show(bool show)
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( HasNativeTaskDialog() )
    {
        // The dialog can't be hidden at all and showing it again after it had
        // been shown before doesn't do anything.
        if ( !show || m_taskDialogRunner )
            return false;

        // We're showing the dialog for the first time, create the thread that
        // will manage it.
        m_taskDialogRunner = new wxProgressDialogTaskRunner;
        m_sharedData = m_taskDialogRunner->GetSharedDataObject();

        // Initialize shared data.
        m_sharedData->m_title = m_title;
        m_sharedData->m_message = m_message;
        m_sharedData->m_range = m_maximum;
        m_sharedData->m_state = Uncancelable;
        m_sharedData->m_style = GetPDStyle();
        m_sharedData->m_parent = GetTopParent();

        if ( HasPDFlag(wxPD_CAN_ABORT) )
        {
            m_sharedData->m_state = Continue;
            m_sharedData->m_labelCancel = _("Cancel");
        }
        else // Dialog can't be cancelled.
        {
            // We still must have at least a single button in the dialog so
            // just don't call it "Cancel" in this case.
            m_sharedData->m_labelCancel = _("Close");
        }

        if ( HasPDFlag(wxPD_ELAPSED_TIME |
                         wxPD_ESTIMATED_TIME |
                            wxPD_REMAINING_TIME) )
        {
            // Set the expanded information field from the beginning to avoid
            // having to re-layout the dialog later when it changes.
            UpdateExpandedInformation(0);
        }

        // Do launch the thread.
        if ( m_taskDialogRunner->Create() != wxTHREAD_NO_ERROR )
        {
            wxLogError( "Unable to create thread!" );
            return false;
        }

        if ( m_taskDialogRunner->Run() != wxTHREAD_NO_ERROR )
        {
            wxLogError( "Unable to start thread!" );
            return false;
        }

        // Wait until the dialog is shown as the program may need some time
        // before it calls Update() and we want to show something to the user
        // in the meanwhile.
        while ( wxEventLoop::GetActive()->Dispatch() )
        {
            wxCriticalSectionLocker locker(m_sharedData->m_cs);
            if ( m_sharedData->m_hwnd )
                break;
        }

        // Do not show the underlying dialog.
        return false;
    }
#endif // wxHAS_MSW_TASKDIALOG

    return wxGenericProgressDialog::Show( show );
}

void wxProgressDialog::UpdateExpandedInformation(int value)
{
#ifdef wxHAS_MSW_TASKDIALOG
    unsigned long elapsedTime;
    unsigned long estimatedTime;
    unsigned long remainingTime;
    UpdateTimeEstimates(value, elapsedTime, estimatedTime, remainingTime);

    // The value of 0 is special, we can't estimate anything before we have at
    // least one update, so leave the times dependent on it indeterminate.
    //
    // This value is also used by Pulse(), as in the indeterminate mode we can
    // never estimate anything.
    if ( !value )
    {
        estimatedTime =
        remainingTime = static_cast<unsigned long>(-1);
    }

    wxString expandedInformation;

    // Calculate the three different timing values.
    if ( HasPDFlag(wxPD_ELAPSED_TIME) )
    {
        expandedInformation << GetElapsedLabel()
                            << " "
                            << GetFormattedTime(elapsedTime);
    }

    if ( HasPDFlag(wxPD_ESTIMATED_TIME) )
    {
        if ( !expandedInformation.empty() )
            expandedInformation += "\n";

        expandedInformation << GetEstimatedLabel()
                            << " "
                            << GetFormattedTime(estimatedTime);
    }

    if ( HasPDFlag(wxPD_REMAINING_TIME) )
    {
        if ( !expandedInformation.empty() )
            expandedInformation += "\n";

        expandedInformation << GetRemainingLabel()
                            << " "
                            << GetFormattedTime(remainingTime);
    }

    // Update with new timing information.
    if ( expandedInformation != m_sharedData->m_expandedInformation )
    {
        m_sharedData->m_expandedInformation = expandedInformation;
        m_sharedData->m_notifications |= wxSPDD_EXPINFO_CHANGED;
    }
#else // !wxHAS_MSW_TASKDIALOG
    wxUnusedVar(value);
#endif // wxHAS_MSW_TASKDIALOG/!wxHAS_MSW_TASKDIALOG
}

// ----------------------------------------------------------------------------
// wxProgressDialogTaskRunner and related methods
// ----------------------------------------------------------------------------

#ifdef wxHAS_MSW_TASKDIALOG

void* wxProgressDialogTaskRunner::Entry()
{
    WinStruct<TASKDIALOGCONFIG> tdc;
    wxMSWTaskDialogConfig wxTdc;

    {
        wxCriticalSectionLocker locker(m_sharedData.m_cs);

        // If we have a parent, we must use it to have correct Z-order and
        // icon, even if this comes at the price of attaching this thread input
        // to the thread that created the parent window, i.e. the main thread.
        wxTdc.parent = m_sharedData.m_parent;
        wxTdc.caption = m_sharedData.m_title.wx_str();

        // Split the message into the title and main body text in the same way
        // as it's done later in PerformNotificationUpdates() when the message
        // is changed by Update() or Pulse().
        m_sharedData.SplitMessageIntoTitleAndBody
                     (
                        wxTdc.message,
                        wxTdc.extendedMessage
                     );

        // MSWCommonTaskDialogInit() will add an IDCANCEL button but we need to
        // give it the correct label.
        wxTdc.btnOKLabel = m_sharedData.m_labelCancel;
        wxTdc.useCustomLabels = true;

        wxTdc.MSWCommonTaskDialogInit( tdc );
        tdc.pfCallback = TaskDialogCallbackProc;
        tdc.lpCallbackData = (LONG_PTR) &m_sharedData;

        if ( m_sharedData.m_style & wxPD_CAN_SKIP )
            wxTdc.AddTaskDialogButton( tdc, Id_SkipBtn, 0, _("Skip") );

        tdc.dwFlags |= TDF_CALLBACK_TIMER | TDF_SHOW_PROGRESS_BAR;

        if ( !m_sharedData.m_expandedInformation.empty() )
        {
            tdc.pszExpandedInformation =
                m_sharedData.m_expandedInformation.t_str();

            // If we have elapsed/estimated/... times to show, show them from
            // the beginning for consistency with the generic version and also
            // because showing them later may be very sluggish if the main
            // thread doesn't update the dialog sufficiently frequently, while
            // hiding them still works reasonably well.
            tdc.dwFlags |= TDF_EXPANDED_BY_DEFAULT;
        }
    }

    TaskDialogIndirect_t taskDialogIndirect = GetTaskDialogIndirectFunc();
    if ( !taskDialogIndirect )
        return nullptr;

    int msAns;
    HRESULT hr = taskDialogIndirect(&tdc, &msAns, nullptr, nullptr);
    if ( FAILED(hr) )
        wxLogApiError( "TaskDialogIndirect", hr );

    // If the main thread is waiting for us to exit inside the event loop in
    // Update(), wake it up so that it checks our status again.
    wxWakeUpIdle();

    return nullptr;
}

// static
HRESULT CALLBACK
wxProgressDialogTaskRunner::TaskDialogCallbackProc
                            (
                                HWND hwnd,
                                UINT uNotification,
                                WPARAM wParam,
                                LPARAM WXUNUSED(lParam),
                                LONG_PTR dwRefData
                            )
{
    bool endDialog = false;

    // Block for shared data critical section.
    {
    wxProgressDialogSharedData * const sharedData =
        (wxProgressDialogSharedData *) dwRefData;

    wxCriticalSectionLocker locker(sharedData->m_cs);

    switch ( uNotification )
    {
        case TDN_CREATED:
            // Store the HWND for the main thread use.
            sharedData->m_hwnd = hwnd;

            // The main thread is sitting in an event dispatching loop waiting
            // for this dialog to be shown, so make sure it does get an event.
            wxWakeUpIdle();

            // Set the maximum value and disable Close button.
            ::SendMessage( hwnd,
                           TDM_SET_PROGRESS_BAR_RANGE,
                           0,
                           MAKELPARAM(0, sharedData->m_range) );

            // If we can't be aborted, the "Close" button will only be enabled
            // when the progress ends (and not even then with wxPD_AUTO_HIDE).
            if ( !(sharedData->m_style & wxPD_CAN_ABORT) )
                EnableCloseButtons(hwnd, false);
            break;

        case TDN_BUTTON_CLICKED:
            switch ( wParam )
            {
                case Id_SkipBtn:
                    ::SendMessage(hwnd, TDM_ENABLE_BUTTON, Id_SkipBtn, FALSE);
                    sharedData->m_skipped = true;
                    return S_FALSE;

                case IDCANCEL:
                    if ( sharedData->m_state == wxProgressDialog::Finished )
                    {
                        // If the main thread is waiting for us, tell it that
                        // we're gone (and if it doesn't wait, it's harmless).
                        sharedData->m_state = wxProgressDialog::Dismissed;

                        // Let Windows close the dialog.
                        return S_OK;
                    }

                    // Close button on the window triggers an IDCANCEL press,
                    // don't allow it when it should only be possible to close
                    // a finished dialog.
                    if ( sharedData->m_style & wxPD_CAN_ABORT )
                    {
                        switch ( sharedData->m_state )
                        {
                            case wxProgressDialog::Canceled:
                                // It can happen that we receive a second
                                // cancel request before we had time to process
                                // the first one, in which case simply do
                                // nothing for the subsequent one.
                                break;

                            case wxProgressDialog::Continue:
                                ::SendMessage(hwnd, TDM_ENABLE_BUTTON, Id_SkipBtn, FALSE);
                                EnableCloseButtons(hwnd, false);

                                sharedData->m_timeStop = wxGetCurrentTime();
                                sharedData->m_state = wxProgressDialog::Canceled;
                                break;

                            // States which shouldn't be possible here:

                            // We shouldn't have an (enabled) cancel button at
                            // all then.
                            case wxProgressDialog::Uncancelable:

                            // This one was already dealt with above.
                            case wxProgressDialog::Finished:

                            // Normally it shouldn't be possible to get any
                            // notifications after switching to this state.
                            case wxProgressDialog::Dismissed:
                                wxFAIL_MSG( "unreachable" );
                                break;
                        }
                    }

                    return S_FALSE;
            }
            break;

        case TDN_TIMER:
            // Don't perform updates if nothing needs to be done.
            if ( sharedData->m_notifications )
                PerformNotificationUpdates(hwnd, sharedData);

            /*
                Decide whether we should end the dialog. This is done if either
                the dialog object itself was destroyed or if the progress
                finished and we were configured to hide automatically without
                waiting for the user to dismiss us.

                Notice that we do not close the dialog if it was cancelled
                because it's up to the user code in the main thread to decide
                whether it really wants to cancel the dialog.
             */
            if ( (sharedData->m_notifications & wxSPDD_DESTROYED) ||
                    (sharedData->m_state == wxProgressDialog::Finished &&
                        sharedData->m_style & wxPD_AUTO_HIDE) )
            {
                // Don't call EndDialog() from here as it could deadlock
                // because we are inside the shared data critical section, do
                // it below after leaving it instead.
                endDialog = true;
            }

            sharedData->m_notifications = 0;

            if ( endDialog )
                break;

            return S_FALSE;
    }

    } // Leave shared data critical section.

    if ( endDialog )
    {
        ::EndDialog( hwnd, IDCLOSE );
        return S_FALSE;
    }

    // Return anything.
    return S_OK;
}

#endif // wxHAS_MSW_TASKDIALOG

#endif // wxUSE_PROGRESSDLG && wxUSE_THREADS
