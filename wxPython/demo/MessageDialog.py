
import  wx

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dlg = wx.MessageDialog(frame, 'Hello from Python and wxPython!',
                          'A Message Box',
                           wx.OK | wx.ICON_INFORMATION)
                           #wx.YES_NO | wx.NO_DEFAULT | wx.CANCEL | wx.ICON_INFORMATION)
    dlg.ShowModal()
    dlg.Destroy()

#---------------------------------------------------------------------------



overview = """\
<html><body>
<h2>wx.MessageDialog</h2>

This class represents a dialog that shows a single or multi-line
message, with a choice of OK, Yes, No and Cancel buttons.
Additionally, various style flags can determine whether an icon is
displayed, and, if so, what kind.  The return value of ShowModal
indicates which button was pressed.

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
