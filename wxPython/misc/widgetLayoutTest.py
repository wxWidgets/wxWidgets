"""
This test app is meant to help check if a widget has a good
DoGetBestSize method (in C++) by allowing the user to dynamically
create any non-toplevel widget which will be placed in a Sizer
designed to show how the widget will be sized naturally.
"""

import wx
import sys
import os

# stuff for debugging
print "wx.VERSION_STRING = ", wx.VERSION_STRING
print "pid:", os.getpid()
##raw_input("Press Enter...")

class LayoutTestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1, "Widget Layout Tester")

        p = wx.Panel(self)

        # Create control widgets
        self.testHistory = wx.ListBox(p, -1, size=(150, 300))
        self.moduleName = wx.TextCtrl(p, -1, "wx")
        self.className  = wx.TextCtrl(p, -1, "")
        self.parameters = wx.TextCtrl(p, -1, "")
        self.postCreate = wx.TextCtrl(p, -1, "", size=(1,75),
                                      style=wx.TE_MULTILINE|wx.TE_DONTWRAP)
        self.expression = wx.TextCtrl(p, -1, "", style=wx.TE_READONLY)
        self.docstring  = wx.TextCtrl(p, -1, "", size=(1,75),
                                      style=wx.TE_READONLY|wx.TE_MULTILINE|wx.TE_DONTWRAP)

        self.expression.SetBackgroundColour(
            wx.SystemSettings.GetColour(wx.SYS_COLOUR_INFOBK))
        self.docstring.SetBackgroundColour(
            wx.SystemSettings.GetColour(wx.SYS_COLOUR_INFOBK))


        addBtn = wx.Button(p, -1, "Add")
        remBtn = wx.Button(p, -1, "Remove")
        repBtn = wx.Button(p, -1, "Replace")
        createBtn = wx.Button(p, -1, " Create Widget ")
        createBtn.SetDefault()
        destroyBtn = wx.Button(p, -1, "Destroy Widget")
        clearBtn = wx.Button(p, -1, "Clear")

        self.createBtn = createBtn
        self.destroyBtn = destroyBtn

        bottomPanel = wx.Panel(p, style=wx.SUNKEN_BORDER, name="bottomPanel")
        bottomPanel.SetMinSize((640,240))
        bottomPanel.SetOwnBackgroundColour("light blue")

        self.testPanel = wx.Panel(bottomPanel, name="testPanel")
        self.testPanel.SetOwnBackgroundColour((205, 183, 181)) # mistyrose3
        self.testWidget = None

        self.infoPane = InfoPane(p)

        # setup event bindings
        self.Bind(wx.EVT_TEXT, self.OnUpdate, self.moduleName)
        self.Bind(wx.EVT_TEXT, self.OnUpdate, self.className)
        self.Bind(wx.EVT_TEXT, self.OnUpdate, self.parameters)
        self.Bind(wx.EVT_UPDATE_UI, self.OnEnableCreate, createBtn)
        self.Bind(wx.EVT_UPDATE_UI, self.OnEnableDestroy, destroyBtn)
        self.Bind(wx.EVT_BUTTON, self.OnCreateWidget, createBtn)
        self.Bind(wx.EVT_BUTTON, self.OnDestroyWidget, destroyBtn)
        self.Bind(wx.EVT_BUTTON, self.OnClear, clearBtn)

        self.Bind(wx.EVT_CLOSE, self.OnSaveHistory)

        self.Bind(wx.EVT_BUTTON, self.OnAddHistory, addBtn)
        self.Bind(wx.EVT_BUTTON, self.OnRemoveHistory, remBtn)
        self.Bind(wx.EVT_BUTTON, self.OnReplaceHistory, repBtn)
        self.Bind(wx.EVT_LISTBOX, self.OnHistorySelect, self.testHistory)
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.OnHistoryActivate, self.testHistory)

        if 'wxMac' in wx.PlatformInfo or 'wxGTK' in wx.PlatformInfo:
            self.testHistory.Bind(wx.EVT_KEY_DOWN, self.OnHistoryKey)
            

        # setup the layout
        mainSizer = wx.BoxSizer(wx.VERTICAL)
        topSizer  = wx.BoxSizer(wx.HORIZONTAL)
        ctlsSizer = wx.FlexGridSizer(2, 2, 5, 5)
        ctlsSizer.AddGrowableCol(1)
        btnSizer =  wx.BoxSizer(wx.HORIZONTAL)

        topSizer.Add(self.testHistory, 0, wx.RIGHT, 30)

        ctlsSizer.Add(wx.StaticText(p, -1, "Module name:"),
                      0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        mcSizer = wx.BoxSizer(wx.HORIZONTAL)
        mcSizer.Add(self.moduleName, 0, 0)
        mcSizer.Add(wx.StaticText(p, -1, "Class name:"),
                      0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL |wx.LEFT|wx.RIGHT, 10)
        mcSizer.Add(self.className, 1, 0)
        ctlsSizer.Add(mcSizer, 0, wx.EXPAND)

        ctlsSizer.Add(wx.StaticText(p, -1, "Parameters:"),
                      0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        ctlsSizer.Add(self.parameters, 0, wx.EXPAND)
        ctlsSizer.Add(wx.StaticText(p, -1, "Create Expr:"),
                      0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        ctlsSizer.Add(self.expression, 0, wx.EXPAND)
        ctlsSizer.Add(wx.StaticText(p, -1, "Post create:"), 0, wx.ALIGN_RIGHT)
        ctlsSizer.Add(self.postCreate, 0, wx.EXPAND)
        ctlsSizer.Add(wx.StaticText(p, -1, "DocString:"), 0, wx.ALIGN_RIGHT)
        ctlsSizer.Add(self.docstring, 0, wx.EXPAND)
        ctlsSizer.AddGrowableRow(4)
        topSizer.Add(ctlsSizer, 1, wx.EXPAND)
  
        btnSizer.Add((5,5))
        btnSizer.Add(addBtn, 0, wx.RIGHT, 5)
        btnSizer.Add(remBtn, 0, wx.RIGHT, 5)
        btnSizer.Add(repBtn, 0, wx.RIGHT, 5)
        btnSizer.Add((0,0), 1)
        btnSizer.Add(createBtn, 0, wx.RIGHT, 5)
        btnSizer.Add(destroyBtn, 0, wx.RIGHT, 5)
        btnSizer.Add(clearBtn, 0, wx.RIGHT, 5)
        btnSizer.Add((0,0), 1)

        mainSizer.Add(topSizer, 0, wx.EXPAND|wx.ALL, 10)
        mainSizer.Add(btnSizer, 0, wx.EXPAND)
        mainSizer.Add((10,10))
        ##mainSizer.Add(wx.StaticLine(p, -1), 0, wx.EXPAND)
        mainSizer.Add(bottomPanel, 1, wx.EXPAND)

        mainSizer.Add(self.infoPane, 0, wx.EXPAND)

        self.bottomSizer = sz = wx.BoxSizer(wx.VERTICAL)
        sz.Add((0,0), 1)
        sz.Add(self.testPanel, 0, wx.ALIGN_CENTER)
        sz.Add((0,0), 1)
        bottomPanel.SetSizer(sz)

        p.SetSizerAndFit(mainSizer)
        self.Fit()

        self.PopulateHistory()




    def PopulateHistory(self):
        """
        Load and eval a list of lists from a file, load the contents
        into self.testHistory
        """
        fname = os.path.join(os.path.dirname(sys.argv[0]),
                             'widgetLayoutTest.cfg')
        try:
            self.history = eval(open(fname).read())
        except:
            self.history = []

        self.testHistory.Clear()

        for idx in range(len(self.history)):
            item = self.history[idx]
            # check if it is too short
            while len(item) < 4:
                item.append('')

            # add it to the listbox
            self.testHistory.Append(item[0] + '.' + item[1])

        self.needSaved = False


    def OnSaveHistory(self, evt):
        if self.needSaved:
            fname = os.path.join(os.path.dirname(sys.argv[0]),
                                 'widgetLayoutTest.cfg')
            f = open(fname, 'wb')
            f.write('[\n')
            for item in self.history:
                f.write(str(item) + ',\n')
            f.write(']\n')
            f.close()
        evt.Skip()


    def OnAddHistory(self, evt):
        moduleName = self.moduleName.GetValue()
        className  = self.className.GetValue()
        parameters = self.parameters.GetValue()
        postCreate = self.postCreate.GetValue()

        item = [str(moduleName), str(className), str(parameters), str(postCreate)]
        self.history.append(item)
        self.testHistory.Append(item[0] + '.' + item[1])

        self.testHistory.SetSelection(len(self.history)-1)
        self.needSaved = True


    def OnRemoveHistory(self, evt):
        idx = self.testHistory.GetSelection()
        if idx != wx.NOT_FOUND:
            del self.history[idx]
            self.testHistory.Delete(idx)
        self.needSaved = True
        self.OnClear(None)


    def OnReplaceHistory(self, evt):
        idx = self.testHistory.GetSelection()
        if idx != wx.NOT_FOUND:
            moduleName = self.moduleName.GetValue()
            className  = self.className.GetValue()
            parameters = self.parameters.GetValue()
            postCreate = self.postCreate.GetValue()

            item = [str(moduleName), str(className), str(parameters), str(postCreate)]
            self.history[idx] = item
            self.testHistory.SetString(idx, item[0] + '.' + item[1])
        self.needSaved = True


    def OnHistorySelect(self, evt):
        #idx = self.testHistory.GetSelection()
        idx = evt.GetInt()
        if idx != wx.NOT_FOUND:
            item = self.history[idx]
            self.moduleName.SetValue(item[0])
            self.className.SetValue(item[1])
            self.parameters.SetValue(item[2])
            self.postCreate.SetValue(item[3])
            if 'wxMac' in wx.PlatformInfo:
                self.OnUpdate(None)


    def OnHistoryActivate(self, evt):
        btn = self.testHistory.GetParent().GetDefaultItem()
        if btn is not None:
            e = wx.CommandEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED, btn.GetId())
            btn.Command(e)


    def OnHistoryKey(self, evt):
        key = evt.GetKeyCode()
        if key == wx.WXK_RETURN:
            self.OnHistoryActivate(None)
        else:
            evt.Skip()


    def OnUpdate(self, evt):
        # get the details from the form
        moduleName = self.moduleName.GetValue()
        className  = self.className.GetValue()
        parameters = self.parameters.GetValue()

        expr = "w = %s.%s( testPanel, %s )" % (moduleName, className, parameters)
        self.expression.SetValue(expr)

        docstring = None
        try:
            docstring = eval("%s.%s.__init__.__doc__" % (moduleName, className))
        except:
            pass
        if docstring is not None:
            self.docstring.SetValue(docstring)
        else:
            self.docstring.SetValue("")

    def OnEnableDestroy(self, evt):
        evt.Enable(self.testWidget is not None)

    def OnEnableCreate(self, evt):
        evt.Enable(self.testWidget is None)


    def OnCreateWidget(self, evt):
        if self.testWidget is not None:
            return

        testPanel = self.testPanel

        # get the details from the form
        moduleName = self.moduleName.GetValue()
        className  = self.className.GetValue()
        parameters = self.parameters.GetValue()
        expr       = self.expression.GetValue()[4:]
        postCreate = self.postCreate.GetValue()
        if 'wxMac' in wx.PlatformInfo:
            postCreate = postCreate.replace('\r', '\n')

        # make sure the module is imported already
        if not sys.modules.has_key(moduleName):
            try:
                m = __import__(moduleName)
            except importError:
                wx.MessageBox("Unable to import module!", "Error")
                return

        # create the widget
        try:
            w = eval(expr)
        except Exception, e:
            wx.MessageBox("Got a '%s' Exception!" % e.__class__.__name__, "Error")
            import traceback
            traceback.print_exc()
            return

        # Is there postCreate code?
        if postCreate:
            ns = {}
            ns.update(globals())
            ns.update(locals())
            try:
                exec postCreate in ns
            except Exception, e:
                wx.MessageBox("Got a '%s' Exception!" % e.__class__.__name__, "Error")
                import traceback
                traceback.print_exc()
                w.Destroy()
                return

        # Put the widget in a sizer and the sizer in the testPanel
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(w, 0, wx.ALL, 5)
        self.testPanel.SetSizer(sizer)
        self.testWidget = w
        self.bottomSizer.Layout()

        # make the destroy button be default now
        self.destroyBtn.SetDefault()

        self.infoPane.Update(w, testPanel)
        

    def OnDestroyWidget(self, evt):
        self.testWidget.Destroy()
        self.testWidget = None
        self.testPanel.SetSizer(None, True)
        self.testPanel.Refresh()

        # ensure the panel shrinks again now that it has no sizer
        self.testPanel.SetMinSize((20,20))
        self.bottomSizer.Layout()
        self.testPanel.SetMinSize(wx.DefaultSize)

        # make the create button be default now
        self.createBtn.SetDefault()

        self.infoPane.Clear()
        

    def OnClear(self, evt):
        self.moduleName.SetValue("")
        self.className.SetValue("")
        self.parameters.SetValue("")
        self.expression.SetValue("")
        self.docstring.SetValue("")
        self.postCreate.SetValue("")






class InfoPane(wx.Panel):
    """
    This class is used to display details of various properties of the
    widget and the testPanel to aid with debugging.
    """
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)

        # create subwidgets
        self.wPane = SizeInfoPane(self, "Widget")
        self.tpPane= SizeInfoPane(self, "testPanel")
        self.cPane = ColourInfoPanel(self, "Widget colours")

        # Setup the layout
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(self.wPane, 0, wx.EXPAND|wx.ALL, 5)
        sizer.Add(self.tpPane, 0, wx.EXPAND|wx.ALL, 5)
        sizer.Add(self.cPane, 0, wx.EXPAND|wx.ALL, 5)

        self.SetSizer(sizer)

        
    def Update(self, w, tp):
        self.wPane.Update(w)
        self.tpPane.Update(tp)
        self.cPane.Update(w)
        
    def Clear(self):
        self.wPane.Clear()
        self.tpPane.Clear()
        self.cPane.Clear()



class SizeInfoPane(wx.Panel):
    """
    A component of the InfoPane that shows vaious window size attributes.
    """
    def __init__(self, parent, label):
        wx.Panel.__init__(self, parent)

        # create subwidgets
        sb = wx.StaticBox(self, -1, label)
        self._size        = wx.TextCtrl(self, -1, "", style=wx.TE_READONLY)
        self._minsize     = wx.TextCtrl(self, -1, "", style=wx.TE_READONLY)
        self._bestsize    = wx.TextCtrl(self, -1, "", style=wx.TE_READONLY)
        self._adjbstsize  = wx.TextCtrl(self, -1, "", style=wx.TE_READONLY)
        self._bestfit     = wx.TextCtrl(self, -1, "", style=wx.TE_READONLY)

        # setup the layout
        fgs = wx.FlexGridSizer(2, 2, 5, 5)
        fgs.AddGrowableCol(1)

        fgs.Add(wx.StaticText(self, -1, "Size:"),
                0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        fgs.Add(self._size, 0, wx.EXPAND)

        fgs.Add(wx.StaticText(self, -1, "MinSize:"),
                0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        fgs.Add(self._minsize, 0, wx.EXPAND)

        fgs.Add(wx.StaticText(self, -1, "BestSize:"),
                0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        fgs.Add(self._bestsize, 0, wx.EXPAND)

        fgs.Add(wx.StaticText(self, -1, "AdjustedBestSize:"),
                0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        fgs.Add(self._adjbstsize, 0, wx.EXPAND)

        fgs.Add(wx.StaticText(self, -1, "BestFittingSize:"),
                0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        fgs.Add(self._bestfit, 0, wx.EXPAND)

        sbs = wx.StaticBoxSizer(sb, wx.VERTICAL)
        sbs.Add(fgs, 0, wx.EXPAND|wx.ALL, 4)

        self.SetSizer(sbs)


    def Update(self, win):
        self._size.SetValue(       str(win.GetSize()) )
        self._minsize.SetValue(    str(win.GetMinSize()) )
        self._bestsize.SetValue(   str(win.GetBestSize()) )
        self._adjbstsize.SetValue( str(win.GetAdjustedBestSize()) )
        self._bestfit.SetValue(    str(win.GetEffectiveMinSize()) )

    def Clear(self):
        self._size.SetValue("")
        self._minsize.SetValue("")
        self._bestsize.SetValue("")
        self._adjbstsize.SetValue("")
        self._bestfit.SetValue("")



class ColourInfoPanel(wx.Panel):
    """
    A component of the InfoPane that shows fg and bg colour attributes.    
    """
    def __init__(self, parent, label):
        wx.Panel.__init__(self, parent)

        # create subwidgets
        sb = wx.StaticBox(self, -1, label)
        self._fgtxt = wx.TextCtrl(self, -1, "", style=wx.TE_READONLY)
        self._fgclr = wx.Panel(self, style=wx.SIMPLE_BORDER)
        self._fgclr.SetMinSize((20,20))
        self._bgtxt = wx.TextCtrl(self, -1, "", style=wx.TE_READONLY)
        self._bgclr = wx.Panel(self, style=wx.SIMPLE_BORDER)
        self._bgclr.SetMinSize((20,20))

        # setup the layout
        fgs = wx.FlexGridSizer(2, 3, 5, 5)
        fgs.AddGrowableCol(1)

        fgs.Add(wx.StaticText(self, -1, "FG colour:"),
                0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        fgs.Add(self._fgtxt, 0, wx.EXPAND)
        fgs.Add(self._fgclr)

        fgs.Add(wx.StaticText(self, -1, "BG colour:"),
                0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        fgs.Add(self._bgtxt, 0, wx.EXPAND)
        fgs.Add(self._bgclr)

        sbs = wx.StaticBoxSizer(sb, wx.VERTICAL)
        sbs.Add(fgs, 0, wx.EXPAND|wx.ALL, 4)

        self.SetSizer(sbs)


    def Update(self, win):
        def clr2hex(c, cp):
            cp.SetBackgroundColour(c)
            cp.Refresh()
            return "#%02X%02X%02X" % c.Get()
        
        self._fgtxt.SetValue( clr2hex(win.GetForegroundColour(), self._fgclr) )
        self._bgtxt.SetValue( clr2hex(win.GetBackgroundColour(), self._bgclr) )


    def Clear(self):
        self._fgtxt.SetValue("")
        self._bgtxt.SetValue("")
        self._fgclr.SetBackgroundColour(self.GetBackgroundColour())
        self._bgclr.SetBackgroundColour(self.GetBackgroundColour())
        self._fgclr.Refresh()
        self._bgclr.Refresh()


        

app = wx.PySimpleApp(redirect=False)
frame = LayoutTestFrame()
app.SetTopWindow(frame)
frame.Show()
app.MainLoop()

