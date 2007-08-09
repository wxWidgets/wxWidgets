
import  wx

#---------------------------------------------------------------------------

# This listbox subclass lets you type the starting letters of what you want to
# select, and scrolls the list to the match if it is found.
class FindPrefixListBox(wx.ListBox):
    def __init__(self, parent, id, pos=wx.DefaultPosition, size=wx.DefaultSize,
                 choices=[], style=0, validator=wx.DefaultValidator):
        wx.ListBox.__init__(self, parent, id, pos, size, choices, style, validator)
        self.typedText = ''
        self.log = parent.log
        self.Bind(wx.EVT_KEY_DOWN, self.OnKey)


    def FindPrefix(self, prefix):
        self.log.WriteText('Looking for prefix: %s\n' % prefix)

        if prefix:
            prefix = prefix.lower()
            length = len(prefix)

            # Changed in 2.5 because ListBox.Number() is no longer supported.
            # ListBox.GetCount() is now the appropriate way to go.
            for x in range(self.GetCount()):
                text = self.GetString(x)
                text = text.lower()

                if text[:length] == prefix:
                    self.log.WriteText('Prefix %s is found.\n' % prefix)
                    return x

        self.log.WriteText('Prefix %s is not found.\n' % prefix)
        return -1


    def OnKey(self, evt):
        key = evt.GetKeyCode()

        if key >= 32 and key <= 127:
            self.typedText = self.typedText + chr(key)
            item = self.FindPrefix(self.typedText)

            if item != -1:
                self.SetSelection(item)

        elif key == wx.WXK_BACK:   # backspace removes one character and backs up
            self.typedText = self.typedText[:-1]

            if not self.typedText:
                self.SetSelection(0)
            else:
                item = self.FindPrefix(self.typedText)

                if item != -1:
                    self.SetSelection(item)
        else:
            self.typedText = ''
            evt.Skip()

    def OnKeyDown(self, evt):
        pass


#---------------------------------------------------------------------------

class TestListBox(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        sampleList = ['zero', 'one', 'two', 'three', 'four', 'five',
                      'six', 'seven', 'eight', 'nine', 'ten', 'eleven',
                      'twelve', 'thirteen', 'fourteen']

        wx.StaticText(self, -1, "This example uses the wx.ListBox control.", (45, 10))
        wx.StaticText(self, -1, "Select one:", (15, 50))
        self.lb1 = wx.ListBox(self, 60, (100, 50), (90, 120), sampleList, wx.LB_SINGLE)
        self.Bind(wx.EVT_LISTBOX, self.EvtListBox, self.lb1)
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.EvtListBoxDClick, self.lb1)
        self.lb1.Bind(wx.EVT_RIGHT_UP, self.EvtRightButton)
        self.lb1.SetSelection(3)
        self.lb1.Append("with data", "This one has data");
        self.lb1.SetClientData(2, "This one has data");


        wx.StaticText(self, -1, "Select many:", (220, 50))
        self.lb2 = wx.ListBox(self, 70, (320, 50), (90, 120), sampleList, wx.LB_EXTENDED)
        self.Bind(wx.EVT_LISTBOX, self.EvtMultiListBox, self.lb2)
        self.lb2.Bind(wx.EVT_RIGHT_UP, self.EvtRightButton)
        self.lb2.SetSelection(0)

        sampleList = sampleList + ['test a', 'test aa', 'test aab',
                                   'test ab', 'test abc', 'test abcc',
                                   'test abcd' ]
        sampleList.sort()
        wx.StaticText(self, -1, "Find Prefix:", (15, 250))
        fp = FindPrefixListBox(self, -1, (100, 250), (90, 120), sampleList, wx.LB_SINGLE)
        fp.SetSelection(0)


    def EvtListBox(self, event):
        self.log.WriteText('EvtListBox: %s, %s, %s, %s\n' %
                           (event.GetString(),
                            event.IsSelection(),
                            event.GetSelection(),
                            event.GetClientData()))

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




overview = """<html><body>
A listbox is used to select one or more of a list of
strings. The strings are displayed in a scrolling box, with the
selected string(s) marked in reverse video. A listbox can be single
selection (if an item is selected, the previous selection is removed)
or multiple selection (clicking an item toggles the item on or off
independently of other selections).
</body></html>
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

