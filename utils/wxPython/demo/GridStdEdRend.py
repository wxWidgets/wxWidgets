from wxPython.wx import *
from wxPython.grid import *

import string, random

#---------------------------------------------------------------------------

class MyCustomRenderer(wxPyGridCellRenderer):
    def __init__(self):
        wxPyGridCellRenderer.__init__(self)

    def Draw(self, grid, attr, dc, rect, row, col, isSelected):
        dc.SetBackgroundMode(wxSOLID)
        dc.SetBrush(wxBrush(wxBLACK, wxSOLID))
        dc.SetPen(wxTRANSPARENT_PEN)
        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height)

        dc.SetBackgroundMode(wxTRANSPARENT)
        dc.SetFont(attr.GetFont())

        text = grid.GetCellValue(row, col)
        colors = [wxRED, wxWHITE, wxCYAN]
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
        return wxSize(w, h)


    def Clone(self):
        return MyCustomRenderer


#---------------------------------------------------------------------------

rendererDemoData = [
    ('wxGridCellStringRenderer\n(the default)', 'this is a text value', wxGridCellStringRenderer, ()),
    ('wxGridCellNumberRenderer', '12345', wxGridCellNumberRenderer, ()),
    ('wxGridCellFloatRenderer', '1234.5678', wxGridCellFloatRenderer, (6,2)),
    ('wxGridCellBoolRenderer', '1', wxGridCellBoolRenderer, ()),
    ('MyCustomRenderer', 'This is my renderer', MyCustomRenderer, ()),
    ]

editorDemoData = [
    ('wxGridCellTextEditor\n(the default)', 'Here is some more text', wxGridCellTextEditor, ()),
    ('wxGridCellNumberEditor\nwith min,max', '101', wxGridCellNumberEditor, (5, 10005)),
    ('wxGridCellNumberEditor\nwithout bounds', '101', wxGridCellNumberEditor, ()),
    ('wxGridCellFloatEditor', '1234.5678', wxGridCellFloatEditor, ()),
    ('wxGridCellBoolEditor', '1', wxGridCellBoolEditor, ()),
    ('wxGridCellChoiceEditor', 'one', wxGridCellChoiceEditor, (['one', 'two', 'three', 'four',
                                                         'kick', 'Microsoft', 'out the',
                                                         'door'], false)),
    ]


comboDemoData = [
    ('wxGridCellNumberRenderer\nwxGridCellNumberEditor', '20792', wxGridCellNumberRenderer, wxGridCellNumberEditor),
    ('wxGridCellBoolRenderer\nwxGridCellBoolEditor', '1', wxGridCellBoolRenderer, wxGridCellBoolEditor),
    ]


class EditorsAndRenderersGrid(wxGrid):
    def __init__(self, parent, log):
        wxGrid.__init__(self, parent, -1)
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
            renderer = apply(renderClass, args)
            self.SetCellValue(row, renCol, label)
            self.SetCellValue(row, renCol+1, value)
            self.SetCellRenderer(row, renCol+1, renderer)
            row = row + 2


        row = 2
        for label, value, editorClass, args in editorDemoData:
            editor = apply(editorClass, args)
            self.SetCellValue(row, edCol, label)
            self.SetCellValue(row, edCol+1, value)
            self.SetCellEditor(row, edCol+1, editor)
            row = row + 2


        row = 18
        for label, value, renClass, edClass in comboDemoData:
            self.SetCellValue(row, renCol, label)
            self.SetCellValue(row, renCol+1, value)
            editor = apply(edClass, ()) #args)
            renderer = apply(renClass, ()) #args)
            self.SetCellEditor(row, renCol+1, editor)
            self.SetCellRenderer(row, renCol+1, renderer)
            row = row + 2


        font = self.GetFont()
        font.SetWeight(wxBOLD)
        attr = wxGridCellAttr()
        attr.SetFont(font)
        attr.SetBackgroundColour(wxLIGHT_GREY)
        attr.SetReadOnly(true)
        attr.SetAlignment(wxRIGHT, -1)
        self.SetColAttr(renCol, attr)
        self.SetColAttr(edCol, attr)

        # There is a bug in wxGTK for this method...
        if wxPlatform != '__WXGTK__':
            self.AutoSizeColumns(true)
        self.AutoSizeRows(true)

        EVT_GRID_CELL_LEFT_DCLICK(self, self.OnLeftDClick)


    # I do this because I don't like the default behaviour of not starting the
    # cell editor on double clicks, but only a second click.
    def OnLeftDClick(self, evt):
        if self.CanEnableCellControl():
            self.EnableCellEditControl()


#---------------------------------------------------------------------------

class TestFrame(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, "Editors and Renderers Demo", size=(640,480))
        grid = EditorsAndRenderersGrid(self, log)



#---------------------------------------------------------------------------

if __name__ == '__main__':
    import sys
    app = wxPySimpleApp()
    frame = TestFrame(None, sys.stdout)
    frame.Show(true)
    app.MainLoop()


#---------------------------------------------------------------------------
