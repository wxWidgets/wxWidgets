
from wxPython.wx         import *
from wxPython.lib.editor import wxEditor

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = wxPanel(nb, -1)
    ed = wxEditor(win, -1, style=wxSUNKEN_BORDER)
    box = wxBoxSizer(wxVERTICAL)
    box.Add(ed, 1, wxALL|wxGROW, 1)
    win.SetSizer(box)
    win.SetAutoLayout(True)

    ed.SetText(["",
                "This is a simple text editor, the class name is",
                "wxEditor.  Type a few lines and try it out.",
                "",
                "It uses Windows-style key commands that can be overriden by subclassing.",
                "Mouse select works. Here are the key commands:",
                "",
                "Cursor movement:     Arrow keys or mouse",
                "Beginning of line:   Home",
                "End of line:         End",
                "Beginning of buffer: Control-Home",
                "End of the buffer:   Control-End",
                "Select text:         Hold down Shift while moving the cursor",
                "Copy:                Control-Insert, Control-C",
                "Cut:                 Shift-Delete,   Control-X",
                "Paste:               Shift-Insert,   Control-V",
                ""])

    return win

#----------------------------------------------------------------------


overview = """
The wxEditor class implements a simple text editor using wxPython.  You
can create a custom editor by subclassing wxEditor.  Even though much of
the editor is implemented in Python, it runs surprisingly smoothly on
normal hardware with small files.

How to use it
-------------
The demo code (demo/wxEditor.py) shows how to use wxEditor as a simple text
box. Use the SetText() and GetText() methods to set or get text from
the component; these both use a list of strings.

The samples/FrogEdit directory has an example of a simple text editor
application that uses the wxEditor component.

Subclassing
-----------
To add or change functionality, you can subclass this
component. One example of this might be to change the key
Alt key commands. In that case you would (for example) override the
SetAltFuncs() method.


"""




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

