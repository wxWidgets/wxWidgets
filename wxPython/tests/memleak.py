
from wxPython.wx import *

#-------------------------------------------------------------------
# class MyWindow(wxScrolledWindow):
#----------------------------------
# Copes with the drawing of the main scrolled window.
#
# Data members:
#  num - <int> number of list entries
#  ostart - <int> line number of the top line of the previous redraw
#  vw - <int> width of the viewing window
#  vh - <int> height of the viewing window
#  smalltext - <int> 0 = size 12pt, 1 = size 9pt text
#
# Method members:
#  OnPaint(evt) - basic draw handler
#  OnDraw(dc) - called by OnPaint, redraws the screen if required
#  update(updatelist) - called every 3 seconds if updates are needed.

class MyWindow(wxScrolledWindow):
  def __init__(self,num,parent,id,pos,size,style):
    wxScrolledWindow.__init__(self,parent,id,pos,size,style)
    self.SetBackgroundColour(wxWHITE)

    self.num=num
    self.ostart=0
    self.smalltext = 0
    self.vw,self.vh=self.GetClientSizeTuple()

    # calculate font pt size needed: a bit of a kludge to get round
    # font compatibility problems of X and Windows.
    dc=wxClientDC(self)

    dc.SetFont(wxFont(12,wxDEFAULT,wxNORMAL,wxNORMAL,FALSE))
    if dc.GetTextExtent("XXXXXXXXXX")[0] > 100:
      self.smalltext = 1

  def OnPaint(self,evt):
    """ overriding OnPaint to give handler. """
    dc = wxPaintDC(self)
    self.PrepareDC(dc)
    self.OnDraw(dc)

  def update(self,updlist):
    """ handles line by line updating of list entries. """
    dc = wxClientDC(self)
    self.PrepareDC(dc)
    dc.SetBrush(wxWHITE_BRUSH)
    dc.SetPen(wxWHITE_PEN)

    if self.smalltext == 1:
      dc.SetFont(wxFont(9,wxDEFAULT,wxNORMAL,wxNORMAL,FALSE))
    else:
      dc.SetFont(wxFont(12,wxDEFAULT,wxNORMAL,wxNORMAL,FALSE))

    dc.BeginDrawing()

    for i in updlist:
      if i >= self.ostart and i < self.ostart+self.vh/17+1:
        dc.DrawRectangle(0,i*17,self.vw,17)
        dc.DrawText("This is a simple test.Line "+str(i)+".",
                    10,i*17+2)
    dc.EndDrawing()

  def OnDraw(self,dc):
    """ Main redraw function. """

    if self.smalltext == 1:
      dc.SetFont(wxFont(9,wxDEFAULT,wxNORMAL,wxNORMAL,FALSE))
    else:
      dc.SetFont(wxFont(12,wxDEFAULT,wxNORMAL,wxNORMAL,FALSE))

    vx,vstart=self.ViewStart()
    self.vw,self.vh=self.GetClientSizeTuple()
    vend=vstart+(self.vh/17) + 1
    if vend > self.num: vend = self.num

    dc.BeginDrawing()
    if vstart > self.ostart: # if moving downwards...
      for i in range(vend-(vstart-self.ostart+1),vend):
        dc.DrawText("This is a simple test. Line "+str(i)+".",
                    10,i*17+2)

    elif vstart < self.ostart: # if moving upwards...
      for i in range(vstart,self.ostart):
        dc.DrawText("This is a simple test. Line "+str(i)+".",
                    10,i*17+2)

    elif vstart == self.ostart: # if not moving (redraw)...
      #dc.Clear()
      for i in range(vstart,vend):
        dc.DrawText("This is a simple test. Line "+str(i)+".",
                    10,i*17+2)

    dc.EndDrawing()
    self.ostart=vstart

#--------------------------------------------------------------------

class MyTimer(wxTimer):
  def __init__(self,frame):
    wxTimer.__init__(self)
    self.frame_ = frame

  def Notify(self):
    self.frame_.idle()


class MyFrame(wxFrame):
  def __init__(self, parent, id, title):
    wxFrame.__init__(self, parent, id, title,
                     wxPoint(100, 100), wxSize(500, 300))

    # number of entries
    self.num = 30

    # set up the scrolling window...
    self.sw = MyWindow(self.num,self, -1,
                       wxDefaultPosition, wxDefaultSize,
                       wxVSCROLL|wxSUNKEN_BORDER)

    self.sw.SetScrollbars(1,17,0,self.num+1)

    lc = wxLayoutConstraints()
    lc.top.SameAs(self, wxTop, 5)
    lc.left.SameAs(self, wxLeft, 5)
    lc.bottom.SameAs(self, wxBottom, 5)
    lc.right.SameAs(self, wxRight,5)
    self.sw.SetConstraints(lc)

    self.timer=MyTimer(self)
    # stupidly short interval time to accelerate memory leak problem:
    self.timer.Start(80)

  def idle(self):
    #usually just update one or two lines; to accelerate problem,
    #every line is updated here.
    self.sw.update(range(self.num))


######################################################################
# Main procedure....

if __name__ == "__main__":
  class MyApp(wxApp):
    def OnInit(self):

      self.frame = MyFrame(NULL, -1, "Memory Leak Tester")
      self.frame.Show(true)

      self.exiting = FALSE;
      return true

  app = MyApp(0)     # Create an instance of the application class
  app.MainLoop()     # Tell it to start processing events

