
import  random

import  wx
import  wx.grid as  gridlib

#---------------------------------------------------------------------------

class MyCustomRenderer(gridlib.PyGridCellRenderer):
    def __init__(self):
        gridlib.PyGridCellRenderer.__init__(self)

    def Draw(self, grid, attr, dc, rect, row, col, isSelected):
        dc.SetBackgroundMode(wx.SOLID)
        dc.SetBrush(wx.Brush(wx.BLACK, wx.SOLID))
        dc.SetPen(wx.TRANSPARENT_PEN)
        dc.DrawRectangleRect(rect)

        dc.SetBackgroundMode(wx.TRANSPARENT)
        dc.SetFont(attr.GetFont())

        text = grid.GetCellValue(row, col)
        colors = ["RED", "WHITE", "SKY BLUE"]
        x = rect.x + 1
        y = rect.y + 1

        for ch in text:
            dc.SetTextForeground(random.choice(colors))
            dc.DrawText(ch, x, y)
            w, h = dc.GetTextExtent(ch)
            x = x + w
            if x > rect.right - 5:
                break


    def GetBestSize(self, grid, attr, dc, row, col):
        text = grid.GetCellValue(row, col)
        dc.SetFont(attr.GetFont())
        w, h = dc.GetTextExtent(text)
        return wx.Size(w, h)


    def Clone(self):
        return MyCustomRenderer()


#---------------------------------------------------------------------------

rendererDemoData = [
    ('GridCellStringRenderer\n(the default)', 'this is a text value', gridlib.GridCellStringRenderer, ()),
    ('GridCellNumberRenderer', '12345', gridlib.GridCellNumberRenderer, ()),
    ('GridCellFloatRenderer', '1234.5678', gridlib.GridCellFloatRenderer, (6,2)),
    ('GridCellBoolRenderer', '1', gridlib.GridCellBoolRenderer, ()),
    ('MyCustomRenderer', 'This is my renderer', MyCustomRenderer, ()),
    ]

editorDemoData = [
    ('GridCellTextEditor\n(the default)', 'Here is some more text', gridlib.GridCellTextEditor, ()),
    ('GridCellNumberEditor\nwith min,max', '101', gridlib.GridCellNumberEditor, (5, 10005)),
    ('GridCellNumberEditor\nwithout bounds', '101', gridlib.GridCellNumberEditor, ()),
    ('GridCellFloatEditor', '1234.5678', gridlib.GridCellFloatEditor, ()),
    ('GridCellBoolEditor', '1', gridlib.GridCellBoolEditor, ()),
    ('GridCellChoiceEditor', 'one', gridlib.GridCellChoiceEditor, (['one', 'two', 'three', 'four',
                                                         'kick', 'Microsoft', 'out the',
                                                         'door'], False)),
    ]

comboDemoData = [
    ('GridCellNumberRenderer\nGridCellNumberEditor', '20792', gridlib.GridCellNumberRenderer, gridlib.GridCellNumberEditor),
    ('GridCellBoolRenderer\nGridCellBoolEditor', '1', gridlib.GridCellBoolRenderer, gridlib.GridCellBoolEditor),
    ]


class EditorsAndRenderersGrid(gridlib.Grid):
    def __init__(self, parent, log):
        gridlib.Grid.__init__(self, parent, -1)
        self.log = log

        self.CreateGrid(25, 8)
        renCol = 1
        edCol = 4


        self.SetCellValue(0, renCol, '''\
Cell Renderers are used to draw
the contents of the cell when they
need to be refreshed.  Different
types of Renderers can be plugged in
to different cells in the grid, it can
even be automatically determined based
on the type of data in the cell.
''')

        self.SetCellValue(0, edCol, '''\
Cell Editors are used when the
value of the cell is edited by
the user.  An editor class is
wrapped around a an object
derived from wxControl and it
implements some methods required
to integrate with the grid.
''')

        self.SetCellValue(16, renCol, '''\
Here are some combinations of Editors and
Renderers used together.
''')

        row = 2

        for label, value, renderClass, args in rendererDemoData:
            renderer = renderClass(*args)
            self.SetCellValue(row, renCol, label)
            self.SetCellValue(row, renCol+1, value)
            self.SetCellRenderer(row, renCol+1, renderer)
            row = row + 2


        row = 2

        for label, value, editorClass, args in editorDemoData:
            editor = editorClass(*args)
            self.SetCellValue(row, edCol, label)
            self.SetCellValue(row, edCol+1, value)
            self.SetCellEditor(row, edCol+1, editor)
            row = row + 2


        row = 18

        for label, value, renClass, edClass in comboDemoData:
            self.SetCellValue(row, renCol, label)
            self.SetCellValue(row, renCol+1, value)
            editor = edClass()
            renderer = renClass()
            self.SetCellEditor(row, renCol+1, editor)
            self.SetCellRenderer(row, renCol+1, renderer)
            row = row + 2

        font = self.GetFont()
        font.SetWeight(wx.BOLD)
        attr = gridlib.GridCellAttr()
        attr.SetFont(font)
        attr.SetBackgroundColour(wx.LIGHT_GREY)
        attr.SetReadOnly(True)
        attr.SetAlignment(wx.RIGHT, -1)
        self.SetColAttr(renCol, attr)
        attr.IncRef()
        self.SetColAttr(edCol, attr)

        # There is a bug in wxGTK for this method...
        self.AutoSizeColumns(True)
        self.AutoSizeRows(True)

        self.Bind(gridlib.EVT_GRID_CELL_LEFT_DCLICK, self.OnLeftDClick)


    # I do this because I don't like the default behaviour of not starting the
    # cell editor on double clicks, but only a second click.
    def OnLeftDClick(self, evt):
        if self.CanEnableCellControl():
            self.EnableCellEditControl()


#---------------------------------------------------------------------------

class TestFrame(wx.Frame):
    def __init__(self, parent, log):
        wx.Frame.__init__(self, parent, -1, "Editors and Renderers Demo", size=(640,480))
        grid = EditorsAndRenderersGrid(self, log)



#---------------------------------------------------------------------------

if __name__ == '__main__':
    import sys
    app = wx.PySimpleApp()
    frame = TestFrame(None, sys.stdout)
    frame.Show(True)
    app.MainLoop()


#---------------------------------------------------------------------------
