
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestLayoutConstraints(wxWindow):
    def __init__(self, parent):
        wxWindow.__init__(self, parent, -1)
        self.SetBackgroundColour(wxNamedColour("MEDIUM ORCHID"))

        self.SetAutoLayout(true)
        EVT_BUTTON(self, 100, self.OnButton)

        self.panelA = wxWindow(self, -1, wxDefaultPosition, wxDefaultSize,
                               wxSIMPLE_BORDER)
        self.panelA.SetBackgroundColour(wxBLUE)
        txt = wxStaticText(self.panelA, -1,
                           "Resize the window and see\n"
                           "what happens...  Notice that\n"
                           "there is no OnSize handler.",
                           wxPoint(5,5), wxSize(-1, 50))
        txt.SetBackgroundColour(wxBLUE)
        txt.SetForegroundColour(wxWHITE)

        lc = wxLayoutConstraints()
        lc.top.SameAs(self, wxTop, 10)
        lc.left.SameAs(self, wxLeft, 10)
        lc.bottom.SameAs(self, wxBottom, 10)
        lc.right.PercentOf(self, wxRight, 50)
        self.panelA.SetConstraints(lc)

        self.panelB = wxWindow(self, -1, wxDefaultPosition, wxDefaultSize,
                               wxSIMPLE_BORDER)
        self.panelB.SetBackgroundColour(wxRED)
        lc = wxLayoutConstraints()
        lc.top.SameAs(self, wxTop, 10)
        lc.right.SameAs(self, wxRight, 10)
        lc.bottom.PercentOf(self, wxBottom, 30)
        lc.left.RightOf(self.panelA, 10)
        self.panelB.SetConstraints(lc)

        self.panelC = wxWindow(self, -1, wxDefaultPosition, wxDefaultSize,
                               wxSIMPLE_BORDER)
        self.panelC.SetBackgroundColour(wxWHITE)
        lc = wxLayoutConstraints()
        lc.top.Below(self.panelB, 10)
        lc.right.SameAs(self, wxRight, 10)
        lc.bottom.SameAs(self, wxBottom, 10)
        lc.left.RightOf(self.panelA, 10)
        self.panelC.SetConstraints(lc)

        b = wxButton(self.panelA, 100, ' Panel A ')
        lc = wxLayoutConstraints()
        lc.centreX.SameAs   (self.panelA, wxCentreX)
        lc.centreY.SameAs   (self.panelA, wxCentreY)
        lc.height.AsIs      ()
        lc.width.PercentOf  (self.panelA, wxWidth, 50)
        b.SetConstraints(lc);

        b = wxButton(self.panelB, 100, ' Panel B ')
        lc = wxLayoutConstraints()
        lc.top.SameAs       (self.panelB, wxTop, 2)
        lc.right.SameAs     (self.panelB, wxRight, 4)
        lc.height.AsIs      ()
        lc.width.AsIs       ()
        b.SetConstraints(lc);

        self.panelD = wxWindow(self.panelC, -1, wxDefaultPosition, wxDefaultSize,
                            wxSIMPLE_BORDER)
        self.panelD.SetBackgroundColour(wxGREEN)
        wxStaticText(self.panelD, -1, "Panel D", wxPoint(4, 4)).SetBackgroundColour(wxGREEN)

        b = wxButton(self.panelC, 100, ' Panel C ')
        lc = wxLayoutConstraints()
        lc.top.Below        (self.panelD)
        lc.left.RightOf     (self.panelD)
        lc.height.AsIs      ()
        lc.width.AsIs       ()
        b.SetConstraints(lc);

        lc = wxLayoutConstraints()
        lc.bottom.PercentOf (self.panelC, wxHeight, 50)
        lc.right.PercentOf  (self.panelC, wxWidth, 50)
        lc.height.SameAs    (b, wxHeight)
        lc.width.SameAs     (b, wxWidth)
        self.panelD.SetConstraints(lc);


    def OnButton(self, event):
        wxBell()


#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestLayoutConstraints(nb)
    return win

#---------------------------------------------------------------------------















overview = """\
Objects of this class can be associated with a window to define its layout constraints, with respect to siblings or its parent.

The class consists of the following eight constraints of class wxIndividualLayoutConstraint, some or all of which should be accessed directly to set the appropriate constraints.

left: represents the left hand edge of the window

right: represents the right hand edge of the window

top: represents the top edge of the window

bottom: represents the bottom edge of the window

width: represents the width of the window

height: represents the height of the window

centreX: represents the horizontal centre point of the window

centreY: represents the vertical centre point of the window

Most constraints are initially set to have the relationship wxUnconstrained, which means that their values should be calculated by looking at known constraints. The exceptions are width and height, which are set to wxAsIs to ensure that if the user does not specify a constraint, the existing width and height will be used, to be compatible with panel items which often have take a default size. If the constraint is wxAsIs, the dimension will not be changed.

wxLayoutConstraints()
-------------------------------------------

Constructor.
"""
