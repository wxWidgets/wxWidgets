from wxPython.wx import *
import os

#----------------------------------------------------------------------

class FileBrowseButton(wxPanel):
    ''' A control to allow the user to type in a filename
    or browse with the standard file dialog to select file

    __init__ (
        parent, id, pos, size -- passed directly to wxPanel initialisation
        style = wxTAB_TRAVERSAL -- passed directly to wxPanel initialisation
        labelText -- Text for label to left of text field
        buttonText -- Text for button which launches the file dialog
        toolTip -- Help text
        dialogTitle -- Title used in file dialog
        startDirectory -- Default directory for file dialog startup
        fileMask -- File mask (glob pattern, such as *.*) to use in file dialog
        fileMode -- wxOPEN or wxSAVE, indicates type of file dialog to use
        changeCallback -- callback receives all changes in value of control
    )
    GetValue() -- retrieve current value of text control
    SetValue(string) -- set current value of text control
    label -- pointer to internal label widget
    textControl -- pointer to internal text control
    browseButton -- pointer to button
    '''
    def __init__ (self, parent, id= -1,
                  pos = wxDefaultPosition, size = wxDefaultSize,
                  style = wxTAB_TRAVERSAL,
                  labelText= "File Entry:",
                  buttonText= "Browse",
                  toolTip= "Type filename or browse computer to choose file",
                  # following are the values for a file dialog box
                  dialogTitle = "Choose a file",
                  startDirectory = ".",
                  initialValue = "",
                  fileMask = "*.*",
                  fileMode = wxOPEN,
                  # callback for when value changes (optional)
                  changeCallback= None
        ):
        wxPanel.__init__ (self, parent, id, pos, size, style)
        # store variables
        self.dialogTitle = dialogTitle
        self.startDirectory = startDirectory
        self.fileMask = fileMask
        self.fileMode = fileMode
        self.changeCallback = changeCallback

        box = wxBoxSizer(wxHORIZONTAL)
        self.label = wxStaticText(self, -1, labelText, style =wxALIGN_RIGHT )
        font = self.label.GetFont()
        w, h, d, e = self.GetFullTextExtent(labelText, font)
        self.label.SetSize(wxSize(w+5, h))
        box.Add( self.label, 0, wxCENTER )

        ID = wxNewId()
        self.textControl = wxTextCtrl(self, ID)
        self.textControl.SetToolTipString( toolTip )
        box.Add( self.textControl, 1, wxLEFT|wxCENTER, 5)
        if changeCallback:
            EVT_TEXT(self.textControl, ID, changeCallback)

        ID = wxNewId()
        self.browseButton = button =wxButton(self, ID, buttonText)
        box.Add( button, 0, wxCENTER)
        button.SetToolTipString( toolTip )
        EVT_BUTTON(button, ID, self.OnBrowse)

        # add a border around the whole thing and resize the panel to fit
        outsidebox = wxBoxSizer(wxVERTICAL)
        outsidebox.Add(box, 1, wxEXPAND|wxALL, 3)
        outsidebox.Fit(self)

        self.SetAutoLayout(true)
        self.SetSizer( outsidebox )
        self.Layout()
        if size.width != -1 or size.height != -1:
            self.SetSize(size)



    def OnBrowse (self, event = None):
        ''' Going to browse for file... '''
        current = self.GetValue ()
        directory = os.path.split(current)
        if os.path.isdir( current):
            directory =current
        elif directory and os.path.isdir( directory[0] ):
            directory = directory [0]
        else:
            directory = self.startDirectory
        dlg = wxFileDialog(self, self.dialogTitle, directory, current, self.fileMask, self.fileMode)
        if dlg.ShowModal() == wxID_OK:
            self.SetValue (dlg.GetPath())
        dlg.Destroy()
        self.textControl.SetFocus()


    def GetValue (self):
        ''' Convenient access to text control value '''
        return self.textControl.GetValue ()


    def SetValue (self, value):
        ''' Convenient setting of text control value '''
        return self.textControl.SetValue (value)


#----------------------------------------------------------------------


if __name__ == "__main__":
    #from skeletonbuilder import rulesfile
    class DemoFrame( wxFrame ):
        def __init__(self, parent):
            wxFrame.__init__(self, parent, 2400, "File entry with browse", size=(500,100) )
            panel = wxPanel (self,-1)
            innerbox = wxBoxSizer(wxVERTICAL)
            control = FileBrowseButton(panel)
            innerbox.Add(  control, 0, wxEXPAND )
            control = FileBrowseButton(panel, labelText = "With Callback", style = wxSUNKEN_BORDER  ,changeCallback= self.OnFileNameChanged)
            innerbox.Add(  control, 0, wxEXPAND|wxALIGN_BOTTOM )
            panel.SetAutoLayout(true)
            panel.SetSizer( innerbox )

        def OnFileNameChanged (self, event):
            print "Filename changed", event.GetString ()

        def OnCloseMe(self, event):
            self.Close(true)

        def OnCloseWindow(self, event):
            self.Destroy()

    class DemoApp(wxApp):
        def OnInit(self):
            wxImage_AddHandler(wxJPEGHandler())
            wxImage_AddHandler(wxPNGHandler())
            wxImage_AddHandler(wxGIFHandler())
            frame = DemoFrame(NULL)
            #frame = RulesPanel(NULL )
            frame.Show(true)
            self.SetTopWindow(frame)
            return true

    def test( ):
        app = DemoApp(0)
        app.MainLoop()
    print 'Creating dialogue'
    test( )



