# This file was created automatically by SWIG.
import wxc

from misc import *

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

from utils import *
class wxPyAppPtr(wxEvtHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetAppName(self):
        val = wxc.wxPyApp_GetAppName(self.this)
        return val
    def GetClassName(self):
        val = wxc.wxPyApp_GetClassName(self.this)
        return val
    def GetExitOnFrameDelete(self):
        val = wxc.wxPyApp_GetExitOnFrameDelete(self.this)
        return val
    def GetPrintMode(self):
        val = wxc.wxPyApp_GetPrintMode(self.this)
        return val
    def GetTopWindow(self):
        val = wxc.wxPyApp_GetTopWindow(self.this)
        val = wxWindowPtr(val)
        return val
    def GetVendorName(self):
        val = wxc.wxPyApp_GetVendorName(self.this)
        return val
    def Dispatch(self):
        val = wxc.wxPyApp_Dispatch(self.this)
        return val
    def ExitMainLoop(self):
        val = wxc.wxPyApp_ExitMainLoop(self.this)
        return val
    def Initialized(self):
        val = wxc.wxPyApp_Initialized(self.this)
        return val
    def MainLoop(self):
        val = wxc.wxPyApp_MainLoop(self.this)
        return val
    def Pending(self):
        val = wxc.wxPyApp_Pending(self.this)
        return val
    def SetAppName(self,arg0):
        val = wxc.wxPyApp_SetAppName(self.this,arg0)
        return val
    def SetClassName(self,arg0):
        val = wxc.wxPyApp_SetClassName(self.this,arg0)
        return val
    def SetExitOnFrameDelete(self,arg0):
        val = wxc.wxPyApp_SetExitOnFrameDelete(self.this,arg0)
        return val
    def SetPrintMode(self,arg0):
        val = wxc.wxPyApp_SetPrintMode(self.this,arg0)
        return val
    def SetTopWindow(self,arg0):
        val = wxc.wxPyApp_SetTopWindow(self.this,arg0.this)
        return val
    def SetVendorName(self,arg0):
        val = wxc.wxPyApp_SetVendorName(self.this,arg0)
        return val
    def AfterMainLoop(self):
        val = wxc.wxPyApp_AfterMainLoop(self.this)
        return val
    def __repr__(self):
        return "<C wxPyApp instance>"
class wxPyApp(wxPyAppPtr):
    def __init__(self) :
        self.this = wxc.new_wxPyApp()
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

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
wxHORIZONTAL = wxc.wxHORIZONTAL
wxVERTICAL = wxc.wxVERTICAL
wxBOTH = wxc.wxBOTH
wxCENTER_FRAME = wxc.wxCENTER_FRAME
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
wxCLIP_CHILDREN = wxc.wxCLIP_CHILDREN
wxRETAINED = wxc.wxRETAINED
wxBACKINGSTORE = wxc.wxBACKINGSTORE
wxTB_3DBUTTONS = wxc.wxTB_3DBUTTONS
wxTB_HORIZONTAL = wxc.wxTB_HORIZONTAL
wxTB_VERTICAL = wxc.wxTB_VERTICAL
wxTB_FLAT = wxc.wxTB_FLAT
wxCOLOURED = wxc.wxCOLOURED
wxFIXED_LENGTH = wxc.wxFIXED_LENGTH
wxALIGN_LEFT = wxc.wxALIGN_LEFT
wxALIGN_CENTER = wxc.wxALIGN_CENTER
wxALIGN_CENTRE = wxc.wxALIGN_CENTRE
wxALIGN_RIGHT = wxc.wxALIGN_RIGHT
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
wxTE_MULTILINE = wxc.wxTE_MULTILINE
wxCB_SIMPLE = wxc.wxCB_SIMPLE
wxCB_DROPDOWN = wxc.wxCB_DROPDOWN
wxCB_SORT = wxc.wxCB_SORT
wxCB_READONLY = wxc.wxCB_READONLY
wxRA_HORIZONTAL = wxc.wxRA_HORIZONTAL
wxRA_VERTICAL = wxc.wxRA_VERTICAL
wxRB_GROUP = wxc.wxRB_GROUP
wxGA_PROGRESSBAR = wxc.wxGA_PROGRESSBAR
wxGA_HORIZONTAL = wxc.wxGA_HORIZONTAL
wxGA_VERTICAL = wxc.wxGA_VERTICAL
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
wxBU_AUTODRAW = wxc.wxBU_AUTODRAW
wxBU_NOAUTODRAW = wxc.wxBU_NOAUTODRAW
wxTR_HAS_BUTTONS = wxc.wxTR_HAS_BUTTONS
wxTR_EDIT_LABELS = wxc.wxTR_EDIT_LABELS
wxTR_LINES_AT_ROOT = wxc.wxTR_LINES_AT_ROOT
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
wxTAB_MULTILINE = wxc.wxTAB_MULTILINE
wxTAB_RIGHTJUSTIFY = wxc.wxTAB_RIGHTJUSTIFY
wxTAB_FIXEDWIDTH = wxc.wxTAB_FIXEDWIDTH
wxTAB_OWNERDRAW = wxc.wxTAB_OWNERDRAW
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
wxICON_EXCLAMATION = wxc.wxICON_EXCLAMATION
wxICON_HAND = wxc.wxICON_HAND
wxICON_QUESTION = wxc.wxICON_QUESTION
wxICON_INFORMATION = wxc.wxICON_INFORMATION
wxICON_STOP = wxc.wxICON_STOP
wxICON_ASTERISK = wxc.wxICON_ASTERISK
wxICON_MASK = wxc.wxICON_MASK
wxCENTRE = wxc.wxCENTRE
wxCENTER = wxc.wxCENTER
wxSIZE_AUTO_WIDTH = wxc.wxSIZE_AUTO_WIDTH
wxSIZE_AUTO_HEIGHT = wxc.wxSIZE_AUTO_HEIGHT
wxSIZE_AUTO = wxc.wxSIZE_AUTO
wxSIZE_USE_EXISTING = wxc.wxSIZE_USE_EXISTING
wxSIZE_ALLOW_MINUS_ONE = wxc.wxSIZE_ALLOW_MINUS_ONE
wxPORTRAIT = wxc.wxPORTRAIT
wxLANDSCAPE = wxc.wxLANDSCAPE
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
wxOPEN = wxc.wxOPEN
wxSAVE = wxc.wxSAVE
wxHIDE_READONLY = wxc.wxHIDE_READONLY
wxOVERWRITE_PROMPT = wxc.wxOVERWRITE_PROMPT
wxACCEL_ALT = wxc.wxACCEL_ALT
wxACCEL_CTRL = wxc.wxACCEL_CTRL
wxACCEL_SHIFT = wxc.wxACCEL_SHIFT
ERR_PARAM = wxc.ERR_PARAM
ERR_NODATA = wxc.ERR_NODATA
ERR_CANCEL = wxc.ERR_CANCEL
ERR_SUCCESS = wxc.ERR_SUCCESS
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
wxEVT_SCROLL_TOP = wxc.wxEVT_SCROLL_TOP
wxEVT_SCROLL_BOTTOM = wxc.wxEVT_SCROLL_BOTTOM
wxEVT_SCROLL_LINEUP = wxc.wxEVT_SCROLL_LINEUP
wxEVT_SCROLL_LINEDOWN = wxc.wxEVT_SCROLL_LINEDOWN
wxEVT_SCROLL_PAGEUP = wxc.wxEVT_SCROLL_PAGEUP
wxEVT_SCROLL_PAGEDOWN = wxc.wxEVT_SCROLL_PAGEDOWN
wxEVT_SCROLL_THUMBTRACK = wxc.wxEVT_SCROLL_THUMBTRACK
wxEVT_SIZE = wxc.wxEVT_SIZE
wxEVT_MOVE = wxc.wxEVT_MOVE
wxEVT_CLOSE_WINDOW = wxc.wxEVT_CLOSE_WINDOW
wxEVT_END_SESSION = wxc.wxEVT_END_SESSION
wxEVT_QUERY_END_SESSION = wxc.wxEVT_QUERY_END_SESSION
wxEVT_ACTIVATE_APP = wxc.wxEVT_ACTIVATE_APP
wxEVT_POWER = wxc.wxEVT_POWER
wxEVT_CHAR_HOOK = wxc.wxEVT_CHAR_HOOK
wxEVT_KEY_UP = wxc.wxEVT_KEY_UP
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
wxEVT_COMMAND_TAB_SEL_CHANGED = wxc.wxEVT_COMMAND_TAB_SEL_CHANGED
wxEVT_COMMAND_TAB_SEL_CHANGING = wxc.wxEVT_COMMAND_TAB_SEL_CHANGING
wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED = wxc.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED
wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING = wxc.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING
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

def _StdWindowCallbacks(win):
    _checkForCallback(win, "OnChar",               wxEVT_CHAR)
    _checkForCallback(win, "OnSize",               wxEVT_SIZE)
    _checkForCallback(win, "OnEraseBackground",    wxEVT_ERASE_BACKGROUND)
    _checkForCallback(win, "OnSysColourChanged",   wxEVT_SYS_COLOUR_CHANGED)
    _checkForCallback(win, "OnInitDialog",         wxEVT_INIT_DIALOG)
    _checkForCallback(win, "OnIdle",               wxEVT_IDLE)
    _checkForCallback(win, "OnPaint",              wxEVT_PAINT)

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


def _StdOnScrollCallback(win):
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

def EVT_LIST_ITEM_DESELECTED(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_ITEM_DESELECTED, func)

def EVT_LIST_KEY_DOWN(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_KEY_DOWN, func)

def EVT_LIST_INSERT_ITEM(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_INSERT_ITEM, func)

def EVT_LIST_COL_CLICK(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LIST_COL_CLICK, func)


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
wxDefaultPosition = wxPyDefaultPosition
wxDefaultSize     = wxPyDefaultSize


# This is to cover up a bug in SWIG.  We are redefining
# the shadow class that is generated for wxAcceleratorTable
# because SWIG incorrectly uses "arg0.this"
class wxAcceleratorTable(wxAcceleratorTablePtr):
    def __init__(self,arg0) :
        self.this = miscc.new_wxAcceleratorTable(arg0)
        self.thisown = 1

#----------------------------------------------------------------------

## class wxPyStdOutWindow(wxFrame):
##     def __init__(self, title = "wxPython: stdout/stderr"):
##         wxFrame.__init__(self, NULL, title)
##         self.title = title
##         self.text = wxTextWindow(self)
##         self.text.SetFont(wxFont(10, wxMODERN, wxNORMAL, wxBOLD))
##         self.SetSize(-1,-1,400,200)
##         self.Show(false)
##         self.isShown = false

##     def write(self, str):  # with this method,
##         if not self.isShown:
##             self.Show(true)
##             self.isShown = true
##         self.text.WriteText(str)

##     def OnCloseWindow(self, event): # doesn't allow the window to close, just hides it
##         self.Show(false)
##         self.isShown = false


_defRedirect = (wxPlatform == '__WXMSW__')

#----------------------------------------------------------------------
# The main application class.  Derive from this and implement an OnInit
# method that creates a frame and then calls self.SetTopWindow(frame)

class wxApp(wxPyApp):
    error = 'wxApp.error'

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

    def RedirectStdio(self, filename):
        if filename:
            sys.stdout = sys.stderr = open(filename, 'a')
        else:
            raise self.error, 'wxPyStdOutWindow not yet implemented.'
            #self.stdioWin = sys.stdout = sys.stderr = wxPyStdOutWindow()

    def RestoreStdio(self):
        sys.stdout, sys.stderr = self.saveStdio
        if self.stdioWin != None:
            self.stdioWin.Show(false)
            self.stdioWin.Destroy()
            self.stdioWin = None


#----------------------------------------------------------------------------
#
# $Log$
# Revision 1.7  1999/02/25 07:18:52  RD
# wxPython version 2.0b5
#
# Revision 1.11  1999/02/20 09:02:55  RD
# Added wxWindow_FromHWND(hWnd) for wxMSW to construct a wxWindow from a
# window handle.  If you can get the window handle into the python code,
# it should just work...  More news on this later.
#
# Added wxImageList, wxToolTip.
#
# Re-enabled wxConfig.DeleteAll() since it is reportedly fixed for the
# wxRegConfig class.
#
# As usual, some bug fixes, tweaks, etc.
#
# Revision 1.10  1999/02/01 00:10:39  RD
#
# Added the missing EVT_LIST_ITEM_SELECTED and friends.
#
# Revision 1.9  1999/01/30 07:30:09  RD
#
# Added wxSashWindow, wxSashEvent, wxLayoutAlgorithm, etc.
#
# Various cleanup, tweaks, minor additions, etc. to maintain
# compatibility with the current wxWindows.
#
# Revision 1.8  1999/01/29 21:13:42  HH
# Added aliases for wxDefaultPosition and wxDefaultSize (from wxPy..) in _extras,
# so that C++ documentation applies.
#
# Revision 1.7  1998/11/25 08:45:21  RD
#
# Added wxPalette, wxRegion, wxRegionIterator, wxTaskbarIcon
# Added events for wxGrid
# Other various fixes and additions
#
# Revision 1.6  1998/11/16 00:00:52  RD
# Generic treectrl for wxPython/GTK compiles...
#
# Revision 1.5  1998/10/20 07:38:02  RD
# bug fix
#
# Revision 1.4  1998/10/20 06:43:54  RD
# New wxTreeCtrl wrappers (untested)
# some changes in helpers
# etc.
#
# Revision 1.3  1998/10/02 06:40:33  RD
#
# Version 0.4 of wxPython for MSW.
#
# Revision 1.2  1998/08/18 19:48:12  RD
# more wxGTK compatibility things.
#
# It builds now but there are serious runtime problems...
#
# Revision 1.1  1998/08/09 08:25:49  RD
# Initial version
#
#

