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
import locale

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

        #--- Internationalization of string sorting with locale module
        if type(item1) == type('') or type(item2) == type(''):
            cmpVal = locale.strcoll(str(item1), str(item2))
        else:
            cmpVal = cmp(item1, item2)
        #---

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
#----------------------------------------------------------------------------

class wxListCtrlAutoWidthMixin:
    """ A mix-in class that automatically resizes the last column to take up
        the remaining width of the wxListCtrl.

        This causes the wxListCtrl to automatically take up the full width of
        the list, without either a horizontal scroll bar (unless absolutely
        necessary) or empty space to the right of the last column.

        NOTE:    This only works for report-style lists.

        WARNING: If you override the EVT_SIZE event in your wxListCtrl, make
                 sure you call event.Skip() to ensure that the mixin's
                 _OnResize method is called.

        This mix-in class was written by Erik Westra <ewestra@wave.co.nz>
"""
    def __init__(self):
        """ Standard initialiser.
        """
        self._lastColMinWidth = None

        EVT_SIZE(self, self._onResize)
        EVT_LIST_COL_END_DRAG(self, self.GetId(), self._onResize)


    def resizeLastColumn(self, minWidth):
        """ Resize the last column appropriately.

            If the list's columns are too wide to fit within the window, we use
            a horizontal scrollbar.  Otherwise, we expand the right-most column
            to take up the remaining free space in the list.

            This method is called automatically when the wxListCtrl is resized;
            you can also call it yourself whenever you want the last column to
            be resized appropriately (eg, when adding, removing or resizing
            columns).

            'minWidth' is the preferred minimum width for the last column.
        """
        self._lastColMinWidth = minWidth
        self._doResize()

    # =====================
    # == Private Methods ==
    # =====================

    def _onResize(self, event):
        """ Respond to the wxListCtrl being resized.

            We automatically resize the last column in the list.
        """
        wxCallAfter(self._doResize)
        event.Skip()


    def _doResize(self):
        """ Resize the last column as appropriate.

            If the list's columns are too wide to fit within the window, we use
            a horizontal scrollbar.  Otherwise, we expand the right-most column
            to take up the remaining free space in the list.

            We remember the current size of the last column, before resizing,
            as the preferred minimum width if we haven't previously been given
            or calculated a minimum width.  This ensure that repeated calls to
            _doResize() don't cause the last column to size itself too large.
        """
        numCols = self.GetColumnCount()
        if numCols == 0: return # Nothing to resize.

        if self._lastColMinWidth == None:
            self._lastColMinWidth = self.GetColumnWidth(numCols - 1)

        # We're showing the vertical scrollbar -> allow for scrollbar width
        # NOTE: on GTK, the scrollbar is included in the client size, but on
        # Windows it is not included
        listWidth = self.GetClientSize().width
        if wxPlatform != '__WXMSW__':
            if self.GetItemCount() > self.GetCountPerPage():
                scrollWidth = wxSystemSettings_GetSystemMetric(wxSYS_VSCROLL_X)
                listWidth = listWidth - scrollWidth

        totColWidth = 0 # Width of all columns except last one.
        for col in range(numCols-1):
            totColWidth = totColWidth + self.GetColumnWidth(col)

        lastColWidth = self.GetColumnWidth(numCols - 1)

        if totColWidth + self._lastColMinWidth > listWidth:
            # We haven't got the width to show the last column at its minimum
            # width -> set it to its minimum width and allow the horizontal
            # scrollbar to show.
            self.SetColumnWidth(numCols-1, self._lastColMinWidth)
            return

        # Resize the last column to take up the remaining available space.

        self.SetColumnWidth(numCols-1, listWidth - totColWidth)



#----------------------------------------------------------------------------

SEL_FOC = wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED
def selectBeforePopup(event):
    """Ensures the item the mouse is pointing at is selected before a popup.

    Works with both single-select and multi-select lists."""
    ctrl = event.GetEventObject()
    if isinstance(ctrl, wxListCtrl):
        n, flags = ctrl.HitTest(event.GetPosition())
        if n >= 0:
            if not ctrl.GetItemState(n, wxLIST_STATE_SELECTED):
                for i in range(ctrl.GetItemCount()):
                    ctrl.SetItemState(i, 0, SEL_FOC)
                #for i in getListCtrlSelection(ctrl, SEL_FOC):
                #    ctrl.SetItemState(i, 0, SEL_FOC)
                ctrl.SetItemState(n, SEL_FOC, SEL_FOC)

def getListCtrlSelection(listctrl, state=wxLIST_STATE_SELECTED):
    """ Returns list of item indexes of given state (selected by defaults) """
    res = []
    idx = -1
    while 1:
        idx = listctrl.GetNextItem(idx, wxLIST_NEXT_ALL, state)
        if idx == -1:
            break
        res.append(idx)
    return res

class ListCtrlSelectionManagerMix:
    """Mixin that defines a platform independent selection policy

    As selection single and multi-select list return the item index or a
    list of item indexes respectively.
    """
    wxEVT_DOPOPUPMENU = wxNewId()
    _menu = None

    def __init__(self):
        EVT_RIGHT_DOWN(self, self.OnLCSMRightDown)
        self.Connect(-1, -1, self.wxEVT_DOPOPUPMENU, self.OnLCSMDoPopup)

    def getPopupMenu(self):
        """ Override to implement dynamic menus (create) """
        return self._menu

    def setPopupMenu(self, menu):
        """ Must be set for default behaviour """
        self._menu = menu

    def afterPopupMenu(self, menu):
        """ Override to implement dynamic menus (destroy) """
        pass

    def getSelection(self):
        res = getListCtrlSelection(self)
        if self.GetWindowStyleFlag() & wxLC_SINGLE_SEL:
            if res:
                return res[0]
            else:
                return -1
        else:
            return res

    def OnLCSMRightDown(self, event):
        selectBeforePopup(event)
        event.Skip()
        menu = self.getPopupMenu()
        if menu:
            evt = wxPyEvent()
            evt.SetEventType(self.wxEVT_DOPOPUPMENU)
            evt.menu = menu
            evt.pos = event.GetPosition()
            wxPostEvent(self, evt)

    def OnLCSMDoPopup(self, event):
        self.PopupMenu(event.menu, event.pos)
        self.afterPopupMenu(event.menu)

#----------------------------------------------------------------------
