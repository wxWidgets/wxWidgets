
import  wx
import  wx.lib.anchors as anchors

#----------------------------------------------------------------------

# Nifty little trick here; apply wx.NewId() to generate a series of
# IDs used later on in the app. 

[   ID_ANCHORSDEMOFRAMEANCHOREDPANEL, 
    ID_ANCHORSDEMOFRAMEHELPSTATICTEXT,
    ID_ANCHORSDEMOFRAMEMAINPANEL, 
    ID_ANCHORSDEMOFRAMEBACKGROUNDPANEL,
    ID_ANCHORSDEMOFRAMERIGHTCHECKBOX, 
    ID_ANCHORSDEMOFRAMEOKBUTTON,
    ID_ANCHORSDEMOFRAMETOPCHECKBOX, 
    ID_ANCHORSDEMOFRAMEBOTTOMCHECKBOX,
    ID_ANCHORSDEMOFRAME, 
    ID_ANCHORSDEMOFRAMELEFTCHECKBOX,
 ] = map(lambda _init_ctrls: wx.NewId(), range(10))

# A small note here: while only certain parts of this frame are actually demonstrating
# the capabilities of the LayoutAnchors feature, all the controls are within the same
# frame; therefore, all controls and windows within the frame must use LayoutAnchors.
# You can't mix LayoutAnchors and sizers.
class AnchorsDemoFrame(wx.Frame):
    def _init_utils(self):
        pass

    def _init_ctrls(self, prnt):
        wx.Frame.__init__(
            self, size=(328, 187), id=ID_ANCHORSDEMOFRAME, 
            title='LayoutAnchors Demonstration', parent=prnt, 
            name='AnchorsDemoFrame', 
            style = wx.DEFAULT_FRAME_STYLE | wx.CLIP_CHILDREN, pos=(261, 123)
            )

        self._init_utils()

        self.mainPanel = wx.Panel(
                            size=(320, 160), parent=self, 
                            id=ID_ANCHORSDEMOFRAMEMAINPANEL, name='panel1', 
                            style=wx.TAB_TRAVERSAL | wx.CLIP_CHILDREN
                            | wx.FULL_REPAINT_ON_RESIZE, 
                            pos=(0, 0)
                            )

        self.mainPanel.SetAutoLayout(True)

        self.okButton = wx.Button(
                            label='OK', id=ID_ANCHORSDEMOFRAMEOKBUTTON, 
                            parent=self.mainPanel, name='okButton', 
                            size=(72, 24), style=0, pos=(240, 128)
                            )

        self.okButton.SetConstraints(
            anchors.LayoutAnchors(self.okButton, False, False, True, True)
            )

        self.Bind(
            wx.EVT_BUTTON, self.OnOkButtonButton, id=ID_ANCHORSDEMOFRAMEOKBUTTON
            )

        self.backgroundPanel = wx.Panel(
                                size=(304, 80), parent=self.mainPanel, 
                                id=ID_ANCHORSDEMOFRAMEBACKGROUNDPANEL, 
                                name='backgroundPanel', 
                                style=wx.SIMPLE_BORDER | wx.CLIP_CHILDREN, 
                                pos = (8, 40)
                                )

        self.backgroundPanel.SetBackgroundColour(wx.Colour(255, 255, 255))
        self.backgroundPanel.SetConstraints(
            anchors.LayoutAnchors(self.backgroundPanel, True, True, True, True)
            )

        self.anchoredPanel = wx.Panel(
                                size=(88, 48), id=ID_ANCHORSDEMOFRAMEANCHOREDPANEL, 
                                parent=self.backgroundPanel, name='anchoredPanel', 
                                style=wx.SIMPLE_BORDER, pos=(104, 16)
                                )

        self.anchoredPanel.SetBackgroundColour(wx.Colour(0, 0, 222))
        self.anchoredPanel.SetConstraints(
            anchors.LayoutAnchors(self.anchoredPanel, False, False, False, False)
            )

        self.leftCheckBox = wx.CheckBox(
                                label='Left', id=ID_ANCHORSDEMOFRAMELEFTCHECKBOX, 
                                parent=self.mainPanel, name='leftCheckBox', 
                                style=0, pos=(8, 8)
                                )

        self.leftCheckBox.SetConstraints(
            anchors.LayoutAnchors(self.leftCheckBox, False, True, False, False)
            )
        
        self.Bind(
            wx.EVT_CHECKBOX, self.OnCheckboxCheckbox, source=self.leftCheckBox,
            id=ID_ANCHORSDEMOFRAMELEFTCHECKBOX
            )

        self.topCheckBox = wx.CheckBox(
                            label='Top', id=ID_ANCHORSDEMOFRAMETOPCHECKBOX, 
                            parent=self.mainPanel, name='topCheckBox', 
                            style=0, pos=(88, 8)
                            )

        self.topCheckBox.SetConstraints(
            anchors.LayoutAnchors(self.topCheckBox, False, True, False, False)
            )
        
        self.Bind(
            wx.EVT_CHECKBOX, self.OnCheckboxCheckbox, source=self.topCheckBox,
            id=ID_ANCHORSDEMOFRAMETOPCHECKBOX
            )

        self.rightCheckBox = wx.CheckBox(
                            label='Right', id=ID_ANCHORSDEMOFRAMERIGHTCHECKBOX, 
                            parent=self.mainPanel, name='rightCheckBox', 
                            style=0, pos=(168, 8)
                            )

        self.rightCheckBox.SetConstraints(
            anchors.LayoutAnchors(self.rightCheckBox, False, True, False, False)
            )

        self.Bind(
            wx.EVT_CHECKBOX, self.OnCheckboxCheckbox, source=self.rightCheckBox,
            id=ID_ANCHORSDEMOFRAMERIGHTCHECKBOX
            )

        self.bottomCheckBox = wx.CheckBox(
                                label='Bottom', id=ID_ANCHORSDEMOFRAMEBOTTOMCHECKBOX, 
                                parent=self.mainPanel, name='bottomCheckBox', 
                                style=0, pos=(248, 8)
                                )

        self.bottomCheckBox.SetConstraints(
            anchors.LayoutAnchors(self.bottomCheckBox, False, True, False, False)
            )

        self.Bind(
            wx.EVT_CHECKBOX, self.OnCheckboxCheckbox, source=self.bottomCheckBox,
            id=ID_ANCHORSDEMOFRAMEBOTTOMCHECKBOX
            )

        self.helpStaticText = wx.StaticText(
                                label='Select anchor options above, then resize window to see the effect', 
                                id=ID_ANCHORSDEMOFRAMEHELPSTATICTEXT, 
                                parent=self.mainPanel, name='helpStaticText', 
                                size=(224, 24), style=wx.ST_NO_AUTORESIZE, 
                                pos=(8, 128)
                                )

        self.helpStaticText.SetConstraints(
            anchors.LayoutAnchors(self.helpStaticText, True, False, True, True)
            )

    def __init__(self, parent):
        self._init_ctrls(parent)

    # Based on the values of the above checkboxes, we will adjust the layout constraints
    # on the sample window whenever one of the checkboxes changes state.
    def OnCheckboxCheckbox(self, event):
        self.anchoredPanel.SetConstraints(
            anchors.LayoutAnchors(self.anchoredPanel,
                          self.leftCheckBox.GetValue(), self.topCheckBox.GetValue(),
                          self.rightCheckBox.GetValue(), self.bottomCheckBox.GetValue()
                          ) 
            )

    def OnOkButtonButton(self, event):
        self.Close()

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Show the LayoutAnchors sample", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        win = AnchorsDemoFrame(self)
        win.Show(True)


#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------


overview = """<html><body>
<h2>LayoutAnchors</h2>
        A class that implements Delphi's Anchors with wxLayoutConstraints.
<p>
        Anchored sides maintain the distance from the edge of the
        control to the same edge of the parent.
        When neither side is selected, the control keeps the same
        relative position to both sides.
<p>
        The current position and size of the control and it's parent
        is used when setting up the constraints. To change the size or
        position of an already anchored control, set the constraints to
        None, reposition or resize and reapply the anchors.
<p>
        Examples:
<p>
        Let's anchor the right and bottom edge of a control and
        resize it's parent.
<p>
<pre>
        ctrl.SetConstraints(LayoutAnchors(ctrl, left=0, top=0, right=1, bottom=1))

        +=========+         +===================+
        | +-----+ |         |                   |
        | |     * |   ->    |                   |
        | +--*--+ |         |           +-----+ |
        +---------+         |           |     * |
                            |           +--*--+ |
                            +-------------------+
        * = anchored edge
</pre>
<p>
        When anchored on both sides the control will stretch horizontally.
<p>
<pre>
        ctrl.SetConstraints(LayoutAnchors(ctrl, 1, 0, 1, 1))

        +=========+         +===================+
        | +-----+ |         |                   |
        | *     * |   ->    |                   |
        | +--*--+ |         | +---------------+ |
        +---------+         | *     ctrl      * |
                            | +-------*-------+ |
                            +-------------------+
        * = anchored edge
</pre>
</html></body>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

