DocStr(wxStyledTextCtrl::AddText,
"Add text to the document at current position.", "");

DocStr(wxStyledTextCtrl::AddStyledText,
"Add array of cells to document.", "");

DocStr(wxStyledTextCtrl::InsertText,
"Insert string at a position.", "");

DocStr(wxStyledTextCtrl::ClearAll,
"Delete all text in the document.", "");

DocStr(wxStyledTextCtrl::ClearDocumentStyle,
"Set all style bytes to 0, remove all folding information.", "");

DocStr(wxStyledTextCtrl::GetLength,
"Returns the number of characters in the document.", "");

DocStr(wxStyledTextCtrl::GetCharAt,
"Returns the character byte at the position.", "");

DocStr(wxStyledTextCtrl::GetCurrentPos,
"Returns the position of the caret.", "");

DocStr(wxStyledTextCtrl::GetAnchor,
"Returns the position of the opposite end of the selection to the caret.", "");

DocStr(wxStyledTextCtrl::GetStyleAt,
"Returns the style byte at the position.", "");

DocStr(wxStyledTextCtrl::Redo,
"Redoes the next action on the undo history.", "");

DocStr(wxStyledTextCtrl::SetUndoCollection,
"Choose between collecting actions into the undo
history and discarding them.", "");

DocStr(wxStyledTextCtrl::SelectAll,
"Select all the text in the document.", "");

DocStr(wxStyledTextCtrl::SetSavePoint,
"Remember the current position in the undo history as the position
at which the document was saved.", "");

DocStr(wxStyledTextCtrl::GetStyledText,
"Retrieve a buffer of cells.", "");

DocStr(wxStyledTextCtrl::CanRedo,
"Are there any redoable actions in the undo history?", "");

DocStr(wxStyledTextCtrl::MarkerLineFromHandle,
"Retrieve the line number at which a particular marker is located.", "");

DocStr(wxStyledTextCtrl::MarkerDeleteHandle,
"Delete a marker.", "");

DocStr(wxStyledTextCtrl::GetUndoCollection,
"Is undo history being collected?", "");

DocStr(wxStyledTextCtrl::GetViewWhiteSpace,
"Are white space characters currently visible?
Returns one of SCWS_* constants.", "");

DocStr(wxStyledTextCtrl::SetViewWhiteSpace,
"Make white space characters invisible, always visible or visible outside indentation.", "");

DocStr(wxStyledTextCtrl::PositionFromPoint,
"Find the position from a point within the window.", "");

DocStr(wxStyledTextCtrl::PositionFromPointClose,
"Find the position from a point within the window but return
INVALID_POSITION if not close to text.", "");

DocStr(wxStyledTextCtrl::GotoLine,
"Set caret to start of a line and ensure it is visible.", "");

DocStr(wxStyledTextCtrl::GotoPos,
"Set caret to a position and ensure it is visible.", "");

DocStr(wxStyledTextCtrl::SetAnchor,
"Set the selection anchor to a position. The anchor is the opposite
end of the selection from the caret.", "");

DocStr(wxStyledTextCtrl::GetCurLine,
"Retrieve the text of the line containing the caret.
Returns the index of the caret on the line.", "");

DocStr(wxStyledTextCtrl::GetEndStyled,
"Retrieve the position of the last correctly styled character.", "");

DocStr(wxStyledTextCtrl::ConvertEOLs,
"Convert all line endings in the document to one mode.", "");

DocStr(wxStyledTextCtrl::GetEOLMode,
"Retrieve the current end of line mode - one of CRLF, CR, or LF.", "");

DocStr(wxStyledTextCtrl::SetEOLMode,
"Set the current end of line mode.", "");

DocStr(wxStyledTextCtrl::StartStyling,
"Set the current styling position to pos and the styling mask to mask.
The styling mask can be used to protect some bits in each styling byte from modification.", "");

DocStr(wxStyledTextCtrl::SetStyling,
"Change style from current styling position for length characters to a style
and move the current styling position to after this newly styled segment.", "");

DocStr(wxStyledTextCtrl::GetBufferedDraw,
"Is drawing done first into a buffer or direct to the screen?", "");

DocStr(wxStyledTextCtrl::SetBufferedDraw,
"If drawing is buffered then each line of text is drawn into a bitmap buffer
before drawing it to the screen to avoid flicker.", "");

DocStr(wxStyledTextCtrl::SetTabWidth,
"Change the visible size of a tab to be a multiple of the width of a space character.", "");

DocStr(wxStyledTextCtrl::GetTabWidth,
"Retrieve the visible size of a tab.", "");

DocStr(wxStyledTextCtrl::SetCodePage,
"Set the code page used to interpret the bytes of the document as characters.", "");

DocStr(wxStyledTextCtrl::MarkerDefine,
"Set the symbol used for a particular marker number,
and optionally the fore and background colours.", "");

DocStr(wxStyledTextCtrl::MarkerSetForeground,
"Set the foreground colour used for a particular marker number.", "");

DocStr(wxStyledTextCtrl::MarkerSetBackground,
"Set the background colour used for a particular marker number.", "");

DocStr(wxStyledTextCtrl::MarkerAdd,
"Add a marker to a line, returning an ID which can be used to find or delete the marker.", "");

DocStr(wxStyledTextCtrl::MarkerDelete,
"Delete a marker from a line.", "");

DocStr(wxStyledTextCtrl::MarkerDeleteAll,
"Delete all markers with a particular number from all lines.", "");

DocStr(wxStyledTextCtrl::MarkerGet,
"Get a bit mask of all the markers set on a line.", "");

DocStr(wxStyledTextCtrl::MarkerNext,
"Find the next line after lineStart that includes a marker in mask.", "");

DocStr(wxStyledTextCtrl::MarkerPrevious,
"Find the previous line before lineStart that includes a marker in mask.", "");

DocStr(wxStyledTextCtrl::MarkerDefineBitmap,
"Define a marker from a bitmap", "");

DocStr(wxStyledTextCtrl::MarkerAddSet,
"Add a set of markers to a line.", "");

DocStr(wxStyledTextCtrl::MarkerSetAlpha,
"Set the alpha used for a marker that is drawn in the text area, not the margin.", "");

DocStr(wxStyledTextCtrl::SetMarginType,
"Set a margin to be either numeric or symbolic.", "");

DocStr(wxStyledTextCtrl::GetMarginType,
"Retrieve the type of a margin.", "");

DocStr(wxStyledTextCtrl::SetMarginWidth,
"Set the width of a margin to a width expressed in pixels.", "");

DocStr(wxStyledTextCtrl::GetMarginWidth,
"Retrieve the width of a margin in pixels.", "");

DocStr(wxStyledTextCtrl::SetMarginMask,
"Set a mask that determines which markers are displayed in a margin.", "");

DocStr(wxStyledTextCtrl::GetMarginMask,
"Retrieve the marker mask of a margin.", "");

DocStr(wxStyledTextCtrl::SetMarginSensitive,
"Make a margin sensitive or insensitive to mouse clicks.", "");

DocStr(wxStyledTextCtrl::GetMarginSensitive,
"Retrieve the mouse click sensitivity of a margin.", "");

DocStr(wxStyledTextCtrl::StyleClearAll,
"Clear all the styles and make equivalent to the global default style.", "");

DocStr(wxStyledTextCtrl::StyleSetForeground,
"Set the foreground colour of a style.", "");

DocStr(wxStyledTextCtrl::StyleSetBackground,
"Set the background colour of a style.", "");

DocStr(wxStyledTextCtrl::StyleSetBold,
"Set a style to be bold or not.", "");

DocStr(wxStyledTextCtrl::StyleSetItalic,
"Set a style to be italic or not.", "");

DocStr(wxStyledTextCtrl::StyleSetSize,
"Set the size of characters of a style.", "");

DocStr(wxStyledTextCtrl::StyleSetFaceName,
"Set the font of a style.", "");

DocStr(wxStyledTextCtrl::StyleSetEOLFilled,
"Set a style to have its end of line filled or not.", "");

DocStr(wxStyledTextCtrl::StyleResetDefault,
"Reset the default style to its state at startup", "");

DocStr(wxStyledTextCtrl::StyleSetUnderline,
"Set a style to be underlined or not.", "");

DocStr(wxStyledTextCtrl::StyleSetCase,
"Set a style to be mixed case, or to force upper or lower case.", "");

DocStr(wxStyledTextCtrl::StyleSetHotSpot,
"Set a style to be a hotspot or not.", "");

DocStr(wxStyledTextCtrl::SetSelForeground,
"Set the foreground colour of the selection and whether to use this setting.", "");

DocStr(wxStyledTextCtrl::SetSelBackground,
"Set the background colour of the selection and whether to use this setting.", "");

DocStr(wxStyledTextCtrl::GetSelAlpha,
"Get the alpha of the selection.", "");

DocStr(wxStyledTextCtrl::SetSelAlpha,
"Set the alpha of the selection.", "");

DocStr(wxStyledTextCtrl::SetCaretForeground,
"Set the foreground colour of the caret.", "");

DocStr(wxStyledTextCtrl::CmdKeyAssign,
"When key+modifier combination km is pressed perform msg.", "");

DocStr(wxStyledTextCtrl::CmdKeyClear,
"When key+modifier combination km is pressed do nothing.", "");

DocStr(wxStyledTextCtrl::CmdKeyClearAll,
"Drop all key mappings.", "");

DocStr(wxStyledTextCtrl::SetStyleBytes,
"Set the styles for a segment of the document.", "");

DocStr(wxStyledTextCtrl::StyleSetVisible,
"Set a style to be visible or not.", "");

DocStr(wxStyledTextCtrl::GetCaretPeriod,
"Get the time in milliseconds that the caret is on and off.", "");

DocStr(wxStyledTextCtrl::SetCaretPeriod,
"Get the time in milliseconds that the caret is on and off. 0 = steady on.", "");

DocStr(wxStyledTextCtrl::SetWordChars,
"Set the set of characters making up words for when moving or selecting by word.
First sets deaults like SetCharsDefault.", "");

DocStr(wxStyledTextCtrl::BeginUndoAction,
"Start a sequence of actions that is undone and redone as a unit.
May be nested.", "");

DocStr(wxStyledTextCtrl::EndUndoAction,
"End a sequence of actions that is undone and redone as a unit.", "");

DocStr(wxStyledTextCtrl::IndicatorSetStyle,
"Set an indicator to plain, squiggle or TT.", "");

DocStr(wxStyledTextCtrl::IndicatorGetStyle,
"Retrieve the style of an indicator.", "");

DocStr(wxStyledTextCtrl::IndicatorSetForeground,
"Set the foreground colour of an indicator.", "");

DocStr(wxStyledTextCtrl::IndicatorGetForeground,
"Retrieve the foreground colour of an indicator.", "");

DocStr(wxStyledTextCtrl::SetWhitespaceForeground,
"Set the foreground colour of all whitespace and whether to use this setting.", "");

DocStr(wxStyledTextCtrl::SetWhitespaceBackground,
"Set the background colour of all whitespace and whether to use this setting.", "");

DocStr(wxStyledTextCtrl::SetStyleBits,
"Divide each styling byte into lexical class bits (default: 5) and indicator
bits (default: 3). If a lexer requires more than 32 lexical states, then this
is used to expand the possible states.", "");

DocStr(wxStyledTextCtrl::GetStyleBits,
"Retrieve number of bits in style bytes used to hold the lexical state.", "");

DocStr(wxStyledTextCtrl::SetLineState,
"Used to hold extra styling information for each line.", "");

DocStr(wxStyledTextCtrl::GetLineState,
"Retrieve the extra styling information for a line.", "");

DocStr(wxStyledTextCtrl::GetMaxLineState,
"Retrieve the last line number that has line state.", "");

DocStr(wxStyledTextCtrl::GetCaretLineVisible,
"Is the background of the line containing the caret in a different colour?", "");

DocStr(wxStyledTextCtrl::SetCaretLineVisible,
"Display the background of the line containing the caret in a different colour.", "");

DocStr(wxStyledTextCtrl::GetCaretLineBackground,
"Get the colour of the background of the line containing the caret.", "");

DocStr(wxStyledTextCtrl::SetCaretLineBackground,
"Set the colour of the background of the line containing the caret.", "");

DocStr(wxStyledTextCtrl::StyleSetChangeable,
"Set a style to be changeable or not (read only).
Experimental feature, currently buggy.", "");

DocStr(wxStyledTextCtrl::AutoCompShow,
"Display a auto-completion list.
The lenEntered parameter indicates how many characters before
the caret should be used to provide context.", "");

DocStr(wxStyledTextCtrl::AutoCompCancel,
"Remove the auto-completion list from the screen.", "");

DocStr(wxStyledTextCtrl::AutoCompActive,
"Is there an auto-completion list visible?", "");

DocStr(wxStyledTextCtrl::AutoCompPosStart,
"Retrieve the position of the caret when the auto-completion list was displayed.", "");

DocStr(wxStyledTextCtrl::AutoCompComplete,
"User has selected an item so remove the list and insert the selection.", "");

DocStr(wxStyledTextCtrl::AutoCompStops,
"Define a set of character that when typed cancel the auto-completion list.", "");

DocStr(wxStyledTextCtrl::AutoCompSetSeparator,
"Change the separator character in the string setting up an auto-completion list.
Default is space but can be changed if items contain space.", "");

DocStr(wxStyledTextCtrl::AutoCompGetSeparator,
"Retrieve the auto-completion list separator character.", "");

DocStr(wxStyledTextCtrl::AutoCompSelect,
"Select the item in the auto-completion list that starts with a string.", "");

DocStr(wxStyledTextCtrl::AutoCompSetCancelAtStart,
"Should the auto-completion list be cancelled if the user backspaces to a
position before where the box was created.", "");

DocStr(wxStyledTextCtrl::AutoCompGetCancelAtStart,
"Retrieve whether auto-completion cancelled by backspacing before start.", "");

DocStr(wxStyledTextCtrl::AutoCompSetFillUps,
"Define a set of characters that when typed will cause the autocompletion to
choose the selected item.", "");

DocStr(wxStyledTextCtrl::AutoCompSetChooseSingle,
"Should a single item auto-completion list automatically choose the item.", "");

DocStr(wxStyledTextCtrl::AutoCompGetChooseSingle,
"Retrieve whether a single item auto-completion list automatically choose the item.", "");

DocStr(wxStyledTextCtrl::AutoCompSetIgnoreCase,
"Set whether case is significant when performing auto-completion searches.", "");

DocStr(wxStyledTextCtrl::AutoCompGetIgnoreCase,
"Retrieve state of ignore case flag.", "");

DocStr(wxStyledTextCtrl::UserListShow,
"Display a list of strings and send notification when user chooses one.", "");

DocStr(wxStyledTextCtrl::AutoCompSetAutoHide,
"Set whether or not autocompletion is hidden automatically when nothing matches.", "");

DocStr(wxStyledTextCtrl::AutoCompGetAutoHide,
"Retrieve whether or not autocompletion is hidden automatically when nothing matches.", "");

DocStr(wxStyledTextCtrl::AutoCompSetDropRestOfWord,
"Set whether or not autocompletion deletes any word characters
after the inserted text upon completion.", "");

DocStr(wxStyledTextCtrl::AutoCompGetDropRestOfWord,
"Retrieve whether or not autocompletion deletes any word characters
after the inserted text upon completion.", "");

DocStr(wxStyledTextCtrl::RegisterImage,
"Register an image for use in autocompletion lists.", "");

DocStr(wxStyledTextCtrl::ClearRegisteredImages,
"Clear all the registered images.", "");

DocStr(wxStyledTextCtrl::AutoCompGetTypeSeparator,
"Retrieve the auto-completion list type-separator character.", "");

DocStr(wxStyledTextCtrl::AutoCompSetTypeSeparator,
"Change the type-separator character in the string setting up an auto-completion list.
Default is '?' but can be changed if items contain '?'.", "");

DocStr(wxStyledTextCtrl::AutoCompSetMaxWidth,
"Set the maximum width, in characters, of auto-completion and user lists.
Set to 0 to autosize to fit longest item, which is the default.", "");

DocStr(wxStyledTextCtrl::AutoCompGetMaxWidth,
"Get the maximum width, in characters, of auto-completion and user lists.", "");

DocStr(wxStyledTextCtrl::AutoCompSetMaxHeight,
"Set the maximum height, in rows, of auto-completion and user lists.
The default is 5 rows.", "");

DocStr(wxStyledTextCtrl::AutoCompGetMaxHeight,
"Set the maximum height, in rows, of auto-completion and user lists.", "");

DocStr(wxStyledTextCtrl::SetIndent,
"Set the number of spaces used for one level of indentation.", "");

DocStr(wxStyledTextCtrl::GetIndent,
"Retrieve indentation size.", "");

DocStr(wxStyledTextCtrl::SetUseTabs,
"Indentation will only use space characters if useTabs is false, otherwise
it will use a combination of tabs and spaces.", "");

DocStr(wxStyledTextCtrl::GetUseTabs,
"Retrieve whether tabs will be used in indentation.", "");

DocStr(wxStyledTextCtrl::SetLineIndentation,
"Change the indentation of a line to a number of columns.", "");

DocStr(wxStyledTextCtrl::GetLineIndentation,
"Retrieve the number of columns that a line is indented.", "");

DocStr(wxStyledTextCtrl::GetLineIndentPosition,
"Retrieve the position before the first non indentation character on a line.", "");

DocStr(wxStyledTextCtrl::GetColumn,
"Retrieve the column number of a position, taking tab width into account.", "");

DocStr(wxStyledTextCtrl::SetUseHorizontalScrollBar,
"Show or hide the horizontal scroll bar.", "");

DocStr(wxStyledTextCtrl::GetUseHorizontalScrollBar,
"Is the horizontal scroll bar visible?", "");

DocStr(wxStyledTextCtrl::SetIndentationGuides,
"Show or hide indentation guides.", "");

DocStr(wxStyledTextCtrl::GetIndentationGuides,
"Are the indentation guides visible?", "");

DocStr(wxStyledTextCtrl::SetHighlightGuide,
"Set the highlighted indentation guide column.
0 = no highlighted guide.", "");

DocStr(wxStyledTextCtrl::GetHighlightGuide,
"Get the highlighted indentation guide column.", "");

DocStr(wxStyledTextCtrl::GetLineEndPosition,
"Get the position after the last visible characters on a line.", "");

DocStr(wxStyledTextCtrl::GetCodePage,
"Get the code page used to interpret the bytes of the document as characters.", "");

DocStr(wxStyledTextCtrl::GetCaretForeground,
"Get the foreground colour of the caret.", "");

DocStr(wxStyledTextCtrl::GetReadOnly,
"In read-only mode?", "");

DocStr(wxStyledTextCtrl::SetCurrentPos,
"Sets the position of the caret.", "");

DocStr(wxStyledTextCtrl::SetSelectionStart,
"Sets the position that starts the selection - this becomes the anchor.", "");

DocStr(wxStyledTextCtrl::GetSelectionStart,
"Returns the position at the start of the selection.", "");

DocStr(wxStyledTextCtrl::SetSelectionEnd,
"Sets the position that ends the selection - this becomes the currentPosition.", "");

DocStr(wxStyledTextCtrl::GetSelectionEnd,
"Returns the position at the end of the selection.", "");

DocStr(wxStyledTextCtrl::SetPrintMagnification,
"Sets the print magnification added to the point size of each style for printing.", "");

DocStr(wxStyledTextCtrl::GetPrintMagnification,
"Returns the print magnification.", "");

DocStr(wxStyledTextCtrl::SetPrintColourMode,
"Modify colours when printing for clearer printed text.", "");

DocStr(wxStyledTextCtrl::GetPrintColourMode,
"Returns the print colour mode.", "");

DocStr(wxStyledTextCtrl::FindText,
"Find some text in the document.", "");

DocStr(wxStyledTextCtrl::FormatRange,
"On Windows, will draw the document into a display context such as a printer.", "");

DocStr(wxStyledTextCtrl::GetFirstVisibleLine,
"Retrieve the display line at the top of the display.", "");

DocStr(wxStyledTextCtrl::GetLine,
"Retrieve the contents of a line.", "");

DocStr(wxStyledTextCtrl::GetLineCount,
"Returns the number of lines in the document. There is always at least one.", "");

DocStr(wxStyledTextCtrl::SetMarginLeft,
"Sets the size in pixels of the left margin.", "");

DocStr(wxStyledTextCtrl::GetMarginLeft,
"Returns the size in pixels of the left margin.", "");

DocStr(wxStyledTextCtrl::SetMarginRight,
"Sets the size in pixels of the right margin.", "");

DocStr(wxStyledTextCtrl::GetMarginRight,
"Returns the size in pixels of the right margin.", "");

DocStr(wxStyledTextCtrl::GetModify,
"Is the document different from when it was last saved?", "");

DocStr(wxStyledTextCtrl::SetSelection,
"Select a range of text.", "");

DocStr(wxStyledTextCtrl::GetSelectedText,
"Retrieve the selected text.", "");

DocStr(wxStyledTextCtrl::GetTextRange,
"Retrieve a range of text.", "");

DocStr(wxStyledTextCtrl::HideSelection,
"Draw the selection in normal style or with selection highlighted.", "");

DocStr(wxStyledTextCtrl::LineFromPosition,
"Retrieve the line containing a position.", "");

DocStr(wxStyledTextCtrl::PositionFromLine,
"Retrieve the position at the start of a line.", "");

DocStr(wxStyledTextCtrl::LineScroll,
"Scroll horizontally and vertically.", "");

DocStr(wxStyledTextCtrl::EnsureCaretVisible,
"Ensure the caret is visible.", "");

DocStr(wxStyledTextCtrl::ReplaceSelection,
"Replace the selected text with the argument text.", "");

DocStr(wxStyledTextCtrl::SetReadOnly,
"Set to read only or read write.", "");

DocStr(wxStyledTextCtrl::CanPaste,
"Will a paste succeed?", "");

DocStr(wxStyledTextCtrl::CanUndo,
"Are there any undoable actions in the undo history?", "");

DocStr(wxStyledTextCtrl::EmptyUndoBuffer,
"Delete the undo history.", "");

DocStr(wxStyledTextCtrl::Undo,
"Undo one action in the undo history.", "");

DocStr(wxStyledTextCtrl::Cut,
"Cut the selection to the clipboard.", "");

DocStr(wxStyledTextCtrl::Copy,
"Copy the selection to the clipboard.", "");

DocStr(wxStyledTextCtrl::Paste,
"Paste the contents of the clipboard into the document replacing the selection.", "");

DocStr(wxStyledTextCtrl::Clear,
"Clear the selection.", "");

DocStr(wxStyledTextCtrl::SetText,
"Replace the contents of the document with the argument text.", "");

DocStr(wxStyledTextCtrl::GetText,
"Retrieve all the text in the document.", "");

DocStr(wxStyledTextCtrl::GetTextLength,
"Retrieve the number of characters in the document.", "");

DocStr(wxStyledTextCtrl::SetOvertype,
"Set to overtype (true) or insert mode.", "");

DocStr(wxStyledTextCtrl::GetOvertype,
"Returns true if overtype mode is active otherwise false is returned.", "");

DocStr(wxStyledTextCtrl::SetCaretWidth,
"Set the width of the insert mode caret.", "");

DocStr(wxStyledTextCtrl::GetCaretWidth,
"Returns the width of the insert mode caret.", "");

DocStr(wxStyledTextCtrl::SetTargetStart,
"Sets the position that starts the target which is used for updating the
document without affecting the scroll position.", "");

DocStr(wxStyledTextCtrl::GetTargetStart,
"Get the position that starts the target.", "");

DocStr(wxStyledTextCtrl::SetTargetEnd,
"Sets the position that ends the target which is used for updating the
document without affecting the scroll position.", "");

DocStr(wxStyledTextCtrl::GetTargetEnd,
"Get the position that ends the target.", "");

DocStr(wxStyledTextCtrl::ReplaceTarget,
"Replace the target text with the argument text.
Text is counted so it can contain NULs.
Returns the length of the replacement text.", "");

DocStr(wxStyledTextCtrl::ReplaceTargetRE,
"Replace the target text with the argument text after \d processing.
Text is counted so it can contain NULs.
Looks for \d where d is between 1 and 9 and replaces these with the strings
matched in the last search operation which were surrounded by \( and \).
Returns the length of the replacement text including any change
caused by processing the \d patterns.", "");

DocStr(wxStyledTextCtrl::SearchInTarget,
"Search for a counted string in the target and set the target to the found
range. Text is counted so it can contain NULs.
Returns length of range or -1 for failure in which case target is not moved.", "");

DocStr(wxStyledTextCtrl::SetSearchFlags,
"Set the search flags used by SearchInTarget.", "");

DocStr(wxStyledTextCtrl::GetSearchFlags,
"Get the search flags used by SearchInTarget.", "");

DocStr(wxStyledTextCtrl::CallTipShow,
"Show a call tip containing a definition near position pos.", "");

DocStr(wxStyledTextCtrl::CallTipCancel,
"Remove the call tip from the screen.", "");

DocStr(wxStyledTextCtrl::CallTipActive,
"Is there an active call tip?", "");

DocStr(wxStyledTextCtrl::CallTipPosAtStart,
"Retrieve the position where the caret was before displaying the call tip.", "");

DocStr(wxStyledTextCtrl::CallTipSetHighlight,
"Highlight a segment of the definition.", "");

DocStr(wxStyledTextCtrl::CallTipSetBackground,
"Set the background colour for the call tip.", "");

DocStr(wxStyledTextCtrl::CallTipSetForeground,
"Set the foreground colour for the call tip.", "");

DocStr(wxStyledTextCtrl::CallTipSetForegroundHighlight,
"Set the foreground colour for the highlighted part of the call tip.", "");

DocStr(wxStyledTextCtrl::CallTipUseStyle,
"Enable use of STYLE_CALLTIP and set call tip tab size in pixels.", "");

DocStr(wxStyledTextCtrl::VisibleFromDocLine,
"Find the display line of a document line taking hidden lines into account.", "");

DocStr(wxStyledTextCtrl::DocLineFromVisible,
"Find the document line of a display line taking hidden lines into account.", "");

DocStr(wxStyledTextCtrl::WrapCount,
"The number of display lines needed to wrap a document line", "");

DocStr(wxStyledTextCtrl::SetFoldLevel,
"Set the fold level of a line.
This encodes an integer level along with flags indicating whether the
line is a header and whether it is effectively white space.", "");

DocStr(wxStyledTextCtrl::GetFoldLevel,
"Retrieve the fold level of a line.", "");

DocStr(wxStyledTextCtrl::GetLastChild,
"Find the last child line of a header line.", "");

DocStr(wxStyledTextCtrl::GetFoldParent,
"Find the parent line of a child line.", "");

DocStr(wxStyledTextCtrl::ShowLines,
"Make a range of lines visible.", "");

DocStr(wxStyledTextCtrl::HideLines,
"Make a range of lines invisible.", "");

DocStr(wxStyledTextCtrl::GetLineVisible,
"Is a line visible?", "");

DocStr(wxStyledTextCtrl::SetFoldExpanded,
"Show the children of a header line.", "");

DocStr(wxStyledTextCtrl::GetFoldExpanded,
"Is a header line expanded?", "");

DocStr(wxStyledTextCtrl::ToggleFold,
"Switch a header line between expanded and contracted.", "");

DocStr(wxStyledTextCtrl::EnsureVisible,
"Ensure a particular line is visible by expanding any header line hiding it.", "");

DocStr(wxStyledTextCtrl::SetFoldFlags,
"Set some style options for folding.", "");

DocStr(wxStyledTextCtrl::EnsureVisibleEnforcePolicy,
"Ensure a particular line is visible by expanding any header line hiding it.
Use the currently set visibility policy to determine which range to display.", "");

DocStr(wxStyledTextCtrl::SetTabIndents,
"Sets whether a tab pressed when caret is within indentation indents.", "");

DocStr(wxStyledTextCtrl::GetTabIndents,
"Does a tab pressed when caret is within indentation indent?", "");

DocStr(wxStyledTextCtrl::SetBackSpaceUnIndents,
"Sets whether a backspace pressed when caret is within indentation unindents.", "");

DocStr(wxStyledTextCtrl::GetBackSpaceUnIndents,
"Does a backspace pressed when caret is within indentation unindent?", "");

DocStr(wxStyledTextCtrl::SetMouseDwellTime,
"Sets the time the mouse must sit still to generate a mouse dwell event.", "");

DocStr(wxStyledTextCtrl::GetMouseDwellTime,
"Retrieve the time the mouse must sit still to generate a mouse dwell event.", "");

DocStr(wxStyledTextCtrl::WordStartPosition,
"Get position of start of word.", "");

DocStr(wxStyledTextCtrl::WordEndPosition,
"Get position of end of word.", "");

DocStr(wxStyledTextCtrl::SetWrapMode,
"Sets whether text is word wrapped.", "");

DocStr(wxStyledTextCtrl::GetWrapMode,
"Retrieve whether text is word wrapped.", "");

DocStr(wxStyledTextCtrl::SetWrapVisualFlags,
"Set the display mode of visual flags for wrapped lines.", "");

DocStr(wxStyledTextCtrl::GetWrapVisualFlags,
"Retrive the display mode of visual flags for wrapped lines.", "");

DocStr(wxStyledTextCtrl::SetWrapVisualFlagsLocation,
"Set the location of visual flags for wrapped lines.", "");

DocStr(wxStyledTextCtrl::GetWrapVisualFlagsLocation,
"Retrive the location of visual flags for wrapped lines.", "");

DocStr(wxStyledTextCtrl::SetWrapStartIndent,
"Set the start indent for wrapped lines.", "");

DocStr(wxStyledTextCtrl::GetWrapStartIndent,
"Retrive the start indent for wrapped lines.", "");

DocStr(wxStyledTextCtrl::SetLayoutCache,
"Sets the degree of caching of layout information.", "");

DocStr(wxStyledTextCtrl::GetLayoutCache,
"Retrieve the degree of caching of layout information.", "");

DocStr(wxStyledTextCtrl::SetScrollWidth,
"Sets the document width assumed for scrolling.", "");

DocStr(wxStyledTextCtrl::GetScrollWidth,
"Retrieve the document width assumed for scrolling.", "");

DocStr(wxStyledTextCtrl::TextWidth,
"Measure the pixel width of some text in a particular style.
NUL terminated text argument.
Does not handle tab or control characters.", "");

DocStr(wxStyledTextCtrl::SetEndAtLastLine,
"Sets the scroll range so that maximum scroll position has
the last line at the bottom of the view (default).
Setting this to false allows scrolling one page below the last line.", "");

DocStr(wxStyledTextCtrl::GetEndAtLastLine,
"Retrieve whether the maximum scroll position has the last
line at the bottom of the view.", "");

DocStr(wxStyledTextCtrl::TextHeight,
"Retrieve the height of a particular line of text in pixels.", "");

DocStr(wxStyledTextCtrl::SetUseVerticalScrollBar,
"Show or hide the vertical scroll bar.", "");

DocStr(wxStyledTextCtrl::GetUseVerticalScrollBar,
"Is the vertical scroll bar visible?", "");

DocStr(wxStyledTextCtrl::AppendText,
"Append a string to the end of the document without changing the selection.", "");

DocStr(wxStyledTextCtrl::GetTwoPhaseDraw,
"Is drawing done in two phases with backgrounds drawn before foregrounds?", "");

DocStr(wxStyledTextCtrl::SetTwoPhaseDraw,
"In twoPhaseDraw mode, drawing is performed in two phases, first the background
and then the foreground. This avoids chopping off characters that overlap the next run.", "");

DocStr(wxStyledTextCtrl::TargetFromSelection,
"Make the target range start and end be the same as the selection range start and end.", "");

DocStr(wxStyledTextCtrl::LinesJoin,
"Join the lines in the target.", "");

DocStr(wxStyledTextCtrl::LinesSplit,
"Split the lines in the target into lines that are less wide than pixelWidth
where possible.", "");

DocStr(wxStyledTextCtrl::SetFoldMarginColour,
"Set the colours used as a chequerboard pattern in the fold margin", "");

DocStr(wxStyledTextCtrl::SetFoldMarginHiColour,
"", "");

DocStr(wxStyledTextCtrl::LineDown,
"Move caret down one line.", "");

DocStr(wxStyledTextCtrl::LineDownExtend,
"Move caret down one line extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::LineUp,
"Move caret up one line.", "");

DocStr(wxStyledTextCtrl::LineUpExtend,
"Move caret up one line extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::CharLeft,
"Move caret left one character.", "");

DocStr(wxStyledTextCtrl::CharLeftExtend,
"Move caret left one character extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::CharRight,
"Move caret right one character.", "");

DocStr(wxStyledTextCtrl::CharRightExtend,
"Move caret right one character extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::WordLeft,
"Move caret left one word.", "");

DocStr(wxStyledTextCtrl::WordLeftExtend,
"Move caret left one word extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::WordRight,
"Move caret right one word.", "");

DocStr(wxStyledTextCtrl::WordRightExtend,
"Move caret right one word extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::Home,
"Move caret to first position on line.", "");

DocStr(wxStyledTextCtrl::HomeExtend,
"Move caret to first position on line extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::LineEnd,
"Move caret to last position on line.", "");

DocStr(wxStyledTextCtrl::LineEndExtend,
"Move caret to last position on line extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::DocumentStart,
"Move caret to first position in document.", "");

DocStr(wxStyledTextCtrl::DocumentStartExtend,
"Move caret to first position in document extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::DocumentEnd,
"Move caret to last position in document.", "");

DocStr(wxStyledTextCtrl::DocumentEndExtend,
"Move caret to last position in document extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::PageUp,
"Move caret one page up.", "");

DocStr(wxStyledTextCtrl::PageUpExtend,
"Move caret one page up extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::PageDown,
"Move caret one page down.", "");

DocStr(wxStyledTextCtrl::PageDownExtend,
"Move caret one page down extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::EditToggleOvertype,
"Switch from insert to overtype mode or the reverse.", "");

DocStr(wxStyledTextCtrl::Cancel,
"Cancel any modes such as call tip or auto-completion list display.", "");

DocStr(wxStyledTextCtrl::DeleteBack,
"Delete the selection or if no selection, the character before the caret.", "");

DocStr(wxStyledTextCtrl::Tab,
"If selection is empty or all on one line replace the selection with a tab character.
If more than one line selected, indent the lines.", "");

DocStr(wxStyledTextCtrl::BackTab,
"Dedent the selected lines.", "");

DocStr(wxStyledTextCtrl::NewLine,
"Insert a new line, may use a CRLF, CR or LF depending on EOL mode.", "");

DocStr(wxStyledTextCtrl::FormFeed,
"Insert a Form Feed character.", "");

DocStr(wxStyledTextCtrl::VCHome,
"Move caret to before first visible character on line.
If already there move to first character on line.", "");

DocStr(wxStyledTextCtrl::VCHomeExtend,
"Like VCHome but extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::ZoomIn,
"Magnify the displayed text by increasing the sizes by 1 point.", "");

DocStr(wxStyledTextCtrl::ZoomOut,
"Make the displayed text smaller by decreasing the sizes by 1 point.", "");

DocStr(wxStyledTextCtrl::DelWordLeft,
"Delete the word to the left of the caret.", "");

DocStr(wxStyledTextCtrl::DelWordRight,
"Delete the word to the right of the caret.", "");

DocStr(wxStyledTextCtrl::LineCut,
"Cut the line containing the caret.", "");

DocStr(wxStyledTextCtrl::LineDelete,
"Delete the line containing the caret.", "");

DocStr(wxStyledTextCtrl::LineTranspose,
"Switch the current line with the previous.", "");

DocStr(wxStyledTextCtrl::LineDuplicate,
"Duplicate the current line.", "");

DocStr(wxStyledTextCtrl::LowerCase,
"Transform the selection to lower case.", "");

DocStr(wxStyledTextCtrl::UpperCase,
"Transform the selection to upper case.", "");

DocStr(wxStyledTextCtrl::LineScrollDown,
"Scroll the document down, keeping the caret visible.", "");

DocStr(wxStyledTextCtrl::LineScrollUp,
"Scroll the document up, keeping the caret visible.", "");

DocStr(wxStyledTextCtrl::DeleteBackNotLine,
"Delete the selection or if no selection, the character before the caret.
Will not delete the character before at the start of a line.", "");

DocStr(wxStyledTextCtrl::HomeDisplay,
"Move caret to first position on display line.", "");

DocStr(wxStyledTextCtrl::HomeDisplayExtend,
"Move caret to first position on display line extending selection to
new caret position.", "");

DocStr(wxStyledTextCtrl::LineEndDisplay,
"Move caret to last position on display line.", "");

DocStr(wxStyledTextCtrl::LineEndDisplayExtend,
"Move caret to last position on display line extending selection to new
caret position.", "");

DocStr(wxStyledTextCtrl::HomeWrap,
"These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
except they behave differently when word-wrap is enabled:
They go first to the start / end of the display line, like (Home|LineEnd)Display
The difference is that, the cursor is already at the point, it goes on to the start
or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.", "");

DocStr(wxStyledTextCtrl::HomeWrapExtend,
"", "");

DocStr(wxStyledTextCtrl::LineEndWrap,
"", "");

DocStr(wxStyledTextCtrl::LineEndWrapExtend,
"", "");

DocStr(wxStyledTextCtrl::VCHomeWrap,
"", "");

DocStr(wxStyledTextCtrl::VCHomeWrapExtend,
"", "");

DocStr(wxStyledTextCtrl::LineCopy,
"Copy the line containing the caret.", "");

DocStr(wxStyledTextCtrl::MoveCaretInsideView,
"Move the caret inside current view if it's not there already.", "");

DocStr(wxStyledTextCtrl::LineLength,
"How many characters are on a line, not including end of line characters?", "");

DocStr(wxStyledTextCtrl::BraceHighlight,
"Highlight the characters at two positions.", "");

DocStr(wxStyledTextCtrl::BraceBadLight,
"Highlight the character at a position indicating there is no matching brace.", "");

DocStr(wxStyledTextCtrl::BraceMatch,
"Find the position of a matching brace or INVALID_POSITION if no match.", "");

DocStr(wxStyledTextCtrl::GetViewEOL,
"Are the end of line characters visible?", "");

DocStr(wxStyledTextCtrl::SetViewEOL,
"Make the end of line characters visible or invisible.", "");

DocStr(wxStyledTextCtrl::GetDocPointer,
"Retrieve a pointer to the document object.", "");

DocStr(wxStyledTextCtrl::SetDocPointer,
"Change the document object used.", "");

DocStr(wxStyledTextCtrl::SetModEventMask,
"Set which document modification events are sent to the container.", "");

DocStr(wxStyledTextCtrl::GetEdgeColumn,
"Retrieve the column number which text should be kept within.", "");

DocStr(wxStyledTextCtrl::SetEdgeColumn,
"Set the column number of the edge.
If text goes past the edge then it is highlighted.", "");

DocStr(wxStyledTextCtrl::GetEdgeMode,
"Retrieve the edge highlight mode.", "");

DocStr(wxStyledTextCtrl::SetEdgeMode,
"The edge may be displayed by a line (EDGE_LINE) or by highlighting text that
goes beyond it (EDGE_BACKGROUND) or not displayed at all (EDGE_NONE).", "");

DocStr(wxStyledTextCtrl::GetEdgeColour,
"Retrieve the colour used in edge indication.", "");

DocStr(wxStyledTextCtrl::SetEdgeColour,
"Change the colour used in edge indication.", "");

DocStr(wxStyledTextCtrl::SearchAnchor,
"Sets the current caret position to be the search anchor.", "");

DocStr(wxStyledTextCtrl::SearchNext,
"Find some text starting at the search anchor.
Does not ensure the selection is visible.", "");

DocStr(wxStyledTextCtrl::SearchPrev,
"Find some text starting at the search anchor and moving backwards.
Does not ensure the selection is visible.", "");

DocStr(wxStyledTextCtrl::LinesOnScreen,
"Retrieves the number of lines completely visible.", "");

DocStr(wxStyledTextCtrl::UsePopUp,
"Set whether a pop up menu is displayed automatically when the user presses
the wrong mouse button.", "");

DocStr(wxStyledTextCtrl::SelectionIsRectangle,
"Is the selection rectangular? The alternative is the more common stream selection.", "");

DocStr(wxStyledTextCtrl::SetZoom,
"Set the zoom level. This number of points is added to the size of all fonts.
It may be positive to magnify or negative to reduce.", "");

DocStr(wxStyledTextCtrl::GetZoom,
"Retrieve the zoom level.", "");

DocStr(wxStyledTextCtrl::CreateDocument,
"Create a new document object.
Starts with reference count of 1 and not selected into editor.", "");

DocStr(wxStyledTextCtrl::AddRefDocument,
"Extend life of document.", "");

DocStr(wxStyledTextCtrl::ReleaseDocument,
"Release a reference to the document, deleting document if it fades to black.", "");

DocStr(wxStyledTextCtrl::GetModEventMask,
"Get which document modification events are sent to the container.", "");

DocStr(wxStyledTextCtrl::SetSTCFocus,
"Change internal focus flag.", "");

DocStr(wxStyledTextCtrl::GetSTCFocus,
"Get internal focus flag.", "");

DocStr(wxStyledTextCtrl::SetStatus,
"Change error status - 0 = OK.", "");

DocStr(wxStyledTextCtrl::GetStatus,
"Get error status.", "");

DocStr(wxStyledTextCtrl::SetMouseDownCaptures,
"Set whether the mouse is captured when its button is pressed.", "");

DocStr(wxStyledTextCtrl::GetMouseDownCaptures,
"Get whether mouse gets captured.", "");

DocStr(wxStyledTextCtrl::SetSTCCursor,
"Sets the cursor to one of the SC_CURSOR* values.", "");

DocStr(wxStyledTextCtrl::GetSTCCursor,
"Get cursor type.", "");

DocStr(wxStyledTextCtrl::SetControlCharSymbol,
"Change the way control characters are displayed:
If symbol is < 32, keep the drawn way, else, use the given character.", "");

DocStr(wxStyledTextCtrl::GetControlCharSymbol,
"Get the way control characters are displayed.", "");

DocStr(wxStyledTextCtrl::WordPartLeft,
"Move to the previous change in capitalisation.", "");

DocStr(wxStyledTextCtrl::WordPartLeftExtend,
"Move to the previous change in capitalisation extending selection
to new caret position.", "");

DocStr(wxStyledTextCtrl::WordPartRight,
"Move to the change next in capitalisation.", "");

DocStr(wxStyledTextCtrl::WordPartRightExtend,
"Move to the next change in capitalisation extending selection
to new caret position.", "");

DocStr(wxStyledTextCtrl::SetVisiblePolicy,
"Set the way the display area is determined when a particular line
is to be moved to by Find, FindNext, GotoLine, etc.", "");

DocStr(wxStyledTextCtrl::DelLineLeft,
"Delete back from the current position to the start of the line.", "");

DocStr(wxStyledTextCtrl::DelLineRight,
"Delete forwards from the current position to the end of the line.", "");

DocStr(wxStyledTextCtrl::SetXOffset,
"Get and Set the xOffset (ie, horizonal scroll position).", "");

DocStr(wxStyledTextCtrl::GetXOffset,
"", "");

DocStr(wxStyledTextCtrl::ChooseCaretX,
"Set the last x chosen value to be the caret x position.", "");

DocStr(wxStyledTextCtrl::SetXCaretPolicy,
"Set the way the caret is kept visible when going sideway.
The exclusion zone is given in pixels.", "");

DocStr(wxStyledTextCtrl::SetYCaretPolicy,
"Set the way the line the caret is on is kept visible.
The exclusion zone is given in lines.", "");

DocStr(wxStyledTextCtrl::SetPrintWrapMode,
"Set printing to line wrapped (SC_WRAP_WORD) or not line wrapped (SC_WRAP_NONE).", "");

DocStr(wxStyledTextCtrl::GetPrintWrapMode,
"Is printing line wrapped?", "");

DocStr(wxStyledTextCtrl::SetHotspotActiveForeground,
"Set a fore colour for active hotspots.", "");

DocStr(wxStyledTextCtrl::SetHotspotActiveBackground,
"Set a back colour for active hotspots.", "");

DocStr(wxStyledTextCtrl::SetHotspotActiveUnderline,
"Enable / Disable underlining active hotspots.", "");

DocStr(wxStyledTextCtrl::SetHotspotSingleLine,
"Limit hotspots to single line so hotspots on two lines don't merge.", "");

DocStr(wxStyledTextCtrl::ParaDown,
"Move caret between paragraphs (delimited by empty lines).", "");

DocStr(wxStyledTextCtrl::ParaDownExtend,
"", "");

DocStr(wxStyledTextCtrl::ParaUp,
"", "");

DocStr(wxStyledTextCtrl::ParaUpExtend,
"", "");

DocStr(wxStyledTextCtrl::PositionBefore,
"Given a valid document position, return the previous position taking code
page into account. Returns 0 if passed 0.", "");

DocStr(wxStyledTextCtrl::PositionAfter,
"Given a valid document position, return the next position taking code
page into account. Maximum value returned is the last position in the document.", "");

DocStr(wxStyledTextCtrl::CopyRange,
"Copy a range of text to the clipboard. Positions are clipped into the document.", "");

DocStr(wxStyledTextCtrl::CopyText,
"Copy argument text to the clipboard.", "");

DocStr(wxStyledTextCtrl::SetSelectionMode,
"Set the selection mode to stream (SC_SEL_STREAM) or rectangular (SC_SEL_RECTANGLE) or
by lines (SC_SEL_LINES).", "");

DocStr(wxStyledTextCtrl::GetSelectionMode,
"Get the mode of the current selection.", "");

DocStr(wxStyledTextCtrl::GetLineSelStartPosition,
"Retrieve the position of the start of the selection at the given line (INVALID_POSITION if no selection on this line).", "");

DocStr(wxStyledTextCtrl::GetLineSelEndPosition,
"Retrieve the position of the end of the selection at the given line (INVALID_POSITION if no selection on this line).", "");

DocStr(wxStyledTextCtrl::LineDownRectExtend,
"Move caret down one line, extending rectangular selection to new caret position.", "");

DocStr(wxStyledTextCtrl::LineUpRectExtend,
"Move caret up one line, extending rectangular selection to new caret position.", "");

DocStr(wxStyledTextCtrl::CharLeftRectExtend,
"Move caret left one character, extending rectangular selection to new caret position.", "");

DocStr(wxStyledTextCtrl::CharRightRectExtend,
"Move caret right one character, extending rectangular selection to new caret position.", "");

DocStr(wxStyledTextCtrl::HomeRectExtend,
"Move caret to first position on line, extending rectangular selection to new caret position.", "");

DocStr(wxStyledTextCtrl::VCHomeRectExtend,
"Move caret to before first visible character on line.
If already there move to first character on line.
In either case, extend rectangular selection to new caret position.", "");

DocStr(wxStyledTextCtrl::LineEndRectExtend,
"Move caret to last position on line, extending rectangular selection to new caret position.", "");

DocStr(wxStyledTextCtrl::PageUpRectExtend,
"Move caret one page up, extending rectangular selection to new caret position.", "");

DocStr(wxStyledTextCtrl::PageDownRectExtend,
"Move caret one page down, extending rectangular selection to new caret position.", "");

DocStr(wxStyledTextCtrl::StutteredPageUp,
"Move caret to top of page, or one page up if already at top of page.", "");

DocStr(wxStyledTextCtrl::StutteredPageUpExtend,
"Move caret to top of page, or one page up if already at top of page, extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::StutteredPageDown,
"Move caret to bottom of page, or one page down if already at bottom of page.", "");

DocStr(wxStyledTextCtrl::StutteredPageDownExtend,
"Move caret to bottom of page, or one page down if already at bottom of page, extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::WordLeftEnd,
"Move caret left one word, position cursor at end of word.", "");

DocStr(wxStyledTextCtrl::WordLeftEndExtend,
"Move caret left one word, position cursor at end of word, extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::WordRightEnd,
"Move caret right one word, position cursor at end of word.", "");

DocStr(wxStyledTextCtrl::WordRightEndExtend,
"Move caret right one word, position cursor at end of word, extending selection to new caret position.", "");

DocStr(wxStyledTextCtrl::SetWhitespaceChars,
"Set the set of characters making up whitespace for when moving or selecting by word.
Should be called after SetWordChars.", "");

DocStr(wxStyledTextCtrl::SetCharsDefault,
"Reset the set of characters for whitespace and word characters to the defaults.", "");

DocStr(wxStyledTextCtrl::AutoCompGetCurrent,
"Get currently selected item position in the auto-completion list", "");

DocStr(wxStyledTextCtrl::Allocate,
"Enlarge the document to a particular size of text bytes.", "");

DocStr(wxStyledTextCtrl::FindColumn,
"Find the position of a column on a line taking into account tabs and
multi-byte characters. If beyond end of line, return line end position.", "");

DocStr(wxStyledTextCtrl::GetCaretSticky,
"Can the caret preferred x position only be changed by explicit movement commands?", "");

DocStr(wxStyledTextCtrl::SetCaretSticky,
"Stop the caret preferred x position changing when the user types.", "");

DocStr(wxStyledTextCtrl::ToggleCaretSticky,
"Switch between sticky and non-sticky: meant to be bound to a key.", "");

DocStr(wxStyledTextCtrl::SetPasteConvertEndings,
"Enable/Disable convert-on-paste for line endings", "");

DocStr(wxStyledTextCtrl::GetPasteConvertEndings,
"Get convert-on-paste setting", "");

DocStr(wxStyledTextCtrl::SelectionDuplicate,
"Duplicate the selection. If selection empty duplicate the line containing the caret.", "");

DocStr(wxStyledTextCtrl::SetCaretLineBackAlpha,
"Set background alpha of the caret line.", "");

DocStr(wxStyledTextCtrl::GetCaretLineBackAlpha,
"Get the background alpha of the caret line.", "");

DocStr(wxStyledTextCtrl::StartRecord,
"Start notifying the container of all key presses and commands.", "");

DocStr(wxStyledTextCtrl::StopRecord,
"Stop notifying the container of all key presses and commands.", "");

DocStr(wxStyledTextCtrl::SetLexer,
"Set the lexing language of the document.", "");

DocStr(wxStyledTextCtrl::GetLexer,
"Retrieve the lexing language of the document.", "");

DocStr(wxStyledTextCtrl::Colourise,
"Colourise a segment of the document using the current lexing language.", "");

DocStr(wxStyledTextCtrl::SetProperty,
"Set up a value that may be used by a lexer for some optional feature.", "");

DocStr(wxStyledTextCtrl::SetKeyWords,
"Set up the key words used by the lexer.", "");

DocStr(wxStyledTextCtrl::SetLexerLanguage,
"Set the lexing language of the document based on string name.", "");

DocStr(wxStyledTextCtrl::GetProperty,
"Retrieve a 'property' value previously set with SetProperty.", "");

DocStr(wxStyledTextCtrl::GetPropertyExpanded,
"Retrieve a 'property' value previously set with SetProperty,
with '$()' variable replacement on returned buffer.", "");

DocStr(wxStyledTextCtrl::GetPropertyInt,
"Retrieve a 'property' value previously set with SetProperty,
interpreted as an int AFTER any '$()' variable replacement.", "");

DocStr(wxStyledTextCtrl::GetStyleBitsNeeded,
"Retrieve the number of bits the current lexer needs for styling.", "");
