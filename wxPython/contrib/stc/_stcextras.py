# Stuff these names into the wx namespace so wxPyConstructObject can find them

wx.wxStyledTextEventPtr       = wxStyledTextEventPtr
wx.wxStyledTextCtrlPtr        = wxStyledTextCtrlPtr

# This constant no longer exists in Scintilla, but I'll put it here for a while to avoid disrupting user code...
wxSTC_CARET_CENTER = 0
