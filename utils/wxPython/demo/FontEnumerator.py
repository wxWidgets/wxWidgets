
from wxPython.wx import *

#----------------------------------------------------------------------

class MyFontEnumerator(wxFontEnumerator):
    def __init__(self, list):
        wxFontEnumerator.__init__(self)
        self.list = list

    def OnFacename(self, face):
        self.list.append(face)
        return true



class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)

        list = []
        e = MyFontEnumerator(list)
        e.EnumerateFacenames()
        list.sort()

        wxStaticText(self, -1, "Face names:", wxPoint(15, 50), wxSize(65, 18))
        self.lb1 = wxListBox(self, 60, wxPoint(80, 50), wxSize(200, 250),
                             list, wxLB_SINGLE)
        self.lb1.SetSelection(0)




#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------












overview = """\
wxFontEnumerator enumerates either all available fonts on the system or only the ones with given attributes - either only fixed-width (suited for use in programs such as terminal emulators and the like) or the fonts available in the given encoding.

"""
