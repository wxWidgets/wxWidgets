/////////////////////////////////////////////////////////////////////////////
// Name:        choicdlg.h
// Purpose:     documentation for wxMultiChoiceDialog class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMultiChoiceDialog
    @wxheader{choicdlg.h}
    
    This class represents a dialog that shows a list of strings, and allows
    the user to select one or more.
    
    @library{wxbase}
    @category{cmndlg}
    
    @seealso
    @ref overview_wxmultichoicedialogoverview "wxMultiChoiceDialog overview",
    wxSingleChoiceDialog
*/
class wxMultiChoiceDialog : public wxDialog
{
public:
    //@{
    /**
        Constructor taking an array of wxString choices.
        
        @param parent 
        Parent window.
        
        @param message 
        Message to show on the dialog.
        
        @param caption 
        The dialog caption.
        
        @param n 
        The number of choices.
        
        @param choices 
        An array of strings, or a string list, containing the choices.
        
        @param style 
        A dialog style (bitlist) containing flags chosen from standard
        dialog styles and the following:
        
        
        wxOK
        
        
        Show an OK button.
        
        wxCANCEL
        
        
        Show a Cancel button.
        
        wxCENTRE
        
        
        Centre the message. Not Windows.
        
        The default value is equivalent to wxDEFAULT_DIALOG_STYLE |  wxRESIZE_BORDER | 
        wxOK |  wxCANCEL |  wxCENTRE.
        
        @param pos 
        Dialog position. Not Windows.
        
        @remarks Use ShowModal() to show the dialog.
    */
    wxMultiChoiceDialog(wxWindow* parent, const wxString& message,
                        const wxString& caption,
                        int n,
                        const wxString* choices,
                        long style = wxCHOICEDLG_STYLE,
                        const wxPoint& pos = wxDefaultPosition);
        wxMultiChoiceDialog(wxWindow* parent,
                            const wxString& message,
                            const wxString& caption,
                            const wxArrayString& choices,
                            long style = wxCHOICEDLG_STYLE,
                            const wxPoint& pos = wxDefaultPosition);
    //@}

    /**
        Returns array with indexes of selected items.
    */
    wxArrayInt GetSelection();

    /**
        Sets selected items from the array of selected items' indexes.
    */
    void SetSelections(const wxArrayInt& selections);

    /**
        Shows the dialog, returning either wxID_OK or wxID_CANCEL.
    */
    int ShowModal();
};


/**
    @class wxSingleChoiceDialog
    @wxheader{choicdlg.h}
    
    This class represents a dialog that shows a list of strings, and allows the
    user to select one. Double-clicking on a list item is equivalent to
    single-clicking and then pressing OK.
    
    @library{wxbase}
    @category{cmndlg}
    
    @seealso
    @ref overview_wxsinglechoicedialogoverview "wxSingleChoiceDialog overview",
    wxMultiChoiceDialog
*/
class wxSingleChoiceDialog : public wxDialog
{
public:
    //@{
    /**
        Constructor, taking an array of wxString choices and optional client data.
        
        @param parent 
        Parent window.
        
        @param message 
        Message to show on the dialog.
        
        @param caption 
        The dialog caption.
        
        @param n 
        The number of choices.
        
        @param choices 
        An array of strings, or a string list, containing the choices.
        
        @param clientData 
        An array of client data to be associated with the items.
        See GetSelectionClientData.
        
        @param style 
        A dialog style (bitlist) containing flags chosen from standard
        dialog styles and the following:
        
        
        wxOK
        
        
        Show an OK button.
        
        wxCANCEL
        
        
        Show a Cancel button.
        
        wxCENTRE
        
        
        Centre the message. Not Windows.
        
        The default value is equivalent to wxDEFAULT_DIALOG_STYLE |  wxRESIZE_BORDER | 
        wxOK |  wxCANCEL |  wxCENTRE.
        
        @param pos 
        Dialog position. Not Windows.
        
        @remarks Use ShowModal() to show the dialog.
    */
    wxSingleChoiceDialog(wxWindow* parent, const wxString& message,
                         const wxString& caption,
                         int n,
                         const wxString* choices,
                         void** clientData = @NULL,
                         long style = wxCHOICEDLG_STYLE,
                         const wxPoint& pos = wxDefaultPosition);
        wxSingleChoiceDialog(wxWindow* parent,
                             const wxString& message,
                             const wxString& caption,
                             const wxArrayString& choices,
                             void** clientData = @NULL,
                             long style = wxCHOICEDLG_STYLE,
                             const wxPoint& pos = wxDefaultPosition);
    //@}

    /**
        Returns the index of selected item.
    */
    int GetSelection();

    /**
        Returns the client data associated with the selection.
    */
    char* GetSelectionClientData();

    /**
        Returns the selected string.
    */
    wxString GetStringSelection();

    /**
        Sets the index of the initially selected item.
    */
    void SetSelection(int selection);

    /**
        Shows the dialog, returning either wxID_OK or wxID_CANCEL.
    */
    int ShowModal();
};


// ============================================================================
// Global functions/macros
// ============================================================================

//@{
/**
    As @b wxGetSingleChoice but returns the index representing the selected
    string. If the user pressed cancel, -1 is returned.
*/
int wxGetSingleChoiceIndex(const wxString& message,
                           const wxString& caption,
                           const wxArrayString& aChoices,
                           wxWindow * parent = @NULL,
                           int x = -1,
                           int y = -1,
                           bool centre = @true,
                           int width=150,
                           int height=200);
    int wxGetSingleChoiceIndex(const wxString& message,
                               const wxString& caption,
                               int n,
                               const wxString& choices[],
                               wxWindow * parent = @NULL,
                               int x = -1,
                               int y = -1,
                               bool centre = @true,
                               int width=150,
                               int height=200);
//@}

//@{
/**
    Pops up a dialog box containing a message, OK/Cancel buttons and a
    single-selection listbox. The user may choose an item and press OK to return a
    string or Cancel to return the empty string. Use
    wxGetSingleChoiceIndex if empty string is a
    valid choice and if you want to be able to detect pressing Cancel reliably.
    
    You may pass the list of strings to choose from either using @e choices
    which is an array of @e n strings for the listbox or by using a single
    @e aChoices parameter of type wxArrayString.
    
    If @e centre is @true, the message text (which may include new line
    characters) is centred; if @false, the message is left-justified.
*/
wxString wxGetSingleChoice(const wxString& message,
                           const wxString& caption,
                           const wxArrayString& aChoices,
                           wxWindow * parent = @NULL,
                           int x = -1,
                           int y = -1,
                           bool centre = @true,
                           int width=150,
                           int height=200);
    wxString wxGetSingleChoice(const wxString& message,
                               const wxString& caption,
                               int n,
                               const wxString& choices[],
                               wxWindow * parent = @NULL,
                               int x = -1,
                               int y = -1,
                               bool centre = @true,
                               int width=150,
                               int height=200);
//@}

//@{
/**
    As @b wxGetSingleChoice but takes an array of client data pointers
    corresponding to the strings, and returns one of these pointers or @NULL if
    Cancel was pressed. The @e client_data array must have the same number of
    elements as @e choices or @e aChoices!
*/
wxString wxGetSingleChoiceData(const wxString& message,
                               const wxString& caption,
                               const wxArrayString& aChoices,
                               const wxString& client_data[],
                               wxWindow * parent = @NULL,
                               int x = -1,
                               int y = -1,
                               bool centre = @true,
                               int width=150,
                               int height=200);
    wxString wxGetSingleChoiceData(const wxString& message,
                                   const wxString& caption,
                                   int n,
                                   const wxString& choices[],
                                   const wxString& client_data[],
                                   wxWindow * parent = @NULL,
                                   int x = -1,
                                   int y = -1,
                                   bool centre = @true,
                                   int width=150,
                                   int height=200);
//@}

//@{
/**
    Pops up a dialog box containing a message, OK/Cancel buttons and a
    multiple-selection listbox. The user may choose an arbitrary (including 0)
    number of items in the listbox whose indices will be returned in
    @e selection array. The initial contents of this array will be used to
    select the items when the dialog is shown.
    
    You may pass the list of strings to choose from either using @e choices
    which is an array of @e n strings for the listbox or by using a single
    @e aChoices parameter of type wxArrayString.
    
    If @e centre is @true, the message text (which may include new line
    characters) is centred; if @false, the message is left-justified.
*/
size_t wxGetMultipleChoices(wxArrayInt& selections,
                            const wxString& message,
                            const wxString& caption,
                            const wxArrayString& aChoices,
                            wxWindow * parent = @NULL,
                            int x = -1,
                            int y = -1,
                            bool centre = @true,
                            int width=150,
                            int height=200);
    size_t wxGetMultipleChoices(wxArrayInt& selections,
                                const wxString& message,
                                const wxString& caption,
                                int n,
                                const wxString& choices[],
                                wxWindow * parent = @NULL,
                                int x = -1,
                                int y = -1,
                                bool centre = @true,
                                int width=150,
                                int height=200);
//@}

