
from wxPython.wx import *

#----------------------------------------------------------------------

class MyURLDropTarget(wxPyDropTarget):
    def __init__(self, window):
        wxPyDropTarget.__init__(self)
        self.window = window

        self.data = wxURLDataObject();
        self.SetDataObject(self.data)

    def OnDragOver(self, x, y, d):
        return wxDragLink

    def OnData(self, x, y, d):
        if not self.GetData():
            return wxDragNone

        url = self.data.GetURL()
        self.window.AppendText(url + "\n")

        return d


#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)

        self.SetAutoLayout(True)
        outsideSizer = wxBoxSizer(wxVERTICAL)

        msg = "Drag-And-Drop of URLs"
        text = wxStaticText(self, -1, "", style=wxALIGN_CENTRE)
        text.SetFont(wxFont(24, wxSWISS, wxNORMAL, wxBOLD, False))
        text.SetLabel(msg)
        w,h = text.GetTextExtent(msg)
        text.SetSize(wxSize(w,h+1))
        text.SetForegroundColour(wxBLUE)
        outsideSizer.Add(text, 0, wxEXPAND|wxALL, 5)
        outsideSizer.Add(wxStaticLine(self, -1), 0, wxEXPAND)
        outsideSizer.Add(20,20)

        self.SetFont(wxFont(10, wxSWISS, wxNORMAL, wxBOLD, False))

        inSizer = wxFlexGridSizer(2, 2, 5, 5)
        inSizer.AddGrowableCol(0)

        inSizer.Add(20,20)
        inSizer.Add(20,20)
        inSizer.Add(wxStaticText(self, -1,
                                 "Drag URLs from your browser to\nthis window:",
                                 style = wxALIGN_RIGHT),
                    0, wxALIGN_RIGHT )
        self.dropText = wxTextCtrl(self, -1, "", size=(380, 180),
                                   style=wxTE_MULTILINE|wxTE_READONLY)
        inSizer.Add(self.dropText, 0, wxEXPAND)


        inSizer.Add(wxStaticText(self, -1,
                                 "Drag this URL to your browser:",
                                 style = wxALIGN_RIGHT),
                    0, wxALIGN_RIGHT )
        self.dragText = wxTextCtrl(self, -1, "http://wxPython.org/")
        inSizer.Add(self.dragText, 0, wxEXPAND)
        EVT_MOTION(self.dragText, self.OnStartDrag)


##         inSizer.Add(wxStaticText(self, -1,
##                                  "Drag this TEXT to your browser:",
##                                  style = wxALIGN_RIGHT),
##                     0, wxALIGN_RIGHT )
##         self.dragText2 = wxTextCtrl(self, -1, "http://wxPython.org/")
##         inSizer.Add(self.dragText2, 0, wxEXPAND)
##         EVT_MOTION(self.dragText2, self.OnStartDrag2)


        outsideSizer.Add(inSizer, 1, wxEXPAND)
        self.SetSizer(outsideSizer)


        self.dropText.SetDropTarget(MyURLDropTarget(self.dropText))



    def OnStartDrag(self, evt):
        if evt.Dragging():
            url = self.dragText.GetValue()
            data = wxURLDataObject()
            data.SetURL(url)

            dropSource = wxDropSource(self.dragText)
            dropSource.SetData(data)
            result = dropSource.DoDragDrop()


    def OnStartDrag2(self, evt):
        if evt.Dragging():
            url = self.dragText2.GetValue()
            data = wxTextDataObject()
            data.SetText(url)

            dropSource = wxDropSource(self.dragText2)
            dropSource.SetData(data)
            result = dropSource.DoDragDrop()


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------




overview = """\
"""




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

