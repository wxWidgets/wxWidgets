
from wxPython.wx import *
from wxPython.help import *

#---------------------------------------------------------------------------
# Create and set a help provider.  Normally you would do this in
# the app's OnInit as it must be done before any SetHelpText calls.
provider = wxSimpleHelpProvider()
wxHelpProvider_Set(provider)



#---------------------------------------------------------------------------

class TestDialog(wxDialog):
    def __init__(self, parent, ID, title,
                 pos=wxDefaultPosition, size=wxDefaultSize,
                 style=wxDEFAULT_DIALOG_STYLE):

        # Instead of calling wxDialog.__init__ we precreate the dialog
        # so we can set an extra style that must be set before
        # creation, and then we create the GUI dialog using the Create
        # method.
        pre = wxPreDialog()
        pre.SetExtraStyle(wxDIALOG_EX_CONTEXTHELP)
        pre.Create(parent, ID, title, pos, size, style)

        # This next step is the most important, it turns this Python
        # object into the real wrapper of the dialog (instead of pre)
        # as far as the wxPython extension is concerned.
        self.this = pre.this


        # Now continue with the normal construction of the dialog
        # contents
        sizer = wxBoxSizer(wxVERTICAL)

        label = wxStaticText(self, -1, "This is a wxDialog")
        label.SetHelpText("This is the help text for the label")
        sizer.Add(label, 0, wxALIGN_CENTRE|wxALL, 5)

        box = wxBoxSizer(wxHORIZONTAL)

        label = wxStaticText(self, -1, "Field #1:")
        label.SetHelpText("This is the help text for the label")
        box.Add(label, 0, wxALIGN_CENTRE|wxALL, 5)

        text = wxTextCtrl(self, -1, "", size=(80,-1))
        text.SetHelpText("Here's some help text for field #1")
        box.Add(text, 1, wxALIGN_CENTRE|wxALL, 5)

        sizer.AddSizer(box, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5)

        box = wxBoxSizer(wxHORIZONTAL)

        label = wxStaticText(self, -1, "Field #2:")
        label.SetHelpText("This is the help text for the label")
        box.Add(label, 0, wxALIGN_CENTRE|wxALL, 5)

        text = wxTextCtrl(self, -1, "", size=(80,-1))
        text.SetHelpText("Here's some help text for field #2")
        box.Add(text, 1, wxALIGN_CENTRE|wxALL, 5)

        sizer.AddSizer(box, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5)

        line = wxStaticLine(self, -1, size=(20,-1), style=wxLI_HORIZONTAL)
        sizer.Add(line, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP, 5)

        box = wxBoxSizer(wxHORIZONTAL)

        if wxPlatform != "__WXMSW__":
            btn = wxContextHelpButton(self)
            box.Add(btn, 0, wxALIGN_CENTRE|wxALL, 5)

        btn = wxButton(self, wxID_OK, " OK ")
        btn.SetDefault()
        btn.SetHelpText("The OK button completes the dialog")
        box.Add(btn, 0, wxALIGN_CENTRE|wxALL, 5)

        btn = wxButton(self, wxID_CANCEL, " Cancel ")
        btn.SetHelpText("The Cancel button cnacels the dialog. (Duh!)")
        box.Add(btn, 0, wxALIGN_CENTRE|wxALL, 5)

        sizer.AddSizer(box, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5)

        self.SetSizer(sizer)
        self.SetAutoLayout(True)
        sizer.Fit(self)



#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestDialog(frame, -1, "This is a wxDialog", size=wxSize(350, 200),
                     #style = wxCAPTION | wxSYSTEM_MENU | wxTHICK_FRAME
                     style = wxDEFAULT_DIALOG_STYLE
                     )
    win.CenterOnScreen()
    val = win.ShowModal()
    if val == wxID_OK:
        log.WriteText("You pressed OK\n")
    else:
        log.WriteText("You pressed Cancel\n")



#---------------------------------------------------------------------------





overview = """\
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

