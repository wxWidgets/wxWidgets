
import wx
import wx.richtext as rt
import images

#----------------------------------------------------------------------

class RichTextFrame(wx.Frame):
    def __init__(self, *args, **kw):
        wx.Frame.__init__(self, *args, **kw)

        self.MakeMenuBar()
        self.MakeToolBar()
        self.CreateStatusBar()
        self.SetStatusText("Welcome to wx.richtext.RichTextCtrl!")

        self.rtc = rt.RichTextCtrl(self, style=wx.VSCROLL|wx.HSCROLL|wx.NO_BORDER);
        wx.CallAfter(self.rtc.SetFocus)

        self.rtc.Freeze()
        self.rtc.BeginSuppressUndo()

        self.rtc.BeginParagraphSpacing(0, 20)

        self.rtc.BeginAlignment(rt.TEXT_ALIGNMENT_CENTRE)
        self.rtc.BeginBold()

        self.rtc.BeginFontSize(14)
        self.rtc.WriteText("Welcome to wxRichTextCtrl, a wxWidgets control for editing and presenting styled text and images")
        self.rtc.EndFontSize()
        self.rtc.Newline()

        self.rtc.BeginItalic()
        self.rtc.WriteText("by Julian Smart")
        self.rtc.EndItalic()

        self.rtc.EndBold()

        self.rtc.Newline()
        self.rtc.WriteImage(images.get_rt_zebraImage())

        self.rtc.EndAlignment()

        self.rtc.Newline()
        self.rtc.Newline()

        self.rtc.WriteText("What can you do with this thing? ")
        self.rtc.WriteImage(images.get_rt_smileyImage())
        self.rtc.WriteText(" Well, you can change text ")

        self.rtc.BeginTextColour((255, 0, 0))
        self.rtc.WriteText("colour, like this red bit.")
        self.rtc.EndTextColour()

        self.rtc.BeginTextColour((0, 0, 255))
        self.rtc.WriteText(" And this blue bit.")
        self.rtc.EndTextColour()

        self.rtc.WriteText(" Naturally you can make things ")
        self.rtc.BeginBold()
        self.rtc.WriteText("bold ")
        self.rtc.EndBold()
        self.rtc.BeginItalic()
        self.rtc.WriteText("or italic ")
        self.rtc.EndItalic()
        self.rtc.BeginUnderline()
        self.rtc.WriteText("or underlined.")
        self.rtc.EndUnderline()

        self.rtc.BeginFontSize(14)
        self.rtc.WriteText(" Different font sizes on the same line is allowed, too.")
        self.rtc.EndFontSize()

        self.rtc.WriteText(" Next we'll show an indented paragraph.")

        self.rtc.BeginLeftIndent(60)
        self.rtc.Newline()

        self.rtc.WriteText("It was in January, the most down-trodden month of an Edinburgh winter. An attractive woman came into the cafe, which is nothing remarkable.")
        self.rtc.EndLeftIndent()

        self.rtc.Newline()

        self.rtc.WriteText("Next, we'll show a first-line indent, achieved using BeginLeftIndent(100, -40).")

        self.rtc.BeginLeftIndent(100, -40)
        self.rtc.Newline()

        self.rtc.WriteText("It was in January, the most down-trodden month of an Edinburgh winteself.rtc. An attractive woman came into the cafe, which is nothing remarkable.")
        self.rtc.EndLeftIndent()

        self.rtc.Newline()

        self.rtc.WriteText("Numbered bullets are possible, again using subindents:")

        self.rtc.BeginNumberedBullet(1, 100, 60)
        self.rtc.Newline()

        self.rtc.WriteText("This is my first item. Note that wxRichTextCtrl doesn't automatically do numbering, but this will be added later.")
        self.rtc.EndNumberedBullet()

        self.rtc.BeginNumberedBullet(2, 100, 60)
        self.rtc.Newline()

        self.rtc.WriteText("This is my second item.")
        self.rtc.EndNumberedBullet()

        self.rtc.Newline()

        self.rtc.WriteText("The following paragraph is right-indented:")

        self.rtc.BeginRightIndent(200)
        self.rtc.Newline()

        self.rtc.WriteText("It was in January, the most down-trodden month of an Edinburgh winter. An attractive woman came into the cafe, which is nothing remarkable.")
        self.rtc.EndRightIndent()

        self.rtc.Newline()

        self.rtc.WriteText("The following paragraph is right-aligned with 1.5 line spacing:")

        self.rtc.BeginAlignment(rt.TEXT_ALIGNMENT_RIGHT)
        self.rtc.BeginLineSpacing(rt.TEXT_ATTR_LINE_SPACING_HALF)
        self.rtc.Newline()

        self.rtc.WriteText("It was in January, the most down-trodden month of an Edinburgh winter. An attractive woman came into the cafe, which is nothing remarkable.")
        self.rtc.EndLineSpacing()
        self.rtc.EndAlignment()

        self.rtc.Newline()
        self.rtc.WriteText("Other notable features of wxRichTextCtrl include:")

        self.rtc.BeginSymbolBullet('*', 100, 60)
        self.rtc.Newline()
        self.rtc.WriteText("Compatibility with wxTextCtrl API")
        self.rtc.EndSymbolBullet()

        self.rtc.BeginSymbolBullet('*', 100, 60)
        self.rtc.Newline()
        self.rtc.WriteText("Easy stack-based BeginXXX()...EndXXX() style setting in addition to SetStyle()")
        self.rtc.EndSymbolBullet()

        self.rtc.BeginSymbolBullet('*', 100, 60)
        self.rtc.Newline()
        self.rtc.WriteText("XML loading and saving")
        self.rtc.EndSymbolBullet()

        self.rtc.BeginSymbolBullet('*', 100, 60)
        self.rtc.Newline()
        self.rtc.WriteText("Undo/Redo, with batching option and Undo suppressing")
        self.rtc.EndSymbolBullet()

        self.rtc.BeginSymbolBullet('*', 100, 60)
        self.rtc.Newline()
        self.rtc.WriteText("Clipboard copy and paste")
        self.rtc.EndSymbolBullet()

        self.rtc.BeginSymbolBullet('*', 100, 60)
        self.rtc.Newline()
        self.rtc.WriteText("wxRichTextStyleSheet with named character and paragraph styles, and control for applying named styles")
        self.rtc.EndSymbolBullet()

        self.rtc.BeginSymbolBullet('*', 100, 60)
        self.rtc.Newline()
        self.rtc.WriteText("A design that can easily be extended to other content types, ultimately with text boxes, tables, controls, and so on")
        self.rtc.EndSymbolBullet()

        self.rtc.Newline()

        self.rtc.WriteText("Note: this sample content was generated programmatically from within the MyFrame constructor in the demo. The images were loaded from inline XPMs. Enjoy wxRichTextCtrl!")

        self.rtc.EndParagraphSpacing()

        self.rtc.EndSuppressUndo()
        self.rtc.Thaw()
        

    def OnFileOpen(self, evt):
        # TODO: Use RichTextBuffer.GetExtWildcard to get the wildcard string
        dlg = wx.FileDialog(self, "Choose a filename",
                            wildcard="All files (*.*)|*.*",
                            style=wx.OPEN)
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            if path:
                # TODO: use the filter index to determine what file type to use
                self.rtc.LoadFile(path, rt.RICHTEXT_TYPE_TEXT)
        dlg.Destroy()

        
    def OnFileSave(self, evt):
        if not self.rtc.GetFilename():
            self.OnFileSaveAs(evt)
            return
        self.rtc.SaveFile()
        
    def OnFileSaveAs(self, evt):
        # TODO: Use RichTextBuffer.GetExtWildcard to get the wildcard string
        dlg = wx.FileDialog(self, "Choose a filename",
                            wildcard="All files (*.*)|*.*",
                            style=wx.SAVE)
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            if path:
                self.rtc.SaveFile(path)
        dlg.Destroy()
        
                
    def OnFileViewHTML(self, evt): pass

    
    def OnFileExit(self, evt):
        self.Close(True)

      
    def OnBold(self, evt):
        self.rtc.ApplyBoldToSelection()
        
    def OnItalic(self, evt): 
        self.rtc.ApplyItalicToSelection()
        
    def OnUnderline(self, evt):
        self.rtc.ApplyUnderlineToSelection()
        
    def OnAlignLeft(self, evt):
        self.rtc.ApplyAlignmentToSelection(rt.TEXT_ALIGNMENT_LEFT)
        
    def OnAlignRight(self, evt):
        self.rtc.ApplyAlignmentToSelection(rt.TEXT_ALIGNMENT_RIGHT)
        
    def OnAlignCenter(self, evt):
        self.rtc.ApplyAlignmentToSelection(rt.TEXT_ALIGNMENT_CENTRE)
        
    def OnIndentMore(self, evt):
        attr = rt.RichTextAttr()
        attr.SetFlags(rt.TEXT_ATTR_LEFT_INDENT)
        ip = self.rtc.GetInsertionPoint()
        if self.rtc.GetStyle(ip, attr):
            r = rt.RichTextRange(ip, ip)
            if self.rtc.HasSelection():
                r = self.rtc.GetSelectionRange()

            attr.SetLeftIndent(attr.GetLeftIndent() + 100)
            attr.SetFlags(rt.TEXT_ATTR_LEFT_INDENT)
            self.rtc.SetStyle(r, attr)
       
        
    def OnIndentLess(self, evt):
        attr = rt.RichTextAttr()
        attr.SetFlags(rt.TEXT_ATTR_LEFT_INDENT)
        ip = self.rtc.GetInsertionPoint()
        if self.rtc.GetStyle(ip, attr):
            r = rt.RichTextRange(ip, ip)
            if self.rtc.HasSelection():
                r = self.rtc.GetSelectionRange()

        if attr.GetLeftIndent() >= 100:
            attr.SetLeftIndent(attr.GetLeftIndent() - 100)
            attr.SetFlags(rt.TEXT_ATTR_LEFT_INDENT)
            self.rtc.SetStyle(r, attr)

        
    def OnParagraphSpacingMore(self, evt):
        attr = rt.RichTextAttr()
        attr.SetFlags(rt.TEXT_ATTR_PARA_SPACING_AFTER)
        ip = self.rtc.GetInsertionPoint()
        if self.rtc.GetStyle(ip, attr):
            r = rt.RichTextRange(ip, ip)
            if self.rtc.HasSelection():
                r = self.rtc.GetSelectionRange()

            attr.SetParagraphSpacingAfter(attr.GetParagraphSpacingAfter() + 20);
            attr.SetFlags(rt.TEXT_ATTR_PARA_SPACING_AFTER)
            self.rtc.SetStyle(r, attr)

        
    def OnParagraphSpacingLess(self, evt):
        attr = rt.RichTextAttr()
        attr.SetFlags(rt.TEXT_ATTR_PARA_SPACING_AFTER)
        ip = self.rtc.GetInsertionPoint()
        if self.rtc.GetStyle(ip, attr):
            r = rt.RichTextRange(ip, ip)
            if self.rtc.HasSelection():
                r = self.rtc.GetSelectionRange()

            if attr.GetParagraphSpacingAfter() >= 20:
                attr.SetParagraphSpacingAfter(attr.GetParagraphSpacingAfter() - 20);
                attr.SetFlags(rt.TEXT_ATTR_PARA_SPACING_AFTER)
                self.rtc.SetStyle(r, attr)

        
    def OnLineSpacingSingle(self, evt): 
        attr = rt.RichTextAttr()
        attr.SetFlags(rt.TEXT_ATTR_LINE_SPACING)
        ip = self.rtc.GetInsertionPoint()
        if self.rtc.GetStyle(ip, attr):
            r = rt.RichTextRange(ip, ip)
            if self.rtc.HasSelection():
                r = self.rtc.GetSelectionRange()

            attr.SetFlags(rt.TEXT_ATTR_LINE_SPACING)
            attr.SetLineSpacing(10)
            self.rtc.SetStyle(r, attr)
 
                
    def OnLineSpacingHalf(self, evt):
        attr = rt.RichTextAttr()
        attr.SetFlags(rt.TEXT_ATTR_LINE_SPACING)
        ip = self.rtc.GetInsertionPoint()
        if self.rtc.GetStyle(ip, attr):
            r = rt.RichTextRange(ip, ip)
            if self.rtc.HasSelection():
                r = self.rtc.GetSelectionRange()

            attr.SetFlags(rt.TEXT_ATTR_LINE_SPACING)
            attr.SetLineSpacing(15)
            self.rtc.SetStyle(r, attr)

        
    def OnLineSpacingDouble(self, evt):
        attr = rt.RichTextAttr()
        attr.SetFlags(rt.TEXT_ATTR_LINE_SPACING)
        ip = self.rtc.GetInsertionPoint()
        if self.rtc.GetStyle(ip, attr):
            r = rt.RichTextRange(ip, ip)
            if self.rtc.HasSelection():
                r = self.rtc.GetSelectionRange()

            attr.SetFlags(rt.TEXT_ATTR_LINE_SPACING)
            attr.SetLineSpacing(20)
            self.rtc.SetStyle(r, attr)


    def OnFont(self, evt):
        if not self.rtc.HasSelection():
            return

        r = self.rtc.GetSelectionRange()
        fontData = wx.FontData()
        fontData.EnableEffects(False)
        attr = rt.RichTextAttr()
        attr.SetFlags(rt.TEXT_ATTR_FONT)
        if self.rtc.GetStyle(self.rtc.GetInsertionPoint(), attr):
            fontData.SetInitialFont(attr.CreateFont())

        dlg = wx.FontDialog(self, fontData)
        if dlg.ShowModal() == wx.ID_OK:
            fontData = dlg.GetFontData()
            font = fontData.GetChosenFont()
            if font:
                attr.SetFlags(rt.TEXT_ATTR_FONT)
                attr.SetFont(font)
                self.rtc.SetStyle(r, attr)
        dlg.Destroy()


    def OnColour(self, evt):
        colourData = wx.ColourData()
        attr = rt.RichTextAttr()
        attr.SetFlags(rt.TEXT_ATTR_TEXT_COLOUR)
        if self.rtc.GetStyle(self.rtc.GetInsertionPoint(), attr):
            colourData.SetColour(attr.GetTextColour())

        dlg = wx.ColourDialog(self, colourData)
        if dlg.ShowModal() == wx.ID_OK:
            colourData = dlg.GetColourData()
            colour = colourData.GetColour()
            if colour:
                if not self.rtc.HasSelection():
                    self.rtc.BeginTextColour(colour)
                else:
                    r = self.rtc.GetSelectionRange()
                    attr.SetFlags(rt.TEXT_ATTR_TEXT_COLOUR)
                    attr.SetTextColour(colour)
                    self.rtc.SetStyle(r, attr)
        dlg.Destroy()
        


    def OnUpdateBold(self, evt):
        evt.Check(self.rtc.IsSelectionBold())
    
    def OnUpdateItalic(self, evt): 
        evt.Check(self.rtc.IsSelectionItalics())
    
    def OnUpdateUnderline(self, evt): 
        evt.Check(self.rtc.IsSelectionUnderlined())
    
    def OnUpdateAlignLeft(self, evt):
        evt.Check(self.rtc.IsSelectionAligned(rt.TEXT_ALIGNMENT_LEFT))
        
    def OnUpdateAlignCenter(self, evt):
        evt.Check(self.rtc.IsSelectionAligned(rt.TEXT_ALIGNMENT_CENTRE))
        
    def OnUpdateAlignRight(self, evt):
        evt.Check(self.rtc.IsSelectionAligned(rt.TEXT_ALIGNMENT_RIGHT))

    
    def ForwardEvent(self, evt):
        # The RichTextCtrl can handle menu and update events for undo,
        # redo, cut, copy, paste, delete, and select all, so just
        # forward the event to it.
        self.rtc.ProcessEvent(evt)


    def MakeMenuBar(self):
        def doBind(item, handler, updateUI=None):
            self.Bind(wx.EVT_MENU, handler, item)
            if updateUI is not None:
                self.Bind(wx.EVT_UPDATE_UI, updateUI, item)
            
        fileMenu = wx.Menu()
        doBind( fileMenu.Append(-1, "&Open\tCtrl+O", "Open a file"),
                self.OnFileOpen )
        doBind( fileMenu.Append(-1, "&Save\tCtrl+S", "Save a file"),
                self.OnFileSave )
        doBind( fileMenu.Append(-1, "&Save As...\tF12", "Save to a new file"),
                self.OnFileSaveAs )
        fileMenu.AppendSeparator()
        doBind( fileMenu.Append(-1, "&View as HTML", "View HTML"),
                self.OnFileViewHTML)
        fileMenu.AppendSeparator()
        doBind( fileMenu.Append(-1, "E&xit\tCtrl+Q", "Quit this program"),
                self.OnFileExit )
        
        editMenu = wx.Menu()
        doBind( editMenu.Append(wx.ID_UNDO, "&Undo\tCtrl+Z"),
                self.ForwardEvent, self.ForwardEvent)
        doBind( editMenu.Append(wx.ID_REDO, "&Redo\tCtrl+Y"),
                self.ForwardEvent, self.ForwardEvent )
        editMenu.AppendSeparator()
        doBind( editMenu.Append(wx.ID_CUT, "Cu&t\tCtrl+X"),
                self.ForwardEvent, self.ForwardEvent )
        doBind( editMenu.Append(wx.ID_COPY, "&Copy\tCtrl+C"),
                self.ForwardEvent, self.ForwardEvent)
        doBind( editMenu.Append(wx.ID_PASTE, "&Paste\tCtrl+V"),
                self.ForwardEvent, self.ForwardEvent)
        doBind( editMenu.Append(wx.ID_CLEAR, "&Delete\tDel"),
                self.ForwardEvent, self.ForwardEvent)
        editMenu.AppendSeparator()
        doBind( editMenu.Append(wx.ID_SELECTALL, "Select A&ll\tCtrl+A"),
                self.ForwardEvent, self.ForwardEvent )
        
        #doBind( editMenu.AppendSeparator(),  )
        #doBind( editMenu.Append(-1, "&Find...\tCtrl+F"),  )
        #doBind( editMenu.Append(-1, "&Replace...\tCtrl+R"),  )

        formatMenu = wx.Menu()
        doBind( formatMenu.AppendCheckItem(-1, "&Bold\tCtrl+B"),
                self.OnBold, self.OnUpdateBold)
        doBind( formatMenu.AppendCheckItem(-1, "&Italic\tCtrl+I"),
                self.OnItalic, self.OnUpdateItalic)
        doBind( formatMenu.AppendCheckItem(-1, "&Underline\tCtrl+U"),
                self.OnUnderline, self.OnUpdateUnderline)
        formatMenu.AppendSeparator()
        doBind( formatMenu.AppendCheckItem(-1, "L&eft Align"),
                self.OnAlignLeft, self.OnUpdateAlignLeft)
        doBind( formatMenu.AppendCheckItem(-1, "&Centre"),
                self.OnAlignCenter, self.OnUpdateAlignCenter)
        doBind( formatMenu.AppendCheckItem(-1, "&Right Align"),
                self.OnAlignRight, self.OnUpdateAlignRight)
        formatMenu.AppendSeparator()
        doBind( formatMenu.Append(-1, "Indent &More"), self.OnIndentMore)
        doBind( formatMenu.Append(-1, "Indent &Less"), self.OnIndentLess)
        formatMenu.AppendSeparator()
        doBind( formatMenu.Append(-1, "Increase Paragraph &Spacing"), self.OnParagraphSpacingMore)
        doBind( formatMenu.Append(-1, "Decrease &Paragraph Spacing"), self.OnParagraphSpacingLess)
        formatMenu.AppendSeparator()
        doBind( formatMenu.Append(-1, "Normal Line Spacing"), self.OnLineSpacingSingle)
        doBind( formatMenu.Append(-1, "1.5 Line Spacing"), self.OnLineSpacingHalf)
        doBind( formatMenu.Append(-1, "Double Line Spacing"), self.OnLineSpacingDouble)
        formatMenu.AppendSeparator()
        doBind( formatMenu.Append(-1, "&Font..."), self.OnFont)
        
        mb = wx.MenuBar()
        mb.Append(fileMenu, "&File")
        mb.Append(editMenu, "&Edit")
        mb.Append(formatMenu, "F&ormat")
        self.SetMenuBar(mb)


    def MakeToolBar(self):
        def doBind(item, handler, updateUI=None):
            self.Bind(wx.EVT_TOOL, handler, item)
            if updateUI is not None:
                self.Bind(wx.EVT_UPDATE_UI, updateUI, item)
        
        tbar = self.CreateToolBar()
        doBind( tbar.AddTool(-1, images.get_rt_openBitmap(),
                            shortHelpString="Open"), self.OnFileOpen)
        doBind( tbar.AddTool(-1, images.get_rt_saveBitmap(),
                            shortHelpString="Save"), self.OnFileSave)
        tbar.AddSeparator()
        doBind( tbar.AddTool(wx.ID_CUT, images.get_rt_cutBitmap(),
                            shortHelpString="Cut"), self.ForwardEvent, self.ForwardEvent)
        doBind( tbar.AddTool(wx.ID_COPY, images.get_rt_copyBitmap(),
                            shortHelpString="Copy"), self.ForwardEvent, self.ForwardEvent)
        doBind( tbar.AddTool(wx.ID_PASTE, images.get_rt_pasteBitmap(),
                            shortHelpString="Paste"), self.ForwardEvent, self.ForwardEvent)
        tbar.AddSeparator()
        doBind( tbar.AddTool(wx.ID_UNDO, images.get_rt_undoBitmap(),
                            shortHelpString="Undo"), self.ForwardEvent, self.ForwardEvent)
        doBind( tbar.AddTool(wx.ID_REDO, images.get_rt_redoBitmap(),
                            shortHelpString="Redo"), self.ForwardEvent, self.ForwardEvent)
        tbar.AddSeparator()
        doBind( tbar.AddTool(-1, images.get_rt_boldBitmap(), isToggle=True,
                            shortHelpString="Bold"), self.OnBold, self.OnUpdateBold)
        doBind( tbar.AddTool(-1, images.get_rt_italicBitmap(), isToggle=True,
                            shortHelpString="Italic"), self.OnItalic, self.OnUpdateItalic)
        doBind( tbar.AddTool(-1, images.get_rt_underlineBitmap(), isToggle=True,
                            shortHelpString="Underline"), self.OnUnderline, self.OnUpdateUnderline)
        tbar.AddSeparator()
        doBind( tbar.AddTool(-1, images.get_rt_alignleftBitmap(), isToggle=True,
                            shortHelpString="Align Left"), self.OnAlignLeft, self.OnUpdateAlignLeft)
        doBind( tbar.AddTool(-1, images.get_rt_centreBitmap(), isToggle=True,
                            shortHelpString="Center"), self.OnAlignCenter, self.OnUpdateAlignCenter)
        doBind( tbar.AddTool(-1, images.get_rt_alignrightBitmap(), isToggle=True,
                            shortHelpString="Align Right"), self.OnAlignRight, self.OnUpdateAlignRight)
        tbar.AddSeparator()
        doBind( tbar.AddTool(-1, images.get_rt_indentlessBitmap(),
                            shortHelpString="Indent Less"), self.OnIndentLess)
        doBind( tbar.AddTool(-1, images.get_rt_indentmoreBitmap(),
                            shortHelpString="Indent More"), self.OnIndentMore)
        tbar.AddSeparator()
        doBind( tbar.AddTool(-1, images.get_rt_fontBitmap(),
                            shortHelpString="Font"), self.OnFont)
        doBind( tbar.AddTool(-1, images.get_rt_colourBitmap(),
                            shortHelpString="Font Colour"), self.OnColour)

        tbar.Realize()

       

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Show the RichTextCtrl sample", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        win = RichTextFrame(self, -1, "wx.richtext.RichTextCtrl",
                            size=(700, 500),
                            style = wx.DEFAULT_FRAME_STYLE)
        win.Show(True)

        # give easy access to PyShell if it's running
        self.rtfrm = win
        self.rtc = win.rtc

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wx.richtext.RichTextCtrl</center></h2>

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

