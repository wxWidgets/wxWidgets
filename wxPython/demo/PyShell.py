
from wxPython.wx import *
from wxPython.lib.shell import PyShell

#----------------------------------------------------------------------


def runTest(frame, nb, log):
    testGlobals = {'hello' : 'How are you?'}
    win = PyShell(nb, globals=testGlobals)
    win.Show(true)
    return win


#----------------------------------------------------------------------


overview = """
A simple GUI version of the interactive interpreter.
"""

