
from wxPython.wx          import *
from wxPython.lib.layoutf import Layoutf

#---------------------------------------------------------------------------

class TestLayoutf(wxPanel):
    def __init__(self, parent):
        wxPanel.__init__(self, parent, -1)

        self.SetAutoLayout(true)
        EVT_BUTTON(self, 100, self.OnButton)

        self.panelA = wxWindow(self, -1, wxPyDefaultPosition, wxPyDefaultSize, wxSIMPLE_BORDER)
        self.panelA.SetBackgroundColour(wxBLUE)
        self.panelA.SetConstraints(Layoutf('t=t10#1;l=l10#1;b=b10#1;r%r50#1',(self,)))

        self.panelB = wxWindow(self, -1, wxPyDefaultPosition, wxPyDefaultSize, wxSIMPLE_BORDER)
        self.panelB.SetBackgroundColour(wxRED)
        self.panelB.SetConstraints(Layoutf('t=t10#1;r=r10#1;b%b30#1;l>10#2', (self,self.panelA)))

        self.panelC = wxWindow(self, -1, wxPyDefaultPosition, wxPyDefaultSize, wxSIMPLE_BORDER)
        self.panelC.SetBackgroundColour(wxWHITE)
        self.panelC.SetConstraints(Layoutf('t_10#3;r=r10#1;b=b10#1;l>10#2', (self,self.panelA,self.panelB)))

        b = wxButton(self.panelA, 100, ' Panel A ')
        b.SetConstraints(Layoutf('X=X#1;Y=Y#1;h*;w%w50#1', (self.panelA,)))

        b = wxButton(self.panelB, 100, ' Panel B ')
        b.SetConstraints(Layoutf('t=t2#1;r=r4#1;h*;w*', (self.panelB,)))

        self.panelD = wxWindow(self.panelC, -1, wxPyDefaultPosition, wxPyDefaultSize, wxSIMPLE_BORDER)
        self.panelD.SetBackgroundColour(wxGREEN)
        self.panelD.SetConstraints(Layoutf('b%h50#1;r%w50#1;h=h#2;w=w#2', (self.panelC, b)))

        b = wxButton(self.panelC, 100, ' Panel C ')
        b.SetConstraints(Layoutf('t_#1;l>#1;h*;w*', (self.panelD,)))

        wxStaticText(self.panelD, -1, "Panel D", wxPoint(4, 4)).SetBackgroundColour(wxGREEN)

    def OnButton(self, event):
        wxBell()



#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestLayoutf(nb)
    return win

#---------------------------------------------------------------------------









overview = Layoutf.__doc__
