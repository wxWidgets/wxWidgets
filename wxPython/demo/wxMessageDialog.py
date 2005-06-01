
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dlg = wxMessageDialog(frame, 'Hello from Python and wxPython!',
                          'A Message Box', wxOK | wxICON_INFORMATION)
                          #wxYES_NO | wxNO_DEFAULT | wxCANCEL | wxICON_INFORMATION)
    dlg.ShowModal()
    dlg.Destroy()

#---------------------------------------------------------------------------













overview = """\
Show a message to the user in a dialog
"""





if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
