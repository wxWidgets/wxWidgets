# This file was created automatically by SWIG.
import wxc

from misc import *

from misc2 import *

from windows import *

from gdi import *

from events import *

from mdi import *

from frames import *

from stattool import *

from controls import *

from controls2 import *

from windows2 import *

from cmndlgs import *

from windows3 import *

from image import *

from printfw import *

from sizers import *
class wxPyAppPtr(wxEvtHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,wxc=wxc):
        if self.thisown == 1 :
            wxc.delete_wxPyApp(self)
    def GetAppName(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_GetAppName,(self,) + _args, _kwargs)
        return val
    def GetClassName(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_GetClassName,(self,) + _args, _kwargs)
        return val
    def GetExitOnFrameDelete(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_GetExitOnFrameDelete,(self,) + _args, _kwargs)
        return val
    def GetPrintMode(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_GetPrintMode,(self,) + _args, _kwargs)
        return val
    def GetTopWindow(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_GetTopWindow,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def GetVendorName(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_GetVendorName,(self,) + _args, _kwargs)
        return val
    def Dispatch(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_Dispatch,(self,) + _args, _kwargs)
        return val
    def ExitMainLoop(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_ExitMainLoop,(self,) + _args, _kwargs)
        return val
    def Initialized(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_Initialized,(self,) + _args, _kwargs)
        return val
    def MainLoop(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_MainLoop,(self,) + _args, _kwargs)
        return val
    def Pending(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_Pending,(self,) + _args, _kwargs)
        return val
    def ProcessIdle(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_ProcessIdle,(self,) + _args, _kwargs)
        return val
    def SetAppName(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_SetAppName,(self,) + _args, _kwargs)
        return val
    def SetClassName(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_SetClassName,(self,) + _args, _kwargs)
        return val
    def SetExitOnFrameDelete(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_SetExitOnFrameDelete,(self,) + _args, _kwargs)
        return val
    def SetPrintMode(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_SetPrintMode,(self,) + _args, _kwargs)
        return val
    def SetTopWindow(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_SetTopWindow,(self,) + _args, _kwargs)
        return val
    def SetVendorName(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_SetVendorName,(self,) + _args, _kwargs)
        return val
    def GetStdIcon(self, *_args, **_kwargs):
        val = apply(wxc.wxPyApp_GetStdIcon,(self,) + _args, _kwargs)
        if val: val = wxIconPtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<C wxPyApp instance at %s>" % (self.this,)
class wxPyApp(wxPyAppPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(wxc.new_wxPyApp,_args,_kwargs)
        self.thisown = 1




class __wxPyCleanupPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,wxc=wxc):
        if self.thisown == 1 :
            wxc.delete___wxPyCleanup(self)
    def __repr__(self):
        return "<C __wxPyCleanup instance at %s>" % (self.this,)
class __wxPyCleanup(__wxPyCleanupPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(wxc.new___wxPyCleanup,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

ptrcast = wxc.ptrcast

ptrvalue = wxc.ptrvalue

ptrset = wxc.ptrset

ptrcreate = wxc.ptrcreate

ptrfree = wxc.ptrfree

ptradd = wxc.ptradd

ptrmap = wxc.ptrmap

_wxStart = wxc._wxStart

_wxSetDictionary = wxc._wxSetDictionary



#-------------- VARIABLE WRAPPERS ------------------

wxMAJOR_VERSION = wxc.wxMAJOR_VERSION
wxMINOR_VERSION = wxc.wxMINOR_VERSION
wxRELEASE_NUMBER = wxc.wxRELEASE_NUMBER
wxNOT_FOUND = wxc.wxNOT_FOUND
wxVSCROLL = wxc.wxVSCROLL
wxHSCROLL = wxc.wxHSCROLL
wxCAPTION = wxc.wxCAPTION
wxDOUBLE_BORDER = wxc.wxDOUBLE_BORDER
wxSUNKEN_BORDER = wxc.wxSUNKEN_BORDER
wxRAISED_BORDER = wxc.wxRAISED_BORDER
wxBORDER = wxc.wxBORDER
wxSIMPLE_BORDER = wxc.wxSIMPLE_BORDER
wxSTATIC_BORDER = wxc.wxSTATIC_BORDER
wxTRANSPARENT_WINDOW = wxc.wxTRANSPARENT_WINDOW
wxNO_BORDER = wxc.wxNO_BORDER
wxUSER_COLOURS = wxc.wxUSER_COLOURS
wxNO_3D = wxc.wxNO_3D
wxTAB_TRAVERSAL = wxc.wxTAB_TRAVERSAL
wxWANTS_CHARS = wxc.wxWANTS_CHARS
wxHORIZONTAL = wxc.wxHORIZONTAL
wxVERTICAL = wxc.wxVERTICAL
wxBOTH = wxc.wxBOTH
wxCENTER_FRAME = wxc.wxCENTER_FRAME
wxCENTRE_ON_SCREEN = wxc.wxCENTRE_ON_SCREEN
wxCENTER_ON_SCREEN = wxc.wxCENTER_ON_SCREEN
wxSTAY_ON_TOP = wxc.wxSTAY_ON_TOP
wxICONIZE = wxc.wxICONIZE
wxMINIMIZE = wxc.wxMINIMIZE
wxMAXIMIZE = wxc.wxMAXIMIZE
wxTHICK_FRAME = wxc.wxTHICK_FRAME
wxSYSTEM_MENU = wxc.wxSYSTEM_MENU
wxMINIMIZE_BOX = wxc.wxMINIMIZE_BOX
wxMAXIMIZE_BOX = wxc.wxMAXIMIZE_BOX
wxTINY_CAPTION_HORIZ = wxc.wxTINY_CAPTION_HORIZ
wxTINY_CAPTION_VERT = wxc.wxTINY_CAPTION_VERT
wxRESIZE_BOX = wxc.wxRESIZE_BOX
wxRESIZE_BORDER = wxc.wxRESIZE_BORDER
wxDIALOG_MODAL = wxc.wxDIALOG_MODAL
wxDIALOG_MODELESS = wxc.wxDIALOG_MODELESS
wxDEFAULT_FRAME_STYLE = wxc.wxDEFAULT_FRAME_STYLE
wxDEFAULT_DIALOG_STYLE = wxc.wxDEFAULT_DIALOG_STYLE
wxFRAME_TOOL_WINDOW = wxc.wxFRAME_TOOL_WINDOW
wxFRAME_FLOAT_ON_PARENT = wxc.wxFRAME_FLOAT_ON_PARENT
wxED_CLIENT_MARGIN = wxc.wxED_CLIENT_MARGIN
wxED_BUTTONS_BOTTOM = wxc.wxED_BUTTONS_BOTTOM
wxED_BUTTONS_RIGHT = wxc.wxED_BUTTONS_RIGHT
wxED_STATIC_LINE = wxc.wxED_STATIC_LINE
wxEXT_DIALOG_STYLE = wxc.wxEXT_DIALOG_STYLE
wxCLIP_CHILDREN = wxc.wxCLIP_CHILDREN
wxRETAINED = wxc.wxRETAINED
wxBACKINGSTORE = wxc.wxBACKINGSTORE
wxTB_3DBUTTONS = wxc.wxTB_3DBUTTONS
wxTB_HORIZONTAL = wxc.wxTB_HORIZONTAL
wxTB_VERTICAL = wxc.wxTB_VERTICAL
wxTB_FLAT = wxc.wxTB_FLAT
wxTB_DOCKABLE = wxc.wxTB_DOCKABLE
wxCOLOURED = wxc.wxCOLOURED
wxFIXED_LENGTH = wxc.wxFIXED_LENGTH
wxALIGN_LEFT = wxc.wxALIGN_LEFT
wxALIGN_CENTER = wxc.wxALIGN_CENTER
wxALIGN_CENTRE = wxc.wxALIGN_CENTRE
wxALIGN_RIGHT = wxc.wxALIGN_RIGHT
wxALIGN_BOTTOM = wxc.wxALIGN_BOTTOM
wxALIGN_TOP = wxc.wxALIGN_TOP
wxLB_NEEDED_SB = wxc.wxLB_NEEDED_SB
wxLB_ALWAYS_SB = wxc.wxLB_ALWAYS_SB
wxLB_SORT = wxc.wxLB_SORT
wxLB_SINGLE = wxc.wxLB_SINGLE
wxLB_MULTIPLE = wxc.wxLB_MULTIPLE
wxLB_EXTENDED = wxc.wxLB_EXTENDED
wxLB_OWNERDRAW = wxc.wxLB_OWNERDRAW
wxLB_HSCROLL = wxc.wxLB_HSCROLL
wxPROCESS_ENTER = wxc.wxPROCESS_ENTER
wxPASSWORD = wxc.wxPASSWORD
wxTE_PROCESS_ENTER = wxc.wxTE_PROCESS_ENTER
wxTE_PASSWORD = wxc.wxTE_PASSWORD
wxTE_READONLY = wxc.wxTE_READONLY
wxTE_RICH = wxc.wxTE_RICH
wxTE_MULTILINE = wxc.wxTE_MULTILINE
wxTE_AUTO_SCROLL = wxc.wxTE_AUTO_SCROLL
wxTE_NO_VSCROLL = wxc.wxTE_NO_VSCROLL
wxCB_SIMPLE = wxc.wxCB_SIMPLE
wxCB_DROPDOWN = wxc.wxCB_DROPDOWN
wxCB_SORT = wxc.wxCB_SORT
wxCB_READONLY = wxc.wxCB_READONLY
wxRA_HORIZONTAL = wxc.wxRA_HORIZONTAL
wxRA_VERTICAL = wxc.wxRA_VERTICAL
wxRA_SPECIFY_ROWS = wxc.wxRA_SPECIFY_ROWS
wxRA_SPECIFY_COLS = wxc.wxRA_SPECIFY_COLS
wxRB_GROUP = wxc.wxRB_GROUP
wxGA_PROGRESSBAR = wxc.wxGA_PROGRESSBAR
wxGA_HORIZONTAL = wxc.wxGA_HORIZONTAL
wxGA_VERTICAL = wxc.wxGA_VERTICAL
wxGA_SMOOTH = wxc.wxGA_SMOOTH
wxSL_HORIZONTAL = wxc.wxSL_HORIZONTAL
wxSL_VERTICAL = wxc.wxSL_VERTICAL
wxSL_AUTOTICKS = wxc.wxSL_AUTOTICKS
wxSL_LABELS = wxc.wxSL_LABELS
wxSL_LEFT = wxc.wxSL_LEFT
wxSL_TOP = wxc.wxSL_TOP
wxSL_RIGHT = wxc.wxSL_RIGHT
wxSL_BOTTOM = wxc.wxSL_BOTTOM
wxSL_BOTH = wxc.wxSL_BOTH
wxSL_SELRANGE = wxc.wxSL_SELRANGE
wxSB_HORIZONTAL = wxc.wxSB_HORIZONTAL
wxSB_VERTICAL = wxc.wxSB_VERTICAL
wxST_SIZEGRIP = wxc.wxST_SIZEGRIP
wxBU_AUTODRAW = wxc.wxBU_AUTODRAW
wxBU_NOAUTODRAW = wxc.wxBU_NOAUTODRAW
wxTR_HAS_BUTTONS = wxc.wxTR_HAS_BUTTONS
wxTR_EDIT_LABELS = wxc.wxTR_EDIT_LABELS
wxTR_LINES_AT_ROOT = wxc.wxTR_LINES_AT_ROOT
wxTR_MULTIPLE = wxc.wxTR_MULTIPLE
wxTR_SINGLE = wxc.wxTR_SINGLE
wxTR_HAS_VARIABLE_ROW_HEIGHT = wxc.wxTR_HAS_VARIABLE_ROW_HEIGHT
wxLC_ICON = wxc.wxLC_ICON
wxLC_SMALL_ICON = wxc.wxLC_SMALL_ICON
wxLC_LIST = wxc.wxLC_LIST
wxLC_REPORT = wxc.wxLC_REPORT
wxLC_ALIGN_TOP = wxc.wxLC_ALIGN_TOP
wxLC_ALIGN_LEFT = wxc.wxLC_ALIGN_LEFT
wxLC_AUTOARRANGE = wxc.wxLC_AUTOARRANGE
wxLC_USER_TEXT = wxc.wxLC_USER_TEXT
wxLC_EDIT_LABELS = wxc.wxLC_EDIT_LABELS
wxLC_NO_HEADER = wxc.wxLC_NO_HEADER
wxLC_NO_SORT_HEADER = wxc.wxLC_NO_SORT_HEADER
wxLC_SINGLE_SEL = wxc.wxLC_SINGLE_SEL
wxLC_SORT_ASCENDING = wxc.wxLC_SORT_ASCENDING
wxLC_SORT_DESCENDING = wxc.wxLC_SORT_DESCENDING
wxLC_MASK_TYPE = wxc.wxLC_MASK_TYPE
wxLC_MASK_ALIGN = wxc.wxLC_MASK_ALIGN
wxLC_MASK_SORT = wxc.wxLC_MASK_SORT
wxSP_VERTICAL = wxc.wxSP_VERTICAL
wxSP_HORIZONTAL = wxc.wxSP_HORIZONTAL
wxSP_ARROW_KEYS = wxc.wxSP_ARROW_KEYS
wxSP_WRAP = wxc.wxSP_WRAP
wxSP_NOBORDER = wxc.wxSP_NOBORDER
wxSP_3D = wxc.wxSP_3D
wxSP_BORDER = wxc.wxSP_BORDER
wxFLOOD_SURFACE = wxc.wxFLOOD_SURFACE
wxFLOOD_BORDER = wxc.wxFLOOD_BORDER
wxODDEVEN_RULE = wxc.wxODDEVEN_RULE
wxWINDING_RULE = wxc.wxWINDING_RULE
wxTOOL_TOP = wxc.wxTOOL_TOP
wxTOOL_BOTTOM = wxc.wxTOOL_BOTTOM
wxTOOL_LEFT = wxc.wxTOOL_LEFT
wxTOOL_RIGHT = wxc.wxTOOL_RIGHT
wxOK = wxc.wxOK
wxYES_NO = wxc.wxYES_NO
wxCANCEL = wxc.wxCANCEL
wxYES = wxc.wxYES
wxNO = wxc.wxNO
wxNO_DEFAULT = wxc.wxNO_DEFAULT
wxYES_DEFAULT = wxc.wxYES_DEFAULT
wxICON_EXCLAMATION = wxc.wxICON_EXCLAMATION
wxICON_HAND = wxc.wxICON_HAND
wxICON_QUESTION = wxc.wxICON_QUESTION
wxICON_INFORMATION = wxc.wxICON_INFORMATION
wxICON_STOP = wxc.wxICON_STOP
wxICON_ASTERISK = wxc.wxICON_ASTERISK
wxICON_MASK = wxc.wxICON_MASK
wxICON_WARNING = wxc.wxICON_WARNING
wxICON_ERROR = wxc.wxICON_ERROR
wxFORWARD = wxc.wxFORWARD
wxBACKWARD = wxc.wxBACKWARD
wxRESET = wxc.wxRESET
wxHELP = wxc.wxHELP
wxMORE = wxc.wxMORE
wxSETUP = wxc.wxSETUP
wxCENTRE = wxc.wxCENTRE
wxCENTER = wxc.wxCENTER
wxSIZE_AUTO_WIDTH = wxc.wxSIZE_AUTO_WIDTH
wxSIZE_AUTO_HEIGHT = wxc.wxSIZE_AUTO_HEIGHT
wxSIZE_AUTO = wxc.wxSIZE_AUTO
wxSIZE_USE_EXISTING = wxc.wxSIZE_USE_EXISTING
wxSIZE_ALLOW_MINUS_ONE = wxc.wxSIZE_ALLOW_MINUS_ONE
wxPORTRAIT = wxc.wxPORTRAIT
wxLANDSCAPE = wxc.wxLANDSCAPE
wxPRINT_QUALITY_HIGH = wxc.wxPRINT_QUALITY_HIGH
wxPRINT_QUALITY_MEDIUM = wxc.wxPRINT_QUALITY_MEDIUM
wxPRINT_QUALITY_LOW = wxc.wxPRINT_QUALITY_LOW
wxPRINT_QUALITY_DRAFT = wxc.wxPRINT_QUALITY_DRAFT
wxID_OPEN = wxc.wxID_OPEN
wxID_CLOSE = wxc.wxID_CLOSE
wxID_NEW = wxc.wxID_NEW
wxID_SAVE = wxc.wxID_SAVE
wxID_SAVEAS = wxc.wxID_SAVEAS
wxID_REVERT = wxc.wxID_REVERT
wxID_EXIT = wxc.wxID_EXIT
wxID_UNDO = wxc.wxID_UNDO
wxID_REDO = wxc.wxID_REDO
wxID_HELP = wxc.wxID_HELP
wxID_PRINT = wxc.wxID_PRINT
wxID_PRINT_SETUP = wxc.wxID_PRINT_SETUP
wxID_PREVIEW = wxc.wxID_PREVIEW
wxID_ABOUT = wxc.wxID_ABOUT
wxID_HELP_CONTENTS = wxc.wxID_HELP_CONTENTS
wxID_HELP_COMMANDS = wxc.wxID_HELP_COMMANDS
wxID_HELP_PROCEDURES = wxc.wxID_HELP_PROCEDURES
wxID_HELP_CONTEXT = wxc.wxID_HELP_CONTEXT
wxID_CUT = wxc.wxID_CUT
wxID_COPY = wxc.wxID_COPY
wxID_PASTE = wxc.wxID_PASTE
wxID_CLEAR = wxc.wxID_CLEAR
wxID_FIND = wxc.wxID_FIND
wxID_FILE1 = wxc.wxID_FILE1
wxID_FILE2 = wxc.wxID_FILE2
wxID_FILE3 = wxc.wxID_FILE3
wxID_FILE4 = wxc.wxID_FILE4
wxID_FILE5 = wxc.wxID_FILE5
wxID_FILE6 = wxc.wxID_FILE6
wxID_FILE7 = wxc.wxID_FILE7
wxID_FILE8 = wxc.wxID_FILE8
wxID_FILE9 = wxc.wxID_FILE9
wxID_OK = wxc.wxID_OK
wxID_CANCEL = wxc.wxID_CANCEL
wxID_APPLY = wxc.wxID_APPLY
wxID_YES = wxc.wxID_YES
wxID_NO = wxc.wxID_NO
wxID_STATIC = wxc.wxID_STATIC
wxBITMAP_TYPE_BMP = wxc.wxBITMAP_TYPE_BMP
wxBITMAP_TYPE_BMP_RESOURCE = wxc.wxBITMAP_TYPE_BMP_RESOURCE
wxBITMAP_TYPE_ICO = wxc.wxBITMAP_TYPE_ICO
wxBITMAP_TYPE_ICO_RESOURCE = wxc.wxBITMAP_TYPE_ICO_RESOURCE
wxBITMAP_TYPE_CUR = wxc.wxBITMAP_TYPE_CUR
wxBITMAP_TYPE_CUR_RESOURCE = wxc.wxBITMAP_TYPE_CUR_RESOURCE
wxBITMAP_TYPE_XBM = wxc.wxBITMAP_TYPE_XBM
wxBITMAP_TYPE_XBM_DATA = wxc.wxBITMAP_TYPE_XBM_DATA
wxBITMAP_TYPE_XPM = wxc.wxBITMAP_TYPE_XPM
wxBITMAP_TYPE_XPM_DATA = wxc.wxBITMAP_TYPE_XPM_DATA
wxBITMAP_TYPE_TIF = wxc.wxBITMAP_TYPE_TIF
wxBITMAP_TYPE_TIF_RESOURCE = wxc.wxBITMAP_TYPE_TIF_RESOURCE
wxBITMAP_TYPE_GIF = wxc.wxBITMAP_TYPE_GIF
wxBITMAP_TYPE_GIF_RESOURCE = wxc.wxBITMAP_TYPE_GIF_RESOURCE
wxBITMAP_TYPE_PNG = wxc.wxBITMAP_TYPE_PNG
wxBITMAP_TYPE_PNG_RESOURCE = wxc.wxBITMAP_TYPE_PNG_RESOURCE
wxBITMAP_TYPE_ANY = wxc.wxBITMAP_TYPE_ANY
wxBITMAP_TYPE_RESOURCE = wxc.wxBITMAP_TYPE_RESOURCE
wxBITMAP_TYPE_JPEG = wxc.wxBITMAP_TYPE_JPEG
wxOPEN = wxc.wxOPEN
wxSAVE = wxc.wxSAVE
wxHIDE_READONLY = wxc.wxHIDE_READONLY
wxOVERWRITE_PROMPT = wxc.wxOVERWRITE_PROMPT
wxACCEL_ALT = wxc.wxACCEL_ALT
wxACCEL_CTRL = wxc.wxACCEL_CTRL
wxACCEL_SHIFT = wxc.wxACCEL_SHIFT
wxPD_AUTO_HIDE = wxc.wxPD_AUTO_HIDE
wxPD_APP_MODAL = wxc.wxPD_APP_MODAL
wxPD_CAN_ABORT = wxc.wxPD_CAN_ABORT
wxPD_ELAPSED_TIME = wxc.wxPD_ELAPSED_TIME
wxPD_ESTIMATED_TIME = wxc.wxPD_ESTIMATED_TIME
wxPD_REMAINING_TIME = wxc.wxPD_REMAINING_TIME
wxMENU_TEAROFF = wxc.wxMENU_TEAROFF
wxMB_DOCKABLE = wxc.wxMB_DOCKABLE
wxNO_FULL_REPAINT_ON_RESIZE = wxc.wxNO_FULL_REPAINT_ON_RESIZE
wxLEFT = wxc.wxLEFT
wxRIGHT = wxc.wxRIGHT
wxUP = wxc.wxUP
wxDOWN = wxc.wxDOWN
wxALL = wxc.wxALL
wxTOP = wxc.wxTOP
wxBOTTOM = wxc.wxBOTTOM
wxNORTH = wxc.wxNORTH
wxSOUTH = wxc.wxSOUTH
wxEAST = wxc.wxEAST
wxWEST = wxc.wxWEST
wxSTRETCH_NOT = wxc.wxSTRETCH_NOT
wxSHRINK = wxc.wxSHRINK
wxGROW = wxc.wxGROW
wxEXPAND = wxc.wxEXPAND
wxNB_FIXEDWIDTH = wxc.wxNB_FIXEDWIDTH
wxNB_LEFT = wxc.wxNB_LEFT
wxNB_RIGHT = wxc.wxNB_RIGHT
wxNB_BOTTOM = wxc.wxNB_BOTTOM
wxLI_HORIZONTAL = wxc.wxLI_HORIZONTAL
wxLI_VERTICAL = wxc.wxLI_VERTICAL
wxHW_SCROLLBAR_NEVER = wxc.wxHW_SCROLLBAR_NEVER
wxHW_SCROLLBAR_AUTO = wxc.wxHW_SCROLLBAR_AUTO
wxDEFAULT = wxc.wxDEFAULT
wxDECORATIVE = wxc.wxDECORATIVE
wxROMAN = wxc.wxROMAN
wxSCRIPT = wxc.wxSCRIPT
wxSWISS = wxc.wxSWISS
wxMODERN = wxc.wxMODERN
wxTELETYPE = wxc.wxTELETYPE
wxVARIABLE = wxc.wxVARIABLE
wxFIXED = wxc.wxFIXED
wxNORMAL = wxc.wxNORMAL
wxLIGHT = wxc.wxLIGHT
wxBOLD = wxc.wxBOLD
wxITALIC = wxc.wxITALIC
wxSLANT = wxc.wxSLANT
wxSOLID = wxc.wxSOLID
wxDOT = wxc.wxDOT
wxLONG_DASH = wxc.wxLONG_DASH
wxSHORT_DASH = wxc.wxSHORT_DASH
wxDOT_DASH = wxc.wxDOT_DASH
wxUSER_DASH = wxc.wxUSER_DASH
wxTRANSPARENT = wxc.wxTRANSPARENT
wxSTIPPLE = wxc.wxSTIPPLE
wxBDIAGONAL_HATCH = wxc.wxBDIAGONAL_HATCH
wxCROSSDIAG_HATCH = wxc.wxCROSSDIAG_HATCH
wxFDIAGONAL_HATCH = wxc.wxFDIAGONAL_HATCH
wxCROSS_HATCH = wxc.wxCROSS_HATCH
wxHORIZONTAL_HATCH = wxc.wxHORIZONTAL_HATCH
wxVERTICAL_HATCH = wxc.wxVERTICAL_HATCH
wxJOIN_BEVEL = wxc.wxJOIN_BEVEL
wxJOIN_MITER = wxc.wxJOIN_MITER
wxJOIN_ROUND = wxc.wxJOIN_ROUND
wxCAP_ROUND = wxc.wxCAP_ROUND
wxCAP_PROJECTING = wxc.wxCAP_PROJECTING
wxCAP_BUTT = wxc.wxCAP_BUTT
wxCLEAR = wxc.wxCLEAR
wxXOR = wxc.wxXOR
wxINVERT = wxc.wxINVERT
wxOR_REVERSE = wxc.wxOR_REVERSE
wxAND_REVERSE = wxc.wxAND_REVERSE
wxCOPY = wxc.wxCOPY
wxAND = wxc.wxAND
wxAND_INVERT = wxc.wxAND_INVERT
wxNO_OP = wxc.wxNO_OP
wxNOR = wxc.wxNOR
wxEQUIV = wxc.wxEQUIV
wxSRC_INVERT = wxc.wxSRC_INVERT
wxOR_INVERT = wxc.wxOR_INVERT
wxNAND = wxc.wxNAND
wxOR = wxc.wxOR
wxSET = wxc.wxSET
wxSRC_OR = wxc.wxSRC_OR
wxSRC_AND = wxc.wxSRC_AND
WXK_BACK = wxc.WXK_BACK
WXK_TAB = wxc.WXK_TAB
WXK_RETURN = wxc.WXK_RETURN
WXK_ESCAPE = wxc.WXK_ESCAPE
WXK_SPACE = wxc.WXK_SPACE
WXK_DELETE = wxc.WXK_DELETE
WXK_START = wxc.WXK_START
WXK_LBUTTON = wxc.WXK_LBUTTON
WXK_RBUTTON = wxc.WXK_RBUTTON
WXK_CANCEL = wxc.WXK_CANCEL
WXK_MBUTTON = wxc.WXK_MBUTTON
WXK_CLEAR = wxc.WXK_CLEAR
WXK_SHIFT = wxc.WXK_SHIFT
WXK_CONTROL = wxc.WXK_CONTROL
WXK_MENU = wxc.WXK_MENU
WXK_PAUSE = wxc.WXK_PAUSE
WXK_CAPITAL = wxc.WXK_CAPITAL
WXK_PRIOR = wxc.WXK_PRIOR
WXK_NEXT = wxc.WXK_NEXT
WXK_END = wxc.WXK_END
WXK_HOME = wxc.WXK_HOME
WXK_LEFT = wxc.WXK_LEFT
WXK_UP = wxc.WXK_UP
WXK_RIGHT = wxc.WXK_RIGHT
WXK_DOWN = wxc.WXK_DOWN
WXK_SELECT = wxc.WXK_SELECT
WXK_PRINT = wxc.WXK_PRINT
WXK_EXECUTE = wxc.WXK_EXECUTE
WXK_SNAPSHOT = wxc.WXK_SNAPSHOT
WXK_INSERT = wxc.WXK_INSERT
WXK_HELP = wxc.WXK_HELP
WXK_NUMPAD0 = wxc.WXK_NUMPAD0
WXK_NUMPAD1 = wxc.WXK_NUMPAD1
WXK_NUMPAD2 = wxc.WXK_NUMPAD2
WXK_NUMPAD3 = wxc.WXK_NUMPAD3
WXK_NUMPAD4 = wxc.WXK_NUMPAD4
WXK_NUMPAD5 = wxc.WXK_NUMPAD5
WXK_NUMPAD6 = wxc.WXK_NUMPAD6
WXK_NUMPAD7 = wxc.WXK_NUMPAD7
WXK_NUMPAD8 = wxc.WXK_NUMPAD8
WXK_NUMPAD9 = wxc.WXK_NUMPAD9
WXK_MULTIPLY = wxc.WXK_MULTIPLY
WXK_ADD = wxc.WXK_ADD
WXK_SEPARATOR = wxc.WXK_SEPARATOR
WXK_SUBTRACT = wxc.WXK_SUBTRACT
WXK_DECIMAL = wxc.WXK_DECIMAL
WXK_DIVIDE = wxc.WXK_DIVIDE
WXK_F1 = wxc.WXK_F1
WXK_F2 = wxc.WXK_F2
WXK_F3 = wxc.WXK_F3
WXK_F4 = wxc.WXK_F4
WXK_F5 = wxc.WXK_F5
WXK_F6 = wxc.WXK_F6
WXK_F7 = wxc.WXK_F7
WXK_F8 = wxc.WXK_F8
WXK_F9 = wxc.WXK_F9
WXK_F10 = wxc.WXK_F10
WXK_F11 = wxc.WXK_F11
WXK_F12 = wxc.WXK_F12
WXK_F13 = wxc.WXK_F13
WXK_F14 = wxc.WXK_F14
WXK_F15 = wxc.WXK_F15
WXK_F16 = wxc.WXK_F16
WXK_F17 = wxc.WXK_F17
WXK_F18 = wxc.WXK_F18
WXK_F19 = wxc.WXK_F19
WXK_F20 = wxc.WXK_F20
WXK_F21 = wxc.WXK_F21
WXK_F22 = wxc.WXK_F22
WXK_F23 = wxc.WXK_F23
WXK_F24 = wxc.WXK_F24
WXK_NUMLOCK = wxc.WXK_NUMLOCK
WXK_SCROLL = wxc.WXK_SCROLL
WXK_PAGEUP = wxc.WXK_PAGEUP
WXK_PAGEDOWN = wxc.WXK_PAGEDOWN
wxCURSOR_NONE = wxc.wxCURSOR_NONE
wxCURSOR_ARROW = wxc.wxCURSOR_ARROW
wxCURSOR_BULLSEYE = wxc.wxCURSOR_BULLSEYE
wxCURSOR_CHAR = wxc.wxCURSOR_CHAR
wxCURSOR_CROSS = wxc.wxCURSOR_CROSS
wxCURSOR_HAND = wxc.wxCURSOR_HAND
wxCURSOR_IBEAM = wxc.wxCURSOR_IBEAM
wxCURSOR_LEFT_BUTTON = wxc.wxCURSOR_LEFT_BUTTON
wxCURSOR_MAGNIFIER = wxc.wxCURSOR_MAGNIFIER
wxCURSOR_MIDDLE_BUTTON = wxc.wxCURSOR_MIDDLE_BUTTON
wxCURSOR_NO_ENTRY = wxc.wxCURSOR_NO_ENTRY
wxCURSOR_PAINT_BRUSH = wxc.wxCURSOR_PAINT_BRUSH
wxCURSOR_PENCIL = wxc.wxCURSOR_PENCIL
wxCURSOR_POINT_LEFT = wxc.wxCURSOR_POINT_LEFT
wxCURSOR_POINT_RIGHT = wxc.wxCURSOR_POINT_RIGHT
wxCURSOR_QUESTION_ARROW = wxc.wxCURSOR_QUESTION_ARROW
wxCURSOR_RIGHT_BUTTON = wxc.wxCURSOR_RIGHT_BUTTON
wxCURSOR_SIZENESW = wxc.wxCURSOR_SIZENESW
wxCURSOR_SIZENS = wxc.wxCURSOR_SIZENS
wxCURSOR_SIZENWSE = wxc.wxCURSOR_SIZENWSE
wxCURSOR_SIZEWE = wxc.wxCURSOR_SIZEWE
wxCURSOR_SIZING = wxc.wxCURSOR_SIZING
wxCURSOR_SPRAYCAN = wxc.wxCURSOR_SPRAYCAN
wxCURSOR_WAIT = wxc.wxCURSOR_WAIT
wxCURSOR_WATCH = wxc.wxCURSOR_WATCH
wxCURSOR_BLANK = wxc.wxCURSOR_BLANK
wxPAPER_NONE = wxc.wxPAPER_NONE
wxPAPER_LETTER = wxc.wxPAPER_LETTER
wxPAPER_LEGAL = wxc.wxPAPER_LEGAL
wxPAPER_A4 = wxc.wxPAPER_A4
wxPAPER_CSHEET = wxc.wxPAPER_CSHEET
wxPAPER_DSHEET = wxc.wxPAPER_DSHEET
wxPAPER_ESHEET = wxc.wxPAPER_ESHEET
wxPAPER_LETTERSMALL = wxc.wxPAPER_LETTERSMALL
wxPAPER_TABLOID = wxc.wxPAPER_TABLOID
wxPAPER_LEDGER = wxc.wxPAPER_LEDGER
wxPAPER_STATEMENT = wxc.wxPAPER_STATEMENT
wxPAPER_EXECUTIVE = wxc.wxPAPER_EXECUTIVE
wxPAPER_A3 = wxc.wxPAPER_A3
wxPAPER_A4SMALL = wxc.wxPAPER_A4SMALL
wxPAPER_A5 = wxc.wxPAPER_A5
wxPAPER_B4 = wxc.wxPAPER_B4
wxPAPER_B5 = wxc.wxPAPER_B5
wxPAPER_FOLIO = wxc.wxPAPER_FOLIO
wxPAPER_QUARTO = wxc.wxPAPER_QUARTO
wxPAPER_10X14 = wxc.wxPAPER_10X14
wxPAPER_11X17 = wxc.wxPAPER_11X17
wxPAPER_NOTE = wxc.wxPAPER_NOTE
wxPAPER_ENV_9 = wxc.wxPAPER_ENV_9
wxPAPER_ENV_10 = wxc.wxPAPER_ENV_10
wxPAPER_ENV_11 = wxc.wxPAPER_ENV_11
wxPAPER_ENV_12 = wxc.wxPAPER_ENV_12
wxPAPER_ENV_14 = wxc.wxPAPER_ENV_14
wxPAPER_ENV_DL = wxc.wxPAPER_ENV_DL
wxPAPER_ENV_C5 = wxc.wxPAPER_ENV_C5
wxPAPER_ENV_C3 = wxc.wxPAPER_ENV_C3
wxPAPER_ENV_C4 = wxc.wxPAPER_ENV_C4
wxPAPER_ENV_C6 = wxc.wxPAPER_ENV_C6
wxPAPER_ENV_C65 = wxc.wxPAPER_ENV_C65
wxPAPER_ENV_B4 = wxc.wxPAPER_ENV_B4
wxPAPER_ENV_B5 = wxc.wxPAPER_ENV_B5
wxPAPER_ENV_B6 = wxc.wxPAPER_ENV_B6
wxPAPER_ENV_ITALY = wxc.wxPAPER_ENV_ITALY
wxPAPER_ENV_MONARCH = wxc.wxPAPER_ENV_MONARCH
wxPAPER_ENV_PERSONAL = wxc.wxPAPER_ENV_PERSONAL
wxPAPER_FANFOLD_US = wxc.wxPAPER_FANFOLD_US
wxPAPER_FANFOLD_STD_GERMAN = wxc.wxPAPER_FANFOLD_STD_GERMAN
wxPAPER_FANFOLD_LGL_GERMAN = wxc.wxPAPER_FANFOLD_LGL_GERMAN
wxPAPER_ISO_B4 = wxc.wxPAPER_ISO_B4
wxPAPER_JAPANESE_POSTCARD = wxc.wxPAPER_JAPANESE_POSTCARD
wxPAPER_9X11 = wxc.wxPAPER_9X11
wxPAPER_10X11 = wxc.wxPAPER_10X11
wxPAPER_15X11 = wxc.wxPAPER_15X11
wxPAPER_ENV_INVITE = wxc.wxPAPER_ENV_INVITE
wxPAPER_LETTER_EXTRA = wxc.wxPAPER_LETTER_EXTRA
wxPAPER_LEGAL_EXTRA = wxc.wxPAPER_LEGAL_EXTRA
wxPAPER_TABLOID_EXTRA = wxc.wxPAPER_TABLOID_EXTRA
wxPAPER_A4_EXTRA = wxc.wxPAPER_A4_EXTRA
wxPAPER_LETTER_TRANSVERSE = wxc.wxPAPER_LETTER_TRANSVERSE
wxPAPER_A4_TRANSVERSE = wxc.wxPAPER_A4_TRANSVERSE
wxPAPER_LETTER_EXTRA_TRANSVERSE = wxc.wxPAPER_LETTER_EXTRA_TRANSVERSE
wxPAPER_A_PLUS = wxc.wxPAPER_A_PLUS
wxPAPER_B_PLUS = wxc.wxPAPER_B_PLUS
wxPAPER_LETTER_PLUS = wxc.wxPAPER_LETTER_PLUS
wxPAPER_A4_PLUS = wxc.wxPAPER_A4_PLUS
wxPAPER_A5_TRANSVERSE = wxc.wxPAPER_A5_TRANSVERSE
wxPAPER_B5_TRANSVERSE = wxc.wxPAPER_B5_TRANSVERSE
wxPAPER_A3_EXTRA = wxc.wxPAPER_A3_EXTRA
wxPAPER_A5_EXTRA = wxc.wxPAPER_A5_EXTRA
wxPAPER_B5_EXTRA = wxc.wxPAPER_B5_EXTRA
wxPAPER_A2 = wxc.wxPAPER_A2
wxPAPER_A3_TRANSVERSE = wxc.wxPAPER_A3_TRANSVERSE
wxPAPER_A3_EXTRA_TRANSVERSE = wxc.wxPAPER_A3_EXTRA_TRANSVERSE
wxDUPLEX_SIMPLEX = wxc.wxDUPLEX_SIMPLEX
wxDUPLEX_HORIZONTAL = wxc.wxDUPLEX_HORIZONTAL
wxDUPLEX_VERTICAL = wxc.wxDUPLEX_VERTICAL
FALSE = wxc.FALSE
false = wxc.false
TRUE = wxc.TRUE
true = wxc.true
wxVERSION_STRING = wxc.wxVERSION_STRING
wxEVT_NULL = wxc.wxEVT_NULL
wxEVT_FIRST = wxc.wxEVT_FIRST
wxEVT_COMMAND_BUTTON_CLICKED = wxc.wxEVT_COMMAND_BUTTON_CLICKED
wxEVT_COMMAND_CHECKBOX_CLICKED = wxc.wxEVT_COMMAND_CHECKBOX_CLICKED
wxEVT_COMMAND_CHOICE_SELECTED = wxc.wxEVT_COMMAND_CHOICE_SELECTED
wxEVT_COMMAND_LISTBOX_SELECTED = wxc.wxEVT_COMMAND_LISTBOX_SELECTED
wxEVT_COMMAND_LISTBOX_DOUBLECLICKED = wxc.wxEVT_COMMAND_LISTBOX_DOUBLECLICKED
wxEVT_COMMAND_CHECKLISTBOX_TOGGLED = wxc.wxEVT_COMMAND_CHECKLISTBOX_TOGGLED
wxEVT_COMMAND_TEXT_UPDATED = wxc.wxEVT_COMMAND_TEXT_UPDATED
wxEVT_COMMAND_TEXT_ENTER = wxc.wxEVT_COMMAND_TEXT_ENTER
wxEVT_COMMAND_MENU_SELECTED = wxc.wxEVT_COMMAND_MENU_SELECTED
wxEVT_COMMAND_SLIDER_UPDATED = wxc.wxEVT_COMMAND_SLIDER_UPDATED
wxEVT_COMMAND_RADIOBOX_SELECTED = wxc.wxEVT_COMMAND_RADIOBOX_SELECTED
wxEVT_COMMAND_RADIOBUTTON_SELECTED = wxc.wxEVT_COMMAND_RADIOBUTTON_SELECTED
wxEVT_COMMAND_SCROLLBAR_UPDATED = wxc.wxEVT_COMMAND_SCROLLBAR_UPDATED
wxEVT_COMMAND_VLBOX_SELECTED = wxc.wxEVT_COMMAND_VLBOX_SELECTED
wxEVT_COMMAND_COMBOBOX_SELECTED = wxc.wxEVT_COMMAND_COMBOBOX_SELECTED
wxEVT_COMMAND_TOOL_CLICKED = wxc.wxEVT_COMMAND_TOOL_CLICKED
wxEVT_COMMAND_TOOL_RCLICKED = wxc.wxEVT_COMMAND_TOOL_RCLICKED
wxEVT_COMMAND_TOOL_ENTER = wxc.wxEVT_COMMAND_TOOL_ENTER
wxEVT_SET_FOCUS = wxc.wxEVT_SET_FOCUS
wxEVT_KILL_FOCUS = wxc.wxEVT_KILL_FOCUS
wxEVT_LEFT_DOWN = wxc.wxEVT_LEFT_DOWN
wxEVT_LEFT_UP = wxc.wxEVT_LEFT_UP
wxEVT_MIDDLE_DOWN = wxc.wxEVT_MIDDLE_DOWN
wxEVT_MIDDLE_UP = wxc.wxEVT_MIDDLE_UP
wxEVT_RIGHT_DOWN = wxc.wxEVT_RIGHT_DOWN
wxEVT_RIGHT_UP = wxc.wxEVT_RIGHT_UP
wxEVT_MOTION = wxc.wxEVT_MOTION
wxEVT_ENTER_WINDOW = wxc.wxEVT_ENTER_WINDOW
wxEVT_LEAVE_WINDOW = wxc.wxEVT_LEAVE_WINDOW
wxEVT_LEFT_DCLICK = wxc.wxEVT_LEFT_DCLICK
wxEVT_MIDDLE_DCLICK = wxc.wxEVT_MIDDLE_DCLICK
wxEVT_RIGHT_DCLICK = wxc.wxEVT_RIGHT_DCLICK
wxEVT_NC_LEFT_DOWN = wxc.wxEVT_NC_LEFT_DOWN
wxEVT_NC_LEFT_UP = wxc.wxEVT_NC_LEFT_UP
wxEVT_NC_MIDDLE_DOWN = wxc.wxEVT_NC_MIDDLE_DOWN
wxEVT_NC_MIDDLE_UP = wxc.wxEVT_NC_MIDDLE_UP
wxEVT_NC_RIGHT_DOWN = wxc.wxEVT_NC_RIGHT_DOWN
wxEVT_NC_RIGHT_UP = wxc.wxEVT_NC_RIGHT_UP
wxEVT_NC_MOTION = wxc.wxEVT_NC_MOTION
wxEVT_NC_ENTER_WINDOW = wxc.wxEVT_NC_ENTER_WINDOW
wxEVT_NC_LEAVE_WINDOW = wxc.wxEVT_NC_LEAVE_WINDOW
wxEVT_NC_LEFT_DCLICK = wxc.wxEVT_NC_LEFT_DCLICK
wxEVT_NC_MIDDLE_DCLICK = wxc.wxEVT_NC_MIDDLE_DCLICK
wxEVT_NC_RIGHT_DCLICK = wxc.wxEVT_NC_RIGHT_DCLICK
wxEVT_CHAR = wxc.wxEVT_CHAR
wxEVT_KEY_DOWN = wxc.wxEVT_KEY_DOWN
wxEVT_KEY_UP = wxc.wxEVT_KEY_UP
wxEVT_CHAR_HOOK = wxc.wxEVT_CHAR_HOOK
wxEVT_SCROLL_TOP = wxc.wxEVT_SCROLL_TOP
wxEVT_SCROLL_BOTTOM = wxc.wxEVT_SCROLL_BOTTOM
wxEVT_SCROLL_LINEUP = wxc.wxEVT_SCROLL_LINEUP
wxEVT_SCROLL_LINEDOWN = wxc.wxEVT_SCROLL_LINEDOWN
wxEVT_SCROLL_PAGEUP = wxc.wxEVT_SCROLL_PAGEUP
wxEVT_SCROLL_PAGEDOWN = wxc.wxEVT_SCROLL_PAGEDOWN
wxEVT_SCROLL_THUMBTRACK = wxc.wxEVT_SCROLL_THUMBTRACK
wxEVT_SCROLLWIN_TOP = wxc.wxEVT_SCROLLWIN_TOP
wxEVT_SCROLLWIN_BOTTOM = wxc.wxEVT_SCROLLWIN_BOTTOM
wxEVT_SCROLLWIN_LINEUP = wxc.wxEVT_SCROLLWIN_LINEUP
wxEVT_SCROLLWIN_LINEDOWN = wxc.wxEVT_SCROLLWIN_LINEDOWN
wxEVT_SCROLLWIN_PAGEUP = wxc.wxEVT_SCROLLWIN_PAGEUP
wxEVT_SCROLLWIN_PAGEDOWN = wxc.wxEVT_SCROLLWIN_PAGEDOWN
wxEVT_SCROLLWIN_THUMBTRACK = wxc.wxEVT_SCROLLWIN_THUMBTRACK
wxEVT_SIZE = wxc.wxEVT_SIZE
wxEVT_MOVE = wxc.wxEVT_MOVE
wxEVT_CLOSE_WINDOW = wxc.wxEVT_CLOSE_WINDOW
wxEVT_END_SESSION = wxc.wxEVT_END_SESSION
wxEVT_QUERY_END_SESSION = wxc.wxEVT_QUERY_END_SESSION
wxEVT_ACTIVATE_APP = wxc.wxEVT_ACTIVATE_APP
wxEVT_POWER = wxc.wxEVT_POWER
wxEVT_ACTIVATE = wxc.wxEVT_ACTIVATE
wxEVT_CREATE = wxc.wxEVT_CREATE
wxEVT_DESTROY = wxc.wxEVT_DESTROY
wxEVT_SHOW = wxc.wxEVT_SHOW
wxEVT_ICONIZE = wxc.wxEVT_ICONIZE
wxEVT_MAXIMIZE = wxc.wxEVT_MAXIMIZE
wxEVT_MOUSE_CAPTURE_CHANGED = wxc.wxEVT_MOUSE_CAPTURE_CHANGED
wxEVT_PAINT = wxc.wxEVT_PAINT
wxEVT_ERASE_BACKGROUND = wxc.wxEVT_ERASE_BACKGROUND
wxEVT_NC_PAINT = wxc.wxEVT_NC_PAINT
wxEVT_PAINT_ICON = wxc.wxEVT_PAINT_ICON
wxEVT_MENU_CHAR = wxc.wxEVT_MENU_CHAR
wxEVT_MENU_INIT = wxc.wxEVT_MENU_INIT
wxEVT_MENU_HIGHLIGHT = wxc.wxEVT_MENU_HIGHLIGHT
wxEVT_POPUP_MENU_INIT = wxc.wxEVT_POPUP_MENU_INIT
wxEVT_CONTEXT_MENU = wxc.wxEVT_CONTEXT_MENU
wxEVT_SYS_COLOUR_CHANGED = wxc.wxEVT_SYS_COLOUR_CHANGED
wxEVT_SETTING_CHANGED = wxc.wxEVT_SETTING_CHANGED
wxEVT_QUERY_NEW_PALETTE = wxc.wxEVT_QUERY_NEW_PALETTE
wxEVT_PALETTE_CHANGED = wxc.wxEVT_PALETTE_CHANGED
wxEVT_JOY_BUTTON_DOWN = wxc.wxEVT_JOY_BUTTON_DOWN
wxEVT_JOY_BUTTON_UP = wxc.wxEVT_JOY_BUTTON_UP
wxEVT_JOY_MOVE = wxc.wxEVT_JOY_MOVE
wxEVT_JOY_ZMOVE = wxc.wxEVT_JOY_ZMOVE
wxEVT_DROP_FILES = wxc.wxEVT_DROP_FILES
wxEVT_DRAW_ITEM = wxc.wxEVT_DRAW_ITEM
wxEVT_MEASURE_ITEM = wxc.wxEVT_MEASURE_ITEM
wxEVT_COMPARE_ITEM = wxc.wxEVT_COMPARE_ITEM
wxEVT_INIT_DIALOG = wxc.wxEVT_INIT_DIALOG
wxEVT_IDLE = wxc.wxEVT_IDLE
wxEVT_UPDATE_UI = wxc.wxEVT_UPDATE_UI
wxEVT_COMMAND_LEFT_CLICK = wxc.wxEVT_COMMAND_LEFT_CLICK
wxEVT_COMMAND_LEFT_DCLICK = wxc.wxEVT_COMMAND_LEFT_DCLICK
wxEVT_COMMAND_RIGHT_CLICK = wxc.wxEVT_COMMAND_RIGHT_CLICK
wxEVT_COMMAND_RIGHT_DCLICK = wxc.wxEVT_COMMAND_RIGHT_DCLICK
wxEVT_COMMAND_SET_FOCUS = wxc.wxEVT_COMMAND_SET_FOCUS
wxEVT_COMMAND_KILL_FOCUS = wxc.wxEVT_COMMAND_KILL_FOCUS
wxEVT_COMMAND_ENTER = wxc.wxEVT_COMMAND_ENTER
wxEVT_COMMAND_TREE_BEGIN_DRAG = wxc.wxEVT_COMMAND_TREE_BEGIN_DRAG
wxEVT_COMMAND_TREE_BEGIN_RDRAG = wxc.wxEVT_COMMAND_TREE_BEGIN_RDRAG
wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT = wxc.wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT
wxEVT_COMMAND_TREE_END_LABEL_EDIT = wxc.wxEVT_COMMAND_TREE_END_LABEL_EDIT
wxEVT_COMMAND_TREE_DELETE_ITEM = wxc.wxEVT_COMMAND_TREE_DELETE_ITEM
wxEVT_COMMAND_TREE_GET_INFO = wxc.wxEVT_COMMAND_TREE_GET_INFO
wxEVT_COMMAND_TREE_SET_INFO = wxc.wxEVT_COMMAND_TREE_SET_INFO
wxEVT_COMMAND_TREE_ITEM_EXPANDED = wxc.wxEVT_COMMAND_TREE_ITEM_EXPANDED
wxEVT_COMMAND_TREE_ITEM_EXPANDING = wxc.wxEVT_COMMAND_TREE_ITEM_EXPANDING
wxEVT_COMMAND_TREE_ITEM_COLLAPSED = wxc.wxEVT_COMMAND_TREE_ITEM_COLLAPSED
wxEVT_COMMAND_TREE_ITEM_COLLAPSING = wxc.wxEVT_COMMAND_TREE_ITEM_COLLAPSING
wxEVT_COMMAND_TREE_SEL_CHANGED = wxc.wxEVT_COMMAND_TREE_SEL_CHANGED
wxEVT_COMMAND_TREE_SEL_CHANGING = wxc.wxEVT_COMMAND_TREE_SEL_CHANGING
wxEVT_COMMAND_TREE_KEY_DOWN = wxc.wxEVT_COMMAND_TREE_KEY_DOWN
wxEVT_COMMAND_LIST_BEGIN_DRAG = wxc.wxEVT_COMMAND_LIST_BEGIN_DRAG
wxEVT_COMMAND_LIST_BEGIN_RDRAG = wxc.wxEVT_COMMAND_LIST_BEGIN_RDRAG
wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT = wxc.wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT
wxEVT_COMMAND_LIST_END_LABEL_EDIT = wxc.wxEVT_COMMAND_LIST_END_LABEL_EDIT
wxEVT_COMMAND_LIST_DELETE_ITEM = wxc.wxEVT_COMMAND_LIST_DELETE_ITEM
wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS = wxc.wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS
wxEVT_COMMAND_LIST_GET_INFO = wxc.wxEVT_COMMAND_LIST_GET_INFO
wxEVT_COMMAND_LIST_SET_INFO = wxc.wxEVT_COMMAND_LIST_SET_INFO
wxEVT_COMMAND_LIST_ITEM_SELECTED = wxc.wxEVT_COMMAND_LIST_ITEM_SELECTED
wxEVT_COMMAND_LIST_ITEM_DESELECTED = wxc.wxEVT_COMMAND_LIST_ITEM_DESELECTED
wxEVT_COMMAND_LIST_KEY_DOWN = wxc.wxEVT_COMMAND_LIST_KEY_DOWN
wxEVT_COMMAND_LIST_INSERT_ITEM = wxc.wxEVT_COMMAND_LIST_INSERT_ITEM
wxEVT_COMMAND_LIST_COL_CLICK = wxc.wxEVT_COMMAND_LIST_COL_CLICK
wxEVT_COMMAND_LIST_ITEM_ACTIVATED = wxc.wxEVT_COMMAND_LIST_ITEM_ACTIVATED
wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK = wxc.wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK
wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK = wxc.wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK
wxEVT_COMMAND_TAB_SEL_CHANGED = wxc.wxEVT_COMMAND_TAB_SEL_CHANGED
wxEVT_COMMAND_TAB_SEL_CHANGING = wxc.wxEVT_COMMAND_TAB_SEL_CHANGING
wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED = wxc.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED
wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING = wxc.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING
wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING = wxc.wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING
wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED = wxc.wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED
wxEVT_COMMAND_SPLITTER_UNSPLIT = wxc.wxEVT_COMMAND_SPLITTER_UNSPLIT
wxEVT_COMMAND_SPLITTER_DOUBLECLICKED = wxc.wxEVT_COMMAND_SPLITTER_DOUBLECLICKED
wxEVT_NAVIGATION_KEY = wxc.wxEVT_NAVIGATION_KEY
__version__ = wxc.__version__
cvar = wxc.cvar
wxPyDefaultPosition = wxPointPtr(wxc.cvar.wxPyDefaultPosition)
wxPyDefaultSize = wxSizePtr(wxc.cvar.wxPyDefaultSize)


#-------------- USER INCLUDE -----------------------

#----------------------------------------------------------------------------
# Name:         _extra.py
# Purpose:	This file is appended to the shadow class file generated
#               by SWIG.  We add some unSWIGable things here.
#
# Author:       Robin Dunn
#
# Created:      6/30/97
# RCS-ID:       $Id$
# Copyright:    (c) 1998 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------

import sys

#----------------------------------------------------------------------
# This gives this module's dictionary to the C++ extension code...

_wxSetDictionary(vars())


#----------------------------------------------------------------------
#----------------------------------------------------------------------
# Helper function to link python methods to wxWindows virtual
# functions by name.

def _checkForCallback(obj, name, event, theID=-1):
    try:    cb = getattr(obj, name)
    except: pass
    else:   obj.Connect(theID, -1, event, cb)

##  def _checkClassCallback(obj, name):
##      try:    cb = getattr(obj, name)
##      except: pass
##      else:   obj._addCallback(name, cb)


def _StdWindowCallbacks(win):
    _checkForCallback(win, "OnChar",               wxEVT_CHAR)
    _checkForCallback(win, "OnSize",               wxEVT_SIZE)
    _checkForCallback(win, "OnEraseBackground",    wxEVT_ERASE_BACKGROUND)
    _checkForCallback(win, "OnSysColourChanged",   wxEVT_SYS_COLOUR_CHANGED)
    _checkForCallback(win, "OnInitDialog",         wxEVT_INIT_DIALOG)
    _checkForCallback(win, "OnPaint",              wxEVT_PAINT)
    _checkForCallback(win, "OnIdle",               wxEVT_IDLE)


def _StdFrameCallbacks(win):
    _StdWindowCallbacks(win)
    _checkForCallback(win, "OnActivate",           wxEVT_ACTIVATE)
    _checkForCallback(win, "OnMenuHighlight",      wxEVT_MENU_HIGHLIGHT)
    _checkForCallback(win, "OnCloseWindow",        wxEVT_CLOSE_WINDOW)


def _StdDialogCallbacks(win):
    _StdWindowCallbacks(win)
    _checkForCallback(win, "OnOk",     wxEVT_COMMAND_BUTTON_CLICKED,   wxID_OK)
    _checkForCallback(win, "OnApply",  wxEVT_COMMAND_BUTTON_CLICKED,   wxID_APPLY)
    _checkForCallback(win, "OnCancel", wxEVT_COMMAND_BUTTON_CLICKED,   wxID_CANCEL)
    _checkForCallback(win, "OnCloseWindow", wxEVT_CLOSE_WINDOW)
    _checkForCallback(win, "OnCharHook",    wxEVT_CHAR_HOOK)


def _StdOnScrollCallbacks(win):
    try:    cb = getattr(win, "OnScroll")
    except: pass
    else:   EVT_SCROLL(win, cb)



#----------------------------------------------------------------------
#----------------------------------------------------------------------
# functions that look and act like the C++ Macros of the same name


# Miscellaneous
def EVT_SIZE(win, func):
    win.Connect(-1, -1, wxEVT_SIZE, func)

def EVT_MOVE(win, func):
    win.Connect(-1, -1, wxEVT_MOVE, func)

def EVT_CLOSE(win, func):
    win.Connect(-1, -1, wxEVT_CLOSE_WINDOW, func)

def EVT_PAINT(win, func):
    win.Connect(-1, -1, wxEVT_PAINT, func)

def EVT_ERASE_BACKGROUND(win, func):
    win.Connect(-1, -1, wxEVT_ERASE_BACKGROUND, func)

def EVT_CHAR(win, func):
    win.Connect(-1, -1, wxEVT_CHAR, func)

def EVT_CHAR_HOOK(win, func):
    win.Connect(-1, -1, wxEVT_CHAR_HOOK, func)

def EVT_KEY_DOWN(win, func):
    win.Connect(-1, -1, wxEVT_KEY_DOWN, func)

def EVT_KEY_UP(win, func):
    win.Connect(-1, -1, wxEVT_KEY_UP, func)

def EVT_MENU_HIGHLIGHT(win, id, func):
    win.Connect(id, -1, wxEVT_MENU_HIGHLIGHT, func)

def EVT_MENU_HIGHLIGHT_ALL(win, func):
    win.Connect(-1, -1, wxEVT_MENU_HIGHLIGHT, func)

def EVT_SET_FOCUS(win, func):
    win.Connect(-1, -1, wxEVT_SET_FOCUS, func)

def EVT_KILL_FOCUS(win, func):
    win.Connect(-1, -1, wxEVT_KILL_FOCUS, func)

def EVT_ACTIVATE(win, func):
    win.Connect(-1, -1, wxEVT_ACTIVATE, func)

def EVT_ACTIVATE_APP(win, func):
    win.Connect(-1, -1, wxEVT_ACTIVATE_APP, func)

def EVT_END_SESSION(win, func):
    win.Connect(-1, -1, wxEVT_END_SESSION, func)

def EVT_QUERY_END_SESSION(win, func):
    win.Connect(-1, -1, wxEVT_QUERY_END_SESSION, func)

def EVT_DROP_FILES(win, func):
    win.Connect(-1, -1, wxEVT_DROP_FILES, func)

def EVT_INIT_DIALOG(win, func):
    win.Connect(-1, -1, wxEVT_INIT_DIALOG, func)

def EVT_SYS_COLOUR_CHANGED(win, func):
    win.Connect(-1, -1, wxEVT_SYS_COLOUR_CHANGED, func)

def EVT_SHOW(win, func):
    win.Connect(-1, -1, wxEVT_SHOW, func)

def EVT_MAXIMIZE(win, func):
    win.Connect(-1, -1, wxEVT_MAXIMIZE, func)

def EVT_ICONIZE(win, func):
    win.Connect(-1, -1, wxEVT_ICONIZE, func)

def EVT_NAVIGATION_KEY(win, func):
    win.Connect(-1, -1, wxEVT_NAVIGATION_KEY, func)

def EVT_IDLE(win, func):
    win.Connect(-1, -1, wxEVT_IDLE, func)

def EVT_UPDATE_UI(win, id, func):
    win.Connect(id, -1, wxEVT_UPDATE_UI, func)


# Mouse Events
def EVT_LEFT_DOWN(win, func):
    win.Connect(-1, -1, wxEVT_LEFT_DOWN, func)

def EVT_LEFT_UP(win, func):
    win.Connect(-1, -1, wxEVT_LEFT_UP, func)

def EVT_MIDDLE_DOWN(win, func):
    win.Connect(-1, -1, wxEVT_MIDDLE_DOWN, func)

def EVT_MIDDLE_UP(win, func):
    win.Connect(-1, -1, wxEVT_MIDDLE_UP, func)

def EVT_RIGHT_DOWN(win, func):
    win.Connect(-1, -1, wxEVT_RIGHT_DOWN, func)

def EVT_RIGHT_UP(win, func):
    win.Connect(-1, -1, wxEVT_RIGHT_UP, func)

def EVT_MOTION(win, func):
    win.Connect(-1, -1, wxEVT_MOTION, func)

def EVT_LEFT_DCLICK(win, func):
    win.Connect(-1, -1, wxEVT_LEFT_DCLICK, func)

def EVT_MIDDLE_DCLICK(win, func):
    win.Connect(-1, -1, wxEVT_MIDDLE_DCLICK, func)

def EVT_RIGHT_DCLICK(win, func):
    win.Connect(-1, -1, wxEVT_RIGHT_DCLICK, func)

def EVT_LEAVE_WINDOW(win, func):
    win.Connect(-1, -1, wxEVT_LEAVE_WINDOW, func)

def EVT_ENTER_WINDOW(win, func):
    win.Connect(-1, -1, wxEVT_ENTER_WINDOW, func)


# all mouse events
def EVT_MOUSE_EVENTS(win, func):
    win.Connect(-1, -1, wxEVT_LEFT_DOWN,     func)
    win.Connect(-1, -1, wxEVT_LEFT_UP,       func)
    win.Connect(-1, -1, wxEVT_MIDDLE_DOWN,   func)
    win.Connect(-1, -1, wxEVT_MIDDLE_UP,     func)
    win.Connect(-1, -1, wxEVT_RIGHT_DOWN,    func)
    win.Connect(-1, -1, wxEVT_RIGHT_UP,      func)
    win.Connect(-1, -1, wxEVT_MOTION,        func)
    win.Connect(-1, -1, wxEVT_LEFT_DCLICK,   func)
    win.Connect(-1, -1, wxEVT_MIDDLE_DCLICK, func)
    win.Connect(-1, -1, wxEVT_RIGHT_DCLICK,  func)
    win.Connect(-1, -1, wxEVT_LEAVE_WINDOW,  func)
    win.Connect(-1, -1, wxEVT_ENTER_WINDOW,  func)

# EVT_COMMAND
def EVT_COMMAND(win, id, cmd, func):
    win.Connect(id, -1, cmd, func)

def EVT_COMMAND_RANGE(win, id1, id2, cmd, func):
    win.Connect(id1, id2, cmd, func)

# Scrolling
def EVT_SCROLL(win, func):
    win.Connect(-1, -1, wxEVT_SCROLL_TOP,       func)
    win.Connect(-1, -1, wxEVT_SCROLL_BOTTOM,    func)
    win.Connect(-1, -1, wxEVT_SCROLL_LINEUP,    func)
    win.Connect(-1, -1, wxEVT_SCROLL_LINEDOWN,  func)
    win.Connect(-1, -1, wxEVT_SCROLL_PAGEUP,    func)
    win.Connect(-1, -1, wxEVT_SCROLL_PAGEDOWN,  func)
    win.Connect(-1, -1, wxEVT_SCROLL_THUMBTRACK,func)

def EVT_SCROLL_TOP(win, func):
    win.Connect(-1, -1, wxEVT_SCROLL_TOP, func)

def EVT_SCROLL_BOTTOM(win, func):
    win.Connect(-1, -1, wxEVT_SCROLL_BOTTOM, func)

def EVT_SCROLL_LINEUP(win, func):
    win.Connect(-1, -1, wxEVT_SCROLL_LINEUP, func)

def EVT_SCROLL_LINEDOWN(win, func):
    win.Connect(-1, -1, wxEVT_SCROLL_LINEDOWN, func)

def EVT_SCROLL_PAGEUP(win, func):
    win.Connect(-1, -1, wxEVT_SCROLL_PAGEUP, func)

def EVT_SCROLL_PAGEDOWN(win, func):
    win.Connect(-1, -1, wxEVT_SCROLL_PAGEDOWN, func)

def EVT_SCROLL_THUMBTRACK(win, func):
    win.Connect(-1, -1, wxEVT_SCROLL_THUMBTRACK, func)



# Scrolling, with an id
def EVT_COMMAND_SCROLL(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLL_TOP,       func)
    win.Connect(id, -1, wxEVT_SCROLL_BOTTOM,    func)
    win.Connect(id, -1, wxEVT_SCROLL_LINEUP,    func)
    win.Connect(id, -1, wxEVT_SCROLL_LINEDOWN,  func)
    win.Connect(id, -1, wxEVT_SCROLL_PAGEUP,    func)
    win.Connect(id, -1, wxEVT_SCROLL_PAGEDOWN,  func)
    win.Connect(id, -1, wxEVT_SCROLL_THUMBTRACK,func)

def EVT_COMMAND_SCROLL_TOP(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLL_TOP, func)

def EVT_COMMAND_SCROLL_BOTTOM(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLL_BOTTOM, func)

def EVT_COMMAND_SCROLL_LINEUP(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLL_LINEUP, func)

def EVT_COMMAND_SCROLL_LINEDOWN(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLL_LINEDOWN, func)

def EVT_COMMAND_SCROLL_PAGEUP(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLL_PAGEUP, func)

def EVT_COMMAND_SCROLL_PAGEDOWN(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLL_PAGEDOWN, func)

def EVT_COMMAND_SCROLL_THUMBTRACK(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLL_THUMBTRACK, func)

#---
def EVT_SCROLLWIN(win, func):
    win.Connect(-1, -1, wxEVT_SCROLLWIN_TOP,       func)
    win.Connect(-1, -1, wxEVT_SCROLLWIN_BOTTOM,    func)
    win.Connect(-1, -1, wxEVT_SCROLLWIN_LINEUP,    func)
    win.Connect(-1, -1, wxEVT_SCROLLWIN_LINEDOWN,  func)
    win.Connect(-1, -1, wxEVT_SCROLLWIN_PAGEUP,    func)
    win.Connect(-1, -1, wxEVT_SCROLLWIN_PAGEDOWN,  func)
    win.Connect(-1, -1, wxEVT_SCROLLWIN_THUMBTRACK,func)

def EVT_SCROLLWIN_TOP(win, func):
    win.Connect(-1, -1, wxEVT_SCROLLWIN_TOP, func)

def EVT_SCROLLWIN_BOTTOM(win, func):
    win.Connect(-1, -1, wxEVT_SCROLLWIN_BOTTOM, func)

def EVT_SCROLLWIN_LINEUP(win, func):
    win.Connect(-1, -1, wxEVT_SCROLLWIN_LINEUP, func)

def EVT_SCROLLWIN_LINEDOWN(win, func):
    win.Connect(-1, -1, wxEVT_SCROLLWIN_LINEDOWN, func)

def EVT_SCROLLWIN_PAGEUP(win, func):
    win.Connect(-1, -1, wxEVT_SCROLLWIN_PAGEUP, func)

def EVT_SCROLLWIN_PAGEDOWN(win, func):
    win.Connect(-1, -1, wxEVT_SCROLLWIN_PAGEDOWN, func)

def EVT_SCROLLWIN_THUMBTRACK(win, func):
    win.Connect(-1, -1, wxEVT_SCROLLWIN_THUMBTRACK, func)



# Scrolling, with an id
def EVT_COMMAND_SCROLLWIN(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLLWIN_TOP,       func)
    win.Connect(id, -1, wxEVT_SCROLLWIN_BOTTOM,    func)
    win.Connect(id, -1, wxEVT_SCROLLWIN_LINEUP,    func)
    win.Connect(id, -1, wxEVT_SCROLLWIN_LINEDOWN,  func)
    win.Connect(id, -1, wxEVT_SCROLLWIN_PAGEUP,    func)
    win.Connect(id, -1, wxEVT_SCROLLWIN_PAGEDOWN,  func)
    win.Connect(id, -1, wxEVT_SCROLLWIN_THUMBTRACK,func)

def EVT_COMMAND_SCROLLWIN_TOP(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLLWIN_TOP, func)

def EVT_COMMAND_SCROLLWIN_BOTTOM(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLLWIN_BOTTOM, func)

def EVT_COMMAND_SCROLLWIN_LINEUP(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLLWIN_LINEUP, func)

def EVT_COMMAND_SCROLLWIN_LINEDOWN(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLLWIN_LINEDOWN, func)

def EVT_COMMAND_SCROLLWIN_PAGEUP(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLLWIN_PAGEUP, func)

def EVT_COMMAND_SCROLLWIN_PAGEDOWN(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLLWIN_PAGEDOWN, func)

def EVT_COMMAND_SCROLLWIN_THUMBTRACK(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLLWIN_THUMBTRACK, func)

# Convenience commands
def EVT_BUTTON(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_BUTTON_CLICKED, func)

def EVT_CHECKBOX(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_CHECKBOX_CLICKED, func)

def EVT_CHOICE(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_CHOICE_SELECTED, func)

def EVT_LISTBOX(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LISTBOX_SELECTED, func)

def EVT_LISTBOX_DCLICK(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, func)

def EVT_TEXT(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TEXT_UPDATED, func)

def EVT_TEXT_ENTER(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TEXT_ENTER, func)

def EVT_MENU(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_MENU_SELECTED, func)

def EVT_MENU_RANGE(win, id1, id2, func):
    win.Connect(id1, id2, wxEVT_COMMAND_MENU_SELECTED, func)

def EVT_SLIDER(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_SLIDER_UPDATED, func)

def EVT_RADIOBOX(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_RADIOBOX_SELECTED, func)

def EVT_RADIOBUTTON(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_RADIOBUTTON_SELECTED, func)

def EVT_VLBOX(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_VLBOX_SELECTED, func)

def EVT_COMBOBOX(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_COMBOBOX_SELECTED, func)

def EVT_TOOL(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TOOL_CLICKED, func)

def EVT_TOOL_RCLICKED(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TOOL_RCLICKED, func)

def EVT_TOOL_ENTER(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TOOL_ENTER, func)

def EVT_CHECKLISTBOX(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, func)


# Generic command events

def EVT_COMMAND_LEFT_CLICK(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LEFT_CLICK, func)

def EVT_COMMAND_LEFT_DCLICK(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LEFT_DCLICK, func)

def EVT_COMMAND_RIGHT_CLICK(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_RIGHT_CLICK, func)

def EVT_COMMAND_RIGHT_DCLICK(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_RIGHT_DCLICK, func)

def EVT_COMMAND_SET_FOCUS(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_SET_FOCUS, func)

def EVT_COMMAND_KILL_FOCUS(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_KILL_FOCUS, func)

def EVT_COMMAND_ENTER(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_ENTER, func)


# wxNotebook events
def EVT_NOTEBOOK_PAGE_CHANGED(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, func)

def EVT_NOTEBOOK_PAGE_CHANGING(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, func)


# wxTreeCtrl events
def EVT_TREE_BEGIN_DRAG(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TREE_BEGIN_DRAG, func)

def EVT_TREE_BEGIN_RDRAG(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TREE_BEGIN_RDRAG, func)

def EVT_TREE_BEGIN_LABEL_EDIT(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, func)

def EVT_TREE_END_LABEL_EDIT(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TREE_END_LABEL_EDIT, func)

def EVT_TREE_GET_INFO(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TREE_GET_INFO, func)

def EVT_TREE_SET_INFO(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TREE_SET_INFO, func)

def EVT_TREE_ITEM_EXPANDED(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TREE_ITEM_EXPANDED, func)

def EVT_TREE_ITEM_EXPANDING(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TREE_ITEM_EXPANDING, func)

def EVT_TREE_ITEM_COLLAPSED(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TREE_ITEM_COLLAPSED, func)

def EVT_TREE_ITEM_COLLAPSING(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TREE_ITEM_COLLAPSING, func)

def EVT_TREE_SEL_CHANGED(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TREE_SEL_CHANGED, func)

def EVT_TREE_SEL_CHANGING(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TREE_SEL_CHANGING, func)

def EVT_TREE_KEY_DOWN(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TREE_KEY_DOWN, func)

def EVT_TREE_DELETE_ITEM(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_TREE_DELETE_ITEM, func)


# wxSpinButton
def EVT_SPIN_UP(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLL_LINEUP, func)

def EVT_SPIN_DOWN(win, id, func):
    win.Connect(id, -1,wxEVT_SCROLL_LINEDOWN, func)

def EVT_SPIN(win, id, func):
    win.Connect(id, -1, wxEVT_SCROLL_TOP,       func)
    win.Connect(id, -1, wxEVT_SCROLL_BOTTOM,    func)
    win.Connect(id, -1, wxEVT_SCROLL_LINEUP,    func)
    win.Connect(id, -1, wxEVT_SCROLL_LINEDOWN,  func)
    win.Connect(id, -1, wxEVT_SCROLL_PAGEUP,    func)
    win.Connect(id, -1, wxEVT_SCROLL_PAGEDOWN,  func)
    win.Connect(id, -1, wxEVT_SCROLL_THUMBTRACK,func)




# wxTaskBarIcon
def EVT_TASKBAR_MOVE(win, func):
    win.Connect(-1, -1, wxEVT_TASKBAR_MOVE, func)

def EVT_TASKBAR_LEFT_DOWN(win, func):
    win.Connect(-1, -1, wxEVT_TASKBAR_LEFT_DOWN, func)

def EVT_TASKBAR_LEFT_UP(win, func):
    win.Connect(-1, -1, wxEVT_TASKBAR_LEFT_UP, func)

def EVT_TASKBAR_RIGHT_DOWN(win, func):
    win.Connect(-1, -1, wxEVT_TASKBAR_RIGHT_DOWN, func)

def EVT_TASKBAR_RIGHT_UP(win, func):
    win.Connect(-1, -1, wxEVT_TASKBAR_RIGHT_UP, func)

def EVT_TASKBAR_LEFT_DCLICK(win, func):
    win.Connect(-1, -1, wxEVT_TASKBAR_LEFT_DCLICK, func)

def EVT_TASKBAR_RIGHT_DCLICK(win, func):
    win.Connect(-1, -1, wxEVT_TASKBAR_RIGHT_DCLICK, func)


# wxGrid
def EVT_GRID_SELECT_CELL(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_SELECT_CELL, fn)

def EVT_GRID_CREATE_CELL(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_CREATE_CELL, fn)

def EVT_GRID_CHANGE_LABELS(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_CHANGE_LABELS, fn)

def EVT_GRID_CHANGE_SEL_LABEL(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_CHANGE_SEL_LABEL, fn)

def EVT_GRID_CELL_CHANGE(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_CELL_CHANGE, fn)

def EVT_GRID_CELL_LCLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_CELL_LCLICK, fn)

def EVT_GRID_CELL_RCLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_CELL_RCLICK, fn)

def EVT_GRID_LABEL_LCLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_LABEL_LCLICK, fn)

def EVT_GRID_LABEL_RCLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_LABEL_RCLICK, fn)


# wxSashWindow
def EVT_SASH_DRAGGED(win, id, func):
    win.Connect(id, -1, wxEVT_SASH_DRAGGED, func)

def EVT_SASH_DRAGGED_RANGE(win, id1, id2, func):
    win.Connect(id1, id2, wxEVT_SASH_DRAGGED, func)

def EVT_QUERY_LAYOUT_INFO(win, func):
    win.Connect(-1, -1, wxEVT_EVT_QUERY_LAYOUT_INFO, func)

def EVT_CALCULATE_LAYOUT(win, func):
    win.Connect(-1, -1, wxEVT_EVT_CALCULATE_LAYOUT, func)


# wxListCtrl
def EVT_LIST_BEGIN_DRAG(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_BEGIN_DRAG, func)

def EVT_LIST_BEGIN_RDRAG(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_BEGIN_RDRAG, func)

def EVT_LIST_BEGIN_LABEL_EDIT(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT, func)

def EVT_LIST_END_LABEL_EDIT(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_END_LABEL_EDIT, func)

def EVT_LIST_DELETE_ITEM(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_DELETE_ITEM, func)

def EVT_LIST_DELETE_ALL_ITEMS(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, func)

def EVT_LIST_GET_INFO(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_GET_INFO, func)

def EVT_LIST_SET_INFO(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_SET_INFO, func)

def EVT_LIST_ITEM_SELECTED(win, id, func):
    win.Connect(id, -1,  wxEVT_COMMAND_LIST_ITEM_SELECTED, func)

def EVT_LIST_ITEM_ACTIVATED(win, id, func):
    win.Connect(id, -1,  wxEVT_COMMAND_LIST_ITEM_ACTIVATED, func)

def EVT_LIST_ITEM_DESELECTED(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_ITEM_DESELECTED, func)

def EVT_LIST_KEY_DOWN(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_KEY_DOWN, func)

def EVT_LIST_INSERT_ITEM(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_INSERT_ITEM, func)

def EVT_LIST_COL_CLICK(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_COL_CLICK, func)

def EVT_LIST_ITEM_RIGHT_CLICK(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, func)

def EVT_LIST_ITEM_MIDDLE_CLICK(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK, func)



#wxSplitterWindow
def EVT_SPLITTER_SASH_POS_CHANGING(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING, func)

def EVT_SPLITTER_SASH_POS_CHANGED(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED, func)

def EVT_SPLITTER_UNSPLIT(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_SPLITTER_UNSPLIT, func)

def EVT_SPLITTER_DOUBLECLICKED(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_SPLITTER_DOUBLECLICKED, func)


#----------------------------------------------------------------------

class wxTimer(wxPyTimer):
    def __init__(self):
        wxPyTimer.__init__(self, self.Notify)   # derived class must provide
                                                # Notify(self) method.

#----------------------------------------------------------------------
# Some wxWin methods can take "NULL" as parameters, but the shadow classes
# expect an object with the SWIG pointer as a 'this' member.  This class
# and instance fools the shadow into passing the NULL pointer.

class NullObj:
    this = 'NULL'       # SWIG converts this to (void*)0

NULL = NullObj()


#----------------------------------------------------------------------
# aliases

wxColor      = wxColour
wxNamedColor = wxNamedColour

wxPyDefaultPosition.Set(-1,-1)
wxPyDefaultSize.Set(-1,-1)

# aliases so that C++ documentation applies:
wxDefaultPosition  = wxPyDefaultPosition
wxDefaultSize      = wxPyDefaultSize


#----------------------------------------------------------------------
# This helper function will take a wxPython object and convert it to
# another wxPython object type.  This will not be able to create objects
# that are derived from wxPython classes by the user, only those that are
# actually part of wxPython and directly corespond to C++ objects.
#
# This is useful in situations where some method returns a generic
# type such as wxWindow, but you know that it is actually some
# derived type such as a wxTextCtrl.  You can't call wxTextCtrl specific
# methods on a wxWindow object, but you can use this function to
# create a wxTextCtrl object that will pass the same pointer to
# the C++ code.  You use it like this:
#
#    textCtrl = wxPyTypeCast(window, "wxTextCtrl")
#
#
# WARNING:  Using this function to type cast objects into types that
#           they are not is not recommended and is likely to cause your
#           program to crash...  Hard.
#

def wxPyTypeCast(obj, typeStr):
    if hasattr(obj, "this"):
        newPtr = ptrcast(obj.this, typeStr+"_p")
    else:
        newPtr = ptrcast(obj, typeStr+"_p")
    theClass = globals()[typeStr+"Ptr"]
    theObj = theClass(newPtr)
    theObj.thisown = obj.thisown
    return theObj


#----------------------------------------------------------------------

class wxPyOnDemandOutputWindow:
    def __init__(self, title = "wxPython: stdout/stderr"):
        self.frame  = None
        self.title  = title

    def SetParent(self, parent):
        self.parent = parent

    def OnCloseWindow(self, event):
        if self.frame != None:
            self.frame.Destroy()
        self.frame = None
        self.text  = None

    # this provides the file-like behaviour
    def write(self, str):
        if not self.frame:
            self.frame = wxFrame(self.parent, -1, self.title)
            self.text  = wxTextCtrl(self.frame, -1, "",
                                    style = wxTE_MULTILINE|wxTE_READONLY)
            self.frame.SetSize(wxSize(450, 300))
            self.frame.Show(true)
            EVT_CLOSE(self.frame, self.OnCloseWindow)
        self.text.AppendText(str)

    def close(self):
        self.frame = None
        self.text  = None



_defRedirect = (wxPlatform == '__WXMSW__')

#----------------------------------------------------------------------
# The main application class.  Derive from this and implement an OnInit
# method that creates a frame and then calls self.SetTopWindow(frame)

class wxApp(wxPyApp):
    error = 'wxApp.error'
    outputWindowClass = wxPyOnDemandOutputWindow

    def __init__(self, redirect=_defRedirect, filename=None):
        wxPyApp.__init__(self)
        self.stdioWin = None
        self.saveStdio = (sys.stdout, sys.stderr)
        if redirect:
            self.RedirectStdio(filename)
        # this initializes wxWindows and then calls our OnInit
        _wxStart(self.OnInit)


    def __del__(self):
        try:
            self.RestoreStdio()
        except:
            pass


    def SetTopWindow(self, frame):
        if self.stdioWin:
            self.stdioWin.SetParent(frame)
            sys.stdout = self.stdioWin #sys.stderr =
        wxPyApp.SetTopWindow(self, frame)

    def MainLoop(self):
        wxPyApp.MainLoop(self)
        self.RestoreStdio()

    def RedirectStdio(self, filename):
        if filename:
            sys.stdout = sys.stderr = open(filename, 'a')
        else:
            self.stdioWin = self.outputWindowClass() # wxPyOnDemandOutputWindow

    def RestoreStdio(self):
        sys.stdout, sys.stderr = self.saveStdio
        if self.stdioWin != None:
            self.stdioWin.close()



#----------------------------------------------------------------------------
# DO NOT hold any other references to this object.  This is how we know when
# to cleanup system resources that wxWin is holding...
__cleanMeUp = __wxPyCleanup()
#----------------------------------------------------------------------------



