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
            self.list.SetItemData(x, x*2)

        self.list.SetColumnWidth(0, wxLIST_AUTOSIZE)
        self.list.SetColumnWidth(1, wxLIST_AUTOSIZE)
        self.list.SetColumnWidth(2, wxLIST_AUTOSIZE)

        self.list.SetItemState(5, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED)

        self.currentItem = 0
        EVT_LIST_ITEM_SELECTED(self, tID, self.OnItemSelected)
        EVT_LIST_DELETE_ITEM(self, tID, self.OnItemDelete)
        EVT_LEFT_DCLICK(self.list, self.OnDoubleClick)
        EVT_RIGHT_DOWN(self.list, self.OnRightDown)

        # for wxMSW
        EVT_COMMAND_RIGHT_CLICK(self.list, tID, self.OnRightClick)

        # for wxGTK
        EVT_RIGHT_UP(self.list, self.OnRightClick)


    def OnRightDown(self, event):
        self.x = event.GetX()
        self.y = event.GetY()
        self.log.WriteText("x, y = %s\n" % str((self.x, self.y)))
        event.Skip()

    def OnItemSelected(self, event):
        self.currentItem = event.m_itemIndex
        self.log.WriteText("OnItemSelected: %s\n" % self.list.GetItemText(self.currentItem))

    def OnItemDelete(self, event):
        self.log.WriteText("OnItemDelete\n")


    def OnDoubleClick(self, event):
        self.log.WriteText("OnDoubleClick item %s\n" % self.list.GetItemText(self.currentItem))


    def OnRightClick(self, event):
        self.log.WriteText("OnRightClick %s\n" % self.list.GetItemText(self.currentItem))
        menu = wxMenu()
        tPopupID1 = 0
        tPopupID2 = 1
        tPopupID3 = 2
        tPopupID4 = 3
        tPopupID5 = 5
        menu.Append(tPopupID1, "One")
        menu.Append(tPopupID2, "Two")
        menu.Append(tPopupID3, "Three")
        menu.Append(tPopupID4, "DeleteAllItems")
        menu.Append(tPopupID5, "GetItem")
        EVT_MENU(self, tPopupID1, self.OnPopupOne)
        EVT_MENU(self, tPopupID2, self.OnPopupTwo)
        EVT_MENU(self, tPopupID3, self.OnPopupThree)
        EVT_MENU(self, tPopupID4, self.OnPopupFour)
        EVT_MENU(self, tPopupID5, self.OnPopupFive)
        self.PopupMenu(menu, wxPoint(self.x, self.y))
        menu.Destroy()

    def OnPopupOne(self, event):
        self.log.WriteText("Popup one\n")

    def OnPopupTwo(self, event):
        self.log.WriteText("Popup two\n")

    def OnPopupThree(self, event):
        self.log.WriteText("Popup three\n")

    def OnPopupFour(self, event):
        self.list.DeleteAllItems()

    def OnPopupFive(self, event):
        item = self.list.GetItem(self.currentItem)
        print item.m_text, item.m_itemId, self.list.GetItemData(self.currentItem)

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
