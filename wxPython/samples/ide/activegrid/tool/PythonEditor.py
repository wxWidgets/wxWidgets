#----------------------------------------------------------------------------
# Name:         PythonEditor.py
# Purpose:      PythonEditor for wx.lib.pydocview tbat uses the Styled Text Control
#
# Author:       Peter Yared
#
# Created:      8/15/03
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import CodeEditor
import wx
import wx.lib.docview
import wx.lib.pydocview
import string
import keyword  # So it knows what to hilite
import wx.py  # For the Python interpreter
import wx.stc # For the Python interpreter
import cStringIO  # For indent
import OutlineService
import STCTextEditor
import keyword # for GetAutoCompleteKeywordList
import sys # for GetAutoCompleteKeywordList
import MessageService # for OnCheckCode
import OutlineService
import FindInDirService
from UICommon import CaseInsensitiveCompare
try:
    import checker # for pychecker
    _CHECKER_INSTALLED = True
except ImportError:
    _CHECKER_INSTALLED = False
import os.path # for pychecker
_ = wx.GetTranslation

if wx.Platform == '__WXMSW__':
    _WINDOWS = True
else:
    _WINDOWS = False


VIEW_PYTHON_INTERPRETER_ID = wx.NewId()


class PythonDocument(CodeEditor.CodeDocument):
    pass


class PythonView(CodeEditor.CodeView):


    def GetCtrlClass(self):
        """ Used in split window to instantiate new instances """
        return PythonCtrl


    def ProcessUpdateUIEvent(self, event):
        if not self.GetCtrl():
            return False
            
        id = event.GetId()
        if id == CodeEditor.CHECK_CODE_ID:
            hasText = self.GetCtrl().GetTextLength() > 0
            event.Enable(hasText)
            return True
            
        return CodeEditor.CodeView.ProcessUpdateUIEvent(self, event)


    def OnActivateView(self, activate, activeView, deactiveView):
        STCTextEditor.TextView.OnActivateView(self, activate, activeView, deactiveView)
        if activate and self.GetCtrl():
            if self.GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI:
                self.LoadOutline()
            else:
                wx.CallAfter(self.LoadOutline)  # need CallAfter because document isn't loaded yet
        

    def OnClose(self, deleteWindow = True):
        status = STCTextEditor.TextView.OnClose(self, deleteWindow)
        wx.CallAfter(self.ClearOutline)  # need CallAfter because when closing the document, it is Activated and then Close, so need to match OnActivateView's CallAfter
        return status
       

    def GetAutoCompleteKeywordList(self, context, hint):
        obj = None
        try:
            if context and len(context):
                obj = eval(context, globals(), locals())
        except:
            if not hint or len(hint) == 0:  # context isn't valid, maybe it was the hint
                hint = context
            
        if obj is None:
            kw = keyword.kwlist[:]
        else:
            symTbl = dir(obj)
            kw = filter(lambda item: item[0] != '_', symTbl)  # remove local variables and methods
        
        if hint and len(hint):
            lowerHint = hint.lower()
            filterkw = filter(lambda item: item.lower().startswith(lowerHint), kw)  # remove variables and methods that don't match hint
            kw = filterkw

        kw.sort(CaseInsensitiveCompare)

        if hint:
            replaceLen = len(hint)
        else:
            replaceLen = 0
            
        return " ".join(kw), replaceLen


    def OnCheckCode(self):
        if not _CHECKER_INSTALLED:       
            wx.MessageBox(_("pychecker not found.  Please install pychecker."), _("Check Code"))
            return

        filename = os.path.basename(self.GetDocument().GetFilename())

        # pychecker only works on files, doesn't take a stream or string input
        if self.GetDocument().IsModified():
            dlg = wx.MessageDialog(self.GetFrame(), _("'%s' has been modfied and must be saved first.  Save file and check code?") % filename, _("Check Code"))
            dlg.CenterOnParent()
            val = dlg.ShowModal()
            dlg.Destroy()
            if val == wx.ID_OK:
                self.GetDocument().Save()
            else:
                return
            
        messageService = wx.GetApp().GetService(MessageService.MessageService)
        messageService.ShowWindow()
        view = messageService.GetView()
        if not view:
            return
            
        view.ClearLines()
        view.SetCallback(self.OnJumpToFoundLine)
        
        # Set cursor to Wait cursor
        wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_WAIT))

        try:
            # This takes a while for involved code
            checker.checkSyntax(self.GetDocument().GetFilename(), view)

        finally:
            # Set cursor to Default cursor
            wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_DEFAULT))


    def OnJumpToFoundLine(self, event):
        messageService = wx.GetApp().GetService(MessageService.MessageService)
        lineText, pos = messageService.GetView().GetCurrLine()
        
        lineEnd = lineText.find(".py:")
        if lineEnd == -1:
            return

        lineStart = lineEnd + len(".py:")
        lineEnd = lineText.find(":", lineStart)
        lineNum = int(lineText[lineStart:lineEnd])

        filename = lineText[0:lineStart - 1]

        foundView = None
        openDocs = wx.GetApp().GetDocumentManager().GetDocuments()
        for openDoc in openDocs:
            if openDoc.GetFilename() == filename:
                foundView = openDoc.GetFirstView()
                break

        if not foundView:
            doc = wx.GetApp().GetDocumentManager().CreateDocument(filename, wx.lib.docview.DOC_SILENT|wx.lib.docview.DOC_OPEN_ONCE)
            foundView = doc.GetFirstView()

        if foundView:
            foundView.GetFrame().SetFocus()
            foundView.Activate()
            foundView.GotoLine(lineNum)
            startPos = foundView.PositionFromLine(lineNum)
            endPos = foundView.GetLineEndPosition(lineNum)
            # wxBug:  Need to select in reverse order, (end, start) to put cursor at head of line so positioning is correct
            #         Also, if we use the correct positioning order (start, end), somehow, when we open a edit window for the first
            #         time, we don't see the selection, it is scrolled off screen
            foundView.SetSelection(endPos, startPos)
            wx.GetApp().GetService(OutlineService.OutlineService).LoadOutline(foundView, position=startPos)

        

class PythonInterpreterView(wx.lib.docview.View):


    def OnCreate(self, doc, flags):
        frame = wx.GetApp().CreateDocumentFrame(self, doc, flags)
        sizer = wx.BoxSizer()        
        self._pyCrust = wx.py.crust.Crust(frame)
        sizer.Add(self._pyCrust, 1, wx.EXPAND, 0)
        frame.SetSizer(sizer)
        frame.Layout()
        self.Activate()
        frame.Show()
        return True


    def ProcessEvent(self, event):
        if not hasattr(self, "_pyCrust") or not self._pyCrust:
            return wx.lib.docview.View.ProcessEvent(self, event)
        stcControl = wx.Window_FindFocus()
        if not isinstance(stcControl, wx.stc.StyledTextCtrl):
            return wx.lib.docview.View.ProcessEvent(self, event)
        id = event.GetId()
        if id == wx.ID_UNDO:
            stcControl.Undo()
            return True
        elif id == wx.ID_REDO:
            stcControl.Redo()
            return True
        elif id == wx.ID_CUT:
            stcControl.Cut()
            return True
        elif id == wx.ID_COPY:
            stcControl.Copy()
            return True
        elif id == wx.ID_PASTE:
            stcControl.Paste()
            return True
        elif id == wx.ID_CLEAR:
            stcControl.Clear()
            return True
        elif id == wx.ID_SELECTALL:
            stcControl.SetSelection(0, -1)
            return True
        else:
            return wx.lib.docview.View.ProcessEvent(self, event)


    def ProcessUpdateUIEvent(self, event):
        if not hasattr(self, "_pyCrust") or not self._pyCrust:
            return wx.lib.docview.View.ProcessUpdateUIEvent(self, event)
        stcControl = wx.Window_FindFocus()
        if not isinstance(stcControl, wx.stc.StyledTextCtrl):
            return wx.lib.docview.View.ProcessUpdateUIEvent(self, event)
        id = event.GetId()
        if id == wx.ID_UNDO:
            event.Enable(stcControl.CanUndo())
            return True
        elif id == wx.ID_REDO:
            event.Enable(stcControl.CanRedo())
            return True
        elif id == wx.ID_CUT:
            event.Enable(stcControl.CanCut())
            return True
        elif id == wx.ID_COPY:
            event.Enable(stcControl.CanCopy())
            return True
        elif id == wx.ID_PASTE:
            event.Enable(stcControl.CanPaste())
            return True
        elif id == wx.ID_CLEAR:
            event.Enable(True)  # wxBug: should be stcControl.CanCut()) but disabling clear item means del key doesn't work in control as expected
            return True
        elif id == wx.ID_SELECTALL:
            event.Enable(stcControl.GetTextLength() > 0)
            return True
        else:
            return wx.lib.docview.View.ProcessUpdateUIEvent(self, event)


    def OnClose(self, deleteWindow=True):
        if deleteWindow and self.GetFrame():
            self.GetFrame().Destroy()
        return True


class PythonInterpreterDocument(wx.lib.docview.Document):
    """ Generate Unique Doc Type """
    pass
    

class PythonService(CodeEditor.CodeService):


    def __init__(self):
        CodeEditor.CodeService.__init__(self)
        docManager = wx.GetApp().GetDocumentManager()
        pythonInterpreterTemplate = wx.lib.docview.DocTemplate(docManager,
                                          _("Python Interpreter"),
                                          "*.Foobar",
                                          "Foobar",
                                          ".Foobar",
                                          _("Python Interpreter Document"),
                                          _("Python Interpreter View"),
                                          PythonInterpreterDocument,
                                          PythonInterpreterView,
                                          flags = wx.lib.docview.TEMPLATE_INVISIBLE,
                                          icon = getPythonIcon())
        docManager.AssociateTemplate(pythonInterpreterTemplate)


    def InstallControls(self, frame, menuBar = None, toolBar = None, statusBar = None, document = None):
        CodeEditor.CodeService.InstallControls(self, frame, menuBar, toolBar, statusBar, document)

        if document and document.GetDocumentTemplate().GetDocumentType() != PythonDocument:
            return
        if not document and wx.GetApp().GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI:
            return

        viewMenu = menuBar.GetMenu(menuBar.FindMenu(_("&View")))

        viewStatusBarItemPos = self.GetMenuItemPos(viewMenu, wx.lib.pydocview.VIEW_STATUSBAR_ID)
        viewMenu.InsertCheckItem(viewStatusBarItemPos + 1, VIEW_PYTHON_INTERPRETER_ID, _("Python &Interpreter"), _("Shows or hides the Python interactive window"))
        wx.EVT_MENU(frame, VIEW_PYTHON_INTERPRETER_ID, frame.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, VIEW_PYTHON_INTERPRETER_ID, frame.ProcessUpdateUIEvent)


    def ProcessEvent(self, event):
        id = event.GetId()
        if id == VIEW_PYTHON_INTERPRETER_ID:
            self.OnViewPythonInterpreter(event)
            return True
        else:
            return CodeEditor.CodeService.ProcessEvent(self, event)


    def ProcessUpdateUIEvent(self, event):
        id = event.GetId()
        if id == VIEW_PYTHON_INTERPRETER_ID:
            event.Enable(True)
            docManager = wx.GetApp().GetDocumentManager()
            event.Check(False)
            for doc in docManager.GetDocuments():
                if isinstance(doc, PythonInterpreterDocument):
                    event.Check(True)
                    break
            return True
        else:
            return CodeEditor.CodeService.ProcessUpdateUIEvent(self, event)


    def OnViewPythonInterpreter(self, event):
        for doc in wx.GetApp().GetDocumentManager().GetDocuments():
            if isinstance(doc, PythonInterpreterDocument):
                doc.DeleteAllViews()
                return
                
        for template in self.GetDocumentManager().GetTemplates():
            if template.GetDocumentType() == PythonInterpreterDocument:
                newDoc = template.CreateDocument('', wx.lib.docview.DOC_SILENT|wx.lib.docview.DOC_OPEN_ONCE)
                if newDoc:
                    newDoc.SetDocumentName(template.GetDocumentName())
                    newDoc.SetDocumentTemplate(template)
                    newDoc.OnNewDocument()
                    newDoc.SetWriteable(False)
                    newDoc.GetFirstView().GetFrame().SetTitle(_("Python Interpreter"))
                break


class PythonCtrl(CodeEditor.CodeCtrl):


    def __init__(self, parent, id=-1, style=wx.NO_FULL_REPAINT_ON_RESIZE):
        CodeEditor.CodeCtrl.__init__(self, parent, id, style)
        self.SetProperty("tab.timmy.whinge.level", "1")
        self.SetProperty("fold.comment.python", "1")
        self.SetProperty("fold.quotes.python", "1")
        self.SetLexer(wx.stc.STC_LEX_PYTHON)
        self.SetKeyWords(0, string.join(keyword.kwlist))


    def CreatePopupMenu(self):
        FINDCLASS_ID = wx.NewId()
        FINDDEF_ID = wx.NewId()

        menu = CodeEditor.CodeCtrl.CreatePopupMenu(self)

        self.Bind(wx.EVT_MENU, self.OnPopFindDefinition, id=FINDDEF_ID)
        menu.Insert(1, FINDDEF_ID, _("Find 'def'"))

        self.Bind(wx.EVT_MENU, self.OnPopFindClass, id=FINDCLASS_ID)
        menu.Insert(2, FINDCLASS_ID, _("Find 'class'"))

        return menu


    def OnPopFindDefinition(self, event):
        view = wx.GetApp().GetDocumentManager().GetCurrentView()
        if hasattr(view, "GetCtrl") and view.GetCtrl() and hasattr(view.GetCtrl(), "GetSelectedText"):
            pattern = view.GetCtrl().GetSelectedText().strip()
            if pattern:
                searchPattern = "def\s+%s" % pattern
                wx.GetApp().GetService(FindInDirService.FindInDirService).FindInProject(searchPattern)


    def OnPopFindClass(self, event):
        view = wx.GetApp().GetDocumentManager().GetCurrentView()
        if hasattr(view, "GetCtrl") and view.GetCtrl() and hasattr(view.GetCtrl(), "GetSelectedText"):
            definition = "class\s+%s"
            pattern = view.GetCtrl().GetSelectedText().strip()
            if pattern:
                searchPattern = definition % pattern
                wx.GetApp().GetService(FindInDirService.FindInDirService).FindInProject(searchPattern)


    def SetViewDefaults(self):
        CodeEditor.CodeCtrl.SetViewDefaults(self, configPrefix="Python", hasWordWrap=True, hasTabs=True, hasFolding=True)


    def GetFontAndColorFromConfig(self):
        return CodeEditor.CodeCtrl.GetFontAndColorFromConfig(self, configPrefix = "Python")


    def UpdateStyles(self):
        CodeEditor.CodeCtrl.UpdateStyles(self)

        if not self.GetFont():
            return

        faces = { 'font' : self.GetFont().GetFaceName(),
                  'size' : self.GetFont().GetPointSize(),
                  'size2': self.GetFont().GetPointSize() - 2,
                  'color' : "%02x%02x%02x" % (self.GetFontColor().Red(), self.GetFontColor().Green(), self.GetFontColor().Blue())
                  }

        # Python styles
        # White space
        self.StyleSetSpec(wx.stc.STC_P_DEFAULT, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        # Comment
        self.StyleSetSpec(wx.stc.STC_P_COMMENTLINE, "face:%(font)s,fore:#007F00,italic,face:%(font)s,size:%(size)d" % faces)
        # Number
        self.StyleSetSpec(wx.stc.STC_P_NUMBER, "face:%(font)s,fore:#007F7F,size:%(size)d" % faces)
        # String
        self.StyleSetSpec(wx.stc.STC_P_STRING, "face:%(font)s,fore:#7F007F,face:%(font)s,size:%(size)d" % faces)
        # Single quoted string
        self.StyleSetSpec(wx.stc.STC_P_CHARACTER, "face:%(font)s,fore:#7F007F,face:%(font)s,size:%(size)d" % faces)
        # Keyword
        self.StyleSetSpec(wx.stc.STC_P_WORD, "face:%(font)s,fore:#00007F,bold,size:%(size)d" % faces)
        # Triple quotes
        self.StyleSetSpec(wx.stc.STC_P_TRIPLE, "face:%(font)s,fore:#7F0000,size:%(size)d" % faces)
        # Triple double quotes
        self.StyleSetSpec(wx.stc.STC_P_TRIPLEDOUBLE, "face:%(font)s,fore:#7F0000,size:%(size)d" % faces)
        # Class name definition
        self.StyleSetSpec(wx.stc.STC_P_CLASSNAME, "face:%(font)s,fore:#0000FF,bold,size:%(size)d" % faces)
        # Function or method name definition
        self.StyleSetSpec(wx.stc.STC_P_DEFNAME, "face:%(font)s,fore:#007F7F,bold,size:%(size)d" % faces)
        # Operators
        self.StyleSetSpec(wx.stc.STC_P_OPERATOR, "face:%(font)s,size:%(size)d" % faces)
        # Identifiers
        self.StyleSetSpec(wx.stc.STC_P_IDENTIFIER, "face:%(font)s,fore:#%(color)s,face:%(font)s,size:%(size)d" % faces)
        # Comment-blocks
        self.StyleSetSpec(wx.stc.STC_P_COMMENTBLOCK, "face:%(font)s,fore:#7F7F7F,size:%(size)d" % faces)
        # End of line where string is not closed
        self.StyleSetSpec(wx.stc.STC_P_STRINGEOL, "face:%(font)s,fore:#000000,face:%(font)s,back:#E0C0E0,eol,size:%(size)d" % faces)


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
        if charBefore and chr(charBefore) in braces and styleBefore == wx.stc.STC_P_OPERATOR:
            braceAtCaret = caretPos - 1

        # check after
        if braceAtCaret < 0:
            charAfter = self.GetCharAt(caretPos)
            styleAfter = self.GetStyleAt(caretPos)
            if charAfter and chr(charAfter) in braces and styleAfter == wx.stc.STC_P_OPERATOR:
                braceAtCaret = caretPos

        if braceAtCaret >= 0:
            braceOpposite = self.BraceMatch(braceAtCaret)

        if braceAtCaret != -1  and braceOpposite == -1:
            self.BraceBadLight(braceAtCaret)
        else:
            self.BraceHighlight(braceAtCaret, braceOpposite)

        evt.Skip()


    def DoIndent(self):
        (text, caretPos) = self.GetCurLine()

        self._tokenizerChars = {}  # This is really too much, need to find something more like a C array
        for i in range(len(text)):
            self._tokenizerChars[i] = 0

        ctext = cStringIO.StringIO(text)
        try:
            tokenize.tokenize(ctext.readline, self)
        except:
            pass

        # Left in for debugging purposes:
        #for i in range(len(text)):
        #    print i, text[i], self._tokenizerChars[i]

        if caretPos == 0 or len(string.strip(text)) == 0:  # At beginning of line or within an empty line
            self.AddText('\n')
        else:
            doExtraIndent = False
            brackets = False
            commentStart = -1
            if caretPos > 1:
                startParenCount = 0
                endParenCount = 0
                startSquareBracketCount = 0
                endSquareBracketCount = 0
                startCurlyBracketCount = 0
                endCurlyBracketCount = 0
                startQuoteCount = 0
                endQuoteCount = 0
                for i in range(caretPos - 1, -1, -1): # Go through each character before the caret
                    if i >= len(text): # Sometimes the caret is at the end of the text if there is no LF
                        continue
                    if self._tokenizerChars[i] == 1:
                        continue
                    elif self._tokenizerChars[i] == 2:
                        startQuoteCount = startQuoteCount + 1
                    elif self._tokenizerChars[i] == 3:
                        endQuoteCount = endQuoteCount + 1
                    elif text[i] == '(': # Would be nice to use a dict for this, but the code is much more readable this way
                        startParenCount = startParenCount + 1
                    elif text[i] == ')':
                        endParenCount = endParenCount + 1
                    elif text[i] == "[":
                        startSquareBracketCount = startSquareBracketCount + 1
                    elif text[i] == "]":
                        endSquareBracketCount = endSquareBracketCount + 1
                    elif text[i] == "{":
                        startCurlyBracketCount = startCurlyBracketCount + 1
                    elif text[i] == "}":
                        endCurlyBracketCount = endCurlyBracketCount + 1
                    elif text[i] == "#":
                        commentStart = i
                        break
                    if startQuoteCount > endQuoteCount or startParenCount > endParenCount or startSquareBracketCount > endSquareBracketCount or startCurlyBracketCount > endCurlyBracketCount:
                        if i + 1 >= caretPos:  # Caret is right at the open paren, so just do indent as if colon was there
                            doExtraIndent = True
                            break
                        else:
                            spaces = " " * (i + 1)
                            brackets = True
                            break
            if not brackets:
                spaces = text[0:len(text) - len(string.lstrip(text))]
                if caretPos < len(spaces):  # If within the opening spaces of a line
                    spaces = spaces[:caretPos]

                # strip comment off
                if commentStart != -1:
                    text = text[0:commentStart]

                textNoTrailingSpaces = text[0:caretPos].rstrip()
                if doExtraIndent or len(textNoTrailingSpaces) and textNoTrailingSpaces[-1] == ':':
                    spaces = spaces + ' ' * self.GetIndent()
            self.AddText('\n' + spaces)
        self.EnsureCaretVisible()


    # Callback for tokenizer in self.DoIndent
    def __call__(self, toktype, toktext, (srow,scol), (erow,ecol), line):
        if toktype == tokenize.COMMENT:
            for i in range(scol, ecol + 1):
                self._validChars[i] = False
        elif toktype == token.STRING:
            self._tokenizerChars[scol] = 2 # Open quote
            self._tokenizerChars[ecol - 1] = 3 # Close quote
            for i in range(scol + 1, ecol - 2):
                self._tokenizerChars[i] = 1 # Part of string, 1 == ignore the char


class PythonOptionsPanel(wx.Panel):

    def __init__(self, parent, id):
        wx.Panel.__init__(self, parent, id)
        pathLabel = wx.StaticText(self, -1, _("python.exe Path:"))
        config = wx.ConfigBase_Get()
        path = config.Read("ActiveGridPythonLocation")
        self._pathTextCtrl = wx.TextCtrl(self, -1, path, size = (150, -1))
        self._pathTextCtrl.SetToolTipString(self._pathTextCtrl.GetValue())
        self._pathTextCtrl.SetInsertionPointEnd()
        choosePathButton = wx.Button(self, -1, _("Browse..."))
        pathSizer = wx.BoxSizer(wx.HORIZONTAL)
        HALF_SPACE = 5
        SPACE = 10
        pathSizer.Add(pathLabel, 0, wx.ALIGN_CENTER_VERTICAL|wx.LEFT|wx.TOP, HALF_SPACE)
        pathSizer.Add(self._pathTextCtrl, 1, wx.EXPAND|wx.LEFT|wx.TOP, HALF_SPACE)
        pathSizer.Add(choosePathButton, 0, wx.ALIGN_RIGHT|wx.LEFT|wx.RIGHT|wx.TOP, HALF_SPACE)
        wx.EVT_BUTTON(self, choosePathButton.GetId(), self.OnChoosePath)
        mainSizer = wx.BoxSizer(wx.VERTICAL)                
        mainSizer.Add(pathSizer, 0, wx.EXPAND|wx.LEFT|wx.RIGHT|wx.TOP, SPACE)

        self._otherOptions = STCTextEditor.TextOptionsPanel(self, -1, configPrefix = "Python", label = "Python", hasWordWrap = True, hasTabs = True, addPage=False, hasFolding=True)
        mainSizer.Add(self._otherOptions, 0, wx.EXPAND|wx.BOTTOM, SPACE)
        self.SetSizer(mainSizer)
        parent.AddPage(self, _("Python"))
        

    def OnChoosePath(self, event):
        defaultDir = os.path.dirname(self._pathTextCtrl.GetValue().strip())
        defaultFile = os.path.basename(self._pathTextCtrl.GetValue().strip())
        if _WINDOWS:
            wildcard = _("Executable (*.exe)|*.exe|All|*.*")
            if not defaultFile:
                defaultFile = "python.exe"
        else:
            wildcard = _("*")
        dlg = wx.FileDialog(wx.GetApp().GetTopWindow(),
                               _("Select a File"),
                               defaultDir=defaultDir,
                               defaultFile=defaultFile,
                               wildcard=wildcard,
                               style=wx.OPEN|wx.FILE_MUST_EXIST|wx.HIDE_READONLY)
        # dlg.CenterOnParent()  # wxBug: caused crash with wx.FileDialog
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            if path:  
                self._pathTextCtrl.SetValue(path)
                self._pathTextCtrl.SetToolTipString(self._pathTextCtrl.GetValue())
                self._pathTextCtrl.SetInsertionPointEnd()
        dlg.Destroy()            


    def OnOK(self, optionsDialog):
        config = wx.ConfigBase_Get()
        config.Write("ActiveGridPythonLocation", self._pathTextCtrl.GetValue().strip())

        self._otherOptions.OnOK(optionsDialog)
        

    def GetIcon(self):
        return getPythonIcon()
        

#----------------------------------------------------------------------------
# Icon Bitmaps - generated by encode_bitmaps.py
#----------------------------------------------------------------------------
from wx import ImageFromStream, BitmapFromImage
import cStringIO


def getPythonData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01\xe7IDAT8\x8d}\x921h\x13Q\x18\xc7\x7fw\xb9\x0ei\x9d*\xbd\xeb\x10\x8f\
,\x99\x1c*A[\xaa\x19B\xe8\xd0\xb1\x0e%K\x87\x88T2\x88Cqp\tD\x14i\xe9\xe0V\
\xdaQ\xb7\xe0P\xa1\x8b\xa0(\x95$z\xd5Q1\x90\xa2\xd7\x9a4^\x87\xa0`\x92!w9\
\x87\xf8.\xb9\xa6\xc97\xbd\xef{\xef\xfb\xbd\xff\xfb\xbfO*~;v\xf9\x1f\xad\xba\
\x05@\xf9\xd4\x06\xc0::$\xbb\x96\x92\x18\x11\n@(4\xdd\xcdB\xd3\xd4\x1d\x85\
\x8b\x97\xe1\xe3;\x83\x99\xe5\x15\xb2\xe0\x8e\x82\xc8\xa3\xe8\x003\xcb+\xac\
\xaee\xdda\xfb\xb2\x90\rPw\x14\x00\x9a\xb5\n\xbf\xfflSz\x9d\xa2Y\xdc"zca\xe8\
\x05\xb2h\x14\xcd\xd0\xf3B\x9f\x98\xe5\xf9\xde\x13"\xaaB\xc7\xb1\xcfU!\x0b\
\xc3D4k\x15\xac\x93\x03\xf4\x89Y\xaf\x96\xffT\x028\x17\xa2\xf4\'\xcdZ\x85\
\xf7F\x06{\xaa\x80ev\xc1\x91\xb91>\x18\x0f\xb8\xb7\x95a\xe9\xca\x0b:\x8e\xed\
\xca\x01E\x1a\x00\x98\r\x89\x92\x91\xa1\xda\xd8\x87\x06ha\x1f\x1b\x80\xcd\
\x9d%\xe0\xa5\x0f"[G\x87\x98\x8d\xde/ia\x05-\xac`\x996\xf9\\\x0b\xcb\xb4)\
\x1bmOMn\xf7\xd5\xf0\'\\\x8b\xdces\xe7\x8d\xef\x80h\xd6\xc2\n\xf9\\\x0b]\xf5\
\xab\xf2\xcdApR#\xf1kp4b\xc9 \xf9\\\x0b\x80\xe4\xcdE\xaf\xdeqlW\xaeVL\xaf`~\
\xd9\x03@W\xd3\x00\xc4\x13\x0b\xc4\x92A\xcf\xd0\xf9\xe8:\x89\xebW\x01(|\xfd\
\xe1\xbe-~F\xbas\xff\x91\xf75\x82n\x9d\x1c\xf0}\xfciw\xdd\xe7A<\xd1\x1b\xa8j\
c\x9f\xb2\xd1F\x92\xe4\x80O\x12\xc0\xc6\xb3\x14\xf6Ta\xe0)g\x81\xba\x9a\xf6\
\x9b(\x07\x14I@\x84lq\xb8?\xe6\xa3\xeb\x00\xdc\xba\x9d\xf4+\x10*~\xfem\xf3\
\xf8\xe1\x06\xc7\xa7\xdb\xe8j\x9a\xf8\xdc\xa4\xb7\x1f[\\\xe5\xd2\x851/\xff\
\x07\xac\x9b\xd1e\x12\x96\x0f\xfd\x00\x00\x00\x00IEND\xaeB`\x82' 


def getPythonBitmap():
    return BitmapFromImage(getPythonImage())

def getPythonImage():
    stream = cStringIO.StringIO(getPythonData())
    return ImageFromStream(stream)

def getPythonIcon():
    return wx.IconFromBitmap(getPythonBitmap())
