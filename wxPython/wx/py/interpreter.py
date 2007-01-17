"""Interpreter executes Python commands."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

import os
import sys
from code import InteractiveInterpreter
import dispatcher
import introspect
import wx

class Interpreter(InteractiveInterpreter):
    """Interpreter based on code.InteractiveInterpreter."""
    
    revision = __revision__
    
    def __init__(self, locals=None, rawin=None, 
                 stdin=sys.stdin, stdout=sys.stdout, stderr=sys.stderr,
                 showInterpIntro=True):
        """Create an interactive interpreter object."""
        InteractiveInterpreter.__init__(self, locals=locals)
        self.stdin = stdin
        self.stdout = stdout
        self.stderr = stderr
        if rawin:
            import __builtin__
            __builtin__.raw_input = rawin
            del __builtin__
        if showInterpIntro:
            copyright = 'Type "help", "copyright", "credits" or "license"'
            copyright += ' for more information.'
            self.introText = 'Python %s on %s%s%s' % \
                             (sys.version, sys.platform, os.linesep, copyright)
        try:
            sys.ps1
        except AttributeError:
            sys.ps1 = '>>> '
        try:
            sys.ps2
        except AttributeError:
            sys.ps2 = '... '
        self.more = 0
        # List of lists to support recursive push().
        self.commandBuffer = []
        self.startupScript = None
        

    def push(self, command):
        """Send command to the interpreter to be executed.
        
        Because this may be called recursively, we append a new list
        onto the commandBuffer list and then append commands into
        that.  If the passed in command is part of a multi-line
        command we keep appending the pieces to the last list in
        commandBuffer until we have a complete command. If not, we
        delete that last list."""

        # In case the command is unicode try encoding it
        if type(command) == unicode:
            try:
                command = command.encode(wx.GetDefaultPyEncoding())
            except UnicodeEncodeError:
                pass # otherwise leave it alone
                
        if not self.more:
            try: del self.commandBuffer[-1]
            except IndexError: pass
        if not self.more: self.commandBuffer.append([])
        self.commandBuffer[-1].append(command)
        source = '\n'.join(self.commandBuffer[-1])
        more = self.more = self.runsource(source)
        dispatcher.send(signal='Interpreter.push', sender=self,
                        command=command, more=more, source=source)
        return more
        
    def runsource(self, source):
        """Compile and run source code in the interpreter."""
        stdin, stdout, stderr = sys.stdin, sys.stdout, sys.stderr
        sys.stdin, sys.stdout, sys.stderr = \
                   self.stdin, self.stdout, self.stderr
        more = InteractiveInterpreter.runsource(self, source)
        # If sys.std* is still what we set it to, then restore it.
        # But, if the executed source changed sys.std*, assume it was
        # meant to be changed and leave it. Power to the people.
        if sys.stdin == self.stdin:
            sys.stdin = stdin
        if sys.stdout == self.stdout:
            sys.stdout = stdout
        if sys.stderr == self.stderr:
            sys.stderr = stderr
        return more
        
    def getAutoCompleteKeys(self):
        """Return list of auto-completion keycodes."""
        return [ord('.')]

    def getAutoCompleteList(self, command='', *args, **kwds):
        """Return list of auto-completion options for a command.
        
        The list of options will be based on the locals namespace."""
        stdin, stdout, stderr = sys.stdin, sys.stdout, sys.stderr
        sys.stdin, sys.stdout, sys.stderr = \
                   self.stdin, self.stdout, self.stderr
        l = introspect.getAutoCompleteList(command, self.locals,
                                           *args, **kwds)
        sys.stdin, sys.stdout, sys.stderr = stdin, stdout, stderr
        return l

    def getCallTip(self, command='', *args, **kwds):
        """Return call tip text for a command.
        
        Call tip information will be based on the locals namespace."""
        return introspect.getCallTip(command, self.locals, *args, **kwds)


class InterpreterAlaCarte(Interpreter):
    """Demo Interpreter."""
    
    def __init__(self, locals, rawin, stdin, stdout, stderr, 
                 ps1='main prompt', ps2='continuation prompt'):
        """Create an interactive interpreter object."""
        Interpreter.__init__(self, locals=locals, rawin=rawin, 
                             stdin=stdin, stdout=stdout, stderr=stderr)
        sys.ps1 = ps1
        sys.ps2 = ps2

   
