from wxPython.wx import *
from wxPython.html import *
import os,sys

if not os.environ.has_key('WXWIN'):
    print "Can't find html samples. Set WXWIN environment variable."
    sys.exit(1)

# Minimal htmlhelpcontroller sample. Everything is done by the framework
help = wxHtmlHelpController() # Not created in OnInit, otherwise it's deleted before we see it.
help.AddBook(os.environ['WXWIN'] + "/samples/html/help/helpfiles/testing.hhp")

class MyApp(wxApp):
    def OnInit(self):
	help.DisplayContents()
	return TRUE

theApp = MyApp(0)
theApp.MainLoop()
