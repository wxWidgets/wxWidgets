# 11/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 
# 11/30/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o dialogs library needs updated to wx
# 
# 12/01/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o dialogs library converted. All is well.
# 

import  wx
import  wx.lib.dialogs  as  dlgs

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    f = open("Main.py", "r")
    msg = f.read()
    f.close()
    
    dlg = dlgs.wxScrolledMessageDialog(frame, msg, "message test")
    dlg.ShowModal()

#---------------------------------------------------------------------------



overview = """\

<code><b>wxScrolledMessageDialog</b>(parent, msg, caption, pos=wx.DefaultPosition, size=(500,300))</code>

This class represents a message dialog that uses a wxTextCtrl to display the
message. This allows more flexible information display without having to be
as much concerned with layout requirements. A text file can simply be used 

This dialog offers no special attributes or methods beyond those supported
by wxDialog.

"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
