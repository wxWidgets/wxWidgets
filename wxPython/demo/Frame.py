
import  wx

#---------------------------------------------------------------------------

class MyFrame(wx.Frame):
    def __init__(
            self, parent, ID, title, pos=wx.DefaultPosition,
            size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE
            ):

        wx.Frame.__init__(self, parent, ID, title, pos, size, style)
        panel = wx.Panel(self, -1)

        button = wx.Button(panel, 1003, "Close Me")
        button.SetPosition((15, 15))
        self.Bind(wx.EVT_BUTTON, self.OnCloseMe, button)
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)


    def OnCloseMe(self, event):
        self.Close(True)

    def OnCloseWindow(self, event):
        self.Destroy()

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Create and Show a Frame", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        win = MyFrame(self, -1, "This is a wx.Frame", size=(350, 200),
                  style = wx.DEFAULT_FRAME_STYLE)
        win.Show(True)

        

#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


#---------------------------------------------------------------------------


overview = """\
A Frame is a window whose size and position can (usually) be changed by 
the user. It usually has thick borders and a title bar, and can optionally 
contain a menu bar, toolbar and status bar. A frame can contain any window 
that is not a Frame or Dialog. It is one of the most fundamental of the 
wxWindows components. 

A Frame that has a status bar and toolbar created via the 
<code>CreateStatusBar</code> / <code>CreateToolBar</code> functions manages 
these windows, and adjusts the value returned by <code>GetClientSize</code>
to reflect the remaining size available to application windows.

By itself, a Frame is not too useful, but with the addition of Panels and
other child objects, it encompasses the framework around which most user
interfaces are constructed.

If you plan on using Sizers and auto-layout features, be aware that the Frame
class lacks the ability to handle these features unless it contains a Panel.
The Panel has all the necessary functionality to both control the size of
child components, and also communicate that information in a useful way to
the Frame itself.
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

