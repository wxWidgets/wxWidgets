
import  wx
import  wx.grid as  Grid

import  images

#---------------------------------------------------------------------------

class MegaTable(Grid.PyGridTableBase):
    """
    A custom wx.Grid Table using user supplied data
    """
    def __init__(self, data, colnames, plugins):
        """data is a list of the form
        [(rowname, dictionary),
        dictionary.get(colname, None) returns the data for column
        colname
        """
        # The base class must be initialized *first*
        Grid.PyGridTableBase.__init__(self)
        self.data = data
        self.colnames = colnames
        self.plugins = plugins or {}
        # XXX
        # we need to store the row length and column length to
        # see if the table has changed size
        self._rows = self.GetNumberRows()
        self._cols = self.GetNumberCols()

    def GetNumberCols(self):
        return len(self.colnames)

    def GetNumberRows(self):
        return len(self.data)

    def GetColLabelValue(self, col):
        return self.colnames[col]

    def GetRowLabelValue(self, row):
        return "row %03d" % int(self.data[row][0])

    def GetValue(self, row, col):
        return str(self.data[row][1].get(self.GetColLabelValue(col), ""))

    def GetRawValue(self, row, col):
        return self.data[row][1].get(self.GetColLabelValue(col), "")

    def SetValue(self, row, col, value):
        self.data[row][1][self.GetColLabelValue(col)] = value

    def ResetView(self, grid):
        """
        (Grid) -> Reset the grid view.   Call this to
        update the grid if rows and columns have been added or deleted
        """
        grid.BeginBatch()

        for current, new, delmsg, addmsg in [
            (self._rows, self.GetNumberRows(), Grid.GRIDTABLE_NOTIFY_ROWS_DELETED, Grid.GRIDTABLE_NOTIFY_ROWS_APPENDED),
            (self._cols, self.GetNumberCols(), Grid.GRIDTABLE_NOTIFY_COLS_DELETED, Grid.GRIDTABLE_NOTIFY_COLS_APPENDED),
        ]:

            if new < current:
                msg = Grid.GridTableMessage(self,delmsg,new,current-new)
                grid.ProcessTableMessage(msg)
            elif new > current:
                msg = Grid.GridTableMessage(self,addmsg,new-current)
                grid.ProcessTableMessage(msg)
                self.UpdateValues(grid)

        grid.EndBatch()

        self._rows = self.GetNumberRows()
        self._cols = self.GetNumberCols()
        # update the column rendering plugins
        self._updateColAttrs(grid)

        # update the scrollbars and the displayed part of the grid
        grid.AdjustScrollbars()
        grid.ForceRefresh()


    def UpdateValues(self, grid):
        """Update all displayed values"""
        # This sends an event to the grid table to update all of the values
        msg = Grid.GridTableMessage(self, Grid.GRIDTABLE_REQUEST_VIEW_GET_VALUES)
        grid.ProcessTableMessage(msg)

    def _updateColAttrs(self, grid):
        """
        wx.Grid -> update the column attributes to add the
        appropriate renderer given the column name.  (renderers
        are stored in the self.plugins dictionary)

        Otherwise default to the default renderer.
        """
        col = 0

        for colname in self.colnames:
            attr = Grid.GridCellAttr()
            if colname in self.plugins:
                renderer = self.plugins[colname](self)

                if renderer.colSize:
                    grid.SetColSize(col, renderer.colSize)

                if renderer.rowSize:
                    grid.SetDefaultRowSize(renderer.rowSize)

                attr.SetReadOnly(True)
                attr.SetRenderer(renderer)

            grid.SetColAttr(col, attr)
            col += 1

    # ------------------------------------------------------
    # begin the added code to manipulate the table (non wx related)
    def AppendRow(self, row):
        #print 'append'
        entry = {}

        for name in self.colnames:
            entry[name] = "Appended_%i"%row

        # XXX Hack
        # entry["A"] can only be between 1..4
        entry["A"] = random.choice(range(4))
        self.data.insert(row, ["Append_%i"%row, entry])

    def DeleteCols(self, cols):
        """
        cols -> delete the columns from the dataset
        cols hold the column indices
        """
        # we'll cheat here and just remove the name from the
        # list of column names.  The data will remain but
        # it won't be shown
        deleteCount = 0
        cols = cols[:]
        cols.sort()

        for i in cols:
            self.colnames.pop(i-deleteCount)
            # we need to advance the delete count
            # to make sure we delete the right columns
            deleteCount += 1

        if not len(self.colnames):
            self.data = []

    def DeleteRows(self, rows):
        """
        rows -> delete the rows from the dataset
        rows hold the row indices
        """
        deleteCount = 0
        rows = rows[:]
        rows.sort()

        for i in rows:
            self.data.pop(i-deleteCount)
            # we need to advance the delete count
            # to make sure we delete the right rows
            deleteCount += 1

    def SortColumn(self, col):
        """
        col -> sort the data based on the column indexed by col
        """
        name = self.colnames[col]
        _data = []

        for row in self.data:
            rowname, entry = row
            _data.append((entry.get(name, None), row))

        _data.sort()
        self.data = []

        for sortvalue, row in _data:
            self.data.append(row)

    # end table manipulation code
    # ----------------------------------------------------------


# --------------------------------------------------------------------
# Sample wx.Grid renderers

class MegaImageRenderer(Grid.PyGridCellRenderer):
    def __init__(self, table):
        """
        Image Renderer Test.  This just places an image in a cell
        based on the row index.  There are N choices and the
        choice is made by  choice[row%N]
        """
        Grid.PyGridCellRenderer.__init__(self)
        self.table = table
        self._choices = [images.getSmilesBitmap,
                         images.getMondrianBitmap,
                         images.get_10s_Bitmap,
                         images.get_01c_Bitmap]

        self.colSize = None
        self.rowSize = None

    def Draw(self, grid, attr, dc, rect, row, col, isSelected):
        choice = self.table.GetRawValue(row, col)
        bmp = self._choices[ choice % len(self._choices)]()
        image = wx.MemoryDC()
        image.SelectObject(bmp)

        # clear the background
        dc.SetBackgroundMode(wx.SOLID)

        if isSelected:
            dc.SetBrush(wx.Brush(wx.BLUE, wx.SOLID))
            dc.SetPen(wx.Pen(wx.BLUE, 1, wx.SOLID))
        else:
            dc.SetBrush(wx.Brush(wx.WHITE, wx.SOLID))
            dc.SetPen(wx.Pen(wx.WHITE, 1, wx.SOLID))
        dc.DrawRectangleRect(rect)


        # copy the image but only to the size of the grid cell
        width, height = bmp.GetWidth(), bmp.GetHeight()

        if width > rect.width-2:
            width = rect.width-2

        if height > rect.height-2:
            height = rect.height-2

        dc.Blit(rect.x+1, rect.y+1, width, height,
                image,
                0, 0, wx.COPY, True)


class MegaFontRenderer(Grid.PyGridCellRenderer):
    def __init__(self, table, color="blue", font="ARIAL", fontsize=8):
        """Render data in the specified color and font and fontsize"""
        Grid.PyGridCellRenderer.__init__(self)
        self.table = table
        self.color = color
        self.font = wx.Font(fontsize, wx.DEFAULT, wx.NORMAL, wx.NORMAL, 0, font)
        self.selectedBrush = wx.Brush("blue", wx.SOLID)
        self.normalBrush = wx.Brush(wx.WHITE, wx.SOLID)
        self.colSize = None
        self.rowSize = 50

    def Draw(self, grid, attr, dc, rect, row, col, isSelected):
        # Here we draw text in a grid cell using various fonts
        # and colors.  We have to set the clipping region on
        # the grid's DC, otherwise the text will spill over
        # to the next cell
        dc.SetClippingRect(rect)

        # clear the background
        dc.SetBackgroundMode(wx.SOLID)
        
        if isSelected:
            dc.SetBrush(wx.Brush(wx.BLUE, wx.SOLID))
            dc.SetPen(wx.Pen(wx.BLUE, 1, wx.SOLID))
        else:
            dc.SetBrush(wx.Brush(wx.WHITE, wx.SOLID))
            dc.SetPen(wx.Pen(wx.WHITE, 1, wx.SOLID))
        dc.DrawRectangleRect(rect)

        text = self.table.GetValue(row, col)
        dc.SetBackgroundMode(wx.SOLID)

        # change the text background based on whether the grid is selected
        # or not
        if isSelected:
            dc.SetBrush(self.selectedBrush)
            dc.SetTextBackground("blue")
        else:
            dc.SetBrush(self.normalBrush)
            dc.SetTextBackground("white")

        dc.SetTextForeground(self.color)
        dc.SetFont(self.font)
        dc.DrawText(text, rect.x+1, rect.y+1)

        # Okay, now for the advanced class :)
        # Let's add three dots "..."
        # to indicate that that there is more text to be read
        # when the text is larger than the grid cell

        width, height = dc.GetTextExtent(text)
        
        if width > rect.width-2:
            width, height = dc.GetTextExtent("...")
            x = rect.x+1 + rect.width-2 - width
            dc.DrawRectangle(x, rect.y+1, width+1, height)
            dc.DrawText("...", x, rect.y+1)

        dc.DestroyClippingRegion()


# --------------------------------------------------------------------
# Sample Grid using a specialized table and renderers that can
# be plugged in based on column names

class MegaGrid(Grid.Grid):
    def __init__(self, parent, data, colnames, plugins=None):
        """parent, data, colnames, plugins=None
        Initialize a grid using the data defined in data and colnames
        (see MegaTable for a description of the data format)
        plugins is a dictionary of columnName -> column renderers.
        """

        # The base class must be initialized *first*
        Grid.Grid.__init__(self, parent, -1)
        self._table = MegaTable(data, colnames, plugins)
        self.SetTable(self._table)
        self._plugins = plugins

        self.Bind(Grid.EVT_GRID_LABEL_RIGHT_CLICK, self.OnLabelRightClicked)

    def Reset(self):
        """reset the view based on the data in the table.  Call
        this when rows are added or destroyed"""
        self._table.ResetView(self)

    def OnLabelRightClicked(self, evt):
        # Did we click on a row or a column?
        row, col = evt.GetRow(), evt.GetCol()
        if row == -1: self.colPopup(col, evt)
        elif col == -1: self.rowPopup(row, evt)

    def rowPopup(self, row, evt):
        """(row, evt) -> display a popup menu when a row label is right clicked"""
        appendID = wx.NewId()
        deleteID = wx.NewId()
        x = self.GetRowSize(row)/2

        if not self.GetSelectedRows():
            self.SelectRow(row)

        menu = wx.Menu()
        xo, yo = evt.GetPosition()
        menu.Append(appendID, "Append Row")
        menu.Append(deleteID, "Delete Row(s)")

        def append(event, self=self, row=row):
            self._table.AppendRow(row)
            self.Reset()

        def delete(event, self=self, row=row):
            rows = self.GetSelectedRows()
            self._table.DeleteRows(rows)
            self.Reset()

        self.Bind(wx.EVT_MENU, append, id=appendID)
        self.Bind(wx.EVT_MENU, delete, id=deleteID)
        self.PopupMenu(menu, (x, yo))
        menu.Destroy()
        return


    def colPopup(self, col, evt):
        """(col, evt) -> display a popup menu when a column label is
        right clicked"""
        x = self.GetColSize(col)/2
        menu = wx.Menu()
        id1 = wx.NewId()
        sortID = wx.NewId()

        xo, yo = evt.GetPosition()
        self.SelectCol(col)
        cols = self.GetSelectedCols()
        self.Refresh()
        menu.Append(id1, "Delete Col(s)")
        menu.Append(sortID, "Sort Column")

        def delete(event, self=self, col=col):
            cols = self.GetSelectedCols()
            self._table.DeleteCols(cols)
            self.Reset()

        def sort(event, self=self, col=col):
            self._table.SortColumn(col)
            self.Reset()

        self.Bind(wx.EVT_MENU, delete, id=id1)

        if len(cols) == 1:
            self.Bind(wx.EVT_MENU, sort, id=sortID)

        self.PopupMenu(menu, (xo, 0))
        menu.Destroy()
        return

# -----------------------------------------------------------------
# Test data
# data is in the form
# [rowname, dictionary]
# where dictionary.get(colname, None) -> returns the value for the cell
#
# the colname must also be supplied
import random
colnames = ["Row", "This", "Is", "A", "Test"]

data = []

for row in range(1000):
    d = {}
    for name in ["This", "Test", "Is"]:
        d[name] = random.random()

    d["Row"] = len(data)
    # XXX
    # the "A" column can only be between one and 4
    d["A"] = random.choice(range(4))
    data.append((str(row), d))

class MegaFontRendererFactory:
    def __init__(self, color, font, fontsize):
        """
        (color, font, fontsize) -> set of a factory to generate
        renderers when called.
        func = MegaFontRenderFactory(color, font, fontsize)
        renderer = func(table)
        """
        self.color = color
        self.font = font
        self.fontsize = fontsize

    def __call__(self, table):
        return MegaFontRenderer(table, self.color, self.font, self.fontsize)


#---------------------------------------------------------------------------

class TestFrame(wx.Frame):
    def __init__(self, parent, plugins={"This":MegaFontRendererFactory("red", "ARIAL", 8),
                                        "A":MegaImageRenderer,
                                        "Test":MegaFontRendererFactory("orange", "TIMES", 24),}):
        wx.Frame.__init__(self, parent, -1,
                         "Test Frame", size=(640,480))

        grid = MegaGrid(self, data, colnames, plugins)
        grid.Reset()


#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Show the MegaGrid", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        win = TestFrame(self)
        win.Show(True)

#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win



overview = """Mega Grid Example

This example attempts to show many examples and tricks of
using a virtual grid object.  Hopefully the source isn't too jumbled.

Features:
<ol>
   <li>Uses a virtual grid
   <li>Columns and rows have popup menus (right click on labels)
   <li>Columns and rows can be deleted (i.e. table can be
      resized)
   <li>Dynamic renderers.  Renderers are plugins based on
      column header name.  Shows a simple Font Renderer and
      an Image Renderer.
</ol>

Look for 'XXX' in the code to indicate some workarounds for non-obvious
behavior and various hacks.

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

