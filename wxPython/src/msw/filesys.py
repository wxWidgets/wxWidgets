# This file was created automatically by SWIG.
import filesysc

from utils import *

from image import *

from misc import *

from gdi import *

from streams import *
import wx
import string

import types
def wxMemoryFSHandler_AddFile(filename, a, b=''):
    if isinstance(a, wxImage):
        __wxMemoryFSHandler_AddFile_wxImage(filename, a, b)
    elif isinstance(a, wxBitmap):
        __wxMemoryFSHandler_AddFile_wxBitmap(filename, a, b)
    elif type(a) == types.StringType:
        #__wxMemoryFSHandler_AddFile_wxString(filename, a)
        __wxMemoryFSHandler_AddFile_Data(filename, a)
    else: raise TypeError, 'wxImage, wxBitmap or string expected'

class wxFSFilePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetStream(self, *_args, **_kwargs):
        val = apply(filesysc.wxFSFile_GetStream,(self,) + _args, _kwargs)
        return val
    def GetMimeType(self, *_args, **_kwargs):
        val = apply(filesysc.wxFSFile_GetMimeType,(self,) + _args, _kwargs)
        return val
    def GetLocation(self, *_args, **_kwargs):
        val = apply(filesysc.wxFSFile_GetLocation,(self,) + _args, _kwargs)
        return val
    def GetAnchor(self, *_args, **_kwargs):
        val = apply(filesysc.wxFSFile_GetAnchor,(self,) + _args, _kwargs)
        return val
    def GetModificationTime(self, *_args, **_kwargs):
        val = apply(filesysc.wxFSFile_GetModificationTime,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<C wxFSFile instance at %s>" % (self.this,)
class wxFSFile(wxFSFilePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(filesysc.new_wxFSFile,_args,_kwargs)
        self.thisown = 1




class wxCPPFileSystemHandlerPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxCPPFileSystemHandler instance at %s>" % (self.this,)
class wxCPPFileSystemHandler(wxCPPFileSystemHandlerPtr):
    def __init__(self,this):
        self.this = this




class wxFileSystemHandlerPtr(wxCPPFileSystemHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self, *_args, **_kwargs):
        val = apply(filesysc.wxFileSystemHandler__setSelf,(self,) + _args, _kwargs)
        return val
    def CanOpen(self, *_args, **_kwargs):
        val = apply(filesysc.wxFileSystemHandler_CanOpen,(self,) + _args, _kwargs)
        return val
    def OpenFile(self, *_args, **_kwargs):
        val = apply(filesysc.wxFileSystemHandler_OpenFile,(self,) + _args, _kwargs)
        if val: val = wxFSFilePtr(val) 
        return val
    def FindFirst(self, *_args, **_kwargs):
        val = apply(filesysc.wxFileSystemHandler_FindFirst,(self,) + _args, _kwargs)
        return val
    def FindNext(self, *_args, **_kwargs):
        val = apply(filesysc.wxFileSystemHandler_FindNext,(self,) + _args, _kwargs)
        return val
    def GetProtocol(self, *_args, **_kwargs):
        val = apply(filesysc.wxFileSystemHandler_GetProtocol,(self,) + _args, _kwargs)
        return val
    def GetLeftLocation(self, *_args, **_kwargs):
        val = apply(filesysc.wxFileSystemHandler_GetLeftLocation,(self,) + _args, _kwargs)
        return val
    def GetAnchor(self, *_args, **_kwargs):
        val = apply(filesysc.wxFileSystemHandler_GetAnchor,(self,) + _args, _kwargs)
        return val
    def GetRightLocation(self, *_args, **_kwargs):
        val = apply(filesysc.wxFileSystemHandler_GetRightLocation,(self,) + _args, _kwargs)
        return val
    def GetMimeTypeFromExt(self, *_args, **_kwargs):
        val = apply(filesysc.wxFileSystemHandler_GetMimeTypeFromExt,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFileSystemHandler instance at %s>" % (self.this,)
class wxFileSystemHandler(wxFileSystemHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(filesysc.new_wxFileSystemHandler,_args,_kwargs)
        self.thisown = 1
        self._setSelf(self, wxFileSystemHandler)




class wxFileSystemPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def ChangePathTo(self, *_args, **_kwargs):
        val = apply(filesysc.wxFileSystem_ChangePathTo,(self,) + _args, _kwargs)
        return val
    def GetPath(self, *_args, **_kwargs):
        val = apply(filesysc.wxFileSystem_GetPath,(self,) + _args, _kwargs)
        return val
    def OpenFile(self, *_args, **_kwargs):
        val = apply(filesysc.wxFileSystem_OpenFile,(self,) + _args, _kwargs)
        if val: val = wxFSFilePtr(val) 
        return val
    def FindFirst(self, *_args, **_kwargs):
        val = apply(filesysc.wxFileSystem_FindFirst,(self,) + _args, _kwargs)
        return val
    def FindNext(self, *_args, **_kwargs):
        val = apply(filesysc.wxFileSystem_FindNext,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFileSystem instance at %s>" % (self.this,)
class wxFileSystem(wxFileSystemPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(filesysc.new_wxFileSystem,_args,_kwargs)
        self.thisown = 1




class wxInternetFSHandlerPtr(wxCPPFileSystemHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def CanOpen(self, *_args, **_kwargs):
        val = apply(filesysc.wxInternetFSHandler_CanOpen,(self,) + _args, _kwargs)
        return val
    def OpenFile(self, *_args, **_kwargs):
        val = apply(filesysc.wxInternetFSHandler_OpenFile,(self,) + _args, _kwargs)
        if val: val = wxFSFilePtr(val) 
        return val
    def __repr__(self):
        return "<C wxInternetFSHandler instance at %s>" % (self.this,)
class wxInternetFSHandler(wxInternetFSHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(filesysc.new_wxInternetFSHandler,_args,_kwargs)
        self.thisown = 1




class wxZipFSHandlerPtr(wxCPPFileSystemHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def CanOpen(self, *_args, **_kwargs):
        val = apply(filesysc.wxZipFSHandler_CanOpen,(self,) + _args, _kwargs)
        return val
    def OpenFile(self, *_args, **_kwargs):
        val = apply(filesysc.wxZipFSHandler_OpenFile,(self,) + _args, _kwargs)
        if val: val = wxFSFilePtr(val) 
        return val
    def FindFirst(self, *_args, **_kwargs):
        val = apply(filesysc.wxZipFSHandler_FindFirst,(self,) + _args, _kwargs)
        return val
    def FindNext(self, *_args, **_kwargs):
        val = apply(filesysc.wxZipFSHandler_FindNext,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxZipFSHandler instance at %s>" % (self.this,)
class wxZipFSHandler(wxZipFSHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(filesysc.new_wxZipFSHandler,_args,_kwargs)
        self.thisown = 1




class wxMemoryFSHandlerPtr(wxCPPFileSystemHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def CanOpen(self, *_args, **_kwargs):
        val = apply(filesysc.wxMemoryFSHandler_CanOpen,(self,) + _args, _kwargs)
        return val
    def OpenFile(self, *_args, **_kwargs):
        val = apply(filesysc.wxMemoryFSHandler_OpenFile,(self,) + _args, _kwargs)
        if val: val = wxFSFilePtr(val) 
        return val
    def FindFirst(self, *_args, **_kwargs):
        val = apply(filesysc.wxMemoryFSHandler_FindFirst,(self,) + _args, _kwargs)
        return val
    def FindNext(self, *_args, **_kwargs):
        val = apply(filesysc.wxMemoryFSHandler_FindNext,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxMemoryFSHandler instance at %s>" % (self.this,)
class wxMemoryFSHandler(wxMemoryFSHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(filesysc.new_wxMemoryFSHandler,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

__wxMemoryFSHandler_AddFile_wxImage = filesysc.__wxMemoryFSHandler_AddFile_wxImage

__wxMemoryFSHandler_AddFile_wxBitmap = filesysc.__wxMemoryFSHandler_AddFile_wxBitmap

__wxMemoryFSHandler_AddFile_Data = filesysc.__wxMemoryFSHandler_AddFile_Data

wxFileSystem_AddHandler = filesysc.wxFileSystem_AddHandler

wxFileSystem_CleanUpHandlers = filesysc.wxFileSystem_CleanUpHandlers

wxMemoryFSHandler_RemoveFile = filesysc.wxMemoryFSHandler_RemoveFile



#-------------- VARIABLE WRAPPERS ------------------

