/////////////////////////////////////////////////////////////////////////////
// Name:        _richtextctrl.i
// Purpose:     wxRichTextCtrl and related classes
//
// Author:      Robin Dunn
//
// Created:     11-April-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
%}

//---------------------------------------------------------------------------
%newgroup

enum {
    wxRE_READONLY,
    wxRE_MULTILINE,

    wxRICHTEXT_SHIFT_DOWN,
    wxRICHTEXT_CTRL_DOWN,
    wxRICHTEXT_ALT_DOWN,

    wxRICHTEXT_SELECTED,
    wxRICHTEXT_TAGGED,
    wxRICHTEXT_FOCUSSED,
    wxRICHTEXT_IS_FOCUS,

};


//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING2(RichTextCtrlNameStr, wxT("richText"));

MustHaveApp(wxRichTextCtrl);


DocStr(wxRichTextCtrl,
"", "");


class wxRichTextCtrl : public wxScrolledWindow
{
public:
    %pythonAppend wxRichTextCtrl         "self._setOORInfo(self)"
    %pythonAppend wxRichTextCtrl()       ""

    wxRichTextCtrl( wxWindow* parent,
                    wxWindowID id = -1,
                    const wxString& value = wxPyEmptyString,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxRE_MULTILINE,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxPyRichTextCtrlNameStr );
    %RenameCtor(PreRichTextCtrl, wxRichTextCtrl());


    bool Create( wxWindow* parent,
                 wxWindowID id = -1,
                 const wxString& value = wxPyEmptyString,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxRE_MULTILINE,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxPyRichTextCtrlNameStr );


    DocDeclStr(
        virtual wxString , GetValue() const,
        "", "");

    DocDeclStr(
        virtual void , SetValue(const wxString& value),
        "", "");


    DocDeclStr(
        virtual wxString , GetRange(long from, long to) const,
        "", "");


    DocDeclStr(
        virtual int , GetLineLength(long lineNo) const ,
        "", "");

    DocDeclStr(
        virtual wxString , GetLineText(long lineNo) const ,
        "", "");

    DocDeclStr(
        virtual int , GetNumberOfLines() const ,
        "", "");


    DocDeclStr(
        virtual bool , IsModified() const ,
        "", "");

    DocDeclStr(
        virtual bool , IsEditable() const ,
        "", "");


    // more readable flag testing methods
    DocDeclStr(
        bool , IsSingleLine() const,
        "", "");

    DocDeclStr(
        bool , IsMultiLine() const,
        "", "");


    DocDeclAStr(
        virtual void , GetSelection(long* OUTPUT, long* OUTPUT) const,
        "GetSelection() --> (start, end)",
        "Returns the start and end positions of the current selection.  If the
values are the same then there is no selection.", "");


    DocDeclStr(
        virtual wxString , GetStringSelection() const,
        "", "");


    DocDeclStr(
        wxString , GetFilename() const,
        "", "");


    DocDeclStr(
        void , SetFilename(const wxString& filename),
        "", "");


    DocDeclStr(
        void , SetDelayedLayoutThreshold(long threshold),
        "Set the threshold in character positions for doing layout optimization
during sizing.", "");


    DocDeclStr(
        long , GetDelayedLayoutThreshold() const,
        "Get the threshold in character positions for doing layout optimization
during sizing.", "");



    DocDeclStr(
        virtual void , Clear(),
        "", "");

    DocDeclStr(
        virtual void , Replace(long from, long to, const wxString& value),
        "", "");

    DocDeclStr(
        virtual void , Remove(long from, long to),
        "", "");


    DocDeclStr(
        virtual bool , LoadFile(const wxString& file, int type = wxRICHTEXT_TYPE_ANY),
        "Load the contents of the document from the given filename.", "");

    DocDeclStr(
        virtual bool , SaveFile(const wxString& file = wxPyEmptyString,
                                int type = wxRICHTEXT_TYPE_ANY),
        "Save the contents of the document to the given filename, or if the
empty string is passed then to the filename set with `SetFilename`.", "");


    DocDeclStr(
        void , SetHandlerFlags(int flags), 
        "Set the handler flags, controlling loading and saving.", "");

    DocDeclStr(
        int , GetHandlerFlags() const, 
        "Get the handler flags, controlling loading and saving.", "");

    // sets/clears the dirty flag
    DocDeclStr(
        virtual void , MarkDirty(),
        "Sets the dirty flag, meaning that the contents of the control have
changed and need to be saved.", "");

    DocDeclStr(
        virtual void , DiscardEdits(),
        "Clears the dirty flag.
:see: `MarkDirty`", "");


    DocDeclStr(
        virtual void , SetMaxLength(unsigned long len),
        "Set the max number of characters which may be entered in a single line
text control.", "");


    DocDeclStr(
        virtual void , WriteText(const wxString& text),
        "Insert text at the current position.", "");

    DocDeclStr(
        virtual void , AppendText(const wxString& text),
        "Append text to the end of the document.", "");


    DocDeclStr(
        virtual bool , SetStyle(const wxRichTextRange& range, const wxRichTextAttr& style),
        "Set the style for the text in ``range`` to ``style``", "");

    DocDeclStr(
        virtual bool , GetStyle(long position, wxRichTextAttr& style),
        "Retrieve the style used at the given position.  Copies the style
values at ``position`` into the ``style`` parameter and returns ``True``
if successful.  Returns ``False`` otherwise.", "");


    DocDeclStr(
        virtual bool , GetStyleForRange(const wxRichTextRange& range, wxRichTextAttr& style),
        "Get the common set of styles for the range", "");
    

    DocDeclStr(
        virtual bool , SetStyleEx(const wxRichTextRange& range, const wxRichTextAttr& style,
                                  int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO),
        "Extended style setting operation with flags including:
RICHTEXT_SETSTYLE_WITH_UNDO, RICHTEXT_SETSTYLE_OPTIMIZE,
RICHTEXT_SETSTYLE_PARAGRAPHS_ONLY, RICHTEXT_SETSTYLE_CHARACTERS_ONLY", "");
    

    
    DocDeclStr(
        virtual bool , GetUncombinedStyle(long position, wxRichTextAttr& style),
        "Get the content (uncombined) attributes for this position.  Copies the
style values at ``position`` into the ``style`` parameter and returns
``True`` if successful.  Returns ``False`` otherwise.", "");


    DocDeclStr(
        virtual bool , SetDefaultStyle(const wxRichTextAttr& style),
        "Set the style used by default for the rich text document.", "");


    DocDeclStrName(
        virtual const wxRichTextAttr , GetDefaultStyleEx() const,
        "Retrieves a copy of the default style object.", "",
        GetDefaultStyle);


// TODO:  Select which of these to keep or rename
    
    /// Set list style
//     virtual bool SetListStyle(const wxRichTextRange& range,
//                               wxRichTextListStyleDefinition* def,
//                               int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO,
//                               int startFrom = 1, int specifiedLevel = -1);
    virtual bool SetListStyle(const wxRichTextRange& range,
                              const wxString& defName,
                              int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO,
                              int startFrom = 1, int specifiedLevel = -1);

    /// Clear list for given range
    virtual bool ClearListStyle(const wxRichTextRange& range, int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO);

    /// Number/renumber any list elements in the given range
    /// def/defName can be NULL/empty to indicate that the existing list style should be used.
//     virtual bool NumberList(const wxRichTextRange& range,
//                             wxRichTextListStyleDefinition* def = NULL,
//                             int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO,
//                             int startFrom = 1, int specifiedLevel = -1);
    virtual bool NumberList(const wxRichTextRange& range,
                            const wxString& defName,
                            int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO,
                            int startFrom = 1, int specifiedLevel = -1);

    /// Promote the list items within the given range. promoteBy can be a positive or negative number, e.g. 1 or -1
    /// def/defName can be NULL/empty to indicate that the existing list style should be used.
//     virtual bool PromoteList(int promoteBy, const wxRichTextRange& range,
//                              wxRichTextListStyleDefinition* def = NULL,
//                              int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO,
//                              int specifiedLevel = -1);
    virtual bool PromoteList(int promoteBy, const wxRichTextRange& range,
                             const wxString& defName,
                             int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO,
                             int specifiedLevel = -1);

    /// Deletes the content in the given range
    virtual bool Delete(const wxRichTextRange& range);


    

    DocDeclStr(
        virtual long , XYToPosition(long x, long y) const,
        "Translate a col,row coordinants into a document position.", "");

    DocDeclAStr(
        virtual void , PositionToXY(long pos, long *OUTPUT, long *OUTPUT) const,
        "PositionToXY(self, long pos) --> (x, y)",
        "Retrieves the col,row for the given position within the document", "");


    DocDeclStr(
        virtual void , ShowPosition(long position),
        "Ensure that the given position in the document is visible.", "");


    DocDeclAStr(
        virtual wxTextCtrlHitTestResult , HitTest(const wxPoint& pt, long *OUTPUT) const,
        "HitTest(self, Point pt) --> (result, pos)",
        "Returns the character position at the given point in pixels.  Note
that ``pt`` should be given in device coordinates, and not be adjusted
for the client area origin nor for scrolling.  The return value is a
tuple of the hit test result and the position.", "

Possible result values are a bitmask of these flags:

    =========================  ====================================
    RICHTEXT_HITTEST_NONE      The point was not on this object.
    RICHTEXT_HITTEST_BEFORE    The point was before the position
                                  returned from HitTest.
    RICHTEXT_HITTEST_AFTER     The point was after the position
                               returned from HitTest.
    RICHTEXT_HITTEST_ON        The point was on the position
                               returned from HitTest
    =========================  ====================================
");

    DocDeclAStrName(
        virtual wxTextCtrlHitTestResult , HitTest(const wxPoint& pt,
                                                  wxTextCoord *OUTPUT,
                                                  wxTextCoord *OUTPUT) const,
        "HitTestRC(self, Point pt) --> (result, col, row)",
        "Returns the column and row of the given point in pixels.  Note that
``pt`` should be given in device coordinates, and not be adjusted for
the client area origin nor for scrolling.  The return value is a tuple
of the hit test result and the column and row values.", "
:see: `HitTest`",
        HitTestXY);


    // Clipboard operations
    DocDeclStr(
        virtual void , Copy(),
        "Copies the selected text to the clipboard.", "");

    DocDeclStr(
        virtual void , Cut(),
        "Copies the selected text to the clipboard and removes the selection.", "");

    DocDeclStr(
        virtual void , Paste(),
        "Pastes text from the clipboard into the document at the current
insertion point.", "");

    DocDeclStr(
        virtual void , DeleteSelection(),
        "Remove the current selection.", "");


    DocDeclStr(
        virtual bool , CanCopy() const,
        "Returns ``True`` if the selection can be copied to the clipboard.", "");

    DocDeclStr(
        virtual bool , CanCut() const,
        "Returns ``True`` if the selection can be cut to the clipboard.", "");

    DocDeclStr(
        virtual bool , CanPaste() const,
        "Returns ``True`` if the current contents of the clipboard can be
pasted into the document.", "");

    DocDeclStr(
        virtual bool , CanDeleteSelection() const,
        "Returns ``True`` if the selection can be removed from the document.", "");


    // Undo/redo
    DocDeclStr(
        virtual void , Undo(),
        "If the last operation can be undone, undoes the last operation.", "");

    DocDeclStr(
        virtual void , Redo(),
        "If the last operation can be redone, redoes the last operation.", "");


    DocDeclStr(
        virtual bool , CanUndo() const,
        "Returns ``True`` if the last operation can be undone.", "");

    DocDeclStr(
        virtual bool , CanRedo() const,
        "Returns ``True`` if the last operation can be redone.", "");


    // Insertion point
    DocDeclStr(
        virtual void , SetInsertionPoint(long pos),
        "Sets the insertion point at the given position.", "");

    DocDeclStr(
        virtual void , SetInsertionPointEnd(),
        "Moves the insertion point to the end of the document.", "");

    DocDeclStr(
        virtual long , GetInsertionPoint() const,
        "Returns the insertion point. This is defined as the zero based index
of the character position to the right of the insertion point.", "");

    DocDeclStr(
        virtual long , GetLastPosition() const,
        "Returns the zero based index of the last position in the document.", "");


    DocDeclStr(
        virtual void , SetSelection(long from, long to),
        "Selects the text starting at the first position up to (but not
including) the character at the last position. If both parameters are
equal to -1 then all text in the control is selected.", "");

    DocDeclStr(
        virtual void , SelectAll(),
        "Select all text in the document.", "");

    DocDeclStr(
        virtual void , SetEditable(bool editable),
        "Makes the document editable or read-only, overriding the RE_READONLY
flag.", "");


    DocDeclStr(
        virtual bool , HasSelection() const,
        "", "");


///// Functionality specific to wxRichTextCtrl

    /// Write an image at the current insertion point. Supply optional type to use
    /// for internal and file storage of the raw data.
    DocDeclStr(
        virtual bool , WriteImage(const wxImage& image, int bitmapType = wxBITMAP_TYPE_PNG),
        "", "");


    /// Write a bitmap at the current insertion point. Supply optional type to use
    /// for internal and file storage of the raw data.
    DocDeclStrName(
        virtual bool , WriteImage(const wxBitmap& bitmap, int bitmapType = wxBITMAP_TYPE_PNG),
        "", "",
        WriteBitmap);


    /// Load an image from file and write at the current insertion point.
    DocDeclStrName(
        virtual bool , WriteImage(const wxString& filename, int bitmapType),
        "", "",
        WriteImageFile);


    /// Write an image block at the current insertion point.
    DocDeclStrName(
        virtual bool , WriteImage(const wxRichTextImageBlock& imageBlock),
        "", "",
        WriteImageBlock);


    /// Insert a newline (actually paragraph) at the current insertion point.
    DocDeclStr(
        virtual bool , Newline(),
        "", "");


    /// Insert a line break at the current insertion point.
    virtual bool LineBreak();

    
    DocDeclStr(
        virtual void , SetBasicStyle(const wxRichTextAttr& style),
        "", "");


    /// Get basic (overall) style
    DocDeclStr(
        virtual const wxRichTextAttr , GetBasicStyle() const,
        "", "");


    /// Begin using a style
    DocDeclStr(
        virtual bool , BeginStyle(const wxRichTextAttr& style),
        "", "");

    
    /// End the style
    DocDeclStr(
        virtual bool , EndStyle(),
        "", "");


    /// End all styles
    DocDeclStr(
        virtual bool , EndAllStyles(),
        "", "");


    /// Begin using bold
    DocDeclStr(
        bool , BeginBold(),
        "", "");


    /// End using bold
    DocDeclStr(
        bool , EndBold(),
        "", "");


    /// Begin using italic
    DocDeclStr(
        bool , BeginItalic(),
        "", "");


    /// End using italic
    DocDeclStr(
        bool , EndItalic(),
        "", "");


    /// Begin using underline
    DocDeclStr(
        bool , BeginUnderline(),
        "", "");


    /// End using underline
    DocDeclStr(
        bool , EndUnderline(),
        "", "");


    /// Begin using point size
    DocDeclStr(
        bool , BeginFontSize(int pointSize),
        "", "");


    /// End using point size
    DocDeclStr(
        bool , EndFontSize(),
        "", "");


    /// Begin using this font
    DocDeclStr(
        bool , BeginFont(const wxFont& font),
        "", "");


    /// End using a font
    DocDeclStr(
        bool , EndFont(),
        "", "");


    /// Begin using this colour
    DocDeclStr(
        bool , BeginTextColour(const wxColour& colour),
        "", "");


    /// End using a colour
    DocDeclStr(
        bool , EndTextColour(),
        "", "");


    /// Begin using alignment
    DocDeclStr(
        bool , BeginAlignment(wxTextAttrAlignment alignment),
        "", "");


    /// End alignment
    DocDeclStr(
        bool , EndAlignment(),
        "", "");


    /// Begin left indent
    DocDeclStr(
        bool , BeginLeftIndent(int leftIndent, int leftSubIndent = 0),
        "", "");


    /// End left indent
    DocDeclStr(
        bool , EndLeftIndent(),
        "", "");


    /// Begin right indent
    DocDeclStr(
        bool , BeginRightIndent(int rightIndent),
        "", "");


    /// End right indent
    DocDeclStr(
        bool , EndRightIndent(),
        "", "");


    /// Begin paragraph spacing
    DocDeclStr(
        bool , BeginParagraphSpacing(int before, int after),
        "", "");


    /// End paragraph spacing
    DocDeclStr(
        bool , EndParagraphSpacing(),
        "", "");


    /// Begin line spacing
    DocDeclStr(
        bool , BeginLineSpacing(int lineSpacing),
        "", "");


    /// End line spacing
    DocDeclStr(
        bool , EndLineSpacing(),
        "", "");


    /// Begin numbered bullet
    DocDeclStr(
        bool , BeginNumberedBullet(int bulletNumber,
                                   int leftIndent,
                                   int leftSubIndent,
                                   int bulletStyle = wxTEXT_ATTR_BULLET_STYLE_ARABIC|wxTEXT_ATTR_BULLET_STYLE_PERIOD),
        "", "");


    /// End numbered bullet
    DocDeclStr(
        bool , EndNumberedBullet(),
        "", "");


    /// Begin symbol bullet
    DocDeclStr(
        bool , BeginSymbolBullet(const wxString& symbol,
                                 int leftIndent,
                                 int leftSubIndent,
                                 int bulletStyle = wxTEXT_ATTR_BULLET_STYLE_SYMBOL),
        "", "");


    /// End symbol bullet
    DocDeclStr(
        bool , EndSymbolBullet(),
        "", "");


    /// Begin standard bullet
    DocDeclStr(
        bool , BeginStandardBullet(const wxString& bulletName,
                                   int leftIndent,
                                   int leftSubIndent,
                                   int bulletStyle = wxTEXT_ATTR_BULLET_STYLE_STANDARD),
        "", "");
    


    /// End standard bullet
    DocDeclStr(
        bool , EndStandardBullet(),
        "", "");
    
    
    /// Begin named character style
    DocDeclStr(
        bool , BeginCharacterStyle(const wxString& characterStyle),
        "", "");


    /// End named character style
    DocDeclStr(
        bool , EndCharacterStyle(),
        "", "");


    /// Begin named paragraph style
    DocDeclStr(
        bool , BeginParagraphStyle(const wxString& paragraphStyle),
        "", "");


    /// End named character style
    DocDeclStr(
        bool , EndParagraphStyle(),
        "", "");


    DocDeclStr(
        bool , BeginListStyle(const wxString& listStyle, int level = 1, int number = 1),
        "Begin named list style.", "");
        
    DocDeclStr(
        bool , EndListStyle(), "End named list style.", "");

    DocDeclStr(
        bool , BeginURL(const wxString& url, const wxString& characterStyle = wxEmptyString),
            "Begin URL.", "");
    
    DocDeclStr(
        bool , EndURL(), "End URL.", "");

    /// Sets the default style to the style under the cursor
    DocDeclStr(
        bool , SetDefaultStyleToCursorStyle(),
        "", "");


    /// Clear the selection
    DocDeclStr(
        virtual void , SelectNone(),
        "", "");

    /// Select the word at the given character position
    DocDeclStr(
        virtual bool , SelectWord(long position),
        "", "");


    /// Get/set the selection range in character positions. -1, -1 means no selection.
    DocDeclStr(
        wxRichTextRange , GetSelectionRange() const,
        "", "");

    DocDeclStr(
        void , SetSelectionRange(const wxRichTextRange& range),
        "", "");

    /// Get/set the selection range in character positions. -1, -1 means no selection.
    /// The range is in internal format, i.e. a single character selection is denoted
    /// by (n, n)
    DocDeclStr(
        const wxRichTextRange& , GetInternalSelectionRange() const,
        "", "");

    DocDeclStr(
        void , SetInternalSelectionRange(const wxRichTextRange& range),
        "", "");



    /// Add a new paragraph of text to the end of the buffer
    DocDeclStr(
        virtual wxRichTextRange , AddParagraph(const wxString& text),
        "", "");


    /// Add an image
    DocDeclStr(
        virtual wxRichTextRange , AddImage(const wxImage& image),
        "", "");


    /// Layout the buffer: which we must do before certain operations, such as
    /// setting the caret position.
    DocDeclStr(
        virtual bool , LayoutContent(bool onlyVisibleRect = false),
        "", "");


    /// Move the caret to the given character position
    DocDeclStr(
        virtual bool , MoveCaret(long pos, bool showAtLineStart = false),
        "", "");


    /// Move right
    DocDeclStr(
        virtual bool , MoveRight(int noPositions = 1, int flags = 0),
        "", "");


    /// Move left
    DocDeclStr(
        virtual bool , MoveLeft(int noPositions = 1, int flags = 0),
        "", "");


    /// Move up
    DocDeclStr(
        virtual bool , MoveUp(int noLines = 1, int flags = 0),
        "", "");


    /// Move up
    DocDeclStr(
        virtual bool , MoveDown(int noLines = 1, int flags = 0),
        "", "");


    /// Move to the end of the line
    DocDeclStr(
        virtual bool , MoveToLineEnd(int flags = 0),
        "", "");


    /// Move to the start of the line
    DocDeclStr(
        virtual bool , MoveToLineStart(int flags = 0),
        "", "");


    /// Move to the end of the paragraph
    DocDeclStr(
        virtual bool , MoveToParagraphEnd(int flags = 0),
        "", "");


    /// Move to the start of the paragraph
    DocDeclStr(
        virtual bool , MoveToParagraphStart(int flags = 0),
        "", "");


    /// Move to the start of the buffer
    DocDeclStr(
        virtual bool , MoveHome(int flags = 0),
        "", "");


    /// Move to the end of the buffer
    DocDeclStr(
        virtual bool , MoveEnd(int flags = 0),
        "", "");


    /// Move n pages up
    DocDeclStr(
        virtual bool , PageUp(int noPages = 1, int flags = 0),
        "", "");


    /// Move n pages down
    DocDeclStr(
        virtual bool , PageDown(int noPages = 1, int flags = 0),
        "", "");


    /// Move n words left
    DocDeclStr(
        virtual bool , WordLeft(int noPages = 1, int flags = 0),
        "", "");


    /// Move n words right
    DocDeclStr(
        virtual bool , WordRight(int noPages = 1, int flags = 0),
        "", "");


    /// Returns the buffer associated with the control.
    DocDeclStr(
        wxRichTextBuffer& , GetBuffer(),
        "", "");


    /// Start batching undo history for commands.
    DocDeclStr(
        virtual bool , BeginBatchUndo(const wxString& cmdName),
        "", "");


    /// End batching undo history for commands.
    DocDeclStr(
        virtual bool , EndBatchUndo(),
        "", "");


    /// Are we batching undo history for commands?
    DocDeclStr(
        virtual bool , BatchingUndo() const,
        "", "");


    /// Start suppressing undo history for commands.
    DocDeclStr(
        virtual bool , BeginSuppressUndo(),
        "", "");


    /// End suppressing undo history for commands.
    DocDeclStr(
        virtual bool , EndSuppressUndo(),
        "", "");


    /// Are we suppressing undo history for commands?
    DocDeclStr(
        virtual bool , SuppressingUndo() const,
        "", "");


    /// Test if this whole range has character attributes of the specified kind. If any
    /// of the attributes are different within the range, the test fails. You
    /// can use this to implement, for example, bold button updating. style must have
    /// flags indicating which attributes are of interest.
    DocDeclStr(
        virtual bool , HasCharacterAttributes(const wxRichTextRange& range,
                                              const wxRichTextAttr& style) const,
        "", "");



    /// Test if this whole range has paragraph attributes of the specified kind. If any
    /// of the attributes are different within the range, the test fails. You
    /// can use this to implement, for example, centering button updating. style must have
    /// flags indicating which attributes are of interest.
    DocDeclStr(
        virtual bool , HasParagraphAttributes(const wxRichTextRange& range,
                                              const wxRichTextAttr& style) const,
        "", "");



    /// Is all of the selection bold?
    DocDeclStr(
        virtual bool , IsSelectionBold(),
        "", "");


    /// Is all of the selection italics?
    DocDeclStr(
        virtual bool , IsSelectionItalics(),
        "", "");


    /// Is all of the selection underlined?
    DocDeclStr(
        virtual bool , IsSelectionUnderlined(),
        "", "");


    /// Is all of the selection aligned according to the specified flag?
    DocDeclStr(
        virtual bool , IsSelectionAligned(wxTextAttrAlignment alignment),
        "", "");


    /// Apply bold to the selection
    DocDeclStr(
        virtual bool , ApplyBoldToSelection(),
        "", "");


    /// Apply italic to the selection
    DocDeclStr(
        virtual bool , ApplyItalicToSelection(),
        "", "");


    /// Apply underline to the selection
    DocDeclStr(
        virtual bool , ApplyUnderlineToSelection(),
        "", "");


    /// Apply alignment to the selection
    DocDeclStr(
        virtual bool , ApplyAlignmentToSelection(wxTextAttrAlignment alignment),
        "", "");


    /// Apply a named style to the selection
    virtual bool ApplyStyle(wxRichTextStyleDefinition* def);

    /// Set style sheet, if any.
    DocDeclStr(
        void , SetStyleSheet(wxRichTextStyleSheet* styleSheet),
        "", "");

    DocDeclStr(
        wxRichTextStyleSheet* , GetStyleSheet() const,
        "", "");


    /// Push style sheet to top of stack
    bool PushStyleSheet(wxRichTextStyleSheet* styleSheet);

    
    /// Pop style sheet from top of stack
    wxRichTextStyleSheet* PopStyleSheet();

    
    /// Apply the style sheet to the buffer, for example if the styles have changed.
    DocDeclStr(
        bool , ApplyStyleSheet(wxRichTextStyleSheet* styleSheet = NULL),
        "", "");



    %property(Buffer, GetBuffer, doc="See `GetBuffer`");
    %property(DefaultStyle, GetDefaultStyle, SetDefaultStyle, doc="See `GetDefaultStyle` and `SetDefaultStyle`");
    %property(DelayedLayoutThreshold, GetDelayedLayoutThreshold, SetDelayedLayoutThreshold, doc="See `GetDelayedLayoutThreshold` and `SetDelayedLayoutThreshold`");
    %property(Filename, GetFilename, SetFilename, doc="See `GetFilename` and `SetFilename`");
    %property(InsertionPoint, GetInsertionPoint, SetInsertionPoint, doc="See `GetInsertionPoint` and `SetInsertionPoint`");
    %property(InternalSelectionRange, GetInternalSelectionRange, SetInternalSelectionRange, doc="See `GetInternalSelectionRange` and `SetInternalSelectionRange`");
    %property(LastPosition, GetLastPosition, doc="See `GetLastPosition`");
    %property(NumberOfLines, GetNumberOfLines, doc="See `GetNumberOfLines`");
    %property(Selection, GetSelection, SetSelectionRange, doc="See `GetSelection` and `SetSelection`");
    %property(SelectionRange, GetSelectionRange, SetSelectionRange, doc="See `GetSelectionRange` and `SetSelectionRange`");
    %property(StringSelection, GetStringSelection, doc="See `GetStringSelection`");
    %property(StyleSheet, GetStyleSheet, SetStyleSheet, doc="See `GetStyleSheet` and `SetStyleSheet`");
    %property(Value, GetValue, SetValue, doc="See `GetValue` and `SetValue`");



    // TODO:  Some of these methods may be useful too...
    

//     /// Set up scrollbars, e.g. after a resize
//     virtual void SetupScrollbars(bool atTop = false);

//     /// Keyboard navigation
//     virtual bool KeyboardNavigate(int keyCode, int flags);

//     /// Paint the background
//     virtual void PaintBackground(wxDC& dc);

// #if wxRICHTEXT_BUFFERED_PAINTING
//     /// Recreate buffer bitmap if necessary
//     virtual bool RecreateBuffer(const wxSize& size = wxDefaultSize);
// #endif

//     /// Set the selection
//     virtual void DoSetSelection(long from, long to, bool scrollCaret = true);

//     /// Write text
//     virtual void DoWriteText(const wxString& value, int flags = 0);

//     /// Should we inherit colours?
//     virtual bool ShouldInheritColours() const { return false; }

//     /// Position the caret
//     virtual void PositionCaret();

//     /// Extend the selection, returning true if the selection was
//     /// changed. Selections are in caret positions.
//     virtual bool ExtendSelection(long oldPosition, long newPosition, int flags);

//     /// Scroll into view. This takes a _caret_ position.
//     virtual bool ScrollIntoView(long position, int keyCode);

//     /// The caret position is the character position just before the caret.
//     /// A value of -1 means the caret is at the start of the buffer.
//     void SetCaretPosition(long position, bool showAtLineStart = false) ;
//     long GetCaretPosition() const { return m_caretPosition; }

//     /// The adjusted caret position is the character position adjusted to take
//     /// into account whether we're at the start of a paragraph, in which case
//     /// style information should be taken from the next position, not current one.
//     long GetAdjustedCaretPosition(long caretPos) const;

//     /// Move caret one visual step forward: this may mean setting a flag
//     /// and keeping the same position if we're going from the end of one line
//     /// to the start of the next, which may be the exact same caret position.
//     void MoveCaretForward(long oldPosition) ;

//     /// Move caret one visual step forward: this may mean setting a flag
//     /// and keeping the same position if we're going from the end of one line
//     /// to the start of the next, which may be the exact same caret position.
//     void MoveCaretBack(long oldPosition) ;

//     /// Get the caret height and position for the given character position
//     bool GetCaretPositionForIndex(long position, wxRect& rect);

//     /// Gets the line for the visible caret position. If the caret is
//     /// shown at the very end of the line, it means the next character is actually
//     /// on the following line. So let's get the line we're expecting to find
//     /// if this is the case.
//     wxRichTextLine* GetVisibleLineForCaretPosition(long caretPosition) const;

//     /// Gets the command processor
//     wxCommandProcessor* GetCommandProcessor() const { return GetBuffer().GetCommandProcessor(); }

//     /// Delete content if there is a selection, e.g. when pressing a key.
//     /// Returns the new caret position in newPos, or leaves it if there
//     /// was no action.
//     bool DeleteSelectedContent(long* newPos= NULL);

//     /// Transform logical to physical
//     wxPoint GetPhysicalPoint(const wxPoint& ptLogical) const;

//     /// Transform physical to logical
//     wxPoint GetLogicalPoint(const wxPoint& ptPhysical) const;

//     /// Finds the caret position for the next word. Direction
//     /// is 1 (forward) or -1 (backwards).
//     virtual long FindNextWordPosition(int direction = 1) const;

//     /// Is the given position visible on the screen?
//     bool IsPositionVisible(long pos) const;

//     /// Returns the first visible position in the current view
//     long GetFirstVisiblePosition() const;

//     /// Returns the caret position since the default formatting was changed. As
//     /// soon as this position changes, we no longer reflect the default style
//     /// in the UI. A value of -2 means that we should only reflect the style of the
//     /// content under the caret.
//     long GetCaretPositionForDefaultStyle() const { return m_caretPositionForDefaultStyle; }

//     /// Set the caret position for the default style that the user is selecting.
//     void SetCaretPositionForDefaultStyle(long pos) { m_caretPositionForDefaultStyle = pos; }

//     /// Should the UI reflect the default style chosen by the user, rather than the style under
//     /// the caret?
//     bool IsDefaultStyleShowing() const { return m_caretPositionForDefaultStyle != -2; }

//     /// Convenience function that tells the control to start reflecting the default
//     /// style, since the user is changing it.
//     void SetAndShowDefaultStyle(const wxRichTextAttr& attr)
//     {
//         SetDefaultStyle(attr);
//         SetCaretPositionForDefaultStyle(GetCaretPosition());
//     }

//     /// Get the first visible point in the window
//     wxPoint GetFirstVisiblePoint() const;

// // Implementation

//      /// Font names take a long time to retrieve, so cache them (on demand)
//      static const wxArrayString& GetAvailableFontNames();
//      static void ClearAvailableFontNames();
    
};


//---------------------------------------------------------------------------
%newgroup


%constant wxEventType wxEVT_COMMAND_RICHTEXT_LEFT_CLICK;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_RIGHT_CLICK;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_MIDDLE_CLICK;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_LEFT_DCLICK;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_RETURN;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_CHARACTER;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_DELETE;

%constant wxEventType wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGING;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGED;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACING;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACED;

%constant wxEventType wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_SELECTION_CHANGED;

%pythoncode {
EVT_RICHTEXT_LEFT_CLICK = wx.PyEventBinder(wxEVT_COMMAND_RICHTEXT_LEFT_CLICK, 1)
EVT_RICHTEXT_RIGHT_CLICK = wx.PyEventBinder(wxEVT_COMMAND_RICHTEXT_RIGHT_CLICK, 1)
EVT_RICHTEXT_MIDDLE_CLICK = wx.PyEventBinder(wxEVT_COMMAND_RICHTEXT_MIDDLE_CLICK, 1)
EVT_RICHTEXT_LEFT_DCLICK = wx.PyEventBinder(wxEVT_COMMAND_RICHTEXT_LEFT_DCLICK, 1)
EVT_RICHTEXT_RETURN = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_RETURN, 1)
EVT_RICHTEXT_CHARACTER = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_CHARACTER, 1)
EVT_RICHTEXT_DELETE = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_DELETE, 1)

EVT_RICHTEXT_STYLESHEET_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGING, 1)
EVT_RICHTEXT_STYLESHEET_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGED, 1)
EVT_RICHTEXT_STYLESHEET_REPLACING = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACING, 1)
EVT_RICHTEXT_STYLESHEET_REPLACED = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACED, 1)

EVT_RICHTEXT_CONTENT_INSERTED = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED, 1)
EVT_RICHTEXT_CONTENT_DELETED = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED, 1)
EVT_RICHTEXT_STYLE_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED, 1)
EVT_RICHTEXT_SELECTION_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_SELECTION_CHANGED, 1)    
}


class wxRichTextEvent : public wxNotifyEvent
{
public:
    wxRichTextEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);

    int GetPosition() const;
    void SetPosition(int n);

    int GetFlags() const;
    void SetFlags(int flags);

    wxRichTextStyleSheet* GetOldStyleSheet() const;
    void SetOldStyleSheet(wxRichTextStyleSheet* sheet);

    wxRichTextStyleSheet* GetNewStyleSheet() const;
    void SetNewStyleSheet(wxRichTextStyleSheet* sheet);

    const wxRichTextRange& GetRange() const;
    void SetRange(const wxRichTextRange& range);

    wxChar GetCharacter() const;
    void SetCharacter(wxChar ch);

    
    %property(Flags, GetFlags, SetFlags);
    %property(Index, GetPosition, SetPosition);
    %property(OldStyleSheet, GetOldStyleSheet, SetOldStyleSheet);
    %property(NewStyleSheet, GetNewStyleSheet, SetNewStyleSheet);
    %property(Range, GetRange, SetRange);
    %property(Character, GetCharacter, SetCharacter);
};


//---------------------------------------------------------------------------
