# The contents of the old wxPython.help module have been moved into
# the wx.core module in the new scheme.  This module will help with
# backwards compatibility by making those symbols visible in
# wxPython.help again.


import wxPython.wx

EVT_HELP = wxPython.wx.EVT_HELP
EVT_HELP_RANGE = wxPython.wx.EVT_HELP_RANGE
EVT_DETAILED_HELP = wxPython.wx.EVT_DETAILED_HELP
EVT_DETAILED_HELP_RANGE = wxPython.wx.EVT_DETAILED_HELP_RANGE
wxHelpEvent = wxPython.wx.wxHelpEvent
wxContextHelp = wxPython.wx.wxContextHelp
wxContextHelpButton = wxPython.wx.wxContextHelpButton
wxHelpProvider = wxPython.wx.wxHelpProvider
wxSimpleHelpProvider = wxPython.wx.wxSimpleHelpProvider
wxHelpProvider_Set = wxPython.wx.wxHelpProvider_Set
wxHelpProvider_Get = wxPython.wx.wxHelpProvider_Get
wxFRAME_EX_CONTEXTHELP = wxPython.wx.wxFRAME_EX_CONTEXTHELP
wxDIALOG_EX_CONTEXTHELP = wxPython.wx.wxDIALOG_EX_CONTEXTHELP
wxID_CONTEXT_HELP = wxPython.wx.wxID_CONTEXT_HELP
wxEVT_HELP = wxPython.wx.wxEVT_HELP
wxEVT_DETAILED_HELP = wxPython.wx.wxEVT_DETAILED_HELP
