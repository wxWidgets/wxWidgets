# 11/20/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 

import  wx

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dlg = wx.MessageDialog(frame, 'Hello from Python and wxPython!',
                          'A Message Box', wx.OK | wx.ICON_INFORMATION)
                          #wxYES_NO | wxNO_DEFAULT | wxCANCEL | wxICON_INFORMATION)
    dlg.ShowModal()
    dlg.Destroy()

#---------------------------------------------------------------------------



overview = """\
This class represents a dialog that shows a single or multi-line message, with a 
choice of OK, Yes, No and Cancel buttons.

Additionally, various style flags can determine whether an icon is displayed,
and, if so, what kind.

The dialog can be modal or not; of modal, the user's response is in the return
code of ShowModal(). If not, the response can be taken from GetReturnCode() (inherited
from the wxDialog super class). If not modal and the return code is required, it
must be retrieved before the dialog is destroyed.

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
