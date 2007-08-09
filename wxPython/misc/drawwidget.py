

import wx

testItems = [
    "", 
    "BitmapButton",
    "Button",
    "CalendarCtrl",
    "CheckBox",
    "CheckListBox",
    "Choice",
    "ComboBox",
    "Gauge",
    "GenericDirCtrl",
    "ListBox",
    "ListCtrl",
    "ListCtrl With GridLines",
    "RadioBox",
    "RadioButton",
    "ScrollBar",
    "Slider",
    "SpinButton",
    "SpinCtrl",
    "StaticBitmap",
    "StaticBox",
    "StaticLine",
    "StaticText",
    "TextCtrl",
    "ToggleButton",
    "TreeCtrl",
    "--------------",
    "Panel",
    "Panel With Border",
    "Panel With BG",
    "Panel With Controls",
    "Panel With RadioBox",
    "--------------",
    "GenericButton",
    
    ]

import keyword
testChoices = keyword.kwlist

testChoices2 = "one two three four five six seven eight nine".split()




class Frame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="Draw Widget Test")
        self.left = wx.Panel(self)
        self.right = DisplayPanel(self)
        self.widget = None

        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(self.left, 1, wx.EXPAND)
        sizer.Add(self.right, 1, wx.EXPAND)
        self.SetSizer(sizer)
        
        menu = wx.Menu()
        menu.Append(wx.ID_EXIT, "E&xit\tAlt-X")
        self.Bind(wx.EVT_MENU, self.OnExit, id=wx.ID_EXIT)
        mbar = wx.MenuBar()
        mbar.Append(menu, "&File")
        self.SetMenuBar(mbar)

        cb = wx.ComboBox(self.left, -1, pos=(20,20),
                         choices=testItems, style=wx.CB_READONLY)
        self.Bind(wx.EVT_COMBOBOX, self.OnWidgetChosen, cb)


    def OnWidgetChosen(self, evt):
        item = evt.GetString()
        item = item.replace(" ", "_")
        func = getattr(self, "Test"+item, None)
        if func is not None:
            func(self.left)
                    
    def OnExit(self, evt):
        self.Close()



    def DoWidget(self, widget):
        self.right.Empty()
        if self.widget is not None:
            self.widget.Destroy()
        self.widget = widget
        if widget is None:
            return

        visiblePos = (20,80)
        hiddenPos = (-1000,-1000)
        
        widget.SetPosition(visiblePos)
        widget.Update()
        self.GetBMP(0)  # Uses just a DC.Blit, so it must be visible

        # the rest should work when the widget is not visible.
        widget.SetPosition(hiddenPos)
        widget.Update()
        self.Update()
        
        ##self.GetBMP(1)
        self.GetBMP(2)
        ##self.GetBMP(4)

        # make it visible again for the user to compare
        widget.SetPosition(visiblePos)


    def GetBMP(self, method):
        w = self.widget
        maskClr = wx.Colour(12, 34, 56)
        sz = w.GetSize()
        bmp = wx.EmptyBitmap(sz.width, sz.height)
        dc = wx.MemoryDC()
        dc.SelectObject(bmp)
        dc.SetBackground(wx.Brush(maskClr))
        dc.Clear()

        if method == 0:
            wdc = wx.WindowDC(w)
            dc.Blit(0,0, sz.width, sz.height, wdc, 0, 0)
        else:            
            ##wx.DrawWindowOnDC(w, dc, method)
            wx.DrawWindowOnDC(w, dc)
            
        dc.SelectObject(wx.NullBitmap)
        bmp.SetMaskColour(maskClr)
        self.right.SetBMP(bmp, method)



        

    def Test(self, p):
        self.DoWidget(None)

    def TestBitmapButton(self, p):
        self.DoWidget(wx.BitmapButton(p, -1,
                                      wx.Bitmap("image.png")))
    def TestButton(self, p):
        self.DoWidget(wx.Button(p, -1, "A button"))

    def TestCalendarCtrl(self, p):
        import wx.calendar
        w = wx.calendar.CalendarCtrl(p, style=wx.calendar.CAL_SEQUENTIAL_MONTH_SELECTION)
        self.DoWidget(w)
        
    def TestCheckBox(self, p):
        self.DoWidget(wx.CheckBox(p, -1, "checkbox"))

    def TestCheckListBox(self, p):
        w = wx.CheckListBox(p, -1, choices=testChoices)
        w.SetSelection(2)        
        self.DoWidget(w)

    def TestChoice(self, p):
        w = wx.Choice(p, -1, choices=testChoices)
        w.SetSelection(2)
        self.DoWidget(w)

    def TestComboBox(self, p):
        w = wx.ComboBox(p, -1, choices=testChoices)
        w.SetSelection(2)
        self.DoWidget(w)
    
    def TestGauge(self, p):
        w = wx.Gauge(p, -1, 100, size=(150, -1))
        w.SetValue(65)
        self.DoWidget(w)
    
    def TestGenericDirCtrl(self, p):
        w = wx.GenericDirCtrl(p, size=(150,200), style=wx.DIRCTRL_DIR_ONLY)
        self.DoWidget(w)
    
    def TestListBox(self, p):
        w = wx.ListBox(p, -1, choices=testChoices)
        w.SetSelection(2)        
        self.DoWidget(w)
    
    def TestListCtrl(self, p, useGridLines=False):
        style=wx.LC_REPORT
        if useGridLines:
            style = style | wx.LC_HRULES | wx.LC_VRULES
        w = wx.ListCtrl(p, -1, size=(250, 100), style=style)
        w.InsertColumn(0, "Col 1")
        w.InsertColumn(1, "Col 2")
        w.InsertColumn(2, "Col 3")
        for x in range(10):
            w.InsertStringItem(x, str(x))
            w.SetStringItem(x, 1, str(x))
            w.SetStringItem(x, 2, str(x))
        self.DoWidget(w)

    def TestListCtrl_With_GridLines(self, p):
        self.TestListCtrl(p, True)
            
    def TestRadioBox(self, p):
        w = wx.RadioBox(p, -1, "RadioBox",
                        choices=testChoices2, majorDimension=3)
        self.DoWidget(w)
    
    def TestRadioButton(self, p):
        self.DoWidget(wx.RadioButton(p, -1, "RadioButton"))
    
    def TestScrollBar(self, p):
        w = wx.ScrollBar(p, -1, size=(150,-1))
        w.SetScrollbar(25, 5, 100, 10)
        self.DoWidget(w)
    
    def TestSlider(self, p):
        w = wx.Slider(p, -1, size=(150,-1))
        self.DoWidget(w)
    
    def TestSpinButton(self, p):
        w = wx.SpinButton(p, -1)
        self.DoWidget(w)
    
    def TestSpinCtrl(self, p):
        w = wx.SpinCtrl(p, -1)
        self.DoWidget(w)
    
    def TestStaticBitmap(self, p):
        w = wx.StaticBitmap(p, -1, wx.Bitmap("image.png"))
        self.DoWidget(w)
    
    def TestStaticBox(self, p):
        w = wx.StaticBox(p, -1, "StaticBox", size=(150,75))
        self.DoWidget(w)
    
    def TestStaticLine(self, p):
        w = wx.StaticLine(p, -1, size=(150,-1))
        self.DoWidget(w)
    
    def TestStaticText(self, p):
        w = wx.StaticText(p, -1, "This is a wx.StaticText")
        self.DoWidget(w)
    
    def TestTextCtrl(self, p):
        self.DoWidget(wx.TextCtrl(p, -1, "This is a TextCtrl", size=(150,-1)))

    def TestToggleButton(self, p):
        w = wx.ToggleButton(p, -1, "Toggle Button")
        self.DoWidget(w)
    
    def TestTreeCtrl(self, p):
        w = wx.TreeCtrl(p, -1, size=(150,200))
        root = w.AddRoot("The Root Item")
        for x in range(15):
            child = w.AppendItem(root, "Item %d" % x)
        w.Expand(root)
        self.DoWidget(w)
    
    def TestPanel(self, p):
        w = wx.Panel(p, size=(100,100))
        self.DoWidget(w)
    
    def TestPanel_With_Border(self, p):
        w = wx.Panel(p, size=(100,100), style=wx.SUNKEN_BORDER)
        self.DoWidget(w)

    def TestPanel_With_BG(self, p):
        w = wx.Panel(p, size=(100,100), style=wx.SUNKEN_BORDER)
        w.SetBackgroundColour("pink")
        self.DoWidget(w)

    def TestPanel_With_Controls(self, p):
        w = wx.Panel(p, size=(100,100), style=wx.SUNKEN_BORDER)
        l1 = wx.StaticText(w, -1, "Name:")
        l2 = wx.StaticText(w, -1, "Email:")
        t1 = wx.TextCtrl(w, -1, "", size=(120,-1))
        t2 = wx.TextCtrl(w, -1, "", size=(120,-1))
        btn = wx.Button(w, wx.ID_OK)
        sizer = wx.BoxSizer(wx.VERTICAL)
        fgs = wx.FlexGridSizer(2,2,5,5)
        fgs.Add(l1)
        fgs.Add(t1)
        fgs.Add(l2)
        fgs.Add(t2)
        sizer.Add(fgs, 0, wx.ALL, 10)
        sizer.Add(btn, 0, wx.ALL, 10)
        w.SetSizerAndFit(sizer)
        self.DoWidget(w)

    def TestPanel_With_RadioBox(self, p):
        w = wx.Panel(p, size=(100,100), style=wx.SUNKEN_BORDER)
        wx.RadioBox(w, -1, "RadioBox", pos=(10,10),
                    choices=testChoices2, majorDimension=3)
        w.Fit()
        self.DoWidget(w)

    def TestGenericButton(self, p):
        import wx.lib.buttons as b
        w = b.GenButton(p, -1, "Generic Button")
        w.SetFont(wx.Font(20, wx.SWISS, wx.NORMAL, wx.BOLD, False))
        w.SetBezelWidth(5)
        w.SetInitialSize()
        w.SetBackgroundColour("Navy")
        w.SetForegroundColour(wx.WHITE)
        self.DoWidget(w)
    
    def TestT(self, p):
        self.DoWidget(w)
    
    def TestT(self, p):
        self.DoWidget(w)
    
    def TestT(self, p):
        self.DoWidget(w)
    
    def TestT(self, p):
        self.DoWidget(w)
    






class DisplayPanel(wx.Panel):
    def __init__(self, parent, ID=-1):
        wx.Panel.__init__(self, parent, ID)
        self.SetBackgroundColour("sky blue")
        self.Empty()
        self.Bind(wx.EVT_PAINT,            self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OEB)

    def Empty(self):
        self.bmps = [None] * 5
        self.Refresh()

    def SetBMP(self, bmp, method):
        self.bmps[method] = bmp
        self.Refresh()

    def OEB(self, evt):
        None

    def OnPaint(self, evt):
        dc = wx.BufferedPaintDC(self)
        dc.SetBackground(wx.Brush(self.GetBackgroundColour()))
        dc.Clear()
        y = 25
        for idx, bmp in enumerate(self.bmps):
            if bmp is not None:
                dc.DrawText(str(idx), 15, y)
                dc.DrawBitmap(bmp, 30,y, True)
                y += bmp.GetHeight() + 15



app = wx.App(False)
f = Frame()
f.SetSize((600,600))
f.Show()
app.MainLoop()
