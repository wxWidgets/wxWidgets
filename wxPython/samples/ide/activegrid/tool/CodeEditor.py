#----------------------------------------------------------------------------
# Name:         CodeEditor.py
# Purpose:      Abstract Code Editor for pydocview tbat uses the Styled Text Control
#
# Author:       Peter Yared
#
# Created:      8/10/03
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------


import STCTextEditor
import wx
import wx.lib.docview
import OutlineService
import os
import re
import string
import sys
import DebuggerService
import MarkerService
_ = wx.GetTranslation
if wx.Platform == '__WXMSW__':
    _WINDOWS = True
else:
    _WINDOWS = False


EXPAND_TEXT_ID = wx.NewId()
COLLAPSE_TEXT_ID = wx.NewId()
EXPAND_TOP_ID = wx.NewId()
COLLAPSE_TOP_ID = wx.NewId()
EXPAND_ALL_ID = wx.NewId()
COLLAPSE_ALL_ID = wx.NewId()
CHECK_CODE_ID = wx.NewId()
AUTO_COMPLETE_ID = wx.NewId()
CLEAN_WHITESPACE = wx.NewId()
COMMENT_LINES_ID = wx.NewId()
UNCOMMENT_LINES_ID = wx.NewId()
INDENT_LINES_ID = wx.NewId()
DEDENT_LINES_ID = wx.NewId()
USE_TABS_ID = wx.NewId()
SET_INDENT_WIDTH_ID = wx.NewId()
FOLDING_ID = wx.NewId()


class CodeDocument(STCTextEditor.TextDocument):
    pass    


class CodeView(STCTextEditor.TextView):


    #----------------------------------------------------------------------------
    # Overridden methods
    #----------------------------------------------------------------------------


    def GetCtrlClass(self):
        """ Used in split window to instantiate new instances """
        return CodeCtrl


    def ProcessEvent(self, event):
        id = event.GetId()
        if id == EXPAND_TEXT_ID:
            self.GetCtrl().ToggleFold(self.GetCtrl().GetCurrentLine())
            return True
        elif id == COLLAPSE_TEXT_ID:
            self.GetCtrl().ToggleFold(self.GetCtrl().GetCurrentLine())
            return True
        elif id == EXPAND_TOP_ID:
            self.GetCtrl().ToggleFoldAll(expand = True, topLevelOnly = True)
            return True
        elif id == COLLAPSE_TOP_ID:
            self.GetCtrl().ToggleFoldAll(expand = False, topLevelOnly = True)
            return True
        elif id == EXPAND_ALL_ID:
            self.GetCtrl().ToggleFoldAll(expand = True)
            return True
        elif id == COLLAPSE_ALL_ID:
            self.GetCtrl().ToggleFoldAll(expand = False)
            return True
        elif id == CHECK_CODE_ID:
            self.OnCheckCode()
            return True
        elif id == AUTO_COMPLETE_ID:
            self.OnAutoComplete()
            return True
        elif id == CLEAN_WHITESPACE:
            self.OnCleanWhiteSpace()
            return True
        elif id == SET_INDENT_WIDTH_ID:
            self.OnSetIndentWidth()
            return True
        elif id == USE_TABS_ID:
            self.GetCtrl().SetUseTabs(not self.GetCtrl().GetUseTabs())
            return True
        elif id == INDENT_LINES_ID:
            self.GetCtrl().CmdKeyExecute(wx.stc.STC_CMD_TAB)
            return True
        elif id == DEDENT_LINES_ID:
            self.GetCtrl().CmdKeyExecute(wx.stc.STC_CMD_BACKTAB)
            return True
        elif id == COMMENT_LINES_ID:
            self.OnCommentLines()
            return True
        elif id == UNCOMMENT_LINES_ID:
            self.OnUncommentLines()
            return True
        else:
            return STCTextEditor.TextView.ProcessEvent(self, event)


    def ProcessUpdateUIEvent(self, event):
        if not self.GetCtrl():
            return False
        hasText = self.GetCtrl().GetTextLength() > 0
        id = event.GetId()
        if id == EXPAND_TEXT_ID:
            event.Enable(self.GetCtrl().CanLineExpand(self.GetCtrl().GetCurrentLine()))
            return True
        elif id == COLLAPSE_TEXT_ID:
            event.Enable(self.GetCtrl().CanLineCollapse(self.GetCtrl().GetCurrentLine()))
            return True
        elif id == EXPAND_TOP_ID:
            event.Enable(hasText)
            return True
        elif id == COLLAPSE_TOP_ID:
            event.Enable(hasText)
            return True
        elif id == EXPAND_ALL_ID:
            event.Enable(hasText)
            return True
        elif id == COLLAPSE_ALL_ID:
            event.Enable(hasText)
            return True
        elif id == CHECK_CODE_ID:
            event.Enable(False)
            return True
        elif id == AUTO_COMPLETE_ID:
            event.Enable(hasText)
            return True
        elif id == CLEAN_WHITESPACE:
            event.Enable(hasText)
            return True
        elif id == SET_INDENT_WIDTH_ID:
            event.Enable(True)
            return True
        elif id == USE_TABS_ID:
            event.Enable(True)
            event.Check(self.GetCtrl().GetUseTabs())
            return True
        elif id == INDENT_LINES_ID:
            event.Enable(hasText)
            return True
        elif id == DEDENT_LINES_ID:
            event.Enable(hasText)
            return True
        elif id == COMMENT_LINES_ID:
            event.Enable(hasText)
            return True
        elif id == UNCOMMENT_LINES_ID:
            event.Enable(hasText)
            return True
        elif id == FOLDING_ID:
            event.Enable(True)
            return True
        else:
            return STCTextEditor.TextView.ProcessUpdateUIEvent(self, event)


    #----------------------------------------------------------------------------
    # Methods for OutlineService
    #----------------------------------------------------------------------------

    def OnChangeFilename(self):
        wx.lib.docview.View.OnChangeFilename(self)
        self.LoadOutline(force=True)
        

    def ClearOutline(self):
        outlineService = wx.GetApp().GetService(OutlineService.OutlineService)
        if not outlineService:
            return

        outlineView = outlineService.GetView()
        if not outlineView:
            return

        outlineView.ClearTreeCtrl()


    def LoadOutline(self, force=False):
        outlineService = wx.GetApp().GetService(OutlineService.OutlineService)
        if not outlineService:
            return
        outlineService.LoadOutline(self, force=force)


    def DoLoadOutlineCallback(self, force=False):
        outlineService = wx.GetApp().GetService(OutlineService.OutlineService)
        if not outlineService:
            return False

        outlineView = outlineService.GetView()
        if not outlineView:
            return False

        treeCtrl = outlineView.GetTreeCtrl()
        if not treeCtrl:
            return False

        view = treeCtrl.GetCallbackView()
        newCheckSum = self.GenCheckSum()
        if not force:
            if view and view is self:
                if self._checkSum == newCheckSum:
                    return False
        self._checkSum = newCheckSum

        treeCtrl.DeleteAllItems()

        document = self.GetDocument()
        if not document:
            return True

        filename = document.GetFilename()
        if filename:
            rootItem = treeCtrl.AddRoot(os.path.basename(filename))
            treeCtrl.SetDoSelectCallback(rootItem, self, None)
        else:
            return True

        text = self.GetValue()
        if not text:
            return True

        CLASS_PATTERN = 'class[ \t]+\w+.*?:'
        DEF_PATTERN = 'def[ \t]+\w+\(.*?\)'
        classPat = re.compile(CLASS_PATTERN, re.M|re.S)
        defPat= re.compile(DEF_PATTERN, re.M|re.S)
        pattern = re.compile('^[ \t]*((' + CLASS_PATTERN + ')|('+ DEF_PATTERN +'.*?:)).*?$', re.M|re.S)

        iter = pattern.finditer(text)
        indentStack = [(0, rootItem)]
        for pattern in iter:
            line = pattern.string[pattern.start(0):pattern.end(0)]
            classLine = classPat.search(line)
            if classLine:
                indent = classLine.start(0)
                itemStr = classLine.string[classLine.start(0):classLine.end(0)-1]  # don't take the closing ':'
            else:
                defLine = defPat.search(line)
                if defLine:
                    indent = defLine.start(0)
                    itemStr = defLine.string[defLine.start(0):defLine.end(0)]

            if indent == 0:
                parentItem = rootItem
            else:
                lastItem = indentStack.pop()
                while lastItem[0] >= indent:
                    lastItem = indentStack.pop()
                indentStack.append(lastItem)
                parentItem = lastItem[1]

            item = treeCtrl.AppendItem(parentItem, itemStr)
            treeCtrl.SetDoSelectCallback(item, self, (pattern.end(0), pattern.start(0) + indent))  # select in reverse order because we want the cursor to be at the start of the line so it wouldn't scroll to the right
            indentStack.append((indent, item))

        treeCtrl.Expand(rootItem)

        return True


    def DoSelectCallback(self, data):
        if data:
            self.EnsureVisibleEnforcePolicy(self.LineFromPosition(data[0]))
            # wxBug: need to select in reverse order (end, start) to place cursor at begining of line,
            #        otherwise, display is scrolled over to the right hard and is hard to view
            self.SetSelection(data[1], data[0])


##    def checksum(self, bytes):        
##        def rotate_right(c):
##            if c&1:
##                return (c>>1)|0x8000
##            else:
##                return c>>1
##                
##        result = 0
##        for ch in bytes:
##            ch = ord(ch) & 0xFF
##            result = (rotate_right(result)+ch) & 0xFFFF
##        return result
##

    def GenCheckSum(self):
        """ Poor man's checksum.  We'll assume most changes will change the length of the file.
        """
        text = self.GetValue()
        if text:
            return len(text)
        else:
            return 0


    #----------------------------------------------------------------------------
    # Format methods
    #----------------------------------------------------------------------------

    def OnCheckCode(self):
        """ Need to overshadow this for each specific subclass """
        if 0:
            try:
                code = self.GetCtrl().GetText()
                codeObj = compile(code, self.GetDocument().GetFilename(), 'exec')
                self._GetParentFrame().SetStatusText(_("The file successfully compiled"))
            except SyntaxError, (message, (fileName, line, col, text)):
                pos = self.GetCtrl().PositionFromLine(line - 1) + col - 1
                self.GetCtrl().SetSelection(pos, pos)
                self._GetParentFrame().SetStatusText(_("Syntax Error: %s") % message)
            except:
                self._GetParentFrame().SetStatusText(sys.exc_info()[0])


    def OnAutoComplete(self):
        self.GetCtrl().AutoCompCancel()
        self.GetCtrl().AutoCompSetAutoHide(0)
        self.GetCtrl().AutoCompSetChooseSingle(True)
        self.GetCtrl().AutoCompSetIgnoreCase(True)
        context, hint = self.GetAutoCompleteHint()
        replaceList, replaceLen = self.GetAutoCompleteKeywordList(context, hint)
        if replaceList and len(replaceList) != 0: 
            self.GetCtrl().AutoCompShow(replaceLen, replaceList)


    def GetAutoCompleteHint(self):
        """ Replace this method with Editor specific method """
        pos = self.GetCtrl().GetCurrentPos()
        if pos == 0:
            return None, None
        if chr(self.GetCtrl().GetCharAt(pos - 1)) == '.':
            pos = pos - 1
            hint = None
        else:
            hint = ''
            
        validLetters = string.letters + string.digits + '_.'
        word = ''
        while (True):
            pos = pos - 1
            if pos < 0:
                break
            char = chr(self.GetCtrl().GetCharAt(pos))
            if char not in validLetters:
                break
            word = char + word
            
        context = word
        if hint is not None:            
            lastDot = word.rfind('.')
            if lastDot != -1:
                context = word[0:lastDot]
                hint = word[lastDot+1:]
                    
        return context, hint
        

    def GetAutoCompleteDefaultKeywords(self):
        """ Replace this method with Editor specific keywords """
        return ['Put', 'Editor Specific', 'Keywords', 'Here']


    def CaseInsensitiveCompare(self, s1, s2):
        """ GetAutoCompleteKeywordList() method used to show keywords in case insensitive order """
        s1L = s1.lower()
        s2L = s2.lower()
        if s1L == s2L:
            return 0
        elif s1L < s2L:
            return -1
        else:
            return 1


    def GetAutoCompleteKeywordList(self, context, hint):            
        """ Replace this method with Editor specific keywords """
        kw = self.GetAutoCompleteDefaultKeywords()
        
        if hint and len(hint):
            lowerHint = hint.lower()
            filterkw = filter(lambda item: item.lower().startswith(lowerHint), kw)  # remove variables and methods that don't match hint
            kw = filterkw

        if hint:
            replaceLen = len(hint)
        else:
            replaceLen = 0
            
        kw.sort(self.CaseInsensitiveCompare)
        return " ".join(kw), replaceLen
        

    def OnCleanWhiteSpace(self):
        newText = ""
        for lineNo in self._GetSelectedLineNumbers():
            lineText = string.rstrip(self.GetCtrl().GetLine(lineNo))
            indent = 0
            lstrip = 0
            for char in lineText:
                if char == '\t':
                    indent = indent + self.GetCtrl().GetIndent()
                    lstrip = lstrip + 1
                elif char in string.whitespace:
                    indent = indent + 1
                    lstrip = lstrip + 1
                else:
                    break
            if self.GetCtrl().GetUseTabs():
                indentText = (indent / self.GetCtrl().GetIndent()) * '\t' + (indent % self.GetCtrl().GetIndent()) * ' '
            else:
                indentText = indent * ' '
            lineText = indentText + lineText[lstrip:] + '\n'
            newText = newText + lineText
        self._ReplaceSelectedLines(newText)


    def OnSetIndentWidth(self):
        dialog = wx.TextEntryDialog(self._GetParentFrame(), _("Enter new indent width (2-10):"), _("Set Indent Width"), "%i" % self.GetCtrl().GetIndent())
        if dialog.ShowModal() == wx.ID_OK:
            try:
                indent = int(dialog.GetValue())
                if indent >= 2 and indent <= 10:
                    self.GetCtrl().SetIndent(indent)
                    self.GetCtrl().SetTabWidth(indent)
            except:
                pass
        dialog.Destroy()


    def GetIndentWidth(self):
        return self.GetCtrl().GetIndent()
                

    def OnCommentLines(self):
        newText = ""
        for lineNo in self._GetSelectedLineNumbers():
            lineText = self.GetCtrl().GetLine(lineNo)
            if (len(lineText) > 1 and lineText[0] == '#') or (len(lineText) > 2 and lineText[:2] == '##'):
                newText = newText + lineText
            else:
                newText = newText + "##" + lineText
        self._ReplaceSelectedLines(newText)


    def OnUncommentLines(self):
        newText = ""
        for lineNo in self._GetSelectedLineNumbers():
            lineText = self.GetCtrl().GetLine(lineNo)
            if len(lineText) >= 2 and lineText[:2] == "##":
                lineText = lineText[2:]
            elif len(lineText) >= 1 and lineText[:1] == "#":
                lineText = lineText[1:]
            newText = newText + lineText
        self._ReplaceSelectedLines(newText)


    def _GetSelectedLineNumbers(self):
        selStart, selEnd = self._GetPositionsBoundingSelectedLines()
        return range(self.GetCtrl().LineFromPosition(selStart), self.GetCtrl().LineFromPosition(selEnd))


    def _GetPositionsBoundingSelectedLines(self):
        startPos = self.GetCtrl().GetCurrentPos()
        endPos = self.GetCtrl().GetAnchor()
        if startPos > endPos:
            temp = endPos
            endPos = startPos
            startPos = temp
        if endPos == self.GetCtrl().PositionFromLine(self.GetCtrl().LineFromPosition(endPos)):
            endPos = endPos - 1  # If it's at the very beginning of a line, use the line above it as the ending line
        selStart = self.GetCtrl().PositionFromLine(self.GetCtrl().LineFromPosition(startPos))
        selEnd = self.GetCtrl().PositionFromLine(self.GetCtrl().LineFromPosition(endPos) + 1)
        return selStart, selEnd


    def _ReplaceSelectedLines(self, text):
        if len(text) == 0:
            return
        selStart, selEnd = self._GetPositionsBoundingSelectedLines()
        self.GetCtrl().SetSelection(selStart, selEnd)
        self.GetCtrl().ReplaceSelection(text)
        self.GetCtrl().SetSelection(selStart + len(text), selStart)


    def OnUpdate(self, sender = None, hint = None):
        if hint == "ViewStuff":
            self.GetCtrl().SetViewDefaults()
        elif hint == "Font":
            font, color = self.GetFontAndColorFromConfig()
            self.GetCtrl().SetFont(font)
            self.GetCtrl().SetFontColor(color)
        else:
            dbg_service = wx.GetApp().GetService(DebuggerService.DebuggerService)
            if dbg_service:
                dbg_service.SetCurrentBreakpointMarkers(self)


class CodeService(STCTextEditor.TextService):


    def __init__(self):
        STCTextEditor.TextService.__init__(self)


    def InstallControls(self, frame, menuBar = None, toolBar = None, statusBar = None, document = None):
        # TODO NEED TO DO INSTANCEOF CHECK HERE FOR SDI
        #if document and document.GetDocumentTemplate().GetDocumentType() != TextDocument:
        #    return
        if not document and wx.GetApp().GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI:
            return

        viewMenu = menuBar.GetMenu(menuBar.FindMenu(_("&View")))
        isWindows = (wx.Platform == '__WXMSW__')

        if not menuBar.FindItemById(EXPAND_TEXT_ID):  # check if below menu items have been already been installed
            foldingMenu = wx.Menu()
            if isWindows:
                foldingMenu.Append(EXPAND_TEXT_ID, _("&Expand\tNumpad-Plus"), _("Expands a collapsed block of text"))
            else:
                foldingMenu.Append(EXPAND_TEXT_ID, _("&Expand"), _("Expands a collapsed block of text"))

            wx.EVT_MENU(frame, EXPAND_TEXT_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, EXPAND_TEXT_ID, frame.ProcessUpdateUIEvent)
            
            if isWindows:
                foldingMenu.Append(COLLAPSE_TEXT_ID, _("&Collapse\tNumpad+Minus"), _("Collapse a block of text"))
            else:
                foldingMenu.Append(COLLAPSE_TEXT_ID, _("&Collapse"), _("Collapse a block of text"))
            wx.EVT_MENU(frame, COLLAPSE_TEXT_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, COLLAPSE_TEXT_ID, frame.ProcessUpdateUIEvent)
            
            if isWindows:
                foldingMenu.Append(EXPAND_TOP_ID, _("Expand &Top Level\tCtrl+Numpad+Plus"), _("Expands the top fold levels in the document"))
            else:
                foldingMenu.Append(EXPAND_TOP_ID, _("Expand &Top Level"), _("Expands the top fold levels in the document"))
            wx.EVT_MENU(frame, EXPAND_TOP_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, EXPAND_TOP_ID, frame.ProcessUpdateUIEvent)
            
            if isWindows:
                foldingMenu.Append(COLLAPSE_TOP_ID, _("Collapse Top &Level\tCtrl+Numpad+Minus"), _("Collapses the top fold levels in the document"))
            else:
                foldingMenu.Append(COLLAPSE_TOP_ID, _("Collapse Top &Level"), _("Collapses the top fold levels in the document"))
            wx.EVT_MENU(frame, COLLAPSE_TOP_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, COLLAPSE_TOP_ID, frame.ProcessUpdateUIEvent)
            
            if isWindows:
                foldingMenu.Append(EXPAND_ALL_ID, _("Expand &All\tShift+Numpad+Plus"), _("Expands all of the fold levels in the document"))
            else:
                foldingMenu.Append(EXPAND_ALL_ID, _("Expand &All"), _("Expands all of the fold levels in the document"))
            wx.EVT_MENU(frame, EXPAND_ALL_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, EXPAND_ALL_ID, frame.ProcessUpdateUIEvent)
            
            if isWindows:
                foldingMenu.Append(COLLAPSE_ALL_ID, _("Colla&pse All\tShift+Numpad+Minus"), _("Collapses all of the fold levels in the document"))
            else:
                foldingMenu.Append(COLLAPSE_ALL_ID, _("Colla&pse All"), _("Collapses all of the fold levels in the document"))
            wx.EVT_MENU(frame, COLLAPSE_ALL_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, COLLAPSE_ALL_ID, frame.ProcessUpdateUIEvent)
            
            viewMenu.AppendMenu(FOLDING_ID, _("&Folding"), foldingMenu)
            wx.EVT_UPDATE_UI(frame, FOLDING_ID, frame.ProcessUpdateUIEvent)

        formatMenuIndex = menuBar.FindMenu(_("&Format"))
        if formatMenuIndex > -1:
            formatMenu = menuBar.GetMenu(formatMenuIndex)
        else:
            formatMenu = wx.Menu()
        if not menuBar.FindItemById(CHECK_CODE_ID):  # check if below menu items have been already been installed
            formatMenu.AppendSeparator()
            formatMenu.Append(CHECK_CODE_ID, _("&Check Code"), _("Checks the document for syntax and indentation errors"))
            wx.EVT_MENU(frame, CHECK_CODE_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, CHECK_CODE_ID, frame.ProcessUpdateUIEvent)
            formatMenu.Append(AUTO_COMPLETE_ID, _("&Auto Complete\tCtrl+Space"), _("Provides suggestions on how to complete the current statement"))
            wx.EVT_MENU(frame, AUTO_COMPLETE_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, AUTO_COMPLETE_ID, frame.ProcessUpdateUIEvent)
            formatMenu.Append(CLEAN_WHITESPACE, _("Clean &Whitespace"), _("Converts leading spaces to tabs or vice versa per 'use tabs' and clears trailing spaces"))
            wx.EVT_MENU(frame, CLEAN_WHITESPACE, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, CLEAN_WHITESPACE, frame.ProcessUpdateUIEvent)
            formatMenu.AppendSeparator()
            formatMenu.Append(INDENT_LINES_ID, _("&Indent Lines\tTab"), _("Indents the selected lines one indent width"))
            wx.EVT_MENU(frame, INDENT_LINES_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, INDENT_LINES_ID, frame.ProcessUpdateUIEvent)
            formatMenu.Append(DEDENT_LINES_ID, _("&Dedent Lines\tShift+Tab"), _("Dedents the selected lines one indent width"))
            wx.EVT_MENU(frame, DEDENT_LINES_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, DEDENT_LINES_ID, frame.ProcessUpdateUIEvent)
            formatMenu.Append(COMMENT_LINES_ID, _("Comment &Lines\tCtrl+Q"), _("Comments out the selected lines be prefixing each one with a comment indicator"))
            wx.EVT_MENU(frame, COMMENT_LINES_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, COMMENT_LINES_ID, frame.ProcessUpdateUIEvent)
            formatMenu.Append(UNCOMMENT_LINES_ID, _("&Uncomment Lines\tCtrl+Shift+Q"), _("Removes comment prefixes from each of the selected lines"))
            wx.EVT_MENU(frame, UNCOMMENT_LINES_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, UNCOMMENT_LINES_ID, frame.ProcessUpdateUIEvent)
            formatMenu.AppendSeparator()
            formatMenu.AppendCheckItem(USE_TABS_ID, _("Use &Tabs"), _("Toggles use of tabs or whitespaces for indents"))
            wx.EVT_MENU(frame, USE_TABS_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, USE_TABS_ID, frame.ProcessUpdateUIEvent)
            formatMenu.Append(SET_INDENT_WIDTH_ID, _("&Set Indent Width..."), _("Sets the indent width"))
            wx.EVT_MENU(frame, SET_INDENT_WIDTH_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, SET_INDENT_WIDTH_ID, frame.ProcessUpdateUIEvent)
        if formatMenuIndex == -1:
            viewMenuIndex = menuBar.FindMenu(_("&View"))
            menuBar.Insert(viewMenuIndex + 1, formatMenu, _("&Format"))

##        accelTable = wx.AcceleratorTable([
##            (wx.ACCEL_NORMAL, wx.WXK_TAB, INDENT_LINES_ID),
##            (wx.ACCEL_SHIFT, wx.WXK_TAB, DEDENT_LINES_ID),
##            eval(_("wx.ACCEL_CTRL, ord('Q'), COMMENT_LINES_ID")),
##            eval(_("wx.ACCEL_CTRL | wx.ACCEL_SHIFT, ord('Q'), UNCOMMENT_LINES_ID"))
##            ])
##        frame.SetAcceleratorTable(accelTable)

    def ProcessUpdateUIEvent(self, event):
        id = event.GetId()
        if id == EXPAND_TEXT_ID:
            event.Enable(False)
            return True
        elif id == COLLAPSE_TEXT_ID:
            event.Enable(False)
            return True
        elif id == EXPAND_TOP_ID:
            event.Enable(False)
            return True
        elif id == COLLAPSE_TOP_ID:
            event.Enable(False)
            return True
        elif id == EXPAND_ALL_ID:
            event.Enable(False)
            return True
        elif id == COLLAPSE_ALL_ID:
            event.Enable(False)
            return True
        elif id == CHECK_CODE_ID:
            event.Enable(False)
            return True
        elif id == AUTO_COMPLETE_ID:
            event.Enable(False)
            return True
        elif id == CLEAN_WHITESPACE:
            event.Enable(False)
            return True
        elif id == SET_INDENT_WIDTH_ID:
            event.Enable(False)
            return True
        elif id == USE_TABS_ID:
            event.Enable(False)
            return True
        elif id == INDENT_LINES_ID:
            event.Enable(False)
            return True
        elif id == DEDENT_LINES_ID:
            event.Enable(False)
            return True
        elif id == COMMENT_LINES_ID:
            event.Enable(False)
            return True
        elif id == UNCOMMENT_LINES_ID:
            event.Enable(False)
            return True
        elif id == FOLDING_ID:
            event.Enable(False)
            return True
        else:
            return STCTextEditor.TextService.ProcessUpdateUIEvent(self, event)


class CodeCtrl(STCTextEditor.TextCtrl):
    CURRENT_LINE_MARKER_NUM = 2
    BREAKPOINT_MARKER_NUM = 1
    CURRENT_LINE_MARKER_MASK = 0x4
    BREAKPOINT_MARKER_MASK = 0x2
    
            
    def __init__(self, parent, ID = -1, style = wx.NO_FULL_REPAINT_ON_RESIZE):
        if ID == -1:
            ID = wx.NewId()
        STCTextEditor.TextCtrl.__init__(self, parent, ID, style)
        
        self.UsePopUp(False)
        self.Bind(wx.EVT_RIGHT_UP, self.OnRightUp)
        self.SetProperty("fold", "1")

        # Setup a margin to hold fold markers
        #self.SetFoldFlags(16)  ###  WHAT IS THIS VALUE?  WHAT ARE THE OTHER FLAGS?  DOES IT MATTER?
        self.SetMarginType(2, wx.stc.STC_MARGIN_SYMBOL)
        self.SetMarginMask(2, wx.stc.STC_MASK_FOLDERS)
        self.SetMarginSensitive(2, True)
        self.SetMarginWidth(2, 12)
        
        self.SetMarginSensitive(1, False)
        self.SetMarginMask(1, 0x4)
        
        self.SetMarginSensitive(0, True)
        self.SetMarginType(0, wx.stc.STC_MARGIN_SYMBOL)
        self.SetMarginMask(0, 0x3)
        self.SetMarginWidth(0, 12)

        self.MarkerDefine(wx.stc.STC_MARKNUM_FOLDEREND,     wx.stc.STC_MARK_BOXPLUSCONNECTED,  "white", "black")
        self.MarkerDefine(wx.stc.STC_MARKNUM_FOLDEROPENMID, wx.stc.STC_MARK_BOXMINUSCONNECTED, "white", "black")
        self.MarkerDefine(wx.stc.STC_MARKNUM_FOLDERMIDTAIL, wx.stc.STC_MARK_TCORNER,  "white", "black")
        self.MarkerDefine(wx.stc.STC_MARKNUM_FOLDERTAIL,    wx.stc.STC_MARK_LCORNER,  "white", "black")
        self.MarkerDefine(wx.stc.STC_MARKNUM_FOLDERSUB,     wx.stc.STC_MARK_VLINE,    "white", "black")
        self.MarkerDefine(wx.stc.STC_MARKNUM_FOLDER,        wx.stc.STC_MARK_BOXPLUS,  "white", "black")
        self.MarkerDefine(wx.stc.STC_MARKNUM_FOLDEROPEN,    wx.stc.STC_MARK_BOXMINUS, "white", "black")
        # Define the current line marker
        self.MarkerDefine(CodeCtrl.CURRENT_LINE_MARKER_NUM, wx.stc.STC_MARK_SHORTARROW, wx.BLACK, (255,255,128))
        # Define the breakpoint marker
        self.MarkerDefine(CodeCtrl.BREAKPOINT_MARKER_NUM, wx.stc.STC_MARK_CIRCLE, wx.BLACK, (255,0,0))
        
        if _WINDOWS:  # should test to see if menu item exists, if it does, add this workaround
            self.CmdKeyClear(wx.stc.STC_KEY_TAB, 0)  # menu item "Indent Lines" from CodeService.InstallControls() generates another INDENT_LINES_ID event, so we'll explicitly disable the tab processing in the editor

        wx.stc.EVT_STC_MARGINCLICK(self, ID, self.OnMarginClick)
        wx.EVT_KEY_DOWN(self, self.OnKeyPressed)
        if self.GetMatchingBraces(): 
            wx.stc.EVT_STC_UPDATEUI(self, ID, self.OnUpdateUI)

        self.StyleClearAll()
        self.UpdateStyles()
        

    def OnRightUp(self, event):
        #Hold onto the current line number, no way to get it later.
        self._rightClickPosition = self.PositionFromPoint(event.GetPosition())
        self._rightClickLine = self.LineFromPosition(self._rightClickPosition)
        self.PopupMenu(self.CreatePopupMenu(), event.GetPosition())
        self._rightClickLine = -1
        self._rightClickPosition = -1
        

    def CreatePopupMenu(self):
        TOGGLEBREAKPOINT_ID = wx.NewId()
        TOGGLEMARKER_ID = wx.NewId()
        SYNCTREE_ID = wx.NewId()
        
        menu = wx.Menu()
        
        self.Bind(wx.EVT_MENU, self.OnPopSyncOutline, id=SYNCTREE_ID)
        item = wx.MenuItem(menu, SYNCTREE_ID, _("Find in Outline View"))
        menu.AppendItem(item)
        menu.AppendSeparator()
        self.Bind(wx.EVT_MENU, self.OnPopToggleBP, id=TOGGLEBREAKPOINT_ID)
        item = wx.MenuItem(menu, TOGGLEBREAKPOINT_ID, _("Toggle Breakpoint"))
        menu.AppendItem(item)
        self.Bind(wx.EVT_MENU, self.OnPopToggleMarker, id=TOGGLEMARKER_ID)
        item = wx.MenuItem(menu, TOGGLEMARKER_ID, _("Toggle Marker"))
        menu.AppendItem(item)
        menu.AppendSeparator()
                
        itemIDs = [wx.ID_UNDO, wx.ID_REDO, None,
                   wx.ID_CUT, wx.ID_COPY, wx.ID_PASTE, wx.ID_CLEAR, None, wx.ID_SELECTALL]

        menuBar = wx.GetApp().GetTopWindow().GetMenuBar()
        for itemID in itemIDs:
            if not itemID:
                menu.AppendSeparator()
            else:
                item = menuBar.FindItemById(itemID)
                if item:
                    menu.Append(itemID, item.GetLabel())

        return menu
                

    def OnPopToggleBP(self, event):
        """ Toggle break point on right click line, not current line """
        wx.GetApp().GetService(DebuggerService.DebuggerService).OnToggleBreakpoint(event, line=self._rightClickLine)
      
  
    def OnPopToggleMarker(self, event):
        """ Toggle marker on right click line, not current line """
        wx.GetApp().GetDocumentManager().GetCurrentView().MarkerToggle(lineNum = self._rightClickLine)


    def OnPopSyncOutline(self, event):
        wx.GetApp().GetService(OutlineService.OutlineService).LoadOutline(wx.GetApp().GetDocumentManager().GetCurrentView(), position=self._rightClickPosition)
          

    def HasSelection(self):
        return self.GetSelectionStart() - self.GetSelectionEnd() != 0  


    def ClearCurrentLineMarkers(self):
        self.MarkerDeleteAll(CodeCtrl.CURRENT_LINE_MARKER_NUM)
        

    def ClearCurrentBreakpoinMarkers(self):
        self.MarkerDeleteAll(CodeCtrl.BREAKPOINT_MARKER_NUM)


    def GetDefaultFont(self):
        if wx.Platform == '__WXMSW__':
            font = "Courier New"
        else:
            font = "Courier"
        return wx.Font(10, wx.DEFAULT, wx.NORMAL, wx.NORMAL, faceName = font)


    def GetMatchingBraces(self):
        """ Overwrite this method for language specific braces """
        return "[]{}()"


    def CanWordWrap(self):
        return False
        

    def SetFont(self, font):
        self._font = font


    def SetFontColor(self, fontColor):
        self._fontColor = fontColor


    def UpdateStyles(self):

        if not self.GetFont():
            return

        faces = { 'font' : self.GetFont().GetFaceName(),
                  'size' : self.GetFont().GetPointSize(),
                  'size2': self.GetFont().GetPointSize() - 2,
                  'color' : "%02x%02x%02x" % (self.GetFontColor().Red(), self.GetFontColor().Green(), self.GetFontColor().Blue())
                  }

        # Global default styles for all languages
        self.StyleSetSpec(wx.stc.STC_STYLE_DEFAULT,     "face:%(font)s,fore:#FFFFFF,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_STYLE_LINENUMBER,  "face:%(font)s,back:#C0C0C0,face:%(font)s,size:%(size2)d" % faces)
        self.StyleSetSpec(wx.stc.STC_STYLE_CONTROLCHAR, "face:%(font)s" % faces)
        self.StyleSetSpec(wx.stc.STC_STYLE_BRACELIGHT,  "face:%(font)s,fore:#000000,back:#70FFFF,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_STYLE_BRACEBAD,    "face:%(font)s,fore:#000000,back:#FF0000,size:%(size)d" % faces)


    def OnKeyPressed(self, event):
        if self.CallTipActive():
            self.CallTipCancel()
        key = event.KeyCode()
        if False:  # key == wx.WXK_SPACE and event.ControlDown():
            pos = self.GetCurrentPos()
            # Tips
            if event.ShiftDown():
                self.CallTipSetBackground("yellow")
                self.CallTipShow(pos, 'param1, param2')
            # Code completion
            else:
                #lst = []
                #for x in range(50000):
                #    lst.append('%05d' % x)
                #st = string.join(lst)
                #print len(st)
                #self.AutoCompShow(0, st)

                kw = keyword.kwlist[:]
                kw.append("zzzzzz")
                kw.append("aaaaa")
                kw.append("__init__")
                kw.append("zzaaaaa")
                kw.append("zzbaaaa")
                kw.append("this_is_a_longer_value")
                kw.append("this_is_a_much_much_much_much_much_much_much_longer_value")

                kw.sort()  # Python sorts are case sensitive
                self.AutoCompSetIgnoreCase(False)  # so this needs to match

                self.AutoCompShow(0, string.join(kw))
        elif key == wx.WXK_RETURN:
            self.DoIndent()
        else:
            STCTextEditor.TextCtrl.OnKeyPressed(self, event)


    def DoIndent(self):
        self.AddText('\n')
        # Need to do a default one for all languges


    def OnMarginClick(self, evt):
        # fold and unfold as needed
        if evt.GetMargin() == 2:
            if evt.GetShift() and evt.GetControl():
                lineCount = self.GetLineCount()
                expanding = True

                # find out if we are folding or unfolding
                for lineNum in range(lineCount):
                    if self.GetFoldLevel(lineNum) & wx.stc.STC_FOLDLEVELHEADERFLAG:
                        expanding = not self.GetFoldExpanded(lineNum)
                        break;

                self.ToggleFoldAll(expanding)
            else:
                lineClicked = self.LineFromPosition(evt.GetPosition())
                if self.GetFoldLevel(lineClicked) & wx.stc.STC_FOLDLEVELHEADERFLAG:
                    if evt.GetShift():
                        self.SetFoldExpanded(lineClicked, True)
                        self.Expand(lineClicked, True, True, 1)
                    elif evt.GetControl():
                        if self.GetFoldExpanded(lineClicked):
                            self.SetFoldExpanded(lineClicked, False)
                            self.Expand(lineClicked, False, True, 0)
                        else:
                            self.SetFoldExpanded(lineClicked, True)
                            self.Expand(lineClicked, True, True, 100)
                    else:
                        self.ToggleFold(lineClicked)

        elif evt.GetMargin() == 0:
            #This is used to toggle breakpoints via the debugger service.
            db_service = wx.GetApp().GetService(DebuggerService.DebuggerService)
            if db_service:
                db_service.OnToggleBreakpoint(evt, line=self.LineFromPosition(evt.GetPosition()))
            

    def OnUpdateUI(self, evt):
        braces = self.GetMatchingBraces()
        
        # check for matching braces
        braceAtCaret = -1
        braceOpposite = -1
        charBefore = None
        caretPos = self.GetCurrentPos()
        if caretPos > 0:
            charBefore = self.GetCharAt(caretPos - 1)
            styleBefore = self.GetStyleAt(caretPos - 1)

        # check before
        if charBefore and chr(charBefore) in braces:
            braceAtCaret = caretPos - 1

        # check after
        if braceAtCaret < 0:
            charAfter = self.GetCharAt(caretPos)
            styleAfter = self.GetStyleAt(caretPos)
            if charAfter and chr(charAfter) in braces:
                braceAtCaret = caretPos

        if braceAtCaret >= 0:
            braceOpposite = self.BraceMatch(braceAtCaret)

        if braceAtCaret != -1  and braceOpposite == -1:
            self.BraceBadLight(braceAtCaret)
        else:
            self.BraceHighlight(braceAtCaret, braceOpposite)

        evt.Skip()


    def ToggleFoldAll(self, expand = True, topLevelOnly = False):
        i = 0
        lineCount = self.GetLineCount()
        while i < lineCount:
            if not topLevelOnly or (topLevelOnly and self.GetFoldLevel(i) & wx.stc.STC_FOLDLEVELNUMBERMASK  == wx.stc.STC_FOLDLEVELBASE):
                if (expand and self.CanLineExpand(i)) or (not expand and self.CanLineCollapse(i)):
                    self.ToggleFold(i)
            i = i + 1


    def CanLineExpand(self, line):
        return not self.GetFoldExpanded(line)


    def CanLineCollapse(self, line):
        return self.GetFoldExpanded(line) and self.GetFoldLevel(line) & wx.stc.STC_FOLDLEVELHEADERFLAG


    def Expand(self, line, doExpand, force=False, visLevels=0, level=-1):
        lastChild = self.GetLastChild(line, level)
        line = line + 1
        while line <= lastChild:
            if force:
                if visLevels > 0:
                    self.ShowLines(line, line)
                else:
                    self.HideLines(line, line)
            else:
                if doExpand:
                    self.ShowLines(line, line)

            if level == -1:
                level = self.GetFoldLevel(line)

            if level & wx.stc.STC_FOLDLEVELHEADERFLAG:
                if force:
                    if visLevels > 1:
                        self.SetFoldExpanded(line, True)
                    else:
                        self.SetFoldExpanded(line, False)
                    line = self.Expand(line, doExpand, force, visLevels-1)

                else:
                    if doExpand and self.GetFoldExpanded(line):
                        line = self.Expand(line, True, force, visLevels-1)
                    else:
                        line = self.Expand(line, False, force, visLevels-1)
            else:
                line = line + 1;

        return line


