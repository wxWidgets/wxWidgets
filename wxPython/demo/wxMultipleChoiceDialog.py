# 11/20/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 
# 11/30/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wx renamer not applied to lib.
# 

import  wx
import  wx.lib.dialogs  as  dlgs

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    lst = [ 'apple', 'pear', 'banana', 'coconut', 'orange',
            'etc', 'etc..', 'etc...' ]

    dlg = dlgs.wxMultipleChoiceDialog(frame,
                                 "Pick some from\n this list\nblah blah...",
                                 "m.s.d.", lst)

    if (dlg.ShowModal() == wx.ID_OK):
        print "Selection:", dlg.GetValue(), " -> ", dlg.GetValueString()

#---------------------------------------------------------------------------



overview = """\
<html>
<body>

This is a Python implementation of a dialog that is not yet implemented in wxWindows
proper, so don't let the wxWindows documentation mislead you.

<p><code><b>wxMultipleChoiceDialog</b>(self, parent, msg, title, lst, 
pos = wx.wxDefaultPosition, size = (200,200), style = wx.wxDEFAULT_DIALOG_STYLE)
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

Additionally, wxMultipleChoiceDialog.lbox is a standard wxListBox which supports all
methods applicable to that class.

</body>
</html>
"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
