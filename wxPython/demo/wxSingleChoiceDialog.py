# 11/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 

import  wx

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dlg = wx.SingleChoiceDialog(
            frame, 'Test Single Choice', 'The Caption',
            ['zero', 'one', 'two', 'three', 'four', 'five', 'six', 'seven', 'eight'], 
            wx.CHOICEDLG_STYLE
            )

    if dlg.ShowModal() == wx.ID_OK:
        log.WriteText('You selected: %s\n' % dlg.GetStringSelection())

    dlg.Destroy()

#---------------------------------------------------------------------------




overview = """\
This class represents a dialog that shows a list of strings, and allows the user
to select one. Double-clicking on a list item is equivalent to single-clicking
and then pressing OK.

As with all dialogs, be sure to retrieve the information you need BEFORE you
destroy the dialog.
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

