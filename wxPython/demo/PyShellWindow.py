

from wxPython.lib.pyshell import PyShellWindow

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = PyShellWindow(nb, -1)
    return win

#----------------------------------------------------------------------

import wxPython.lib.pyshell

overview = wxPython.lib.pyshell.__doc__
