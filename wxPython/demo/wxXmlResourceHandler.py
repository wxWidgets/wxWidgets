
from wxPython.wx import *
from wxPython.xrc import *

#----------------------------------------------------------------------

resourceText = r'''<?xml version="1.0"?>
<resource>

<!-- Notice that the class is NOT a standard wx class -->

<object class="MyBluePanel" name="MyPanel">
    <size>200,100</size>
    <object class="wxStaticText" name="lable1" subclass="wxPython.wx.wxPreStaticText">
        <label>This blue panel is a class derived from wxPanel,\nand is loaded by a custom wxXmlResourceHandler.</label>
        <pos>10,10</pos>
    </object>
</object>
</resource>
'''

#----------------------------------------------------------------------

class MyBluePanel(wxPanel):
    def __init__(self, parent, id, pos, size, style, name):
        wxPanel.__init__(self, parent, id, pos, size, style, name)

        # This is the little bit of customization that we do for this
        # silly example.  It could just as easily have been done in
        # the resource.
        self.SetBackgroundColour("BLUE")
        self.SetForegroundColour("WHITE")


# To do it the more complex way, (see below) we need to write the
# class a little differently...  This could obviously be done with a
# single class, but I wanted to make separate ones to make clear what
# the different requirements are.
class PreMyBluePanel(wxPanel):
    def __init__(self):
        p = wxPrePanel()
        self.this = p.this

    def Create(self, parent, id, pos, size, style, name):
        wxPanel.Create(self, parent, id, pos, size, style, name)
        self.SetBackgroundColour("BLUE")
        self.SetForegroundColour("WHITE")


#----------------------------------------------------------------------

class MyBluePanelXmlHandler(wxXmlResourceHandler):
    def __init__(self):
        wxXmlResourceHandler.__init__(self)
        # Specify the styles recognized by objects of this type
        self.AddStyle("wxNO_3D", wxNO_3D);
        self.AddStyle("wxTAB_TRAVERSAL", wxTAB_TRAVERSAL);
        self.AddStyle("wxWS_EX_VALIDATE_RECURSIVELY", wxWS_EX_VALIDATE_RECURSIVELY);
        self.AddStyle("wxCLIP_CHILDREN", wxCLIP_CHILDREN);
        self.AddWindowStyles();

    # This method and the next one are required for XmlResourceHandlers
    def CanHandle(self, node):
        return self.IsOfClass(node, "MyBluePanel")

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
            panel = MyBluePanel(self.GetParentAsWindow(),
                                self.GetID(),
                                self.GetPosition(),
                                self.GetSize(),
                                self.GetStyle("style", wxTAB_TRAVERSAL),
                                self.GetName()
                                )
        else:
            # When using the more complex (but more flexible) method
            # the instance may already have been created, check for it
            panel = self.GetInstance()
            if panel is None:
                # if not, then create the instance (but not the window)
                panel = PreMyBluePanel()

            # Now call the panel's Create method to actually create the window
            panel.Create(self.GetParentAsWindow(),
                         self.GetID(),
                         self.GetPosition(),
                         self.GetSize(),
                         self.GetStyle("style", wxTAB_TRAVERSAL),
                         self.GetName()
                         )

        # These two things should be done in either case:
        # Set standard window attributes
        self.SetupWindow(panel)
        # Create any child windows of this node
        self.CreateChildren(panel)

        return panel


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
        res.InsertHandler(MyBluePanelXmlHandler())
        res.LoadFromString(resourceText)

        # Now create a panel from the resource data
        panel = res.LoadObject(self, "MyPanel", "MyBluePanel")

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
<h2><center>wxXmlResourceHandler</center></h2>

Deriving a class from wxXmlResourceHandler allows you to specify your
own classes in XRC resources, and your handler class will then be used
to create instances of that class when the resource is loaded.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

