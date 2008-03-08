/////////////////////////////////////////////////////////////////////////////
// Name:        richtext/richtextstyles.h
// Purpose:     documentation for wxRichTextStyleListCtrl class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxRichTextStyleListCtrl
    @headerfile richtextstyles.h wx/richtext/richtextstyles.h
    
    This class incorporates a wxRichTextStyleListBox and
    a choice control that allows the user to select the category of style to view.
    It is demonstrated in the wxRichTextCtrl sample in @c samples/richtext.
    
    To use wxRichTextStyleListCtrl, add the control to your window hierarchy and
    call wxRichTextStyleListCtrl::SetStyleType with
    one of wxRichTextStyleListBox::wxRICHTEXT_STYLE_ALL,
    wxRichTextStyleListBox::wxRICHTEXT_STYLE_PARAGRAPH,
    wxRichTextStyleListBox::wxRICHTEXT_STYLE_CHARACTER and
    wxRichTextStyleListBox::wxRICHTEXT_STYLE_LIST to set the current view.
    Associate the control with a style sheet and rich text control with
    SetStyleSheet and SetRichTextCtrl,
    so that when a style is double-clicked, it is applied to the selection.
    
    @beginStyleTable
    @style{wxRICHTEXTSTYLELIST_HIDE_TYPE_SELECTOR}:
           This style hides the category selection control.
    @endStyleTable
    
    @library{wxrichtext}
    @category{FIXME}
*/
class wxRichTextStyleListCtrl : public wxControl
{
public:
    //@{
    /**
        Constructors.
    */
    wxRichTextStyleListCtrl(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = 0);
        wxRichTextStyleListCtrl();
    //@}

    /**
        Creates the windows.
    */
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0);

    /**
        Returns the associated rich text control, if any.
    */
    wxRichTextCtrl* GetRichTextCtrl();

    /**
        Returns the wxChoice control used for selecting the style category.
    */
    wxChoice* GetStyleChoice();

    /**
        Returns the wxListBox control used to view the style list.
    */
    wxRichTextStyleListBox* GetStyleListBox();

    /**
        Returns the associated style sheet, if any.
    */
    wxRichTextStyleSheet* GetStyleSheet();

    /**
        Returns the type of style to show in the list box.
    */
    wxRichTextStyleListBox::wxRichTextStyleType GetStyleType();

    /**
        Associates the control with a wxRichTextCtrl.
    */
    void SetRichTextCtrl(wxRichTextCtrl* ctrl);

    /**
        Associates the control with a style sheet.
    */
    void SetStyleSheet(wxRichTextStyleSheet* styleSheet);

    /**
        Sets the style type to display. One of
        wxRichTextStyleListBox::wxRICHTEXT_STYLE_ALL, wxRichTextStyleListBox::wxRICHTEXT_STYLE_PARAGRAPH,
        wxRichTextStyleListBox::wxRICHTEXT_STYLE_CHARACTER and
        wxRichTextStyleListBox::wxRICHTEXT_STYLE_LIST.
    */
    void SetStyleType(wxRichTextStyleListBox::wxRichTextStyleType styleType);

    /**
        Updates the style list box.
    */
    void UpdateStyles();
};


/**
    @class wxRichTextStyleDefinition
    @headerfile richtextstyles.h wx/richtext/richtextstyles.h
    
    This is a base class for paragraph and character styles.
    
    @library{wxrichtext}
    @category{FIXME}
*/
class wxRichTextStyleDefinition : public wxObject
{
public:
    /**
        Constructor.
    */
    wxRichTextStyleDefinition(const wxString& name = wxEmptyString);

    /**
        Destructor.
    */
    ~wxRichTextStyleDefinition();

    /**
        Returns the style on which this style is based.
    */
    const wxString GetBaseStyle();

    /**
        Returns the style's description.
    */
    const wxString GetDescription();

    /**
        Returns the style name.
    */
    const wxString GetName();

    //@{
    /**
        Returns the attributes associated with this style.
    */
    wxTextAttr GetStyle();
        const wxTextAttr GetStyle();
    //@}

    /**
        Returns the style attributes combined with the attributes of the specified base
        style, if any. This function works recursively.
    */
    wxTextAttr GetStyleMergedWithBase(wxRichTextStyleSheet* sheet);

    /**
        Sets the name of the style that this style is based on.
    */
    void SetBaseStyle(const wxString& name);

    /**
        Sets the style description.
    */
    void SetDescription(const wxString& descr);

    /**
        Sets the name of the style.
    */
    void SetName(const wxString& name);

    /**
        Sets the attributes for this style.
    */
    void SetStyle(const wxTextAttr& style);
};


/**
    @class wxRichTextParagraphStyleDefinition
    @headerfile richtextstyles.h wx/richtext/richtextstyles.h
    
    This class represents a paragraph style definition, usually added to a
    wxRichTextStyleSheet.
    
    @library{wxrichtext}
    @category{FIXME}
*/
class wxRichTextParagraphStyleDefinition : public wxRichTextStyleDefinition
{
public:
    /**
        Constructor.
    */
    wxRichTextParagraphStyleDefinition(const wxString& name = wxEmptyString);

    /**
        Destructor.
    */
    ~wxRichTextParagraphStyleDefinition();

    /**
        Returns the style that should normally follow this style.
    */
    const wxString GetNextStyle();

    /**
        Sets the style that should normally follow this style.
    */
    void SetNextStyle(const wxString& name);
};


/**
    @class wxRichTextStyleListBox
    @headerfile richtextstyles.h wx/richtext/richtextstyles.h
    
    This is a listbox that can display the styles in a wxRichTextStyleSheet,
    and apply the selection to an associated wxRichTextCtrl.
    
    See @c samples/richtext for an example of how to use it.
    
    @library{wxrichtext}
    @category{FIXME}
    
    @seealso
    wxRichTextStyleComboCtrl, @ref overview_wxrichtextctrloverview "wxRichTextCtrl
    overview"
*/
class wxRichTextStyleListBox : public wxHtmlListBox
{
public:
    /**
        Constructor.
    */
    wxRichTextStyleListBox(wxWindow* parent,
                           wxWindowID id = wxID_ANY,
                           const wxPoint& pos = wxDefaultPosition,
                           const wxSize& size = wxDefaultSize,
                           long style = 0);

    /**
        Destructor.
    */
    ~wxRichTextStyleListBox();

    /**
        Applies the @e ith style to the associated rich text control.
    */
    void ApplyStyle(int i);

    /**
        Converts units in tenths of a millimetre to device units.
    */
    int ConvertTenthsMMToPixels(wxDC& dc, int units);

    /**
        Creates a suitable HTML fragment for a definition.
    */
    wxString CreateHTML(wxRichTextStyleDefinition* def);

    /**
        If the return value is @true, clicking on a style name in the list will
        immediately
        apply the style to the associated rich text control.
    */
    bool GetApplyOnSelection();

    /**
        Returns the wxRichTextCtrl associated with this listbox.
    */
    wxRichTextCtrl* GetRichTextCtrl();

    /**
        Gets a style for a listbox index.
    */
    wxRichTextStyleDefinition* GetStyle(size_t i);

    /**
        Returns the style sheet associated with this listbox.
    */
    wxRichTextStyleSheet* GetStyleSheet();

    /**
        Returns the type of style to show in the list box.
    */
    wxRichTextStyleListBox::wxRichTextStyleType GetStyleType();

    /**
        Returns the HTML for this item.
    */
    wxString OnGetItem(size_t n);

    /**
        Implements left click behaviour, applying the clicked style to the
        wxRichTextCtrl.
    */
    void OnLeftDown(wxMouseEvent& event);

    /**
        Reacts to selection.
    */
    void OnSelect(wxCommandEvent& event);

    /**
        If @e applyOnSelection is @true, clicking on a style name in the list will
        immediately
        apply the style to the associated rich text control.
    */
    void SetApplyOnSelection(bool applyOnSelection);

    /**
        Associates the listbox with a wxRichTextCtrl.
    */
    void SetRichTextCtrl(wxRichTextCtrl* ctrl);

    /**
        Associates the control with a style sheet.
    */
    void SetStyleSheet(wxRichTextStyleSheet* styleSheet);

    /**
        Sets the style type to display. One of
        wxRichTextStyleListBox::wxRICHTEXT_STYLE_ALL, wxRichTextStyleListBox::wxRICHTEXT_STYLE_PARAGRAPH,
        wxRichTextStyleListBox::wxRICHTEXT_STYLE_CHARACTER and
        wxRichTextStyleListBox::wxRICHTEXT_STYLE_LIST.
    */
    void SetStyleType(wxRichTextStyleListBox::wxRichTextStyleType styleType);

    /**
        Updates the list from the associated style sheet.
    */
    void UpdateStyles();
};


/**
    @class wxRichTextStyleComboCtrl
    @headerfile richtextstyles.h wx/richtext/richtextstyles.h
    
    This is a combo control that can display the styles in a wxRichTextStyleSheet,
    and apply the selection to an associated wxRichTextCtrl.
    
    See @c samples/richtext for an example of how to use it.
    
    @library{wxrichtext}
    @category{FIXME}
    
    @seealso
    wxRichTextStyleListBox, @ref overview_wxrichtextctrloverview "wxRichTextCtrl
    overview"
*/
class wxRichTextStyleComboCtrl : public wxComboCtrl
{
public:
    /**
        Constructor.
    */
    wxRichTextStyleComboCtrl(wxWindow* parent,
                             wxWindowID id = wxID_ANY,
                             const wxPoint& pos = wxDefaultPosition,
                             const wxSize& size = wxDefaultSize,
                             long style = 0);

    /**
        Destructor.
    */
    ~wxRichTextStyleComboCtrl();

    /**
        Returns the wxRichTextCtrl associated with this control.
    */
    wxRichTextCtrl* GetRichTextCtrl();

    /**
        Returns the style sheet associated with this control.
    */
    wxRichTextStyleSheet* GetStyleSheet();

    /**
        Associates the control with a wxRichTextCtrl.
    */
    void SetRichTextCtrl(wxRichTextCtrl* ctrl);

    /**
        Associates the control with a style sheet.
    */
    void SetStyleSheet(wxRichTextStyleSheet* styleSheet);

    /**
        Updates the combo control from the associated style sheet.
    */
    void UpdateStyles();
};


/**
    @class wxRichTextCharacterStyleDefinition
    @headerfile richtextstyles.h wx/richtext/richtextstyles.h
    
    This class represents a character style definition, usually added to a
    wxRichTextStyleSheet.
    
    @library{wxrichtext}
    @category{FIXME}
*/
class wxRichTextCharacterStyleDefinition : public wxRichTextStyleDefinition
{
public:
    /**
        Constructor.
    */
    wxRichTextCharacterStyleDefinition(const wxString& name = wxEmptyString);

    /**
        Destructor.
    */
    ~wxRichTextCharacterStyleDefinition();
};


/**
    @class wxRichTextListStyleDefinition
    @headerfile richtextstyles.h wx/richtext/richtextstyles.h
    
    This class represents a list style definition, usually added to a
    wxRichTextStyleSheet.
    
    The class inherits paragraph attributes from
    wxRichTextStyleParagraphDefinition, and adds 10 further attribute objects, one for each level of a list.
    When applying a list style to a paragraph, the list style's base and
    appropriate level attributes are merged with the
    paragraph's existing attributes.
    
    You can apply a list style to one or more paragraphs using
    wxRichTextCtrl::SetListStyle. You
    can also use the functions wxRichTextCtrl::NumberList,
    wxRichTextCtrl::PromoteList and 
    wxRichTextCtrl::ClearListStyle. As usual, there are wxRichTextBuffer versions
    of these functions
    so that you can apply them directly to a buffer without requiring a control.
    
    @library{wxrichtext}
    @category{FIXME}
*/
class wxRichTextListStyleDefinition : public wxRichTextParagraphStyleDefinition
{
public:
    /**
        Constructor.
    */
    wxRichTextListStyleDefinition(const wxString& name = wxEmptyString);

    /**
        Destructor.
    */
    ~wxRichTextListStyleDefinition();

    /**
        This function combines the given paragraph style with the list style's base
        attributes and level style matching the given indent, returning the combined attributes.
        If @e styleSheet is specified, the base style for this definition will also be
        included in the result.
    */
    wxTextAttr CombineWithParagraphStyle(int indent,
                                         const wxTextAttr& paraStyle,
                                         wxRichTextStyleSheet* styleSheet = @NULL);

    /**
        This function finds the level (from 0 to 9) whose indentation attribute mostly
        closely matches @e indent (expressed in tenths of a millimetre).
    */
    int FindLevelForIndent(int indent);

    /**
        This function combines the list style's base attributes and the level style
        matching the given indent, returning the combined attributes.
        If @e styleSheet is specified, the base style for this definition will also be
        included in the result.
    */
    wxTextAttr GetCombinedStyle(int indent,
                                wxRichTextStyleSheet* styleSheet = @NULL);

    /**
        This function combines the list style's base attributes and the style for the
        specified level, returning the combined attributes.
        If @e styleSheet is specified, the base style for this definition will also be
        included in the result.
    */
    wxTextAttr GetCombinedStyleLevel(int level,
                                     wxRichTextStyleSheet* styleSheet = @NULL);

    /**
        Returns the style for the given level. @e level is a number between 0 and 9.
    */
    const wxTextAttr* GetLevelAttributes(int level);

    /**
        Returns the number of levels. This is hard-wired to 10.
        
        Returns the style for the given level. @e level is a number between 0 and 9.
    */
    int GetLevelCount();

    /**
        Returns @true if the given level has numbered list attributes.
    */
    int IsNumbered(int level);

    //@{
    /**
        Sets the style for the given level. @e level is a number between 0 and 9.
        
        The first and most flexible form uses a wxTextAttr object, while the second
        form is for convenient setting of the most commonly-used attributes.
    */
    void SetLevelAttributes(int level, const wxTextAttr& attr);
        void SetLevelAttributes(int level, int leftIndent,
                                int leftSubIndent,
                                int bulletStyle,
                                const wxString& bulletSymbol = wxEmptyString);
    //@}
};


/**
    @class wxRichTextStyleSheet
    @headerfile richtextstyles.h wx/richtext/richtextstyles.h
    
    A style sheet contains named paragraph and character styles that make it
    easy for a user to apply combinations of attributes to a wxRichTextCtrl.
    
    You can use a wxRichTextStyleListBox in your
    user interface to show available styles to the user, and allow application
    of styles to the control.
    
    @library{wxrichtext}
    @category{FIXME}
*/
class wxRichTextStyleSheet : public wxObject
{
public:
    /**
        Constructor.
    */
    wxRichTextStyleSheet();

    /**
        Destructor.
    */
    ~wxRichTextStyleSheet();

    /**
        Adds a definition to the character style list.
    */
    bool AddCharacterStyle(wxRichTextCharacterStyleDefinition* def);

    /**
        Adds a definition to the list style list.
    */
    bool AddListStyle(wxRichTextListStyleDefinition* def);

    /**
        Adds a definition to the paragraph style list.
    */
    bool AddParagraphStyle(wxRichTextParagraphStyleDefinition* def);

    /**
        Adds a definition to the appropriate style list.
    */
    bool AddStyle(wxRichTextStyleDefinition* def);

    /**
        Deletes all styles.
    */
    void DeleteStyles();

    /**
        Finds a character definition by name.
    */
    wxRichTextCharacterStyleDefinition* FindCharacterStyle(const wxString& name);

    /**
        Finds a list definition by name.
    */
    wxRichTextListStyleDefinition* FindListStyle(const wxString& name);

    /**
        Finds a paragraph definition by name.
    */
    wxRichTextParagraphStyleDefinition* FindParagraphStyle(const wxString& name);

    /**
        Finds a style definition by name.
    */
    wxRichTextStyleDefinition* FindStyle(const wxString& name);

    /**
        Returns the @e nth character style.
    */
    wxRichTextCharacterStyleDefinition* GetCharacterStyle(size_t n);

    /**
        Returns the number of character styles.
    */
    size_t GetCharacterStyleCount();

    /**
        Returns the style sheet's description.
    */
    const wxString GetDescription();

    /**
        Returns the @e nth list style.
    */
    wxRichTextListStyleDefinition* GetListStyle(size_t n);

    /**
        Returns the number of list styles.
    */
    size_t GetListStyleCount();

    /**
        Returns the style sheet's name.
    */
    const wxString GetName();

    /**
        Returns the @e nth paragraph style.
    */
    wxRichTextParagraphStyleDefinition* GetParagraphStyle(size_t n);

    /**
        Returns the number of paragraph styles.
    */
    size_t GetParagraphStyleCount();

    /**
        Removes a character style.
    */
    bool RemoveCharacterStyle(wxRichTextStyleDefinition* def,
                              bool deleteStyle = @false);

    /**
        Removes a list style.
    */
    bool RemoveListStyle(wxRichTextStyleDefinition* def,
                         bool deleteStyle = @false);

    /**
        Removes a paragraph style.
    */
    bool RemoveParagraphStyle(wxRichTextStyleDefinition* def,
                              bool deleteStyle = @false);

    /**
        Removes a style.
    */
    bool RemoveStyle(wxRichTextStyleDefinition* def,
                     bool deleteStyle = @false);

    /**
        Sets the style sheet's description.
    */
    void SetDescription(const wxString& descr);

    /**
        Sets the style sheet's name.
    */
    void SetName(const wxString& name);
};
