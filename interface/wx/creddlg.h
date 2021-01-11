/////////////////////////////////////////////////////////////////////////////
// Name:        creddlg.h
// Created:     2018-10-23
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxCredentialEntryDialog

    This class represents a dialog that requests a user name and a password
    from the user.

    Currently it is implemented as a generic wxWidgets dialog on all platforms.

    Simple example of using this dialog assuming @c MyFrame object has a member
    @c m_request of wxWebRequest type:
    @code
        void MyFrame::OnWebRequestState(wxWebRequestEvent& evt)
        {
            if ( evt.GetState() == wxWebRequest::State_Unauthorized )
            {
                wxCredentialEntryDialog dialog
                    (
                        this,
                        wxString::Format
                        (
                            "Please enter credentials for accessing "
                            "the web page at %s",
                            evt.GetResponse().GetURL()
                        ),
                        "My Application Title"
                    );
                if ( dialog.ShowModal() == wxID_OK )
                {
                    m_request.GetAuthChallenge().
                        SetCredentials(dialog.GetCredentials());
                }
                //else: the dialog was cancelled
            }
        }
    @endcode

    @note For secure saving and loading users and passwords, have a look at
        wxSecretStore.

    @since 3.1.5

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
        const wxWebCredentials& cred = wxWebCredentials());

    /**
        Create the dialog constructed using the default constructor.

        @param parent
            Parent window.
        @param message
            Message to show on the dialog.
        @param title
            Title of the dialog.
        @param cred
            The default username and password to use (optional).
    */
    bool Create(wxWindow* parent, const wxString& message,
        const wxString& title,
        const wxWebCredentials& cred = wxWebCredentials());

    /**
        Returns the credentials entered by the user.

        This should be called if ShowModal() returned ::wxID_OK.
    */
    wxWebCredentials GetCredentials() const;

    /**
        Sets the current user name.

        This function may be called before showing the dialog to provide the
        default value for the user name, if it's different from the one given
        at the creation time.
    */
    void SetUser(const wxString& user);

    /**
        Sets the current password.

        This function may be called before showing the dialog for the reasons
        similar to SetUser().
    */
    void SetPassword(const wxString& password);
};
