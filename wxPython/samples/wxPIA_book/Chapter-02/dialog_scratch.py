#!/usr/bin/env python

import wx
import images    

class App(wx.App):

    def __init__(self, redirect=True, filename=None):
        wx.App.__init__(self, redirect, filename)
    
    def OnInit(self):
        dlg = wx.MessageDialog(None, 'Is this the coolest thing ever!',
                          'MessageDialog', wx.YES_NO | wx.ICON_QUESTION)
        result = dlg.ShowModal()
        dlg.Destroy()

        dlg = wx.TextEntryDialog(None, "Who is buried in Grant's tomb?",
                'A Question', 'Cary Grant')
        if dlg.ShowModal() == wx.ID_OK:
            response = dlg.GetValue()
        dlg.Destroy()

        dlg = wx.SingleChoiceDialog(None, 
                'What version of Python are you using?', 'Single Choice',
               ['1.5.2', '2.0', '2.1.3', '2.2', '2.3.1'])
        if dlg.ShowModal() == wx.ID_OK:
            response = dlg.GetStringSelection()
        dlg.Destroy()

        return True


if __name__ == '__main__':
    app = App(False, "output")
    fred = app.MainLoop()
    
