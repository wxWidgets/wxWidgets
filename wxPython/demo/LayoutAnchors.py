
from wxPython.wx import *
from wxPython.lib.anchors import LayoutAnchors

#----------------------------------------------------------------------


[wxID_ANCHORSDEMOFRAMEANCHOREDPANEL, wxID_ANCHORSDEMOFRAMEHELPSTATICTEXT,
 wxID_ANCHORSDEMOFRAMEMAINPANEL, wxID_ANCHORSDEMOFRAMEBACKGROUNDPANEL,
 wxID_ANCHORSDEMOFRAMERIGHTCHECKBOX, wxID_ANCHORSDEMOFRAMEOKBUTTON,
 wxID_ANCHORSDEMOFRAMETOPCHECKBOX, wxID_ANCHORSDEMOFRAMEBOTTOMCHECKBOX,
 wxID_ANCHORSDEMOFRAME, wxID_ANCHORSDEMOFRAMELEFTCHECKBOX,
 ] = map(lambda _init_ctrls: wxNewId(), range(10))

class AnchorsDemoFrame(wxFrame):
    def _init_utils(self):
        pass

    def _init_ctrls(self, prnt):
        wxFrame.__init__(self, size = wxSize(328, 187), id = wxID_ANCHORSDEMOFRAME, title = 'LayoutAnchors Demonstration', parent = prnt, name = 'AnchorsDemoFrame', style = wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN, pos = wxPoint(261, 123))
        self._init_utils()

        self.mainPanel = wxPanel(size = wxSize(320, 160), parent = self, id = wxID_ANCHORSDEMOFRAMEMAINPANEL, name = 'panel1', style = wxTAB_TRAVERSAL | wxCLIP_CHILDREN, pos = wxPoint(0, 0))
        self.mainPanel.SetAutoLayout(true)

        self.okButton = wxButton(label = 'OK', id = wxID_ANCHORSDEMOFRAMEOKBUTTON, parent = self.mainPanel, name = 'okButton', size = wxSize(72, 24), style = 0, pos = wxPoint(240, 128))
        self.okButton.SetConstraints(LayoutAnchors(self.okButton, false, false, true, true))
        EVT_BUTTON(self.okButton, wxID_ANCHORSDEMOFRAMEOKBUTTON, self.OnOkButtonButton)

        self.backgroundPanel = wxPanel(size = wxSize(304, 80), parent = self.mainPanel, id = wxID_ANCHORSDEMOFRAMEBACKGROUNDPANEL, name = 'backgroundPanel', style = wxSIMPLE_BORDER | wxCLIP_CHILDREN, pos = wxPoint(8, 40))
        self.backgroundPanel.SetBackgroundColour(wxColour(255, 255, 255))
        self.backgroundPanel.SetConstraints(LayoutAnchors(self.backgroundPanel, true, true, true, true))

        self.anchoredPanel = wxPanel(size = wxSize(88, 48), id = wxID_ANCHORSDEMOFRAMEANCHOREDPANEL, parent = self.backgroundPanel, name = 'anchoredPanel', style = wxSIMPLE_BORDER, pos = wxPoint(104, 16))
        self.anchoredPanel.SetBackgroundColour(wxColour(0, 0, 222))
        self.anchoredPanel.SetConstraints(LayoutAnchors(self.anchoredPanel, false, false, false, false))

        self.leftCheckBox = wxCheckBox(label = 'Left', id = wxID_ANCHORSDEMOFRAMELEFTCHECKBOX, parent = self.mainPanel, name = 'leftCheckBox', size = wxSize(40, 16), style = 0, pos = wxPoint(8, 8))
        self.leftCheckBox.SetConstraints(LayoutAnchors(self.leftCheckBox, false, true, false, false))
        EVT_CHECKBOX(self.leftCheckBox, wxID_ANCHORSDEMOFRAMELEFTCHECKBOX, self.OnCheckboxCheckbox)

        self.topCheckBox = wxCheckBox(label = 'Top', id = wxID_ANCHORSDEMOFRAMETOPCHECKBOX, parent = self.mainPanel, name = 'topCheckBox', size = wxSize(40, 16), style = 0, pos = wxPoint(88, 8))
        self.topCheckBox.SetConstraints(LayoutAnchors(self.topCheckBox, false, true, false, false))
        EVT_CHECKBOX(self.topCheckBox, wxID_ANCHORSDEMOFRAMETOPCHECKBOX, self.OnCheckboxCheckbox)

        self.rightCheckBox = wxCheckBox(label = 'Right', id = wxID_ANCHORSDEMOFRAMERIGHTCHECKBOX, parent = self.mainPanel, name = 'rightCheckBox', size = wxSize(48, 16), style = 0, pos = wxPoint(168, 8))
        self.rightCheckBox.SetConstraints(LayoutAnchors(self.rightCheckBox, false, true, false, false))
        EVT_CHECKBOX(self.rightCheckBox, wxID_ANCHORSDEMOFRAMERIGHTCHECKBOX, self.OnCheckboxCheckbox)

        self.bottomCheckBox = wxCheckBox(label = 'Bottom', id = wxID_ANCHORSDEMOFRAMEBOTTOMCHECKBOX, parent = self.mainPanel, name = 'bottomCheckBox', size = wxSize(56, 16), style = 0, pos = wxPoint(248, 8))
        self.bottomCheckBox.SetConstraints(LayoutAnchors(self.bottomCheckBox, false, true, false, false))
        EVT_CHECKBOX(self.bottomCheckBox, wxID_ANCHORSDEMOFRAMEBOTTOMCHECKBOX, self.OnCheckboxCheckbox)

        self.helpStaticText = wxStaticText(label = 'Select anchor options above, then resize window to see the effect', id = wxID_ANCHORSDEMOFRAMEHELPSTATICTEXT, parent = self.mainPanel, name = 'helpStaticText', size = wxSize(224, 24), style = wxST_NO_AUTORESIZE, pos = wxPoint(8, 128))
        self.helpStaticText.SetConstraints(LayoutAnchors(self.helpStaticText, true, false, true, true))

    def __init__(self, parent):
        self._init_ctrls(parent)

    def OnCheckboxCheckbox(self, event):
        self.anchoredPanel.SetConstraints(
            LayoutAnchors(self.anchoredPanel,
                          self.leftCheckBox.GetValue(), self.topCheckBox.GetValue(),
                          self.rightCheckBox.GetValue(), self.bottomCheckBox.GetValue()) )

    def OnOkButtonButton(self, event):
        self.Close()

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = AnchorsDemoFrame(frame)
    frame.otherWin = win
    win.Show(true)




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









