# This file was created automatically by SWIG.
import wxpc

from misc import *

from windows import *

from gdi import *

from events import *

from mdi import *

from controls import *

from controls2 import *

from windows2 import *

from cmndlgs import *
class wxPyAppPtr(wxEvtHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetAppName(self):
        val = wxpc.wxPyApp_GetAppName(self.this)
        return val
    def GetClassName(self):
        val = wxpc.wxPyApp_GetClassName(self.this)
        return val
    def GetExitOnFrameDelete(self):
        val = wxpc.wxPyApp_GetExitOnFrameDelete(self.this)
        return val
    def GetPrintMode(self):
        val = wxpc.wxPyApp_GetPrintMode(self.this)
        return val
    def GetTopWindow(self):
        val = wxpc.wxPyApp_GetTopWindow(self.this)
        val = wxWindowPtr(val)
        return val
    def GetVendorName(self):
        val = wxpc.wxPyApp_GetVendorName(self.this)
        return val
    def Dispatch(self):
        val = wxpc.wxPyApp_Dispatch(self.this)
        return val
    def ExitMainLoop(self):
        val = wxpc.wxPyApp_ExitMainLoop(self.this)
        return val
    def Initialized(self):
        val = wxpc.wxPyApp_Initialized(self.this)
        return val
    def MainLoop(self):
        val = wxpc.wxPyApp_MainLoop(self.this)
        return val
    def Pending(self):
        val = wxpc.wxPyApp_Pending(self.this)
        return val
    def SetAppName(self,arg0):
        val = wxpc.wxPyApp_SetAppName(self.this,arg0)
        return val
    def SetClassName(self,arg0):
        val = wxpc.wxPyApp_SetClassName(self.this,arg0)
        return val
    def SetExitOnFrameDelete(self,arg0):
        val = wxpc.wxPyApp_SetExitOnFrameDelete(self.this,arg0)
        return val
    def SetPrintMode(self,arg0):
        val = wxpc.wxPyApp_SetPrintMode(self.this,arg0)
        return val
    def SetTopWindow(self,arg0):
        val = wxpc.wxPyApp_SetTopWindow(self.this,arg0.this)
        return val
    def SetVendorName(self,arg0):
        val = wxpc.wxPyApp_SetVendorName(self.this,arg0)
        return val
    def AfterMainLoop(self):
        val = wxpc.wxPyApp_AfterMainLoop(self.this)
        return val
    def __repr__(self):
        return "<C wxPyApp instance>"
class wxPyApp(wxPyAppPtr):
    def __init__(self) :
        self.this = wxpc.new_wxPyApp()
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

_wxStart = wxpc._wxStart

_wxSetDictionary = wxpc._wxSetDictionary



#-------------- VARIABLE WRAPPERS ------------------

wxMAJOR_VERSION = wxpc.wxMAJOR_VERSION
wxMINOR_VERSION = wxpc.wxMINOR_VERSION
wxRELEASE_NUMBER = wxpc.wxRELEASE_NUMBER
NOT_FOUND = wxpc.NOT_FOUND
wxVSCROLL = wxpc.wxVSCROLL
wxHSCROLL = wxpc.wxHSCROLL
wxCAPTION = wxpc.wxCAPTION
wxDOUBLE_BORDER = wxpc.wxDOUBLE_BORDER
wxSUNKEN_BORDER = wxpc.wxSUNKEN_BORDER
wxRAISED_BORDER = wxpc.wxRAISED_BORDER
wxBORDER = wxpc.wxBORDER
wxSIMPLE_BORDER = wxpc.wxSIMPLE_BORDER
wxSTATIC_BORDER = wxpc.wxSTATIC_BORDER
wxTRANSPARENT_WINDOW = wxpc.wxTRANSPARENT_WINDOW
wxNO_BORDER = wxpc.wxNO_BORDER
wxUSER_COLOURS = wxpc.wxUSER_COLOURS
wxNO_3D = wxpc.wxNO_3D
wxTAB_TRAVERSAL = wxpc.wxTAB_TRAVERSAL
wxHORIZONTAL = wxpc.wxHORIZONTAL
wxVERTICAL = wxpc.wxVERTICAL
wxBOTH = wxpc.wxBOTH
wxCENTER_FRAME = wxpc.wxCENTER_FRAME
wxSTAY_ON_TOP = wxpc.wxSTAY_ON_TOP
wxICONIZE = wxpc.wxICONIZE
wxMINIMIZE = wxpc.wxMINIMIZE
wxMAXIMIZE = wxpc.wxMAXIMIZE
wxTHICK_FRAME = wxpc.wxTHICK_FRAME
wxSYSTEM_MENU = wxpc.wxSYSTEM_MENU
wxMINIMIZE_BOX = wxpc.wxMINIMIZE_BOX
wxMAXIMIZE_BOX = wxpc.wxMAXIMIZE_BOX
wxTINY_CAPTION_HORIZ = wxpc.wxTINY_CAPTION_HORIZ
wxTINY_CAPTION_VERT = wxpc.wxTINY_CAPTION_VERT
wxRESIZE_BOX = wxpc.wxRESIZE_BOX
wxRESIZE_BORDER = wxpc.wxRESIZE_BORDER
wxDIALOG_MODAL = wxpc.wxDIALOG_MODAL
wxDIALOG_MODELESS = wxpc.wxDIALOG_MODELESS
wxDEFAULT_FRAME_STYLE = wxpc.wxDEFAULT_FRAME_STYLE
wxDEFAULT_DIALOG_STYLE = wxpc.wxDEFAULT_DIALOG_STYLE
wxRETAINED = wxpc.wxRETAINED
wxBACKINGSTORE = wxpc.wxBACKINGSTORE
wxTB_3DBUTTONS = wxpc.wxTB_3DBUTTONS
wxTB_HORIZONTAL = wxpc.wxTB_HORIZONTAL
wxTB_VERTICAL = wxpc.wxTB_VERTICAL
wxCOLOURED = wxpc.wxCOLOURED
wxFIXED_LENGTH = wxpc.wxFIXED_LENGTH
wxALIGN_LEFT = wxpc.wxALIGN_LEFT
wxALIGN_CENTER = wxpc.wxALIGN_CENTER
wxALIGN_CENTRE = wxpc.wxALIGN_CENTRE
wxALIGN_RIGHT = wxpc.wxALIGN_RIGHT
wxLB_NEEDED_SB = wxpc.wxLB_NEEDED_SB
wxLB_ALWAYS_SB = wxpc.wxLB_ALWAYS_SB
wxLB_SORT = wxpc.wxLB_SORT
wxLB_SINGLE = wxpc.wxLB_SINGLE
wxLB_MULTIPLE = wxpc.wxLB_MULTIPLE
wxLB_EXTENDED = wxpc.wxLB_EXTENDED
wxLB_OWNERDRAW = wxpc.wxLB_OWNERDRAW
wxLB_HSCROLL = wxpc.wxLB_HSCROLL
wxPROCESS_ENTER = wxpc.wxPROCESS_ENTER
wxPASSWORD = wxpc.wxPASSWORD
wxTE_PROCESS_ENTER = wxpc.wxTE_PROCESS_ENTER
wxTE_PASSWORD = wxpc.wxTE_PASSWORD
wxTE_READONLY = wxpc.wxTE_READONLY
wxTE_MULTILINE = wxpc.wxTE_MULTILINE
wxCB_SIMPLE = wxpc.wxCB_SIMPLE
wxCB_DROPDOWN = wxpc.wxCB_DROPDOWN
wxCB_SORT = wxpc.wxCB_SORT
wxCB_READONLY = wxpc.wxCB_READONLY
wxRA_HORIZONTAL = wxpc.wxRA_HORIZONTAL
wxRA_VERTICAL = wxpc.wxRA_VERTICAL
wxRB_GROUP = wxpc.wxRB_GROUP
wxGA_PROGRESSBAR = wxpc.wxGA_PROGRESSBAR
wxGA_HORIZONTAL = wxpc.wxGA_HORIZONTAL
wxGA_VERTICAL = wxpc.wxGA_VERTICAL
wxSL_HORIZONTAL = wxpc.wxSL_HORIZONTAL
wxSL_VERTICAL = wxpc.wxSL_VERTICAL
wxSL_AUTOTICKS = wxpc.wxSL_AUTOTICKS
wxSL_LABELS = wxpc.wxSL_LABELS
wxSL_LEFT = wxpc.wxSL_LEFT
wxSL_TOP = wxpc.wxSL_TOP
wxSL_RIGHT = wxpc.wxSL_RIGHT
wxSL_BOTTOM = wxpc.wxSL_BOTTOM
wxSL_BOTH = wxpc.wxSL_BOTH
wxSL_SELRANGE = wxpc.wxSL_SELRANGE
wxSB_HORIZONTAL = wxpc.wxSB_HORIZONTAL
wxSB_VERTICAL = wxpc.wxSB_VERTICAL
wxBU_AUTODRAW = wxpc.wxBU_AUTODRAW
wxBU_NOAUTODRAW = wxpc.wxBU_NOAUTODRAW
wxTR_HAS_BUTTONS = wxpc.wxTR_HAS_BUTTONS
wxTR_EDIT_LABELS = wxpc.wxTR_EDIT_LABELS
wxLC_ICON = wxpc.wxLC_ICON
wxLC_SMALL_ICON = wxpc.wxLC_SMALL_ICON
wxLC_LIST = wxpc.wxLC_LIST
wxLC_REPORT = wxpc.wxLC_REPORT
wxLC_ALIGN_TOP = wxpc.wxLC_ALIGN_TOP
wxLC_ALIGN_LEFT = wxpc.wxLC_ALIGN_LEFT
wxLC_AUTOARRANGE = wxpc.wxLC_AUTOARRANGE
wxLC_USER_TEXT = wxpc.wxLC_USER_TEXT
wxLC_EDIT_LABELS = wxpc.wxLC_EDIT_LABELS
wxLC_NO_HEADER = wxpc.wxLC_NO_HEADER
wxLC_NO_SORT_HEADER = wxpc.wxLC_NO_SORT_HEADER
wxLC_SINGLE_SEL = wxpc.wxLC_SINGLE_SEL
wxLC_SORT_ASCENDING = wxpc.wxLC_SORT_ASCENDING
wxLC_SORT_DESCENDING = wxpc.wxLC_SORT_DESCENDING
wxLC_MASK_TYPE = wxpc.wxLC_MASK_TYPE
wxLC_MASK_ALIGN = wxpc.wxLC_MASK_ALIGN
wxLC_MASK_SORT = wxpc.wxLC_MASK_SORT
wxSP_VERTICAL = wxpc.wxSP_VERTICAL
wxSP_HORIZONTAL = wxpc.wxSP_HORIZONTAL
wxSP_ARROW_KEYS = wxpc.wxSP_ARROW_KEYS
wxSP_WRAP = wxpc.wxSP_WRAP
wxSP_NOBORDER = wxpc.wxSP_NOBORDER
wxSP_3D = wxpc.wxSP_3D
wxSP_BORDER = wxpc.wxSP_BORDER
wxTAB_MULTILINE = wxpc.wxTAB_MULTILINE
wxTAB_RIGHTJUSTIFY = wxpc.wxTAB_RIGHTJUSTIFY
wxTAB_FIXEDWIDTH = wxpc.wxTAB_FIXEDWIDTH
wxTAB_OWNERDRAW = wxpc.wxTAB_OWNERDRAW
wxFLOOD_SURFACE = wxpc.wxFLOOD_SURFACE
wxFLOOD_BORDER = wxpc.wxFLOOD_BORDER
wxODDEVEN_RULE = wxpc.wxODDEVEN_RULE
wxWINDING_RULE = wxpc.wxWINDING_RULE
wxTOOL_TOP = wxpc.wxTOOL_TOP
wxTOOL_BOTTOM = wxpc.wxTOOL_BOTTOM
wxTOOL_LEFT = wxpc.wxTOOL_LEFT
wxTOOL_RIGHT = wxpc.wxTOOL_RIGHT
wxOK = wxpc.wxOK
wxYES_NO = wxpc.wxYES_NO
wxCANCEL = wxpc.wxCANCEL
wxYES = wxpc.wxYES
wxNO = wxpc.wxNO
wxICON_EXCLAMATION = wxpc.wxICON_EXCLAMATION
wxICON_HAND = wxpc.wxICON_HAND
wxICON_QUESTION = wxpc.wxICON_QUESTION
wxICON_INFORMATION = wxpc.wxICON_INFORMATION
wxICON_STOP = wxpc.wxICON_STOP
wxICON_ASTERISK = wxpc.wxICON_ASTERISK
wxICON_MASK = wxpc.wxICON_MASK
wxCENTRE = wxpc.wxCENTRE
wxCENTER = wxpc.wxCENTER
wxSIZE_AUTO_WIDTH = wxpc.wxSIZE_AUTO_WIDTH
wxSIZE_AUTO_HEIGHT = wxpc.wxSIZE_AUTO_HEIGHT
wxSIZE_AUTO = wxpc.wxSIZE_AUTO
wxSIZE_USE_EXISTING = wxpc.wxSIZE_USE_EXISTING
wxSIZE_ALLOW_MINUS_ONE = wxpc.wxSIZE_ALLOW_MINUS_ONE
wxDF_TEXT = wxpc.wxDF_TEXT
wxDF_BITMAP = wxpc.wxDF_BITMAP
wxDF_METAFILE = wxpc.wxDF_METAFILE
wxDF_DIB = wxpc.wxDF_DIB
wxDF_OEMTEXT = wxpc.wxDF_OEMTEXT
wxDF_FILENAME = wxpc.wxDF_FILENAME
wxPORTRAIT = wxpc.wxPORTRAIT
wxLANDSCAPE = wxpc.wxLANDSCAPE
wxID_OPEN = wxpc.wxID_OPEN
wxID_CLOSE = wxpc.wxID_CLOSE
wxID_NEW = wxpc.wxID_NEW
wxID_SAVE = wxpc.wxID_SAVE
wxID_SAVEAS = wxpc.wxID_SAVEAS
wxID_REVERT = wxpc.wxID_REVERT
wxID_EXIT = wxpc.wxID_EXIT
wxID_UNDO = wxpc.wxID_UNDO
wxID_REDO = wxpc.wxID_REDO
wxID_HELP = wxpc.wxID_HELP
wxID_PRINT = wxpc.wxID_PRINT
wxID_PRINT_SETUP = wxpc.wxID_PRINT_SETUP
wxID_PREVIEW = wxpc.wxID_PREVIEW
wxID_ABOUT = wxpc.wxID_ABOUT
wxID_HELP_CONTENTS = wxpc.wxID_HELP_CONTENTS
wxID_HELP_COMMANDS = wxpc.wxID_HELP_COMMANDS
wxID_HELP_PROCEDURES = wxpc.wxID_HELP_PROCEDURES
wxID_HELP_CONTEXT = wxpc.wxID_HELP_CONTEXT
wxID_CUT = wxpc.wxID_CUT
wxID_COPY = wxpc.wxID_COPY
wxID_PASTE = wxpc.wxID_PASTE
wxID_CLEAR = wxpc.wxID_CLEAR
wxID_FIND = wxpc.wxID_FIND
wxID_FILE1 = wxpc.wxID_FILE1
wxID_FILE2 = wxpc.wxID_FILE2
wxID_FILE3 = wxpc.wxID_FILE3
wxID_FILE4 = wxpc.wxID_FILE4
wxID_FILE5 = wxpc.wxID_FILE5
wxID_FILE6 = wxpc.wxID_FILE6
wxID_FILE7 = wxpc.wxID_FILE7
wxID_FILE8 = wxpc.wxID_FILE8
wxID_FILE9 = wxpc.wxID_FILE9
wxID_OK = wxpc.wxID_OK
wxID_CANCEL = wxpc.wxID_CANCEL
wxID_APPLY = wxpc.wxID_APPLY
wxID_YES = wxpc.wxID_YES
wxID_NO = wxpc.wxID_NO
wxBITMAP_TYPE_BMP = wxpc.wxBITMAP_TYPE_BMP
wxBITMAP_TYPE_BMP_RESOURCE = wxpc.wxBITMAP_TYPE_BMP_RESOURCE
wxBITMAP_TYPE_ICO = wxpc.wxBITMAP_TYPE_ICO
wxBITMAP_TYPE_ICO_RESOURCE = wxpc.wxBITMAP_TYPE_ICO_RESOURCE
wxBITMAP_TYPE_CUR = wxpc.wxBITMAP_TYPE_CUR
wxBITMAP_TYPE_CUR_RESOURCE = wxpc.wxBITMAP_TYPE_CUR_RESOURCE
wxBITMAP_TYPE_XBM = wxpc.wxBITMAP_TYPE_XBM
wxBITMAP_TYPE_XBM_DATA = wxpc.wxBITMAP_TYPE_XBM_DATA
wxBITMAP_TYPE_XPM = wxpc.wxBITMAP_TYPE_XPM
wxBITMAP_TYPE_XPM_DATA = wxpc.wxBITMAP_TYPE_XPM_DATA
wxBITMAP_TYPE_TIF = wxpc.wxBITMAP_TYPE_TIF
wxBITMAP_TYPE_TIF_RESOURCE = wxpc.wxBITMAP_TYPE_TIF_RESOURCE
wxBITMAP_TYPE_GIF = wxpc.wxBITMAP_TYPE_GIF
wxBITMAP_TYPE_GIF_RESOURCE = wxpc.wxBITMAP_TYPE_GIF_RESOURCE
wxBITMAP_TYPE_PNG = wxpc.wxBITMAP_TYPE_PNG
wxBITMAP_TYPE_PNG_RESOURCE = wxpc.wxBITMAP_TYPE_PNG_RESOURCE
wxBITMAP_TYPE_ANY = wxpc.wxBITMAP_TYPE_ANY
wxBITMAP_TYPE_RESOURCE = wxpc.wxBITMAP_TYPE_RESOURCE
wxOPEN = wxpc.wxOPEN
wxSAVE = wxpc.wxSAVE
wxHIDE_READONLY = wxpc.wxHIDE_READONLY
wxOVERWRITE_PROMPT = wxpc.wxOVERWRITE_PROMPT
ERR_PARAM = wxpc.ERR_PARAM
ERR_NODATA = wxpc.ERR_NODATA
ERR_CANCEL = wxpc.ERR_CANCEL
ERR_SUCCESS = wxpc.ERR_SUCCESS
wxDEFAULT = wxpc.wxDEFAULT
wxDECORATIVE = wxpc.wxDECORATIVE
wxROMAN = wxpc.wxROMAN
wxSCRIPT = wxpc.wxSCRIPT
wxSWISS = wxpc.wxSWISS
wxMODERN = wxpc.wxMODERN
wxTELETYPE = wxpc.wxTELETYPE
wxVARIABLE = wxpc.wxVARIABLE
wxFIXED = wxpc.wxFIXED
wxNORMAL = wxpc.wxNORMAL
wxLIGHT = wxpc.wxLIGHT
wxBOLD = wxpc.wxBOLD
wxITALIC = wxpc.wxITALIC
wxSLANT = wxpc.wxSLANT
wxSOLID = wxpc.wxSOLID
wxDOT = wxpc.wxDOT
wxLONG_DASH = wxpc.wxLONG_DASH
wxSHORT_DASH = wxpc.wxSHORT_DASH
wxDOT_DASH = wxpc.wxDOT_DASH
wxUSER_DASH = wxpc.wxUSER_DASH
wxTRANSPARENT = wxpc.wxTRANSPARENT
wxSTIPPLE = wxpc.wxSTIPPLE
wxBDIAGONAL_HATCH = wxpc.wxBDIAGONAL_HATCH
wxCROSSDIAG_HATCH = wxpc.wxCROSSDIAG_HATCH
wxFDIAGONAL_HATCH = wxpc.wxFDIAGONAL_HATCH
wxCROSS_HATCH = wxpc.wxCROSS_HATCH
wxHORIZONTAL_HATCH = wxpc.wxHORIZONTAL_HATCH
wxVERTICAL_HATCH = wxpc.wxVERTICAL_HATCH
wxJOIN_BEVEL = wxpc.wxJOIN_BEVEL
wxJOIN_MITER = wxpc.wxJOIN_MITER
wxJOIN_ROUND = wxpc.wxJOIN_ROUND
wxCAP_ROUND = wxpc.wxCAP_ROUND
wxCAP_PROJECTING = wxpc.wxCAP_PROJECTING
wxCAP_BUTT = wxpc.wxCAP_BUTT
wxCLEAR = wxpc.wxCLEAR
wxXOR = wxpc.wxXOR
wxINVERT = wxpc.wxINVERT
wxOR_REVERSE = wxpc.wxOR_REVERSE
wxAND_REVERSE = wxpc.wxAND_REVERSE
wxCOPY = wxpc.wxCOPY
wxAND = wxpc.wxAND
wxAND_INVERT = wxpc.wxAND_INVERT
wxNO_OP = wxpc.wxNO_OP
wxNOR = wxpc.wxNOR
wxEQUIV = wxpc.wxEQUIV
wxSRC_INVERT = wxpc.wxSRC_INVERT
wxOR_INVERT = wxpc.wxOR_INVERT
wxNAND = wxpc.wxNAND
wxOR = wxpc.wxOR
wxSET = wxpc.wxSET
wxSRC_OR = wxpc.wxSRC_OR
wxSRC_AND = wxpc.wxSRC_AND
WXK_BACK = wxpc.WXK_BACK
WXK_TAB = wxpc.WXK_TAB
WXK_RETURN = wxpc.WXK_RETURN
WXK_ESCAPE = wxpc.WXK_ESCAPE
WXK_SPACE = wxpc.WXK_SPACE
WXK_DELETE = wxpc.WXK_DELETE
WXK_START = wxpc.WXK_START
WXK_LBUTTON = wxpc.WXK_LBUTTON
WXK_RBUTTON = wxpc.WXK_RBUTTON
WXK_CANCEL = wxpc.WXK_CANCEL
WXK_MBUTTON = wxpc.WXK_MBUTTON
WXK_CLEAR = wxpc.WXK_CLEAR
WXK_SHIFT = wxpc.WXK_SHIFT
WXK_CONTROL = wxpc.WXK_CONTROL
WXK_MENU = wxpc.WXK_MENU
WXK_PAUSE = wxpc.WXK_PAUSE
WXK_CAPITAL = wxpc.WXK_CAPITAL
WXK_PRIOR = wxpc.WXK_PRIOR
WXK_NEXT = wxpc.WXK_NEXT
WXK_END = wxpc.WXK_END
WXK_HOME = wxpc.WXK_HOME
WXK_LEFT = wxpc.WXK_LEFT
WXK_UP = wxpc.WXK_UP
WXK_RIGHT = wxpc.WXK_RIGHT
WXK_DOWN = wxpc.WXK_DOWN
WXK_SELECT = wxpc.WXK_SELECT
WXK_PRINT = wxpc.WXK_PRINT
WXK_EXECUTE = wxpc.WXK_EXECUTE
WXK_SNAPSHOT = wxpc.WXK_SNAPSHOT
WXK_INSERT = wxpc.WXK_INSERT
WXK_HELP = wxpc.WXK_HELP
WXK_NUMPAD0 = wxpc.WXK_NUMPAD0
WXK_NUMPAD1 = wxpc.WXK_NUMPAD1
WXK_NUMPAD2 = wxpc.WXK_NUMPAD2
WXK_NUMPAD3 = wxpc.WXK_NUMPAD3
WXK_NUMPAD4 = wxpc.WXK_NUMPAD4
WXK_NUMPAD5 = wxpc.WXK_NUMPAD5
WXK_NUMPAD6 = wxpc.WXK_NUMPAD6
WXK_NUMPAD7 = wxpc.WXK_NUMPAD7
WXK_NUMPAD8 = wxpc.WXK_NUMPAD8
WXK_NUMPAD9 = wxpc.WXK_NUMPAD9
WXK_MULTIPLY = wxpc.WXK_MULTIPLY
WXK_ADD = wxpc.WXK_ADD
WXK_SEPARATOR = wxpc.WXK_SEPARATOR
WXK_SUBTRACT = wxpc.WXK_SUBTRACT
WXK_DECIMAL = wxpc.WXK_DECIMAL
WXK_DIVIDE = wxpc.WXK_DIVIDE
WXK_F1 = wxpc.WXK_F1
WXK_F2 = wxpc.WXK_F2
WXK_F3 = wxpc.WXK_F3
WXK_F4 = wxpc.WXK_F4
WXK_F5 = wxpc.WXK_F5
WXK_F6 = wxpc.WXK_F6
WXK_F7 = wxpc.WXK_F7
WXK_F8 = wxpc.WXK_F8
WXK_F9 = wxpc.WXK_F9
WXK_F10 = wxpc.WXK_F10
WXK_F11 = wxpc.WXK_F11
WXK_F12 = wxpc.WXK_F12
WXK_F13 = wxpc.WXK_F13
WXK_F14 = wxpc.WXK_F14
WXK_F15 = wxpc.WXK_F15
WXK_F16 = wxpc.WXK_F16
WXK_F17 = wxpc.WXK_F17
WXK_F18 = wxpc.WXK_F18
WXK_F19 = wxpc.WXK_F19
WXK_F20 = wxpc.WXK_F20
WXK_F21 = wxpc.WXK_F21
WXK_F22 = wxpc.WXK_F22
WXK_F23 = wxpc.WXK_F23
WXK_F24 = wxpc.WXK_F24
WXK_NUMLOCK = wxpc.WXK_NUMLOCK
WXK_SCROLL = wxpc.WXK_SCROLL
WXK_PAGEUP = wxpc.WXK_PAGEUP
WXK_PAGEDOWN = wxpc.WXK_PAGEDOWN
wxCURSOR_ARROW = wxpc.wxCURSOR_ARROW
wxCURSOR_BULLSEYE = wxpc.wxCURSOR_BULLSEYE
wxCURSOR_CHAR = wxpc.wxCURSOR_CHAR
wxCURSOR_CROSS = wxpc.wxCURSOR_CROSS
wxCURSOR_HAND = wxpc.wxCURSOR_HAND
wxCURSOR_IBEAM = wxpc.wxCURSOR_IBEAM
wxCURSOR_LEFT_BUTTON = wxpc.wxCURSOR_LEFT_BUTTON
wxCURSOR_MAGNIFIER = wxpc.wxCURSOR_MAGNIFIER
wxCURSOR_MIDDLE_BUTTON = wxpc.wxCURSOR_MIDDLE_BUTTON
wxCURSOR_NO_ENTRY = wxpc.wxCURSOR_NO_ENTRY
wxCURSOR_PAINT_BRUSH = wxpc.wxCURSOR_PAINT_BRUSH
wxCURSOR_PENCIL = wxpc.wxCURSOR_PENCIL
wxCURSOR_POINT_LEFT = wxpc.wxCURSOR_POINT_LEFT
wxCURSOR_POINT_RIGHT = wxpc.wxCURSOR_POINT_RIGHT
wxCURSOR_QUESTION_ARROW = wxpc.wxCURSOR_QUESTION_ARROW
wxCURSOR_RIGHT_BUTTON = wxpc.wxCURSOR_RIGHT_BUTTON
wxCURSOR_SIZENESW = wxpc.wxCURSOR_SIZENESW
wxCURSOR_SIZENS = wxpc.wxCURSOR_SIZENS
wxCURSOR_SIZENWSE = wxpc.wxCURSOR_SIZENWSE
wxCURSOR_SIZEWE = wxpc.wxCURSOR_SIZEWE
wxCURSOR_SIZING = wxpc.wxCURSOR_SIZING
wxCURSOR_SPRAYCAN = wxpc.wxCURSOR_SPRAYCAN
wxCURSOR_WAIT = wxpc.wxCURSOR_WAIT
wxCURSOR_WATCH = wxpc.wxCURSOR_WATCH
wxCURSOR_BLANK = wxpc.wxCURSOR_BLANK
FALSE = wxpc.FALSE
false = wxpc.false
TRUE = wxpc.TRUE
true = wxpc.true
wxEVT_NULL = wxpc.wxEVT_NULL
wxEVT_FIRST = wxpc.wxEVT_FIRST
wxEVT_COMMAND_BUTTON_CLICKED = wxpc.wxEVT_COMMAND_BUTTON_CLICKED
wxEVT_COMMAND_CHECKBOX_CLICKED = wxpc.wxEVT_COMMAND_CHECKBOX_CLICKED
wxEVT_COMMAND_CHOICE_SELECTED = wxpc.wxEVT_COMMAND_CHOICE_SELECTED
wxEVT_COMMAND_LISTBOX_SELECTED = wxpc.wxEVT_COMMAND_LISTBOX_SELECTED
wxEVT_COMMAND_LISTBOX_DOUBLECLICKED = wxpc.wxEVT_COMMAND_LISTBOX_DOUBLECLICKED
wxEVT_COMMAND_CHECKLISTBOX_TOGGLED = wxpc.wxEVT_COMMAND_CHECKLISTBOX_TOGGLED
wxEVT_COMMAND_TEXT_UPDATED = wxpc.wxEVT_COMMAND_TEXT_UPDATED
wxEVT_COMMAND_TEXT_ENTER = wxpc.wxEVT_COMMAND_TEXT_ENTER
wxEVT_COMMAND_MENU_SELECTED = wxpc.wxEVT_COMMAND_MENU_SELECTED
wxEVT_COMMAND_SLIDER_UPDATED = wxpc.wxEVT_COMMAND_SLIDER_UPDATED
wxEVT_COMMAND_RADIOBOX_SELECTED = wxpc.wxEVT_COMMAND_RADIOBOX_SELECTED
wxEVT_COMMAND_RADIOBUTTON_SELECTED = wxpc.wxEVT_COMMAND_RADIOBUTTON_SELECTED
wxEVT_COMMAND_SCROLLBAR_UPDATED = wxpc.wxEVT_COMMAND_SCROLLBAR_UPDATED
wxEVT_COMMAND_VLBOX_SELECTED = wxpc.wxEVT_COMMAND_VLBOX_SELECTED
wxEVT_COMMAND_COMBOBOX_SELECTED = wxpc.wxEVT_COMMAND_COMBOBOX_SELECTED
wxEVT_COMMAND_TOOL_CLICKED = wxpc.wxEVT_COMMAND_TOOL_CLICKED
wxEVT_COMMAND_TOOL_RCLICKED = wxpc.wxEVT_COMMAND_TOOL_RCLICKED
wxEVT_COMMAND_TOOL_ENTER = wxpc.wxEVT_COMMAND_TOOL_ENTER
wxEVT_SET_FOCUS = wxpc.wxEVT_SET_FOCUS
wxEVT_KILL_FOCUS = wxpc.wxEVT_KILL_FOCUS
wxEVT_LEFT_DOWN = wxpc.wxEVT_LEFT_DOWN
wxEVT_LEFT_UP = wxpc.wxEVT_LEFT_UP
wxEVT_MIDDLE_DOWN = wxpc.wxEVT_MIDDLE_DOWN
wxEVT_MIDDLE_UP = wxpc.wxEVT_MIDDLE_UP
wxEVT_RIGHT_DOWN = wxpc.wxEVT_RIGHT_DOWN
wxEVT_RIGHT_UP = wxpc.wxEVT_RIGHT_UP
wxEVT_MOTION = wxpc.wxEVT_MOTION
wxEVT_ENTER_WINDOW = wxpc.wxEVT_ENTER_WINDOW
wxEVT_LEAVE_WINDOW = wxpc.wxEVT_LEAVE_WINDOW
wxEVT_LEFT_DCLICK = wxpc.wxEVT_LEFT_DCLICK
wxEVT_MIDDLE_DCLICK = wxpc.wxEVT_MIDDLE_DCLICK
wxEVT_RIGHT_DCLICK = wxpc.wxEVT_RIGHT_DCLICK
wxEVT_NC_LEFT_DOWN = wxpc.wxEVT_NC_LEFT_DOWN
wxEVT_NC_LEFT_UP = wxpc.wxEVT_NC_LEFT_UP
wxEVT_NC_MIDDLE_DOWN = wxpc.wxEVT_NC_MIDDLE_DOWN
wxEVT_NC_MIDDLE_UP = wxpc.wxEVT_NC_MIDDLE_UP
wxEVT_NC_RIGHT_DOWN = wxpc.wxEVT_NC_RIGHT_DOWN
wxEVT_NC_RIGHT_UP = wxpc.wxEVT_NC_RIGHT_UP
wxEVT_NC_MOTION = wxpc.wxEVT_NC_MOTION
wxEVT_NC_ENTER_WINDOW = wxpc.wxEVT_NC_ENTER_WINDOW
wxEVT_NC_LEAVE_WINDOW = wxpc.wxEVT_NC_LEAVE_WINDOW
wxEVT_NC_LEFT_DCLICK = wxpc.wxEVT_NC_LEFT_DCLICK
wxEVT_NC_MIDDLE_DCLICK = wxpc.wxEVT_NC_MIDDLE_DCLICK
wxEVT_NC_RIGHT_DCLICK = wxpc.wxEVT_NC_RIGHT_DCLICK
wxEVT_CHAR = wxpc.wxEVT_CHAR
wxEVT_SCROLL_TOP = wxpc.wxEVT_SCROLL_TOP
wxEVT_SCROLL_BOTTOM = wxpc.wxEVT_SCROLL_BOTTOM
wxEVT_SCROLL_LINEUP = wxpc.wxEVT_SCROLL_LINEUP
wxEVT_SCROLL_LINEDOWN = wxpc.wxEVT_SCROLL_LINEDOWN
wxEVT_SCROLL_PAGEUP = wxpc.wxEVT_SCROLL_PAGEUP
wxEVT_SCROLL_PAGEDOWN = wxpc.wxEVT_SCROLL_PAGEDOWN
wxEVT_SCROLL_THUMBTRACK = wxpc.wxEVT_SCROLL_THUMBTRACK
wxEVT_SIZE = wxpc.wxEVT_SIZE
wxEVT_MOVE = wxpc.wxEVT_MOVE
wxEVT_CLOSE_WINDOW = wxpc.wxEVT_CLOSE_WINDOW
wxEVT_END_SESSION = wxpc.wxEVT_END_SESSION
wxEVT_QUERY_END_SESSION = wxpc.wxEVT_QUERY_END_SESSION
wxEVT_ACTIVATE_APP = wxpc.wxEVT_ACTIVATE_APP
wxEVT_POWER = wxpc.wxEVT_POWER
wxEVT_CHAR_HOOK = wxpc.wxEVT_CHAR_HOOK
wxEVT_KEY_UP = wxpc.wxEVT_KEY_UP
wxEVT_ACTIVATE = wxpc.wxEVT_ACTIVATE
wxEVT_CREATE = wxpc.wxEVT_CREATE
wxEVT_DESTROY = wxpc.wxEVT_DESTROY
wxEVT_SHOW = wxpc.wxEVT_SHOW
wxEVT_ICONIZE = wxpc.wxEVT_ICONIZE
wxEVT_MAXIMIZE = wxpc.wxEVT_MAXIMIZE
wxEVT_MOUSE_CAPTURE_CHANGED = wxpc.wxEVT_MOUSE_CAPTURE_CHANGED
wxEVT_PAINT = wxpc.wxEVT_PAINT
wxEVT_ERASE_BACKGROUND = wxpc.wxEVT_ERASE_BACKGROUND
wxEVT_NC_PAINT = wxpc.wxEVT_NC_PAINT
wxEVT_PAINT_ICON = wxpc.wxEVT_PAINT_ICON
wxEVT_MENU_CHAR = wxpc.wxEVT_MENU_CHAR
wxEVT_MENU_INIT = wxpc.wxEVT_MENU_INIT
wxEVT_MENU_HIGHLIGHT = wxpc.wxEVT_MENU_HIGHLIGHT
wxEVT_POPUP_MENU_INIT = wxpc.wxEVT_POPUP_MENU_INIT
wxEVT_CONTEXT_MENU = wxpc.wxEVT_CONTEXT_MENU
wxEVT_SYS_COLOUR_CHANGED = wxpc.wxEVT_SYS_COLOUR_CHANGED
wxEVT_SETTING_CHANGED = wxpc.wxEVT_SETTING_CHANGED
wxEVT_QUERY_NEW_PALETTE = wxpc.wxEVT_QUERY_NEW_PALETTE
wxEVT_PALETTE_CHANGED = wxpc.wxEVT_PALETTE_CHANGED
wxEVT_JOY_BUTTON_DOWN = wxpc.wxEVT_JOY_BUTTON_DOWN
wxEVT_JOY_BUTTON_UP = wxpc.wxEVT_JOY_BUTTON_UP
wxEVT_JOY_MOVE = wxpc.wxEVT_JOY_MOVE
wxEVT_JOY_ZMOVE = wxpc.wxEVT_JOY_ZMOVE
wxEVT_DROP_FILES = wxpc.wxEVT_DROP_FILES
wxEVT_DRAW_ITEM = wxpc.wxEVT_DRAW_ITEM
wxEVT_MEASURE_ITEM = wxpc.wxEVT_MEASURE_ITEM
wxEVT_COMPARE_ITEM = wxpc.wxEVT_COMPARE_ITEM
wxEVT_INIT_DIALOG = wxpc.wxEVT_INIT_DIALOG
wxEVT_IDLE = wxpc.wxEVT_IDLE
wxEVT_UPDATE_UI = wxpc.wxEVT_UPDATE_UI
wxEVT_COMMAND_LEFT_CLICK = wxpc.wxEVT_COMMAND_LEFT_CLICK
wxEVT_COMMAND_LEFT_DCLICK = wxpc.wxEVT_COMMAND_LEFT_DCLICK
wxEVT_COMMAND_RIGHT_CLICK = wxpc.wxEVT_COMMAND_RIGHT_CLICK
wxEVT_COMMAND_RIGHT_DCLICK = wxpc.wxEVT_COMMAND_RIGHT_DCLICK
wxEVT_COMMAND_SET_FOCUS = wxpc.wxEVT_COMMAND_SET_FOCUS
wxEVT_COMMAND_KILL_FOCUS = wxpc.wxEVT_COMMAND_KILL_FOCUS
wxEVT_COMMAND_ENTER = wxpc.wxEVT_COMMAND_ENTER
wxEVT_COMMAND_TREE_BEGIN_DRAG = wxpc.wxEVT_COMMAND_TREE_BEGIN_DRAG
wxEVT_COMMAND_TREE_BEGIN_RDRAG = wxpc.wxEVT_COMMAND_TREE_BEGIN_RDRAG
wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT = wxpc.wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT
wxEVT_COMMAND_TREE_END_LABEL_EDIT = wxpc.wxEVT_COMMAND_TREE_END_LABEL_EDIT
wxEVT_COMMAND_TREE_DELETE_ITEM = wxpc.wxEVT_COMMAND_TREE_DELETE_ITEM
wxEVT_COMMAND_TREE_GET_INFO = wxpc.wxEVT_COMMAND_TREE_GET_INFO
wxEVT_COMMAND_TREE_SET_INFO = wxpc.wxEVT_COMMAND_TREE_SET_INFO
wxEVT_COMMAND_TREE_ITEM_EXPANDED = wxpc.wxEVT_COMMAND_TREE_ITEM_EXPANDED
wxEVT_COMMAND_TREE_ITEM_EXPANDING = wxpc.wxEVT_COMMAND_TREE_ITEM_EXPANDING
wxEVT_COMMAND_TREE_SEL_CHANGED = wxpc.wxEVT_COMMAND_TREE_SEL_CHANGED
wxEVT_COMMAND_TREE_SEL_CHANGING = wxpc.wxEVT_COMMAND_TREE_SEL_CHANGING
wxEVT_COMMAND_TREE_KEY_DOWN = wxpc.wxEVT_COMMAND_TREE_KEY_DOWN
wxEVT_COMMAND_LIST_BEGIN_DRAG = wxpc.wxEVT_COMMAND_LIST_BEGIN_DRAG
wxEVT_COMMAND_LIST_BEGIN_RDRAG = wxpc.wxEVT_COMMAND_LIST_BEGIN_RDRAG
wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT = wxpc.wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT
wxEVT_COMMAND_LIST_END_LABEL_EDIT = wxpc.wxEVT_COMMAND_LIST_END_LABEL_EDIT
wxEVT_COMMAND_LIST_DELETE_ITEM = wxpc.wxEVT_COMMAND_LIST_DELETE_ITEM
wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS = wxpc.wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS
wxEVT_COMMAND_LIST_GET_INFO = wxpc.wxEVT_COMMAND_LIST_GET_INFO
wxEVT_COMMAND_LIST_SET_INFO = wxpc.wxEVT_COMMAND_LIST_SET_INFO
wxEVT_COMMAND_LIST_ITEM_SELECTED = wxpc.wxEVT_COMMAND_LIST_ITEM_SELECTED
wxEVT_COMMAND_LIST_ITEM_DESELECTED = wxpc.wxEVT_COMMAND_LIST_ITEM_DESELECTED
wxEVT_COMMAND_LIST_KEY_DOWN = wxpc.wxEVT_COMMAND_LIST_KEY_DOWN
wxEVT_COMMAND_LIST_INSERT_ITEM = wxpc.wxEVT_COMMAND_LIST_INSERT_ITEM
wxEVT_COMMAND_LIST_COL_CLICK = wxpc.wxEVT_COMMAND_LIST_COL_CLICK
wxEVT_COMMAND_TAB_SEL_CHANGED = wxpc.wxEVT_COMMAND_TAB_SEL_CHANGED
wxEVT_COMMAND_TAB_SEL_CHANGING = wxpc.wxEVT_COMMAND_TAB_SEL_CHANGING
wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED = wxpc.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED
wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING = wxpc.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING
__version__ = wxpc.__version__
cvar = wxpc.cvar
wxPyDefaultPosition = wxPointPtr(wxpc.cvar.wxPyDefaultPosition)
wxPyDefaultSize = wxSizePtr(wxpc.cvar.wxPyDefaultSize)


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



def EVT_NOTEBOOK_PAGE_CHANGED(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, func)

def EVT_NOTEBOOK_PAGE_CHANGING(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, func)



#----------------------------------------------------------------------
#----------------------------------------------------------------------
# We need to add to the shadow classes a bit to facilitate callbacks via
# virtual functions.  These classes replace the shadows generated by SWIG.

class wxWindow(wxWindowPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(windowsc.new_wxWindow,(arg0.this,arg1,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)


class wxFrame(wxFramePtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(windowsc.new_wxFrame,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        _StdFrameCallbacks(self)


if wxPlatform == '__WXMSW__':
    class wxMiniFrame(wxMiniFramePtr):
        def __init__(self,arg0,arg1,arg2,*args) :
            argl = map(None,args)
            try: argl[0] = argl[0].this
            except: pass
            try: argl[1] = argl[1].this
            except: pass
            args = tuple(argl)
            self.this = apply(windowsc.new_wxMiniFrame,(arg0.this,arg1,arg2,)+args)
            self.thisown = 1
            _StdFrameCallbacks(self)


class wxPanel(wxPanelPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(windowsc.new_wxPanel,(arg0.this,arg1,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)


class wxDialog(wxDialogPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(windowsc.new_wxDialog,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)
        _checkForCallback(self, "OnOk",     wxEVT_COMMAND_BUTTON_CLICKED,   wxID_OK)
        _checkForCallback(self, "OnApply",  wxEVT_COMMAND_BUTTON_CLICKED,   wxID_APPLY)
        _checkForCallback(self, "OnCancel", wxEVT_COMMAND_BUTTON_CLICKED,   wxID_CANCEL)
        _checkForCallback(self, "OnCloseWindow", wxEVT_CLOSE_WINDOW)
        _checkForCallback(self, "OnCharHook",    wxEVT_CHAR_HOOK)


class wxScrolledWindow(wxScrolledWindowPtr):
    def __init__(self,arg0,*args) :
        argl = map(None,args)
        try: argl[1] = argl[1].this
        except: pass
        try: argl[2] = argl[2].this
        except: pass
        args = tuple(argl)
        self.this = apply(windowsc.new_wxScrolledWindow,(arg0.this,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)
        _StdOnScrollCallback(self)



class wxMDIParentFrame(wxMDIParentFramePtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(mdic.new_wxMDIParentFrame,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        _StdFrameCallbacks(self)


class wxMDIChildFrame(wxMDIChildFramePtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(mdic.new_wxMDIChildFrame,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        _StdFrameCallbacks(self)


class wxMDIClientWindow(wxMDIClientWindowPtr):
    def __init__(self,arg0,*args) :
        self.this = apply(mdic.new_wxMDIClientWindow,(arg0.this,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)
        _StdOnScrollCallback(self)



class wxControl(wxControlPtr):
    def __init__(self,this):
        self.this = this
        _StdWindowCallbacks(self)

class wxButton(wxButtonPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxButton,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)

class wxBitmapButton(wxBitmapButtonPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxBitmapButton,(arg0.this,arg1,arg2.this,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)

class wxCheckBox(wxCheckBoxPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxCheckBox,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)



class wxChoice(wxChoicePtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxChoice,(arg0.this,arg1,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)

class wxComboBox(wxComboBoxPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[1] = argl[1].this
        except: pass
        try: argl[2] = argl[2].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxComboBox,(arg0.this,arg1,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)

class wxGauge(wxGaugePtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxGauge,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)

class wxStaticBox(wxStaticBoxPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxStaticBox,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)

class wxStaticText(wxStaticTextPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxStaticText,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)

class wxListBox(wxListBoxPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxListBox,(arg0.this,arg1,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)

class wxTextCtrl(wxTextCtrlPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[1] = argl[1].this
        except: pass
        try: argl[2] = argl[2].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxTextCtrl,(arg0.this,arg1,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)

class wxScrollBar(wxScrollBarPtr):
    def __init__(self,arg0,*args) :
        argl = map(None,args)
        try: argl[1] = argl[1].this
        except: pass
        try: argl[2] = argl[2].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxScrollBar,(arg0.this,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)

if wxPlatform == '__WXMSW__':
    class wxSpinButton(wxSpinButtonPtr):
        def __init__(self,arg0,*args) :
            argl = map(None,args)
            try: argl[1] = argl[1].this
            except: pass
            try: argl[2] = argl[2].this
            except: pass
            args = tuple(argl)
            self.this = apply(controlsc.new_wxSpinButton,(arg0.this,)+args)
            self.thisown = 1
            _StdWindowCallbacks(self)

class wxStaticBitmap(wxStaticBitmapPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxStaticBitmap,(arg0.this,arg1,arg2.this,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)

class wxRadioBox(wxRadioBoxPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxRadioBox,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)

class wxRadioButton(wxRadioButtonPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxRadioButton,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)

class wxSlider(wxSliderPtr):
    def __init__(self,arg0,arg1,arg2,arg3,arg4,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxSlider,(arg0.this,arg1,arg2,arg3,arg4,)+args)
        self.thisown = 1
        _StdWindowCallbacks(self)






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

wxColor = wxColour
wxNamedColor = wxNamedColour

wxPyDefaultPosition.Set(-1,-1)
wxPyDefaultSize.Set(-1,-1)

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
# Revision 1.1  1998/08/19 00:10:03  RD
# GTK version of the SWIG-generated files
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

