"""PyCrust Interpreter executes Python commands."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__date__ = "July 1, 2001"
__version__ = "$Revision$"[11:-2]

import os
import sys
from code import InteractiveInterpreter
import introspect


class Interpreter(InteractiveInterpreter):
    """PyCrust Interpreter based on code.InteractiveInterpreter."""
    
    revision = __version__
    
    def __init__(self, locals=None, rawin=None, \
                 stdin=sys.stdin, stdout=sys.stdout, stderr=sys.stderr):
        """Create an interactive interpreter object."""
        InteractiveInterpreter.__init__(self, locals=locals)
        self.stdin = stdin
        self.stdout = stdout
        self.stderr = stderr
        if rawin is not None:
            import __builtin__
            __builtin__.raw_input = rawin
            del __builtin__
        copyright = 'Type "copyright", "credits" or "license" for more information.'
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
        self.commandBuffer = []  # List of lists to support recursive push().
        self.commandHistory = []
        self.startupScript = os.environ.get('PYTHONSTARTUP')

    def push(self, command):
        """Send command to the interpreter to be executed.
        
        Because this may be called recursively, we append a new list 
        onto the commandBuffer list and then append commands into that.
        If the passed in command is part of a multi-line command we keep
        appending the pieces to the last list in commandBuffer until we
        have a complete command, then, finally, we delete that last list.
        """ 
        if not self.more: self.commandBuffer.append([])
        self.commandBuffer[-1].append(command)
        source = '\n'.join(self.commandBuffer[-1])
        self.more = self.runsource(source)
        if not self.more: del self.commandBuffer[-1]
        return self.more
        
    def runsource(self, source):
        """Compile and run source code in the interpreter."""
        stdin, stdout, stderr = sys.stdin, sys.stdout, sys.stderr
        sys.stdin = self.stdin
        sys.stdout = self.stdout
        sys.stderr = self.stderr
        more = InteractiveInterpreter.runsource(self, source)
        sys.stdin, sys.stdout, sys.stderr = stdin, stdout, stderr
        return more
        
    def getAutoCompleteList(self, command='', *args, **kwds):
        """Return list of auto-completion options for a command.
        
        The list of options will be based on the locals namespace."""
        return introspect.getAutoCompleteList(command, self.locals, *args, **kwds)

    def getCallTip(self, command='', *args, **kwds):
        """Return call tip text for a command.
        
        The call tip information will be based on the locals namespace."""
        return introspect.getCallTip(command, self.locals, *args, **kwds)


class InterpreterAlaCarte(Interpreter):
    """PyCrustAlaCarte Demo Interpreter."""
    
    def __init__(self, locals, rawin, stdin, stdout, stderr, \
                 ps1='main prompt', ps2='continuation prompt'):
        """Create an interactive interpreter object."""
        Interpreter.__init__(self, locals=locals, rawin=rawin, \
                             stdin=stdin, stdout=stdout, stderr=stderr)
        sys.ps1 = ps1
        sys.ps2 = ps2

