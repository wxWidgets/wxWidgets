# 11/22/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
#

import  wx
import  wx.xrc  as  xrc

#----------------------------------------------------------------------

resourceText = r'''<?xml version="1.0"?>
<resource>

<!-- Notice that the class IS a standard wx class, and a custom
     subclass is specified as "moduleName.ClassName"  Try changing
     the classname to one that does not exist and see what happens -->

<object class="wxPanel" subclass="wxXmlResourceSubclass.MyBluePanel" name="MyPanel">
    <size>200,100</size>
    <object class="wxStaticText" name="label1">
        <label>This blue panel is a class derived from wxPanel
and is loaded by a using a subclass attribute of the object tag.</label>
        <pos>10,10</pos>
    </object>
</object>
</resource>
'''

#----------------------------------------------------------------------

class MyBluePanel(wx.Panel):
    def __init__(self):
        p = wx.PrePanel()
        self.PostCreate(p)

        self.Bind(wx.EVT_WINDOW_CREATE, self.OnCreate)
        
    def OnCreate(self, evt):
        # This is the little bit of customization that we do for this
        # silly example.  It could just as easily have been done in
        # the resource.  We do it in the EVT_WINDOW_CREATE handler 
        # because the window doesn't really exist yet in the __init__.
        self.SetBackgroundColour("BLUE")
        self.SetForegroundColour("WHITE")

#----------------------------------------------------------------------


class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        # make the components
        label = wx.StaticText(self, -1, "The lower panel was built from this XML:")
        label.SetFont(wx.Font(12, wx.SWISS, wx.NORMAL, wx.BOLD))

        text = wx.TextCtrl(self, -1, resourceText,
                          style=wx.TE_READONLY|wx.TE_MULTILINE)
        text.SetInsertionPoint(0)

        line = wx.StaticLine(self, -1)

        # Load the resource
        res = xrc.EmptyXmlResource()
        res.LoadFromString(resourceText)

        # Now create a panel from the resource data
        panel = res.LoadPanel(self, "MyPanel")

        # and do the layout
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(label, 0, wx.EXPAND|wx.TOP|wx.LEFT, 5)
        sizer.Add(text, 1, wx.EXPAND|wx.ALL, 5)
        sizer.Add(line, 0, wx.EXPAND)
        sizer.Add(panel, 1, wx.EXPAND|wx.ALL, 5)

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

