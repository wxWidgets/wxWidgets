
from wxPython.wx import *
from wxPython.lib import fancytext

#----------------------------------------------------------------------

test_str = ('<font style="italic" family="swiss" color="red" weight="bold" >'
            'some  |<sup>23</sup> <angle/>text<sub>with <angle/> subscript</sub>'
            '</font> some other text')

test_str2 = '<font family="swiss" color="dark green" size="40">big green text</font>'


class TestPanel(wxPanel):
    def __init__(self, parent):
        wxPanel.__init__(self, parent, -1)
        EVT_PAINT(self, self.OnPaint)


    def OnPaint(self, evt):
        dc = wxPaintDC(self)

        sz = fancytext.getExtent(test_str, dc)
        fancytext.renderToDC(test_str, dc, 20, 20)

        fancytext.renderToDC(test_str2, dc, 20, 20 + sz.height + 10)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb)
    return win

#----------------------------------------------------------------------



import string
overview = string.replace(fancytext.__doc__, "<", "&lt;")

