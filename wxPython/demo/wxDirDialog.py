# 11/17/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 

import  wx

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    
    # In this case we include a "New directory" button. 
    dlg = wx.DirDialog(frame, "Choose a directory:",
                      style=wx.DD_DEFAULT_STYLE|wx.DD_NEW_DIR_BUTTON)

    # If the user selects OK, then we process the dialog's data.
    # This is done by getting the path data from the dialog - BEFORE
    # we destroy it. 
    if dlg.ShowModal() == wx.ID_OK:
        log.WriteText('You selected: %s\n' % dlg.GetPath())

    # Only destroy a dialog after you're done with it.
    dlg.Destroy()

#---------------------------------------------------------------------------



overview = """\
This class represents the directory chooser dialog.  It is used when all you
need from the user is the name of a directory. Data is retrieved via utility
methods; see the <code>DirDialog</code> documentation for specifics.
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

