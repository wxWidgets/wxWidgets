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

class MyFrame(wxFrame):
    def __init__(self, wf):
	self.wf = wf
	wxFrame.__init__(self, NULL, -1, "test threads", wxDefaultPosition, wxSize(300,200))
	self.panel = wxPanel(self, -1)
	self.panel.SetBackgroundColour(wxNamedColour('white'))
	self.panel.SetFocus()
	menu = wxMenu()
	menu.Append(1001, "New")
	menu.Append(1002, "End")
	menu.AppendSeparator()
	menu.Append(1003, "Exit")
	self.cnt = 0;
	menubar = wxMenuBar()
	menubar.Append(menu, "Game")
	self.SetMenuBar(menubar)
	self.CreateStatusBar(2)
	EVT_MENU(self, 1001, self.OnGameNew)
	EVT_MENU(self, 1002, self.OnGameEnd)
	EVT_MENU(self, 1003, self.OnWindowClose)
	EVT_CHAR(self.panel, self.OnChar)
	self.played = 0
	self.won = 0
	self.history = []
	self.average = 0.0
	self.OnGameNew(None)
    def Draw(self, dc = None):
	if not dc:
	    dc = wxClientDC(self.panel)
	dc.Clear()
	(x,y) = self.panel.GetSizeTuple()
	x1 = x-150; y1 = 20
	for letter in self.word:
	    if self.guess.count(letter):
		dc.DrawText(letter, x1, y1)
	    else:
		dc.DrawText('.', x1, y1)
	    x1 = x1 + 10
	x1 = x-150
	dc.DrawText("played: %d" % (self.played,), x1, 50)
	if self.played:
	    percent = (100.*self.won)/self.played
	else:
	    percent = 0.0
	dc.DrawText("won: %d (%g %%)" % (self.won, percent), x1, 70)
	dc.DrawText("average: %g" % (self.average,), x1, 90)
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
	dc = wxPaintDC(self.panel)
	self.Draw(dc)
    def OnGameNew(self, event):
	self.word = self.wf.Get()
	self.guess = []
	self.tries = 0
	self.misses = 0
	self.in_progress = 1
	self.Draw()
    def OnGameEnd(self, event):
	self.UpdateAverages(0)
	self.misses = 7;
	self.guess = map(chr, range(ord('a'),ord('z')+1))
	self.in_progress = 0
	self.Draw()
    def UpdateAverages(self, has_won):
	if has_won:
	    self.won = self.won + 1
	self.played = self.played+1
	self.history.append(self.misses)
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
	    return
	if self.guess.count(key):
	    self.SetStatusText('Already guessed %s' % (key,),0)
	    return
	self.guess.append(key)
	self.guess.sort()
	guesses = ""
	for letter in self.guess: 
	    guesses = guesses + letter
	self.tries = self.tries+1
	if not key in self.word:
	    self.misses = self.misses+1
	if self.misses == 7:
	    self.SetStatusText("Too bad, you're dead!",0)
	    self.SetStatusText("Press a key to restart",1)
	    self.OnGameEnd(None)
	    return
	has_won = 1
	for letter in self.word:
	    if not self.guess.count(letter):
		has_won = 0
		break
	if has_won:    
	    self.in_progress = 0
	    self.UpdateAverages(has_won)
	    self.SetStatusText("Congratulations!",0)
	    self.SetStatusText("Press a key to restart",1)
	    self.Draw()
	    return
	self.SetStatusText(guesses,1)
	self.SetStatusText("tries %d misses %d" % (self.tries,self.misses),0)
	self.Draw()
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
