
import  wx

#---------------------------------------------------------------------------

class TestLayoutConstraints(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)
        self.SetAutoLayout(True)
        self.Bind(wx.EVT_BUTTON, self.OnButton, id=100)

        self.SetBackgroundColour(wx.NamedColour("MEDIUM ORCHID"))

        self.panelA = wx.Window(self, -1, style=wx.SIMPLE_BORDER)
        self.panelA.SetBackgroundColour(wx.BLUE)

        txt = wx.StaticText(
                    self.panelA, -1,
                    "Resize the window and see\n"
                    "what happens...  Notice that\n"
                    "there is no OnSize handler.",
                    (5,5), (-1, 50)
                    )

        txt.SetBackgroundColour(wx.BLUE)
        txt.SetForegroundColour(wx.WHITE)

        lc = wx.LayoutConstraints()
        lc.top.SameAs(self, wx.Top, 10)
        lc.left.SameAs(self, wx.Left, 10)
        lc.bottom.SameAs(self, wx.Bottom, 10)
        lc.right.PercentOf(self, wx.Right, 50)
        self.panelA.SetConstraints(lc)

        self.panelB = wx.Window(self, -1, style=wx.SIMPLE_BORDER)
        self.panelB.SetBackgroundColour(wx.RED)
        lc = wx.LayoutConstraints()
        lc.top.SameAs(self, wx.Top, 10)
        lc.right.SameAs(self, wx.Right, 10)
        lc.bottom.PercentOf(self, wx.Bottom, 30)
        lc.left.RightOf(self.panelA, 10)
        self.panelB.SetConstraints(lc)

        self.panelC = wx.Window(self, -1, style=wx.SIMPLE_BORDER)
        self.panelC.SetBackgroundColour(wx.WHITE)
        lc = wx.LayoutConstraints()
        lc.top.Below(self.panelB, 10)
        lc.right.SameAs(self, wx.Right, 10)
        lc.bottom.SameAs(self, wx.Bottom, 10)
        lc.left.RightOf(self.panelA, 10)
        self.panelC.SetConstraints(lc)

        b = wx.Button(self.panelA, 100, ' Panel A ')
        lc = wx.LayoutConstraints()
        lc.centreX.SameAs   (self.panelA, wx.CentreX)
        lc.centreY.SameAs   (self.panelA, wx.CentreY)
        lc.height.AsIs      ()
        lc.width.PercentOf  (self.panelA, wx.Width, 50)
        b.SetConstraints(lc)

        b = wx.Button(self.panelB, 100, ' Panel B ')
        lc = wx.LayoutConstraints()
        lc.top.SameAs       (self.panelB, wx.Top, 2)
        lc.right.SameAs     (self.panelB, wx.Right, 4)
        lc.height.AsIs      ()
        lc.width.AsIs       ()
        b.SetConstraints(lc)

        self.panelD = wx.Window(self.panelC, -1, style=wx.SIMPLE_BORDER)
        self.panelD.SetBackgroundColour(wx.GREEN)
        wx.StaticText(
            self.panelD, -1, "Panel D", (4, 4)
            ).SetBackgroundColour(wx.GREEN)

        b = wx.Button(self.panelC, 100, ' Panel C ')
        lc = wx.LayoutConstraints()
        lc.top.Below        (self.panelD)
        lc.left.RightOf     (self.panelD)
        lc.height.AsIs      ()
        lc.width.AsIs       ()
        b.SetConstraints(lc)

        lc = wx.LayoutConstraints()
        lc.bottom.PercentOf (self.panelC, wx.Height, 50)
        lc.right.PercentOf  (self.panelC, wx.Width, 50)
        lc.height.SameAs    (b, wx.Height)
        lc.width.SameAs     (b, wx.Width)
        self.panelD.SetConstraints(lc)


    def OnButton(self, event):
        wx.Bell()


#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestLayoutConstraints(nb)
    return win

#---------------------------------------------------------------------------



overview = """\
<html><body>
Objects of this class can be associated with a window to define its
layout constraints, with respect to siblings or its parent.

<p>The class consists of the following eight constraints of class
wxIndividualLayoutConstraint, some or all of which should be accessed
directly to set the appropriate constraints.

<p><ul>
<li>left: represents the left hand edge of the window

<li>right: represents the right hand edge of the window

<li>top: represents the top edge of the window

<li>bottom: represents the bottom edge of the window

<li>width: represents the width of the window

<li>height: represents the height of the window

<li>centreX: represents the horizontal centre point of the window

<li>centreY: represents the vertical centre point of the window
</ul>
<p>Most constraints are initially set to have the relationship
wxUnconstrained, which means that their values should be calculated by
looking at known constraints. The exceptions are width and height,
which are set to wxAsIs to ensure that if the user does not specify a
constraint, the existing width and height will be used, to be
compatible with panel items which often have take a default size. If
the constraint is wxAsIs, the dimension will not be changed.

"""




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

