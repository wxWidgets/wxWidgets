"""
This test app is meant to help check if a widget has a good
DoGetBestSize method (in C++) by allowing the user to dynamically
create any non-toplevel widget which will be placed in a Sizer
designed to show how the widget will be sized naturally.
"""

import wx
import sys
import os


class LayoutTestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1, "Widget Layout Tester")

        p = wx.Panel(self)

        # Create control widgets
        self.testHistory = wx.ListBox(p, -1, size=(150, 250))
        self.moduleName = wx.TextCtrl(p, -1, "wx")
        self.className  = wx.TextCtrl(p, -1, "")
        self.parameters = wx.TextCtrl(p, -1, "")
        self.expression = wx.TextCtrl(p, -1, "", style=wx.TE_READONLY)
        self.docstring  = wx.TextCtrl(p, -1, "", size=(1,125),
                                      style=wx.TE_READONLY|wx.TE_MULTILINE|wx.TE_DONTWRAP)
                                      

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
        bottomPanel.SetSizeHints((640,240))
        bottomPanel.SetDefaultBackgroundColour("light blue")

        self.testPanel = wx.Panel(bottomPanel, name="testPanel")
        self.testPanel.SetDefaultBackgroundColour((205, 183, 181)) # mistyrose3
        self.testWidget = None


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
        

        # setup the layout
        mainSizer = wx.BoxSizer(wx.VERTICAL)
        topSizer  = wx.BoxSizer(wx.HORIZONTAL)
        ctlsSizer = wx.FlexGridSizer(2, 2, 5, 5)
        ctlsSizer.AddGrowableCol(1)
        btnSizer =  wx.BoxSizer(wx.HORIZONTAL)
        
        topSizer.Add(self.testHistory, 0, wx.RIGHT, 30)

        ctlsSizer.Add((1,25))
        ctlsSizer.Add((1,25))
        ctlsSizer.Add(wx.StaticText(p, -1, "Module name:"),
                      0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        ctlsSizer.Add(self.moduleName, 0, wx.EXPAND)
        ctlsSizer.Add(wx.StaticText(p, -1, "Class name:"),
                      0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        ctlsSizer.Add(self.className, 0, wx.EXPAND)
        ctlsSizer.Add(wx.StaticText(p, -1, "Parameters:"),
                      0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        ctlsSizer.Add(self.parameters, 0, wx.EXPAND)
        ctlsSizer.Add(wx.StaticText(p, -1, "Expression:"),
                      0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        ctlsSizer.Add(self.expression, 0, wx.EXPAND)
        ctlsSizer.Add(wx.StaticText(p, -1, "DocString:"), 0, wx.ALIGN_RIGHT)
        ctlsSizer.Add(self.docstring, 0, wx.EXPAND)
        topSizer.Add(ctlsSizer, 1)

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
            while len(item) < 3:
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

        item = [str(moduleName), str(className), str(parameters)]
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


    def OnReplaceHistory(self, evt):
        idx = self.testHistory.GetSelection()
        if idx != wx.NOT_FOUND:
            moduleName = self.moduleName.GetValue()
            className  = self.className.GetValue()
            parameters = self.parameters.GetValue()

            item = [str(moduleName), str(className), str(parameters)]
            self.history[idx] = item
            self.testHistory.SetString(idx, item[0] + '.' + item[1])
        self.needSaved = True


    def OnHistorySelect(self, evt):
        idx = self.testHistory.GetSelection()
        if idx != wx.NOT_FOUND:
            item = self.history[idx]
            self.moduleName.SetValue(item[0])
            self.className.SetValue(item[1])
            self.parameters.SetValue(item[2])


    def OnHistoryActivate(self, evt):
        btn = self.testHistory.GetParent().GetDefaultItem()
        if btn is not None:
            e = wx.CommandEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED, btn.GetId())
            btn.Command(e)
            


    def OnUpdate(self, evt):
        # get the details from the form
        moduleName = self.moduleName.GetValue()
        className  = self.className.GetValue()
        parameters = self.parameters.GetValue()
        
        expr = "%s.%s(self.testPanel, %s)" % (moduleName, className, parameters)
        self.expression.SetValue(expr)

        docstring = ""
        try:
            docstring = eval("%s.%s.__init__.__doc__" % (moduleName, className))
        except:
            pass
        self.docstring.SetValue(docstring)
        

    def OnEnableDestroy(self, evt):
        evt.Enable(self.testWidget is not None)

    def OnEnableCreate(self, evt):
        evt.Enable(self.testWidget is None)


    def OnCreateWidget(self, evt):
        if self.testWidget is not None:
            return
        
        # get the details from the form
        moduleName = self.moduleName.GetValue()
        className  = self.className.GetValue()
        parameters = self.parameters.GetValue()
        expr       = self.expression.GetValue()

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

        # Put the widget in a sizer and the sizer in the testPanel
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(w, 0, wx.ALL, 5)
        self.testPanel.SetSizer(sizer)
        self.testWidget = w
        self.bottomSizer.Layout()

        # make the destroy button be default now
        self.destroyBtn.SetDefault()

        if False:
            print 'w size', w.GetSize()
            print 'w minsize', w.GetMinSize()
            print 'w bestsize', w.GetBestSize()
            print 'w abstsize', w.GetAdjustedBestSize()
        
            tp = self.testPanel
            #print tp.GetSizer()
            print 'tp size', tp.GetSize()
            print 'tp minsize', tp.GetMinSize()
            print 'tp bestsize', tp.GetBestSize()
            print 'tp abstsize', tp.GetAdjustedBestSize()
            
        

    def OnDestroyWidget(self, evt):
        self.testWidget.Destroy()
        self.testWidget = None
        self.testPanel.SetSizer(None, True)
        self.testPanel.Refresh()
        
        # ensure the panel shrinks again
        self.testPanel.SetSizeHints((20,20)) 
        self.bottomSizer.Layout()
        self.testPanel.SetSizeHints(wx.DefaultSize)
        
        # make the create button be default now
        self.createBtn.SetDefault()


    def OnClear(self, evt):
        self.moduleName.SetValue("")
        self.className.SetValue("")
        self.parameters.SetValue("")
        self.expression.SetValue("")
        self.docstring.SetValue("")
        


app = wx.PySimpleApp(redirect=False)
frame = LayoutTestFrame()
app.SetTopWindow(frame)
frame.Show()
app.MainLoop()

