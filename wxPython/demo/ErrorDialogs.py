# demo for ErrorDialogs.py
# usual wxWindows license stuff here.
# by Chris Fama, with thanks to Robin Dunn, and others on the wxPython-users
# mailing list.

from wxPython.wx import *
from wxPython.lib.ErrorDialogs import *
_debug = 0
ID_BUTTON_wxPyFatalErrorDialog = 10001
ID_BUTTON_wxPyNonFatalErrorDialog = 10002
ID_BUTTON_wxPyFatalErrorDialogWithTraceback = 10003
ID_BUTTON_wxPyNonFatalErrorDialogWithTraceback = 10004

def ErrorDialogsDemoPanelFunc( parent, call_fit = true, set_sizer = true ):
    item0 = wxBoxSizer( wxVERTICAL )

    item1 = wxStaticText( parent, -1, "Please select one of the buttons below...", wxDefaultPosition, wxDefaultSize, 0 )
    item0.AddWindow( item1, 0, wxALIGN_CENTRE|wxALL, 5 )

    item2 = wxFlexGridSizer( 0, 2, 0, 0 )

    item6 = wxButton( parent, ID_BUTTON_wxPyNonFatalErrorDialogWithTraceback, "wxPyNonFatalErrorDialogWithTraceback", wxDefaultPosition, wxDefaultSize, 0 )
    item6.SetDefault()
    item2.AddWindow( item6, 0, wxALIGN_CENTRE|wxALL, 5 )

    item3 = wxButton( parent, ID_BUTTON_wxPyFatalErrorDialog, "wxPyFatalErrorDialog", wxDefaultPosition, wxDefaultSize, 0 )
    item2.AddWindow( item3, 0, wxALIGN_CENTRE|wxALL, 5 )

    item4 = wxButton( parent, ID_BUTTON_wxPyNonFatalErrorDialog, "wxPyNonFatalErrorDialog", wxDefaultPosition, wxDefaultSize, 0 )
    item2.AddWindow( item4, 0, wxALIGN_CENTRE|wxALL, 5 )

    item5 = wxButton( parent, ID_BUTTON_wxPyFatalErrorDialogWithTraceback, "wxPyFatalErrorDialogWithTraceback", wxDefaultPosition, wxDefaultSize, 0 )
    item2.AddWindow( item5, 0, wxALIGN_CENTRE|wxALL, 5 )

    item0.AddSizer( item2, 0, wxALIGN_CENTRE|wxALL, 5 )

    if set_sizer == true:
        parent.SetAutoLayout( true )
        parent.SetSizer( item0 )
        if call_fit == true:
            item0.Fit( parent )
            item0.SetSizeHints( parent )

    return item0

# End of generated bit

class MyPanel(wxPanel):
    def __init__(self,parent=None):
        wxPanel.__init__(self,parent,-1)
        args = (None, -1)
        kwargs = {
            'programname': "sumthing",
            'mailto': "me@sumwear",
            'whendismissed': "from wxPython.wx import * ; wxBell()"}
        self.dialogs = map(apply,
                           [wxPyNonFatalErrorDialogWithTraceback,
                            wxPyNonFatalErrorDialog,#WithTraceback
                            wxPyFatalErrorDialogWithTraceback,
                            wxPyFatalErrorDialog],#WithTraceback
                           (args,) * 4,
                           (kwargs,) * 4)
        ErrorDialogsDemoPanelFunc(self)

        EVT_BUTTON(self,
                   ID_BUTTON_wxPyFatalErrorDialog,
                   self.DoDialog)
        EVT_BUTTON(self,
                   ID_BUTTON_wxPyFatalErrorDialogWithTraceback,
                   self.DoDialog)
        EVT_BUTTON(self,
                   ID_BUTTON_wxPyNonFatalErrorDialog,
                   self.DoDialog)
        EVT_BUTTON(self,
                   ID_BUTTON_wxPyNonFatalErrorDialogWithTraceback,
                   self.DoDialog)
        EVT_CLOSE(self,self.OnClose)

    IndexFromID = {
        ID_BUTTON_wxPyFatalErrorDialog: 3,
        ID_BUTTON_wxPyFatalErrorDialogWithTraceback: 2,
        ID_BUTTON_wxPyNonFatalErrorDialog: 1,
        ID_BUTTON_wxPyNonFatalErrorDialogWithTraceback: 0
        }

    def DoDialog(self,event):
        sys.stderr = self.dialogs[self.IndexFromID[event.GetId()]]
        print "%s.DoDialog(): testing %s..." % (self,sys.stderr)
        this_will_generate_a_NameError_exception

    def OnClose(self,evt):
        for d in self.dialogs:
            d.Destroy ()
        self.Destroy ()

class MyFrame(wxFrame):
    def __init__(self,parent=None):
        wxFrame.__init__(self,parent,-1,
                         "Please make a selection...",
                         )
        self. panel = MyPanel(self)
        EVT_CLOSE (self,self.OnCloseWindow)

    def OnCloseWindow(self,event):
        self.panel.Close()
        self.Destroy()

class MyApp(wxApp):
    def OnInit(self):
        frame = MyFrame()
        frame.Show(true)
        self.SetTopWindow(frame)
        return true

def runTest(pframe, nb, log):
    panel = MyPanel(nb)
    return panel

from wxPython.lib import ErrorDialogs
ErrorDialogs._debug = 1

if __name__ == "__main__":
    sys.stderr = wxPyNonWindowingErrorHandler()
    app = MyApp(0)
    app.MainLoop()
    sys.exit()
else:
    overview = ErrorDialogs.__doc__
