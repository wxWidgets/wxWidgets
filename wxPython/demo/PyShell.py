
from wxPython.wx import *
from wxPython.lib.shell import PyShell

#----------------------------------------------------------------------


def runTest(frame, nb, log):
    win = PyShell(nb)
    win.Show(true)
    return win


#----------------------------------------------------------------------


overview = """
A simple GUI version of the interactive interpreter.
"""

