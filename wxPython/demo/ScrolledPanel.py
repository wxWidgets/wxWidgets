
from wxPython.wx import *

#----------------------------------------------------------------------

text = "one two buckle my shoe three four shut the door five six pick up sticks seven eight lay them straight nine ten big fat hen"


class ScrolledPanel(wxScrolledWindow):
    def __init__(self, parent, log):
        self.log = log
        wxScrolledWindow.__init__(self, parent, -1,
                                  style = wxTAB_TRAVERSAL|wxHSCROLL|wxVSCROLL)


        box = wxBoxSizer(wxVERTICAL)
        box.Add(wxStaticText(self, -1,
                             "This sample shows how to make a scrollable data entry \n"
                             "form by using a wxSizer in a wxScrolledWindow."),
                0, wxCENTER|wxALL, 5)
        box.Add(wxStaticLine(self, -1), 0, wxEXPAND|wxALL, 5)

        fgs = wxFlexGridSizer(cols=2, vgap=4, hgap=4)
        fgs.AddGrowableCol(1)

        # Add some spacers
        fgs.Add(75, 10)
        fgs.Add(150, 10)

        for word in text.split():
            label = wxStaticText(self, -1, word+":")
            tc = wxTextCtrl(self, -1, word)
            fgs.Add(label, flag=wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL)
            fgs.Add(tc, flag=wxEXPAND)

        box.Add(fgs, 1)
        box.Add(10, 40)  # some more empty space at the bottom
        self.SetSizer(box)


        # The following is all that is needed to integrate the sizer and the
        # scrolled window.  In this case we will only support vertical scrolling.
        self.EnableScrolling(false, true)
        self.SetScrollRate(0, 20)
        box.SetVirtualSizeHints(self)

        EVT_CHILD_FOCUS(self, self.OnChildFocus)


    def OnChildFocus(self, evt):
        # If the child window that gets the focus is not visible,
        # this handler will try to scroll enough to see it.  If you
        # need to handle horizontal auto-scrolling too then this will
        # need adapted.
        evt.Skip()
        child = evt.GetWindow()

        sppu_y = self.GetScrollPixelsPerUnit()[1]
        vs_y   = self.GetViewStart()[1]
        cpos = child.GetPosition()
        csz  = child.GetSize()

        # is it above the top?
        if cpos.y < 0:
            new_vs = cpos.y / sppu_y
            self.Scroll(-1, new_vs)

        # is it below the bottom ?
        if cpos.y + csz.height > self.GetClientSize().height:
            diff = (cpos.y + csz.height - self.GetClientSize().height) / sppu_y
            self.Scroll(-1, vs_y + diff + 1)


#----------------------------------------------------------------------

## class ScrollToHandler(wxEvtHandler):
##     """This class helps to scroll the panel

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = ScrolledPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
This sample shows how to make a scrollable data entry form by
using a wxSizer in a wxScrolledWindow.
</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

