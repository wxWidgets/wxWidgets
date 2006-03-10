
import  wx

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log

        sampleList = ['zero', 'one', 'two', 'three', 'four', 'five',
                      'six', 'seven', 'eight', 'nine', 'ten', 'eleven',
                      'twelve', 'thirteen', 'fourteen']

        wx.StaticText(self, -1, "This example uses the wxCheckListBox control.", (45, 15))

        lb = wx.CheckListBox(self, -1, (80, 50), wx.DefaultSize, sampleList)
        self.Bind(wx.EVT_LISTBOX, self.EvtListBox, lb)
        self.Bind(wx.EVT_CHECKLISTBOX, self.EvtCheckListBox, lb)
        lb.SetSelection(0)
        self.lb = lb

        lb.Bind(wx.EVT_RIGHT_DOWN, self.OnDoHitTest)
        
        pos = lb.GetPosition().x + lb.GetSize().width + 25
        btn = wx.Button(self, -1, "Test SetString", (pos, 50))
        self.Bind(wx.EVT_BUTTON, self.OnTestButton, btn)

    def EvtListBox(self, event):
        self.log.WriteText('EvtListBox: %s\n' % event.GetString())

    def EvtCheckListBox(self, event):
        index = event.GetSelection()
        label = self.lb.GetString(index)
        status = 'un'
        if self.lb.IsChecked(index):
            status = ''
        self.log.WriteText('Box %s is %schecked \n' % (label, status))
        self.lb.SetSelection(index)    # so that (un)checking also selects (moves the highlight)
        

    def OnTestButton(self, evt):
        self.lb.SetString(4, "FUBAR")

    def OnDoHitTest(self, evt):
        item = self.lb.HitTest(evt.GetPosition())
        self.log.write("HitTest: %d\n" % item)

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------


overview = """\
A checklistbox is like a Listbox, but allows items to be checked or unchecked rather
than relying on extended selection (e.g. shift-select) to select multiple items in
the list. 

This class is currently implemented under Windows and GTK. 

This demo shows the basic CheckListBox and how to use the SetString method to change
labels dynamically.
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

