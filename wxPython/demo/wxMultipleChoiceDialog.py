
from wxPython.wx          import *
from wxPython.lib.dialogs import wxMultipleChoiceDialog

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    lst = [ 'apple', 'pear', 'banana', 'coconut', 'orange',
            'etc', 'etc..', 'etc...' ]
    dlg = wxMultipleChoiceDialog(frame,
                                 "Pick some from\n this list\nblah blah...",
                                 "m.s.d.", lst)
    if (dlg.ShowModal() == wxID_OK):
        print "Selection:", dlg.GetValue(), " -> ", dlg.GetValueString()

#---------------------------------------------------------------------------









overview = """\
"""
