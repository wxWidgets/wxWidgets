#----------------------------------------------------------------------------
# Name:         STCTextEditor.py
# Purpose:      Text Editor for wx.lib.pydocview tbat uses the Styled Text Control
#
# Author:       Peter Yared, Morgan Hua
#
# Created:      8/10/03
# CVS-ID:       $Id$
# Copyright:    (c) 2003-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import wx
import wx.stc
import wx.lib.docview
import wx.lib.multisash
import wx.lib.pydocview
import string
import FindService
import os
import sys
_ = wx.GetTranslation

#----------------------------------------------------------------------------
# Constants
#----------------------------------------------------------------------------

TEXT_ID = wx.NewId()
VIEW_WHITESPACE_ID = wx.NewId()
VIEW_EOL_ID = wx.NewId()
VIEW_INDENTATION_GUIDES_ID = wx.NewId()
VIEW_RIGHT_EDGE_ID = wx.NewId()
VIEW_LINE_NUMBERS_ID = wx.NewId()
ZOOM_ID = wx.NewId()
ZOOM_NORMAL_ID = wx.NewId()
ZOOM_IN_ID = wx.NewId()
ZOOM_OUT_ID = wx.NewId()
CHOOSE_FONT_ID = wx.NewId()
WORD_WRAP_ID = wx.NewId()
TEXT_STATUS_BAR_ID = wx.NewId()


#----------------------------------------------------------------------------
# Classes
#----------------------------------------------------------------------------

class TextDocument(wx.lib.docview.Document):


    def OnSaveDocument(self, filename):
        view = self.GetFirstView()
        docFile = file(self._documentFile, "w")
        docFile.write(view.GetValue())
        docFile.close()
        self.Modify(False)
        self.SetDocumentSaved(True)
        return True


    def OnOpenDocument(self, filename):
        view = self.GetFirstView()
        docFile = file(self._documentFile, 'r')
        data = docFile.read()
        view.SetValue(data)
        self.SetFilename(filename, True)
        self.Modify(False)
        self.UpdateAllViews()
        self._savedYet = True
        return True


    def IsModified(self):
        view = self.GetFirstView()
        if view:
            return wx.lib.docview.Document.IsModified(self) or view.IsModified()
        else:
            return wx.lib.docview.Document.IsModified(self)


    def Modify(self, mod):
        view = self.GetFirstView()
        wx.lib.docview.Document.Modify(self, mod)
        if not mod and view:
            view.SetModifyFalse()


    def OnCreateCommandProcessor(self):
        # Don't create a command processor, it has its own
        pass

# Use this to override MultiClient.Select to prevent yellow background.
def MultiClientSelectBGNotYellow(a):
        a.GetParent().multiView.UnSelect()
        a.selected = True
        #a.SetBackgroundColour(wx.Colour(255,255,0)) # Yellow
        a.Refresh()

class TextView(wx.lib.docview.View):
    MARKER_NUM = 0
    MARKER_MASK = 0x1
    
    #----------------------------------------------------------------------------
    # Overridden methods
    #----------------------------------------------------------------------------

    def __init__(self):
        wx.lib.docview.View.__init__(self)
        self._textEditor = None
        self._markerCount = 0
        self._commandProcessor = None
        self._multiSash = None


    def GetCtrlClass(self):
        return TextCtrl


    def GetCtrl(self):
        # look for active one first
        self._textEditor = self._GetActiveCtrl(self._multiSash)
        if self._textEditor == None:  # it is possible none are active
            # look for any existing one
            self._textEditor = self._FindCtrl(self._multiSash)
        return self._textEditor
        

##    def GetCtrls(self, parent = None):
##        """ Walk through the MultiSash windows and find all Ctrls """
##        controls = []
##        if isinstance(parent, self.GetCtrlClass()):
##            return [parent]
##        if hasattr(parent, "GetChildren"):
##            for child in parent.GetChildren():
##                controls = controls + self.GetCtrls(child)
##        return controls

            
    def OnCreate(self, doc, flags):
        frame = wx.GetApp().CreateDocumentFrame(self, doc, flags, style = wx.DEFAULT_FRAME_STYLE | wx.NO_FULL_REPAINT_ON_RESIZE)
        wx.lib.multisash.MultiClient.Select = MultiClientSelectBGNotYellow
        self._multiSash = wx.lib.multisash.MultiSash(frame, -1)
        self._multiSash.SetDefaultChildClass(self.GetCtrlClass()) # wxBug:  MultiSash instantiates the first TextCtrl with this call
        self._textEditor = self.GetCtrl()  # wxBug: grab the TextCtrl from the MultiSash datastructure
        self._CreateSizer(frame)
        self.Activate()
        frame.Show(True)
        frame.Layout()
        return True


    def _GetActiveCtrl(self, parent):
        """ Walk through the MultiSash windows and find the active Control """
        if isinstance(parent, wx.lib.multisash.MultiClient) and parent.selected:
            return parent.child
        if hasattr(parent, "GetChildren"):
            for child in parent.GetChildren():
                found = self._GetActiveCtrl(child)
                if found:
                    return found
        return None


    def _FindCtrl(self, parent):
        """ Walk through the MultiSash windows and find the first TextCtrl """
        if isinstance(parent, self.GetCtrlClass()):
            return parent
        if hasattr(parent, "GetChildren"):
            for child in parent.GetChildren():
                found = self._FindCtrl(child)
                if found:
                    return found
        return None


    def _CreateSizer(self, frame):
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(self._multiSash, 1, wx.EXPAND)
        frame.SetSizer(sizer)
        frame.SetAutoLayout(True)


    def OnUpdate(self, sender = None, hint = None):
        if hint == "ViewStuff":
            self.GetCtrl().SetViewDefaults()
        elif hint == "Font":
            font, color = self.GetFontAndColorFromConfig()
            self.GetCtrl().SetFont(font)
            self.GetCtrl().SetFontColor(color)

            
    def OnActivateView(self, activate, activeView, deactiveView):
        if activate and self.GetCtrl():
            # In MDI mode just calling set focus doesn't work and in SDI mode using CallAfter causes an endless loop
            if self.GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI:
                self.GetCtrl().SetFocus()
            else:
                wx.CallAfter(self.GetCtrl().SetFocus)
                   
                                
    def OnClose(self, deleteWindow = True):
        if not wx.lib.docview.View.OnClose(self, deleteWindow):
            return False
        self.Activate(False)
        if deleteWindow and self.GetFrame():
            self.GetFrame().Destroy()
        return True


    def ProcessEvent(self, event):        
        id = event.GetId()
        if id == wx.ID_UNDO:
            self.GetCtrl().Undo()
            return True
        elif id == wx.ID_REDO:
            self.GetCtrl().Redo()
            return True
        elif id == wx.ID_CUT:
            self.GetCtrl().Cut()
            return True
        elif id == wx.ID_COPY:
            self.GetCtrl().Copy()
            return True
        elif id == wx.ID_PASTE:
            self.GetCtrl().OnPaste()
            return True
        elif id == wx.ID_CLEAR:
            self.GetCtrl().OnClear()
            return True
        elif id == wx.ID_SELECTALL:
            self.GetCtrl().SetSelection(0, -1)
            return True
        elif id == VIEW_WHITESPACE_ID:
            self.GetCtrl().SetViewWhiteSpace(not self.GetCtrl().GetViewWhiteSpace())
            return True
        elif id == VIEW_EOL_ID:
            self.GetCtrl().SetViewEOL(not self.GetCtrl().GetViewEOL())
            return True
        elif id == VIEW_INDENTATION_GUIDES_ID:
            self.GetCtrl().SetViewIndentationGuides(not self.GetCtrl().GetViewIndentationGuides())
            return True
        elif id == VIEW_RIGHT_EDGE_ID:
            self.GetCtrl().SetViewRightEdge(not self.GetCtrl().GetViewRightEdge())
            return True
        elif id == VIEW_LINE_NUMBERS_ID:
            self.GetCtrl().SetViewLineNumbers(not self.GetCtrl().GetViewLineNumbers())
            return True
        elif id == ZOOM_NORMAL_ID:
            self.GetCtrl().SetZoom(0)
            return True
        elif id == ZOOM_IN_ID:
            self.GetCtrl().CmdKeyExecute(wx.stc.STC_CMD_ZOOMIN)
            return True
        elif id == ZOOM_OUT_ID:
            self.GetCtrl().CmdKeyExecute(wx.stc.STC_CMD_ZOOMOUT)
            return True
        elif id == CHOOSE_FONT_ID:
            self.OnChooseFont()
            return True
        elif id == WORD_WRAP_ID:
            self.GetCtrl().SetWordWrap(not self.GetCtrl().GetWordWrap())
            return True
        elif id == FindService.FindService.FIND_ID:
            self.OnFind()
            return True
        elif id == FindService.FindService.FIND_PREVIOUS_ID:
            self.DoFind(forceFindPrevious = True)
            return True
        elif id == FindService.FindService.FIND_NEXT_ID:
            self.DoFind(forceFindNext = True)
            return True
        elif id == FindService.FindService.REPLACE_ID:
            self.OnFind(replace = True)
            return True
        elif id == FindService.FindService.FINDONE_ID:
            self.DoFind()
            return True
        elif id == FindService.FindService.REPLACEONE_ID:
            self.DoFind(replace = True)
            return True
        elif id == FindService.FindService.REPLACEALL_ID:
            self.DoFind(replaceAll = True)
            return True
        elif id == FindService.FindService.GOTO_LINE_ID:
            self.OnGotoLine(event)
            return True
        else:
            return wx.lib.docview.View.ProcessEvent(self, event)


    def ProcessUpdateUIEvent(self, event):
        if not self.GetCtrl():
            return False

        hasSelection = self.GetCtrl().GetSelectionStart() != self.GetCtrl().GetSelectionEnd()
        hasText = self.GetCtrl().GetTextLength() > 0
        notOnLastChar = self.GetCtrl().GetSelectionStart() != self.GetCtrl().GetTextLength()

        id = event.GetId()
        if id == wx.ID_UNDO:
             event.Enable(self.GetCtrl().CanUndo())
             event.SetText(_("Undo") + '\t' + _('Ctrl+Z'))
             return True
        elif id == wx.ID_REDO:
            event.Enable(self.GetCtrl().CanRedo())
            event.SetText(_("Redo") + '\t' + _('Ctrl+Y'))
            return True
        elif id == wx.ID_CUT:
            event.Enable(hasSelection)
            return True
        elif id == wx.ID_COPY:
            event.Enable(hasSelection)
            return True
        elif id == wx.ID_PASTE:
            event.Enable(self.GetCtrl().CanPaste())
            return True
        elif id == wx.ID_CLEAR:
            event.Enable(hasSelection)
            return True
        elif id == wx.ID_SELECTALL:
            event.Enable(hasText)
            return True
        elif id == TEXT_ID:
            event.Enable(True)
            return True
        elif id == VIEW_WHITESPACE_ID:
            event.Enable(hasText)
            event.Check(self.GetCtrl().GetViewWhiteSpace())
            return True
        elif id == VIEW_EOL_ID:
            event.Enable(hasText)
            event.Check(self.GetCtrl().GetViewEOL())
            return True
        elif id == VIEW_INDENTATION_GUIDES_ID:
            event.Enable(hasText)
            event.Check(self.GetCtrl().GetIndentationGuides())
            return True
        elif id == VIEW_RIGHT_EDGE_ID:
            event.Enable(hasText)
            event.Check(self.GetCtrl().GetViewRightEdge())
            return True
        elif id == VIEW_LINE_NUMBERS_ID:
            event.Enable(hasText)
            event.Check(self.GetCtrl().GetViewLineNumbers())
            return True
        elif id == ZOOM_ID:
            event.Enable(True)
            return True
        elif id == ZOOM_NORMAL_ID:
            event.Enable(self.GetCtrl().GetZoom() != 0)
            return True
        elif id == ZOOM_IN_ID:
            event.Enable(self.GetCtrl().GetZoom() < 20)
            return True
        elif id == ZOOM_OUT_ID:
            event.Enable(self.GetCtrl().GetZoom() > -10)
            return True
        elif id == CHOOSE_FONT_ID:
            event.Enable(True)
            return True
        elif id == WORD_WRAP_ID:
            event.Enable(self.GetCtrl().CanWordWrap())
            event.Check(self.GetCtrl().CanWordWrap() and self.GetCtrl().GetWordWrap())
            return True
        elif id == FindService.FindService.FIND_ID:
            event.Enable(hasText)
            return True
        elif id == FindService.FindService.FIND_PREVIOUS_ID:
            event.Enable(hasText and
                         self._FindServiceHasString() and
                         self.GetCtrl().GetSelection()[0] > 0)
            return True
        elif id == FindService.FindService.FIND_NEXT_ID:
            event.Enable(hasText and
                         self._FindServiceHasString() and
                         self.GetCtrl().GetSelection()[0] < self.GetCtrl().GetLength())
            return True
        elif id == FindService.FindService.REPLACE_ID:
            event.Enable(hasText)
            return True
        elif id == FindService.FindService.GOTO_LINE_ID:
            event.Enable(True)
            return True
        elif id == TEXT_STATUS_BAR_ID:
            self.OnUpdateStatusBar(event)
            return True
        else:
            return wx.lib.docview.View.ProcessUpdateUIEvent(self, event)


    def _GetParentFrame(self):
        return wx.GetTopLevelParent(self.GetFrame())


    #----------------------------------------------------------------------------
    # Methods for TextDocument to call
    #----------------------------------------------------------------------------

    def IsModified(self):
        if not self.GetCtrl():
            return False
        return self.GetCtrl().GetModify()


    def SetModifyFalse(self):
        self.GetCtrl().SetSavePoint()


    def GetValue(self):
        if self.GetCtrl():
            return self.GetCtrl().GetText()
        else:
            return None


    def SetValue(self, value):
        self.GetCtrl().SetText(value)
        self.GetCtrl().UpdateLineNumberMarginWidth()
        self.GetCtrl().EmptyUndoBuffer()


    #----------------------------------------------------------------------------
    # STC events
    #----------------------------------------------------------------------------

    def OnUpdateStatusBar(self, event):
        statusBar = self._GetParentFrame().GetStatusBar()
        statusBar.SetInsertMode(self.GetCtrl().GetOvertype() == 0)
        statusBar.SetLineNumber(self.GetCtrl().GetCurrentLine() + 1)
        statusBar.SetColumnNumber(self.GetCtrl().GetColumn(self.GetCtrl().GetCurrentPos()) + 1)


    #----------------------------------------------------------------------------
    # Format methods
    #----------------------------------------------------------------------------

    def OnChooseFont(self):
        data = wx.FontData()
        data.EnableEffects(True)
        data.SetInitialFont(self.GetCtrl().GetFont())
        data.SetColour(self.GetCtrl().GetFontColor())
        fontDialog = wx.FontDialog(self.GetFrame(), data)
        if fontDialog.ShowModal() == wx.ID_OK:
            data = fontDialog.GetFontData()
            self.GetCtrl().SetFont(data.GetChosenFont())
            self.GetCtrl().SetFontColor(data.GetColour())
            self.GetCtrl().UpdateStyles()
        fontDialog.Destroy()


    #----------------------------------------------------------------------------
    # Find methods
    #----------------------------------------------------------------------------

    def OnFind(self, replace = False):
        findService = wx.GetApp().GetService(FindService.FindService)
        if findService:
            findService.ShowFindReplaceDialog(findString = self.GetCtrl().GetSelectedText(), replace = replace)


    def DoFind(self, forceFindNext = False, forceFindPrevious = False, replace = False, replaceAll = False):
        findService = wx.GetApp().GetService(FindService.FindService)
        if not findService:
            return
        findString = findService.GetFindString()
        if len(findString) == 0:
            return -1
        replaceString = findService.GetReplaceString()
        flags = findService.GetFlags()
        startLoc, endLoc = self.GetCtrl().GetSelection()

        wholeWord = flags & wx.FR_WHOLEWORD > 0
        matchCase = flags & wx.FR_MATCHCASE > 0
        regExp = flags & FindService.FindService.FR_REGEXP > 0
        down = flags & wx.FR_DOWN > 0
        wrap = flags & FindService.FindService.FR_WRAP > 0

        if forceFindPrevious:   # this is from function keys, not dialog box
            down = False
            wrap = False        # user would want to know they're at the end of file
        elif forceFindNext:
            down = True
            wrap = False        # user would want to know they're at the end of file

        badSyntax = False
        
        # On replace dialog operations, user is allowed to replace the currently highlighted text to determine if it should be replaced or not.
        # Typically, it is the text from a previous find operation, but we must check to see if it isn't, user may have moved the cursor or selected some other text accidentally.
        # If the text is a match, then replace it.
        if replace:
            result, start, end, replText = findService.DoFind(findString, replaceString, self.GetCtrl().GetSelectedText(), 0, 0, True, matchCase, wholeWord, regExp, replace)
            if result > 0:
                self.GetCtrl().ReplaceSelection(replText)
                self.GetDocument().Modify(True)
                wx.GetApp().GetTopWindow().PushStatusText(_("1 occurrence of \"%s\" replaced") % findString)
                if down:
                    startLoc += len(replText)  # advance start location past replacement string to new text
                endLoc = startLoc
            elif result == FindService.FIND_SYNTAXERROR:
                badSyntax = True
                wx.GetApp().GetTopWindow().PushStatusText(_("Invalid regular expression \"%s\"") % findString)

        if not badSyntax:
            text = self.GetCtrl().GetText()
    
            # Find the next matching text occurance or if it is a ReplaceAll, replace all occurances
            # Even if the user is Replacing, we should replace here, but only select the text and let the user replace it with the next Replace operation
            result, start, end, text = findService.DoFind(findString, replaceString, text, startLoc, endLoc, down, matchCase, wholeWord, regExp, False, replaceAll, wrap)
            if result > 0:
                self.GetCtrl().SetTargetStart(0)
                self.GetCtrl().SetTargetEnd(self.GetCtrl().GetLength())
                self.GetCtrl().ReplaceTarget(text)  # Doing a SetText causes a clear document to be shown when undoing, so using replacetarget instead
                self.GetDocument().Modify(True)
                if result == 1:
                    wx.GetApp().GetTopWindow().PushStatusText(_("1 occurrence of \"%s\" replaced") % findString)
                else:
                    wx.GetApp().GetTopWindow().PushStatusText(_("%i occurrences of \"%s\" replaced") % (result, findString))
            elif result == 0:
                self.GetCtrl().SetSelection(start, end)
                self.GetCtrl().EnsureVisible(self.GetCtrl().LineFromPosition(end))  # show bottom then scroll up to top
                self.GetCtrl().EnsureVisible(self.GetCtrl().LineFromPosition(start)) # do this after ensuring bottom is visible
                wx.GetApp().GetTopWindow().PushStatusText(_("Found \"%s\".") % findString)
            elif result == FindService.FIND_SYNTAXERROR:
                # Dialog for this case gets popped up by the FindService.
                wx.GetApp().GetTopWindow().PushStatusText(_("Invalid regular expression \"%s\"") % findString)
            else:
                wx.MessageBox(_("Can't find \"%s\".") % findString, "Find",
                          wx.OK | wx.ICON_INFORMATION)


    def _FindServiceHasString(self):
        findService = wx.GetApp().GetService(FindService.FindService)
        if not findService or not findService.GetFindString():
            return False
        return True


    def OnGotoLine(self, event):
        findService = wx.GetApp().GetService(FindService.FindService)
        if findService:
            line = findService.GetLineNumber(self.GetDocumentManager().FindSuitableParent())
            if line > -1:
                line = line - 1
                self.GetCtrl().EnsureVisible(line)
                self.GetCtrl().GotoLine(line)


    def GotoLine(self, lineNum):
        if lineNum > -1:
            lineNum = lineNum - 1  # line numbering for editor is 0 based, we are 1 based.
            self.GetCtrl().EnsureVisibleEnforcePolicy(lineNum)
            self.GetCtrl().GotoLine(lineNum)


    def SetSelection(self, start, end):
        self.GetCtrl().SetSelection(start, end)


    def EnsureVisible(self, line):
        self.GetCtrl().EnsureVisible(line-1)  # line numbering for editor is 0 based, we are 1 based.

    def EnsureVisibleEnforcePolicy(self, line):
        self.GetCtrl().EnsureVisibleEnforcePolicy(line-1)  # line numbering for editor is 0 based, we are 1 based.

    def LineFromPosition(self, pos):
        return self.GetCtrl().LineFromPosition(pos)+1  # line numbering for editor is 0 based, we are 1 based.


    def PositionFromLine(self, line):
        return self.GetCtrl().PositionFromLine(line-1)  # line numbering for editor is 0 based, we are 1 based.


    def GetLineEndPosition(self, line):
        return self.GetCtrl().GetLineEndPosition(line-1)  # line numbering for editor is 0 based, we are 1 based.


    def GetLine(self, lineNum):
        return self.GetCtrl().GetLine(lineNum-1)  # line numbering for editor is 0 based, we are 1 based.

    def MarkerDefine(self):
        """ This must be called after the texteditor is instantiated """
        self.GetCtrl().MarkerDefine(TextView.MARKER_NUM, wx.stc.STC_MARK_CIRCLE, wx.BLACK, wx.BLUE)


    def MarkerToggle(self, lineNum = -1, marker_index=MARKER_NUM, mask=MARKER_MASK):
        if lineNum == -1:
            lineNum = self.GetCtrl().GetCurrentLine()
        if self.GetCtrl().MarkerGet(lineNum) & mask:
            self.GetCtrl().MarkerDelete(lineNum, marker_index)
            self._markerCount -= 1
        else:
            self.GetCtrl().MarkerAdd(lineNum, marker_index)
            self._markerCount += 1

    def MarkerAdd(self, lineNum = -1, marker_index=MARKER_NUM, mask=MARKER_MASK):
        if lineNum == -1:
            lineNum = self.GetCtrl().GetCurrentLine()
        self.GetCtrl().MarkerAdd(lineNum, marker_index)
        self._markerCount += 1


    def MarkerDelete(self, lineNum = -1, marker_index=MARKER_NUM, mask=MARKER_MASK):
        if lineNum == -1:
            lineNum = self.GetCtrl().GetCurrentLine()
        if self.GetCtrl().MarkerGet(lineNum) & mask:
            self.GetCtrl().MarkerDelete(lineNum, marker_index)
            self._markerCount -= 1

    def MarkerDeleteAll(self, marker_num=MARKER_NUM):
        self.GetCtrl().MarkerDeleteAll(marker_num)
        if marker_num == self.MARKER_NUM:
            self._markerCount = 0


    def MarkerNext(self, lineNum = -1):
        if lineNum == -1:
            lineNum = self.GetCtrl().GetCurrentLine() + 1  # start search below current line
        foundLine = self.GetCtrl().MarkerNext(lineNum, self.MARKER_MASK)
        if foundLine == -1:
            # wrap to top of file
            foundLine = self.GetCtrl().MarkerNext(0, self.MARKER_MASK)
            if foundLine == -1:
                wx.GetApp().GetTopWindow().PushStatusText(_("No markers"))
                return
        
        self.GotoLine(foundLine + 1)


    def MarkerPrevious(self, lineNum = -1):
        if lineNum == -1:
            lineNum = self.GetCtrl().GetCurrentLine() - 1  # start search above current line
            if lineNum == -1:
                lineNum = self.GetCtrl().GetLineCount()

        foundLine = self.GetCtrl().MarkerPrevious(lineNum, self.MARKER_MASK)
        if foundLine == -1:
            # wrap to bottom of file
            foundLine = self.GetCtrl().MarkerPrevious(self.GetCtrl().GetLineCount(), self.MARKER_MASK)
            if foundLine == -1:
                wx.GetApp().GetTopWindow().PushStatusText(_("No markers"))
                return

        self.GotoLine(foundLine + 1)


    def MarkerExists(self, lineNum = -1, mask=MARKER_MASK):
        if lineNum == -1:
            lineNum = self.GetCtrl().GetCurrentLine()
        if self.GetCtrl().MarkerGet(lineNum) & mask:
            return True
        else:
            return False


    def GetMarkerCount(self):
        return self._markerCount


class TextService(wx.lib.pydocview.DocService):


    def __init__(self):
        wx.lib.pydocview.DocService.__init__(self)


    def InstallControls(self, frame, menuBar = None, toolBar = None, statusBar = None, document = None):
        if document and document.GetDocumentTemplate().GetDocumentType() != TextDocument:
            return
        if not document and wx.GetApp().GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI:
            return

        statusBar = TextStatusBar(frame, TEXT_STATUS_BAR_ID)
        frame.SetStatusBar(statusBar)
        wx.EVT_UPDATE_UI(frame, TEXT_STATUS_BAR_ID, frame.ProcessUpdateUIEvent)

        viewMenu = menuBar.GetMenu(menuBar.FindMenu(_("&View")))

        viewMenu.AppendSeparator()
        textMenu = wx.Menu()
        textMenu.AppendCheckItem(VIEW_WHITESPACE_ID, _("&Whitespace"), _("Shows or hides whitespace"))
        wx.EVT_MENU(frame, VIEW_WHITESPACE_ID, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, VIEW_WHITESPACE_ID, frame.ProcessUpdateUIEvent)
        textMenu.AppendCheckItem(VIEW_EOL_ID, _("&End of Line Markers"), _("Shows or hides indicators at the end of each line"))
        wx.EVT_MENU(frame, VIEW_EOL_ID, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, VIEW_EOL_ID, frame.ProcessUpdateUIEvent)
        textMenu.AppendCheckItem(VIEW_INDENTATION_GUIDES_ID, _("&Indentation Guides"), _("Shows or hides indentations"))
        wx.EVT_MENU(frame, VIEW_INDENTATION_GUIDES_ID, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, VIEW_INDENTATION_GUIDES_ID, frame.ProcessUpdateUIEvent)
        textMenu.AppendCheckItem(VIEW_RIGHT_EDGE_ID, _("&Right Edge"), _("Shows or hides the right edge marker"))
        wx.EVT_MENU(frame, VIEW_RIGHT_EDGE_ID, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, VIEW_RIGHT_EDGE_ID, frame.ProcessUpdateUIEvent)
        textMenu.AppendCheckItem(VIEW_LINE_NUMBERS_ID, _("&Line Numbers"), _("Shows or hides the line numbers"))
        wx.EVT_MENU(frame, VIEW_LINE_NUMBERS_ID, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, VIEW_LINE_NUMBERS_ID, frame.ProcessUpdateUIEvent)
        
        viewMenu.AppendMenu(TEXT_ID, _("&Text"), textMenu)
        wx.EVT_UPDATE_UI(frame, TEXT_ID, frame.ProcessUpdateUIEvent)
        
        isWindows = (wx.Platform == '__WXMSW__')

        zoomMenu = wx.Menu()
        zoomMenu.Append(ZOOM_NORMAL_ID, _("Normal Size"), _("Sets the document to its normal size"))
        wx.EVT_MENU(frame, ZOOM_NORMAL_ID, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, ZOOM_NORMAL_ID, frame.ProcessUpdateUIEvent)
        if isWindows:
            zoomMenu.Append(ZOOM_IN_ID, _("Zoom In\tCtrl+Page Up"), _("Zooms the document to a larger size"))
        else:
            zoomMenu.Append(ZOOM_IN_ID, _("Zoom In"), _("Zooms the document to a larger size"))
        wx.EVT_MENU(frame, ZOOM_IN_ID, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, ZOOM_IN_ID, frame.ProcessUpdateUIEvent)
        if isWindows:
            zoomMenu.Append(ZOOM_OUT_ID, _("Zoom Out\tCtrl+Page Down"), _("Zooms the document to a smaller size"))
        else:
            zoomMenu.Append(ZOOM_OUT_ID, _("Zoom Out"), _("Zooms the document to a smaller size"))
        wx.EVT_MENU(frame, ZOOM_OUT_ID, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, ZOOM_OUT_ID, frame.ProcessUpdateUIEvent)
        
        viewMenu.AppendMenu(ZOOM_ID, _("&Zoom"), zoomMenu)
        wx.EVT_UPDATE_UI(frame, ZOOM_ID, frame.ProcessUpdateUIEvent)

        formatMenuIndex = menuBar.FindMenu(_("&Format"))
        if formatMenuIndex > -1:
            formatMenu = menuBar.GetMenu(formatMenuIndex)
        else:
            formatMenu = wx.Menu()
        if not menuBar.FindItemById(CHOOSE_FONT_ID):
            formatMenu.Append(CHOOSE_FONT_ID, _("&Font..."), _("Sets the font to use"))
            wx.EVT_MENU(frame, CHOOSE_FONT_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, CHOOSE_FONT_ID, frame.ProcessUpdateUIEvent)
        if not menuBar.FindItemById(WORD_WRAP_ID):
            formatMenu.AppendCheckItem(WORD_WRAP_ID, _("Word Wrap"), _("Wraps text horizontally when checked"))
            wx.EVT_MENU(frame, WORD_WRAP_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, WORD_WRAP_ID, frame.ProcessUpdateUIEvent)
        if formatMenuIndex == -1:
            viewMenuIndex = menuBar.FindMenu(_("&View"))
            menuBar.Insert(viewMenuIndex + 1, formatMenu, _("&Format"))

        # wxBug: wxToolBar::GetToolPos doesn't exist, need it to find cut tool and then insert find in front of it.
        toolBar.AddSeparator()
        toolBar.AddTool(ZOOM_IN_ID, getZoomInBitmap(), shortHelpString = _("Zoom In"), longHelpString = _("Zooms the document to a larger size"))
        toolBar.AddTool(ZOOM_OUT_ID, getZoomOutBitmap(), shortHelpString = _("Zoom Out"), longHelpString = _("Zooms the document to a smaller size"))
        toolBar.Realize()


    def ProcessUpdateUIEvent(self, event):
        id = event.GetId()
        if id == TEXT_ID:
            event.Enable(False)
            return True
        elif id == VIEW_WHITESPACE_ID:
            event.Enable(False)
            return True
        elif id == VIEW_EOL_ID:
            event.Enable(False)
            return True
        elif id == VIEW_INDENTATION_GUIDES_ID:
            event.Enable(False)
            return True
        elif id == VIEW_RIGHT_EDGE_ID:
            event.Enable(False)
            return True
        elif id == VIEW_LINE_NUMBERS_ID:
            event.Enable(False)
            return True
        elif id == ZOOM_ID:
            event.Enable(False)
            return True
        elif id == ZOOM_NORMAL_ID:
            event.Enable(False)
            return True
        elif id == ZOOM_IN_ID:
            event.Enable(False)
            return True
        elif id == ZOOM_OUT_ID:
            event.Enable(False)
            return True
        elif id == CHOOSE_FONT_ID:
            event.Enable(False)
            return True
        elif id == WORD_WRAP_ID:
            event.Enable(False)
            return True
        else:
            return False


class TextStatusBar(wx.StatusBar):

    # wxBug: Would be nice to show num key status in statusbar, but can't figure out how to detect if it is enabled or disabled

    def __init__(self, parent, id, style = wx.ST_SIZEGRIP, name = "statusBar"):
        wx.StatusBar.__init__(self, parent, id, style, name)
        self.SetFieldsCount(4)
        self.SetStatusWidths([-1, 50, 50, 55])

    def SetInsertMode(self, insert = True):
        if insert:
            newText = _("Ins")
        else:
            newText = _("")
        if self.GetStatusText(1) != newText:     # wxBug: Need to check if the text has changed, otherwise it flickers under win32
            self.SetStatusText(newText, 1)

    def SetLineNumber(self, lineNumber):
        newText = _("Ln %i") % lineNumber
        if self.GetStatusText(2) != newText:
            self.SetStatusText(newText, 2)

    def SetColumnNumber(self, colNumber):
        newText = _("Col %i") % colNumber
        if self.GetStatusText(3) != newText:
            self.SetStatusText(newText, 3)


class TextOptionsPanel(wx.Panel):


    def __init__(self, parent, id, configPrefix = "Text", label = "Text", hasWordWrap = True, hasTabs = False, addPage=True):
        wx.Panel.__init__(self, parent, id)
        self._configPrefix = configPrefix
        self._hasWordWrap = hasWordWrap
        self._hasTabs = hasTabs
        SPACE = 10
        HALF_SPACE   = 5
        config = wx.ConfigBase_Get()
        self._textFont = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL)
        fontData = config.Read(self._configPrefix + "EditorFont", "")
        if fontData:
            nativeFont = wx.NativeFontInfo()
            nativeFont.FromString(fontData)
            self._textFont.SetNativeFontInfo(nativeFont)
        self._originalTextFont = self._textFont
        self._textColor = wx.BLACK
        colorData = config.Read(self._configPrefix + "EditorColor", "")
        if colorData:
            red = int("0x" + colorData[0:2], 16)
            green = int("0x" + colorData[2:4], 16)
            blue = int("0x" + colorData[4:6], 16)
            self._textColor = wx.Color(red, green, blue)
        self._originalTextColor = self._textColor
        fontLabel = wx.StaticText(self, -1, _("Font:"))
        self._sampleTextCtrl = wx.TextCtrl(self, -1, "", size = (125, 21))
        self._sampleTextCtrl.SetEditable(False)
        chooseFontButton = wx.Button(self, -1, _("Choose Font..."))
        wx.EVT_BUTTON(self, chooseFontButton.GetId(), self.OnChooseFont)
        if self._hasWordWrap:
            self._wordWrapCheckBox = wx.CheckBox(self, -1, _("Wrap words inside text area"))
            self._wordWrapCheckBox.SetValue(wx.ConfigBase_Get().ReadInt(self._configPrefix + "EditorWordWrap", False))
        self._viewWhitespaceCheckBox = wx.CheckBox(self, -1, _("Show whitespace"))
        self._viewWhitespaceCheckBox.SetValue(config.ReadInt(self._configPrefix + "EditorViewWhitespace", False))
        self._viewEOLCheckBox = wx.CheckBox(self, -1, _("Show end of line markers"))
        self._viewEOLCheckBox.SetValue(config.ReadInt(self._configPrefix + "EditorViewEOL", False))
        self._viewIndentationGuideCheckBox = wx.CheckBox(self, -1, _("Show indentation guides"))
        self._viewIndentationGuideCheckBox.SetValue(config.ReadInt(self._configPrefix + "EditorViewIndentationGuides", False))
        self._viewRightEdgeCheckBox = wx.CheckBox(self, -1, _("Show right edge"))
        self._viewRightEdgeCheckBox.SetValue(config.ReadInt(self._configPrefix + "EditorViewRightEdge", False))
        self._viewLineNumbersCheckBox = wx.CheckBox(self, -1, _("Show line numbers"))
        self._viewLineNumbersCheckBox.SetValue(config.ReadInt(self._configPrefix + "EditorViewLineNumbers", True))
        if self._hasTabs:
            self._hasTabsCheckBox = wx.CheckBox(self, -1, _("Use spaces instead of tabs"))
            self._hasTabsCheckBox.SetValue(not wx.ConfigBase_Get().ReadInt(self._configPrefix + "EditorUseTabs", False))
            indentWidthLabel = wx.StaticText(self, -1, _("Indent Width:"))
            self._indentWidthChoice = wx.Choice(self, -1, choices = ["2", "4", "6", "8", "10"])
            self._indentWidthChoice.SetStringSelection(str(config.ReadInt(self._configPrefix + "EditorIndentWidth", 4)))
        textPanelBorderSizer = wx.BoxSizer(wx.VERTICAL)
        textPanelSizer = wx.BoxSizer(wx.VERTICAL)
        textFontSizer = wx.BoxSizer(wx.HORIZONTAL)
        textFontSizer.Add(fontLabel, 0, wx.ALIGN_LEFT | wx.RIGHT | wx.TOP, HALF_SPACE)
        textFontSizer.Add(self._sampleTextCtrl, 0, wx.ALIGN_LEFT | wx.EXPAND | wx.RIGHT, HALF_SPACE)
        textFontSizer.Add(chooseFontButton, 0, wx.ALIGN_RIGHT | wx.LEFT, HALF_SPACE)
        textPanelSizer.Add(textFontSizer, 0, wx.ALL, HALF_SPACE)
        if self._hasWordWrap:
            textPanelSizer.Add(self._wordWrapCheckBox, 0, wx.ALL, HALF_SPACE)
        textPanelSizer.Add(self._viewWhitespaceCheckBox, 0, wx.ALL, HALF_SPACE)
        textPanelSizer.Add(self._viewEOLCheckBox, 0, wx.ALL, HALF_SPACE)
        textPanelSizer.Add(self._viewIndentationGuideCheckBox, 0, wx.ALL, HALF_SPACE)
        textPanelSizer.Add(self._viewRightEdgeCheckBox, 0, wx.ALL, HALF_SPACE)
        textPanelSizer.Add(self._viewLineNumbersCheckBox, 0, wx.ALL, HALF_SPACE)
        if self._hasTabs:
            textPanelSizer.Add(self._hasTabsCheckBox, 0, wx.ALL, HALF_SPACE)
            textIndentWidthSizer = wx.BoxSizer(wx.HORIZONTAL)
            textIndentWidthSizer.Add(indentWidthLabel, 0, wx.ALIGN_LEFT | wx.RIGHT | wx.TOP, HALF_SPACE)
            textIndentWidthSizer.Add(self._indentWidthChoice, 0, wx.ALIGN_LEFT | wx.EXPAND, HALF_SPACE)
            textPanelSizer.Add(textIndentWidthSizer, 0, wx.ALL, HALF_SPACE)
        textPanelBorderSizer.Add(textPanelSizer, 0, wx.ALL, SPACE)
##        styleButton = wx.Button(self, -1, _("Choose Style..."))
##        wx.EVT_BUTTON(self, styleButton.GetId(), self.OnChooseStyle)
##        textPanelBorderSizer.Add(styleButton, 0, wx.ALL, SPACE)
        self.SetSizer(textPanelBorderSizer)
        self.UpdateSampleFont()
        if addPage:
            parent.AddPage(self, _(label))

    def UpdateSampleFont(self):
        nativeFont = wx.NativeFontInfo()
        nativeFont.FromString(self._textFont.GetNativeFontInfoDesc())
        font = wx.NullFont
        font.SetNativeFontInfo(nativeFont)
        font.SetPointSize(self._sampleTextCtrl.GetFont().GetPointSize())  # Use the standard point size
        self._sampleTextCtrl.SetFont(font)
        self._sampleTextCtrl.SetForegroundColour(self._textColor)
        self._sampleTextCtrl.SetValue(str(self._textFont.GetPointSize()) + _(" pt. ") + self._textFont.GetFaceName())
        self._sampleTextCtrl.Refresh()
        self.Layout()


##    def OnChooseStyle(self, event):
##        import STCStyleEditor
##        import os
##        base = os.path.split(__file__)[0]
##        config = os.path.abspath(os.path.join(base, 'stc-styles.rc.cfg'))
##        
##        dlg = STCStyleEditor.STCStyleEditDlg(None,
##                                'Python', 'python',
##                                #'HTML', 'html',
##                                #'XML', 'xml',
##                                config)
##        try:
##            dlg.ShowModal()
##        finally:
##            dlg.Destroy()


    def OnChooseFont(self, event):
        data = wx.FontData()
        data.EnableEffects(True)
        data.SetInitialFont(self._textFont)
        data.SetColour(self._textColor)
        fontDialog = wx.FontDialog(self, data)
        if fontDialog.ShowModal() == wx.ID_OK:
            data = fontDialog.GetFontData()
            self._textFont = data.GetChosenFont()
            self._textColor = data.GetColour()
            self.UpdateSampleFont()
        fontDialog.Destroy()


    def OnOK(self, optionsDialog):
        config = wx.ConfigBase_Get()
        doViewStuffUpdate = config.ReadInt(self._configPrefix + "EditorViewWhitespace", False) != self._viewWhitespaceCheckBox.GetValue()
        config.WriteInt(self._configPrefix + "EditorViewWhitespace", self._viewWhitespaceCheckBox.GetValue())
        doViewStuffUpdate = doViewStuffUpdate or config.ReadInt(self._configPrefix + "EditorViewEOL", False) != self._viewEOLCheckBox.GetValue()
        config.WriteInt(self._configPrefix + "EditorViewEOL", self._viewEOLCheckBox.GetValue())
        doViewStuffUpdate = doViewStuffUpdate or config.ReadInt(self._configPrefix + "EditorViewIndentationGuides", False) != self._viewIndentationGuideCheckBox.GetValue()
        config.WriteInt(self._configPrefix + "EditorViewIndentationGuides", self._viewIndentationGuideCheckBox.GetValue())
        doViewStuffUpdate = doViewStuffUpdate or config.ReadInt(self._configPrefix + "EditorViewRightEdge", False) != self._viewRightEdgeCheckBox.GetValue()
        config.WriteInt(self._configPrefix + "EditorViewRightEdge", self._viewRightEdgeCheckBox.GetValue())
        doViewStuffUpdate = doViewStuffUpdate or config.ReadInt(self._configPrefix + "EditorViewLineNumbers", True) != self._viewLineNumbersCheckBox.GetValue()
        config.WriteInt(self._configPrefix + "EditorViewLineNumbers", self._viewLineNumbersCheckBox.GetValue())
        if self._hasWordWrap:
            doViewStuffUpdate = doViewStuffUpdate or config.ReadInt(self._configPrefix + "EditorWordWrap", False) != self._wordWrapCheckBox.GetValue()
            config.WriteInt(self._configPrefix + "EditorWordWrap", self._wordWrapCheckBox.GetValue())
        if self._hasTabs:
            doViewStuffUpdate = doViewStuffUpdate or not config.ReadInt(self._configPrefix + "EditorUseTabs", True) != self._hasTabsCheckBox.GetValue()
            config.WriteInt(self._configPrefix + "EditorUseTabs", not self._hasTabsCheckBox.GetValue())
            newIndentWidth = int(self._indentWidthChoice.GetStringSelection())
            oldIndentWidth = config.ReadInt(self._configPrefix + "EditorIndentWidth", 4)
            if newIndentWidth != oldIndentWidth:
                doViewStuffUpdate = True
                config.WriteInt(self._configPrefix + "EditorIndentWidth", newIndentWidth)
        doFontUpdate = self._originalTextFont != self._textFont or self._originalTextColor != self._textColor
        config.Write(self._configPrefix + "EditorFont", self._textFont.GetNativeFontInfoDesc())
        config.Write(self._configPrefix + "EditorColor", "%02x%02x%02x" % (self._textColor.Red(), self._textColor.Green(), self._textColor.Blue()))
        if doViewStuffUpdate or doFontUpdate:
            for document in optionsDialog.GetDocManager().GetDocuments():
                if issubclass(document.GetDocumentTemplate().GetDocumentType(), TextDocument):
                    if doViewStuffUpdate:
                        document.UpdateAllViews(hint = "ViewStuff")
                    if doFontUpdate:
                        document.UpdateAllViews(hint = "Font")


class TextCtrl(wx.stc.StyledTextCtrl):

    def __init__(self, parent, ID = -1, style = wx.NO_FULL_REPAINT_ON_RESIZE):
        if ID == -1:
            ID = wx.NewId()
        wx.stc.StyledTextCtrl.__init__(self, parent, ID, style = style)

        self._font = None
        self._fontColor = None
        
        self.SetVisiblePolicy(wx.stc.STC_VISIBLE_STRICT,0)
        self.SetYCaretPolicy(0, 0)
        
        self.CmdKeyClear(wx.stc.STC_KEY_ADD, wx.stc.STC_SCMOD_CTRL)
        self.CmdKeyClear(wx.stc.STC_KEY_SUBTRACT, wx.stc.STC_SCMOD_CTRL)
        self.CmdKeyAssign(wx.stc.STC_KEY_PRIOR, wx.stc.STC_SCMOD_CTRL, wx.stc.STC_CMD_ZOOMIN)
        self.CmdKeyAssign(wx.stc.STC_KEY_NEXT, wx.stc.STC_SCMOD_CTRL, wx.stc.STC_CMD_ZOOMOUT)
        self.Bind(wx.stc.EVT_STC_ZOOM, self.OnUpdateLineNumberMarginWidth)  # auto update line num width on zoom
        wx.EVT_KEY_DOWN(self, self.OnKeyPressed)
        self.SetMargins(0,0)

        self.SetUseTabs(0)
        self.SetTabWidth(4)
        self.SetIndent(4)

        self.SetViewWhiteSpace(False)
        self.SetEOLMode(wx.stc.STC_EOL_LF)
        self.SetEdgeMode(wx.stc.STC_EDGE_NONE)
        self.SetEdgeColumn(78)

        self.SetMarginType(1, wx.stc.STC_MARGIN_NUMBER)
        self.SetMarginWidth(1, self.EstimatedLineNumberMarginWidth())
        self.UpdateStyles()

        self.SetCaretForeground("BLACK")
        
        self.SetViewDefaults()
        font, color = self.GetFontAndColorFromConfig()
        self.SetFont(font)
        self.SetFontColor(color)
        self.MarkerDefineDefault()

        # for multisash initialization
        if isinstance(parent, wx.lib.multisash.MultiClient):
            while parent.GetParent():
                parent = parent.GetParent()
                if hasattr(parent, "GetView"):
                    break
            if hasattr(parent, "GetView"):
                textEditor = parent.GetView()._textEditor
                if textEditor:
                    doc = textEditor.GetDocPointer()
                    if doc:
                        self.SetDocPointer(doc)


        
    def SetViewDefaults(self, configPrefix = "Text", hasWordWrap = True, hasTabs = False):
        config = wx.ConfigBase_Get()
        self.SetViewWhiteSpace(config.ReadInt(configPrefix + "EditorViewWhitespace", False))
        self.SetViewEOL(config.ReadInt(configPrefix + "EditorViewEOL", False))
        self.SetIndentationGuides(config.ReadInt(configPrefix + "EditorViewIndentationGuides", False))
        self.SetViewRightEdge(config.ReadInt(configPrefix + "EditorViewRightEdge", False))
        self.SetViewLineNumbers(config.ReadInt(configPrefix + "EditorViewLineNumbers", True))
        if hasWordWrap:
            self.SetWordWrap(config.ReadInt(configPrefix + "EditorWordWrap", False))
        if hasTabs:  # These methods do not exist in STCTextEditor and are meant for subclasses
            self.SetUseTabs(config.ReadInt(configPrefix + "EditorUseTabs", False))
            self.SetIndent(config.ReadInt(configPrefix + "EditorIndentWidth", 4))
            self.SetTabWidth(config.ReadInt(configPrefix + "EditorIndentWidth", 4))
        else:
            self.SetUseTabs(True)
            self.SetIndent(4)
            self.SetTabWidth(4)


        
    def GetDefaultFont(self):
        """ Subclasses should override this """
        return wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL)
        

    def GetDefaultColor(self):
        """ Subclasses should override this """
        return wx.BLACK


    def GetFontAndColorFromConfig(self, configPrefix = "Text"):
        font = self.GetDefaultFont()
        config = wx.ConfigBase_Get()
        fontData = config.Read(configPrefix + "EditorFont", "")
        if fontData:
            nativeFont = wx.NativeFontInfo()
            nativeFont.FromString(fontData)
            font.SetNativeFontInfo(nativeFont)
        color = self.GetDefaultColor()
        colorData = config.Read(configPrefix + "EditorColor", "")
        if colorData:
            red = int("0x" + colorData[0:2], 16)
            green = int("0x" + colorData[2:4], 16)
            blue = int("0x" + colorData[4:6], 16)
            color = wx.Color(red, green, blue)
        return font, color


    def GetFont(self):
        return self._font
        
    def SetFont(self, font):
        self._font = font
        self.StyleSetFont(wx.stc.STC_STYLE_DEFAULT, self._font)


    def GetFontColor(self):
        return self._fontColor


    def SetFontColor(self, fontColor = wx.BLACK):
        self._fontColor = fontColor
        self.StyleSetForeground(wx.stc.STC_STYLE_DEFAULT, "#%02x%02x%02x" % (self._fontColor.Red(), self._fontColor.Green(), self._fontColor.Blue()))


    def UpdateStyles(self):
        self.StyleClearAll()
        return


    def EstimatedLineNumberMarginWidth(self):
        MARGIN = 4
        baseNumbers = "000"
        lineNum = self.GetLineCount()
        lineNum = lineNum/100
        while lineNum >= 10:
            lineNum = lineNum/10
            baseNumbers = baseNumbers + "0"

        return self.TextWidth(wx.stc.STC_STYLE_LINENUMBER, baseNumbers) + MARGIN


    def OnUpdateLineNumberMarginWidth(self, event):
        self.UpdateLineNumberMarginWidth()

            
    def UpdateLineNumberMarginWidth(self):
        if self.GetViewLineNumbers():
            self.SetMarginWidth(1, self.EstimatedLineNumberMarginWidth())
        
    def MarkerDefineDefault(self):
        """ This must be called after the textcontrol is instantiated """
        self.MarkerDefine(TextView.MARKER_NUM, wx.stc.STC_MARK_ROUNDRECT, wx.BLACK, wx.BLUE)


    def OnClear(self):
        # Used when Delete key is hit.
        sel = self.GetSelection()
                
        # Delete the selection or if no selection, the character after the caret.
        if sel[0] == sel[1]:
            self.SetSelection(sel[0], sel[0] + 1)
        else:
            # remove any folded lines also.
            startLine = self.LineFromPosition(sel[0])
            endLine = self.LineFromPosition(sel[1])
            endLineStart = self.PositionFromLine(endLine)
            if startLine != endLine and sel[1] - endLineStart == 0:
                while not self.GetLineVisible(endLine):
                    endLine += 1
                self.SetSelectionEnd(self.PositionFromLine(endLine))
            
        self.Clear()


    def OnPaste(self):
        # replace any folded lines also.
        sel = self.GetSelection()
        startLine = self.LineFromPosition(sel[0])
        endLine = self.LineFromPosition(sel[1])
        endLineStart = self.PositionFromLine(endLine)
        if startLine != endLine and sel[1] - endLineStart == 0:
            while not self.GetLineVisible(endLine):
                endLine += 1
            self.SetSelectionEnd(self.PositionFromLine(endLine))
                
        self.Paste()
        

    def OnKeyPressed(self, event):
        key = event.GetKeyCode()
        if key == wx.WXK_NUMPAD_ADD:  #wxBug: For whatever reason, the key accelerators for numpad add and subtract with modifiers are not working so have to trap them here
            if event.ControlDown():
                self.ToggleFoldAll(expand = True, topLevelOnly = True)
            elif event.ShiftDown():
                self.ToggleFoldAll(expand = True)
            else:
                self.ToggleFold(self.GetCurrentLine())
        elif key == wx.WXK_NUMPAD_SUBTRACT:
            if event.ControlDown():
                self.ToggleFoldAll(expand = False, topLevelOnly = True)
            elif event.ShiftDown():
                self.ToggleFoldAll(expand = False)
            else:
                self.ToggleFold(self.GetCurrentLine())
        else:
            event.Skip()


    #----------------------------------------------------------------------------
    # View Text methods
    #----------------------------------------------------------------------------

    def GetViewRightEdge(self):
        return self.GetEdgeMode() != wx.stc.STC_EDGE_NONE


    def SetViewRightEdge(self, viewRightEdge):
        if viewRightEdge:
            self.SetEdgeMode(wx.stc.STC_EDGE_LINE)
        else:
            self.SetEdgeMode(wx.stc.STC_EDGE_NONE)


    def GetViewLineNumbers(self):
        return self.GetMarginWidth(1) > 0


    def SetViewLineNumbers(self, viewLineNumbers = True):
        if viewLineNumbers:
            self.SetMarginWidth(1, self.EstimatedLineNumberMarginWidth())
        else:
            self.SetMarginWidth(1, 0)


    def CanWordWrap(self):
        return True


    def GetWordWrap(self):
        return self.GetWrapMode() == wx.stc.STC_WRAP_WORD


    def SetWordWrap(self, wordWrap):
        if wordWrap:
            self.SetWrapMode(wx.stc.STC_WRAP_WORD)
        else:
            self.SetWrapMode(wx.stc.STC_WRAP_NONE)

        
#----------------------------------------------------------------------------
# Icon Bitmaps - generated by encode_bitmaps.py
#----------------------------------------------------------------------------
from wx import ImageFromStream, BitmapFromImage
from wx import EmptyIcon
import cStringIO


def getTextData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00`IDAT8\x8d\xed\x931\x0e\xc00\x08\x03m\x92\xff\xff8q\xa7JU!$\x12\x1d\
\xeb\t\t8n\x81\xb4\x86J\xfa]h\x0ee\x83\xb4\xc6\x14\x00\x00R\xcc \t\xcd\xa1\
\x08\xd2\xa3\xe1\x08*\t$\x1d\xc4\x012\x0b\x00\xce\xe4\xc8\xe0\t}\xf7\x8f\rV\
\xd9\x1a\xec\xe0\xbf\xc1\xd7\x06\xd9\xf5UX\xfdF+m\x03\xb8\x00\xe4\xc74B"x\
\xf1\xf4\x00\x00\x00\x00IEND\xaeB`\x82' 


def getTextBitmap():
    return BitmapFromImage(getTextImage())

def getTextImage():
    stream = cStringIO.StringIO(getTextData())
    return ImageFromStream(stream)

def getTextIcon():
    icon = EmptyIcon()
    icon.CopyFromBitmap(getTextBitmap())
    return icon


#----------------------------------------------------------------------------
# Menu Bitmaps - generated by encode_bitmaps.py
#----------------------------------------------------------------------------
#----------------------------------------------------------------------
def getZoomInData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00wIDAT8\x8d\xa5\x93Q\x12\x80 \x08D\xb5\xe9X\xee\xe9\xb7{\xd5Gc\xa9\
\xacX\xca\x1f\xa0\x8fE0\x92<\xc3\x82\xed*\x08\xa0\xf2I~\x07\x000\x17T,\xdb\
\xd6;\x08\xa4\x00\xa4GA\xab\xca\x00\xbc*\x1eD\xb4\x90\xa4O\x1e\xe3\x16f\xcc(\
\xc8\x95F\x95\x8d\x02\xef\xa1n\xa0\xce\xc5v\x91zc\xacU\xbey\x03\xf0.\xa8\xb8\
\x04\x8c\xac\x04MM\xa1lA\xfe\x85?\x90\xe5=X\x06\\\xebCA\xb3Q\xf34\x14\x00\
\x00\x00\x00IEND\xaeB`\x82' 

def getZoomInBitmap():
    return BitmapFromImage(getZoomInImage())

def getZoomInImage():
    stream = cStringIO.StringIO(getZoomInData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getZoomOutData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00qIDAT8\x8d\xa5\x92Q\x0e\xc0 \x08C-z\xff\x13O\xd9\xd7\x16"\x05\x8d\
\xf6O\xa2\x8f"\x05\xa4\x96\x1b5V\xd4\xd1\xd5\x9e!\x15\xdb\x00\x1d]\xe7\x07\
\xac\xf6Iv.B*fW\x0e\x90u\xc9 d\x84\x87v\x82\xb4\xf5\x08\'r\x0e\xa2N\x91~\x07\
\xd9G\x95\xe2W\xeb\x00\x19\xc4\xd6\\FX\x12\xa3 \xb1:\x05\xacdAG[\xb0y9r`u\
\x9d\x83k\xc0\x0b#3@0A\x0c"\x93\x00\x00\x00\x00IEND\xaeB`\x82' 
 

def getZoomOutBitmap():
    return BitmapFromImage(getZoomOutImage())

def getZoomOutImage():
    stream = cStringIO.StringIO(getZoomOutData())
    return ImageFromStream(stream)



