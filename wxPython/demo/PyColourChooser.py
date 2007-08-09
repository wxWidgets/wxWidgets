
import  wx
import  wx.lib.colourchooser    as  cc

#---------------------------------------------------------------

class TestColourChooser(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log

        chooser = cc.PyColourChooser(self, -1)
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
The PyColourChooser component creates a colour chooser window
that is similar to the Microsoft Windows colour chooser dialog.
This dialog component is drawn in a panel, and thus can be
embedded inside any widget (although it cannot be resized).
This colour chooser may also be substituted for the colour
chooser on any platform that might have an ugly one :)

How to use it
------------------------------

The demo (demo/PyColourChooser.py code shows how to display
a colour chooser and retrieve its options.

Contact and Author Info
------------------------------

PyColourChooser was written and is maintained by:

    Michael Gilfix <mgilfix@eecs.tufts.edu>

You can find the latest PyColourChooser code at
http://sourceforge.net/projects/wxcolourchooser/. If you have
any suggestions or want to submit a patch, please send
it my way at: mgilfix@eecs.tufts.edu
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
