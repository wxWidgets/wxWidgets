#!/usr/bin/env python
#----------------------------------------------------------------------------
# Name:         src/stc/gen_docs.py
# Purpose:      Helper function and data used by gen_iface.py to generate
#               docstrings for wxStyledTextCtrl methods.
# Author:       New Pagodi
# Created:      2017-02-24
# Copyright:    (c) 2017 wxWidgets development team
# Licence:      wxWindows licence
#----------------------------------------------------------------------------


# List of the categories for the documentation.  Tuple items are:
#
#         1. the brief name for this category (this must be 1 word)
#         2. the full name of this category.  This will appear in the documentation.
#         3. the description of this category or 0 for none.
#
categoriesList = [
    ('TextRetrievalAndModification'    ,'Text retrieval and modification', 0),
    ('Searching'                       ,'Searching and replacing', 0),
    ('Overtype'                        ,'Overtype', 0),
    ('CutCopyAndPaste'                 ,'Cut copy and paste', 0),
    ('ErrorHandling'                   ,'Error handling', 0),
    ('UndoAndRedo'                     ,'Undo and Redo', 0),
    ('SelectionAndInformation'         ,'Selection and information', 0),
    ('MultipleSelectionAndVirtualSpace','Multiple Selection and Virtual Space', 0),
    ('ScrollingAndAutomaticScrolling'  ,'Scrolling and automatic scrolling', 0),
    ('WhiteSpace'                      ,'White space', 0),
    ('Cursor'                          ,'Cursor', 0),
    ('MouseCapture'                    ,'Mouse capture', 0),
    ('LineEndings'                     ,'Line endings', 0),
    ('Words'                           ,'Words', 0),
    ('Styling'                         ,'Styling', 0),
    ('StyleDefinition'                 ,'Style definition', 0),
    ('CaretAndSelectionStyles'         ,'Caret selection and hotspot styles', 0),
    ('CharacterRepresentations'        ,'Character representations', 0),
    ('Margins'                         ,'Margins', 0),
    ('Annotations'                     ,'Annotations', 0),
    ('OtherSettings'                   ,'Other settings', 0),
    ('BraceHighlighting'               ,'Brace highlighting', 0),
    ('TabsAndIndentationGuides'        ,'Tabs and Indentation Guides', 0),
    ('Markers'                         ,'Markers',
        ('@see MarkerDefineBitmap',)),
    ('Indicators'                      ,'Indicators', 0),
    ('Autocompletion'                  ,'Autocompletion',
        ('@see RegisterImage(int, const wxBitmap&)',)),
    ('UserLists'                       ,'User lists', 0),
    ('CallTips'                        ,'Call tips', 0),
    ('KeyboardCommands'                ,'Keyboard commands', 0),
    ('KeyBindings'                     ,'Key bindings', 0),
    ('PopupEditMenu'                   ,'Popup edit menu', 0),
    ('MacroRecording'                  ,'Macro recording', 0),
    ('Printing'                        ,'Printing', 0),
    ('DirectAccess'                    ,'Direct access', 0),
    ('MultipleViews'                   ,'Multiple views', 0),
    ('BackgroundLoadSave'              ,'Background loading and saving', 0),
    ('Folding'                         ,'Folding', 0),
    ('LineWrapping'                    ,'Line wrapping', 0),
    ('Zooming'                         ,'Zooming', 0),
    ('LongLines'                       ,'Long lines', 0),
    ('Lexer'                           ,'Lexer', 0),
    ('Notifications'                   ,'Event related items', 0),
    ('DeprecatedMessages'              ,'Deprecated items',
        ('These methods should not be used in new code.',))
]

# Assign each method to a category
docsMap = {
    'AnnotationClearAll':'Annotations',
    'AnnotationGetLines':'Annotations',
    'AnnotationGetStyle':'Annotations',
    'AnnotationGetStyleOffset':'Annotations',
    'AnnotationGetStyles':'Annotations',
    'AnnotationGetText':'Annotations',
    'AnnotationGetVisible':'Annotations',
    'AnnotationSetStyle':'Annotations',
    'AnnotationSetStyleOffset':'Annotations',
    'AnnotationSetStyles':'Annotations',
    'AnnotationSetText':'Annotations',
    'AnnotationSetVisible':'Annotations',
    'AutoCActive':'Autocompletion',
    'AutoCCancel':'Autocompletion',
    'AutoCComplete':'Autocompletion',
    'AutoCGetAutoHide':'Autocompletion',
    'AutoCGetCancelAtStart':'Autocompletion',
    'AutoCGetCaseInsensitiveBehaviour':'Autocompletion',
    'AutoCGetChooseSingle':'Autocompletion',
    'AutoCGetCurrent':'Autocompletion',
    'AutoCGetCurrentText':'Autocompletion',
    'AutoCGetDropRestOfWord':'Autocompletion',
    'AutoCGetIgnoreCase':'Autocompletion',
    'AutoCGetMaxHeight':'Autocompletion',
    'AutoCGetMaxWidth':'Autocompletion',
    'AutoCGetMulti':'Autocompletion',
    'AutoCGetOrder':'Autocompletion',
    'AutoCGetSeparator':'Autocompletion',
    'AutoCGetTypeSeparator':'Autocompletion',
    'AutoCPosStart':'Autocompletion',
    'AutoCSelect':'Autocompletion',
    'AutoCSetAutoHide':'Autocompletion',
    'AutoCSetCancelAtStart':'Autocompletion',
    'AutoCSetCaseInsensitiveBehaviour':'Autocompletion',
    'AutoCSetChooseSingle':'Autocompletion',
    'AutoCSetDropRestOfWord':'Autocompletion',
    'AutoCSetFillUps':'Autocompletion',
    'AutoCSetIgnoreCase':'Autocompletion',
    'AutoCSetMaxHeight':'Autocompletion',
    'AutoCSetMaxWidth':'Autocompletion',
    'AutoCSetMulti':'Autocompletion',
    'AutoCSetOrder':'Autocompletion',
    'AutoCSetSeparator':'Autocompletion',
    'AutoCSetTypeSeparator':'Autocompletion',
    'AutoCShow':'Autocompletion',
    'AutoCStops':'Autocompletion',
    'ClearRegisteredImages':'Autocompletion',
    'RegisterImage':'Autocompletion',
    'RegisterRGBAImage':'Autocompletion',
    'CreateLoader':'BackgroundLoadSave',
    'BraceBadLight':'BraceHighlighting',
    'BraceBadLightIndicator':'BraceHighlighting',
    'BraceHighlight':'BraceHighlighting',
    'BraceHighlightIndicator':'BraceHighlighting',
    'BraceMatch':'BraceHighlighting',
    'CallTipActive':'CallTips',
    'CallTipCancel':'CallTips',
    'CallTipPosStart':'CallTips',
    'CallTipSetBack':'CallTips',
    'CallTipSetFore':'CallTips',
    'CallTipSetForeHlt':'CallTips',
    'CallTipSetHlt':'CallTips',
    'CallTipSetPosition':'CallTips',
    'CallTipSetPosStart':'CallTips',
    'CallTipShow':'CallTips',
    'CallTipUseStyle':'CallTips',
    'GetCaretFore':'CaretAndSelectionStyles',
    'GetCaretLineBack':'CaretAndSelectionStyles',
    'GetCaretLineBackAlpha':'CaretAndSelectionStyles',
    'GetCaretLineVisible':'CaretAndSelectionStyles',
    'GetCaretLineVisibleAlways':'CaretAndSelectionStyles',
    'GetCaretPeriod':'CaretAndSelectionStyles',
    'GetCaretSticky':'CaretAndSelectionStyles',
    'GetCaretStyle':'CaretAndSelectionStyles',
    'GetCaretWidth':'CaretAndSelectionStyles',
    'GetHotspotActiveBack':'CaretAndSelectionStyles',
    'GetHotspotActiveFore':'CaretAndSelectionStyles',
    'GetHotspotActiveUnderline':'CaretAndSelectionStyles',
    'GetHotspotSingleLine':'CaretAndSelectionStyles',
    'GetSelAlpha':'CaretAndSelectionStyles',
    'GetSelEOLFilled':'CaretAndSelectionStyles',
    'SetCaretFore':'CaretAndSelectionStyles',
    'SetCaretLineBack':'CaretAndSelectionStyles',
    'SetCaretLineBackAlpha':'CaretAndSelectionStyles',
    'SetCaretLineVisible':'CaretAndSelectionStyles',
    'SetCaretLineVisibleAlways':'CaretAndSelectionStyles',
    'SetCaretPeriod':'CaretAndSelectionStyles',
    'SetCaretSticky':'CaretAndSelectionStyles',
    'SetCaretStyle':'CaretAndSelectionStyles',
    'SetCaretWidth':'CaretAndSelectionStyles',
    'SetHotspotActiveBack':'CaretAndSelectionStyles',
    'SetHotspotActiveFore':'CaretAndSelectionStyles',
    'SetHotspotActiveUnderline':'CaretAndSelectionStyles',
    'SetHotspotSingleLine':'CaretAndSelectionStyles',
    'SetSelAlpha':'CaretAndSelectionStyles',
    'SetSelBack':'CaretAndSelectionStyles',
    'SetSelEOLFilled':'CaretAndSelectionStyles',
    'SetSelFore':'CaretAndSelectionStyles',
    'ToggleCaretSticky':'CaretAndSelectionStyles',
    'ClearRepresentation':'CharacterRepresentations',
    'GetControlCharSymbol':'CharacterRepresentations',
    'GetRepresentation':'CharacterRepresentations',
    'SetControlCharSymbol':'CharacterRepresentations',
    'SetRepresentation':'CharacterRepresentations',
    'GetCursor':'Cursor',
    'SetCursor':'Cursor',
    'CanPaste':'CutCopyAndPaste',
    'Clear':'CutCopyAndPaste',
    'Copy':'CutCopyAndPaste',
    'CopyAllowLine':'CutCopyAndPaste',
    'CopyRange':'CutCopyAndPaste',
    'CopyText':'CutCopyAndPaste',
    'Cut':'CutCopyAndPaste',
    'GetPasteConvertEndings':'CutCopyAndPaste',
    'Paste':'CutCopyAndPaste',
    'SetPasteConvertEndings':'CutCopyAndPaste',
    'GetKeysUnicode':'DeprecatedMessages',
    'GetStyleBits':'DeprecatedMessages',
    'GetStyleBitsNeeded':'DeprecatedMessages',
    'SetKeysUnicode':'DeprecatedMessages',
    'SetStyleBits':'DeprecatedMessages',
    'GetCharacterPointer':'DirectAccess',
    'GetDirectFunction':'DirectAccess',
    'GetDirectPointer':'DirectAccess',
    'GetGapPosition':'DirectAccess',
    'GetRangePointer':'DirectAccess',
    'GetStatus':'ErrorHandling',
    'SetStatus':'ErrorHandling',
    'ContractedFoldNext':'Folding',
    'DocLineFromVisible':'Folding',
    'EnsureVisible':'Folding',
    'EnsureVisibleEnforcePolicy':'Folding',
    'ExpandChildren':'Folding',
    'FoldAll':'Folding',
    'FoldChildren':'Folding',
    'FoldDisplayTextSetStyle':'Folding',
    'FoldLine':'Folding',
    'GetAllLinesVisible':'Folding',
    'GetAutomaticFold':'Folding',
    'GetFoldExpanded':'Folding',
    'GetFoldLevel':'Folding',
    'GetFoldParent':'Folding',
    'GetLastChild':'Folding',
    'GetLineVisible':'Folding',
    'HideLines':'Folding',
    'SetAutomaticFold':'Folding',
    'SetFoldExpanded':'Folding',
    'SetFoldFlags':'Folding',
    'SetFoldLevel':'Folding',
    'ShowLines':'Folding',
    'ToggleFold':'Folding',
    'ToggleFoldShowText':'Folding',
    'VisibleFromDocLine':'Folding',
    'GetIndicatorCurrent':'Indicators',
    'GetIndicatorValue':'Indicators',
    'IndicatorAllOnFor':'Indicators',
    'IndicatorClearRange':'Indicators',
    'IndicatorEnd':'Indicators',
    'IndicatorFillRange':'Indicators',
    'IndicatorStart':'Indicators',
    'IndicatorValueAt':'Indicators',
    'IndicGetAlpha':'Indicators',
    'IndicGetFlags':'Indicators',
    'IndicGetFore':'Indicators',
    'IndicGetHoverFore':'Indicators',
    'IndicGetHoverStyle':'Indicators',
    'IndicGetOutlineAlpha':'Indicators',
    'IndicGetStyle':'Indicators',
    'IndicGetUnder':'Indicators',
    'IndicSetAlpha':'Indicators',
    'IndicSetFlags':'Indicators',
    'IndicSetFore':'Indicators',
    'IndicSetHoverFore':'Indicators',
    'IndicSetHoverStyle':'Indicators',
    'IndicSetOutlineAlpha':'Indicators',
    'IndicSetStyle':'Indicators',
    'IndicSetUnder':'Indicators',
    'SetIndicatorCurrent':'Indicators',
    'SetIndicatorValue':'Indicators',
    'AssignCmdKey':'KeyBindings',
    'ClearAllCmdKeys':'KeyBindings',
    'ClearCmdKey':'KeyBindings',
    'BackTab':'KeyboardCommands',
    'Cancel':'KeyboardCommands',
    'CharLeft':'KeyboardCommands',
    'CharLeftExtend':'KeyboardCommands',
    'CharLeftRectExtend':'KeyboardCommands',
    'CharRight':'KeyboardCommands',
    'CharRightExtend':'KeyboardCommands',
    'CharRightRectExtend':'KeyboardCommands',
    'DeleteBack':'KeyboardCommands',
    'DeleteBackNotLine':'KeyboardCommands',
    'DelLineLeft':'KeyboardCommands',
    'DelLineRight':'KeyboardCommands',
    'DelWordLeft':'KeyboardCommands',
    'DelWordRight':'KeyboardCommands',
    'DelWordRightEnd':'KeyboardCommands',
    'DocumentEnd':'KeyboardCommands',
    'DocumentEndExtend':'KeyboardCommands',
    'DocumentStart':'KeyboardCommands',
    'DocumentStartExtend':'KeyboardCommands',
    'EditToggleOvertype':'KeyboardCommands',
    'FormFeed':'KeyboardCommands',
    'Home':'KeyboardCommands',
    'HomeDisplay':'KeyboardCommands',
    'HomeDisplayExtend':'KeyboardCommands',
    'HomeExtend':'KeyboardCommands',
    'HomeRectExtend':'KeyboardCommands',
    'HomeWrap':'KeyboardCommands',
    'HomeWrapExtend':'KeyboardCommands',
    'LineCopy':'KeyboardCommands',
    'LineCut':'KeyboardCommands',
    'LineDelete':'KeyboardCommands',
    'LineDown':'KeyboardCommands',
    'LineDownExtend':'KeyboardCommands',
    'LineDownRectExtend':'KeyboardCommands',
    'LineDuplicate':'KeyboardCommands',
    'LineEnd':'KeyboardCommands',
    'LineEndDisplay':'KeyboardCommands',
    'LineEndDisplayExtend':'KeyboardCommands',
    'LineEndExtend':'KeyboardCommands',
    'LineEndRectExtend':'KeyboardCommands',
    'LineEndWrap':'KeyboardCommands',
    'LineEndWrapExtend':'KeyboardCommands',
    'LineScrollDown':'KeyboardCommands',
    'LineScrollUp':'KeyboardCommands',
    'LineTranspose':'KeyboardCommands',
    'LineUp':'KeyboardCommands',
    'LineUpExtend':'KeyboardCommands',
    'LineUpRectExtend':'KeyboardCommands',
    'LowerCase':'KeyboardCommands',
    'NewLine':'KeyboardCommands',
    'PageDown':'KeyboardCommands',
    'PageDownExtend':'KeyboardCommands',
    'PageDownRectExtend':'KeyboardCommands',
    'PageUp':'KeyboardCommands',
    'PageUpExtend':'KeyboardCommands',
    'PageUpRectExtend':'KeyboardCommands',
    'ParaDown':'KeyboardCommands',
    'ParaDownExtend':'KeyboardCommands',
    'ParaUp':'KeyboardCommands',
    'ParaUpExtend':'KeyboardCommands',
    'ScrollToEnd':'KeyboardCommands',
    'ScrollToStart':'KeyboardCommands',
    'SelectionDuplicate':'KeyboardCommands',
    'StutteredPageDown':'KeyboardCommands',
    'StutteredPageDownExtend':'KeyboardCommands',
    'StutteredPageUp':'KeyboardCommands',
    'StutteredPageUpExtend':'KeyboardCommands',
    'Tab':'KeyboardCommands',
    'UpperCase':'KeyboardCommands',
    'VCHome':'KeyboardCommands',
    'VCHomeDisplay':'KeyboardCommands',
    'VCHomeDisplayExtend':'KeyboardCommands',
    'VCHomeExtend':'KeyboardCommands',
    'VCHomeRectExtend':'KeyboardCommands',
    'VCHomeWrap':'KeyboardCommands',
    'VCHomeWrapExtend':'KeyboardCommands',
    'VerticalCentreCaret':'KeyboardCommands',
    'WordLeft':'KeyboardCommands',
    'WordLeftEnd':'KeyboardCommands',
    'WordLeftEndExtend':'KeyboardCommands',
    'WordLeftExtend':'KeyboardCommands',
    'WordPartLeft':'KeyboardCommands',
    'WordPartLeftExtend':'KeyboardCommands',
    'WordPartRight':'KeyboardCommands',
    'WordPartRightExtend':'KeyboardCommands',
    'WordRight':'KeyboardCommands',
    'WordRightEnd':'KeyboardCommands',
    'WordRightEndExtend':'KeyboardCommands',
    'WordRightExtend':'KeyboardCommands',
    'AllocateSubStyles':'Lexer',
    'ChangeLexerState':'Lexer',
    'Colourise':'Lexer',
    'DescribeKeyWordSets':'Lexer',
    'DescribeProperty':'Lexer',
    'DistanceToSecondaryStyles':'Lexer',
    'FreeSubStyles':'Lexer',
    'GetLexer':'Lexer',
    'GetLexerLanguage':'Lexer',
    'GetPrimaryStyleFromStyle':'Lexer',
    'GetProperty':'Lexer',
    'GetPropertyExpanded':'Lexer',
    'GetPropertyInt':'Lexer',
    'GetStyleFromSubStyle':'Lexer',
    'GetSubStyleBases':'Lexer',
    'GetSubStylesLength':'Lexer',
    'GetSubStylesStart':'Lexer',
    'LoadLexerLibrary':'Lexer',
    'PrivateLexerCall':'Lexer',
    'PropertyNames':'Lexer',
    'PropertyType':'Lexer',
    'SetIdentifiers':'Lexer',
    'SetKeyWords':'Lexer',
    'SetLexer':'Lexer',
    'SetLexerLanguage':'Lexer',
    'SetProperty':'Lexer',
    'ConvertEOLs':'LineEndings',
    'GetEOLMode':'LineEndings',
    'GetLineEndTypesActive':'LineEndings',
    'GetLineEndTypesAllowed':'LineEndings',
    'GetLineEndTypesSupported':'LineEndings',
    'GetViewEOL':'LineEndings',
    'SetEOLMode':'LineEndings',
    'SetLineEndTypesAllowed':'LineEndings',
    'SetViewEOL':'LineEndings',
    'GetLayoutCache':'LineWrapping',
    'GetPositionCache':'LineWrapping',
    'GetWrapIndentMode':'LineWrapping',
    'GetWrapMode':'LineWrapping',
    'GetWrapStartIndent':'LineWrapping',
    'GetWrapVisualFlags':'LineWrapping',
    'GetWrapVisualFlagsLocation':'LineWrapping',
    'LinesJoin':'LineWrapping',
    'LinesSplit':'LineWrapping',
    'SetPositionCache':'LineWrapping',
    'SetWrapIndentMode':'LineWrapping',
    'SetWrapMode':'LineWrapping',
    'SetWrapStartIndent':'LineWrapping',
    'SetWrapVisualFlags':'LineWrapping',
    'SetWrapVisualFlagsLocation':'LineWrapping',
    'WrapCount':'LineWrapping',
    'GetEdgeColour':'LongLines',
    'GetEdgeColumn':'LongLines',
    'GetEdgeMode':'LongLines',
    'MultiEdgeAddLine':'LongLines',
    'MultiEdgeClearAll':'LongLines',
    'SetEdgeColour':'LongLines',
    'SetEdgeColumn':'LongLines',
    'SetEdgeMode':'LongLines',
    'StartRecord':'MacroRecording',
    'StopRecord':'MacroRecording',
    'GetMarginBackN':'Margins',
    'GetMarginCursorN':'Margins',
    'GetMarginLeft':'Margins',
    'GetMarginMaskN':'Margins',
    'GetMarginOptions':'Margins',
    'GetMarginRight':'Margins',
    'GetMargins':'Margins',
    'GetMarginSensitiveN':'Margins',
    'GetMarginTypeN':'Margins',
    'GetMarginWidthN':'Margins',
    'MarginGetStyle':'Margins',
    'MarginGetStyleOffset':'Margins',
    'MarginGetStyles':'Margins',
    'MarginGetText':'Margins',
    'MarginSetStyle':'Margins',
    'MarginSetStyleOffset':'Margins',
    'MarginSetStyles':'Margins',
    'MarginSetText':'Margins',
    'MarginTextClearAll':'Margins',
    'SetFoldMarginColour':'Margins',
    'SetFoldMarginHiColour':'Margins',
    'SetMarginBackN':'Margins',
    'SetMarginCursorN':'Margins',
    'SetMarginLeft':'Margins',
    'SetMarginMaskN':'Margins',
    'SetMarginOptions':'Margins',
    'SetMarginRight':'Margins',
    'SetMargins':'Margins',
    'SetMarginSensitiveN':'Margins',
    'SetMarginTypeN':'Margins',
    'SetMarginWidthN':'Margins',
    'MarkerAdd':'Markers',
    'MarkerAddSet':'Markers',
    'MarkerDefine':'Markers',
    'MarkerDefinePixmap':'Markers',
    'MarkerDefineRGBAImage':'Markers',
    'MarkerDelete':'Markers',
    'MarkerDeleteAll':'Markers',
    'MarkerDeleteHandle':'Markers',
    'MarkerEnableHighlight':'Markers',
    'MarkerGet':'Markers',
    'MarkerLineFromHandle':'Markers',
    'MarkerNext':'Markers',
    'MarkerPrevious':'Markers',
    'MarkerSetAlpha':'Markers',
    'MarkerSetBack':'Markers',
    'MarkerSetBackSelected':'Markers',
    'MarkerSetFore':'Markers',
    'MarkerSymbolDefined':'Markers',
    'RGBAImageSetHeight':'Markers',
    'RGBAImageSetScale':'Markers',
    'RGBAImageSetWidth':'Markers',
    'GetMouseDownCaptures':'MouseCapture',
    'GetMouseWheelCaptures':'MouseCapture',
    'SetMouseDownCaptures':'MouseCapture',
    'SetMouseWheelCaptures':'MouseCapture',
    'AddSelection':'MultipleSelectionAndVirtualSpace',
    'ClearSelections':'MultipleSelectionAndVirtualSpace',
    'DropSelectionN':'MultipleSelectionAndVirtualSpace',
    'GetAdditionalCaretFore':'MultipleSelectionAndVirtualSpace',
    'GetAdditionalCaretsBlink':'MultipleSelectionAndVirtualSpace',
    'GetAdditionalCaretsVisible':'MultipleSelectionAndVirtualSpace',
    'GetAdditionalSelAlpha':'MultipleSelectionAndVirtualSpace',
    'GetAdditionalSelectionTyping':'MultipleSelectionAndVirtualSpace',
    'GetMainSelection':'MultipleSelectionAndVirtualSpace',
    'GetMultiPaste':'MultipleSelectionAndVirtualSpace',
    'GetMultipleSelection':'MultipleSelectionAndVirtualSpace',
    'GetRectangularSelectionAnchor':'MultipleSelectionAndVirtualSpace',
    'GetRectangularSelectionAnchorVirtualSpace':'MultipleSelectionAndVirtualSpace',
    'GetRectangularSelectionCaret':'MultipleSelectionAndVirtualSpace',
    'GetRectangularSelectionCaretVirtualSpace':'MultipleSelectionAndVirtualSpace',
    'GetRectangularSelectionModifier':'MultipleSelectionAndVirtualSpace',
    'GetSelectionEmpty':'MultipleSelectionAndVirtualSpace',
    'GetSelectionNAnchor':'MultipleSelectionAndVirtualSpace',
    'GetSelectionNAnchorVirtualSpace':'MultipleSelectionAndVirtualSpace',
    'GetSelectionNCaret':'MultipleSelectionAndVirtualSpace',
    'GetSelectionNCaretVirtualSpace':'MultipleSelectionAndVirtualSpace',
    'GetSelectionNEnd':'MultipleSelectionAndVirtualSpace',
    'GetSelectionNStart':'MultipleSelectionAndVirtualSpace',
    'GetSelections':'MultipleSelectionAndVirtualSpace',
    'GetVirtualSpaceOptions':'MultipleSelectionAndVirtualSpace',
    'MultipleSelectAddEach':'MultipleSelectionAndVirtualSpace',
    'MultipleSelectAddNext':'MultipleSelectionAndVirtualSpace',
    'RotateSelection':'MultipleSelectionAndVirtualSpace',
    'SetAdditionalCaretFore':'MultipleSelectionAndVirtualSpace',
    'SetAdditionalCaretsBlink':'MultipleSelectionAndVirtualSpace',
    'SetAdditionalCaretsVisible':'MultipleSelectionAndVirtualSpace',
    'SetAdditionalSelAlpha':'MultipleSelectionAndVirtualSpace',
    'SetAdditionalSelBack':'MultipleSelectionAndVirtualSpace',
    'SetAdditionalSelectionTyping':'MultipleSelectionAndVirtualSpace',
    'SetAdditionalSelFore':'MultipleSelectionAndVirtualSpace',
    'SetMainSelection':'MultipleSelectionAndVirtualSpace',
    'SetMultiPaste':'MultipleSelectionAndVirtualSpace',
    'SetMultipleSelection':'MultipleSelectionAndVirtualSpace',
    'SetRectangularSelectionAnchor':'MultipleSelectionAndVirtualSpace',
    'SetRectangularSelectionAnchorVirtualSpace':'MultipleSelectionAndVirtualSpace',
    'SetRectangularSelectionCaret':'MultipleSelectionAndVirtualSpace',
    'SetRectangularSelectionCaretVirtualSpace':'MultipleSelectionAndVirtualSpace',
    'SetRectangularSelectionModifier':'MultipleSelectionAndVirtualSpace',
    'SetSelectionNAnchor':'MultipleSelectionAndVirtualSpace',
    'SetSelectionNAnchorVirtualSpace':'MultipleSelectionAndVirtualSpace',
    'SetSelectionNCaret':'MultipleSelectionAndVirtualSpace',
    'SetSelectionNCaretVirtualSpace':'MultipleSelectionAndVirtualSpace',
    'SetSelectionNEnd':'MultipleSelectionAndVirtualSpace',
    'SetSelectionNStart':'MultipleSelectionAndVirtualSpace',
    'SetVirtualSpaceOptions':'MultipleSelectionAndVirtualSpace',
    'SwapMainAnchorCaret':'MultipleSelectionAndVirtualSpace',
    'AddRefDocument':'MultipleViews',
    'CreateDocument':'MultipleViews',
    'GetDocPointer':'MultipleViews',
    'ReleaseDocument':'MultipleViews',
    'SetDocPointer':'MultipleViews',
    'GetIdentifier':'Notifications',
    'GetModEventMask':'Notifications',
    'GetMouseDwellTime':'Notifications',
    'SetIdentifier':'Notifications',
    'SetModEventMask':'Notifications',
    'SetMouseDwellTime':'Notifications',
    'GetBufferedDraw':'OtherSettings',
    'GetCodePage':'OtherSettings',
    'GetFontQuality':'OtherSettings',
    'GetFocus':'OtherSettings',
    'GetIMEInteraction':'OtherSettings',
    'GetPhasesDraw':'OtherSettings',
    'GetTechnology':'OtherSettings',
    'GetTwoPhaseDraw':'OtherSettings',
    'SetBufferedDraw':'OtherSettings',
    'SetCodePage':'OtherSettings',
    'SetFocus':'OtherSettings',
    'SetFontQuality':'OtherSettings',
    'SetIMEInteraction':'OtherSettings',
    'SetLayoutCache':'OtherSettings',
    'SetPhasesDraw':'OtherSettings',
    'SetTechnology':'OtherSettings',
    'SetTwoPhaseDraw':'OtherSettings',
    'GetOvertype':'Overtype',
    'SetOvertype':'Overtype',
    'UsePopUp':'PopupEditMenu',
    'FormatRange':'Printing',
    'GetPrintColourMode':'Printing',
    'GetPrintMagnification':'Printing',
    'GetPrintWrapMode':'Printing',
    'SetPrintColourMode':'Printing',
    'SetPrintMagnification':'Printing',
    'SetPrintWrapMode':'Printing',
    'GetEndAtLastLine':'ScrollingAndAutomaticScrolling',
    'GetFirstVisibleLine':'ScrollingAndAutomaticScrolling',
    'GetHScrollBar':'ScrollingAndAutomaticScrolling',
    'GetScrollWidth':'ScrollingAndAutomaticScrolling',
    'GetScrollWidthTracking':'ScrollingAndAutomaticScrolling',
    'GetVScrollBar':'ScrollingAndAutomaticScrolling',
    'GetXOffset':'ScrollingAndAutomaticScrolling',
    'LineScroll':'ScrollingAndAutomaticScrolling',
    'ScrollCaret':'ScrollingAndAutomaticScrolling',
    'ScrollRange':'ScrollingAndAutomaticScrolling',
    'SetEndAtLastLine':'ScrollingAndAutomaticScrolling',
    'SetFirstVisibleLine':'ScrollingAndAutomaticScrolling',
    'SetHScrollBar':'ScrollingAndAutomaticScrolling',
    'SetScrollWidth':'ScrollingAndAutomaticScrolling',
    'SetScrollWidthTracking':'ScrollingAndAutomaticScrolling',
    'SetVisiblePolicy':'ScrollingAndAutomaticScrolling',
    'SetVScrollBar':'ScrollingAndAutomaticScrolling',
    'SetXCaretPolicy':'ScrollingAndAutomaticScrolling',
    'SetXOffset':'ScrollingAndAutomaticScrolling',
    'SetYCaretPolicy':'ScrollingAndAutomaticScrolling',
    'FindText':'Searching',
    'GetSearchFlags':'Searching',
    'GetTag':'Searching',
    'GetTargetEnd':'Searching',
    'GetTargetStart':'Searching',
    'GetTargetText':'Searching',
    'ReplaceTarget':'Searching',
    'ReplaceTargetRE':'Searching',
    'SearchAnchor':'Searching',
    'SearchInTarget':'Searching',
    'SearchNext':'Searching',
    'SearchPrev':'Searching',
    'SetSearchFlags':'Searching',
    'SetTargetEnd':'Searching',
    'SetTargetRange':'Searching',
    'SetTargetStart':'Searching',
    'TargetFromSelection':'Searching',
    'TargetWholeDocument':'Searching',
    'CharPositionFromPoint':'SelectionAndInformation',
    'CharPositionFromPointClose':'SelectionAndInformation',
    'ChooseCaretX':'SelectionAndInformation',
    'CountCharacters':'SelectionAndInformation',
    'FindColumn':'SelectionAndInformation',
    'GetAnchor':'SelectionAndInformation',
    'GetColumn':'SelectionAndInformation',
    'GetCurLine':'SelectionAndInformation',
    'GetCurrentPos':'SelectionAndInformation',
    'GetLength':'SelectionAndInformation',
    'GetLineCount':'SelectionAndInformation',
    'GetLineEndPosition':'SelectionAndInformation',
    'GetLineSelEndPosition':'SelectionAndInformation',
    'GetLineSelStartPosition':'SelectionAndInformation',
    'GetModify':'SelectionAndInformation',
    'GetMouseSelectionRectangularSwitch':'SelectionAndInformation',
    'GetSelectionEnd':'SelectionAndInformation',
    'GetSelectionMode':'SelectionAndInformation',
    'GetSelectionStart':'SelectionAndInformation',
    'GetSelText':'SelectionAndInformation',
    'GetTextLength':'SelectionAndInformation',
    'GotoLine':'SelectionAndInformation',
    'GotoPos':'SelectionAndInformation',
    'HideSelection':'SelectionAndInformation',
    'LineFromPosition':'SelectionAndInformation',
    'LineLength':'SelectionAndInformation',
    'LinesOnScreen':'SelectionAndInformation',
    'MoveCaretInsideView':'SelectionAndInformation',
    'MoveSelectedLinesDown':'SelectionAndInformation',
    'MoveSelectedLinesUp':'SelectionAndInformation',
    'PointXFromPosition':'SelectionAndInformation',
    'PositionAfter':'SelectionAndInformation',
    'PositionBefore':'SelectionAndInformation',
    'PositionFromLine':'SelectionAndInformation',
    'PositionFromPoint':'SelectionAndInformation',
    'PositionFromPointClose':'SelectionAndInformation',
    'PositionRelative':'SelectionAndInformation',
    'SelectAll':'SelectionAndInformation',
    'SelectionIsRectangle':'SelectionAndInformation',
    'SetAnchor':'SelectionAndInformation',
    'SetCurrentPos':'SelectionAndInformation',
    'SetEmptySelection':'SelectionAndInformation',
    'SetMouseSelectionRectangularSwitch':'SelectionAndInformation',
    'SetSelectionEnd':'SelectionAndInformation',
    'SetSelectionMode':'SelectionAndInformation',
    'SetSelectionStart':'SelectionAndInformation',
    'TextHeight':'SelectionAndInformation',
    'TextWidth':'SelectionAndInformation',
    'StyleClearAll':'StyleDefinition',
    'StyleGetBack':'StyleDefinition',
    'StyleGetBold':'StyleDefinition',
    'StyleGetCase':'StyleDefinition',
    'StyleGetChangeable':'StyleDefinition',
    'StyleGetCharacterSet':'StyleDefinition',
    'StyleGetEOLFilled':'StyleDefinition',
    'StyleGetFont':'StyleDefinition',
    'StyleGetFore':'StyleDefinition',
    'StyleGetHotSpot':'StyleDefinition',
    'StyleGetItalic':'StyleDefinition',
    'StyleGetSize':'StyleDefinition',
    'StyleGetSizeFractional':'StyleDefinition',
    'StyleGetUnderline':'StyleDefinition',
    'StyleGetVisible':'StyleDefinition',
    'StyleGetWeight':'StyleDefinition',
    'StyleResetDefault':'StyleDefinition',
    'StyleSetBack':'StyleDefinition',
    'StyleSetBold':'StyleDefinition',
    'StyleSetCase':'StyleDefinition',
    'StyleSetChangeable':'StyleDefinition',
    'StyleSetCharacterSet':'StyleDefinition',
    'StyleSetEOLFilled':'StyleDefinition',
    'StyleSetFont':'StyleDefinition',
    'StyleSetFore':'StyleDefinition',
    'StyleSetHotSpot':'StyleDefinition',
    'StyleSetItalic':'StyleDefinition',
    'StyleSetSize':'StyleDefinition',
    'StyleSetSizeFractional':'StyleDefinition',
    'StyleSetUnderline':'StyleDefinition',
    'StyleSetVisible':'StyleDefinition',
    'StyleSetWeight':'StyleDefinition',
    'GetEndStyled':'Styling',
    'GetIdleStyling':'Styling',
    'GetLineState':'Styling',
    'GetMaxLineState':'Styling',
    'SetIdleStyling':'Styling',
    'SetLineState':'Styling',
    'SetStyling':'Styling',
    'SetStylingEx':'Styling',
    'StartStyling':'Styling',
    'AddTabStop':'TabsAndIndentationGuides',
    'ClearTabStops':'TabsAndIndentationGuides',
    'GetBackSpaceUnIndents':'TabsAndIndentationGuides',
    'GetHighlightGuide':'TabsAndIndentationGuides',
    'GetIndent':'TabsAndIndentationGuides',
    'GetIndentationGuides':'TabsAndIndentationGuides',
    'GetLineIndentation':'TabsAndIndentationGuides',
    'GetLineIndentPosition':'TabsAndIndentationGuides',
    'GetNextTabStop':'TabsAndIndentationGuides',
    'GetTabIndents':'TabsAndIndentationGuides',
    'GetTabWidth':'TabsAndIndentationGuides',
    'GetUseTabs':'TabsAndIndentationGuides',
    'SetBackSpaceUnIndents':'TabsAndIndentationGuides',
    'SetHighlightGuide':'TabsAndIndentationGuides',
    'SetIndent':'TabsAndIndentationGuides',
    'SetIndentationGuides':'TabsAndIndentationGuides',
    'SetLineIndentation':'TabsAndIndentationGuides',
    'SetTabIndents':'TabsAndIndentationGuides',
    'SetTabWidth':'TabsAndIndentationGuides',
    'SetUseTabs':'TabsAndIndentationGuides',
    'AddStyledText':'TextRetrievalAndModification',
    'AddText':'TextRetrievalAndModification',
    'Allocate':'TextRetrievalAndModification',
    'AllocateExtendedStyles':'TextRetrievalAndModification',
    'AppendText':'TextRetrievalAndModification',
    'ChangeInsertion':'TextRetrievalAndModification',
    'ClearAll':'TextRetrievalAndModification',
    'ClearDocumentStyle':'TextRetrievalAndModification',
    'DeleteRange':'TextRetrievalAndModification',
    'GetCharAt':'TextRetrievalAndModification',
    'GetLine':'TextRetrievalAndModification',
    'GetReadOnly':'TextRetrievalAndModification',
    'GetStyleAt':'TextRetrievalAndModification',
    'GetStyledText':'TextRetrievalAndModification',
    'GetText':'TextRetrievalAndModification',
    'GetTextRange':'TextRetrievalAndModification',
    'InsertText':'TextRetrievalAndModification',
    'ReleaseAllExtendedStyles':'TextRetrievalAndModification',
    'ReplaceSel':'TextRetrievalAndModification',
    'SetReadOnly':'TextRetrievalAndModification',
    'SetSavePoint':'TextRetrievalAndModification',
    'SetText':'TextRetrievalAndModification',
    'AddUndoAction':'UndoAndRedo',
    'BeginUndoAction':'UndoAndRedo',
    'CanRedo':'UndoAndRedo',
    'CanUndo':'UndoAndRedo',
    'EmptyUndoBuffer':'UndoAndRedo',
    'EndUndoAction':'UndoAndRedo',
    'GetUndoCollection':'UndoAndRedo',
    'Redo':'UndoAndRedo',
    'SetUndoCollection':'UndoAndRedo',
    'Undo':'UndoAndRedo',
    'UserListShow':'UserLists',
    'GetExtraAscent':'WhiteSpace',
    'GetExtraDescent':'WhiteSpace',
    'GetTabDrawMode':'WhiteSpace',
    'GetViewWS':'WhiteSpace',
    'GetWhitespaceSize':'WhiteSpace',
    'SetExtraAscent':'WhiteSpace',
    'SetExtraDescent':'WhiteSpace',
    'SetTabDrawMode':'WhiteSpace',
    'SetViewWS':'WhiteSpace',
    'SetWhitespaceBack':'WhiteSpace',
    'SetWhitespaceFore':'WhiteSpace',
    'SetWhitespaceSize':'WhiteSpace',
    'GetPunctuationChars':'Words',
    'GetWhitespaceChars':'Words',
    'GetWordChars':'Words',
    'IsRangeWord':'Words',
    'SetCharsDefault':'Words',
    'SetPunctuationChars':'Words',
    'SetWhitespaceChars':'Words',
    'SetWordChars':'Words',
    'WordEndPosition':'Words',
    'WordStartPosition':'Words',
    'GetZoom':'Zooming',
    'SetZoom':'Zooming',
    'ZoomIn':'Zooming',
    'ZoomOut':'Zooming'
}


# A few of docstrings in Scintilla.iface are unsuitable for use with wxSTC.
# This map can be used to supply alternate docstrings for those items.
docOverrides = {
    'ClearRegisteredImages':('Clear all the registered images.',),
    'RegisterImage':('Register an image for use in autocompletion lists.',),

    'MarkerDefine':
        ('Set the symbol used for a particular marker number,',
        'and optionally the fore and background colours.',),

    'MarkerDefinePixmap':('Define a marker from a bitmap',),

    'SetCodePage':
        ('Set the code page used to interpret the bytes of the document as characters.',),

    'GetXOffset':('Get the xOffset (ie, horizontal scroll position).',),
    'SetXOffset':('Set the xOffset (ie, horizontal scroll position).',),
    'GetSelText':('Retrieve the selected text.',),

    'PointXFromPosition':
        ('Retrieve the point in the window where a position is displayed.',),

    'StyleGetFont':('Get the font facename of a style',),
    'GetLine':('Retrieve the contents of a line.',),
    'GetStyledText':('Retrieve a buffer of cells.',),
    'GetText':('Retrieve all the text in the document.',),
    'GetTextRange':('Retrieve a range of text.',),

    'GetWordChars':
        ('Get the set of characters making up words for when moving or selecting by word.',),

    'GetLexerLanguage':('Retrieve the lexing language of the document.',)
}


# This map can be used to change a few words in a docstring to make it
# consistent with names used by wxSTC.
#
# For example {'INVALID_POSITION':'wxSTC_INVALID_POSITION'} will turn the
# string 'Return INVALID_POSITION if not close to text.'
# into 'Return wxSTC_INVALID_POSITION if not close to text.'
#
docSubstitutions = {
    'BraceMatch':{'INVALID_POSITION':'wxSTC_INVALID_POSITION'},
    'GetLineSelEndPosition':{'INVALID_POSITION':'wxSTC_INVALID_POSITION'},
    'GetLineSelStartPosition':{'INVALID_POSITION':'wxSTC_INVALID_POSITION'},
    'PositionFromPointClose':{'INVALID_POSITION':'wxSTC_INVALID_POSITION'},
    'CharPositionFromPointClose':{'INVALID_POSITION':'wxSTC_INVALID_POSITION'},

    'GetRepresentation':{'Result is NUL-terminated.':''},
    'GetSubStyleBases':{'Result is NUL-terminated.':''},
    'DescribeProperty':{'Result is NUL-terminated.':''},
    'GetProperty':{'Result is NUL-terminated.':''},
    'GetPropertyExpanded':{'Result is NUL-terminated.':''},
    'GetTag':{'Result is NUL-terminated.':''},
    'TextWidth':{'NUL terminated text argument.':''},
    'GetCurLine':{'Result is NUL-terminated.':'',
        'Returns the index of the caret on the line.':''},
    'AutoCGetCurrentText':{'Result is NUL-terminated.':'',
        'Returns the length of the item text':''},
    'StartStyling':
        {'The unused parameter is no longer used and should be set to 0.':''},

    'DescribeKeyWordSets':{'\\n':'\\\\n','Result is NUL-terminated.':''},
    'PropertyNames':{'\\n':'\\\\n','Result is NUL-terminated.':''},
    'ReplaceTargetRE':{'\\d':'\\\\d','\\(':'\\\\(','\\)':'\\\\)'},

    'CallTipUseStyle':{'STYLE_CALLTIP':'wxSTC_STYLE_CALLTIP'},
    'ChangeInsertion':{'SC_MOD_INSERTCHECK':'wxSTC_MOD_INSERTCHECK'},
    'GetWrapIndentMode':{'Default is fixed':'Default is wxSTC_WRAPINDENT_FIXED'},
    'GetTabDrawMode':{'SCTD_*':'wxSTC_TD_*'},
    'GetViewWS':{'SCWS_*':'wxSTC_WS_*'},
    'SetCursor':{'SC_CURSOR':'wxSTC_CURSOR'},
    'SetMarginBackN':{'SC_MARGIN_COLOUR':'wxSTC_MARGIN_COLOUR'},
    'SetWrapIndentMode':{'Default is fixed':'Default is wxSTC_WRAPINDENT_FIXED'},

    'GetEOLMode':
        {'CRLF, CR, or LF':'wxSTC_EOL_CRLF, wxSTC_EOL_CR, or wxSTC_EOL_LF'},

    'SetEdgeMode':{'EDGE_LINE':'wxSTC_EDGE_LINE',
        'EDGE_MULTILINE':'wxSTC_EDGE_MULTILINE',
        'EDGE_BACKGROUND':'wxSTC_EDGE_BACKGROUND','EDGE_NONE':'wxSTC_EDGE_NONE'},

    'SetPrintWrapMode':
        {'SC_WRAP_WORD':'wxSTC_WRAP_WORD','SC_WRAP_NONE':'wxSTC_WRAP_NONE'},

    'SetRectangularSelectionModifier':{'SCMOD_CTRL':'wxSTC_KEYMOD_CTRL ',
        'SCMOD_ALT':'wxSTC_KEYMOD_ALT','SCMOD_SUPER':'wxSTC_KEYMOD_SUPER'},

    'SetSelectionMode':{'SC_SEL_STREAM':'wxSTC_SEL_STREAM',
        'SC_SEL_RECTANGLE':'wxSTC_SEL_RECTANGLE','SC_SEL_THIN':'wxSTC_SEL_THIN',
        'SC_SEL_LINES':'wxSTC_SEL_LINES'},

    'SetFontQuality':{' from the FontQuality enumeration':''},

    'GetWrapVisualFlags':{'Retrive':'Retrieve'},

    'GetWrapVisualFlagsLocation':{'Retrive':'Retrieve'},

    'GetWrapStartIndent':{'Retrive':'Retrieve'}
}


# This map can be used to add extended documentation to the interface header.
extendedDocs = {

    'GetSearchFlags':
        ('The return value will be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_FIND_WHOLEWORD wxSTC_FIND_* @endlink constants.',),

    'SetSearchFlags':
        ('The input should be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_FIND_WHOLEWORD wxSTC_FIND_* @endlink constants.',),

    'GetStatus':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_STATUS_OK wxSTC_STATUS_* @endlink constants.',),

    'SetStatus':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_STATUS_OK wxSTC_STATUS_* @endlink constants.',),

    'GetSelectionMode':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_SEL_STREAM wxSTC_SEL_* @endlink constants.',),

    'GetMultiPaste':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_MULTIPASTE_ONCE wxSTC_MULTIPASTE_* '
        '@endlink constants.',),

    'SetMultiPaste':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_MULTIPASTE_ONCE wxSTC_MULTIPASTE_* '
        '@endlink constants.',),

    'GetVirtualSpaceOptions':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_VS_NONE wxSTC_VS_* @endlink constants.',),

    'SetVirtualSpaceOptions':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_VS_NONE wxSTC_VS_* @endlink constants.',),

    'GetRectangularSelectionModifier':
        ('The return value will be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_KEYMOD_NORM wxSTC_KEYMOD_* @endlink constants.',),

    'SetXCaretPolicy':
        ('The first argument should be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_CARET_SLOP wxSTC_CARET_* @endlink constants.',),

    'SetYCaretPolicy':
        ('The first argument should be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_CARET_SLOP wxSTC_CARET_* @endlink constants.',),

    'SetVisiblePolicy':
        ('The first argument should be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_VISIBLE_SLOP wxSTC_VISIBLE_* @endlink constants.',),

    'SetViewWS':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_WS_INVISIBLE wxSTC_WS_* @endlink  constants.',),

    'SetTabDrawMode':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_TD_LONGARROW wxSTC_TD_* @endlink constants.',),

    'GetCursor':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_CURSORNORMAL wxSTC_CURSOR* @endlink constants.',),

    'SetEOLMode':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_EOL_CRLF wxSTC_EOL_* @endlink  constants.',),

    'GetLineEndTypesActive':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_LINE_END_TYPE_DEFAULT wxSTC_LINE_END_TYPE_* '
        '@endlink constants.',),

    'GetLineEndTypesAllowed':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_LINE_END_TYPE_DEFAULT wxSTC_LINE_END_TYPE_* '
        '@endlink constants.',),

    'SetLineEndTypesAllowed':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_LINE_END_TYPE_DEFAULT '
        'wxSTC_LINE_END_TYPE_* @endlink constants.',),

   'GetLineEndTypesSupported':
        ('The return value will be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_LINE_END_TYPE_DEFAULT '
        'wxSTC_LINE_END_TYPE_* @endlink constants.',),

    'GetIdleStyling':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_IDLESTYLING_NONE wxSTC_IDLESTYLING_* '
        '@endlink constants.',),

    'SetIdleStyling':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_IDLESTYLING_NONE wxSTC_IDLESTYLING_* '
        '@endlink constants.',),

    'StyleGetWeight':
        ('The return value will be an integer that is possibly one of the',
        '@link wxStyledTextCtrl::wxSTC_WEIGHT_NORMAL wxSTC_WEIGHT_* @endlink constants.',),

    'StyleSetWeight':
        ('The second argument can be an integer or one of the',
        '@link wxStyledTextCtrl::wxSTC_WEIGHT_NORMAL wxSTC_WEIGHT_* @endlink constants.',),

    'StyleGetCase':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_CASE_MIXED wxSTC_CASE_* @endlink constants.',),

    'StyleSetCase':
        ('The second argument should be one of the',
        '@link wxStyledTextCtrl::wxSTC_CASE_MIXED wxSTC_CASE_* @endlink constants.',),

    'GetCaretStyle':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_CARETSTYLE_INVISIBLE wxSTC_CARETSTYLE_* '
        '@endlink constants.',),

    'SetCaretStyle':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_CARETSTYLE_INVISIBLE wxSTC_CARETSTYLE_* '
        '@endlink constants.',),

    'GetCaretSticky':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_CARETSTICKY_OFF wxSTC_CARETSTICKY_* '
        '@endlink constants.',),

    'SetCaretSticky':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_CARETSTICKY_OFF wxSTC_CARETSTICKY_* '
        '@endlink constants.',),

    'GetMarginTypeN':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_MARGIN_SYMBOL wxSTC_MARGIN_* @endlink constants.',),

    'SetMarginTypeN':
        ('The second argument should be one of the',
        '@link wxStyledTextCtrl::wxSTC_MARGIN_SYMBOL wxSTC_MARGIN_* @endlink constants.',),

    'GetMarginCursorN':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_CURSORNORMAL wxSTC_CURSOR* @endlink constants.',),

    'SetMarginCursorN':
        ('The second argument should be one of the',
        '@link wxStyledTextCtrl::wxSTC_CURSORNORMAL wxSTC_CURSOR* @endlink constants.',),

    'GetMarginOptions':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_MARGINOPTION_NONE wxSTC_MARGINOPTION_* '
        '@endlink constants.',),

    'SetMarginOptions':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_MARGINOPTION_NONE wxSTC_MARGINOPTION_* '
        '@endlink constants.',),

    'AnnotationGetVisible':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_ANNOTATION_HIDDEN wxSTC_ANNOTATION_* '
        '@endlink constants.',),

    'AnnotationSetVisible':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_ANNOTATION_HIDDEN wxSTC_ANNOTATION_* '
        '@endlink constants.',),

    'GetPhasesDraw':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_PHASES_ONE wxSTC_PHASES_* @endlink constants.',),

    'SetPhasesDraw':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_PHASES_ONE wxSTC_PHASES_* @endlink constants.',),

    'GetTechnology':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_TECHNOLOGY_DEFAULT wxSTC_TECHNOLOGY_* '
        '@endlink constants.',),

    'SetTechnology':
        ('@remarks',
        'For the wxMSW port, the input can be either wxSTC_TECHNOLOGY_DEFAULT',
        'or wxSTC_TECHNOLOGY_DIRECTWRITE.  With other ports, this method has',
        'no effect.',),

    'GetFontQuality':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_EFF_QUALITY_DEFAULT wxSTC_EFF_QUALITY_* '
        '@endlink constants.',),

    'SetFontQuality':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_EFF_QUALITY_DEFAULT wxSTC_EFF_QUALITY_* '
        '@endlink constants.',
        '@remarks',
        'This method only has any effect with the wxMSW port and when',
        'technology has been set to wxSTC_TECHNOLOGY_DIRECTWRITE.',),

    'GetIMEInteraction':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_IME_WINDOWED wxSTC_IME_* @endlink constants.',),

    'SetIMEInteraction':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_IME_WINDOWED wxSTC_IME_* @endlink constants.',),

    'GetIndentationGuides':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_IV_NONE wxSTC_IV_* @endlink constants.',),

    'SetIndentationGuides':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_IV_NONE wxSTC_IV_* @endlink constants.',),

    'MarkerSymbolDefined':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_MARK_CIRCLE wxSTC_MARK_* @endlink constants.',),

    'MarkerDefine':
        ('The second argument should be one of the',
        '@link wxStyledTextCtrl::wxSTC_MARK_CIRCLE wxSTC_MARK_* @endlink constants.',),

    'IndicGetStyle':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_INDIC_PLAIN wxSTC_INDIC_* @endlink constants.',),

    'IndicSetStyle':
        ('The second argument should be one of the',
        '@link wxStyledTextCtrl::wxSTC_INDIC_PLAIN wxSTC_INDIC_* @endlink constants.',),

    'IndicGetFlags':
        ('The return value will be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_INDICFLAG_VALUEFORE wxSTC_INDICFLAG_* '
        '@endlink constants.',),

    'IndicSetFlags':
        ('The second argument should be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_INDICFLAG_VALUEFORE wxSTC_INDICFLAG_* '
        '@endlink constants.',),

    'AutoCGetCaseInsensitiveBehaviour':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_CASEINSENSITIVEBEHAVIOUR_RESPECTCASE '
        'wxSTC_CASEINSENSITIVEBEHAVIOUR_* @endlink constants.',),

    'AutoCSetCaseInsensitiveBehaviour':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_CASEINSENSITIVEBEHAVIOUR_RESPECTCASE '
        'wxSTC_CASEINSENSITIVEBEHAVIOUR_* @endlink constants.',),

    'AutoCGetMulti':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_MULTIAUTOC_ONCE wxSTC_MULTIAUTOC_* '
        '@endlink constants.',),

    'AutoCSetMulti':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_MULTIAUTOC_ONCE wxSTC_MULTIAUTOC_* '
        '@endlink constants.',),

    'AutoCGetOrder':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_ORDER_PRESORTED wxSTC_ORDER_* @endlink constants.',),

    'AutoCSetOrder':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_ORDER_PRESORTED wxSTC_ORDER_* @endlink constants.',),

    'GetPrintColourMode':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_PRINT_NORMAL wxSTC_PRINT_* @endlink constants.',),

    'SetPrintColourMode':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_PRINT_NORMAL wxSTC_PRINT_* @endlink constants.',),

    'GetPrintWrapMode':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_WRAP_NONE wxSTC_WRAP_* @endlink constants.',),

    'SetFoldFlags':
        ('The second argument should be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_FOLDFLAG_LINEBEFORE_EXPANDED '
        'wxSTC_FOLDFLAG_* @endlink constants.',),

    'GetAutomaticFold':
        ('The return value will be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_AUTOMATICFOLD_SHOW '
        'wxSTC_AUTOMATICFOLD_* @endlink constants.',),

    'SetAutomaticFold':
        ('The input should be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_AUTOMATICFOLD_SHOW '
        'wxSTC_AUTOMATICFOLD_* @endlink constants.',),

    'FoldDisplayTextSetStyle':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_FOLDDISPLAYTEXT_HIDDEN '
        'wxSTC_FOLDDISPLAYTEXT_* @endlink constants.',),

    'GetWrapMode':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_WRAP_NONE wxSTC_WRAP_* @endlink constants.',),

    'SetWrapMode':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_WRAP_NONE wxSTC_WRAP_* @endlink constants.',),

    'GetWrapVisualFlags':
        ('The return value will be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_WRAPVISUALFLAG_NONE '
        'wxSTC_WRAPVISUALFLAG_* @endlink constants.',),

    'SetWrapVisualFlags':
        ('The input should be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_WRAPVISUALFLAG_NONE '
        'wxSTC_WRAPVISUALFLAG_* @endlink constants.',),

    'GetWrapVisualFlagsLocation':
        ('The return value will be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_WRAPVISUALFLAGLOC_DEFAULT '
        'wxSTC_WRAPVISUALFLAGLOC_* @endlink constants.',),

    'SetWrapVisualFlagsLocation':
        ('The input should be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_WRAPVISUALFLAGLOC_DEFAULT '
        'wxSTC_WRAPVISUALFLAGLOC_* @endlink constants.',),

    'GetWrapIndentMode':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_WRAPINDENT_FIXED wxSTC_WRAPINDENT_* '
        '@endlink constants.',),

    'SetWrapIndentMode':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_WRAPINDENT_FIXED wxSTC_WRAPINDENT_* '
        '@endlink constants.',),

    'GetLayoutCache':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_CACHE_NONE wxSTC_CACHE_* @endlink constants.',),

    'SetLayoutCache':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_CACHE_NONE wxSTC_CACHE_* @endlink constants.',),

    'GetEdgeMode':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_EDGE_NONE wxSTC_EDGE_* @endlink constants.',),

    'SetEdgeMode':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_EDGE_NONE wxSTC_EDGE_* @endlink constants.',),

    'GetLexer':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_LEX_A68K wxSTC_LEX_* @endlink constants.',),

    'SetLexer':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_LEX_A68K wxSTC_LEX_* @endlink constants.',),

    'GetModEventMask':
        ('The return value will wxSTC_MODEVENTMASKALL if all changes generate',
         'events.  Otherwise it will be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_MOD_INSERTTEXT wxSTC_MOD_* @endlink constants,',
        'the @link wxStyledTextCtrl::wxSTC_PERFORMED_REDO wxSTC_PERFORMED_* @endlink constants,',
        'wxSTC_STARTACTION, wxSTC_MULTILINEUNDOREDO, '
        'wxSTC_MULTISTEPUNDOREDO, and wxSTC_LASTSTEPINUNDOREDO.',),

    'SetModEventMask':
        ('The input should be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_MOD_INSERTTEXT wxSTC_MOD_* @endlink constants,',
        'the @link wxStyledTextCtrl::wxSTC_PERFORMED_REDO wxSTC_PERFORMED_* @endlink constants,',
        'wxSTC_STARTACTION, wxSTC_MULTILINEUNDOREDO, '
        'wxSTC_MULTISTEPUNDOREDO, and wxSTC_LASTSTEPINUNDOREDO.',
        'The input can also be wxSTC_MODEVENTMASKALL to indicate that all changes',
        'should generate events.'),

    'GetMouseDwellTime':
        ('The return value will be a time in milliseconds or wxSTC_TIME_FOREVER.',),

    'SetMouseDwellTime':
        ('The input should be a time in milliseconds or wxSTC_TIME_FOREVER.',),

    'FindText':
        ('@param minPos',
        '    The position (starting from zero) in the document at which to begin',
        '    the search',
        '@param maxPos',
        '    The last position (starting from zero) in the document to which',
        '    the search will be restricted.',
        '@param text',
        '    The text to search for.',
        '@param flags',
        '    (Optional)  The search flags.  This should be a bit list containing',
        '     one or more of the @link wxStyledTextCtrl::wxSTC_FIND_WHOLEWORD',
        '    wxSTC_FIND_* @endlink constants.',
        '@param findEnd',
        '    (Optional)  This parameter can optionally be used to receive the',
        '    end position (starting from zero) of the found text.  This is',
        '    primarily needed when searching using regular expressions.',
        '    This parameter is available since wxWidgets 3.1.1.',
        '@return',
        '    The position (starting from zero) in the document at which the text',
        '    was found or wxSTC_INVALID_POSITION if the search fails.',
        '@remarks',
        '    A backwards search can be performed by setting minPos to be greater',
        '    than maxPos.',),

    'AddUndoAction':
        ('The flags argument can be either 0 or wxSTC_UNDO_MAY_COALESCE.',),

    'AssignCmdKey':
        ('The second argument should be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_KEYMOD_NORM wxSTC_KEYMOD_* @endlink constants',
        'and the third argument should be one of the',
        '@link wxStyledTextCtrl::wxSTC_CMD_BACKTAB wxSTC_CMD_* @endlink constants.',),

    'ClearCmdKey':
        ('The second argument should be a bit list containing one or more of the',
        '@link wxStyledTextCtrl::wxSTC_KEYMOD_NORM wxSTC_KEYMOD_* @endlink constants.'),

    'FoldLine':
        ('The second argument should be one of the',
        '@link wxStyledTextCtrl::wxSTC_FOLDACTION_CONTRACT wxSTC_FOLDACTION_* '
        '@endlink constants.'),

    'FoldChildren':
        ('The second argument should be one of the',
        '@link wxStyledTextCtrl::wxSTC_FOLDACTION_CONTRACT wxSTC_FOLDACTION_* '
        '@endlink constants.'),

    'FoldAll':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_FOLDACTION_CONTRACT wxSTC_FOLDACTION_* '
        '@endlink constants.'),

    'UsePopUp':
        ('The input should be one of the',
        '@link wxStyledTextCtrl::wxSTC_POPUP_NEVER wxSTC_POPUP_* @endlink constants.',
        '@remarks',
        'When wxContextMenuEvent is used to create a custom popup menu,',
        'this function should be called with wxSTC_POPUP_NEVER.  Otherwise',
        'the default menu will be shown instead of the custom one.',),

    'PropertyType':
        ('The return value will be one of the',
        '@link wxStyledTextCtrl::wxSTC_TYPE_BOOLEAN wxSTC_TYPE_* @endlink constants.',),

    'GetCurLine':
        ('linePos can optionally be passed in to receive the index of the',
        'caret on the line.',),

    'StyleSetCharacterSet':
        ('Converts the Scintilla character set values to a wxFontEncoding.',),

}


# This map can be used to add since annotations to the interface header.
sinceAnnotations= {
    'AddTabStop':'3.1.0',
    'AllocateExtendedStyles':'3.1.0',
    'AllocateSubStyles':'3.1.0',
    'AutoCGetMulti':'3.1.0',
    'AutoCGetOrder':'3.1.0',
    'AutoCSetMulti':'3.1.0',
    'AutoCSetOrder':'3.1.0',
    'CallTipPosStart':'3.1.0',
    'ChangeInsertion':'3.1.0',
    'ClearRepresentation':'3.1.0',
    'ClearTabStops':'3.1.0',
    'DistanceToSecondaryStyles':'3.1.0',
    'DropSelectionN':'3.1.0',
    'ExpandChildren':'3.1.0',
    'FoldAll':'3.1.0',
    'FoldChildren':'3.1.0',
    'FoldLine':'3.1.0',
    'FreeSubStyles':'3.1.0',
    'GetAutomaticFold':'3.1.0',
    'GetCaretLineVisibleAlways':'3.1.0',
    'GetIMEInteraction':'3.1.0',
    'GetLineEndTypesActive':'3.1.0',
    'GetLineEndTypesAllowed':'3.1.0',
    'GetLineEndTypesSupported':'3.1.0',
    'GetMouseSelectionRectangularSwitch':'3.1.0',
    'GetNextTabStop':'3.1.0',
    'GetPhasesDraw':'3.1.0',
    'GetPrimaryStyleFromStyle':'3.1.0',
    'GetRepresentation':'3.1.0',
    'GetSelectionEmpty':'3.1.0',
    'GetStyleFromSubStyle':'3.1.0',
    'GetSubStyleBases':'3.1.0',
    'GetSubStylesLength':'3.1.0',
    'GetSubStylesStart':'3.1.0',
    'GetTargetText':'3.1.0',
    'IndicGetFlags':'3.1.0',
    'IndicGetHoverFore':'3.1.0',
    'IndicGetHoverStyle':'3.1.0',
    'IndicSetFlags':'3.1.0',
    'IndicSetHoverFore':'3.1.0',
    'IndicSetHoverStyle':'3.1.0',
    'PositionRelative':'3.1.0',
    'ReleaseAllExtendedStyles':'3.1.0',
    'RGBAImageSetScale':'3.1.0',
    'ScrollRange':'3.1.0',
    'SetAutomaticFold':'3.1.0',
    'SetCaretLineVisibleAlways':'3.1.0',
    'SetIdentifiers':'3.1.0',
    'SetIMEInteraction':'3.1.0',
    'SetLineEndTypesAllowed':'3.1.0',
    'SetMouseSelectionRectangularSwitch':'3.1.0',
    'SetPhasesDraw':'3.1.0',
    'SetRepresentation':'3.1.0',
    'SetTargetRange':'3.1.0',
    'VCHomeDisplay':'3.1.0',
    'VCHomeDisplayExtend':'3.1.0',

    'AutoCGetCurrentText':'3.1.1',
    'FoldDisplayTextSetStyle':'3.1.1',
    'GetDirectFunction':'3.1.1',
    'GetDirectPointer':'3.1.1',
    'GetFontQuality':'3.1.1',
    'GetIdleStyling':'3.1.1',
    'GetLexerLanguage':'3.1.1',
    'GetMarginBackN':'3.1.1',
    'GetMargins':'3.1.1',
    'GetMouseWheelCaptures':'3.1.1',
    'GetTabDrawMode':'3.1.1',
    'IsRangeWord':'3.1.1',
    'LoadLexerLibrary':'3.1.1',
    'MultiEdgeAddLine':'3.1.1',
    'MultiEdgeClearAll':'3.1.1',
    'MultipleSelectAddEach':'3.1.1',
    'MultipleSelectAddNext':'3.1.1',
    'SetFontQuality':'3.1.1',
    'SetIdleStyling':'3.1.1',
    'SetMarginBackN':'3.1.1',
    'SetMargins':'3.1.1',
    'SetMouseWheelCaptures':'3.1.1',
    'SetTabDrawMode':'3.1.1',
    'TargetWholeDocument':'3.1.1',
    'ToggleFoldShowText':'3.1.1',

    'MarkerDefinePixmap':'3.1.3',
    'RegisterImage':'3.1.3'
}


#----------------------------------------------------------------------------

def buildDocs(name, docs, icat):
    docsLong = 0

    # If an item does not have a category or the category to which it is
    # assigned is not in categoriesList, it will be assigned to 'OtherSettings'
    if name in docsMap:
        category = docsMap[name]
    else:
        category = 'OtherSettings'

    if category not in [ x for (x,y,z) in categoriesList]:
        category = 'OtherSettings'

    if name in docOverrides:
        docs = docOverrides[name]

    if name in docSubstitutions:
        post = docSubstitutions[name]
        newdocs = []
        for d in docs:
            for x,y in post.items():
                d=d.replace(x,y)
            newdocs.append(d)
        newdocs[:] = [x for x in newdocs if x.strip()!='']
        docs=tuple(newdocs)

    if name in extendedDocs:
        docsLong = extendedDocs[name]

    if icat=='Provisional':
        note = ('','This method is provisional and is subject to change'
                'in future versions of wxStyledTextCtrl.',)
        if docsLong==0:
            docsLong = note
        else:
            docsLong = docsLong + note

    if name in sinceAnnotations:
        note = ('@since '+sinceAnnotations[name],)
        if docsLong==0:
            docsLong = note
        else:
            docsLong = docsLong + note

    if category=='DeprecatedMessages' or icat=='Deprecated':
        note = ('@deprecated',)
        if docsLong==0:
            docsLong = note
        else:
            docsLong = docsLong + note

    return category, docs, docsLong
