
import  wx
import  wx.lib.dialogs

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    f = open("Main.py", "r")
    msg = f.read()
    f.close()
    
    dlg = wx.lib.dialogs.ScrolledMessageDialog(frame, msg, "message test")
    dlg.ShowModal()

#---------------------------------------------------------------------------



overview = """\

<code><b>ScrolledMessageDialog</b>(parent, msg, caption, pos=wx.DefaultPosition, size=(500,300))</code>

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
