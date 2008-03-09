/////////////////////////////////////////////////////////////////////////////
// Name:        fdrepdlg.h
// Purpose:     documentation for wxFindDialogEvent class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxFindDialogEvent
    @wxheader{fdrepdlg.h}

    wxFindReplaceDialog events

    @library{wxcore}
    @category{events}
*/
class wxFindDialogEvent : public wxCommandEvent
{
public:
    /**
        Constuctor used by wxWidgets only.
    */
    wxFindDialogEvent(wxEventType commandType = wxEVT_NULL,
                      int id = 0);

    /**
        Return the pointer to the dialog which generated this event.
    */
    wxFindReplaceDialog* GetDialog();

    /**
        Return the string to find (never empty).
    */
    wxString GetFindString();

    /**
        Get the currently selected flags: this is the combination of @c wxFR_DOWN,
        @c wxFR_WHOLEWORD and @c wxFR_MATCHCASE flags.
    */
    int GetFlags();

    /**
        Return the string to replace the search string with (only for replace and
        replace all events).
    */
    const wxString GetReplaceString();
};


/**
    @class wxFindReplaceData
    @wxheader{fdrepdlg.h}

    wxFindReplaceData holds the data for
    wxFindReplaceDialog. It is used to initialize
    the dialog with the default values and will keep the last values from the
    dialog when it is closed. It is also updated each time a
    wxFindDialogEvent is generated so instead of
    using the wxFindDialogEvent methods you can also directly query this object.

    Note that all @c SetXXX() methods may only be called before showing the
    dialog and calling them has no effect later.

    @library{wxcore}
    @category{FIXME}
*/
class wxFindReplaceData : public wxObject
{
public:
    /**
        Constuctor initializes the flags to default value (0).
    */
    wxFindReplaceData(wxUint32 flags = 0);

    /**
        Get the string to find.
    */
    const wxString GetFindString();

    /**
        Get the combination of @c wxFindReplaceFlags values.
    */
    int GetFlags();

    /**
        Get the replacement string.
    */
    const wxString GetReplaceString();

    /**
        Set the string to find (used as initial value by the dialog).
    */
    void SetFindString(const wxString& str);

    /**
        Set the flags to use to initialize the controls of the dialog.
    */
    void SetFlags(wxUint32 flags);

    /**
        Set the replacement string (used as initial value by the dialog).
    */
    void SetReplaceString(const wxString& str);
};


/**
    @class wxFindReplaceDialog
    @wxheader{fdrepdlg.h}

    wxFindReplaceDialog is a standard modeless dialog which is used to allow the
    user to search for some text (and possibly replace it with something else).
    The actual searching is supposed to be done in the owner window which is the
    parent of this dialog. Note that it means that unlike for the other standard
    dialogs this one @b must have a parent window. Also note that there is no
    way to use this dialog in a modal way; it is always, by design and
    implementation, modeless.

    Please see the dialogs sample for an example of using it.

    @library{wxcore}
    @category{cmndlg}
*/
class wxFindReplaceDialog : public wxDialog
{
public:
    //@{
    /**
        After using default constructor Create()
        must be called.
        The @a parent and @a data parameters must be non-@NULL.
    */
    wxFindReplaceDialog();
    wxFindReplaceDialog(wxWindow* parent,
                        wxFindReplaceData* data,
                        const wxString& title,
                        int style = 0);
    //@}

    /**
        Destructor.
    */
    ~wxFindReplaceDialog();

    /**
        Creates the dialog; use wxWindow::Show to show it on screen.
        The @a parent and @a data parameters must be non-@NULL.
    */
    bool Create(wxWindow* parent, wxFindReplaceData* data,
                const wxString& title, int style = 0);

    /**
        Get the wxFindReplaceData object used by this
        dialog.
    */
    const wxFindReplaceData* GetData();
};
