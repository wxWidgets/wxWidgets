
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    max = 20
    dlg = wxProgressDialog("Progress dialog example",
                           "An informative message",
                           max,
                           frame,
                           wxPD_CAN_ABORT | wxPD_APP_MODAL)

    keepGoing = True
    count = 0
    while keepGoing and count < max:
        count = count + 1
        wxSleep(1)

        if count == max / 2:
            keepGoing = dlg.Update(count, "Half-time!")
        else:
            keepGoing = dlg.Update(count)

    dlg.Destroy()


#---------------------------------------------------------------------------




overview = """\
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
