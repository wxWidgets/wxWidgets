# Thread testing example. Harm van der Heijden, March 26 1999.
#
# Rule One in threading: make sure only one thread interacts with the
# user interface. See the wxTextCtrlQueue class for an example of how
# to accomplish this

import thread
import time
from   whrandom import random

from wxPython.wx import *

# Set this to zero to prevent entering the wxApp mainloop
# (for testing whether threads work at all in the absense of wxWindows)
use_wxpython = 1

# write a message to stdout every second
def DoThread(mesg):
    while 1:
        sleeptime = (random() * 3) + 0.5
	print "Hello from %s (%1.3f)" % (mesg, sleeptime)
	time.sleep(sleeptime)

# the same, but write it to a textctrl.
def DoTextCtrlThread(text, mesg):
    while 1:
        sleeptime = (random() * 3) + 0.5
	text.WriteText("Hello from %s (%1.3f)\n" % (mesg, sleeptime))
	time.sleep(sleeptime)

# A very simple queue for textctrls.
# Nice demonstration of the power of OO programming too (at least I think so!)
# WriteText puts text in the queue, rather than writing it immediately.
# The main (UI) thread must call Flush to force output. (see MyFrame::OnIdle)
class wxTextCtrlQueue(wxTextCtrl):
    def __init__(self, parent, id, value, pos, size, flags):
	wxTextCtrl.__init__(self,parent, id, value, pos, size, flags)
	self.queue = []
    def WriteText(self, value):
	self.queue.append(value)
    def Flush(self):
	queue = self.queue
	self.queue = []
	for value in queue:
	    wxTextCtrl.WriteText(self,value)

# MyFrame and MyApp are very simple classes to test python threads in
# wxPython.
class MyFrame(wxFrame):
    def __init__(self):
	wxFrame.__init__(self, NULL, -1, "test threads", wxDefaultPosition, wxSize(300,200))
	self.text = wxTextCtrlQueue(self, -1, "thread output\n", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE)
	menu = wxMenu()
	menu.Append(1001, "Start thread")
	self.cnt = 0;
	menubar = wxMenuBar()
	menubar.Append(menu, "Action")
	self.SetMenuBar(menubar)
	EVT_MENU(self, 1001, self.StartThread)
    def StartThread(self, event):
	self.cnt = self.cnt + 1
	thread.start_new_thread(DoTextCtrlThread, (self.text, "thread %d" % self.cnt))
    def OnIdle(self, event):
	self.text.Flush()

class MyApp(wxApp):
    def OnInit(self):
	frame = MyFrame()
	self.SetTopWindow(frame)
	frame.Show(TRUE)
	return TRUE

# Start two threads that print a message every second
thread.start_new_thread(DoThread, ("thread A",))
thread.start_new_thread(DoThread, ("thread B",))

# if using wxpython, open a frame. Otherwise, just hang in while 1
if use_wxpython:
    app = MyApp(0)
    app.MainLoop()
else:
    while 1:
	print "main loop"
	time.sleep(4)
print 'done!'
