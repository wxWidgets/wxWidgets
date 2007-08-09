
import  wx


USE_GENERIC = 0

if USE_GENERIC:
    from wx.lib.stattext import GenStaticText as StaticText
else:
    StaticText = wx.StaticText


#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)
        ##self.SetBackgroundColour("sky blue")

        StaticText(self, -1, "This is an example of static text", (20, 10))
        StaticText(self, -1, "using the wx.StaticText Control.", (20, 30))

        StaticText(
            self, -1, "Is this yellow?", (20, 70), (120, -1)
            ).SetBackgroundColour('Yellow')

        StaticText(
            self, -1, "align center", (160, 70), (120, -1), wx.ALIGN_CENTER
            ).SetBackgroundColour('Yellow')

        StaticText(
            self, -1, "align right", (300, 70), (120, -1), wx.ALIGN_RIGHT
            ).SetBackgroundColour('Yellow')

        str = "This is a different font."
        text = StaticText(self, -1, str, (20, 120))
        font = wx.Font(18, wx.SWISS, wx.NORMAL, wx.NORMAL)
        text.SetFont(font)

        StaticText(self, -1,
                   "Multi-line wx.StaticText\nline 2\nline 3\n\nafter empty line",
                   (20,170))
        StaticText(self, -1,
                   "Align right multi-line\nline 2\nline 3\n\nafter empty line",
                   (220,170), style=wx.ALIGN_RIGHT)


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

