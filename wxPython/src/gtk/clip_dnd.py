# This file was created automatically by SWIG.
import clip_dndc

from misc import *

from gdi import *
import wx
class wxDataFormatPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,clip_dndc=clip_dndc):
        if self.thisown == 1 :
            clip_dndc.delete_wxDataFormat(self)
    def SetType(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDataFormat_SetType,(self,) + _args, _kwargs)
        return val
    def GetType(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDataFormat_GetType,(self,) + _args, _kwargs)
        return val
    def GetId(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDataFormat_GetId,(self,) + _args, _kwargs)
        return val
    def SetId(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDataFormat_SetId,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxDataFormat instance at %s>" % (self.this,)
class wxDataFormat(wxDataFormatPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(clip_dndc.new_wxDataFormat,_args,_kwargs)
        self.thisown = 1




class wxDataObjectPtr :
    Get = clip_dndc.wxDataObject_Get
    Set = clip_dndc.wxDataObject_Set
    Both = clip_dndc.wxDataObject_Both
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,clip_dndc=clip_dndc):
        if self.thisown == 1 :
            clip_dndc.delete_wxDataObject(self)
    def GetPreferredFormat(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDataObject_GetPreferredFormat,(self,) + _args, _kwargs)
        if val: val = wxDataFormatPtr(val) ; val.thisown = 1
        return val
    def GetFormatCount(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDataObject_GetFormatCount,(self,) + _args, _kwargs)
        return val
    def GetAllFormats(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDataObject_GetAllFormats,(self,) + _args, _kwargs)
        return val
    def GetDataSize(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDataObject_GetDataSize,(self,) + _args, _kwargs)
        return val
    def GetDataHere(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDataObject_GetDataHere,(self,) + _args, _kwargs)
        return val
    def SetData(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDataObject_SetData,(self,) + _args, _kwargs)
        return val
    def IsSupportedFormat(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDataObject_IsSupportedFormat,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxDataObject instance at %s>" % (self.this,)
class wxDataObject(wxDataObjectPtr):
    def __init__(self,this):
        self.this = this




class wxDataObjectSimplePtr(wxDataObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetFormat(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDataObjectSimple_GetFormat,(self,) + _args, _kwargs)
        if val: val = wxDataFormatPtr(val) 
        return val
    def SetFormat(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDataObjectSimple_SetFormat,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxDataObjectSimple instance at %s>" % (self.this,)
class wxDataObjectSimple(wxDataObjectSimplePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(clip_dndc.new_wxDataObjectSimple,_args,_kwargs)
        self.thisown = 1




class wxPyDataObjectSimplePtr(wxDataObjectSimplePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxPyDataObjectSimple__setSelf,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPyDataObjectSimple instance at %s>" % (self.this,)
class wxPyDataObjectSimple(wxPyDataObjectSimplePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(clip_dndc.new_wxPyDataObjectSimple,_args,_kwargs)
        self.thisown = 1
        self._setSelf(self, wxPyDataObjectSimple)




class wxDataObjectCompositePtr(wxDataObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Add(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDataObjectComposite_Add,(self,) + _args, _kwargs)
        _args[0].thisown = 0
        return val
    def __repr__(self):
        return "<C wxDataObjectComposite instance at %s>" % (self.this,)
class wxDataObjectComposite(wxDataObjectCompositePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(clip_dndc.new_wxDataObjectComposite,_args,_kwargs)
        self.thisown = 1




class wxTextDataObjectPtr(wxDataObjectSimplePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetTextLength(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxTextDataObject_GetTextLength,(self,) + _args, _kwargs)
        return val
    def GetText(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxTextDataObject_GetText,(self,) + _args, _kwargs)
        return val
    def SetText(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxTextDataObject_SetText,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxTextDataObject instance at %s>" % (self.this,)
class wxTextDataObject(wxTextDataObjectPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(clip_dndc.new_wxTextDataObject,_args,_kwargs)
        self.thisown = 1




class wxPyTextDataObjectPtr(wxTextDataObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxPyTextDataObject__setSelf,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPyTextDataObject instance at %s>" % (self.this,)
class wxPyTextDataObject(wxPyTextDataObjectPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(clip_dndc.new_wxPyTextDataObject,_args,_kwargs)
        self.thisown = 1
        self._setSelf(self, wxPyTextDataObject)




class wxBitmapDataObjectPtr(wxDataObjectSimplePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetBitmap(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxBitmapDataObject_GetBitmap,(self,) + _args, _kwargs)
        if val: val = wxBitmapPtr(val) ; val.thisown = 1
        return val
    def SetBitmap(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxBitmapDataObject_SetBitmap,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxBitmapDataObject instance at %s>" % (self.this,)
class wxBitmapDataObject(wxBitmapDataObjectPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(clip_dndc.new_wxBitmapDataObject,_args,_kwargs)
        self.thisown = 1




class wxPyBitmapDataObjectPtr(wxBitmapDataObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxPyBitmapDataObject__setSelf,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPyBitmapDataObject instance at %s>" % (self.this,)
class wxPyBitmapDataObject(wxPyBitmapDataObjectPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(clip_dndc.new_wxPyBitmapDataObject,_args,_kwargs)
        self.thisown = 1
        self._setSelf(self, wxPyBitmapDataObject)




class wxFileDataObjectPtr(wxDataObjectSimplePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetFilenames(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxFileDataObject_GetFilenames,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFileDataObject instance at %s>" % (self.this,)
class wxFileDataObject(wxFileDataObjectPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(clip_dndc.new_wxFileDataObject,_args,_kwargs)
        self.thisown = 1




class wxCustomDataObjectPtr(wxDataObjectSimplePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def TakeData(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxCustomDataObject_TakeData,(self,) + _args, _kwargs)
        return val
    def SetData(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxCustomDataObject_SetData,(self,) + _args, _kwargs)
        return val
    def GetSize(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxCustomDataObject_GetSize,(self,) + _args, _kwargs)
        return val
    def GetData(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxCustomDataObject_GetData,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxCustomDataObject instance at %s>" % (self.this,)
class wxCustomDataObject(wxCustomDataObjectPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(clip_dndc.new_wxCustomDataObject,_args,_kwargs)
        self.thisown = 1




class wxClipboardPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Open(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxClipboard_Open,(self,) + _args, _kwargs)
        return val
    def Close(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxClipboard_Close,(self,) + _args, _kwargs)
        return val
    def IsOpened(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxClipboard_IsOpened,(self,) + _args, _kwargs)
        return val
    def AddData(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxClipboard_AddData,(self,) + _args, _kwargs)
        _args[0].thisown = 0
        return val
    def SetData(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxClipboard_SetData,(self,) + _args, _kwargs)
        _args[0].thisown = 0
        return val
    def IsSupported(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxClipboard_IsSupported,(self,) + _args, _kwargs)
        return val
    def GetData(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxClipboard_GetData,(self,) + _args, _kwargs)
        return val
    def Clear(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxClipboard_Clear,(self,) + _args, _kwargs)
        return val
    def Flush(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxClipboard_Flush,(self,) + _args, _kwargs)
        return val
    def UsePrimarySelection(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxClipboard_UsePrimarySelection,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxClipboard instance at %s>" % (self.this,)
class wxClipboard(wxClipboardPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(clip_dndc.new_wxClipboard,_args,_kwargs)
        self.thisown = 1




class wxDropSourcePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDropSource__setSelf,(self,) + _args, _kwargs)
        return val
    def __del__(self,clip_dndc=clip_dndc):
        if self.thisown == 1 :
            clip_dndc.delete_wxDropSource(self)
    def SetData(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDropSource_SetData,(self,) + _args, _kwargs)
        return val
    def GetDataObject(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDropSource_GetDataObject,(self,) + _args, _kwargs)
        if val: val = wxDataObjectPtr(val) 
        return val
    def SetCursor(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDropSource_SetCursor,(self,) + _args, _kwargs)
        return val
    def DoDragDrop(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDropSource_DoDragDrop,(self,) + _args, _kwargs)
        return val
    def base_GiveFeedback(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxDropSource_base_GiveFeedback,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxDropSource instance at %s>" % (self.this,)
class wxDropSource(wxDropSourcePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(clip_dndc.new_wxDropSource,_args,_kwargs)
        self.thisown = 1
        self._setSelf(self, wxDropSource, 0)




class wxDropTargetPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxDropTarget instance at %s>" % (self.this,)
class wxDropTarget(wxDropTargetPtr):
    def __init__(self,this):
        self.this = this




class wxPyDropTargetPtr(wxDropTargetPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxPyDropTarget__setSelf,(self,) + _args, _kwargs)
        return val
    def __del__(self,clip_dndc=clip_dndc):
        if self.thisown == 1 :
            clip_dndc.delete_wxPyDropTarget(self)
    def GetDataObject(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxPyDropTarget_GetDataObject,(self,) + _args, _kwargs)
        if val: val = wxDataObjectPtr(val) 
        return val
    def SetDataObject(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxPyDropTarget_SetDataObject,(self,) + _args, _kwargs)
        if _args:_args[0].thisown = 0
        return val
    def base_OnEnter(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxPyDropTarget_base_OnEnter,(self,) + _args, _kwargs)
        return val
    def base_OnDragOver(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxPyDropTarget_base_OnDragOver,(self,) + _args, _kwargs)
        return val
    def base_OnLeave(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxPyDropTarget_base_OnLeave,(self,) + _args, _kwargs)
        return val
    def base_OnDrop(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxPyDropTarget_base_OnDrop,(self,) + _args, _kwargs)
        return val
    def GetData(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxPyDropTarget_GetData,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPyDropTarget instance at %s>" % (self.this,)
class wxPyDropTarget(wxPyDropTargetPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(clip_dndc.new_wxPyDropTarget,_args,_kwargs)
        self.thisown = 1
        if _args:_args[0].thisown = 0
        self._setSelf(self, wxPyDropTarget)




class wxTextDropTargetPtr(wxPyDropTargetPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxTextDropTarget__setSelf,(self,) + _args, _kwargs)
        return val
    def base_OnEnter(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxTextDropTarget_base_OnEnter,(self,) + _args, _kwargs)
        return val
    def base_OnDragOver(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxTextDropTarget_base_OnDragOver,(self,) + _args, _kwargs)
        return val
    def base_OnLeave(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxTextDropTarget_base_OnLeave,(self,) + _args, _kwargs)
        return val
    def base_OnDrop(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxTextDropTarget_base_OnDrop,(self,) + _args, _kwargs)
        return val
    def base_OnData(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxTextDropTarget_base_OnData,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxTextDropTarget instance at %s>" % (self.this,)
class wxTextDropTarget(wxTextDropTargetPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(clip_dndc.new_wxTextDropTarget,_args,_kwargs)
        self.thisown = 1
        self._setSelf(self, wxTextDropTarget)




class wxFileDropTargetPtr(wxPyDropTargetPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxFileDropTarget__setSelf,(self,) + _args, _kwargs)
        return val
    def base_OnEnter(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxFileDropTarget_base_OnEnter,(self,) + _args, _kwargs)
        return val
    def base_OnDragOver(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxFileDropTarget_base_OnDragOver,(self,) + _args, _kwargs)
        return val
    def base_OnLeave(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxFileDropTarget_base_OnLeave,(self,) + _args, _kwargs)
        return val
    def base_OnDrop(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxFileDropTarget_base_OnDrop,(self,) + _args, _kwargs)
        return val
    def base_OnData(self, *_args, **_kwargs):
        val = apply(clip_dndc.wxFileDropTarget_base_OnData,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFileDropTarget instance at %s>" % (self.this,)
class wxFileDropTarget(wxFileDropTargetPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(clip_dndc.new_wxFileDropTarget,_args,_kwargs)
        self.thisown = 1
        self._setSelf(self, wxFileDropTarget)






#-------------- FUNCTION WRAPPERS ------------------

def wxCustomDataFormat(*_args, **_kwargs):
    val = apply(clip_dndc.wxCustomDataFormat,_args,_kwargs)
    if val: val = wxDataFormatPtr(val); val.thisown = 1
    return val

wxIsDragResultOk = clip_dndc.wxIsDragResultOk



#-------------- VARIABLE WRAPPERS ------------------

wxDF_INVALID = clip_dndc.wxDF_INVALID
wxDF_TEXT = clip_dndc.wxDF_TEXT
wxDF_BITMAP = clip_dndc.wxDF_BITMAP
wxDF_METAFILE = clip_dndc.wxDF_METAFILE
wxDF_SYLK = clip_dndc.wxDF_SYLK
wxDF_DIF = clip_dndc.wxDF_DIF
wxDF_TIFF = clip_dndc.wxDF_TIFF
wxDF_OEMTEXT = clip_dndc.wxDF_OEMTEXT
wxDF_DIB = clip_dndc.wxDF_DIB
wxDF_PALETTE = clip_dndc.wxDF_PALETTE
wxDF_PENDATA = clip_dndc.wxDF_PENDATA
wxDF_RIFF = clip_dndc.wxDF_RIFF
wxDF_WAVE = clip_dndc.wxDF_WAVE
wxDF_UNICODETEXT = clip_dndc.wxDF_UNICODETEXT
wxDF_ENHMETAFILE = clip_dndc.wxDF_ENHMETAFILE
wxDF_FILENAME = clip_dndc.wxDF_FILENAME
wxDF_LOCALE = clip_dndc.wxDF_LOCALE
wxDF_PRIVATE = clip_dndc.wxDF_PRIVATE
wxDF_MAX = clip_dndc.wxDF_MAX
cvar = clip_dndc.cvar
wxFormatInvalid = wxDataFormatPtr(clip_dndc.cvar.wxFormatInvalid)
wxTheClipboard = wxClipboardPtr(clip_dndc.cvar.wxTheClipboard)
wxDragError = clip_dndc.wxDragError
wxDragNone = clip_dndc.wxDragNone
wxDragCopy = clip_dndc.wxDragCopy
wxDragMove = clip_dndc.wxDragMove
wxDragCancel = clip_dndc.wxDragCancel
