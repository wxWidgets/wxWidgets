"""The PyCrust Shell is an interactive text control in which a user types in
commands to be sent to the interpreter. This particular shell is based on
wxPython's wxStyledTextCtrl. The latest files are always available at the
SourceForge project page at http://sourceforge.net/projects/pycrust/.
Sponsored by Orbtech.com - Your Source For Python Development Services"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__version__ = "$Revision$"[11:-2]

from wxPython.wx import *
from wxPython.stc import *
import keyword
import os
import sys
from pseudo import PseudoFileIn
from pseudo import PseudoFileOut
from pseudo import PseudoFileErr
from version import VERSION


if wxPlatform == '__WXMSW__':
    faces = { 'times'  : 'Times New Roman',
              'mono'   : 'Courier New',
              'helv'   : 'Lucida Console',
              'lucida' : 'Lucida Console',
              'other'  : 'Comic Sans MS',
              'size'   : 10,
              'lnsize' : 9,
              'backcol': '#FFFFFF',
            }
    # Versions of wxPython prior to 2.3.2 had a sizing bug on Win platform.
    # The font was 2 points too large. So we need to reduce the font size.
    if ((wxMAJOR_VERSION, wxMINOR_VERSION) == (2, 3) and wxRELEASE_NUMBER < 2) \
    or (wxMAJOR_VERSION <= 2 and wxMINOR_VERSION <= 2):
        faces['size'] -= 2
        faces['lnsize'] -= 2
else:  # GTK
    faces = { 'times'  : 'Times',
              'mono'   : 'Courier',
              'helv'   : 'Helvetica',
              'other'  : 'new century schoolbook',
              'size'   : 12,
              'lnsize' : 10,
              'backcol': '#FFFFFF',
            }


class ShellFacade:
    """Simplified interface to all shell-related functionality.

    This is a semi-transparent facade, in that all attributes of other are 
    still accessible, even though only some are visible to the user."""
    
    name = 'PyCrust Shell Interface'
    revision = __version__
    
    def __init__(self, other):
        """Create a ShellFacade instance."""
        methods = ['ask',
                   'clear',
                   'pause',
                   'prompt',
                   'quit',
                   'redirectStderr',
                   'redirectStdin',
                   'redirectStdout',
                   'run',
                   'runfile',
                  ]
        for method in methods:
            self.__dict__[method] = getattr(other, method)
        d = self.__dict__
        d['other'] = other
        d['help'] = 'There is no help available, yet.'
        

    def __getattr__(self, name):
        if hasattr(self.other, name):
            return getattr(self.other, name)
        else:
            raise AttributeError, name

    def __setattr__(self, name, value):
        if self.__dict__.has_key(name):
            self.__dict__[name] = value
        elif hasattr(self.other, name):
            return setattr(self.other, name, value)
        else:
            raise AttributeError, name

    def _getAttributeNames(self):
        """Return list of magic attributes to extend introspection."""
        list = ['autoCallTip',
                'autoComplete',
                'autoCompleteCaseInsensitive',
                'autoCompleteIncludeDouble',
                'autoCompleteIncludeMagic',
                'autoCompleteIncludeSingle',
               ]
        list.sort()
        return list


class Shell(wxStyledTextCtrl):
    """PyCrust Shell based on wxStyledTextCtrl."""
    
    name = 'PyCrust Shell'
    revision = __version__
    
    def __init__(self, parent, id=-1, pos=wxDefaultPosition, \
                 size=wxDefaultSize, style=wxCLIP_CHILDREN, introText='', \
                 locals=None, InterpClass=None, *args, **kwds):
        """Create a PyCrust Shell instance."""
        wxStyledTextCtrl.__init__(self, parent, id, pos, size, style)
        # Grab these so they can be restored by self.redirect* methods.
        self.stdin = sys.stdin
        self.stdout = sys.stdout
        self.stderr = sys.stderr
        # Add the current working directory "." to the search path.
        sys.path.insert(0, os.curdir)
        # Import a default interpreter class if one isn't provided.
        if InterpClass == None:
            from interpreter import Interpreter
        else:
            Interpreter = InterpClass
        # Create default locals so we have something interesting.
        shellLocals = {'__name__': 'PyCrust-Shell', 
                       '__doc__': 'PyCrust-Shell, The PyCrust Python Shell.',
                       '__version__': VERSION,
                      }
        # Add the dictionary that was passed in.
        if locals:
            shellLocals.update(locals)
        self.interp = Interpreter(locals=shellLocals, \
                                  rawin=self.readRaw, \
                                  stdin=PseudoFileIn(self.readIn), \
                                  stdout=PseudoFileOut(self.writeOut), \
                                  stderr=PseudoFileErr(self.writeErr), \
                                  *args, **kwds)
        # Keep track of the most recent prompt starting and ending positions.
        self.promptPos = [0, 0]
        # Keep track of the most recent non-continuation prompt.
        self.prompt1Pos = [0, 0]
        # Keep track of multi-line commands.
        self.more = 0
        # Create the command history.  Commands are added into the front of
        # the list (ie. at index 0) as they are entered. self.historyIndex
        # is the current position in the history; it gets incremented as you
        # retrieve the previous command, decremented as you retrieve the
        # next, and reset when you hit Enter. self.historyIndex == -1 means
        # you're on the current command, not in the history.
        self.history = []
        self.historyIndex = -1
        # Assign handlers for keyboard events.
        EVT_KEY_DOWN(self, self.OnKeyDown)
        EVT_CHAR(self, self.OnChar)
        # Configure various defaults and user preferences.
        self.config()
        # Display the introductory banner information.
        try: self.showIntro(introText)
        except: pass
        # Assign some pseudo keywords to the interpreter's namespace.
        try: self.setBuiltinKeywords()
        except: pass
        # Add 'shell' to the interpreter's local namespace.
        try: self.setLocalShell()
        except: pass
        # Do this last so the user has complete control over their
        # environment. They can override anything they want.
        try: self.execStartupScript(self.interp.startupScript)
        except: pass

    def destroy(self):
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
        # Do we want to automatically pop up command argument help?
        self.autoCallTip = 1
        self.CallTipSetBackground(wxColour(255, 255, 232))

    def showIntro(self, text=''):
        """Display introductory text in the shell."""
        if text:
            if not text.endswith(os.linesep): text += os.linesep
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
## POB: This is having some weird side-effects so I'm taking it out.
##        import __builtin__
##        from pseudo import PseudoKeyword
##        __builtin__.close = __builtin__.exit = __builtin__.quit = \
##            PseudoKeyword(self.quit)
        import __builtin__
        from pseudo import PseudoKeyword
        __builtin__.close = __builtin__.exit = __builtin__.quit = \
            'Click on the close button to leave the application.'

    def quit(self):
        """Quit the application."""
        
        # XXX Good enough for now but later we want to send a close event.
        
        # In the close event handler we can make sure they want to quit.
        # Other applications, like PythonCard, may choose to hide rather than
        # quit so we should just post the event and let the surrounding app
        # decide what it wants to do.
        self.write('Click on the close button to leave the application.')
    
    def setLocalShell(self):
        """Add 'shell' to locals as reference to ShellFacade instance."""
        self.interp.locals['shell'] = ShellFacade(other=self)
    
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

    def OnChar(self, event):
        """Keypress event handler.

        Prevents modification of previously submitted commands/responses."""
        if not self.CanEdit():
            return
        key = event.KeyCode()
        currpos = self.GetCurrentPos()
        stoppos = self.promptPos[1]
        if key == ord('.'):
            # The dot or period key activates auto completion.
            # Get the command between the prompt and the cursor.
            # Add a dot to the end of the command.
            command = self.GetTextRange(stoppos, currpos) + '.'
            self.write('.')
            if self.autoComplete: self.autoCompleteShow(command)
        elif key == ord('('):
            # The left paren activates a call tip and cancels
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

    def OnKeyDown(self, event):
        """Key down event handler.

        Prevents modification of previously submitted commands/responses."""
        key = event.KeyCode()
        controlDown = event.ControlDown()
        altDown = event.AltDown()
        shiftDown = event.ShiftDown()
        currpos = self.GetCurrentPos()
        stoppos = self.promptPos[1]
        # Return is used to submit a command to the interpreter.
        if key == WXK_RETURN:
            if self.AutoCompActive(): self.AutoCompCancel()
            if self.CallTipActive(): self.CallTipCancel()
            self.processLine()
        # If the auto-complete window is up let it do its thing.
        elif self.AutoCompActive():
            event.Skip()
        # Let Ctrl-Alt-* get handled normally.
        elif controlDown and altDown:
            event.Skip()
        # Cut to the clipboard.
        elif controlDown and key in (ord('X'), ord('x')):
            self.Cut()
        # Copy to the clipboard.
        elif controlDown and not shiftDown and key in (ord('C'), ord('c')):
            self.Copy()
        # Copy to the clipboard, including prompts.
        elif controlDown and shiftDown and key in (ord('C'), ord('c')):
            self.CopyWithPrompts()
        # Paste from the clipboard.
        elif controlDown and key in (ord('V'), ord('v')):
            self.Paste()
        # Retrieve the previous command from the history buffer.
        elif (controlDown and key == WXK_UP) \
        or (altDown and key in (ord('P'), ord('p'))):
            self.OnHistoryRetrieve(step=+1)
        # Retrieve the next command from the history buffer.
        elif (controlDown and key == WXK_DOWN) \
        or (altDown and key in (ord('N'), ord('n'))):
            self.OnHistoryRetrieve(step=-1)
        # Search up the history for the text in front of the cursor.
        elif key == WXK_F8:
            self.OnHistorySearch()
        # Home needs to be aware of the prompt.
        elif key == WXK_HOME:
            if currpos >= stoppos:
                if event.ShiftDown():
                    # Select text from current position to end of prompt.
                    self.SetSelection(self.GetCurrentPos(), stoppos)
                else:
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
            if currpos > self.prompt1Pos[1]:
                event.Skip()
        # Only allow these keys after the latest prompt.
        elif key in (WXK_TAB, WXK_DELETE):
            if self.CanEdit():
                event.Skip()
        # Don't toggle between insert mode and overwrite mode.
        elif key == WXK_INSERT:
            pass
        # Don't allow line deletion.
        elif controlDown and key in (ord('L'), ord('l')):
            pass
        # Don't allow line transposition.
        elif controlDown and key in (ord('T'), ord('t')):
            pass
        # Protect the readonly portion of the shell.
        elif not self.CanEdit():
            pass
        else:
            event.Skip()

    def OnHistoryRetrieve(self, step):
        """Retrieve the previous/next command from the history buffer."""
        if not self.CanEdit():
            return
        startpos = self.GetCurrentPos()
        newindex = self.historyIndex + step
        if not (-1 <= newindex < len(self.history)):
            return
        self.historyIndex = newindex
        if newindex == -1:
            self.ReplaceSelection('')
        else:
            self.ReplaceSelection('')
            command = self.history[self.historyIndex]
            command = command.replace('\n', os.linesep + sys.ps2)
            self.ReplaceSelection(command)
        endpos = self.GetCurrentPos()
        self.SetSelection(endpos, startpos)

    def OnHistorySearch(self):
        """Search up the history buffer for the text in front of the cursor."""
        if not self.CanEdit():
            return
        startpos = self.GetCurrentPos()
        # The text up to the cursor is what we search for.
        numCharsAfterCursor = self.GetTextLength() - startpos
        searchText = self.getCommand(rstrip=0)
        if numCharsAfterCursor > 0:
            searchText = searchText[:-numCharsAfterCursor]
        if not searchText:
            return
        # Search upwards from the current history position and loop back
        # to the beginning if we don't find anything.
        if (self.historyIndex <= -1) \
        or (self.historyIndex >= len(self.history)-2):
            searchOrder = range(len(self.history))
        else:
            searchOrder = range(self.historyIndex+1, len(self.history)) + \
                          range(self.historyIndex)
        for i in searchOrder:
            command = self.history[i]
            if command[:len(searchText)] == searchText:
                # Replace the current selection with the one we've found.
                self.ReplaceSelection(command[len(searchText):])
                endpos = self.GetCurrentPos()
                self.SetSelection(endpos, startpos)
                # We've now warped into middle of the history.
                self.historyIndex = i
                break

    def setStatusText(self, text):
        """Display status information."""
        
        # This method will most likely be replaced by the enclosing app
        # to do something more interesting, like write to a status bar.
        print text

    def processLine(self):
        """Process the line of text at which the user hit Enter."""
        
        # The user hit ENTER and we need to decide what to do. They could be
        # sitting on any line in the shell.
        
        thepos = self.GetCurrentPos()
        endpos = self.GetTextLength()
        # If they hit RETURN at the very bottom, execute the command.
        if thepos == endpos:
            self.interp.more = 0
            if self.getCommand():
                command = self.GetTextRange(self.prompt1Pos[1], endpos)
            else:
                # This is a hack, now that we allow editing of previous
                # lines, which throws off our promptPos values.
                newend = endpos - len(self.getCommand(rstrip=0))
                command = self.GetTextRange(self.prompt1Pos[1], newend)
            command = command.replace(os.linesep + sys.ps2, '\n')
            self.push(command)
        # Or replace the current command with the other command.
        elif thepos < self.prompt1Pos[0]:
            theline = self.GetCurrentLine()
            command = self.getCommand(rstrip=0)
            # If the new line contains a command (even an invalid one).
            if command:
                command = self.getMultilineCommand()
                self.SetCurrentPos(endpos)
                startpos = self.prompt1Pos[1]
                self.SetSelection(startpos, endpos)
                self.ReplaceSelection('')
                self.write(command)
                self.more = 0
            # Otherwise, put the cursor back where we started.
            else:
                self.SetCurrentPos(thepos)
                self.SetAnchor(thepos)
        # Or add a new line to the current single or multi-line command.
        elif thepos > self.prompt1Pos[1]:
            self.write(os.linesep)
            self.more = 1
            self.prompt()

    def getMultilineCommand(self, rstrip=1):
        """Extract a multi-line command from the editor.
        
        The command may not necessarily be valid Python syntax."""
        # XXX Need to extract real prompts here. Need to keep track of the
        # prompt every time a command is issued.
        ps1 = str(sys.ps1)
        ps1size = len(ps1)
        ps2 = str(sys.ps2)
        ps2size = len(ps2)
        # This is a total hack job, but it works.
        text = self.GetCurLine()[0]
        line = self.GetCurrentLine()
        while text[:ps2size] == ps2 and line > 0:
            line -= 1
            self.GotoLine(line)
            text = self.GetCurLine()[0]
        if text[:ps1size] == ps1:
            line = self.GetCurrentLine()
            self.GotoLine(line)
            startpos = self.GetCurrentPos() + ps1size
            line += 1
            self.GotoLine(line)
            while self.GetCurLine()[0][:ps2size] == ps2:
                line += 1
                self.GotoLine(line)
            stoppos = self.GetCurrentPos()
            command = self.GetTextRange(startpos, stoppos)
            command = command.replace(os.linesep + sys.ps2, '\n')
            command = command.rstrip()
            command = command.replace('\n', os.linesep + sys.ps2)
        else:
            command = ''
        if rstrip:
            command = command.rstrip()
        return command
    
    def getCommand(self, text=None, rstrip=1):
        """Extract a command from text which may include a shell prompt.
        
        The command may not necessarily be valid Python syntax."""
        if not text:
            text = self.GetCurLine()[0]
        # Strip the prompt off the front of text leaving just the command.
        command = self.lstripPrompt(text)
        if command == text:
            command = ''  # Real commands have prompts.
        if rstrip:
            command = command.rstrip()
        return command

    def lstripPrompt(self, text):
        """Return text without a leading prompt."""
        ps1 = str(sys.ps1)
        ps1size = len(ps1)
        ps2 = str(sys.ps2)
        ps2size = len(ps2)
        # Strip the prompt off the front of text.
        if text[:ps1size] == ps1:
            text = text[ps1size:]
        elif text[:ps2size] == ps2:
            text = text[ps2size:]
        return text
    
    def push(self, command):
        """Send command to the interpreter for execution."""
        self.write(os.linesep)
        self.more = self.interp.push(command)
        if not self.more:
            self.addHistory(command.rstrip())
        self.prompt()

    def addHistory(self, command):
        """Add command to the command history."""
        # Reset the history position.
        self.historyIndex = -1
        # Insert this command into the history, unless it's a blank
        # line or the same as the last command.
        if command != '' \
        and (len(self.history) == 0 or command != self.history[0]):
            self.history.insert(0, command)

    def write(self, text):
        """Display text in the shell.

        Replace line endings with OS-specific endings."""
        text = self.fixLineEndings(text)
        self.AddText(text)
        self.EnsureCaretVisible()

    def fixLineEndings(self, text):
        """Return text with line endings replaced by OS-specific endings."""
        lines = text.split('\r\n')
        for l in range(len(lines)):
            chunks = lines[l].split('\r')
            for c in range(len(chunks)):
                chunks[c] = os.linesep.join(chunks[c].split('\n'))
            lines[l] = os.linesep.join(chunks)
        text = os.linesep.join(lines)
        return text

    def prompt(self):
        """Display appropriate prompt for the context, either ps1 or ps2.
        
        If this is a continuation line, autoindent as necessary."""
        if self.more:
            prompt = str(sys.ps2)
        else:
            prompt = str(sys.ps1)
        pos = self.GetCurLine()[1]
        if pos > 0: self.write(os.linesep)
        self.promptPos[0] = self.GetCurrentPos()
        if not self.more: self.prompt1Pos[0] = self.GetCurrentPos()
        self.write(prompt)
        self.promptPos[1] = self.GetCurrentPos()
        if not self.more:
            self.prompt1Pos[1] = self.GetCurrentPos()
            # Keep the undo feature from undoing previous responses.
            self.EmptyUndoBuffer()
        # XXX Add some autoindent magic here if more.
        if self.more:
            self.write(' '*4)  # Temporary hack indentation.
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
                self.write(text + os.linesep)
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
        # Go to the very bottom of the text.
        endpos = self.GetTextLength()
        self.SetCurrentPos(endpos)        
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
    
    def redirectStdin(self, redirect=1):
        """If redirect is true then sys.stdin will come from the shell."""
        if redirect:
            sys.stdin = PseudoFileIn(self.readIn)
        else:
            sys.stdin = self.stdin

    def redirectStdout(self, redirect=1):
        """If redirect is true then sys.stdout will go to the shell."""
        if redirect:
            sys.stdout = PseudoFileOut(self.writeOut)
        else:
            sys.stdout = self.stdout

    def redirectStderr(self, redirect=1):
        """If redirect is true then sys.stderr will go to the shell."""
        if redirect:
            sys.stderr = PseudoFileErr(self.writeErr)
        else:
            sys.stderr = self.stderr

    def CanCut(self):
        """Return true if text is selected and can be cut."""
        if self.GetSelectionStart() != self.GetSelectionEnd() \
        and self.GetSelectionStart() >= self.prompt1Pos[1] \
        and self.GetSelectionEnd() >= self.prompt1Pos[1]:
            return 1
        else:
            return 0
    
    def CanCopy(self):
        """Return true if text is selected and can be copied."""
        return self.GetSelectionStart() != self.GetSelectionEnd()

    def CanPaste(self):
        """Return true if a paste should succeed."""
        if self.CanEdit() and wxStyledTextCtrl.CanPaste(self):
            return 1
        else:
            return 0

    def CanEdit(self):
        """Return true if editing should succeed."""
        return self.GetCurrentPos() >= self.prompt1Pos[1]

    def Cut(self):
        """Remove selection and place it on the clipboard."""
        if self.CanCut() and self.CanCopy():
            if self.AutoCompActive(): self.AutoCompCancel()
            if self.CallTipActive: self.CallTipCancel()
            self.Copy()
            self.ReplaceSelection('')

    def Copy(self):
        """Copy selection and place it on the clipboard."""
        if self.CanCopy():
            command = self.GetSelectedText()
            command = command.replace(os.linesep + sys.ps2, os.linesep)
            command = command.replace(os.linesep + sys.ps1, os.linesep)
            command = self.lstripPrompt(text=command)
            data = wxTextDataObject(command)
            if wxTheClipboard.Open():
                wxTheClipboard.SetData(data)
                wxTheClipboard.Close()

    def CopyWithPrompts(self):
        """Copy selection, including prompts, and place it on the clipboard."""
        if self.CanCopy():
            command = self.GetSelectedText()
            data = wxTextDataObject(command)
            if wxTheClipboard.Open():
                wxTheClipboard.SetData(data)
                wxTheClipboard.Close()

    def Paste(self):
        """Replace selection with clipboard contents."""
        if self.CanPaste():
            if wxTheClipboard.Open():
                if wxTheClipboard.IsSupported(wxDataFormat(wxDF_TEXT)):
                    data = wxTextDataObject()
                    if wxTheClipboard.GetData(data):
                        command = data.GetText()
                        command = command.rstrip()
                        command = self.fixLineEndings(command)
                        command = self.lstripPrompt(text=command)
                        command = command.replace(os.linesep + sys.ps2, '\n')
                        command = command.replace(os.linesep, '\n')
                        command = command.replace('\n', os.linesep + sys.ps2)
                        self.ReplaceSelection('')
                        self.write(command)
                wxTheClipboard.Close()


wxID_SELECTALL = NewId()  # This *should* be defined by wxPython.
ID_AUTOCOMP = NewId()
ID_AUTOCOMP_SHOW = NewId()
ID_AUTOCOMP_INCLUDE_MAGIC = NewId()
ID_AUTOCOMP_INCLUDE_SINGLE = NewId()
ID_AUTOCOMP_INCLUDE_DOUBLE = NewId()
ID_CALLTIPS = NewId()
ID_CALLTIPS_SHOW = NewId()


class ShellMenu:
    """Mixin class to add standard menu items."""
    
    def createMenus(self):
        m = self.fileMenu = wxMenu()
        m.AppendSeparator()
        m.Append(wxID_EXIT, 'E&xit', 'Exit PyCrust')

        m = self.editMenu = wxMenu()
        m.Append(wxID_UNDO, '&Undo \tCtrl+Z', 'Undo the last action')
        m.Append(wxID_REDO, '&Redo \tCtrl+Y', 'Redo the last undone action')
        m.AppendSeparator()
        m.Append(wxID_CUT, 'Cu&t \tCtrl+X', 'Cut the selection')
        m.Append(wxID_COPY, '&Copy \tCtrl+C', 'Copy the selection')
        m.Append(wxID_PASTE, '&Paste \tCtrl+V', 'Paste')
        m.AppendSeparator()
        m.Append(wxID_CLEAR, 'Cle&ar', 'Delete the selection')
        m.Append(wxID_SELECTALL, 'Select A&ll', 'Select all text')

        m = self.autocompMenu = wxMenu()
        m.Append(ID_AUTOCOMP_SHOW, 'Show Auto Completion', \
                 'Show auto completion during dot syntax', \
                 checkable=1)
        m.Append(ID_AUTOCOMP_INCLUDE_MAGIC, 'Include Magic Attributes', \
                 'Include attributes visible to __getattr__ and __setattr__', \
                 checkable=1)
        m.Append(ID_AUTOCOMP_INCLUDE_SINGLE, 'Include Single Underscores', \
                 'Include attibutes prefixed by a single underscore', \
                 checkable=1)
        m.Append(ID_AUTOCOMP_INCLUDE_DOUBLE, 'Include Double Underscores', \
                 'Include attibutes prefixed by a double underscore', \
                 checkable=1)

        m = self.calltipsMenu = wxMenu()
        m.Append(ID_CALLTIPS_SHOW, 'Show Call Tips', \
                 'Show call tips with argument specifications', checkable=1)

        m = self.optionsMenu = wxMenu()
        m.AppendMenu(ID_AUTOCOMP, '&Auto Completion', self.autocompMenu, \
                     'Auto Completion Options')
        m.AppendMenu(ID_CALLTIPS, '&Call Tips', self.calltipsMenu, \
                     'Call Tip Options')

        m = self.helpMenu = wxMenu()
        m.AppendSeparator()
        m.Append(wxID_ABOUT, '&About...', 'About PyCrust')

        b = self.menuBar = wxMenuBar()
        b.Append(self.fileMenu, '&File')
        b.Append(self.editMenu, '&Edit')
        b.Append(self.optionsMenu, '&Options')
        b.Append(self.helpMenu, '&Help')
        self.SetMenuBar(b)

        EVT_MENU(self, wxID_EXIT, self.OnExit)
        EVT_MENU(self, wxID_UNDO, self.OnUndo)
        EVT_MENU(self, wxID_REDO, self.OnRedo)
        EVT_MENU(self, wxID_CUT, self.OnCut)
        EVT_MENU(self, wxID_COPY, self.OnCopy)
        EVT_MENU(self, wxID_PASTE, self.OnPaste)
        EVT_MENU(self, wxID_CLEAR, self.OnClear)
        EVT_MENU(self, wxID_SELECTALL, self.OnSelectAll)
        EVT_MENU(self, wxID_ABOUT, self.OnAbout)
        EVT_MENU(self, ID_AUTOCOMP_SHOW, \
                 self.OnAutoCompleteShow)
        EVT_MENU(self, ID_AUTOCOMP_INCLUDE_MAGIC, \
                 self.OnAutoCompleteIncludeMagic)
        EVT_MENU(self, ID_AUTOCOMP_INCLUDE_SINGLE, \
                 self.OnAutoCompleteIncludeSingle)
        EVT_MENU(self, ID_AUTOCOMP_INCLUDE_DOUBLE, \
                 self.OnAutoCompleteIncludeDouble)
        EVT_MENU(self, ID_CALLTIPS_SHOW, \
                 self.OnCallTipsShow)

        EVT_UPDATE_UI(self, wxID_UNDO, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, wxID_REDO, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, wxID_CUT, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, wxID_COPY, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, wxID_PASTE, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, wxID_CLEAR, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, ID_AUTOCOMP_SHOW, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, ID_AUTOCOMP_INCLUDE_MAGIC, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, ID_AUTOCOMP_INCLUDE_SINGLE, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, ID_AUTOCOMP_INCLUDE_DOUBLE, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, ID_CALLTIPS_SHOW, self.OnUpdateMenu)

    def OnExit(self, event):
        self.Close(true)

    def OnUndo(self, event):
        self.shell.Undo()

    def OnRedo(self, event):
        self.shell.Redo()

    def OnCut(self, event):
        self.shell.Cut()

    def OnCopy(self, event):
        self.shell.Copy()

    def OnPaste(self, event):
        self.shell.Paste()

    def OnClear(self, event):
        self.shell.Clear()

    def OnSelectAll(self, event):
        self.shell.SelectAll()

    def OnAbout(self, event):
        """Display an About PyCrust window."""
        import sys
        title = 'About PyCrust'
        text = 'PyCrust %s\n\n' % VERSION + \
               'Yet another Python shell, only flakier.\n\n' + \
               'Half-baked by Patrick K. O\'Brien,\n' + \
               'the other half is still in the oven.\n\n' + \
               'Shell Revision: %s\n' % self.shell.revision + \
               'Interpreter Revision: %s\n\n' % self.shell.interp.revision + \
               'Python Version: %s\n' % sys.version.split()[0] + \
               'wxPython Version: %s\n' % wx.__version__ + \
               'Platform: %s\n' % sys.platform
        dialog = wxMessageDialog(self, text, title, wxOK | wxICON_INFORMATION)
        dialog.ShowModal()
        dialog.Destroy()

    def OnAutoCompleteShow(self, event):
        self.shell.autoComplete = event.IsChecked()

    def OnAutoCompleteIncludeMagic(self, event):
        self.shell.autoCompleteIncludeMagic = event.IsChecked()

    def OnAutoCompleteIncludeSingle(self, event):
        self.shell.autoCompleteIncludeSingle = event.IsChecked()

    def OnAutoCompleteIncludeDouble(self, event):
        self.shell.autoCompleteIncludeDouble = event.IsChecked()

    def OnCallTipsShow(self, event):
        self.shell.autoCallTip = event.IsChecked()

    def OnUpdateMenu(self, event):
        """Update menu items based on current status."""
        id = event.GetId()
        if id == wxID_UNDO:
            event.Enable(self.shell.CanUndo())
        elif id == wxID_REDO:
            event.Enable(self.shell.CanRedo())
        elif id == wxID_CUT:
            event.Enable(self.shell.CanCut())
        elif id == wxID_COPY:
            event.Enable(self.shell.CanCopy())
        elif id == wxID_PASTE:
            event.Enable(self.shell.CanPaste())
        elif id == wxID_CLEAR:
            event.Enable(self.shell.CanCut())
        elif id == ID_AUTOCOMP_SHOW:
            event.Check(self.shell.autoComplete)
        elif id == ID_AUTOCOMP_INCLUDE_MAGIC:
            event.Check(self.shell.autoCompleteIncludeMagic)
        elif id == ID_AUTOCOMP_INCLUDE_SINGLE:
            event.Check(self.shell.autoCompleteIncludeSingle)
        elif id == ID_AUTOCOMP_INCLUDE_DOUBLE:
            event.Check(self.shell.autoCompleteIncludeDouble)
        elif id == ID_CALLTIPS_SHOW:
            event.Check(self.shell.autoCallTip)
            

class ShellFrame(wxFrame, ShellMenu):
    """Frame containing the PyCrust shell component."""
    
    name = 'PyCrust Shell Frame'
    revision = __version__
    
    def __init__(self, parent=None, id=-1, title='PyShell', \
                 pos=wxDefaultPosition, size=wxDefaultSize, \
                 style=wxDEFAULT_FRAME_STYLE, locals=None, \
                 InterpClass=None, *args, **kwds):
        """Create a PyCrust ShellFrame instance."""
        wxFrame.__init__(self, parent, id, title, pos, size, style)
        intro = 'Welcome To PyCrust %s - The Flakiest Python Shell' % VERSION
        intro += '\nSponsored by Orbtech.com - Your Source For Python Development Services'
        self.CreateStatusBar()
        self.SetStatusText(intro.replace('\n', ', '))
        if wxPlatform == '__WXMSW__':
            import os
            filename = os.path.join(os.path.dirname(__file__), 'PyCrust.ico')
            icon = wxIcon(filename, wxBITMAP_TYPE_ICO)
            self.SetIcon(icon)
        self.shell = Shell(parent=self, id=-1, introText=intro, \
                           locals=locals, InterpClass=InterpClass, \
                           *args, **kwds)
        # Override the shell so that status messages go to the status bar.
        self.shell.setStatusText = self.SetStatusText
        self.createMenus()


           
    
