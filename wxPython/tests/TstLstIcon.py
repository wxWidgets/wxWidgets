#!/bin/env python
#----------------------------------------------------------------------------
# Name:         TstLstIcon.py
# Purpose:      Lest Icon List
#
# Author:       Lorne White
#
# Version:      0.8
# Licence:      wxWindows, wxPython license
#----------------------------------------------------------------------------

import sys, os
from   wxPython.wx import *

class AppFrame(wxFrame):
    def __init__(self, parent, id=-1, title="New"):
        wxFrame.__init__(self, parent, id, title, wxPyDefaultPosition, wxSize(420, 320))
        if wxPlatform == '__WXMSW__':
            self.icon = wxIcon('bitmaps/mondrian.ico', wxBITMAP_TYPE_ICO)
            self.SetIcon(self.icon)

        self.CreateStatusBar()

        self.mainmenu = wxMenuBar()
        menu = wxMenu()

        menu = self.MakeFileMenu()
        self.mainmenu.Append(menu, '&File')

        self.SetMenuBar(self.mainmenu)

        self.il = wxImageList(32, 32)
        self.idx1 = idx1 = self.il.Add(wxNoRefBitmap('table.bmp', wxBITMAP_TYPE_BMP))
        self.idx2 = idx2 = self.il.Add(wxNoRefBitmap('query.bmp', wxBITMAP_TYPE_BMP))

        self.nb = nb = wxNotebook(self, -1)

        self.list = wxListCtrl(nb, 1100, wxDefaultPosition, wxDefaultSize)

        nb.AddPage(self.list, "Tables")

        self.list.SetSingleStyle(wxLC_ICON)
        self.list.SetWindowStyleFlag(wxSTATIC_BORDER|wxVSCROLL)
        self.list.SetImageList(self.il, wxIMAGE_LIST_NORMAL)

        self.qlist = wxListCtrl(nb, 1200, wxDefaultPosition, wxDefaultSize)
        nb.AddPage(self.qlist, "Queries")

        self.qlist.SetSingleStyle(wxLC_ICON)
        self.qlist.SetWindowStyleFlag(wxSTATIC_BORDER|wxVSCROLL)
        self.qlist.SetImageList(self.il, wxIMAGE_LIST_NORMAL)

        self.UpdateView2()
        self.UpdateView1()

        self.nb.SetSelection(1)
        self.nb.SetSelection(0)
        #self.nb.Refresh()
        #self.nb.ResizeChildren()

    def MakeFileMenu(self):
        self.fl_mn = menu = wxMenu()

        mID = NewId()
        menu.Append(mID, 'E&xit', 'Exit')
        EVT_MENU(self, mID, self.OnFileExit)

        return menu


    def UpdateView1(self):
        vset = "ViewA "
        for i in range(20):
            self.list.InsertImageStringItem(i, vset + str(i), self.idx1)

    def UpdateView2(self):
        vset = "ViewB "
        for i in range(5):
            self.qlist.InsertImageStringItem(i, vset + str(i), self.idx2)

    def OnFileExit(self, event):
        self.Close()

#---------------------------------------------------------------------------


class MyApp(wxApp):
    def OnInit(self):
        frame = AppFrame(NULL, -1, "Demo")
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
