""" Demonstrate filebrowsebutton module of the wxPython.lib Library.

14.1.2001 Bernhard Reiter <bernhard@intevation.de>
    Added demo for DirBrowseButton and improved overview text.
"""
from wxPython.wx import *
from wxPython.lib.filebrowsebutton import FileBrowseButton, FileBrowseButtonWithHistory,DirBrowseButton


#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, ID, log):
        wxPanel.__init__(self, parent, ID)
        self.log = log
        self.fbb = FileBrowseButton(self, -1, wxPoint(20,20), wxSize(450, -1),
                                    changeCallback = self.fbbCallback)
        self.fbbh = FileBrowseButtonWithHistory(self, -1, wxPoint(20, 50),
                                                wxSize(450, -1),
                                                #changeCallback = self.fbbhCallback
                                                )
        self.dbb = DirBrowseButton(self, -1, wxPoint(20,80), wxSize(450,-1),
                                    changeCallback = self.dbbCallback)


        self.fbbh.SetHistory(['You', 'can', 'put', 'some', 'file', 'names', 'here'])


    def fbbCallback(self, evt):
        self.log.write('FileBrowseButton: %s\n' % evt.GetString())



    def fbbhCallback(self, evt):
        if hasattr(self, 'fbbh'):
            value = evt.GetString()
            self.log.write('FileBrowseButtonWithHistory: %s\n' % value)
            history = self.fbbh.GetHistory()
            history.append(value)
            self.fbbh.SetHistory(history)

    def dbbCallback(self, evt):
        self.log.write('DirBrowseButton: %s\n' % evt.GetString())



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, -1, log)
    return win



#----------------------------------------------------------------------

overview = """<html><body>
<h2>class FileBrowseButton:</h2>
<small><pre>
%s
</pre></small>

<h2>class FileBrowseButtonWithHistory(FileBrowseButton):</h2>
<small><pre>
%s
</pre></small>

<h2>class DirBrowseButton(FileBrowseButton):</h2>
<small><pre>
%s
</pre></small>

</body><</html>
""" % ( FileBrowseButton.__doc__,
        FileBrowseButtonWithHistory.__doc__ ,
        str(DirBrowseButton.__doc__) )
