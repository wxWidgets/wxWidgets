
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dlg = wxSingleChoiceDialog(frame, 'Test Single Choice', 'The Caption',
                               ['zero', 'one', 'two', 'three', 'four', 'five',
                                'six', 'seven', 'eight'], wxOK|wxCANCEL)
    if dlg.ShowModal() == wxID_OK:
        log.WriteText('You selected: %s\n' % dlg.GetStringSelection())
    dlg.Destroy()

#---------------------------------------------------------------------------




overview = """\
This class represents a dialog that shows a list of strings, and allows the user
to select one. Double-clicking on a list item is equivalent to single-clicking
and then pressing OK.

"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

