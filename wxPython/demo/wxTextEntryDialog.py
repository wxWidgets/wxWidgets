
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dlg = wxTextEntryDialog(frame, 'What is your favorite programming language?',
                            'Duh??', 'Python')
    dlg.SetValue("Python is the best!")  #### this doesn't work?
    if dlg.ShowModal() == wxID_OK:
        log.WriteText('You entered: %s\n' % dlg.GetValue())
    dlg.Destroy()


#---------------------------------------------------------------------------












overview = """\

"""
