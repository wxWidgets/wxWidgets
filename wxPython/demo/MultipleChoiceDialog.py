
import  wx
import  wx.lib.dialogs

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Create and Show a MultipleChoiceDialog", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        lst = [ 'apple', 'pear', 'banana', 'coconut', 'orange', 'grape', 'pineapple',
                'blueberry', 'raspberry', 'blackberry', 'snozzleberry',
                'etc', 'etc..', 'etc...' ]

        dlg = wx.lib.dialogs.MultipleChoiceDialog(
                                    self,
                                    "Pick some from\n this list\nblah blah...",
                                    "m.s.d.", lst)

        if (dlg.ShowModal() == wx.ID_OK):
            self.log.write("Selection: %s -> %s\n" % (dlg.GetValue(), dlg.GetValueString()))

        dlg.Destroy()



#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#---------------------------------------------------------------------------



overview = """\
<html>
<body>

This is a Python implementation of a dialog that is not yet implemented in wxWindows
proper, so don't let the wxWindows documentation mislead you.

<p><code><b>MultipleChoiceDialog</b>(self, parent, msg, title, lst, 
pos = wx.wxDefaultPosition, size = (200,200), style = wx.DEFAULT_DIALOG_STYLE)
</code>

<dl>
<dt><code>parent</code>
<dd>The parent window

<dt><code>msg</code>
<dd>The message that will be displayed above the list

<dt><code>title</code>
<dd>The text that will appear on the title bar

<dt><code>lst</code>
<dd>A Python list of choices that will appear in the dialog.

<dt><code>pos</code>
<dd>The position of the dialog

<dt><code>size</code>
<dd>The size of the dialog

<dt><code>style</code>
<dd>The style for the dialog. Only styles normally available to wxDialog are
available.

</dl>

<b><font size=+1><code>Methods</code></font></b>

<dl>
<dt><code>GetValue</code>
<dd>Returns a tuple containing the indices of the selected items

<dt><code>GetValueString</code>
<dd>Returns a tuple containing the text of the selected items

</dl>

Additionally, MultipleChoiceDialog.lbox is a standard wx.ListBox which supports all
methods applicable to that class.

</body>
</html>
"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
