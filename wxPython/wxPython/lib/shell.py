# shell.py
"""wxPython interactive shell

Copyright (c) 1999 SIA "ANK"

this module is free software.  it may be used under same terms as Python itself

Notes:
i would like to use command completion (see rlcompleter library module),
but i cannot load it because i don't have readline...

History:
03-oct-1999 [als] created
04-oct-1999 [als] PyShellOutput.intro moved from __init__ parameters
                  to class attributes; html debug disabled
04-oct-1999 [als] fixed bug with class attributes
                  input prompts and output styles added to customized demo
                  some html cleanups
04-oct-1999 [rpd] Changed to use the new sizers
05-oct-1990 [als] changes inspired by code.InteractiveInterpreter()
                  from Python Library.  if i knew about this class earlier,
                  i would rather inherit from it.
                  renamed to wxPyShell.py since i've renounced the 8.3 scheme

"""
__version__ ="$Revision$"
# $RCSfile$

import sys, string, code, traceback
from wxPython.wx import *
from wxPython.html import *


class PyShellInput(wxPanel):
    """PyShell input window

    """
    PS1 =" Enter Command:"
    PS2 ="... continue:"
    def __init__(self, parent, shell, id=-1):
        """Create input window

        shell must be a PyShell object.
        it is used for exception handling, eval() namespaces,
        and shell.output is used for output
        (print's go to overridden stdout)
        """
        wxPanel.__init__(self, parent, id)
        self.shell =shell
        # make a private copy of class attrs
        self.PS1 =PyShellInput.PS1
        self.PS2 =PyShellInput.PS2
        # create controls
        self.label =wxStaticText(self, -1, self.PS1)
        tid =wxNewId()
        self.entry =wxTextCtrl(self, tid, style = wxTE_MULTILINE)
        EVT_CHAR(self.entry, self.OnChar)
        self.entry.SetFont(wxFont(9, wxMODERN, wxNORMAL, wxNORMAL, false))
        sizer =wxBoxSizer(wxVERTICAL)
        sizer.AddMany([(self.label, 0, wxEXPAND), (self.entry, 1, wxEXPAND)])
        self.SetSizer(sizer)
        self.SetAutoLayout(true)
        EVT_SET_FOCUS(self, self.OnSetFocus)
        # when in "continuation" mode,
        # two consecutive newlines are required
        # to avoid execution of unfinished block
        self.first_line =1

    def OnSetFocus(self, event):
        self.entry.SetFocus()


    def Clear(self, event=None):
        """reset input state"""
        self.label.SetLabel(self.PS1)
        self.label.Refresh()
        self.entry.SetSelection(0, self.entry.GetLastPosition())
        self.first_line =1
        # self.entry.SetFocus()

    def OnChar(self, event):
        """called on CHARevent.  executes input on newline"""
        # print "On Char:", event.__dict__.keys()
        if event.KeyCode() !=WXK_RETURN:
            # not of our business
            event.Skip()
            return
        text =self.entry.GetValue()
        # weird CRLF thingy
        text =string.replace(text, "\r\n", "\n")
        # see if we've finished
        if (not (self.first_line or text[-1] =="\n")  # in continuation mode
            or (text[-1] =="\\")  # escaped newline
          ):
            # XXX should escaped newline put myself i "continuation" mode?
            event.Skip()
            return
        # ok, we can try to execute this
        rc =self.shell.TryExec(text)
        if rc:
            # code is incomplete; continue input
            if self.first_line:
                self.label.SetLabel(self.PS2)
                self.label.Refresh()
                self.first_line =0
            event.Skip()
        else:
            self.Clear()

class PyShellOutput(wxPanel):
    """PyShell output window

    for now, it is based on simple wxTextCtrl,
    but i'm looking at HTML classes to provide colorized output
    """
    # attributes for for different (input, output, exception) display styles:
    # begin tag, end tag, newline
    in_style =(" <font color=\"#000080\"><tt>&gt;&gt;&gt;&nbsp;",
               "</tt></font><br>\n", "<br>\n...&nbsp;")
    out_style =("<tt>", "</tt>\n", "<br>\n")
    exc_style =("<font color=\"#FF0000\"><tt>",
                "</tt></font>\n", "<br>\n")
    intro ="<H3>wxPython Interactive Shell</H3>\n"
    html_debug =0
    # entity references
    erefs =(("&", "&amp;"), (">", "&gt;"), ("<", "&lt;"), ("  ", "&nbsp; "))
    def __init__(self, parent, id=-1):
        wxPanel.__init__(self, parent, id)
        # make a private copy of class attrs
        self.in_style =PyShellOutput.in_style
        self.out_style =PyShellOutput.out_style
        self.exc_style =PyShellOutput.exc_style
        self.intro =PyShellOutput.intro
        self.html_debug =PyShellOutput.html_debug
        # create windows
        if self.html_debug:
            # this was used in html debugging,
            # but i don't want to delete it; it's funny
            splitter =wxSplitterWindow(self, -1)
            self.view =wxTextCtrl(splitter, -1,
                       style = wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL)
            self.html =wxHtmlWindow(splitter)
            splitter.SplitVertically(self.view, self.html)
            splitter.SetSashPosition(40)
            splitter.SetMinimumPaneSize(3)
            self.client =splitter
        else:
            self.view =None
            self.html =wxHtmlWindow(self)
            self.client =self.html  # used in OnSize()
        self.text =self.intro
        self.html.SetPage(self.text)
        self.html.SetAutoLayout(TRUE)
        self.line_buffer =""
        # refreshes are annoying
        self.in_batch =0
        self.dirty =0


    def OnSize(self, event):
        self.client.SetSize(self.GetClientSize())

    def OnIdle(self, event):
        """when there's nothing to do, we can update display"""
        if self.in_batch and self.dirty: self.UpdWindow()

    def BeginBatch(self):
        """do not refresh display till EndBatch()"""
        self.in_batch =1

    def EndBatch(self):
        """end batch; start updating display immediately"""
        self.in_batch =0
        if self.dirty: self.UpdWindow()

    def UpdWindow(self):
        """sync display with text buffer"""
        html =self.html
        html.SetPage(self.text)
        self.dirty =0
        # scroll to the end
        (x,y) =html.GetVirtualSize()
        html.Scroll(0, y)

    def AddText(self, text, style=None):
        """write text to output window"""
        # a trick needed to defer default from compile-time to execute-time
        if style ==None: style =self.out_style
        if 0 and __debug__: sys.__stdout__.write(text)
        # handle entities
        for (symbol, eref) in self.erefs:
            text =string.replace(text, symbol, eref)
        # replace newlines
        text =string.replace(text, "\n", style[2])
        # add to contents
        self.text =self.text +style[0] +text +style[1]
        if not self.in_batch: self.UpdWindow()
        else: self.dirty =1
        if self.html_debug:
            # html debug output needn't to be too large
            self.view.SetValue(self.text[-4096:])

    def write(self, str, style=None):
        """stdout-like interface"""
        if style ==None: style =self.out_style
        # do not process incomplete lines
        if len(str) <1:
            # hm... what was i supposed to do?
            return
        elif str[-1] !="\n":
            self.line_buffer =self.line_buffer +str
        else:
            self.AddText(self.line_buffer +str, style)
            self.line_buffer =""

    def flush(self, style=None):
        """write out all that was left in line buffer"""
        if style ==None: style =self.out_style
        self.AddText(self.line_buffer +"\n", style)

    def write_in(self, str, style=None):
        """write text in "input" style"""
        if style ==None: style =self.in_style
        self.AddText(str, style)

    def write_exc(self, str, style=None):
        """write text in "exception" style"""
        if style ==None: style =self.exc_style
        self.AddText(str, style)

class PyShell(wxPanel):
    """interactive Python shell with wxPython interface

    """
    def __init__(self, parent, globals=globals(), locals={},
                 id=-1, pos=wxDefaultPosition, size=wxDefaultSize,
                 style=wxTAB_TRAVERSAL, name="shell"):
        """create PyShell window"""
        wxPanel.__init__(self, parent, id, pos, size, style, name)
        self.globals =globals
        self.locals =locals
        splitter =wxSplitterWindow(self, -1)
        self.output =PyShellOutput(splitter)
        self.input =PyShellInput(splitter, self)
        self.input.SetFocus()
        splitter.SplitHorizontally(self.input, self.output)
        splitter.SetSashPosition(100)
        splitter.SetMinimumPaneSize(20)
        self.splitter =splitter
        EVT_SET_FOCUS(self, self.OnSetFocus)

    def OnSetFocus(self, event):
        self.input.SetFocus()

    def TryExec(self, source, symbol="single"):
        """Compile and run some source in the interpreter.

        borrowed from code.InteractiveInterpreter().runsource()
        as i said above, i would rather like to inherit from that class

        returns 1 if more input is required, or 0, otherwise
        """
        try:
            cc = code.compile_command(source, symbol=symbol)
        except (OverflowError, SyntaxError):
            # [als] hm... never seen anything of that kind
            self.ShowSyntaxError()
            return 0
        if cc is None:
            # source is incomplete
            return 1
        # source is sucessfully compiled
        out =self.output
        # redirect system stdout to the output window
        prev_out =sys.stdout
        sys.stdout =out
        # begin printout batch (html updates are deferred until EndBatch())
        out.BeginBatch()
        out.write_in(source)
        try:
            exec cc in self.globals, self.locals
        except SystemExit:
            # SystemExit is not handled and has to be re-raised
            raise
        except:
            # all other exceptions produce traceback output
            self.ShowException()
        # switch back to saved stdout
        sys.stdout =prev_out
        # commit printout
        out.flush()
        out.EndBatch()
        return 0

    def ShowException(self):
        """display the traceback for the latest exception"""
        (etype, value, tb) =sys.exc_info()
        # remove myself from traceback
        tblist =traceback.extract_tb(tb)[1:]
        msg =string.join(traceback.format_exception_only(etype, value)
                        +traceback.format_list(tblist))
        self.output.write_exc(msg)

    def ShowSyntaxError(self):
        """display message about syntax error (no traceback here)"""
        (etype, value, tb) =sys.exc_info()
        msg =string.join(traceback.format_exception_only(etype, value))
        self.output.write_exc(msg)

    def OnSize(self, event):
        self.splitter.SetSize(self.GetClientSize())

#----------------------------------------------------------------------
if __name__ == '__main__':
    class MyFrame(wxFrame):
        """Very standard Frame class. Nothing special here!"""
        def __init__(self, parent=NULL, id =-1,
                     title="wxPython Interactive Shell"):
            wxFrame.__init__(self, parent, id, title)
            self.shell =PyShell(self)

    class MyApp(wxApp):
        """Demonstrates usage of both default and customized shells"""
        def OnInit(self):
            frame = MyFrame()
            frame.Show(TRUE)
            self.SetTopWindow(frame)
##             PyShellInput.PS1 =" let's get some work done..."
##             PyShellInput.PS2 =" ok, what do you really mean?"
##             PyShellOutput.in_style =(
##                 "<I><font color=\"#008000\"><tt>&gt;&gt;&gt;&nbsp;",
##                 "</tt></font></I><br>\n", "<br>\n...&nbsp;")
##             PyShellOutput.out_style =(
##                 "<font color=\"#000080\"><tt>",
##                 "</tt></font><br>\n", "<br>\n")
##             PyShellOutput.exc_style =("<B><font color=\"#FF0000\"><tt>",
##                 "</tt></font></B>\n", "<br>\n")
##             PyShellOutput.intro ="<I><B>Customized wxPython Shell</B>" \
##                 "<br>&lt;-- move this sash to see html debug output</I><br>\n"
##             PyShellOutput.html_debug =1
##             frame = MyFrame(title="Customized wxPython Shell")
##             frame.Show(TRUE)
            return TRUE

    app = MyApp(0)
    app.MainLoop()

