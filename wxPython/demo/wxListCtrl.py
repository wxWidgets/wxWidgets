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

musicdata = {
1 : ("Bad English", "The Price Of Love", "Rock"),
2 : ("DNA featuring Suzanne Vega", "Tom's Diner", "Rock"),
3 : ("George Michael", "Praying For Time", "Rock"),
4 : ("Gloria Estefan", "Here We Are", "Rock"),
5 : ("Linda Ronstadt", "Don't Know Much", "Rock"),
6 : ("Michael Bolton", "How Am I Supposed To Live Without You", "Blues"),
7 : ("Paul Young", "Oh Girl", "Rock"),
8 : ("Paula Abdul", "Opposites Attract", "Rock"),
9 : ("Richard Marx", "Should've Known Better", "Rock"),
10: ("Rod Stewart", "Forever Young", "Rock"),
11: ("Roxette", "Dangerous", "Rock"),
12: ("Sheena Easton", "The Lover In Me", "Rock"),
13: ("Sinead O'Connor", "Nothing Compares 2 U", "Rock"),
14: ("Stevie B.", "Because I Love You", "Rock"),
15: ("Taylor Dayne", "Love Will Lead You Back", "Rock"),
16: ("The Bangles", "Eternal Flame", "Rock"),
17: ("Wilson Phillips", "Release Me", "Rock"),
18: ("Billy Joel", "Blonde Over Blue", "Rock"),
19: ("Billy Joel", "Famous Last Words", "Rock"),
20: ("Billy Joel", "Lullabye (Goodnight, My Angel)", "Rock"),
21: ("Billy Joel", "The River Of Dreams", "Rock"),
22: ("Billy Joel", "Two Thousand Years", "Rock"),
23: ("Janet Jackson", "Alright", "Rock"),
24: ("Janet Jackson", "Black Cat", "Rock"),
25: ("Janet Jackson", "Come Back To Me", "Rock"),
26: ("Janet Jackson", "Escapade", "Rock"),
27: ("Janet Jackson", "Love Will Never Do (Without You)", "Rock"),
28: ("Janet Jackson", "Miss You Much", "Rock"),
29: ("Janet Jackson", "Rhythm Nation", "Rock"),
30: ("Janet Jackson", "State Of The World", "Rock"),
31: ("Janet Jackson", "The Knowledge", "Rock"),
32: ("Spyro Gyra", "End of Romanticism", "Jazz"),
33: ("Spyro Gyra", "Heliopolis", "Jazz"),
34: ("Spyro Gyra", "Jubilee", "Jazz"),
35: ("Spyro Gyra", "Little Linda", "Jazz"),
36: ("Spyro Gyra", "Morning Dance", "Jazz"),
37: ("Spyro Gyra", "Song for Lorraine", "Jazz"),
38: ("Yes", "Owner Of A Lonely Heart", "Rock"),
39: ("Yes", "Rhythm Of Love", "Rock"),
}


class TestListCtrlPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)

        self.log = log
        tID = wxNewId()

        self.il = wxImageList(16, 16)
        idx1 = self.il.Add(wxBitmap('bitmaps/smiles.bmp', wxBITMAP_TYPE_BMP))

        self.list = wxListCtrl(self, tID, wxDefaultPosition, wxDefaultSize,
                               wxLC_REPORT|wxSUNKEN_BORDER)
        self.list.SetImageList(self.il, wxIMAGE_LIST_SMALL)

        self.list.SetToolTip(wxToolTip("This is a ToolTip!"))
        wxToolTip_Enable(true)

        self.list.InsertColumn(0, "Artist")
        self.list.InsertColumn(1, "Title")
        self.list.InsertColumn(2, "Genre")
        items = musicdata.items()
        for x in range(len(items)):
            key, data = items[x]
            self.list.InsertImageStringItem(x, data[0], idx1)
            self.list.SetStringItem(x, 1, data[1])
            self.list.SetStringItem(x, 2, data[2])
            self.list.SetItemData(x, key)

        self.list.SetColumnWidth(0, wxLIST_AUTOSIZE)
        self.list.SetColumnWidth(1, wxLIST_AUTOSIZE)
        ##self.list.SetColumnWidth(2, wxLIST_AUTOSIZE)

        self.list.SetItemState(5, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED)

        # show how to change the colour of an item
        item = self.list.GetItem(1)
        item.SetTextColour(wxBLUE)
        self.list.SetItem(item)

        self.currentItem = 0
        EVT_SIZE(self, self.OnSize)
        EVT_LIST_ITEM_SELECTED(self, tID, self.OnItemSelected)
        EVT_LIST_DELETE_ITEM(self, tID, self.OnItemDelete)
        EVT_LIST_COL_CLICK(self, tID, self.OnColClick)
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

    def OnColClick(self, event):
        self.log.WriteText("OnColClick: %d\n" % event.m_col)
        self.col = event.m_col
        self.list.SortItems(self.ColumnSorter)

    def ColumnSorter(self, key1, key2):
        item1 = musicdata[key1][self.col]
        item2 = musicdata[key2][self.col]
        if item1 == item2:  return 0
        elif item1 < item2: return -1
        else:               return 1


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
