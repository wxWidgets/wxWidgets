# This file was created automatically by SWIG.
import miscc
class wxObjectPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetClassName(self, *_args, **_kwargs):
        val = miscc.wxObject_GetClassName(self, *_args, **_kwargs)
        return val
    def Destroy(self, *_args, **_kwargs):
        val = miscc.wxObject_Destroy(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxObject instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxObject(wxObjectPtr):
    def __init__(self,this):
        self.this = this




class wxSizePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=miscc.delete_wxSize):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def Set(self, *_args, **_kwargs):
        val = miscc.wxSize_Set(self, *_args, **_kwargs)
        return val
    def GetX(self, *_args, **_kwargs):
        val = miscc.wxSize_GetX(self, *_args, **_kwargs)
        return val
    def GetY(self, *_args, **_kwargs):
        val = miscc.wxSize_GetY(self, *_args, **_kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = miscc.wxSize_GetWidth(self, *_args, **_kwargs)
        return val
    def GetHeight(self, *_args, **_kwargs):
        val = miscc.wxSize_GetHeight(self, *_args, **_kwargs)
        return val
    def SetWidth(self, *_args, **_kwargs):
        val = miscc.wxSize_SetWidth(self, *_args, **_kwargs)
        return val
    def SetHeight(self, *_args, **_kwargs):
        val = miscc.wxSize_SetHeight(self, *_args, **_kwargs)
        return val
    def asTuple(self, *_args, **_kwargs):
        val = miscc.wxSize_asTuple(self, *_args, **_kwargs)
        return val
    def __eq__(self, *_args, **_kwargs):
        val = miscc.wxSize___eq__(self, *_args, **_kwargs)
        return val
    def __ne__(self, *_args, **_kwargs):
        val = miscc.wxSize___ne__(self, *_args, **_kwargs)
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
        return "<%s.%s instance; proxy of C++ wxSize instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
    
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxSize'+str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.width = val
        elif index == 1: self.height = val
        else: raise IndexError
    def __nonzero__(self):               return self.asTuple() != (0,0)
    def __getinitargs__(self):           return ()
    def __getstate__(self):              return self.asTuple()
    def __setstate__(self, state):       self.Set(*state)

class wxSize(wxSizePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = miscc.new_wxSize(*_args,**_kwargs)
        self.thisown = 1




class wxRealPointPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=miscc.delete_wxRealPoint):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def Set(self, *_args, **_kwargs):
        val = miscc.wxRealPoint_Set(self, *_args, **_kwargs)
        return val
    def asTuple(self, *_args, **_kwargs):
        val = miscc.wxRealPoint_asTuple(self, *_args, **_kwargs)
        return val
    def __add__(self, *_args, **_kwargs):
        val = miscc.wxRealPoint___add__(self, *_args, **_kwargs)
        if val: val = wxRealPointPtr(val) ; val.thisown = 1
        return val
    def __sub__(self, *_args, **_kwargs):
        val = miscc.wxRealPoint___sub__(self, *_args, **_kwargs)
        if val: val = wxRealPointPtr(val) ; val.thisown = 1
        return val
    def __eq__(self, *_args, **_kwargs):
        val = miscc.wxRealPoint___eq__(self, *_args, **_kwargs)
        return val
    def __ne__(self, *_args, **_kwargs):
        val = miscc.wxRealPoint___ne__(self, *_args, **_kwargs)
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
        return "<%s.%s instance; proxy of C++ wxRealPoint instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
    
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxRealPoint'+str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.width = val
        elif index == 1: self.height = val
        else: raise IndexError
    def __nonzero__(self):      return self.asTuple() != (0.0, 0.0)
    def __getinitargs__(self):           return ()
    def __getstate__(self):              return self.asTuple()
    def __setstate__(self, state):       self.Set(*state)

class wxRealPoint(wxRealPointPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = miscc.new_wxRealPoint(*_args,**_kwargs)
        self.thisown = 1




class wxPointPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=miscc.delete_wxPoint):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def Set(self, *_args, **_kwargs):
        val = miscc.wxPoint_Set(self, *_args, **_kwargs)
        return val
    def asTuple(self, *_args, **_kwargs):
        val = miscc.wxPoint_asTuple(self, *_args, **_kwargs)
        return val
    def __add__(self, *_args, **_kwargs):
        val = miscc.wxPoint___add__(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def __sub__(self, *_args, **_kwargs):
        val = miscc.wxPoint___sub__(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def __eq__(self, *_args, **_kwargs):
        val = miscc.wxPoint___eq__(self, *_args, **_kwargs)
        return val
    def __ne__(self, *_args, **_kwargs):
        val = miscc.wxPoint___ne__(self, *_args, **_kwargs)
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
        return "<%s.%s instance; proxy of C++ wxPoint instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
    
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxPoint'+str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.x = val
        elif index == 1: self.y = val
        else: raise IndexError
    def __nonzero__(self):      return self.asTuple() != (0,0)
    def __getinitargs__(self):           return ()
    def __getstate__(self):              return self.asTuple()
    def __setstate__(self, state):       self.Set(*state)

class wxPoint(wxPointPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = miscc.new_wxPoint(*_args,**_kwargs)
        self.thisown = 1




class wxRectPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=miscc.delete_wxRect):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def GetX(self, *_args, **_kwargs):
        val = miscc.wxRect_GetX(self, *_args, **_kwargs)
        return val
    def SetX(self, *_args, **_kwargs):
        val = miscc.wxRect_SetX(self, *_args, **_kwargs)
        return val
    def GetY(self, *_args, **_kwargs):
        val = miscc.wxRect_GetY(self, *_args, **_kwargs)
        return val
    def SetY(self, *_args, **_kwargs):
        val = miscc.wxRect_SetY(self, *_args, **_kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = miscc.wxRect_GetWidth(self, *_args, **_kwargs)
        return val
    def SetWidth(self, *_args, **_kwargs):
        val = miscc.wxRect_SetWidth(self, *_args, **_kwargs)
        return val
    def GetHeight(self, *_args, **_kwargs):
        val = miscc.wxRect_GetHeight(self, *_args, **_kwargs)
        return val
    def SetHeight(self, *_args, **_kwargs):
        val = miscc.wxRect_SetHeight(self, *_args, **_kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = miscc.wxRect_GetPosition(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetSize(self, *_args, **_kwargs):
        val = miscc.wxRect_GetSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def SetPosition(self, *_args, **_kwargs):
        val = miscc.wxRect_SetPosition(self, *_args, **_kwargs)
        return val
    def SetSize(self, *_args, **_kwargs):
        val = miscc.wxRect_SetSize(self, *_args, **_kwargs)
        return val
    def GetLeft(self, *_args, **_kwargs):
        val = miscc.wxRect_GetLeft(self, *_args, **_kwargs)
        return val
    def GetTop(self, *_args, **_kwargs):
        val = miscc.wxRect_GetTop(self, *_args, **_kwargs)
        return val
    def GetBottom(self, *_args, **_kwargs):
        val = miscc.wxRect_GetBottom(self, *_args, **_kwargs)
        return val
    def GetRight(self, *_args, **_kwargs):
        val = miscc.wxRect_GetRight(self, *_args, **_kwargs)
        return val
    def SetLeft(self, *_args, **_kwargs):
        val = miscc.wxRect_SetLeft(self, *_args, **_kwargs)
        return val
    def SetRight(self, *_args, **_kwargs):
        val = miscc.wxRect_SetRight(self, *_args, **_kwargs)
        return val
    def SetTop(self, *_args, **_kwargs):
        val = miscc.wxRect_SetTop(self, *_args, **_kwargs)
        return val
    def SetBottom(self, *_args, **_kwargs):
        val = miscc.wxRect_SetBottom(self, *_args, **_kwargs)
        return val
    def Deflate(self, *_args, **_kwargs):
        val = miscc.wxRect_Deflate(self, *_args, **_kwargs)
        return val
    def Inflate(self, *_args, **_kwargs):
        val = miscc.wxRect_Inflate(self, *_args, **_kwargs)
        return val
    def InsideXY(self, *_args, **_kwargs):
        val = miscc.wxRect_InsideXY(self, *_args, **_kwargs)
        return val
    def Inside(self, *_args, **_kwargs):
        val = miscc.wxRect_Inside(self, *_args, **_kwargs)
        return val
    def Intersects(self, *_args, **_kwargs):
        val = miscc.wxRect_Intersects(self, *_args, **_kwargs)
        return val
    def OffsetXY(self, *_args, **_kwargs):
        val = miscc.wxRect_OffsetXY(self, *_args, **_kwargs)
        return val
    def Offset(self, *_args, **_kwargs):
        val = miscc.wxRect_Offset(self, *_args, **_kwargs)
        return val
    def Set(self, *_args, **_kwargs):
        val = miscc.wxRect_Set(self, *_args, **_kwargs)
        return val
    def asTuple(self, *_args, **_kwargs):
        val = miscc.wxRect_asTuple(self, *_args, **_kwargs)
        return val
    def __add__(self, *_args, **_kwargs):
        val = miscc.wxRect___add__(self, *_args, **_kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def __eq__(self, *_args, **_kwargs):
        val = miscc.wxRect___eq__(self, *_args, **_kwargs)
        return val
    def __ne__(self, *_args, **_kwargs):
        val = miscc.wxRect___ne__(self, *_args, **_kwargs)
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
        return "<%s.%s instance; proxy of C++ wxRect instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
    
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxRect'+str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.x = val
        elif index == 1: self.y = val
        elif index == 2: self.width = val
        elif index == 3: self.height = val
        else: raise IndexError
    def __nonzero__(self):               return self.asTuple() != (0,0,0,0)
    def __getinitargs__(self):           return ()
    def __getstate__(self):              return self.asTuple()
    def __setstate__(self, state):       self.Set(*state)

    # override the __getattr__ made by SWIG
    def __getattr__(self, name):
        d = {
            'x' : miscc.wxRect_x_get,
            'y' : miscc.wxRect_y_get,
            'width' : miscc.wxRect_width_get,
            'height' : miscc.wxRect_height_get,
            'top' : miscc.wxRect_GetTop,
            'bottom' : miscc.wxRect_GetBottom,
            'left' : miscc.wxRect_GetLeft,
            'right' : miscc.wxRect_GetRight,
            }
        try:
            func = d[name]
        except KeyError:
            raise AttributeError,name
        return func(self)

    # and also the __setattr__
    def __setattr__(self, name, value):
        d = {
            'x' : miscc.wxRect_x_set,
            'y' : miscc.wxRect_y_set,
            'width' : miscc.wxRect_width_set,
            'height' : miscc.wxRect_height_set,
            'top' : miscc.wxRect_SetTop,
            'bottom' : miscc.wxRect_SetBottom,
            'left' : miscc.wxRect_SetLeft,
            'right' : miscc.wxRect_SetRight,
            }
        try:
            func = d[name]
        except KeyError:
            self.__dict__[name] = value
            return
        func(self, value)
    
class wxRect(wxRectPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = miscc.new_wxRect(*_args,**_kwargs)
        self.thisown = 1




class wxPoint2DDoublePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetFloor(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble_GetFloor(self, *_args, **_kwargs)
        return val
    def GetRounded(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble_GetRounded(self, *_args, **_kwargs)
        return val
    def GetVectorLength(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble_GetVectorLength(self, *_args, **_kwargs)
        return val
    def GetVectorAngle(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble_GetVectorAngle(self, *_args, **_kwargs)
        return val
    def SetVectorLength(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble_SetVectorLength(self, *_args, **_kwargs)
        return val
    def SetVectorAngle(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble_SetVectorAngle(self, *_args, **_kwargs)
        return val
    def GetDistance(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble_GetDistance(self, *_args, **_kwargs)
        return val
    def GetDistanceSquare(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble_GetDistanceSquare(self, *_args, **_kwargs)
        return val
    def GetDotProduct(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble_GetDotProduct(self, *_args, **_kwargs)
        return val
    def GetCrossProduct(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble_GetCrossProduct(self, *_args, **_kwargs)
        return val
    def Set(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble_Set(self, *_args, **_kwargs)
        return val
    def __neg__(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble___neg__(self, *_args, **_kwargs)
        if val: val = wxPoint2DDoublePtr(val) ; val.thisown = 1
        return val
    def __iadd__(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble___iadd__(self, *_args, **_kwargs)
        if val: val = wxPoint2DDoublePtr(val) 
        return val
    def __isub__(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble___isub__(self, *_args, **_kwargs)
        if val: val = wxPoint2DDoublePtr(val) 
        return val
    def __imul__(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble___imul__(self, *_args, **_kwargs)
        if val: val = wxPoint2DDoublePtr(val) 
        return val
    def __idiv__(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble___idiv__(self, *_args, **_kwargs)
        if val: val = wxPoint2DDoublePtr(val) 
        return val
    def __eq__(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble___eq__(self, *_args, **_kwargs)
        return val
    def __ne__(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble___ne__(self, *_args, **_kwargs)
        return val
    def asTuple(self, *_args, **_kwargs):
        val = miscc.wxPoint2DDouble_asTuple(self, *_args, **_kwargs)
        return val
    def __setattr__(self,name,value):
        if name == "m_x" :
            miscc.wxPoint2DDouble_m_x_set(self,value)
            return
        if name == "m_y" :
            miscc.wxPoint2DDouble_m_y_set(self,value)
            return
        if name == "x" :
            miscc.wxPoint2DDouble_x_set(self,value)
            return
        if name == "y" :
            miscc.wxPoint2DDouble_y_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "m_x" : 
            return miscc.wxPoint2DDouble_m_x_get(self)
        if name == "m_y" : 
            return miscc.wxPoint2DDouble_m_y_get(self)
        if name == "x" : 
            return miscc.wxPoint2DDouble_x_get(self)
        if name == "y" : 
            return miscc.wxPoint2DDouble_y_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxPoint2DDouble instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
    
    def SetPolarCoordinates(self, angle, length):
        self.SetVectorLength(length)
        self.SetVectorAngle(angle)
    def Normalize(self):
        self.SetVectorLength(1.0)
    
    
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxPoint2DDouble'+str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.m_x = val
        elif index == 1: self.m_yt = val
        else: raise IndexError
    def __nonzero__(self):               return self.asTuple() != (0.0, 0.0)
    def __getinitargs__(self):           return ()
    def __getstate__(self):              return self.asTuple()
    def __setstate__(self, state):       self.Set(*state)

class wxPoint2DDouble(wxPoint2DDoublePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = miscc.new_wxPoint2DDouble(*_args,**_kwargs)
        self.thisown = 1



def wxPoint2DDoubleCopy(*_args,**_kwargs):
    val = wxPoint2DDoublePtr(miscc.new_wxPoint2DDoubleCopy(*_args,**_kwargs))
    val.thisown = 1
    return val

def wxPoint2DDoubleFromPoint(*_args,**_kwargs):
    val = wxPoint2DDoublePtr(miscc.new_wxPoint2DDoubleFromPoint(*_args,**_kwargs))
    val.thisown = 1
    return val


class wxIndividualLayoutConstraintPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Above(self, *_args, **_kwargs):
        val = miscc.wxIndividualLayoutConstraint_Above(self, *_args, **_kwargs)
        return val
    def Absolute(self, *_args, **_kwargs):
        val = miscc.wxIndividualLayoutConstraint_Absolute(self, *_args, **_kwargs)
        return val
    def AsIs(self, *_args, **_kwargs):
        val = miscc.wxIndividualLayoutConstraint_AsIs(self, *_args, **_kwargs)
        return val
    def Below(self, *_args, **_kwargs):
        val = miscc.wxIndividualLayoutConstraint_Below(self, *_args, **_kwargs)
        return val
    def Unconstrained(self, *_args, **_kwargs):
        val = miscc.wxIndividualLayoutConstraint_Unconstrained(self, *_args, **_kwargs)
        return val
    def LeftOf(self, *_args, **_kwargs):
        val = miscc.wxIndividualLayoutConstraint_LeftOf(self, *_args, **_kwargs)
        return val
    def PercentOf(self, *_args, **_kwargs):
        val = miscc.wxIndividualLayoutConstraint_PercentOf(self, *_args, **_kwargs)
        return val
    def RightOf(self, *_args, **_kwargs):
        val = miscc.wxIndividualLayoutConstraint_RightOf(self, *_args, **_kwargs)
        return val
    def SameAs(self, *_args, **_kwargs):
        val = miscc.wxIndividualLayoutConstraint_SameAs(self, *_args, **_kwargs)
        return val
    def Set(self, *_args, **_kwargs):
        val = miscc.wxIndividualLayoutConstraint_Set(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxIndividualLayoutConstraint instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxIndividualLayoutConstraint(wxIndividualLayoutConstraintPtr):
    def __init__(self,this):
        self.this = this




class wxLayoutConstraintsPtr(wxObjectPtr):
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
        return "<%s.%s instance; proxy of C++ wxLayoutConstraints instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxLayoutConstraints(wxLayoutConstraintsPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = miscc.new_wxLayoutConstraints(*_args,**_kwargs)
        self.thisown = 1




class wxAcceleratorEntryPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=miscc.delete_wxAcceleratorEntry):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def Set(self, *_args, **_kwargs):
        val = miscc.wxAcceleratorEntry_Set(self, *_args, **_kwargs)
        return val
    def GetFlags(self, *_args, **_kwargs):
        val = miscc.wxAcceleratorEntry_GetFlags(self, *_args, **_kwargs)
        return val
    def GetKeyCode(self, *_args, **_kwargs):
        val = miscc.wxAcceleratorEntry_GetKeyCode(self, *_args, **_kwargs)
        return val
    def GetCommand(self, *_args, **_kwargs):
        val = miscc.wxAcceleratorEntry_GetCommand(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxAcceleratorEntry instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxAcceleratorEntry(wxAcceleratorEntryPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = miscc.new_wxAcceleratorEntry(*_args,**_kwargs)
        self.thisown = 1




class wxAcceleratorTablePtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=miscc.delete_wxAcceleratorTable):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxAcceleratorTable instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxAcceleratorTable(wxAcceleratorTablePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = miscc.new_wxAcceleratorTable(*_args,**_kwargs)
        self.thisown = 1




class wxBusyInfoPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=miscc.delete_wxBusyInfo):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxBusyInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxBusyInfo(wxBusyInfoPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = miscc.new_wxBusyInfo(*_args,**_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

wxIntersectRect = miscc.wxIntersectRect

wxNewId = miscc.wxNewId

wxRegisterId = miscc.wxRegisterId

wxGetCurrentId = miscc.wxGetCurrentId

wxBell = miscc.wxBell

wxEndBusyCursor = miscc.wxEndBusyCursor

wxGetElapsedTime = miscc.wxGetElapsedTime

wxGetFreeMemory = miscc.wxGetFreeMemory

wxGetMousePosition = miscc.wxGetMousePosition

wxIsBusy = miscc.wxIsBusy

wxNow = miscc.wxNow

wxShell = miscc.wxShell

wxStartTimer = miscc.wxStartTimer

wxGetOsVersion = miscc.wxGetOsVersion

wxGetOsDescription = miscc.wxGetOsDescription

wxShutdown = miscc.wxShutdown

wxSleep = miscc.wxSleep

wxUsleep = miscc.wxUsleep

wxYield = miscc.wxYield

wxYieldIfNeeded = miscc.wxYieldIfNeeded

wxEnableTopLevelWindows = miscc.wxEnableTopLevelWindows

wxStripMenuCodes = miscc.wxStripMenuCodes

wxGetEmailAddress = miscc.wxGetEmailAddress

wxGetHostName = miscc.wxGetHostName

wxGetFullHostName = miscc.wxGetFullHostName

wxGetUserId = miscc.wxGetUserId

wxGetUserName = miscc.wxGetUserName

wxGetHomeDir = miscc.wxGetHomeDir

wxGetUserHome = miscc.wxGetUserHome

wxGetProcessId = miscc.wxGetProcessId

def wxGetAccelFromString(*_args, **_kwargs):
    val = miscc.wxGetAccelFromString(*_args,**_kwargs)
    if val: val = wxAcceleratorEntryPtr(val)
    return val



#-------------- VARIABLE WRAPPERS ------------------

wxSHUTDOWN_POWEROFF = miscc.wxSHUTDOWN_POWEROFF
wxSHUTDOWN_REBOOT = miscc.wxSHUTDOWN_REBOOT
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
cvar = miscc.cvar
wxNullAcceleratorTable = wxAcceleratorTablePtr(miscc.cvar.wxNullAcceleratorTable)
