
from wxPython.wx import *

USE_GENERIC = 0

if USE_GENERIC:
    from wxPython.lib.stattext import wxGenStaticText as wxStaticText

#---------------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent):
        wxPanel.__init__(self, parent, -1)

        wxStaticText(self, -1, "This is an example of static text", (20, 10))

        wxStaticText(self, -1, "using the wxStaticText Control.", (20, 30))

        wxStaticText(self, -1, "Is this yellow?", (20, 70), (90, -1)).SetBackgroundColour('Yellow')

        wxStaticText(self, -1, "align center", (120, 70), (90, -1), wxALIGN_CENTER).SetBackgroundColour('Yellow')

        wxStaticText(self, -1, "align right", (220, 70), (90, -1), wxALIGN_RIGHT).SetBackgroundColour('Yellow')

        str = "This is a different font."
        text = wxStaticText(self, -1, str, (20, 100))
        font = wxFont(18, wxSWISS, wxNORMAL, wxNORMAL)
        text.SetFont(font)
        #text.SetSize(text.GetBestSize())

        wxStaticText(self, -1, "Multi-line wxStaticText\nline 2\nline 3\n\nafter empty line", (20,150))
        wxStaticText(self, -1, "Align right multi-line\nline 2\nline 3\n\nafter empty line", (220,150), style=wxALIGN_RIGHT)


#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    panel = TestPanel(nb)
    return panel


#---------------------------------------------------------------------------



overview = '''\
A static text control displays one or more lines of read-only text.

'''



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

