import wx
import wx.wizard

class TitledPage(wx.wizard.WizardPageSimple):
    def __init__(self, parent, title):
        wx.wizard.WizardPageSimple.__init__(self, parent)
        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(self.sizer)
        titleText = wx.StaticText(self, -1, title)
        titleText.SetFont(
                wx.Font(18, wx.SWISS, wx.NORMAL, wx.BOLD))
        self.sizer.Add(titleText, 0,
                wx.ALIGN_CENTRE | wx.ALL, 5)
        self.sizer.Add(wx.StaticLine(self, -1), 0,
                wx.EXPAND | wx.ALL, 5)

if __name__ == "__main__":
    app = wx.PySimpleApp()
    wizard = wx.wizard.Wizard(None, -1, "Simple Wizard")
    page1 = TitledPage(wizard, "Page 1")
    page2 = TitledPage(wizard, "Page 2")
    page3 = TitledPage(wizard, "Page 3")
    page4 = TitledPage(wizard, "Page 4")
    page1.sizer.Add(wx.StaticText(page1, -1,
            "Testing the wizard"))
    page4.sizer.Add(wx.StaticText(page4, -1,
            "This is the last page."))
    wx.wizard.WizardPageSimple_Chain(page1, page2)
    wx.wizard.WizardPageSimple_Chain(page2, page3)
    wx.wizard.WizardPageSimple_Chain(page3, page4)
    wizard.FitToPage(page1)

    if wizard.RunWizard(page1):
        print "Success"

    wizard.Destroy()

    

