
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
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.EvtListBoxDClick, lb)
        lb.SetSelection(0)
        self.lb = lb

        pos = lb.GetPosition().x + lb.GetSize().width + 25
        btn = wx.Button(self, -1, "Test SetString", (pos, 50))
        self.Bind(wx.EVT_BUTTON, self.OnTestButton, btn)
        self.Bind(wx.EVT_RIGHT_UP, self.OnDoPopup)

    def EvtListBox(self, event):
        self.log.WriteText('EvtListBox: %s\n' % event.GetString())

    def EvtListBoxDClick(self, event):
        self.log.WriteText('EvtListBoxDClick:\n')

    def OnTestButton(self, evt):
        self.lb.SetString(4, "FUBAR")


    def OnDoPopup(self, evt):
        menu = wx.Menu()
        # Make this first item bold
        item = wx.MenuItem(menu, wx.NewId(), "If supported, this is bold")
        df = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)

        nf = wx.Font(
                df.GetPointSize(), df.GetFamily(), df.GetStyle(), 
                wx.BOLD, False, df.GetFaceName()
                )

        item.SetFont(nf)
        menu.AppendItem(item)

        menu.AppendItem(wx.MenuItem(menu, wx.NewId(), "Normal Item &1"))
        menu.AppendItem(wx.MenuItem(menu, wx.NewId(), "Normal Item &2"))
        menu.AppendItem(wx.MenuItem(menu, wx.NewId(), "Normal Item &3"))
        menu.AppendItem(wx.MenuItem(menu, wx.NewId(), "Normal Item &4"))

        self.PopupMenu(menu, evt.GetPosition())
        menu.Destroy()
        evt.Skip()


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

