
import  wx
import  wx.lib.multisash    as  sash
import  wx.stc              as  stc

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

class TestWindow(stc.StyledTextCtrl):

    # shared document reference
    doc = None
    
    def __init__(self, parent):
        stc.StyledTextCtrl.__init__(self, parent, -1, style=wx.NO_BORDER)
        self.SetMarginWidth(1,0)

        if wx.Platform == '__WXMSW__':
            fSize = 10
        else:
            fSize = 12

        self.StyleSetFont(
            stc.STC_STYLE_DEFAULT,
            wx.Font(fSize, wx.MODERN, wx.NORMAL, wx.NORMAL)
            )

        if self.doc:
            self.SetDocPointer(self.doc)
        else:
            self.SetText(sampleText)
            TestWindow.doc = self.GetDocPointer()
            

    def ShutDownDemo(self):
        # Reset doc reference in case this demo is run again
        TestWindow.doc = None

        
#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    multi = sash.MultiSash(nb, -1, pos = (0,0), size = (640,480))

    # Use this method to set the default class that will be created when
    # a new sash is created. The class's constructor needs 1 parameter
    # which is the parent of the window
    multi.SetDefaultChildClass(TestWindow)

    return multi

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>MultiSash</center></h2>

MultiSash allows the user to split a window any number of times
either horizontally or vertically, and to close the split off windows
when desired.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

