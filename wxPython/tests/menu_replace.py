
from wxPython.wx import *

class MainWindowFrame(wxFrame):

    def __init__(self, parentframe, title):

        wxFrame.__init__(self, parentframe, -1,
                         title, size=wxSize(500, 200))

        self.mb = wxMenuBar()
        self.SetMenuBar(self.mb)
        self.sbut = wxButton(self, -1,
                             'switch menu (push twice and then use menu)')
        EVT_BUTTON(self, self.sbut.GetId(), self.switch)

        #help menu
        self.helpmenu = wxMenu()
        self.othermenu = wxMenu()

        self.mb.Append(self.helpmenu, '&Help')


    def switch(self, event):

        lastmenu = self.mb.GetMenuCount() - 1
        self.mb.Replace(lastmenu, self.othermenu, 'Other')



class TheApp(wxApp):

    def OnInit(self):

        mainwin = MainWindowFrame(NULL, 'menutest')
        mainwin.Show(true)
        self.SetTopWindow(mainwin)
        return true

app = TheApp(0)
app.MainLoop()


