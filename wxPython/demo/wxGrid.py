
from wxPython.wx import *

#---------------------------------------------------------------------------

buttonDefs = {
    814 : ('GridSimple',     'Simple wxGrid, catching all events'),
    815 : ('GridStdEdRend',  'wxGrid showing Editors and Renderers'),
    818 : ('GridHugeTable',  'A wxGrid with a HUGE table (100 MILLION cells!)'),
    817 : ('GridCustTable',  'wxGrid using a custom Table, with non-string data'),
    819 : ('GridEnterHandler', 'Remapping keys to behave differently'),
    }


class ButtonPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        box = wxBoxSizer(wxVERTICAL)
        box.Add(20, 30)
        keys = buttonDefs.keys()
        keys.sort()
        for k in keys:
            text = buttonDefs[k][1]
            btn = wxButton(self, k, text)
            box.Add(btn, 0, wxALIGN_CENTER|wxALL, 15)
            EVT_BUTTON(self, k, self.OnButton)

        self.SetAutoLayout(true)
        self.SetSizer(box)


    def OnButton(self, evt):
        modName = buttonDefs[evt.GetId()][0]
        module = __import__(modName)
        frame = module.TestFrame(self, self.log)
        frame.Show(true)


#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = ButtonPanel(nb, log)
    return win

#---------------------------------------------------------------------------










overview = """\
<html><body>
<h2>wxGrid</h2>

This demo shows various ways of using the <b><i>new and
improved</i></b> wxGrid class.  Unfortunatly it has not been
documented yet, and while it is somewhat backwards compatible, if you
try to go by the current wxGrid documentation you will probably just
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
You can also look at the <a href="data/grid.i">SWIG interface
file</a> used to generate the grid module for a lot more clues as to
how things work.

"""

