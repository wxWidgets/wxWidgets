from wxPython.wx import *
from wxPython.html import *
import sys,string

# A bunch of simple widgets, all somehow derived from wxWindow
class Widget1(wxWindow):
    def __init__(self, parent, param):
	wxWindow.__init__(self, parent, -1)
	self.text = wxTextCtrl(self, -1, param['param_str'], wxPoint(5,5),
	                   wxSize(200,150), wxTE_MULTILINE)
	but = wxButton(self, 1001, "Click me", wxPoint(50,160), wxSize(100,30))
	EVT_BUTTON(self, 1001, self.OnButton)
	self.SetSize(wxSize(210,200))
    def OnButton(self, event):
	self.text.AppendText( "Click!\n" )

class Widget2(wxButton):
    def __init__(self, parent, param):
	wxButton.__init__(self, parent, int(param['id']), param['title'])

class Widget3(wxTextCtrl):
    def __init__(self, parent, param):
	wxTextCtrl.__init__(self, parent, -1, "No clicks")
	self.clicked = 0;
	EVT_BUTTON(parent, int(param['button_id']), self.OnButton)
    def OnButton(self, event):
	self.clicked = self.clicked + 1
	self.SetValue("%d clicks" % (self.clicked,))

# make the widgets known in the widget module (aka htmlc)
widget.Widget1 = Widget1
widget.Widget2 = Widget2
widget.Widget3 = Widget3

# our default page
default_page = """
<H2>wxPython widgets go HTML</H2>
A bunch of wxPython widgets are scattered on this HTML page.
Here's one:
<center><python class="Widget1" greeting="Hello World"></center>
<hr>
Here's another:
<center><python class="Widget2" float=70 id=1002 title="Button A"></center>
It should always take up 70% of the page width.
<p>And then there's this, listening to button A:
<python class="Widget3" button_id=1002></p>
"""

# our explanation
apology = """
For some bizarre reason, it takes forever and a day to display the
widgets if they are constructed in the frame's constructor. This
only happens in MSW, wxGTK works fine.
<p>Select <I>File->Show it</I> to draw the widgets."""

default_page = default_page + "The HTML code for this page is\n <pre>" + default_page + "</pre>"


class HtmlViewer(wxFrame):
	def __init__(self, parent, id, title, pos = wxDefaultPosition, size = wxSize(400,400)):
		wxFrame.__init__(self, parent, id, title, pos, size)
                self.CreateStatusBar(1)
                self.html = wxHtmlWindow(self)
                self.html.SetRelatedFrame(self, "HTML Viewer:  \%s")
                self.html.SetRelatedStatusBar(0)
		mbar = wxMenuBar()
		menu = wxMenu()
                menu.Append(1500, "Show it")
		menu.Append(1503, "Exit")
		mbar.Append(menu, "File")
                EVT_MENU(self, 1500, self.OnShowIt)
                EVT_MENU(self, 1503, self.OnClose)
                self.SetMenuBar(mbar)
                # change apology below to default_page, if you dare!
                self.html.SetPage( default_page )
	def OnClose(self,event):
		self.Destroy()
        def OnShowIt(self,event):
                self.html.SetPage( default_page )
                # now quickly remove the menu option, to hide that
                # other bug; namely that widgets aren't removed when the
                # HTML page is.
                self.GetMenuBar().Enable(1500, FALSE)        

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
