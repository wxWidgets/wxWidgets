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
                     wxWindow *parent = nullptr,
                     int style = wxPD_APP_MODAL | wxPD_AUTO_HIDE);

    virtual ~wxProgressDialog();

    virtual bool Update(int value, const wxString& newmsg = wxEmptyString, bool *skip = nullptr) override;
    virtual bool Pulse(const wxString& newmsg = wxEmptyString, bool *skip = nullptr) override;

    virtual void Resume() override;

    virtual int GetValue() const override;
    virtual wxString GetMessage() const override;

    virtual void SetRange(int maximum) override;

    // Return whether "Cancel" or "Skip" button was pressed, always return
    // false if the corresponding button is not shown.
    virtual bool WasSkipped() const override;
    virtual bool WasCancelled() const override;

    virtual void SetTitle(const wxString& title) override;
    virtual wxString GetTitle() const override;

    virtual void SetIcons(const wxIconBundle& icons) override;
    virtual void DoMoveWindow(int x, int y, int width, int height) override;
    virtual void DoGetPosition(int *x, int *y) const override;
    virtual void DoGetSize(int *width, int *height) const override;
    virtual void Fit() override;

    virtual bool Show( bool show = true ) override;

    // Must provide overload to avoid hiding it (and warnings about it)
    virtual void Update() override { wxGenericProgressDialog::Update(); }

    virtual WXWidget GetHandle() const override;

private:
    // Common part of Update() and Pulse().
    //
    // Returns false if the user requested cancelling the dialog.
    bool DoNativeBeforeUpdate(bool *skip);

    // Dispatch the pending events to let the windows to update, just as the
    // generic version does. This is done as part of DoNativeBeforeUpdate().
    void DispatchEvents();

    // Updates the various timing information for both determinate
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
