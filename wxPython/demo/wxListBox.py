
from wxPython.wx import *

import string

#---------------------------------------------------------------------------

class wxFindPrefixListBox(wxListBox):
    def __init__(self, parent, id, pos=wxDefaultPosition, size=wxDefaultSize,
                 choices=[], style=0, validator=wxDefaultValidator):
        wxListBox.__init__(self, parent, id, pos, size, choices, style, validator)
        self.typedText = ''
        EVT_KEY_UP(self, self.OnKey)


    def FindPrefix(self, prefix):
        if prefix:
            prefix = string.lower(prefix)
            length = len(prefix)
            for x in range(self.Number()):
                text = self.GetString(x)
                text = string.lower(text)
                if text[:length] == prefix:
                    return x
        return -1


    def OnKey(self, evt):
        key = evt.GetKeyCode()
        if key >= 32 and key <= 127:
            self.typedText = self.typedText + chr(key)
            item = self.FindPrefix(self.typedText)
            if item != -1:
                self.SetSelection(item)

        elif key == WXK_BACK:   # backspace removes one character and backs up
            self.typedText = self.typedText[:-1]
            if not self.typedText:
                self.SetSelection(0)
            else:
                item = self.FindPrefix(self.typedText)
                if item != -1:
                    self.SetSelection(item)

        else:
            evt.Skip()


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
        self.lb1.SetSelection(3)
        self.lb1.Append("with data", "This one has data");
        self.lb1.SetClientData(2, "This one has data");


        wxStaticText(self, -1, "Select many:", wxPoint(200, 50), wxSize(65, 18))
        self.lb2 = wxListBox(self, 70, wxPoint(280, 50), wxSize(80, 120),
                       sampleList, wxLB_EXTENDED)
        EVT_LISTBOX(self, 70, self.EvtMultiListBox)
        EVT_RIGHT_UP(self.lb2, self.EvtRightButton)
        self.lb2.SetSelection(0)


        sampleList = sampleList + ['test a', 'test aa', 'test aab',
                                   'test ab', 'test abc', 'test abcc',
                                   'test abcd' ]
        sampleList.sort()
        wxStaticText(self, -1, "Find Prefix:", wxPoint(15, 250))
        fp = wxFindPrefixListBox(self, -1, wxPoint(80, 250), wxSize(80, 120),
                                 sampleList, wxLB_SINGLE)
        fp.SetSelection(0)


    def EvtListBox(self, event):
        self.log.WriteText('EvtListBox: %s\n' % event.GetString())
        lb = event.GetEventObject()
        data = lb.GetClientData(lb.GetSelection())
        if data is not None:
            self.log.WriteText('\tdata: %s\n' % data)


    def EvtListBoxDClick(self, event):
        self.log.WriteText('EvtListBoxDClick: %s\n' % self.lb1.GetSelection())
        self.lb1.Delete(self.lb1.GetSelection())

    def EvtMultiListBox(self, event):
        self.log.WriteText('EvtMultiListBox: %s\n' % str(self.lb2.GetSelections()))

    def EvtRightButton(self, event):
        self.log.WriteText('EvtRightButton: %s\n' % event.GetPosition())
        if event.GetEventObject().GetId() == 70:
            selections = list(self.lb2.GetSelections())
            selections.reverse()
            for index in selections:
                self.lb2.Delete(index)


#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestListBox(nb, log)
    return win

#---------------------------------------------------------------------------














overview = """\
A listbox is used to select one or more of a list of strings. The strings are displayed in a scrolling box, with the selected string(s) marked in reverse video. A listbox can be single selection (if an item is selected, the previous selection is removed) or multiple selection (clicking an item toggles the item on or off independently of other selections).

"""
