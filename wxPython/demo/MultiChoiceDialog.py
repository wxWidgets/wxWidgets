
import  wx

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Create and Show a wx.MultiChoiceDialog", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        lst = [ 'apple', 'pear', 'banana', 'coconut', 'orange', 'grape', 'pineapple',
                'blueberry', 'raspberry', 'blackberry', 'snozzleberry',
                'etc', 'etc..', 'etc...' ]

        dlg = wx.MultiChoiceDialog( self, 
                                   "Pick some fruit from\nthis list",
                                   "wx.MultiChoiceDialog", lst)

        if (dlg.ShowModal() == wx.ID_OK):
            selections = dlg.GetSelections()
            strings = [lst[x] for x in selections]
            self.log.write("Selections: %s -> %s\n" % (selections, strings))

        dlg.Destroy()



#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#---------------------------------------------------------------------------



overview = """\
<html>
<body>
<h1>wx.MultiChoiceDialog</h1>

This class represents a dialog that shows a list of strings, and
allows the user to select one or more.

</body>
</html>
"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
