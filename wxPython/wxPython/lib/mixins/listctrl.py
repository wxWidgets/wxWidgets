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
    the column header is clicked on.

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

    Interesting methods to override are GetColumnSorter,
    GetSecondarySortValues, and GetSortImages.  See below for details.
    """

    def __init__(self, numColumns):
        self._colSortFlag = [0] * numColumns
        self._col = -1

        list = self.GetListCtrl()
        if not list:
            raise ValueError, "No wxListCtrl available"
        EVT_LIST_COL_CLICK(list, list.GetId(), self.__OnColClick)


    def SortListItems(self, col=-1, ascending=1):
        """Sort the list on demand.  Can also be used to set the sort column and order."""
        oldCol = self._col
        if col != -1:
            self._col = col
            self._colSortFlag[col] = ascending
        self.GetListCtrl().SortItems(self.GetColumnSorter())
        self.__updateImages(oldCol)


    def GetColumnWidths(self):
        """
        Returns a list of column widths.  Can be used to help restore the current
        view later.
        """
        list = self.GetListCtrl()
        rv = []
        for x in range(len(self._colSortFlag)):
            rv.append(list.GetColumnWidth(x))
        return rv


    def GetSortImages(self):
        """
        Returns a tuple of image list indexesthe indexes in the image list for an image to be put on the column
        header when sorting in descending order.
        """
        return (-1, -1)  # (decending, ascending) image IDs


    def GetColumnSorter(self):
        """Returns a callable object to be used for comparing column values when sorting."""
        return self.__ColumnSorter


    def GetSecondarySortValues(self, col, key1, key2):
        """Returns a tuple of 2 values to use for secondary sort values when the
           items in the selected column match equal.  The default just returns the
           item data values."""
        return (key1, key2)


    def __OnColClick(self, evt):
        oldCol = self._col
        self._col = col = evt.GetColumn()
        self._colSortFlag[col] = not self._colSortFlag[col]
        self.GetListCtrl().SortItems(self.GetColumnSorter())
        self.__updateImages(oldCol)
        evt.Skip()


    def __ColumnSorter(self, key1, key2):
        col = self._col
        ascending = self._colSortFlag[col]
        item1 = self.itemDataMap[key1][col]
        item2 = self.itemDataMap[key2][col]

        cmpVal = cmp(item1, item2)

        # If the items are equal then pick something else to make the sort value unique
        if cmpVal == 0:
            cmpVal = apply(cmp, self.GetSecondarySortValues(col, key1, key2))

        if ascending:
            return cmpVal
        else:
            return -cmpVal


    def __updateImages(self, oldCol):
        sortImages = self.GetSortImages()
        if self._col != -1 and sortImages[0] != -1:
            img = sortImages[self._colSortFlag[self._col]]
            list = self.GetListCtrl()
            if oldCol != -1:
                list.ClearColumnImage(oldCol)
            list.SetColumnImage(self._col, img)


#----------------------------------------------------------------------------






