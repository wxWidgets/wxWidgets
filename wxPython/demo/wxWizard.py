
from wxPython.wx import *
from wxPython.wizard import *

import images

#----------------------------------------------------------------------

class TitledPage(wxWizardPageSimple):
    def __init__(self, parent, title):
        wxWizardPageSimple.__init__(self, parent)
        self.sizer = sizer = wxBoxSizer(wxVERTICAL)
        self.SetSizer(sizer)

        title = wxStaticText(self, -1, title)
        title.SetFont(wxFont(18, wxSWISS, wxNORMAL, wxBOLD))
        sizer.AddWindow(title, 0, wxALIGN_CENTRE|wxALL, 5)
        sizer.AddWindow(wxStaticLine(self, -1), 0, wxEXPAND|wxALL, 5)



#----------------------------------------------------------------------

class TestPanel(wxPanel):
    ID_wiz = wxNewId()

    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)

        b = wxButton(self, -1, "Run Simple Wizard", pos=(50,50))
        EVT_BUTTON(self, b.GetId(), self.OnRunSimpleWizard)

        EVT_WIZARD_PAGE_CHANGED(self, self.ID_wiz, self.OnWizPageChanged)
        EVT_WIZARD_PAGE_CHANGING(self, self.ID_wiz, self.OnWizPageChanging)
        EVT_WIZARD_CANCEL(self, self.ID_wiz, self.OnWizCancel)


    def OnWizPageChanged(self, evt):
        if evt.GetDirection():
            dir = "forward"
        else:
            dir = "backward"
        self.log.write("OnWizPageChanged: %s, %s\n" % (dir, evt.GetPage()))

    def OnWizPageChanging(self, evt):
        if evt.GetDirection():
            dir = "forward"
        else:
            dir = "backward"
        self.log.write("OnWizPageChanging: %s, %s\n" % (dir, evt.GetPage()))

    def OnWizCancel(self, evt):
        pass


    def OnRunSimpleWizard(self, evt):
        # Create the wizard and the pages
        wizard = wxWizard(self, self.ID_wiz, "Simple Wizard",
                          images.getWizTest1Bitmap())
        page1 = TitledPage(wizard, "Page 1")
        page2 = TitledPage(wizard, "Page 2")
        page3 = TitledPage(wizard, "Page 3")
        page4 = TitledPage(wizard, "Page 4")

        page1.sizer.Add(wxStaticText(page1, -1, """\
This wizard is totally useless, but is meant to show how to
chain simple wizard pages together in a non-dynamic manner.
IOW, the order of the pages never changes, and so the
wxWizardPageSimple class can be used for the pages."""))
        wizard.FitToPage(page1)

        # Use the convenience Chain function to connect the pages
        wxWizardPageSimple_Chain(page1, page2)
        wxWizardPageSimple_Chain(page2, page3)
        wxWizardPageSimple_Chain(page3, page4)

        if wizard.RunWizard(page1):
            wxMessageBox("Wizard completed successfully", "That's all folks!")
        else:
            wxMessageBox("Wizard was cancelled", "That's all folks!")


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wxWizard</center></h2>

wxWizard is the central class for implementing 'wizard-like'
dialogs. These dialogs are mostly familiar to Windows users and are
nothing else but a sequence of 'pages' each of them displayed inside a
dialog which has the buttons to pass to the next (and previous) pages.
<p>
The wizards are typically used to decompose a complex dialog into
several simple steps and are mainly useful to the novice users, hence
it is important to keep them as simple as possible.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

