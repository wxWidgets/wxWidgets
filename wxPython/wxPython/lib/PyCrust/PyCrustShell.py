"""
"""
__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__date__ = "July 1, 2001"
__version__ = "$Revision$"[11:-2]

import os
from PyCrustVersion import version

class Shell:
    """PyCrust Shell manages the Editor and Interpreter."""
    name = 'PyCrust Shell'
    revision = __version__
    def __init__(self, editorParent=None, introText='', editor=None, interp=None):
        """Create a PyCrust shell object to manage the editor and interpreter."""
        try:
            eval('crap')
        except:
            pass
        self.introText = introText
        # Create a default editor if one isn't provided.
        if editor == None:
            from PyCrustEditor import Editor
            self.editor = Editor(editorParent, id=-1)
        else:
            self.editor = editor
        # Link the editor to the shell so that the shell is a conduit for
        # pushing commands to the interpreter.
        self.editor.shellPush = self.shellPush
        # Create a default interpreter if one isn't provided.
        if interp == None:
            from PyCrustInterp import Interpreter
            from pseudo import PseudoFileIn, PseudoFileOut, PseudoFileErr
            self.stdin = PseudoFileIn(self.editor.readIn)
            self.stdout = PseudoFileOut(self.editor.writeOut)
            self.stderr = PseudoFileErr(self.editor.writeErr)
            # Override the default locals so we have something interesting.
            locals = {'__name__': 'PyCrust',
                      '__doc__': 'PyCrust, The Python Shell.',
                      '__version__': version,
                      }
            self.interp = Interpreter(locals=locals,
                                      rawin=self.editor.readRaw,
                                      stdin=self.stdin,
                                      stdout=self.stdout,
                                      stderr=self.stderr)
        else:
            self.interp = interp
        # XXX redo this using hasattr() or something so that we can link
        # these if a provided editor has this method.
        if editor == None or editor == self:
            # Override so the auto complete list comes from the interpreter.
            self.editor.getAutoCompleteList = self.interp.getAutoCompleteList
            # Override so the call tip comes from the interpreter.
            self.editor.getCallTip = self.interp.getCallTip
        # Keep track of whether the interpreter needs more.
        self.more = 0

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
        del self.editor
        del self.stdin
        del self.stdout
        del self.stderr
        del self.interp

    def showIntro(self, text=''):
        """Display introductory text in the shell editor."""
        if text:
            if text[-1] != '\n': text += '\n'
            self.editor.write(text)
        try:
            self.editor.write(self.interp.introText)
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
        self.editor.write('Click on the close button to leave the application.')

    def setLocalShell(self):
        """Add 'shell' to locals."""
        self.interp.locals['shell'] = self

    def execStartupScript(self, startupScript):
        """Execute the user's PYTHONSTARTUP script if they have one."""
        if startupScript and os.path.isfile(startupScript):
            startupText = 'Startup script executed: ' + startupScript
            self.editor.push('print %s;execfile(%s)' % \
                            (`startupText`, `startupScript`))
        else:
            self.editor.push('')

    def run(self, command, prompt=1, verbose=1):
        """Execute command within the shell as if it was typed in directly.
        >>> shell.run('print "this"')
        >>> print "this"
        this
        >>>
        """
        command = command.rstrip()
        if prompt: self.editor.prompt()
        if verbose: self.editor.write(command)
        self.editor.push(command)

    def runfile(self, filename):
        """Execute all commands in file as if they were typed into the shell."""
        file = open(filename)
        try:
            self.editor.prompt()
            for command in file.readlines():
                if command[:6] == 'shell.':  # Run shell methods silently.
                    self.run(command, prompt=0, verbose=0)
                else:
                    self.run(command, prompt=0, verbose=1)
        finally:
            file.close()

    def push(self, command):
        """Send command to the interpreter for execution."""
        self.more = self.interp.push(command)
        return self.more

    shellPush = push

    def ask(self, prompt='Please enter your response:'):
        """Get response from the user."""
        return raw_input(prompt=prompt)

    def pause(self):
        """Halt execution pending a response from the user."""
        self.ask('Press enter to continue:')

    def clear(self):
        """Delete all text from the shell editor."""
        self.editor.clear()


