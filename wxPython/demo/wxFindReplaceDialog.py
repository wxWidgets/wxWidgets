
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        b = wxButton(self, -1, "Show Find Dialog", (25, 50))
        EVT_BUTTON(self, b.GetId(), self.OnShowFind)

        b = wxButton(self, -1, "Show Find && Replace Dialog", (25, 90))
        EVT_BUTTON(self, b.GetId(), self.OnShowFindReplace)

        EVT_COMMAND_FIND(self, -1, self.OnFind)
        EVT_COMMAND_FIND_NEXT(self, -1, self.OnFind)
        EVT_COMMAND_FIND_REPLACE(self, -1, self.OnFind)
        EVT_COMMAND_FIND_REPLACE_ALL(self, -1, self.OnFind)
        EVT_COMMAND_FIND_CLOSE(self, -1, self.OnFindClose)


    def OnShowFind(self, evt):
        data = wxFindReplaceData()
        dlg = wxFindReplaceDialog(self, data, "Find")
        dlg.data = data  # save a reference to it...
        dlg.Show(True)


    def OnShowFindReplace(self, evt):
        data = wxFindReplaceData()
        dlg = wxFindReplaceDialog(self, data, "Find & Replace", wxFR_REPLACEDIALOG)
        dlg.data = data  # save a reference to it...
        dlg.Show(True)


    def OnFind(self, evt):
        map = {
            wxEVT_COMMAND_FIND : "FIND",
            wxEVT_COMMAND_FIND_NEXT : "FIND_NEXT",
            wxEVT_COMMAND_FIND_REPLACE : "REPLACE",
            wxEVT_COMMAND_FIND_REPLACE_ALL : "REPLACE_ALL",
            }
        et = evt.GetEventType()

        try:
            evtType = map[et]
        except KeyError:
            evtType = "**Unknown Event Type**"

        if et == wxEVT_COMMAND_FIND_REPLACE or et == wxEVT_COMMAND_FIND_REPLACE_ALL:
            replaceTxt = "Replace text: " + evt.GetReplaceString()
        else:
            replaceTxt = ""

        self.log.write("%s -- Find text: %s  %s  Flags: %d  \n" %
                       (evtType, evt.GetFindString(), replaceTxt, evt.GetFlags()))


    def OnFindClose(self, evt):
        self.log.write("wxFindReplaceDialog closing...\n")
        evt.GetDialog().Destroy()

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#---------------------------------------------------------------------------




overview = """\
A generic find and replace dialog.
"""




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

