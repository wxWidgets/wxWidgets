
from wxPython.wx import *
from wxPython.help import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
##     win = wxDialog(frame, -1, "This is a wxDialog", size=wxSize(350, 200),
##                    style = wxCAPTION | wxSYSTEM_MENU | wxTHICK_FRAME
##                    #style = wxDEFAULT_DIALOG_STYLE
##                    )

    # Create and set a help provider.  Normally you would do this in
    # the app's OnInit as it must be done before any SetHelpText calls.
    provider = wxSimpleHelpProvider()
    wxHelpProvider_Set(provider)

    win = wxPreDialog()
    win.SetExtraStyle(wxDIALOG_EX_CONTEXTHELP)
    win.Create(frame, -1, "This is a wxDialog", size=wxSize(350, 200),
                   #style = wxCAPTION | wxSYSTEM_MENU | wxTHICK_FRAME
                   style = wxDEFAULT_DIALOG_STYLE
                   )

    sizer = wxBoxSizer(wxVERTICAL)

    label = wxStaticText(win, -1, "This is a wxDialog")
    label.SetHelpText("This is the help text for the label")
    sizer.Add(label, 0, wxALIGN_CENTRE|wxALL, 5)

    box = wxBoxSizer(wxHORIZONTAL)

    label = wxStaticText(win, -1, "Field #1:")
    label.SetHelpText("This is the help text for the label")
    box.Add(label, 0, wxALIGN_CENTRE|wxALL, 5)

    text = wxTextCtrl(win, -1, "", size=(80,-1))
    text.SetHelpText("Here's some help text for field #1")
    box.Add(text, 1, wxALIGN_CENTRE|wxALL, 5)

    sizer.AddSizer(box, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5)

    box = wxBoxSizer(wxHORIZONTAL)

    label = wxStaticText(win, -1, "Field #2:")
    label.SetHelpText("This is the help text for the label")
    box.Add(label, 0, wxALIGN_CENTRE|wxALL, 5)

    text = wxTextCtrl(win, -1, "", size=(80,-1))
    text.SetHelpText("Here's some help text for field #2")
    box.Add(text, 1, wxALIGN_CENTRE|wxALL, 5)

    sizer.AddSizer(box, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5)

    line = wxStaticLine(win, -1, size=(20,-1), style=wxLI_HORIZONTAL)
    sizer.Add(line, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP, 5)

    box = wxBoxSizer(wxHORIZONTAL)

    if wxPlatform != "__WXMSW__":
        btn = wxContextHelpButton(win)
        box.Add(btn, 0, wxALIGN_CENTRE|wxALL, 5)

    btn = wxButton(win, wxID_OK, " OK ")
    btn.SetDefault()
    btn.SetHelpText("The OK button completes the dialog")
    box.Add(btn, 0, wxALIGN_CENTRE|wxALL, 5)

    btn = wxButton(win, wxID_CANCEL, " Cancel ")
    btn.SetHelpText("The Cancel button cnacels the dialog. (Duh!)")
    box.Add(btn, 0, wxALIGN_CENTRE|wxALL, 5)

    sizer.AddSizer(box, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5)

    win.SetSizer(sizer)
    win.SetAutoLayout(true)
    sizer.Fit(win)

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

