#----------------------------------------------------------------------------
# Name:         dbg.py
# RCS-ID:       $Id$
# Author:       Will Sadkin
# Email:        wsadkin@nameconnector.com
# Created:      07/11/2002
# Copyright:    (c) 2002 by Will Sadkin, 2002
# License:      wxWindows license
#----------------------------------------------------------------------------
# 12/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o V2.5 compatability update 
#

"""
This module provides a useful debugging framework that supports
showing nesting of function calls and allows a program to contain
lots of debugging print statements that can easily be turned on
or off to debug the code.  It also supports the ability to
have each function indent the debugging statements contained
within it, including those of any other function called within
its scope, thus allowing you to see in what order functions are
being called, and from where.

This capability is particularly useful in wxPython applications,
where exactly events occur that cause functions to be called is
not entirely clear, and because wxPython programs can't be run
from inside other debugging environments that have their own
message loops.

This module defines a Logger class, responsible for managing
debugging output.  Each Logger instance can be given a name
at construction; if this is done, '<name>:' will precede each
logging output made by that Logger instance.

The log() function this class provides takes a set of positional
arguments that are printed in order if debugging is enabled
(just like print does), followed by a set of keyword arguments
that control the behavior of the log() function itself on subsequent
calls.  The current keyword arguments are:

indent
    When set to a value of 1, this increments the current
    indentation level, causing all subsequent dbg() outputs to be
    indented by 3 more spaces.  When set to a value of 0,
    this process is reversed, causing the indent to decrease by
    3 spaces.  The default indentation level is 0.

enable
    When set to a value of 1, this turns on dbg() output for
    for program importing this module, until told to do otherwise.
    When set to a value of 0, dbg output is turned off.  (dbg
    output is off by default.)

suspend
    When set to a value of 1, this increments the current
    "suspension" level.  This makes it possible for a function
    to temporarily suspend its and any of its dependents'
    potential outputs that use the same Logger instance.
    When set to a value of 0, the suspension level is
    decremented.  When the value goes back to 0, potential
    logging is resumed (actual output depends on the
    "enable" status of the Logger instance in question.)

wxlog
    When set to a value of 1, the output will be sent to the
    active wxLog target.

stream
    When set to a non-None value, the current output stream
    (default of sys.stdout) is pushed onto a stack of streams,
    and is replaced in the dbg system with the specified stream.
    When called with a value of None, the previous stream will
    be restored (if stacked.)  If set to None without previously
    changing it will result in no action being taken.

You can also call the log function implicitly on the Logger
instance, ie. you can type:
    from wxPython.tools.dbg import Logger
    dbg = Logger()
    dbg('something to print')

Using this fairly simple mechanism, it is possible to get fairly
useful debugging output in a program.  Consider the following
code example:

>>> d = {1:'a', 2:'dictionary', 3:'of', 4:'words'}
>>> dbg = dbg.Logger('module')
>>> dbg(enable=1)
module: dbg enabled
>>> def foo(d):
...     dbg('foo', indent=1)
...     bar(d)
...     dbg('end of foo', indent=0)
...
>>> def bar(d):
...     dbg('bar', indent=1)
...     dbg('contents of d:', indent=1)
...     l = d.items()
...     l.sort()
...     for key, value in l:
...         dbg('%d =' % key, value)
...     dbg(indent=0)
...     dbg('end of bar', indent=0)
...
>>> foo(d)
module: foo
   module: bar
      module: contents of d:
         module: 1 = a
         module: 2 = dictionary
         module: 3 = of
         module: 4 = words
      module: end of bar
   module: end of foo
>>>

"""


class Logger:
    def __init__(self, name=None):
        import sys
        self.name = name
        self._indent = 0     # current number of indentations
        self._dbg = 0        # enable/disable flag
        self._suspend = 0    # allows code to "suspend/resume" potential dbg output
        self._wxLog = 0      # use wxLogMessage for debug output
        self._outstream = sys.stdout  # default output stream
        self._outstream_stack = []    # for restoration of streams as necessary


    def IsEnabled():
        return self._dbg

    def IsSuspended():
        return _suspend


    def log(self, *args, **kwargs):
        """
        This function provides a useful framework for generating
        optional debugging output that can be displayed at an
        arbitrary level of indentation.
        """
        if not self._dbg and not 'enable' in kwargs.keys():
            return

        if self._dbg and len(args) and not self._suspend:
            # (emulate print functionality)
            strs = [str(arg) for arg in args]
            output = ' '.join(strs)
            if self.name: output = self.name+': ' + output
            output = ' ' * 3 * self._indent + output

            if self._wxLog:
                from wxPython.wx import wxLogMessage    # (if not already imported)
                wxLogMessage(output)
            else:
                self._outstream.write(output + '\n')
                self._outstream.flush()
        # else do nothing

        # post process args:
        for kwarg, value in kwargs.items():
            if kwarg == 'indent':
                self.SetIndent(value)
            elif kwarg == 'enable':
                self.SetEnabled(value)
            elif kwarg == 'suspend':
                self.SetSuspend(value)
            elif kwarg == 'wxlog':
                self.SetWxLog(value)
            elif kwarg == 'stream':
                self.SetStream(value)

    # aliases for the log function
    dbg = log       # backwards compatible
    msg = log       #
    __call__ = log  # this one lets you 'call' the instance directly


    def SetEnabled(self, value):
        if value:
            old_dbg = self._dbg
            self._dbg = 1
            if not old_dbg:
                self.dbg('dbg enabled')
        else:
            if self._dbg:
                self.dbg('dbg disabled')
                self._dbg = 0


    def SetSuspend(self, value):
        if value:
            self._suspend += 1
        elif self._suspend > 0:
            self._suspend -= 1


    def SetIndent(self, value):
        if value:
            self._indent += 1
        elif self._indent > 0:
            self._indent -= 1


    def SetWxLog(self, value):
        self._wxLog = value


    def SetStream(self, value):
        if value:
            self._outstream_stack.append( self._outstream )
            self._outstream = value
        elif value is None and len(self._outstream_stack) > 0:
            self._outstream = self._outstream_stack.pop(-1)


#------------------------------------------------------------

if __name__ == "__main__":
    import  sys
    import  wx
    
    wx.Log_SetActiveTarget( wx.LogStderr() )
    logger = Logger('module')
    dbg = logger.dbg
    dbg(enable=1)
    logger('test __call__ interface')
    dbg('testing wxLog output to stderr:', wxlog=1, indent=1)
    dbg('1,2,3...')
    dbg('testing wx.LogNull:')
    devnull = wx.LogNull()
    dbg('4,5,6...') # shouldn't print, according to doc...
    del devnull
    dbg('(resuming to wx.LogStdErr)', '7,8,9...', indent=0)
    dbg('disabling wx.Log output, switching to stderr:')
    dbg(wxlog=0, stream=sys.stderr)
    dbg(logger._outstream, 'switching back to stdout:')
    dbg(stream=None)
    dbg(logger._outstream )
    def foo(str):
        dbg('foo:', indent=1)
        dbg(str, indent=0)
    foo('testing dbg inside function')
    class bar(Logger):
        def __init__(self, name):
            Logger.__init__(self, name)
        def enable(self, value):
            self.dbg(enable=value)
        def foo(self, str):
            self.dbg('foo:', indent=1)
            self.dbg(str, indent=0)
    f = bar('class mixin')
    f.foo("shouldn't print")
    f.enable(1)
    f.foo("should print")
    dbg('test completed.', enable=0)
    dbg('(double-checking ;-)')

