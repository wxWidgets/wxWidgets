
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
        self.SetCursor(wxStockCursor(wxCURSOR_PENCIL))

        EVT_LEFT_DOWN(self, self.OnLeftDown)
        EVT_LEFT_UP(self, self.OnLeftUp)
        EVT_RIGHT_UP(self, self.OnRightUp)
        EVT_MOTION(self, self.OnMotion)


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
        if event.ControlDown():
            self.StartDragOpperation()
        else:
            self.curLine = []
            self.x, self.y = event.GetPositionTuple()
            self.CaptureMouse()


    def OnLeftUp(self, event):
        self.lines.append(self.curLine)
        self.curLine = []
        self.ReleaseMouse()

    def OnRightUp(self, event):
        self.lines = []
        self.Refresh()

    def OnMotion(self, event):
        if event.Dragging() and not event.ControlDown():
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
        result = dropSource.DoDragDrop()
        self.log.WriteText("DragDrop completed: %d\n" % result)

#----------------------------------------------------------------------


class DoodleDropTarget(wxPyDropTarget):
    def __init__(self, window, log):
        wxPyDropTarget.__init__(self)
        self.log = log
        self.dv = window
        self.data = wxCustomDataObject(wxCustomDataFormat("DoodleLines"))
        self.SetDataObject(self.data)

    def OnEnter(self, x, y, d):
        self.log.WriteText("OnEnter: %d, %d, %d\n" % (x, y, d))
        return wxDragCopy

    def OnLeave(self):
        self.log.WriteText("OnLeave\n")

    def OnDrop(self, x, y):
        self.log.WriteText("OnDrop: %d %d\n" % (x, y))
        return true

    def OnData(self, x, y, d):
        self.log.WriteText("OnData: %d, %d, %d\n" % (x, y, d))
        if self.GetData():
            linesdata = self.data.GetData()
            lines = cPickle.loads(linesdata)
            self.dv.SetLines(lines)
        return d

    #def OnDragOver(self, x, y, d):
    #    self.log.WriteText("OnDragOver: %d, %d, %d\n" % (x, y, d))
    #    return wxDragCopy



class DoodleViewer(wxWindow):
    def __init__(self, parent, log):
        wxWindow.__init__(self, parent, -1, style=wxSUNKEN_BORDER)
        self.log = log
        self.SetBackgroundColour(wxWHITE)
        self.lines = []
        self.x = self.y = 0
        dt = DoodleDropTarget(self, log)
        self.SetDropTarget(dt)

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

        self.SetFont(wxFont(10, wxSWISS, wxNORMAL, wxBOLD, false))

        sizer = wxBoxSizer(wxHORIZONTAL)
        sizer.Add(wxStaticText(self, -1,
                               "Draw a little picture in this window\n"
                               "then Ctrl-Drag it to the lower \n"
                               "window or to another application\n"
                               "that accepts BMP's as a drop target."),
                  1, wxEXPAND|wxALL, 10)

        insizer = wxBoxSizer(wxVERTICAL)
        insizer.Add(DoodlePad(self, log), 1, wxEXPAND|wxALL, 5)
        insizer.Add(DoodleViewer(self, log), 1, wxEXPAND|wxALL, 5)

        sizer.Add(insizer, 1, wxEXPAND)
        self.SetAutoLayout(true)
        self.SetSizer(sizer)




#----------------------------------------------------------------------
#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)

        self.SetAutoLayout(true)
        sizer = wxBoxSizer(wxVERTICAL)

        msg = "Custom Drag-And-Drop"
        text = wxStaticText(self, -1, "", style=wxALIGN_CENTRE)
        text.SetFont(wxFont(24, wxSWISS, wxNORMAL, wxBOLD, false))
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
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------












overview = """\
This demo shows Drag and Drop using a custom data type and a custom data object.  A type called "DoodleLines" is created and a Python Pickle of a list is actually transfered in the drag and drop opperation.

A second data object is also created containing a bitmap of the image and is made available to any drop target that accepts bitmaps, such as MS Word.

The two data objects are combined in a wxDataObjectComposite and the rest is handled by the framework.
"""
