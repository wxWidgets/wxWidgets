"""
infoframe.py
Released under wxWindows license etc.

This is a fairly rudimentary, but slightly fancier tha
wxPyOnDemandOutputWindow (on which it's based; thanks Robin), version
of the same sort of thing: a file-like class called
wxInformationalMessagesFrame. This window also has a status bar with a
couple of buttons for controlling the echoing of all output to a file
with a randomly-chosen filename...

The class behaves similarly to wxPyOnDemandOutputWindow in that (at
least by default) the frame does not appear until written to, but is
somewhat different in that, either under programmatic (the
DisableOutput method) or user (the frame's close button, it's status
bar's "Dismiss" button, or a "Disable output" item of some menu,
perhaps of some other frame), the frame will be destroyed, an
associated file closed, and writing to it will then do nothing.  This
can be reversed: either under programmatic (the EnableOutput method)
or user (an "Enable output" item of some menu), a new frame will be
opened,And an associated file (with a "randomly"selected filename)
opened for writing [to which all subsequent displayed messages will be
echoed].

Please note that, like wxPyOnDemandOutputWindow, the instance is not
itself a subclass of wxWindow: when the window is open (and ONLY
then), it's "frame" attribute is the actual instance of wFrame...

Typical usage:
    from wxPython.lib.infoframe import *
    ... # ... modify your wxApp as follows:
    class myApp[wxApp):
        outputWindowClass = wxPyInformationalMessagesFrame
        ...
If you're running on Linux, you'll also have to supply an argument 1 to your
constructor of myApp to redirect stdout/stderr to this window (it's done
automatically for you on Windows).

If you don't want to redirect stdout/stderr, but use the class directly: do
it this way:

 InformationalMessagesFrame = wxPyInformationalMessagesFrame\
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
manually closed by the user, after which it will reappear again until
written to... Also note that all output is echoed to a file with a
randomly-generated name [see the mktemp module in the standard
library], in the directory given as the 'dir' keyword argument to the
InformationalMessagesFrame constructor [which has a default value of
'.'), or set via the method SetOutputDirectory... This file will be
closed with the window--a new one will be created [by default] upon
each subsequent reopening.

Please also note the methods EnableOutput and DisableOutput, and the
possible arguments for the constructor in the code below... (* TO DO:
explain this here...*) Neither of these methods need be used at all,
and in this case the frame will only be displayed once it has been
written to, like wxPyOnDemandOutputWindow.

The former, EnableOutput, displays the frame with an introductory
message, opens a random file to which future displayed output also
goes (unless the nofile attribute is present), and sets the __debug__
variable of each module to 1 (unless the no __debug__ attribute is
present].  This is so that you can say, in any module whatsoever,

    if __debug__:
        InformationalMessagesFrame("... with lots of %<Character> constructs"
                                    % TUPLE)

without worrying about the overhead of evaluating the arguments, and
calling the wxInformationalMessagesFrame instance, in the case where
debugging is not turned on.  (This won't happen if the instance has an
attribute no__debug__; you can arrange this by sub-classing...)

"Debug mode" can also be turned on by selecting the item-"Enable
output" from the "Debug" menu [the default--see the optional arguments
to the SetOtherMenuBar method] of a frame which has been either passed
appropriately to the constructor of the wxInformationalMessagesFrame
(see the code), or set via the SetOtherMenuBar method thereof.  This
also writes an empty string to the instance, meaning that the frame
will open (unless DisablOutput has been called) with an appropriate
introductory message (which will vary according to whether the
instance/class has the "no __debug__" attribute)^  I have found this to
be an extremely useful tool, in lieu of a full wxPython debugger...

Following this, the menu item is also disabled, and an item "Disable
output" (again, by default) is enabled.  Note that these need not be
done: e.g., you don't NEED to have a menu with appropriate items; in
this case simply do not call the SetOtherMenuBar method or use the
othermenubar keyword argument of the class instance constructor.

The DisableOutput method does the reverse of this; it closes the
window (and associated file), and sets the __debug__ variable of each
module whose name begins with a capital letter {this happens to be the
author's personal practice; all my python module start with capital
letters} to 0.  It also enables/disabled the appropriate menu items,
if this was done previously (or SetOtherMenuBar has been called...).
Note too that after a call to DisableOutput, nothing further will be
done upon subsequent write()'s, until the EnableOutput method is
called, either explicitly or by the menu selection above...

Finally, note that the file-like method close() destroys the window
(and closes any associated file) and there is a file-like method
write() which displays it's argument.

All (well, most) of this is made clear by the example code at the end
of this file, which is run if the file is run by itself; otherwise,
see the appropriate "stub" file in the wxPython demo.

"""

from wxPython.wx import *
import string, sys, types, tempfile, os

class _MyStatusBar(wxStatusBar):
    def __init__(self, parent,callbacks=None,useopenbutton=0):
        wxStatusBar.__init__(self, parent, -1, style=wxTAB_TRAVERSAL)
        self.SetFieldsCount(3)

        self.SetStatusText("",0)

        ID = NewId()
        self.button1 = wxButton(self,ID,"Dismiss",
                               style=wxTAB_TRAVERSAL)
        EVT_BUTTON(self,ID,self.OnButton1)

        ID = NewId()
        if not useopenbutton:
            self.button2 = wxButton(self,ID,"Close File",
                                   style=wxTAB_TRAVERSAL)
        else:
            self.button2 = wxButton(self,ID,"Open New File",
                                   style=wxTAB_TRAVERSAL)
        EVT_BUTTON(self,ID,self.OnButton2)
        self.useopenbutton = useopenbutton
        self.callbacks = callbacks

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

    def OnButton1(self,event):
        self.callbacks[0] ()

    def OnButton2(self,event):
        if self.useopenbutton and self.callbacks[2] ():
                self.button2.SetLabel ("Close File")
        elif self.callbacks[1] ():
                self.button2.SetLabel ("Open New File")
        self.useopenbutton = 1 - self.useopenbutton
        self.OnSize("")
        self.button2.Refresh(TRUE)
        self.Refresh()



class wxPyInformationalMessagesFrame:
    def __init__(self,
                 progname="",
                 text="informational messages",
                 dir='.',
                 menuname="Debug",
                 enableitem="Enable output",
                 disableitem="Disable output",
                 othermenubar=None):

        self.SetOtherMenuBar(othermenubar,
                             menuname=menuname,
                             enableitem=enableitem,
                             disableitem=disableitem)

        if hasattr(self,"othermenu") and self.othermenu is not None:
            i = self.othermenu.FindMenuItem(self.menuname,self.disableitem)
            self.othermenu.Enable(i,0)
            i = self.othermenu.FindMenuItem(self.menuname,self.enableitem)
            self.othermenu.Enable(i,1)

        self.frame  = None
        self.title  = "%s %s" % (progname,text)
        self.parent = None # use the SetParent method if desired...
        self.softspace = 1 # of rather limited use
        if dir:
            self.SetOutputDirectory(dir)


    def SetParent(self, parent):
        self.parent = parent


    def SetOtherMenuBar(self,
                        othermenu,
                        menuname="Debug",
                        enableitem="Enable output",
                        disableitem="Disable output"):
        self.othermenu = othermenu
        self.menuname = menuname
        self.enableitem = enableitem
        self.disableitem = disableitem


    f = None


    def write(self,string):
        if not wxThread_IsMain():
            # Aquire the GUI mutex before making GUI calls.  Mutex is released
            # when locker is deleted at the end of this function.
            locker = wxMutexGuiLocker()

        if self.Enabled:
            if self.f:
                self.f.write(string)
                self.f.flush()

            move = 1
            if (hasattr(self,"text")
                and self.text is not None
                and self.text.GetInsertionPoint() != self.text.GetLastPosition()):
                move = 0

            if not self.frame:
                self.frame = wxFrame(self.parent, -1, self.title, size=(450, 300),
                                     style=wxDEFAULT_FRAME_STYLE|wxNO_FULL_REPAINT_ON_RESIZE)
                self.text  = wxTextCtrl(self.frame, -1, "",
                                        style = wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH)

                self.frame.sb = _MyStatusBar(self.frame,
                                             callbacks=[self.DisableOutput,
                                                       self.CloseFile,
                                                       self.OpenNewFile],
                                             useopenbutton=hasattr(self,
                                                                  "nofile"))
                self.frame.SetStatusBar(self.frame.sb)
                self.frame.Show(true)
                EVT_CLOSE(self.frame, self.OnCloseWindow)

                if hasattr(self,"nofile"):
                    self.text.AppendText(
                        "Please close this window (or select the "
                        "'Dismiss' button below) when desired.  By "
                        "default all messages written to this window "
                        "will NOT be written to a file--you "
                        "may change this by selecting 'Open New File' "
                        "below, allowing you to select a "
                        "new file...\n\n")
                else:
                    tempfile.tempdir = self.dir
                    filename = os.path.abspath(tempfile.mktemp ())
                    self.text.AppendText(
                        "Please close this window (or select the "
                        "'Dismiss' button below) when desired.  By "
                        "default all messages written to this window "
                        "will also be written to the file '%s'--you "
                        "may close this file by selecting 'Close "
                        "File' below, whereupon this button will be "
                        "replaced with one allowing you to select a "
                        "new file...\n\n" % filename)
                    try:
                        self.f = open(filename, 'w')
                        self.frame.sb.SetStatusText("File '%s' opened..."
                                                    % filename,
                                                    0)
                    except EnvironmentError:
                        self.frame.sb.SetStatusText("File creation failed "
                                                    "(filename '%s')..."
                                                    % filename,
                                                    0)
            self.text.AppendText(string)

            if move:
                self.text.ShowPosition(self.text.GetLastPosition())

            if  not hasattr(self,"no__debug__"):
                for m in sys.modules.values():
                    if m is  not None:# and m.__dict__.has_key("__debug__"):
                        m.__dict__["__debug__"] = 1

            if hasattr(self,"othermenu") and self.othermenu is not None:
                i = self.othermenu.FindMenuItem(self.menuname,self.disableitem)
                self.othermenu.Enable(i,1)
                i = self.othermenu.FindMenuItem(self.menuname,self.enableitem)
                self.othermenu.Enable(i,0)


    Enabled = 1

    def OnCloseWindow(self, event, exiting=0):
        if self.f:
            self.f.close()
            self.f = None

        if (hasattr(self,"othermenu") and self.othermenu is not None
            and self.frame is not None
            and not exiting):

            i = self.othermenu.FindMenuItem(self.menuname,self.disableitem)
            self.othermenu.Enable(i,0)
            i = self.othermenu.FindMenuItem(self.menuname,self.enableitem)
            self.othermenu.Enable(i,1)

        if  not hasattr(self,"no__debug__"):
            for m in sys.modules.values():
                if m is  not None:# and m.__dict__.has_key("__debug__"):
                    m.__dict__["__debug__"] = 0

        if self.frame is not None: # typically true, but, e.g., allows
                                   # DisableOutput method (which calls this
                                   # one) to be called when the frame is not
                                   # actually open, so that it is always safe
                                   # to call this method...
            frame = self.frame
            self.frame = self.text = None
            frame.Destroy()
        self.Enabled = 1


    def EnableOutput(self,
                     event=None,# event must be the first optional argument...
                     othermenubar=None,
                     menuname="Debug",
                     enableitem="Enable output",
                     disableitem="Disable output"):

        if othermenubar is not None:
            self.SetOtherMenuBar(othermenubar,
                                 menuname=menuname,
                                 enableitem=enableitem,
                                 disableitem=disableitem)
        self.Enabled = 1
        if self.f:
            self.f.close()
            self.f = None
        self.write("")


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
                if hasattr(self,"nofile"):
                    self.frame.sb = _MyStatusBar(self.frame,
                                                callbacks=[self.DisableOutput,
                                                           self.CloseFile,
                                                           self.OpenNewFile])
                    self.frame.SetStatusBar(self.frame.sb)
            if hasattr(self,"nofile"):
                delattr(self,"nofile")
            return 1


    def DisableOutput(self,
                      event=None,# event must be the first optional argument...
                      exiting=0):
        self.write("<InformationalMessagesFrame>.DisableOutput()\n")
        if hasattr(self,"frame") \
           and self.frame is not None:
            self.OnCloseWindow("Dummy",exiting=exiting)
        self.Enabled = 0


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
        self.dir = os.path.abspath(dir)
##        sys.__stderr__.write("Directory: os.path.abspath(%s) = %s\n"
##                             % (dir,self.dir))



class Dummy_wxPyInformationalMessagesFrame:
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

