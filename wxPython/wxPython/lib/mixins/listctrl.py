#----------------------------------------------------------------------------
# Name:        wxPython.lib.mixins.listctrl
# Purpose:     Helpful mix-in classes for wxListCtrl
#
# Author:      Robin Dunn
#
# Created:     15-May-2001
# RCS-ID:      $Id$
# Copyright:   (c) 2001 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------------

from wxPython.wx import *

#----------------------------------------------------------------------------

class wxColumnSorterMixin:
    """
    A mixin class that handles sorting of a wxListCtrl in REPORT mode when
    the coilumn header is clicked on.

    There are a few requirments needed in order for this to work genericly:

      1. The combined class must have a GetListCtrl method that
         returns the wxListCtrl to be sorted, and the list control
         must exist at the time the wxColumnSorterMixin.__init__
         method is called because it uses GetListCtrl.

      2. Items in the list control must have a unique data value set
         with list.SetItemData.

      3. The combined class must have an attribute named itemDataMap
         that is a dictionary mapping the data values to a sequence of
         objects representing the values in each column.  These values
         are compared in the column sorter to determine sort order.

    """
    def __init__(self, numColumns):
        self._colSortFlag = [0] * numColumns
        self._col = 0
        self._colSortFlag[self._col] = 1

        list = self.GetListCtrl()
        if not list:
            raise ValueError, "No wxListCtrl available"
        EVT_LIST_COL_CLICK(list, list.GetId(), self.OnColClick)


    def SortListItems(self, col=-1, ascending=1):
        """Sort the list on demand.  Can also be used to set the sort column and order."""
        if col != -1:
            self._col = col
            self._colSortFlag[col] = ascending
        self.GetListCtrl().SortItems(self.ColumnSorter)


    def OnColClick(self, evt):
        self._col = col = evt.m_col
        self._colSortFlag[col] = not self._colSortFlag[col]
        self.GetListCtrl().SortItems(self.ColumnSorter)


    def ColumnSorter(self, key1, key2):
        col = self._col
        sortFlag = self._colSortFlag[col]
        item1 = self.itemDataMap[key1][col]
        item2 = self.itemDataMap[key2][col]
        if item1 == item2:  return 0
        if sortFlag:
            if item1 < item2: return -1
            else:             return 1
        else:
            if item1 > item2: return -1
            else:             return 1


    def GetColumnWidths(self):
        list = self.GetListCtrl()
        rv = []
        for x in range(len(self._colSortFlag)):
            rv.append(list.GetColumnWidth(x))
        return rv

#----------------------------------------------------------------------------






