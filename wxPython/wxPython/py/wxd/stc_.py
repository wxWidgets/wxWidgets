"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


from Controls import Control
import Parameters as wx


class StyledTextCtrl(Control):
    """StyledTextCtrl class."""

    def __init__(self, parent, id, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=0, name=wx.PySTCNameStr):
        """Create a StyledTextCtrl instance."""
        pass

    def AddRefDocument(self, docPointer):
        """Extend life of document."""
        pass

    def AddStyledText(self, data):
        """Add array of cells to document."""
        pass

    def AddText(self, text):
        """Add text to the document."""
        pass

    def AutoCompActive(self):
        """Is there an auto-completion list visible?"""
        pass

    def AutoCompCancel(self):
        """Remove the auto-completion list from the screen."""
        pass

    def AutoCompComplete(self):
        """User has selected an item so remove the list and insert the
        selection."""
        pass

    def AutoCompGetAutoHide(self):
        """Return whether or not autocompletion is hidden
        automatically when nothing matches."""
        pass

    def AutoCompGetCancelAtStart(self):
        """Return whether auto-completion cancelled by backspacing
        before start."""
        pass

    def AutoCompGetChooseSingle(self):
        """Return whether a single item auto-completion list
        automatically choose the item."""
        pass

    def AutoCompGetDropRestOfWord(self):
        """Return whether or not autocompletion deletes any word
        characters after the inserted text upon completion."""
        pass

    def AutoCompGetIgnoreCase(self):
        """Return state of ignore case flag."""
        pass

    def AutoCompGetSeparator(self):
        """Return the auto-completion list separator character."""
        pass

    def AutoCompPosStart(self):
        """Return the position of the caret when the auto-completion
        list was displayed."""
        pass

    def AutoCompSelect(self, text):
        """Select the item in the auto-completion list that starts
        with a string."""
        pass

    def AutoCompSetAutoHide(self, autoHide):
        """Set whether or not autocompletion is hidden automatically
        when nothing matches."""
        pass

    def AutoCompSetCancelAtStart(self, cancel):
        """Should the auto-completion list be cancelled if the user
        backspaces to a position before where the box was created."""
        pass

    def AutoCompSetChooseSingle(self, chooseSingle):
        """Should a single item auto-completion list automatically
        choose the item."""
        pass

    def AutoCompSetDropRestOfWord(self, dropRestOfWord):
        """Set whether or not autocompletion deletes any word
        characters after the inserted text upon completion."""
        pass

    def AutoCompSetFillUps(self, characterSet):
        """Define a set of characters that when typed will cause the
        autocompletion to choose the selected item."""
        pass

    def AutoCompSetIgnoreCase(self, ignoreCase):
        """Set whether case is significant when performing
        auto-completion searches."""
        pass

    def AutoCompSetSeparator(self, separatorCharacter):
        """Change the separator character in the string setting up an
        auto-completion list.  Default is space but can be changed if
        items contain space."""
        pass

    def AutoCompShow(self, lenEntered, itemList):
        """Display a auto-completion list.

        The lenEntered parameter indicates how many characters before
        the caret should be used to provide context."""
        pass

    def AutoCompStops(self, characterSet):
        """Define a set of character that when typed cancel the
        auto-completion list."""
        pass

    def BeginUndoAction(self):
        """Start a sequence of actions that is undone and redone as a
        unit.  May be nested."""
        pass

    def BraceBadLight(self, pos):
        """Highlight the character at a position indicating there is
        no matching brace."""
        pass

    def BraceHighlight(self, pos1, pos2):
        """Highlight the characters at two positions."""
        pass

    def BraceMatch(self, pos):
        """Find the position of a matching brace or INVALID_POSITION
        if no match."""
        pass

    def CallTipActive(self):
        """Is there an active call tip?"""
        pass

    def CallTipCancel(self):
        """Remove the call tip from the screen."""
        pass

    def CallTipPosAtStart(self):
        """Return the position where the caret was before displaying
        the call tip."""
        pass

    def CallTipSetBackground(self, back):
        """Set the background colour for the call tip."""
        pass

    def CallTipSetHighlight(self, start, end):
        """Highlight a segment of the definition."""
        pass

    def CallTipShow(self, pos, definition):
        """Show a call tip containing a definition near position pos."""
        pass

    def CanPaste(self):
        """Will a paste succeed?"""
        pass

    def CanRedo(self):
        """Are there any redoable actions in the undo history?"""
        pass

    def CanUndo(self):
        """Are there any undoable actions in the undo history?"""
        pass

    def Clear(self):
        """Clear the selection."""
        pass

    def ClearAll(self):
        """Delete all text in the document."""
        pass

    def ClearDocumentStyle(self):
        """Set all style bytes to 0, remove all folding information."""
        pass

    def CmdKeyAssign(self, key, modifiers, cmd):
        """When key+modifier combination km is pressed perform msg."""
        pass

    def CmdKeyClear(self, key, modifiers):
        """When key+modifier combination km do nothing."""
        pass

    def CmdKeyClearAll(self):
        """Drop all key mappings."""
        pass

    def CmdKeyExecute(self, cmd):
        """Perform one of the operations defined by the STC_CMD_*
        constants."""
        pass

    def Colourise(self, start, end):
        """Colourise a segment of the document using the current
        lexing language."""
        pass

    def ConvertEOLs(self, eolMode):
        """Convert all line endings in the document to one mode."""
        pass

    def Copy(self):
        """Copy the selection to the clipboard."""
        pass

    def CreateDocument(self):
        """Create a new document object.

        Starts with reference count of 1 and not selected into editor."""
        pass

    def Cut(self):
        """Cut the selection to the clipboard."""
        pass

    def DelLineLeft(self):
        """Delete from the current position to the start of the line."""
        pass

    def DelLineRight(self):
        """Delete from the current position to the end of the line."""
        pass

    def DocLineFromVisible(self, lineDisplay):
        """Find the document line of a display line taking hidden
        lines into account."""
        pass

    def EmptyUndoBuffer(self):
        """Delete the undo history."""
        pass

    def EndUndoAction(self):
        """End a sequence of actions that is undone and redone as a
        unit."""
        pass

    def EnsureCaretVisible(self):
        """Ensure the caret is visible."""
        pass

    def EnsureVisible(self, line):
        """Ensure a particular line is visible by expanding any header
        line hiding it."""
        pass

    def EnsureVisibleEnforcePolicy(self, line):
        """Ensure a particular line is visible by expanding any header
        line hiding it.  Use the currently set visibility policy to
        determine which range to display."""
        pass

    def FindText(self, minPos, maxPos, text, flags=0):
        """Find some text in the document."""
        pass

    def FormatRange(self, doDraw, startPos, endPos, draw,
                    target, renderRect, pageRect):
        """On Windows, will draw the document into a display context
        such as a printer."""
        pass

    def GetAnchor(self):
        """Return the position of the opposite end of the selection to
        the caret."""
        pass

    def GetBackSpaceUnIndents(self):
        """Does a backspace pressed when caret is within indentation
        unindent?"""
        pass

    def GetBufferedDraw(self):
        """Is drawing done first into a buffer or direct to the
        screen?"""
        pass

    def GetCaretForeground(self):
        """Return the foreground colour of the caret."""
        pass

    def GetCaretLineBack(self):
        """Return the colour of the background of the line containing
        the caret."""
        pass

    def GetCaretLineVisible(self):
        """Is the background of the line containing the caret in a
        different colour?"""
        pass

    def GetCaretPeriod(self):
        """Return the time in milliseconds that the caret is on and
        off."""
        pass

    def GetCaretWidth(self):
        """Return the width of the insert mode caret."""
        pass

    def GetCharAt(self, pos):
        """Return the character byte at the position."""
        pass

    def GetCodePage(self):
        """Return the code page used to interpret the bytes of the
        document as characters."""
        pass

    def GetColumn(self, pos):
        """Return the column number of a position, taking tab width
        into account."""
        pass

    def GetControlCharSymbol(self):
        """Return the way control characters are displayed."""
        pass

    def GetCurLine(self):
        """Return (text, pos) tuple with the text of the line
        containing the caret and the position of the caret within the
        line."""
        pass

    def GetCurrentLine(self):
        """Return the line number of the line with the caret."""
        pass

    def GetCurrentPos(self):
        """Return the position of the caret."""
        pass

    def GetCursor(self):
        """Return cursor type."""
        pass

    def GetDocPointer(self):
        """Return a pointer to the document object."""
        pass

    def GetEOLMode(self):
        """Return the current end of line mode: CRLF, CR, or LF."""
        pass

    def GetEdgeColour(self):
        """Return the colour used in edge indication."""
        pass

    def GetEdgeColumn(self):
        """Return the column number which text should be kept within."""
        pass

    def GetEdgeMode(self):
        """Return the edge highlight mode."""
        pass

    def GetEndAtLastLine(self):
        """Return whether the maximum scroll position has the last
        line at the bottom of the view."""
        pass

    def GetEndStyled(self):
        """Return the position of the last correctly styled character."""
        pass

    def GetFirstVisibleLine(self):
        """Return the line at the top of the display."""
        pass

    def GetFoldExpanded(self, line):
        """Is a header line expanded?"""
        pass

    def GetFoldLevel(self, line):
        """Return the fold level of a line."""
        pass

    def GetFoldParent(self, line):
        """Find the parent line of a child line."""
        pass

    def GetHighlightGuide(self):
        """Return the highlighted indentation guide column."""
        pass

    def GetIndent(self):
        """Return indentation size."""
        pass

    def GetIndentationGuides(self):
        """Are the indentation guides visible?"""
        pass

    def GetLastChild(self, line, level):
        """Find the last child line of a header line."""
        pass

    def GetLastKeydownProcessed(self):
        """Can be used to prevent the EVT_CHAR handler from adding the
        char."""
        pass

    def GetLayoutCache(self):
        """Return the degree of caching of layout information."""
        pass

    def GetLength(self):
        """Return the number of characters in the document."""
        pass

    def GetLexer(self):
        """Return the lexing language of the document."""
        pass

    def GetLine(self, line):
        """Return the contents of a line."""
        pass

    def GetLineCount(self):
        """Return the number of lines in the document. There is
        always at least one."""
        pass

    def GetLineEndPosition(self, line):
        """Return the position after the last visible characters on a
        line."""
        pass

    def GetLineIndentPosition(self, line):
        """Return the position before the first non indentation
        character on a line."""
        pass

    def GetLineIndentation(self, line):
        """Return the number of columns that a line is indented."""
        pass

    def GetLineState(self, line):
        """Return the extra styling information for a line."""
        pass

    def GetLineVisible(self, line):
        """Is a line visible?"""
        pass

    def GetMarginLeft(self):
        """Return the size in pixels of the left margin."""
        pass

    def GetMarginMask(self, margin):
        """Return the marker mask of a margin."""
        pass

    def GetMarginRight(self):
        """Return the size in pixels of the right margin."""
        pass

    def GetMarginSensitive(self, margin):
        """Return the mouse click sensitivity of a margin."""
        pass

    def GetMarginType(self, margin):
        """Return the type of a margin."""
        pass

    def GetMarginWidth(self, margin):
        """Return the width of a margin in pixels."""
        pass

    def GetMaxLineState(self):
        """Return the last line number that has line state."""
        pass

    def GetModEventMask(self):
        """Return which document modification events are sent to the
        container."""
        pass

    def GetModify(self):
        """Is the document different from when it was last saved?"""
        pass

    def GetMouseDownCaptures(self):
        """Return whether mouse gets captured."""
        pass

    def GetMouseDwellTime(self):
        """Return the time the mouse must sit still to generate a
        mouse dwell event."""
        pass

    def GetOvertype(self):
        """Return true if overtype mode is active otherwise false is
        returned."""
        pass

    def GetPrintColourMode(self):
        """Return the print colour mode."""
        pass

    def GetPrintMagnification(self):
        """Return the print magnification."""
        pass

    def GetReadOnly(self):
        """In read-only mode?"""
        pass

    def GetSTCFocus(self):
        """Return internal focus flag."""
        pass

    def GetScrollWidth(self):
        """Return the document width assumed for scrolling."""
        pass

    def GetSearchFlags(self):
        """Return the search flags used by SearchInTarget."""
        pass

    def GetSelectedText(self):
        """Return the selected text."""
        pass

    def GetSelectionEnd(self):
        """Return the position at the end of the selection."""
        pass

    def GetSelection(self):
        """Return the start and end positions of the current
        selection."""
        pass

    def GetSelectionStart(self):
        """Return the position at the start of the selection."""
        pass

    def GetStatus(self):
        """Return error status."""
        pass

    def GetStyleAt(self, pos):
        """Return the style byte at the position."""
        pass

    def GetStyleBits(self):
        """Return number of bits in style bytes used to hold the
        lexical state."""
        pass

    def GetStyledText(self, startPos, endPos):
        """Return a buffer of cells."""
        pass

    def GetTabIndents(self):
        """Does a tab pressed when caret is within indentation indent?"""
        pass

    def GetTabWidth(self):
        """Return the visible size of a tab."""
        pass

    def GetTargetEnd(self):
        """Return the position that ends the target."""
        pass

    def GetTargetStart(self):
        """Return the position that starts the target."""
        pass

    def GetText(self):
        """Return all the text in the document."""
        pass

    def GetTextLength(self):
        """Return the number of characters in the document."""
        pass

    def GetTextRange(self, startPos, endPos):
        """Return a range of text."""
        pass

    def GetUndoCollection(self):
        """Is undo history being collected?"""
        pass

    def GetUseHorizontalScrollBar(self):
        """Is the horizontal scroll bar visible?"""
        pass

    def GetUseTabs(self):
        """Return whether tabs will be used in indentation."""
        pass

    def GetViewEOL(self):
        """Are the end of line characters visible?"""
        pass

    def GetViewWhiteSpace(self):
        """Are white space characters currently visible? Return one
        of SCWS_* constants."""
        pass

    def GetWrapMode(self):
        """Return whether text is word wrapped."""
        pass

    def GetXOffset(self):
        """Return the xOffset (self, ie, horizonal scroll position)."""
        pass

    def GetZoom(self):
        """Return the zoom level."""
        pass

    def GotoLine(self, line):
        """Set caret to start of a line and ensure it is visible."""
        pass

    def GotoPos(self, pos):
        """Set caret to a position and ensure it is visible."""
        pass

    def HideLines(self, lineStart, lineEnd):
        """Make a range of lines invisible."""
        pass

    def HideSelection(self, normal):
        """Draw the selection in normal style or with selection
        highlighted."""
        pass

    def HomeDisplay(self):
        """Move caret to first position on display line."""
        pass

    def HomeDisplayExtend(self):
        """Move caret to first position on display line extending
        selection to new caret position."""
        pass

    def IndicatorGetForeground(self, indic):
        """Return the foreground colour of an indicator."""
        pass

    def IndicatorGetStyle(self, indic):
        """Return the style of an indicator."""
        pass

    def IndicatorSetForeground(self, indic, fore):
        """Set the foreground colour of an indicator."""
        pass

    def IndicatorSetStyle(self, indic, style):
        """Set an indicator to plain, squiggle or TT."""
        pass

    def InsertText(self, pos, text):
        """Insert string at a position."""
        pass

    def LineEndDisplay(self):
        """Move caret to last position on display line."""
        pass

    def LineEndDisplayExtend(self):
        """Move caret to last position on display line extending
        selection to new caret position."""
        pass

    def LineFromPosition(self, pos):
        """Return the line containing a position."""
        pass

    def LineLength(self, line):
        """How many characters are on a line, not including end of
        line characters?"""
        pass

    def LineScroll(self, columns, lines):
        """Scroll horizontally and vertically."""
        pass

    def LinesOnScreen(self):
        """Retrieves the number of lines completely visible."""
        pass

    def MarkerAdd(self, line, markerNumber):
        """Add a marker to a line, return an ID that can be used to
        find or delete the marker."""
        pass

    def MarkerDefine(self, markerNumber, markerSymbol,
                     foreground=wx.NullColour, background=wx.NullColour):
        """Set the symbol used for a particular marker number, and
        optionally the fore and background colours."""
        pass

    def MarkerDelete(self, line, markerNumber):
        """Delete a marker from a line."""
        pass

    def MarkerDeleteAll(self, markerNumber):
        """Delete all markers with a particular number from all lines."""
        pass

    def MarkerDeleteHandle(self, handle):
        """Delete a marker."""
        pass

    def MarkerGet(self, line):
        """Return a bit mask of all the markers set on a line."""
        pass

    def MarkerLineFromHandle(self, handle):
        """Return the line number at which a particular marker is
        located."""
        pass

    def MarkerNext(self, lineStart, markerMask):
        """Find the next line after lineStart that includes a marker
        in mask."""
        pass

    def MarkerPrevious(self, lineStart, markerMask):
        """Find the previous line before lineStart that includes a
        marker in mask."""
        pass

    def MarkerSetBackground(self, markerNumber, back):
        """Set the background colour used for a particular marker
        number."""
        pass

    def MarkerSetForeground(self, markerNumber, fore):
        """Set the foreground colour used for a particular marker
        number."""
        pass

    def MoveCaretInsideView(self):
        """Move the caret inside current view if it is not there
        already."""
        pass

    def Paste(self):
        """Paste the contents of the clipboard into the document
        replacing the selection."""
        pass

    def PointFromPosition(self, pos):
        """Return the point in the window where a position is
        displayed."""
        pass

    def PositionFromLine(self, line):
        """Return the position at the start of a line."""
        pass

    def PositionFromPoint(self, pt):
        """Find the position from a point within the window."""
        pass

    def PositionFromPointClose(self, x, y):
        """Find the position from a point within the window but return
        INVALID_POSITION if not close to text."""
        pass

    def Redo(self):
        """Redo the next action on the undo history."""
        pass

    def ReleaseDocument(self, docPointer):
        """Release a reference to the document, deleting document if
        it fades to black."""
        pass

    def ReplaceSelection(self, text):
        """Replace the selected text with the argument text."""
        pass

    def ReplaceTarget(self, text):
        """Replace the target text with the argument text.

        Text is counted so it can contain nulls.  Return the length
        of the replacement text."""
        pass

    def ReplaceTargetRE(self, text):
        """Replace the target text with the argument text after \d
        processing.

        Text is counted so it can contain nulls.  Looks for \d where d
        is between 1 and 9 and replaces these with the strings matched
        in the last search operation which were surrounded by \(self,
        and \).  Return the length of the replacement text including
        any change caused by processing the \d patterns."""
        pass

    def ScrollToColumn(self, column):
        """Scroll enough to make the given column visible."""
        pass

    def ScrollToLine(self, line):
        """Scroll enough to make the given line visible."""
        pass

    def SearchAnchor(self):
        """Sets the current caret position to be the search anchor."""
        pass

    def SearchInTarget(self, text):
        """Search for a counted string in the target and set the
        target to the found range.

        Text is counted so it can contain nulls.  Return length of
        range or -1 for failure in which case target is not moved."""
        pass

    def SearchNext(self, flags, text):
        """Find some text starting at the search anchor.  Does not
        ensure the selection is visible."""
        pass

    def SearchPrev(self, flags, text):
        """Find some text starting at the search anchor and moving
        backwards.  Does not ensure the selection is visible."""
        pass

    def SelectAll(self):
        """Select all the text in the document."""
        pass

    def SelectionIsRectangle(self):
        """Is the selection rectangular?  The alternative is the more
        common stream selection."""
        pass

    def SendMsg(self, msg, wp=0, lp=0):
        """Send a message to Scintilla."""
        pass

    def SetAnchor(self, posAnchor):
        """Set the selection anchor to a position.  The anchor is the
        opposite end of the selection from the caret."""
        pass

    def SetBackSpaceUnIndents(self, bsUnIndents):
        """Sets whether a backspace pressed when caret is within
        indentation unindents."""
        pass

    def SetBufferedDraw(self, buffered):
        """If drawing is buffered then each line of text is drawn into
        a bitmap buffer before drawing it to the screen to avoid
        flicker."""
        pass

    def SetCaretForeground(self, fore):
        """Set the foreground colour of the caret."""
        pass

    def SetCaretLineBack(self, back):
        """Set the colour of the background of the line containing the
        caret."""
        pass

    def SetCaretLineVisible(self, show):
        """Display the background of the line containing the caret in
        a different colour."""
        pass

    def SetCaretPeriod(self, periodMilliseconds):
        """Return the time in milliseconds that the caret is on and
        off. 0 = steady on."""
        pass

    def SetCaretWidth(self, pixelWidth):
        """Set the width of the insert mode caret."""
        pass

    def SetCodePage(self, codePage):
        """Set the code page used to interpret the bytes of the
        document as characters."""
        pass

    def SetControlCharSymbol(self, symbol):
        """Change the way control characters are displayed: If symbol
        is < 32, keep the drawn way, else, use the given character."""
        pass

    def SetCurrentPos(self, pos):
        """Sets the position of the caret."""
        pass

    def SetCursor(self, cursorType):
        """Sets the cursor to one of the SC_CURSOR* values."""
        pass

    def SetDocPointer(self, docPointer):
        """Change the document object used."""
        pass

    def SetEOLMode(self, eolMode):
        """Set the current end of line mode."""
        pass

    def SetEdgeColour(self, edgeColour):
        """Change the colour used in edge indication."""
        pass

    def SetEdgeColumn(self, column):
        """Set the column number of the edge.  If text goes past the
        edge then it is highlighted."""
        pass

    def SetEdgeMode(self, mode):
        """The edge may be displayed by a line (self, EDGE_LINE) or by
        highlighting text that goes beyond it (self, EDGE_BACKGROUND)
        or not displayed at all (self, EDGE_NONE)."""
        pass

    def SetEndAtLastLine(self, endAtLastLine):
        """Sets the scroll range so that maximum scroll position has
        the last line at the bottom of the view (self, default).
        Setting this to false allows scrolling one page below the last
        line."""
        pass

    def SetFoldExpanded(self, line, expanded):
        """Show the children of a header line."""
        pass

    def SetFoldFlags(self, flags):
        """Set some debugging options for folding."""
        pass

    def SetFoldLevel(self, line, level):
        """Set the fold level of a line.  This encodes an integer
        level along with flags indicating whether the line is a header
        and whether it is effectively white space."""
        pass

    def SetHScrollBar(self, bar):
        """Set the horizontal scrollbar to use instead of the one
        that's built-in."""
        pass

    def SetHighlightGuide(self, column):
        """Set the highlighted indentation guide column.  0 = no
        highlighted guide."""
        pass

    def SetIndent(self, indentSize):
        """Set the number of spaces used for one level of indentation."""
        pass

    def SetIndentationGuides(self, show):
        """Show or hide indentation guides."""
        pass

    def SetKeyWords(self, keywordSet, keyWords):
        """Set up the key words used by the lexer."""
        pass

    def SetLastKeydownProcessed(self, val):
        """Set ???."""
        pass

    def SetLayoutCache(self, mode):
        """Sets the degree of caching of layout information."""
        pass

    def SetLexer(self, lexer):
        """Set the lexing language of the document."""
        pass

    def SetLexerLanguage(self, language):
        """Set the lexing language of the document based on string
        name."""
        pass

    def SetLineIndentation(self, line, indentSize):
        """Change the indentation of a line to a number of columns."""
        pass

    def SetLineState(self, line, state):
        """Used to hold extra styling information for each line."""
        pass

    def SetMarginLeft(self, pixelWidth):
        """Sets the size in pixels of the left margin."""
        pass

    def SetMarginMask(self, margin, mask):
        """Set a mask that determines which markers are displayed in a
        margin."""
        pass

    def SetMarginRight(self, pixelWidth):
        """Sets the size in pixels of the right margin."""
        pass

    def SetMarginSensitive(self, margin, sensitive):
        """Make a margin sensitive or insensitive to mouse clicks."""
        pass

    def SetMarginType(self, margin, marginType):
        """Set a margin to be either numeric or symbolic."""
        pass

    def SetMarginWidth(self, margin, pixelWidth):
        """Set the width of a margin to a width expressed in pixels."""
        pass

    def SetMargins(self, left, right):
        """Set the left and right margin in the edit area, measured in
        pixels."""
        pass

    def SetModEventMask(self, mask):
        """Set which document modification events are sent to the
        container."""
        pass

    def SetMouseDownCaptures(self, captures):
        """Set whether the mouse is captured when its button is
        pressed."""
        pass

    def SetMouseDwellTime(self, periodMilliseconds):
        """Sets the time the mouse must sit still to generate a mouse
        dwell event."""
        pass

    def SetOvertype(self, overtype):
        """Set to overtype (self, true) or insert mode."""
        pass

    def SetPrintColourMode(self, mode):
        """Modify colours when printing for clearer printed text."""
        pass

    def SetPrintMagnification(self, magnification):
        """Sets the print magnification added to the point size of
        each style for printing."""
        pass

    def SetProperty(self, key, value):
        """Set up a value that may be used by a lexer for some
        optional feature."""
        pass

    def SetReadOnly(self, readOnly):
        """Set to read only or read write."""
        pass

    def SetSTCFocus(self, focus):
        """Change internal focus flag."""
        pass

    def SetSavePoint(self):
        """Remember the current position in the undo history as the
        position at which the document was saved."""
        pass

    def SetScrollWidth(self, pixelWidth):
        """Sets the document width assumed for scrolling."""
        pass

    def SetSearchFlags(self, flags):
        """Set the search flags used by SearchInTarget."""
        pass

    def SetSelBackground(self, useSetting, back):
        """Set the background colour of the selection and whether to
        use this setting."""
        pass

    def SetSelForeground(self, useSetting, fore):
        """Set the foreground colour of the selection and whether to
        use this setting."""
        pass

    def SetSelection(self, start, end):
        """Select a range of text."""
        pass

    def SetSelectionEnd(self, pos):
        """Sets the position that ends the selection - this becomes
        the currentPosition."""
        pass

    def SetSelectionStart(self, pos):
        """Sets the position that starts the selection - this becomes
        the anchor."""
        pass

    def SetStatus(self, statusCode):
        """Change error status - 0 = OK."""
        pass

    def SetStyleBits(self, bits):
        """Divide each styling byte into lexical class bits (self,
        default: 5) and indicator bits (self, default: 3). If a lexer
        requires more than 32 lexical states, then this is used to
        expand the possible states."""
        pass

    def SetStyleBytes(self, length, styleBytes):
        """Set the styles for a segment of the document."""
        pass

    def SetStyling(self, length, style):
        """Change style from current styling position for length
        characters to a style and move the current styling position to
        after this newly styled segment."""
        pass

    def SetTabIndents(self, tabIndents):
        """Sets whether a tab pressed when caret is within indentation
        indents."""
        pass

    def SetTabWidth(self, tabWidth):
        """Change the visible size of a tab to be a multiple of the
        width of a space character."""
        pass

    def SetTargetEnd(self, pos):
        """Sets the position that ends the target which is used for
        updating the document without affecting the scroll position."""
        pass

    def SetTargetStart(self, pos):
        """Sets the position that starts the target which is used for
        updating the document without affecting the scroll position."""
        pass

    def SetText(self, text):
        """Replace the contents of the document with the argument
        text."""
        pass

    def SetUndoCollection(self, collectUndo):
        """Choose between collecting actions into the undo history and
        discarding them."""
        pass

    def SetUseHorizontalScrollBar(self, show):
        """Show or hide the horizontal scroll bar."""
        pass

    def SetUseTabs(self, useTabs):
        """Indentation will only use space characters if useTabs is
        false, otherwise it will use a combination of tabs and spaces."""
        pass

    def SetVScrollBar(self, bar):
        """Set the vertical scrollbar to use instead of the one that's
        built-in."""
        pass

    def SetViewEOL(self, visible):
        """Make the end of line characters visible or invisible."""
        pass

    def SetViewWhiteSpace(self, viewWS):
        """Make white space characters invisible, always visible or
        visible outside indentation."""
        pass

    def SetVisiblePolicy(self, visiblePolicy, visibleSlop):
        """Set the way the display area is determined when a
        particular line is to be moved to by Find, FindNext, GotoLine,
        etc."""
        pass

    def SetWhitespaceBackground(self, useSetting, back):
        """Set the background colour of all whitespace and whether to
        use this setting."""
        pass

    def SetWhitespaceForeground(self, useSetting, fore):
        """Set the foreground colour of all whitespace and whether to
        use this setting."""
        pass

    def SetWordChars(self, characters):
        """Set the set of characters making up words for when moving
        or selecting by word."""
        pass

    def SetWrapMode(self, mode):
        """Sets whether text is word wrapped."""
        pass

    def SetXCaretPolicy(self, caretPolicy, caretSlop):
        """Set the way the caret is kept visible when going sideway.
        The exclusion zone is given in pixels."""
        pass

    def SetXOffset(self, newOffset):
        """Set the xOffset (self, ie, horizonal scroll position)."""
        pass

    def SetYCaretPolicy(self, caretPolicy, caretSlop):
        """Set the way the line the caret is on is kept visible.  The
        exclusion zone is given in lines."""
        pass

    def SetZoom(self, zoom):
        """Set the zoom level. This number of points is added to the
        size of all fonts.  It may be positive to magnify or negative
        to reduce."""
        pass

    def ShowLines(self, lineStart, lineEnd):
        """Make a range of lines visible."""
        pass

    def StartRecord(self):
        """Start notifying the container of all key presses and
        commands."""
        pass

    def StartStyling(self, pos, mask):
        """Set the current styling position and mask.

        The styling mask can be used to protect some bits in each
        styling byte from modification."""
        pass

    def StopRecord(self):
        """Stop notifying the container of all key presses and
        commands."""
        pass

    def StyleClearAll(self):
        """Clear all the styles and make equivalent to the global
        default style."""
        pass

    def StyleResetDefault(self):
        """Reset the default style to its state at startup."""
        pass

    def StyleSetBackground(self, style, back):
        """Set the background colour of a style."""
        pass

    def StyleSetBold(self, style, bold):
        """Set a style to be bold or not."""
        pass

    def StyleSetCase(self, style, caseForce):
        """Set a style to be mixed case, or to force upper or lower
        case."""
        pass

    def StyleSetChangeable(self, style, changeable):
        """Set a style to be changeable or not (self, read only).
        Experimental feature, currently buggy."""
        pass

    def StyleSetCharacterSet(self, style, characterSet):
        """Set the character set of the font in a style."""
        pass

    def StyleSetEOLFilled(self, style, filled):
        """Set a style to have its end of line filled or not."""
        pass

    def StyleSetFaceName(self, style, fontName):
        """Set the font of a style."""
        pass

    def StyleSetFont(self, styleNum, font):
        """Set style size, face, bold, italic, and underline
        attributes from a Font's attributes."""
        pass

    def StyleSetFontAttr(self, styleNum, size, faceName,
                         bold, italic, underline):
        """Set all font style attributes at once."""
        pass

    def StyleSetForeground(self, style, fore):
        """Set the foreground colour of a style."""
        pass

    def StyleSetItalic(self, style, italic):
        """Set a style to be italic or not."""
        pass

    def StyleSetSize(self, style, sizePoints):
        """Set the size of characters of a style."""
        pass

    def StyleSetSpec(self, styleNum, spec):
        """Extract style settings from a spec-string which is composed
        of one or more of the following comma separated elements:

        bold             turns on bold
        italic           turns on italics
        fore:#RRGGBB     sets the foreground colour
        back:#RRGGBB     sets the background colour
        face:[facename]  sets the font face name to use
        size:[num]       sets the font size in points
        eol              turns on eol filling
        underline        turns on underlining
        """
        pass

    def StyleSetUnderline(self, style, underline):
        """Set a style to be underlined or not."""
        pass

    def StyleSetVisible(self, style, visible):
        """Set a style to be visible or not."""
        pass

    def TextHeight(self, line):
        """Return the height of a particular line of text in pixels."""
        pass

    def TextWidth(self, style, text):
        """Measure the pixel width of some text in a particular style.
        Nul terminated text argument.  Does not handle tab or control
        characters."""
        pass

    def ToggleFold(self, line):
        """Switch a header line between expanded and contracted."""
        pass

    def Undo(self):
        """Undo one action in the undo history."""
        pass

    def UsePopUp(self, allowPopUp):
        """Set whether a pop up menu is displayed automatically when
        the user presses the wrong mouse button."""
        pass

    def UserListShow(self, listType, itemList):
        """Display a list of strings and send notification when user
        chooses one."""
        pass

    def VisibleFromDocLine(self, line):
        """Find the display line of a document line taking hidden
        lines into account."""
        pass

    def WordEndPosition(self, pos, onlyWordCharacters):
        """Return position of end of word."""
        pass

    def WordPartLeft(self):
        """Move to the previous change in capitalisation."""
        pass

    def WordPartLeftExtend(self):
        """Move to the previous change in capitalisation extending
        selection to new caret position."""
        pass

    def WordPartRight(self):
        """Move to the change next in capitalisation."""
        pass

    def WordPartRightExtend(self):
        """Move to the next change in capitalisation extending
        selection to new caret position."""
        pass

    def WordStartPosition(self, pos, onlyWordCharacters):
        """Return position of start of word."""
        pass
