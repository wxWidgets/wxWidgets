
from wxPython.wx import *
from wxPython.lib.filebrowsebutton import FileBrowseButton, FileBrowseButtonWithHistory


#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, ID, log):
        wxPanel.__init__(self, parent, ID)
        self.log = log
        self.fbb = FileBrowseButton(self, -1, wxPoint(20,20), wxSize(350, -1),
                                    changeCallback = self.fbbCallback)
        self.fbbh = FileBrowseButtonWithHistory(self, -1, wxPoint(20, 50),
                                                wxSize(350, -1),
                                                changeCallback = self.fbbhCallback)
        self.fbbh.SetHistory([])


    def fbbCallback(self, evt):
        self.log.write('FileBrowseButton: %s\n' % evt.GetString())


    def fbbhCallback(self, evt):
        value = evt.GetString()
        self.log.write('FileBrowseButtonWithHistory: %s\n' % value)
        history = self.fbbh.GetHistory()
        history.append(value)
        self.fbbh.SetHistory(history)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, -1, log)
    return win



#----------------------------------------------------------------------



overview = FileBrowseButton.__doc__
