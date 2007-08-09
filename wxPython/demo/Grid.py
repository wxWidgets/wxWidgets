
import  wx

#---------------------------------------------------------------------------

buttonDefs = {
    814 : ('GridSimple',      ' Simple wx.Grid, catching all events '),
    815 : ('GridStdEdRend',   ' wx.Grid showing Editors and Renderers '),
    818 : ('GridHugeTable',   ' A wx.Grid with a HUGE table (100 MILLION cells!) '),
    817 : ('GridCustTable',   ' wx.Grid using a custom Table, with non-string data '),
    819 : ('GridEnterHandler',' Remapping keys to behave differently '),
    820 : ('GridCustEditor',  ' Shows how to create a custom Cell Editor '),
    821 : ('GridDragable',    ' A wx.Grid with dragable rows and columns '),
    822 : ('GridDragAndDrop', ' Shows how to make a grid a drop target for files'),
    }


class ButtonPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log

        box = wx.BoxSizer(wx.VERTICAL)
        box.Add((20, 20))
        keys = buttonDefs.keys()
        keys.sort()

        for k in keys:
            text = buttonDefs[k][1]
            btn = wx.Button(self, k, text)
            box.Add(btn, 0, wx.ALIGN_CENTER|wx.ALL, 10)
            self.Bind(wx.EVT_BUTTON, self.OnButton, btn)

        self.SetSizer(box)
        box.Fit(self)
        

    def OnButton(self, evt):
        modName = buttonDefs[evt.GetId()][0]
        module = __import__(modName)
        frame = module.TestFrame(None, self.log)
        frame.Show(True)


#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = ButtonPanel(nb, log)
    return win

#---------------------------------------------------------------------------



overview = """\
<html><body>
<h2>wx.Grid</h2>

This demo shows various ways of using the <b><i>new and
improved</i></b> wx.Grid class.  Unfortunatly it has not been
documented yet, and while it is somewhat backwards compatible, if you
try to go by the current wx.Grid documentation you will probably just
confuse yourself.
<p>
You can look at the sources for these samples to learn a lot about how
the new classes work.
<p><ol>
<li><a href="GridSimple.py">GridSimple.py</a> A simple grid that shows
how to catch all the various events.

<p>
<li><a href="GridStdEdRend.py">GridStdEdRend.py</a> A grid that
uses non-default Cell Editors and Cell Renderers.

<p>
<li><a href="GridHugeTable.py">GridHugeTable.py</a> A grid that
uses a non-default Grid Table.  This table is read-only and simply
generates on the fly a unique string for each cell.

<p>
<li><a href="GridCustTable.py">GridCustTable.py</a> This grid
shows how to deal with tables that have non-string data, and how Cell
Editors and Cell Renderers are automatically chosen based on the data
type.

<p>
<li><a href="GridEnterHandler.py">GridEnterHandler.py</a>This one
changes how the ENTER key works, moving the current cell left to right
and wrapping around to the next row when needed.
</ol>
<p>

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

