"""The PyCrust Shell is an interactive text control in which a user types in
commands to be sent to the interpreter. This particular shell is based on
wxPython's wxStyledTextCtrl. The latest files are always available at the
SourceForge project page at http://sourceforge.net/projects/pycrust/."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__date__ = "July 1, 2001"
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
              'size'   : 8,
              'lnsize' : 7,
              'backcol': '#FFFFFF',
            }
else:  # GTK
    faces = { 'times'  : 'Times',
              'mono'   : 'Courier',
              'helv'   : 'Helvetica',
              'other'  : 'new century schoolbook',
              'size'   : 12,
              'lnsize' : 10,
              'backcol': '#FFFFFF',
            }


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
        shellLocals = {'__name__': 'PyShell', 
                       '__doc__': 'PyShell, The PyCrust Python Shell.',
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
        # Keep track of multi-line commands.
        self.more = 0
        # Create the command history.  Commands are added into the front of
        # the list (ie. at index 0) as they are entered.  self.historyPos is
        # the current position in the history; it gets incremented as you
        # retrieve the previous command, decremented as you retrieve the next,
        # and reset when you hit Enter. self.historyPos == -1 means you're on
        # the current command, not in the history. self.tempCommand is
        # storage space for whatever was on the last line when you first hit
        # "Retrieve-Previous", so that the final "Retrieve-Next" will restore
        # whatever was originally there.  self.lastCommandRecalled remembers
        # the index of the last command to be recalled from the history, so
        # you can repeat a group of commands by going up-up-up-enter to find
        # the first one in the group then down-enter-down-enter to recall each
        # subsequent command.  Also useful for multiline commands, in lieu of
        # a proper implementation of those.
        self.history = []
        self.historyPos = -1
        self.tempCommand = ''
        self.lastCommandRecalled = -1
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
        # De we want to automatically pop up command argument help?
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
        lines of text."""
        key = event.KeyCode()
        currpos = self.GetCurrentPos()
        stoppos = self.promptPos[1]
        # If the auto-complete window is up let it do its thing.
        if self.AutoCompActive():
            event.Skip()
        # Control+UpArrow steps up through the history.
        elif key == WXK_UP and event.ControlDown() \
        and self.historyPos < len(self.history) - 1:
            # Move to the end of the buffer.
            endpos = self.GetTextLength()
            self.SetCurrentPos(endpos)
            # The first Control+Up stores the current command;
            # Control+Down brings it back.
            if self.historyPos == -1:
                self.tempCommand = self.getCommand()
            # Now replace the current line with the next one from the history.
            self.historyPos = self.historyPos + 1
            self.SetSelection(stoppos, endpos)
            self.ReplaceSelection(self.history[self.historyPos])
        # Control+DownArrow steps down through the history.
        elif key == WXK_DOWN and event.ControlDown():
            # Move to the end of the buffer.
            endpos = self.GetTextLength()
            self.SetCurrentPos(endpos)
            # Are we at the bottom end of the history?
            if self.historyPos == -1:
                # Do we have a lastCommandRecalled stored?
                if self.lastCommandRecalled >= 0:
                    # Replace the current line with the command after the
                    # last-recalled command (you'd think there should be a +1
                    # here but there isn't because the history was shuffled up
                    # by 1 after the previous command was recalled).
                    self.SetSelection(stoppos, endpos)
                    self.ReplaceSelection(self.history[self.lastCommandRecalled])
                    # We've now warped into middle of the history.
                    self.historyPos = self.lastCommandRecalled
                    self.lastCommandRecalled = -1
            else:
                # Fetch either the previous line from the history, or the saved
                # command if we're back at the start.
                self.historyPos = self.historyPos - 1
                if self.historyPos == -1:
                    newText = self.tempCommand
                else:
                    newText = self.history[self.historyPos]
                # Replace the current line with the new text.
                self.SetSelection(stoppos, endpos)
                self.ReplaceSelection(newText)
        # F8 on the last line does a search up the history for the text in
        # front of the cursor.
        elif key == WXK_F8 and self.GetCurrentLine() == self.GetLineCount()-1:
            tempCommand = self.getCommand()
            # The first F8 saves the current command, just like Control+Up.
            if self.historyPos == -1:
                self.tempCommand = tempCommand
            # The text up to the cursor is what we search for.
            searchText = tempCommand
            numCharsAfterCursor = self.GetTextLength() - self.GetCurrentPos()
            if numCharsAfterCursor > 0:
                searchText = searchText[:-numCharsAfterCursor]
            # Search upwards from the current history position and loop back
            # to the beginning if we don't find anything.
            for i in range(self.historyPos+1, len(self.history)) + \
                     range(self.historyPos):
                command = self.history[i]
                if command[:len(searchText)] == searchText:
                    # Replace the current line with the one we've found.
                    endpos = self.GetTextLength()
                    self.SetSelection(stoppos, endpos)
                    self.ReplaceSelection(command)
                    # Put the cursor back at the end of the search text.
                    pos = self.GetTextLength() - len(command) + len(searchText)
                    self.SetCurrentPos(pos)
                    self.SetAnchor(pos)
                    # We've now warped into middle of the history.
                    self.historyPos = i
                    self.lastCommandRecalled = -1
                    break
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
        lines of text."""
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
        command = self.getCommand()
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

    def getCommand(self, text=None):
        """Extract a command from text which may include a shell prompt.
        
        The command may not necessarily be valid Python syntax."""
        if not text:
            text = self.GetCurLine()[0]
        # XXX Need to extract real prompts here. Need to keep track of the
        # prompt every time a command is issued.
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
        self.addHistory(command)
        self.write(os.linesep)
        self.more = self.interp.push(command)
        self.prompt()
        # Keep the undo feature from undoing previous responses. The only
        # thing that can be undone is stuff typed after the prompt, before
        # hitting enter. After they hit enter it becomes permanent.
        self.EmptyUndoBuffer()

    def addHistory(self, command):
        """Add command to the command history."""
        # Store the last-recalled command; see the main comment for
        # self.lastCommandRecalled.
        if command != '':
            self.lastCommandRecalled = self.historyPos
        # Reset the history position.
        self.historyPos = -1
        # Insert this command into the history, unless it's a blank
        # line or the same as the last command.
        if command != '' \
        and (len(self.history) == 0 or command != self.history[0]):
            self.history.insert(0, command)

    def write(self, text):
        """Display text in the shell.

        Replace line endings with OS-specific endings."""
        lines = text.split('\r\n')
        for l in range(len(lines)):
            chunks = lines[l].split('\r')
            for c in range(len(chunks)):
                chunks[c] = os.linesep.join(chunks[c].split('\n'))
            lines[l] = os.linesep.join(chunks)
        text = os.linesep.join(lines)
        self.AddText(text)
        self.EnsureCaretVisible()
        #self.ScrollToColumn(0)

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
        return self.GetSelectionStart() != self.GetSelectionEnd()
    
    def CanCopy(self):
        """Return true if text is selected and can be copied."""
        return self.GetSelectionStart() != self.GetSelectionEnd()


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
        m.Append(wxID_CUT, 'Cu&t', 'Cut the selection')
        m.Append(wxID_COPY, '&Copy', 'Copy the selection')
        m.Append(wxID_PASTE, '&Paste', 'Paste')
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
        self.CreateStatusBar()
        self.SetStatusText(intro)
        if wxPlatform == '__WXMSW__':
            icon = wxIcon('PyCrust.ico', wxBITMAP_TYPE_ICO)
            self.SetIcon(icon)
        self.shell = Shell(parent=self, id=-1, introText=intro, \
                           locals=locals, InterpClass=InterpClass, \
                           *args, **kwds)
        # Override the shell so that status messages go to the status bar.
        self.shell.setStatusText = self.SetStatusText
        self.createMenus()


