# 11/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 
# 11/30/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wx renamer not applied to library.
# 

import  wx
import  wx.lib.colourchooser    as  cc

#---------------------------------------------------------------

class TestColourChooser(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log

        chooser = cc.wxPyColourChooser(self, -1)
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(chooser, 0, wx.ALL, 25)

        self.SetAutoLayout(True)
        self.SetSizer(sizer)

#---------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestColourChooser(nb, log)
    return win

#---------------------------------------------------------------

overview = """
The wxPyColourChooser component creates a colour chooser window
that is similar to the Microsoft Windows colour chooser dialog.
This dialog component is drawn in a panel, and thus can be
embedded inside any widget (although it cannot be resized).
This colour chooser may also be substituted for the colour
chooser on any platform that might have an ugly one :)

How to use it
------------------------------

The demo (demo/wxPyColourChooser.py code shows how to display
a colour chooser and retrieve its options.

Contact and Author Info
------------------------------

wxPyColourChooser was written and is maintained by:

    Michael Gilfix <mgilfix@eecs.tufts.edu>

You can find the latest wxPyColourChooser code at
http://www.sourceforge.net/wxcolourchooser. If you have
any suggestions or want to submit a patch, please send
it my way at: mgilfix@eecs.tufts.edu
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
