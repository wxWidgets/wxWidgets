# This file was created automatically by SWIG.
import filesysc

from utils import *

from image import *

from misc import *

from gdi import *

from fonts import *

from streams import *
import wx

import types
def wxMemoryFSHandler_AddFile(filename, a, b=''):
    if wx.wxPy_isinstance(a, (wxImage, wxImagePtr)):
        __wxMemoryFSHandler_AddFile_wxImage(filename, a, b)
    elif wx.wxPy_isinstance(a, (wxBitmap, wxBitmapPtr)):
        __wxMemoryFSHandler_AddFile_wxBitmap(filename, a, b)
    elif type(a) == types.StringType:
        #__wxMemoryFSHandler_AddFile_wxString(filename, a)
        __wxMemoryFSHandler_AddFile_Data(filename, a)
    else: raise TypeError, 'wxImage, wxBitmap or string expected'

class wxFSFilePtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetStream(self, *_args, **_kwargs):
        val = filesysc.wxFSFile_GetStream(self, *_args, **_kwargs)
        return val
    def GetMimeType(self, *_args, **_kwargs):
        val = filesysc.wxFSFile_GetMimeType(self, *_args, **_kwargs)
        return val
    def GetLocation(self, *_args, **_kwargs):
        val = filesysc.wxFSFile_GetLocation(self, *_args, **_kwargs)
        return val
    def GetAnchor(self, *_args, **_kwargs):
        val = filesysc.wxFSFile_GetAnchor(self, *_args, **_kwargs)
        return val
    def GetModificationTime(self, *_args, **_kwargs):
        val = filesysc.wxFSFile_GetModificationTime(self, *_args, **_kwargs)
        if val: val = wxDateTimePtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxFSFile instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxFSFile(wxFSFilePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = filesysc.new_wxFSFile(*_args,**_kwargs)
        self.thisown = 1




class wxCPPFileSystemHandlerPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxCPPFileSystemHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxCPPFileSystemHandler(wxCPPFileSystemHandlerPtr):
    def __init__(self,this):
        self.this = this




class wxFileSystemHandlerPtr(wxCPPFileSystemHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = filesysc.wxFileSystemHandler__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def CanOpen(self, *_args, **_kwargs):
        val = filesysc.wxFileSystemHandler_CanOpen(self, *_args, **_kwargs)
        return val
    def OpenFile(self, *_args, **_kwargs):
        val = filesysc.wxFileSystemHandler_OpenFile(self, *_args, **_kwargs)
        return val
    def FindFirst(self, *_args, **_kwargs):
        val = filesysc.wxFileSystemHandler_FindFirst(self, *_args, **_kwargs)
        return val
    def FindNext(self, *_args, **_kwargs):
        val = filesysc.wxFileSystemHandler_FindNext(self, *_args, **_kwargs)
        return val
    def GetProtocol(self, *_args, **_kwargs):
        val = filesysc.wxFileSystemHandler_GetProtocol(self, *_args, **_kwargs)
        return val
    def GetLeftLocation(self, *_args, **_kwargs):
        val = filesysc.wxFileSystemHandler_GetLeftLocation(self, *_args, **_kwargs)
        return val
    def GetAnchor(self, *_args, **_kwargs):
        val = filesysc.wxFileSystemHandler_GetAnchor(self, *_args, **_kwargs)
        return val
    def GetRightLocation(self, *_args, **_kwargs):
        val = filesysc.wxFileSystemHandler_GetRightLocation(self, *_args, **_kwargs)
        return val
    def GetMimeTypeFromExt(self, *_args, **_kwargs):
        val = filesysc.wxFileSystemHandler_GetMimeTypeFromExt(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxFileSystemHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxFileSystemHandler(wxFileSystemHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = filesysc.new_wxFileSystemHandler(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxFileSystemHandler)




class wxFileSystemPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def ChangePathTo(self, *_args, **_kwargs):
        val = filesysc.wxFileSystem_ChangePathTo(self, *_args, **_kwargs)
        return val
    def GetPath(self, *_args, **_kwargs):
        val = filesysc.wxFileSystem_GetPath(self, *_args, **_kwargs)
        return val
    def OpenFile(self, *_args, **_kwargs):
        val = filesysc.wxFileSystem_OpenFile(self, *_args, **_kwargs)
        return val
    def FindFirst(self, *_args, **_kwargs):
        val = filesysc.wxFileSystem_FindFirst(self, *_args, **_kwargs)
        return val
    def FindNext(self, *_args, **_kwargs):
        val = filesysc.wxFileSystem_FindNext(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxFileSystem instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxFileSystem(wxFileSystemPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = filesysc.new_wxFileSystem(*_args,**_kwargs)
        self.thisown = 1




class wxInternetFSHandlerPtr(wxCPPFileSystemHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def CanOpen(self, *_args, **_kwargs):
        val = filesysc.wxInternetFSHandler_CanOpen(self, *_args, **_kwargs)
        return val
    def OpenFile(self, *_args, **_kwargs):
        val = filesysc.wxInternetFSHandler_OpenFile(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxInternetFSHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxInternetFSHandler(wxInternetFSHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = filesysc.new_wxInternetFSHandler(*_args,**_kwargs)
        self.thisown = 1




class wxZipFSHandlerPtr(wxCPPFileSystemHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def CanOpen(self, *_args, **_kwargs):
        val = filesysc.wxZipFSHandler_CanOpen(self, *_args, **_kwargs)
        return val
    def OpenFile(self, *_args, **_kwargs):
        val = filesysc.wxZipFSHandler_OpenFile(self, *_args, **_kwargs)
        return val
    def FindFirst(self, *_args, **_kwargs):
        val = filesysc.wxZipFSHandler_FindFirst(self, *_args, **_kwargs)
        return val
    def FindNext(self, *_args, **_kwargs):
        val = filesysc.wxZipFSHandler_FindNext(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxZipFSHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxZipFSHandler(wxZipFSHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = filesysc.new_wxZipFSHandler(*_args,**_kwargs)
        self.thisown = 1




class wxMemoryFSHandlerPtr(wxCPPFileSystemHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def CanOpen(self, *_args, **_kwargs):
        val = filesysc.wxMemoryFSHandler_CanOpen(self, *_args, **_kwargs)
        return val
    def OpenFile(self, *_args, **_kwargs):
        val = filesysc.wxMemoryFSHandler_OpenFile(self, *_args, **_kwargs)
        return val
    def FindFirst(self, *_args, **_kwargs):
        val = filesysc.wxMemoryFSHandler_FindFirst(self, *_args, **_kwargs)
        return val
    def FindNext(self, *_args, **_kwargs):
        val = filesysc.wxMemoryFSHandler_FindNext(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxMemoryFSHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxMemoryFSHandler(wxMemoryFSHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = filesysc.new_wxMemoryFSHandler(*_args,**_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

wxFileSystem_URLToFileName = filesysc.wxFileSystem_URLToFileName

__wxMemoryFSHandler_AddFile_wxImage = filesysc.__wxMemoryFSHandler_AddFile_wxImage

__wxMemoryFSHandler_AddFile_wxBitmap = filesysc.__wxMemoryFSHandler_AddFile_wxBitmap

__wxMemoryFSHandler_AddFile_Data = filesysc.__wxMemoryFSHandler_AddFile_Data

wxFileSystem_AddHandler = filesysc.wxFileSystem_AddHandler

wxFileSystem_CleanUpHandlers = filesysc.wxFileSystem_CleanUpHandlers

wxFileSystem_FileNameToURL = filesysc.wxFileSystem_FileNameToURL

wxMemoryFSHandler_RemoveFile = filesysc.wxMemoryFSHandler_RemoveFile



#-------------- VARIABLE WRAPPERS ------------------

