from wxPython.wx import *
from wxPython.html import *
import sys

default_page = """
<H1>HTML Viewer</H1>Please select <I>File->Open</I>
to open a HTML file, or edit this page in the
text control below and select <I>File->Update</I>
<P>
The python source can be seen
<a href="%s">here</a>.
""" % (sys.argv[0], )

class HtmlViewer(wxFrame):
	def __init__(self, parent, id, title, pos = wxDefaultPosition, size = wxSize(400,400)):
		wxFrame.__init__(self, parent, id, title, pos, size)
                self.CreateStatusBar(1)
                split = wxSplitterWindow(self, -1)
                self.html = wxHtmlWindow(split)
                self.html.SetRelatedFrame(self, "HTML Viewer:  \%s")
                self.html.SetRelatedStatusBar(0)
                self.txt = wxTextCtrl(split, -1, default_page,
                                      wxDefaultPosition, wxDefaultSize,
                                      wxTE_MULTILINE)
                split.SplitHorizontally(self.html, self.txt, size.y/2)
		mbar = wxMenuBar()
		menu = wxMenu()
		menu.Append(1500, "Open")
                menu.Append(1501, "Reset")
                menu.Append(1502, "Update HTML")
		menu.AppendSeparator()
		menu.Append(1503, "Exit")
		mbar.Append(menu, "File")
                menu = wxMenu()
                menu.Append(1510, "Back")
                menu.Append(1511, "Forward")
                mbar.Append(menu, "Go")
		self.SetMenuBar(mbar)
		self.filename = ""
                EVT_MENU(self, 1500, self.OnFileOpen)
                EVT_MENU(self, 1501, self.OnFileReset)
                EVT_MENU(self, 1502, self.OnFileUpdate)
                EVT_MENU(self, 1503, self.OnClose)
                EVT_MENU(self, 1510, self.OnGoBack)
                EVT_MENU(self, 1511, self.OnGoForward)
                # A default opening text
                self.html.SetPage( default_page )
        def OnFileOpen(self, event):
		dlg = wxFileDialog(NULL, "Open file")
		if dlg.ShowModal() == wxID_OK:
			self.filename = dlg.GetPath()
                        self.html.LoadPage(self.filename)
        def OnFileReset(self, event):
                self.html.SetPage( default_page )
                self.txt.SetValue( default_page )
        def OnFileUpdate(self, event):
                self.html.SetPage( self.txt.GetValue() )
        def OnGoBack(self, event):
                self.html.HistoryBack()
        def OnGoForward(self, event):
                self.html.HistoryForward()
	def OnClose(self,event):
		self.Destroy()

class MyApp(wxApp):
	def OnInit(self):
		frame = HtmlViewer(NULL, -1, "HTML Viewer")
		frame.Show(TRUE)
		self.SetTopWindow(frame)
		return TRUE

wxImage_AddHandler(wxPNGHandler())
wxImage_AddHandler(wxGIFHandler())
wxImage_AddHandler(wxJPEGHandler())

theApp = MyApp(0)
theApp.MainLoop()
