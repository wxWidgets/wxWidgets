#!/usr/bin/python
"""This is SlashDot 1.2

    It's the obligatory Slashdot.org headlines reader that
any modern widget set/library must have in order to be taken
seriously :-)

    Usage is quite simple; wxSlash attempts to download the
'ultramode.txt' file from http://slashdot.org, which
contains the headlines in a computer friendly format. It
then displays said headlines in a wxWindows list control.

    You can read articles using either Python's html library
or an external browser. Uncheck the 'browser->internal' menu
item to use the latter option.  Use the settings dialog box
to set which external browser is started.

    This code is available under the wxWindows license, see
elsewhere. If you modify this code, be aware of the fact
that slashdot.org's maintainer, CmdrTaco, explicitly asks
'ultramode.txt' downloaders not to do this automatically
more than twice per hour. If this feature is abused,
CmdrTaco may remove the ultramode file completely and that
will make a *lot* of people unhappy.

    I want to thank Alex Shnitman whose slashes.pl
(Perl/GTK) script gave me the idea for this applet.

    Have fun with it,

    Harm van der Heijden (H.v.d.Heijden@phys.tue.nl)
"""

from wxPython.wx import *
from httplib import HTTP
from htmllib import HTMLParser
import os
import re
import formatter

class HTMLTextView(wxFrame):
    def __init__(self, parent, id, title='HTMLTextView', url=None):
        wxFrame.__init__(self, parent, id, title, wxPyDefaultPosition,
                         wxSize(600,400))

        EVT_CLOSE(self, self.OnCloseWindow)
        self.mainmenu = wxMenuBar()

        menu = wxMenu()
        menu.Append(201, '&Open URL...', 'Open URL')
        EVT_MENU(self, 201, self.OnFileOpen)
        menu.Append(209, 'E&xit', 'Exit viewer')
        EVT_MENU(self, 209, self.OnFileExit)

        self.mainmenu.Append(menu, '&File')
        self.SetMenuBar(self.mainmenu)
        self.CreateStatusBar(1)

        self.text = wxTextCtrl(self, -1, "", wxPyDefaultPosition,
                               wxPyDefaultSize, wxTE_MULTILINE | wxTE_READONLY)

        if (url):
            self.OpenURL(url)

    def logprint(self, x):
        self.SetStatusText(x)

    def OpenURL(self, url):
        self.url = url
        m = re.match('file:(\S+)\s*', url)
        if m:
            f = open(m.groups()[0],'r')
        else:
            m = re.match('http://([^/]+)(/\S*)\s*', url)
            if m:
                host = m.groups()[0]
                path = m.groups()[1]
            else:
                m = re.match('http://(\S+)\s*', url)
                if not m:
                    # Invalid URL
                    self.logprint("Invalid or unsupported URL: %s" % (url))
                    return
                host = m.groups()[0]
                path = ''
            f = RetrieveAsFile(host,path,self.logprint)
        if not f:
            self.logprint("Could not open %s" % (url))
            return
        self.logprint("Receiving data...")
        data = f.read()
        tmp = open('tmphtml.txt','w')
        fmt = formatter.AbstractFormatter(formatter.DumbWriter(tmp))
        p = HTMLParser(fmt)
        self.logprint("Parsing data...")
        p.feed(data)
        p.close()
        tmp.close()
        tmp = open('tmphtml.txt', 'r')
        self.text.SetValue(tmp.read())
        self.SetTitle(url)
        self.logprint(url)

    def OnFileOpen(self, event):
        dlg = wxTextEntryDialog(self, "Enter URL to open:", "")
        if dlg.ShowModal() == wxID_OK:
            url = dlg.GetValue()
        else:
            url = None
        if url:
            self.OpenURL(url)

    def OnFileExit(self, event):
        self.Close()

    def OnCloseWindow(self, event):
        self.Destroy()


def ParseSlashdot(f):
    art_sep = re.compile('%%\r?\n')
    line_sep = re.compile('\r?\n')
    data = f.read()
    list = art_sep.split(data)
    art_list = []
    for i in range(1,len(list)-1):
        art_list.append(line_sep.split(list[i]))
    return art_list

def myprint(x):
    print x

def RetrieveAsFile(host, path='', logprint = myprint):
    try:
        h = HTTP(host)
    except:
        logprint("Failed to create HTTP connection to %s... is the network available?" % (host))
        return None
    h.putrequest('GET',path)
    h.putheader('Accept','text/html')
    h.putheader('Accept','text/plain')
    h.endheaders()
    errcode, errmsg, headers = h.getreply()
    if errcode != 200:
        logprint("HTTP error code %d: %s" % (errcode, errmsg))
        return None
    f = h.getfile()
#    f = open('/home/harm/ultramode.txt','r')
    return f


class AppStatusBar(wxStatusBar):
    def __init__(self, parent):
        wxStatusBar.__init__(self,parent, -1)
        self.SetFieldsCount(2)
        self.SetStatusWidths([-1, 100])
        self.but = wxButton(self, 1001, "Refresh")
        EVT_BUTTON(self, 1001, parent.OnViewRefresh)
        EVT_SIZE(self, self.OnSize)
        self.OnSize(None)

    def logprint(self,x):
        self.SetStatusText(x,0)

    def OnSize(self, event):
        rect = self.GetFieldRect(1)
        self.but.SetPosition(wxPoint(rect.x+2, rect.y+2))
        self.but.SetSize(wxSize(rect.width-4, rect.height-4))

# This is a simple timer class to start a function after a short delay;
class QuickTimer(wxTimer):
    def __init__(self, func, wait=100):
	wxTimer.__init__(self)
	self.callback = func
	self.Start(wait); # wait .1 second (.001 second doesn't work. why?)
    def Notify(self):
	self.Stop();
	apply(self.callback, ());

class AppFrame(wxFrame):
    def __init__(self, parent, id, title):
        wxFrame.__init__(self, parent, id, title, wxPyDefaultPosition,
                         wxSize(650, 250))

	# if the window manager closes the window:
	EVT_CLOSE(self, self.OnCloseWindow);

	# Now Create the menu bar and items
        self.mainmenu = wxMenuBar()

        menu = wxMenu()
        menu.Append(209, 'E&xit', 'Enough of this already!')
        EVT_MENU(self, 209, self.OnFileExit)
        self.mainmenu.Append(menu, '&File')
        menu = wxMenu()
        menu.Append(210, '&Refresh', 'Refresh headlines')
        EVT_MENU(self, 210, self.OnViewRefresh)
        menu.Append(211, '&Slashdot Index', 'View Slashdot index')
        EVT_MENU(self, 211, self.OnViewIndex)
        menu.Append(212, 'Selected &Article', 'View selected article')
        EVT_MENU(self, 212, self.OnViewArticle)
        self.mainmenu.Append(menu, '&View')
        menu = wxMenu()
        menu.Append(220, '&Internal', 'Use internal text browser',TRUE)
        menu.Check(220, true)
        self.UseInternal = 1;
        EVT_MENU(self, 220, self.OnBrowserInternal)
        menu.Append(222, '&Settings...', 'External browser Settings')
        EVT_MENU(self, 222, self.OnBrowserSettings)
        self.mainmenu.Append(menu, '&Browser')
	menu = wxMenu()
	menu.Append(230, '&About', 'Some documentation');
	EVT_MENU(self, 230, self.OnAbout)
	self.mainmenu.Append(menu, '&Help')

        self.SetMenuBar(self.mainmenu)

	if wxPlatform == '__WXGTK__':
	    # I like lynx. Also Netscape 4.5 doesn't react to my cmdline opts
	    self.BrowserSettings = "xterm -e lynx %s &"
	elif wxPlatform == '__WXMSW__':
	    # netscape 4.x likes to hang out here...
	    self.BrowserSettings = '\\progra~1\\Netscape\\Communicator\\Program\\netscape.exe %s'
	else:
	    # a wild guess...
	    self.BrowserSettings = 'netscape %s'

	# A status bar to tell people what's happening
	self.sb = AppStatusBar(self)
        self.SetStatusBar(self.sb)

        self.list = wxListCtrl(self, 1100, style=wxLC_REPORT)
	self.list.InsertColumn(0, 'Subject')
	self.list.InsertColumn(1, 'Date')
	self.list.InsertColumn(2, 'Posted by')
	self.list.InsertColumn(3, 'Comments')
        self.list.SetColumnWidth(0, 300)
        self.list.SetColumnWidth(1, 150)
        self.list.SetColumnWidth(2, 100)
        self.list.SetColumnWidth(3, 100)

        EVT_LIST_ITEM_SELECTED(self, 1100, self.OnItemSelected)
	EVT_LEFT_DCLICK(self.list, self.OnLeftDClick)

	self.logprint("Connecting to slashdot... Please wait.")
	# wxYield doesn't yet work here. That's why we use a timer
	# to make sure that we see some GUI stuff before the slashdot
	# file is transfered.
	self.timer = QuickTimer(self.DoRefresh, 1000)

    def logprint(self, x):
        self.sb.logprint(x)

    def OnFileExit(self, event):
        self.Destroy()

    def DoRefresh(self):
        f = RetrieveAsFile('slashdot.org','/ultramode.txt',self.sb.logprint)
        art_list = ParseSlashdot(f)
        self.list.DeleteAllItems()
        self.url = []
        self.current = -1
        i = 0;
        for article in art_list:
            self.list.InsertStringItem(i, article[0])
            self.list.SetStringItem(i, 1, article[2])
            self.list.SetStringItem(i, 2, article[3])
            self.list.SetStringItem(i, 3, article[6])
            self.url.append(article[1])
            i = i + 1
	self.logprint("File retrieved OK.")

    def OnViewRefresh(self, event):
	self.logprint("Connecting to slashdot... Please wait.");
	wxYield()
	self.DoRefresh()

    def DoViewIndex(self):
        if self.UseInternal:
            self.view = HTMLTextView(self, -1, 'slashdot.org',
                                     'http://slashdot.org')
            self.view.Show(true)
        else:
            self.logprint(self.BrowserSettings % ('http://slashdot.org'))
            #os.system(self.BrowserSettings % ('http://slashdot.org'))
            wxExecute(self.BrowserSettings % ('http://slashdot.org'))
	self.logprint("OK")

    def OnViewIndex(self, event):
	self.logprint("Starting browser... Please wait.")
	wxYield()
	self.DoViewIndex()

    def DoViewArticle(self):
        if self.current<0: return
        url = self.url[self.current]
        if self.UseInternal:
            self.view = HTMLTextView(self, -1, url, url)
            self.view.Show(true)
        else:
            self.logprint(self.BrowserSettings % (url))
            os.system(self.BrowserSettings % (url))
	self.logprint("OK")

    def OnViewArticle(self, event):
	self.logprint("Starting browser... Please wait.")
	wxYield()
	self.DoViewArticle()

    def OnBrowserInternal(self, event):
        if self.mainmenu.Checked(220):
            self.UseInternal = 1
        else:
            self.UseInternal = 0

    def OnBrowserSettings(self, event):
        dlg = wxTextEntryDialog(self, "Enter command to view URL.\nUse %s as a placeholder for the URL.", "", self.BrowserSettings);
        if dlg.ShowModal() == wxID_OK:
            self.BrowserSettings = dlg.GetValue()

    def OnAbout(self, event):
	dlg = wxMessageDialog(self, __doc__, "wxSlash", wxOK | wxICON_INFORMATION)
	dlg.ShowModal()

    def OnItemSelected(self, event):
        self.current = event.m_itemIndex
        self.logprint("URL: %s" % (self.url[self.current]))

    def OnLeftDClick(self, event):
	(x,y) = event.Position();
	# Actually, we should convert x,y to logical coords using
	# a dc, but only for a wxScrolledWindow widget.
	# Now wxGTK derives wxListCtrl from wxScrolledWindow,
	# and wxMSW from wxControl... So that doesn't work.
	#dc = wxClientDC(self.list)
	##self.list.PrepareDC(dc)
	#x = dc.DeviceToLogicalX( event.GetX() )
	#y = dc.DeviceToLogicalY( event.GetY() )
	id = self.list.HitTest(wxPoint(x,y))
	#print "Double click at %d %d" % (x,y), id
	# Okay, we got a double click. Let's assume it's the current selection
	wxYield()
	self.OnViewArticle(event)

    def OnCloseWindow(self, event):
        self.Destroy()


#---------------------------------------------------------------------------
# if running standalone

if __name__ == '__main__':
    class MyApp(wxApp):
        def OnInit(self):
            frame = AppFrame(NULL, -1, "Slashdot Breaking News")
            frame.Show(true)
            self.SetTopWindow(frame)
            return true

    app = MyApp(0)
    app.MainLoop()



#---------------------------------------------------------------------------
# if running as part of the Demo Framework...

def runTest(frame, nb, log):
    win = AppFrame(NULL, -1, "Slashdot Breaking News")
    frame.otherWin = win
    win.Show(true)


overview = __doc__


#----------------------------------------------------------------------------


