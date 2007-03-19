import wx
import wx.grid
##import os; print os.getpid(); raw_input("press enter...")

class TestFrame(wx.Frame):
    def __init__(self, *args, **kw):
        wx.Frame.__init__(self, *args, **kw)
        self.grid = wx.grid.Grid(self)
        self.grid.CreateGrid(10,10)
        self.grid.EnableDragCell(True)
        self.grid.Bind(wx.grid.EVT_GRID_CELL_BEGIN_DRAG, self.OnBeginDrag)
        #self.grid.GetGridWindow().Bind(wx.EVT_MOTION, self.OnMouseDrag)
        

    def OnBeginDrag(self, evt):
        print "OnBeginDrag"

        # The grid window has the mouse captured when this event is sent??
        if self.grid.GetGridWindow().HasCapture():
            print "Releasing capture"
            self.grid.GetGridWindow().ReleaseMouse()
        
        src = wx.DropSource(self.grid)
        data = wx.TextDataObject("This is a test")
        src.SetData(data)
        result = src.DoDragDrop()
        print "result = %d" % result


    def OnMouseDrag(self, evt):
        if not evt.Dragging():
            evt.Skip()
            return        
        print "OnMouseDrag"
        src = wx.DropSource(self.grid)
        data = wx.TextDataObject("This is a test")
        src.SetData(data)
        result = src.DoDragDrop()
        print "result = %d" % result
            

from wx.lib.mixins.inspection import InspectableApp

app = InspectableApp(False)
app.Init()
frm = TestFrame(None, title="EVT_GRID_CELL_BEGIN_DRAG")
frm.Show()
app.MainLoop()

