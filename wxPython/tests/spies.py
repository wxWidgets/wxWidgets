## import all of the wxPython GUI package
from wxPython.wx import *

"""
I am trying to write a routine which will produce an entryform which I can
later use to write GUI entryforms for databasis work.  When you run this
program and chose option 150 (Invoer) under "L=EAers" the following error
appears (repeated 6 times):

Gtk-CRITICAL **: file gtkwidget.c: line 1592 (gtk_widget_map): assertion
`GTK_WIDGET_VISIBLE (widget) == TRUE' failed.=20
"""

class ToetsInvoer(wxPanel):
    def __init__(self, parent):
        wxPanel.__init__(self, parent, -1)

        wxStaticText(self, -1, "wxTextCtrl", wxPoint(5, 25), wxSize(75, 20))
        wxTextCtrl(self, 10, "", wxPoint(80, 25), wxSize(150, 20))
        EVT_TEXT(self, 10, self.EvtText)

        wxStaticText(self, -1, "Passsword", wxPoint(5, 50), wxSize(75, 20))
        wxTextCtrl(self, 20, "", wxPoint(80, 50), wxSize(150, 20), wxTE_PASSWORD)
        EVT_TEXT(self, 20, self.EvtText)

        wxStaticText(self, -1, "Multi-line", wxPoint(5, 75), wxSize(75, 20))
        wxTextCtrl(self, 30, "", wxPoint(80, 75), wxSize(200, 150), wxTE_MULTILINE)
        EVT_TEXT(self, 30, self.EvtText)

        self.Show(true)

    def EvtText(self, event):
        self.log.WriteText('EvtText: %s\n' % event.GetString())

#---------------------------------------------------------------------------

## Create a new frame class, derived from the wxPython Frame.
class HoofRaam(wxFrame):

    def __init__(self, pa, id, titel):
        # First, call the base class' __init__ method to create the frame
        wxFrame.__init__(self, pa, id, titel,wxPyDefaultPosition,
                         wxSize(420, 200))

        self.CreateStatusBar(2)
        mainmenu = wxMenuBar()
        menu = wxMenu()
        menu.Append(100, '&Kopieer', 'Maak rugsteun')
        menu.Append(101, '&Nuwe stel', 'Begin nuwe databasis')
        menu.Append(150, '&Invoer', 'Toets invoervorm')
        menu.AppendSeparator()
        menu.Append(200, 'Kl&aar', 'Gaan uit die program uit!')
        mainmenu.Append(menu, "&L=EAers")
        self.SetMenuBar(mainmenu)
#         if wxPlatform == '__WXMSW__':
#             print menu.GetHelpString(100)
#             print mainmenu.GetHelpString(101)
#             print mainmenu.GetHelpString(200)
#             self.DragAcceptFiles(true)



        # Associate some events with methods of this class
        self.Connect(-1, -1, wxEVT_SIZE, self.OnSize)
        self.Connect(-1, -1, wxEVT_MOVE, self.OnMove)
        self.Connect(-1, -1, wxEVT_COMMAND_MENU_SELECTED, self.OnMenuCommand)
        self.Connect(-1, -1, wxEVT_DROP_FILES, self.OnDropFiles)

        self.child = None
        #self.child = ToetsInvoer(self)

    # This method is called automatically when the CLOSE event is
    # sent to this window
    def OnCloseWindow(self, event):
        # tell the window to kill itself
        self.Destroy()


    # This method is called by the System when the window is resized,
    # because of the association above.
    def OnSize(self, event):
        size = event.GetSize()
        print "grootte:", size.width, size.height
        event.Skip()

    # This method is called by the System when the window is moved,
    # because of the association above.
    def OnMove(self, event):
        pos = event.GetPosition()
        print "pos:", pos.x, pos.y

    def OnMenuCommand(self, event):
        # why isn't this a wxMenuEvent???
        print event, event.GetInt()
        if event.GetInt() == 200:
            self.Close()
        if event.GetInt() == 150:
            x = ToetsInvoer(self)
        if event.GetInt() == 101:
            print "Nommer 101 is gekies"

    def OnDropFiles(self, event): #werk net in windows
        fileList = event.GetFiles()
        for file in fileList:
            print file

    def OnCloseWindow(self, event):
        print 'Klaar'
        self.Destroy()

#---------------------------------------------------------------------------

# Every wxWindows application must have a class derived from wxApp
class MyProg(wxApp):

    # wxWindows calls this method to initialize the application
    def OnInit(self):

        # Create an instance of our customized Frame class
        raam = HoofRaam(NULL, -1, "Taalunie")
        raam.Show(true)

        # Tell wxWindows that this is our main window
        self.SetTopWindow(raam)

        # Return a success flag
        return true

#---------------------------------------------------------------------------


app = MyProg(0)     # Create an instance of the application class
app.MainLoop()     # Tell it to start processing events



