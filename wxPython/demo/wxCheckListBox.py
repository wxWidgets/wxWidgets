
from wxPython.wx import *

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        sampleList = ['zero', 'one', 'two', 'three', 'four', 'five',
                      'six', 'seven', 'eight', 'nine', 'ten', 'eleven',
                      'twelve', 'thirteen', 'fourteen']

        wxStaticText(self, -1, "This example uses the wxCheckListBox control.",
                               (45, 15))

        lb = wxCheckListBox(self, 60, (80, 50), (80, 120),
                            sampleList)
        EVT_LISTBOX(self, 60, self.EvtListBox)
        EVT_LISTBOX_DCLICK(self, 60, self.EvtListBoxDClick)
        lb.SetSelection(0)
        self.lb = lb

        pos = lb.GetPosition().x + lb.GetSize().width + 25
        btn = wxButton(self, -1, "Test SetString", (pos, 50))
        EVT_BUTTON(self, btn.GetId(), self.OnTestButton)

        EVT_RIGHT_UP(self, self.OnDoPopup)


    def EvtListBox(self, event):
        self.log.WriteText('EvtListBox: %s\n' % event.GetString())

    def EvtListBoxDClick(self, event):
        self.log.WriteText('EvtListBoxDClick:\n')

    def OnTestButton(self, evt):
        self.lb.SetString(4, "FUBAR")


    def OnDoPopup(self, evt):
        menu = wxMenu()
        # Make this first item bold
        item = wxMenuItem(menu, wxNewId(), "If supported, this is bold")
        df = wxSystemSettings_GetSystemFont(wxSYS_DEFAULT_GUI_FONT)
        nf = wxFont(df.GetPointSize(), df.GetFamily(), df.GetStyle(), wxBOLD,
                    False, df.GetFaceName())
        item.SetFont(nf)
        menu.AppendItem(item)

        menu.AppendItem(wxMenuItem(menu, wxNewId(), "Normal Item &1"))
        menu.AppendItem(wxMenuItem(menu, wxNewId(), "Normal Item &2"))
        menu.AppendItem(wxMenuItem(menu, wxNewId(), "Normal Item &3"))
        menu.AppendItem(wxMenuItem(menu, wxNewId(), "Normal Item &4"))

        self.PopupMenu(menu, evt.GetPosition())
        menu.Destroy()
        evt.Skip()


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------










overview = """\
"""


