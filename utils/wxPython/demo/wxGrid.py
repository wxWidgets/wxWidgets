
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestGrid(wxGrid):
    def __init__(self, parent, log):
        wxGrid.__init__(self, parent, -1)
        self.log = log

        self.CreateGrid(16, 16)
        self.SetColumnWidth(3, 200)
        self.SetRowHeight(4, 45)
        self.SetCellValue("First cell", 0, 0)
        self.SetCellValue("Another cell", 1, 1)
        self.SetCellValue("Yet another cell", 2, 2)
        self.SetCellTextFont(wxFont(12, wxROMAN, wxITALIC, wxNORMAL), 0, 0)
        self.SetCellTextColour(wxRED, 1, 1)
        self.SetCellBackgroundColour(wxCYAN, 2, 2)
        self.UpdateDimensions()
        self.AdjustScrollbars()

        EVT_GRID_SELECT_CELL(self, self.OnSelectCell)
        EVT_GRID_CELL_CHANGE(self, self.OnCellChange)
        EVT_GRID_CELL_LCLICK(self, self.OnCellClick)
        EVT_GRID_LABEL_LCLICK(self, self.OnLabelClick)

        self.SetEditInPlace(true)
        #print self.GetCells()


    def OnSelectCell(self, event):
        self.log.WriteText("OnSelectCell: (%d, %d)\n" % (event.m_row, event.m_col))

    def OnCellChange(self, event):
        self.log.WriteText("OnCellChange: (%d, %d)\n" % (event.m_row, event.m_col))

    def OnCellClick(self, event):
        self.log.WriteText("OnCellClick: (%d, %d)\n" % (event.m_row, event.m_col))

    def OnLabelClick(self, event):
        self.log.WriteText("OnLabelClick: (%d, %d)\n" % (event.m_row, event.m_col))

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestGrid(nb, log)
    return win

#---------------------------------------------------------------------------



















overview = """\
wxGrid is a class for displaying and editing tabular information.

wxGrid()
-----------------

wxGrid(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style=0, const wxString& name="grid")

Constructor. Before using a wxGrid object, you must call CreateGrid to set up the required rows and columns.
"""
