
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestListBox(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)

        sampleList = ['zero', 'one', 'two', 'three', 'four', 'five',
                      'six', 'seven', 'eight', 'nine', 'ten', 'eleven',
                      'twelve', 'thirteen', 'fourteen']

        wxStaticText(self, -1, "This example uses the wxListBox control.",
                               wxPoint(45, 10))

        wxStaticText(self, -1, "Select one:", wxPoint(15, 50), wxSize(65, 18))
        self.lb1 = wxListBox(self, 60, wxPoint(80, 50), wxSize(80, 120),
                       sampleList, wxLB_SINGLE)
        EVT_LISTBOX(self, 60, self.EvtListBox)
        EVT_LISTBOX_DCLICK(self, 60, self.EvtListBoxDClick)
        EVT_RIGHT_UP(self.lb1, self.EvtRightButton)
        self.lb1.SetSelection(0)


        wxStaticText(self, -1, "Select many:", wxPoint(200, 50), wxSize(65, 18))
        self.lb2 = wxListBox(self, 70, wxPoint(280, 50), wxSize(80, 120),
                       sampleList, wxLB_EXTENDED)
        EVT_LISTBOX(self, 70, self.EvtMultiListBox)
        EVT_LISTBOX_DCLICK(self, 70, self.EvtListBoxDClick)
        self.lb2.SetSelection(0)


    def EvtListBox(self, event):
        self.log.WriteText('EvtListBox: %s\n' % event.GetString())

    def EvtListBoxDClick(self, event):
        self.log.WriteText('EvtListBoxDClick: %s\n' % self.lb1.GetSelection())
        self.lb1.Delete(self.lb1.GetSelection())

    def EvtMultiListBox(self, event):
        self.log.WriteText('EvtMultiListBox: %s\n' % str(self.lb2.GetSelections()))

    def EvtRightButton(self, event):
        self.log.WriteText('EvtRightButton: %s\n' % event.GetPosition())

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestListBox(nb, log)
    return win

#---------------------------------------------------------------------------














overview = """\
A listbox is used to select one or more of a list of strings. The strings are displayed in a scrolling box, with the selected string(s) marked in reverse video. A listbox can be single selection (if an item is selected, the previous selection is removed) or multiple selection (clicking an item toggles the item on or off independently of other selections).

List box elements are numbered from zero.

wxListBox()
---------------------

Default constructor.

wxListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listBox")

Constructor, creating and showing a list box.

Parameters
-------------------

parent = Parent window. Must not be NULL.

id = Window identifier. A value of -1 indicates a default value.

pos = Window position.

size = Window size. If the default size (-1, -1) is specified then the window is sized appropriately.

n = Number of strings with which to initialise the control.

choices = An array of strings with which to initialise the control.

style = Window style. See wxListBox.

validator = Window validator.

name = Window name.
"""
