# 11/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 

import  wx

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dlg = wx.TextEntryDialog(
            frame, 'What is your favorite programming language?',
            'Duh??', 'Python')

    dlg.SetValue("Python is the best!")
    
    if dlg.ShowModal() == wx.ID_OK:
        log.WriteText('You entered: %s\n' % dlg.GetValue())

    dlg.Destroy()


#---------------------------------------------------------------------------



overview = """\
This class represents a dialog that requests a one-line text string from the user. 
It is implemented as a generic wxWindows dialog. Along with the usual wxDialog 
style flags, all of the wxTextCtrl TE_* style flags are accepted, so, for example,
wx.TE_PASSWORD could be used to create a password dialog.

As with other dialogs of this type, the user input must be retrieved prior to
destroying the dialog.

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
