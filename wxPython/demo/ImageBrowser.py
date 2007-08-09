#----------------------------------------------------------------------------
# Name:         ImageBrowser.py
# Purpose:      Image Selection dialog for wxPython demo
#
# Author:       Lorne White (email: lorne.white@telusplanet.net)
#
# Version       0.5
# Date:         Feb 26, 2001
# Licence:      wxWindows license
#----------------------------------------------------------------------------

import  os

import  wx
import  wx.lib.imagebrowser    as  ib


#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Create and Show an ImageDialog", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        # get current working directory
        dir = os.getcwd()

        # set the initial directory for the demo bitmaps
        initial_dir = os.path.join(dir, 'bitmaps')

        # open the image browser dialog
        dlg = ib.ImageDialog(self, initial_dir)

        dlg.Centre()

        if dlg.ShowModal() == wx.ID_OK:
            # show the selected file
            self.log.WriteText("You Selected File: " + dlg.GetFile())        
        else:
            self.log.WriteText("You pressed Cancel\n")

        dlg.Destroy()


#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

    
#---------------------------------------------------------------------------


overview = """\
"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

