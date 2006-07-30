"""Basic regular expression demostration facility (Perl style syntax)."""

from wxPython.wx import *
import re

#----------------------------------------------------------------------

class ReDemoPanel(wxPanel):
    def __init__(self, parent):
        wxPanel.__init__(self, parent, -1)

        rePrompt = wxStaticText(self, -1, "Enter a Perl-style regular expression")
        reText = wxTextCtrl(self, 101, "")

        options = self.AddOptions()

        sPrompt = wxStaticText(self, -1, "Enter a string to search")
        sText = wxTextCtrl(self, 102, "", style=wxTE_MULTILINE)

        dispPrompt = wxStaticText(self, -1, "Groups:")
        dispText = wxTextCtrl(self, 103, "", style=wxTE_MULTILINE|wxTE_READONLY)


#----------------------------------------------------------------------
#----------------------------------------------------------------------
