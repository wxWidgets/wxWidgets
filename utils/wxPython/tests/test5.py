#!/bin/env python
#----------------------------------------------------------------------------
# Name:         test5.py
# Purpose:      Testing wxTaskBarIcon for win32 systems
#
# Author:       Robin Dunn
#
# Created:      17-Nov-1998
# RCS-ID:       $Id$
# Copyright:    (c) 1998 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------


from wxPython import *


#---------------------------------------------------------------------------

class MyDialog(wxDialog):
    def __init__(self):
        wxDialog.__init__(self, NULL, -1, "wxTaskBarIcon Test",
                          wxPoint(-1,-1), wxSize(380,250),
                          wxDIALOG_MODELESS|wxDEFAULT_DIALOG_STYLE)

        # build the contents of the Dialog
        wxStaticText(self, -1,
                     "Press OK to hide me, Exit to quit.",
                     wxPoint(10, 20))
        wxStaticText(self, -1,
                     "Double-click on the taskbar icon to show me again.",
                     wxPoint(10, 40))

        okButton = wxButton(self, wxID_OK, "OK", wxPoint(100, 180), wxSize(80, 25))
        exitButton = wxButton(self, wxID_EXIT, "Exit", wxPoint(185, 180), wxSize(80, 25))
        okButton.SetDefault()
        self.Centre(wxBOTH)

        EVT_BUTTON(self, wxID_OK, self.OnOK)
        EVT_BUTTON(self, wxID_EXIT, self.OnExit)


        # make the TaskBar icon
        self.tbIcon = wxTaskBarIcon()
        icon = wxIcon('bitmaps/smiles.ico', wxBITMAP_TYPE_ICO)
        self.tbIcon.SetIcon(icon, "Test ToolTip")
        EVT_TASKBAR_LEFT_DCLICK(self.tbIcon, self.OnTaskBarActivate)



    def OnTaskBarActivate(self, event):
        self.Show(true)

    def OnOK(self, event):
        self.Show(false)

    def OnExit(self, event):
        self.Close(true)

    def OnCloseWindow(self, event):
        self.Destroy()
        del self.tbIcon  # ensure the tbIcon is cleaned up...



#---------------------------------------------------------------------------

class MyApp(wxApp):
    def OnInit(self):
        self.dlg = MyDialog()
        self.dlg.Show(true)
        self.SetTopWindow(self.dlg)
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
# Revision 1.1  1998/11/25 08:47:12  RD
# Added wxPalette, wxRegion, wxRegionIterator, wxTaskbarIcon
# Added events for wxGrid
# Other various fixes and additions
#
#
