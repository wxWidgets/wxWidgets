from wxPython.wx import *
from wxPython.lib.scrolledpanel import wxScrolledPanel

#----------------------------------------------------------------------

text = "one two buckle my shoe three four shut the door five six pick up sticks seven eight lay them straight nine ten big fat hen"


class TestPanel(wxScrolledPanel):
    def __init__(self, parent, log):
        self.log = log
        wxScrolledPanel.__init__(self, parent, -1)

        vbox = wxBoxSizer(wxVERTICAL)
        desc = wxStaticText(self, -1,
                            "wxScrolledPanel extends wxScrolledWindow, adding all "
                            "the necessary bits to set up scroll handling for you.\n\n"
                            "Here are three fixed size examples of its use. The "
                            "demo panel for this sample is also using it -- the \nwxStaticLine"
                            "below is intentionally made too long so a scrollbar will be "
                            "activated."
                            )
        desc.SetForegroundColour("Blue")
        vbox.Add(desc, 0, wxALIGN_LEFT|wxALL, 5)
        vbox.Add(wxStaticLine(self, -1, size=(1024,-1)), 0, wxALL, 5)
        vbox.AddSpacer(20,20)

        words = text.split()

        panel1 = wxScrolledPanel(self, -1, size=(120,300),
                                 style = wxTAB_TRAVERSAL|wxSUNKEN_BORDER )
        fgs1 = wxFlexGridSizer(cols=2, vgap=4, hgap=4)

        for word in words:
            label = wxStaticText(panel1, -1, word+":")
            tc = wxTextCtrl(panel1, -1, word, size=(50,-1))
            fgs1.Add(label, flag=wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL)
            fgs1.Add(tc, flag=wxEXPAND|wxRIGHT, border=25)

        panel1.SetSizer( fgs1 )
        panel1.SetAutoLayout(1)
        panel1.SetupScrolling( scroll_x=False )

        panel2 = wxScrolledPanel(self, -1, size=(350, 40),
                                 style = wxTAB_TRAVERSAL|wxSUNKEN_BORDER)
        panel3 = wxScrolledPanel(self, -1, size=(200,100),
                                 style = wxTAB_TRAVERSAL|wxSUNKEN_BORDER)

        fgs2 = wxFlexGridSizer(cols=25, vgap=4, hgap=4)
        fgs3 = wxFlexGridSizer(cols=5, vgap=4, hgap=4)

        for i in range(len(words)):
            word = words[i]
            if i % 5 != 4:
                label2 = wxStaticText(panel2, -1, word)
                fgs2.Add(label2, flag=wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL)
                label3 = wxStaticText(panel3, -1, word)
                fgs3.Add(label3, flag=wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL)
            else:
                tc2 = wxTextCtrl(panel2, -1, word, size=(50,-1))
                fgs2.Add(tc2, flag=wxLEFT, border=5)
                tc3 = wxTextCtrl(panel3, -1, word )
                fgs3.Add(tc3, flag=wxLEFT, border=5)

        panel2.SetSizer( fgs2 )
        panel2.SetAutoLayout(1)
        panel2.SetupScrolling(scroll_y = False)

        panel3.SetSizer( fgs3 )
        panel3.SetAutoLayout(1)
        panel3.SetupScrolling()

        hbox = wxBoxSizer(wxHORIZONTAL)
        hbox.AddSpacer(20,20)
        hbox.Add(panel1, 0)
        hbox.AddSpacer(40, 10)

        vbox2 = wxBoxSizer(wxVERTICAL)
        vbox2.Add(panel2, 0)
        vbox2.AddSpacer(20, 50)

        vbox2.Add(panel3, 0)
        vbox2.AddSpacer(20, 10)
        hbox.Add(vbox2)

        vbox.AddSizer(hbox, 0)
        self.SetSizer(vbox)
        self.SetAutoLayout(1)
        self.SetupScrolling()


#----------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
wxScrolledPanel fills a "hole" in the implementation of wxScrolledWindow,
providing automatic scrollbar and scrolling behavior and the tab traversal
mangement that wxScrolledWindow lacks.
</body></html>
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
