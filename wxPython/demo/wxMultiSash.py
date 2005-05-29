from wxPython.wx import *
from wxPython.lib.multisash import wxMultiSash
from wxPython.stc import *

#---------------------------------------------------------------------------

sampleText="""\
You can drag the little tab on the vertical sash left to create another view,
or you can drag the tab on the horizontal sash to the top to create another
horizontal view.

The red blocks on the sashes will destroy the view (bottom,left) this block
belongs to.

A yellow rectangle also highlights the current selected view.

By calling GetSaveData on the multiSash control the control will return its
contents and the positions of each sash as a dictionary.
Calling SetSaveData with such a dictionary will restore the control to the
state it was in when it was saved.

If the class, that is used as a view, has GetSaveData/SetSaveData implemented,
these will also be called to save/restore their state. Any object can be
returned by GetSaveData, as it is just another object in the dictionary.
"""

#---------------------------------------------------------------------------

class TestWindow(wxStyledTextCtrl):
    def __init__(self, parent):
        wxStyledTextCtrl.__init__(self, parent, -1, style=wxNO_BORDER)
        self.SetMarginWidth(1,0)
        if wxPlatform == '__WXMSW__':
            fSize = 10
        else:
            fSize = 12
        self.StyleSetFont(wxSTC_STYLE_DEFAULT,
                          wxFont(fSize, wxMODERN, wxNORMAL, wxNORMAL))
        self.SetText(sampleText)

class TestFrame(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, "Multi Sash Demo",
                         size=(640,480))
        self.multi = wxMultiSash(self,-1,pos = wxPoint(0,0),
                                 size = (640,480))

        # Use this method to set the default class that will be created when
        # a new sash is created. The class's constructor needs 1 parameter
        # which is the parent of the window
        self.multi.SetDefaultChildClass(TestWindow)


#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    multi = wxMultiSash(nb, -1, pos = (0,0), size = (640,480))

    # Use this method to set the default class that will be created when
    # a new sash is created. The class's constructor needs 1 parameter
    # which is the parent of the window
    multi.SetDefaultChildClass(TestWindow)

    return multi

#    win = TestPanel(nb, log)
#    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wxMultiSash</center></h2>

wxMultiSash allows the user to split a window any number of times
either horizontally or vertically, and to close the split off windows
when desired.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

