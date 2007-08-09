# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

import _activex
import new
new_instancemethod = new.instancemethod
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


def _swig_setattr_nondynamic_method(set):
    def set_attr(self,name,value):
        if (name == "thisown"): return self.this.own(value)
        if hasattr(self,name) or (name == "this"):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


import _core
wx = _core 
__docfilter__ = wx.__DocFilter(globals()) 
#---------------------------------------------------------------------------

class CLSID(object):
    """
    This class wraps the Windows CLSID structure and is used to
    specify the class of the ActiveX object that is to be created.  A
    CLSID can be constructed from either a ProgID string, (such as
    'WordPad.Document.1') or a classID string, (such as
    '{CA8A9783-280D-11CF-A24D-444553540000}').
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, String id) -> CLSID

        This class wraps the Windows CLSID structure and is used to
        specify the class of the ActiveX object that is to be created.  A
        CLSID can be constructed from either a ProgID string, (such as
        'WordPad.Document.1') or a classID string, (such as
        '{CA8A9783-280D-11CF-A24D-444553540000}').
        """
        _activex.CLSID_swiginit(self,_activex.new_CLSID(*args, **kwargs))
    __swig_destroy__ = _activex.delete_CLSID
    __del__ = lambda self : None;
    def GetCLSIDString(*args, **kwargs):
        """GetCLSIDString(self) -> String"""
        return _activex.CLSID_GetCLSIDString(*args, **kwargs)

    def GetProgIDString(*args, **kwargs):
        """GetProgIDString(self) -> String"""
        return _activex.CLSID_GetProgIDString(*args, **kwargs)

    def __str__(self):   return self.GetCLSIDString() 
_activex.CLSID_swigregister(CLSID)

#---------------------------------------------------------------------------

class ParamX(object):
    """Proxy of C++ ParamX class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    flags = property(_activex.ParamX_flags_get)
    isPtr = property(_activex.ParamX_isPtr_get)
    isSafeArray = property(_activex.ParamX_isSafeArray_get)
    isOptional = property(_activex.ParamX_isOptional_get)
    vt = property(_activex.ParamX_vt_get)
    name = property(_activex.ParamX_name_get)
    vt_type = property(_activex.ParamX_vt_type_get)

    isIn = property(_activex.ParamX_IsIn)

    isOut = property(_activex.ParamX_IsOut)

    isRetVal = property(_activex.ParamX_IsRetVal)

_activex.ParamX_swigregister(ParamX)

class FuncX(object):
    """Proxy of C++ FuncX class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    name = property(_activex.FuncX_name_get)
    memid = property(_activex.FuncX_memid_get)
    hasOut = property(_activex.FuncX_hasOut_get)
    retType = property(_activex.FuncX_retType_get)
    params = property(_activex.FuncX_params_get)
_activex.FuncX_swigregister(FuncX)

class PropX(object):
    """Proxy of C++ PropX class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    name = property(_activex.PropX_name_get)
    memid = property(_activex.PropX_memid_get)
    type = property(_activex.PropX_type_get)
    arg = property(_activex.PropX_arg_get)
    putByRef = property(_activex.PropX_putByRef_get)
    canGet = property(_activex.PropX_CanGet)

    canSet = property(_activex.PropX_CanSet)

_activex.PropX_swigregister(PropX)

class ParamXArray(object):
    """Proxy of C++ ParamXArray class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def __nonzero__(*args, **kwargs):
        """__nonzero__(self) -> bool"""
        return _activex.ParamXArray___nonzero__(*args, **kwargs)

    def __len__(*args, **kwargs):
        """__len__(self) -> int"""
        return _activex.ParamXArray___len__(*args, **kwargs)

    def __getitem__(*args, **kwargs):
        """__getitem__(self, int idx) -> ParamX"""
        return _activex.ParamXArray___getitem__(*args, **kwargs)

_activex.ParamXArray_swigregister(ParamXArray)

class FuncXArray(object):
    """Proxy of C++ FuncXArray class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def __nonzero__(*args, **kwargs):
        """__nonzero__(self) -> bool"""
        return _activex.FuncXArray___nonzero__(*args, **kwargs)

    def __len__(*args, **kwargs):
        """__len__(self) -> int"""
        return _activex.FuncXArray___len__(*args, **kwargs)

    def __getitem__(*args, **kwargs):
        """__getitem__(self, int idx) -> FuncX"""
        return _activex.FuncXArray___getitem__(*args, **kwargs)

_activex.FuncXArray_swigregister(FuncXArray)

class PropXArray(object):
    """Proxy of C++ PropXArray class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def __nonzero__(*args, **kwargs):
        """__nonzero__(self) -> bool"""
        return _activex.PropXArray___nonzero__(*args, **kwargs)

    def __len__(*args, **kwargs):
        """__len__(self) -> int"""
        return _activex.PropXArray___len__(*args, **kwargs)

    def __getitem__(*args, **kwargs):
        """__getitem__(self, int idx) -> PropX"""
        return _activex.PropXArray___getitem__(*args, **kwargs)

_activex.PropXArray_swigregister(PropXArray)

#---------------------------------------------------------------------------

class ActiveXWindow(_core.Window):
    """
    ActiveXWindow derives from wxWindow and the constructor accepts a
    CLSID for the ActiveX Control that should be created.  The
    ActiveXWindow class simply adds methods that allow you to query
    some of the TypeInfo exposed by the ActiveX object, and also to
    get/set properties or call methods by name.  The Python
    implementation automatically handles converting parameters and
    return values to/from the types expected by the ActiveX code as
    specified by the TypeInfo.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, CLSID clsId, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, 
            String name=PanelNameStr) -> ActiveXWindow

        Creates an ActiveX control from the clsID given and makes it act
        as much like a regular wx.Window as possible.
        """
        _activex.ActiveXWindow_swiginit(self,_activex.new_ActiveXWindow(*args, **kwargs))
        self._setOORInfo(self)

    def GetCLSID(*args, **kwargs):
        """
        GetCLSID(self) -> CLSID

        Return the CLSID used to construct this ActiveX window
        """
        return _activex.ActiveXWindow_GetCLSID(*args, **kwargs)

    def GetAXEventCount(*args, **kwargs):
        """
        GetAXEventCount(self) -> int

        Number of events defined for this control
        """
        return _activex.ActiveXWindow_GetAXEventCount(*args, **kwargs)

    def GetAXEventDesc(*args, **kwargs):
        """
        GetAXEventDesc(self, int idx) -> FuncX

        Returns event description by index
        """
        return _activex.ActiveXWindow_GetAXEventDesc(*args, **kwargs)

    def GetAXPropCount(*args, **kwargs):
        """
        GetAXPropCount(self) -> int

        Number of properties defined for this control
        """
        return _activex.ActiveXWindow_GetAXPropCount(*args, **kwargs)

    def GetAXPropDesc(*args):
        """
        GetAXPropDesc(self, int idx) -> PropX
        GetAXPropDesc(self, String name) -> PropX
        """
        return _activex.ActiveXWindow_GetAXPropDesc(*args)

    def GetAXMethodCount(*args, **kwargs):
        """
        GetAXMethodCount(self) -> int

        Number of methods defined for this control
        """
        return _activex.ActiveXWindow_GetAXMethodCount(*args, **kwargs)

    def GetAXMethodDesc(*args):
        """
        GetAXMethodDesc(self, int idx) -> FuncX
        GetAXMethodDesc(self, String name) -> FuncX
        """
        return _activex.ActiveXWindow_GetAXMethodDesc(*args)

    def GetAXEvents(*args, **kwargs):
        """
        GetAXEvents(self) -> FuncXArray

        Returns a sequence of FuncX objects describing the events
        available for this ActiveX object.
        """
        return _activex.ActiveXWindow_GetAXEvents(*args, **kwargs)

    def GetAXMethods(*args, **kwargs):
        """
        GetAXMethods(self) -> FuncXArray

        Returns a sequence of FuncX objects describing the methods
        available for this ActiveX object.
        """
        return _activex.ActiveXWindow_GetAXMethods(*args, **kwargs)

    def GetAXProperties(*args, **kwargs):
        """
        GetAXProperties(self) -> PropXArray

        Returns a sequence of PropX objects describing the properties
        available for this ActiveX object.
        """
        return _activex.ActiveXWindow_GetAXProperties(*args, **kwargs)

    def SetAXProp(*args, **kwargs):
        """
        SetAXProp(self, String name, PyObject value)

        Set a property of the ActiveX object by name.
        """
        return _activex.ActiveXWindow_SetAXProp(*args, **kwargs)

    def GetAXProp(*args, **kwargs):
        """
        GetAXProp(self, String name) -> PyObject

        Get the value of an ActiveX property by name.
        """
        return _activex.ActiveXWindow_GetAXProp(*args, **kwargs)

    def _CallAXMethod(*args):
        """
        _CallAXMethod(self, String name, PyObject args) -> PyObject

        The implementation for CallMethod.  Calls an ActiveX method, by
        name passing the parameters given in args.
        """
        return _activex.ActiveXWindow__CallAXMethod(*args)

    def CallAXMethod(self, name, *args):
        """
        Front-end for _CallMethod.  Simply passes all positional args
        after the name as a single tuple to _CallMethod.
        """
        return self._CallAXMethod(name, args)

_activex.ActiveXWindow_swigregister(ActiveXWindow)

#---------------------------------------------------------------------------


def RegisterActiveXEvent(*args, **kwargs):
  """
    RegisterActiveXEvent(String eventName) -> EventType

    Creates a standard wx event ID for the given eventName.
    """
  return _activex.RegisterActiveXEvent(*args, **kwargs)
class ActiveXEvent(_core.CommandEvent):
    """
    An instance of ActiveXEvent is sent to the handler for all bound
    ActiveX events.  Any event parameters from the ActiveX cntrol are
    turned into attributes of the Python proxy for this event object.
    Additionally, there is a property called eventName that will
    return (surprisingly <wink>) the name of the ActiveX event.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    eventName = property(_activex.ActiveXEvent_EventName)

    def _preCallInit(*args, **kwargs):
        """_preCallInit(self, PyObject pyself)"""
        return _activex.ActiveXEvent__preCallInit(*args, **kwargs)

    def _postCallCleanup(*args, **kwargs):
        """_postCallCleanup(self, PyObject pyself)"""
        return _activex.ActiveXEvent__postCallCleanup(*args, **kwargs)

_activex.ActiveXEvent_swigregister(ActiveXEvent)

#---------------------------------------------------------------------------

class IEHtmlWindowBase(ActiveXWindow):
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        _activex.IEHtmlWindowBase_swiginit(self,_activex.new_IEHtmlWindowBase(*args, **kwargs))
        self._setOORInfo(self)

    def SetCharset(*args, **kwargs): return _activex.IEHtmlWindowBase_SetCharset(*args, **kwargs)
    def LoadString(*args, **kwargs): return _activex.IEHtmlWindowBase_LoadString(*args, **kwargs)
    def LoadStream(*args, **kwargs): return _activex.IEHtmlWindowBase_LoadStream(*args, **kwargs)
    def GetStringSelection(*args, **kwargs): return _activex.IEHtmlWindowBase_GetStringSelection(*args, **kwargs)
    def GetText(*args, **kwargs): return _activex.IEHtmlWindowBase_GetText(*args, **kwargs)
_activex.IEHtmlWindowBase_swigregister(IEHtmlWindowBase)

#---------------------------------------------------------------------------
# Some helper and utility functions for ActiveX


t4 = " " * 4
t8 = " " * 8

def GetAXInfo(ax):
    """
    Returns a printable summary of the TypeInfo from the ActiveX instance
    passed in.
    """

    def ProcessFuncX(f, out, name):
        out.append(name)
        out.append(t4 + "retType:  %s" % f.retType.vt_type)
        if f.params:
            out.append(t4 + "params:")
            for p in f.params:
                out.append(t8 + p.name)
                out.append(t8+t4+ "in:%s  out:%s  optional:%s  type:%s" % (p.isIn, p.isOut, p.isOptional, p.vt_type))
        out.append('')

    def ProcessPropX(p, out):
        out.append(GernerateAXModule.trimPropName(p.name))
        out.append(t4+ "type:%s  arg:%s  canGet:%s  canSet:%s" % (p.type.vt_type, p.arg.vt_type, p.canGet, p.canSet))
        out.append('')

    out = []

    out.append("PROPERTIES")
    out.append("-"*20)
    for p in ax.GetAXProperties():
        ProcessPropX(p, out)
    out.append('\n\n')

    out.append("METHODS")
    out.append("-"*20)
    for m in ax.GetAXMethods():
        ProcessFuncX(m, out, GernerateAXModule.trimMethodName(m.name))
    out.append('\n\n')

    out.append("EVENTS")
    out.append("-"*20)
    for e in ax.GetAXEvents():
        ProcessFuncX(e, out, GernerateAXModule.trimEventName(e.name))
    out.append('\n\n')

    return "\n".join(out)



class GernerateAXModule:
    def __init__(self, ax, className, modulePath, moduleName=None, verbose=False):
        """
        Make a Python module file with a class that has been specialized
        for the AcitveX object.

            ax           An instance of the ActiveXWindow class
            className    The name to use for the new class
            modulePath   The path where the new module should be written to
            moduleName   The name of the .py file to create.  If not given
                         then the className will be used.
        """
        import os
        if moduleName is None:
            moduleName = className + '.py'
        filename = os.path.join(modulePath, moduleName)
        if verbose:
            print "Creating module in:", filename
            print "  ProgID:  ", ax.GetCLSID().GetProgIDString()
            print "  CLSID:   ", ax.GetCLSID().GetCLSIDString()
            print
        self.mf = file(filename, "w")
        self.WriteFileHeader(ax)
        self.WriteEvents(ax)
        self.WriteClassHeader(ax, className)
        self.WriteMethods(ax)
        self.WriteProperties(ax)
        self.WriteDocs(ax)
        self.mf.close()
        del self.mf


    def WriteFileHeader(self, ax):
        self.write("# This module was generated by the wx.activex.GernerateAXModule class\n"
                   "# (See also the genaxmodule script.)\n")
        self.write("import wx")
        self.write("import wx.activex\n")
        self.write("clsID = '%s'\nprogID = '%s'\n"
                   % (ax.GetCLSID().GetCLSIDString(), ax.GetCLSID().GetProgIDString()))
        self.write("\n")


    def WriteEvents(self, ax):
        events = ax.GetAXEvents()
        if events:
            self.write("# Create eventTypes and event binders")
            for e in events:
                self.write("wxEVT_%s = wx.activex.RegisterActiveXEvent('%s')"
                           % (self.trimEventName(e.name), e.name))
            self.write()
            for e in events:
                n = self.trimEventName(e.name)
                self.write("EVT_%s = wx.PyEventBinder(wxEVT_%s, 1)" % (n,n))
            self.write("\n")


    def WriteClassHeader(self, ax, className):
        self.write("# Derive a new class from ActiveXWindow")
        self.write("""\
class %s(wx.activex.ActiveXWindow):
    def __init__(self, parent, ID=-1, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=0, name='%s'):
        wx.activex.ActiveXWindow.__init__(self, parent,
            wx.activex.CLSID('%s'),
            ID, pos, size, style, name)
        """ % (className, className, ax.GetCLSID().GetCLSIDString()) )


    def WriteMethods(self, ax):
        methods = ax.GetAXMethods()
        if methods:
            self.write(t4, "# Methods exported by the ActiveX object")
            for m in methods:
                name = self.trimMethodName(m.name)
                self.write(t4, "def %s(self%s):" % (name, self.getParameters(m, True)))
                self.write(t8, "return self.CallAXMethod('%s'%s)" % (m.name, self.getParameters(m, False)))
                self.write()
            

    def WriteProperties(self, ax):
        props = ax.GetAXProperties()
        if props:
            self.write(t4, "# Getters, Setters and properties")
            for p in props:
                getterName = setterName = "None"
                if p.canGet:
                    getterName = "_get_" + p.name
                    self.write(t4, "def %s(self):" % getterName)
                    self.write(t8, "return self.GetAXProp('%s')" % p.name)
                if p.canSet:
                    setterName = "_set_" + p.name
                    self.write(t4, "def %s(self, %s):" % (setterName, p.arg.name))
                    self.write(t8, "self.SetAXProp('%s', %s)" % (p.name, p.arg.name))

                self.write(t4, "%s = property(%s, %s)" %
                           (self.trimPropName(p.name), getterName, setterName))
                self.write()
                

    def WriteDocs(self, ax):
        self.write()
        doc = GetAXInfo(ax)
        for line in doc.split('\n'):
            self.write("#  ", line)
            


    def write(self, *args):
        for a in args:
            self.mf.write(a)
        self.mf.write("\n")


    def trimEventName(name):
        if name.startswith("On"):
            name = name[2:]
        return name
    trimEventName = staticmethod(trimEventName)


    def trimPropName(name):
        #name = name[0].lower() + name[1:]
        name = name.lower()
        import keyword
        if name in keyword.kwlist: name += '_'
        return name
    trimPropName = staticmethod(trimPropName)


    def trimMethodName(name):
        import keyword
        if name in keyword.kwlist: name += '_'
        return name
    trimMethodName = staticmethod(trimMethodName)
    

    def getParameters(self, m, withDefaults):
        import keyword
        st = ""
        # collect the input parameters, if both isIn and isOut are
        # False then assume it is an input paramater
        params = []
        for p in m.params:
            if p.isIn or (not p.isIn and not p.isOut):
                params.append(p)
        # did we get any?
        for p in params:
            name = p.name
            if name in keyword.kwlist: name += '_'
            st += ", "
            st += name
            if withDefaults and p.isOptional:
                st += '=None'
        return st


#---------------------------------------------------------------------------



