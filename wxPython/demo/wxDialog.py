
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = wxDialog(frame, -1, "This is a wxDialog", size=wxSize(350, 200), style=wxCAPTION)

    sizer = wxBoxSizer(wxVERTICAL)

    label = wxStaticText(win, -1, "This is a wxDialog")
    sizer.Add(label, 0, wxALIGN_CENTRE|wxALL, 5)

    box = wxBoxSizer(wxHORIZONTAL)

    label = wxStaticText(win, -1, "Field #1:")
    box.Add(label, 0, wxALIGN_CENTRE|wxALL, 5)

    text = wxTextCtrl(win, -1, "", size=(80,-1))
    box.Add(text, 1, wxALIGN_CENTRE|wxALL, 5)

    sizer.AddSizer(box, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5)

    box = wxBoxSizer(wxHORIZONTAL)

    label = wxStaticText(win, -1, "Field #2:")
    box.Add(label, 0, wxALIGN_CENTRE|wxALL, 5)

    text = wxTextCtrl(win, -1, "", size=(80,-1))
    box.Add(text, 1, wxALIGN_CENTRE|wxALL, 5)

    sizer.AddSizer(box, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5)

    line = wxStaticLine(win, -1, size=(20,-1), style=wxLI_HORIZONTAL)
    sizer.Add(line, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP, 5)

    box = wxBoxSizer(wxHORIZONTAL)

    btn = wxButton(win, wxID_OK, " OK ")
    btn.SetDefault()
    box.Add(btn, 0, wxALIGN_CENTRE|wxALL, 5)

    btn = wxButton(win, wxID_CANCEL, " Cancel ")
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
