
import wx
from wx.lib.wordwrap import wordwrap

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Show a wx.AboutBox", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        # First we create and fill the info object
        info = wx.AboutDialogInfo()
        info.Name = "Hello World"
        info.Version = "1.2.3"
        info.Copyright = "(C) 2006 Programmers and Coders Everywhere"
        info.Description = wordwrap(
            "A \"hello world\" program is a software program that prints out "
            "\"Hello world!\" on a display device. It is used in many introductory "
            "tutorials for teaching a programming language."
            
            "\n\nSuch a program is typically one of the simplest programs possible "
            "in a computer language. A \"hello world\" program can be a useful "
            "sanity test to make sure that a language's compiler, development "
            "environment, and run-time environment are correctly installed.",
            350, wx.ClientDC(self))
        info.WebSite = ("http://en.wikipedia.org/wiki/Hello_world", "Hello World home page")
        info.Developers = [ "Joe Programmer",
                            "Jane Coder",
                            "Vippy the Mascot" ]

        # Then we call wx.AboutBox giving it that info object
        wx.AboutBox(info)
        

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wx.AboutBox</center></h2>

This function shows the native standard about dialog containing the
information specified in info. If the current platform has a native
about dialog which is capable of showing all the fields in info, the
native dialog is used, otherwise the function falls back to the
generic wxWidgets version of the dialog.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

