
import  cPickle
import  wx

#----------------------------------------------------------------------


class DoodlePad(wx.Window):
    def __init__(self, parent, log):
        wx.Window.__init__(self, parent, -1, style=wx.SUNKEN_BORDER)
        self.log = log
        self.SetBackgroundColour(wx.WHITE)
        self.lines = []
        self.x = self.y = 0
        self.SetMode("Draw")

        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        self.Bind(wx.EVT_LEFT_UP,  self.OnLeftUp)
        self.Bind(wx.EVT_RIGHT_UP, self.OnRightUp)
        self.Bind(wx.EVT_MOTION, self.OnMotion)
        self.Bind(wx.EVT_PAINT, self.OnPaint)


    def SetMode(self, mode):
        self.mode = mode
        if self.mode == "Draw":
            self.SetCursor(wx.StockCursor(wx.CURSOR_PENCIL))
        else:
            self.SetCursor(wx.STANDARD_CURSOR)


    def OnPaint(self, event):
        dc = wx.PaintDC(self)
        self.DrawSavedLines(dc)

    def DrawSavedLines(self, dc):
        dc.BeginDrawing()
        dc.SetPen(wx.Pen(wx.BLUE, 3))
        for line in self.lines:
            for coords in line:
                dc.DrawLine(*coords)
        dc.EndDrawing()


    def OnLeftDown(self, event):
        if self.mode == "Drag":
            self.StartDragOpperation()
        elif self.mode == "Draw":
            self.curLine = []
            self.x, self.y = event.GetPositionTuple()
            self.CaptureMouse()
        else:
            wx.Bell()
            self.log.write("unknown mode!\n")


    def OnLeftUp(self, event):
        if self.HasCapture():
            self.lines.append(self.curLine)
            self.curLine = []
            self.ReleaseMouse()

    def OnRightUp(self, event):
        self.lines = []
        self.Refresh()

    def OnMotion(self, event):
        if self.HasCapture() and event.Dragging() and not self.mode == "Drag":
            dc = wx.ClientDC(self)
            dc.BeginDrawing()
            dc.SetPen(wx.Pen(wx.BLUE, 3))
            coords = (self.x, self.y) + event.GetPositionTuple()
            self.curLine.append(coords)
            dc.DrawLine(*coords)
            self.x, self.y = event.GetPositionTuple()
            dc.EndDrawing()


    def StartDragOpperation(self):
        # pickle the lines list
        linesdata = cPickle.dumps(self.lines, 1)

        # create our own data format and use it in a
        # custom data object
        ldata = wx.CustomDataObject("DoodleLines")
        ldata.SetData(linesdata)

        # Also create a Bitmap version of the drawing
        size = self.GetSize()
        bmp = wx.EmptyBitmap(size.width, size.height)
        dc = wx.MemoryDC()
        dc.SelectObject(bmp)
        dc.SetBackground(wx.WHITE_BRUSH)
        dc.Clear()
        self.DrawSavedLines(dc)
        dc.SelectObject(wx.NullBitmap)

        # Now make a data object for the bitmap and also a composite
        # data object holding both of the others.
        bdata = wx.BitmapDataObject(bmp)
        data = wx.DataObjectComposite()
        data.Add(ldata)
        data.Add(bdata)

        # And finally, create the drop source and begin the drag
        # and drop opperation
        dropSource = wx.DropSource(self)
        dropSource.SetData(data)
        self.log.WriteText("Begining DragDrop\n")
        result = dropSource.DoDragDrop(wx.Drag_AllowMove)
        self.log.WriteText("DragDrop completed: %d\n" % result)

        if result == wx.DragMove:
            self.lines = []
            self.Refresh()


#----------------------------------------------------------------------


class DoodleDropTarget(wx.PyDropTarget):
    def __init__(self, window, log):
        wx.PyDropTarget.__init__(self)
        self.log = log
        self.dv = window

        # specify the type of data we will accept
        self.data = wx.CustomDataObject("DoodleLines")
        self.SetDataObject(self.data)


    # some virtual methods that track the progress of the drag
    def OnEnter(self, x, y, d):
        self.log.WriteText("OnEnter: %d, %d, %d\n" % (x, y, d))
        return d

    def OnLeave(self):
        self.log.WriteText("OnLeave\n")

    def OnDrop(self, x, y):
        self.log.WriteText("OnDrop: %d %d\n" % (x, y))
        return True

    def OnDragOver(self, x, y, d):
        #self.log.WriteText("OnDragOver: %d, %d, %d\n" % (x, y, d))

        # The value returned here tells the source what kind of visual
        # feedback to give.  For example, if wxDragCopy is returned then
        # only the copy cursor will be shown, even if the source allows
        # moves.  You can use the passed in (x,y) to determine what kind
        # of feedback to give.  In this case we return the suggested value
        # which is based on whether the Ctrl key is pressed.
        return d



    # Called when OnDrop returns True.  We need to get the data and
    # do something with it.
    def OnData(self, x, y, d):
        self.log.WriteText("OnData: %d, %d, %d\n" % (x, y, d))

        # copy the data from the drag source to our data object
        if self.GetData():
            # convert it back to a list of lines and give it to the viewer
            linesdata = self.data.GetData()
            lines = cPickle.loads(linesdata)
            self.dv.SetLines(lines)
            
        # what is returned signals the source what to do
        # with the original data (move, copy, etc.)  In this
        # case we just return the suggested value given to us.
        return d  
        


class DoodleViewer(wx.Window):
    def __init__(self, parent, log):
        wx.Window.__init__(self, parent, -1, style=wx.SUNKEN_BORDER)
        self.log = log
        self.SetBackgroundColour(wx.WHITE)
        self.lines = []
        self.x = self.y = 0
        dt = DoodleDropTarget(self, log)
        self.SetDropTarget(dt)
        self.Bind(wx.EVT_PAINT, self.OnPaint)


    def SetLines(self, lines):
        self.lines = lines
        self.Refresh()

    def OnPaint(self, event):
        dc = wx.PaintDC(self)
        self.DrawSavedLines(dc)

    def DrawSavedLines(self, dc):
        dc.BeginDrawing()
        dc.SetPen(wx.Pen(wx.RED, 3))

        for line in self.lines:
            for coords in line:
                dc.DrawLine(*coords)
        dc.EndDrawing()

#----------------------------------------------------------------------

class CustomDnDPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)

        self.SetFont(wx.Font(10, wx.SWISS, wx.NORMAL, wx.BOLD, False))

        # Make the controls
        text1 = wx.StaticText(self, -1,
                            "Draw a little picture in this window\n"
                            "then switch the mode below and drag the\n"
                            "picture to the lower window or to another\n"
                            "application that accepts Bitmaps as a\n"
                             "drop target.\n"
                            )

        rb1 = wx.RadioButton(self, -1, "Draw", style=wx.RB_GROUP)
        rb1.SetValue(True)
        rb2 = wx.RadioButton(self, -1, "Drag")
        rb2.SetValue(False)

        text2 = wx.StaticText(self, -1,
                             "The lower window is accepting a\n"
                             "custom data type that is a pickled\n"
                             "Python list of lines data.")

        self.pad = DoodlePad(self, log)
        view = DoodleViewer(self, log)

        # put them in sizers
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        box = wx.BoxSizer(wx.VERTICAL)
        rbox = wx.BoxSizer(wx.HORIZONTAL)

        rbox.Add(rb1)
        rbox.Add(rb2)
        box.Add(text1, 0, wx.ALL, 10)
        box.Add(rbox, 0, wx.ALIGN_CENTER)
        box.Add((10,90))
        box.Add(text2, 0, wx.ALL, 10)

        sizer.Add(box)

        dndsizer = wx.BoxSizer(wx.VERTICAL)
        dndsizer.Add(self.pad, 1, wx.EXPAND|wx.ALL, 5)
        dndsizer.Add(view, 1, wx.EXPAND|wx.ALL, 5)

        sizer.Add(dndsizer, 1, wx.EXPAND)

        self.SetAutoLayout(True)
        self.SetSizer(sizer)

        # Events
        self.Bind(wx.EVT_RADIOBUTTON, self.OnRadioButton, rb1)
        self.Bind(wx.EVT_RADIOBUTTON, self.OnRadioButton, rb2)


    def OnRadioButton(self, evt):
        rb = self.FindWindowById(evt.GetId())
        self.pad.SetMode(rb.GetLabel())


#----------------------------------------------------------------------
#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)

        self.SetAutoLayout(True)
        sizer = wx.BoxSizer(wx.VERTICAL)

        msg = "Custom Drag-And-Drop"
        text = wx.StaticText(self, -1, "", style=wx.ALIGN_CENTRE)
        text.SetFont(wx.Font(24, wx.SWISS, wx.NORMAL, wx.BOLD, False))
        text.SetLabel(msg)
        w,h = text.GetTextExtent(msg)
        text.SetSize(wx.Size(w,h+1))
        text.SetForegroundColour(wx.BLUE)
        sizer.Add(text, 0, wx.EXPAND|wx.ALL, 5)
        sizer.Add(wx.StaticLine(self, -1), 0, wx.EXPAND)

        sizer.Add(CustomDnDPanel(self, log), 1, wx.EXPAND)

        self.SetSizer(sizer)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    #win = TestPanel(nb, log)
    win = CustomDnDPanel(nb, log)
    return win


if __name__ == '__main__':
    import sys
    
    class DummyLog:
        def WriteText(self, text):
            sys.stdout.write(text)

    class TestApp(wx.App):
        def OnInit(self):
            wx.InitAllImageHandlers()
            self.MakeFrame()
            return True

        def MakeFrame(self, event=None):
            frame = wx.Frame(None, -1, "Custom Drag and Drop", size=(550,400))
            menu = wx.Menu()
            item = menu.Append(-1, "Window")
            mb = wx.MenuBar()
            mb.Append(menu, "New")
            frame.SetMenuBar(mb)
            frame.Bind(wx.EVT_MENU, self.MakeFrame, item)
            panel = TestPanel(frame, DummyLog())
            frame.Show(True)
            self.SetTopWindow(frame)

    #----------------------------------------------------------------------

    app = TestApp(0)
    app.MainLoop()

#----------------------------------------------------------------------


overview = """<html><body>
This demo shows Drag and Drop using a custom data type and a custom
data object.  A type called "DoodleLines" is created and a Python
Pickle of a list is actually transfered in the drag and drop
opperation.

A second data object is also created containing a bitmap of the image
and is made available to any drop target that accepts bitmaps, such as
MS Word.

The two data objects are combined in a wx.DataObjectComposite and the
rest is handled by the framework.
</body></html>
"""

