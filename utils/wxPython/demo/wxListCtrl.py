#!/bin/env python
#----------------------------------------------------------------------------
# Name:         ListCtrl.py
# Purpose:      Testing lots of stuff, controls, window types, etc.
#
# Author:       Robin Dunn & Gary Dumer
#
# Created:
# RCS-ID:       $Id$
# Copyright:    (c) 1998 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------

from wxPython.wx import *

#---------------------------------------------------------------------------

class TestListCtrlPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)

        self.log = log
        tID = NewId()

        self.il = wxImageList(16, 16)
        idx1 = self.il.Add(wxNoRefBitmap('bitmaps/smiles.bmp', wxBITMAP_TYPE_BMP))

        self.list = wxListCtrl(self, tID, wxDefaultPosition, wxDefaultSize,
                               wxLC_REPORT|wxSUNKEN_BORDER)
        self.list.SetImageList(self.il, wxIMAGE_LIST_SMALL)

        self.list.SetToolTip(wxToolTip("This is a ToolTip!"))
        wxToolTip_Enable(true)

        self.list.InsertColumn(0, "Column 0")
        self.list.InsertColumn(1, "Column 1")
        self.list.InsertColumn(2, "One More Column (2)")
        for x in range(50):
            self.list.InsertImageStringItem(x, "This is item %d" % x, idx1)
            self.list.SetStringItem(x, 1, "Col 1, item %d" % x)
            self.list.SetStringItem(x, 2, "item %d in column 2" % x)

        self.list.SetColumnWidth(0, wxLIST_AUTOSIZE)
        self.list.SetColumnWidth(1, wxLIST_AUTOSIZE)
        self.list.SetColumnWidth(2, wxLIST_AUTOSIZE)


    def OnSize(self, event):
        w,h = self.GetClientSizeTuple()
        self.list.SetDimensions(0, 0, w, h)




#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestListCtrlPanel(nb, log)
    return win

#---------------------------------------------------------------------------
















overview = """\
A list control presents lists in a number of formats: list view, report view, icon view and small icon view. Elements are numbered from zero.

wxListCtrl()
------------------------

Default constructor.

wxListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listCtrl")

Constructor, creating and showing a list control.

Parameters
-------------------

parent = Parent window. Must not be NULL.

id = Window identifier. A value of -1 indicates a default value.

pos = Window position.

size = Window size. If the default size (-1, -1) is specified then the window is sized appropriately.

style = Window style. See wxListCtrl.

validator = Window validator.

name = Window name.
"""
