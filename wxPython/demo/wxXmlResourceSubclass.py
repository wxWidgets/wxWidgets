
from wxPython.wx import *
from wxPython.xrc import *

#----------------------------------------------------------------------

resourceText = r'''<?xml version="1.0"?>
<resource>

<!-- Notice that the class IS a standard wx class, and a custom
     subclass is specified as "moduleName.ClassName" -->

<object class="wxPanel" subclass="wxXmlResourceSubclass.MyBluePanel" name="MyPanel">
    <size>200,100</size>
    <object class="wxStaticText" name="label1" subclass="wxPython.wx.wxPreStaticText">
        <label>This blue panel is a class derived from wxPanel
and is loaded by a using a subclass attribute of the object tag.</label>
        <pos>10,10</pos>
    </object>
</object>
</resource>
'''

#----------------------------------------------------------------------

class MyBluePanel(wxPanel):
    def __init__(self):
        #print "MyBluePanel.__init__"
        p = wxPrePanel()
        self.this = p.this
        self.thisown = p.thisown
        self._setOORInfo(self)

        EVT_WINDOW_CREATE(self, self.OnCreate)
        
    def OnCreate(self, evt):
        # This is the little bit of customization that we do for this
        # silly example.  It could just as easily have been done in
        # the resource.  We do it in the EVT_WINDOW_CREATE handler 
        # because the window doesn't really exist yet in the __init__.
        self.SetBackgroundColour("BLUE")
        self.SetForegroundColour("WHITE")

#----------------------------------------------------------------------


class TestPanel(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)

        # make the components
        label = wxStaticText(self, -1, "The lower panel was built from this XML:")
        label.SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD))

        text = wxTextCtrl(self, -1, resourceText,
                          style=wxTE_READONLY|wxTE_MULTILINE)
        text.SetInsertionPoint(0)

        line = wxStaticLine(self, -1)

        # Load the resource
        res = wxEmptyXmlResource()
        res.LoadFromString(resourceText)

        # Now create a panel from the resource data
        panel = res.LoadPanel(self, "MyPanel")

        # and do the layout
        sizer = wxBoxSizer(wxVERTICAL)
        sizer.Add(label, 0, wxEXPAND|wxTOP|wxLEFT, 5)
        sizer.Add(text, 1, wxEXPAND|wxALL, 5)
        sizer.Add(line, 0, wxEXPAND)
        sizer.Add(panel, 1, wxEXPAND|wxALL, 5)

        self.SetSizer(sizer)
        self.SetAutoLayout(True)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wxXmlResourceSubclass</center></h2>

Sometimes it is necessary to use custom classes, but you still want
them to be created from XRC.  The subclass XRC attribute allows you to
do that.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

