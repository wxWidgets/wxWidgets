
from wxPython.wx import *
from wxPython.wizard import *

import images

#----------------------------------------------------------------------

def makePageTitle(wizPg, title):
    sizer = wxBoxSizer(wxVERTICAL)
    wizPg.SetSizer(sizer)
    title = wxStaticText(wizPg, -1, title)
    title.SetFont(wxFont(18, wxSWISS, wxNORMAL, wxBOLD))
    sizer.AddWindow(title, 0, wxALIGN_CENTRE|wxALL, 5)
    sizer.AddWindow(wxStaticLine(wizPg, -1), 0, wxEXPAND|wxALL, 5)
    return sizer

#----------------------------------------------------------------------

class TitledPage(wxWizardPageSimple):
    def __init__(self, parent, title):
        wxWizardPageSimple.__init__(self, parent)
        self.sizer = makePageTitle(self, title)


#----------------------------------------------------------------------

class SkipNextPage(wxPyWizardPage):
    def __init__(self, parent, title):
        wxPyWizardPage.__init__(self, parent)
        self.next = self.prev = None
        self.sizer = makePageTitle(self, title)

        self.cb = wxCheckBox(self, -1, "Skip next page")
        self.sizer.Add(self.cb, 0, wxALL, 5)

    def SetNext(self, next):
        self.next = next

    def SetPrev(self, prev):
        self.prev = prev


    # Classes derived from wxPyWizardPanel must override
    # GetNext and GetPrev, and may also override GetBitmap
    # as well as all those methods overridable by
    # wxPyWindow.

    def GetNext(self):
        """If the checkbox is set then return the next page's next page"""
        next = self.next
        if self.cb.GetValue():
            next = next.GetNext()
        return next

    def GetPrev(self):
        return self.prev

#----------------------------------------------------------------------

class UseAltBitmapPage(wxPyWizardPage):
    def __init__(self, parent, title):
        wxPyWizardPage.__init__(self, parent)
        self.next = self.prev = None
        self.sizer = makePageTitle(self, title)

        self.sizer.Add(wxStaticText(self, -1, "This page uses a different bitmap"),
                       0, wxALL, 5)

    def SetNext(self, next):
        self.next = next

    def SetPrev(self, prev):
        self.prev = prev

    def GetNext(self):
        return self.next

    def GetPrev(self):
        return self.prev

    def GetBitmap(self):
        # You usually wouldn't need to override this method
        # since you can set a non-default bitmap in the
        # wxWizardPageSimple constructor, but if you need to
        # dynamically change the bitmap based on the
        # contents of the wizard, or need to also change the
        # next/prev order then it can be done by overriding
        # GetBitmap.
        return images.getWizTest2Bitmap()

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    ID_wiz = wxNewId()

    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)

        b = wxButton(self, -1, "Run Simple Wizard", pos=(50, 50))
        EVT_BUTTON(self, b.GetId(), self.OnRunSimpleWizard)

        b = wxButton(self, -1, "Run Dynamic Wizard", pos=(50, 100))
        EVT_BUTTON(self, b.GetId(), self.OnRunDynamicWizard)

        EVT_WIZARD_PAGE_CHANGED(self, self.ID_wiz, self.OnWizPageChanged)
        EVT_WIZARD_PAGE_CHANGING(self, self.ID_wiz, self.OnWizPageChanging)
        EVT_WIZARD_CANCEL(self, self.ID_wiz, self.OnWizCancel)


    def OnWizPageChanged(self, evt):
        if evt.GetDirection():
            dir = "forward"
        else:
            dir = "backward"
        page = evt.GetPage()
        self.log.write("OnWizPageChanged: %s, %s\n" % (dir, page.__class__))


    def OnWizPageChanging(self, evt):
        if evt.GetDirection():
            dir = "forward"
        else:
            dir = "backward"
        page = evt.GetPage()
        self.log.write("OnWizPageChanging: %s, %s\n" % (dir, page.__class__))


    def OnWizCancel(self, evt):
        page = evt.GetPage()
        self.log.write("OnWizCancel: %s\n" % page.__class__)

        # Show how to prevent cancelling of the wizard.  The
        # other events can be Veto'd too.
        if page is self.page1:
            wxMessageBox("Cancelling on the first page has been prevented.", "Sorry")
            evt.Veto()


    def OnRunSimpleWizard(self, evt):
        # Create the wizard and the pages
        wizard = wxWizard(self, self.ID_wiz, "Simple Wizard",
                          images.getWizTest1Bitmap())
        page1 = TitledPage(wizard, "Page 1")
        page2 = TitledPage(wizard, "Page 2")
        page3 = TitledPage(wizard, "Page 3")
        page4 = TitledPage(wizard, "Page 4")
        self.page1 = page1

        page1.sizer.Add(wxStaticText(page1, -1, """
This wizard is totally useless, but is meant to show how to
chain simple wizard pages together in a non-dynamic manner.
IOW, the order of the pages never changes, and so the
wxWizardPageSimple class can easily be used for the pages."""))
        wizard.FitToPage(page1)
        page4.sizer.Add(wxStaticText(page4, -1, "\nThis is the last page."))

        # Use the convenience Chain function to connect the pages
        wxWizardPageSimple_Chain(page1, page2)
        wxWizardPageSimple_Chain(page2, page3)
        wxWizardPageSimple_Chain(page3, page4)

        if wizard.RunWizard(page1):
            wxMessageBox("Wizard completed successfully", "That's all folks!")
        else:
            wxMessageBox("Wizard was cancelled", "That's all folks!")



    def OnRunDynamicWizard(self, evt):
        # Create the wizard and the pages
        wizard = wxWizard(self, self.ID_wiz, "Simple Wizard",
                          images.getWizTest1Bitmap())

        page1 = TitledPage(wizard, "Page 1")
        page2 = SkipNextPage(wizard, "Page 2")
        page3 = TitledPage(wizard, "Page 3")
        page4 = UseAltBitmapPage(wizard, "Page 4")
        page5 = TitledPage(wizard, "Page 5")
        self.page1 = page1

        page1.sizer.Add(wxStaticText(page1, -1, """
This wizard shows the ability to choose at runtime the order
of the pages and also which bitmap is shown.
"""))
        wizard.FitToPage(page1)
        page5.sizer.Add(wxStaticText(page5, -1, "\nThis is the last page."))

        # Set the initial order of the pages
        page1.SetNext(page2)
        page2.SetPrev(page1)
        page2.SetNext(page3)
        page3.SetPrev(page2)
        page3.SetNext(page4)
        page4.SetPrev(page3)
        page4.SetNext(page5)
        page5.SetPrev(page4)


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

