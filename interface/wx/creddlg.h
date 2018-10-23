/////////////////////////////////////////////////////////////////////////////
// Name:        creddlg.h
// Created:     2018-10-23
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxCredentialEntryDialog

    This class represents a dialog that requests a user name and a password
    from the user. It is implemented as a generic wxWidgets dialog on all
    platforms.

    @note For secure saving and loading, user and passwords have a look at
        wxSecretStore.

    @since 3.1.2

    @library{wxcore}
    @category{cmndlg}

    @see @ref overview_cmndlg_cred
*/
class wxCredentialEntryDialog: public wxDialog
{
public:
    /**
        Default constructor.

        Call Create() to really create the dialog later.
    */
    wxCredentialEntryDialog();

    /**
        Constructor.

        Use ShowModal() to show the dialog.

        See Create() method for parameter description.
    */
    wxCredentialEntryDialog(wxWindow* parent, const wxString& message,
        const wxString& title,
        const wxString& user = "",
        const wxString& password = "");

    /**
        @param parent
            Parent window.
        @param message
            Message to show on the dialog.
        @param title
            The title of the dialog.
        @param user
            The default user value.
        @param password
            The default password.
    */
    bool Create(wxWindow* parent, const wxString& message,
        const wxString& title,
        const wxString& user = "",
        const wxString& password = "");

    /**
        Returns the entered user name.
    */
    wxString GetUser() const;

    /**
        Sets the current user name.
    */
    void SetUser(const wxString& user);

    /**
        Returns the entered password.
    */
    wxString GetPassword() const;

    /**
        Sets the current password.
    */
    void SetPassword(const wxString& password);
};
