#----------------------------------------------------------------------------
# Name:         dbg.py
# RCS-ID:       $Id$
# Author:       Will Sadkin
# Email:        wsadkin@nameconnector.com
# Created:      07/11/2002
# Copyright:    (c) 2002 by Will Sadkin, 2002
# License:      wxWindows license
#----------------------------------------------------------------------------

"""
This module provides a useful debugging framework that supports
showing nesting of function calls and allows a program to contain
lots of dbg() print statements that can easily be turned off
once the code has been debugged.  It also supports the ability to
have each function indent the debugging statements contained
within it, including those of any other function called within
its scope, thus allowing you to see in what order functions are
being called, and from where.

This capability is particularly useful in wxPython applications,
where exactly events occur that cause functions to be called is
not entirely clear, and because wxPython programs can't be run
from inside other debugging environments that have their own
message loops.

the dbg() function this module provides takes a set of positional
arguments that are printed in order if debugging is enabled
(just like print does), followed by a set of keyword arguments
that control the behavior of the dbg() function itself on subsequent
calls:

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


Using this fairly simple mechanism, it is possible to get fairly
useful debugging output in a program.  Consider the following
code example:

>>> d = {1:'a', 2:'dictionary', 3:'of', 4:'words'}
>>> dbg(enable=1)
>>> def foo(d):
        dbg('foo', indent=1)
>>>     bar(d)
>>>     dbg('end of foo', indent=0)
>>>
>>> def bar(d):
>>>     dbg('bar', indent=1)
>>>     dbg('contents of d:', indent=1)
>>>     l = d.items()
>>>     l.sort()
>>>     for key, value in l:
>>>         dbg('%d =' % key, value)
>>>     dbg(indent=0)
>>>     dbg('end of bar', indent=0)
>>>
>>> foo(d)
foo
   bar
      contents of d:
         1 = a
         2 = dictionary
         3 = of
         4 = words
      end of bar
   end of foo
>>>

"""

_indent = 0     # current number of indentations
_dbg = 0        # enable/disable flag
_suspend = 0    # allows code to "suspend/resume" potential dbg output
_wxLog = 0      # use wxLogMessage for debug output


import sys
_outstream = sys.stdout  # default output stream
_outstream_stack = []    # for restoration of streams as necessary
del sys


def dbgEnabled():
    return _dbg

def dbgSuspended():
    return _suspend

def dbg(*args, **kwargs):
    """
    This function provides a useful framework for generating
    optional debugging output that can be displayed at an
    arbitrary level of indentation.
    """
    global _indent, _dbg, _suspend, _wxLog, _outstream, _outstream_stack
    if not _dbg and not 'enable' in kwargs.keys():
        return

    if _dbg and len(args) and not _suspend:
        # (emulate print functionality)
        strs = [str(arg) for arg in args]
        output = ' ' * 3 * _indent + ' '.join(strs)

        if _wxLog:
            from wxPython.wx import wxLogMessage    # (if not already imported)
            wxLogMessage(output)
        else:
            _outstream.write(output + '\n')
            _outstream.flush()
    # else do nothing

    # post process args:
    for kwarg, value in kwargs.items():
        if kwarg == 'indent' and value:
            _indent = _indent + 1
        elif kwarg == 'indent' and not value and _indent > 0:
            _indent = _indent - 1

        if kwarg == 'enable' and value:
            old_dbg = _dbg
            _dbg = 1
            if not old_dbg:
                dbg('dbg enabled')
        elif kwarg == 'enable' and not value:
            if _dbg:
                dbg('dbg disabled')
                _dbg = 0

        if kwarg == 'suspend':
            _suspend = value

        if kwarg == 'wxlog':
            _wxLog = value

        if kwarg == 'stream' and value:
            _outstream_stack.append( _outstream )
            _outstream = value
        elif kwarg == 'stream' and value is None and len(_outstream_stack) > 0:
            _outstream = _outstream_stack.pop(-1)


#------------------------------------------------------------

if __name__ == "__main__":
    from wxPython.wx import *
    wxLog_SetActiveTarget( wxLogStderr() )
    dbg(enable=1)
    dbg('testing wxLog output to stderr:', wxlog=1, indent=1)
    dbg('1,2,3...')
    dbg('testing wxLogNull:')
    devnull = wxLogNull()
    dbg('4,5,6...') # shouldn't print, according to doc...
    del devnull
    dbg('(resuming to wxLogStdErr)', '7,8,9...', indent=0)
    dbg('disabling wxLog output, switching to stderr:')
    dbg(wxlog=0, stream=sys.stderr)
    dbg(_outstream, 'switching back to stdout:')
    dbg(stream=None)
    dbg(_outstream )
    dbg('test completed.', enable=0)
    dbg('(double-checking ;-)')

