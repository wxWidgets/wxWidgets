import random
from wxPython.wx import *

class WordFetcher:
    def __init__(self, filename):
	try:
	    f = open(filename, "r")
	except:
	    print "Couldn't open dictionary file %s, using build-ins" % (filename,)
	    self.words = self.builtin_words
	    return
	self.words = []
	while f and len(self.words)<100:
	    line = f.readline()
	    self.words.append(line[0:-1])
	print self.words
    def Get(self):
	return self.words[int(random.random()*len(self.words))]
    builtin_words = [ 'albatros', 'banana', 'electrometer', 'eggshell' ]

class HangmanWnd(wxWindow):
    def __init__(self, parent, id):
	wxWindow.__init__(self, parent, id)
	self.SetBackgroundColour(wxNamedColour('white'))
	self.SetFocus()
	self.frame = parent # ugly
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
	if self.guess.count(key):
	    self.frame.SetStatusText('Already guessed %s' % (key,),0)
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
	dc.DrawText(guesses[:13], x1+70, 70)
	dc.DrawText(guesses[13:], x1+70, 90)
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
	menu.AppendSeparator()
	menu.Append(1004, "Exit")
	menubar = wxMenuBar()
	menubar.Append(menu, "Game")
	menu = wxMenu()
	menu.Append(1010, "Internal", "Use internal dictionary", TRUE)
	menu.Append(1011, "ASCII File...")
	menubar.Append(menu, "Dictionary")
	self.SetMenuBar(menubar)
	self.CreateStatusBar(2)
	EVT_MENU(self, 1001, self.OnGameNew)
	EVT_MENU(self, 1002, self.OnGameEnd)
	EVT_MENU(self, 1003, self.OnGameReset)
	EVT_MENU(self, 1004, self.OnWindowClose)
	EVT_MENU(self, 1011, self.OnDictFile)
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
    def OnDictFile(self, event):
	pass
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
	if res == 1:
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
	print "Reading word list"
	wf = WordFetcher("/usr/share/games/hangman-words")
	frame = MyFrame(wf)
	self.SetTopWindow(frame)
	frame.Show(TRUE)
	return TRUE

if __name__ == '__main__':
    app = MyApp(0)
    app.MainLoop()
