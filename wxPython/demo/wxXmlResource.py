
from wxPython.wx import *
from wxPython.xrc import *
from Main import opj

#----------------------------------------------------------------------

RESFILE = opj("data/resource_wdr.xrc")

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        # make the components
        label = wxStaticText(self, -1, "The lower panel was built from this XML:")
        label.SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD))

        resourceText = open(RESFILE).read()
        text = wxTextCtrl(self, -1, resourceText,
                          style=wxTE_READONLY|wxTE_MULTILINE)
        text.SetInsertionPoint(0)

        line = wxStaticLine(self, -1)

        # This shows a few different ways to load XML Resources
        if 0:
            # XML Resources can be loaded from a file like this:
            res = wxXmlResource(RESFILE)

        elif 1:
            # or from a Virtual FileSystem:
            wxFileSystem_AddHandler(wxMemoryFSHandler())
            wxMemoryFSHandler_AddFile("XRC_Resources/data_file", resourceText)
            res = wxXmlResource("memory:XRC_Resources/data_file")

        else:
            # or from a string, like this:
            res = wxEmptyXmlResource()
            res.LoadFromString(resourceText)


        # Now create a panel from the resource data
        panel = res.LoadPanel(self, "MyPanel")

        # and do the layout
        sizer = wxBoxSizer(wxVERTICAL)
        sizer.Add(label, 0, wxEXPAND|wxTOP|wxLEFT, 5)
        sizer.Add(text, 1, wxEXPAND|wxALL, 5)
        sizer.Add(line, 0, wxEXPAND)
        sizer.Add(panel, 1, wxEXPAND|wxALL, 5)

        self.SetSizer(sizer)
        self.SetAutoLayout(True)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

