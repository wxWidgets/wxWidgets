#!/bin/env python
#----------------------------------------------------------------------------
# Name:         test1.py
# Purpose:      A minimal wxPython program
#
# Author:       Robin Dunn
#
# Created:
# RCS-ID:       $Id$
# Copyright:    (c) 1998 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------


from wxPython.wx import *


#---------------------------------------------------------------------------

class MyFrame(wxFrame):
    def __init__(self, parent, id, title):
        wxFrame.__init__(self, parent, id, title, wxPoint(100, 100), wxSize(160, 100))
        self.Connect(-1, -1, wxEVT_MOVE, self.OnMove)

    def OnCloseWindow(self, event):
        self.Destroy()

    def OnSize(self, event):
        size = event.GetSize()
        print "size:", size.width, size.height

    def OnMove(self, event):
        pos = event.GetPosition()
        print "pos:", pos.x, pos.y



#---------------------------------------------------------------------------


class MyApp(wxApp):
    def OnInit(self):
        frame = MyFrame(NULL, -1, "This is a test")
        frame.Show(true)
        self.SetTopWindow(frame)
        return true

#---------------------------------------------------------------------------


def main():
    app = MyApp(0)
    app.MainLoop()


def t():
    import pdb
    pdb.run('main()')

if __name__ == '__main__':
    main()



#----------------------------------------------------------------------------
#
# $Log$
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
