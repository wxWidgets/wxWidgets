#-------------------------------------------------

from wxPython.wx import *


class MyFrame(wxFrame):
    def __init__(self,parent,title,id):
        wxFrame.__init__(self,parent,title,id,
                         wxPoint(100,100),wxSize(300,300))


        self.SetBackgroundColour(wxWHITE)
        self.windowx,self.windowy=self.GetClientSizeTuple()

        # make a memory DC to draw into...
        self.mask=wxMemoryDC()

        self.maskbitmap=wxEmptyBitmap(self.windowx,self.windowy)
        self.mask.SelectObject(self.maskbitmap)
        self.mask.SetBackgroundMode(wxTRANSPARENT)

        self.mask.SetDeviceOrigin(0,0)

        self.mask.SetBrush(wxBrush(wxColour(150,160,0),wxSOLID))
        self.mask.SetPen(wxPen(wxColour(1,2,3),1,wxSOLID))
        self.mask.DrawRectangle(0,0,self.windowx,self.windowy)

        img = wxImageFromBitmap(self.maskbitmap)
        print (img.GetRed(0,0), img.GetGreen(0,0), img.GetBlue(0,0))


    def OnPaint(self,evt):
        """ overriding OnPaint to give handler. """
        dc = wxPaintDC(self)

        dc.Blit(0,0,self.windowx,self.windowy,self.mask,0,0,wxCOPY)

#-----------------------------------------------------------

if __name__ == "__main__":
    class MyApp(wxApp):
        def OnInit(self):

            self.frame = MyFrame(NULL, -1, "Blit Test")
            self.frame.Show(true)

            self.exiting = FALSE;
            return true

    app = MyApp(0)     # Create an instance of the application
    app.MainLoop()     # Tell it to start processing events



