
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestChoice(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)

        sampleList = ['zero', 'one', 'two', 'three', 'four', 'five',
                      'six', 'seven', 'eight']

        wxStaticText(self, -1, "This example uses the wxChoice control.",
                               wxPoint(15, 10))

        wxStaticText(self, -1, "Select one:", wxPoint(15, 50), wxSize(75, 20))
        wxChoice(self, 40, (80, 50), (95, 125),
                 choices = sampleList)
        EVT_CHOICE(self, 40, self.EvtChoice)

    def EvtChoice(self, event):
        self.log.WriteText('EvtChoice: %s\n' % event.GetString())

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestChoice(nb, log)
    return win

#---------------------------------------------------------------------------












overview = """\
A choice item is used to select one of a list of strings. Unlike a listbox, only the selection is visible until the user pulls down the menu of choices.

wxChoice()
-------------------

Default constructor.

wxChoice(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int n, const wxString choices[], long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "choice")

Constructor, creating and showing a choice.

Parameters
-------------------

parent = Parent window. Must not be NULL.

id = Window identifier. A value of -1 indicates a default value.

pos = Window position.

size = Window size. If the default size (-1, -1) is specified then the choice is sized appropriately.

n = Number of strings with which to initialise the choice control.

choices = An array of strings with which to initialise the choice control.

style = Window style. See wxChoice.

validator = Window validator.

name = Window name.
"""
