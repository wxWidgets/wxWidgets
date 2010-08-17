/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/progdlg.cpp
// Purpose:     wxProgressDialog
// Author:      Rickard Westerlund
// Created:     2010-07-22
// RCS-ID:      $Id$
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_PROGRESSDLG && wxUSE_THREADS

#include "wx/scopedptr.h"
#include "wx/dynlib.h"
#include "wx/msw/private/msgdlg.h"
#include "wx/progdlg.h"

#ifdef wxHAS_MSW_TASKDIALOG
using wxMSWMessageDialog::wxMSWTaskDialogConfig;
#endif

// ============================================================================
// Definitions
// ============================================================================

// Notification values of SharedProgressDialogData::m_notifications
#define wxSPDD_VALUE_CHANGED     0x001
#define wxSPDD_RANGE_CHANGED     0x002
#define wxSPDD_PBMARQUEE_CHANGED 0x004
#define wxSPDD_TITLE_CHANGED     0x008
#define wxSPDD_MESSAGE_CHANGED   0x010
#define wxSPDD_EXPINFO_CHANGED   0x020
#define wxSPDD_ENABLE_SKIP       0x040
#define wxSPDD_ENABLE_ABORT      0x080
#define wxSPDD_DISABLE_SKIP      0x100
#define wxSPDD_DISABLE_ABORT     0x200
#define wxSPDD_FINISHED          0x400
#define wxSPDD_DESTROYED         0x800

static const int wxID_SKIP = wxID_HIGHEST + 1;

// ============================================================================
// Helper classes
// ============================================================================

// Class used to share data between the main thread and the task dialog runner.
class SharedProgressDialogData
{
public:
    SharedProgressDialogData() : m_value( 0 ),
                                 m_progressBarMarquee( false ),
                                 m_skipped( false ),
                                 m_notifications( 0 )
        { }

    wxCriticalSection m_cs;
    long m_style; // wxProgressDialog style
    int m_value;
    int m_range;
    bool m_progressBarMarquee;
    wxString m_title;
    wxString m_message;
    wxString m_expandedInformation;
    wxString m_labelCancel; // Privately used by callback.
    bool m_skipped;
    wxGenericProgressDialog::ProgressDialogState m_state;
    unsigned long m_timeStop;

    // Bit field that indicates fields that have been modified by the
    // main thread so the task dialog runner knows what to update.
    int m_notifications;
};

// Runner thread that takes care of displaying and updating the
// task dialog.
class TaskDialogRunner : public wxThread
{
public:
    TaskDialogRunner(wxWindow* parent) : wxThread( wxTHREAD_JOINABLE ),
                                         m_parent( parent )
        { }

    SharedProgressDialogData* GetSharedDataObject()
        { return &m_sharedData; }

private:
    wxWindow* m_parent;
    SharedProgressDialogData m_sharedData;

    virtual void* Entry();

    static HRESULT CALLBACK TaskDialogCallbackProc(HWND hwnd,
                                                   UINT uNotification,
                                                   WPARAM wParam,
                                                   LPARAM lParam,
                                                   LONG_PTR dwRefData);
};

// ============================================================================
// Helper methods
// ============================================================================

namespace
{
    bool UsesCloseButtonOnly(long style)
    {
        return !((style & wxPD_CAN_ABORT) || (style & wxPD_AUTO_HIDE));
    }

#ifdef wxHAS_MSW_TASKDIALOG
    BOOL CALLBACK DisplayCloseButton(HWND hwnd, LPARAM lParam)
    {
        SharedProgressDialogData *sharedData =
            (SharedProgressDialogData *) lParam;

        if ( wxGetWindowText( hwnd ) == sharedData->m_labelCancel )
        {
            sharedData->m_labelCancel = _("Close");
            SendMessage( hwnd, WM_SETTEXT, 0,
                         (LPARAM) sharedData->m_labelCancel.wx_str() );

            return FALSE;
        }

        return TRUE;
    }

    void PerformNotificationUpdates(HWND hwnd,
                                    SharedProgressDialogData *sharedData)
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
                ::SetWindowText( hwnd, sharedData->m_title.wx_str() );

            if ( sharedData->m_notifications & wxSPDD_MESSAGE_CHANGED )
            {
                ::SendMessage( hwnd,
                               TDM_SET_ELEMENT_TEXT,
                               TDE_MAIN_INSTRUCTION,
                               (LPARAM) sharedData->m_message.wx_str() );
            }

            if ( sharedData->m_notifications & wxSPDD_EXPINFO_CHANGED )
            {
                wxString &expandedInformation =
                    sharedData->m_expandedInformation;
                if ( !expandedInformation.empty() )
                {
                    ::SendMessage( hwnd,
                                   TDM_SET_ELEMENT_TEXT,
                                   TDE_EXPANDED_INFORMATION,
                                   (LPARAM) expandedInformation.wx_str() );
                }
            }

            if ( sharedData->m_notifications & wxSPDD_ENABLE_SKIP )
                ::SendMessage( hwnd, TDM_ENABLE_BUTTON, wxID_SKIP, 1 );

            if ( sharedData->m_notifications & wxSPDD_ENABLE_ABORT )
                ::SendMessage( hwnd, TDM_ENABLE_BUTTON, IDCANCEL, 1 );

            if ( sharedData->m_notifications & wxSPDD_DISABLE_SKIP )
                ::SendMessage( hwnd, TDM_ENABLE_BUTTON, wxID_SKIP, 0 );

            if ( sharedData->m_notifications & wxSPDD_DISABLE_ABORT )
                ::SendMessage( hwnd, TDM_ENABLE_BUTTON, IDCANCEL, 0 );

            // Is the progress finished?
            if ( sharedData->m_notifications & wxSPDD_FINISHED )
            {
                sharedData->m_state = wxGenericProgressDialog::Finished;

                if ( !(sharedData->m_style & wxPD_AUTO_HIDE) )
                {
                    // Change Cancel into Close and activate the button.
                    ::SendMessage( hwnd, TDM_ENABLE_BUTTON, wxID_SKIP, 0 );
                    ::SendMessage( hwnd, TDM_ENABLE_BUTTON, IDCANCEL, 1 );
                    ::EnumChildWindows( hwnd, DisplayCloseButton,
                                        (LPARAM) sharedData );
                }
            }
    }
#endif
};

// ============================================================================
// wxProgressDialog implementation
// ============================================================================

wxProgressDialog::wxProgressDialog( const wxString& title,
                                    const wxString& message,
                                    int maximum,
                                    wxWindow *parent,
                                    int style )
    : wxGenericProgressDialog(),
      m_labelElapsed( _("Elapsed time:") ),
      m_labelEstimated( _("Estimated time:") ),
      m_labelRemaining( _("Remaining time:") ),
      m_taskDialogRunner( NULL ),
      m_sharedData( NULL ),
      m_title( title ),
      m_message( message ),
      m_value( 0 ),
      m_skipped( false )
{
    Create( title, message, maximum, parent, style );
}

wxProgressDialog::~wxProgressDialog()
{
#ifdef wxHAS_MSW_TASKDIALOG
    if ( m_sharedData != NULL )
    {
        wxCriticalSectionLocker locker(m_sharedData->m_cs);
        m_sharedData->m_notifications |= wxSPDD_DESTROYED;
    }

    if ( m_taskDialogRunner.get() != NULL )
        m_taskDialogRunner->Wait();

    if ( !HasFlag(wxPD_APP_MODAL) )
        GetParent()->Enable();

    if ( GetParent() != NULL )
        GetParent()->Raise();
#endif
}

bool wxProgressDialog::Update(int value, const wxString& newmsg, bool *skip)
{
    if ( !HasNativeProgressDialog() )
        return wxGenericProgressDialog::Update( value, newmsg, skip );

    wxCriticalSectionLocker locker(m_sharedData->m_cs);

    // Do nothing in canceled state.
    if ( !DoBeforeUpdate( skip ) )
        return false;

    value /= m_factor;

    wxASSERT_MSG( value <= m_maximum, wxT("invalid progress value") );

    m_value = value;
    m_sharedData->m_value = value;
    m_sharedData->m_notifications |= wxSPDD_VALUE_CHANGED;

    if ( !newmsg.empty() )
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

    UpdateExpandedInformation( m_value );

    // Has the the progress bar finished?
    if ( value == m_maximum )
    {
        if ( m_state == Finished )
            return true;

        m_state = Finished;
        m_sharedData->m_state = Finished;
        m_sharedData->m_notifications |= wxSPDD_FINISHED;
        if( !HasFlag(wxPD_AUTO_HIDE) && newmsg.empty() )
        {
            // Provide the finishing message if the application didn't.
            m_message = _("Done.");
            m_sharedData->m_message = m_message;
            m_sharedData->m_notifications |= wxSPDD_MESSAGE_CHANGED;
        }
    }

    return m_sharedData->m_state != Canceled;
}

bool wxProgressDialog::Pulse(const wxString& newmsg, bool *skip)
{
    if ( !HasNativeProgressDialog() )
        return wxGenericProgressDialog::Pulse( newmsg, skip );

    wxCriticalSectionLocker locker(m_sharedData->m_cs);

    // Do nothing in canceled state.
    if ( !DoBeforeUpdate( skip ) )
        return false;

    if ( !m_sharedData->m_progressBarMarquee )
    {
        m_sharedData->m_progressBarMarquee = true;
        m_sharedData->m_notifications |= wxSPDD_PBMARQUEE_CHANGED;
    }

    if ( !newmsg.empty() )
    {
        m_message = newmsg;
        m_sharedData->m_message = newmsg;
        m_sharedData->m_notifications |= wxSPDD_MESSAGE_CHANGED;
    }

    UpdateExpandedInformation( m_value );

    return m_sharedData->m_state != Canceled;
}

bool wxProgressDialog::DoBeforeUpdate(bool *skip)
{
    if ( m_sharedData->m_skipped  )
    {
        m_skipped = true;

        if ( skip && !*skip )
        {
            *skip = true;
            m_sharedData->m_skipped = false;
            m_sharedData->m_notifications |= wxSPDD_ENABLE_SKIP;
        }
    }
    else
    {
        m_skipped = false;
    }

    if ( m_sharedData->m_state == Canceled )
        m_timeStop = m_sharedData->m_timeStop;

    return m_sharedData->m_state != Canceled;
}

void wxProgressDialog::Resume()
{
    wxGenericProgressDialog::Resume();

    if ( !HasNativeProgressDialog() )      
        return;

    wxCriticalSectionLocker locker(m_sharedData->m_cs);
    m_sharedData->m_state = m_state;
    m_sharedData->m_notifications |= wxSPDD_ENABLE_SKIP;

    if ( !UsesCloseButtonOnly(m_windowStyle) )
        m_sharedData->m_notifications |= wxSPDD_ENABLE_ABORT;
}

int wxProgressDialog::GetValue() const
{
    if ( !HasNativeProgressDialog() )
        return wxGenericProgressDialog::GetValue();

    return m_value;
}

wxString wxProgressDialog::GetMessage() const
{
    if ( !HasNativeProgressDialog() )
        return wxGenericProgressDialog::GetMessage();

    return m_message;
}

void wxProgressDialog::SetRange(int maximum)
{
    wxGenericProgressDialog::SetRange( maximum );

    if ( HasNativeProgressDialog() )
    {
        wxCriticalSectionLocker locker(m_sharedData->m_cs);

        m_sharedData->m_range = maximum;
        m_sharedData->m_notifications |= wxSPDD_RANGE_CHANGED;
    }
}

bool wxProgressDialog::WasSkipped() const
{
    if ( !HasNativeProgressDialog() )
        return wxGenericProgressDialog::WasSkipped();

    return m_skipped;
}

bool wxProgressDialog::WasCancelled() const
{
    if ( !HasNativeProgressDialog() )
        return wxGenericProgressDialog::WasCancelled();

    wxCriticalSectionLocker locker(m_sharedData->m_cs);
    return m_sharedData->m_state == Canceled;
}

void wxProgressDialog::SetTitle(const wxString &title)
{
    // Ignore possible calls during construction.
    if ( m_sharedData != NULL)
    {
        wxCriticalSectionLocker locker(m_sharedData->m_cs);
        m_sharedData->m_title = title;
        m_sharedData->m_notifications = wxSPDD_TITLE_CHANGED;
    }
}

bool wxProgressDialog::Show(bool show)
{
    if ( !HasNativeProgressDialog() )
        return wxGenericProgressDialog::Show( show );

#ifdef wxHAS_MSW_TASKDIALOG
    // Once shown it cannot be hidden.
    if ( !show || m_taskDialogRunner != NULL )
        return false;

    m_taskDialogRunner.reset(new TaskDialogRunner( GetParent() ));
    m_sharedData = m_taskDialogRunner->GetSharedDataObject();

    // Initialize shared data.
    m_sharedData->m_title = m_title;
    m_sharedData->m_message = m_message;
    m_sharedData->m_range = m_maximum;
    m_sharedData->m_state = Uncancelable;
    m_sharedData->m_style = m_windowStyle;

    if ( HasFlag(wxPD_CAN_ABORT) )
    {
        m_sharedData->m_state = Continue;
        m_sharedData->m_labelCancel = _("Cancel");
    }
    else if ( !HasFlag(wxPD_AUTO_HIDE) )
    {
        m_sharedData->m_labelCancel = _("Close");
    }

    if ( m_windowStyle & (wxPD_ELAPSED_TIME
                          | wxPD_ESTIMATED_TIME
                          | wxPD_REMAINING_TIME) )
    {
        // Use a non-empty string just to have the collapsible pane shown.
        m_sharedData->m_expandedInformation = " ";
    }

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

    if ( !HasFlag(wxPD_APP_MODAL) )
            GetParent()->Disable();
#endif

    // Do not show the underlying dialog.
    return false;
}

bool wxProgressDialog::HasNativeProgressDialog() const
{
    // For a native implementation task dialogs are required, which
    // also require at least one button to be present so the flags needs
    // to be check as well if this is the case.
    return wxMSWMessageDialog::HasNativeTaskDialog()
           && ((m_windowStyle & (wxPD_CAN_SKIP | wxPD_CAN_ABORT))
               || !(m_windowStyle & wxPD_AUTO_HIDE));
}

void wxProgressDialog::UpdateExpandedInformation(int value)
{
    unsigned long estimatedTime;
    unsigned long elapsedTime;
    unsigned long remainingTime;
    UpdateTimeEstimates(value, elapsedTime, estimatedTime, remainingTime);

    wxString oldExpandedInformation = m_expandedInformation;
    m_expandedInformation = "";

    // Calculate the three different timing values.
    if ( m_windowStyle & wxPD_ELAPSED_TIME )
    {
        m_expandedInformation +=
            m_labelElapsed + " " + GetFormattedTime(elapsedTime);
    }

    if ( m_windowStyle & wxPD_ESTIMATED_TIME )
    {
        if ( !m_expandedInformation.empty() )
            m_expandedInformation += "\n";

        m_expandedInformation += m_labelEstimated + " ";
        if ( m_sharedData->m_progressBarMarquee )
            m_expandedInformation += GetFormattedTime(-1);
        else
            m_expandedInformation += GetFormattedTime(estimatedTime);
    }

    if ( m_windowStyle & wxPD_REMAINING_TIME )
    {
        if ( !m_expandedInformation.empty() )
            m_expandedInformation += "\n";

        m_expandedInformation += m_labelRemaining + " ";
        if ( m_sharedData->m_progressBarMarquee )
            m_expandedInformation += GetFormattedTime(-1);
        else
            m_expandedInformation += GetFormattedTime(remainingTime);
    }

    // Update with new timing information.
    if ( oldExpandedInformation != m_expandedInformation )
    {
        m_sharedData->m_expandedInformation = m_expandedInformation;
        m_sharedData->m_notifications |= wxSPDD_EXPINFO_CHANGED;
    }
}

// ----------------------------------------------------------------------------
// TaskDialogRunner and related methods
// ----------------------------------------------------------------------------

void* TaskDialogRunner::Entry()
{
#ifdef wxHAS_MSW_TASKDIALOG
    wxScopedPtr<wxWindowDisabler> windowDisabler( NULL );
    WinStruct<TASKDIALOGCONFIG> tdc;
    wxMSWMessageDialog::wxMSWTaskDialogConfig wxTdc;

    {
        wxCriticalSectionLocker locker(m_sharedData.m_cs);

        if ( m_sharedData.m_style & wxPD_APP_MODAL )
            windowDisabler.reset(new wxWindowDisabler());

        wxTdc.parent = NULL;
        wxTdc.caption = m_sharedData.m_title.wx_str();
        wxTdc.message = m_sharedData.m_message.wx_str();

        wxTdc.MSWCommonTaskDialogInit( tdc );
        tdc.pfCallback = TaskDialogCallbackProc;
        tdc.lpCallbackData = (LONG_PTR) &m_sharedData;

        // Undo some of the effects of MSWCommonTaskDialogInit().
        tdc.dwFlags &= ~TDF_EXPAND_FOOTER_AREA; // Expand in content area.
        tdc.dwCommonButtons = 0; // Dont use common buttons.

        wxTdc.useCustomLabels = true;

        if ( m_sharedData.m_style & wxPD_CAN_SKIP )
            wxTdc.AddTaskDialogButton( tdc, wxID_SKIP, 0, _("Skip") );

        // Use a Cancel button when requested or use a Close button when
        // the dialog does not automatically hide.
        if ( (m_sharedData.m_style & wxPD_CAN_ABORT)
             || !(m_sharedData.m_style & wxPD_AUTO_HIDE) )
        {
            wxTdc.AddTaskDialogButton( tdc, IDCANCEL, 0,
                                       m_sharedData.m_labelCancel );
        }

        tdc.dwFlags |= TDF_CALLBACK_TIMER
                       | TDF_SHOW_PROGRESS_BAR
                       | TDF_SHOW_MARQUEE_PROGRESS_BAR;

        if ( !m_sharedData.m_expandedInformation.empty() )
        {
            tdc.pszExpandedInformation =
                m_sharedData.m_expandedInformation.wx_str();
        }
    }

    wxLoadedDLL dllComCtl32( "comctl32.dll" );
    taskDialogIndirect_t taskDialogIndirect =
        (taskDialogIndirect_t) dllComCtl32.GetSymbol( "TaskDialogIndirect" );

    int msAns;
    HRESULT hr = taskDialogIndirect(&tdc, &msAns, NULL, NULL);
    if ( FAILED(hr) )
        wxLogApiError( "TaskDialogIndirect", hr );
#endif

    return NULL;
}

// static
HRESULT CALLBACK
TaskDialogRunner::TaskDialogCallbackProc
                                        (
#ifdef wxHAS_MSW_TASKDIALOG
                                          HWND hwnd,
                                          UINT uNotification,
                                          WPARAM wParam,
                                          LPARAM WXUNUSED(lParam),
                                          LONG_PTR dwRefData
#else
                                          HWND WXUNUSED(hwnd),
                                          UINT WXUNUSED(uNotification),
                                          WPARAM WXUNUSED(wParam),
                                          LPARAM WXUNUSED(lParam),
                                          LONG_PTR WXUNUSED(dwRefData)
#endif
                                         )
{
#ifdef wxHAS_MSW_TASKDIALOG
    SharedProgressDialogData *sharedData =
        (SharedProgressDialogData *) dwRefData;

    wxCriticalSectionLocker locker(sharedData->m_cs);

    switch ( uNotification )
    {
        case TDN_CREATED:
            // Set the maximum value and disable Close button.
            ::SendMessage( hwnd,
                           TDM_SET_PROGRESS_BAR_RANGE,
                           0,
                           MAKELPARAM(0, sharedData->m_range) );

            if ( UsesCloseButtonOnly(sharedData->m_style) )
                ::SendMessage( hwnd, TDM_ENABLE_BUTTON, IDCANCEL, 0 );
            break;

        case TDN_BUTTON_CLICKED:
            switch ( wParam )
            {
                case wxID_SKIP:
                    ::SendMessage( hwnd,
                                   TDM_ENABLE_BUTTON,
                                   wxID_SKIP,
                                   0 );
                    sharedData->m_skipped = true;
                    return TRUE;

                case IDCANCEL:
                    if ( sharedData->m_state
                            == wxGenericProgressDialog::Finished )
                    {
                        return FALSE;
                    }
                    else
                    {
                        // Close button on the window triggers an IDCANCEL
                        // press, don't allow it when it should only be
                        // possible to close a finished dialog.
                        if ( !UsesCloseButtonOnly(sharedData->m_style) )
                        {
                            wxCHECK_MSG(sharedData->m_state
                                            ==
                                            wxGenericProgressDialog::Continue,
                                        TRUE,
                                        "Dialog not in a cancelable state!");

                            ::SendMessage( hwnd,
                                           TDM_ENABLE_BUTTON,
                                           IDCANCEL,
                                           0 );

                            sharedData->m_timeStop = wxGetCurrentTime();
                            sharedData->m_state =
                                wxGenericProgressDialog::Canceled;
                        }

                        return TRUE;
                    }
            }
            break;

        case TDN_TIMER:
            PerformNotificationUpdates(hwnd, sharedData);

            // End dialog in three different cases:
            // 1. Progress finished and dialog should automatically hide.
            // 2. The wxProgressDialog object was destructed and should
            //    automatically hide.
            // 3. The dialog was canceled and wxProgressDialog object
            //    was destroyed.
            bool isCanceled =
                sharedData->m_state == wxGenericProgressDialog::Canceled;
            bool isFinished =
                sharedData->m_state == wxGenericProgressDialog::Finished;
            bool wasDestroyed =
                (sharedData->m_notifications & wxSPDD_DESTROYED) != 0;
            bool shouldAutoHide = (sharedData->m_style & wxPD_AUTO_HIDE) != 0;

            if ( (shouldAutoHide && (isFinished || wasDestroyed))
                 || (wasDestroyed && isCanceled) )
            {
                ::EndDialog( hwnd, IDCLOSE );
            }

            sharedData->m_notifications = 0;

            return TRUE;
    }

#endif

    // Return anything.
    return 0;
}

#endif // wxUSE_PROGRESSDLG && wxUSE_THREADS
