
import  wx

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)

        wx.StaticText(self, -1, "This is an example of static text", (20, 10))
        wx.StaticText(self, -1, "using the wx.StaticText Control.", (20, 30))

        wx.StaticText(
            self, -1, "Is this yellow?", (20, 70), (90, -1)
            ).SetBackgroundColour('Yellow')

        wx.StaticText(
            self, -1, "align center", (120, 70), (90, -1), wx.ALIGN_CENTER
            ).SetBackgroundColour('Yellow')

        wx.StaticText(
            self, -1, "align right", (220, 70), (90, -1), wx.ALIGN_RIGHT
            ).SetBackgroundColour('Yellow')

        str = "This is a different font."
        text = wx.StaticText(self, -1, str, (20, 100))
        font = wx.Font(18, wx.SWISS, wx.NORMAL, wx.NORMAL)
        text.SetFont(font)
        text.SetSize(text.GetBestSize())

        wx.StaticText(self, -1, "Multi-line wx.StaticText\nline 2\nline 3\n\nafter empty line", (20,150))
        wx.StaticText(self, -1, "Align right multi-line\nline 2\nline 3\n\nafter empty line", (220,150), style=wx.ALIGN_RIGHT)


#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    panel = TestPanel(nb)
    return panel


#---------------------------------------------------------------------------


overview = '''\
A StaticText control displays one or more lines of read-only text.

'''


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

