# This file was created automatically by SWIG.
import stattoolc

from misc import *

from windows import *

from gdi import *

from fonts import *

from clip_dnd import *

from controls import *

from events import *
import wx
wxITEM_NORMAL = 0 # predeclare this since wx isn't fully imported yet
class wxStatusBarPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = stattoolc.wxStatusBar_Create(self, *_args, **_kwargs)
        return val
    def GetFieldRect(self, *_args, **_kwargs):
        val = stattoolc.wxStatusBar_GetFieldRect(self, *_args, **_kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def GetFieldsCount(self, *_args, **_kwargs):
        val = stattoolc.wxStatusBar_GetFieldsCount(self, *_args, **_kwargs)
        return val
    def GetStatusText(self, *_args, **_kwargs):
        val = stattoolc.wxStatusBar_GetStatusText(self, *_args, **_kwargs)
        return val
    def GetBorderX(self, *_args, **_kwargs):
        val = stattoolc.wxStatusBar_GetBorderX(self, *_args, **_kwargs)
        return val
    def GetBorderY(self, *_args, **_kwargs):
        val = stattoolc.wxStatusBar_GetBorderY(self, *_args, **_kwargs)
        return val
    def SetFieldsCount(self, *_args, **_kwargs):
        val = stattoolc.wxStatusBar_SetFieldsCount(self, *_args, **_kwargs)
        return val
    def SetStatusText(self, *_args, **_kwargs):
        val = stattoolc.wxStatusBar_SetStatusText(self, *_args, **_kwargs)
        return val
    def SetStatusWidths(self, *_args, **_kwargs):
        val = stattoolc.wxStatusBar_SetStatusWidths(self, *_args, **_kwargs)
        return val
    def PushStatusText(self, *_args, **_kwargs):
        val = stattoolc.wxStatusBar_PushStatusText(self, *_args, **_kwargs)
        return val
    def PopStatusText(self, *_args, **_kwargs):
        val = stattoolc.wxStatusBar_PopStatusText(self, *_args, **_kwargs)
        return val
    def SetMinHeight(self, *_args, **_kwargs):
        val = stattoolc.wxStatusBar_SetMinHeight(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxStatusBar instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxStatusBar(wxStatusBarPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = stattoolc.new_wxStatusBar(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreStatusBar(*_args,**_kwargs):
    val = wxStatusBarPtr(stattoolc.new_wxPreStatusBar(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxToolBarToolBasePtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Destroy(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_Destroy(self, *_args, **_kwargs)
        return val
    def GetId(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_GetId(self, *_args, **_kwargs)
        return val
    def GetControl(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_GetControl(self, *_args, **_kwargs)
        return val
    def GetToolBar(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_GetToolBar(self, *_args, **_kwargs)
        return val
    def IsButton(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_IsButton(self, *_args, **_kwargs)
        return val
    def IsControl(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_IsControl(self, *_args, **_kwargs)
        return val
    def IsSeparator(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_IsSeparator(self, *_args, **_kwargs)
        return val
    def GetStyle(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_GetStyle(self, *_args, **_kwargs)
        return val
    def GetKind(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_GetKind(self, *_args, **_kwargs)
        return val
    def IsEnabled(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_IsEnabled(self, *_args, **_kwargs)
        return val
    def IsToggled(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_IsToggled(self, *_args, **_kwargs)
        return val
    def CanBeToggled(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_CanBeToggled(self, *_args, **_kwargs)
        return val
    def GetNormalBitmap(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_GetNormalBitmap(self, *_args, **_kwargs)
        if val: val = wxBitmapPtr(val) 
        return val
    def GetDisabledBitmap(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_GetDisabledBitmap(self, *_args, **_kwargs)
        if val: val = wxBitmapPtr(val) 
        return val
    def GetBitmap(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_GetBitmap(self, *_args, **_kwargs)
        if val: val = wxBitmapPtr(val) ; val.thisown = 1
        return val
    def GetLabel(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_GetLabel(self, *_args, **_kwargs)
        return val
    def GetShortHelp(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_GetShortHelp(self, *_args, **_kwargs)
        return val
    def GetLongHelp(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_GetLongHelp(self, *_args, **_kwargs)
        return val
    def Enable(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_Enable(self, *_args, **_kwargs)
        return val
    def Toggle(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_Toggle(self, *_args, **_kwargs)
        return val
    def SetToggle(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_SetToggle(self, *_args, **_kwargs)
        return val
    def SetShortHelp(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_SetShortHelp(self, *_args, **_kwargs)
        return val
    def SetLongHelp(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_SetLongHelp(self, *_args, **_kwargs)
        return val
    def SetNormalBitmap(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_SetNormalBitmap(self, *_args, **_kwargs)
        return val
    def SetDisabledBitmap(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_SetDisabledBitmap(self, *_args, **_kwargs)
        return val
    def SetLabel(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_SetLabel(self, *_args, **_kwargs)
        return val
    def Detach(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_Detach(self, *_args, **_kwargs)
        return val
    def Attach(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_Attach(self, *_args, **_kwargs)
        return val
    def GetClientData(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_GetClientData(self, *_args, **_kwargs)
        return val
    def SetClientData(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarToolBase_SetClientData(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxToolBarToolBase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
    
    GetBitmap1 = GetNormalBitmap
    GetBitmap2 = GetDisabledBitmap
    SetBitmap1 = SetNormalBitmap
    SetBitmap2 = SetDisabledBitmap
    
class wxToolBarToolBase(wxToolBarToolBasePtr):
    def __init__(self,this):
        self.this = this




class wxToolBarBasePtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def DoAddTool(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_DoAddTool(self, *_args, **_kwargs)
        return val
    def DoInsertTool(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_DoInsertTool(self, *_args, **_kwargs)
        return val
    def AddControl(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_AddControl(self, *_args, **_kwargs)
        return val
    def InsertControl(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_InsertControl(self, *_args, **_kwargs)
        return val
    def FindControl(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_FindControl(self, *_args, **_kwargs)
        return val
    def AddSeparator(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_AddSeparator(self, *_args, **_kwargs)
        return val
    def InsertSeparator(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_InsertSeparator(self, *_args, **_kwargs)
        return val
    def RemoveTool(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_RemoveTool(self, *_args, **_kwargs)
        return val
    def DeleteToolByPos(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_DeleteToolByPos(self, *_args, **_kwargs)
        return val
    def DeleteTool(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_DeleteTool(self, *_args, **_kwargs)
        return val
    def ClearTools(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_ClearTools(self, *_args, **_kwargs)
        return val
    def Realize(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_Realize(self, *_args, **_kwargs)
        return val
    def EnableTool(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_EnableTool(self, *_args, **_kwargs)
        return val
    def ToggleTool(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_ToggleTool(self, *_args, **_kwargs)
        return val
    def SetToggle(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_SetToggle(self, *_args, **_kwargs)
        return val
    def GetToolClientData(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_GetToolClientData(self, *_args, **_kwargs)
        return val
    def SetToolClientData(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_SetToolClientData(self, *_args, **_kwargs)
        return val
    def GetToolPos(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_GetToolPos(self, *_args, **_kwargs)
        return val
    def GetToolState(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_GetToolState(self, *_args, **_kwargs)
        return val
    def GetToolEnabled(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_GetToolEnabled(self, *_args, **_kwargs)
        return val
    def SetToolShortHelp(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_SetToolShortHelp(self, *_args, **_kwargs)
        return val
    def GetToolShortHelp(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_GetToolShortHelp(self, *_args, **_kwargs)
        return val
    def SetToolLongHelp(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_SetToolLongHelp(self, *_args, **_kwargs)
        return val
    def GetToolLongHelp(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_GetToolLongHelp(self, *_args, **_kwargs)
        return val
    def SetMarginsXY(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_SetMarginsXY(self, *_args, **_kwargs)
        return val
    def SetMargins(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_SetMargins(self, *_args, **_kwargs)
        return val
    def SetToolPacking(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_SetToolPacking(self, *_args, **_kwargs)
        return val
    def SetToolSeparation(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_SetToolSeparation(self, *_args, **_kwargs)
        return val
    def GetToolMargins(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_GetToolMargins(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetMargins(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_GetMargins(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetToolPacking(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_GetToolPacking(self, *_args, **_kwargs)
        return val
    def GetToolSeparation(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_GetToolSeparation(self, *_args, **_kwargs)
        return val
    def SetRows(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_SetRows(self, *_args, **_kwargs)
        return val
    def SetMaxRowsCols(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_SetMaxRowsCols(self, *_args, **_kwargs)
        return val
    def GetMaxRows(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_GetMaxRows(self, *_args, **_kwargs)
        return val
    def GetMaxCols(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_GetMaxCols(self, *_args, **_kwargs)
        return val
    def SetToolBitmapSize(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_SetToolBitmapSize(self, *_args, **_kwargs)
        return val
    def GetToolBitmapSize(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_GetToolBitmapSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetToolSize(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_GetToolSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def FindToolForPosition(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_FindToolForPosition(self, *_args, **_kwargs)
        return val
    def IsVertical(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarBase_IsVertical(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxToolBarBase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
    
    # These match the original Add methods for this class, kept for
    # backwards compatibility with versions < 2.3.3.


    def AddTool(self, id, bitmap,
                pushedBitmap = wxNullBitmap,
                isToggle = 0,
                clientData = None,
                shortHelpString = '',
                longHelpString = '') :
        '''Old style method to add a tool to the toolbar.'''
        kind = wx.wxITEM_NORMAL
        if isToggle: kind = wx.wxITEM_CHECK
        return self.DoAddTool(id, '', bitmap, pushedBitmap, kind,
                              shortHelpString, longHelpString, clientData)

    def AddSimpleTool(self, id, bitmap,
                      shortHelpString = '',
                      longHelpString = '',
                      isToggle = 0):
        '''Old style method to add a tool to the toolbar.'''
        kind = wx.wxITEM_NORMAL
        if isToggle: kind = wx.wxITEM_CHECK
        return self.DoAddTool(id, '', bitmap, wxNullBitmap, kind,
                              shortHelpString, longHelpString, None)

    def InsertTool(self, pos, id, bitmap,
                   pushedBitmap = wxNullBitmap,
                   isToggle = 0,
                   clientData = None,
                   shortHelpString = '',
                   longHelpString = ''):
        '''Old style method to insert a tool in the toolbar.'''
        kind = wx.wxITEM_NORMAL
        if isToggle: kind = wx.wxITEM_CHECK
        return self.DoInsertTool(pos, id, '', bitmap, pushedBitmap, kind,
                                 shortHelpString, longHelpString, clientData)

    def InsertSimpleTool(self, pos, id, bitmap,
                         shortHelpString = '',
                         longHelpString = '',
                         isToggle = 0):
        '''Old style method to insert a tool in the toolbar.'''
        kind = wx.wxITEM_NORMAL
        if isToggle: kind = wx.wxITEM_CHECK
        return self.DoInsertTool(pos, id, '', bitmap, wxNullBitmap, kind,
                                 shortHelpString, longHelpString, None)


    # The following are the new toolbar Add methods starting with
    # 2.3.3.  They are renamed to have 'Label' in the name so as to be
    # able to keep backwards compatibility with using the above
    # methods.  Eventually these should migrate to be the methods used
    # primarily and lose the 'Label' in the name...

    def AddLabelTool(self, id, label, bitmap,
                     bmpDisabled = wxNullBitmap,
                     kind = wxITEM_NORMAL,
                     shortHelp = '', longHelp = '',
                     clientData = None):
        '''
        The full AddTool() function.

        If bmpDisabled is wxNullBitmap, a shadowed version of the normal bitmap
        is created and used as the disabled image.
        '''
        return self.DoAddTool(id, label, bitmap, bmpDisabled, kind,
                              shortHelp, longHelp, clientData)


    def InsertLabelTool(self, pos, id, label, bitmap,
                        bmpDisabled = wxNullBitmap,
                        kind = wxITEM_NORMAL,
                        shortHelp = '', longHelp = '',
                        clientData = None):
        '''
        Insert the new tool at the given position, if pos == GetToolsCount(), it
        is equivalent to AddTool()
        '''
        return self.DoInsertTool(pos, id, label, bitmap, bmpDisabled, kind,
                                 shortHelp, longHelp, clientData)

    def AddCheckLabelTool(self, id, label, bitmap,
                        bmpDisabled = wxNullBitmap,
                        shortHelp = '', longHelp = '',
                        clientData = None):
        '''Add a check tool, i.e. a tool which can be toggled'''
        return self.DoAddTool(id, label, bitmap, bmpDisabled, wx.wxITEM_CHECK,
                              shortHelp, longHelp, clientData)

    def AddRadioLabelTool(self, id, label, bitmap,
                          bmpDisabled = wxNullBitmap,
                          shortHelp = '', longHelp = '',
                          clientData = None):
        '''
        Add a radio tool, i.e. a tool which can be toggled and releases any
        other toggled radio tools in the same group when it happens
        '''
        return self.DoAddTool(id, label, bitmap, bmpDisabled, wx.wxITEM_RADIO,
                              shortHelp, longHelp, clientData)


    # For consistency with the backwards compatible methods above, here are
    # some non-'Label' versions of the Check and Radio methods
    def AddCheckTool(self, id, bitmap,
                     bmpDisabled = wxNullBitmap,
                     shortHelp = '', longHelp = '',
                     clientData = None):
        '''Add a check tool, i.e. a tool which can be toggled'''
        return self.DoAddTool(id, '', bitmap, bmpDisabled, wx.wxITEM_CHECK,
                              shortHelp, longHelp, clientData)

    def AddRadioTool(self, id, bitmap,
                     bmpDisabled = wxNullBitmap,
                     shortHelp = '', longHelp = '',
                     clientData = None):
        '''
        Add a radio tool, i.e. a tool which can be toggled and releases any
        other toggled radio tools in the same group when it happens
        '''
        return self.DoAddTool(id, '', bitmap, bmpDisabled, wx.wxITEM_RADIO,
                              shortHelp, longHelp, clientData)
    
class wxToolBarBase(wxToolBarBasePtr):
    def __init__(self,this):
        self.this = this




class wxToolBarPtr(wxToolBarBasePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = stattoolc.wxToolBar_Create(self, *_args, **_kwargs)
        return val
    def FindToolForPosition(self, *_args, **_kwargs):
        val = stattoolc.wxToolBar_FindToolForPosition(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxToolBar instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxToolBar(wxToolBarPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = stattoolc.new_wxToolBar(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreToolBar(*_args,**_kwargs):
    val = wxToolBarPtr(stattoolc.new_wxPreToolBar(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxToolBarSimplePtr(wxToolBarBasePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarSimple_Create(self, *_args, **_kwargs)
        return val
    def FindToolForPosition(self, *_args, **_kwargs):
        val = stattoolc.wxToolBarSimple_FindToolForPosition(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxToolBarSimple instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxToolBarSimple(wxToolBarSimplePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = stattoolc.new_wxToolBarSimple(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreToolBarSimple(*_args,**_kwargs):
    val = wxToolBarSimplePtr(stattoolc.new_wxPreToolBarSimple(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val




#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxTOOL_STYLE_BUTTON = stattoolc.wxTOOL_STYLE_BUTTON
wxTOOL_STYLE_SEPARATOR = stattoolc.wxTOOL_STYLE_SEPARATOR
wxTOOL_STYLE_CONTROL = stattoolc.wxTOOL_STYLE_CONTROL
wxTB_HORIZONTAL = stattoolc.wxTB_HORIZONTAL
wxTB_VERTICAL = stattoolc.wxTB_VERTICAL
wxTB_3DBUTTONS = stattoolc.wxTB_3DBUTTONS
wxTB_FLAT = stattoolc.wxTB_FLAT
wxTB_DOCKABLE = stattoolc.wxTB_DOCKABLE
wxTB_NOICONS = stattoolc.wxTB_NOICONS
wxTB_TEXT = stattoolc.wxTB_TEXT
wxTB_NODIVIDER = stattoolc.wxTB_NODIVIDER
wxTB_NOALIGN = stattoolc.wxTB_NOALIGN
wxTB_HORZ_LAYOUT = stattoolc.wxTB_HORZ_LAYOUT
wxTB_HORZ_TEXT = stattoolc.wxTB_HORZ_TEXT
