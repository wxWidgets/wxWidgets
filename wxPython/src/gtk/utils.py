# This file was created automatically by SWIG.
import utilsc
class wxConfigPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,utilsc=utilsc):
        if self.thisown == 1 :
            utilsc.delete_wxConfig(self)
    def DontCreateOnDemand(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_DontCreateOnDemand,(self,) + _args, _kwargs)
        return val
    def DeleteAll(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_DeleteAll,(self,) + _args, _kwargs)
        return val
    def DeleteEntry(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_DeleteEntry,(self,) + _args, _kwargs)
        return val
    def DeleteGroup(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_DeleteGroup,(self,) + _args, _kwargs)
        return val
    def Exists(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_Exists,(self,) + _args, _kwargs)
        return val
    def Flush(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_Flush,(self,) + _args, _kwargs)
        return val
    def GetAppName(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_GetAppName,(self,) + _args, _kwargs)
        return val
    def GetFirstGroup(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_GetFirstGroup,(self,) + _args, _kwargs)
        return val
    def GetFirstEntry(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_GetFirstEntry,(self,) + _args, _kwargs)
        return val
    def GetNextGroup(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_GetNextGroup,(self,) + _args, _kwargs)
        return val
    def GetNextEntry(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_GetNextEntry,(self,) + _args, _kwargs)
        return val
    def GetNumberOfEntries(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_GetNumberOfEntries,(self,) + _args, _kwargs)
        return val
    def GetNumberOfGroups(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_GetNumberOfGroups,(self,) + _args, _kwargs)
        return val
    def GetPath(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_GetPath,(self,) + _args, _kwargs)
        return val
    def GetVendorName(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_GetVendorName,(self,) + _args, _kwargs)
        return val
    def HasEntry(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_HasEntry,(self,) + _args, _kwargs)
        return val
    def HasGroup(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_HasGroup,(self,) + _args, _kwargs)
        return val
    def IsExpandingEnvVars(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_IsExpandingEnvVars,(self,) + _args, _kwargs)
        return val
    def IsRecordingDefaults(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_IsRecordingDefaults,(self,) + _args, _kwargs)
        return val
    def Read(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_Read,(self,) + _args, _kwargs)
        return val
    def ReadInt(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_ReadInt,(self,) + _args, _kwargs)
        return val
    def ReadFloat(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_ReadFloat,(self,) + _args, _kwargs)
        return val
    def SetExpandEnvVars(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_SetExpandEnvVars,(self,) + _args, _kwargs)
        return val
    def SetPath(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_SetPath,(self,) + _args, _kwargs)
        return val
    def SetRecordDefaults(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_SetRecordDefaults,(self,) + _args, _kwargs)
        return val
    def Write(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_Write,(self,) + _args, _kwargs)
        return val
    def WriteInt(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_WriteInt,(self,) + _args, _kwargs)
        return val
    def WriteFloat(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfig_WriteFloat,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxConfig instance at %s>" % (self.this,)
class wxConfig(wxConfigPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(utilsc.new_wxConfig,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

