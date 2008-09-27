/////////////////////////////////////////////////////////////////////////////
// Name:        progdlg.h
// Purpose:     interface of wxProgressDialog
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxProgressDialog

    This class represents a dialog that shows a short message and a
    progress bar. Optionally, it can display ABORT and SKIP buttons,
    the elapsed, remaining and estimated time for the end of the progress.

    @beginStyleTable
    @style{wxPD_APP_MODAL}
           Make the progress dialog modal. If this flag is not given, it is
           only "locally" modal - that is the input to the parent window is
           disabled, but not to the other ones.
    @style{wxPD_AUTO_HIDE}
           Causes the progress dialog to disappear from screen as soon as the
           maximum value of the progress meter has been reached.
    @style{wxPD_SMOOTH}
           Causes smooth progress of the gauge control.
    @style{wxPD_CAN_ABORT}
           This flag tells the dialog that it should have a "Cancel" button
           which the user may press. If this happens, the next call to
           Update() will return @false.
    @style{wxPD_CAN_SKIP}
           This flag tells the dialog that it should have a "Skip" button
           which the user may press. If this happens, the next call to
           Update() will return @true in its skip parameter.
    @style{wxPD_ELAPSED_TIME}
           This flag tells the dialog that it should show elapsed time (since
           creating the dialog).
    @style{wxPD_ESTIMATED_TIME}
           This flag tells the dialog that it should show estimated time.
    @style{wxPD_REMAINING_TIME}
           This flag tells the dialog that it should show remaining time.
    @endStyleTable

    @library{wxbase}
    @category{cmndlg}
*/
class wxProgressDialog : public wxDialog
{
public:
    /**
        Constructor. Creates the dialog, displays it and disables user input
        for other windows, or, if @c wxPD_APP_MODAL flag is not given, for its
        parent window only.

        @param title
            Dialog title to show in titlebar.
        @param message
            Message displayed above the progress bar.
        @param maximum
            Maximum value for the progress bar.
        @param parent
            Parent window.
        @param style
            The dialog style. See wxProgressDialog.
    */
    wxProgressDialog(const wxString& title, const wxString& message,
                     int maximum = 100,
                     wxWindow* parent = NULL,
                     int style = wxPD_AUTO_HIDE | wxPD_APP_MODAL);

    /**
        Destructor. Deletes the dialog and enables all top level windows.
    */
    virtual ~wxProgressDialog();

    /**
        Works like Update() but makes the gauge control run in indeterminate mode
        (see wxGauge documentation); sets the remaining and the estimated time labels
        (if present) to "Unknown" or to @a newmsg (if it's non-empty); moves the progress
        bar a bit to indicate that some progress was done.
    */
    virtual bool Pulse(const wxString& newmsg = "",
                       bool* skip = NULL);

    /**
        Can be used to continue with the dialog, after the user had clicked the "Abort" button.
    */
    void Resume();

    /**
        Updates the dialog, setting the progress bar to the new value and, if
        given changes the message above it. Returns @true unless the "Cancel" button
        has been pressed.

        If @false is returned, the application can either immediately destroy the
        dialog or ask the user for the confirmation and if the abort is not confirmed
        the dialog may be resumed with Resume() function.

        @param value
            The new value of the progress meter. It should be less than or equal to
            the maximum value given to the constructor and the dialog is closed if
            it is equal to the maximum.
        @param newmsg
            The new messages for the progress dialog text, if it is
            empty (which is the default) the message is not changed.
        @param skip
            If "Skip" button was pressed since last Update() call,
            this is set to @true.
    */
    virtual bool Update(int value, const wxString& newmsg = "",
                        bool* skip = NULL);
};

