/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.h
// Purpose:     documentation for wxTextAttr class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxTextAttr
    @wxheader{textctrl.h}

    wxTextAttr represents the character and paragraph attributes, or style,
    for a range of text in a wxTextCtrl or wxRichTextCtrl.

    When setting up a wxTextAttr object, pass a bitlist mask to
    wxTextAttr::SetFlags to
    indicate which style elements should be changed. As a convenience, when you
    call a setter such
    as SetFont, the relevant bit will be set.

    @library{wxcore}
    @category{richtext}

    @seealso
    wxTextCtrl, wxRichTextCtrl
*/
class wxTextAttr
{
public:
    //@{
    /**
        Constructors.
    */
    wxTextAttr();
    wxTextAttr(const wxColour& colText,
               const wxColour& colBack = wxNullColour,
               const wxFont& font = wxNullFont,
               wxTextAttrAlignment alignment = wxTEXT_ALIGNMENT_DEFAULT);
    wxTextAttr(const wxTextAttr& attr);
    //@}

    /**
        Applies the attributes in @a style to the original object, but not those
        attributes from @a style that are the same as those in @a compareWith (if passed).
    */
    bool Apply(const wxTextAttr& style,
               const wxTextAttr* compareWith = NULL);

    /**
        Creates a font from the font attributes.
    */
    wxFont CreateFont();

    /**
        Returns the alignment flags.
        See SetAlignment() for a list of available styles.
    */
    wxTextAttrAlignment GetAlignment();

    /**
        Returns the background colour.
    */
    const wxColour GetBackgroundColour();

    /**
        Returns a string containing the name of the font associated with the bullet
        symbol.
        Only valid for attributes with wxTEXT_ATTR_BULLET_SYMBOL.
    */
    const wxString GetBulletFont();

    /**
        Returns the standard bullet name, applicable if the bullet style is
        wxTEXT_ATTR_BULLET_STYLE_STANDARD.
        Currently the following standard bullet names are supported:
         @c standard/circle
         @c standard/square
         @c standard/diamond
         @c standard/triangle
        For wxRichTextCtrl users only: if you wish your rich text controls to support
        further bullet graphics, you can derive a
        class from wxRichTextRenderer or wxRichTextStdRenderer, override @c
        DrawStandardBullet and @c EnumerateStandardBulletNames, and
        set an instance of the class using wxRichTextBuffer::SetRenderer.
    */
    const wxString GetBulletName();

    /**
        Returns the bullet number.
    */
    int GetBulletNumber();

    /**
        Returns the bullet style.
        See SetBulletStyle() for a list of available styles.
    */
    int GetBulletStyle();

    /**
        Returns the bullet text, which could be a symbol, or (for example) cached
        outline text.
    */
    const wxString GetBulletText();

    /**
        Returns the name of the character style.
    */
    const wxString GetCharacterStyleName();

    /**
        Returns flags indicating which attributes are applicable.
        See SetFlags() for a list of available flags.
    */
    long GetFlags();

    /**
        Creates and returns a font specified by the font attributes in the wxTextAttr
        object. Note that
        wxTextAttr does not store a wxFont object, so this is only a temporary font.
        For greater
        efficiency, access the font attributes directly.
    */
    wxFont GetFont();

    /**
        Gets the font attributes from the given font, using only the attributes
        specified by @e flags.
    */
    bool GetFontAttributes(const wxFont& font,
                           int flags = wxTEXT_ATTR_FONT);

    /**
        Returns the font encoding.
    */
    wxFontEncoding GetFontEncoding();

    /**
        Returns the font face name.
    */
    const wxString GetFontFaceName();

    /**
        Returns the font size in points.
    */
    int GetFontSize();

    /**
        Returns the font style.
    */
    int GetFontStyle();

    /**
        Returns @true if the font is underlined.
    */
    bool GetFontUnderlined();

    /**
        Returns the font weight.
    */
    int GetFontWeight();

    /**
        Returns the left indent in tenths of a millimetre.
    */
    long GetLeftIndent();

    /**
        Returns the left sub-indent in tenths of a millimetre.
    */
    long GetLeftSubIndent();

    /**
        Returns the line spacing value, one of wxTEXT_ATTR_LINE_SPACING_NORMAL,
        wxTEXT_ATTR_LINE_SPACING_HALF, and wxTEXT_ATTR_LINE_SPACING_TWICE.
    */
    int GetLineSpacing();

    /**
        Returns the name of the list style.
    */
    const wxString GetListStyleName();

    /**
        Returns the outline level.
    */
    bool GetOutlineLevel();

    /**
        Returns the space in tenths of a millimeter after the paragraph.
    */
    int GetParagraphSpacingAfter();

    /**
        Returns the space in tenths of a millimeter before the paragraph.
    */
    int GetParagraphSpacingBefore();

    /**
        Returns the name of the paragraph style.
    */
    const wxString GetParagraphStyleName();

    /**
        Returns the right indent in tenths of a millimeter.
    */
    long GetRightIndent();

    /**
        Returns an array of tab stops, each expressed in tenths of a millimeter. Each
        stop
        is measured from the left margin and therefore each value must be larger than
        the last.
    */
    const wxArrayInt GetTabs();

    /**
        Returns the text foreground colour.
    */
    const wxColour GetTextColour();

    /**
        Returns the text effect bits of interest. See SetFlags() for further
        information.
    */
    int GetTextEffectFlags();

    /**
        Returns the text effects, a bit list of styles. See SetTextEffects() for
        details.
    */
    int GetTextEffects();

    /**
        Returns the URL for the content. Content with wxTEXT_ATTR_URL style
        causes wxRichTextCtrl to show a hand cursor over it, and wxRichTextCtrl
        generates
        a wxTextUrlEvent when the content is clicked.
    */
    const wxString GetURL();

    /**
        Returns @true if the attribute object specifies alignment.
    */
    bool HasAlignment();

    /**
        Returns @true if the attribute object specifies a background colour.
    */
    bool HasBackgroundColour();

    /**
        Returns @true if the attribute object specifies a standard bullet name.
    */
    bool HasBulletName();

    /**
        Returns @true if the attribute object specifies a bullet number.
    */
    bool HasBulletNumber();

    /**
        Returns @true if the attribute object specifies a bullet style.
    */
    bool HasBulletStyle();

    /**
        Returns @true if the attribute object specifies bullet text (usually
        specifying a symbol).
    */
    bool HasBulletText();

    /**
        Returns @true if the attribute object specifies a character style name.
    */
    bool HasCharacterStyleName();

    /**
        Returns @true if the @a flag is present in the attribute object's flag
        bitlist.
    */
    bool HasFlag(long flag);

    /**
        Returns @true if the attribute object specifies any font attributes.
    */
    bool HasFont();

    /**
        Returns @true if the attribute object specifies an encoding.
    */
    bool HasFontEncoding();

    /**
        Returns @true if the attribute object specifies a font face name.
    */
    bool HasFontFaceName();

    /**
        Returns @true if the attribute object specifies italic style.
    */
    bool HasFontItalic();

    /**
        Returns @true if the attribute object specifies a font point size.
    */
    bool HasFontSize();

    /**
        Returns @true if the attribute object specifies either underlining or no
        underlining.
    */
    bool HasFontUnderlined();

    /**
        Returns @true if the attribute object specifies font weight (bold, light or
        normal).
    */
    bool HasFontWeight();

    /**
        Returns @true if the attribute object specifies a left indent.
    */
    bool HasLeftIndent();

    /**
        Returns @true if the attribute object specifies line spacing.
    */
    bool HasLineSpacing();

    /**
        Returns @true if the attribute object specifies a list style name.
    */
    bool HasListStyleName();

    /**
        Returns @true if the attribute object specifies an outline level.
    */
    bool HasOutlineLevel();

    /**
        Returns @true if the attribute object specifies a page break before this
        paragraph.
    */
    bool HasPageBreak();

    /**
        Returns @true if the attribute object specifies spacing after a paragraph.
    */
    bool HasParagraphSpacingAfter();

    /**
        Returns @true if the attribute object specifies spacing before a paragraph.
    */
    bool HasParagraphSpacingBefore();

    /**
        Returns @true if the attribute object specifies a paragraph style name.
    */
    bool HasParagraphStyleName();

    /**
        Returns @true if the attribute object specifies a right indent.
    */
    bool HasRightIndent();

    /**
        Returns @true if the attribute object specifies tab stops.
    */
    bool HasTabs();

    /**
        Returns @true if the attribute object specifies a text foreground colour.
    */
    bool HasTextColour();

    /**
        Returns @true if the attribute object specifies text effects.
    */
    bool HasTextEffects();

    /**
        Returns @true if the attribute object specifies a URL.
    */
    bool HasURL();

    /**
        Returns @true if the object represents a character style, that is,
        the flags specify a font or a text background or foreground colour.
    */
    bool IsCharacterStyle();

    /**
        Returns @false if we have any attributes set, @true otherwise.
    */
    bool IsDefault();

    /**
        Returns @true if the object represents a paragraph style, that is,
        the flags specify alignment, indentation, tabs, paragraph spacing, or
        bullet style.
    */
    bool IsParagraphStyle();

    //@{
    /**
        Creates a new @c wxTextAttr which is a merge of @a base and
        @e overlay. Properties defined in @a overlay take precedence over those
        in @e base. Properties undefined/invalid in both are undefined in the
        result.
    */
    void Merge(const wxTextAttr& overlay);
    static wxTextAttr Merge(const wxTextAttr& base,
                            const wxTextAttr& overlay);
    //@}

    /**
        Sets the paragraph alignment. These are the possible values for @e alignment:
        
        Of these, wxTEXT_ALIGNMENT_JUSTIFIED is unimplemented. In future justification
        may be supported
        when printing or previewing, only.
    */
    void SetAlignment(wxTextAttrAlignment alignment);

    /**
        Sets the background colour.
    */
    void SetBackgroundColour(const wxColour& colBack);

    /**
        Sets the name of the font associated with the bullet symbol.
        Only valid for attributes with wxTEXT_ATTR_BULLET_SYMBOL.
    */
    void SetBulletFont(const wxString& font);

    /**
        Sets the standard bullet name, applicable if the bullet style is
        wxTEXT_ATTR_BULLET_STYLE_STANDARD.
        See GetBulletName() for a list
        of supported names, and how to expand the range of supported types.
    */
    void SetBulletName(const wxString& name);

    /**
        Sets the bullet number.
    */
    void SetBulletNumber(int n);

    /**
        Sets the bullet style. The following styles can be passed:
        
        Currently wxTEXT_ATTR_BULLET_STYLE_BITMAP is not supported.
    */
    void SetBulletStyle(int style);

    /**
        Sets the bullet text, which could be a symbol, or (for example) cached outline
        text.
    */
    void SetBulletText(const wxString text);

    /**
        Sets the character style name.
    */
    void SetCharacterStyleName(const wxString& name);

    /**
        Sets the flags determining which styles are being specified. The following
        flags can be passed in a bitlist:
    */
    void SetFlags(long flags);

    /**
        Sets the attributes for the given font. Note that wxTextAttr does not store an
        actual wxFont object.
    */
    void SetFont(const wxFont& font);

    /**
        Sets the font encoding.
    */
    void SetFontEncoding(wxFontEncoding encoding);

    /**
        Sets the paragraph alignment.
    */
    void SetFontFaceName(const wxString& faceName);

    /**
        Sets the font size in points.
    */
    void SetFontSize(int pointSize);

    /**
        Sets the font style (normal, italic or slanted).
    */
    void SetFontStyle(int fontStyle);

    /**
        Sets the font underlining.
    */
    void SetFontUnderlined(bool underlined);

    /**
        Sets the font weight.
    */
    void SetFontWeight(int fontWeight);

    /**
        Sets the left indent and left subindent in tenths of a millimetre.
        The sub-indent is an offset from the left of the paragraph, and is used for all
        but the
        first line in a paragraph. A positive value will cause the first line to appear
        to the left
        of the subsequent lines, and a negative value will cause the first line to be
        indented
        relative to the subsequent lines.
        wxRichTextBuffer uses indentation to render a bulleted item. The left indent is
        the distance between
        the margin and the bullet. The content of the paragraph, including the first
        line, starts
        at leftMargin + leftSubIndent. So the distance between the left edge of the
        bullet and the
        left of the actual paragraph is leftSubIndent.
    */
    void SetLeftIndent(int indent, int subIndent = 0);

    /**
        Sets the line spacing. @a spacing is a multiple, where 10 means single-spacing,
        15 means 1.5 spacing, and 20 means double spacing. The following constants are
        defined for convenience:
    */
    void SetLineSpacing(int spacing);

    /**
        Sets the list style name.
    */
    void SetListStyleName(const wxString& name);

    /**
        Specifies the outline level. Zero represents normal text. At present, the
        outline level is
        not used, but may be used in future for determining list levels and for
        applications
        that need to store document structure information.
    */
    void SetOutlineLevel(int level);

    /**
        Specifies a page break before this paragraph.
    */
    void SetPageBreak(bool pageBreak = true);

    /**
        Sets the spacing after a paragraph, in tenths of a millimetre.
    */
    void SetParagraphSpacingAfter(int spacing);

    /**
        Sets the spacing before a paragraph, in tenths of a millimetre.
    */
    void SetParagraphSpacingBefore(int spacing);

    /**
        Sets the name of the paragraph style.
    */
    void SetParagraphStyleName(const wxString& name);

    /**
        Sets the right indent in tenths of a millimetre.
    */
    void SetRightIndent(int indent);

    /**
        Sets the tab stops, expressed in tenths of a millimetre.
        Each stop is measured from the left margin and therefore each value must be
        larger than the last.
    */
    void SetTabs(const wxArrayInt& tabs);

    /**
        Sets the text foreground colout.
    */
    void SetTextColour(const wxColour& colText);

    /**
        Sets the text effect bits of interest. You should also pass wxTEXT_ATTR_EFFECTS
        to SetFlags().
        See SetFlags() for further information.
    */
    void SetTextEffectFlags(int flags);

    /**
        Sets the text effects, a bit list of styles.
        The following styles can be passed:
        
        Of these, only wxTEXT_ATTR_EFFECT_CAPITALS and wxTEXT_ATTR_EFFECT_STRIKETHROUGH
        are implemented.
        wxTEXT_ATTR_EFFECT_CAPITALS capitalises text when displayed (leaving the case
        of the actual buffer
        text unchanged), and wxTEXT_ATTR_EFFECT_STRIKETHROUGH draws a line through text.
        To set effects, you should also pass wxTEXT_ATTR_EFFECTS to SetFlags(), and call
        SetTextEffectFlags() with the styles (taken from the
        above set) that you are interested in setting.
    */
    void SetTextEffects(int effects);

    /**
        Sets the URL for the content. Sets the wxTEXT_ATTR_URL style; content with this
        style
        causes wxRichTextCtrl to show a hand cursor over it, and wxRichTextCtrl
        generates
        a wxTextUrlEvent when the content is clicked.
    */
    void SetURL(const wxString& url);

    /**
        Assignment from a wxTextAttr object.
    */
    void operator operator=(const wxTextAttr& attr);
};


/**
    @class wxTextCtrl
    @wxheader{textctrl.h}

    A text control allows text to be displayed and edited. It may be
    single line or multi-line.

    @beginStyleTable
    @style{wxTE_PROCESS_ENTER}:
           The control will generate the event wxEVT_COMMAND_TEXT_ENTER
           (otherwise pressing Enter key is either processed internally by the
           control or used for navigation between dialog controls).
    @style{wxTE_PROCESS_TAB}:
           The control will receive wxEVT_CHAR events for TAB pressed -
           normally, TAB is used for passing to the next control in a dialog
           instead. For the control created with this style, you can still use
           Ctrl-Enter to pass to the next control from the keyboard.
    @style{wxTE_MULTILINE}:
           The text control allows multiple lines.
    @style{wxTE_PASSWORD}:
           The text will be echoed as asterisks.
    @style{wxTE_READONLY}:
           The text will not be user-editable.
    @style{wxTE_RICH}:
           Use rich text control under Win32, this allows to have more than
           64KB of text in the control even under Win9x. This style is ignored
           under other platforms.
    @style{wxTE_RICH2}:
           Use rich text control version 2.0 or 3.0 under Win32, this style is
           ignored under other platforms
    @style{wxTE_AUTO_URL}:
           Highlight the URLs and generate the wxTextUrlEvents when mouse
           events occur over them. This style is only supported for wxTE_RICH
           Win32 and multi-line wxGTK2 text controls.
    @style{wxTE_NOHIDESEL}:
           By default, the Windows text control doesn't show the selection
           when it doesn't have focus - use this style to force it to always
           show it. It doesn't do anything under other platforms.
    @style{wxHSCROLL}:
           A horizontal scrollbar will be created and used, so that text won't
           be wrapped. No effect under wxGTK1.
    @style{wxTE_NO_VSCROLL}:
           For multiline controls only: vertical scrollbar will never be
           created. This limits the amount of text which can be entered into
           the control to what can be displayed in it under MSW but not under
           GTK2. Currently not implemented for the other platforms.
    @style{wxTE_LEFT}:
           The text in the control will be left-justified (default).
    @style{wxTE_CENTRE}:
           The text in the control will be centered (currently wxMSW and
           wxGTK2 only).
    @style{wxTE_RIGHT}:
           The text in the control will be right-justified (currently wxMSW
           and wxGTK2 only).
    @style{wxTE_DONTWRAP}:
           Same as wxHSCROLL style: don't wrap at all, show horizontal
           scrollbar instead.
    @style{wxTE_CHARWRAP}:
           Wrap the lines too long to be shown entirely at any position
           (wxUniv and wxGTK2 only).
    @style{wxTE_WORDWRAP}:
           Wrap the lines too long to be shown entirely at word boundaries
           (wxUniv and wxGTK2 only).
    @style{wxTE_BESTWRAP}:
           Wrap the lines at word boundaries or at any other character if
           there are words longer than the window width (this is the default).
    @style{wxTE_CAPITALIZE}:
           On PocketPC and Smartphone, causes the first letter to be
           capitalized.
    @endStyleTable

    @library{wxcore}
    @category{ctrl}
    @appearance{textctrl.png}

    @seealso
    wxTextCtrl::Create, wxValidator
*/
class wxTextCtrl : public wxControl
{
public:
    //@{
    /**
        Constructor, creating and showing a text control.
        
        @param parent
            Parent window. Should not be @NULL.
        @param id
            Control identifier. A value of -1 denotes a default value.
        @param value
            Default text value.
        @param pos
            Text control position.
        @param size
            Text control size.
        @param style
            Window style. See wxTextCtrl.
        @param validator
            Window validator.
        @param name
            Window name.
        
        @remarks The horizontal scrollbar (wxHSCROLL style flag) will only be
                 created for multi-line text controls. Without a
                 horizontal scrollbar, text lines that don't fit in the
                 control's size will be wrapped (but no newline
                 character is inserted). Single line controls don't have
                 a horizontal scrollbar, the text is automatically
                 scrolled so that the insertion point is always visible.
        
        @see Create(), wxValidator
    */
    wxTextCtrl();
    wxTextCtrl(wxWindow* parent, wxWindowID id,
               const wxString& value = "",
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxTextCtrlNameStr);
    //@}

    /**
        Destructor, destroying the text control.
    */
    ~wxTextCtrl();

    /**
        Appends the text to the end of the text control.
        
        @param text
            Text to write to the text control.
        
        @remarks After the text is appended, the insertion point will be at the
                 end of the text control. If this behaviour is not
                 desired, the programmer should use GetInsertionPoint
                 and SetInsertionPoint.
        
        @see WriteText()
    */
    void AppendText(const wxString& text);

    /**
        Call this function to enable auto-completion of the text typed in a single-line
        text control using the given @e choices.
        Notice that currently this function is only implemented in wxGTK2 and wxMSW
        ports and does nothing under the other platforms.
        This function is new since wxWidgets version 2.9.0
        
        @returns @true if the auto-completion was enabled or @false if the
                 operation failed, typically because auto-completion is
                 not supported by the current platform.
        
        @see AutoCompleteFileNames()
    */
    bool AutoComplete(const wxArrayString& choices);

    /**
        Call this function to enable auto-completion of the text typed in a single-line
        text control using all valid file system paths.
        Notice that currently this function is only implemented in wxGTK2 port and does
        nothing under the other platforms.
        This function is new since wxWidgets version 2.9.0
        
        @returns @true if the auto-completion was enabled or @false if the
                 operation failed, typically because auto-completion is
                 not supported by the current platform.
        
        @see AutoComplete()
    */
    bool AutoCompleteFileNames();

    /**
        Returns @true if the selection can be copied to the clipboard.
    */
    virtual bool CanCopy();

    /**
        Returns @true if the selection can be cut to the clipboard.
    */
    virtual bool CanCut();

    /**
        Returns @true if the contents of the clipboard can be pasted into the
        text control. On some platforms (Motif, GTK) this is an approximation
        and returns @true if the control is editable, @false otherwise.
    */
    virtual bool CanPaste();

    /**
        Returns @true if there is a redo facility available and the last operation
        can be redone.
    */
    virtual bool CanRedo();

    /**
        Returns @true if there is an undo facility available and the last operation
        can be undone.
    */
    virtual bool CanUndo();

    /**
        Sets the text value and marks the control as not-modified (which means that
        IsModified() would return @false immediately
        after the call to SetValue).
        Note that this function will not generate the @c wxEVT_COMMAND_TEXT_UPDATED
        event.
        This is the only difference with SetValue().
        See @ref overview_progevent "this topic" for more information.
        This function is new since wxWidgets version 2.7.1
        
        @param value
            The new value to set. It may contain newline characters if the text control
        is multi-line.
    */
    virtual void ChangeValue(const wxString& value);

    /**
        Clears the text in the control.
        Note that this function will generate a @c wxEVT_COMMAND_TEXT_UPDATED
        event.
    */
    virtual void Clear();

    /**
        Copies the selected text to the clipboard under Motif and MS Windows.
    */
    virtual void Copy();

    /**
        Creates the text control for two-step construction. Derived classes
        should call or replace this function. See wxTextCtrl()
        for further details.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& value = "",
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxTextCtrlNameStr);

    /**
        Copies the selected text to the clipboard and removes the selection.
    */
    virtual void Cut();

    /**
        Resets the internal 'modified' flag as if the current edits had been saved.
    */
    void DiscardEdits();

    /**
        This functions inserts into the control the character which would have been
        inserted if the given key event had occurred in the text control. The
        @a event object should be the same as the one passed to @c EVT_KEY_DOWN
        handler previously by wxWidgets.
        Please note that this function doesn't currently work correctly for all keys
        under any platform but MSW.
        
        @returns @true if the event resulted in a change to the control, @false
                 otherwise.
    */
    bool EmulateKeyPress(const wxKeyEvent& event);

    /**
        Returns the style currently used for the new text.
        
        @see SetDefaultStyle()
    */
    const wxTextAttr GetDefaultStyle();

    /**
        Returns the insertion point. This is defined as the zero based index of the
        character position to the right of the insertion point. For example, if
        the insertion point is at the end of the text control, it is equal to
        both GetValue().Length() and
        GetLastPosition().
        The following code snippet safely returns the character at the insertion
        point or the zero character if the point is at the end of the control.
    */
    virtual long GetInsertionPoint();

    /**
        Returns the zero based index of the last position in the text control,
        which is equal to the number of characters in the control.
    */
    virtual wxTextPos GetLastPosition();

    /**
        Gets the length of the specified line, not including any trailing newline
        character(s).
        
        @param lineNo
            Line number (starting from zero).
        
        @returns The length of the line, or -1 if lineNo was invalid.
    */
    int GetLineLength(long lineNo);

    /**
        Returns the contents of a given line in the text control, not including
        any trailing newline character(s).
        
        @param lineNo
            The line number, starting from zero.
        
        @returns The contents of the line.
    */
    wxString GetLineText(long lineNo);

    /**
        Returns the number of lines in the text control buffer.
        
        @remarks Note that even empty text controls have one line (where the
                 insertion point is), so GetNumberOfLines() never
                 returns 0.
    */
    int GetNumberOfLines();

    /**
        Returns the string containing the text starting in the positions @a from and
        up to @a to in the control. The positions must have been returned by another
        wxTextCtrl method.
        Please note that the positions in a multiline wxTextCtrl do @b not
        correspond to the indices in the string returned by
        GetValue() because of the different new line
        representations (@c CR or @c CR LF) and so this method should be used to
        obtain the correct results instead of extracting parts of the entire value. It
        may also be more efficient, especially if the control contains a lot of data.
    */
    virtual wxString GetRange(long from, long to);

    /**
        Gets the current selection span. If the returned values are equal, there was
        no selection.
        Please note that the indices returned may be used with the other wxTextctrl
        methods but don't necessarily represent the correct indices into the string
        returned by GetValue() for multiline controls
        under Windows (at least,) you should use
        GetStringSelection() to get the selected
        text.
        
        @param from
            The returned first position.
        @param to
            The returned last position.
    */
    virtual void GetSelection(long* from, long* to);

    /**
        Gets the text currently selected in the control. If there is no selection, the
        returned string is empty.
    */
    virtual wxString GetStringSelection();

    /**
        Returns the style at this position in the text control. Not all platforms
        support this function.
        
        @returns @true on success, @false if an error occurred - it may also mean
                 that the styles are not supported under this platform.
        
        @see SetStyle(), wxTextAttr
    */
    bool GetStyle(long position, wxTextAttr& style);

    /**
        Gets the contents of the control. Notice that for a multiline text control,
        the lines will be separated by (Unix-style) \n characters, even
        under Windows where they are separated by a \r\n
        sequence in the native control.
    */
    wxString GetValue();

    /**
        This function finds the character at the specified position expressed in
        pixels. If the return code is not @c wxTE_HT_UNKNOWN the row and column
        of the character closest to this position are returned in the @a col and
        @a row parameters (unless the pointers are @NULL which is allowed).
        Please note that this function is currently only implemented in wxUniv,
        wxMSW and wxGTK2 ports.
        
        @see PositionToXY(), XYToPosition()
    */
    wxTextCtrlHitTestResult HitTest(const wxPoint& pt,
                                    wxTextCoord col,
                                    wxTextCoord row);

    /**
        Returns @true if the controls contents may be edited by user (note that it
        always can be changed by the program), i.e. if the control hasn't been put in
        read-only mode by a previous call to
        SetEditable().
    */
    bool IsEditable();

    /**
        Returns @true if the control is currently empty. This is the same as
        @c GetValue().empty() but can be much more efficient for the multiline
        controls containing big amounts of text.
        This function is new since wxWidgets version 2.7.1
    */
    bool IsEmpty();

    /**
        Returns @true if the text has been modified by user. Note that calling
        SetValue() doesn't make the control modified.
        
        @see MarkDirty()
    */
    bool IsModified();

    /**
        Returns @true if this is a multi line edit control and @false
        otherwise.
        
        @see IsSingleLine()
    */
    bool IsMultiLine();

    /**
        Returns @true if this is a single line edit control and @false
        otherwise.
        
        @see @ref issingleline() IsMultiLine
    */
    bool IsSingleLine();

    /**
        Loads and displays the named file, if it exists.
        
        @param filename
            The filename of the file to load.
        @param fileType
            The type of file to load. This is currently ignored in wxTextCtrl.
        
        @returns @true if successful, @false otherwise.
    */
    bool LoadFile(const wxString& filename,
                  int fileType = wxTEXT_TYPE_ANY);

    /**
        Mark text as modified (dirty).
        
        @see IsModified()
    */
    void MarkDirty();

    /**
        This event handler function implements default drag and drop behaviour, which
        is to load the first dropped file into the control.
        
        @param event
            The drop files event.
        
        @remarks This is not implemented on non-Windows platforms.
        
        @see wxDropFilesEvent
    */
    void OnDropFiles(wxDropFilesEvent& event);

    /**
        Pastes text from the clipboard to the text item.
    */
    virtual void Paste();

    /**
        Converts given position to a zero-based column, line number pair.
        
        @param pos
            Position.
        @param x
            Receives zero based column number.
        @param y
            Receives zero based line number.
        
        @returns @true on success, @false on failure (most likely due to a too
                 large position parameter).
        
        @see XYToPosition()
    */
    bool PositionToXY(long pos, long* x, long* y);

    /**
        If there is a redo facility and the last operation can be redone, redoes the
        last operation. Does nothing
        if there is no redo facility.
    */
    virtual void Redo();

    /**
        Removes the text starting at the first given position up to (but not including)
        the character at the last position.
        
        @param from
            The first position.
        @param to
            The last position.
    */
    virtual void Remove(long from, long to);

    /**
        Replaces the text starting at the first position up to (but not including)
        the character at the last position with the given text.
        
        @param from
            The first position.
        @param to
            The last position.
        @param value
            The value to replace the existing text with.
    */
    virtual void Replace(long from, long to, const wxString& value);

    /**
        Saves the contents of the control in a text file.
        
        @param filename
            The name of the file in which to save the text.
        @param fileType
            The type of file to save. This is currently ignored in wxTextCtrl.
        
        @returns @true if the operation was successful, @false otherwise.
    */
    bool SaveFile(const wxString& filename,
                  int fileType = wxTEXT_TYPE_ANY);

    /**
        Changes the default style to use for the new text which is going to be added
        to the control using WriteText() or
        AppendText().
        If either of the font, foreground, or background colour is not set in
        @e style, the values of the previous default style are used for them. If
        the previous default style didn't set them neither, the global font or colours
        of the text control itself are used as fall back.
        However if the @a style parameter is the default wxTextAttr, then the
        default style is just reset (instead of being combined with the new style which
        wouldn't change it at all).
        
        @param style
            The style for the new text.
        
        @returns @true on success, @false if an error occurred - may also mean that
                 the styles are not supported under this platform.
        
        @see GetDefaultStyle()
    */
    bool SetDefaultStyle(const wxTextAttr& style);

    /**
        Makes the text item editable or read-only, overriding the @b wxTE_READONLY flag.
        
        @param editable
            If @true, the control is editable. If @false, the control is read-only.
        
        @see IsEditable()
    */
    virtual void SetEditable(const bool editable);

    /**
        Sets the insertion point at the given position.
        
        @param pos
            Position to set.
    */
    virtual void SetInsertionPoint(long pos);

    /**
        Sets the insertion point at the end of the text control. This is equivalent
        to wxTextCtrl::SetInsertionPoint(wxTextCtrl::GetLastPosition()).
    */
    virtual void SetInsertionPointEnd();

    /**
        This function sets the maximum number of characters the user can enter into the
        control. In other words, it allows to limit the text value length to @e len
        not counting the terminating @c NUL character.
        If @a len is 0, the previously set max length limit, if any, is discarded
        and the user may enter as much text as the underlying native text control
        widget supports (typically at least 32Kb).
        If the user tries to enter more characters into the text control when it
        already is filled up to the maximal length, a
        @c wxEVT_COMMAND_TEXT_MAXLEN event is sent to notify the program about it
        (giving it the possibility to show an explanatory message, for example) and the
        extra input is discarded.
        Note that under GTK+, this function may only be used with single line text
        controls.
    */
    virtual void SetMaxLength(unsigned long len);

    /**
        Marks the control as being modified by the user or not.
        
        @see MarkDirty(), DiscardEdits()
    */
    void SetModified(bool modified);

    /**
        Selects the text starting at the first position up to (but not including) the
        character at the last position. If both parameters are equal to -1 all text
        in the control is selected.
        
        @param from
            The first position.
        @param to
            The last position.
    */
    virtual void SetSelection(long from, long to);

    /**
        Changes the style of the given range. If any attribute within @a style is
        not set, the corresponding attribute from GetDefaultStyle() is used.
        
        @param start
            The start of the range to change.
        @param end
            The end of the range to change.
        @param style
            The new style for the range.
        
        @returns @true on success, @false if an error occurred - it may also mean
                 that the styles are not supported under this platform.
        
        @see GetStyle(), wxTextAttr
    */
    bool SetStyle(long start, long end, const wxTextAttr& style);

    /**
        Sets the text value and marks the control as not-modified (which means that
        IsModified() would return @false immediately
        after the call to SetValue).
        Note that this function will generate a @c wxEVT_COMMAND_TEXT_UPDATED
        event.
        This function is deprecated and should not be used in new code. Please use the
        ChangeValue() function instead.
        
        @param value
            The new value to set. It may contain newline characters if the text control
        is multi-line.
    */
    virtual void SetValue(const wxString& value);

    /**
        Makes the line containing the given position visible.
        
        @param pos
            The position that should be visible.
    */
    void ShowPosition(long pos);

    /**
        If there is an undo facility and the last operation can be undone, undoes the
        last operation. Does nothing
        if there is no undo facility.
    */
    virtual void Undo();

    /**
        Writes the text into the text control at the current insertion position.
        
        @param text
            Text to write to the text control.
        
        @remarks Newlines in the text string are the only control characters
                 allowed, and they will cause appropriate line breaks.
                 See () and AppendText() for more
                 convenient ways of writing to the window.
    */
    void WriteText(const wxString& text);

    /**
        Converts the given zero based column and line number to a position.
        
        @param x
            The column number.
        @param y
            The line number.
        
        @returns The position value, or -1 if x or y was invalid.
    */
    long XYToPosition(long x, long y);

    //@{
    /**
        Operator definitions for appending to a text control, for example:
    */
    wxTextCtrl operator(const wxString& s);
    wxTextCtrl operator(int i);
    wxTextCtrl operator(long i);
    wxTextCtrl operator(float f);
    wxTextCtrl operator(double d);
    wxTextCtrl operator(char c);
    //@}
};


/**
    @class wxStreamToTextRedirector
    @wxheader{textctrl.h}

    This class can be used to (temporarily) redirect all output sent to a C++
    ostream object to a wxTextCtrl instead.

    @b NB: Some compilers and/or build configurations don't support multiply
    inheriting wxTextCtrl from @c std::streambuf in which
    case this class is not compiled in. You also must have @c wxUSE_STD_IOSTREAM
    option on (i.e. set to 1) in your setup.h to be able to use it. Under Unix,
    specify @c --enable-std_iostreams switch when running configure for this.

    Example of usage:

    @code
    using namespace std;

      wxTextCtrl *text = new wxTextCtrl(...);

      {
        wxStreamToTextRedirector redirect(text);

        // this goes to the text control
        cout  "Hello, text!"  endl;
      }

      // this goes somewhere else, presumably to stdout
      cout  "Hello, console!"  endl;
    @endcode


    @library{wxcore}
    @category{logging}

    @seealso
    wxTextCtrl
*/
class wxStreamToTextRedirector
{
public:
    /**
        The constructor starts redirecting output sent to @a ostr or @e cout for
        the default parameter value to the text control @e text.
        
        @param text
            The text control to append output too, must be non-@NULL
        @param ostr
            The C++ stream to redirect, cout is used if it is @NULL
    */
    wxStreamToTextRedirector(wxTextCtrl text, ostream* ostr = NULL);

    /**
        When a wxStreamToTextRedirector object is destroyed, the redirection is ended
        and any output sent to the C++ ostream which had been specified at the time of
        the object construction will go to its original destination.
    */
    ~wxStreamToTextRedirector();
};
