from wxPython.wx import *
#from Lib.Gui.PlainWidgets import *

class TestLayoutConstraints(wxPanel):
    def __init__(self, parent):
        wxPanel.__init__(self, parent, -1)
        #
        nb = wxNotebook(self, -1)
        page = wxPanel(nb, -1)
        page.SetBackgroundColour(wxBLUE)
        button = wxButton(page, -1, 'press me')
        #
        nb.AddPage(page, 'testpage')
        #
        lc = wxLayoutConstraints()
        lc.top.PercentOf(parent, wxBottom, 0)
        lc.bottom.PercentOf(parent, wxBottom, 100)
        lc.left.PercentOf(parent, wxRight, 0)
        lc.right.PercentOf(parent, wxRight, 100)
        self.SetConstraints(lc)
        self.SetAutoLayout(true)
        #
        lc = wxLayoutConstraints()
        lc.top.PercentOf(self, wxBottom, 0)
        lc.bottom.PercentOf(self, wxBottom, 100)
        lc.left.PercentOf(self, wxRight, 0)
        lc.right.PercentOf(self, wxRight, 100)
        nb.SetConstraints(lc)
#        nb.SetAutoLayout(true)
        #
#        lc = wxLayoutConstraints()
#        lc.top.PercentOf(nb, wxBottom, 0)
#        lc.bottom.PercentOf(nb, wxBottom, 100)
#        lc.left.PercentOf(nb, wxRight, 0)
#        lc.right.PercentOf(nb, wxRight, 100)
#        page.SetConstraints(lc)
        page.SetAutoLayout(true)

        # this should center "button" on "page":
        lc = wxLayoutConstraints()
        lc.centreY.PercentOf(page, wxBottom, 50)
        lc.centreX.PercentOf(page, wxRight, 50)
        lc.width.AsIs()
        lc.height.AsIs()
        button.SetConstraints(lc)
#        button.SetAutoLayout(true)
        #
        button.Layout()
        page.Layout()
        nb.Layout()
        self.Layout()


if __name__ == "__main__":
    class MyFrame(wxFrame):
        def __init__(self, *argT, **optionD):
            apply(wxFrame.__init__, (self,) + argT, optionD)
            self.SetAutoLayout(true)
            TestLayoutConstraints(self)

    class MyApp(wxApp):
        def OnInit(self):
            frame = MyFrame(None, -1, "TestLayoutConstraints",
                            size=wxSize(400,300))
            frame.Show(true)
            return true

    app = MyApp()
    app.MainLoop()
