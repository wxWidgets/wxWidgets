# demo for ErrorDialogs.py
# usual wxWindows license stuff here.
# by Chris Fama, with thanks to Robin Dunn, and others on the wxPython-users
# mailing list.

from wxPython.wx import *
from wxPython.lib.ErrorDialogs import *
_debug = 0
ID_TEXT = 10000
ID_BUTTON_wxPyNonFatalError = 10001
ID_BUTTON_wxPyFatalError = 10002
ID_BUTTON_wxPyFatalErrorDialog = 10003
ID_BUTTON_wxPyNonFatalErrorDialog = 10004
ID_BUTTON_wxPyFatalErrorDialogWithTraceback = 10005
ID_BUTTON_wxPyNonFatalErrorDialogWithTraceback = 10006

def ErrorDialogsDemoPanelFunc( parent, call_fit = True, set_sizer = True ):
    item0 = wxBoxSizer( wxVERTICAL )

    item1 = wxStaticText( parent, ID_TEXT, "Please select one of the buttons below for an example using explicit errors...", wxDefaultPosition, wxDefaultSize, 0 )
    item0.AddWindow( item1, 0, wxALIGN_CENTRE|wxALL, 5 )

    item2 = wxFlexGridSizer( 0, 2, 0, 0 )

    item3 = wxButton( parent, ID_BUTTON_wxPyNonFatalError, "wxPyNonFatalError", wxDefaultPosition, wxDefaultSize, 0 )
    item2.AddWindow( item3, 0, wxALIGN_CENTRE|wxALL, 5 )

    item4 = wxButton( parent, ID_BUTTON_wxPyFatalError, "wxPyFatalError", wxDefaultPosition, wxDefaultSize, 0 )
    item2.AddWindow( item4, 0, wxALIGN_CENTRE|wxALL, 5 )

    item0.AddSizer( item2, 0, wxALIGN_CENTRE|wxALL, 5 )

    item5 = wxStaticText( parent, ID_TEXT, "Please select one of the buttons below for interpreter errors...", wxDefaultPosition, wxDefaultSize, 0 )
    item0.AddWindow( item5, 0, wxALIGN_CENTRE|wxALL, 5 )

    item6 = wxFlexGridSizer( 0, 2, 0, 0 )

    item7 = wxButton( parent, ID_BUTTON_wxPyFatalErrorDialog, "wxPyFatalErrorDialog", wxDefaultPosition, wxDefaultSize, 0 )
    item6.AddWindow( item7, 0, wxALIGN_CENTRE|wxALL, 5 )

    item8 = wxButton( parent, ID_BUTTON_wxPyNonFatalErrorDialog, "wxPyNonFatalErrorDialog", wxDefaultPosition, wxDefaultSize, 0 )
    item6.AddWindow( item8, 0, wxALIGN_CENTRE|wxALL, 5 )

    item9 = wxButton( parent, ID_BUTTON_wxPyFatalErrorDialogWithTraceback, "wxPyFatalErrorDialogWithTraceback", wxDefaultPosition, wxDefaultSize, 0 )
    item6.AddWindow( item9, 0, wxALIGN_CENTRE|wxALL, 5 )

    item10 = wxButton( parent, ID_BUTTON_wxPyNonFatalErrorDialogWithTraceback, "wxPyNonFatalErrorDialogWithTraceback", wxDefaultPosition, wxDefaultSize, 0 )
    item10.SetDefault()
    item6.AddWindow( item10, 0, wxALIGN_CENTRE|wxALL, 5 )

    item0.AddSizer( item6, 0, wxALIGN_CENTRE|wxALL, 5 )

    item11 = wxFlexGridSizer( 0, 2, 0, 0 )

    item0.AddSizer( item11, 0, wxALIGN_CENTRE|wxALL, 5 )

    if set_sizer == True:
        parent.SetAutoLayout( True )
        parent.SetSizer( item0 )
        if call_fit == True:
            item0.Fit( parent )
            item0.SetSizeHints( parent )

    return item0

# Menu bar functions

# Bitmap functions


# End of generated file

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
                   ID_BUTTON_wxPyNonFatalError,
                   self.DoDialog)
        EVT_BUTTON(self,
                   ID_BUTTON_wxPyFatalError,
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


    IndexFromID = {
        ID_BUTTON_wxPyFatalErrorDialog: 3,
        ID_BUTTON_wxPyFatalErrorDialogWithTraceback: 2,
        ID_BUTTON_wxPyNonFatalErrorDialog: 1,
        ID_BUTTON_wxPyNonFatalErrorDialogWithTraceback: 0
        }

    def DoDialog(self,event):
        id = event.GetId()
        if id in [ID_BUTTON_wxPyFatalError,ID_BUTTON_wxPyNonFatalError]:
            if id == ID_BUTTON_wxPyFatalError:
                print "%s.DoDialog(): testing explicit wxPyFatalError..."\
                      % self
                wxPyFatalError(self,"Test Non-fatal error.<p>"
                               "Nearly arbitrary HTML (i.e., that which is"
                               " understood by <B><I>wxHtmlWindow</i></b>)."
                               "<p><table border=\"2\"><tr><td>This</td><td>is</td></tr>"
                               "<tr><td>a</td><td>table</td></tr></table></p>")
            else:
                print "%s.DoDialog(): testing explicit wxPyNonFatalError..."\
                      % self
                wxPyNonFatalError(self,"Test Non-fatal error.<p>"
                                  "Nearly arbitrary HTML (i.e., that which is"
                                  " understood by <B><I>wxHtmlWindow</i></b>)."
                                  "<p><table border=\"2\"><tr><td>This</td><td>is</td></tr>"
                                  "<tr><td>a</td><td>table</td></tr></table></p>")
        else:
            sys.stderr = self.dialogs[self.IndexFromID[id]]
            print "%s.DoDialog(): testing %s..." % (self,sys.stderr)
            this_will_generate_a_NameError_exception

    def ShutdownDemo(self):
        for d in self.dialogs:
            d.Destroy()



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
        frame.Show(True)
        self.SetTopWindow(frame)
        return True

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
