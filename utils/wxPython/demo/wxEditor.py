
from wxPython.wx         import *
from wxPython.lib.editor import wxEditor, wxPyEditor

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = wxPanel(nb, -1)
    ed = wxEditor(win)
    pyed = wxPyEditor(win)
    box = wxBoxSizer(wxVERTICAL)
    box.Add(ed, 1, wxALL|wxGROW, 5)
    box.Add(pyed, 1, wxALL|wxGROW, 5)
    win.SetSizer(box)
    win.SetAutoLayout(true)

    ed.SetText(["",
                "This is a simple text editor, the class name is",
                "wxEditor.  Type a few lines and try it out."])

    pyed.SetText(["# This one is a derived class named wxPyEditor.",
                  "# It adds syntax highlighting, folding (press",
                  "# F12 on the \"def\" line below) and other stuff.",
                  "import sys",
                  "def hello():",
                  "    print 'hello'",
                  "    for x in sys.path:",
                  "        print x",
                  ""])
    return win

#----------------------------------------------------------------------





overview = """\
"""



