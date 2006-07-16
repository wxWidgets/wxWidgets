#----------------------------------------------------------------------------
# Name:         fileutils.py
# Purpose:      Active grid miscellaneous utilities
#
# Author:       Jeff Norton
#
# Created:      12/10/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import logging
import copy
import os
import shutil
import sys
import zipfile

import activegrid.util.aglogging as aglogging
import activegrid.util.sysutils as sysutils
import activegrid.util.utillang as utillang
from activegrid.util.lang import *

global fileutilsLogger
fileutilsLogger = logging.getLogger("activegrid.util.fileutils")
# FATAL : No logging
# ERROR : No logging
# WARN  : No logging
# INFO  : No logging
# DEBUG : debugging
aglogging.setLevelFatal(fileutilsLogger)
#logging.getLogger().addHandler(logging.StreamHandler(sys.stderr))

def addRef(varname):
    return "${%s}" % varname

AG_SYSTEM_VAR_NAMES = [] # all AG System vars, with ${} syntax

AG_SYSTEM_VAR = "AG_SYSTEM"
AG_SYSTEM_VAR_REF = addRef(AG_SYSTEM_VAR)
AG_SYSTEM_VAR_NAMES.append(AG_SYSTEM_VAR_REF)

AG_SYSTEM_STATIC_VAR = "AG_SYSTEM_STATIC"
AG_SYSTEM_STATIC_VAR_REF = addRef(AG_SYSTEM_STATIC_VAR)
AG_SYSTEM_VAR_NAMES.append(AG_SYSTEM_STATIC_VAR_REF)

AG_APP_VAR = "AG_APP"
AG_APP_STATIC_VAR = "AG_APP_STATIC"

# _initAGSystemVars needs to be called to initialize the following two
# containers:
EXPANDED_AG_SYSTEM_VARS = {} # ${varname} -> value (path)
# ${varname}, ordered from longest to shortest path value
AG_SYSTEM_VARS_LENGTH_ORDER = [] 

def _initAGSystemVars():
    if (len(EXPANDED_AG_SYSTEM_VARS) > 0):
        return
    
    for v in AG_SYSTEM_VAR_NAMES:
        EXPANDED_AG_SYSTEM_VARS[v] = os.path.abspath(expandVars(v))
        AG_SYSTEM_VARS_LENGTH_ORDER.append(v)
        
    AG_SYSTEM_VARS_LENGTH_ORDER.sort(_sortByValLength)


def parameterizePathWithAGSystemVar(inpath):
    """Returns parameterized path if path starts with a known AG directory. Otherwise returns path as it was passed in."""
    _initAGSystemVars()
    path = inpath
    if not sysutils.isWindows():
        # ensure we have forward slashes
        path = path.replace("\\", "/")
        
    path = os.path.abspath(path)

    for varname in AG_SYSTEM_VARS_LENGTH_ORDER:
        varval = EXPANDED_AG_SYSTEM_VARS[varname]
        if path.startswith(varval):
            return path.replace(varval, varname)
        
    return inpath

def startsWithAgSystemVar(path):
    """Returns True if path starts with a known AG system env var, False otherwise."""
    for varname in AG_SYSTEM_VAR_NAMES:
        if path.startswith(varname):
            return True
    return False
        
def _sortByValLength(v1, v2):
    return len(EXPANDED_AG_SYSTEM_VARS[v2]) - len(EXPANDED_AG_SYSTEM_VARS[v1])

def makeDirsForFile(filename):
    d = os.path.dirname(filename)
    if (not os.path.exists(d)):
        os.makedirs(d)

def createFile(filename, mode='w'):
    f = None
    if (not os.path.exists(filename)):
        makeDirsForFile(filename)
    f = file(filename, mode)
    return f

def compareFiles(file1, file2, ignore=None):
##    result = filecmp.cmp(file1, file2)
##    if result:
##        return 0
##    return -1
    file1.seek(0)
    file2.seek(0)
    while True:
        line1 = file1.readline()
        line2 = file2.readline()
        if (len(line1) == 0):
            if (len(line2) == 0):
                return 0
            else:
                return -1
        elif (len(line2) == 0):
            return -1
        elif (line1 != line2):
            if (ignore != None):
                if (line1.startswith(ignore) or line2.startswith(ignore)):
                    continue
            line1 = line1.replace(" ", "")
            line2 = line2.replace(" ", "")
            if (line1 != line2):
                len1 = len(line1)
                len2 = len(line2)
                if ((abs(len1 - len2) == 1) and (len1 > 0) and (len2 > 0) 
                    and (line1[-1] == "\n") and (line2[-1] == "\n")):
                    if (len1 > len2):
                        longer = line1
                        shorter = line2
                    else:
                        shorter = line1
                        longer = line2
                    if ((longer[-2] == "\r") and (longer[:-2] == shorter[:-1])):
                        continue
                    if ((longer[-2:] == shorter[-2:]) and (longer[-3] == "\r") and (longer[:-3] == shorter[:-2])):
                        continue
                return -1

def expandKnownAGVars(value):
    return expandVars(value, includeEnv=False)

def expandVars(value, includeEnv=True):
    """Syntax: ${myvar,default="default value"}"""
    import activegrid.runtime as runtime
    sx = value.find("${")
    if (sx >= 0):
        result = asString(value[:sx])
        endx = value.find("}")
        if (endx > 1):
            defaultValue = None
            defsx = value.find(",default=\"")
            if ((defsx > sx) and (defsx < endx)):
                varname = value[sx+2:defsx]
                if (value[endx-1] == '"'):
                    defaultValue = value[defsx+10:endx-1]
            if (defaultValue == None):
                varname = value[sx+2:endx]
            if (varname == AG_SYSTEM_VAR):
                varval = runtime.appInfo.getSystemDir()
            elif (varname == AG_SYSTEM_STATIC_VAR):
                varval = runtime.appInfo.getSystemStaticDir()
            elif (varname == AG_APP_VAR):
                varval = runtime.appInfo.getAppDir()
            elif (varname == AG_APP_STATIC_VAR):
                varval = runtime.appInfo.getAppStaticDir()
            else:
                if (includeEnv):
                    varval = os.getenv(varname)
                else:
                    varval = None
            if ((varval == None) and (defaultValue != None)):
                varval = defaultValue
            if (varval == None):
                result += value[sx:endx+1]
            else:
                result += varval
            return result + expandVars(value[endx+1:])
    return value

def toPHPpath(path, otherdir=None):
    return convertSourcePath(path, "php", otherdir=otherdir)
                    
def toPythonpath(path, otherdir=None):
    return convertSourcePath(path, "python", otherdir=otherdir)
                    
def toUnixPath(path):
    if (path != None and os.sep != '/'):
        path = path.replace(os.sep, '/')
    return path

def convertSourcePath(path, to, otherdir=None):
    fromname = "python"
    if (to == "python"):
        fromname = "php"
    pythonNode = os.sep + fromname + os.sep
    ix = path.find(pythonNode)
    if (ix < 0):
        ix = path.find(fromname) - 1
        if ((ix < 0) or (len(path) <= ix+7)
            or (path[ix] not in ("\\", "/")) or (path[ix+7]  not in ("\\", "/"))):
            raise Exception("Not in a %s source tree.  Cannot create file name for %s." % (fromname, path))
        if (otherdir == None):
            return path[:ix+1] + to + path[ix+7:]
        else:
            return otherdir + path[ix+7:]
    if (otherdir == None):
        return path.replace(pythonNode, os.sep + to + os.sep)
    else:
        return otherdir + path[ix+7:]


def visit(directory, files, extension, maxLevel=None, level=1):
    testdirs = os.listdir(directory)
    for thing in testdirs:
        fullpath = os.path.join(directory, thing)
        if (os.path.isdir(fullpath) and (maxLevel == None or level < maxLevel)):
            visit(fullpath, files, extension, maxLevel, level+1)
        elif thing.endswith(extension):
            fullname = os.path.normpath(os.path.join(directory, thing))
            if not fullname in files:
                files.append(fullname)
 
def listFilesByExtensionInPath(path=[], extension='.lyt', maxLevel=None):
    retval = []
    for directory in path:
        visit(directory, retval, extension, maxLevel)
    return retval

def getFileLastModificationTime(fileName):
    return os.path.getmtime(fileName)

def findFileLocation(location, fileName):
    i = fileName.rfind(os.sep)
    if i > 0:
        fileName = fileName[:i]
    while location[0:2] == '..' and location[2:3] == os.sep:
        location = location[3:]
        i = fileName.rfind(os.sep)
        fileName = fileName[:i]
    absPath = fileName + os.sep + location
    return absPath

def getAllExistingFiles(files, basepath=None, forceForwardSlashes=False):
    """For each file in files, if it exists, adds its absolute path to the rtn list. If file is a dir, calls this function recursively on all child files in the dir.
    If basepath is set, and if the file being processed is relative to basedir, adds that relative path to rtn list instead of the abs path.
    Is this is Windows, and forceForwardSlashes is True, make sure returned paths only have forward slashes."""
    if isinstance(files, basestring):
        files = [files]
    rtn = []
    for file in files:
        if os.path.exists(file): 
            if os.path.isfile(file):
                if basepath and hasAncestorDir(file, basepath):
                    rtn.append(getRelativePath(file, basepath))
                else:
                    rtn.append(os.path.abspath(str(file)))
            elif os.path.isdir(file):
                dircontent = [os.path.join(file, f) for f in os.listdir(file)]
                rtn.extend(getAllExistingFiles(dircontent, basepath))
                
    if forceForwardSlashes and sysutils.isWindows():
        newRtn = []
        for f in rtn:
            newRtn.append(f.replace("\\", "/"))
        rtn = newRtn
        
    return rtn

def hasAncestorDir(file, parent):
    """Returns true if file has the dir 'parent' as some parent in its path."""
    return getRelativePath(file, parent) != None

def getRelativePath(file, basedir):
    """Returns relative path from 'basedir' to 'file', assuming 'file' lives beneath 'basedir'. If it doesn't, returns None."""
    file = os.path.abspath(file)
    parent = os.path.abspath(basedir)

    if file == parent:
        return None

    if file.startswith(parent):
        return file[len(parent)+1:]
    
    return None

def isEmptyDir(dir):
    if not os.path.isdir(dir):
        return False
    return len(os.listdir(dir)) == 0

ifDefPy()
def zip(zipfilepath, basedir=None, files=None):
    """Zip all files in files and save zip as zipfilepath. If files is None, zip all files in basedir. For all files to be zipped, if they are relative to basedir, include the relative path in the archive."""
    
    if not files and not basedir:
        raise AssertionError("Either 'basedir' or 'files' must be set")
            
    if not files:
        aglogging.debug(fileutilsLogger,\
                        "Looking for files to zip in %s" % basedir)
        files = getAllExistingFiles(basedir)
    else:
        # removes files that don't exist and gets abs for each
        files = getAllExistingFiles(files) 

    if len(files) == 0:
        aglogging.debug(fileutilsLogger, "No files to zip, nothing to do")
        return
    
    z = zipfile.ZipFile(zipfilepath, mode="w", compression=zipfile.ZIP_DEFLATED)

    try:
        for file in files:
            arcname = None
            if basedir:
                arcname = getRelativePath(file, basedir)
            if not arcname:
                arcname = file
                aglogging.debug(fileutilsLogger,\
                                "%s: adding %s with arcname %s" %\
                                (zipfilepath, file, arcname))
            z.write(file, arcname)
    finally:
        z.close()
endIfDef()        


ifDefPy()
def unzip(zipfilepath, extractdir):
    """Unzip zipfilepath into extractdir."""
    z = zipfile.ZipFile(zipfilepath, mode="r")
    for info in z.infolist():
        filename = os.path.join(extractdir, info.filename)
        try:
            dir = os.path.dirname(filename)
            aglogging.debug(fileutilsLogger, "Creating dir %s" % dir)
            os.makedirs(dir) # do we have to worry about permissions?
        except:
            pass
        if os.path.isdir(filename):
            continue
        aglogging.debug(fileutilsLogger,\
                       ("Writing arcfile %s to %s" % (info.filename, filename)))
        f = open(filename, "w")
        f.write(z.read(info.filename))
        f.close()
endIfDef()

ifDefPy()
def copyFile(src, dest):
    """Copies file src to dest. Creates directories in 'dest' path if necessary."""
    destdir = os.path.dirname(dest)
    if not os.path.exists(destdir):
        os.makedirs(destdir)
    shutil.copy(src, dest)
endIfDef()

ifDefPy()
def copyDir(src, dest):
    """Copies dir 'src' into dir 'dest'. Creates 'dest' if it does not exist."""
    shutil.copytree(src, dest)
endIfDef()

ifDefPy()
def remove(file):
    if not os.path.exists(file):
        return
    if os.path.isfile(file):
        os.remove(file)
    elif os.path.isdir(file):
        shutil.rmtree(file)
endIfDef()

def getUserTempDir():
    systemTempDir = utillang.getSystemTempDir()
    userName = sysutils.getUserName()
    userNameNoSpace = userName.replace('_','__').replace(' ','_')
    userTempDir = systemTempDir + os.sep + "activegrid_" + userNameNoSpace
    return userTempDir

def createUserTempDir():
    userTempDir = getUserTempDir()
    if not os.path.exists(userTempDir):
        os.makedirs(userTempDir)
        os.chmod(userTempDir, 0700)

createUserTempDir()

ifDefPy()
import warnings
warnings.filterwarnings("ignore", message="tmpnam is a potential security risk to your program")
def getTmpFile():
    return os.tmpnam()
endIfDef()

ifDefPy()
#@accepts str, dict, str, str, boolean
def replaceToken(infilepath, tokens={}, outfilepath=None, delim="@@",\
                 useEnv=False):
    """Replaces tokens of form 'delim'<tokenname>'delim' in file at 'infilepath', using values in dict 'tokens'. If 'outfilepath' is set, writes output to 'outfilepath', if not set, overwrites original file. If 'useEnv' is True, adds os.environ to 'tokens'. This makes it possible to define an env var FOO=BLAH, and have @@FOO@@ be replaced with BLAH, without explicitly passing FOO=BLAH in 'tokens'. Note that entries in 'tokens' take precedence over entries in os.environ."""

    if useEnv:
        for key, val in os.environ.items():
            # passed in tokens take precedence
            if not tokens.has_key(key):
                tokens[key] = val        
    
    f = open(infilepath, "r")
    try:
        content = f.read()
    finally:
        if f: f.close()

    for token, value in tokens.items():
        content = content.replace("%s%s%s" % (delim, token , delim), str(value))

    if not outfilepath: outfilepath = infilepath
    f = open(outfilepath, "w")
    try:
        f.write(content)
    finally:
        if f: f.close()
endIfDef()
