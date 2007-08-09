
import  wx
import  wx.grid             as  gridlib
import  wx.lib.gridmovers   as  gridmovers

#---------------------------------------------------------------------------

class CustomDataTable(gridlib.PyGridTableBase):
    def __init__(self, log):
        gridlib.PyGridTableBase.__init__(self)
        self.log = log

        self.identifiers = ['id','ds','sv','pr','pl','op','fx','ts']

        self.rowLabels = ['Row1','Row2','Row3']

        self.colLabels = {'id':'ID','ds':'Description','sv':'Severity',
                          'pr':'Priority','pl':'Platform','op':'Opened?',
                          'fx':'Fixed?','ts':'Tested?'}

        self.data = [{'id':1010,
                      'ds':"The foo doesn't bar",
                      'sv':"major",
                      'pr':1,
                      'pl':'MSW',
                      'op':1,
                      'fx':1,
                      'ts':1
                      },
                     {'id':1011,
                      'ds':"I've got a wicket in my wocket",
                      'sv':"wish list",
                      'pr':2,
                      'pl':'other',
                      'op':0,
                      'fx':0,
                      'ts':0
                      },
                     {'id':1012,
                      'ds':"Rectangle() returns a triangle",
                      'sv':"critical",
                      'pr':5,
                      'pl':'all',
                      'op':0,
                      'fx':0,
                      'ts':0
                      }
                     ]

    #--------------------------------------------------
    # required methods for the wxPyGridTableBase interface

    def GetNumberRows(self):
        return len(self.data)

    def GetNumberCols(self):
        return len(self.identifiers)

    def IsEmptyCell(self, row, col):
        id = self.identifiers[col]
        return not self.data[row][id]

    def GetValue(self, row, col):
        id = self.identifiers[col]
        return self.data[row][id]

    def SetValue(self, row, col, value):
        id = self.identifiers[col]
        self.data[row][id] = value

    #--------------------------------------------------
    # Some optional methods

    # Called when the grid needs to display column labels
    def GetColLabelValue(self, col):
        id = self.identifiers[col]
        return self.colLabels[id]

    # Called when the grid needs to display row labels
    def GetRowLabelValue(self,row):
        return self.rowLabels[row]

    #--------------------------------------------------
    # Methods added for demo purposes.

    # The physical moving of the cols/rows is left to the implementer.
    # Because of the dynamic nature of a wxGrid the physical moving of
    # columns differs from implementation to implementation

    # Move the column
    def MoveColumn(self,frm,to):
        grid = self.GetView()

        if grid:
            # Move the identifiers
            old = self.identifiers[frm]
            del self.identifiers[frm]

            if to > frm:
                self.identifiers.insert(to-1,old)
            else:
                self.identifiers.insert(to,old)

            # Notify the grid
            grid.BeginBatch()
           
            msg = gridlib.GridTableMessage(
                    self, gridlib.GRIDTABLE_NOTIFY_COLS_INSERTED, to, 1
                    )
            grid.ProcessTableMessage(msg)

            msg = gridlib.GridTableMessage(
                    self, gridlib.GRIDTABLE_NOTIFY_COLS_DELETED, frm, 1
                    )
            grid.ProcessTableMessage(msg)
            
            grid.EndBatch()

    # Move the row
    def MoveRow(self,frm,to):
        grid = self.GetView()

        if grid:
            # Move the rowLabels and data rows
            oldLabel = self.rowLabels[frm]
            oldData = self.data[frm]
            del self.rowLabels[frm]
            del self.data[frm]

            if to > frm:
                self.rowLabels.insert(to-1,oldLabel)
                self.data.insert(to-1,oldData)
            else:
                self.rowLabels.insert(to,oldLabel)
                self.data.insert(to,oldData)

            # Notify the grid
            grid.BeginBatch()

            msg = gridlib.GridTableMessage(
                    self, gridlib.GRIDTABLE_NOTIFY_ROWS_INSERTED, to, 1
                    )
            grid.ProcessTableMessage(msg)

            msg = gridlib.GridTableMessage(
                    self, gridlib.GRIDTABLE_NOTIFY_ROWS_DELETED, frm, 1
                    )
            grid.ProcessTableMessage(msg)
    
            grid.EndBatch()


#---------------------------------------------------------------------------


class DragableGrid(gridlib.Grid):
    def __init__(self, parent, log):
        gridlib.Grid.__init__(self, parent, -1)

        table = CustomDataTable(log)

        # The second parameter means that the grid is to take ownership of the
        # table and will destroy it when done.  Otherwise you would need to keep
        # a reference to it and call it's Destroy method later.
        self.SetTable(table, True)

        # Enable Column moving
        gridmovers.GridColMover(self)
        self.Bind(gridmovers.EVT_GRID_COL_MOVE, self.OnColMove, self)

        # Enable Row moving
        gridmovers.GridRowMover(self)
        self.Bind(gridmovers.EVT_GRID_ROW_MOVE, self.OnRowMove, self)

    # Event method called when a column move needs to take place
    def OnColMove(self,evt):
        frm = evt.GetMoveColumn()       # Column being moved
        to = evt.GetBeforeColumn()      # Before which column to insert
        self.GetTable().MoveColumn(frm,to)

    # Event method called when a row move needs to take place
    def OnRowMove(self,evt):
        frm = evt.GetMoveRow()          # Row being moved
        to = evt.GetBeforeRow()         # Before which row to insert
        self.GetTable().MoveRow(frm,to)

#---------------------------------------------------------------------------

class TestFrame(wx.Frame):
    def __init__(self, parent, log):
        wx.Frame.__init__(self, parent, -1, "Custom Table, data driven Grid  Demo", size=(640,480))
        grid = DragableGrid(self, log)


#---------------------------------------------------------------------------

if __name__ == '__main__':
    import sys
    app = wx.PySimpleApp()
    frame = TestFrame(None, sys.stdout)
    frame.Show(True)
    app.MainLoop()

#---------------------------------------------------------------------------
