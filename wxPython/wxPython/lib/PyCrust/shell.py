"""PyCrust Shell is the gui text control in which a user interacts and types
in commands to be sent to the interpreter. This particular shell is based on
wxPython's wxStyledTextCtrl.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__date__ = "July 1, 2001"
__version__ = "$Revision$"[11:-2]

from wxPython.wx import *
from wxPython.stc import *

import keyword
import os
import sys

from version import VERSION

if wxPlatform == '__WXMSW__':
    faces = { 'times'  : 'Times New Roman',
              'mono'   : 'Courier New',
              'helv'   : 'Lucida Console',
              'lucida' : 'Lucida Console',
              'other'  : 'Comic Sans MS',
              'size'   : 8,
              'lnsize' : 7,
              'backcol': '#FFFFFF',
            }
else:  # GTK
    faces = { 'times'  : 'Times',
              'mono'   : 'Courier',
              'helv'   : 'Helvetica',
              'other'  : 'new century schoolbook',
              'size'   : 9,
              'lnsize' : 8,
              'backcol': '#FFFFFF',
            }


class Shell(wxStyledTextCtrl):
    """PyCrust Shell based on wxStyledTextCtrl."""
    name = 'PyCrust Shell'
    revision = __version__
    def __init__(self, parent, id, introText='', locals=None, interp=None):
        """Create a PyCrust Shell object."""
        wxStyledTextCtrl.__init__(self, parent, id, style=wxCLIP_CHILDREN)
        self.introText = introText
        # Keep track of the most recent prompt starting and ending positions.
        self.promptPos = [0, 0]
        # Keep track of multi-line commands.
        self.more = 0
        # Assign handlers for keyboard events.
        EVT_KEY_DOWN(self, self.OnKeyDown)
        EVT_CHAR(self, self.OnChar)
        # Create a default interpreter if one isn't provided.
        if interp == None:
            from interpreter import Interpreter
            from pseudo import PseudoFileIn, PseudoFileOut, PseudoFileErr
            self.stdin = PseudoFileIn(self.readIn)
            self.stdout = PseudoFileOut(self.writeOut)
            self.stderr = PseudoFileErr(self.writeErr)
            # Override the default locals so we have something interesting.
            self.locals = {'__name__': 'PyCrust', 
                           '__doc__': 'PyCrust, The Python Shell.',
                           '__version__': VERSION,
                          }
            # Add the dictionary that was passed in.
            if locals:
                self.locals.update(locals)
            self.interp = Interpreter(locals=self.locals, 
                                      rawin=self.readRaw,
                                      stdin=self.stdin, 
                                      stdout=self.stdout, 
                                      stderr=self.stderr)
        else:
            self.interp = interp

        # Configure various defaults and user preferences.
        self.config()

        try:
            self.showIntro(self.introText)
        except:
            pass

        try:
            self.setBuiltinKeywords()
        except:
            pass
        
        try:
            self.setLocalShell()
        except:
            pass
        
        # Do this last so the user has complete control over their
        # environment. They can override anything they want.
        try:
            self.execStartupScript(self.interp.startupScript)
        except:
            pass
            
    def destroy(self):
        del self.stdin
        del self.stdout
        del self.stderr
        del self.interp
        
    def config(self):
        """Configure shell based on user preferences."""
        self.SetMarginType(1, wxSTC_MARGIN_NUMBER)
        self.SetMarginWidth(1, 40)
        
        self.SetLexer(wxSTC_LEX_PYTHON)
        self.SetKeyWords(0, ' '.join(keyword.kwlist))

        self.setStyles(faces)
        self.SetViewWhiteSpace(0)
        self.SetTabWidth(4)
        self.SetUseTabs(0)
        # Do we want to automatically pop up command completion options?
        self.autoComplete = 1
        self.autoCompleteIncludeMagic = 1
        self.autoCompleteIncludeSingle = 1
        self.autoCompleteIncludeDouble = 1
        self.autoCompleteCaseInsensitive = 1
        self.AutoCompSetIgnoreCase(self.autoCompleteCaseInsensitive)
        # De we want to automatically pop up command argument help?
        self.autoCallTip = 1
        self.CallTipSetBackground(wxColour(255, 255, 232))

    def showIntro(self, text=''):
        """Display introductory text in the shell."""
        if text:
            if text[-1] != '\n': text += '\n'
            self.write(text)
        try:
            self.write(self.interp.introText)
        except AttributeError:
            pass
    
    def setBuiltinKeywords(self):
        """Create pseudo keywords as part of builtins.
        
        This is a rather clever hack that sets "close", "exit" and "quit" 
        to a PseudoKeyword object so that we can make them do what we want.
        In this case what we want is to call our self.quit() method.
        The user can type "close", "exit" or "quit" without the final parens.
        """
        import __builtin__
        from pseudo import PseudoKeyword
        __builtin__.close = __builtin__.exit = __builtin__.quit = \
            PseudoKeyword(self.quit)

    def quit(self):
        """Quit the application."""
        
        # XXX Good enough for now but later we want to send a close event.
        
        # In the close event handler we can prompt to make sure they want to quit.
        # Other applications, like PythonCard, may choose to hide rather than
        # quit so we should just post the event and let the surrounding app
        # decide what it wants to do.
        self.write('Click on the close button to leave the application.')
    
    def setLocalShell(self):
        """Add 'shell' to locals."""
        self.interp.locals['shell'] = self
    
    def execStartupScript(self, startupScript):
        """Execute the user's PYTHONSTARTUP script if they have one."""
        if startupScript and os.path.isfile(startupScript):
            startupText = 'Startup script executed: ' + startupScript
            self.push('print %s;execfile(%s)' % \
                      (`startupText`, `startupScript`))
        else:
            self.push('')
            
    def setStyles(self, faces):
        """Configure font size, typeface and color for lexer."""
        
        # Default style
        self.StyleSetSpec(wxSTC_STYLE_DEFAULT, "face:%(mono)s,size:%(size)d" % faces)

        self.StyleClearAll()

        # Built in styles
        self.StyleSetSpec(wxSTC_STYLE_LINENUMBER, "back:#C0C0C0,face:%(mono)s,size:%(lnsize)d" % faces)
        self.StyleSetSpec(wxSTC_STYLE_CONTROLCHAR, "face:%(mono)s" % faces)
        self.StyleSetSpec(wxSTC_STYLE_BRACELIGHT, "fore:#0000FF,back:#FFFF88")
        self.StyleSetSpec(wxSTC_STYLE_BRACEBAD, "fore:#FF0000,back:#FFFF88")

        # Python styles
        self.StyleSetSpec(wxSTC_P_DEFAULT, "face:%(mono)s" % faces)
        self.StyleSetSpec(wxSTC_P_COMMENTLINE, "fore:#007F00,face:%(mono)s" % faces)
        self.StyleSetSpec(wxSTC_P_NUMBER, "")
        self.StyleSetSpec(wxSTC_P_STRING, "fore:#7F007F,face:%(mono)s" % faces)
        self.StyleSetSpec(wxSTC_P_CHARACTER, "fore:#7F007F,face:%(mono)s" % faces)
        self.StyleSetSpec(wxSTC_P_WORD, "fore:#00007F,bold")
        self.StyleSetSpec(wxSTC_P_TRIPLE, "fore:#7F0000")
        self.StyleSetSpec(wxSTC_P_TRIPLEDOUBLE, "fore:#000033,back:#FFFFE8")
        self.StyleSetSpec(wxSTC_P_CLASSNAME, "fore:#0000FF,bold")
        self.StyleSetSpec(wxSTC_P_DEFNAME, "fore:#007F7F,bold")
        self.StyleSetSpec(wxSTC_P_OPERATOR, "")
        self.StyleSetSpec(wxSTC_P_IDENTIFIER, "")
        self.StyleSetSpec(wxSTC_P_COMMENTBLOCK, "fore:#7F7F7F")
        self.StyleSetSpec(wxSTC_P_STRINGEOL, "fore:#000000,face:%(mono)s,back:#E0C0E0,eolfilled" % faces)

    def OnKeyDown(self, event):
        """Key down event handler.
        
        The main goal here is to not allow modifications to previous 
        lines of text.
        """
        key = event.KeyCode()
        currpos = self.GetCurrentPos()
        stoppos = self.promptPos[1]
        # If the auto-complete window is up let it do its thing.
        if self.AutoCompActive():
            event.Skip()
        # Return is used to submit a command to the interpreter.
        elif key == WXK_RETURN:
            if self.CallTipActive: self.CallTipCancel()
            self.processLine()
        # Home needs to be aware of the prompt.
        elif key == WXK_HOME:
            if currpos >= stoppos:
                self.SetCurrentPos(stoppos)
                self.SetAnchor(stoppos)
            else:
                event.Skip()
        # Basic navigation keys should work anywhere.
        elif key in (WXK_END, WXK_LEFT, WXK_RIGHT, WXK_UP, WXK_DOWN, \
                     WXK_PRIOR, WXK_NEXT):
            event.Skip()
        # Don't backspace over the latest prompt.
        elif key == WXK_BACK:
            if currpos > stoppos:
                event.Skip()
        # Only allow these keys after the latest prompt.
        elif key in (WXK_TAB, WXK_DELETE):
            if currpos >= stoppos:
                event.Skip()
        # Don't toggle between insert mode and overwrite mode.
        elif key == WXK_INSERT:
            pass
        else:
            event.Skip()

    def OnChar(self, event):
        """Keypress event handler.
        
        The main goal here is to not allow modifications to previous 
        lines of text.
        """
        key = event.KeyCode()
        currpos = self.GetCurrentPos()
        stoppos = self.promptPos[1]
        if currpos >= stoppos:
            if key == 46:
                # "." The dot or period key activates auto completion.
                # Get the command between the prompt and the cursor.
                # Add a dot to the end of the command.
                command = self.GetTextRange(stoppos, currpos) + '.'
                self.write('.')
                if self.autoComplete: self.autoCompleteShow(command)
            elif key == 40:
                # "(" The left paren activates a call tip and cancels
                # an active auto completion.
                if self.AutoCompActive(): self.AutoCompCancel()
                # Get the command between the prompt and the cursor.
                # Add the '(' to the end of the command.
                command = self.GetTextRange(stoppos, currpos) + '('
                self.write('(')
                if self.autoCallTip: self.autoCallTipShow(command)
            else:
                # Allow the normal event handling to take place.
                event.Skip()
        else:
            pass

    def setStatusText(self, text):
        """Display status information."""
        
        # This method will most likely be replaced by the enclosing app
        # to do something more interesting, like write to a status bar.
        print text

    def processLine(self):
        """Process the line of text at which the user hit Enter."""
        
        # The user hit ENTER and we need to decide what to do. They could be
        # sitting on any line in the shell.
        
        # Grab information about the current line.
        thepos = self.GetCurrentPos()
        theline = self.GetCurrentLine()
        thetext = self.GetCurLine()[0]
        command = self.getCommand(thetext)
        # Go to the very bottom of the text.
        endpos = self.GetTextLength()
        self.SetCurrentPos(endpos)
        endline = self.GetCurrentLine()
        # If they hit RETURN on the last line, execute the command.
        if theline == endline:
            self.push(command)
        # Otherwise, replace the last line with the new line.
        else:
            # If the new line contains a command (even an invalid one).
            if command:
                startpos = self.PositionFromLine(endline)
                self.SetSelection(startpos, endpos)
                self.ReplaceSelection('')
                self.prompt()
                self.write(command)
            # Otherwise, put the cursor back where we started.
            else:
                self.SetCurrentPos(thepos)
                self.SetAnchor(thepos)

    def getCommand(self, text):
        """Extract a command from text which may include a shell prompt.
        
        The command may not necessarily be valid Python syntax.
        """
        
        # XXX Need to extract real prompts here. Need to keep track of the
        # prompt every time a command is issued. Do this in the interpreter
        # with a line number, prompt, command dictionary. For the history, perhaps.
        ps1 = str(sys.ps1)
        ps1size = len(ps1)
        ps2 = str(sys.ps2)
        ps2size = len(ps2)
        text = text.rstrip()
        # Strip the prompt off the front of text leaving just the command.
        if text[:ps1size] == ps1:
            command = text[ps1size:]
        elif text[:ps2size] == ps2:
            command = text[ps2size:]
        else:
            command = ''
        return command
    
    def push(self, command):
        """Send command to the interpreter for execution."""
        self.write('\n')
        self.more = self.interp.push(command)
        self.prompt()
        # Keep the undo feature from undoing previous responses. The only
        # thing that can be undone is stuff typed after the prompt, before
        # hitting enter. After they hit enter it becomes permanent.
        self.EmptyUndoBuffer()

    def write(self, text):
        """Display text in the shell."""
        self.AddText(text)
        self.EnsureCaretVisible()
        #self.ScrollToColumn(0)
    
    def prompt(self):
        """Display appropriate prompt for the context, either ps1 or ps2.
        
        If this is a continuation line, autoindent as necessary.
        """
        if self.more:
            prompt = str(sys.ps2)
        else:
            prompt = str(sys.ps1)
        pos = self.GetCurLine()[1]
        if pos > 0: self.write('\n')
        self.promptPos[0] = self.GetCurrentPos()
        self.write(prompt)
        self.promptPos[1] = self.GetCurrentPos()
        # XXX Add some autoindent magic here if more.
        if self.more:
            self.write('\t')  # Temporary hack indentation.
        self.EnsureCaretVisible()
        self.ScrollToColumn(0)
    
    def readIn(self):
        """Replacement for stdin."""
        prompt = 'Please enter your response:'
        dialog = wxTextEntryDialog(None, prompt, \
                                   'Input Dialog (Standard)', '')
        try:
            if dialog.ShowModal() == wxID_OK:
                text = dialog.GetValue()
                self.write(text + '\n')
                return text
        finally:
            dialog.Destroy()
        return ''

    def readRaw(self, prompt='Please enter your response:'):
        """Replacement for raw_input."""
        dialog = wxTextEntryDialog(None, prompt, \
                                   'Input Dialog (Raw)', '')
        try:
            if dialog.ShowModal() == wxID_OK:
                text = dialog.GetValue()
                return text
        finally:
            dialog.Destroy()
        return ''

    def ask(self, prompt='Please enter your response:'):
        """Get response from the user."""
        return raw_input(prompt=prompt)
        
    def pause(self):
        """Halt execution pending a response from the user."""
        self.ask('Press enter to continue:')
        
    def clear(self):
        """Delete all text from the shell."""
        self.ClearAll()
        
    def run(self, command, prompt=1, verbose=1):
        """Execute command within the shell as if it was typed in directly.
        >>> shell.run('print "this"')
        >>> print "this"
        this
        >>> 
        """
        command = command.rstrip()
        if prompt: self.prompt()
        if verbose: self.write(command)
        self.push(command)

    def runfile(self, filename):
        """Execute all commands in file as if they were typed into the shell."""
        file = open(filename)
        try:
            self.prompt()
            for command in file.readlines():
                if command[:6] == 'shell.':  # Run shell methods silently.
                    self.run(command, prompt=0, verbose=0)
                else:
                    self.run(command, prompt=0, verbose=1)
        finally:
            file.close()
    
    def autoCompleteShow(self, command):
        """Display auto-completion popup list."""
        list = self.interp.getAutoCompleteList(command, \
                    includeMagic=self.autoCompleteIncludeMagic, \
                    includeSingle=self.autoCompleteIncludeSingle, \
                    includeDouble=self.autoCompleteIncludeDouble)
        if list:
            options = ' '.join(list)
            offset = 0
            self.AutoCompShow(offset, options)

    def autoCallTipShow(self, command):
        """Display argument spec and docstring in a popup bubble thingie."""
        if self.CallTipActive: self.CallTipCancel()
        tip = self.interp.getCallTip(command)
        if tip:
            offset = self.GetCurrentPos()
            self.CallTipShow(offset, tip)

    def writeOut(self, text):
        """Replacement for stdout."""
        self.write(text)
    
    def writeErr(self, text):
        """Replacement for stderr."""
        self.write(text)
    
    def CanCut(self):
        """Return true if text is selected and can be cut."""
        return self.GetSelectionStart() != self.GetSelectionEnd()
    
    def CanCopy(self):
        """Return true if text is selected and can be copied."""
        return self.GetSelectionStart() != self.GetSelectionEnd()

