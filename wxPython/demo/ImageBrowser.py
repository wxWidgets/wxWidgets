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

from wxPython.wx import *
from wxPython.lib.imagebrowser import *
import os

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dir = os.getcwd()       # get working directory
    initial_dir = os.path.join(dir, 'bitmaps')  # set the initial directory for the demo bitmaps
    win = ImageDialog(frame, initial_dir)   # open the image browser dialog
    win.Centre()
    if win.ShowModal() == wxID_OK:      
        log.WriteText("You Selected File: " + win.GetFile())        # show the selected file
    else:
        log.WriteText("You pressed Cancel\n")
            
#---------------------------------------------------------------------------










overview = """\
"""
