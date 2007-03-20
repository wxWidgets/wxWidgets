#----------------------------------------------------------------------------
# Name:         appdirs.py
# Purpose:      Utilities for retrieving special application dirs
#
# Author:       Kevin Ollivier, Jeff Norton
#
# Created:      8/27/05
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

from activegrid.util.lang import *
import sys
import os
import string
import activegrid.util.sysutils as sysutils

def _getSystemDir(kind):
    if (kind == AG_LOGS_DIR):
        return os.path.join(getSystemDir(AG_SYSTEM_DIR) , "logs")
    elif (kind == AG_DEMOS_DIR):
        return os.path.join(getSystemDir(AG_SYSTEM_DIR), "demos")
    else:
        path = ""
        if (sysutils.isServer()):
            path = os.getenv("ACTIVEGRID_SERVER_HOME")
            if ((path is None) or (len(path) < 1)):
                path = sysutils.mainModuleDir
        else:
            path = os.getenv("AG_DOCUMENTS_DIR")
            if ((path is None) or (len(path) < 1)):
                if sysutils.isWindows():
                    ifDefPy()
                    try:
                        from win32com.shell import shell, shellcon
                        path = shell.SHGetFolderPath(0, shellcon.CSIDL_PERSONAL, None, 0)
                    except:
                        pass
                    endIfDef()
                    if ((path is None) or (len(path) < 1)):
                        homedrive = asString(os.getenv("HOMEDRIVE"))
                        homepath = os.getenv("HOMEPATH")
##                        if ((homedrive is not None) and (len(homedrive) > 0) and (homepath is not None) and (len(homepath) > 0)):
                        path = os.path.join(homedrive, homepath, "MYDOCU~1")
                else:
                    ifDefPy()
                    if sys.platform == "darwin":
                        try:
                            import macfs
                            import MACFS
                            fsspec_disk, fsspec_desktop = macfs.FindFolder(MACFS.kOnSystemDisk, MACFS.kDocumentsFolderType, 0)
                            path = macfs.FSSpec((fsspec_disk, fsspec_desktop, '')).as_pathname()
                        except:
                            pass
                    endIfDef()
                
                ifDefPy()
                if ((path is None) or (len(path) < 1)):
                    path = os.path.expanduser("~")
                endIfDef()
                if ((path is None) or (len(path) < 1)):
                    path = "/"
                path = os.path.join(path, "ActiveGrid")
            
        return path
        

AG_SYSTEM_DIR = 0
AG_LOGS_DIR = 1
AG_DEMOS_DIR = 2

__systemDir = None
__logsDir = None
__demosDir = None

def getSystemDir(kind=0):
    if (kind == AG_SYSTEM_DIR):
        global __systemDir
        if (__systemDir is None):
            __systemDir = _getSystemDir(kind)
        return __systemDir
    elif (kind == AG_LOGS_DIR):
        global __logsDir
        if (__logsDir is None):
            __logsDir = _getSystemDir(kind)
        return __logsDir
    elif (kind == AG_DEMOS_DIR):
        global __demosDir
        if (__demosDir is None):
            __demosDir = _getSystemDir(kind)
        return __demosDir
    return None


# NOTE: We don't set this at startup because wxStandardPaths needs a running
# application object. This makes sure the wxApp will always be created when
# we get the folder.
ifDefPy()
def getAppDataFolder():
    try:
        # NOTE: cannot import wx from the server
        import wx
        # wxStandardPaths requires a running app
        if wx.GetApp() and wx.Platform != "__WXGTK__":
            data_folder = wx.StandardPaths.Get().GetUserDataDir()
            if not os.path.exists(data_folder):
                os.mkdir(data_folder)
            return data_folder
    except:
        pass
    # wxBug: on *nix, it wants to point to ~/.appname, but
    # so does wxConfig... For now, redirect this to ~/.appbuilder
    # when this is fixed, we'll migrate settings to the correct place
    return os.path.join(os.path.expanduser("~"), ".appbuilder")
endIfDef()

ifDefPy()
def createSystemDirs():
    if (not os.path.exists(getSystemDir())):
        os.mkdir(getSystemDir())
    if (not os.path.exists(getSystemDir(AG_LOGS_DIR))):
        os.mkdir(getSystemDir(AG_LOGS_DIR))
    if (not os.path.exists(getSystemDir(AG_DEMOS_DIR))):
        os.mkdir(getSystemDir(AG_DEMOS_DIR))
endIfDef()
