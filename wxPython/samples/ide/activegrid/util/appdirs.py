#----------------------------------------------------------------------------
# Name:         appdirs.py
# Purpose:      Utilities for retrieving special application dirs
#
# Author:       Kevin Ollivier
#
# Created:      8/27/05
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

# NOTE: This was made a separate file because it depends upon the 
# wx.StandardPaths module, and thus, on wxWidgets, unlike other
# utils modules. I wanted to ensure this module is never loaded
# from the web server, etc.

import sys
import os
import string
import wx

def isWindows():
    return os.name == 'nt'

def _generateDocumentsDir():
    path = ""
    if sys.platform == "win32":
        from win32com.shell import shell, shellcon
        path=shell.SHGetFolderPath(0, shellcon.CSIDL_PERSONAL, None, 0)
    elif sys.platform == "darwin":
        import macfs, MACFS
        fsspec_disk, fsspec_desktop = macfs.FindFolder( MACFS.kOnSystemDisk, MACFS.kDocumentsFolderType, 0)
        path = macfs.FSSpec((fsspec_disk, fsspec_desktop, '')).as_pathname()
    
    if path == "":
        path = os.path.expanduser("~")

    return path
            
documents_folder = _generateDocumentsDir()

# NOTE: We don't set this at startup because wxStandardPaths needs a running
# application object. This makes sure the wxApp will always be created when
# we get the folder.
def getAppDataFolder():
    # wxStandardPaths requires a running app
    if wx.GetApp() and wx.Platform != "__WXGTK__":
        data_folder = wx.StandardPaths.Get().GetUserDataDir()
        if not os.path.exists(data_folder):
            os.mkdir(data_folder)
        return data_folder
    else:
        # wxBug: on *nix, it wants to point to ~/.appname, but
        # so does wxConfig... For now, redirect this to ~/.appbuilder
        # when this is fixed, we'll migrate settings to the correct place
        return os.path.join(os.path.expanduser("~"), ".appbuilder")

    return ""
