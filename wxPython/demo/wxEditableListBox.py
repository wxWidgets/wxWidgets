from wxPython.wx import *
from wxPython.gizmos import *

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        self.elb = wxEditableListBox(self, -1, "List of Stuff",
                                     (50,50), (250, 250),
                                     )
                                     #style=wxEL_ALLOW_NEW | wxEL_ALLOW_EDIT | wxEL_ALLOW_DELETE)

        self.elb.SetStrings(["This is a nifty ListBox widget",
                             "that is editable by the user.",
                             "",
                             "Use the buttons above to",
                             "manipulate items in the list",
                             "Or to add new ones.",
                             ])



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


#----------------------------------------------------------------------





overview = """\
This class provides a composite control that lets the
user easily enter and edit a list of strings.
"""
