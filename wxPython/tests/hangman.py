"""Hangman.py, a simple wxPython game, inspired by the 
old bsd game by Ken Arnold.
From the original man page:

 In hangman, the computer picks a word from the on-line 
 word list and you must try to guess it.  The computer 
 keeps track of which letters have been guessed and how 
 many wrong guesses you have made on the screen in a
 graphic fashion. 

That says it all, doesn't it?

Have fun with it,

Harm van der Heijden (H.v.d.Heijden@phys.tue.nl)"""

import random,re,string
from wxPython.wx import *

class WordFetcher:
    def __init__(self, filename, min_length = 5):
	self.min_length = min_length
	print "Trying to open file %s" % (filename,)
	try:
	    f = open(filename, "r")
	except:
	    print "Couldn't open dictionary file %s, using build-ins" % (filename,)
	    self.words = self.builtin_words
	    self.filename = None
	    return
	self.words = f.read()
	self.filename = filename
	print "Got %d bytes." % (len(self.words),)
    def SetMinLength(min_length):
	self.min_length = min_length
    def Get(self):
	reg = re.compile('\s+([a-zA-Z]+)\s+')
	n = 50 # safety valve; maximum number of tries to find a suitable word
	while n:
	    index = int(random.random()*len(self.words))
	    m = reg.search(self.words[index:])
	    if m and len(m.groups()[0]) >= self.min_length: break
	    n = n - 1
	if n: return string.lower(m.groups()[0])
	return "error"
    builtin_words = ' albatros  banana  electrometer  eggshell'

def stdprint(x):
    print x

class URLWordFetcher(WordFetcher):
    def __init__(self, url):
	self.OpenURL(url)
	WordFetcher.__init__(self, "hangman_dict.txt")
    def logprint(self,x):
	print x
    def RetrieveAsFile(self, host, path=''):
	from httplib import HTTP
	try:
	    h = HTTP(host)
	except:
	    self.logprint("Failed to create HTTP connection to %s... is the network available?" % (host))
	    return None
	h.putrequest('GET',path)
	h.putheader('Accept','text/html')
	h.putheader('Accept','text/plain')
	h.endheaders()
	errcode, errmsg, headers = h.getreply()
	if errcode != 200:
	    self.logprint("HTTP error code %d: %s" % (errcode, errmsg))
	    return None
	f = h.getfile()
	return f
    def OpenURL(self,url):
	from htmllib import HTMLParser
	import formatter
	self.url = url
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
	f = self.RetrieveAsFile(host,path)
	if not f:
	    self.logprint("Could not open %s" % (url))
	    return
        self.logprint("Receiving data...")
        data = f.read()
        tmp = open('hangman_dict.txt','w')
        fmt = formatter.AbstractFormatter(formatter.DumbWriter(tmp))
        p = HTMLParser(fmt)
        self.logprint("Parsing data...")
        p.feed(data)
        p.close()
        tmp.close()

class HangmanWnd(wxWindow):
    def __init__(self, parent, id, pos=wxDefaultPosition, size=wxDefaultSize):
	wxWindow.__init__(self, parent, id, pos, size)
	self.SetBackgroundColour(wxNamedColour('white'))
	if wxPlatform == '__WXGTK__':
	    self.font = wxFont(12, wxMODERN, wxNORMAL, wxNORMAL)
	else:
	    self.font = wxFont(10, wxMODERN, wxNORMAL, wxNORMAL)
	self.SetFocus()
    def StartGame(self, word):
	self.word = word
	self.guess = []
	self.tries = 0
	self.misses = 0
	self.Draw()
    def EndGame(self):
	self.misses = 7;
	self.guess = map(chr, range(ord('a'),ord('z')+1))
	self.Draw()
    def HandleKey(self, key):
	self.message = ""
	if self.guess.count(key):
	    self.message = 'Already guessed %s' % (key,)
	    return 0
	self.guess.append(key)
	self.guess.sort()
	self.tries = self.tries+1
	if not key in self.word:
	    self.misses = self.misses+1
	if self.misses == 7:
	    self.EndGame()
	    return 1
	has_won = 1
	for letter in self.word:
	    if not self.guess.count(letter):
		has_won = 0
		break
	if has_won:
	    self.Draw()
	    return 2
	self.Draw()
	return 0
    def Draw(self, dc = None):
	if not dc:
	    dc = wxClientDC(self)
	dc.SetFont(self.font)
	dc.Clear()
	(x,y) = self.GetSizeTuple()
	x1 = x-200; y1 = 20
	for letter in self.word:
	    if self.guess.count(letter):
		dc.DrawText(letter, x1, y1)
	    else:
		dc.DrawText('.', x1, y1)
	    x1 = x1 + 10
	x1 = x-200
	dc.DrawText("tries %d misses %d" % (self.tries,self.misses),x1,50)
	guesses = ""
	for letter in self.guess: 
	    guesses = guesses + letter
	dc.DrawText("guessed:", x1, 70)
	dc.DrawText(guesses[:13], x1+80, 70)
	dc.DrawText(guesses[13:], x1+80, 90)
	dc.SetUserScale(x/1000., y/1000.)
	self.DrawVictim(dc)
    def DrawVictim(self, dc):
	dc.SetPen(wxPen(wxNamedColour('black'), 20))
	dc.DrawLines([(10, 980), (10,900), (700,900), (700,940), (720,940),
		      (720,980), (900,980)])
	dc.DrawLines([(100,900), (100, 100), (300,100)])
	dc.DrawLine(100,200,200,100)
	if ( self.misses == 0 ): return
	dc.SetPen(wxPen(wxNamedColour('blue'), 10))
	dc.DrawLine(300,100,300,200)
	if ( self.misses == 1 ): return
	dc.DrawEllipse(250,200,100,100)
	if ( self.misses == 2 ): return
	dc.DrawLine(300,300,300,600)
	if ( self.misses == 3) : return
	dc.DrawLine(300,300,250,550)
	if ( self.misses == 4) : return
	dc.DrawLine(300,300,350,550)
	if ( self.misses == 5) : return
	dc.DrawLine(300,600,350,850)
	if ( self.misses == 6) : return
	dc.DrawLine(300,600,250,850)
    def OnPaint(self, event):
	dc = wxPaintDC(self)
	self.Draw(dc)

class HangmanDemo(HangmanWnd):
    def __init__(self, wf, parent, id, pos, size):
	HangmanWnd.__init__(self, parent, id, pos, size)
	self.StartGame("dummy")
	self.start_new = 1
	self.wf = wf
	self.delay = 500
	self.timer = self.PlayTimer(self.MakeMove)
    def MakeMove(self):
	self.timer.Stop()
	if self.start_new:
	    self.StartGame(self.wf.Get())
	    self.start_new = 0
	    self.left = list('aaaabcdeeeeefghiiiiijklmnnnoooopqrssssttttuuuuvwxyz')
	else:
	    key = self.left[int(random.random()*len(self.left))]
	    while self.left.count(key): self.left.remove(key)
	    self.start_new = self.HandleKey(key)
	self.timer.Start(self.delay)
    def Stop(self):
	self.timer.Stop()
    class PlayTimer(wxTimer):
	def __init__(self,func):
	    wxTimer.__init__(self)
	    self.func = func
	    self.Start(1000)
	def Notify(self):
	    apply(self.func, ())

class HangmanDemoFrame(wxFrame):
    def __init__(self, wf, parent, id, pos, size):
	wxFrame.__init__(self, parent, id, "Hangman demo", pos, size)
	self.demo = HangmanDemo(wf, self, -1, wxDefaultPosition, wxDefaultSize)
    def OnCloseWindow(self, event):
	self.demo.timer.Stop()
	self.Destroy()

class AboutBox(wxDialog):
    def __init__(self, parent,wf):
	wxDialog.__init__(self, parent, -1, "About Hangman", wxDefaultPosition, wxSize(350,450))
	self.wnd = HangmanDemo(wf, self, -1, wxPoint(1,1), wxSize(350,150))
	self.static = wxStaticText(self, -1, __doc__, wxPoint(1,160), wxSize(350, 250))
	self.button = wxButton(self, 2001, "OK", wxPoint(150,420), wxSize(50,-1))
	EVT_BUTTON(self, 2001, self.OnOK)
    def OnOK(self, event):
	self.wnd.Stop()
	self.EndModal(wxID_OK)
	
class MyFrame(wxFrame):
    def __init__(self, wf):
	self.wf = wf
	wxFrame.__init__(self, NULL, -1, "hangman", wxDefaultPosition, wxSize(400,300))
	self.wnd = HangmanWnd(self, -1)
	menu = wxMenu()
	menu.Append(1001, "New")
	menu.Append(1002, "End")
	menu.AppendSeparator()
	menu.Append(1003, "Reset")
	menu.Append(1004, "Demo...")
	menu.AppendSeparator()
	menu.Append(1005, "Exit")
	menubar = wxMenuBar()
	menubar.Append(menu, "Game")
	menu = wxMenu()
	#menu.Append(1010, "Internal", "Use internal dictionary", TRUE)
	menu.Append(1011, "ASCII File...")
	urls = [ 'wxPython home', 'http://208.240.253.245/wxPython/main.html',
		 'slashdot.org', 'http://slashdot.org/',
		 'cnn.com', 'http://cnn.com',
		 'The New York Times', 'http://www.nytimes.com',
		 'De Volkskrant', 'http://www.volkskrant.nl/frameless/25000006.html',
		 'Gnu GPL', 'http://www.fsf.org/copyleft/gpl.html',
		 'Bijbel: Genesis', 'http://www.coas.com/bijbel/gn1.htm']
	urlmenu = wxMenu()
	for item in range(0,len(urls),2):
	    urlmenu.Append(1020+item/2, urls[item], urls[item+1])
	urlmenu.Append(1080, 'Other...', 'Enter an URL')
	menu.AppendMenu(1012, 'URL', urlmenu, 'Use a webpage')
	menu.Append(1013, 'Dump', 'Write contents to stdout')
	menubar.Append(menu, "Dictionary")
	self.urls = urls
	self.urloffset = 1020
	menu = wxMenu()
	menu.Append(1090, "About...")
	menubar.Append(menu, "Help")
	self.SetMenuBar(menubar)
	self.CreateStatusBar(2)
	EVT_MENU(self, 1001, self.OnGameNew)
	EVT_MENU(self, 1002, self.OnGameEnd)
	EVT_MENU(self, 1003, self.OnGameReset)
	EVT_MENU(self, 1004, self.OnGameDemo)
	EVT_MENU(self, 1005, self.OnWindowClose)
	EVT_MENU(self, 1011, self.OnDictFile)
	EVT_MENU_RANGE(self, 1020, 1020+len(urls)/2, self.OnDictURL)
	EVT_MENU(self, 1080, self.OnDictURLSel)
	EVT_MENU(self, 1013, self.OnDictDump)
	EVT_MENU(self, 1090, self.OnHelpAbout)
	EVT_CHAR(self.wnd, self.OnChar)
	self.OnGameReset()
    def OnGameNew(self, event):
	word = self.wf.Get()
	self.in_progress = 1
	self.SetStatusText("",0)
	self.wnd.StartGame(word)
    def OnGameEnd(self, event):
	self.UpdateAverages(0)
	self.in_progress = 0
	self.SetStatusText("",0)
	self.wnd.EndGame()
    def OnGameReset(self, event=None):
	self.played = 0
	self.won = 0
	self.history = []
	self.average = 0.0
	self.OnGameNew(None)
    def OnGameDemo(self, event):
	frame = HangmanDemoFrame(self.wf, self, -1, wxDefaultPosition, self.GetSize())
	frame.Show(TRUE)
    def OnDictFile(self, event):
	fd = wxFileDialog(self)
	if (self.wf.filename):
	    fd.SetFilename(self.wf.filename)
	if fd.ShowModal() == wxID_OK:
	    file = fd.GetPath()
	    self.wf = WordFetcher(file)
    def OnDictURL(self, event):
	item = (event.GetId() - self.urloffset)*2
	print "Trying to open %s at %s" % (self.urls[item], self.urls[item+1])
	self.wf = URLWordFetcher(self.urls[item+1])
    def OnDictURLSel(self, event):
	msg = wxTextEntryDialog(self, "Enter the URL of the dictionary document", "Enter URL")
	if msg.ShowModal() == wxID_OK:
	    url = msg.GetValue()
	    self.wf = URLWordFetcher(url)
    def OnDictDump(self, event):
	print self.wf.words
    def OnHelpAbout(self, event):
	about = AboutBox(self, self.wf)
	about.ShowModal()
	about.wnd.Stop() # that damn timer won't stop!
    def UpdateAverages(self, has_won):
	if has_won:
	    self.won = self.won + 1
	self.played = self.played+1
	self.history.append(self.wnd.misses) # ugly
	total = 0.0
	for m in self.history:
	    total = total + m
	self.average = float(total/len(self.history))
    def OnChar(self, event):
	if not self.in_progress:
	    self.OnGameNew(None)
	    return
	key = event.KeyCode();
	if key >= ord('A') and key <= ord('Z'):
	    key = key + ord('a') - ord('A')
	key = chr(key)
	if key < 'a' or key > 'z':
	    event.Skip()
	    return
	res = self.wnd.HandleKey(key)
	if res == 0:
	    self.SetStatusText(self.wnd.message)
	elif res == 1:
	    self.UpdateAverages(0)
	    self.SetStatusText("Too bad, you're dead!",0)
	    self.in_progress = 0
        elif res == 2:
	    self.in_progress = 0
	    self.UpdateAverages(1)
	    self.SetStatusText("Congratulations!",0)
	if self.played:
	    percent = (100.*self.won)/self.played
	else:
	    percent = 0.0
	self.SetStatusText("p %d, w %d (%g %%), av %g" % (self.played,self.won, percent, self.average),1)

    def OnWindowClose(self, event):
	self.Destroy()
	
class MyApp(wxApp):
    def OnInit(self):
	if wxPlatform == '__WXGTK__':
	    defaultfile = "/usr/share/games/hangman-words"
	elif wxPlatform == '__WXMSW__':
	    defaultfile = "c:\\windows\\hardware.txt"
	else:
	    defaultfile = ""
	wf = WordFetcher(defaultfile)
	frame = MyFrame(wf)
	self.SetTopWindow(frame)
	frame.Show(TRUE)
	return TRUE

if __name__ == '__main__':
    app = MyApp(0)
    app.MainLoop()
