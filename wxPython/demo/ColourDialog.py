 
import  wx

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Create and Show a ColourDialog", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        dlg = wx.ColourDialog(self)

        # Ensure the full colour dialog is displayed, 
        # not the abbreviated version.
        dlg.GetColourData().SetChooseFull(True)

        if dlg.ShowModal() == wx.ID_OK:

            # If the user selected OK, then the dialog's wx.ColourData will
            # contain valid information. Fetch the data ...
            data = dlg.GetColourData()

            # ... then do something with it. The actual colour data will be
            # returned as a three-tuple (r, g, b) in this particular case.
            self.log.WriteText('You selected: %s\n' % str(data.GetColour().Get()))

        # Once the dialog is destroyed, Mr. wx.ColourData is no longer your
        # friend. Don't use it again!
        dlg.Destroy()

#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


#---------------------------------------------------------------------------


overview = """\
This class represents the colour chooser dialog.

Use of this dialog is a multi-stage process. 

The actual information about how to display the dialog and the colors in the 
dialog's 'registers' are contained in a wx.ColourData instance that is created by 
the dialog at init time. Before displaying the dialog, you may alter these settings 
to suit your needs. In the example, we set the dialog up to show the extended colour 
data selection pane. Otherwise, only the more compact and less extensive colour 
dialog is shown.  You may also preset the colour as well as other items. 

If the user selects something and selects OK, then the wx.ColourData instance contains
the colour data that the user selected. Before destroying the dialog, retrieve the data.
<b>Do not try to retain the wx.ColourData instance.</b> It will probably not be valid
after the dialog is destroyed.

Along with he wx.ColourDialog documentation, see also the wx.ColourData documentation 
for details.
"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

