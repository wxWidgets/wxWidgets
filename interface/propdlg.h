/////////////////////////////////////////////////////////////////////////////
// Name:        propdlg.h
// Purpose:     documentation for wxPropertySheetDialog class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxPropertySheetDialog
    @wxheader{propdlg.h}

    This class represents a property sheet dialog: a tabbed dialog
    for showing settings. It is optimized to show flat tabs
    on PocketPC devices, and can be customized to use different
    controllers instead of the default notebook style.

    To use this class, call wxPropertySheetDialog::Create from your own
    Create function. Then call wxPropertySheetDialog::CreateButtons, and create
    pages, adding them to the book control.
    Finally call wxPropertySheetDialog::LayoutDialog.

    For example:

    @code
    bool MyPropertySheetDialog::Create(...)
    {
        if (!wxPropertySheetDialog::Create(...))
            return @false;

        CreateButtons(wxOK|wxCANCEL|wxHELP);

        // Add page
        wxPanel* panel = new wxPanel(GetBookCtrl(), ...);
        GetBookCtrl()-AddPage(panel, wxT("General"));

        LayoutDialog();
        return @true;
    }
    @endcode

    If necessary, override CreateBookCtrl and AddBookCtrl to create and add a
    different
    kind of book control. You would then need to use two-step construction for the
    dialog.
    Or, change the style of book control by calling
    wxPropertySheetDialog::SetSheetStyle
    before calling Create.

    The dialogs sample shows this class being used with notebook and toolbook
    controllers (for
    Windows-style and Mac-style settings dialogs).

    To make pages of the dialog scroll when the display is too small to fit the
    whole dialog, you can switch
    layout adaptation on globally with wxDialog::EnableLayoutAdaptation or
    per dialog with wxDialog::SetLayoutAdaptationMode. For more
    about layout adaptation, see @ref overview_autoscrollingdialogs "Automatic
    scrolling dialogs".

    @library{wxadv}
    @category{managedwnd}
*/
class wxPropertySheetDialog : public wxDialog
{
public:
    /**
        Constructor.
    */
    wxPropertySheetDialog(wxWindow* parent, wxWindowID id,
                          const wxString& title,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          long style = wxDEFAULT_DIALOG_STYLE,
                          const wxString& name = "dialogBox");

    /**
        Override this if you wish to add the book control in a way different from the
        standard way (for example, using different spacing).
    */
    virtual void AddBookCtrl(wxSizer* sizer);

    /**
        Call this from your own Create function, before adding buttons and pages.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE,
                const wxString& name = "dialogBox");

    /**
        Override this if you wish to create a different kind of book control; by
        default, the value
        passed to SetSheetStyle() is used to determine the control.
        The default behaviour is to create a notebook except on Smartphone, where a
        choicebook is used.
    */
    virtual wxBookCtrlBase* CreateBookCtrl();

    /**
        Call this to create the buttons for the dialog. This calls
        wxDialog::CreateButtonSizer, and
        the flags are the same. On PocketPC, no buttons are created.
    */
    void CreateButtons(int flags = wxOK|wxCANCEL);

    /**
        Returns the book control that will contain your settings pages.
    */
    wxBookCtrlBase* GetBookCtrl();

    /**
        Returns the inner sizer that contains the book control and button sizer.
    */
    wxSizer* GetInnerSizer();

    /**
        Returns the sheet style. See SetSheetStyle() for
        permissable values.
    */
    long GetSheetStyle();

    /**
        Call this to lay out the dialog. On PocketPC, this does nothing, since the
        dialog will be shown
        full-screen, and the layout will be done when the dialog receives a size event.
    */
    void LayoutDialog(int centreFlags = wxBOTH);

    /**
        Sets the book control used for the dialog. You will normally not need to use
        this.
    */
    void SetBookCtrl(wxBookCtrlBase* bookCtrl);

    /**
        Sets the inner sizer that contains the book control and button sizer. You will
        normally not need to use this.
    */
    void SetInnerSizer(wxSizer* sizer);

    /**
        You can customize the look and feel of the dialog by setting the sheet style.
        It is
        a bit list of the following values:
        
        wxPROPSHEET_DEFAULT
        
        Uses the default look and feel for the controller window,
        normally a notebook except on Smartphone where a choice control is used.
        
        wxPROPSHEET_NOTEBOOK
        
        Uses a notebook for the controller window.
        
        wxPROPSHEET_TOOLBOOK
        
        Uses a toolbook for the controller window.
        
        wxPROPSHEET_CHOICEBOOK
        
        Uses a choicebook for the controller window.
        
        wxPROPSHEET_LISTBOOK
        
        Uses a listbook for the controller window.
        
        wxPROPSHEET_TREEBOOK
        
        Uses a treebook for the controller window.
        
        wxPROPSHEET_SHRINKTOFIT
        
        Shrinks the dialog window to fit the currently selected page (common behaviour
        for
        property sheets on Mac OS X).
    */
    void SetSheetStyle(long style);
};
