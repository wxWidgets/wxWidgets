
import wx
import wx.lib.buttonpanel as bp
import images

#----------------------------------------------------------------------
class ButtonPanelDemo(wx.Frame):

    def __init__(self, parent, id=wx.ID_ANY, title="ButtonPanel wxPython Demo", *args, **kw):
        
        wx.Frame.__init__(self, parent, id, title, *args, **kw)

        self.CreateMenuBar()

        self.statusbar = self.CreateStatusBar(2, wx.ST_SIZEGRIP)
        self.statusbar.SetStatusWidths([-2, -1])
        # statusbar fields
        statusbar_fields = [("ButtonPanel wxPython Demo, Andrea Gavana @ 02 Oct 2006"),
                            ("Welcome To wxPython!")]

        for i in range(len(statusbar_fields)):
            self.statusbar.SetStatusText(statusbar_fields[i], i)
        
        mainPanel = wx.Panel(self, -1)
        self.logtext = wx.TextCtrl(mainPanel, -1, "", size=(-1, 250),
                                   style=wx.TE_MULTILINE|wx.TE_READONLY|wx.TE_RICH2)
        
        vSizer = wx.BoxSizer(wx.VERTICAL) 
        mainPanel.SetSizer(vSizer) 

        # Create the buttons and place them on the 
        # tab area of the main book 
        alignment = bp.BP_ALIGN_RIGHT
        self.titleBar = bp.ButtonPanel(mainPanel, -1, "A Simple Test & Demo",
                                       alignment=alignment)

        self.btn1bmp = images.get_bp_btn1Bitmap()
        self.btn2bmp = images.get_bp_btn2Bitmap()
        self.btn3bmp = images.get_bp_btn3Bitmap()
        self.btn4bmp = images.get_bp_btn4Bitmap()
        self.CreateButtons(alignment)

        # set the color the text is drawn with
        self.titleBar.SetColor(bp.BP_TEXT_COLOR, wx.WHITE)

        # These default to white and whatever is set in the system
        # settings for the wx.SYS_COLOUR_ACTIVECAPTION.  We'll use
        # some specific settings to ensure a consistent look for the
        # demo.
        self.titleBar.SetColor(bp.BP_CAPTION_BORDER_COLOR,   wx.Colour(120,23,224))
        self.titleBar.SetColor(bp.BP_CAPTION_COLOR,          wx.Colour(60,11,112))
        self.titleBar.SetColor(bp.BP_CAPTION_GRADIENT_COLOR, wx.Colour(120,23,224))
        
        vSizer.Add(self.titleBar, 0, wx.EXPAND)
        vSizer.Add(self.logtext, 1, wx.EXPAND|wx.ALL, 5)
        vSizer.Layout()

        sizer = wx.BoxSizer()
        sizer.Add(mainPanel, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Fit()
        
        
    def CreateMenuBar(self):
        mb = wx.MenuBar()
        
        file_menu = wx.Menu()
        item = wx.MenuItem(file_menu, wx.ID_ANY, "&Quit")
        file_menu.AppendItem(item)
        self.Bind(wx.EVT_MENU, self.OnClose, item)

        edit_menu = wx.Menu()
        item = wx.MenuItem(edit_menu, wx.ID_ANY, "BP_ALIGN_RIGHT", kind=wx.ITEM_RADIO)
        edit_menu.AppendItem(item)
        self.Bind(wx.EVT_MENU, self.OnAlignRight, item)
        item = wx.MenuItem(edit_menu, wx.ID_ANY, "BP_ALIGN_LEFT", kind=wx.ITEM_RADIO)
        edit_menu.AppendItem(item)
        self.Bind(wx.EVT_MENU, self.OnAlignLeft, item)

        help_menu = wx.Menu()
        item = wx.MenuItem(help_menu, wx.ID_ANY, "&About...")
        help_menu.AppendItem(item)
        self.Bind(wx.EVT_MENU, self.OnAbout, item)
      
        mb.Append(file_menu, "&File")
        mb.Append(edit_menu, "&Edit")
        mb.Append(help_menu, "&Help")
        
        self.SetMenuBar(mb)


    def CreateButtons(self, alignment=bp.BP_ALIGN_RIGHT):
        self.buttons = []
        self.Freeze()
        
        self.titleBar.RemoveAllButtons()

        bmps = [ self.btn1bmp,
                 self.btn2bmp,
                 self.btn3bmp,
                 self.btn4bmp,
                 ]
        if alignment == bp.BP_ALIGN_LEFT:
            bmps.reverse()

        for bmp in bmps:
            btn = bp.ButtonInfo(wx.NewId(), bmp)
            self.titleBar.AddButton(btn)
            self.Bind(wx.EVT_BUTTON, self.OnButton, btn)
            self.buttons.append(btn.GetId())

        self.strings = ["First", "Second", "Third", "Fourth"]
        if alignment == bp.BP_ALIGN_RIGHT:
            self.strings.reverse()

        self.titleBar.SetAlignment(alignment)        
        self.Thaw()

        
    def OnAlignLeft(self, event):
        self.CreateButtons(alignment=bp.BP_ALIGN_LEFT)
        event.Skip()
            

    def OnAlignRight(self, event):
        self.CreateButtons(alignment=bp.BP_ALIGN_RIGHT)
        event.Skip()


    def OnButton(self, event):
        btn = event.GetId()
        indx = self.buttons.index(btn)
        self.logtext.AppendText("Event Fired From " + self.strings[indx] + " Button\n")
        event.Skip()


    def OnClose(self, event):
        self.Destroy()
        event.Skip()        


    def OnAbout(self, event):

        msg = "This Is The About Dialog Of The ButtonPanel Demo.\n\n" + \
              "Author: Andrea Gavana @ 02 Oct 2006\n\n" + \
              "Please Report Any Bug/Requests Of Improvements\n" + \
              "To Me At The Following Adresses:\n\n" + \
              "andrea.gavana@gmail.com\n" + "gavana@kpo.kz\n\n" + \
              "Based On Eran C++ Implementation (wxWidgets Forum).\n\n" + \
              "Welcome To wxPython " + wx.VERSION_STRING + "!!"
              
        dlg = wx.MessageDialog(self, msg, "ButtonPanel wxPython Demo",
                               wx.OK | wx.ICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()        

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, " Test ButtonPanel ", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        self.win = ButtonPanelDemo(self)
        self.win.Show(True)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = bp.__doc__



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])


  
    
