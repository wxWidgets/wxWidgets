# 11/17/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 

import  os
import  wx

#---------------------------------------------------------------------------

# This is how you pre-establish a file filter so that the dialog
# only shows the extention(s) you want it to.
wildcard = "Python source (*.py)|*.py|" \
           "Compiled Python (*.pyc)|*.pyc|" \
           "All files (*.*)|*.*"

def runTest(frame, nb, log):
    log.WriteText("CWD: %s\n" % os.getcwd())

    # Create the dialog. In this case the current directory is forced as the starting
    # directory for the dialog, and no default file name is forced. This can easilly
    # be changed in your program. This is an 'open' dialog, and allows multitple
    # file selection to boot.
    #
    # Finally, of the directory is changed in the process of getting files, this
    # dialog is set up to change the current working directory to the path chosen.
    dlg = wx.FileDialog(
        frame, message="Choose a file", defaultDir=os.getcwd(), 
        defaultFile="", wildcard=wildcard, style=wx.OPEN | wx.MULTIPLE | wx.CHANGE_DIR
        )
        
    # Show the dialog and retrieve the user response. If it is the OK response, 
    # process the data.
    if dlg.ShowModal() == wx.ID_OK:
        # This returns a Python list of files that were selected.
        paths = dlg.GetPaths()

        log.WriteText('You selected %d files:' % len(paths))

        for path in paths:
            log.WriteText('           %s\n' % path)

    # Compare this with the debug above; did we change working dirs?
    log.WriteText("CWD: %s\n" % os.getcwd())

    # Destroy the dialog. Don't do this until you are done with it!
    # BAD things can happen otherwise!
    dlg.Destroy()

#---------------------------------------------------------------------------


overview = """\
This class provides the file selection dialog. It incorporates OS-native features
depending on the OS in use, and can be used both for open and save operations. 
The files displayed can be filtered by setting up a wildcard filter, multiple files
can be selected (open only), and files can be forced in a read-only mode.

There are two ways to get the results back from the dialog. GetFiles() returns only
the file names themselves, in a Python list. GetPaths() returns the full path and 
filenames combined as a Python list.

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

