
from wxPython.wx import *

#----------------------------------------------------------------------

## class MyFontEnumerator(wxFontEnumerator):
##     def __init__(self, list):
##         wxFontEnumerator.__init__(self)
##         self.list = list

##     def OnFacename(self, face):
##         self.list.append(face)
##         return true



class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)

##         list = []
##         e = MyFontEnumerator(list)
##         e.EnumerateFacenames()

        e = wxFontEnumerator()
        e.EnumerateFacenames()
        list = e.GetFacenames()

        list.sort()

        wxStaticText(self, -1, "Face names:", (15, 50), (65, 18))
        self.lb1 = wxListBox(self, -1, (80, 50), (200, 250),
                             list, wxLB_SINGLE)
        EVT_LISTBOX(self, self.lb1.GetId(), self.OnSelect)

        self.txt = wxStaticText(self, -1, "Sample text...", (285, 50))

        self.lb1.SetSelection(0)


    def OnSelect(self, evt):
        face = self.lb1.GetStringSelection()
        font = wxFont(28, wxDEFAULT, wxNORMAL, wxNORMAL, false, face)
        self.txt.SetFont(font)
        self.txt.SetSize(self.txt.GetBestSize())

##         st = font.GetNativeFontInfo().ToString()
##         ni2 = wxNativeFontInfo()
##         ni2.FromString(st)
##         font2 = wxFontFromNativeInfo(ni2)

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------








overview = """\
wxFontEnumerator enumerates either all available fonts on the system or only the ones with given attributes - either only fixed-width (suited for use in programs such as terminal emulators and the like) or the fonts available in the given encoding.

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

