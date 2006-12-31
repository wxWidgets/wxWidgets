import wx
import wxaddons.sized_controls as sc

class FormDialog(sc.SizedDialog):
    def __init__(self, parent, id):
        sc.SizedDialog.__init__(self, None, -1, "SizedForm Dialog", 
                        style=wx.DEFAULT_DIALOG_STYLE | wx.RESIZE_BORDER)
        
        pane = self.GetContentsPane()
        pane.SetSizerType("form")
        
        # row 1
        wx.StaticText(pane, -1, "Name")
        textCtrl = wx.TextCtrl(pane, -1, "Your name here")
        textCtrl.SetSizerProps(expand=True)
        
        # row 2
        wx.StaticText(pane, -1, "Email")
        emailCtrl = wx.TextCtrl(pane, -1, "")
        emailCtrl.SetSizerProps(expand=True)
        
        # row 3
        wx.StaticText(pane, -1, "Gender")
        wx.Choice(pane, -1, choices=["male", "female"])
        
        # row 4
        wx.StaticText(pane, -1, "State")
        wx.TextCtrl(pane, -1, size=(60, -1)) # two chars for state
        
        # row 5
        wx.StaticText(pane, -1, "Title")
        
        # here's how to add a 'nested sizer' using sized_controls
        radioPane = sc.SizedPanel(pane, -1)
        radioPane.SetSizerType("horizontal")
        radioPane.SetSizerProps(expand=True)
        
        # make these children of the radioPane to have them use
        # the horizontal layout
        wx.RadioButton(radioPane, -1, "Mr.")
        wx.RadioButton(radioPane, -1, "Mrs.")
        wx.RadioButton(radioPane, -1, "Dr.")
        # end row 5
        
        # add dialog buttons
        self.SetButtonSizer(self.CreateStdDialogButtonSizer(wx.OK | wx.CANCEL))
        
        # a little trick to make sure that you can't resize the dialog to
        # less screen space than the controls need
        self.Fit()
        self.SetMinSize(self.GetSize())


class ErrorDialog(sc.SizedDialog):
    def __init__(self, parent, id):
        sc.SizedDialog.__init__(self, parent, id, "Error log viewer", 
                                wx.DefaultPosition, wx.Size(420, 340), 
                                style=wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER)
        
        # Always use self.GetContentsPane() - this ensures that your dialog
        # automatically adheres to HIG spacing requirements on all platforms.
        # pane here is a sc.SizedPanel with a vertical sizer layout. All children
        # should be added to this pane, NOT to self.
        pane = self.GetContentsPane()
        
        # first row
        self.listCtrl = wx.ListCtrl(pane, -1, style=wx.LC_REPORT)
        self.listCtrl.SetSizerProps(expand=True, proportion=1)
        self.ConfigureListCtrl()
        
        # second row
        self.lblDetails = wx.StaticText(pane, -1, "Error Details")
        
        # third row
        self.details = wx.TextCtrl(pane, -1, style=wx.TE_MULTILINE)
        self.details.SetSizerProps(expand=True, proportion=1)

        # final row
        # since we want to use a custom button layout, we won't use the 
        # CreateStdDialogBtnSizer here, we'll just create our own panel with
        # a horizontal layout and add the buttons to that.
        btnpane = sc.SizedPanel(pane, -1)
        btnpane.SetSizerType("horizontal")
        btnpane.SetSizerProps(expand=True)

        self.saveBtn = wx.Button(btnpane, wx.ID_SAVE)
        spacer = sc.SizedPanel(btnpane, -1)
        spacer.SetSizerProps(expand=True, proportion=1)

        self.clearBtn = wx.Button(btnpane, -1, "Clear")

        self.Fit()
        self.SetMinSize(self.GetSize())
        
    def ConfigureListCtrl(self):
        self.listCtrl.InsertColumn(0, "Time")
        self.listCtrl.InsertColumn(1, "Error Message")
        self.listCtrl.SetColumnWidth(0, 100)
        self.listCtrl.SetColumnWidth(1, 280)
        
#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        self.parent = parent
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Sized Controls Form Dialog", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnFormButton, b)

        b2 = wx.Button(self, -1, "Sized Controls Error Dialog", (50,90))
        self.Bind(wx.EVT_BUTTON, self.OnErrorButton, b2)            
        

    def OnFormButton(self, evt):
        print 
        dlg = FormDialog(self, -1)
        dlg.CenterOnScreen()

        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
    
        if val == wx.ID_OK:
            self.log.WriteText("You pressed OK\n")
        else:
            self.log.WriteText("You pressed Cancel\n")

        dlg.Destroy()
        
    def OnErrorButton(self, evt):
            
        dlg = ErrorDialog(self, -1)
        dlg.CenterOnScreen()

        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
    
        if val == wx.ID_OK:
            self.log.WriteText("You pressed OK\n")
        else:
            self.log.WriteText("You pressed Cancel\n")

        dlg.Destroy()
        
def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win
        

if __name__ == "__main__":  
    app = wx.PySimpleApp()
    dlg = FormDialog()
    dlg.ShowModal()