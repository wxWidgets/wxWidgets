#----------------------------------------------------------------------
# Name:        wxPython.lib.pyshell
# Purpose:     A Python Interactive Interpreter running in a wxStyledTextCtrl
#              window.
#
# Author:      Robin Dunn
#
# Created:     7-July-2000
# RCS-ID:      $Id$
# Copyright:   (c) 2000 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

"""
PyShellWindow is a class that provides an Interactive Interpreter running
inside a wxStyledTextCtrl, similar to the Python shell windows found in
IDLE and PythonWin.

There is still much to be done to improve this class, such as line
buffering/recall, colourizing the python code, autoindent, etc. but it's
a good start.

"""


from wxPython.wx  import *
from wxPython.stc import *

import sys, string, keyword
from code import InteractiveInterpreter

#----------------------------------------------------------------------
# default styles, etc. to use for the STC

_default_properties = {
    'marginWidth' : 5,
    'ps1'         : '>>> ',
    'stdout'      : 'fore:#0000FF',
    'stderr'      : 'fore:#007f00',
    'trace'       : 'fore:#FF0000',

    # TODO:  Add properties for the various Python lexer styles

    }


# style numbers
_stdout_style = 15
_stderr_style = 16
_trace_style = 17


#----------------------------------------------------------------------

class PyShellWindow(wxStyledTextCtrl, InteractiveInterpreter):
    def __init__(self, parent, ID, pos=wxDefaultPosition,
                 size=wxDefaultSize, style=0,
                 locals=None, properties=None, banner=None):
        wxStyledTextCtrl.__init__(self, parent, ID, pos, size, style)
        InteractiveInterpreter.__init__(self, locals)

        # the line cache is used to cycle through previous commands
        self.lines = []
        self.lastUsedLine = self.curLine = 0

        # set defaults and then deal with any user defined properties
        self.props = {}
        self.props.update(_default_properties)
        if properties:
            self.props.update(properties)
        self.UpdateProperties()

        # copyright/banner message
        if banner is None:
            self.write("Python %s on %s\n%s\n(%s)\n" %
                       (sys.version, sys.platform, sys.copyright,
                        self.__class__.__name__))
        else:
            self.write("%s\n" % banner)

        # write the initial prompt
        self.Prompt()

        # Event handlers
        EVT_KEY_DOWN(self, self.OnKey)



    def GetLocals(self): return self.locals
    def SetLocals(self, locals): self.locals = locals

    def GetProperties(self): return self.props
    def SetProperties(self, properties):
        self.props.update(properties)
        self.UpdateProperties()


    def UpdateProperties(self):
        """
        Reset the editor and other settings based on the contents of the
        current properties dictionary.
        """
        p = self.props

        self.SetLexer(wxSTC_LEX_PYTHON)
        self.SetKeywords(0, string.join(keyword.kwlist))

        # set the selection margin
        self.SetMarginWidth(1, p['marginWidth'])

        # styles
        self.StyleSetSpec(_stdout_style, p['stdout'])
        self.StyleSetSpec(_stderr_style, p['stderr'])
        self.StyleSetSpec(_trace_style, p['trace'])



    # used for writing to stdout, etc.
    def _write(self, text, style=_stdout_style):
        pos = self.GetCurrentPos()
        self.AddText(text)
        self.StartStyling(pos, 0xFF)
        self.SetStyleFor(len(text), style)
        self.EnsureCaretVisible()
        wxYield()

    write = _write

    def writeTrace(self, text):
        self._write(text, _trace_style)


    def Prompt(self):
        # is the current line non-empty?
        text, pos = self.GetCurrentLineText()
        if pos != 0:
            self.AddText('\n')
        self.AddText(self.props['ps1'])
        self.lastPromptPos = self.GetCurrentPos()
        self.EnsureCaretVisible()
        self.ScrollToColumn(0)


    def PushLine(self, text):
        # TODO:  Add the text to the line cache, manage the cache so
        #        it doesn't get too big.
        pass



    def OnKey(self, evt):
        key = evt.KeyCode()
        if key == WXK_RETURN:
            pos = self.GetCurrentPos()
            lastPos = self.GetTextLength()

            # if not on the last line, duplicate the current line
            if self.GetLineCount()-1 !=  self.GetCurrentLine():
                text, col = self.GetCurrentLineText()
                prompt = self.props['ps1']
                lp = len(prompt)
                if text[:lp] == prompt:
                    text = text[lp:]

                self.SetSelection(self.lastPromptPos, lastPos)
                self.ReplaceSelection(text[:-1])

            else:  # try to execute the text from the prompt to the end
                if lastPos == self.lastPromptPos:
                    self.AddText('\n')
                    self.Prompt()
                    return

                text = self.GetTextRange(self.lastPromptPos, lastPos)
                self.AddText('\n')

                more = self.runsource(text)
                if not more:
                    self.PushLine(text)
                    self.Prompt()

        # TODO:  Add handlers for Alt-P and Alt-N to cycle through entries
        #        in the line cache

        else:
            evt.Skip()


    # overloaded methods from InteractiveInterpreter
    def runsource(self, source):
        stdout, stderr = sys.stdout, sys.stderr
        sys.stdout = FauxFile(self, _stdout_style)
        sys.stderr = FauxFile(self, _stderr_style)

        more = InteractiveInterpreter.runsource(self, source)

        sys.stdout, sys.stderr = stdout, stderr
        return more

    def showsyntaxerror(self, filename=None):
        self.write = self.writeTrace
        InteractiveInterpreter.showsyntaxerror(self, filename)
        self.write = self._write

    def showtraceback(self):
        self.write = self.writeTrace
        InteractiveInterpreter.showtraceback(self)
        self.write = self._write

#----------------------------------------------------------------------

class FauxFile:
    def __init__(self, psw, style):
        self.psw = psw
        self.style = style

    def write(self, text):
        self.psw.write(text, self.style)

    def writelines(self, lst):
        map(self.write, lst)

    def flush(self):
        pass


#----------------------------------------------------------------------
# test code

if __name__ == '__main__':
    app = wxPyWidgetTester(size = (640, 480))
    app.SetWidget(PyShellWindow, -1)
    app.MainLoop()


#----------------------------------------------------------------------


