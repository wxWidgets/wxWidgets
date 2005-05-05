#!/usr/bin/env python

# Copyright (c) 2001-2004, MetaSlash Inc.  All rights reserved.

"""
Copyright notice from pychecker:
    
Copyright (c) 2000-2001, MetaSlash Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

 - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

 - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.

 - Neither name of MetaSlash Inc. nor the names of contributors
   may be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
"""


"""
Check python source code files for possible errors and print warnings

Contact Info:
  http://pychecker.sourceforge.net/
  pychecker-list@lists.sourceforge.net
"""

import string
import types
import sys
import imp
import os
import glob
import traceback
import re
import wx
_ = wx.GetTranslation

# see __init__.py for meaning, this must match the version there
LOCAL_MAIN_VERSION = 1


def setupNamespace(path) :
    # remove pychecker if it's the first component, it needs to be last
    if sys.path[0][-9:] == 'pychecker' :
        del sys.path[0]

    # make sure pychecker is last in path, so we can import
    checker_path = os.path.dirname(os.path.dirname(path))
    if checker_path not in sys.path :
        sys.path.append(checker_path)

if __name__ == '__main__' :
    setupNamespace(sys.argv[0])

from pychecker import utils
from pychecker import printer
from pychecker import warn
from pychecker import OP
from pychecker import Config
from pychecker import function
from pychecker.Warning import Warning

# Globals for storing a dictionary of info about modules and classes
_allModules = {}
_cfg = None

# Constants
_DEFAULT_MODULE_TOKENS = ('__builtins__', '__doc__', '__file__', '__name__',
                          '__path__')
_DEFAULT_CLASS_TOKENS = ('__doc__', '__name__', '__module__')

_VERSION_MISMATCH_ERROR = '''
There seem to be two versions of PyChecker being used.
One is probably in python/site-packages, the other in a local directory.
If you want to run the local version, you must remove the version
from site-packages.  Or you can install the current version
by doing python setup.py install.
'''

def cfg() :
    return utils.cfg()

def _flattenList(list) :
    "Returns a list which contains no lists"

    new_list = []
    for element in list :
        if type(element) == types.ListType :
            new_list.extend(_flattenList(element))
        else :
            new_list.append(element)

    return new_list

def getModules(arg_list) :
    "Returns a list of module names that can be imported"

    global _output
    
    new_arguments = []
    for arg in arg_list :
        # is this a wildcard filespec? (necessary for windows)
        if '*' in arg or '?' in arg or '[' in arg :
            arg = glob.glob(arg)
        new_arguments.append(arg)

    PY_SUFFIXES = ['.py']
    PY_SUFFIX_LENS = [3]
    if _cfg.quixote:
        PY_SUFFIXES.append('.ptl')
        PY_SUFFIX_LENS.append(4)
        
    modules = []
    for arg in _flattenList(new_arguments) :
        fullpath = arg
        # is it a .py file?
        for suf, suflen in zip(PY_SUFFIXES, PY_SUFFIX_LENS):
            if len(arg) > suflen and arg[-suflen:] == suf:
                arg_dir = os.path.dirname(arg)
                if arg_dir and not os.path.exists(arg) :
                    txt = _('File or pathname element does not exist: "%s"') % arg
                    _output.AddLines(txt)
                    continue

                module_name = os.path.basename(arg)[:-suflen]
                if arg_dir not in sys.path :
                    sys.path.insert(0, arg_dir)
                arg = module_name
        modules.append((arg, fullpath))

    return modules

def _q_file(f):
    # crude hack!!!
    # imp.load_module requires a real file object, so we can't just
    # fiddle def lines and yield them
    import tempfile
    fd, newfname = tempfile.mkstemp(suffix=".py", text=True)
    newf = os.fdopen(fd, 'r+')
    os.unlink(newfname)
    for line in f:
        mat = re.match(r'(\s*def\s+\w+\s*)\[(html|plain)\](.*)', line)
        if mat is None:
            newf.write(line)
        else:
            newf.write(mat.group(1)+mat.group(3)+'\n')
    newf.seek(0)
    return newf

def _q_find_module(p, path):
    if not _cfg.quixote:
        return imp.find_module(p, path)
    else:
        for direc in path:
            try:
                return imp.find_module(p, [direc])
            except ImportError:
                f = os.path.join(direc, p+".ptl")
                if os.path.exists(f):
                    return _q_file(file(f)), f, ('.ptl', 'U', 1)

def _findModule(name) :
    """Returns the result of an imp.find_module(), ie, (file, filename, smt)
       name can be a module or a package name.  It is *not* a filename."""

    path = sys.path[:]
    packages = string.split(name, '.')
    for p in packages :
        # smt = (suffix, mode, type)
        file, filename, smt = _q_find_module(p, path)
        if smt[-1] == imp.PKG_DIRECTORY :
            try :
                # package found - read path info from init file
                m = imp.load_module(p, file, filename, smt)
            finally :
                if file is not None :
                    file.close()

            # importing xml plays a trick, which replaces itself with _xmlplus
            # both have subdirs w/same name, but different modules in them
            # we need to choose the real (replaced) version
            if m.__name__ != p :
                try :
                    file, filename, smt = _q_find_module(m.__name__, path)
                    m = imp.load_module(p, file, filename, smt)
                finally :
                    if file is not None :
                        file.close()

            new_path = m.__path__
            if type(new_path) == types.ListType :
                new_path = filename
            if new_path not in path :
                path.insert(1, new_path)
        elif smt[-1] != imp.PY_COMPILED:
            if p is not packages[-1] :
                if file is not None :
                    file.close()
                raise ImportError, "No module named %s" % packages[-1]
            return file, filename, smt

    # in case we have been given a package to check
    return file, filename, smt


class Variable :
    "Class to hold all information about a variable"

    def __init__(self, name, type):
        self.name = name
        self.type = type
        self.value = None

    def __str__(self) :
        return self.name

    __repr__ = utils.std_repr


def _filterDir(object, ignoreList) :
    "Return a list of tokens (attributes) in a class, except for ignoreList"

    tokens = dir(object)
    for token in ignoreList :
        if token in tokens :
            tokens.remove(token)
    return tokens

def _getClassTokens(c) :
    return _filterDir(c, _DEFAULT_CLASS_TOKENS)


class Class :
    "Class to hold all information about a class"

    def __init__(self, name, module) :
        self.name = name
        self.classObject = getattr(module, name)

        modname = getattr(self.classObject, '__module__', None)
        if modname is None:
            # hm, some ExtensionClasses don't have a __module__ attribute
            # so try parsing the type output
            typerepr = repr(type(self.classObject))
            mo = re.match("^<type ['\"](.+)['\"]>$", typerepr)
            if mo:
                modname = ".".join(mo.group(1).split(".")[:-1])

        self.module = sys.modules.get(modname)
        if not self.module:
            self.module = module
            
            global _output
            txt = _("warning: couldn't find real module for class %s (module name: %s)\n") % (self.classObject, modname)
            _output.AddLines(txt)
            
        self.ignoreAttrs = 0
        self.methods = {}
        self.members = { '__class__': types.ClassType,
                         '__doc__': types.StringType,
                         '__dict__': types.DictType, }
        self.memberRefs = {}
        self.statics = {}
        self.lineNums = {}

    def __str__(self) :
        return self.name

    __repr__ = utils.std_repr

    def getFirstLine(self) :
        "Return first line we can find in THIS class, not any base classes"

        lineNums = []
        classDir = dir(self.classObject)
        for m in self.methods.values() :
            if m != None and m.function.func_code.co_name in classDir:
                lineNums.append(m.function.func_code.co_firstlineno)
        if lineNums :
            return min(lineNums)
        return 0


    def allBaseClasses(self, c = None) :
        "Return a list of all base classes for this class and it's subclasses"

        baseClasses = []
        if c == None :
            c = self.classObject
        for base in c.__bases__ :
            baseClasses = baseClasses + [ base ] + self.allBaseClasses(base)
        return baseClasses

    def __getMethodName(self, func_name, className = None) :
        if func_name[0:2] == '__' and func_name[-2:] != '__' :
            if className == None :
                className = self.name
            if className[0] != '_' :
                className = '_' + className
            func_name = className + func_name
        return func_name

    def addMethod(self, method, methodName = None) :
        if type(method) == types.StringType :
            self.methods[method] = None
        else :
            assert methodName is not None, "must supply methodName"
            self.methods[methodName] = function.Function(method, 1)
                
    def addMethods(self, classObject) :
        for classToken in _getClassTokens(classObject) :
            token = getattr(classObject, classToken, None)
            if token is None:
                continue

            # Looks like a method.  Need to code it this way to
            # accommodate ExtensionClass and Python 2.2.  Yecchh.
            if (hasattr(token, "func_code") and
                hasattr(token.func_code, "co_argcount")): 
                self.addMethod(token, token.__name__)

            elif hasattr(token, '__get__') and \
                 not hasattr(token, '__set__') and \
                 type(token) is not types.ClassType :
                self.addMethod(getattr(token, '__name__', classToken))
            else :
                self.members[classToken] = type(token)
                self.memberRefs[classToken] = None

        self.cleanupMemberRefs()
        # add standard methods
        for methodName in ('__class__',) :
            self.addMethod(methodName, classObject.__name__)

    def addMembers(self, classObject) :
        if not cfg().onlyCheckInitForMembers :
            for classToken in _getClassTokens(classObject) :
                method = getattr(classObject, classToken, None)
                if type(method) == types.MethodType :
                    self.addMembersFromMethod(method.im_func)
        else:
            try:
                self.addMembersFromMethod(classObject.__init__.im_func)
            except AttributeError:
                pass

    def addMembersFromMethod(self, method) :
        if not hasattr(method, 'func_code') :
            return

        func_code, code, i, maxCode, extended_arg = OP.initFuncCode(method)
        stack = []
        while i < maxCode :
            op, oparg, i, extended_arg = OP.getInfo(code, i, extended_arg)
            if op >= OP.HAVE_ARGUMENT :
                operand = OP.getOperand(op, func_code, oparg)
                if OP.LOAD_CONST(op) or OP.LOAD_FAST(op) :
                    stack.append(operand)
                elif OP.STORE_ATTR(op) :
                    if len(stack) > 0 :
                        if stack[-1] == cfg().methodArgName:
                            value = None
                            if len(stack) > 1 :
                                value = type(stack[-2])
                            self.members[operand] = value
                            self.memberRefs[operand] = None
                        stack = []

        self.cleanupMemberRefs()

    def cleanupMemberRefs(self) :
        try :
            del self.memberRefs[Config.CHECKER_VAR]
        except KeyError :
            pass

    def abstractMethod(self, m):
        """Return 1 if method is abstract, None if not
           An abstract method always raises an exception.
        """
        if not self.methods.get(m, None):
            return None
        func_code, bytes, i, maxCode, extended_arg = \
                   OP.initFuncCode(self.methods[m].function)
        # abstract if the first conditional is RAISE_VARARGS
        while i < maxCode:
            op, oparg, i, extended_arg = OP.getInfo(bytes, i, extended_arg)
            if OP.RAISE_VARARGS(op):
                return 1
            if OP.conditional(op):
                break
        return None

    def isAbstract(self):
        """Return the method names that make a class abstract.
           An abstract class has at least one abstract method."""
        result = []
        for m in self.methods.keys():
            if self.abstractMethod(m):
                result.append(m)
        return result

def _getLineInFile(moduleName, linenum):
    line = ''
    file, filename, smt = _findModule(moduleName)
    try:
        lines = file.readlines()
        line = string.rstrip(lines[linenum - 1])
    except (IOError, IndexError):
        pass
    file.close()
    return line

def importError(moduleName):
    exc_type, exc_value, tb = sys.exc_info()

    # First, try to get a nice-looking name for this exception type.
    exc_name = getattr(exc_type, '__name__', None)
    if not exc_name:
        # either it's a string exception or a user-defined exception class
        # show string or fully-qualified class name
        exc_name = str(exc_type)
        
    # Print a traceback, unless this is an ImportError.  ImportError is
    # presumably the most common import-time exception, so this saves
    # the clutter of a traceback most of the time.  Also, the locus of
    # the error is usually irrelevant for ImportError, so the lack of
    # traceback shouldn't be a problem.
    if exc_type is SyntaxError:
        # SyntaxErrors are special, we want to control how we format
        # the output and make it consistent for all versions of Python
        e = exc_value
        msg = '%s (%s, line %d)' % (e.msg, e.filename, e.lineno)
        line = _getLineInFile(moduleName, e.lineno)
        offset = e.offset
        if type(offset) is not types.IntType:
            offset = 0
        exc_value = '%s\n    %s\n   %s^' % (msg, line, ' ' * offset)
    elif exc_type is not ImportError:
        global _output
        txt = _("  Caught exception importing module %s:\n") % moduleName
        _output.AddLines(txt)

        try:
            tbinfo = traceback.extract_tb(tb)
        except:
            tbinfo = []
            txt = _("      Unable to format traceback\n")
            _output.AddLines(txt)
        for filename, line, func, text in tbinfo[1:]:
            txt = _("    File \"%s\", line %d") % (filename, line)
            _output.AddLines(txt)
            if func != "?":
                txt = _(", in %s()") % func
                _output.AddLines(txt)
            _output.AddLines("\n")
            if text:
                txt = _("      %s\n") % text
                _output.AddLines(txt)

    # And finally print the exception type and value.
    # Careful formatting exc_value -- can fail for some user exceptions
    txt = "  %s: " % exc_name
    _output.AddLines(txt)
    try:
        txt = str(exc_value) + '\n'
        _output.AddLines(txt)
    except:
        txt = _('**error formatting exception value**\n')
        _output.AddLines(txt)


def _getPyFile(filename):
    """Return the file and '.py' filename from a filename which could
    end with .py, .pyc, or .pyo"""

    if filename[-1] in 'oc' and filename[-4:-1] == '.py':
        return filename[:-1]
    return filename

class Module :
    "Class to hold all information for a module"

    def __init__(self, moduleName, check = 1, fullpath = None) :
        self.moduleName = moduleName
        self.variables = {}
        self.functions = {}
        self.classes = {}
        self.modules = {}
        self.moduleLineNums = {}
        self.attributes = [ '__dict__' ]
        self.main_code = None
        self.module = None
        self.check = check
        self.fullpath = fullpath
        _allModules[moduleName] = self

    def __str__(self) :
        return self.moduleName

    __repr__ = utils.std_repr

    def addVariable(self, var, varType) :
        self.variables[var] = Variable(var, varType)

    def addFunction(self, func) :
        self.functions[func.__name__] = function.Function(func)

    def __addAttributes(self, c, classObject) :
        for base in classObject.__bases__ :
            self.__addAttributes(c, base)
        c.addMethods(classObject)
        c.addMembers(classObject)

    def addClass(self, name) :
        self.classes[name] = c = Class(name, self.module)
        try:
            objName = str(c.classObject)
        except TypeError:
            # this can happen if there is a goofy __getattr__
            c.ignoreAttrs = 1
        else:
            packages = string.split(objName, '.')
            c.ignoreAttrs = packages[0] in cfg().blacklist
        if not c.ignoreAttrs :
            self.__addAttributes(c, c.classObject)

    def addModule(self, name) :
        module = _allModules.get(name, None)
        if module is None :
            self.modules[name] = module = Module(name, 0)
            if imp.is_builtin(name) == 0 :
                module.load()
            else :
                globalModule = globals().get(name)
                if globalModule :
                    module.attributes.extend(dir(globalModule))
        else :
            self.modules[name] = module

    def filename(self) :
        try :
            filename = self.module.__file__
        except AttributeError :
            filename = self.moduleName
        return _getPyFile(filename)

    def load(self, warnings = None):
        try :
            # there's no need to reload modules we already have
            global _output, _statusDlg, _count
            txt = _("Loading Module %s\n") % self.moduleName
            _output.AddLines(txt)
            _count += 1
            if _count == 100:
                _count = 95
            _statusDlg.Update(_count, txt)
            
            module = sys.modules.get(self.moduleName)
            if module :
                if not _allModules[self.moduleName].module :
                    return self._initModule(module)
                return 1

            return self._initModule(self.setupMainCode())
        except (SystemExit, KeyboardInterrupt) :
            exc_type, exc_value, exc_tb = sys.exc_info()
            raise exc_type, exc_value
        except SyntaxError, (message, (fileName, line, col, text)):
            # ActiveGrid: added this for better feedback when module couldn't be loaded.
            w = Warning(self.fullpath, line, _("Syntax Error: %s\n%s\n%s^error near here") % (message, text, ' '*(col-1)))
            warnings.append(w)
            return 0
        except:
            exc_type, exc_value, exc_tb = sys.exc_info()
            w = Warning(self.moduleName, 1, "%s: %s.\nUnable to import module %s." % (exc_type, exc_value, self.moduleName))
            warnings.append(w)
            importError(self.moduleName)
            return 0

    def initModule(self, module) :
        if not self.module:
            filename = _getPyFile(module.__file__)
            if string.lower(filename[-3:]) == '.py':
                try:
                    file = open(filename)
                except IOError:
                    pass
                else:
                    self._setupMainCode(file, filename, module)
            return self._initModule(module)
        return 1

    def _initModule(self, module):
        self.module = module
        self.attributes = dir(self.module)

        pychecker_attr = getattr(module, Config.CHECKER_VAR, None)
        if pychecker_attr is not None :
            utils.pushConfig()
            utils.updateCheckerArgs(pychecker_attr, 'suppressions', 0, [])

        for tokenName in _filterDir(self.module, _DEFAULT_MODULE_TOKENS) :
            token = getattr(self.module, tokenName)
            if isinstance(token, types.ModuleType) :
                # get the real module name, tokenName could be an alias
                self.addModule(token.__name__)
            elif isinstance(token, types.FunctionType) :
                self.addFunction(token)
            elif isinstance(token, types.ClassType) or \
                 hasattr(token, '__bases__') :
                self.addClass(tokenName)
            else :
                self.addVariable(tokenName, type(token))

        if pychecker_attr is not None :
            utils.popConfig()
        return 1

    def setupMainCode(self) :
        file, filename, smt = _findModule(self.moduleName)
        # FIXME: if the smt[-1] == imp.PKG_DIRECTORY : load __all__
        module = imp.load_module(self.moduleName, file, filename, smt)
        self._setupMainCode(file, filename, module)
        return module

    def _setupMainCode(self, file, filename, module):
        try :
            self.main_code = function.create_from_file(file, filename, module)
        finally :
            if file != None :
                file.close()


def getAllModules() :
    "Returns a list of all modules that should be checked."
    modules = []
    for module in _allModules.values() :
        if module.check :
            modules.append(module)
    return modules

_BUILTIN_MODULE_ATTRS = { 'sys': [ 'ps1', 'ps2', 'tracebacklimit', 
                                   'exc_type', 'exc_value', 'exc_traceback',
                                   'last_type', 'last_value', 'last_traceback',
                                 ],
                        }

def fixupBuiltinModules(needs_init=0):
    for moduleName in sys.builtin_module_names :
        if needs_init:
            _ = Module(moduleName, 0)
        module = _allModules.get(moduleName, None)
        if module is not None :
            try :
                m = imp.init_builtin(moduleName)
            except ImportError :
                pass
            else :
                extra_attrs = _BUILTIN_MODULE_ATTRS.get(moduleName, [])
                module.attributes = [ '__dict__' ] + dir(m) + extra_attrs


def _printWarnings(warnings, stream=None):
    if stream is None:
        stream = sys.stdout
    
    warnings.sort()
    lastWarning = None
    for warning in warnings :
        if lastWarning != None :
            # ignore duplicate warnings
            if cmp(lastWarning, warning) == 0 :
                continue
            # print blank line between files
            if lastWarning.file != warning.file :
                global _output
                _output.AddLines("\n")

        lastWarning = warning
        _output.AddLines(warning.format() + "\n")


def processFiles(files, cfg = None, pre_process_cb = None) :
    # insert this here, so we find files in the local dir before std library
    if sys.path[0] != '' :
        sys.path.insert(0, '')

    # ensure we have a config object, it's necessary
    global _cfg
    if cfg is not None :
        _cfg = cfg
    elif _cfg is None :
        _cfg = Config.Config()

    warnings = []
    utils.initConfig(_cfg)
    for moduleName, filename in getModules(files) :
        if callable(pre_process_cb) :
            pre_process_cb(moduleName)
        module = Module(moduleName, fullpath = filename)

        # reload the given module, otherwise won't get new syntax errors.
        sysModule = sys.modules.get(moduleName)
        if sysModule:
            try:
                reload(sysModule)
            except:
                pass
                
        module.load(warnings)
    utils.popConfig()
    return warnings


def getWarnings(files, cfg = None, suppressions = None):
    warnings = processFiles(files, cfg)
    fixupBuiltinModules()
    return warnings + warn.find(getAllModules(), _cfg, suppressions)


def _print_processing(name) :
    if not _cfg.quiet :
        global _output, _statusDlg, _count
        txt = _("Processing %s...\n") % name
        _output.AddLines(txt)
        _count += 1
        _statusDlg.Update(_count, txt)
        


def checkSyntax(filename, messageView):
    """ Massively hacked version of main for ActiveGrid IDE integration """
    global _cfg
    _cfg, files, suppressions = Config.setupFromArgs([filename])
    if not files :
        return 0
                
    global _output, _statusDlg, _count
    _output = messageView
    # wxBug:  Need to show progress dialog box, or message window never gets updated until the method returns    
    _statusDlg = wx.ProgressDialog(_("Check Code"), _("Checking %s") % filename, maximum = 100, style = wx.PD_AUTO_HIDE | wx.PD_APP_MODAL | wx.PD_ELAPSED_TIME)  
    _count = 0

    # insert this here, so we find files in the local dir before std library
    if sys.path[0] != '' :
        sys.path.insert(0, '')

    importWarnings = processFiles(files, _cfg, _print_processing)
    fixupBuiltinModules()
    if _cfg.printParse :
        for module in getAllModules() :
            printer.module(module)

    warnings = warn.find(getAllModules(), _cfg, suppressions)
    
    _statusDlg.Update(100, _("Done"))
    _statusDlg.Destroy()
    
    if not _cfg.quiet :
        _output.AddLines(_("\nWarnings and Errors...\n"))
    if warnings or importWarnings :
        _printWarnings(importWarnings + warnings)
        return 1

    if not _cfg.quiet :
        _output.AddLines(_("No Syntax Errors"))
    return 0
    
##
##
##def main(argv) :
##    __pychecker__ = 'no-miximport'
##    import pychecker
##    if LOCAL_MAIN_VERSION != pychecker.MAIN_MODULE_VERSION :
##        sys.stderr.write(_VERSION_MISMATCH_ERROR)
##        sys.exit(100)
##
##    # remove empty arguments
##    argv = filter(None, argv)
##        
##    # if the first arg starts with an @, read options from the file
##    # after the @ (this is mostly for windows)
##    if len(argv) >= 2 and argv[1][0] == '@':
##        # read data from the file
##        command_file = argv[1][1:]
##        try:
##            f = open(command_file, 'r')
##            command_line = f.read()
##            f.close()
##        except IOError, err:
##            sys.stderr.write("Unable to read commands from file: %s\n  %s\n" % \
##                             (command_file, err))
##            sys.exit(101)
##
##        # convert to an argv list, keeping argv[0] and the files to process
##        argv = argv[:1] + string.split(command_line) + argv[2:]
## 
##    global _cfg
##    _cfg, files, suppressions = Config.setupFromArgs(argv[1:])
##    if not files :
##        return 0
##
##    # insert this here, so we find files in the local dir before std library
##    sys.path.insert(0, '')
##
##    importWarnings = processFiles(files, _cfg, _print_processing)
##    fixupBuiltinModules()
##    if _cfg.printParse :
##        for module in getAllModules() :
##            printer.module(module)
##
##    warnings = warn.find(getAllModules(), _cfg, suppressions)
##    if not _cfg.quiet :
##        print "\nWarnings...\n"
##    if warnings or importWarnings :
##        _printWarnings(importWarnings + warnings)
##        return 1
##
##    if not _cfg.quiet :
##        print "None"
##    return 0
##
##
##if __name__ == '__main__' :
##    try :
##        sys.exit(main(sys.argv))
##    except Config.UsageError :
##        sys.exit(127)
##
##else :
##    _orig__import__ = None
##    _suppressions = None
##    _warnings_cache = {}
##
##    def _get_unique_warnings(warnings):
##        for i in range(len(warnings)-1, -1, -1):
##            w = warnings[i].format()
##            if _warnings_cache.has_key(w):
##                del warnings[i]
##            else:
##                _warnings_cache[w] = 1
##        return warnings
##
##    def __import__(name, globals=None, locals=None, fromlist=None):
##        if globals is None:
##            globals = {}
##        if locals is None:
##            locals = {}
##        if fromlist is None:
##            fromlist = []
##
##        check = not sys.modules.has_key(name) and name[:10] != 'pychecker.'
##        pymodule = _orig__import__(name, globals, locals, fromlist)
##        if check :
##            try :
##                module = Module(pymodule.__name__)
##                if module.initModule(pymodule):
##                    warnings = warn.find([module], _cfg, _suppressions)
##                    _printWarnings(_get_unique_warnings(warnings))
##                else :
##                    print 'Unable to load module', pymodule.__name__
##            except Exception:
##                name = getattr(pymodule, '__name__', str(pymodule))
##                importError(name)
##
##        return pymodule
##
##    def _init() :
##        global _cfg, _suppressions, _orig__import__
##
##        args = string.split(os.environ.get('PYCHECKER', ''))
##        _cfg, files, _suppressions = Config.setupFromArgs(args)
##        utils.initConfig(_cfg)
##        fixupBuiltinModules(1)
##
##        # keep the orig __import__ around so we can call it
##        import __builtin__
##        _orig__import__ = __builtin__.__import__
##        __builtin__.__import__ = __import__
##
##    if not os.environ.get('PYCHECKER_DISABLED') :
##        _init()
##
