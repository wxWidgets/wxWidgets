# This file was created automatically by SWIG.
import misc2c

from windows import *

from misc import *

from gdi import *
class wxToolTipPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetTip(self,arg0):
        val = misc2c.wxToolTip_SetTip(self.this,arg0)
        return val
    def GetTip(self):
        val = misc2c.wxToolTip_GetTip(self.this)
        return val
    def GetWindow(self):
        val = misc2c.wxToolTip_GetWindow(self.this)
        val = wxWindowPtr(val)
        return val
    def __repr__(self):
        return "<C wxToolTip instance>"
class wxToolTip(wxToolTipPtr):
    def __init__(self,arg0) :
        self.this = misc2c.new_wxToolTip(arg0)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

def wxFindWindowByLabel(arg0,*args):
    argl = map(None,args)
    try: argl[0] = argl[0].this
    except: pass
    args = tuple(argl)
    val = apply(misc2c.wxFindWindowByLabel,(arg0,)+args)
    val = wxWindowPtr(val)
    return val

def wxFindWindowByName(arg0,*args):
    argl = map(None,args)
    try: argl[0] = argl[0].this
    except: pass
    args = tuple(argl)
    val = apply(misc2c.wxFindWindowByName,(arg0,)+args)
    val = wxWindowPtr(val)
    return val

wxToolTip_Enable = misc2c.wxToolTip_Enable

wxToolTip_SetDelay = misc2c.wxToolTip_SetDelay



#-------------- VARIABLE WRAPPERS ------------------

