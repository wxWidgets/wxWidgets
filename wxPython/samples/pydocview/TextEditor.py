#----------------------------------------------------------------------------
# Name:         TextEditor.py
# Purpose:      Text Editor for pydocview
#
# Author:       Peter Yared
#
# Created:      8/15/03
# CVS-ID:       $Id$
# Copyright:    (c) 2003-2005 ActiveGrid, Inc.
# License:      ASL 2.0  http://apache.org/licenses/LICENSE-2.0
#----------------------------------------------------------------------------
import wx
import wx.lib.docview
import wx.lib.pydocview
import string
import FindService
_ = wx.GetTranslation

class TextDocument(wx.lib.docview.Document):


    def OnSaveDocument(self, filename):
        view = self.GetFirstView()
        if not view.GetTextCtrl().SaveFile(filename):
            return False
        self.Modify(False)
        self.SetDocumentSaved(True)
        #if wx.Platform == "__WXMAC__":
        #    fn = wx.Filename(filename)
        #    fn.MacSetDefaultTypeAndCreator()
        return True


    def OnOpenDocument(self, filename):
        view = self.GetFirstView()
        if not view.GetTextCtrl().LoadFile(filename):
            return False
        self.SetFilename(filename, True)
        self.Modify(False)
        self.UpdateAllViews()
        self._savedYet = True
        return True


    def IsModified(self):
        view = self.GetFirstView()
        if view and view.GetTextCtrl():
            return wx.lib.docview.Document.IsModified(self) or view.GetTextCtrl().IsModified()
        else:
            return wx.lib.docview.Document.IsModified(self)


    def Modify(self, mod):
        view = self.GetFirstView()
        wx.lib.docview.Document.Modify(self, mod)
        if not mod and view and view.GetTextCtrl():
            view.GetTextCtrl().DiscardEdits()


class TextView(wx.lib.docview.View):


    #----------------------------------------------------------------------------
    # Overridden methods
    #----------------------------------------------------------------------------

    def __init__(self):
        wx.lib.docview.View.__init__(self)
        self._textCtrl = None
        self._wordWrap = wx.ConfigBase_Get().ReadInt("TextEditorWordWrap", True)


    def OnCreate(self, doc, flags):
        frame = wx.GetApp().CreateDocumentFrame(self, doc, flags)
        sizer = wx.BoxSizer()
        font, color = self._GetFontAndColorFromConfig()
        self._textCtrl = self._BuildTextCtrl(frame, font, color = color)
        sizer.Add(self._textCtrl, 1, wx.EXPAND, 0)
        frame.SetSizer(sizer)
        frame.Layout()
        frame.Show(True)
        self.Activate()
        return True


    def _BuildTextCtrl(self, parent, font, color = wx.BLACK, value = "", selection = [0, 0]):
        if self._wordWrap:
            wordWrapStyle = wx.TE_WORDWRAP
        else:
            wordWrapStyle = wx.TE_DONTWRAP
        textCtrl = wx.TextCtrl(parent, -1, pos = wx.DefaultPosition, size = parent.GetClientSize(), style = wx.TE_MULTILINE | wordWrapStyle)
        textCtrl.SetFont(font)
        textCtrl.SetForegroundColour(color)
        textCtrl.SetValue(value)
        return textCtrl


    def _GetFontAndColorFromConfig(self):
        font = wx.Font(10, wx.DEFAULT, wx.NORMAL, wx.NORMAL)
        config = wx.ConfigBase_Get()
        fontData = config.Read("TextEditorFont", "")
        if fontData:
            nativeFont = wx.NativeFontInfo()
            nativeFont.FromString(fontData)
            font.SetNativeFontInfo(nativeFont)
        color = wx.BLACK
        colorData = config.Read("TextEditorColor", "")
        if colorData:
            red = int("0x" + colorData[0:2], 16)
            green = int("0x" + colorData[2:4], 16)
            blue = int("0x" + colorData[4:6], 16)
            color = wx.Color(red, green, blue)
        return font, color


    def OnCreateCommandProcessor(self):
        # Don't create a command processor, it has its own
        pass


    def OnActivateView(self, activate, activeView, deactiveView):
        if activate and self._textCtrl:
            # In MDI mode just calling set focus doesn't work and in SDI mode using CallAfter causes an endless loop
            if self.GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI:
                self._textCtrl.SetFocus()
            else:
                def SetFocusToTextCtrl():
                    if self._textCtrl:  # Need to make sure it is there in case we are in the closeall mode of the MDI window
                        self._textCtrl.SetFocus()
                wx.CallAfter(SetFocusToTextCtrl)


    def OnUpdate(self, sender = None, hint = None):
        if hint == "Word Wrap":
            self.SetWordWrap(wx.ConfigBase_Get().ReadInt("TextEditorWordWrap", True))
        elif hint == "Font":
            font, color = self._GetFontAndColorFromConfig()
            self.SetFont(font, color)


    def OnClose(self, deleteWindow = True):
        if not wx.lib.docview.View.OnClose(self, deleteWindow):
            return False
        self.Activate(False)
        if deleteWindow and self.GetFrame():
            self.GetFrame().Destroy()
        return True


    # Since ProcessEvent is not virtual, we have to trap the relevant events using this pseudo-ProcessEvent instead of EVT_MENU
    def ProcessEvent(self, event):
        id = event.GetId()
        if id == wx.ID_UNDO:
            if not self._textCtrl:
                return False
            self._textCtrl.Undo()
            return True
        elif id == wx.ID_REDO:
            if not self._textCtrl:
                return False
            self._textCtrl.Redo()
            return True
        elif id == wx.ID_CUT:
            if not self._textCtrl:
                return False
            self._textCtrl.Cut()
            return True
        elif id == wx.ID_COPY:
            if not self._textCtrl:
                return False
            self._textCtrl.Copy()
            return True
        elif id == wx.ID_PASTE:
            if not self._textCtrl:
                return False
            self._textCtrl.Paste()
            return True
        elif id == wx.ID_CLEAR:
            if not self._textCtrl:
                return False
            self._textCtrl.Replace(self._textCtrl.GetSelection()[0], self._textCtrl.GetSelection()[1], '')
            return True
        elif id == wx.ID_SELECTALL:
            if not self._textCtrl:
                return False
            self._textCtrl.SetSelection(-1, -1)
            return True
        elif id == TextService.CHOOSE_FONT_ID:
            if not self._textCtrl:
                return False
            self.OnChooseFont(event)
            return True
        elif id == TextService.WORD_WRAP_ID:
            if not self._textCtrl:
                return False
            self.OnWordWrap(event)
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
        if not self._textCtrl:
            return False

        hasText = len(self._textCtrl.GetValue()) > 0

        id = event.GetId()
        if id == wx.ID_UNDO:
            event.Enable(self._textCtrl.CanUndo())
            return True
        elif id == wx.ID_REDO:
            event.Enable(self._textCtrl.CanRedo())
            return True
        if id == wx.ID_CUT:
            event.Enable(self._textCtrl.CanCut())
            return True
        elif id == wx.ID_COPY:
            event.Enable(self._textCtrl.CanCopy())
            return True
        elif id == wx.ID_PASTE:
            event.Enable(self._textCtrl.CanPaste())
            return True
        elif id == wx.ID_CLEAR:
            event.Enable(self._textCtrl.CanCopy())
            return True
        elif id == wx.ID_SELECTALL:
            event.Enable(hasText)
            return True
        elif id == TextService.CHOOSE_FONT_ID:
            event.Enable(True)
            return True
        elif id == TextService.WORD_WRAP_ID:
            event.Enable(True)
            return True
        elif id == FindService.FindService.FIND_ID:
            event.Enable(hasText)
            return True
        elif id == FindService.FindService.FIND_PREVIOUS_ID:
            event.Enable(hasText and
                         self._FindServiceHasString() and
                         self._textCtrl.GetSelection()[0] > 0)
            return True
        elif id == FindService.FindService.FIND_NEXT_ID:
            event.Enable(hasText and
                         self._FindServiceHasString() and
                         self._textCtrl.GetSelection()[0] < len(self._textCtrl.GetValue()))
            return True
        elif id == FindService.FindService.REPLACE_ID:
            event.Enable(hasText)
            return True
        elif id == FindService.FindService.GOTO_LINE_ID:
            event.Enable(True)
            return True
        else:
            return wx.lib.docview.View.ProcessUpdateUIEvent(self, event)


    #----------------------------------------------------------------------------
    # Methods for TextDocument to call
    #----------------------------------------------------------------------------

    def GetTextCtrl(self):
        return self._textCtrl


    #----------------------------------------------------------------------------
    # Format methods
    #----------------------------------------------------------------------------

    def OnChooseFont(self, event):
        data = wx.FontData()
        data.EnableEffects(True)
        data.SetInitialFont(self._textCtrl.GetFont())
        data.SetColour(self._textCtrl.GetForegroundColour())
        fontDialog = wx.FontDialog(self.GetFrame(), data)
        if fontDialog.ShowModal() == wx.ID_OK:
            data = fontDialog.GetFontData()
            self.SetFont(data.GetChosenFont(), data.GetColour())
        fontDialog.Destroy()


    def SetFont(self, font, color):
        self._textCtrl.SetFont(font)
        self._textCtrl.SetForegroundColour(color)
        self._textCtrl.Refresh()
        self._textCtrl.Layout()


    def OnWordWrap(self, event):
        self.SetWordWrap(not self.GetWordWrap())


    def GetWordWrap(self):
        return self._wordWrap


    def SetWordWrap(self, wordWrap = True):
        self._wordWrap = wordWrap
        temp = self._textCtrl
        self._textCtrl = self._BuildTextCtrl(temp.GetParent(),
                                             font = temp.GetFont(),
                                             color = temp.GetForegroundColour(),
                                             value = temp.GetValue(),
                                             selection = temp.GetSelection())
        self.GetDocument().Modify(temp.IsModified())
        temp.Destroy()


    #----------------------------------------------------------------------------
    # Find methods
    #----------------------------------------------------------------------------

    def OnFind(self, replace = False):
        findService = wx.GetApp().GetService(FindService.FindService)
        if findService:
            findService.ShowFindReplaceDialog(findString = self._textCtrl.GetStringSelection(), replace = replace)


    def DoFind(self, forceFindNext = False, forceFindPrevious = False, replace = False, replaceAll = False):
        findService = wx.GetApp().GetService(FindService.FindService)
        if not findService:
            return
        findString = findService.GetFindString()
        if len(findString) == 0:
            return -1
        replaceString = findService.GetReplaceString()
        flags = findService.GetFlags()
        startLoc, endLoc = self._textCtrl.GetSelection()

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

        # On replace dialog operations, user is allowed to replace the currently highlighted text to determine if it should be replaced or not.
        # Typically, it is the text from a previous find operation, but we must check to see if it isn't, user may have moved the cursor or selected some other text accidentally.
        # If the text is a match, then replace it.
        if replace:
            result, start, end, replText = findService.DoFind(findString, replaceString, self._textCtrl.GetStringSelection(), 0, 0, True, matchCase, wholeWord, regExp, replace)
            if result > 0:
                self._textCtrl.Replace(startLoc, endLoc, replaceString)
                self.GetDocument().Modify(True)
                wx.GetApp().GetTopWindow().PushStatusText(_("1 occurrence of \"%s\" replaced") % findString)
                if down:
                    startLoc += len(replText)  # advance start location past replacement string to new text
                endLoc = startLoc

        text = self._textCtrl.GetValue()
        if wx.Platform == "__WXMSW__":
            text = string.replace(text, '\n', '\r\n')

        # Find the next matching text occurance or if it is a ReplaceAll, replace all occurances
        # Even if the user is Replacing, we should replace here, but only select the text and let the user replace it with the next Replace operation
        result, start, end, text = findService.DoFind(findString, replaceString, text, startLoc, endLoc, down, matchCase, wholeWord, regExp, False, replaceAll, wrap)
        if result > 0:
            self._textCtrl.SetValue(text)
            self.GetDocument().Modify(True)
            if result == 1:
                wx.GetApp().GetTopWindow().PushStatusText(_("1 occurrence of \"%s\" replaced") % findString)
            else:
                wx.GetApp().GetTopWindow().PushStatusText(_("%i occurrences of \"%s\" replaced") % (result, findString))
        elif result == 0:
            self._textCtrl.SetSelection(start, end)
            self._textCtrl.SetFocus()
            wx.GetApp().GetTopWindow().PushStatusText(_("Found \"%s\"") % findString)
        else:
            wx.GetApp().GetTopWindow().PushStatusText(_("Can't find \"%s\"") % findString)


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
                pos = self._textCtrl.XYToPosition(0, line - 1)
                self._textCtrl.SetSelection(pos, pos)


class TextService(wx.lib.pydocview.DocService):


    WORD_WRAP_ID = wx.NewId()
    CHOOSE_FONT_ID = wx.NewId()


    def __init__(self):
        wx.lib.pydocview.DocService.__init__(self)


    def InstallControls(self, frame, menuBar = None, toolBar = None, statusBar = None, document = None):
        if document and document.GetDocumentTemplate().GetDocumentType() != TextDocument:
            return
        config = wx.ConfigBase_Get()

        formatMenuIndex = menuBar.FindMenu(_("&Format"))
        if formatMenuIndex > -1:
            formatMenu = menuBar.GetMenu(formatMenuIndex)
        else:
            formatMenu = wx.Menu()
        formatMenu = wx.Menu()
        if not menuBar.FindItemById(TextService.WORD_WRAP_ID):
            formatMenu.AppendCheckItem(TextService.WORD_WRAP_ID, _("Word Wrap"), _("Wraps text horizontally when checked"))
            formatMenu.Check(TextService.WORD_WRAP_ID, config.ReadInt("TextEditorWordWrap", True))
            wx.EVT_MENU(frame, TextService.WORD_WRAP_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, TextService.WORD_WRAP_ID, frame.ProcessUpdateUIEvent)
        if not menuBar.FindItemById(TextService.CHOOSE_FONT_ID):
            formatMenu.Append(TextService.CHOOSE_FONT_ID, _("Font..."), _("Sets the font to use"))
            wx.EVT_MENU(frame, TextService.CHOOSE_FONT_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, TextService.CHOOSE_FONT_ID, frame.ProcessUpdateUIEvent)
        if formatMenuIndex == -1:
            viewMenuIndex = menuBar.FindMenu(_("&View"))
            menuBar.Insert(viewMenuIndex + 1, formatMenu, _("&Format"))


    def ProcessUpdateUIEvent(self, event):
        id = event.GetId()
        if id == TextService.CHOOSE_FONT_ID:
            event.Enable(False)
            return True
        elif id == TextService.WORD_WRAP_ID:
            event.Enable(False)
            return True
        else:
            return False


class TextOptionsPanel(wx.Panel):


    def __init__(self, parent, id):
        wx.Panel.__init__(self, parent, id)
        SPACE = 10
        HALF_SPACE   = 5
        config = wx.ConfigBase_Get()
        self._textFont = wx.Font(10, wx.DEFAULT, wx.NORMAL, wx.NORMAL)
        fontData = config.Read("TextEditorFont", "")
        if fontData:
            nativeFont = wx.NativeFontInfo()
            nativeFont.FromString(fontData)
            self._textFont.SetNativeFontInfo(nativeFont)
        self._originalTextFont = self._textFont
        self._textColor = wx.BLACK
        colorData = config.Read("TextEditorColor", "")
        if colorData:
            red = int("0x" + colorData[0:2], 16)
            green = int("0x" + colorData[2:4], 16)
            blue = int("0x" + colorData[4:6], 16)
            self._textColor = wx.Color(red, green, blue)
        self._originalTextColor = self._textColor
        parent.AddPage(self, _("Text"))
        fontLabel = wx.StaticText(self, -1, _("Font:"))
        self._sampleTextCtrl = wx.TextCtrl(self, -1, "", size = (125, -1))
        self._sampleTextCtrl.SetEditable(False)
        chooseFontButton = wx.Button(self, -1, _("Choose Font..."))
        wx.EVT_BUTTON(self, chooseFontButton.GetId(), self.OnChooseFont)
        self._wordWrapCheckBox = wx.CheckBox(self, -1, _("Wrap words inside text area"))
        self._wordWrapCheckBox.SetValue(wx.ConfigBase_Get().ReadInt("TextEditorWordWrap", True))
        textPanelBorderSizer = wx.BoxSizer(wx.VERTICAL)
        textPanelSizer = wx.BoxSizer(wx.VERTICAL)
        textFontSizer = wx.BoxSizer(wx.HORIZONTAL)
        textFontSizer.Add(fontLabel, 0, wx.ALIGN_LEFT | wx.RIGHT | wx.TOP, HALF_SPACE)
        textFontSizer.Add(self._sampleTextCtrl, 0, wx.ALIGN_LEFT | wx.EXPAND | wx.RIGHT, HALF_SPACE)
        textFontSizer.Add(chooseFontButton, 0, wx.ALIGN_RIGHT | wx.LEFT, HALF_SPACE)
        textPanelSizer.Add(textFontSizer, 0, wx.ALL, HALF_SPACE)
        textPanelSizer.Add(self._wordWrapCheckBox, 0, wx.ALL, HALF_SPACE)
        textPanelBorderSizer.Add(textPanelSizer, 0, wx.ALL, SPACE)
        self.SetSizer(textPanelBorderSizer)
        self.UpdateSampleFont()


    def UpdateSampleFont(self):
        nativeFont = wx.NativeFontInfo()
        nativeFont.FromString(self._textFont.GetNativeFontInfoDesc())
        font = wx.NullFont
        font.SetNativeFontInfo(nativeFont)
        font.SetPointSize(self._sampleTextCtrl.GetFont().GetPointSize())  # Use the standard point size
        self._sampleTextCtrl.SetFont(font)
        self._sampleTextCtrl.SetForegroundColour(self._textColor)
        self._sampleTextCtrl.SetValue(_("%d pt. %s") % (self._textFont.GetPointSize(), self._textFont.GetFaceName()))
        self._sampleTextCtrl.Refresh()
        self.Layout()


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
        doWordWrapUpdate = config.ReadInt("TextEditorWordWrap", True) != self._wordWrapCheckBox.GetValue()
        config.WriteInt("TextEditorWordWrap", self._wordWrapCheckBox.GetValue())
        doFontUpdate = self._originalTextFont != self._textFont or self._originalTextColor != self._textColor
        config.Write("TextEditorFont", self._textFont.GetNativeFontInfoDesc())
        config.Write("TextEditorColor", "%02x%02x%02x" % (self._textColor.Red(), self._textColor.Green(), self._textColor.Blue()))
        if doWordWrapUpdate or doFontUpdate:
            for document in optionsDialog.GetDocManager().GetDocuments():
                if document.GetDocumentTemplate().GetDocumentType() == TextDocument:
                    if doWordWrapUpdate:
                        document.UpdateAllViews(hint = "Word Wrap")
                    if doFontUpdate:
                        document.UpdateAllViews(hint = "Font")
