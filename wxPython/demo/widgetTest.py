# 
# This file is used for the wx.HtmlWindow demo.
#

import  sys

import  wx
import  wx.html as  html

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, id=-1, size=wx.DefaultSize, bgcolor=None):
        wx.Panel.__init__(self, parent, id, size=size)

        if bgcolor:
            self.SetBackgroundColour(bgcolor)

        wx.StaticText(self, -1, 'Name:', (10, 10))
        wx.StaticText(self, -1, 'Email:', (10, 40))

        self.name  = wx.TextCtrl(self, -1, '', (50, 10), (100, -1))
        self.email = wx.TextCtrl(self, -1, '', (50, 40), (100, -1))

        wx.Button(self, -1, 'Okay', (50, 70))
        self.Bind(wx.EVT_BUTTON, self.OnButton)


    def OnButton(self, event):
        name = self.name.GetValue()
        email = self.email.GetValue()
        dlg = wx.MessageDialog(
                self, 'You entered:\n    %s\n    %s' % (name, email),
                'Results', style = wx.OK | wx.ICON_INFORMATION
                )

        dlg.ShowModal()
        dlg.Destroy()



#----------------------------------------------------------------------

class TestHtmlPanel(wx.Panel):
    def __init__(self, parent, id=-1, size=wx.DefaultSize):

        import About

        wx.Panel.__init__(self, parent, id, size=size)
        self.html = html.HtmlWindow(self, -1, (5,5), (400, 350))
        py_version = sys.version.split()[0]
        self.html.SetPage(About.MyAboutBox.text %
                          (wx.VERSION_STRING,
                           ", ".join(wx.PlatformInfo[1:]),
                           py_version))
        ir = self.html.GetInternalRepresentation()
        self.html.SetSize( (ir.GetWidth()+5, ir.GetHeight()+5) )
        self.Fit()

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestHtmlPanel(frame)
    return win

#----------------------------------------------------------------------

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

