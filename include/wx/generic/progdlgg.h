///////////////////////////////////////////////////////////////////////////////
// Name:        progdlgg.h
// Purpose:     wxGenericProgressDialog class
// Author:      Karsten Ballueder
// Modified by: Francesco Montorsi
// Created:     09.05.1999
// RCS-ID:      $Id$
// Copyright:   (c) Karsten Ballueder
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef __PROGDLGH_G__
#define __PROGDLGH_G__

#include "wx/dialog.h"

class WXDLLIMPEXP_FWD_CORE wxButton;
class WXDLLIMPEXP_FWD_CORE wxGauge;
class WXDLLIMPEXP_FWD_CORE wxStaticText;

/*
    Progress dialog which shows a moving progress bar.
    Taken from the Mahogany project.
*/
class WXDLLIMPEXP_CORE wxGenericProgressDialog : public wxDialog
{
public:
    wxGenericProgressDialog();
    wxGenericProgressDialog(const wxString& title, const wxString& message,
                            int maximum = 100,
                            wxWindow *parent = NULL,
                            int style = wxPD_APP_MODAL | wxPD_AUTO_HIDE);

    virtual ~wxGenericProgressDialog();

    virtual bool Update(int value, const wxString& newmsg = wxEmptyString, bool *skip = NULL);
    virtual bool Pulse(const wxString& newmsg = wxEmptyString, bool *skip = NULL);

    void Resume();

    int GetValue() const;
    int GetRange() const;
    wxString GetMessage() const;

    void SetRange(int maximum);

    // Return whether "Cancel" or "Skip" button was pressed, always return
    // false if the corresponding button is not shown.
    bool WasCancelled() const;
    bool WasSkipped() const;

    // Must provide overload to avoid hiding it (and warnings about it)
    virtual void Update() { wxDialog::Update(); }

    virtual bool Show( bool show = true );

    // This enum is an implementation detail and should not be used
    // by user code.
    enum ProgressDialogState
    {
        Uncancelable = -1,   // dialog can't be canceled
        Canceled,            // can be cancelled and, in fact, was
        Continue,            // can be cancelled but wasn't
        Finished             // finished, waiting to be removed from screen
    };

protected:
    // continue processing or not (return value for Update())
    ProgressDialogState m_state;

    // the maximum value
    int m_maximum;

#if defined(__WXMSW__ ) || defined(__WXPM__)
    // the factor we use to always keep the value in 16 bit range as the native
    // control only supports ranges from 0 to 65,535
    size_t m_factor;
#endif // __WXMSW__

    // time when the dialog was created
    unsigned long m_timeStart;
    // time when the dialog was closed or cancelled
    unsigned long m_timeStop;
    // time between the moment the dialog was closed/cancelled and resume
    unsigned long m_break;

    void Create(const wxString& title,
                const wxString& message,
                int maximum,
                wxWindow *parent,
                int style);

    // Updates estimated times from a given progress bar value and stores the
    // results in provided arguments.
    void UpdateTimeEstimates(int value,
                             unsigned long &elapsedTime,
                             unsigned long &estimatedTime,
                             unsigned long &remainingTime);

    // Converts seconds to HH:mm:ss format.
    static wxString GetFormattedTime(int time);

    // callback for optional abort button
    void OnCancel(wxCommandEvent&);

    // callback for optional skip button
    void OnSkip(wxCommandEvent&);

    // callback to disable "hard" window closing
    void OnClose(wxCloseEvent&);

    // must be called to reenable the other windows temporarily disabled while
    // the dialog was shown
    void ReenableOtherWindows();

private:
    // update the label to show the given time (in seconds)
    static void SetTimeLabel(unsigned long val, wxStaticText *label);

    // create the label with given text and another one to show the time nearby
    // as the next windows in the sizer, returns the created control
    wxStaticText *CreateLabel(const wxString& text, wxSizer *sizer);

    // updates the label message
    void UpdateMessage(const wxString &newmsg);

    // common part of Update() and Pulse(), returns true if not cancelled
    bool DoBeforeUpdate(bool *skip);

    // common part of Update() and Pulse()
    void DoAfterUpdate();

    // shortcuts for enabling buttons
    void EnableClose();
    void EnableSkip(bool enable = true);
    void EnableAbort(bool enable = true);
    void DisableSkip() { EnableSkip(false); }
    void DisableAbort() { EnableAbort(false); }

    // the widget displaying current status (may be NULL)
    wxGauge *m_gauge;
    // the message displayed
    wxStaticText *m_msg;
    // displayed elapsed, estimated, remaining time
    wxStaticText *m_elapsed,
                 *m_estimated,
                 *m_remaining;

    // parent top level window (may be NULL)
    wxWindow *m_parentTop;

    // skip some portion
    bool m_skip;

#if !defined(__SMARTPHONE__)
    // the abort and skip buttons (or NULL if none)
    wxButton *m_btnAbort;
    wxButton *m_btnSkip;
#endif

    // saves the time when elapsed time was updated so there is only one
    // update per second
    unsigned long m_last_timeupdate;
    // tells how often a change of the estimated time has to be confirmed
    // before it is actually displayed - this reduces the frequence of updates
    // of estimated and remaining time
    const int m_delay;
    // counts the confirmations
    int m_ctdelay;
    unsigned long m_display_estimated;

    bool m_hasAbortButton,
         m_hasSkipButton;

    // for wxPD_APP_MODAL case
    class WXDLLIMPEXP_FWD_CORE wxWindowDisabler *m_winDisabler;

    DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(wxGenericProgressDialog);
};

#endif // __PROGDLGH_G__
