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
# 12/14/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 compatability update.
# o ListCtrlSelectionManagerMix untested.
#
# 12/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wxColumnSorterMixin -> ColumnSorterMixin 
# o wxListCtrlAutoWidthMixin -> ListCtrlAutoWidthMixin
# ...
# 13/10/2004 - Pim Van Heuven (pim@think-wize.com)
# o wxTextEditMixin: Support Horizontal scrolling when TAB is pressed on long
#       ListCtrls, support for WXK_DOWN, WXK_UP, performance improvements on
#       very long ListCtrls, Support for virtual ListCtrls
#
# 15-Oct-2004 - Robin Dunn
# o wxTextEditMixin: Added Shift-TAB support
#

import  locale
import  wx

#----------------------------------------------------------------------------

class ColumnSorterMixin:
    """
    A mixin class that handles sorting of a wx.ListCtrl in REPORT mode when
    the column header is clicked on.

    There are a few requirments needed in order for this to work genericly:

      1. The combined class must have a GetListCtrl method that
         returns the wx.ListCtrl to be sorted, and the list control
         must exist at the time the wx.ColumnSorterMixin.__init__
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
        self.SetColumnCount(numColumns)
        list = self.GetListCtrl()
        if not list:
            raise ValueError, "No wx.ListCtrl available"
        list.Bind(wx.EVT_LIST_COL_CLICK, self.__OnColClick, list)


    def SetColumnCount(self, newNumColumns):
        self._colSortFlag = [0] * newNumColumns
        self._col = -1


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
        self._colSortFlag[col] = int(not self._colSortFlag[col])
        self.GetListCtrl().SortItems(self.GetColumnSorter())
        if wx.Platform != "__WXMAC__" or wx.SystemOptions.GetOptionInt("mac.listctrl.always_use_generic") == 1:
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

class ListCtrlAutoWidthMixin:
    """ A mix-in class that automatically resizes the last column to take up
        the remaining width of the wx.ListCtrl.

        This causes the wx.ListCtrl to automatically take up the full width of
        the list, without either a horizontal scroll bar (unless absolutely
        necessary) or empty space to the right of the last column.

        NOTE:    This only works for report-style lists.

        WARNING: If you override the EVT_SIZE event in your wx.ListCtrl, make
                 sure you call event.Skip() to ensure that the mixin's
                 _OnResize method is called.

        This mix-in class was written by Erik Westra <ewestra@wave.co.nz>
    """
    def __init__(self):
        """ Standard initialiser.
        """
        self._resizeColMinWidth = None
        self._resizeColStyle = "LAST"
        self._resizeCol = 0
        self.Bind(wx.EVT_SIZE, self._onResize)
        self.Bind(wx.EVT_LIST_COL_END_DRAG, self._onResize, self)


    def setResizeColumn(self, col):
        """
        Specify which column that should be autosized.  Pass either
        'LAST' or the column number.  Default is 'LAST'.
        """
        if col == "LAST":
            self._resizeColStyle = "LAST"
        else:
            self._resizeColStyle = "COL"
            self._resizeCol = col
        

    def resizeLastColumn(self, minWidth):
        """ Resize the last column appropriately.

            If the list's columns are too wide to fit within the window, we use
            a horizontal scrollbar.  Otherwise, we expand the right-most column
            to take up the remaining free space in the list.

            This method is called automatically when the wx.ListCtrl is resized;
            you can also call it yourself whenever you want the last column to
            be resized appropriately (eg, when adding, removing or resizing
            columns).

            'minWidth' is the preferred minimum width for the last column.
        """
        self.resizeColumn(minWidth)


    def resizeColumn(self, minWidth):
        self._resizeColMinWidth = minWidth
        self._doResize()
        

    # =====================
    # == Private Methods ==
    # =====================

    def _onResize(self, event):
        """ Respond to the wx.ListCtrl being resized.

            We automatically resize the last column in the list.
        """
        if 'gtk2' in wx.PlatformInfo:
            self._doResize()
        else:
            wx.CallAfter(self._doResize)
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
        
        if not self:  # avoid a PyDeadObject error
            return

        if self.GetSize().height < 32:
            return  # avoid an endless update bug when the height is small.
        
        numCols = self.GetColumnCount()
        if numCols == 0: return # Nothing to resize.

        if(self._resizeColStyle == "LAST"):
            resizeCol = self.GetColumnCount()
        else:
            resizeCol = self._resizeCol

        if self._resizeColMinWidth == None:
            self._resizeColMinWidth = self.GetColumnWidth(resizeCol - 1)

        # We're showing the vertical scrollbar -> allow for scrollbar width
        # NOTE: on GTK, the scrollbar is included in the client size, but on
        # Windows it is not included
        listWidth = self.GetClientSize().width
        if wx.Platform != '__WXMSW__':
            if self.GetItemCount() > self.GetCountPerPage():
                scrollWidth = wx.SystemSettings_GetMetric(wx.SYS_VSCROLL_X)
                listWidth = listWidth - scrollWidth

        totColWidth = 0 # Width of all columns except last one.
        for col in range(numCols):
            if col != (resizeCol-1):
                totColWidth = totColWidth + self.GetColumnWidth(col)

        resizeColWidth = self.GetColumnWidth(resizeCol - 1)

        if totColWidth + self._resizeColMinWidth > listWidth:
            # We haven't got the width to show the last column at its minimum
            # width -> set it to its minimum width and allow the horizontal
            # scrollbar to show.
            self.SetColumnWidth(resizeCol-1, self._resizeColMinWidth)
            return

        # Resize the last column to take up the remaining available space.

        self.SetColumnWidth(resizeCol-1, listWidth - totColWidth)




#----------------------------------------------------------------------------
#----------------------------------------------------------------------------

SEL_FOC = wx.LIST_STATE_SELECTED | wx.LIST_STATE_FOCUSED
def selectBeforePopup(event):
    """Ensures the item the mouse is pointing at is selected before a popup.

    Works with both single-select and multi-select lists."""
    ctrl = event.GetEventObject()
    if isinstance(ctrl, wx.ListCtrl):
        n, flags = ctrl.HitTest(event.GetPosition())
        if n >= 0:
            if not ctrl.GetItemState(n, wx.LIST_STATE_SELECTED):
                for i in range(ctrl.GetItemCount()):
                    ctrl.SetItemState(i, 0, SEL_FOC)
                #for i in getListCtrlSelection(ctrl, SEL_FOC):
                #    ctrl.SetItemState(i, 0, SEL_FOC)
                ctrl.SetItemState(n, SEL_FOC, SEL_FOC)


def getListCtrlSelection(listctrl, state=wx.LIST_STATE_SELECTED):
    """ Returns list of item indexes of given state (selected by defaults) """
    res = []
    idx = -1
    while 1:
        idx = listctrl.GetNextItem(idx, wx.LIST_NEXT_ALL, state)
        if idx == -1:
            break
        res.append(idx)
    return res

wxEVT_DOPOPUPMENU = wx.NewEventType()
EVT_DOPOPUPMENU = wx.PyEventBinder(wxEVT_DOPOPUPMENU, 0)


class ListCtrlSelectionManagerMix:
    """Mixin that defines a platform independent selection policy

    As selection single and multi-select list return the item index or a
    list of item indexes respectively.
    """
    _menu = None

    def __init__(self):
        self.Bind(wx.EVT_RIGHT_DOWN, self.OnLCSMRightDown)
        self.Bind(EVT_DOPOPUPMENU, self.OnLCSMDoPopup)
#        self.Connect(-1, -1, self.wxEVT_DOPOPUPMENU, self.OnLCSMDoPopup)


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
        if self.GetWindowStyleFlag() & wx.LC_SINGLE_SEL:
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
            evt = wx.PyEvent()
            evt.SetEventType(wxEVT_DOPOPUPMENU)
            evt.menu = menu
            evt.pos = event.GetPosition()
            wx.PostEvent(self, evt)


    def OnLCSMDoPopup(self, event):
        self.PopupMenu(event.menu, event.pos)
        self.afterPopupMenu(event.menu)


#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
from bisect import bisect


class TextEditMixin:
    """    
    A mixin class that enables any text in any column of a
    multi-column listctrl to be edited by clicking on the given row
    and column.  You close the text editor by hitting the ENTER key or
    clicking somewhere else on the listctrl. You switch to the next
    column by hiting TAB.

    To use the mixin you have to include it in the class definition
    and call the __init__ function::

        class TestListCtrl(wx.ListCtrl, TextEditMixin):
            def __init__(self, parent, ID, pos=wx.DefaultPosition,
                         size=wx.DefaultSize, style=0):
                wx.ListCtrl.__init__(self, parent, ID, pos, size, style)
                TextEditMixin.__init__(self) 


    Authors:     Steve Zatz, Pim Van Heuven (pim@think-wize.com)
    """

    editorBgColour = wx.Colour(255,255,175) # Yellow
    editorFgColour = wx.Colour(0,0,0)       # black
        
    def __init__(self):
        #editor = wx.TextCtrl(self, -1, pos=(-1,-1), size=(-1,-1),
        #                     style=wx.TE_PROCESS_ENTER|wx.TE_PROCESS_TAB \
        #                     |wx.TE_RICH2)

        self.make_editor()
        self.Bind(wx.EVT_TEXT_ENTER, self.CloseEditor)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        self.Bind(wx.EVT_LEFT_DCLICK, self.OnLeftDown)
        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.OnItemSelected)


    def make_editor(self, col_style=wx.LIST_FORMAT_LEFT):
        
        style =wx.TE_PROCESS_ENTER|wx.TE_PROCESS_TAB|wx.TE_RICH2
        style |= {wx.LIST_FORMAT_LEFT: wx.TE_LEFT,
                  wx.LIST_FORMAT_RIGHT: wx.TE_RIGHT,
                  wx.LIST_FORMAT_CENTRE : wx.TE_CENTRE
                  }[col_style]
        
        editor = wx.TextCtrl(self, -1, style=style)
        editor.SetBackgroundColour(self.editorBgColour)
        editor.SetForegroundColour(self.editorFgColour)
        font = self.GetFont()
        editor.SetFont(font)

        self.curRow = 0
        self.curCol = 0

        editor.Hide()
        if hasattr(self, 'editor'):
            self.editor.Destroy()
        self.editor = editor

        self.col_style = col_style
        self.editor.Bind(wx.EVT_CHAR, self.OnChar)
        self.editor.Bind(wx.EVT_KILL_FOCUS, self.CloseEditor)
        
        
    def OnItemSelected(self, evt):
        self.curRow = evt.GetIndex()
        evt.Skip()
        

    def OnChar(self, event):
        ''' Catch the TAB, Shift-TAB, cursor DOWN/UP key code
            so we can open the editor at the next column (if any).'''

        keycode = event.GetKeyCode()
        if keycode == wx.WXK_TAB and event.ShiftDown():
            self.CloseEditor()
            if self.curCol-1 >= 0:
                self.OpenEditor(self.curCol-1, self.curRow)
            
        elif keycode == wx.WXK_TAB:
            self.CloseEditor()
            if self.curCol+1 < self.GetColumnCount():
                self.OpenEditor(self.curCol+1, self.curRow)

        elif keycode == wx.WXK_ESCAPE:
            self.CloseEditor()

        elif keycode == wx.WXK_DOWN:
            self.CloseEditor()
            if self.curRow+1 < self.GetItemCount():
                self._SelectIndex(self.curRow+1)
                self.OpenEditor(self.curCol, self.curRow)

        elif keycode == wx.WXK_UP:
            self.CloseEditor()
            if self.curRow > 0:
                self._SelectIndex(self.curRow-1)
                self.OpenEditor(self.curCol, self.curRow)
            
        else:
            event.Skip()

    
    def OnLeftDown(self, evt=None):
        ''' Examine the click and double
        click events to see if a row has been click on twice. If so,
        determine the current row and columnn and open the editor.'''
        
        if self.editor.IsShown():
            self.CloseEditor()
            
        x,y = evt.GetPosition()
        row,flags = self.HitTest((x,y))
    
        if row != self.curRow: # self.curRow keeps track of the current row
            evt.Skip()
            return
        
        # the following should really be done in the mixin's init but
        # the wx.ListCtrl demo creates the columns after creating the
        # ListCtrl (generally not a good idea) on the other hand,
        # doing this here handles adjustable column widths
        
        self.col_locs = [0]
        loc = 0
        for n in range(self.GetColumnCount()):
            loc = loc + self.GetColumnWidth(n)
            self.col_locs.append(loc)

        
        col = bisect(self.col_locs, x+self.GetScrollPos(wx.HORIZONTAL)) - 1
        self.OpenEditor(col, row)


    def OpenEditor(self, col, row):
        ''' Opens an editor at the current position. '''

        # give the derived class a chance to Allow/Veto this edit.
        evt = wx.ListEvent(wx.wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT, self.GetId())
        evt.m_itemIndex = row
        evt.m_col = col
        item = self.GetItem(row, col)
        evt.m_item.SetId(item.GetId()) 
        evt.m_item.SetColumn(item.GetColumn()) 
        evt.m_item.SetData(item.GetData()) 
        evt.m_item.SetText(item.GetText()) 
        ret = self.GetEventHandler().ProcessEvent(evt)
        if ret and not evt.IsAllowed():
            return   # user code doesn't allow the edit.

        if self.GetColumn(col).m_format != self.col_style:
            self.make_editor(self.GetColumn(col).m_format)
    
        x0 = self.col_locs[col]
        x1 = self.col_locs[col+1] - x0

        scrolloffset = self.GetScrollPos(wx.HORIZONTAL)

        # scroll forward
        if x0+x1-scrolloffset > self.GetSize()[0]:
            if wx.Platform == "__WXMSW__":
                # don't start scrolling unless we really need to
                offset = x0+x1-self.GetSize()[0]-scrolloffset
                # scroll a bit more than what is minimum required
                # so we don't have to scroll everytime the user presses TAB
                # which is very tireing to the eye
                addoffset = self.GetSize()[0]/4
                # but be careful at the end of the list
                if addoffset + scrolloffset < self.GetSize()[0]:
                    offset += addoffset

                self.ScrollList(offset, 0)
                scrolloffset = self.GetScrollPos(wx.HORIZONTAL)
            else:
                # Since we can not programmatically scroll the ListCtrl
                # close the editor so the user can scroll and open the editor
                # again
                self.editor.SetValue(self.GetItem(row, col).GetText())
                self.curRow = row
                self.curCol = col
                self.CloseEditor()
                return

        y0 = self.GetItemRect(row)[1]
        
        editor = self.editor
        editor.SetDimensions(x0-scrolloffset,y0, x1,-1)
        
        editor.SetValue(self.GetItem(row, col).GetText()) 
        editor.Show()
        editor.Raise()
        editor.SetSelection(-1,-1)
        editor.SetFocus()
    
        self.curRow = row
        self.curCol = col

    
    # FIXME: this function is usually called twice - second time because
    # it is binded to wx.EVT_KILL_FOCUS. Can it be avoided? (MW)
    def CloseEditor(self, evt=None):
        ''' Close the editor and save the new value to the ListCtrl. '''
        if not self.editor.IsShown():
            return
        text = self.editor.GetValue()
        self.editor.Hide()
        self.SetFocus()
        
        # post wxEVT_COMMAND_LIST_END_LABEL_EDIT
        # Event can be vetoed. It doesn't has SetEditCanceled(), what would 
        # require passing extra argument to CloseEditor() 
        evt = wx.ListEvent(wx.wxEVT_COMMAND_LIST_END_LABEL_EDIT, self.GetId())
        evt.m_itemIndex = self.curRow
        evt.m_col = self.curCol
        item = self.GetItem(self.curRow, self.curCol)
        evt.m_item.SetId(item.GetId()) 
        evt.m_item.SetColumn(item.GetColumn()) 
        evt.m_item.SetData(item.GetData()) 
        evt.m_item.SetText(text) #should be empty string if editor was canceled
        ret = self.GetEventHandler().ProcessEvent(evt)
        if not ret or evt.IsAllowed():
            if self.IsVirtual():
                # replace by whather you use to populate the virtual ListCtrl
                # data source
                self.SetVirtualData(self.curRow, self.curCol, text)
            else:
                self.SetStringItem(self.curRow, self.curCol, text)
        self.RefreshItem(self.curRow)

    def _SelectIndex(self, row):
        listlen = self.GetItemCount()
        if row < 0 and not listlen:
            return
        if row > (listlen-1):
            row = listlen -1
            
        self.SetItemState(self.curRow, ~wx.LIST_STATE_SELECTED,
                          wx.LIST_STATE_SELECTED)
        self.EnsureVisible(row)
        self.SetItemState(row, wx.LIST_STATE_SELECTED,
                          wx.LIST_STATE_SELECTED)



#----------------------------------------------------------------------------
#----------------------------------------------------------------------------

"""
FILENAME: CheckListCtrlMixin.py
AUTHOR:   Bruce Who (bruce.who.hk at gmail.com)
DATE:     2006-02-09
$Revision$
DESCRIPTION:
    This script provide a mixin for ListCtrl which add a checkbox in the first
    column of each row. It is inspired by limodou's CheckList.py(which can be
    got from his NewEdit) and improved:
        - You can just use InsertStringItem() to insert new items;
        - Once a checkbox is checked/unchecked, the corresponding item is not
          selected;
        - You can use SetItemData() and GetItemData();
        - Interfaces are changed to OnCheckItem(), IsChecked(), CheckItem().

    You should not set a imagelist for the ListCtrl once this mixin is used.

HISTORY:
1.3     - You can check/uncheck a group of sequential items by <Shift-click>:
          First click(or <Shift-Click>) item1 to check/uncheck it, then
          Shift-click item2 to check/uncheck it, and you'll find that all
          items between item1 and item2 are check/unchecked!
1.2     - Add ToggleItem()
1.1     - Initial version
"""

from wx import ImageFromStream, BitmapFromImage
import cStringIO, zlib

def getUncheckData():
    return zlib.decompress(
"x\xda\xeb\x0c\xf0s\xe7\xe5\x92\xe2b``\xe0\xf5\xf4p\t\x02\xd2\x02 \xcc\xc1\
\x06$\xe5?\xffO\x04R,\xc5N\x9e!\x1c@P\xc3\x91\xd2\x01\xe4\xbb{\xba8\x86X\xf4\
&\xa7\xa4$\xa5-`1\x08\\2\xbb\xb1\xb1\x91\xf5\xd8\x84o\xeb\xff\xfaw\x1d[.=[2\
\x90'\x01\x08v\xec]\xd3\xa3qvU`l\x81\xd9\xd18\t\xd3\x84+\x0cll[\xa6t\xcc9\
\xd4\xc1\xda\xc3<O\x9a1\xc3\x88\xc3j\xfa\x86_\xee@#\x19<]\xfd\\\xd69%4\x01\
\x00\xdc\x80-\x05" )

def getUncheckBitmap():
    return BitmapFromImage(getUncheckImage())

def getUncheckImage():
    stream = cStringIO.StringIO(getUncheckData())
    return ImageFromStream(stream)

def getCheckData():
    return zlib.decompress(
'x\xda\xeb\x0c\xf0s\xe7\xe5\x92\xe2b``\xe0\xf5\xf4p\t\x02\xd2\x02 \xcc\xc1\
\x06$\xe5?\xffO\x04R,\xc5N\x9e!\x1c@P\xc3\x91\xd2\x01\xe47{\xba8\x86X\xf4&\
\xa7\xa4$\xa5-`1\x08\\2\xbb\xb1\xb1\x91\xf5\xd8\x84o\xeb\xff\xfaw\x1d[.=[2\
\x90\'\x01\x08v\xec\\2C\xe3\xec+\xc3\xbd\x05fG\xe3\x14n1\xcc5\xad\x8a8\x1a\
\xb9\xa1\xeb\xd1\x853-\xaa\xc76\xecb\xb8i\x16c&\\\xc2\xb8\xe9Xvbx\xa1T\xc3U\
\xd6p\'\xbd\x85\x19\xff\xbe\xbf\xd7\xe7R\xcb`\xd8\xa5\xf8\x83\xe1^\xc4\x0e\
\xa1"\xce\xc3n\x93x\x14\xd8\x16\xb0(\x15q)\x8b\x19\xf0U\xe4\xb10\x08V\xa8\
\x99\xf3\xdd\xde\xad\x06t\x0e\x83\xa7\xab\x9f\xcb:\xa7\x84&\x00\xe0HE\xab' )

def getCheckBitmap():
    return BitmapFromImage(getCheckImage())

def getCheckImage():
    stream = cStringIO.StringIO(getCheckData())
    return ImageFromStream(stream)



class CheckListCtrlMixin:
    """
    This is a mixin for ListCtrl which add a checkbox in the first
    column of each row. It is inspired by limodou's CheckList.py(which
    can be got from his NewEdit) and improved:
    
        - You can just use InsertStringItem() to insert new items;

        - Once a checkbox is checked/unchecked, the corresponding item
          is not selected;

        - You can use SetItemData() and GetItemData();

        - Interfaces are changed to OnCheckItem(), IsChecked(),
          CheckItem().

    You should not set a imagelist for the ListCtrl once this mixin is used.
    """
    def __init__(self, check_image=None, uncheck_image=None):
        self.__imagelist_ = wx.ImageList(16, 16)
        if not check_image:
            check_image = getCheckBitmap()
        if not uncheck_image:
            uncheck_image = getUncheckBitmap()
        self.uncheck_image = self.__imagelist_.Add(uncheck_image)
        self.check_image = self.__imagelist_.Add(check_image)
        self.SetImageList(self.__imagelist_, wx.IMAGE_LIST_SMALL)
        self.__last_check_ = None

        self.Bind(wx.EVT_LEFT_DOWN, self.__OnLeftDown_)
        
        # override the default methods of ListCtrl/ListView
        self.InsertStringItem = self.__InsertStringItem_

    # NOTE: if you use InsertItem, InsertImageItem or InsertImageStringItem,
    #       you must set the image yourself.
    def __InsertStringItem_(self, index, label):
        index = self.InsertImageStringItem(index, label, 0)
        return index

    def __OnLeftDown_(self, evt):
        (index, flags) = self.HitTest(evt.GetPosition())
        if flags == wx.LIST_HITTEST_ONITEMICON:
            img_idx = self.GetItem(index).GetImage()
            flag_check = img_idx == 0
            begin_index = index
            end_index = index
            if self.__last_check_ is not None \
                    and wx.GetKeyState(wx.WXK_SHIFT):
                last_index, last_flag_check = self.__last_check_
                if last_flag_check == flag_check:
                    # XXX what if the previous item is deleted or new items
                    # are inserted?
                    item_count = self.GetItemCount()
                    if last_index < item_count:
                        if last_index < index:
                            begin_index = last_index
                            end_index = index
                        elif last_index > index:
                            begin_index = index
                            end_index = last_index
                        else:
                            assert False
            while begin_index <= end_index:
                self.CheckItem(begin_index, flag_check)
                begin_index += 1
            self.__last_check_ = (index, flag_check)
        else:
            evt.Skip()

    def OnCheckItem(self, index, flag):
        pass

    def IsChecked(self, index):
        return self.GetItem(index).GetImage() == 1

    def CheckItem(self, index, check = True):
        img_idx = self.GetItem(index).GetImage()
        if img_idx == 0 and check is True:
            self.SetItemImage(index, 1)
            self.OnCheckItem(index, True)
        elif img_idx == 1 and check is False:
            self.SetItemImage(index, 0)
            self.OnCheckItem(index, False)

    def ToggleItem(self, index):
        self.CheckItem(index, not self.IsChecked(index))


#----------------------------------------------------------------------------
