
import  wx
import  wx.xrc  as  xrc

#----------------------------------------------------------------------

resourceText = r'''<?xml version="1.0"?>
<resource>

<!-- Notice that the class IS a standard wx class, and a custom
     subclass is specified as "moduleName.ClassName"  Try changing
     the classname to one that does not exist and see what happens -->

<object class="wxPanel" subclass="XmlResourceSubclass.MyCustomPanel" name="MyPanel">
    <size>200,100</size>
    <object class="wxStaticText" name="label1">
        <label>This panel is a custom class derived from wx.Panel,\nand is loaded by a custom XmlResourceHandler.</label>
        <pos>10,10</pos>
    </object>
</object>
</resource>
'''

#----------------------------------------------------------------------

class MyCustomPanel(wx.Panel):
    def __init__(self):
        p = wx.PrePanel()
        # the Create step is done by XRC.
        self.PostCreate(p)
        self.Bind(wx.EVT_WINDOW_CREATE, self.OnCreate)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        
        
    def OnCreate(self, evt):
        # This is the little bit of customization that we do for this
        # silly example.  It could just as easily have been done in
        # the resource.  We do it in the EVT_WINDOW_CREATE handler 
        # because the window doesn't really exist yet in the __init__.
        if self is evt.GetEventObject():
            t = wx.StaticText(self, -1, "MyCustomPanel")
            f = t.GetFont()
            f.SetWeight(wx.BOLD)
            f.SetPointSize(f.GetPointSize()+2)
            t.SetFont(f)
            self.t = t
            # On OSX the EVT_SIZE happens before EVT_WINDOW_CREATE !?!
            # so give it another kick
            wx.CallAfter(self.OnSize, None)
        evt.Skip()

    def OnSize(self, evt):
        if hasattr(self, 't'):
            sz = self.GetSize()
            w, h = self.t.GetTextExtent(self.t.GetLabel())
            self.t.SetPosition(((sz.width-w)/2, (sz.height-h)/2))
    
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
<h2><center>wx.XmlResourceSubclass</center></h2>

Sometimes it is necessary to use custom classes, but you still want
them to be created from XRC.  The subclass XRC attribute allows you to
do that.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

