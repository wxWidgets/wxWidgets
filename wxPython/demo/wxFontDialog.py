
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        btn = wxButton(self, -1, "Select Font")
        EVT_BUTTON(self, btn.GetId(), self.OnSelectFont)

        self.sampleText = wxTextCtrl(self, -1, "Sample Text")
        #from wxPython.lib.stattext import wxGenStaticText
        #self.sampleText = wxGenStaticText(self, -1, "Sample Text")

        self.curFont = self.sampleText.GetFont()
        self.curClr = wxBLACK

        fgs = wxFlexGridSizer(cols=2, vgap=5, hgap=5)
        fgs.AddGrowableCol(1)
        fgs.AddGrowableRow(0)

        fgs.Add(btn)
        fgs.Add(self.sampleText, 0, wxADJUST_MINSIZE|wxGROW)

        fgs.Add(15,15); fgs.Add(15,15)   # an empty row

        fgs.Add(wxStaticText(self, -1, "PointSize:"))
        self.ps = wxStaticText(self, -1, "")
        font = self.ps.GetFont()
        font.SetWeight(wxBOLD)
        self.ps.SetFont(font)
        fgs.Add(self.ps, 0, wxADJUST_MINSIZE)

        fgs.Add(wxStaticText(self, -1, "Family:"))
        self.family = wxStaticText(self, -1, "")
        self.family.SetFont(font)
        fgs.Add(self.family, 0, wxADJUST_MINSIZE)

        fgs.Add(wxStaticText(self, -1, "Style:"))
        self.style = wxStaticText(self, -1, "")
        self.style.SetFont(font)
        fgs.Add(self.style, 0, wxADJUST_MINSIZE)

        fgs.Add(wxStaticText(self, -1, "Weight:"))
        self.weight = wxStaticText(self, -1, "")
        self.weight.SetFont(font)
        fgs.Add(self.weight, 0, wxADJUST_MINSIZE)

        fgs.Add(wxStaticText(self, -1, "Face:"))
        self.face = wxStaticText(self, -1, "")
        self.face.SetFont(font)
        fgs.Add(self.face, 0, wxADJUST_MINSIZE)

        fgs.Add(15,15); fgs.Add(15,15)   # an empty row

        fgs.Add(wxStaticText(self, -1, "wxNativeFontInfo:"))
        self.nfi = wxStaticText(self, -1, "")
        self.nfi.SetFont(font)
        fgs.Add(self.nfi, 0, wxADJUST_MINSIZE)

        # give it some border space
        sizer = wxBoxSizer(wxVERTICAL)
        sizer.Add(fgs, 0, wxGROW|wxADJUST_MINSIZE|wxALL, 25)

        self.SetSizer(sizer)
        self.UpdateUI()


    def UpdateUI(self):
        self.sampleText.SetFont(self.curFont)
        self.ps.SetLabel(str(self.curFont.GetPointSize()))
        self.family.SetLabel(self.curFont.GetFamilyString())
        self.style.SetLabel(self.curFont.GetStyleString())
        self.weight.SetLabel(self.curFont.GetWeightString())
        self.face.SetLabel(self.curFont.GetFaceName())
        self.nfi.SetLabel(self.curFont.GetNativeFontInfo().ToString())
        self.Layout()


    def OnSelectFont(self, evt):
        data = wxFontData()
        data.EnableEffects(True)
        data.SetColour(self.curClr)         # set colour
        data.SetInitialFont(self.curFont)

        dlg = wxFontDialog(self, data)
        if dlg.ShowModal() == wxID_OK:
            data = dlg.GetFontData()
            font = data.GetChosenFont()
            colour = data.GetColour()
            self.log.WriteText('You selected: "%s", %d points, color %s\n' %
                               (font.GetFaceName(), font.GetPointSize(),
                                colour.Get()))
            self.curFont = font
            self.curClr = colour
            self.UpdateUI()
        dlg.Destroy()





#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


#---------------------------------------------------------------------------




overview = """\
This class allows you to use the system font chooser dialog.

"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

