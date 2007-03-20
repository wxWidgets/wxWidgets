#!/usr/bin/env python
# Copyright (c) 2002-2003 ActiveState
# See LICENSE.txt for license details.
""" Contents of LICENSE.txt:
Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
"""
 
r"""
    Python interface for process control.

    This module defines three Process classes for spawning,
    communicating and control processes. They are: Process, ProcessOpen,
    ProcessProxy. All of the classes allow one to specify the command (cmd),
    starting working directory (cwd), and environment to create for the
    new process (env) and to "wait" for termination of the child and
    "kill" the child.

    Process:
        Use this class to simply launch a process (either a GUI app or a
        console app in a new console) with which you do not intend to
        communicate via it std handles.

    ProcessOpen:
        Think of this as a super version of Python's os.popen3() method.
        This spawns the given command and sets up pipes for
        stdin/stdout/stderr which can then be used to communicate with
        the child.

    ProcessProxy:
        This is a heavy-weight class that, similar to ProcessOpen,
        spawns the given commands and sets up pipes to the child's
        stdin/stdout/stderr. However, it also starts three threads to
        proxy communication between each of the child's and parent's std
        handles. At the parent end of this communication are, by
        default, IOBuffer objects. You may specify your own objects here
        (usually sub-classing from IOBuffer, which handles some
        synchronization issues for you). The result is that it is
        possible to have your own IOBuffer instance that gets, say, a
        .write() "event" for every write that the child does on its
        stdout.

        Understanding ProcessProxy is pretty complex. Some examples
        below attempt to help show some uses. Here is a diagram of the
        comminucation:

                            <parent process>
               ,---->->->------'   ^   `------>->->----,
               |                   |                   v
           IOBuffer             IOBuffer            IOBuffer        
           (p.stdout)           (p.stderr)          (p.stdin)
               |                   |                   |
           _OutFileProxy        _OutFileProxy       _InFileProxy
           thread               thread              thread
               |                   ^                   |
               `----<-<-<------,   |   ,------<-<-<----'
                            <child process>

    Usage:
        import process
        p = process.<Process class>(cmd='echo hi', ...)
        #... use the various methods and attributes

    Examples:
      A simple 'hello world':
        >>> import process
        >>> p = process.ProcessOpen(['echo', 'hello'])
        >>> p.stdout.read()
        'hello\r\n'
        >>> p.wait()   # .wait() returns the child's exit status
        0

      Redirecting the stdout handler:
        >>> import sys
        >>> p = process.ProcessProxy(['echo', 'hello'], stdout=sys.stdout)
        hello

      Using stdin (need to use ProcessProxy here because it defaults to
      text-mode translation on Windows, ProcessOpen does not support
      this):
        >>> p = process.ProcessProxy(['sort'])
        >>> p.stdin.write('5\n')
        >>> p.stdin.write('2\n')
        >>> p.stdin.write('7\n')
        >>> p.stdin.close()
        >>> p.stdout.read()
        '2\n5\n7\n'

      Specifying environment variables:
        >>> p = process.ProcessOpen(['perl', '-e', 'print $ENV{FOO}'])
        >>> p.stdout.read()
        ''
        >>> p = process.ProcessOpen(['perl', '-e', 'print $ENV{FOO}'],
        ...                         env={'FOO':'bar'})
        >>> p.stdout.read()
        'bar'

      Killing a long running process (On Linux, to poll you must use
      p.wait(os.WNOHANG)):
        >>> p = ProcessOpen(['perl', '-e', 'while (1) {}'])
        >>> try:
        ...     p.wait(os.WNOHANG)  # poll to see if is process still running
        ... except ProcessError, ex:
        ...     if ex.errno == ProcessProxy.WAIT_TIMEOUT:
        ...             print "process is still running"
        ...
        process is still running
        >>> p.kill(42)
        >>> p.wait()
        42

      Providing objects for stdin/stdout/stderr:
        XXX write this, mention IOBuffer subclassing.
"""
#TODO:
#   - Discuss the decision to NOT have the stdout/stderr _OutFileProxy's
#     wait for process termination before closing stdin. It will just
#     close stdin when stdout is seen to have been closed. That is
#     considered Good Enough (tm). Theoretically it would be nice to
#     only abort the stdin proxying when the process terminates, but
#     watching for process termination in any of the parent's thread
#     adds the undesired condition that the parent cannot exit with the
#     child still running. That sucks.
#     XXX Note that I don't even know if the current stdout proxy even
#         closes the stdin proxy at all.
#   - DavidA: if I specify "unbuffered" for my stdin handler (in the
#     ProcessProxy constructor) then the stdin IOBuffer should do a
#     fparent.read() rather than a fparent.readline(). TrentM: can I do
#     that? What happens?
#

import os
import sys
import threading
import types
import pprint 
if sys.platform.startswith("win"):
    import msvcrt
    import win32api
    import win32file
    import win32pipe
    import pywintypes
    import win32process
    import win32event
    # constants pulled from win32con to save memory
    VER_PLATFORM_WIN32_WINDOWS = 1
    CTRL_BREAK_EVENT = 1
    SW_SHOWDEFAULT = 10
    WM_CLOSE = 0x10
    DUPLICATE_SAME_ACCESS = 2
    
else:
    import signal


#---- exceptions

class ProcessError(Exception):
    def __init__(self, msg, errno=-1):
        Exception.__init__(self, msg)
        self.errno = errno


#---- internal logging facility

class Logger:
    DEBUG, INFO, WARN, ERROR, FATAL = range(5)
    def __init__(self, name, level=None, streamOrFileName=sys.stderr):
        self.name = name
        if level is None:
            self.level = self.WARN
        else:
            self.level = level
        if type(streamOrFileName) == types.StringType:
            self.stream = open(streamOrFileName, 'w')
            self._opennedStream = 1
        else:
            self.stream = streamOrFileName
            self._opennedStream = 0
    def __del__(self):
        if self._opennedStream:
            self.stream.close()
    def _getLevelName(self, level):
        levelNameMap = {
            self.DEBUG: "DEBUG",
            self.INFO: "INFO",
            self.WARN: "WARN",
            self.ERROR: "ERROR",
            self.FATAL: "FATAL",
        }
        return levelNameMap[level]
    def log(self, level, msg, *args):
        if level < self.level:
            return
        message = "%s: %s:" % (self.name, self._getLevelName(level).lower())
        message = message + (msg % args) + "\n"
        self.stream.write(message)
        self.stream.flush()
    def debug(self, msg, *args):
        self.log(self.DEBUG, msg, *args)
    def info(self, msg, *args):
        self.log(self.INFO, msg, *args)
    def warn(self, msg, *args):
        self.log(self.WARN, msg, *args)
    def error(self, msg, *args):
        self.log(self.ERROR, msg, *args)
    def fatal(self, msg, *args):
        self.log(self.FATAL, msg, *args)

# Loggers:
#   - 'log' to log normal process handling
#   - 'logres' to track system resource life
#   - 'logfix' to track wait/kill proxying in _ThreadFixer
if 1:   # normal/production usage
    log = Logger("process", Logger.WARN)
else:   # development/debugging usage
    log = Logger("process", Logger.DEBUG, sys.stdout)
if 1:   # normal/production usage
    logres = Logger("process.res", Logger.WARN)
else:   # development/debugging usage
    logres = Logger("process.res", Logger.DEBUG, sys.stdout)
if 1:   # normal/production usage
    logfix = Logger("process.waitfix", Logger.WARN)
else:   # development/debugging usage
    logfix = Logger("process.waitfix", Logger.DEBUG, sys.stdout)



#---- globals

_version_ = (0, 5, 0)

# List of registered processes (see _(un)registerProcess).
_processes = []



#---- internal support routines

def _escapeArg(arg):
    """Escape the given command line argument for the shell."""
    #XXX There is a probably more that we should escape here.
    return arg.replace('"', r'\"')


def _joinArgv(argv):
    r"""Join an arglist to a string appropriate for running.

        >>> import os
        >>> _joinArgv(['foo', 'bar "baz'])
        'foo "bar \\"baz"'
    """
    cmdstr = ""
    for arg in argv:
        if ' ' in arg or ';' in arg:
            cmdstr += '"%s"' % _escapeArg(arg)
        else:
            cmdstr += _escapeArg(arg)
        cmdstr += ' '
    if cmdstr.endswith(' '): cmdstr = cmdstr[:-1]  # strip trailing space
    return cmdstr


def _getPathFromEnv(env):
    """Return the PATH environment variable or None.

    Do the right thing for case sensitivity per platform.
    XXX Icky. This guarantee of proper case sensitivity of environment
        variables should be done more fundamentally in this module.
    """
    if sys.platform.startswith("win"):
        for key in env.keys():
            if key.upper() == "PATH":
                return env[key]
        else:
            return None
    else:
        if env.has_key("PATH"):
            return env["PATH"]
        else:
            return None


def _whichFirstArg(cmd, env=None):
    """Return the given command ensuring that the first arg (the command to
    launch) is a full path to an existing file.

    Raise a ProcessError if no such executable could be found.
    """
    # Parse out the first arg.
    if cmd.startswith('"'):
        # The .replace() is to ensure it does not mistakenly find the
        # second '"' in, say (escaped quote):
        #           "C:\foo\"bar" arg1 arg2
        idx = cmd.replace('\\"', 'XX').find('"', 1)
        if idx == -1:
            raise ProcessError("Malformed command: %r" % cmd)
        first, rest = cmd[1:idx], cmd[idx+1:]
        rest = rest.lstrip()
    else:
        if ' ' in cmd:
            first, rest = cmd.split(' ', 1)
        else:
            first, rest = cmd, ""

    # Ensure the first arg is a valid path to the appropriate file.
    import which
    if os.sep in first:
        altpath = [os.path.dirname(first)]
        firstbase = os.path.basename(first)
        candidates = list(which.which(firstbase, path=altpath))
    elif env:
        altpath = _getPathFromEnv(env)
        if altpath:
            candidates = list(which.which(first, altpath.split(os.pathsep)))
        else:
            candidates = list(which.which(first))
    else:
        candidates = list(which.which(first))
    if candidates:
        return _joinArgv( [candidates[0]] ) + ' ' + rest
    else:
        raise ProcessError("Could not find an appropriate leading command "\
                           "for: %r" % cmd)


if sys.platform.startswith("win"):
    def _SaferCreateProcess(appName,        # app name
                            cmd,            # command line 
                            processSA,      # process security attributes 
                            threadSA,       # thread security attributes 
                            inheritHandles, # are handles are inherited
                            creationFlags,  # creation flags 
                            env,            # environment
                            cwd,            # current working directory
                            si):            # STARTUPINFO pointer
        """If CreateProcess fails from environment type inconsistency then
        fix that and try again.
        
        win32process.CreateProcess requires that all environment keys and
        values either be all ASCII or all unicode. Try to remove this burden
        from the user of process.py.
        """
        isWin9x = win32api.GetVersionEx()[3] == VER_PLATFORM_WIN32_WINDOWS
        # On Win9x all keys and values of 'env' must be ASCII (XXX
        # Actually this is probably only true if the Unicode support
        # libraries, which are not installed by default, are not
        # installed). On other Windows flavours all keys and values of
        # 'env' must all be ASCII *or* all Unicode. We will try to
        # automatically convert to the appropriate type, issuing a
        # warning if such an automatic conversion is necessary.

        #XXX Komodo 2.0 Beta 1 hack. This requirement should be
        #    pushed out to Komodo code using process.py. Or should it?
        if isWin9x and env:
            aenv = {}
            for key, value in env.items():
                aenv[str(key)] = str(value)
            env = aenv
        
        log.debug("""\
_SaferCreateProcess(appName=%r,
                    cmd=%r,
                    env=%r,
                    cwd=%r)
    os.getcwd(): %r
""", appName, cmd, env, cwd, os.getcwd())
        try:
            hProcess, hThread, processId, threadId\
                = win32process.CreateProcess(appName, cmd, processSA,
                                             threadSA, inheritHandles,
                                             creationFlags, env, cwd, si)
        except TypeError, ex:
            if ex.args == ('All dictionary items must be strings, or all must be unicode',):
                # Try again with an all unicode environment.
                #XXX Would be nice if didn't have to depend on the error
                #    string to catch this.
                #XXX Removing this warning for 2.3 release. See bug
                #    23215. The right fix is to correct the PHPAppInfo
                #    stuff to heed the warning.
                #import warnings
                #warnings.warn('env: ' + str(ex), stacklevel=4)
                if isWin9x and env:
                    aenv = {}
                    try:
                        for key, value in env.items():
                            aenv[str(key)] = str(value)
                    except UnicodeError, ex:
                        raise ProcessError(str(ex))
                    env = aenv
                elif env:
                    uenv = {}
                    for key, val in env.items():
                        try:
                            uenv[unicode(key)] = unicode(val)   # default encoding
                        except UnicodeError:
                            try:
                                uenv[unicode(key, 'iso-8859-1')] = unicode(val, 'iso-8859-1')   # backup encoding
                            except UnicodeError:
                                log.warn('Skipping environment variable "%s" in execution process: unable to convert to unicode using either the default encoding or ISO-8859-1' % (key))
                    env = uenv
                hProcess, hThread, processId, threadId\
                    = win32process.CreateProcess(appName, cmd, processSA,
                                                 threadSA, inheritHandles,
                                                 creationFlags, env, cwd,
                                                 si)
            else:
                raise
        return hProcess, hThread, processId, threadId


# Maintain references to all spawned ProcessProxy objects to avoid hangs.
#   Otherwise, if the user lets the a ProcessProxy object go out of
#   scope before the process has terminated, it is possible to get a
#   hang (at least it *used* to be so when we had the
#   win32api.CloseHandle(<stdin handle>) call in the __del__() method).
#   XXX Is this hang possible on Linux as well?
# A reference is removed from this list when the process's .wait or
# .kill method is called.
# XXX Should an atexit() handler be registered to kill all curently
#     running processes? Else *could* get hangs, n'est ce pas?
def _registerProcess(process):
    global _processes
    log.info("_registerprocess(process=%r)", process)

    # Clean up zombie processes.
    #   If the user does not call .wait() or .kill() on processes then
    #   the ProcessProxy object will not get cleaned up until Python
    #   exits and _processes goes out of scope. Under heavy usage that
    #   is a big memory waste. Cleaning up here alleviates that.
    for p in _processes[:]: # use copy of _process, because we may modifiy it
        try:
            # poll to see if is process still running
            if sys.platform.startswith("win"):
                timeout = 0
            else:
                timeout = os.WNOHANG
            p.wait(timeout)
            _unregisterProcess(p)
        except ProcessError, ex:
            if ex.errno == ProcessProxy.WAIT_TIMEOUT:
                pass
            else:
                raise
        
    _processes.append(process)

def _unregisterProcess(process):
    global _processes
    log.info("_unregisterProcess(process=%r)", process)
    try:
        _processes.remove(process)
        del process
    except ValueError:
        pass


def _fixupCommand(cmd, env=None):
    """Fixup the command string so it is launchable via CreateProcess.

    One cannot just launch, say "python", via CreateProcess. A full path
    to an executable is required. In general there are two choices:
        1. Launch the command string via the shell. The shell will find
           the fullpath to the appropriate executable. This shell will
           also be able to execute special shell commands, like "dir",
           which don't map to an actual executable.
        2. Find the fullpath to the appropriate executable manually and
           launch that exe.

    Option (1) is preferred because you don't have to worry about not
    exactly duplicating shell behaviour and you get the added bonus of
    being able to launch "dir" and friends.

    However, (1) is not always an option. Doing so when the shell is
    command.com (as on all Win9x boxes) or when using WinNT's cmd.exe,
    problems are created with .kill() because these shells seem to eat
    up Ctrl-C's and Ctrl-Break's sent via
    win32api.GenerateConsoleCtrlEvent().  Strangely this only happens
    when spawn via this Python interface. For example, Ctrl-C get
    through to hang.exe here:
      C:\> ...\w9xpopen.exe "C:\WINDOWS\COMMAND.COM /c hang.exe"
      ^C
    but not here:
      >>> p = ProcessOpen('hang.exe')
      # This results in the same command to CreateProcess as
      # above.
      >>> p.kill()

    Hence, for these platforms we fallback to option (2).  Cons:
      - cannot spawn shell commands like 'dir' directly
      - cannot spawn batch files
    """
    if sys.platform.startswith("win"):
        # Fixup the command string to spawn.  (Lifted from
        # posixmodule.c::_PyPopenCreateProcess() with some modifications)
        comspec = os.environ.get("COMSPEC", None)
        win32Version = win32api.GetVersion()
        if comspec is None:
            raise ProcessError("Cannot locate a COMSPEC environment "\
                               "variable to use as the shell")
        # Explicitly check if we are using COMMAND.COM.  If we
        # are then use the w9xpopen hack.
        elif (win32Version & 0x80000000L == 0) and\
             (win32Version &        0x5L >= 5) and\
             os.path.basename(comspec).lower() != "command.com":
            # 2000/XP and not using command.com.
            if '"' in cmd or "'" in cmd:
                cmd = comspec + ' /c "%s"' % cmd
            else:
                cmd = comspec + ' /c ' + cmd
        elif (win32Version & 0x80000000L == 0) and\
             (win32Version &        0x5L  < 5) and\
             os.path.basename(comspec).lower() != "command.com":
            # NT and not using command.com.
            try:
                cmd = _whichFirstArg(cmd, env)
            except ProcessError:
                raise ProcessError("Could not find a suitable executable "\
                    "to launch for '%s'. On WinNT you must manually prefix "\
                    "shell commands and batch files with 'cmd.exe /c' to "\
                    "have the shell run them." % cmd)
        else:
            # Oh gag, we're on Win9x and/or using COMMAND.COM. Use the
            # workaround listed in KB: Q150956
            w9xpopen = os.path.join(
                os.path.dirname(win32api.GetModuleFileName(0)),
                'w9xpopen.exe')
            if not os.path.exists(w9xpopen):
                # Eeek - file-not-found - possibly an embedding
                # situation - see if we can locate it in sys.exec_prefix
                w9xpopen = os.path.join(os.path.dirname(sys.exec_prefix),
                                        'w9xpopen.exe')
                if not os.path.exists(w9xpopen):
                    raise ProcessError(\
                        "Can not locate 'w9xpopen.exe' which is needed "\
                        "for ProcessOpen to work with your shell or "\
                        "platform.")
            ## This would be option (1):
            #cmd = '%s "%s /c %s"'\
            #      % (w9xpopen, comspec, cmd.replace('"', '\\"'))
            try:
                cmd = _whichFirstArg(cmd, env)
            except ProcessError:
                raise ProcessError("Could not find a suitable executable "\
                    "to launch for '%s'. On Win9x you must manually prefix "\
                    "shell commands and batch files with 'command.com /c' "\
                    "to have the shell run them." % cmd)
            cmd = '%s "%s"' % (w9xpopen, cmd.replace('"', '\\"'))
    return cmd

class _FileWrapper:
    """Wrap a system file object, hiding some nitpicky details.
    
    This class provides a Python file-like interface to either a Python
    file object (pretty easy job), a file descriptor, or an OS-specific
    file handle (e.g.  Win32 handles to file objects on Windows). Any or
    all of these object types may be passed to this wrapper. If more
    than one is specified this wrapper prefers to work with certain one
    in this order:
        - file descriptor (because usually this allows for
          return-immediately-on-read-if-anything-available semantics and
          also provides text mode translation on Windows)
        - OS-specific handle (allows for the above read semantics)
        - file object (buffering can cause difficulty for interacting
          with spawned programs)

    It also provides a place where related such objects can be kept
    alive together to prevent premature ref-counted collection. (E.g. on
    Windows a Python file object may be associated with a Win32 file
    handle. If the file handle is not kept alive the Python file object
    will cease to function.)
    """
    def __init__(self, file=None, descriptor=None, handle=None):
        self._file = file
        self._descriptor = descriptor
        self._handle = handle
        self._closed = 0
        if self._descriptor is not None or self._handle is not None:
            self._lineBuf = "" # to support .readline()

    def __del__(self):
        self.close()

    def __getattr__(self, name):
        """Forward to the underlying file object."""
        if self._file is not None:
            return getattr(self._file, name)
        else:
            raise ProcessError("no file object to pass '%s' attribute to"
                               % name)

    def _win32Read(self, nBytes):
        try:
            log.info("[%s] _FileWrapper.read: waiting for read on pipe",
                     id(self))
            errCode, text = win32file.ReadFile(self._handle, nBytes)
        except pywintypes.error, ex:
            # Ignore errors for now, like "The pipe is being closed.",
            # etc. XXX There *may* be errors we don't want to avoid.
            log.info("[%s] _FileWrapper.read: error reading from pipe: %s",
                     id(self), ex)
            return ""
        assert errCode == 0,\
               "Why is 'errCode' from ReadFile non-zero? %r" % errCode
        if not text:
            # Empty text signifies that the pipe has been closed on
            # the parent's end.
            log.info("[%s] _FileWrapper.read: observed close of parent",
                     id(self))
            # Signal the child so it knows to stop listening.
            self.close()
            return ""
        else:
            log.info("[%s] _FileWrapper.read: read %d bytes from pipe: %r",
                     id(self), len(text), text)
        return text

    def read(self, nBytes=-1):
        # nBytes <= 0 means "read everything"
        #   Note that we are changing the "read everything" cue to
        #   include 0, because actually doing
        #   win32file.ReadFile(<handle>, 0) results in every subsequent
        #   read returning 0, i.e. it shuts down the pipe.
        if self._descriptor is not None:
            if nBytes <= 0:
                text, self._lineBuf = self._lineBuf, ""
                while 1:
                    t = os.read(self._descriptor, 4092)
                    if not t:
                        break
                    else:
                        text += t
            else:
                if len(self._lineBuf) >= nBytes:
                    text, self._lineBuf =\
                        self._lineBuf[:nBytes], self._lineBuf[nBytes:]
                else:
                    nBytesToGo = nBytes - len(self._lineBuf)
                    text = self._lineBuf + os.read(self._descriptor,
                                                   nBytesToGo)
                    self._lineBuf = ""
            return text
        elif self._handle is not None:
            if nBytes <= 0:
                text, self._lineBuf = self._lineBuf, ""
                while 1:
                    t = self._win32Read(4092)
                    if not t:
                        break
                    else:
                        text += t
            else:
                if len(self._lineBuf) >= nBytes:
                    text, self._lineBuf =\
                        self._lineBuf[:nBytes], self._lineBuf[nBytes:]
                else:
                    nBytesToGo = nBytes - len(self._lineBuf)
                    text, self._lineBuf =\
                        self._lineBuf + self._win32Read(nBytesToGo), ""
            return text
        elif self._file is not None:
            return self._file.read(nBytes)
        else:   
            raise "FileHandle.read: no handle to read with"

    def readline(self):
        if self._descriptor is not None or self._handle is not None:
            while 1:
                #XXX This is not portable to the Mac.
                idx = self._lineBuf.find('\n')
                if idx != -1:
                    line, self._lineBuf =\
                        self._lineBuf[:idx+1], self._lineBuf[idx+1:]
                    break
                else:
                    lengthBefore = len(self._lineBuf)
                    t = self.read(4092)
                    if len(t) <= lengthBefore: # no new data was read
                        line, self._lineBuf = self._lineBuf, ""
                        break
                    else:
                        self._lineBuf += t
            return line
        elif self._file is not None:
            return self._file.readline()
        else:
            raise "FileHandle.readline: no handle to read with"

    def readlines(self):
        if self._descriptor is not None or self._handle is not None:
            lines = []
            while 1:
                line = self.readline()
                if line:
                    lines.append(line)
                else:
                    break
            return lines
        elif self._file is not None:
            return self._file.readlines()
        else:
            raise "FileHandle.readline: no handle to read with"

    def write(self, text):
        if self._descriptor is not None:
            os.write(self._descriptor, text)
        elif self._handle is not None:
            try:
                errCode, nBytesWritten = win32file.WriteFile(self._handle, text)
            except pywintypes.error, ex:
                # Ingore errors like "The pipe is being closed.", for
                # now.
                log.info("[%s] _FileWrapper.write: error writing to pipe, "\
                         "ignored", id(self))
                return
            assert errCode == 0,\
                   "Why is 'errCode' from WriteFile non-zero? %r" % errCode
            if not nBytesWritten:
                # No bytes written signifies that the pipe has been
                # closed on the child's end.
                log.info("[%s] _FileWrapper.write: observed close of pipe",
                         id(self))
                return
            else:
                log.info("[%s] _FileWrapper.write: wrote %d bytes to pipe: %r",
                         id(self), len(text), text)
        elif self._file is not None:
            self._file.write(text)
        else:   
            raise "FileHandle.write: nothing to write with"

    def close(self):
        """Close all associated file objects and handles."""
        log.debug("[%s] _FileWrapper.close()", id(self))
        if not self._closed:
            self._closed = 1
            if self._file is not None:
                log.debug("[%s] _FileWrapper.close: close file", id(self))
                self._file.close()
                log.debug("[%s] _FileWrapper.close: done file close", id(self))
            if self._descriptor is not None:
                try:
                    os.close(self._descriptor)
                except OSError, ex:
                    if ex.errno == 9:
                        # Ignore: OSError: [Errno 9] Bad file descriptor
                        # XXX *Should* we be ignoring this? It appears very
                        #     *in*frequently in test_wait.py.
                        log.debug("[%s] _FileWrapper.close: closing "\
                                  "descriptor raised OSError", id(self))
                    else:
                        raise
            if self._handle is not None:
                log.debug("[%s] _FileWrapper.close: close handle", id(self))
                try:
                    win32api.CloseHandle(self._handle)
                except win32api.error:
                    log.debug("[%s] _FileWrapper.close: closing handle raised",
                              id(self))
                    pass
                log.debug("[%s] _FileWrapper.close: done closing handle",
                          id(self))

    def __repr__(self):
        return "<_FileWrapper: file:%r fd:%r os_handle:%r>"\
               % (self._file, self._descriptor, self._handle)


class _CountingCloser:
    """Call .close() on the given object after own .close() is called
    the precribed number of times.
    """
    def __init__(self, objectsToClose, count):
        """
        "objectsToClose" is a list of object on which to call .close().
        "count" is the number of times this object's .close() method
            must be called before .close() is called on the given objects.
        """
        self.objectsToClose = objectsToClose
        self.count = count
        if self.count <= 0:
            raise ProcessError("illegal 'count' value: %s" % self.count)

    def close(self):
        self.count -= 1
        log.debug("[%d] _CountingCloser.close(): count=%d", id(self),
                  self.count)
        if self.count == 0:
            for objectToClose in self.objectsToClose:
                objectToClose.close()



#---- public interface

class Process:
    """Create a process.

    One can optionally specify the starting working directory, the
    process environment, and std handles to have the child process
    inherit (all defaults are the parent's current settings). 'wait' and
    'kill' method allow for control of the child's termination.
    """
    # TODO:
    #   - Rename this or merge it with ProcessOpen somehow.
    #
    if sys.platform.startswith("win"):
        # .wait() argument constants
        INFINITE = win32event.INFINITE
        # .wait() return error codes
        WAIT_FAILED = win32event.WAIT_FAILED
        WAIT_TIMEOUT = win32event.WAIT_TIMEOUT
        # creation "flags" constants
        # XXX Should drop these and just document usage of
        #     win32process.CREATE_* constants on windows.
        CREATE_NEW_CONSOLE = win32process.CREATE_NEW_CONSOLE
    else:
        # .wait() argument constants
        INFINITE = 0
        # .wait() return error codes
        WAIT_TIMEOUT = 258
        WAIT_FAILED = -1
        # creation "flags" constants
        CREATE_NEW_CONSOLE = 0x10 # same as win32process.CREATE_NEW_CONSOLE

    def __init__(self, cmd, cwd=None, env=None, flags=0):
        """Create a child process.

        "cmd" is a command string or argument vector to spawn.
        "cwd" is a working directory in which to start the child process.
        "env" is an environment dictionary for the child.
        "flags" are system-specific process creation flags. On Windows
            this can be a bitwise-OR of any of the win32process.CREATE_*
            constants (Note: win32process.CREATE_NEW_PROCESS_GROUP is always
            OR'd in). On Unix, this is currently ignored.
        """
        log.info("Process.__init__(cmd=%r, cwd=%r, env=%r, flags=%r)",
                 cmd, cwd, env, flags)
        self._cmd = cmd
        if not self._cmd:
            raise ProcessError("You must specify a command.")
        self._cwd = cwd
        self._env = env
        self._flags = flags
        if sys.platform.startswith("win"):
            self._flags |= win32process.CREATE_NEW_PROCESS_GROUP

        if sys.platform.startswith("win"):
            self._startOnWindows()
        else:
            self.__retvalCache = None
            self._startOnUnix()

    def _runChildOnUnix(self):
        #XXX Errors running the child do *not* get communicated back.

        #XXX Perhaps we should *always* prefix with '/bin/sh -c'? There is a
        #    disparity btwn how this works on Linux and Windows.
        if isinstance(self._cmd, types.StringTypes):
            # This is easier than trying to reproduce shell interpretation to
            # separate the arguments.
            cmd = ['/bin/sh', '-c', self._cmd]
        else:
            cmd = self._cmd

        # Close all file descriptors (except std*) inherited from the parent.
        MAXFD = 256 # Max number of file descriptors (os.getdtablesize()???)
        for i in range(3, MAXFD):
            try:
                os.close(i)
            except OSError:
                pass

        try:
            if self._env:
                os.execvpe(cmd[0], cmd, self._env)
            else:
                os.execvp(cmd[0], cmd)
        finally:
            os._exit(1)  # Should never get here.

    def _forkAndExecChildOnUnix(self):
        """Fork and start the child process.

        Sets self._pid as a side effect.
        """
        pid = os.fork()
        if pid == 0: # child
            self._runChildOnUnix()
        # parent
        self._pid = pid

    def _startOnUnix(self):
        if self._cwd:
            oldDir = os.getcwd()
            try:
                os.chdir(self._cwd)
            except OSError, ex:
                raise ProcessError(msg=str(ex), errno=ex.errno)
        self._forkAndExecChildOnUnix()

        # parent
        if self._cwd:
            os.chdir(oldDir)

    def _startOnWindows(self):
        if type(self._cmd) in (types.ListType, types.TupleType):
            # And arg vector was passed in.
            cmd = _joinArgv(self._cmd)
        else:
            cmd = self._cmd

        si = win32process.STARTUPINFO() 
        si.dwFlags = win32process.STARTF_USESHOWWINDOW
        si.wShowWindow = SW_SHOWDEFAULT

        if not (self._flags & self.CREATE_NEW_CONSOLE):
            #XXX This is hacky.
            # We cannot then use _fixupCommand because this will cause a
            # shell to be openned as the command is launched. Therefore need
            # to ensure be have the full path to the executable to launch.
            try:
                cmd = _whichFirstArg(cmd, self._env)
            except ProcessError:
                # Could not find the command, perhaps it is an internal
                # shell command -- fallback to _fixupCommand
                cmd = _fixupCommand(cmd, self._env)
        else:
            cmd = _fixupCommand(cmd, self._env)
        log.debug("cmd = %r", cmd)

        # Start the child process.
        try:
            self._hProcess, self._hThread, self._processId, self._threadId\
                = _SaferCreateProcess(
                    None,           # app name
                    cmd,            # command line 
                    None,           # process security attributes 
                    None,           # primary thread security attributes 
                    0,              # handles are inherited 
                    self._flags,    # creation flags 
                    self._env,      # environment
                    self._cwd,      # current working directory
                    si)             # STARTUPINFO pointer 
            win32api.CloseHandle(self._hThread)
        except win32api.error, ex:
            raise ProcessError(msg="Error creating process for '%s': %s"\
                                   % (cmd, ex.args[2]),
                               errno=ex.args[0])

    def wait(self, timeout=None): 
        """Wait for the started process to complete.
        
        "timeout" (on Windows) is a floating point number of seconds after
            which to timeout.  Default is win32event.INFINITE.
        "timeout" (on Unix) is akin to the os.waitpid() "options" argument
            (os.WNOHANG may be used to return immediately if the process has
            not exited). Default is 0, i.e. wait forever.

        If the wait time's out it will raise a ProcessError. Otherwise it
        will return the child's exit value (on Windows) or the child's exit
        status excoded as per os.waitpid() (on Linux):
            "a 16-bit number, whose low byte is the signal number that killed
            the process, and whose high byte is the exit status (if the
            signal number is zero); the high bit of the low byte is set if a
            core file was produced."
        In the latter case, use the os.W*() methods to interpret the return
        value.
        """
        # XXX Or should returning the exit value be move out to another
        #     function as on Win32 process control? If so, then should
        #     perhaps not make WaitForSingleObject semantic transformation.
        if sys.platform.startswith("win"):
            if timeout is None:
                timeout = win32event.INFINITE
            else:
                timeout = timeout * 1000.0 # Win32 API's timeout is in millisecs

            rc = win32event.WaitForSingleObject(self._hProcess, timeout)
            if rc == win32event.WAIT_FAILED:
                raise ProcessError("'WAIT_FAILED' when waiting for process to "\
                                   "terminate: %r" % self._cmd, rc)
            elif rc == win32event.WAIT_TIMEOUT:
                raise ProcessError("'WAIT_TIMEOUT' when waiting for process to "\
                                   "terminate: %r" % self._cmd, rc)

            retval = win32process.GetExitCodeProcess(self._hProcess)
        else:
            # os.waitpid() will raise:
            #       OSError: [Errno 10] No child processes
            # on subsequent .wait() calls. Change these semantics to have
            # subsequent .wait() calls return the exit status and return
            # immediately without raising an exception.
            # (XXX It would require synchronization code to handle the case
            # of multiple simultaneous .wait() requests, however we can punt
            # on that because it is moot while Linux still has the problem
            # for which _ThreadFixer() exists.)
            if self.__retvalCache is not None:
                retval = self.__retvalCache
            else:
                if timeout is None:
                    timeout = 0
                pid, sts = os.waitpid(self._pid, timeout)
                if pid == self._pid:
                    self.__retvalCache = retval = sts
                else:
                    raise ProcessError("Wait for process timed out.",
                                       self.WAIT_TIMEOUT)
        return retval

    def kill(self, exitCode=0, gracePeriod=1.0, sig=None):
        """Kill process.
        
        "exitCode" [deprecated, not supported] (Windows only) is the
            code the terminated process should exit with.
        "gracePeriod" (Windows only) is a number of seconds the process is
            allowed to shutdown with a WM_CLOSE signal before a hard
            terminate is called.
        "sig" (Unix only) is the signal to use to kill the process. Defaults
            to signal.SIGKILL. See os.kill() for more information.

        Windows:
            Try for an orderly shutdown via WM_CLOSE.  If still running
            after gracePeriod (1 sec. default), terminate.
        """
        if sys.platform.startswith("win"):
            import win32gui
            # Send WM_CLOSE to windows in this process group.
            win32gui.EnumWindows(self._close_, 0)

            # Send Ctrl-Break signal to all processes attached to this
            # console. This is supposed to trigger shutdown handlers in
            # each of the processes.
            try:
                win32api.GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT,
                                                  self._processId)
            except AttributeError:
                log.warn("The win32api module does not have "\
                         "GenerateConsoleCtrlEvent(). This may mean that "\
                         "parts of this process group have NOT been killed.")
            except win32api.error, ex:
                if ex.args[0] not in (6, 87):
                    # Ignore the following:
                    #   api_error: (87, 'GenerateConsoleCtrlEvent', 'The parameter is incorrect.')
                    #   api_error: (6, 'GenerateConsoleCtrlEvent', 'The handle is invalid.')
                    # Get error 6 if there is no console.
                    raise
            
            # Last resort: call TerminateProcess if it has not yet.
            retval = 0
            try:
                self.wait(gracePeriod)
            except ProcessError, ex:
                log.info("[%s] Process.kill: calling TerminateProcess", id(self))
                win32process.TerminateProcess(self._hProcess, -1)
                win32api.Sleep(100) # wait for resources to be released

        else:
            if sig is None:
                sig = signal.SIGKILL
            try:
                os.kill(self._pid, sig)
            except OSError, ex:
                if ex.errno != 3:
                    # Ignore:   OSError: [Errno 3] No such process
                    raise

    def _close_(self, hwnd, dummy):
        """Callback used by .kill() on Windows.

        EnumWindows callback - sends WM_CLOSE to any window owned by this
        process.
        """
        threadId, processId = win32process.GetWindowThreadProcessId(hwnd)
        if processId == self._processId:
            import win32gui
            win32gui.PostMessage(hwnd, WM_CLOSE, 0, 0)


class ProcessOpen(Process):
    """Create a process and setup pipes to it standard handles.

    This is a super popen3.
    """
    # TODO:
    #   - Share some implementation with Process and ProcessProxy.
    #

    def __init__(self, cmd, mode='t', cwd=None, env=None):
        """Create a Process with proxy threads for each std handle.

        "cmd" is the command string or argument vector to run.
        "mode" (Windows only) specifies whether the pipes used to communicate
            with the child are openned in text, 't', or binary, 'b', mode.
            This is ignored on platforms other than Windows. Default is 't'.
        "cwd" optionally specifies the directory in which the child process
            should be started. Default is None, a.k.a. inherits the cwd from
            the parent.
        "env" is optionally a mapping specifying the environment in which to
            start the child. Default is None, a.k.a. inherits the environment
            of the parent.
        """
        # Keep a reference to ensure it is around for this object's destruction.
        self.__log = log
        log.info("ProcessOpen.__init__(cmd=%r, mode=%r, cwd=%r, env=%r)",
                 cmd, mode, cwd, env)
        self._cmd = cmd
        if not self._cmd:
            raise ProcessError("You must specify a command.")
        self._cwd = cwd
        self._env = env
        self._mode = mode
        if self._mode not in ('t', 'b'):
            raise ProcessError("'mode' must be 't' or 'b'.")
        self._closed = 0

        if sys.platform.startswith("win"):
            self._startOnWindows()
        else:
            self.__retvalCache = None
            self._startOnUnix()

        _registerProcess(self)

    def __del__(self):
        #XXX Should probably not rely upon this.
        logres.info("[%s] ProcessOpen.__del__()", id(self))
        self.close()
        del self.__log # drop reference

    def close(self):
        if not self._closed:
            self.__log.info("[%s] ProcessOpen.close()" % id(self))

            # Ensure that all IOBuffer's are closed. If they are not, these
            # can cause hangs. 
            try:
                self.__log.info("[%s] ProcessOpen: closing stdin (%r)."\
                                % (id(self), self.stdin))
                self.stdin.close()
            except AttributeError:
                # May not have gotten far enough in the __init__ to set
                # self.stdin, etc.
                pass
            try:
                self.__log.info("[%s] ProcessOpen: closing stdout (%r)."\
                                % (id(self), self.stdout))
                self.stdout.close()
            except AttributeError:
                # May not have gotten far enough in the __init__ to set
                # self.stdout, etc.
                pass
            try:
                self.__log.info("[%s] ProcessOpen: closing stderr (%r)."\
                                % (id(self), self.stderr))
                self.stderr.close()
            except AttributeError:
                # May not have gotten far enough in the __init__ to set
                # self.stderr, etc.
                pass

            self._closed = 1

    def _forkAndExecChildOnUnix(self, fdChildStdinRd, fdChildStdoutWr,
                                fdChildStderrWr):
        """Fork and start the child process.

        Sets self._pid as a side effect.
        """
        pid = os.fork()
        if pid == 0: # child
            os.dup2(fdChildStdinRd, 0)
            os.dup2(fdChildStdoutWr, 1)
            os.dup2(fdChildStderrWr, 2)
            self._runChildOnUnix()
        # parent
        self._pid = pid

    def _startOnUnix(self):
        # Create pipes for std handles.
        fdChildStdinRd, fdChildStdinWr = os.pipe()
        fdChildStdoutRd, fdChildStdoutWr = os.pipe()
        fdChildStderrRd, fdChildStderrWr = os.pipe()

        if self._cwd:
            oldDir = os.getcwd()
            try:
                os.chdir(self._cwd)
            except OSError, ex:
                raise ProcessError(msg=str(ex), errno=ex.errno)
        self._forkAndExecChildOnUnix(fdChildStdinRd, fdChildStdoutWr,
                                     fdChildStderrWr)
        if self._cwd:
            os.chdir(oldDir)

        os.close(fdChildStdinRd)
        os.close(fdChildStdoutWr)
        os.close(fdChildStderrWr)

        self.stdin = _FileWrapper(descriptor=fdChildStdinWr)
        logres.info("[%s] ProcessOpen._start(): create child stdin: %r",
                    id(self), self.stdin)
        self.stdout = _FileWrapper(descriptor=fdChildStdoutRd)
        logres.info("[%s] ProcessOpen._start(): create child stdout: %r",
                    id(self), self.stdout)
        self.stderr = _FileWrapper(descriptor=fdChildStderrRd)
        logres.info("[%s] ProcessOpen._start(): create child stderr: %r",
                    id(self), self.stderr)

    def _startOnWindows(self):
        if type(self._cmd) in (types.ListType, types.TupleType):
            # An arg vector was passed in.
            cmd = _joinArgv(self._cmd)
        else:
            cmd = self._cmd

        # Create pipes for std handles.
        # (Set the bInheritHandle flag so pipe handles are inherited.)
        saAttr = pywintypes.SECURITY_ATTRIBUTES()
        saAttr.bInheritHandle = 1
        #XXX Should maybe try with os.pipe. Dunno what that does for
        #    inheritability though.
        hChildStdinRd, hChildStdinWr = win32pipe.CreatePipe(saAttr, 0) 
        hChildStdoutRd, hChildStdoutWr = win32pipe.CreatePipe(saAttr, 0) 
        hChildStderrRd, hChildStderrWr = win32pipe.CreatePipe(saAttr, 0) 

        try:
            # Duplicate the parent ends of the pipes so they are not
            # inherited. 
            hChildStdinWrDup = win32api.DuplicateHandle(
                win32api.GetCurrentProcess(),
                hChildStdinWr,
                win32api.GetCurrentProcess(),
                0,
                0, # not inherited
                DUPLICATE_SAME_ACCESS)
            win32api.CloseHandle(hChildStdinWr)
            self._hChildStdinWr = hChildStdinWrDup
            hChildStdoutRdDup = win32api.DuplicateHandle(
                win32api.GetCurrentProcess(),
                hChildStdoutRd,
                win32api.GetCurrentProcess(),
                0,
                0, # not inherited
                DUPLICATE_SAME_ACCESS)
            win32api.CloseHandle(hChildStdoutRd)
            self._hChildStdoutRd = hChildStdoutRdDup
            hChildStderrRdDup = win32api.DuplicateHandle(
                win32api.GetCurrentProcess(),
                hChildStderrRd,
                win32api.GetCurrentProcess(),
                0,
                0, # not inherited
                DUPLICATE_SAME_ACCESS)
            win32api.CloseHandle(hChildStderrRd)
            self._hChildStderrRd = hChildStderrRdDup

            # Set the translation mode and buffering.
            if self._mode == 't':
                flags = os.O_TEXT
            else:
                flags = 0
            fdChildStdinWr = msvcrt.open_osfhandle(self._hChildStdinWr, flags)
            fdChildStdoutRd = msvcrt.open_osfhandle(self._hChildStdoutRd, flags)
            fdChildStderrRd = msvcrt.open_osfhandle(self._hChildStderrRd, flags)

            self.stdin = _FileWrapper(descriptor=fdChildStdinWr,
                                      handle=self._hChildStdinWr)
            logres.info("[%s] ProcessOpen._start(): create child stdin: %r",
                        id(self), self.stdin)
            self.stdout = _FileWrapper(descriptor=fdChildStdoutRd,
                                       handle=self._hChildStdoutRd)
            logres.info("[%s] ProcessOpen._start(): create child stdout: %r",
                        id(self), self.stdout)
            self.stderr = _FileWrapper(descriptor=fdChildStderrRd,
                                       handle=self._hChildStderrRd)
            logres.info("[%s] ProcessOpen._start(): create child stderr: %r",
                        id(self), self.stderr)

            # Start the child process.
            si = win32process.STARTUPINFO() 
            si.dwFlags = win32process.STARTF_USESHOWWINDOW
            si.wShowWindow = 0 # SW_HIDE
            si.hStdInput = hChildStdinRd
            si.hStdOutput = hChildStdoutWr
            si.hStdError = hChildStderrWr
            si.dwFlags |= win32process.STARTF_USESTDHANDLES

            cmd = _fixupCommand(cmd, self._env)

            creationFlags = win32process.CREATE_NEW_PROCESS_GROUP
            try:
                self._hProcess, hThread, self._processId, threadId\
                    = _SaferCreateProcess(
                        None,           # app name
                        cmd,            # command line 
                        None,           # process security attributes 
                        None,           # primary thread security attributes 
                        1,              # handles are inherited 
                        creationFlags,  # creation flags 
                        self._env,      # environment
                        self._cwd,      # current working directory
                        si)             # STARTUPINFO pointer 
            except win32api.error, ex:
                raise ProcessError(msg=ex.args[2], errno=ex.args[0])
            win32api.CloseHandle(hThread)

        finally:
            # Close child ends of pipes on the parent's side (the
            # parent's ends of the pipe are closed in the _FileWrappers.)
            win32file.CloseHandle(hChildStdinRd)
            win32file.CloseHandle(hChildStdoutWr)
            win32file.CloseHandle(hChildStderrWr)

    def wait(self, timeout=None): 
        """Wait for the started process to complete.
        
        "timeout" (on Windows) is a floating point number of seconds after
            which to timeout.  Default is win32event.INFINITE.
        "timeout" (on Unix) is akin to the os.waitpid() "options" argument
            (os.WNOHANG may be used to return immediately if the process has
            not exited). Default is 0, i.e. wait forever.

        If the wait time's out it will raise a ProcessError. Otherwise it
        will return the child's exit value (on Windows) or the child's exit
        status excoded as per os.waitpid() (on Linux):
            "a 16-bit number, whose low byte is the signal number that killed
            the process, and whose high byte is the exit status (if the
            signal number is zero); the high bit of the low byte is set if a
            core file was produced."
        In the latter case, use the os.W*() methods to interpret the return
        value.
        """
        # XXX Or should returning the exit value be move out to another
        #    function as on Win32 process control? If so, then should
        #    perhaps not make WaitForSingleObject semantic
        #    transformation.
        # TODO:
        #   - Need to rationalize the .wait() API for Windows vs. Unix.
        #     It is a real pain in the current situation.
        if sys.platform.startswith("win"):
            if timeout is None:
                timeout = win32event.INFINITE
            else:
                timeout = timeout * 1000.0 # Win32 API's timeout is in millisecs

            #rc = win32event.WaitForSingleObject(self._hProcess, timeout)
            rc = win32event.WaitForSingleObject(self._hProcess, int(timeout)) # MATT -- Making timeout an integer
            if rc == win32event.WAIT_FAILED:
                raise ProcessError("'WAIT_FAILED' when waiting for process to "\
                                   "terminate: %r" % self._cmd, rc)
            elif rc == win32event.WAIT_TIMEOUT:
                raise ProcessError("'WAIT_TIMEOUT' when waiting for process to "\
                                   "terminate: %r" % self._cmd, rc)

            retval = win32process.GetExitCodeProcess(self._hProcess)
        else:
            # os.waitpid() will raise:
            #       OSError: [Errno 10] No child processes
            # on subsequent .wait() calls. Change these semantics to have
            # subsequent .wait() calls return the exit status and return
            # immediately without raising an exception.
            # (XXX It would require synchronization code to handle the case
            # of multiple simultaneous .wait() requests, however we can punt
            # on that because it is moot while Linux still has the problem
            # for which _ThreadFixer() exists.)
            if self.__retvalCache is not None:
                retval = self.__retvalCache
            else:
                if timeout is None:
                    timeout = 0
                pid, sts = os.waitpid(self._pid, timeout)
                if pid == self._pid:
                    self.__retvalCache = retval = sts
                else:
                    raise ProcessError("Wait for process timed out.",
                                       self.WAIT_TIMEOUT)
        _unregisterProcess(self)
        return retval

    def kill(self, exitCode=0, gracePeriod=1.0, sig=None):
        """Kill process.
        
        "exitCode" [deprecated, not supported] (Windows only) is the
            code the terminated process should exit with.
        "gracePeriod" (Windows only) is a number of seconds the process is
            allowed to shutdown with a WM_CLOSE signal before a hard
            terminate is called.
        "sig" (Unix only) is the signal to use to kill the process. Defaults
            to signal.SIGKILL. See os.kill() for more information.

        Windows:
            Try for an orderly shutdown via WM_CLOSE.  If still running
            after gracePeriod (1 sec. default), terminate.
        """
        if sys.platform.startswith("win"):
            import win32gui
            # Send WM_CLOSE to windows in this process group.
            win32gui.EnumWindows(self._close_, 0)

            # Send Ctrl-Break signal to all processes attached to this
            # console. This is supposed to trigger shutdown handlers in
            # each of the processes.
            try:
                win32api.GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT,
                                                  self._processId)
            except AttributeError:
                log.warn("The win32api module does not have "\
                         "GenerateConsoleCtrlEvent(). This may mean that "\
                         "parts of this process group have NOT been killed.")
            except win32api.error, ex:
                if ex.args[0] not in (6, 87):
                    # Ignore the following:
                    #   api_error: (87, 'GenerateConsoleCtrlEvent', 'The parameter is incorrect.')
                    #   api_error: (6, 'GenerateConsoleCtrlEvent', 'The handle is invalid.')
                    # Get error 6 if there is no console.
                    raise
            
            # Last resort: call TerminateProcess if it has not yet.
            retval = 0
            try:
                self.wait(gracePeriod)
            except ProcessError, ex:
                log.info("[%s] Process.kill: calling TerminateProcess", id(self))
                win32process.TerminateProcess(self._hProcess, -1)
                win32api.Sleep(100) # wait for resources to be released

        else:
            if sig is None:
                sig = signal.SIGKILL
            try:
                os.kill(self._pid, sig)
            except OSError, ex:
                if ex.errno != 3:
                    # Ignore:   OSError: [Errno 3] No such process
                    raise

        _unregisterProcess(self)

    def _close_(self, hwnd, dummy):
        """Callback used by .kill() on Windows.

        EnumWindows callback - sends WM_CLOSE to any window owned by this
        process.
        """
        threadId, processId = win32process.GetWindowThreadProcessId(hwnd)
        if processId == self._processId:
            import win32gui
            win32gui.PostMessage(hwnd, WM_CLOSE, 0, 0)


class ProcessProxy(Process):
    """Create a process and proxy communication via the standard handles.
    """
    #XXX To add to docstring:
    #   - stdout/stderr proxy handling
    #   - stdin proxy handling
    #   - termination
    #   - how to .start(), i.e. basic usage rules
    #   - mention that pased in stdin/stdout/stderr objects have to
    #     implement at least .write (is .write correct for stdin)?
    #   - if you pass in stdin, stdout, and/or stderr streams it is the
    #     user's responsibility to close them afterwards.
    #   - 'cmd' arg can be a command string or an arg vector
    #   - etc.
    #TODO:
    #   - .suspend() and .resume()? See Win32::Process Perl module.
    #
    def __init__(self, cmd, mode='t', cwd=None, env=None,
                 stdin=None, stdout=None, stderr=None):
        """Create a Process with proxy threads for each std handle.

        "cmd" is the command string or argument vector to run.
        "mode" (Windows only) specifies whether the pipes used to communicate
            with the child are openned in text, 't', or binary, 'b', mode.
            This is ignored on platforms other than Windows. Default is 't'.
        "cwd" optionally specifies the directory in which the child process
            should be started. Default is None, a.k.a. inherits the cwd from
            the parent.
        "env" is optionally a mapping specifying the environment in which to
            start the child. Default is None, a.k.a. inherits the environment
            of the parent.
        "stdin", "stdout", "stderr" can be used to specify objects with
            file-like interfaces to handle read (stdout/stderr) and write
            (stdin) events from the child. By default a process.IOBuffer
            instance is assigned to each handler. IOBuffer may be
            sub-classed. See the IOBuffer doc string for more information.
        """
        # Keep a reference to ensure it is around for this object's destruction.
        self.__log = log
        log.info("ProcessProxy.__init__(cmd=%r, mode=%r, cwd=%r, env=%r, "\
                 "stdin=%r, stdout=%r, stderr=%r)",
                 cmd, mode, cwd, env, stdin, stdout, stderr)
        self._cmd = cmd
        if not self._cmd:
            raise ProcessError("You must specify a command.")
        self._mode = mode
        if self._mode not in ('t', 'b'):
            raise ProcessError("'mode' must be 't' or 'b'.")
        self._cwd = cwd
        self._env = env
        if stdin is None:
            self.stdin = IOBuffer(name='<stdin>')
        else:
            self.stdin = stdin
        if stdout is None:
            self.stdout = IOBuffer(name='<stdout>')
        else:
            self.stdout = stdout
        if stderr is None:
            self.stderr = IOBuffer(name='<stderr>')
        else:
            self.stderr = stderr
        self._closed = 0

        if sys.platform.startswith("win"):
            self._startOnWindows()
        else:
            self.__retvalCache = None
            self._startOnUnix()

        _registerProcess(self)

    def __del__(self):
        #XXX Should probably not rely upon this.
        logres.info("[%s] ProcessProxy.__del__()", id(self))
        self.close()
        del self.__log # drop reference

    def close(self):
        if not self._closed:
            self.__log.info("[%s] ProcessProxy.close()" % id(self))

            # Ensure that all IOBuffer's are closed. If they are not, these
            # can cause hangs. 
            self.__log.info("[%s] ProcessProxy: closing stdin (%r)."\
                            % (id(self), self.stdin))
            try:
                self.stdin.close()
                self._stdinProxy.join()
            except AttributeError:
                # May not have gotten far enough in the __init__ to set
                # self.stdin, etc.
                pass
            self.__log.info("[%s] ProcessProxy: closing stdout (%r)."\
                            % (id(self), self.stdout))
            try:
                self.stdout.close()
                if self._stdoutProxy is not threading.currentThread():
                    self._stdoutProxy.join()
            except AttributeError:
                # May not have gotten far enough in the __init__ to set
                # self.stdout, etc.
                pass
            self.__log.info("[%s] ProcessProxy: closing stderr (%r)."\
                            % (id(self), self.stderr))
            try:
                self.stderr.close()
                if self._stderrProxy is not threading.currentThread():
                    self._stderrProxy.join()
            except AttributeError:
                # May not have gotten far enough in the __init__ to set
                # self.stderr, etc.
                pass

            self._closed = 1

    def _forkAndExecChildOnUnix(self, fdChildStdinRd, fdChildStdoutWr,
                                fdChildStderrWr):
        """Fork and start the child process.

        Sets self._pid as a side effect.
        """
        pid = os.fork()
        if pid == 0: # child
            os.dup2(fdChildStdinRd, 0)
            os.dup2(fdChildStdoutWr, 1)
            os.dup2(fdChildStderrWr, 2)
            self._runChildOnUnix()
        # parent
        self._pid = pid

    def _startOnUnix(self):
        # Create pipes for std handles.
        fdChildStdinRd, fdChildStdinWr = os.pipe()
        fdChildStdoutRd, fdChildStdoutWr = os.pipe()
        fdChildStderrRd, fdChildStderrWr = os.pipe()

        if self._cwd:
            oldDir = os.getcwd()
            try:
                os.chdir(self._cwd)
            except OSError, ex:
                raise ProcessError(msg=str(ex), errno=ex.errno)
        self._forkAndExecChildOnUnix(fdChildStdinRd, fdChildStdoutWr,
                                     fdChildStderrWr)
        if self._cwd:
            os.chdir(oldDir)

        os.close(fdChildStdinRd)
        os.close(fdChildStdoutWr)
        os.close(fdChildStderrWr)

        childStdin = _FileWrapper(descriptor=fdChildStdinWr)
        logres.info("[%s] ProcessProxy._start(): create child stdin: %r",
                    id(self), childStdin)
        childStdout = _FileWrapper(descriptor=fdChildStdoutRd)
        logres.info("[%s] ProcessProxy._start(): create child stdout: %r",
                    id(self), childStdout)
        childStderr = _FileWrapper(descriptor=fdChildStderrRd)
        logres.info("[%s] ProcessProxy._start(): create child stderr: %r",
                    id(self), childStderr)

        # Create proxy threads for the out pipes.
        self._stdinProxy = _InFileProxy(self.stdin, childStdin, name='<stdin>')
        self._stdinProxy.start()
        # Clean up the parent's side of <stdin> when it is observed that
        # the child has closed its side of <stdout> and <stderr>. (This
        # is one way of determining when it is appropriate to clean up
        # this pipe, with compromises. See the discussion at the top of
        # this module.)
        closer = _CountingCloser([self.stdin, childStdin, self], 2)
        self._stdoutProxy = _OutFileProxy(childStdout, self.stdout, 
                                          [closer],
                                          name='<stdout>')
        self._stdoutProxy.start()
        self._stderrProxy = _OutFileProxy(childStderr, self.stderr,
                                          [closer],
                                          name='<stderr>')
        self._stderrProxy.start()

    def _startOnWindows(self):
        if type(self._cmd) in (types.ListType, types.TupleType):
            # An arg vector was passed in.
            cmd = _joinArgv(self._cmd)
        else:
            cmd = self._cmd

        # Create pipes for std handles.
        # (Set the bInheritHandle flag so pipe handles are inherited.)
        saAttr = pywintypes.SECURITY_ATTRIBUTES()
        saAttr.bInheritHandle = 1
        #XXX Should maybe try with os.pipe. Dunno what that does for
        #    inheritability though.
        hChildStdinRd, hChildStdinWr = win32pipe.CreatePipe(saAttr, 0) 
        hChildStdoutRd, hChildStdoutWr = win32pipe.CreatePipe(saAttr, 0) 
        hChildStderrRd, hChildStderrWr = win32pipe.CreatePipe(saAttr, 0) 

        try:
            # Duplicate the parent ends of the pipes so they are not
            # inherited. 
            hChildStdinWrDup = win32api.DuplicateHandle(
                win32api.GetCurrentProcess(),
                hChildStdinWr,
                win32api.GetCurrentProcess(),
                0,
                0, # not inherited
                DUPLICATE_SAME_ACCESS)
            win32api.CloseHandle(hChildStdinWr)
            self._hChildStdinWr = hChildStdinWrDup
            hChildStdoutRdDup = win32api.DuplicateHandle(
                win32api.GetCurrentProcess(),
                hChildStdoutRd,
                win32api.GetCurrentProcess(),
                0,
                0, # not inherited
                DUPLICATE_SAME_ACCESS)
            win32api.CloseHandle(hChildStdoutRd)
            self._hChildStdoutRd = hChildStdoutRdDup
            hChildStderrRdDup = win32api.DuplicateHandle(
                win32api.GetCurrentProcess(),
                hChildStderrRd,
                win32api.GetCurrentProcess(),
                0,
                0, # not inherited
                DUPLICATE_SAME_ACCESS)
            win32api.CloseHandle(hChildStderrRd)
            self._hChildStderrRd = hChildStderrRdDup

            # Set the translation mode.
            if self._mode == 't':
                flags = os.O_TEXT
                mode = ''
            else:
                flags = 0
                mode = 'b'
            fdChildStdinWr = msvcrt.open_osfhandle(self._hChildStdinWr, flags)
            fdChildStdoutRd = msvcrt.open_osfhandle(self._hChildStdoutRd, flags)
            fdChildStderrRd = msvcrt.open_osfhandle(self._hChildStderrRd, flags)

            childStdin = _FileWrapper(descriptor=fdChildStdinWr,
                                      handle=self._hChildStdinWr)
            logres.info("[%s] ProcessProxy._start(): create child stdin: %r",
                        id(self), childStdin)
            childStdout = _FileWrapper(descriptor=fdChildStdoutRd,
                                       handle=self._hChildStdoutRd)
            logres.info("[%s] ProcessProxy._start(): create child stdout: %r",
                        id(self), childStdout)
            childStderr = _FileWrapper(descriptor=fdChildStderrRd,
                                       handle=self._hChildStderrRd)
            logres.info("[%s] ProcessProxy._start(): create child stderr: %r",
                        id(self), childStderr)

            # Start the child process.
            si = win32process.STARTUPINFO() 
            si.dwFlags = win32process.STARTF_USESHOWWINDOW
            si.wShowWindow = 0 # SW_HIDE
            si.hStdInput = hChildStdinRd
            si.hStdOutput = hChildStdoutWr
            si.hStdError = hChildStderrWr
            si.dwFlags |= win32process.STARTF_USESTDHANDLES

            cmd = _fixupCommand(cmd, self._env)
            log.debug("cmd = %r", cmd)

            creationFlags = win32process.CREATE_NEW_PROCESS_GROUP
            try:
                self._hProcess, hThread, self._processId, threadId\
                    = _SaferCreateProcess(
                        None,           # app name
                        cmd,            # command line 
                        None,           # process security attributes 
                        None,           # primary thread security attributes 
                        1,              # handles are inherited 
                        creationFlags,  # creation flags 
                        self._env,      # environment
                        self._cwd,      # current working directory
                        si)             # STARTUPINFO pointer 
            except win32api.error, ex:
                raise ProcessError(msg=ex.args[2], errno=ex.args[0])
            win32api.CloseHandle(hThread)

        finally:
            # Close child ends of pipes on the parent's side (the
            # parent's ends of the pipe are closed in the _FileWrappers.)
            win32file.CloseHandle(hChildStdinRd)
            win32file.CloseHandle(hChildStdoutWr)
            win32file.CloseHandle(hChildStderrWr)

        # Create proxy threads for the pipes.
        self._stdinProxy = _InFileProxy(self.stdin, childStdin, name='<stdin>')
        self._stdinProxy.start()
        # Clean up the parent's side of <stdin> when it is observed that
        # the child has closed its side of <stdout>. (This is one way of
        # determining when it is appropriate to clean up this pipe, with
        # compromises. See the discussion at the top of this module.)
        self._stdoutProxy = _OutFileProxy(childStdout, self.stdout, 
                                          [self.stdin, childStdin, self],
                                          name='<stdout>')
        self._stdoutProxy.start()
        self._stderrProxy = _OutFileProxy(childStderr, self.stderr,
                                          name='<stderr>')
        self._stderrProxy.start()

    def wait(self, timeout=None): 
        """Wait for the started process to complete.
        
        "timeout" (on Windows) is a floating point number of seconds after
            which to timeout.  Default is win32event.INFINITE.
        "timeout" (on Unix) is akin to the os.waitpid() "options" argument
            (os.WNOHANG may be used to return immediately if the process has
            not exited). Default is 0, i.e. wait forever.

        If the wait time's out it will raise a ProcessError. Otherwise it
        will return the child's exit value (on Windows) or the child's exit
        status excoded as per os.waitpid() (on Linux):
            "a 16-bit number, whose low byte is the signal number that killed
            the process, and whose high byte is the exit status (if the
            signal number is zero); the high bit of the low byte is set if a
            core file was produced."
        In the latter case, use the os.W*() methods to interpret the return
        value.
        """
        # XXX Or should returning the exit value be move out to another
        #     function as on Win32 process control? If so, then should
        #     perhaps not make WaitForSingleObject semantic transformation.
        if sys.platform.startswith("win"):
            if timeout is None:
                timeout = win32event.INFINITE
            else:
                timeout = timeout * 1000.0 # Win32 API's timeout is in millisecs

            rc = win32event.WaitForSingleObject(self._hProcess, timeout)
            if rc == win32event.WAIT_FAILED:
                raise ProcessError("'WAIT_FAILED' when waiting for process to "\
                                   "terminate: %r" % self._cmd, rc)
            elif rc == win32event.WAIT_TIMEOUT:
                raise ProcessError("'WAIT_TIMEOUT' when waiting for process to "\
                                   "terminate: %r" % self._cmd, rc)

            retval = win32process.GetExitCodeProcess(self._hProcess)
        else:
            # os.waitpid() will raise:
            #       OSError: [Errno 10] No child processes
            # on subsequent .wait() calls. Change these semantics to have
            # subsequent .wait() calls return the exit status and return
            # immediately without raising an exception.
            # (XXX It would require synchronization code to handle the case
            # of multiple simultaneous .wait() requests, however we can punt
            # on that because it is moot while Linux still has the problem
            # for which _ThreadFixer() exists.)
            if self.__retvalCache is not None:
                retval = self.__retvalCache
            else:
                if timeout is None:
                    timeout = 0
                pid, sts = os.waitpid(self._pid, timeout)
                if pid == self._pid:
                    self.__retvalCache = retval = sts
                else:
                    raise ProcessError("Wait for process timed out.",
                                       self.WAIT_TIMEOUT)
        _unregisterProcess(self)
        return retval

    def kill(self, exitCode=0, gracePeriod=1.0, sig=None):
        """Kill process.
        
        "exitCode" [deprecated, not supported] (Windows only) is the
            code the terminated process should exit with.
        "gracePeriod" (Windows only) is a number of seconds the process is
            allowed to shutdown with a WM_CLOSE signal before a hard
            terminate is called.
        "sig" (Unix only) is the signal to use to kill the process. Defaults
            to signal.SIGKILL. See os.kill() for more information.

        Windows:
            Try for an orderly shutdown via WM_CLOSE.  If still running
            after gracePeriod (1 sec. default), terminate.
        """
        if sys.platform.startswith("win"):
            import win32gui
            # Send WM_CLOSE to windows in this process group.
            win32gui.EnumWindows(self._close_, 0)

            # Send Ctrl-Break signal to all processes attached to this
            # console. This is supposed to trigger shutdown handlers in
            # each of the processes.
            try:
                win32api.GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT,
                                                  self._processId)
            except AttributeError:
                log.warn("The win32api module does not have "\
                         "GenerateConsoleCtrlEvent(). This may mean that "\
                         "parts of this process group have NOT been killed.")
            except win32api.error, ex:
                if ex.args[0] not in (6, 87):
                    # Ignore the following:
                    #   api_error: (87, 'GenerateConsoleCtrlEvent', 'The parameter is incorrect.')
                    #   api_error: (6, 'GenerateConsoleCtrlEvent', 'The handle is invalid.')
                    # Get error 6 if there is no console.
                    raise
            
            # Last resort: call TerminateProcess if it has not yet.
            retval = 0
            try:
                self.wait(gracePeriod)
            except ProcessError, ex:
                log.info("[%s] Process.kill: calling TerminateProcess", id(self))
                win32process.TerminateProcess(self._hProcess, -1)
                win32api.Sleep(100) # wait for resources to be released

        else:
            if sig is None:
                sig = signal.SIGKILL
            try:
                os.kill(self._pid, sig)
            except OSError, ex:
                if ex.errno != 3:
                    # Ignore:   OSError: [Errno 3] No such process
                    raise

        _unregisterProcess(self)

    def _close_(self, hwnd, dummy):
        """Callback used by .kill() on Windows.

        EnumWindows callback - sends WM_CLOSE to any window owned by this
        process.
        """
        threadId, processId = win32process.GetWindowThreadProcessId(hwnd)
        if processId == self._processId:
            import win32gui
            win32gui.PostMessage(hwnd, WM_CLOSE, 0, 0)


class IOBuffer:
    """Want to be able to both read and write to this buffer from
    difference threads and have the same read/write semantics as for a
    std handler.

    This class is subclass-able. _doRead(), _doWrite(), _doReadline(),
    _doClose(), _haveLine(), and _haveNumBytes() can be overridden for
    specific functionality. The synchronization issues (block on read
    until write provides the needed data, termination) are handled for
    free.

    Cannot support:
        .seek()     # Because we are managing *two* positions (one each
        .tell()     #   for reading and writing), these do not make
                    #   sense.
    """
    #TODO:
    #   - Is performance a problem? This will likely be slower that
    #     StringIO.StringIO().
    #
    def __init__(self, mutex=None, stateChange=None, name=None):
        """'name' can be set for debugging, it will be used in log messages."""
        if name is not None:
            self._name = name
        else:
            self._name = id(self)
        log.info("[%s] IOBuffer.__init__()" % self._name)

        self.__buf = ''
        # A state change is defined as the buffer being closed or a
        # write occuring.
        if mutex is not None:
            self._mutex = mutex
        else:
            self._mutex = threading.Lock()
        if stateChange is not None:
            self._stateChange = stateChange
        else:
            self._stateChange = threading.Condition()
        self._closed = 0

    def _doWrite(self, s):
        self.__buf += s  # Append to buffer.

    def write(self, s):
        log.info("[%s] IOBuffer.write(s=%r)", self._name, s)
        # Silently drop writes after the buffer has been close()'d.
        if self._closed:
            return
        # If empty write, close buffer (mimicking behaviour from
        # koprocess.cpp.)
        if not s:
            self.close()
            return

        self._mutex.acquire()
        self._doWrite(s)
        self._stateChange.acquire()
        self._stateChange.notifyAll()   # Notify of the write().
        self._stateChange.release()
        self._mutex.release()

    def writelines(self, list):
        self.write(''.join(list))

    def _doRead(self, n):
        """Pop 'n' bytes from the internal buffer and return them."""
        if n < 0:
            idx = len(self.__buf)
        else:
            idx = min(n, len(self.__buf))
        retval, self.__buf = self.__buf[:idx], self.__buf[idx:]
        return retval

    def read(self, n=-1):
        log.info("[%s] IOBuffer.read(n=%r)" % (self._name, n))
        log.info("[%s] IOBuffer.read(): wait for data" % self._name)
        if n < 0:
            # Wait until the buffer is closed, i.e. no more writes will
            # come.
            while 1:
                if self._closed: break
                #log.debug("[%s]     <<< IOBuffer.read: state change .wait()"\
                #          % self._name)
                self._stateChange.acquire()
                self._stateChange.wait()
                self._stateChange.release()
                #log.debug("[%s]     >>> IOBuffer.read: done change .wait()"\
                #          % self._name)
        else:
            # Wait until there are the requested number of bytes to read
            # (or until the buffer is closed, i.e. no more writes will
            # come).
            # XXX WARNING: I *think* there is a race condition around
            #     here whereby self.fparent.read() in _InFileProxy can
            #     hang. *Sometime* test_stdin::test_stdin_buffer() will
            #     hang. This was *before* I moved the
            #     _stateChange.acquire() and .release() calls out side
            #     of the 'while 1:' here. ...and now they are back
            #     inside.
            while 1:
                if self._closed: break
                if self._haveNumBytes(n): break
                #log.debug("[%s]     <<< IOBuffer.read: state change .wait()"\
                #          % self._name)
                self._stateChange.acquire()
                self._stateChange.wait()
                self._stateChange.release()
                #log.debug("[%s]     >>> IOBuffer.read: done change .wait()"\
                #          % self._name)
        log.info("[%s] IOBuffer.read(): done waiting for data" % self._name)

        self._mutex.acquire()
        retval = self._doRead(n)
        self._mutex.release()
        return retval

    def _doReadline(self, n):
        """Pop the front line (or n bytes of it, whichever is less) from
        the internal buffer and return it.
        """
        idx = self.__buf.find('\n')
        if idx == -1:
            idx = len(self.__buf)
        else:
            idx += 1 # include the '\n'
        if n is not None:
            idx = min(idx, n) 
        retval, self.__buf = self.__buf[:idx], self.__buf[idx:]
        return retval

    def _haveLine(self):
        return self.__buf.find('\n') != -1

    def _haveNumBytes(self, n=None):
        return len(self.__buf) >= n

    def readline(self, n=None):
        # Wait until there is a full line (or at least 'n' bytes)
        # in the buffer or until the buffer is closed, i.e. no more
        # writes will come.
        log.info("[%s] IOBuffer.readline(n=%r)" % (self._name, n))

        log.info("[%s] IOBuffer.readline(): wait for data" % self._name)
        while 1:
            if self._closed: break
            if self._haveLine(): break
            if n is not None and self._haveNumBytes(n): break
            self._stateChange.acquire()
            self._stateChange.wait()
            self._stateChange.release()
        log.info("[%s] IOBuffer.readline(): done waiting for data"\
                 % self._name)

        self._mutex.acquire()
        retval = self._doReadline(n)
        self._mutex.release()
        return retval

    def readlines(self):
        lines = []
        while 1:
            line = self.readline()
            if line:
                lines.append(line)
            else:
                break
        return lines

    def _doClose(self):
        pass

    def close(self):
        if not self._closed:
            log.info("[%s] IOBuffer.close()" % self._name)
            self._doClose()
            self._closed = 1
            self._stateChange.acquire()
            self._stateChange.notifyAll()   # Notify of the close().
            self._stateChange.release()

    def flush(self):
        log.info("[%s] IOBuffer.flush()" % self._name)
        #XXX Perhaps flush() should unwedged possible waiting .read()
        #    and .readline() calls that are waiting for more data???


class _InFileProxy(threading.Thread):
    """A thread to proxy stdin.write()'s from the parent to the child."""
    def __init__(self, fParent, fChild, name=None):
        """
        "fParent" is a Python file-like object setup for writing.
        "fChild" is a Win32 handle to the a child process' output pipe.
        "name" can be set for debugging, it will be used in log messages.
        """
        log.info("[%s, %s] _InFileProxy.__init__(fChild=%r, fParent=%r)",
                 name, id(self), fChild, fParent)
        threading.Thread.__init__(self, name=name)
        self.fChild = fChild
        self.fParent = fParent

    def run(self):
        log.info("[%s] _InFileProxy: start" % self.getName())
        try:
            self._proxyFromParentToChild()
        finally:
            log.info("[%s] _InFileProxy: closing parent (%r)"\
                     % (self.getName(), self.fParent))
            try:
                self.fParent.close()
            except IOError:
                pass # Ignore: IOError: [Errno 4] Interrupted system call
        log.info("[%s] _InFileProxy: done" % self.getName())

    def _proxyFromParentToChild(self):
        CHUNKSIZE = 4096
        # Read output from the child process, and (for now) just write
        # it out.
        while 1:
            log.info("[%s] _InFileProxy: waiting for read on parent (%r)"\
                     % (self.getName(), self.fParent))
            # XXX Get hangs here (!) even with
            #     self.stdin.close() in ProcessProxy' __del__() under this
            #     cond:
            #           p = ProcessProxy([...], stdin=sys.stdin)
            #     The user must manually send '\n' via <Enter> or EOF
            #     via <Ctrl-Z> to unlock this. How to get around that?
            #     See cleanOnTermination note in _OutFileProxy.run()
            #     below.
            #log.debug("XXX          -> start read on %r" % self.fParent)
            try:
                text = self.fParent.read(CHUNKSIZE)
            except ValueError, ex:
                # ValueError is raised with trying to write to a closed
                # file/pipe.
                text = None
            #log.debug("XXX          <- done read on %r" % self.fParent)
            if not text:
                # Empty text signifies that the pipe has been closed on
                # the parent's end.
                log.info("[%s] _InFileProxy: observed close of parent (%r)"\
                         % (self.getName(), self.fParent))
                # Signal the child so it knows to stop listening.
                try:
                    logres.info("[%s] _InFileProxy: closing child after "\
                                "observing parent's close: %r", self.getName(),
                                self.fChild)
                    try:
                        self.fChild.close()
                    except IOError:
                        pass # Ignore: IOError: [Errno 4] Interrupted system call
                except IOError, ex:
                    # Ignore: IOError: [Errno 9] Bad file descriptor
                    # XXX Do we *know* we want to do that?
                    pass
                break
            else:
                log.info("[%s] _InFileProxy: read %d bytes from parent: %r"\
                         % (self.getName(), len(text), text))

            log.info("[%s, %s] _InFileProxy: writing %r to child (%r)",
                     self.getName(), id(self), text, self.fChild)
            try:
                self.fChild.write(text)
            except (OSError, IOError), ex:
                # Ignore errors for now. For example:
                # - Get this on Win9x when writing multiple lines to "dir":
                #   OSError: [Errno 32] Broken pipe
                #XXX There *may* be errors we don't want to avoid.
                #XXX Should maybe just ignore EnvironmentError (base class).
                log.info("[%s] _InFileProxy: error writing to child (%r), "\
                         "closing: %s" % (self.getName(), self.fParent, ex))
                break
            log.info("[%s] _InFileProxy: wrote %d bytes to child: %r"\
                     % (self.getName(), len(text), text))


class _OutFileProxy(threading.Thread):
    """A thread to watch an "out" file from the spawned child process
    and pass on write's to the parent.
    """
    def __init__(self, fChild, fParent, toClose=[], name=None):
        """
        "fChild" is a Win32 handle to the a child process' output pipe.
        "fParent" is a Python file-like object setup for writing.
        "toClose" is a list of objects on which to call .close when this
            proxy is terminating.
        "name" can be set for debugging, it will be used in log messages.
        """
        log.info("[%s] _OutFileProxy.__init__(fChild=%r, fParent=%r, "\
                 "toClose=%r)", name, fChild, fParent, toClose)
        threading.Thread.__init__(self, name=name)
        self.fChild = fChild
        self.fParent = fParent
        self.toClose = toClose

    def run(self):
        log.info("[%s] _OutFileProxy: start" % self.getName())
        try:
            self._proxyFromChildToParent()
        finally:
            logres.info("[%s] _OutFileProxy: terminating, close child (%r)",
                        self.getName(), self.fChild)
            try:
                self.fChild.close()
            except IOError:
                pass # Ignore: IOError: [Errno 4] Interrupted system call
            log.info("[%s] _OutFileProxy: closing parent (%r)",
                     self.getName(), self.fParent)
            try:
                self.fParent.close()
            except IOError:
                pass # Ignore: IOError: [Errno 4] Interrupted system call
            while self.toClose:
                logres.info("[%s] _OutFileProxy: closing %r after "\
                            "closing parent", self.getName(), self.toClose[0])
                try:
                    self.toClose[0].close()
                except IOError:
                    pass # Ignore: IOError: [Errno 4] Interrupted system call
                del self.toClose[0]
        log.info("[%s] _OutFileProxy: done" % self.getName())

    def _proxyFromChildToParent(self):
        CHUNKSIZE = 4096
        # Read output from the child process, and (for now) just write
        # it out.
        while 1:
            text = None
            try:
                log.info("[%s] _OutFileProxy: waiting for read on child (%r)"\
                         % (self.getName(), self.fChild))
                text = self.fChild.read(CHUNKSIZE)
            except IOError, ex:
                # Ignore: IOError: [Errno 9] Bad file descriptor
                # XXX Do we *know* we want to do that?
                log.info("[%s] _OutFileProxy: error reading from child (%r), "\
                         "shutting down: %s", self.getName(), self.fChild, ex)
                break
            if not text:
                # Empty text signifies that the pipe has been closed on
                # the child's end.
                log.info("[%s] _OutFileProxy: observed close of child (%r)"\
                         % (self.getName(), self.fChild))
                break

            log.info("[%s] _OutFileProxy: text(len=%d): %r",
                     self.getName(), len(text), text)
            self.fParent.write(text)



if sys.platform.startswith("linux"):
    class _ThreadFixer:
        """Mixin class for various classes in the Process hierarchy to
        work around the known LinuxThreads bug where one cannot .wait()
        on a created process from a subthread of the thread that created
        the process.

        Usage:
            class ProcessXXX(_ThreadFixer, BrokenProcessXXX):
                _pclass = BrokenProcessXXX

        Details:
            Because we must do all real os.wait() calls on the child
            process from the thread that spawned it, we use a proxy
            thread whose only responsibility is just that. The proxy
            thread just starts the child and then immediately wait's for
            the child to terminate. On termination is stores the exit
            status (for use by the main thread) and notifies any thread
            waiting for this termination (possibly the main thread). The
            overriden .wait() uses this stored exit status and the
            termination notification to simulate the .wait().
        """
        def __init__(self, *args, **kwargs):
            # Keep a reference to 'log' ensure it is around for this object's
            # destruction.
            self.__log = log
            self.__waiter = None
            self.__hasTerminated = threading.Condition()
            self.__terminationResult = None
            self.__childStarted = threading.Condition()
            self._pclass.__init__(self, *args, **kwargs)

        def _forkAndExecChildOnUnix(self, *args, **kwargs):
            """Fork and start the child process do it in a special subthread
            that will negotiate subsequent .wait()'s.

            Sets self._pid as a side effect.
            """
            self.__waiter = threading.Thread(
                target=self.__launchAndWait, args=args, kwargs=kwargs)

            # Start subthread that will launch child and wait until it
            # *has* started.
            self.__childStarted.acquire()
            self.__waiter.start()
            self.__childStarted.wait()
            self.__childStarted.release()

        def __launchAndWait(self, *args, **kwargs):
            """Launch the given command and wait for it to terminate.

            When the process has terminated then store its exit value
            and finish.
            """
            logfix.info("start child in thread %s",
                        threading.currentThread().getName())

            # Spawn the child process and notify the main thread of
            # this.
            self.__childStarted.acquire()
            self._pclass._forkAndExecChildOnUnix(self, *args, **kwargs)
            self.__childStarted.notifyAll()
            self.__childStarted.release()

            # Wait on the thread and store appropriate results when
            # finished.
            try:
                waitResult = self._pclass.wait(self)
            except ProcessError, ex:
                waitResult = ex
            self.__hasTerminated.acquire()
            self.__terminationResult = waitResult
            self.__hasTerminated.notifyAll()
            self.__hasTerminated.release()

            self.__waiter = None # drop ref that would keep instance alive
        
        def wait(self, timeout=None): 
            # If the process __hasTerminated then return the exit
            # status. Otherwise simulate the wait as appropriate.
            # Note:
            #   - This class is only used on linux so 'timeout' has the
            #     Unix 'timeout' semantics.
            self.__hasTerminated.acquire()
            if self.__terminationResult is None:
                if timeout == os.WNOHANG:   # Poll.
                    self.__hasTerminated.wait(0)
                else:                       # Block until process finishes.
                    self.__hasTerminated.wait()
            terminationResult = self.__terminationResult
            self.__hasTerminated.release()

            if terminationResult is None:
                # process has not finished yet
                raise ProcessError("Wait for process timed out.",
                                   self.WAIT_TIMEOUT)
            elif isinstance(terminationResult, Exception):
                # some error waiting for process termination
                raise terminationResult
            else:
                # the process terminated
                return terminationResult

    _ThreadBrokenProcess = Process
    class Process(_ThreadFixer, _ThreadBrokenProcess):
        _pclass = _ThreadBrokenProcess

    _ThreadBrokenProcessOpen = ProcessOpen
    class ProcessOpen(_ThreadFixer, _ThreadBrokenProcessOpen):
        _pclass = _ThreadBrokenProcessOpen

    _ThreadBrokenProcessProxy = ProcessProxy
    class ProcessProxy(_ThreadFixer, _ThreadBrokenProcessProxy):
        _pclass = _ThreadBrokenProcessProxy


