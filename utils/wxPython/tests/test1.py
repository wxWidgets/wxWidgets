#!/bin/env python
#----------------------------------------------------------------------------
# Name:         test1.py
# Purpose:      A minimal wxPython program
#
# Author:       Robin Dunn
#
# Created:      A long time ago, in a galaxy far, far away...
# RCS-ID:       $Id$
# Copyright:    (c) 1998 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------


<<<<<<< test1.py
<<<<<<< test1.py
#from wxPython import *
from wxpc import *
=======
=======
## import all of the wxPython GUI package
>>>>>>> 1.4
from wxPython.wx import *
>>>>>>> 1.3

#---------------------------------------------------------------------------

## Create a new frame class, derived from the wxPython Frame.
class MyFrame(wxFrame):

    def __init__(self, parent, id, title):
        # First, call the base class' __init__ method to create the frame
        wxFrame.__init__(self, parent, id, title,
                         wxPoint(100, 100), wxSize(160, 100))

        # Associate some events with methods of this class
        EVT_SIZE(self, self.OnSize)
        EVT_MOVE(self, self.OnMove)


    # This method is called automatically when the CLOSE event is
    # sent to this window
    def OnCloseWindow(self, event):
        # tell the window to kill itself
        self.Destroy()


    # This method is called by the System when the window is resized,
    # because of the association above.
    def OnSize(self, event):
        size = event.GetSize()
        print "size:", size.width, size.height

    # This method is called by the System when the window is moved,
    # because of the association above.
    def OnMove(self, event):
        pos = event.GetPosition()
        print "pos:", pos.x, pos.y



#---------------------------------------------------------------------------


<<<<<<< test1.py
=======
# Every wxWindows application must have a class derived from wxApp
>>>>>>> 1.4
class MyApp(wxApp):

    # wxWindows calls this method to initialize the application
    def OnInit(self):

        # Create an instance of our customized Frame class
        frame = MyFrame(NULL, -1, "This is a test")
        frame.Show(true)

        # Tell wxWindows that this is our main window
        self.SetTopWindow(frame)

        # Return a success flag
        return true

#---------------------------------------------------------------------------

<<<<<<< test1.py

def main():
    app = MyApp(0)
    app.MainLoop()


def t():
    import pdb
    pdb.run('main()')
=======
>>>>>>> 1.4

<<<<<<< test1.py
if __name__ == '__main__':
    main()
=======
app = MyApp(0)     # Create an instance of the application class
app.MainLoop()     # Tell it to start processing events
>>>>>>> 1.4

print 'done!'


#----------------------------------------------------------------------------
#
# $Log$
# Revision 1.6  1999/06/28 21:39:48  VZ
# 1. wxStaticLine implemented (generic (ugly) and MSW versions)
# 2. wxTextDialog looks fine under MSW again
# 3. startup tips added: code, sample, docs
# 4. read-only text controls don't participate in TAB traversal
#
# Revision 1.3  1998/12/15 20:44:34  RD
# Changed the import semantics from "from wxPython import *" to "from
# wxPython.wx import *"  This is for people who are worried about
# namespace pollution, they can use "from wxPython import wx" and then
# prefix all the wxPython identifiers with "wx."
#
# Added wxTaskbarIcon for wxMSW.
#
# Made the events work for wxGrid.
#
# Added wxConfig.
#
# Added wxMiniFrame for wxGTK, (untested.)
#
# Changed many of the args and return values that were pointers to gdi
# objects to references to reflect changes in the wxWindows API.
#
<<<<<<< test1.py
# Other assorted fixes and additions.
#
# Revision 1.2  1998/10/02 06:42:27  RD
#
# Version 0.4 of wxPython for MSW.
#
# Revision 1.1  1998/08/09 08:28:05  RD
# Initial version
#
#
=======

>>>>>>> 1.4
