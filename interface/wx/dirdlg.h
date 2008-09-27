/////////////////////////////////////////////////////////////////////////////
// Name:        dirdlg.h
// Purpose:     interface of wxDirDialog
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxDirDialog

    This class represents the directory chooser dialog.

    @beginStyleTable
    @style{wxDD_DEFAULT_STYLE}
           Equivalent to a combination of wxDEFAULT_DIALOG_STYLE and
           wxRESIZE_BORDER (the last one is not used under wxWinCE).
    @style{wxDD_DIR_MUST_EXIST}
           The dialog will allow the user to choose only an existing folder.
           When this style is not given, a "Create new directory" button is
           added to the dialog (on Windows) or some other way is provided to
           the user to type the name of a new folder.
    @style{wxDD_CHANGE_DIR}
           Change the current working directory to the directory chosen by the
           user.
    @endStyleTable

    @note On Windows the new directory button is only available with recent
          versions of the common dialogs.

    @library{wxcore}
    @category{cmndlg}

    @see @ref overview_cmndlg_dir, wxFileDialog
*/
class wxDirDialog : public wxDialog
{
public:
    /**
        Constructor. Use ShowModal() to show the dialog.

        @param parent
            Parent window.
        @param message
            Message to show on the dialog.
        @param defaultPath
            The default path, or the empty string.
        @param style
            The dialog style. See wxDirDialog
        @param pos
            Dialog position. Ignored under Windows.
        @param size
            Dialog size. Ignored under Windows.
        @param name
            The dialog name, not used.
    */
    wxDirDialog(wxWindow* parent,
                const wxString& message = "Choose a directory",
                const wxString& defaultPath = "",
                long style = wxDD_DEFAULT_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = "wxDirCtrl");

    /**
        Destructor.
    */
    virtual ~wxDirDialog();

    /**
        Returns the message that will be displayed on the dialog.
    */
    virtual wxString GetMessage() const;

    /**
        Returns the default or user-selected path.
    */
    virtual wxString GetPath() const;

    /**
        Sets the message that will be displayed on the dialog.
    */
    virtual void SetMessage(const wxString& message);

    /**
        Sets the default path.
    */
    virtual void SetPath(const wxString& path);

    /**
        Shows the dialog, returning wxID_OK if the user pressed OK, and
        wxID_CANCEL otherwise.
    */
    int ShowModal();
};



// ============================================================================
// Global functions/macros
// ============================================================================

/** @ingroup group_funcmacro_dialog */
//@{

/**
    Pops up a directory selector dialog. The arguments have the same meaning
    as those of wxDirDialog::wxDirDialog(). The message is displayed at the
    top, and the default_path, if specified, is set as the initial selection.

    The application must check for an empty return value (if the user pressed
    Cancel). For example:

    @code
    const wxString& dir = wxDirSelector("Choose a folder");
    if ( !dir.empty() )
    {
        ...
    }
    @endcode

    @header{wx/dirdlg.h}
*/
wxString wxDirSelector(const wxString& message = wxDirSelectorPromptStr,
                       const wxString& default_path = "",
                       long style = 0,
                       const wxPoint& pos = wxDefaultPosition,
                       wxWindow* parent = NULL);

//@}

