/////////////////////////////////////////////////////////////////////////////
// Name:        richtext/richtextbuffer.h
// Purpose:     documentation for wxRichTextBuffer class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxRichTextBuffer
    @headerfile richtextbuffer.h wx/richtext/richtextbuffer.h

    This class represents the whole buffer associated with a wxRichTextCtrl.

    @library{wxrichtext}
    @category{FIXME}

    @seealso
    wxTextAttr, wxRichTextCtrl
*/
class wxRichTextBuffer
{
public:
    //@{
    /**
        Default constructors.
    */
    wxRichTextBuffer(const wxRichTextBuffer& obj);
    wxRichTextBuffer();
    //@}

    /**
        Destructor.
    */
    ~wxRichTextBuffer();

    /**
        Adds an event handler to the buffer's list of handlers. A buffer associated with
        a contol has the control as the only event handler, but the application is free
        to add more if further notification is required. All handlers are notified
        of an event originating from the buffer, such as the replacement of a style
        sheet
        during loading. The buffer never deletes any of the event handlers, unless
        RemoveEventHandler() is
        called with @true as the second argument.
    */
    bool AddEventHandler(wxEvtHandler* handler);

    /**
        Adds a file handler.
    */
    void AddHandler(wxRichTextFileHandler* handler);

    /**
        Adds a paragraph of text.
    */
    wxRichTextRange AddParagraph(const wxString& text);

    /**
        Returns @true if the buffer is currently collapsing commands into a single
        notional command.
    */
    bool BatchingUndo();

    /**
        Begins using alignment.
    */
    bool BeginAlignment(wxTextAttrAlignment alignment);

    /**
        Begins collapsing undo/redo commands. Note that this may not work properly
        if combining commands that delete or insert content, changing ranges for
        subsequent actions.
        
        @e cmdName should be the name of the combined command that will appear
        next to Undo and Redo in the edit menu.
    */
    bool BeginBatchUndo(const wxString& cmdName);

    /**
        Begin applying bold.
    */
    bool BeginBold();

    /**
        Begins applying the named character style.
    */
    bool BeginCharacterStyle(const wxString& characterStyle);

    /**
        Begins using this font.
    */
    bool BeginFont(const wxFont& font);

    /**
        Begins using the given point size.
    */
    bool BeginFontSize(int pointSize);

    /**
        Begins using italic.
    */
    bool BeginItalic();

    /**
        Begin using @e leftIndent for the left indent, and optionally @e leftSubIndent
        for
        the sub-indent. Both are expressed in tenths of a millimetre.
        
        The sub-indent is an offset from the left of the paragraph, and is used for all
        but the
        first line in a paragraph. A positive value will cause the first line to appear
        to the left
        of the subsequent lines, and a negative value will cause the first line to be
        indented
        relative to the subsequent lines.
    */
    bool BeginLeftIndent(int leftIndent, int leftSubIndent = 0);

    /**
        Begins line spacing using the specified value. @e spacing is a multiple, where
        10 means single-spacing,
        15 means 1.5 spacing, and 20 means double spacing. The following constants are
        defined for convenience:
    */
    bool BeginLineSpacing(int lineSpacing);

    /**
        Begins using a specified list style. Optionally, you can also pass a level and
        a number.
    */
    bool BeginListStyle(const wxString& listStyle, int level=1,
                        int number=1);

    /**
        Begins a numbered bullet. This call will be needed for each item in the list,
        and the
        application should take care of incrementing the numbering.
        
        @e bulletNumber is a number, usually starting with 1.
        
        @e leftIndent and @e leftSubIndent are values in tenths of a millimetre.
        
        @e bulletStyle is a bitlist of the following values:
        
        
        wxRichTextBuffer uses indentation to render a bulleted item. The left indent is
        the distance between
        the margin and the bullet. The content of the paragraph, including the first
        line, starts
        at leftMargin + leftSubIndent. So the distance between the left edge of the
        bullet and the
        left of the actual paragraph is leftSubIndent.
    */
    bool BeginNumberedBullet(int bulletNumber, int leftIndent,
                             int leftSubIndent,
                             int bulletStyle = wxTEXT_ATTR_BULLET_STYLE_ARABIC|wxTEXT_ATTR_BULLET_STYLE_PERIOD);

    /**
        Begins paragraph spacing; pass the before-paragraph and after-paragraph spacing
        in tenths of
        a millimetre.
    */
    bool BeginParagraphSpacing(int before, int after);

    /**
        Begins applying the named paragraph style.
    */
    bool BeginParagraphStyle(const wxString& paragraphStyle);

    /**
        Begins a right indent, specified in tenths of a millimetre.
    */
    bool BeginRightIndent(int rightIndent);

    /**
        Begins applying a standard bullet, using one of the standard bullet names
        (currently @c standard/circle or @c standard/square.
        See BeginNumberedBullet() for an explanation of how indentation is used to
        render the bulleted paragraph.
    */
    bool BeginStandardBullet(const wxString& bulletName,
                             int leftIndent,
                             int leftSubIndent,
                             int bulletStyle = wxTEXT_ATTR_BULLET_STYLE_STANDARD);

    /**
        Begins using a specified style.
    */
    bool BeginStyle(const wxTextAttr& style);

    /**
        Begins suppressing undo/redo commands. The way undo is suppressed may be
        implemented
        differently by each command. If not dealt with by a command implementation, then
        it will be implemented automatically by not storing the command in the undo
        history
        when the action is submitted to the command processor.
    */
    bool BeginSuppressUndo();

    /**
        Begins applying a symbol bullet, using a character from the current font. See
        BeginNumberedBullet() for
        an explanation of how indentation is used to render the bulleted paragraph.
    */
    bool BeginSymbolBullet(wxChar symbol, int leftIndent,
                           int leftSubIndent,
                           int bulletStyle = wxTEXT_ATTR_BULLET_STYLE_SYMBOL);

    /**
        Begins using the specified text foreground colour.
    */
    bool BeginTextColour(const wxColour& colour);

    /**
        Begins applying wxTEXT_ATTR_URL to the content. Pass a URL and optionally, a
        character style to apply,
        since it is common to mark a URL with a familiar style such as blue text with
        underlining.
    */
    bool BeginURL(const wxString& url,
                  const wxString& characterStyle = wxEmptyString);

    /**
        Begins using underline.
    */
    bool BeginUnderline();

    /**
        Returns @true if content can be pasted from the clipboard.
    */
    bool CanPasteFromClipboard();

    /**
        Cleans up the file handlers.
    */
    void CleanUpHandlers();

    /**
        Clears the buffer.
    */
    void Clear();

    //@{
    /**
        Clears the list style from the given range, clearing list-related attributes
        and applying any named paragraph style associated with each paragraph.
        
        @e flags is a bit list of the following:
        
         wxRICHTEXT_SETSTYLE_WITH_UNDO: specifies that this command will be undoable.
        
        See also SetListStyle(), PromoteList(), NumberList().
    */
    bool ClearListStyle(const wxRichTextRange& range,
                        int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO);
    bool ClearListStyle(const wxRichTextRange& range,
                        int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO);
    //@}

    /**
        Clears the style stack.
    */
    void ClearStyleStack();

    /**
        Clones the object.
    */
    wxRichTextObject* Clone();

    /**
        Copies the given buffer.
    */
    void Copy(const wxRichTextBuffer& obj);

    /**
        Copy the given range to the clipboard.
    */
    bool CopyToClipboard(const wxRichTextRange& range);

    /**
        Submits a command to delete the given range.
    */
    bool DeleteRangeWithUndo(const wxRichTextRange& range,
                             wxRichTextCtrl* ctrl);

    //@{
    /**
        Dumps the contents of the buffer for debugging purposes.
    */
    void Dump();
    void Dump(wxTextOutputStream& stream);
    //@}

    /**
        Ends alignment.
    */
    bool EndAlignment();

    /**
        Ends all styles that have been started with a Begin... command.
    */
    bool EndAllStyles();

    /**
        Ends collapsing undo/redo commands, and submits the combined command.
    */
    bool EndBatchUndo();

    /**
        Ends using bold.
    */
    bool EndBold();

    /**
        Ends using the named character style.
    */
    bool EndCharacterStyle();

    /**
        Ends using a font.
    */
    bool EndFont();

    /**
        Ends using a point size.
    */
    bool EndFontSize();

    /**
        Ends using italic.
    */
    bool EndItalic();

    /**
        Ends using a left indent.
    */
    bool EndLeftIndent();

    /**
        Ends using a line spacing.
    */
    bool EndLineSpacing();

    /**
        Ends using a specified list style.
    */
    bool EndListStyle();

    /**
        Ends a numbered bullet.
    */
    bool EndNumberedBullet();

    /**
        Ends paragraph spacing.
    */
    bool EndParagraphSpacing();

    /**
        Ends applying a named character style.
    */
    bool EndParagraphStyle();

    /**
        Ends using a right indent.
    */
    bool EndRightIndent();

    /**
        Ends using a standard bullet.
    */
    bool EndStandardBullet();

    /**
        Ends the current style.
    */
    bool EndStyle();

    /**
        Ends suppressing undo/redo commands.
    */
    bool EndSuppressUndo();

    /**
        Ends using a symbol bullet.
    */
    bool EndSymbolBullet();

    /**
        Ends using a text foreground colour.
    */
    bool EndTextColour();

    /**
        Ends applying a URL.
    */
#define bool EndURL()     /* implementation is private */

    /**
        Ends using underline.
    */
    bool EndUnderline();

    //@{
    /**
        Finds a handler by name.
    */
    wxRichTextFileHandler* FindHandler(int imageType);
    wxRichTextFileHandler* FindHandler(const wxString& extension,
                                       int imageType);
    wxRichTextFileHandler* FindHandler(const wxString& name);
    //@}

    /**
        Finds a handler by filename or, if supplied, type.
    */
    wxRichTextFileHandler* FindHandlerFilenameOrType(const wxString& filename,
            int imageType);

    /**
        Gets the basic (overall) style. This is the style of the whole
        buffer before further styles are applied, unlike the default style, which
        only affects the style currently being applied (for example, setting the default
        style to bold will cause subsequently inserted text to be bold).
    */
    const wxTextAttr GetBasicStyle();

    /**
        Gets the collapsed command.
    */
    wxRichTextCommand* GetBatchedCommand();

    /**
        Gets the command processor. A text buffer always creates its own command
        processor when it is
        initialized.
    */
    wxCommandProcessor* GetCommandProcessor();

    /**
        Returns the current default style, affecting the style currently being applied
        (for example, setting the default
        style to bold will cause subsequently inserted text to be bold).
    */
    const wxTextAttr GetDefaultStyle();

    /**
        Gets a wildcard incorporating all visible handlers. If @e types is present,
        it will be filled with the file type corresponding to each filter. This can be
        used to determine the type to pass to @ref getextwildcard() LoadFile given a
        selected filter.
    */
    wxString GetExtWildcard(bool combine = @false, bool save = @false,
                            wxArrayInt* types = @NULL);

    /**
        Returns the list of file handlers.
    */
    wxList GetHandlers();

    /**
        Returns the object to be used to render certain aspects of the content, such as
        bullets.
    */
    static wxRichTextRenderer* GetRenderer();

    /**
        Gets the attributes at the given position.
        
        This function gets the combined style - that is, the style you see on the
        screen as a result
        of combining base style, paragraph style and character style attributes. To get
        the character
        or paragraph style alone, use GetUncombinedStyle().
    */
    bool GetStyle(long position, wxTextAttr& style);

    /**
        This function gets a style representing the common, combined attributes in the
        given range.
        Attributes which have different values within the specified range will not be
        included the style
        flags.
        
        The function is used to get the attributes to display in the formatting dialog:
        the user
        can edit the attributes common to the selection, and optionally specify the
        values of further
        attributes to be applied uniformly.
        
        To apply the edited attributes, you can use SetStyle() specifying
        the wxRICHTEXT_SETSTYLE_OPTIMIZE flag, which will only apply attributes that
        are different
        from the @e combined attributes within the range. So, the user edits the
        effective, displayed attributes
        for the range, but his choice won't be applied unnecessarily to content. As an
        example,
        say the style for a paragraph specifies bold, but the paragraph text doesn't
        specify a weight. The
        combined style is bold, and this is what the user will see on-screen and in the
        formatting
        dialog. The user now specifies red text, in addition to bold. When applying with
        SetStyle, the content font weight attributes won't be changed to bold because
        this is already specified
        by the paragraph. However the text colour attributes @e will be changed to
        show red.
    */
    bool GetStyleForRange(const wxRichTextRange& range,
                          wxTextAttr& style);

    /**
        Returns the current style sheet associated with the buffer, if any.
    */
    wxRichTextStyleSheet* GetStyleSheet();

    /**
        Get the size of the style stack, for example to check correct nesting.
    */
    size_t GetStyleStackSize();

    /**
        Gets the attributes at the given position.
        
        This function gets the @e uncombined style - that is, the attributes associated
        with the
        paragraph or character content, and not necessarily the combined attributes you
        see on the
        screen. To get the combined attributes, use GetStyle().
        
        If you specify (any) paragraph attribute in @e style's flags, this function
        will fetch
        the paragraph attributes. Otherwise, it will return the character attributes.
    */
    bool GetUncombinedStyle(long position, wxTextAttr& style);

    /**
        Finds the text position for the given position, putting the position in @e
        textPosition if
        one is found. @e pt is in logical units (a zero y position is
        at the beginning of the buffer).
        
        The function returns one of the following values:
    */
    int HitTest(wxDC& dc, const wxPoint& pt, long& textPosition);

    /**
        Initialisation.
    */
    void Init();

    /**
        Initialises the standard handlers. Currently, only the plain text
        loading/saving handler
        is initialised by default.
    */
    void InitStandardHandlers();

    /**
        Inserts a handler at the front of the list.
    */
    void InsertHandler(wxRichTextFileHandler* handler);

    /**
        Submits a command to insert the given image.
    */
    bool InsertImageWithUndo(long pos,
                             const wxRichTextImageBlock& imageBlock,
                             wxRichTextCtrl* ctrl);

    /**
        Submits a command to insert a newline.
    */
    bool InsertNewlineWithUndo(long pos, wxRichTextCtrl* ctrl);

    /**
        Submits a command to insert the given text.
    */
    bool InsertTextWithUndo(long pos, const wxString& text,
                            wxRichTextCtrl* ctrl);

    /**
        Returns @true if the buffer has been modified.
    */
    bool IsModified();

    //@{
    /**
        Loads content from a file.
    */
    bool LoadFile(wxInputStream& stream,
                  int type = wxRICHTEXT_TYPE_ANY);
    bool LoadFile(const wxString& filename,
                  int type = wxRICHTEXT_TYPE_ANY);
    //@}

    /**
        Marks the buffer as modified or unmodified.
    */
    void Modify(bool modify = @true);

    //@{
    /**
        Numbers the paragraphs in the given range. Pass flags to determine how the
        attributes are set.
        Either the style definition or the name of the style definition (in the current
        sheet) can be passed.
        
        @e flags is a bit list of the following:
        
         wxRICHTEXT_SETSTYLE_WITH_UNDO: specifies that this command will be undoable.
         wxRICHTEXT_SETSTYLE_RENUMBER: specifies that numbering should start from @e
        startFrom, otherwise existing attributes are used.
         wxRICHTEXT_SETSTYLE_SPECIFY_LEVEL: specifies that @e listLevel should be used
        as the level for all paragraphs, otherwise the current indentation will be used.
        
        See also SetListStyle(), PromoteList(), ClearListStyle().
    */
    bool NumberList(const wxRichTextRange& range,
                    const wxRichTextListStyleDefinition* style,
                    int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO,
                    int startFrom = -1,
                    int listLevel = -1);
    bool Number(const wxRichTextRange& range,
                const wxString& styleName,
                int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO,
                int startFrom = -1,
                int listLevel = -1);
    //@}

    /**
        Pastes the clipboard content to the buffer at the given position.
    */
    bool PasteFromClipboard(long position);

    //@{
    /**
        Promotes or demotes the paragraphs in the given range. A positive @e promoteBy
        produces a smaller indent, and a negative number
        produces a larger indent. Pass flags to determine how the attributes are set.
        Either the style definition or the name of the style definition (in the current
        sheet) can be passed.
        
        @e flags is a bit list of the following:
        
         wxRICHTEXT_SETSTYLE_WITH_UNDO: specifies that this command will be undoable.
         wxRICHTEXT_SETSTYLE_RENUMBER: specifies that numbering should start from @e
        startFrom, otherwise existing attributes are used.
         wxRICHTEXT_SETSTYLE_SPECIFY_LEVEL: specifies that @e listLevel should be used
        as the level for all paragraphs, otherwise the current indentation will be used.
        
        See also SetListStyle(), See also SetListStyle(), ClearListStyle().
    */
    bool PromoteList(int promoteBy, const wxRichTextRange& range,
                     const wxRichTextListStyleDefinition* style,
                     int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO,
                     int listLevel = -1);
    bool PromoteList(int promoteBy, const wxRichTextRange& range,
                     const wxString& styleName,
                     int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO,
                     int listLevel = -1);
    //@}

    /**
        Removes an event handler from the buffer's list of handlers, deleting the
        object if @e deleteHandler is @true.
    */
    bool RemoveEventHandler(wxEvtHandler* handler,
                            bool deleteHandler = @false);

    /**
        Removes a handler.
    */
    bool RemoveHandler(const wxString& name);

    /**
        Clears the buffer, adds a new blank paragraph, and clears the command history.
    */
    void ResetAndClearCommands();

    //@{
    /**
        Saves content to a file.
    */
    bool SaveFile(wxOutputStream& stream,
                  int type = wxRICHTEXT_TYPE_ANY);
    bool SaveFile(const wxString& filename,
                  int type = wxRICHTEXT_TYPE_ANY);
    //@}

    /**
        Sets the basic (overall) style. This is the style of the whole
        buffer before further styles are applied, unlike the default style, which
        only affects the style currently being applied (for example, setting the default
        style to bold will cause subsequently inserted text to be bold).
    */
    void SetBasicStyle(const wxTextAttr& style);

    /**
        Sets the default style, affecting the style currently being applied (for
        example, setting the default
        style to bold will cause subsequently inserted text to be bold).
        
        This is not cumulative - setting the default style will replace the previous
        default style.
    */
    void SetDefaultStyle(const wxTextAttr& style);

    //@{
    /**
        Sets the list attributes for the given range, passing flags to determine how
        the attributes are set.
        Either the style definition or the name of the style definition (in the current
        sheet) can be passed.
        
        @e flags is a bit list of the following:
        
         wxRICHTEXT_SETSTYLE_WITH_UNDO: specifies that this command will be undoable.
         wxRICHTEXT_SETSTYLE_RENUMBER: specifies that numbering should start from @e
        startFrom, otherwise existing attributes are used.
         wxRICHTEXT_SETSTYLE_SPECIFY_LEVEL: specifies that @e listLevel should be used
        as the level for all paragraphs, otherwise the current indentation will be used.
        
        See also NumberList(), PromoteList(), ClearListStyle().
    */
    bool SetListStyle(const wxRichTextRange& range,
                      const wxRichTextListStyleDefinition* style,
                      int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO,
                      int startFrom = -1,
                      int listLevel = -1);
    bool SetListStyle(const wxRichTextRange& range,
                      const wxString& styleName,
                      int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO,
                      int startFrom = -1,
                      int listLevel = -1);
    //@}

    /**
        Sets @e renderer as the object to be used to render certain aspects of the
        content, such as bullets.
        You can override default rendering by deriving a new class from
        wxRichTextRenderer or wxRichTextStdRenderer,
        overriding one or more virtual functions, and setting an instance of the class
        using this function.
    */
    static void SetRenderer(wxRichTextRenderer* renderer);

    /**
        Sets the attributes for the given range. Pass flags to determine how the
        attributes are set.
        
        The end point of range is specified as the last character position of the span
        of text.
        So, for example, to set the style for a character at position 5, use the range
        (5,5).
        This differs from the wxRichTextCtrl API, where you would specify (5,6).
        
        @e flags may contain a bit list of the following values:
        
         wxRICHTEXT_SETSTYLE_NONE: no style flag.
         wxRICHTEXT_SETSTYLE_WITH_UNDO: specifies that this operation should be
        undoable.
         wxRICHTEXT_SETSTYLE_OPTIMIZE: specifies that the style should not be applied
        if the
        combined style at this point is already the style in question.
         wxRICHTEXT_SETSTYLE_PARAGRAPHS_ONLY: specifies that the style should only be
        applied to paragraphs,
        and not the content. This allows content styling to be preserved independently
        from that of e.g. a named paragraph style.
         wxRICHTEXT_SETSTYLE_CHARACTERS_ONLY: specifies that the style should only be
        applied to characters,
        and not the paragraph. This allows content styling to be preserved
        independently from that of e.g. a named paragraph style.
         wxRICHTEXT_SETSTYLE_RESET: resets (clears) the existing style before applying
        the new style.
         wxRICHTEXT_SETSTYLE_REMOVE: removes the specified style. Only the style flags
        are used in this operation.
    */
    bool SetStyle(const wxRichTextRange& range,
                  const wxTextAttr& style,
                  int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO);

    /**
        Sets the current style sheet, if any. This will allow the application to use
        named character and paragraph styles found in the style sheet.
    */
    void SetStyleSheet(wxRichTextStyleSheet* styleSheet);

    /**
        Submit an action immediately, or delay it according to whether collapsing is on.
    */
    bool SubmitAction(wxRichTextAction* action);

    /**
        Returns @true if undo suppression is currently on.
    */
    bool SuppressingUndo();
};


/**
    @class wxRichTextFileHandler
    @headerfile richtextbuffer.h wx/richtext/richtextbuffer.h

    This is the base class for file handlers, for loading and/or saving content
    associated with a wxRichTextBuffer.

    @library{wxrichtext}
    @category{FIXME}
*/
class wxRichTextFileHandler : public wxObject
{
public:
    /**
        Constructor.
    */
    wxRichTextFileHandler(const wxString& name = wxEmptyString,
                          const wxString& ext = wxEmptyString,
                          int type = 0);

    /**
        Override this function and return @true if this handler can we handle @e
        filename. By default,
        this function checks the extension.
    */
    bool CanHandle(const wxString& filename);

    /**
        Override and return @true if this handler can load content.
    */
    bool CanLoad();

    /**
        Override and return @true if this handler can save content.
    */
    bool CanSave();

    /**
        Override to load content from @e stream into @e buffer.
    */
    bool DoLoadFile(wxRichTextBuffer* buffer, wxInputStream& stream);

    /**
        Override to save content to @e stream from @e buffer.
    */
    bool DoSaveFile(wxRichTextBuffer* buffer, wxOutputStream& stream);

    /**
        Returns the encoding associated with the handler (if any).
    */
    const wxString GetEncoding();

    /**
        Returns the extension associated with the handler.
    */
    wxString GetExtension();

    /**
        Returns flags that change the behaviour of loading or saving. See the
        documentation for each
        handler class to see what flags are relevant for each handler.
    */
    int GetFlags();

    /**
        Returns the name of the handler.
    */
    wxString GetName();

    /**
        Returns the type of the handler.
    */
    int GetType();

    /**
        Returns @true if this handler should be visible to the user.
    */
    bool IsVisible();

    //@{
    /**
        Loads content from a stream or file. Not all handlers will implement file
        loading.
    */
    bool LoadFile(wxRichTextBuffer* buffer, wxInputStream& stream);
    bool LoadFile(wxRichTextBuffer* buffer,
                  const wxString& filename);
    //@}

    //@{
    /**
        Saves content to a stream or file. Not all handlers will implement file saving.
    */
    bool SaveFile(wxRichTextBuffer* buffer, wxOutputStream& stream);
    bool SaveFile(wxRichTextBuffer* buffer,
                  const wxString& filename);
    //@}

    /**
        Sets the encoding to use when saving a file. If empty, a suitable encoding is
        chosen.
    */
    void SetEncoding(const wxString& encoding);

    /**
        Sets the default extension to recognise.
    */
    void SetExtension(const wxString& ext);

    /**
        Sets flags that change the behaviour of loading or saving. See the
        documentation for each
        handler class to see what flags are relevant for each handler.
        
        You call this function directly if you are using a file handler explicitly
        (without
        going through the text control or buffer LoadFile/SaveFile API). Or, you can
        call the control or buffer's SetHandlerFlags function to set the flags that will
        be used for subsequent load and save operations.
    */
    void SetFlags(int flags);

    /**
        Sets the name of the handler.
    */
    void SetName(const wxString& name);

    /**
        Sets the handler type.
    */
    void SetType(int type);

    /**
        Sets whether the handler should be visible to the user (via the application's
        load and save
        dialogs).
    */
    void SetVisible(bool visible);
};


/**
    @class wxRichTextRange
    @headerfile richtextbuffer.h wx/richtext/richtextbuffer.h

    This class stores beginning and end positions for a range of data.

    @library{wxrichtext}
    @category{FIXME}
*/
class wxRichTextRange
{
public:
    //@{
    /**
        Constructors.
    */
    wxRichTextRange(long start, long end);
    wxRichTextRange(const wxRichTextRange& range);
    wxRichTextRange();
    //@}

    /**
        Destructor.
    */
    ~wxRichTextRange();

    /**
        Returns @true if the given position is within this range. Does not
        match if the range is empty.
    */
    bool Contains(long pos);

    /**
        Converts the internal range, which uses the first and last character positions
        of the range,
        to the API-standard range, whose end is one past the last character in the
        range.
        In other words, one is added to the end position.
    */
    wxRichTextRange FromInternal();

    /**
        Returns the end position.
    */
    long GetEnd();

    /**
        Returns the length of the range.
    */
    long GetLength();

    /**
        Returns the start of the range.
    */
    long GetStart();

    /**
        Returns @true if this range is completely outside @e range.
    */
    bool IsOutside(const wxRichTextRange& range);

    /**
        Returns @true if this range is completely within @e range.
    */
    bool IsWithin(const wxRichTextRange& range);

    /**
        Limits this range to be within @e range.
    */
    bool LimitTo(const wxRichTextRange& range);

    /**
        Sets the end of the range.
    */
    void SetEnd(long end);

    /**
        Sets the range.
    */
    void SetRange(long start, long end);

    /**
        Sets the start of the range.
    */
    void SetStart(long start);

    /**
        Swaps the start and end.
    */
    void Swap();

    /**
        Converts the API-standard range, whose end is one past the last character in
        the range,
        to the internal form, which uses the first and last character positions of the
        range.
        In other words, one is subtracted from the end position.
    */
    wxRichTextRange ToInternal();

    /**
        Adds @e range to this range.
    */
    wxRichTextRange operator+(const wxRichTextRange& range);

    /**
        Subtracts @e range from this range.
    */
    wxRichTextRange operator-(const wxRichTextRange& range);

    /**
        Assigns @e range to this range.
    */
    void operator=(const wxRichTextRange& range);

    /**
        Returns @true if @e range is the same as this range.
    */
    bool operator==(const wxRichTextRange& range);
};
