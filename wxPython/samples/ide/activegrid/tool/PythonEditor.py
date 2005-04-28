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


    def ProcessUpdateUIEvent(self, event):
        if not self.GetCtrl():
            return False
            
        id = event.GetId()
        if id == CodeEditor.CHECK_CODE_ID:
            hasText = self.GetCtrl().GetTextLength() > 0
            event.Enable(hasText)
            return True
            
        return CodeEditor.CodeView.ProcessUpdateUIEvent(self, event)


    def GetCtrlClass(self):
        """ Used in split window to instantiate new instances """
        return PythonCtrl


    def OnActivateView(self, activate, activeView, deactiveView):
        STCTextEditor.TextView.OnActivateView(self, activate, activeView, deactiveView)
        if activate:
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

        kw.sort(self.CaseInsensitiveCompare)

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

        # This takes a while for involved code
        checker.checkSyntax(self.GetDocument().GetFilename(), view)

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
            doc = wx.GetApp().GetDocumentManager().CreateDocument(filename, wx.lib.docview.DOC_SILENT)
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


class PythonService(CodeEditor.CodeService):


    def __init__(self):
        CodeEditor.CodeService.__init__(self)


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
                if isinstance(doc.GetFirstView(), PythonInterpreterView):
                    event.Check(True)
                    break
            return True
        else:
            return CodeEditor.CodeService.ProcessUpdateUIEvent(self, event)


    def OnViewPythonInterpreter(self, event):
        for doc in wx.GetApp().GetDocumentManager().GetDocuments():
            if isinstance(doc.GetFirstView(), PythonInterpreterView):
                doc.GetFirstView().GetDocument().DeleteAllViews()
                return
                
        docManager = self.GetDocumentManager()
        template = wx.lib.docview.DocTemplate(docManager,
                                          _("Python Interpreter"),
                                          "*.Foobar",
                                          "Foobar",
                                          ".Foobar",
                                          _("Python Interpreter Document"),
                                          _("Python Interpreter View"),
                                          wx.lib.docview.Document,
                                          PythonInterpreterView,
                                          flags = wx.lib.docview.TEMPLATE_INVISIBLE)
        newDoc = template.CreateDocument('', wx.lib.docview.DOC_SILENT)
        if newDoc:
            newDoc.SetDocumentName(template.GetDocumentName())
            newDoc.SetDocumentTemplate(template)
            newDoc.OnNewDocument()
            newDoc.SetWriteable(False)
            newDoc.GetFirstView().GetFrame().SetTitle(_("Python Interpreter"))


class PythonCtrl(CodeEditor.CodeCtrl):


    def __init__(self, parent, ID = -1, style = wx.NO_FULL_REPAINT_ON_RESIZE):
        CodeEditor.CodeCtrl.__init__(self, parent, ID, style)
        self.SetProperty("tab.timmy.whinge.level", "1")
        self.SetProperty("fold.comment.python", "1")
        self.SetProperty("fold.quotes.python", "1")
        self.SetLexer(wx.stc.STC_LEX_PYTHON)
        self.SetKeyWords(0, string.join(keyword.kwlist))


    def SetViewDefaults(self):
        CodeEditor.CodeCtrl.SetViewDefaults(self, configPrefix = "Python", hasWordWrap = False, hasTabs = True)


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
        pathSizer.Add(pathLabel, 0, wx.ALIGN_LEFT | wx.LEFT | wx.RIGHT | wx.TOP, HALF_SPACE)
        pathSizer.Add(self._pathTextCtrl, 0, wx.ALIGN_LEFT | wx.EXPAND | wx.RIGHT, HALF_SPACE)
        pathSizer.Add(choosePathButton, 0, wx.ALIGN_RIGHT | wx.LEFT, HALF_SPACE)
        wx.EVT_BUTTON(self, choosePathButton.GetId(), self.OnChoosePath)
        mainSizer = wx.BoxSizer(wx.VERTICAL)                
        mainSizer.Add(pathSizer, 0, wx.LEFT | wx.RIGHT | wx.TOP, 10)

        self._otherOptions = STCTextEditor.TextOptionsPanel(self, -1, configPrefix = "Python", label = "Python", hasWordWrap = False, hasTabs = True, addPage=False)
        mainSizer.Add(self._otherOptions)
        self.SetSizer(mainSizer)
        parent.AddPage(self, _("Python"))
        
    def OnChoosePath(self, event):
        if _WINDOWS:
            wildcard = _("*.exe")
        else:
            wildcard = _("*")
        path = wx.FileSelector(_("Select a File"),
                               _(""),
                               _(""),
                               wildcard = wildcard ,
                               flags = wx.HIDE_READONLY,
                               parent = wx.GetApp().GetTopWindow())
        if path:  
            self._pathTextCtrl.SetValue(path)
            self._pathTextCtrl.SetToolTipString(self._pathTextCtrl.GetValue())
            self._pathTextCtrl.SetInsertionPointEnd()

    def OnOK(self, optionsDialog):
        if len(self._pathTextCtrl.GetValue()) > 0:
            config = wx.ConfigBase_Get()
            config.Write("ActiveGridPythonLocation", self._pathTextCtrl.GetValue())

        self._otherOptions.OnOK(optionsDialog)
#----------------------------------------------------------------------------
# Icon Bitmaps - generated by encode_bitmaps.py
#----------------------------------------------------------------------------
from wx import ImageFromStream, BitmapFromImage
import cStringIO


def getPythonData():
    return \
"\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00\xd5IDAT8\x8d\x8d\x93Y\x0e\xc3 \x0cD\x9fM\xcf\xddNr2.\x96\xb8\x1f\
\x05\n\x84.#Y\x10\xa3\x19o\xb1\x99'*\xe2<\x82\x0e\xe6\xc9\xf8\x01\xef?\xa4\
\xf7)]\x05\x970O\xcdr\xce!\x119\xe7\x00\x02\x88\xfe}i\xb5\x848\x8f\xa8\x19\
\xcc\x19}+\xc5\xcc\xd3\x92<CZ\x0b\x99\xc4\xb2N\x01<\x80\xad\xdc?\x88\xf8\x1c\
X\x8f7\xe1\x1f\xdc*\xa9a+\xe1\xa3\xdc\xe7\xb4\xf6\xd1\xe5\xb6'\xc3@\xc5\xa0#\
\xab\x94\xd1\x0bL\xf0\xe6\x17\xa8v\xc3\x8aS\xa0.\x8be\x13\xe3\x15\x8f\xe1\
\xa5D\xee\xc9\xdb~%\xc7y\x84\xbb'sO\xd6\xd4\x17\xe4~\xc4\xf5\xef\xac\xa7\r\
\xbbp?b&\x0f\x89i\x14\x93\xca\x14z\xc5oh\x02E\xc4<\xd92\x03\xe0:B^\xc4K#\xe7\
\xe5\x00\x02\xfd\xb9H\x9ex\x02\x9a\x05a\xd2\xd3c\xc0\xcc\x00\x00\x00\x00IEND\
\xaeB`\x82" 


def getPythonBitmap():
    return BitmapFromImage(getPythonImage())

def getPythonImage():
    stream = cStringIO.StringIO(getPythonData())
    return ImageFromStream(stream)

def getPythonIcon():
    return wx.IconFromBitmap(getPythonBitmap())
