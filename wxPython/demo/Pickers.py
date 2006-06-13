
import wx

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        box = wx.BoxSizer(wx.VERTICAL)
        title = wx.StaticText(self, -1, "Picker Controls")
        title.SetFont(wx.FFont(24, wx.FONTFAMILY_SWISS, wx.FONTFLAG_BOLD))
        title.SetForegroundColour("navy")
        box.Add(title, 0, wx.ALIGN_CENTER|wx.ALL, 5)
        #print title.GetBestSize(), title.GetMinSize(), title.GetSize()
        
        box.Add(wx.StaticLine(self), 0, wx.EXPAND)
        
        fgs = wx.FlexGridSizer(cols=4, hgap=5, vgap=5)
        fgs.AddGrowableCol(3)
        fgs.Add((10,10))  # spacer
        lbl = wx.StaticText(self, -1, "default style")
        lbl.SetFont(wx.FFont(12, wx.FONTFAMILY_SWISS, wx.FONTFLAG_BOLD))
        fgs.Add(lbl)
        fgs.Add((10,10))  # spacer
        lbl = wx.StaticText(self, -1, "with textctrl")
        lbl.SetFont(wx.FFont(12, wx.FONTFAMILY_SWISS, wx.FONTFLAG_BOLD))
        fgs.Add(lbl, 0, wx.ALIGN_CENTER)

        fgs.Add(wx.StaticText(self, -1, "wx.ColourPickerCtrl:"), 0, wx.ALIGN_CENTER_VERTICAL)
        cp1 = wx.ColourPickerCtrl(self)
        fgs.Add(cp1, 0, wx.ALIGN_CENTER)
        fgs.Add((10,10))  # spacer
        cp2 = wx.ColourPickerCtrl(self, style=wx.CLRP_USE_TEXTCTRL)
        cp2.SetTextCtrlProportion(5)
        fgs.Add(cp2, 0, wx.EXPAND)
        self.Bind(wx.EVT_COLOURPICKER_CHANGED, self.OnPickColor, cp1)
        self.Bind(wx.EVT_COLOURPICKER_CHANGED, self.OnPickColor, cp2)

        fgs.Add(wx.StaticText(self, -1, "wx.DirPickerCtrl:"), 0, wx.ALIGN_CENTER_VERTICAL)
        dp1 = wx.DirPickerCtrl(self)
        fgs.Add(dp1, 0, wx.ALIGN_CENTER)
        fgs.Add((10,10))  # spacer
        dp2 = wx.DirPickerCtrl(self, style=wx.DIRP_USE_TEXTCTRL)
        dp2.SetTextCtrlProportion(2)
        fgs.Add(dp2, 0, wx.EXPAND)
        self.Bind(wx.EVT_DIRPICKER_CHANGED, self.OnPickFileDir, dp1)
        self.Bind(wx.EVT_DIRPICKER_CHANGED, self.OnPickFileDir, dp2)

        fgs.Add(wx.StaticText(self, -1, "wx.FilePickerCtrl:"), 0, wx.ALIGN_CENTER_VERTICAL)
        fp1 = wx.FilePickerCtrl(self)
        fgs.Add(fp1, 0, wx.ALIGN_CENTER)
        fgs.Add((10,10))  # spacer
        fp2 = wx.FilePickerCtrl(self, style=wx.FLP_USE_TEXTCTRL)
        fp2.SetTextCtrlProportion(2)
        fgs.Add(fp2, 0, wx.EXPAND)
        self.Bind(wx.EVT_FILEPICKER_CHANGED, self.OnPickFileDir, fp1)
        self.Bind(wx.EVT_FILEPICKER_CHANGED, self.OnPickFileDir, fp2)

        fgs.Add(wx.StaticText(self, -1, "wx.FontPickerCtrl:"), 0, wx.ALIGN_CENTER_VERTICAL)
        fnt1 = wx.FontPickerCtrl(self, style=wx.FNTP_FONTDESC_AS_LABEL)
        fgs.Add(fnt1, 0, wx.ALIGN_CENTER)
        fgs.Add((10,10))  # spacer
        fnt2 = wx.FontPickerCtrl(self, style=wx.FNTP_FONTDESC_AS_LABEL|wx.FNTP_USE_TEXTCTRL)
        fnt2.SetTextCtrlProportion(2)
        fgs.Add(fnt2, 0, wx.EXPAND)
        self.Bind(wx.EVT_FONTPICKER_CHANGED, self.OnPickFont, fnt1)
        self.Bind(wx.EVT_FONTPICKER_CHANGED, self.OnPickFont, fnt2)


        box.Add(fgs, 1, wx.EXPAND|wx.ALL, 5)
        self.SetSizer(box)


    def OnPickColor(self, evt):
        self.log.write("You chose: %s\n" % repr(evt.GetColour()))

    def OnPickFileDir(self, evt):
        self.log.write("You chose: %s\n" % repr(evt.GetPath()))

    def OnPickFont(self, evt):
        font = evt.GetFont()        
        self.log.write("You chose: %s\n" % font.GetNativeFontInfoUserDesc())
                       
#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>Picker Controls</center></h2>

The Picker controls are either native or generic controls usually
comprised of a button and with an optional text control next to it.
The pickers enable the user to choose something using one of the
common dialogs and then displays the result in some way.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

