#----------------------------------------------------------------------
# sizer test code
#----------------------------------------------------------------------

from wxPython.wx         import *
from wxPython.lib.sizers import *

#----------------------------------------------------------------------

def makeSimpleBox1(win):
    box = wxBoxSizer(wxHORIZONTAL)
    box.Add(wxButton(win, 1010, "one"), 0)
    box.Add(wxButton(win, 1010, "two"), 0)
    box.Add(wxButton(win, 1010, "three"), 0)
    box.Add(wxButton(win, 1010, "four"), 0)

    return box

#----------------------------------------------------------------------

def makeSimpleBox2(win):
    box = wxBoxSizer(wxVERTICAL)
    box.Add(wxButton(win, 1010, "one"), 0)
    box.Add(wxButton(win, 1010, "two"), 0)
    box.Add(wxButton(win, 1010, "three"), 0)
    box.Add(wxButton(win, 1010, "four"), 0)

    return box

#----------------------------------------------------------------------

def makeSimpleBox3(win):
    box = wxBoxSizer(wxHORIZONTAL)
    box.Add(wxButton(win, 1010, "one"), 0)
    box.Add(wxButton(win, 1010, "two"), 0)
    box.Add(wxButton(win, 1010, "three"), 0)
    box.Add(wxButton(win, 1010, "four"), 0)
    box.Add(wxButton(win, 1010, "five"), 1)

    return box

#----------------------------------------------------------------------

def makeSimpleBox4(win):
    box = wxBoxSizer(wxHORIZONTAL)
    box.Add(wxButton(win, 1010, "one"), 0)
    box.Add(wxButton(win, 1010, "two"), 0)
    box.Add(wxButton(win, 1010, "three"), 1)
    box.Add(wxButton(win, 1010, "four"), 1)
    box.Add(wxButton(win, 1010, "five"), 1)

    return box

#----------------------------------------------------------------------

def makeSimpleBox5(win):
    box = wxBoxSizer(wxHORIZONTAL)
    box.Add(wxButton(win, 1010, "one"), 0)
    box.Add(wxButton(win, 1010, "two"), 0)
    box.Add(wxButton(win, 1010, "three"), 3)
    box.Add(wxButton(win, 1010, "four"), 1)
    box.Add(wxButton(win, 1010, "five"), 1)

    return box

#----------------------------------------------------------------------

def makeSimpleBox6(win):
    box = wxBoxSizer(wxHORIZONTAL, wxSize(250, 50))
    box.Add(wxButton(win, 1010, "10"), 10)
    box.Add(wxButton(win, 1010, "20"), 20)
    box.Add(wxButton(win, 1010, "30"), 30)
    box.Add(wxButton(win, 1010, "15"), 15)
    box.Add(wxButton(win, 1010, "5"), 5)

    return box

#----------------------------------------------------------------------

def makeSimpleBorder1(win):
    bdr = wxBorderSizer(wxALL)
    btn = wxButton(win, 1010, "border")
    btn.SetSize(wxSize(80, 80))
    bdr.Add(btn, 15)

    return bdr

#----------------------------------------------------------------------

def makeSimpleBorder2(win):
    bdr = wxBorderSizer(wxEAST | wxWEST)
    btn = wxButton(win, 1010, "border")
    btn.SetSize(wxSize(80, 80))
    bdr.Add(btn, 15)

    return bdr

#----------------------------------------------------------------------

def makeSimpleBorder3(win):
    bdr = wxBorderSizer(wxNORTH | wxWEST)
    btn = wxButton(win, 1010, "border")
    btn.SetSize(wxSize(80, 80))
    bdr.Add(btn, 15)

    return bdr

#----------------------------------------------------------------------
#----------------------------------------------------------------------

def makeBoxInBox(win):
    box = wxBoxSizer(wxVERTICAL)

    box.Add(wxButton(win, 1010, "one"))

    box2 = wxBoxSizer(wxHORIZONTAL)
    box2.AddMany([ wxButton(win, 1010, "two"),
                   wxButton(win, 1010, "three"),
                   wxButton(win, 1010, "four"),
                   wxButton(win, 1010, "five"),
                   ])

    box3 = wxBoxSizer(wxVERTICAL)
    box3.AddMany([ (wxButton(win, 1010, "six"),   0),
                   (wxButton(win, 1010, "seven"), 2),
                   (wxButton(win, 1010, "eight"), 1),
                   (wxButton(win, 1010, "nine"),  1),
                   ])

    box2.Add(box3, 1)
    box.Add(box2, 1)

    box.Add(wxButton(win, 1010, "ten"))

    return box

#----------------------------------------------------------------------

def makeBoxInBorder(win):
    bdr = wxBorderSizer(wxALL)
    box = makeSimpleBox3(win)
    bdr.Add(box, 15)

    return bdr

#----------------------------------------------------------------------

def makeBorderInBox(win):
    insideBox = wxBoxSizer(wxHORIZONTAL)

    box2 = wxBoxSizer(wxHORIZONTAL)
    box2.AddMany([ wxButton(win, 1010, "one"),
                   wxButton(win, 1010, "two"),
                   wxButton(win, 1010, "three"),
                   wxButton(win, 1010, "four"),
                   wxButton(win, 1010, "five"),
                 ])

    insideBox.Add(box2, 0)

    bdr = wxBorderSizer(wxALL)
    bdr.Add(wxButton(win, 1010, "border"), 20)
    insideBox.Add(bdr, 1)

    box3 = wxBoxSizer(wxVERTICAL)
    box3.AddMany([ (wxButton(win, 1010, "six"),   0),
                   (wxButton(win, 1010, "seven"), 2),
                   (wxButton(win, 1010, "eight"), 1),
                   (wxButton(win, 1010, "nine"),  1),
                 ])
    insideBox.Add(box3, 1)

    outsideBox = wxBoxSizer(wxVERTICAL)
    outsideBox.Add(wxButton(win, 1010, "top"))
    outsideBox.Add(insideBox, 1)
    outsideBox.Add(wxButton(win, 1010, "bottom"))

    return outsideBox


#----------------------------------------------------------------------

theTests = [
    ("Simple horizontal boxes", makeSimpleBox1,
     "This is a HORIZONTAL box sizer with four non-stretchable buttons held "
     "within it.  Notice that the buttons are added and aligned in the horizontal "
     "dimension.  Also notice that they are fixed size in the horizontal dimension, "
     "but will stretch vertically."
     ),

    ("Simple vertical boxes", makeSimpleBox2,
     "Exactly the same as the previous sample but using a VERTICAL box sizer "
     "instead of a HORIZONTAL one."
     ),

    ("Add a stretchable", makeSimpleBox3,
     "We've added one more button with the strechable flag turned on.  Notice "
     "how it grows to fill the extra space in the otherwise fixed dimension."
     ),

    ("More than one stretchable", makeSimpleBox4,
     "Here there are several items that are stretchable, they all divide up the "
     "extra space evenly."
     ),

    ("Weighting factor", makeSimpleBox5,
     "This one shows more than one strechable, but one of them has a weighting "
     "factor so it gets more of the free space."
     ),

#    ("Percent Sizer", makeSimpleBox6,
#     "You can use the wxBoxSizer like a Percent Sizer.  Just make sure that all "
#     "the weighting factors add up to 100!"
#     ),

    ("", None, ""),

    ("Simple border sizer", makeSimpleBorder1,
     "The wxBorderSizer leaves empty space around its contents.  This one "
     "gives a border all the way around."
     ),

    ("East and West border", makeSimpleBorder2,
     "You can pick and choose which sides have borders."
     ),

    ("North and West border", makeSimpleBorder3,
     "You can pick and choose which sides have borders."
     ),

    ("", None, ""),

    ("Boxes inside of boxes", makeBoxInBox,
     "This one shows nesting of boxes within boxes within boxes, using both "
     "orientations.  Notice also that button seven has a greater weighting "
     "factor than its siblings."
     ),

    ("Boxes inside a Border", makeBoxInBorder,
     "Sizers of different types can be nested withing each other as well. "
     "Here is a box sizer with several buttons embedded within a border sizer."
     ),

    ("Border in a Box", makeBorderInBox,
     "Another nesting example.  This one has Boxes and a Border inside another Box."
     ),

    ]
#----------------------------------------------------------------------

class TestFrame(wxFrame):
    def __init__(self, parent, title, sizerFunc):
        wxFrame.__init__(self, parent, -1, title)
        EVT_BUTTON(self, 1010, self.OnButton)

        self.sizer = sizerFunc(self)
        self.CreateStatusBar()
        self.SetStatusText("Resize this frame to see how the sizers respond...")
        self.sizer.FitWindow(self)


    def OnSize(self, event):
        size = self.GetClientSize()
        self.sizer.Layout(size)

    def OnCloseWindow(self, event):
        self.MakeModal(false)
        self.Destroy()

    def OnButton(self, event):
        self.Close(true)

#----------------------------------------------------------------------



class TestSelectionPanel(wxPanel):
    def __init__(self, parent, frame):
        wxPanel.__init__(self, parent, -1)
        self.frame = frame

        self.list = wxListBox(self, 401,
                              wxDLG_PNT(self, 10, 10), wxDLG_SZE(self, 100, 60),
                              [])
        EVT_LISTBOX(self, 401, self.OnSelect)
        EVT_LISTBOX_DCLICK(self, 401, self.OnDClick)

        wxButton(self, 402, "Try it!", wxDLG_PNT(self, 120, 10)).SetDefault()
        EVT_BUTTON(self, 402, self.OnDClick)

        self.text = wxTextCtrl(self, -1, "",
                               wxDLG_PNT(self, 10, 80),
                               wxDLG_SZE(self, 200, 60),
                               wxTE_MULTILINE | wxTE_READONLY)

        for item in theTests:
            self.list.Append(item[0])



    def OnSelect(self, event):
        pos = self.list.GetSelection()
        self.text.SetValue(theTests[pos][2])


    def OnDClick(self, event):
        pos = self.list.GetSelection()
        title = theTests[pos][0]
        func = theTests[pos][1]

        if func:
            win = TestFrame(self, title, func)
            win.CentreOnParent(wxBOTH)
            win.Show(true)
            win.MakeModal(true)

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestSelectionPanel(nb, frame)
    return win

overview = wxSizer.__doc__        + '\n' + '-' * 80 + '\n' + \
           wxBoxSizer.__doc__     + '\n' + '-' * 80 + '\n' + \
           wxBorderSizer.__doc__

#----------------------------------------------------------------------



if __name__ == '__main__':

    class MainFrame(wxFrame):
        def __init__(self):
            wxFrame.__init__(self, NULL, -1, "Testing...")

	    self.CreateStatusBar()
	    mainmenu = wxMenuBar()
	    menu = wxMenu()
	    menu.Append(200, 'E&xit', 'Get the heck outta here!')
	    mainmenu.Append(menu, "&File")
	    self.SetMenuBar(mainmenu)
            EVT_MENU(self, 200, self.OnExit)
            self.panel = TestSelectionPanel(self, self)
            self.SetSize(wxSize(400, 380))

        def OnCloseWindow(self, event):
            self.Destroy()

        def OnExit(self, event):
            self.Close(true)


    class TestApp(wxApp):
        def OnInit(self):
            frame = MainFrame()
            frame.Show(true)
            self.SetTopWindow(frame)
            return true

    app = TestApp(0)
    app.MainLoop()


#----------------------------------------------------------------------
