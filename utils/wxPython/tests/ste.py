

from wxPython.wx import *
from wxPython.ste import wxStyledTextEditorCtrl


app = wxPySimpleApp()
frame = wxFrame(None, -1, "Testing...", (0,0), (320,480))
ed = wxStyledTextEditorCtrl(frame, -1)
ed.AddText(open('test7.py').read())

#ed.AddText("This is a test\nThis is only a test.\n\nHere we go cowboys, here we go!!!\nThe End")
#ed.SetBufferedDraw(false)

#ed.StyleSetFontAttr(1, 14, "Times New Roman", true, false)
#ed.StyleSetForeground(1, wxBLUE)
#ed.StyleSetFont(2, wxFont(12, wxMODERN, wxNORMAL, wxNORMAL, false))

#ed.StartStyling(48, 0xff)
#ed.SetStyleFor(7, 1)


frame.Show(true)
app.MainLoop()
