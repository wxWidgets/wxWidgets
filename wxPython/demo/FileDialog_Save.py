# 11/17/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 

import  os
import  wx

#---------------------------------------------------------------------------

# This is how you pre-establish a file filter so that the dialog
# only shows the extention(s) you want it to.
wildcard = "Python source (*.py)|*.py|"     \
           "Compiled Python (*.pyc)|*.pyc|" \
           "SPAM files (*.spam)|*.spam|"    \
           "Egg file (*.egg)|*.egg|"        \
           "All files (*.*)|*.*"

def runTest(frame, nb, log):
    log.WriteText("CWD: %s\n" % os.getcwd())

    # Create the dialog. In this case the current directory is forced as the starting
    # directory for the dialog, and no default file name is forced. This can easilly
    # be changed in your program. This is an 'save' dialog.
    #
    # Unlike the 'open dialog' example found elsewhere, this example does NOT
    # force the current working directory to change if the user chooses a different
    # directory than the one initially set.
    dlg = wx.FileDialog(
        frame, message="Save file as ...", defaultDir=os.getcwd(), 
        defaultFile="", wildcard=wildcard, style=wx.SAVE
        )

    # This sets the default filter that the user will initially see. Otherwise,
    # the first filter in the list will be used by default.
    dlg.SetFilterIndex(2)

    # Show the dialog and retrieve the user response. If it is the OK response, 
    # process the data.
    if dlg.ShowModal() == wx.ID_OK:
        path = dlg.GetPath()
        log.WriteText('You selected "%s"' % path)

        # Normally, at this point you would save your data using the file and path
        # data that the user provided to you, but since we didn't actually start
        # with any data to work with, that would be difficult.
        # 
        # The code to do so would be similar to this, assuming 'data' contains
        # the data you want to save:
        #
        # fp = file(path, 'w') # Create file anew
        # fp.write(data)
        # fp.close()
        #
        # You might want to add some error checking :-)
        #

    # Note that the current working dir didn't change. This is good since
    # that's the way we set it up.
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

One important thing to note: if you use the file extention filters, then files saved
with the filter set to something will automatically get that extention appended to them
if it is not already there. For example, suppose the dialog was displaying the 'egg'
extention and you entered a file name of 'fried'. It would be saved as 'fried.egg.'
Yum!
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

