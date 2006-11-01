import wx

class DragController(wx.Control):
    """
    Just a little control to handle dragging the text from a text
    control.  We use a separate control so as to not interfere with
    the native drag-select functionality of the native text control.
    """
    def __init__(self, parent, source, size=(25,25)):
        wx.Control.__init__(self, parent, -1, size=size,
                            style=wx.SIMPLE_BORDER)
        self.source = source
        self.SetMinSize(size)
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        
    def OnPaint(self, evt):
        # draw a simple arrow
        dc = wx.BufferedPaintDC(self)
        dc.SetBackground(wx.Brush(self.GetBackgroundColour()))
        dc.Clear()
        w, h = dc.GetSize()
        y = h/2
        dc.SetPen(wx.Pen("dark blue", 2))
        dc.DrawLine(w/8,   y,  w-w/8, y)
        dc.DrawLine(w-w/8, y,  w/2,   h/4)
        dc.DrawLine(w-w/8, y,  w/2,   3*h/4)

    def OnLeftDown(self, evt):
        text = self.source.GetValue()
        data = wx.TextDataObject(text)
        dropSource = wx.DropSource(self)
        dropSource.SetData(data)
        result = dropSource.DoDragDrop(wx.Drag_AllowMove)

        # if the user wants to move the data then we should delete it
        # from the source
        if result == wx.DragMove:
            self.source.SetValue("")
        
class MyFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="Drop Source")
        p = wx.Panel(self)

        # create the controls
        label1 = wx.StaticText(p, -1, "Put some text in this control:")
        label2 = wx.StaticText(p, -1,
           "Then drag from the neighboring bitmap and\n"
           "drop in an application that accepts dropped\n"
           "text, such as MS Word.")
        text = wx.TextCtrl(p, -1, "Some text")
        dragctl = DragController(p, text)

        # setup the layout with sizers
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(label1, 0, wx.ALL, 5)
        hrow = wx.BoxSizer(wx.HORIZONTAL)
        hrow.Add(text, 1, wx.RIGHT, 5)
        hrow.Add(dragctl, 0)
        sizer.Add(hrow, 0, wx.EXPAND|wx.ALL, 5)
        sizer.Add(label2, 0, wx.ALL, 5)
        p.SetSizer(sizer)
        sizer.Fit(self)
        

app = wx.PySimpleApp()
frm = MyFrame()
frm.Show()
app.MainLoop()
