#----------------------------------------------------------------------------
# Name:         cachedloader.py
# Purpose:
#
# Author:       Joel Hare
#
# Created:      8/31/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import copy
import os.path
import string
import cStringIO

import time

# TODO: Instantiate the database and create a pool


class CachedLoader(object):
    def __init__(self):
        self.cache = {}
        self.baseLoadDir = None
        
    def fullPath(self, fileName):
        if os.path.isabs(fileName):
            absPath = fileName
        elif self.baseLoadDir:
            absPath = os.path.join(self.baseLoadDir, fileName)
        else:
            absPath = os.path.abspath(fileName)
        return absPath
        
    def setPrototype(self, fileName, loadedFile):
        absPath = self.fullPath(fileName)
        mtime = time.time() + 31536000.0 # Make sure prototypes aren't replaced by files on disk
        self.cache[absPath] = (mtime, loadedFile)

    def update(self, loader):
        self.cache.update(loader.cache)
        
    def clear(self):
        self.cache.clear()

    def delete(self, fileName):
        absPath = self.fullPath(fileName)
        del self.cache[absPath]
        
    def needsLoad(self, fileName):
        absPath = self.fullPath(fileName)
        try:
            cached = self.cache[absPath]
            cachedTime = cached[0]
            if cachedTime >= os.path.getmtime(absPath):
                return False
        except KeyError:
            pass
        return True
        
    def load(self, fileName, loader):
        absPath = self.fullPath(fileName)
        loadedFile = None
        try:
            cached = self.cache[absPath]
        except KeyError:
            cached = None
    
        if cached:
            cachedTime = cached[0]
            # ToDO We might need smarter logic for checking if a file needs to be reloaded
            # ToDo We need a way to disable checking if this is a production server
            if cachedTime >= os.path.getmtime(absPath):
                loadedFile = cached[1]
    
        if not loadedFile:
            targetFile = file(absPath)
            try:
                mtime = os.path.getmtime(absPath)
                loadedFile = loader(targetFile)
                self.cache[absPath] = (mtime, loadedFile)
            finally:
                targetFile.close()
        return loadedFile

