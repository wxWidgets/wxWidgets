# This file was created automatically by SWIG.
import miscc
class wxSizePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,miscc=miscc):
        if self.thisown == 1 :
            miscc.delete_wxSize(self)
    def Set(self, *_args, **_kwargs):
        val = apply(miscc.wxSize_Set,(self,) + _args, _kwargs)
        return val
    def GetX(self, *_args, **_kwargs):
        val = apply(miscc.wxSize_GetX,(self,) + _args, _kwargs)
        return val
    def GetY(self, *_args, **_kwargs):
        val = apply(miscc.wxSize_GetY,(self,) + _args, _kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = apply(miscc.wxSize_GetWidth,(self,) + _args, _kwargs)
        return val
    def GetHeight(self, *_args, **_kwargs):
        val = apply(miscc.wxSize_GetHeight,(self,) + _args, _kwargs)
        return val
    def SetWidth(self, *_args, **_kwargs):
        val = apply(miscc.wxSize_SetWidth,(self,) + _args, _kwargs)
        return val
    def SetHeight(self, *_args, **_kwargs):
        val = apply(miscc.wxSize_SetHeight,(self,) + _args, _kwargs)
        return val
    def asTuple(self, *_args, **_kwargs):
        val = apply(miscc.wxSize_asTuple,(self,) + _args, _kwargs)
        return val
    def __setattr__(self,name,value):
        if name == "x" :
            miscc.wxSize_x_set(self,value)
            return
        if name == "y" :
            miscc.wxSize_y_set(self,value)
            return
        if name == "width" :
            miscc.wxSize_width_set(self,value)
            return
        if name == "height" :
            miscc.wxSize_height_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "x" : 
            return miscc.wxSize_x_get(self)
        if name == "y" : 
            return miscc.wxSize_y_get(self)
        if name == "width" : 
            return miscc.wxSize_width_get(self)
        if name == "height" : 
            return miscc.wxSize_height_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C wxSize instance at %s>" % (self.this,)
    def __str__(self): return str(self.asTuple())
    def __repr__(self): return str(self.asTuple())
class wxSize(wxSizePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(miscc.new_wxSize,_args,_kwargs)
        self.thisown = 1




class wxRealPointPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,miscc=miscc):
        if self.thisown == 1 :
            miscc.delete_wxRealPoint(self)
    def Set(self, *_args, **_kwargs):
        val = apply(miscc.wxRealPoint_Set,(self,) + _args, _kwargs)
        return val
    def asTuple(self, *_args, **_kwargs):
        val = apply(miscc.wxRealPoint_asTuple,(self,) + _args, _kwargs)
        return val
    def __setattr__(self,name,value):
        if name == "x" :
            miscc.wxRealPoint_x_set(self,value)
            return
        if name == "y" :
            miscc.wxRealPoint_y_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "x" : 
            return miscc.wxRealPoint_x_get(self)
        if name == "y" : 
            return miscc.wxRealPoint_y_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C wxRealPoint instance at %s>" % (self.this,)
    def __str__(self): return str(self.asTuple())
    def __repr__(self): return str(self.asTuple())
class wxRealPoint(wxRealPointPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(miscc.new_wxRealPoint,_args,_kwargs)
        self.thisown = 1




class wxPointPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,miscc=miscc):
        if self.thisown == 1 :
            miscc.delete_wxPoint(self)
    def Set(self, *_args, **_kwargs):
        val = apply(miscc.wxPoint_Set,(self,) + _args, _kwargs)
        return val
    def asTuple(self, *_args, **_kwargs):
        val = apply(miscc.wxPoint_asTuple,(self,) + _args, _kwargs)
        return val
    def __setattr__(self,name,value):
        if name == "x" :
            miscc.wxPoint_x_set(self,value)
            return
        if name == "y" :
            miscc.wxPoint_y_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "x" : 
            return miscc.wxPoint_x_get(self)
        if name == "y" : 
            return miscc.wxPoint_y_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C wxPoint instance at %s>" % (self.this,)
    def __str__(self): return str(self.asTuple())
    def __repr__(self): return str(self.asTuple())
class wxPoint(wxPointPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(miscc.new_wxPoint,_args,_kwargs)
        self.thisown = 1




class wxRectPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,miscc=miscc):
        if self.thisown == 1 :
            miscc.delete_wxRect(self)
    def GetX(self, *_args, **_kwargs):
        val = apply(miscc.wxRect_GetX,(self,) + _args, _kwargs)
        return val
    def SetX(self, *_args, **_kwargs):
        val = apply(miscc.wxRect_SetX,(self,) + _args, _kwargs)
        return val
    def GetY(self, *_args, **_kwargs):
        val = apply(miscc.wxRect_GetY,(self,) + _args, _kwargs)
        return val
    def SetY(self, *_args, **_kwargs):
        val = apply(miscc.wxRect_SetY,(self,) + _args, _kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = apply(miscc.wxRect_GetWidth,(self,) + _args, _kwargs)
        return val
    def SetWidth(self, *_args, **_kwargs):
        val = apply(miscc.wxRect_SetWidth,(self,) + _args, _kwargs)
        return val
    def GetHeight(self, *_args, **_kwargs):
        val = apply(miscc.wxRect_GetHeight,(self,) + _args, _kwargs)
        return val
    def SetHeight(self, *_args, **_kwargs):
        val = apply(miscc.wxRect_SetHeight,(self,) + _args, _kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = apply(miscc.wxRect_GetPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetSize(self, *_args, **_kwargs):
        val = apply(miscc.wxRect_GetSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetLeft(self, *_args, **_kwargs):
        val = apply(miscc.wxRect_GetLeft,(self,) + _args, _kwargs)
        return val
    def GetTop(self, *_args, **_kwargs):
        val = apply(miscc.wxRect_GetTop,(self,) + _args, _kwargs)
        return val
    def GetBottom(self, *_args, **_kwargs):
        val = apply(miscc.wxRect_GetBottom,(self,) + _args, _kwargs)
        return val
    def GetRight(self, *_args, **_kwargs):
        val = apply(miscc.wxRect_GetRight,(self,) + _args, _kwargs)
        return val
    def asTuple(self, *_args, **_kwargs):
        val = apply(miscc.wxRect_asTuple,(self,) + _args, _kwargs)
        return val
    def __setattr__(self,name,value):
        if name == "x" :
            miscc.wxRect_x_set(self,value)
            return
        if name == "y" :
            miscc.wxRect_y_set(self,value)
            return
        if name == "width" :
            miscc.wxRect_width_set(self,value)
            return
        if name == "height" :
            miscc.wxRect_height_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "x" : 
            return miscc.wxRect_x_get(self)
        if name == "y" : 
            return miscc.wxRect_y_get(self)
        if name == "width" : 
            return miscc.wxRect_width_get(self)
        if name == "height" : 
            return miscc.wxRect_height_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C wxRect instance at %s>" % (self.this,)
    def __str__(self): return str(self.asTuple())
    def __repr__(self): return str(self.asTuple())
class wxRect(wxRectPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(miscc.new_wxRect,_args,_kwargs)
        self.thisown = 1




class wxPyTimerPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,miscc=miscc):
        if self.thisown == 1 :
            miscc.delete_wxPyTimer(self)
    def GetInterval(self, *_args, **_kwargs):
        val = apply(miscc.wxPyTimer_GetInterval,(self,) + _args, _kwargs)
        return val
    def IsOneShot(self, *_args, **_kwargs):
        val = apply(miscc.wxPyTimer_IsOneShot,(self,) + _args, _kwargs)
        return val
    def Start(self, *_args, **_kwargs):
        val = apply(miscc.wxPyTimer_Start,(self,) + _args, _kwargs)
        return val
    def Stop(self, *_args, **_kwargs):
        val = apply(miscc.wxPyTimer_Stop,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPyTimer instance at %s>" % (self.this,)
class wxPyTimer(wxPyTimerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(miscc.new_wxPyTimer,_args,_kwargs)
        self.thisown = 1




class wxIndividualLayoutConstraintPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Above(self, *_args, **_kwargs):
        val = apply(miscc.wxIndividualLayoutConstraint_Above,(self,) + _args, _kwargs)
        return val
    def Absolute(self, *_args, **_kwargs):
        val = apply(miscc.wxIndividualLayoutConstraint_Absolute,(self,) + _args, _kwargs)
        return val
    def AsIs(self, *_args, **_kwargs):
        val = apply(miscc.wxIndividualLayoutConstraint_AsIs,(self,) + _args, _kwargs)
        return val
    def Below(self, *_args, **_kwargs):
        val = apply(miscc.wxIndividualLayoutConstraint_Below,(self,) + _args, _kwargs)
        return val
    def Unconstrained(self, *_args, **_kwargs):
        val = apply(miscc.wxIndividualLayoutConstraint_Unconstrained,(self,) + _args, _kwargs)
        return val
    def LeftOf(self, *_args, **_kwargs):
        val = apply(miscc.wxIndividualLayoutConstraint_LeftOf,(self,) + _args, _kwargs)
        return val
    def PercentOf(self, *_args, **_kwargs):
        val = apply(miscc.wxIndividualLayoutConstraint_PercentOf,(self,) + _args, _kwargs)
        return val
    def RightOf(self, *_args, **_kwargs):
        val = apply(miscc.wxIndividualLayoutConstraint_RightOf,(self,) + _args, _kwargs)
        return val
    def SameAs(self, *_args, **_kwargs):
        val = apply(miscc.wxIndividualLayoutConstraint_SameAs,(self,) + _args, _kwargs)
        return val
    def Set(self, *_args, **_kwargs):
        val = apply(miscc.wxIndividualLayoutConstraint_Set,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxIndividualLayoutConstraint instance at %s>" % (self.this,)
class wxIndividualLayoutConstraint(wxIndividualLayoutConstraintPtr):
    def __init__(self,this):
        self.this = this




class wxLayoutConstraintsPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "bottom" :
            miscc.wxLayoutConstraints_bottom_set(self,value.this)
            return
        if name == "centreX" :
            miscc.wxLayoutConstraints_centreX_set(self,value.this)
            return
        if name == "centreY" :
            miscc.wxLayoutConstraints_centreY_set(self,value.this)
            return
        if name == "height" :
            miscc.wxLayoutConstraints_height_set(self,value.this)
            return
        if name == "left" :
            miscc.wxLayoutConstraints_left_set(self,value.this)
            return
        if name == "right" :
            miscc.wxLayoutConstraints_right_set(self,value.this)
            return
        if name == "top" :
            miscc.wxLayoutConstraints_top_set(self,value.this)
            return
        if name == "width" :
            miscc.wxLayoutConstraints_width_set(self,value.this)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "bottom" : 
            return wxIndividualLayoutConstraintPtr(miscc.wxLayoutConstraints_bottom_get(self))
        if name == "centreX" : 
            return wxIndividualLayoutConstraintPtr(miscc.wxLayoutConstraints_centreX_get(self))
        if name == "centreY" : 
            return wxIndividualLayoutConstraintPtr(miscc.wxLayoutConstraints_centreY_get(self))
        if name == "height" : 
            return wxIndividualLayoutConstraintPtr(miscc.wxLayoutConstraints_height_get(self))
        if name == "left" : 
            return wxIndividualLayoutConstraintPtr(miscc.wxLayoutConstraints_left_get(self))
        if name == "right" : 
            return wxIndividualLayoutConstraintPtr(miscc.wxLayoutConstraints_right_get(self))
        if name == "top" : 
            return wxIndividualLayoutConstraintPtr(miscc.wxLayoutConstraints_top_get(self))
        if name == "width" : 
            return wxIndividualLayoutConstraintPtr(miscc.wxLayoutConstraints_width_get(self))
        raise AttributeError,name
    def __repr__(self):
        return "<C wxLayoutConstraints instance at %s>" % (self.this,)
class wxLayoutConstraints(wxLayoutConstraintsPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(miscc.new_wxLayoutConstraints,_args,_kwargs)
        self.thisown = 1




class wxRegionPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,miscc=miscc):
        if self.thisown == 1 :
            miscc.delete_wxRegion(self)
    def Clear(self, *_args, **_kwargs):
        val = apply(miscc.wxRegion_Clear,(self,) + _args, _kwargs)
        return val
    def Contains(self, *_args, **_kwargs):
        val = apply(miscc.wxRegion_Contains,(self,) + _args, _kwargs)
        return val
    def ContainsPoint(self, *_args, **_kwargs):
        val = apply(miscc.wxRegion_ContainsPoint,(self,) + _args, _kwargs)
        return val
    def ContainsRect(self, *_args, **_kwargs):
        val = apply(miscc.wxRegion_ContainsRect,(self,) + _args, _kwargs)
        return val
    def GetBox(self, *_args, **_kwargs):
        val = apply(miscc.wxRegion_GetBox,(self,) + _args, _kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def Intersect(self, *_args, **_kwargs):
        val = apply(miscc.wxRegion_Intersect,(self,) + _args, _kwargs)
        return val
    def IsEmpty(self, *_args, **_kwargs):
        val = apply(miscc.wxRegion_IsEmpty,(self,) + _args, _kwargs)
        return val
    def Subtract(self, *_args, **_kwargs):
        val = apply(miscc.wxRegion_Subtract,(self,) + _args, _kwargs)
        return val
    def Union(self, *_args, **_kwargs):
        val = apply(miscc.wxRegion_Union,(self,) + _args, _kwargs)
        return val
    def Xor(self, *_args, **_kwargs):
        val = apply(miscc.wxRegion_Xor,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxRegion instance at %s>" % (self.this,)
class wxRegion(wxRegionPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(miscc.new_wxRegion,_args,_kwargs)
        self.thisown = 1




class wxRegionIteratorPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,miscc=miscc):
        if self.thisown == 1 :
            miscc.delete_wxRegionIterator(self)
    def GetX(self, *_args, **_kwargs):
        val = apply(miscc.wxRegionIterator_GetX,(self,) + _args, _kwargs)
        return val
    def GetY(self, *_args, **_kwargs):
        val = apply(miscc.wxRegionIterator_GetY,(self,) + _args, _kwargs)
        return val
    def GetW(self, *_args, **_kwargs):
        val = apply(miscc.wxRegionIterator_GetW,(self,) + _args, _kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = apply(miscc.wxRegionIterator_GetWidth,(self,) + _args, _kwargs)
        return val
    def GetH(self, *_args, **_kwargs):
        val = apply(miscc.wxRegionIterator_GetH,(self,) + _args, _kwargs)
        return val
    def GetHeight(self, *_args, **_kwargs):
        val = apply(miscc.wxRegionIterator_GetHeight,(self,) + _args, _kwargs)
        return val
    def GetRect(self, *_args, **_kwargs):
        val = apply(miscc.wxRegionIterator_GetRect,(self,) + _args, _kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def HaveRects(self, *_args, **_kwargs):
        val = apply(miscc.wxRegionIterator_HaveRects,(self,) + _args, _kwargs)
        return val
    def Reset(self, *_args, **_kwargs):
        val = apply(miscc.wxRegionIterator_Reset,(self,) + _args, _kwargs)
        return val
    def Next(self, *_args, **_kwargs):
        val = apply(miscc.wxRegionIterator_Next,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxRegionIterator instance at %s>" % (self.this,)
class wxRegionIterator(wxRegionIteratorPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(miscc.new_wxRegionIterator,_args,_kwargs)
        self.thisown = 1




class wxAcceleratorEntryPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,miscc=miscc):
        if self.thisown == 1 :
            miscc.delete_wxAcceleratorEntry(self)
    def Set(self, *_args, **_kwargs):
        val = apply(miscc.wxAcceleratorEntry_Set,(self,) + _args, _kwargs)
        return val
    def GetFlags(self, *_args, **_kwargs):
        val = apply(miscc.wxAcceleratorEntry_GetFlags,(self,) + _args, _kwargs)
        return val
    def GetKeyCode(self, *_args, **_kwargs):
        val = apply(miscc.wxAcceleratorEntry_GetKeyCode,(self,) + _args, _kwargs)
        return val
    def GetCommand(self, *_args, **_kwargs):
        val = apply(miscc.wxAcceleratorEntry_GetCommand,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxAcceleratorEntry instance at %s>" % (self.this,)
class wxAcceleratorEntry(wxAcceleratorEntryPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(miscc.new_wxAcceleratorEntry,_args,_kwargs)
        self.thisown = 1




class wxAcceleratorTablePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,miscc=miscc):
        if self.thisown == 1 :
            miscc.delete_wxAcceleratorTable(self)
    def __repr__(self):
        return "<C wxAcceleratorTable instance at %s>" % (self.this,)
class wxAcceleratorTable(wxAcceleratorTablePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(miscc.new_wxAcceleratorTable,_args,_kwargs)
        self.thisown = 1




class wxBusyInfoPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,miscc=miscc):
        if self.thisown == 1 :
            miscc.delete_wxBusyInfo(self)
    def __repr__(self):
        return "<C wxBusyInfo instance at %s>" % (self.this,)
class wxBusyInfo(wxBusyInfoPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(miscc.new_wxBusyInfo,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

wxNewId = miscc.wxNewId

wxRegisterId = miscc.wxRegisterId

NewId = miscc.NewId

RegisterId = miscc.RegisterId

wxBell = miscc.wxBell

wxDisplaySize = miscc.wxDisplaySize

wxEndBusyCursor = miscc.wxEndBusyCursor

wxExecute = miscc.wxExecute

wxGetElapsedTime = miscc.wxGetElapsedTime

wxGetFreeMemory = miscc.wxGetFreeMemory

wxGetMousePosition = miscc.wxGetMousePosition

wxIsBusy = miscc.wxIsBusy

wxNow = miscc.wxNow

wxShell = miscc.wxShell

wxStartTimer = miscc.wxStartTimer

wxGetOsVersion = miscc.wxGetOsVersion

wxSleep = miscc.wxSleep

wxYield = miscc.wxYield

wxSafeYield = miscc.wxSafeYield

wxEnableTopLevelWindows = miscc.wxEnableTopLevelWindows

wxGetResource = miscc.wxGetResource

wxStripMenuCodes = miscc.wxStripMenuCodes



#-------------- VARIABLE WRAPPERS ------------------

wxLeft = miscc.wxLeft
wxTop = miscc.wxTop
wxRight = miscc.wxRight
wxBottom = miscc.wxBottom
wxWidth = miscc.wxWidth
wxHeight = miscc.wxHeight
wxCentre = miscc.wxCentre
wxCenter = miscc.wxCenter
wxCentreX = miscc.wxCentreX
wxCentreY = miscc.wxCentreY
wxUnconstrained = miscc.wxUnconstrained
wxAsIs = miscc.wxAsIs
wxPercentOf = miscc.wxPercentOf
wxAbove = miscc.wxAbove
wxBelow = miscc.wxBelow
wxLeftOf = miscc.wxLeftOf
wxRightOf = miscc.wxRightOf
wxSameAs = miscc.wxSameAs
wxAbsolute = miscc.wxAbsolute
wxOutRegion = miscc.wxOutRegion
wxPartRegion = miscc.wxPartRegion
wxInRegion = miscc.wxInRegion
