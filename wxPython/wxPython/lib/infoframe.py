"""
infoframe.py
Released under wxWindows license etc.

This is a fairly rudimentary, but slightly fancier tha
wxPyOnDemandOutputWindow (on which it's based; thanks Robin), version
of the same sort of thing: a file-like class called
InformationalMessagesFrame.. This window also has a status bar with a
couple of buttons for controlling the echoing of all output to a file
with a randomly-chosen filename... [[A LITTLE MORE COULD BE SAID
HERE]]

Typical usage:
    from wxPython.lib.infoframe import *
    ... # ... modify your wxApp as follows:
    class myApp[wxApp):
        outputWindowClass = wxInformationalMessagesFrame
        ...
If you're running on Linux, you'll also have to supply an argument 1 to your
constructor of myApp to redirect stdout/stderr to this window (it's done
automatically for you on Windows).

If you don't want to redirect stdout/stderr, but use the class directly: do
it this way:

InformationalMessagesFrame = wxInformationalMessagesFrame\
                                         ([options from progname (default ""),
                                           txt (default "informational
                                                         messages"])
#^^^^ early in the program
...
InformationalMessagesFrame([comma-separated list of items to
                             display.  Note that these will never
                             be separated by spaces as they may
                             be when used in the Python 'print'
                             command])

The latter statement, of course, may be repeated arbitrarily often.
The window will not appear until it is written to, and it may be
manually closed by the user, after which it will not appear again
until written to... Also note that all output is echoed to a file with
a randomly-generated name [see the mktemp module in the standard
library], in the directory given as the 'dir' keyword argument to the
InformationalMessagesFrame constructor [which has a default value of
'.'), or set via the method SetOutputDirectory...

Please also note the methods EnableOutput and DisableOutput, and the
possible arguments for the constructor in the code below... (* TO DO:
explain this here...*) The former, EnableOutput, displays the frame
with an introductory message, opens a random file to which future
displayed output also goes, and sets the __debug__ variable of each
module whose name begins with a capital letter {this happens to be the
author's personal practice; all my python module start with capital
letters} to 1.  This is so that you can say

    if __debug__:
        InformationalMessagesFrame("... with lots of %<Character> constructs"
                                    % TUPLE)

without worrying about a huge about of overhead in the case where
debugging is not turned on.  "Debug mode" can also be turned on by
selecting the item-"Enable debugging output" from the "Debug" menu of
a frame which has been either passed appropriately to the constructor
of the wxInformationalMessagesFrame (see the code), or set via the
SetOtherMenuBar method thereof.  (I have found this to be an extremely
useful tool, in lieu of a full wxPython debugger...)  This menu item
is also disabled, and an item "Disable debugging output" (which calls
the method described in the next paragraph) is enabled.  Note that
these things need not be done: e.g., you don't need to have a "Debug"
menu with appropriate items; in this case simply do not call the
SetOtherMenuBar method or use the othermenubar keyword argument of the
class Instance constructor.

The DisableOutput method does the reverse of this; it closes the
window (and associated file), and sets the __debug__ variable of each
module whose name begins with a capital letter {this happens to be the
author's personal practice; all my python module start with capital
letters} to 0.  It also enables/disabled the appropriate menu items,
if this was done previously (or SetOtherMenuBar has been called...).

Finally, note that the file-like method close() destroys the window
(and any associated file) and there is a file-like method write()
which displays it's argument [actually, it's very similar to
DisableOutput).  Also, class instances are callable as noted above,
displaying successive arguments if this is done.

"""

from wxPython.wx import *
import string, sys, types, tempfile, os

class _MyStatusBar(wxStatusBar):
    def __init__(self, parent,callbacks=None):
        wxStatusBar.__init__(self, parent, -1, style=wxTAB_TRAVERSAL)
        self.SetFieldsCount(3)

        self.SetStatusText("",0)

        ID = NewId()
        self.button1 = wxButton(self,ID,"Dismiss",
                               style=wxTAB_TRAVERSAL)
        EVT_BUTTON(self,ID,callbacks[0])

        ID = NewId()
        self.button2 = wxButton(self,ID,"Close File",
                               style=wxTAB_TRAVERSAL)
        EVT_BUTTON(self,ID,self.OnButton2)
        self.usealternate = 0
        self.callbacks = [callbacks[1],callbacks[2]]

        # figure out how tall to make the status bar
        dc = wxClientDC(self)
        dc.SetFont(self.GetFont())
        (w,h) = dc.GetTextExtent('X')
        h = int(h * 1.8)
        self.SetSize(wxSize(100, h))
        self.OnSize("dummy")
        EVT_SIZE(self,self.OnSize)

    # reposition things...
    def OnSize(self, event):
        self.CalculateSizes()
        rect = self.GetFieldRect(1)
        self.button1.SetPosition(wxPoint(rect.x+5, rect.y+2))
        self.button1.SetSize(wxSize(rect.width-10, rect.height-4))
        rect = self.GetFieldRect(2)
        self.button2.SetPosition(wxPoint(rect.x+5, rect.y+2))
        self.button2.SetSize(wxSize(rect.width-10, rect.height-4))

    # widths........
    def CalculateSizes(self):
        dc = wxClientDC(self.button1)
        dc.SetFont(self.button1.GetFont())
        (w1,h) = dc.GetTextExtent(self.button1.GetLabel())

        dc = wxClientDC(self.button2)
        dc.SetFont(self.button2.GetFont())
        (w2,h) = dc.GetTextExtent(self.button2.GetLabel())

        self.SetStatusWidths([-1,w1+15,w2+15])

    def OnButton2(self,event):
        if self.usealternate:
            if self.callbacks[1] ():
                self.button2.SetLabel ("Close File")
                self.usealternate = 1 - self.usealternate
        else:
            if self.callbacks[0] ():
                self.button2.SetLabel ("Open New File")
                self.usealternate = 1 - self.usealternate
        self.OnSize("")
        self.button2.Refresh(TRUE)
        self.Refresh()

class wxInformationalMessagesFrame:#wxPyOnDemandOutputWindow):
    parent = None

    def SetParent(self, parent):
        self.parent = parent

    def SetOtherMenuBar(self,othermenu):
        self.othermenu = othermenu

    def __init__(self,progname="",text="informational messages",dir=',',
                 othermenubar=None):
        self.othermenu = othermenubar
        self.frame  = None
        self.title  = "%s %s" % (progname,text)
        self.softspace = 1 # of rather limited use
        if dir:
            self.SetOutputDirectory(dir)
        if __debug__:
            self.EnableOutput()
        #wxPyOnDemandOutputWindow.__init__(self,self.title)
        for m in sys.modules.values():
            if m is  not None:# and m.__dict__.has_key("__debug__"):
                m.__dict__["__debug__"] = self.Enabled

    f = None

    def write(self,string):
        if self.Enabled:
            if self.f:
                self.f.write (string)
                self.f.flush ()
            move = 1
            if hasattr(self,"text")\
               and self.text is not None\
               and self.text.GetInsertionPoint()\
               <> self.text.GetLastPosition():
                move = 0
            if not self.frame:
                self.frame = wxFrame(self.parent, -1, self.title)
                self.text  = wxTextCtrl(self.frame, -1, "",
                                        style = wxTE_MULTILINE|wxTE_READONLY
                                        |wxTE_RICH)# appears to cause problem?
                self.frame.sb = _MyStatusBar(self.frame,
                                            callbacks=[self.DisableOutput,
                                                       self.CloseFile,
                                                       self.OpenNewFile])
                self.frame.SetStatusBar(self.frame.sb)
                self.frame.SetSize(wxSize(450, 300))
                self.frame.Show(true)
                EVT_CLOSE(self.frame, self.OnCloseWindow)
            self.text.AppendText(string)
##            if __debug__ and type(sys.__stderr__) == types.FileType\
##               and sys.__stderr__.isatty():
##                sys.__stderr__.write(
##                    "%s.write(): self.text.GetInsertionPoint() = %s, "\
##                    "self.text.GetLastPosition() = %s, "\
##                    "move = %d\n" % (self,
##                                     self.text.GetInsertionPoint(),
##                                     self.text.GetLastPosition(),
##                                     move))
            if move:
                self.text.ShowPosition(self.text.GetLastPosition())

    Enabled = __debug__

    def OnCloseWindow(self,event,exiting=0):
        if self.f:
            self.f.close()
            self.f = None
        if hasattr(self,"othermenu") and self.othermenu is not None\
           and self.frame is not None\
           and not exiting:
            i = self.othermenu.FindMenuItem('Debug','Disable debugging output')
            self.othermenu.Enable(i,0)
            i = self.othermenu.FindMenuItem('Debug','Enable debugging output')
            self.othermenu.Enable(i,1)
        for m in sys.modules.values():
            if m is  not None:# and m.__dict__.has_key("__debug__"):
                m.__dict__["__debug__"] = 0
        if self.frame is not None: # should be true, but, e.g., allows
                                   # DisableOutput method (which calls this
                                   # one) to be called when the frame is not
                                   # actually open, so that it is always safe
                                   # to call this method...
            frame = self.frame
            self.frame = self.text = None
            frame.Destroy()
        self.Enabled = 0

    def EnableOutput(self,othermenubar=None):
        if othermenubar is not None:
            self.othermenu = othermenubar
        self.Enabled = 1
        for m in sys.modules.values():
            if m is  not None:# and m.__dict__.has_key("__debug__"):
                m.__dict__["__debug__"] = 1
        if hasattr(self,"othermenu") and self.othermenu is not None:
            i = self.othermenu.FindMenuItem('Debug','Disable debugging output')
            self.othermenu.Enable(i,1)
            i = self.othermenu.FindMenuItem('Debug','Enable debugging output')
            self.othermenu.Enable(i,0)
        if not self.f:
            try:
                filename = tempfile.mktemp ()
                self.write("Please close this window (or select the "
                           "'Dismiss' button below) when desired.  By "
                           "default all messages written to this window "
                           "will also be written to the file '%s'--you "
                           "may close this file by selecting 'Close "
                           "File' below, whereupon this button will be "
                           "replaced with one allowing you to select a "
                           "new file...\n\n" % os.path.abspath(filename))
                self.f = open (filename,'w')
                self.frame.sb.SetStatusText("File '%s' opened..."
                                            % os.path.abspath(self.f.name),
                                            0)
            except EnvironmentError:
                self.frame.sb.SetStatusText("File creation failed (filename "
                                            "'%s')..."
                                            % os.path.abspath(filename),
                                            0)

    def CloseFile(self):
        if self.f:
            if self.frame:
                self.frame.sb.SetStatusText("File '%s' closed..."
                                            % os.path.abspath(self.f.name),
                                            0)
            self.f.close ()
            self.f = None
        else:
            if self.frame:
                self.frame.sb.SetStatusText("")
        if self.frame:
            self.frame.sb.Refresh()
        return 1

    def OpenNewFile(self):
        self.CloseFile()
        dlg = wxFileDialog(self.frame,
                          "Choose a new log file", self.dir,"","*",
                           wxSAVE | wxHIDE_READONLY | wxOVERWRITE_PROMPT)
        if dlg.ShowModal() == wxID_CANCEL:
            dlg.Destroy()
            return 0
        else:
            try:
                self.f = open(os.path.abspath(dlg.GetPath()),'w')
            except EnvironmentError:
                dlg.Destroy()
                return 0
            dlg.Destroy()
            if self.frame:
                self.frame.sb.SetStatusText("File '%s' opened..."
                                            % os.path.abspath(self.f.name),
                                            0)
            return 1

    def DisableOutput(self,exiting=0):
        self.write("<InformationalMessagesFrame>.DisableOutput()\n")
        self.CloseFile()
        self.Enabled = 0
        if hasattr(self,"othermenu") and self.othermenu is not None:
            i = self.othermenu.FindMenuItem('Debug','Disable debugging output')
            self.othermenu.Enable(i,0)
            i = self.othermenu.FindMenuItem('Debug','Enable debugging output')
            self.othermenu.Enable(i,1)
        if hasattr(self,"frame") \
           and self.frame is not None:
            self.OnCloseWindow("Dummy",exiting=exiting)

    def close(self):
        self.DisableOutput()

    def flush(self):
        if self.text:
            self.text.SetInsertionPointEnd()
        wxYield()

    def __call__(self,* args):
        for s in args:
            self.write (str (s))

    def SetOutputDirectory(self,dir):
        self.dir = tempfile.tempdir = dir

class DummyFile:
    def __init__(self,progname=""):
        self.softspace = 1
    def __call__(self,*args):
        pass
    def write(self,s):
        pass
    def flush(self):
        pass
    def close(self):
        pass
    def EnableOutput(self):
        pass
    def __call__(self,* args):
        pass
    def DisableOutput(self,exiting=0):
        pass
    def SetParent(self,wX):
        pass

if __name__ == "__main__":
    __debug__ = 1

    ImportErrors = 0
    try:
        import Errors
        importErrors = 1
    except ImportError:
        pass

    class MyFrame(wxFrame):
        def __init__(self):
            wxFrame.__init__(self,None,-1,"Close me...",size=(300,10))
            EVT_CLOSE(self,self.OnClose)

        def OnClose(self,event):
            if isinstance(sys.stdout,wxInformationalMessagesFrame):
                sys.stdout.close()# shouldn't be necessary?
            self.Destroy()

    class MyApp(wxApp):
        outputWindowClass = wxInformationalMessagesFrame
        def OnInit(self):
            if ImportErrors:
                sys.stderr = Errors.NonWindowingErrorWindow(
                    file=self.stdioWin)
            print "Starting.\n",
            frame = MyFrame()
            frame.Show(TRUE)
            self.SetTopWindow(frame)
            if isinstance(sys.stdout,wxInformationalMessagesFrame):
                sys.stdout.SetParent(frame)#  Shouldn't this mean the
                #wxInternationalMessagesFrame is Destroy()'d when MFrame is?
            return true

    app = MyApp()
    app.MainLoop()
