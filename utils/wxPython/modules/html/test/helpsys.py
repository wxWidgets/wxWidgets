from wxPython.wx import *
from wxPython.html import *
import sys,os

if not os.environ.has_key('WXWIN'):
    print "Can't find html samples. Set WXWIN environment variable."
    sys.exit(1)

help = wxHtmlHelpSystem() 
# Create the book in a different way, supply title, contents file, index, default topic and basedir
help.AddBookParam('Sample help docs', 'contents.hhc', '', 'main.htm', 
		  os.environ['WXWIN'] + "/samples/html/help/helpfiles")

class HelpFrame(wxFrame):
    def __init__(self):
	wxFrame.__init__(self, NULL, -1, "", wxDefaultPosition, wxSize(500,500))
	bar = self.CreateStatusBar(2)
	bar.SetStatusWidths([400,100])
	help.CreateToolBar(self)
	self.text = wxTextCtrl(bar, 1001, "Hello world", wxPoint(400,0), wxSize(80,30))
	button = wxButton(bar, 1002, "Go", wxPoint(480,0), wxSize(20,30))

	html = wxHtmlWindow(self,-1)
	html.SetRelatedFrame(self, "%s")
        html.SetRelatedStatusBar(0)

	self.searchframe = wxMiniFrame(self, -1, "Search result", wxDefaultPosition, wxSize(300,400))
	self.searchbox = wxListBox(self.searchframe, wxID_HTML_SEARCHLIST)

	self.mframe = wxMiniFrame(self, -1, "Contents", wxDefaultPosition, wxSize(300,500))
	tree = help.CreateContentsTree(self.mframe)
	help.SetControls(self, html, tree, NULL, self.searchbox)
	EVT_TREE_SEL_CHANGED(self, wxID_HTML_TREECTRL, help.OnContentsSel)
	EVT_LISTBOX(self, wxID_HTML_SEARCHLIST, help.OnSearchSel)
	EVT_TOOL(self, wxID_HTML_PANEL, self.ToggleContents)
	EVT_TOOL(self, wxID_HTML_BACK, help.OnToolbar)
	EVT_TOOL(self, wxID_HTML_FORWARD, help.OnToolbar)
	EVT_TEXT_ENTER(self, 1001, self.OnTextSearch)
	EVT_BUTTON(self, 1002, self.OnTextSearch)
	EVT_CLOSE(self.mframe, self.OnCloseContents)
	EVT_CLOSE(self.searchframe, self.OnCloseSearch)
	self.mframe.Show(TRUE)
	print help.GetSearchList()

    def ToggleContents(self, event):
	self.mframe.Show(not self.mframe.IsShown())

    def OnCloseContents(self, event):
	if event.CanVeto():
	    self.mframe.Show(FALSE)
	else:
	    self.mframe.Destroy()
	
    def OnCloseSearch(self, event):
	if event.CanVeto():
	    self.searchframe.Show(FALSE)
	else:
	    self.searchframe.Destroy()
	
    def OnTextSearch(self, event):
	self.searchbox.Clear()
	self.searchframe.Show(TRUE)
	print "searching for %s" % (self.text.GetValue(),)
	help.KeywordSearch(self.text.GetValue())

class MyApp(wxApp):
    def OnInit(self):
	frame = HelpFrame()
	frame.Show(TRUE)
	return TRUE

theApp = MyApp(0)
theApp.MainLoop()
