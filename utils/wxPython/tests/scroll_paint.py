from wxPython.wx import *

class MyWindow(wxScrolledWindow):
    def __init__(self,parent,id,pos,size,style):
        wxScrolledWindow.__init__(self,parent,id,pos,size,style)
        self.SetBackgroundColour(wxWHITE)

    def OnPaint(self,evt):
        dc = wxPaintDC(self)
        # normally call a redraw/draw function here.
        # this time, print 'redraw!'
        print "redraw!"


class MyFrame(wxFrame):
    def __init__(self, parent, id, title):
        wxFrame.__init__(self, parent, id, title,
                         wxPoint(100, 100), wxSize(500, 300))

        self.sw = MyWindow(self, -1,
                           wxDefaultPosition, wxDefaultSize,
                           wxVSCROLL|wxSUNKEN_BORDER)

        self.sw.SetScrollbars(1,20,0,30)


if __name__ == "__main__":
    class MyApp(wxApp):
        def OnInit(self):

            self.frame = MyFrame(NULL, -1, "Scrolling Test App")
            self.frame.Show(true)
            self.exiting = FALSE;
            return true

    app = MyApp(0)     # Create an instance of the app class
    app.MainLoop()     # Tell it to start processing events

