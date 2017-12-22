/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/progdlg.h
// Purpose:     wxProgressDialog
// Author:      Rickard Westerlund
// Created:     2010-07-22
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROGDLG_H_
#define _WX_PROGDLG_H_

class wxProgressDialogTaskRunner;
class wxProgressDialogSharedData;

class WXDLLIMPEXP_CORE wxProgressDialog : public wxGenericProgressDialog
{
public:
    wxProgressDialog(const wxString& title, const wxString& message,
                     int maximum = 100,
                     wxWindow *parent = NULL,
                     int style = wxPD_APP_MODAL | wxPD_AUTO_HIDE);

    virtual ~wxProgressDialog();

    virtual bool Update(int value, const wxString& newmsg = wxEmptyString, bool *skip = NULL) wxOVERRIDE;
    virtual bool Pulse(const wxString& newmsg = wxEmptyString, bool *skip = NULL) wxOVERRIDE;

    virtual void Resume() wxOVERRIDE;

    virtual int GetValue() const wxOVERRIDE;
    virtual wxString GetMessage() const wxOVERRIDE;

    virtual void SetRange(int maximum) wxOVERRIDE;

    // Return whether "Cancel" or "Skip" button was pressed, always return
    // false if the corresponding button is not shown.
    virtual bool WasSkipped() const wxOVERRIDE;
    virtual bool WasCancelled() const wxOVERRIDE;

    virtual void SetTitle(const wxString& title) wxOVERRIDE;
    virtual wxString GetTitle() const wxOVERRIDE;

    virtual void SetIcons(const wxIconBundle& icons) wxOVERRIDE;
    virtual void DoMoveWindow(int x, int y, int width, int height) wxOVERRIDE;
    virtual void DoGetPosition(int *x, int *y) const wxOVERRIDE;
    virtual void DoGetSize(int *width, int *height) const wxOVERRIDE;
    virtual void Fit() wxOVERRIDE;

    virtual bool Show( bool show = true ) wxOVERRIDE;

    // Must provide overload to avoid hiding it (and warnings about it)
    virtual void Update() wxOVERRIDE { wxGenericProgressDialog::Update(); }

    virtual WXWidget GetHandle() const wxOVERRIDE;

private:
    // Common part of Update() and Pulse().
    //
    // Returns false if the user requested cancelling the dialog.
    bool DoNativeBeforeUpdate(bool *skip);

    // Dispatch the pending events to let the windows to update, just as the
    // generic version does. This is done as part of DoNativeBeforeUpdate().
    void DispatchEvents();

    // Updates the various timing informations for both determinate
    // and indeterminate modes. Requires the shared object to have
    // been entered.
    void UpdateExpandedInformation(int value);

    // Get the task dialog geometry when using the native dialog.
    wxRect GetTaskDialogRect() const;


    wxProgressDialogTaskRunner *m_taskDialogRunner;

    wxProgressDialogSharedData *m_sharedData;

    // Store the message and title we currently use to be able to return it
    // from Get{Message,Title}()
    wxString m_message,
             m_title;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxProgressDialog);
};

#endif // _WX_PROGDLG_H_
