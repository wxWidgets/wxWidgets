/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/progdlg.h
// Purpose:     wxProgressDialog
// Author:      Rickard Westerlund
// Created:     2010-07-22
// RCS-ID:      $Id$
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROGDLG_H_
#define _WX_PROGDLG_H_

class TaskDialogRunner;
class SharedProgressDialogData;

class WXDLLIMPEXP_CORE wxProgressDialog : public wxGenericProgressDialog
{
public:
    wxProgressDialog(const wxString& title, const wxString& message,
                     int maximum = 100,
                     wxWindow *parent = NULL,
                     int style = wxPD_APP_MODAL | wxPD_AUTO_HIDE);

    virtual ~wxProgressDialog();

    virtual bool Update(int value, const wxString& newmsg = wxEmptyString, bool *skip = NULL);
    virtual bool Pulse(const wxString& newmsg = wxEmptyString, bool *skip = NULL);

    void Resume();

    int GetValue() const;
    wxString GetMessage() const;

    void SetRange(int maximum);

    // Return whether "Cancel" or "Skip" button was pressed, always return
    // false if the corresponding button is not shown.
    bool WasSkipped() const;
    bool WasCancelled() const;

    virtual void SetTitle(const wxString &title);

    virtual bool Show( bool show = true );

private:
    const wxString m_labelElapsed;
    const wxString m_labelEstimated;
    const wxString m_labelRemaining;

    // Local copies of shared data to conform to const methods.
    wxString m_title;
    wxString m_message;
    wxString m_expandedInformation;
    int m_value;
    bool m_skipped;

    bool HasNativeProgressDialog() const;

    // Performs common routines to Update() and Pulse(). Requires the
    // shared object to have been entered.
    bool DoBeforeUpdate(bool *skip);

    // Updates the various timing informations for both determinate
    // and indeterminate modes. Requires the shared object to have
    // been entered.
    void UpdateExpandedInformation(int value);

    SharedProgressDialogData *m_sharedData;

    wxScopedPtr<TaskDialogRunner> m_taskDialogRunner;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxProgressDialog);
};

#endif // _WX_PROGDLG_H_
