#----------------------------------------------------------------------------
# Name:         aglogging.py
# Purpose:      Utilities to help with logging
#
# Author:       Jeff Norton
#
# Created:      01/04/05
# CVS-ID:       $Id$
# Copyright:    (c) 2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import sys
import os
import re
import traceback
import logging
import logging.config
from activegrid.util.lang import *
import activegrid.util.objutils as objutils
import activegrid.util.sysutils as sysutils
import activegrid.util.appdirs as appdirs

LEVEL_FATAL = logging.FATAL
LEVEL_ERROR = logging.ERROR
LEVEL_WARN = logging.WARN
LEVEL_INFO = logging.INFO
LEVEL_DEBUG = logging.DEBUG

EXCEPTION_INFO = 'exceptionInfo'
loggingInitialized = False

LOG_MODE_IDE = 1
LOG_MODE_TESTRUN = 2
LOG_MODE_RUN = 3
def initLogging(mode, force=False):
    global ag_debugLogger, loggingInitialized
    if (force or not loggingInitialized):
        loggingInitialized = True
        configFile = None
        if (mode == LOG_MODE_IDE):
            configFile = os.getenv("AG_LOGCONFIG_IDE")
        elif (mode == LOG_MODE_TESTRUN):
            configFile = os.getenv("AG_LOGCONFIG_PYTESTRUN")
        else:
            configFile = os.getenv("AG_LOGCONFIG_RUN")
        if ((configFile == None) or not os.path.exists(configFile)):
            if (mode == LOG_MODE_IDE):
                configFile = "IDELog"
            elif (mode == LOG_MODE_TESTRUN):
                configFile = "TestRunLog"
            else:
                configFile = "RunLog"
            configFile = os.path.join(appdirs.getSystemDir(appdirs.AG_LOGS_DIR), "py" + configFile + ".ini")
        if (os.path.exists(configFile)):
            print "Using logging configuration file: %s" % configFile
            fileConfig(configFile)
        else:
            print "*** Cannot find logging configuration file (%s) -- setting default logging level to WARN ***" % (configFile)
            defaultStream = sys.stderr
            if (mode == LOG_MODE_RUN):
                defaultStream = sys.stdout
            handler = logging.StreamHandler(defaultStream)
            handler.setLevel(logging.DEBUG)
            handler.setFormatter(logging.Formatter("%(asctime)s %(name)s %(levelname)s: %(message)s"))
            logging.getLogger().addHandler(handler)
            logging.getLogger().setLevel(logging.WARN)
        ag_debugLogger = logging.getLogger("activegrid.debug")
        ag_debugLogger.setLevel(logging.DEBUG)
        return configFile
    
ag_debugLogger = logging.getLogger("activegrid.debug")

def log(logger, level, msg, *params):
    if (logger == None):
        logger = ag_debugLogger
    apply(logger.log, (level, msg) + params)

def fatal(logger, msg, *params):
    apply(logger.fatal, (msg,) + params)

def error(logger, msg, *params):
    apply(logger.error, (msg,) + params)

def warn(logger, msg, *params):
    apply(logger.warn, (msg,) + params)

def info(logger, msg, *params):
    apply(logger.info, (msg,) + params)

def debug(logger, msg, *params):
    if (logger == None):
        logger = ag_debugLogger
    apply(logger.debug, (msg,) + params)
    
def setLevelFatal(logger):
    logger.setLevel(LEVEL_FATAL)
    
def setLevelError(logger):
    logger.setLevel(LEVEL_ERROR)
    
def setLevelWarn(logger):
    logger.setLevel(LEVEL_WARN)
    
def setLevelInfo(logger):
    logger.setLevel(LEVEL_INFO)
    
def setLevelDebug(logger):
    logger.setLevel(LEVEL_DEBUG)
    
def isEnabledForError(logger):
    return logger.isEnabledFor(LEVEL_ERROR)

def isEnabledForWarn(logger):
    return logger.isEnabledFor(LEVEL_WARN)

def isEnabledForInfo(logger):
    return logger.isEnabledFor(LEVEL_INFO)

def isEnabledForDebug(logger):
    return logger.isEnabledFor(LEVEL_DEBUG)

TEST_MODE_NONE = 0
TEST_MODE_DETERMINISTIC = 1
TEST_MODE_NON_DETERMINISTIC = 2

global agTestMode
agTestMode = TEST_MODE_NONE

def setTestMode(mode):
    global agTestMode
    agTestMode = mode
        
def getTestMode():
    global agTestMode
    return agTestMode
    
def testMode(normalObj, testObj=None, nonDeterministicObj=None):
    testMode = getTestMode()
    if testMode > TEST_MODE_NONE:
        if ((nonDeterministicObj != None) and (testMode == TEST_MODE_NON_DETERMINISTIC)):
            return nonDeterministicObj
        return testObj
    return normalObj

pythonFileRefPattern = asString(r'(?<=File ")[^"]*(#[^#]*")(, line )[0-9]*')
phpFileRefPattern = asString(r'( in ).*#([^#]*#[^ ]*)(?= on line )')
pathSepPattern = os.sep
if (pathSepPattern == "\\"):
    pathSepPattern = "\\\\"
pythonFileRefPattern = pythonFileRefPattern.replace("#", pathSepPattern)
pythonFileRefPattern = re.compile(pythonFileRefPattern)
phpFileRefPattern = phpFileRefPattern.replace("#", pathSepPattern)
phpFileRefPattern = re.compile(phpFileRefPattern)

def removeFileRefs(str):
    str = pythonFileRefPattern.sub(_fileNameReplacement, str)
    str = phpFileRefPattern.sub(_fileNameReplacementPHP, str)
    return str
    
def removePHPFileRefs(str):
    str = phpFileRefPattern.sub(_fileNameReplacementPHP, str)
    return str
    
def _fileNameReplacement(match):
    return "...%s" % match.group(1).replace(os.sep, "/")
    
def _fileNameReplacementPHP(match):
    return "%s...%s" % (match.group(1), match.group(2).replace(os.sep, "/"))
    
def formatTraceback(tb=None):
    if (tb == None):
        extype, val, tb = sys.exc_info()
    tbs = "\n" + "".join(traceback.format_tb(tb))
    return tbs

def formatExceptionCause(cause, stacktrace=False):
    if (cause == None):
        return ""
    tbs = ""
    if (stacktrace):
        tbs = formatTraceback()
    return "Caused by %s.%s: %s%s" % (cause.__module__, cause.__class__.__name__, str(cause), tbs)

def addExceptionInfo(e, key, value):
    if not hasattr(e, EXCEPTION_INFO):
        try:
            setattr(e, EXCEPTION_INFO, {})
        except:
            return # Make sure we still report the real exception even if we can't add the extra info
    if not e.exceptionInfo.has_key(key): # Never overwrite exception info since we assume earlier info is more specific
        e.exceptionInfo[key] = value
            
def reportException(exception, out=None, stacktrace=False, diffable=False):
    exstr = exceptionToString(exception, stacktrace, diffable)
    if (out == None):
        print exstr
    else:
        print >> out, exstr

def exceptionToString(exception, stacktrace=False, diffable=False):
    extype = objutils.typeToString(exception)
    val = exception
    if (stacktrace):
        e,v,t = sys.exc_info()
    if (diffable):
        exstr = removeFileRefs(str(val))
    else:
        exstr = str(val)
    if hasattr(val, EXCEPTION_INFO):
        firstTime = True
        for infoKey, infoValue in getattr(val, EXCEPTION_INFO).items():
            if firstTime:
                prefix = " EXTRA INFO:"
                firstTime = False
            else:
                prefix = ","
            exstr += ("%s %s=%s" % (prefix, infoKey, infoValue))
    result = "Got Exception = %s: %s" % (extype, exstr)
    if (stacktrace):
        fmt = traceback.format_exception(extype, val, t)
        for s in fmt:
            if (diffable):
                s = removeFileRefs(s)
            result = result + "\n" + s
    return result
    
def fileConfig(fname, defaults=None):
    """
    This is copied from logging.config so that we could fix the class lookup of
    handlers.  Previously handlers had to be defined in logging.handlers and we
    need to be able to define our own.
    """
    import ConfigParser, string

    cp = ConfigParser.ConfigParser(defaults)
    if hasattr(cp, 'readfp') and hasattr(fname, 'readline'):
        cp.readfp(fname)
    else:
        cp.read(fname)
    #first, do the formatters...
    flist = cp.get("formatters", "keys")
    if len(flist):
        flist = string.split(flist, ",")
        formatters = {}
        for form in flist:
            sectname = "formatter_%s" % form
            opts = cp.options(sectname)
            if "format" in opts:
                fs = cp.get(sectname, "format", 1)
            else:
                fs = None
            if "datefmt" in opts:
                dfs = cp.get(sectname, "datefmt", 1)
            else:
                dfs = None
            f = logging.Formatter(fs, dfs)
            formatters[form] = f
    #next, do the handlers...
    #critical section...
    logging._acquireLock()
    try:
##        try:
            #first, lose the existing handlers...
            logging._handlers.clear()
            #now set up the new ones...
            hlist = cp.get("handlers", "keys")
            if len(hlist):
                hlist = string.split(hlist, ",")
                handlers = {}
                fixups = [] #for inter-handler references
                for hand in hlist:
##                    try:
                        sectname = "handler_%s" % hand
                        classname = cp.get(sectname, "class")
                        opts = cp.options(sectname)
                        if "formatter" in opts:
                            fmt = cp.get(sectname, "formatter")
                        else:
                            fmt = ""
                        klass = None
                        try:
                            klass = eval(classname, vars(logging))
                        except:
                            pass
                        if (klass == None):
                            klass = objutils.classForName(classname)
                        args = cp.get(sectname, "args")
                        args = eval(args, vars(logging))
                        h = apply(klass, args)
                        if "level" in opts:
                            level = cp.get(sectname, "level")
                            h.setLevel(logging._levelNames[level])
                        if len(fmt):
                            h.setFormatter(formatters[fmt])
                        #temporary hack for FileHandler and MemoryHandler.
                        if klass == logging.handlers.MemoryHandler:
                            if "target" in opts:
                                target = cp.get(sectname,"target")
                            else:
                                target = ""
                            if len(target): #the target handler may not be loaded yet, so keep for later...
                                fixups.append((h, target))
                        handlers[hand] = h
##                    except Exception, e:     #if an error occurs when instantiating a handler, too bad
##                        pass    #this could happen e.g. because of lack of privileges
                #now all handlers are loaded, fixup inter-handler references...
                for fixup in fixups:
                    h = fixup[0]
                    t = fixup[1]
                    h.setTarget(handlers[t])
            #at last, the loggers...first the root...
            llist = cp.get("loggers", "keys")
            llist = string.split(llist, ",")
            llist.remove("root")
            sectname = "logger_root"
            root = logging.root
            log = root
            opts = cp.options(sectname)
            if "level" in opts:
                level = cp.get(sectname, "level")
                log.setLevel(logging._levelNames[level])
            for h in root.handlers[:]:
                root.removeHandler(h)
            hlist = cp.get(sectname, "handlers")
            if len(hlist):
                hlist = string.split(hlist, ",")
                for hand in hlist:
                    log.addHandler(handlers[hand])
            #and now the others...
            #we don't want to lose the existing loggers,
            #since other threads may have pointers to them.
            #existing is set to contain all existing loggers,
            #and as we go through the new configuration we
            #remove any which are configured. At the end,
            #what's left in existing is the set of loggers
            #which were in the previous configuration but
            #which are not in the new configuration.
            existing = root.manager.loggerDict.keys()
            #now set up the new ones...
            for log in llist:
                sectname = "logger_%s" % log
                qn = cp.get(sectname, "qualname")
                opts = cp.options(sectname)
                if "propagate" in opts:
                    propagate = cp.getint(sectname, "propagate")
                else:
                    propagate = 1
                logger = logging.getLogger(qn)
                if qn in existing:
                    existing.remove(qn)
                if "level" in opts:
                    level = cp.get(sectname, "level")
                    logger.setLevel(logging._levelNames[level])
                for h in logger.handlers[:]:
                    logger.removeHandler(h)
                logger.propagate = propagate
                logger.disabled = 0
                hlist = cp.get(sectname, "handlers")
                if len(hlist):
                    hlist = string.split(hlist, ",")
                    for hand in hlist:
                        logger.addHandler(handlers[hand])
            #Disable any old loggers. There's no point deleting
            #them as other threads may continue to hold references
            #and by disabling them, you stop them doing any logging.
            for log in existing:
                root.manager.loggerDict[log].disabled = 1
##        except:
##            import traceback
##            ei = sys.exc_info()
##            traceback.print_exception(ei[0], ei[1], ei[2], None, sys.stderr)
##            del ei
    finally:
        logging._releaseLock()
