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
from wxPython.lib.mixins.listctrl import wxColumnSorterMixin, wxListCtrlAutoWidthMixin

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
40: ("Cusco", "Dream Catcher", "New Age"),
41: ("Cusco", "Geronimos Laughter", "New Age"),
42: ("Cusco", "Ghost Dance", "New Age"),
43: ("Blue Man Group", "Drumbone", "New Age"),
44: ("Blue Man Group", "Endless Column", "New Age"),
45: ("Blue Man Group", "Klein Mandelbrot", "New Age"),
46: ("Kenny G", "Silhouette", "Jazz"),
47: ("Sade", "Smooth Operator", "Jazz"),
48: ("David Arkenstone", "Papillon (On The Wings Of The Butterfly)", "New Age"),
49: ("David Arkenstone", "Stepping Stars", "New Age"),
50: ("David Arkenstone", "Carnation Lily Lily Rose", "New Age"),
51: ("David Lanz", "Behind The Waterfall", "New Age"),
52: ("David Lanz", "Cristofori's Dream", "New Age"),
53: ("David Lanz", "Heartsounds", "New Age"),
54: ("David Lanz", "Leaves on the Seine", "New Age"),
}

import images


class TestListCtrl(wxListCtrl, wxListCtrlAutoWidthMixin):
    def __init__(self, parent, ID, pos=wxDefaultPosition,
                 size=wxDefaultSize, style=0):
        wxListCtrl.__init__(self, parent, ID, pos, size, style)
        wxListCtrlAutoWidthMixin.__init__(self)



class TestListCtrlPanel(wxPanel, wxColumnSorterMixin):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1, style=wxWANTS_CHARS)

        self.log = log
        tID = wxNewId()

        self.il = wxImageList(16, 16)

        self.idx1 = self.il.Add(images.getSmilesBitmap())
        self.sm_up = self.il.Add(images.getSmallUpArrowBitmap())
        self.sm_dn = self.il.Add(images.getSmallDnArrowBitmap())

        self.list = TestListCtrl(self, tID,
                                 style=wxLC_REPORT | wxSUNKEN_BORDER
                                 #| wxLC_NO_HEADER
                                 #| wxLC_VRULES | wxLC_HRULES
                                 )
        self.list.SetImageList(self.il, wxIMAGE_LIST_SMALL)

        self.PopulateList()

        # Now that the list exists we can init the other base class,
        # see wxPython/lib/mixins/listctrl.py
        self.itemDataMap = musicdata
        wxColumnSorterMixin.__init__(self, 3)
        #self.SortListItems(0, True)

        EVT_SIZE(self, self.OnSize)
        EVT_LIST_ITEM_SELECTED(self, tID, self.OnItemSelected)
        EVT_LIST_ITEM_DESELECTED(self, tID, self.OnItemDeselected)
        EVT_LIST_ITEM_ACTIVATED(self, tID, self.OnItemActivated)
        EVT_LIST_DELETE_ITEM(self, tID, self.OnItemDelete)
        EVT_LIST_COL_CLICK(self, tID, self.OnColClick)
        EVT_LIST_COL_RIGHT_CLICK(self, tID, self.OnColRightClick)
        EVT_LIST_COL_BEGIN_DRAG(self, tID, self.OnColBeginDrag)
        EVT_LIST_COL_DRAGGING(self, tID, self.OnColDragging)
        EVT_LIST_COL_END_DRAG(self, tID, self.OnColEndDrag)

        EVT_LEFT_DCLICK(self.list, self.OnDoubleClick)
        EVT_RIGHT_DOWN(self.list, self.OnRightDown)

        # for wxMSW
        EVT_COMMAND_RIGHT_CLICK(self.list, tID, self.OnRightClick)

        # for wxGTK
        EVT_RIGHT_UP(self.list, self.OnRightClick)


    def PopulateList(self):
        if 0:
            # for normal, simple columns, you can add them like this:
            self.list.InsertColumn(0, "Artist")
            self.list.InsertColumn(1, "Title", wxLIST_FORMAT_RIGHT)
            self.list.InsertColumn(2, "Genre")
        else:
            # but since we want images on the column header we have to do it the hard way:
            info = wxListItem()
            info.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE | wxLIST_MASK_FORMAT
            info.m_image = -1
            info.m_format = 0
            info.m_text = "Artist"
            self.list.InsertColumnInfo(0, info)

            info.m_format = wxLIST_FORMAT_RIGHT
            info.m_text = "Title"
            self.list.InsertColumnInfo(1, info)

            info.m_format = 0
            info.m_text = "Genre"
            self.list.InsertColumnInfo(2, info)

        items = musicdata.items()
        for x in range(len(items)):
            key, data = items[x]
            self.list.InsertImageStringItem(x, data[0], self.idx1)
            self.list.SetStringItem(x, 1, data[1])
            self.list.SetStringItem(x, 2, data[2])
            self.list.SetItemData(x, key)

        self.list.SetColumnWidth(0, wxLIST_AUTOSIZE)
        self.list.SetColumnWidth(1, wxLIST_AUTOSIZE)
        self.list.SetColumnWidth(2, 100)

        # show how to select an item
        self.list.SetItemState(5, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED)

        # show how to change the colour of a couple items
        item = self.list.GetItem(1)
        item.SetTextColour(wxBLUE)
        self.list.SetItem(item)
        item = self.list.GetItem(4)
        item.SetTextColour(wxRED)
        self.list.SetItem(item)

        self.currentItem = 0


    # Used by the wxColumnSorterMixin, see wxPython/lib/mixins/listctrl.py
    def GetListCtrl(self):
        return self.list

    # Used by the wxColumnSorterMixin, see wxPython/lib/mixins/listctrl.py
    def GetSortImages(self):
        return (self.sm_dn, self.sm_up)


    def OnRightDown(self, event):
        self.x = event.GetX()
        self.y = event.GetY()
        self.log.WriteText("x, y = %s\n" % str((self.x, self.y)))
        item, flags = self.list.HitTest((self.x, self.y))
        if flags & wxLIST_HITTEST_ONITEM:
            self.list.Select(item)
        event.Skip()


    def getColumnText(self, index, col):
        item = self.list.GetItem(index, col)
        return item.GetText()


    def OnItemSelected(self, event):
        ##print event.GetItem().GetTextColour()
        self.currentItem = event.m_itemIndex
        self.log.WriteText("OnItemSelected: %s, %s, %s, %s\n" %
                           (self.currentItem,
                            self.list.GetItemText(self.currentItem),
                            self.getColumnText(self.currentItem, 1),
                            self.getColumnText(self.currentItem, 2)))
        if self.currentItem == 10:
            self.log.WriteText("OnItemSelected: Veto'd selection\n")
            #event.Veto()  # doesn't work
            # this does
            self.list.SetItemState(10, 0, wxLIST_STATE_SELECTED)

    def OnItemDeselected(self, evt):
        item = evt.GetItem()
        self.log.WriteText("OnItemDeselected: %d" % evt.m_itemIndex)

        # Show how to reselect something we don't want deselected
        if evt.m_itemIndex == 11:
            wxCallAfter(self.list.SetItemState, 11, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED)


    def OnItemActivated(self, event):
        self.currentItem = event.m_itemIndex
        self.log.WriteText("OnItemActivated: %s\nTopItem: %s" %
                           (self.list.GetItemText(self.currentItem), self.list.GetTopItem()))

    def OnItemDelete(self, event):
        self.log.WriteText("OnItemDelete\n")

    def OnColClick(self, event):
        self.log.WriteText("OnColClick: %d\n" % event.GetColumn())

    def OnColRightClick(self, event):
        item = self.list.GetColumn(event.GetColumn())
        self.log.WriteText("OnColRightClick: %d %s\n" %
                           (event.GetColumn(), (item.GetText(), item.GetAlign(),
                                                item.GetWidth(), item.GetImage())))

    def OnColBeginDrag(self, event):
        self.log.WriteText("OnColBeginDrag\n")
        ## Show how to not allow a column to be resized
        #if event.GetColumn() == 0:
        #    event.Veto()


    def OnColDragging(self, event):
        self.log.WriteText("OnColDragging\n")

    def OnColEndDrag(self, event):
        self.log.WriteText("OnColEndDrag\n")

    def OnDoubleClick(self, event):
        self.log.WriteText("OnDoubleClick item %s\n" % self.list.GetItemText(self.currentItem))
        event.Skip()

    def OnRightClick(self, event):
        self.log.WriteText("OnRightClick %s\n" % self.list.GetItemText(self.currentItem))

        # only do this part the first time
        if not hasattr(self, "popupID1"):
            self.popupID1 = wxNewId()
            self.popupID2 = wxNewId()
            self.popupID3 = wxNewId()
            self.popupID4 = wxNewId()
            self.popupID5 = wxNewId()
            EVT_MENU(self, self.popupID1, self.OnPopupOne)
            EVT_MENU(self, self.popupID2, self.OnPopupTwo)
            EVT_MENU(self, self.popupID3, self.OnPopupThree)
            EVT_MENU(self, self.popupID4, self.OnPopupFour)
            EVT_MENU(self, self.popupID5, self.OnPopupFive)

        # make a menu
        menu = wxMenu()
        # Show how to put an icon in the menu
        item = wxMenuItem(menu, self.popupID1,"One")
        item.SetBitmap(images.getSmilesBitmap())
        menu.AppendItem(item)
        # add some other items
        menu.Append(self.popupID2, "Two")
        menu.Append(self.popupID3, "ClearAll and repopulate")
        menu.Append(self.popupID4, "DeleteAllItems")
        menu.Append(self.popupID5, "GetItem")

        # Popup the menu.  If an item is selected then its handler
        # will be called before PopupMenu returns.
        self.PopupMenu(menu, wxPoint(self.x, self.y))
        menu.Destroy()


    def OnPopupOne(self, event):
        self.log.WriteText("Popup one\n")
        print "FindItem:", self.list.FindItem(-1, "Roxette")
        print "FindItemData:", self.list.FindItemData(-1, 11)

    def OnPopupTwo(self, event):
        self.log.WriteText("Popup two\n")

    def OnPopupThree(self, event):
        self.log.WriteText("Popup three\n")
        self.list.ClearAll()
        wxCallAfter(self.PopulateList)
        #wxYield()
        #self.PopulateList()

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

"""





if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

