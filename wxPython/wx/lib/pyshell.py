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
# 12/10/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 compatability update.
# o Added deprecation warning.
#

"""
PyShellWindow is a class that provides an Interactive Interpreter running
inside a wxStyledTextCtrl, similar to the Python shell windows found in
IDLE and PythonWin.

There is still much to be done to improve this class, such as line
buffering/recall, autoindent, calltips, autocomplete, fixing the colourizer,
etc...  But it's a good start.


8-10-2001         THIS MODULE IS NOW DEPRECATED.  Please see the most excellent
                  PyCrust package instead.

"""

import  keyword
import  sys
import  warnings

from code import InteractiveInterpreter

import  wx
import  wx.stc as stc

warningmsg = r"""\

########################################\
# THIS MODULE IS NOW DEPRECATED         |
#                                       |
# Please see the most excellent PyCrust |
# package instead.                      |
########################################/

"""

warnings.warn(warningmsg, DeprecationWarning, stacklevel=2)

#----------------------------------------------------------------------
# default styles, etc. to use for the STC

if wx.Platform == '__WXMSW__':
    _defaultSize = 8
else:
    _defaultSize = 10


_default_properties = {
    'selMargin'   : 0,
    'marginWidth' : 1,
    'ps1'         : '>>> ',
    'stdout'      : 'fore:#0000FF',
    'stderr'      : 'fore:#007f00',
    'trace'       : 'fore:#FF0000',

    'default'     : 'size:%d' % _defaultSize,
    'bracegood'   : 'fore:#FFFFFF,back:#0000FF,bold',
    'bracebad'    : 'fore:#000000,back:#FF0000,bold',

    # properties for the various Python lexer styles
    'comment'     : 'fore:#007F00',
    'number'      : 'fore:#007F7F',
    'string'      : 'fore:#7F007F,italic',
    'char'        : 'fore:#7F007F,italic',
    'keyword'     : 'fore:#00007F,bold',
    'triple'      : 'fore:#7F0000',
    'tripledouble': 'fore:#7F0000',
    'class'       : 'fore:#0000FF,bold,underline',
    'def'         : 'fore:#007F7F,bold',
    'operator'    : 'bold',

    }


# new style numbers
_stdout_style = 15
_stderr_style = 16
_trace_style = 17


#----------------------------------------------------------------------

class PyShellWindow(stc.StyledTextCtrl, InteractiveInterpreter):
    def __init__(self, parent, ID, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=0,
                 locals=None, properties=None, banner=None):
        stc.StyledTextCtrl.__init__(self, parent, ID, pos, size, style)
        InteractiveInterpreter.__init__(self, locals)

        self.lastPromptPos = 0

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
            self.write("Python %s on %s\n" % #%s\n(%s)\n" %
                       (sys.version, sys.platform,
                        #sys.copyright, self.__class__.__name__
                        ))
        else:
            self.write("%s\n" % banner)

        # write the initial prompt
        self.Prompt()

        # Event handlers
        self.Bind(wx.EVT_KEY_DOWN, self.OnKey)
        self.Bind(stc.EVT_STC_UPDATEUI, self.OnUpdateUI, id=ID)
        #self.Bind(stc.EVT_STC_STYLENEEDED, self.OnStyle, id=ID)


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

        #self.SetEdgeMode(stc.STC_EDGE_LINE)
        #self.SetEdgeColumn(80)


        # set the selection margin and window margin
        self.SetMarginWidth(1, p['selMargin'])
        self.SetMargins(p['marginWidth'], p['marginWidth'])

        # styles
        self.StyleSetSpec(stc.STC_STYLE_DEFAULT, p['default'])
        self.StyleClearAll()
        self.StyleSetSpec(_stdout_style, p['stdout'])
        self.StyleSetSpec(_stderr_style, p['stderr'])
        self.StyleSetSpec(_trace_style, p['trace'])

        self.StyleSetSpec(stc.STC_STYLE_BRACELIGHT, p['bracegood'])
        self.StyleSetSpec(stc.STC_STYLE_BRACEBAD, p['bracebad'])
        self.StyleSetSpec(stc.STC_P_COMMENTLINE, p['comment'])
        self.StyleSetSpec(stc.STC_P_NUMBER, p['number'])
        self.StyleSetSpec(stc.STC_P_STRING, p['string'])
        self.StyleSetSpec(stc.STC_P_CHARACTER, p['char'])
        self.StyleSetSpec(stc.STC_P_WORD, p['keyword'])
        self.StyleSetSpec(stc.STC_P_TRIPLE, p['triple'])
        self.StyleSetSpec(stc.STC_P_TRIPLEDOUBLE, p['tripledouble'])
        self.StyleSetSpec(stc.STC_P_CLASSNAME, p['class'])
        self.StyleSetSpec(stc.STC_P_DEFNAME, p['def'])
        self.StyleSetSpec(stc.STC_P_OPERATOR, p['operator'])
        self.StyleSetSpec(stc.STC_P_COMMENTBLOCK, p['comment'])


    # used for writing to stdout, etc.
    def _write(self, text, style=_stdout_style):
        self.lastPromptPos = 0
        pos = self.GetCurrentPos()
        self.AddText(text)
        self.StartStyling(pos, 0xFF)
        self.SetStyling(len(text), style)
        self.EnsureCaretVisible()
        wx.Yield()

    write = _write

    def writeTrace(self, text):
        self._write(text, _trace_style)


    def Prompt(self):
        # is the current line non-empty?
        text, pos = self.GetCurLine()
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
        key = evt.GetKeyCode()
        if key == wx.WXK_RETURN:
            pos = self.GetCurrentPos()
            lastPos = self.GetTextLength()

            # if not on the last line, duplicate the current line
            if self.GetLineCount()-1 !=  self.GetCurrentLine():
                text, col = self.GetCurLine()
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


    def OnStyle(self, evt):
        # Only style from the prompt pos to the end
        lastPos = self.GetTextLength()
        if self.lastPromptPos and self.lastPromptPos != lastPos:
            self.SetLexer(stc.STC_LEX_PYTHON)
            self.SetKeywords(0, ' '.join(keyword.kwlist))

            self.Colourise(self.lastPromptPos, lastPos)

            self.SetLexer(0)


    def OnUpdateUI(self, evt):
        # check for matching braces
        braceAtCaret = -1
        braceOpposite = -1
        charBefore = None
        caretPos = self.GetCurrentPos()
        if caretPos > 0:
            charBefore = self.GetCharAt(caretPos - 1)
            styleBefore = self.GetStyleAt(caretPos - 1)

        # check before
        if charBefore and chr(charBefore) in "[]{}()" and styleBefore == stc.STC_P_OPERATOR:
            braceAtCaret = caretPos - 1

        # check after
        if braceAtCaret < 0:
            charAfter = self.GetCharAt(caretPos)
            styleAfter = self.GetStyleAt(caretPos)
            if charAfter and chr(charAfter) in "[]{}()" and styleAfter == stc.STC_P_OPERATOR:
                braceAtCaret = caretPos

        if braceAtCaret >= 0:
            braceOpposite = self.BraceMatch(braceAtCaret)

        if braceAtCaret != -1  and braceOpposite == -1:
            self.BraceBadlight(braceAtCaret)
        else:
            self.BraceHighlight(braceAtCaret, braceOpposite)



    #----------------------------------------------
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
    app = wx.PyWidgetTester(size = (640, 480))
    app.SetWidget(PyShellWindow, -1)
    app.MainLoop()


#----------------------------------------------------------------------


