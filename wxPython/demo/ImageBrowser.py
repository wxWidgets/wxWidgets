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
#
# 11/23/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 
# 11/25/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Library has to be updated, it is using obsolete names 
#   (wxPyDefaultSize, etc)
# 

import  os

import  wx
import  wx.lib.imagebrowser    as  ib
#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    # get current working directory
    dir = os.getcwd()

    # set the initial directory for the demo bitmaps
    initial_dir = os.path.join(dir, 'bitmaps')

    # open the image browser dialog
    win = ib.ImageDialog(frame, initial_dir)
    
    win.Centre()

    if win.ShowModal() == wx.ID_OK:
        # show the selected file
        log.WriteText("You Selected File: " + win.GetFile())        
    else:
        log.WriteText("You pressed Cancel\n")

    win.Destroy()
    
#---------------------------------------------------------------------------


overview = """\
"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

