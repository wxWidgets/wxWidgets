# This file was created automatically by SWIG.
import utilsc
class wxConfigPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, utilsc=utilsc):
        if self.thisown == 1 :
            utilsc.delete_wxConfig(self.this)
    def DontCreateOnDemand(self):
        val = utilsc.wxConfig_DontCreateOnDemand(self.this)
        return val
    def DeleteAll(self):
        val = utilsc.wxConfig_DeleteAll(self.this)
        return val
    def DeleteEntry(self,arg0,*args):
        val = apply(utilsc.wxConfig_DeleteEntry,(self.this,arg0,)+args)
        return val
    def DeleteGroup(self,arg0):
        val = utilsc.wxConfig_DeleteGroup(self.this,arg0)
        return val
    def Exists(self,arg0):
        val = utilsc.wxConfig_Exists(self.this,arg0)
        return val
    def Flush(self,*args):
        val = apply(utilsc.wxConfig_Flush,(self.this,)+args)
        return val
    def GetAppName(self):
        val = utilsc.wxConfig_GetAppName(self.this)
        return val
    def GetFirstGroup(self):
        val = utilsc.wxConfig_GetFirstGroup(self.this)
        return val
    def GetFirstEntry(self):
        val = utilsc.wxConfig_GetFirstEntry(self.this)
        return val
    def GetNextGroup(self,arg0):
        val = utilsc.wxConfig_GetNextGroup(self.this,arg0)
        return val
    def GetNextEntry(self,arg0):
        val = utilsc.wxConfig_GetNextEntry(self.this,arg0)
        return val
    def GetNumberOfEntries(self,*args):
        val = apply(utilsc.wxConfig_GetNumberOfEntries,(self.this,)+args)
        return val
    def GetNumberOfGroups(self,*args):
        val = apply(utilsc.wxConfig_GetNumberOfGroups,(self.this,)+args)
        return val
    def GetPath(self):
        val = utilsc.wxConfig_GetPath(self.this)
        return val
    def GetVendorName(self):
        val = utilsc.wxConfig_GetVendorName(self.this)
        return val
    def HasEntry(self,arg0):
        val = utilsc.wxConfig_HasEntry(self.this,arg0)
        return val
    def HasGroup(self,arg0):
        val = utilsc.wxConfig_HasGroup(self.this,arg0)
        return val
    def IsExpandingEnvVars(self):
        val = utilsc.wxConfig_IsExpandingEnvVars(self.this)
        return val
    def IsRecordingDefaults(self):
        val = utilsc.wxConfig_IsRecordingDefaults(self.this)
        return val
    def Read(self,arg0,*args):
        val = apply(utilsc.wxConfig_Read,(self.this,arg0,)+args)
        return val
    def ReadInt(self,arg0,*args):
        val = apply(utilsc.wxConfig_ReadInt,(self.this,arg0,)+args)
        return val
    def ReadFloat(self,arg0,*args):
        val = apply(utilsc.wxConfig_ReadFloat,(self.this,arg0,)+args)
        return val
    def SetExpandEnvVars(self,*args):
        val = apply(utilsc.wxConfig_SetExpandEnvVars,(self.this,)+args)
        return val
    def SetPath(self,arg0):
        val = utilsc.wxConfig_SetPath(self.this,arg0)
        return val
    def SetRecordDefaults(self,*args):
        val = apply(utilsc.wxConfig_SetRecordDefaults,(self.this,)+args)
        return val
    def Write(self,arg0,arg1):
        val = utilsc.wxConfig_Write(self.this,arg0,arg1)
        return val
    def WriteInt(self,arg0,arg1):
        val = utilsc.wxConfig_WriteInt(self.this,arg0,arg1)
        return val
    def WriteFloat(self,arg0,arg1):
        val = utilsc.wxConfig_WriteFloat(self.this,arg0,arg1)
        return val
    def __repr__(self):
        return "<C wxConfig instance>"
class wxConfig(wxConfigPtr):
    def __init__(self,*args) :
        self.this = apply(utilsc.new_wxConfig,()+args)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

