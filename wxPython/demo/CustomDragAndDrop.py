
from wxPython.wx import *

import cPickle

#----------------------------------------------------------------------


class DoodlePad(wxWindow):
    def __init__(self, parent, log):
        wxWindow.__init__(self, parent, -1, style=wxSUNKEN_BORDER)
        self.log = log
        self.SetBackgroundColour(wxWHITE)
        self.lines = []
        self.x = self.y = 0
        self.SetMode("Draw")

        EVT_LEFT_DOWN(self, self.OnLeftDown)
        EVT_LEFT_UP(self, self.OnLeftUp)
        EVT_RIGHT_UP(self, self.OnRightUp)
        EVT_MOTION(self, self.OnMotion)
        EVT_PAINT(self, self.OnPaint)


    def SetMode(self, mode):
        self.mode = mode
        if self.mode == "Draw":
            self.SetCursor(wxStockCursor(wxCURSOR_PENCIL))
        else:
            self.SetCursor(wxSTANDARD_CURSOR)


    def OnPaint(self, event):
        dc = wxPaintDC(self)
        self.DrawSavedLines(dc)

    def DrawSavedLines(self, dc):
        dc.BeginDrawing()
        dc.SetPen(wxPen(wxBLUE, 3))
        for line in self.lines:
            for coords in line:
                apply(dc.DrawLine, coords)
        dc.EndDrawing()


    def OnLeftDown(self, event):
        if self.mode == "Drag":
            self.StartDragOpperation()
        elif self.mode == "Draw":
            self.curLine = []
            self.x, self.y = event.GetPositionTuple()
            self.CaptureMouse()
        else:
            wxBell()
            self.log.write("unknown mode!\n")


    def OnLeftUp(self, event):
        self.lines.append(self.curLine)
        self.curLine = []
        self.ReleaseMouse()

    def OnRightUp(self, event):
        self.lines = []
        self.Refresh()

    def OnMotion(self, event):
        if event.Dragging() and not self.mode == "Drag":
            dc = wxClientDC(self)
            dc.BeginDrawing()
            dc.SetPen(wxPen(wxBLUE, 3))
            coords = (self.x, self.y) + event.GetPositionTuple()
            self.curLine.append(coords)
            apply(dc.DrawLine, coords)
            self.x, self.y = event.GetPositionTuple()
            dc.EndDrawing()


    def StartDragOpperation(self):
        # pickle the lines list
        linesdata = cPickle.dumps(self.lines, 1)

        # create our own data format and use it in a
        # custom data object
        ldata = wxCustomDataObject(wxCustomDataFormat("DoodleLines"))
        ldata.SetData(linesdata)

        # Also create a Bitmap version of the drawing
        size = self.GetSize()
        bmp = wxEmptyBitmap(size.width, size.height)
        dc = wxMemoryDC()
        dc.SelectObject(bmp)
        dc.SetBackground(wxWHITE_BRUSH)
        dc.Clear()
        self.DrawSavedLines(dc)
        dc.SelectObject(wxNullBitmap)

        # Now make a data object for the bitmap and also a composite
        # data object holding both of the others.
        bdata = wxBitmapDataObject(bmp)
        data = wxDataObjectComposite()
        data.Add(ldata)
        data.Add(bdata)

        # And finally, create the drop source and begin the drag
        # and drop opperation
        dropSource = wxDropSource(self)
        dropSource.SetData(data)
        self.log.WriteText("Begining DragDrop\n")
        result = dropSource.DoDragDrop(wxDrag_AllowMove)
        self.log.WriteText("DragDrop completed: %d\n" % result)
        if result == wxDragMove:
            self.lines = []
            self.Refresh()


#----------------------------------------------------------------------


class DoodleDropTarget(wxPyDropTarget):
    def __init__(self, window, log):
        wxPyDropTarget.__init__(self)
        self.log = log
        self.dv = window

        # specify the type of data we will accept
        self.df = wxCustomDataFormat("DoodleLines")
        self.data = wxCustomDataObject(self.df)
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
        return d  # what is returned signals the source what to do
                  # with the original data (move, copy, etc.)  In this
                  # case we just return the suggested value given to us.




class DoodleViewer(wxWindow):
    def __init__(self, parent, log):
        wxWindow.__init__(self, parent, -1, style=wxSUNKEN_BORDER)
        self.log = log
        self.SetBackgroundColour(wxWHITE)
        self.lines = []
        self.x = self.y = 0
        dt = DoodleDropTarget(self, log)
        self.SetDropTarget(dt)
        EVT_PAINT(self, self.OnPaint)


    def SetLines(self, lines):
        self.lines = lines
        self.Refresh()

    def OnPaint(self, event):
        dc = wxPaintDC(self)
        self.DrawSavedLines(dc)

    def DrawSavedLines(self, dc):
        dc.BeginDrawing()
        dc.SetPen(wxPen(wxRED, 3))
        for line in self.lines:
            for coords in line:
                apply(dc.DrawLine, coords)
        dc.EndDrawing()

#----------------------------------------------------------------------

class CustomDnDPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)

        self.SetFont(wxFont(10, wxSWISS, wxNORMAL, wxBOLD, False))

        # Make the controls
        text1 = wxStaticText(self, -1,
                            "Draw a little picture in this window\n"
                            "then switch the mode below and drag the\n"
                            "picture to the lower window or to another\n"
                            "application that accepts BMP's as a drop\n"
                             "target.\n"
                            )

        rb1 = wxRadioButton(self, -1, "Draw", style=wxRB_GROUP)
        rb1.SetValue(True)
        rb2 = wxRadioButton(self, -1, "Drag")
        rb2.SetValue(False)

        text2 = wxStaticText(self, -1,
                             "The lower window is accepting a\n"
                             "custom data type that is a pickled\n"
                             "Python list of lines data.")

        self.pad = DoodlePad(self, log)
        view = DoodleViewer(self, log)

        # put them in sizers
        sizer = wxBoxSizer(wxHORIZONTAL)
        box = wxBoxSizer(wxVERTICAL)
        rbox = wxBoxSizer(wxHORIZONTAL)

        rbox.Add(rb1)
        rbox.Add(rb2)
        box.Add(text1, 0, wxALL, 10)
        box.Add(rbox, 0, wxALIGN_CENTER)
        box.Add(10,90)
        box.Add(text2, 0, wxALL, 10)

        sizer.Add(box)

        dndsizer = wxBoxSizer(wxVERTICAL)
        dndsizer.Add(self.pad, 1, wxEXPAND|wxALL, 5)
        dndsizer.Add(view, 1, wxEXPAND|wxALL, 5)

        sizer.Add(dndsizer, 1, wxEXPAND)

        self.SetAutoLayout(True)
        self.SetSizer(sizer)

        # Events
        EVT_RADIOBUTTON(self, rb1.GetId(), self.OnRadioButton)
        EVT_RADIOBUTTON(self, rb2.GetId(), self.OnRadioButton)


    def OnRadioButton(self, evt):
        rb = self.FindWindowById(evt.GetId())
        self.pad.SetMode(rb.GetLabel())


#----------------------------------------------------------------------
#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)

        self.SetAutoLayout(True)
        sizer = wxBoxSizer(wxVERTICAL)

        msg = "Custom Drag-And-Drop"
        text = wxStaticText(self, -1, "", style=wxALIGN_CENTRE)
        text.SetFont(wxFont(24, wxSWISS, wxNORMAL, wxBOLD, False))
        text.SetLabel(msg)
        w,h = text.GetTextExtent(msg)
        text.SetSize(wxSize(w,h+1))
        text.SetForegroundColour(wxBLUE)
        sizer.Add(text, 0, wxEXPAND|wxALL, 5)
        sizer.Add(wxStaticLine(self, -1), 0, wxEXPAND)

        sizer.Add(CustomDnDPanel(self, log), 1, wxEXPAND)

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

    class TestApp(wxApp):
        def OnInit(self):
            wxInitAllImageHandlers()
            self.MakeFrame()
            return True

        def MakeFrame(self, event=None):
            frame = wxFrame(None, -1, "Custom Drag and Drop", size=(550,400))
            menu = wxMenu()
            menu.Append(6543, "Window")
            mb = wxMenuBar()
            mb.Append(menu, "New")
            frame.SetMenuBar(mb)
            EVT_MENU(frame, 6543, self.MakeFrame)
            panel = TestPanel(frame, DummyLog())
            frame.Show(True)
            self.SetTopWindow(frame)



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

The two data objects are combined in a wxDataObjectComposite and the
rest is handled by the framework.
</body></html>
"""



