import wx
import wxaddons.sized_controls as sc

overview = """\
<html><body><h2>Sized Controls</h2>
SizedControls is an addon library that attempts to simplify the
creation of sizer-based layouts. It adds the following classes:

<h3>SizedPanel</h3>

This class automatically creates its own sizer (a vertical box sizer
by default) and automatically adds its children to the sizer. You can
change the SizedPanel's sizer type by calling
panel.SetSizerType(\"type\", [args]), where valid types are
\"horizontal\", \"vertical\", \"form\" (a 2-col flex grid sizer), and
\"grid\". Args include \"cols\" and \"rows\" attributes for
grids. This class also applies control borders that adhere to the
native platform's Human Interface Guidelines (HIG) on Win, GTK and
Mac.

<h3>SizedFrame and SizedDialog</h3>

These classes automatically setup a SizedPanel which is appropriately
positioned and given appropriate borders in accordance with the
platform's HIGs.

<p>Since controls are added to the parent's sizer upon creation, you
don't need to use sizer.Add or even create sizers yourself. You just
use SetSizerType() to change the sizer you want to use, and
control.SetSizerProps() to change the sizer properties of the
control. So as a result, code that used to look like this:

<table bgcolor=\"#EFEFEF\"><tr><td><pre>
... wx.Dialog init code...

panel = wx.Panel(self, -1)
b1 = wx.Button(panel, -1)
b2 = wx.Button(panel, -1)
t1 = wx.TextCtrl(panel, -1)
b3 = wx.Button(panel, -1)

sizer = wx.BoxSizer(wx.HORIZONTAL)
sizer.Add(b1, 0, wx.ALL, 6)
sizer.Add(b2, 0, wx.ALL, 6)
sizer.Add(t1, 0, wx.EXPAND | wx.ALL, 6)
sizer.Add(b3, 0, wx.ALL, 6)
panel.SetSizer(sizer)

dlgSizer = wx.BoxSizer()
dlgSizer.Add(panel, 1, wx.EXPAND)
self.SetSizer(dlgSizer)
self.SetAutoLayout(True)

... rest of dialog ...</pre>
</td></tr></table>

would now look like this:

<table bgcolor=\"#EFEFEF\"><tr><td><pre>
... wx.Dialog init code...

panel = self.GetContentsPane()
panel.SetSizerType(\"horizontal\")

b1 = wx.Button(panel, -1)
b2 = wx.Button(panel, -1)

t1 = wx.TextCtrl(panel, -1)
t1.SetSizerProps(expand=True)

b3 = wx.Button(panel, -1)

... rest of dialog ...</pre>
</td></tr></table>

and the latter example will adhere to HIG spacing guidelines on all platforms,
unlike the former example. Please check the demos for more complete and sophisticated examples of SizedControls
in action.

<h3>wx.Window.SetSizerProps Quick Reference</h3>

<p><pre>wx.Window.SetSizerProps(&lt;props&gt;)</pre>

<p>
<table bgcolor=\"#EFEFEF\">
<tr>
<td valign="middle" width="90"><b>Parameter</b></td> <td valign="middle"><b>Values</b></td> <td valign="middle"><b>Summary</b></td>
</tr>

<tr>
<td><i>expand</i></td> <td>True/False</td>
<td>Whether or not the control should grow to fill free space if 
free space is available.</td>
</tr>

<tr>
<td><i>proportion</i></td> <td>Number (typically 0-10)</td> 
<td>How much of the free space the control should take up. Note that this value is 
<i>relative</i> to other controls, so a proportion of 2 means take up 
'twice as much' space as controls with a proportion of 1.</td>
</tr>

<tr>
<td><i>halign</i> <td>"left", "center", "centre", "right"</td>
<td>Determines horizontal alignment of control.</td>
</tr>

<tr>
<td><i>valign</i> <td>"top", "center", "centre", "bottom"</td>
<td>Determines vertical alignment of control.</td>
</tr>

<tr>
<td><i>border</i> <td>Tuple: ([<i>dirs</i>], integer)</td>
<td>Specifies amount of border padding to apply to specified directions. </br>
Example: (["left", "right"], 6) would add six pixels to left and right borders. </br>
Note that, unfortunately,
it is not currently possible to assign different border sizes to each direction.</td>
</tr>

<tr>
<td><i>minsize</i> <td>One of the following string values: "fixed", "adjust"</td>
<td>Determines whether or not the minsize can be updated when the control's best size changes.</td>
</tr>

</table>
"""

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
                                style=wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER)
        
        # Always use self.GetContentsPane() - this ensures that your dialog
        # automatically adheres to HIG spacing requirements on all platforms.
        # pane here is a sc.SizedPanel with a vertical sizer layout. All children
        # should be added to this pane, NOT to self.
        pane = self.GetContentsPane()
        
        # first row
        self.listCtrl = wx.ListCtrl(pane, -1, size=(300, -1), style=wx.LC_REPORT)
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
        

class GridFrame(sc.SizedFrame):
    def __init__(self, parent, id):
        sc.SizedFrame.__init__(self, parent, id, "Grid Layout Demo Frame")
        
        pane = self.GetContentsPane()
        pane.SetSizerType("grid", {"cols":3}) # 3-column grid layout
        
        # row 1
        wx.TextCtrl(pane, -1).SetSizerProps(halign="left")
        wx.TextCtrl(pane, -1).SetSizerProps(halign="center")
        wx.TextCtrl(pane, -1).SetSizerProps(halign="right")
        
        # row 2
        wx.TextCtrl(pane, -1).SetSizerProps(valign="center")
        wx.TextCtrl(pane, -1).SetSizerProps(expand=True, proportion=1)
        wx.TextCtrl(pane, -1).SetSizerProps(valign="center")
        
        # row 3
        wx.TextCtrl(pane, -1).SetSizerProps(halign="left")
        wx.TextCtrl(pane, -1).SetSizerProps(halign="center")
        wx.TextCtrl(pane, -1).SetSizerProps(halign="right")
        
        self.CreateStatusBar() # should always do this when there's a resize border
        
        self.Fit()
        self.SetMinSize(self.GetSize())
        
        
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
        
        b3 = wx.Button(self, -1, "Sized Controls Grid Layout Demo", (50,130))
        self.Bind(wx.EVT_BUTTON, self.OnGridButton, b3)    
        

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
        
    def OnGridButton(self, evt):
            
        dlg = GridFrame(self, -1)
        dlg.CenterOnScreen()

        dlg.Show()
        
def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

if __name__ == "__main__":  
    app = wx.PySimpleApp()
    dlg = FormDialog()
    dlg.ShowModal()
