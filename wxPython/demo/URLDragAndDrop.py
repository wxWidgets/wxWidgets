
import  wx

#----------------------------------------------------------------------

class MyURLDropTarget(wx.PyDropTarget):
    def __init__(self, window):
        wx.PyDropTarget.__init__(self)
        self.window = window

        self.data = wx.URLDataObject();
        self.SetDataObject(self.data)

    def OnDragOver(self, x, y, d):
        return wx.DragLink

    def OnData(self, x, y, d):
        if not self.GetData():
            return wx.DragNone

        url = self.data.GetURL()
        self.window.AppendText(url + "\n")

        return d


#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)

        self.SetAutoLayout(True)
        outsideSizer = wx.BoxSizer(wx.VERTICAL)

        msg = "Drag-And-Drop of URLs"
        text = wx.StaticText(self, -1, "", style=wx.ALIGN_CENTRE)
        text.SetFont(wx.Font(24, wx.SWISS, wx.NORMAL, wx.BOLD, False))
        text.SetLabel(msg)
        w,h = text.GetTextExtent(msg)
        text.SetSize(wx.Size(w,h+1))
        text.SetForegroundColour(wx.BLUE)
        outsideSizer.Add(text, 0, wx.EXPAND|wx.ALL, 5)
        outsideSizer.Add(wx.StaticLine(self, -1), 0, wx.EXPAND)
        outsideSizer.Add((20,20))

        self.SetFont(wx.Font(10, wx.SWISS, wx.NORMAL, wx.BOLD, False))

        inSizer = wx.FlexGridSizer(2, 2, 5, 5)
        inSizer.AddGrowableCol(0)

        inSizer.Add((20,20))
        inSizer.Add((20,20))
        inSizer.Add(wx.StaticText(self, -1,
                                 "Drag URLs from your browser to\nthis window:",
                                 style = wx.ALIGN_RIGHT),
                    0, wx.ALIGN_RIGHT )
        self.dropText = wx.TextCtrl(self, -1, "", size=(380, 180),
                                   style=wx.TE_MULTILINE|wx.TE_READONLY)
        inSizer.Add(self.dropText, 0, wx.EXPAND)


        inSizer.Add(wx.StaticText(self, -1,
                                 "Drag this URL to your browser:",
                                 style = wx.ALIGN_RIGHT),
                    0, wx.ALIGN_RIGHT )
        self.dragText = wx.TextCtrl(self, -1, "http://wxPython.org/")
        inSizer.Add(self.dragText, 0, wx.EXPAND)
        self.dragText.Bind(wx.EVT_MOTION, self.OnStartDrag)


##         inSizer.Add(wx.StaticText(self, -1,
##                                  "Drag this TEXT to your browser:",
##                                  style = wx.ALIGN_RIGHT),
##                     0, wx.ALIGN_RIGHT )
##         self.dragText2 = wx.TextCtrl(self, -1, "http://wxPython.org/")
##         inSizer.Add(self.dragText2, 0, wx.EXPAND)
##         self.dragText2.Bind(EVT_MOTION, self.OnStartDrag2)


        outsideSizer.Add(inSizer, 1, wx.EXPAND)
        self.SetSizer(outsideSizer)

        self.dropText.SetDropTarget(MyURLDropTarget(self.dropText))


    def OnStartDrag(self, evt):
        if evt.Dragging():
            url = self.dragText.GetValue()
            data = wx.URLDataObject()
            data.SetURL(url)

            dropSource = wx.DropSource(self.dragText)
            dropSource.SetData(data)
            result = dropSource.DoDragDrop()


    def OnStartDrag2(self, evt):
        if evt.Dragging():
            url = self.dragText2.GetValue()
            data = wx.TextDataObject()
            data.SetText(url)

            dropSource = wx.DropSource(self.dragText2)
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
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

