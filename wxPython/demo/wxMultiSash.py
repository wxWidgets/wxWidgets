# 11/20/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 
# 11/30/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wx renamer needs to be applied to multisash lib.
# o There appears to be a problem with the image that
#   the library is trying to use for the alternate cursor
# 

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

        self.SetText(sampleText)

class TestFrame(wx.Frame):
    def __init__(self, parent, log):
        wx.Frame.__init__(self, parent, -1, "Multi Sash Demo", size=(640,480))
        self.multi = sash.wxMultiSash(self,-1,pos=(0,0), size=(640,480))

        # Use this method to set the default class that will be created when
        # a new sash is created. The class's constructor needs 1 parameter
        # which is the parent of the window
        self.multi.SetDefaultChildClass(TestWindow)


#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    multi = sash.wxMultiSash(nb, -1, pos = (0,0), size = (640,480))

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

