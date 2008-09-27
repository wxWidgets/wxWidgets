/////////////////////////////////////////////////////////////////////////////
// Name:        richtext/richtextformatdlg.h
// Purpose:     interface of wxRichTextFormattingDialogFactory
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxRichTextFormattingDialogFactory

    This class provides pages for wxRichTextFormattingDialog, and allows other
    customization of the dialog.
    A default instance of this class is provided automatically. If you wish to
    change the behaviour of the
    formatting dialog (for example add or replace a page), you may derive from this
    class,
    override one or more functions, and call the static function
    wxRichTextFormattingDialog::SetFormattingDialogFactory.

    @library{wxrichtext}
    @category{FIXME}
*/
class wxRichTextFormattingDialogFactory : public wxObject
{
public:
    /**
        Constructor.
    */
    wxRichTextFormattingDialogFactory();

    /**
        Destructor.
    */
    virtual ~wxRichTextFormattingDialogFactory();

    /**
        Creates the main dialog buttons.
    */
    virtual bool CreateButtons(wxRichTextFormattingDialog* dialog);

    /**
        Creates a page, given a page identifier.
    */
    virtual wxPanel* CreatePage(int page, wxString& title,
                                wxRichTextFormattingDialog* dialog);

    /**
        Creates all pages under the dialog's book control, also calling AddPage.
    */
    virtual bool CreatePages(long pages,
                             wxRichTextFormattingDialog* dialog);

    /**
        Enumerate all available page identifiers.
    */
    virtual int GetPageId(int i) const;

    /**
        Gets the number of available page identifiers.
    */
    virtual int GetPageIdCount() const;

    /**
        Gets the image index for the given page identifier.
    */
    virtual int GetPageImage(int id) const;

    /**
        Set the property sheet style, called at the start of
        wxRichTextFormattingDialog::Create.
    */
    virtual bool SetSheetStyle(wxRichTextFormattingDialog* dialog);

    /**
        Invokes help for the dialog.
    */
    virtual bool ShowHelp(int page,
                          wxRichTextFormattingDialog* dialog);
};



/**
    @class wxRichTextFormattingDialog

    This dialog allows the user to edit a character and/or paragraph style.

    In the constructor, specify the pages that will be created. Use GetStyle
    to retrieve the common style for a given range, and then use ApplyStyle
    to apply the user-selected formatting to a control. For example:

    @code
    wxRichTextRange range;
        if (m_richTextCtrl-HasSelection())
            range = m_richTextCtrl-GetSelectionRange();
        else
            range = wxRichTextRange(0, m_richTextCtrl-GetLastPosition()+1);

        int pages =
    wxRICHTEXT_FORMAT_FONT|wxRICHTEXT_FORMAT_INDENTS_SPACING|wxRICHTEXT_FORMAT_TABS|wxRICHTEXT_FORMAT_BULLETS;

        wxRichTextFormattingDialog formatDlg(pages, this);
        formatDlg.GetStyle(m_richTextCtrl, range);

        if (formatDlg.ShowModal() == wxID_OK)
        {
            formatDlg.ApplyStyle(m_richTextCtrl, range);
        }
    @endcode

    @library{wxrichtext}
    @category{richtext}
*/
class wxRichTextFormattingDialog : public wxPropertySheetDialog
{
public:
    //@{
    /**
        Constructors.
        
        @param flags
            The pages to show.
        @param parent
            The dialog's parent.
        @param id
            The dialog's identifier.
        @param title
            The dialog's caption.
        @param pos
            The dialog's position.
        @param size
            The dialog's size.
        @param style
            The dialog's window style.
    */
    wxRichTextFormattingDialog(long flags, wxWindow* parent);
    const wxPoint& pos = wxDefaultPosition, const wxSize& sz = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE)
    wxRichTextFormattingDialog();
    //@}

    /**
        Destructor.
    */
    virtual ~wxRichTextFormattingDialog();

    /**
        Apply attributes to the given range, only changing attributes that need to be
        changed.
    */
    bool ApplyStyle(wxRichTextCtrl* ctrl,
                    const wxRichTextRange& range,
                    int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_OPTIMIZE);

    /**
        Creation: see @ref overview_wxrichtextformattingdialog "the constructor" for
        details about the parameters.
    */
    bool Create(long flags, wxWindow* parent, const wxString& title,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                                     const wxSize& sz = wxDefaultSize,
                                                        long style = wxDEFAULT_DIALOG_STYLE);

    //@{
    /**
        Gets the attributes being edited.
    */
    const wxTextAttr GetAttributes();
    const wxTextAttr&  GetAttributes();
    //@}

    /**
        Helper for pages to get the top-level dialog.
    */
    static wxRichTextFormattingDialog* GetDialog(wxWindow* win);

    /**
        Helper for pages to get the attributes.
    */
    static wxTextAttr* GetDialogAttributes(wxWindow* win);

    /**
        Helper for pages to get the style.
    */
    static wxRichTextStyleDefinition* GetDialogStyleDefinition(wxWindow* win);

    /**
        Returns the object to be used to customize the dialog and provide pages.
    */
    static wxRichTextFormattingDialogFactory* GetFormattingDialogFactory();

    /**
        Returns the image list associated with the dialog, used for example if showing
        the dialog as a toolbook.
    */
    wxImageList* GetImageList() const;

    /**
        Gets common attributes from the given range and calls SetAttributes. Attributes
        that do not have common values in the given range
        will be omitted from the style's flags.
    */
    virtual bool GetStyle(wxRichTextCtrl* ctrl, const wxRichTextRange& range);

    /**
        Gets the associated style definition, if any.
    */
    virtual wxRichTextStyleDefinition* GetStyleDefinition() const;

    /**
        Gets the associated style sheet, if any.
    */
    virtual wxRichTextStyleSheet* GetStyleSheet() const;

    /**
        Sets the attributes to be edited.
    */
    void SetAttributes(const wxTextAttr& attr);

    /**
        Sets the formatting factory object to be used for customization and page
        creation.
        It deletes the existing factory object.
    */
    static void SetFormattingDialogFactory(wxRichTextFormattingDialogFactory* factory);

    /**
        Sets the image list associated with the dialog's property sheet.
    */
    void SetImageList(wxImageList* imageList);

    /**
        Sets the attributes and optionally updates the display, if @a update is @true.
    */
    virtual bool SetStyle(const wxTextAttr& style, bool update = true);

    /**
        Sets the style definition and optionally update the display, if @a update is @c
        @true.
    */
    virtual bool SetStyleDefinition(const wxRichTextStyleDefinition& styleDef,
                                    wxRichTextStyleSheet* sheet,
                                    bool update = true);

    /**
        Updates the display.
    */
    virtual bool UpdateDisplay();
};

