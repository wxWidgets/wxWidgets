from wxPython.wx import *
from wxPython.grid import *

#---------------------------------------------------------------------------

class NewEnterHandlingGrid(wxGrid):
    def __init__(self, parent, log):
        wxGrid.__init__(self, parent, -1)
        self.log = log

        self.CreateGrid(20, 6)

        self.SetCellValue(0, 0, "Enter moves to the right")
        self.SetCellValue(0, 5, "Enter wraps to next row")
        self.SetColSize(0, 150)
        self.SetColSize(5, 150)

        EVT_KEY_DOWN(self, self.OnKeyDown)


    def OnKeyDown(self, evt):
        if evt.KeyCode() != WXK_RETURN:
            evt.Skip()
            return

        if evt.ControlDown():   # the edit control needs this key
            evt.Skip()
            return

        self.DisableCellEditControl()
        success = self.MoveCursorRight(evt.ShiftDown())
        if not success:
            newRow = self.GetGridCursorRow() + 1
            if newRow < self.GetTable().GetNumberRows():
                self.SetGridCursor(newRow, 0)
                self.MakeCellVisible(newRow, 0)
            else:
                # this would be a good place to add a new row if your app
                # needs to do that
                pass


#---------------------------------------------------------------------------

class TestFrame(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, "Simple Grid Demo", size=(640,480))
        grid = NewEnterHandlingGrid(self, log)



#---------------------------------------------------------------------------

if __name__ == '__main__':
    import sys
    app = wxPySimpleApp()
    frame = TestFrame(None, sys.stdout)
    frame.Show(true)
    app.MainLoop()


#---------------------------------------------------------------------------
