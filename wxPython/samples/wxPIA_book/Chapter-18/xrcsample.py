"""
XRC is an XML-based resource format for wxPython.  With it you can
define the layout of widgets, and then load that XRC at runtime to
create the layout.  There are several GUI designers available that
understand the XRC format, a simple one called XRCed comes with
wxPython.
"""

import wx
import wx.xrc


class MyFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="XRC Sample",
                          size=(400,225))
        res = wx.xrc.XmlResource("xrcsample.xrc")
        panel = res.LoadPanel(self, "ID_PANEL")
        
        self.Bind(wx.EVT_BUTTON, self.OnOk,
                  wx.xrc.XRCCTRL(self, "ID_OK"))
        self.Bind(wx.EVT_BUTTON, self.OnCancel,
                  wx.xrc.XRCCTRL(self, "ID_CANCEL"))


    def OnOk(self, evt):
        namectrl  = wx.xrc.XRCCTRL(self, "ID_NAME")
        name = namectrl.GetValue()
        emailctrl = wx.xrc.XRCCTRL(self, "ID_EMAIL")
        email = emailctrl.GetValue()
        phonectrl = wx.xrc.XRCCTRL(self, "ID_PHONE")
        phone = phonectrl.GetValue()
        print "You entered:\n  name: %s\n  email: %s\n  phone: %s\n" \
              % (name, email, phone)

    def OnCancel(self, evt):
        self.Close()
    

app = wx.PySimpleApp(redirect=True)
frm = MyFrame()
frm.Show()
app.MainLoop()
