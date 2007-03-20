
import  wx
import  wx.xrc  as  xrc

#----------------------------------------------------------------------

resourceText = r'''<?xml version="1.0"?>
<resource>

<!-- Notice that the class is NOT a standard wx class -->

<object class="MyCustomPanel" name="MyPanel">
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
    def __init__(self, parent, id, pos, size, style, name):
        wx.Panel.__init__(self, parent, id, pos, size, style, name)

        # This is the little bit of customization that we do for this
        # silly example.  
        self.Bind(wx.EVT_SIZE, self.OnSize)
        t = wx.StaticText(self, -1, "MyCustomPanel")
        f = t.GetFont()
        f.SetWeight(wx.BOLD)
        f.SetPointSize(f.GetPointSize()+2)
        t.SetFont(f)
        self.t = t

    def OnSize(self, evt):
        sz = self.GetSize()
        w, h = self.t.GetTextExtent(self.t.GetLabel())
        self.t.SetPosition(((sz.width-w)/2, (sz.height-h)/2))


# To do it the more complex way, (see below) we need to write the
# class a little differently...  This could obviously be done with a
# single class, but I wanted to make separate ones to make clear what
# the different requirements are.
class PreMyCustomPanel(wx.Panel):
    def __init__(self):
        p = wx.PrePanel()
        self.PostCreate(p)

    def Create(self, parent, id, pos, size, style, name):
        wx.Panel.Create(self, parent, id, pos, size, style, name)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        t = wx.StaticText(self, -1, "MyCustomPanel")
        f = t.GetFont()
        f.SetWeight(wx.BOLD)
        f.SetPointSize(f.GetPointSize()+2)
        t.SetFont(f)
        self.t = t

    def OnSize(self, evt):
        sz = self.GetSize()
        w, h = self.t.GetTextExtent(self.t.GetLabel())
        self.t.SetPosition(((sz.width-w)/2, (sz.height-h)/2))

#----------------------------------------------------------------------

class MyCustomPanelXmlHandler(xrc.XmlResourceHandler):
    def __init__(self):
        xrc.XmlResourceHandler.__init__(self)
        # Specify the styles recognized by objects of this type
        self.AddStyle("wxNO_3D", wx.NO_3D)
        self.AddStyle("wxTAB_TRAVERSAL", wx.TAB_TRAVERSAL)
        self.AddStyle("wxWS_EX_VALIDATE_RECURSIVELY", wx.WS_EX_VALIDATE_RECURSIVELY)
        self.AddStyle("wxCLIP_CHILDREN", wx.CLIP_CHILDREN)
        self.AddWindowStyles()

    # This method and the next one are required for XmlResourceHandlers
    def CanHandle(self, node):
        return self.IsOfClass(node, "MyCustomPanel")

    def DoCreateResource(self):
        # NOTE: wxWindows can be created in either a single-phase or
        # in a two-phase way.  Single phase is what you normally do,
        # and two-phase creates the instnace first, and then later
        # creates the actual window when the Create method is called.
        # (In wxPython the first phase is done using the wxPre*
        # function, for example, wxPreFrame, wxPrePanel, etc.)
        #
        # wxXmlResource supports either method, a premade instance can
        # be created and populated by xrc using the appropriate
        # LoadOn* method (such as LoadOnPanel) or xrc can create the
        # instance too, using the Load* method.  However this makes
        # the handlers a bit more complex.  If you can be sure that a
        # particular class will never be loaded using a pre-existing
        # instance, then you can make the handle much simpler.  I'll
        # show both methods below.

        if 1:
            # The simple method assumes that there is no existing
            # instance.  Be sure of that with an assert.
            assert self.GetInstance() is None

            # Now create the object
            panel = MyCustomPanel(self.GetParentAsWindow(),
                                  self.GetID(),
                                  self.GetPosition(),
                                  self.GetSize(),
                                  self.GetStyle("style", wx.TAB_TRAVERSAL),
                                  self.GetName()
                                  )
        else:
            # When using the more complex (but more flexible) method
            # the instance may already have been created, check for it
            panel = self.GetInstance()
            if panel is None:
                # if not, then create the instance (but not the window)
                panel = PreMyCustomPanel()

            # Now call the panel's Create method to actually create the window
            panel.Create(self.GetParentAsWindow(),
                         self.GetID(),
                         self.GetPosition(),
                         self.GetSize(),
                         self.GetStyle("style", wx.TAB_TRAVERSAL),
                         self.GetName()
                         )

        # These two things should be done in either case:
        # Set standard window attributes
        self.SetupWindow(panel)
        # Create any child windows of this node
        self.CreateChildren(panel)

        return panel


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
        res.InsertHandler(MyCustomPanelXmlHandler())
        res.LoadFromString(resourceText)

        # Now create a panel from the resource data
        panel = res.LoadObject(self, "MyPanel", "MyCustomPanel")

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
<h2><center>wx.XmlResourceHandler</center></h2>

Deriving a class from wx.XmlResourceHandler allows you to specify your
own classes in XRC resources, and your handler class will then be used
to create instances of that class when the resource is loaded.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

